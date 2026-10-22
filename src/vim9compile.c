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

#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

// Functions defined with :def are stored in this growarray.
// They are never removed, so that they can be found by index.
// Deleted functions have the df_deleted flag set.
garray_T def_functions = {0, 0, ಗಾತ್ರ(dfunc_T), 50, ಖಾಲಿ};

ಸ್ಥಿರ ಶೂನ್ಯ delete_def_function_contents(dfunc_T *dfunc, ಪೂರ್ಣಾಂಕ mark_deleted);

/*
 * Lookup variable "name" in the local scope and return it in "lvar".
 * "lvar->lv_from_outer" is incremented accordingly.
 * If "lvar" is NULL only check if the variable can be found.
 * Return FAIL if not found.
 */
    ಪೂರ್ಣಾಂಕ
lookup_local(char_u *name, size_t len, lvar_T *lvar, cctx_T *cctx)
{
    ಪೂರ್ಣಾಂಕ	    idx;
    lvar_T  *lvp;

    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (((len == 4 && STRNCMP(name, "this", 4) == 0)
		|| (len == 5 && STRNCMP(name, "super", 5) == 0))
	    && cctx->ctx_ufunc != ಖಾಲಿ
	    && (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)))
    {
	ಪೂರ್ಣಾಂಕ is_super = *name == 's';
	ಆದರೆ (is_super)
	{
	    ಆದರೆ (name[5] != '.')
	    {
		emsg(_(e_super_must_be_followed_by_dot));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (cctx->ctx_ufunc->uf_class != ಖಾಲಿ
		    && cctx->ctx_ufunc->uf_class->class_extends == ಖಾಲಿ)
	    {
		emsg(_(e_using_super_not_ಒಳ_ಮಗು_class));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಆದರೆ (lvar != ಖಾಲಿ)
	{
	    CLEAR_ಸೂಚಕ(lvar);
	    lvar->lv_loop_ಆಳ = -1;
	    lvar->lv_name = (char_u *)(is_super ? "super" : "this");
	    ಆದರೆ (cctx->ctx_ufunc->uf_class != ಖಾಲಿ)
	    {
		lvar->lv_type = &cctx->ctx_ufunc->uf_class->class_object_type;
		ಆದರೆ (is_super)
		{
		    type_T *type = get_type_ptr(cctx->ctx_type_list);

		    ಆದರೆ (type != ಖಾಲಿ)
		    {
			*type = *lvar->lv_type;
			lvar->lv_type = type;
			type->tt_flags |= TTFLAG_SUPER;
		    }
		}
	    }
	}
	ಹಿಂತಿರುಗಿ OK;
    }

    // Find local in current function scope.
    ಗಾಗಿ (idx = 0; idx < cctx->ctx_locals.ga_len; ++idx)
    {
	lvp = ((lvar_T *)cctx->ctx_locals.ga_ದತ್ತಾಂಶ) + idx;
	ಆದರೆ (lvp->lv_name != ಖಾಲಿ
		&& STRNCMP(name, lvp->lv_name, len) == 0
					       && STRLEN(lvp->lv_name) == len)
	{
	    ಆದರೆ (lvar != ಖಾಲಿ)
	    {
		*lvar = *lvp;
		lvar->lv_from_outer = 0;
		// If the variable was declared inside a loop set
		// lvar->lv_loop_idx and lvar->lv_loop_depth.
		get_loop_var_idx(cctx, idx, lvar);
	    }
	    ಹಿಂತಿರುಗಿ OK;
	}
    }

    // Find local in outer function scope.
    ಆದರೆ (cctx->ctx_outer != ಖಾಲಿ)
    {
	ಆದರೆ (lookup_local(name, len, lvar, cctx->ctx_outer) == OK)
	{
	    ಆದರೆ (lvar != ಖಾಲಿ)
	    {
		cctx->ctx_outer_ಬಳಸಿದ = TRUE;
		++lvar->lv_from_outer;
	    }
	    ಹಿಂತಿರುಗಿ OK;
	}
    }

    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Lookup an argument in the current function and an enclosing function.
 * Returns the argument index in "idxp"
 * Returns the argument type in "type"
 * Sets "gen_load_outer" to TRUE if found in outer scope.
 * Returns OK when found, FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
arg_exists(
	char_u	*name,
	size_t	len,
	ಪೂರ್ಣಾಂಕ	*idxp,
	type_T	**type,
	ಪೂರ್ಣಾಂಕ	*gen_ತುಂಬು_outer,
	cctx_T	*cctx)
{
    ಪೂರ್ಣಾಂಕ	    idx;
    char_u  *va_name;

    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಗಾಗಿ (idx = 0; idx < cctx->ctx_ufunc->uf_ವಾದಗಳು_visible; ++idx)
    {
	char_u *arg = FUNCARG(cctx->ctx_ufunc, idx);

	ಆದರೆ (STRNCMP(name, arg, len) == 0 && arg[len] == NUL)
	{
	    ಆದರೆ (idxp != ಖಾಲಿ)
	    {
		// Arguments are located above the frame pointer.  One further
		// if there is a vararg argument
		*idxp = idx - (cctx->ctx_ufunc->uf_ವಾದಗಳು.ga_len
							    + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ)
			      + (cctx->ctx_ufunc->uf_va_name != ಖಾಲಿ ? -1 : 0);

		ಆದರೆ (cctx->ctx_ufunc->uf_arg_types != ಖಾಲಿ)
		    *type = cctx->ctx_ufunc->uf_arg_types[idx];
		ಇಲ್ಲದಿದ್ದರೆ
		    *type = &t_any;
	    }
	    ಹಿಂತಿರುಗಿ OK;
	}
    }

    va_name = cctx->ctx_ufunc->uf_va_name;
    ಆದರೆ (va_name != ಖಾಲಿ
		    && STRNCMP(name, va_name, len) == 0 && va_name[len] == NUL)
    {
	ಆದರೆ (idxp != ಖಾಲಿ)
	{
	    // varargs is always the last argument
	    *idxp = -ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ - 1;
	    *type = cctx->ctx_ufunc->uf_va_type;
	}
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (cctx->ctx_outer != ಖಾಲಿ)
    {
	// Lookup the name for an argument of the outer function.
	ಆದರೆ (arg_exists(name, len, idxp, type, gen_ತುಂಬು_outer, cctx->ctx_outer)
									 == OK)
	{
	    ಆದರೆ (gen_ತುಂಬು_outer != ಖಾಲಿ)
	    {
		++*gen_ತುಂಬು_outer;
		cctx->ctx_outer_ಬಳಸಿದ = TRUE;
	    }
	    ಹಿಂತಿರುಗಿ OK;
	}
    }

    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Lookup a script-local variable in the current script, possibly defined in a
 * block that contains the function "cctx->ctx_ufunc".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * If "len" is <= 0 "name" must be NUL terminated.
 * Return NULL when not found.
 */
    ಸ್ಥಿರ sallvar_T *
ಕಂಡುಹಿಡಿ_script_var(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    scriptitem_T    *si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    hashitem_T	    *hi;
    ಪೂರ್ಣಾಂಕ		    cc;
    sallvar_T	    *sav;
    ufunc_T	    *ufunc;

    // Find the list of all script variables with the right name.
    ಆದರೆ (len > 0)
    {
	cc = name[len];
	name[len] = NUL;
    }
    hi = hash_ಕಂಡುಹಿಡಿ(&si->sn_all_vars.dv_hashtab, name);
    ಆದರೆ (len > 0)
	name[len] = cc;
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    sav = HI2SAV(hi);
    ಆದರೆ (sav->sav_ಖಂಡ_id == 0)
	// variable defined in the top script scope is always visible
	ಹಿಂತಿರುಗಿ sav;

    ಆದರೆ (cctx == ಖಾಲಿ)
    {
	ಆದರೆ (cstack == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	// Not in a function scope, find variable with block ID equal to or
	// smaller than the current block id.  Use "cstack" to go up the block
	// scopes.
	ಆಗ (sav != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ idx;

	    ಗಾಗಿ (idx = cstack->cs_idx; idx >= 0; --idx)
		ಆದರೆ (cstack->cs_ಖಂಡ_id[idx] == sav->sav_ಖಂಡ_id)
		    ನಿಲ್ಲಿಸು;
	    ಆದರೆ (idx >= 0)
		ನಿಲ್ಲಿಸು;
	    sav = sav->sav_ಮುಂದಿನ;
	}
	ಹಿಂತಿರುಗಿ sav;
    }

    // Go over the variables with this name and find one that was visible
    // from the function.
    ufunc = cctx->ctx_ufunc;
    ಆಗ (sav != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ idx;

	ಆದರೆ (ufunc->uf_ಖಂಡ_ಆಳ == 0 && sav->sav_ಖಂಡ_id == 0)
	    // If the function was defined at the script level (not inside a
	    // block), script-scope variables are always visible.
	    ಹಿಂತಿರುಗಿ sav;

	// Go over the blocks that this function was defined in.  If the
	// variable block ID matches it was visible to the function.
	ಗಾಗಿ (idx = 0; idx < ufunc->uf_ಖಂಡ_ಆಳ; ++idx)
	    ಆದರೆ (ufunc->uf_ಖಂಡ_ids[idx] == sav->sav_ಖಂಡ_id)
		ಹಿಂತಿರುಗಿ sav;
	sav = sav->sav_ಮುಂದಿನ;
    }

    // Not found, variable was not visible.
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * If "name" can be found in the current script set it's "block_id".
 */
    ಶೂನ್ಯ
update_script_var_ಖಂಡ_id(char_u *name, ಪೂರ್ಣಾಂಕ ಖಂಡ_id)
{
    scriptitem_T    *si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    hashitem_T	    *hi;
    sallvar_T	    *sav;

    hi = hash_ಕಂಡುಹಿಡಿ(&si->sn_all_vars.dv_hashtab, name);
    ಆದರೆ (HASHITEM_ಖಾಲಿಗಳು(hi))
	ಹಿಂತಿರುಗಿ;
    sav = HI2SAV(hi);
    sav->sav_ಖಂಡ_id = ಖಂಡ_id;
}

/*
 * Lookup a variable (without s: prefix) in the current script.
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
script_var_exists(char_u *name, size_t len, cctx_T *cctx, cstack_T *cstack)
{
    ಆದರೆ (current_sctx.sc_sid <= 0)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (current_script_is_vim9())
    {
	// Check script variables that were visible where the function was
	// defined.
	ಆದರೆ (ಕಂಡುಹಿಡಿ_script_var(name, len, cctx, cstack) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ OK;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	hashtab_T	*ht = &SCRIPT_VARS(current_sctx.sc_sid);
	dictitem_T	*di;
	ಪೂರ್ಣಾಂಕ		cc;

	// Check script variables that are currently visible
	cc = name[len];
	name[len] = NUL;
	di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, name, TRUE);
	name[len] = cc;
	ಆದರೆ (di != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Returns the index of a class method or class variable with name "name"
 * accessible in the currently compiled function.
 * If "cl_ret" is not NULL set it to the class.
 * Otherwise return -1.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
cctx_class_midx(
    cctx_T  *cctx,
    ಪೂರ್ಣಾಂಕ	    is_ವಿಧಾನ,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ಆದರೆ (cctx == ಖಾಲಿ || cctx->ctx_ufunc == ಖಾಲಿ
	    || cctx->ctx_ufunc->uf_class == ಖಾಲಿ
	    || cctx->ctx_ufunc->uf_defclass == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    // Search for the class method or variable in the class where the calling
    // function is defined.
    class_T *cl = cctx->ctx_ufunc->uf_defclass;
    ಪೂರ್ಣಾಂಕ m_idx = is_ವಿಧಾನ ? class_ವಿಧಾನ_idx(cl, name, len)
					: class_member_idx(cl, name, len);
    ಆದರೆ (m_idx < 0)
    {
	cl = cl->class_extends;
	ಆಗ (cl != ಖಾಲಿ)
	{
	    m_idx = is_ವಿಧಾನ ? class_ವಿಧಾನ_idx(cl, name, len)
					: class_member_idx(cl, name, len);
	    ಆದರೆ (m_idx >= 0)
		ನಿಲ್ಲಿಸು;
	    cl = cl->class_extends;
	}
    }

    ಆದರೆ (m_idx >= 0)
    {
	ಆದರೆ (cl_ret != ಖಾಲಿ)
	    *cl_ret = cl;
    }

    ಹಿಂತಿರುಗಿ m_idx;
}

/*
 * Returns the index of a class method with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * method is defined is returned in "cl_ret".
 */
    ಪೂರ್ಣಾಂಕ
cctx_class_ವಿಧಾನ_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ಹಿಂತಿರುಗಿ cctx_class_midx(cctx, TRUE, name, len, cl_ret);
}

/*
 * Returns the index of a class variable with name "name" accessible in the
 * currently compiled function.  Returns -1 if not found.  The class where the
 * variable is defined is returned in "cl_ret".
 */
    ಪೂರ್ಣಾಂಕ
cctx_class_member_idx(
    cctx_T  *cctx,
    char_u  *name,
    size_t  len,
    class_T **cl_ret)
{
    ಹಿಂತಿರುಗಿ cctx_class_midx(cctx, FALSE, name, len, cl_ret);
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable or
 * imported.  Also if "name" is "this" and in a class method.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
variable_exists(char_u *name, size_t len, cctx_T *cctx)
{
    ಹಿಂತಿರುಗಿ (cctx != ಖಾಲಿ
		&& (lookup_local(name, len, ಖಾಲಿ, cctx) == OK
		    || arg_exists(name, len, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, cctx) == OK
		    || (len == 4
			&& cctx->ctx_ufunc != ಖಾಲಿ
			&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW))
			&& STRNCMP(name, "this", 4) == 0)))
	    || script_var_exists(name, len, cctx, ಖಾಲಿ) == OK
	    || cctx_class_member_idx(cctx, name, len, ಖಾಲಿ) >= 0
	    || ಕಂಡುಹಿಡಿ_imported(name, len, FALSE) != ಖಾಲಿ;
}

/*
 * Return TRUE if "name" is a local variable, argument, script variable,
 * imported or function.  Or commands are being skipped, a declaration may have
 * been skipped then.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಅಂಶ_exists(char_u *name, size_t len, ಪೂರ್ಣಾಂಕ cmd ಬಳಸದ, cctx_T *cctx)
{
    ಹಿಂತಿರುಗಿ variable_exists(name, len, cctx);
}

/*
 * Check if "p[len]" is already defined, either in script "import_sid" or in
 * compilation context "cctx".
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Does not check the global namespace.
 * If "is_arg" is TRUE the error message is for an argument name.
 * Return FAIL and give an error if it defined.
 */
    ಪೂರ್ಣಾಂಕ
check_defined(
	char_u	    *p,
	size_t	    len,
	cctx_T	    *cctx,
	cstack_T    *cstack,
	ಪೂರ್ಣಾಂಕ	    is_arg)
{
    ಪೂರ್ಣಾಂಕ		c = p[len];
    ufunc_T	*ufunc = ಖಾಲಿ;

    // underscore argument is OK
    ಆದರೆ (len == 1 && *p == '_')
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (script_var_exists(p, len, cctx, cstack) == OK)
    {
	ಆದರೆ (is_arg)
	    semsg(_(e_ವಾದ_already_declared_ಒಳ_script_str), p);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_variable_already_declared_ಒಳ_script_str), p);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (cctx_class_member_idx(cctx, p, len, ಖಾಲಿ) >= 0)
    {
	ಆದರೆ (is_arg)
	    semsg(_(e_ವಾದ_already_declared_ಒಳ_class_str), p);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_variable_already_declared_ಒಳ_class_str), p);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    p[len] = NUL;
    ಆದರೆ ((cctx != ಖಾಲಿ
		&& (lookup_local(p, len, ಖಾಲಿ, cctx) == OK
		    || arg_exists(p, len, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, cctx) == OK))
	    || ಕಂಡುಹಿಡಿ_imported(p, len, FALSE) != ಖಾಲಿ
	    || (ufunc = ಕಂಡುಹಿಡಿ_func_even_dead(p, 0)) != ಖಾಲಿ)
    {
	// A local or script-local function can shadow a global function.
	ಆದರೆ (ufunc == ಖಾಲಿ || ((ufunc->uf_flags & FC_DEAD) == 0
		    && (!func_is_global(ufunc)
					     || (p[0] == 'g' && p[1] == ':'))))
	{
	    ಆದರೆ (is_arg)
		semsg(_(e_ವಾದ_name_shadows_existing_variable_str), p);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_name_already_defined_str), p);
	    p[len] = c;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    p[len] = c;
    ಹಿಂತಿರುಗಿ OK;
}


/*
 * Return TRUE if "actual" could be "expected" and a runtime typecheck is to be
 * used.  Return FALSE if the types will never match.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
use_typecheck(type_T *actual, type_T *expected)
{
    ಆದರೆ (actual->tt_type == VAR_ANY
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
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ (actual->tt_type == VAR_OBJECT && expected->tt_type == VAR_OBJECT)
	ಹಿಂತಿರುಗಿ TRUE;
    ಆದರೆ ((actual->tt_type == VAR_LIST
		|| actual->tt_type == VAR_TUPLE
		|| actual->tt_type == VAR_DICT)
	    && actual->tt_type == expected->tt_type)
	// This takes care of a nested list or dict.
	ಹಿಂತಿರುಗಿ use_typecheck(actual->tt_member, expected->tt_member);
    ಹಿಂತಿರುಗಿ FALSE;
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
    ಪೂರ್ಣಾಂಕ
need_type_where(
	type_T	*actual,
	type_T	*expected,
	ಪೂರ್ಣಾಂಕ	typechk_flags,	// acceptable types (type check flags)
	ಪೂರ್ಣಾಂಕ	ಸ್ಥಾನಾಂತರ,
	where_T	where,
	cctx_T	*cctx,
	ಪೂರ್ಣಾಂಕ	silent,
	ಪೂರ್ಣಾಂಕ	actual_is_const)
{
    ಪೂರ್ಣಾಂಕ ret;

    ಆದರೆ (expected->tt_type != VAR_CLASS && expected->tt_type != VAR_TYPEALIAS)
    {
	ಆದರೆ (check_type_is_ಮೌಲ್ಯ(actual) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (expected == &t_bool && actual != &t_bool
					&& (actual->tt_flags & TTFLAG_BOOL_OK))
    {
	// Using "0", "1" or the result of an expression with "&&" or "||" as a
	// boolean is OK but requires a conversion.
	generate_2BOOL(cctx, FALSE, ಸ್ಥಾನಾಂತರ);
	ಹಿಂತಿರುಗಿ OK;
    }

    ret = check_type_maybe(expected, actual, FALSE, where);
    ಆದರೆ (ret == OK)
	ಹಿಂತಿರುಗಿ OK;

    // If actual a constant a runtime check makes no sense.  If it's
    // null_function it is OK.
    ಆದರೆ (actual_is_const && ret == MAYBE && actual == &t_func_unknown)
	ಹಿಂತಿರುಗಿ OK;

    // If the actual type can be the expected type add a runtime check.
    ಆದರೆ (!actual_is_const && ret == MAYBE && use_typecheck(actual, expected))
    {
	generate_TYPECHECK(cctx, expected, typechk_flags, ಸ್ಥಾನಾಂತರ,
		where.wt_ಬಗೆ == WT_VARIABLE, where.wt_ಸೂಚ್ಯಂಕ);
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (!silent)
	type_mismatch_where(expected, actual, where);
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಪೂರ್ಣಾಂಕ
need_type(
	type_T	*actual,
	type_T	*expected,
	ಪೂರ್ಣಾಂಕ	typechk_flags,	// acceptable types (type check flags)
	ಪೂರ್ಣಾಂಕ	ಸ್ಥಾನಾಂತರ,
	ಪೂರ್ಣಾಂಕ	arg_idx,
	cctx_T	*cctx,
	ಪೂರ್ಣಾಂಕ	silent,
	ಪೂರ್ಣಾಂಕ	actual_is_const)
{
    where_T where = WHERE_INIT;

    ಆದರೆ (arg_idx > 0)
    {
	where.wt_ಸೂಚ್ಯಂಕ = arg_idx;
	where.wt_ಬಗೆ = WT_ವಾದ;
    }
    ಹಿಂತಿರುಗಿ need_type_where(actual, expected, typechk_flags, ಸ್ಥಾನಾಂತರ, where,
						cctx, silent, actual_is_const);
}

/*
 * Set type of variable "lvar" to "type".  If the variable is a constant then
 * the type gets TTFLAG_CONST.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_var_type(lvar_T *lvar, type_T *type_arg, cctx_T *cctx)
{
    type_T	*type = type_arg;

    ಆದರೆ (lvar->lv_const == ನಿಯೋಜಿಸು_CONST && (type->tt_flags & TTFLAG_CONST) == 0)
    {
	ಆದರೆ (type->tt_flags & TTFLAG_STATIC)
	    // entry in static_types[] is followed by const type
	    type = type + 1;
	ಇಲ್ಲದಿದ್ದರೆ
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
	ಪೂರ್ಣಾಂಕ	ನಿಯೋಜಿಸು,
	type_T	*type)
{
    lvar_T  *lvar;
    dfunc_T *dfunc;

    ಆದರೆ (arg_exists(name, len, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, cctx) == OK)
    {
	emsg_namelen(_(e_str_is_ಬಳಸಿದ_as_ವಾದ), name, (ಪೂರ್ಣಾಂಕ)len);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (GA_GROW_FAILS(&cctx->ctx_locals, 1))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    lvar = ((lvar_T *)cctx->ctx_locals.ga_ದತ್ತಾಂಶ) + cctx->ctx_locals.ga_len++;
    CLEAR_ಸೂಚಕ(lvar);

    // Every local variable uses the next entry on the stack.  We could re-use
    // the last ones when leaving a scope, but then variables used in a closure
    // might get overwritten.  To keep things simple do not re-use stack
    // entries.  This is less efficient, but memory is cheap these days.
    dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + cctx->ctx_ufunc->uf_dfunc_idx;
    lvar->lv_idx = dfunc->df_var_ಹೆಸರುಗಳು.ga_len;

    lvar->lv_name = vim_strnsave(name, len == 0 ? STRLEN(name) : len);
    lvar->lv_const = ನಿಯೋಜಿಸು;
    ಆದರೆ (type == &t_unknown || type == &t_any)
	// type not known yet, may be inferred from RHS
	lvar->lv_type = type;
    ಇಲ್ಲದಿದ್ದರೆ
	// may use TTFLAG_CONST
	ಗಣ_var_type(lvar, type, cctx);

    // Remember the name for debugging.
    ಆದರೆ (GA_GROW_FAILS(&dfunc->df_var_ಹೆಸರುಗಳು, 1))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ((char_u **)dfunc->df_var_ಹೆಸರುಗಳು.ga_ದತ್ತಾಂಶ)[lvar->lv_idx] =
						    vim_strsave(lvar->lv_name);
    ++dfunc->df_var_ಹೆಸರುಗಳು.ga_len;

    ಹಿಂತಿರುಗಿ lvar;
}

/*
 * If "check_writable" is ASSIGN_CONST give an error if the variable was
 * defined with :final or :const, if "check_writable" is ASSIGN_FINAL give an
 * error if the variable was defined with :const.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_ಅಂಶ_writable(svar_T *sv, ಪೂರ್ಣಾಂಕ check_writable, char_u *name)
{
    ಆದರೆ ((check_writable == ನಿಯೋಜಿಸು_CONST && sv->sv_const != 0)
	    || (check_writable == ನಿಯೋಜಿಸು_FINAL
					      && sv->sv_const == ನಿಯೋಜಿಸು_CONST))
    {
	semsg(_(e_cannot_change_readonly_variable_str), name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Find "name" in script-local items of script "sid".
 * Pass "check_writable" to check_item_writable().
 * "cctx" is NULL at the script level, "cstack" is NULL in a function.
 * Returns the index in "sn_var_vals" if found.
 * If found but not in "sn_var_vals" returns -1.
 * If not found or the variable is not writable returns -2.
 */
    ಪೂರ್ಣಾಂಕ
get_script_ಅಂಶ_idx(
	ಪೂರ್ಣಾಂಕ	    sid,
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    check_writable,
	cctx_T	    *cctx,
	cstack_T    *cstack)
{
    hashtab_T	    *ht;
    dictitem_T	    *di;
    scriptitem_T    *si = SCRIPT_ಅಂಶ(sid);
    svar_T	    *sv;
    ಪೂರ್ಣಾಂಕ		    idx;

    ಆದರೆ (!SCRIPT_ID_VALID(sid))
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (sid == current_sctx.sc_sid)
    {
	sallvar_T *sav = ಕಂಡುಹಿಡಿ_script_var(name, 0, cctx, cstack);

	ಆದರೆ (sav == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -2;
	idx = sav->sav_var_vals_idx;
	sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;
	ಆದರೆ (check_ಅಂಶ_writable(sv, check_writable, name) == FAIL)
	    ಹಿಂತಿರುಗಿ -2;
	ಹಿಂತಿರುಗಿ idx;
    }

    // First look the name up in the hashtable.
    ht = &SCRIPT_VARS(sid);
    di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, name, TRUE);
    ಆದರೆ (di == ಖಾಲಿ)
    {
	ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ)
	{
	    hashitem_T *hi;

	    // A variable exported from an autoload script is in the global
	    // variables, we can find it in the all_vars table.
	    hi = hash_ಕಂಡುಹಿಡಿ(&si->sn_all_vars.dv_hashtab, name);
	    ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		ಹಿಂತಿರುಗಿ HI2SAV(hi)->sav_var_vals_idx;
	}
	ಹಿಂತಿರುಗಿ -2;
    }

    // Now find the svar_T index in sn_var_vals.
    ಗಾಗಿ (idx = 0; idx < si->sn_var_vals.ga_len; ++idx)
    {
	sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;
	ಆದರೆ (sv->sv_tv == &di->di_tv)
	{
	    ಆದರೆ (check_ಅಂಶ_writable(sv, check_writable, name) == FAIL)
		ಹಿಂತಿರುಗಿ -2;
	    ಹಿಂತಿರುಗಿ idx;
	}
    }
    ಹಿಂತಿರುಗಿ -1;
}

    ಸ್ಥಿರ imported_T *
ಕಂಡುಹಿಡಿ_imported_ಒಳ_script(char_u *name, size_t len, ಪೂರ್ಣಾಂಕ sid)
{
    scriptitem_T    *si;
    ಪೂರ್ಣಾಂಕ		    idx;

    ಆದರೆ (!SCRIPT_ID_VALID(sid))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    si = SCRIPT_ಅಂಶ(sid);
    ಗಾಗಿ (idx = 0; idx < si->sn_imports.ga_len; ++idx)
    {
	imported_T *import = ((imported_T *)si->sn_imports.ga_ದತ್ತಾಂಶ) + idx;

	ಆದರೆ (len == 0 ? STRCMP(name, import->imp_name) == 0
		     : STRLEN(import->imp_name) == len
				  && STRNCMP(name, import->imp_name, len) == 0)
	    ಹಿಂತಿರುಗಿ import;
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Find "name" in imported items of the current script.
 * If "len" is 0 use any length that works.
 * If "load" is TRUE and the script was not loaded yet, load it now.
 */
    imported_T *
ಕಂಡುಹಿಡಿ_imported(char_u *name, size_t len, ಪೂರ್ಣಾಂಕ ತುಂಬು)
{
    ಆದರೆ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Skip over "s:" before "s:something" to find the import name.
    ಪೂರ್ಣಾಂಕ off = name[0] == 's' && name[1] == ':' ? 2 : 0;

    imported_T *ret = ಕಂಡುಹಿಡಿ_imported_ಒಳ_script(name + off, len - off,
							  current_sctx.sc_sid);
    ಆದರೆ (ret != ಖಾಲಿ && ತುಂಬು && (ret->imp_flags & IMP_FLAGS_AUTOLOAD))
    {
	scid_T	actual_sid = 0;
	ಪೂರ್ಣಾಂಕ	save_emsg_off = emsg_off;

	// "emsg_off" will be set when evaluating an expression silently, but
	// we do want to know about errors in a script.  Also because it then
	// aborts when an error is encountered.
	emsg_off = FALSE;

	// script found before but not loaded yet
	ret->imp_flags &= ~IMP_FLAGS_AUTOLOAD;
	(ಶೂನ್ಯ)do_ಮೂಲ(SCRIPT_ಅಂಶ(ret->imp_sid)->sn_name, FALSE,
						       DOSO_NONE, &actual_sid);
	// If the script is a symlink it may be sourced with another name, may
	// need to adjust the script ID for that.
	ಆದರೆ (actual_sid != 0)
	    ret->imp_sid = actual_sid;

	emsg_off = save_emsg_off;
    }
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Called when checking for a following operator at "arg".  When the rest of
 * the line is empty or only a comment, peek the next line.  If there is a next
 * line return a pointer to it and set "nextp".
 * Otherwise skip over white space.
 */
    char_u *
may_peek_ಮುಂದಿನ_ಸಾಲು(cctx_T *cctx, char_u *arg, char_u **nextp)
{
    char_u *p = skipwhite(arg);

    *nextp = ಖಾಲಿ;
    ಆದರೆ (*p == NUL || (VIM_ISWHITE(*arg) && vim9_comment_ಆರಂಭ(p)))
    {
	*nextp = peek_ಮುಂದಿನ_ಸಾಲು_from_context(cctx);
	ಆದರೆ (*nextp != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ *nextp;
    }
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Return a pointer to the next line that isn't empty or only contains a
 * comment. Skips over white space.
 * Returns NULL if there is none.
 */
    char_u *
peek_ಮುಂದಿನ_ಸಾಲು_from_context(cctx_T *cctx)
{
    ಪೂರ್ಣಾಂಕ lnum = cctx->ctx_lnum;

    ಆಗ (++lnum < cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len)
    {
	char_u *ಸಾಲು = ((char_u **)cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_ದತ್ತಾಂಶ)[lnum];
	char_u *p;

	// ignore NULLs inserted for continuation lines
	ಆದರೆ (ಸಾಲು != ಖಾಲಿ)
	{
	    p = skipwhite(ಸಾಲು);
	    ಆದರೆ (vim9_bad_comment(p))
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ಆದರೆ (*p != NUL && !vim9_comment_ಆರಂಭ(p))
		ಹಿಂತಿರುಗಿ p;
	}
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Get the next line of the function from "cctx".
 * Skips over empty lines.  Skips over comment lines if "skip_comment" is TRUE.
 * Returns NULL when at the end.
 */
    char_u *
ಮುಂದಿನ_ಸಾಲು_from_context(cctx_T *cctx, ಪೂರ್ಣಾಂಕ skip_comment)
{
    char_u	*ಸಾಲು;

    ಮಾಡು
    {
	++cctx->ctx_lnum;
	ಆದರೆ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len)
	{
	    ಸಾಲು = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	}
	ಸಾಲು = ((char_u **)cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_ದತ್ತಾಂಶ)[cctx->ctx_lnum];
	cctx->ctx_ಸಾಲು_ಆರಂಭ = ಸಾಲು;
	SOURCING_LNUM = cctx->ctx_lnum + 1;
    } ಆಗ (ಸಾಲು == ಖಾಲಿ || *skipwhite(ಸಾಲು) == NUL
		     || (skip_comment && vim9_comment_ಆರಂಭ(skipwhite(ಸಾಲು))));
    ಹಿಂತಿರುಗಿ ಸಾಲು;
}

/*
 * Skip over white space at "whitep" and assign to "*arg".
 * If "*arg" is at the end of the line, advance to the next line.
 * Also when "whitep" points to white space and "*arg" is on a "#".
 * Return FAIL if beyond the last line, "*arg" is unmodified then.
 */
    ಪೂರ್ಣಾಂಕ
may_get_ಮುಂದಿನ_ಸಾಲು(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    *arg = skipwhite(whitep);
    ಆದರೆ (vim9_bad_comment(*arg))
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (**arg == NUL || (VIM_ISWHITE(*whitep) && vim9_comment_ಆರಂಭ(*arg)))
    {
	char_u *ಮುಂದಿನ = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, TRUE);

	ಆದರೆ (ಮುಂದಿನ == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	*arg = skipwhite(ಮುಂದಿನ);
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Idem, and give an error when failed.
 */
    ಪೂರ್ಣಾಂಕ
may_get_ಮುಂದಿನ_ಸಾಲು_error(char_u *whitep, char_u **arg, cctx_T *cctx)
{
    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು(whitep, arg, cctx) == FAIL)
    {
	SOURCING_LNUM = cctx->ctx_lnum + 1;
	emsg(_(e_ಸಾಲು_incomplete));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get a line from the compilation context, compatible with exarg_T getline().
 * Return a pointer to the line in allocated memory.
 * Return NULL for end-of-file or some error.
 */
    ಸ್ಥಿರ char_u *
exarg_getline(
	ಪೂರ್ಣಾಂಕ c ಬಳಸದ,
	ಶೂನ್ಯ *cookie,
	ಪೂರ್ಣಾಂಕ indent ಬಳಸದ,
	getline_opt_T options ಬಳಸದ)
{
    cctx_T  *cctx = (cctx_T *)cookie;
    char_u  *p;

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (cctx->ctx_lnum >= cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len - 1)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	++cctx->ctx_lnum;
	p = ((char_u **)cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_ದತ್ತಾಂಶ)[cctx->ctx_lnum];
	// Comment lines result in NULL pointers, skip them.
	ಆದರೆ (p != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ vim_strsave(p);
    }
}

    ಶೂನ್ಯ
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
    ಪೂರ್ಣಾಂಕ
func_needs_compiling(ufunc_T *ufunc, compiletype_T compile_type)
{
    ಆಯ್ಕೆ (ufunc->uf_def_ಸ್ಥಿತಿ)
    {
	ಸ್ಥಿತಿ UF_ಗಮ್ಯ_BE_COMPILED:
	    ಹಿಂತಿರುಗಿ TRUE;

	ಸ್ಥಿತಿ UF_COMPILED:
	{
	    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							 + ufunc->uf_dfunc_idx;

	    ಆಯ್ಕೆ (compile_type)
	    {
		ಸ್ಥಿತಿ CT_PROFILE:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
		    ಹಿಂತಿರುಗಿ dfunc->df_instr_prof == ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
		ಸ್ಥಿತಿ CT_NONE:
		    ಹಿಂತಿರುಗಿ dfunc->df_instr == ಖಾಲಿ;
		ಸ್ಥಿತಿ CT_DEBUG:
		    ಹಿಂತಿರುಗಿ dfunc->df_instr_debug == ಖಾಲಿ;
	    }
	}

	ಸ್ಥಿತಿ UF_NOT_COMPILED:
	ಸ್ಥಿತಿ UF_COMPILE_ERROR:
	ಸ್ಥಿತಿ UF_COMPILING:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Compile a nested :def command.
 */
    ಸ್ಥಿರ char_u *
compile_nested_function(exarg_T *eap, cctx_T *cctx, garray_T *ಸಾಲುಗಳು_ಗಮ್ಯ_free)
{
    ಪೂರ್ಣಾಂಕ		is_global = *eap->arg == 'g' && eap->arg[1] == ':';
    char_u	*name_ಆರಂಭ = eap->arg;
    char_u	*name_ಕೊನೆ = ಗಮ್ಯ_name_ಕೊನೆ(eap->arg, TRUE);
    ಪೂರ್ಣಾಂಕ		off;
    char_u	*func_name;
    string_T	lambda_name;
    ufunc_T	*ufunc;
    ಪೂರ್ಣಾಂಕ		r = FAIL;
    compiletype_T   compile_type;
    ಪೂರ್ಣಾಂಕ		funcref_isn_idx = -1;
    lvar_T	*lvar = ಖಾಲಿ;
    char_u	*bracket_ಆರಂಭ = ಖಾಲಿ;

    ಆದರೆ (eap->forceit)
    {
	semsg(_(e_cannot_use_bang_with_nested_def_str),
		eap->cmdidx == CMD_def ? ":def" : ":function");
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (*name_ಆರಂಭ == '/')
    {
	name_ಕೊನೆ = skip_regexp(name_ಆರಂಭ + 1, '/', TRUE);
	ಆದರೆ (*name_ಕೊನೆ == '/')
	    ++name_ಕೊನೆ;
	ಗಣ_nextcmd(eap, name_ಕೊನೆ);
    }

    ಆದರೆ (*name_ಕೊನೆ == '<')
    {
	bracket_ಆರಂಭ = name_ಕೊನೆ;
	ಆದರೆ (skip_generic_func_type_ವಾದಗಳು(&name_ಕೊನೆ) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (name_ಕೊನೆ == name_ಆರಂಭ || *skipwhite(name_ಕೊನೆ) != '(')
    {
	ಆದರೆ (!ತುದಿಗಳು_excmd2(name_ಆರಂಭ, name_ಕೊನೆ))
	{
	    ಆದರೆ (*skipwhite(name_ಕೊನೆ) == '.')
		semsg(_(e_cannot_define_dict_func_ಒಳ_vim9_script_str),
								     eap->cmd);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_invalid_command_str), eap->cmd);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}

	// "def" or "def Name": list functions
	ಆದರೆ (generate_DEF(cctx, name_ಆರಂಭ, name_ಕೊನೆ - name_ಆರಂಭ) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ eap->nextcmd == ಖಾಲಿ ? (char_u *)"" : eap->nextcmd;
    }

    ಆದರೆ (bracket_ಆರಂಭ != ಖಾಲಿ)
	// generic function.  The function name ends before the list of types
	// (opening angle bracket).
	name_ಕೊನೆ = bracket_ಆರಂಭ;

    // Only g:Func() can use a namespace.
    ಆದರೆ (name_ಆರಂಭ[1] == ':' && !is_global)
    {
	semsg(_(e_namespace_not_supported_str), name_ಆರಂಭ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (cctx->ctx_skip != SKIP_YES
	    && check_defined(name_ಆರಂಭ, name_ಕೊನೆ - name_ಆರಂಭ, cctx,
							  ಖಾಲಿ, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (!ASCII_ISUPPER(is_global ? name_ಆರಂಭ[2] : name_ಆರಂಭ[0]))
    {
	semsg(_(e_function_name_must_ಆರಂಭ_with_capital_str), name_ಆರಂಭ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    eap->arg = name_ಕೊನೆ;
    fill_exarg_from_cctx(eap, cctx);

    eap->forceit = FALSE;
    // We use the special <lambda>99 name, but it's not really a lambda.
    lambda_name = get_lambda_name();
    lambda_name.string = vim_strnsave(lambda_name.string, lambda_name.ಉದ್ದಳತೆ);
    ಆದರೆ (lambda_name.string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // This may free the current line, make a copy of the name.
    off = is_global ? 2 : 0;
    func_name = vim_strnsave(name_ಆರಂಭ + off, name_ಕೊನೆ - name_ಆರಂಭ - off);
    ಆದರೆ (func_name == ಖಾಲಿ)
    {
	r = FAIL;
	ಹೋಗು theend;
    }

    // A type variable cannot have the name of the function.  The function is
    // defined with the "<lambda>" name, check it here.
    ಆದರೆ (bracket_ಆರಂಭ != ಖಾಲಿ)
    {
	size_t	len = STRLEN(func_name);

	ಗಾಗಿ (char_u *t = bracket_ಆರಂಭ + 1; *t != '>' && *t != NUL; )
	{
	    char_u *te = ಗಮ್ಯ_name_ಕೊನೆ(t, FALSE);

	    ಆದರೆ ((size_t)(te - t) == len && STRNCMP(t, func_name, len) == 0)
	    {
		semsg(_(e_redefining_script_ಅಂಶ_str), func_name);
		r = FAIL;
		ಹೋಗು theend;
	    }
	    t = te == t ? t + 1 : te;
	}
    }

    // Make sure "KeyTyped" is not set, it may cause indent to be written.
    ಪೂರ್ಣಾಂಕ save_ಕೀಲಿ_Typed = ಕೀಲಿ_Typed;
    ಕೀಲಿ_Typed = FALSE;

    ufunc = define_function(eap, lambda_name.string, ಸಾಲುಗಳು_ಗಮ್ಯ_free, 0, ಖಾಲಿ, 0,
									cctx);

    ಕೀಲಿ_Typed = save_ಕೀಲಿ_Typed;

    ಆದರೆ (ufunc == ಖಾಲಿ)
    {
	r = eap->skip ? OK : FAIL;
	ಹೋಗು theend;
    }
    ಆದರೆ (eap->nextcmd != ಖಾಲಿ)
    {
	semsg(_(e_ಪಠ್ಯ_ಸಿಕ್ಕಿತು_after_str_str),
	      eap->cmdidx == CMD_def ? "enddef" : "endfunction", eap->nextcmd);
	r = FAIL;
	func_ptr_unref(ufunc);
	ಹೋಗು theend;
    }

    // copy over the block scope IDs before compiling
    ಆದರೆ (!is_global && cctx->ctx_ufunc->uf_ಖಂಡ_ಆಳ > 0)
    {
	ಪೂರ್ಣಾಂಕ ಖಂಡ_ಆಳ = cctx->ctx_ufunc->uf_ಖಂಡ_ಆಳ;

	ufunc->uf_ಖಂಡ_ids = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, ಖಂಡ_ಆಳ);
	ಆದರೆ (ufunc->uf_ಖಂಡ_ids != ಖಾಲಿ)
	{
	    mch_memmove(ufunc->uf_ಖಂಡ_ids, cctx->ctx_ufunc->uf_ಖಂಡ_ids,
						    ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * ಖಂಡ_ಆಳ);
	    ufunc->uf_ಖಂಡ_ಆಳ = ಖಂಡ_ಆಳ;
	}
    }

    // Define the funcref before compiling, so that it is found by any
    // recursive call.
    ಆದರೆ (is_global)
    {
	r = generate_NEWFUNC(cctx, lambda_name.string, func_name);
	func_name = ಖಾಲಿ;
	lambda_name.string = ಖಾಲಿ;
	lambda_name.ಉದ್ದಳತೆ = 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Define a local variable for the function reference.
	lvar = reserve_local(cctx, func_name, name_ಕೊನೆ - name_ಆರಂಭ,
					    ನಿಯೋಜಿಸು_CONST, ufunc->uf_func_type);
	ಆದರೆ (lvar == ಖಾಲಿ)
	{
	    func_ptr_unref(ufunc);
	    ಹೋಗು theend;
	}
	lvar->lv_ufunc = ufunc;
	ಆದರೆ (generate_FUNCREF(cctx, ufunc, ಖಾಲಿ, FALSE, 0, &funcref_isn_idx) == FAIL)
	{
	    func_ptr_unref(ufunc);
	    ಹೋಗು theend;
	}
	r = generate_STORE(cctx, ISN_STORE, lvar->lv_idx, ಖಾಲಿ);
    }

    compile_type = get_compile_type(ufunc);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    // If the outer function is profiled, also compile the nested function for
    // profiling.
    ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE)
	compile_type = CT_PROFILE;
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, TRUE, compile_type, cctx) == FAIL)
    {
	func_ptr_unref(ufunc);
	ಆದರೆ (lvar != ಖಾಲಿ)
	    // Now the local variable can't be used.
	    *lvar->lv_name = '/';  // impossible value
	ಹೋಗು theend;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    // When the outer function is compiled for profiling, the nested function
    // may be called without profiling.  Compile it here in the right context.
    ಆದರೆ (compile_type == CT_PROFILE && func_needs_compiling(ufunc, CT_NONE))
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);
#ಆದರೆ_ಅಂತ್ಯ

    // If a FUNCREF instruction was generated, set the index after compiling.
    ಆದರೆ (funcref_isn_idx != -1 && ufunc->uf_def_ಸ್ಥಿತಿ == UF_COMPILED)
    {
	isn_T	*funcref_isn = ((isn_T *)cctx->ctx_instr.ga_ದತ್ತಾಂಶ) +
							funcref_isn_idx;
	funcref_isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

theend:
    vim_free(lambda_name.string);
    vim_free(func_name);
    ಹಿಂತಿರುಗಿ r == FAIL ? ಖಾಲಿ : (char_u *)"";
}

/*
 * Compile one Vim expression {expr} in string "p".
 * "p" points to the opening "{".
 * Return a pointer to the character after "}", NULL for an error.
 */
    char_u *
compile_one_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(char_u *p, cctx_T *cctx)
{
    char_u	*ಖಂಡ_ಆರಂಭ;
    char_u	*ಖಂಡ_ಕೊನೆ;

    // Skip the opening {.
    ಖಂಡ_ಆರಂಭ = skipwhite(p + 1);
    ಖಂಡ_ಕೊನೆ = ಖಂಡ_ಆರಂಭ;
    ಆದರೆ (*ಖಂಡ_ಆರಂಭ != NUL && skip_ಅಭಿವ್ಯಕ್ತಿ(&ಖಂಡ_ಕೊನೆ, ಖಾಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಖಂಡ_ಕೊನೆ = skipwhite(ಖಂಡ_ಕೊನೆ);
    // The block must be closed by a }.
    ಆದರೆ (*ಖಂಡ_ಕೊನೆ != '}')
    {
	semsg(_(e_missing_close_curly_str), p);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (compile_expr0(&ಖಂಡ_ಆರಂಭ, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    may_generate_2STRING(-1, TOSTRING_INTERPOLATE, cctx);

    ಹಿಂತಿರುಗಿ ಖಂಡ_ಕೊನೆ + 1;
}

/*
 * Compile a string "str" (either containing a literal string or a mix of
 * literal strings and Vim expressions of the form `{expr}`).  This is used
 * when compiling a heredoc assignment to a variable or an interpolated string
 * in a Vim9 def function.  Vim9 instructions are generated to push strings,
 * evaluate expressions, concatenate them and create a list of lines.  When
 * "evalstr" is TRUE, Vim expressions in "str" are evaluated.
 */
    ಪೂರ್ಣಾಂಕ
compile_all_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(char_u *str, ಪೂರ್ಣಾಂಕ evalstr, cctx_T *cctx)
{
    char_u	*p = str;
    char_u	*val;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;

    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (!evalstr || *str == NUL)
    {
	// Literal string, possibly empty.
	val = *str != NUL ? vim_strsave(str) : ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ generate_PUSHS(cctx, &val);
    }

    // Push all the string pieces to the stack, followed by a ISN_CONCAT.
    ಆಗ (*p != NUL)
    {
	char_u	*lit_ಆರಂಭ;
	ಪೂರ್ಣಾಂಕ	escaped_brace = FALSE;

	// Look for a block start.
	lit_ಆರಂಭ = p;
	ಆಗ (*p != '{' && *p != '}' && *p != NUL)
	    ++p;

	ಆದರೆ (*p != NUL && *p == p[1])
	{
	    // Escaped brace, unescape and continue.
	    // Include the brace in the literal string.
	    ++p;
	    escaped_brace = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '}')
	{
	    semsg(_(e_stray_closing_curly_str), str);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// Append the literal part.
	ಆದರೆ (p != lit_ಆರಂಭ)
	{
	    val = vim_strnsave(lit_ಆರಂಭ, (size_t)(p - lit_ಆರಂಭ));
	    ಆದರೆ (generate_PUSHS(cctx, &val) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ++ಎಣಿಕೆ_2;
	}

	ಆದರೆ (*p == NUL)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (escaped_brace)
	{
	    // Skip the second brace.
	    ++p;
	    ಮುಂದುವರಿ;
	}

	p = compile_one_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(p, cctx);
	ಆದರೆ (p == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	++ಎಣಿಕೆ_2;
    }

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ಆದರೆ (ಎಣಿಕೆ_2 > 1)
	ಹಿಂತಿರುಗಿ generate_CONCAT(cctx, ಎಣಿಕೆ_2);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return the length of an assignment operator, or zero if there isn't one.
 */
    ಪೂರ್ಣಾಂಕ
assignment_len(char_u *p, ಪೂರ್ಣಾಂಕ *heredoc)
{
    ಆದರೆ (*p == '=')
    {
	ಆದರೆ (p[1] == '<' && p[2] == '<')
	{
	    *heredoc = TRUE;
	    ಹಿಂತಿರುಗಿ 3;
	}
	ಹಿಂತಿರುಗಿ 1;
    }
    ಆದರೆ (vim_strchr((char_u *)"+-*/%", *p) != ಖಾಲಿ && p[1] == '=')
	ಹಿಂತಿರುಗಿ 2;
    ಆದರೆ (STRNCMP(p, "..=", 3) == 0)
	ಹಿಂತಿರುಗಿ 3;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Generate the load instruction for "name".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
generate_loadvar(cctx_T *cctx, lhs_T *lhs)
{
    char_u	*name = lhs->lhs_name;
    type_T	*type = lhs->lhs_type;
    ಪೂರ್ಣಾಂಕ		res = OK;

    ಆಯ್ಕೆ (lhs->lhs_dest)
    {
	ಸ್ಥಿತಿ dest_option:
	ಸ್ಥಿತಿ dest_func_option:
	    generate_ತುಂಬು(cctx, ISN_LOADOPT, 0, name, type);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_global:
	    ಆದರೆ (vim_strchr(name, AUTOLOAD_CHAR) == ಖಾಲಿ)
	    {
		ಆದರೆ (name[2] == NUL)
		    generate_instr_type(cctx, ISN_LOADGDICT, &t_dict_any);
		ಇಲ್ಲದಿದ್ದರೆ
		    generate_ತುಂಬು(cctx, ISN_LOADG, 0, name + 2, type);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		generate_ತುಂಬು(cctx, ISN_LOADAUTO, 0, name, type);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_ಬಫರ್:
	    generate_ತುಂಬು(cctx, ISN_LOADB, 0, name + 2, type);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_ಕಿಟಕಿ:
	    generate_ತುಂಬು(cctx, ISN_LOADW, 0, name + 2, type);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_tab:
	    generate_ತುಂಬು(cctx, ISN_LOADT, 0, name + 2, type);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_script:
	ಸ್ಥಿತಿ dest_script_v9:
	    res = compile_ತುಂಬು_scriptvar(cctx,
			    name + (name[1] == ':' ? 2 : 0), ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_env:
	    // Include $ in the name here
	    generate_ತುಂಬು(cctx, ISN_LOADENV, 0, name, type);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_reg:
	    generate_ತುಂಬು(cctx, ISN_LOADREG, name[1], ಖಾಲಿ, &t_string);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_vimvar:
	    generate_LOADV(cctx, name + 2);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_local:
	    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
	    {
		lvar_T	*lvar = lhs->lhs_lvar;
		ಆದರೆ (lvar->lv_from_outer > 0)
		    generate_LOADOUTER(cctx, lvar->lv_idx, lvar->lv_from_outer,
				 lvar->lv_loop_ಆಳ, lvar->lv_loop_idx, type);
		ಇಲ್ಲದಿದ್ದರೆ
		    generate_ತುಂಬು(cctx, ISN_ತುಂಬು, lvar->lv_idx, ಖಾಲಿ, type);
	    }
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_class_member:
	    generate_CLASSMEMBER(cctx, TRUE, lhs->lhs_class,
						     lhs->lhs_classmember_idx);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ dest_ಅಭಿವ್ಯಕ್ತಿ:
	    // list or dict value should already be on the stack.
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ res;
}

/*
 * Skip over "[expr]" or ".member".
 * Does not check for any errors.
 */
    ಸ್ಥಿರ char_u *
skip_ಸೂಚ್ಯಂಕ(char_u *ಆರಂಭ)
{
    char_u *p = ಆರಂಭ;

    ಆದರೆ (*p == '[')
    {
	p = skipwhite(p + 1);
	(ಶೂನ್ಯ)skip_ಅಭಿವ್ಯಕ್ತಿ(&p, ಖಾಲಿ);
	p = skipwhite(p);
	ಆದರೆ (*p == ']')
	    ಹಿಂತಿರುಗಿ p + 1;
	ಹಿಂತಿರುಗಿ p;
    }
    // if (*p == '.')
    ಹಿಂತಿರುಗಿ ಗಮ್ಯ_name_ಕೊನೆ(p + 1, TRUE);
}

    ಶೂನ್ಯ
vim9_declare_error(char_u *name)
{
    ಅಕ್ಷರ *scope = "";

    ಆಯ್ಕೆ (*name)
    {
	ಸ್ಥಿತಿ 'g': scope = _("global"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'b': scope = _("buffer"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'w': scope = _("window"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 't': scope = _("tab"); ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'v': scope = "v:"; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '$': semsg(_(e_cannot_declare_an_environment_variable_str), name);
		  ಹಿಂತಿರುಗಿ;
	ಸ್ಥಿತಿ '&': semsg(_(e_cannot_declare_an_option_str), name);
		  ಹಿಂತಿರುಗಿ;
	ಸ್ಥಿತಿ '@': semsg(_(e_cannot_declare_a_ರಿಜಿಸ್ಟರ್_str), name);
		  ಹಿಂತಿರುಗಿ;
	ಪೂರ್ವನಿಯೋಜಿತ: ಹಿಂತಿರುಗಿ;
    }
    semsg(_(e_cannot_declare_a_scope_variable_str), scope, name);
}

/*
 * Return TRUE if "name" is a valid register to use.
 * Return FALSE and give an error message if not.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
valid_dest_reg(ಪೂರ್ಣಾಂಕ name)
{
    ಆದರೆ (name == '@')
       name = '"';
    ಆದರೆ (name == '/' || name == '=' || name == '#' || valid_yank_reg(name, TRUE))
	ಹಿಂತಿರುಗಿ TRUE;
    emsg_invreg(name);
    ಹಿಂತಿರುಗಿ FAIL;
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
    ಪೂರ್ಣಾಂಕ
get_var_dest(
	char_u		*name,
	ನಿಯೋಜಿಸು_dest_T	*dest,
	cmdidx_T	cmdidx,
	ಪೂರ್ಣಾಂಕ		has_ಸೂಚ್ಯಂಕ,
	ಪೂರ್ಣಾಂಕ		*option_scope,
	ಪೂರ್ಣಾಂಕ		*vimvaridx,
	type_T		**type,
	cctx_T		*cctx)
{
    char_u *p;

    ಆದರೆ (*name == '&')
    {
	ಪೂರ್ಣಾಂಕ		cc;
	ದೀರ್ಘ		numval;
	getoption_T	opt_type;
	ಪೂರ್ಣಾಂಕ		opt_p_flags;

	*dest = dest_option;
	ಆದರೆ (cmdidx == CMD_final || cmdidx == CMD_const)
	{
	    emsg(_(e_cannot_ಬೀಗ_option));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	p = name;
	p = ಕಂಡುಹಿಡಿ_option_ಕೊನೆ(&p, option_scope);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    // cannot happen?
	    emsg(_(e_unexpected_characters_ಒಳ_assignment));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	cc = *p;
	*p = NUL;
	opt_type = get_option_ಮೌಲ್ಯ(skip_option_env_lead(name),
				   &numval, ಖಾಲಿ, &opt_p_flags, *option_scope);
	*p = cc;
	ಆಯ್ಕೆ (opt_type)
	{
	    ಸ್ಥಿತಿ gov_unknown:
		    semsg(_(e_unknown_option_str), name);
		    ಹಿಂತಿರುಗಿ FAIL;
	    ಸ್ಥಿತಿ gov_string:
	    ಸ್ಥಿತಿ gov_hidden_string:
		    ಆದರೆ (opt_p_flags & P_FUNC)
		    {
			// might be a Funcref, check the type later
			*type = &t_any;
			*dest = dest_func_option;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			*type = &t_string;
		    }
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ gov_bool:
	    ಸ್ಥಿತಿ gov_hidden_bool:
		    *type = &t_bool;
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ gov_ಸಂಖ್ಯೆ:
	    ಸ್ಥಿತಿ gov_hidden_ಸಂಖ್ಯೆ:
		    *type = &t_ಸಂಖ್ಯೆ;
		    ನಿಲ್ಲಿಸು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*name == '$')
    {
	*dest = dest_env;
	*type = &t_string;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*name == '@')
    {
	ಆದರೆ (!valid_dest_reg(name[1]))
	    ಹಿಂತಿರುಗಿ FAIL;
	*dest = dest_reg;
	*type = name[1] == '#' ? &t_ಸಂಖ್ಯೆ_or_string : &t_string;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(name, "g:", 2) == 0)
    {
	*dest = dest_global;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(name, "b:", 2) == 0)
    {
	*dest = dest_ಬಫರ್;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(name, "w:", 2) == 0)
    {
	*dest = dest_ಕಿಟಕಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(name, "t:", 2) == 0)
    {
	*dest = dest_tab;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(name, "v:", 2) == 0)
    {
	typval_T	*vtv;
	ಪೂರ್ಣಾಂಕ		di_flags;

	*vimvaridx = ಕಂಡುಹಿಡಿ_vim_var(name + 2, &di_flags);
	ಆದರೆ (*vimvaridx < 0)
	{
	    semsg(_(e_variable_not_ಸಿಕ್ಕಿತು_str), name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	// An item of a read-only variable can be changed, as when not compiled.
	// We use the current value of "sandbox" here, is that OK?
	ಆದರೆ (!has_ಸೂಚ್ಯಂಕ && var_check_ro(di_flags, name, FALSE))
	    ಹಿಂತಿರುಗಿ FAIL;
	*dest = dest_vimvar;
	vtv = get_vim_var_tv(*vimvaridx);
	*type = typval2type_vimvar(vtv, cctx->ctx_type_list);
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_decl_command(cmdidx_T cmdidx)
{
    ಹಿಂತಿರುಗಿ cmdidx == CMD_let || cmdidx == CMD_var
				 || cmdidx == CMD_final || cmdidx == CMD_const;
}

/*
 * Returns TRUE if the class or object variable in "lhs" is modifiable.
 * "var_start" points to the start of the variable name and "lhs->lhs_varlen"
 * has the total length.  Note that the "lhs" can be nested an object reference
 * (e.g.  a.b.c.d.var).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lhs_class_member_modifiable(lhs_T *lhs, char_u	*var_ಆರಂಭ, cctx_T *cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    class_T	*cl = lhs->lhs_type->tt_class;
    ಪೂರ್ಣಾಂಕ		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_ಆರಂಭ + varlen + 1;
    size_t	namelen = lhs->lhs_ಕೊನೆ - var_ಆರಂಭ - varlen - 1;
    ocmember_T	*m;

    m = member_lookup(cl, lhs->lhs_type->tt_type, name, namelen, ಖಾಲಿ);
    ಆದರೆ (m == ಖಾಲಿ)
    {
	member_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, lhs->lhs_type->tt_type, name, namelen);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಆದರೆ (IS_ENUM(cl))
    {
	semsg(_(e_enumvalue_str_cannot_be_modified), cl->class_name.string,
		m->ocm_name.string);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    ಆದರೆ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_class != cl)))
    {
	ಅಕ್ಷರ *ಸಂದೇಶ = (m->ocm_access == VIM_ACCESS_ಖಾಸಗಿ)
				? e_cannot_access_protected_variable_str
				: e_variable_is_not_writable_str;
	emsg_var_cl_define(ಸಂದೇಶ, m->ocm_name.string, 0, cl);
	ಹಿಂತಿರುಗಿ FALSE;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Initialize "lhs" with default values
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
lhs_init_defaults(lhs_T *lhs)
{
    CLEAR_ಸೂಚಕ(lhs);
    lhs->lhs_dest = dest_local;
    lhs->lhs_vimvaridx = -1;
    lhs->lhs_scriptvar_idx = -1;
    lhs->lhs_member_idx = -1;
}

/*
 * When compiling a LHS variable name, find the end of the destination and the
 * end of the variable name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lhs_ಕಂಡುಹಿಡಿ_var_ಕೊನೆ(
    lhs_T	*lhs,
    char_u	*var_ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		is_decl,
    char_u	**var_endp)
{
    char_u  *var_ಕೊನೆ = *var_endp;

    // "lhs_dest_end" is the end of the destination, including "[expr]" or
    // ".name".
    // "var_end" is the end of the variable/option/etc. name.
    lhs->lhs_dest_ಕೊನೆ = skip_var_one(var_ಆರಂಭ, FALSE);
    ಆದರೆ (*var_ಆರಂಭ == '@')
    {
	ಆದರೆ (!valid_dest_reg(var_ಆರಂಭ[1]))
	    ಹಿಂತಿರುಗಿ FAIL;
	var_ಕೊನೆ = var_ಆರಂಭ + 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// skip over the leading "&", "&l:", "&g:" and "$"
	var_ಕೊನೆ = skip_option_env_lead(var_ಆರಂಭ);
	var_ಕೊನೆ = ಗಮ್ಯ_name_ಕೊನೆ(var_ಕೊನೆ, TRUE);
    }

    // "a: type" is declaring variable "a" with a type, not dict "a:".
    ಆದರೆ (is_decl && lhs->lhs_dest_ಕೊನೆ == var_ಆರಂಭ + 2
					&& lhs->lhs_dest_ಕೊನೆ[-1] == ':')
	--lhs->lhs_dest_ಕೊನೆ;
    ಆದರೆ (is_decl && var_ಕೊನೆ == var_ಆರಂಭ + 2 && var_ಕೊನೆ[-1] == ':')
	--var_ಕೊನೆ;

    lhs->lhs_ಕೊನೆ = lhs->lhs_dest_ಕೊನೆ;
    *var_endp = var_ಕೊನೆ;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set various fields in "lhs"
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lhs_init(
    lhs_T	*lhs,
    char_u	*var_ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		is_decl,
    ಪೂರ್ಣಾಂಕ		heredoc,
    char_u	**var_endp)
{
    char_u *var_ಕೊನೆ = *var_endp;

    lhs_init_defaults(lhs);

    // Find the end of the variable and the destination
    ಆದರೆ (lhs_ಕಂಡುಹಿಡಿ_var_ಕೊನೆ(lhs, var_ಆರಂಭ, is_decl, &var_ಕೊನೆ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // compute the length of the destination without "[expr]" or ".name"
    lhs->lhs_varlen = var_ಕೊನೆ - var_ಆರಂಭ;
    lhs->lhs_varlen_ಒಟ್ಟು = lhs->lhs_varlen;
    lhs->lhs_name = vim_strnsave(var_ಆರಂಭ, lhs->lhs_varlen);
    ಆದರೆ (lhs->lhs_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (lhs->lhs_dest_ಕೊನೆ > var_ಆರಂಭ + lhs->lhs_varlen)
	// Something follows after the variable: "var[idx]" or "var.key".
	lhs->lhs_has_ಸೂಚ್ಯಂಕ = TRUE;

    lhs->lhs_type = heredoc ? &t_list_string : &t_any;

    *var_endp = var_ಕೊನೆ;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile a LHS class variable name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_lhs_class_variable(
    cctx_T	*cctx,
    lhs_T	*lhs,
    class_T	*defcl,
    ಪೂರ್ಣಾಂಕ		is_decl)
{
    ಆದರೆ (cctx->ctx_ufunc->uf_defclass != defcl)
    {
	// A class variable can be accessed without the class name
	// only inside a class.
	semsg(_(e_class_variable_str_accessible_only_inside_class_str),
		lhs->lhs_name, defcl->class_name.string);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (is_decl)
    {
	semsg(_(e_variable_already_declared_ಒಳ_class_str), lhs->lhs_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ocmember_T	*m = &defcl->class_class_members[lhs->lhs_classmember_idx];
    ಆದರೆ (oc_var_check_ro(defcl, m))
	ಹಿಂತಿರುಗಿ FAIL;

    lhs->lhs_dest = dest_class_member;
    // The class variable is defined either in the current class or
    // in one of the parent class in the hierarchy.
    lhs->lhs_class = defcl;
    lhs->lhs_type = oc_member_type_by_idx(defcl, FALSE,
						lhs->lhs_classmember_idx);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile an imported LHS variable
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_lhs_import_var(
    lhs_T	*lhs,
    imported_T	*import,
    char_u	*var_ಆರಂಭ,
    char_u	**var_endp,
    char_u	**rawnamep)
{
    char_u	*var_ಕೊನೆ = *var_endp;
    char_u	*dot = vim_strchr(var_ಆರಂಭ, '.');
    char_u	*p;

    // for an import the name is what comes after the dot
    ಆದರೆ (dot == ಖಾಲಿ)
    {
	semsg(_(e_no_dot_after_imported_name_str), var_ಆರಂಭ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    p = skipwhite(dot + 1);
    var_ಕೊನೆ = ಗಮ್ಯ_name_ಕೊನೆ(p, TRUE);
    ಆದರೆ (var_ಕೊನೆ == p)
    {
	semsg(_(e_missing_name_after_imported_name_str), var_ಆರಂಭ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    vim_free(lhs->lhs_name);
    lhs->lhs_varlen = var_ಕೊನೆ - p;
    lhs->lhs_name = vim_strnsave(p, lhs->lhs_varlen);
    ಆದರೆ (lhs->lhs_name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    *rawnamep = lhs->lhs_name;
    lhs->lhs_scriptvar_sid = import->imp_sid;

    // TODO: where do we check this name is exported?

    // Check if something follows: "exp.var[idx]" or
    // "exp.var.key".
    lhs->lhs_has_ಸೂಚ್ಯಂಕ = lhs->lhs_dest_ಕೊನೆ > skipwhite(var_ಕೊನೆ);

    *var_endp = var_ಕೊನೆ;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Process a script-local variable when compiling a LHS variable name.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_lhs_script_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ಆರಂಭ,
    char_u	*var_ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		is_decl)
{
    ಪೂರ್ಣಾಂಕ		script_namespace = FALSE;
    ಪೂರ್ಣಾಂಕ		script_var = FALSE;
    imported_T	*import;
    char_u	*var_name;
    size_t	var_name_len;

    ಆದರೆ (lhs->lhs_varlen > 1 && STRNCMP(var_ಆರಂಭ, "s:", 2) == 0)
	script_namespace = TRUE;

    ಆದರೆ (script_namespace)
    {
	var_name = var_ಆರಂಭ + 2;
	var_name_len = lhs->lhs_varlen - 2;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	var_name = var_ಆರಂಭ;
	var_name_len = lhs->lhs_varlen;
    }

    ಆದರೆ (script_var_exists(var_name, var_name_len, cctx, ಖಾಲಿ) == OK)
	script_var = TRUE;

    import = ಕಂಡುಹಿಡಿ_imported(var_ಆರಂಭ, lhs->lhs_varlen, FALSE);

    ಆದರೆ (script_namespace || script_var || import != ಖಾಲಿ)
    {
	char_u *rawname = lhs->lhs_name + (lhs->lhs_name[1] == ':' ? 2 : 0);

	ಆದರೆ (script_namespace && current_script_is_vim9())
	{
	    semsg(_(e_cannot_use_s_colon_ಒಳ_vim9_script_str), var_ಆರಂಭ);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (is_decl)
	{
	    ಆದರೆ (script_namespace)
		semsg(_(e_cannot_declare_script_variable_ಒಳ_function_str),
			lhs->lhs_name);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_variable_already_declared_ಒಳ_script_str),
			lhs->lhs_name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cctx->ctx_ufunc->uf_script_ctx_ಆವೃತ್ತಿ == SCRIPT_ಆವೃತ್ತಿ_VIM9
		&& script_namespace
		&& !script_var && import == ಖಾಲಿ)
	{
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	lhs->lhs_dest = current_script_is_vim9() ? dest_script_v9 :
								dest_script;

	// existing script-local variables should have a type
	lhs->lhs_scriptvar_sid = current_sctx.sc_sid;
	ಆದರೆ (import != ಖಾಲಿ)
	{
	    ಆದರೆ (compile_lhs_import_var(lhs, import, var_ಆರಂಭ, &var_ಕೊನೆ,
							&rawname) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (SCRIPT_ID_VALID(lhs->lhs_scriptvar_sid))
	{
	    // Check writable only when no index follows.
	    lhs->lhs_scriptvar_idx = get_script_ಅಂಶ_idx(
					lhs->lhs_scriptvar_sid, rawname,
					lhs->lhs_has_ಸೂಚ್ಯಂಕ ?  ನಿಯೋಜಿಸು_FINAL :
					ನಿಯೋಜಿಸು_CONST, cctx, ಖಾಲಿ);
	    ಆದರೆ (lhs->lhs_scriptvar_idx >= 0)
	    {
		scriptitem_T *si = SCRIPT_ಅಂಶ(lhs->lhs_scriptvar_sid);
		svar_T	 *sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ)
						+ lhs->lhs_scriptvar_idx;

		lhs->lhs_type = sv->sv_type;
	    }
	}

	ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ check_defined(var_ಆರಂಭ, lhs->lhs_varlen, cctx, ಖಾಲಿ, FALSE);
}

/*
 * Compile the LHS destination.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_lhs_var_dest(
    cctx_T	*cctx,
    lhs_T	*lhs,
    ಪೂರ್ಣಾಂಕ		cmdidx,
    char_u	*var_ಆರಂಭ,
    char_u	*var_ಕೊನೆ,
    ಪೂರ್ಣಾಂಕ		is_decl,
    ಪೂರ್ಣಾಂಕ		has_cmd)	// "var" before "var_start"
{
    ಪೂರ್ಣಾಂಕ	    declare_error = FALSE;

    ಆದರೆ (get_var_dest(lhs->lhs_name, &lhs->lhs_dest, cmdidx,
				lhs->lhs_has_ಸೂಚ್ಯಂಕ,
				&lhs->lhs_opt_flags, &lhs->lhs_vimvaridx,
				&lhs->lhs_type, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (lhs->lhs_dest != dest_local && cmdidx != CMD_const
						&& cmdidx != CMD_final)
    {
	// Specific kind of variable recognized.
	declare_error = is_decl;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	class_T	*defcl;

	// No specific kind of variable recognized, just a name.
	ಆದರೆ (check_reserved_name(lhs->lhs_name, lhs->lhs_has_ಸೂಚ್ಯಂಕ
						&& *var_ಕೊನೆ == '.') == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (lookup_local(var_ಆರಂಭ, lhs->lhs_varlen, &lhs->lhs_local_lvar,
								cctx) == OK)
	{
	    lhs->lhs_lvar = &lhs->lhs_local_lvar;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    CLEAR_ಕ್ಷೇತ್ರ(lhs->lhs_arg_lvar);
	    ಆದರೆ (arg_exists(var_ಆರಂಭ, lhs->lhs_varlen,
			&lhs->lhs_arg_lvar.lv_idx, &lhs->lhs_arg_lvar.lv_type,
			&lhs->lhs_arg_lvar.lv_from_outer, cctx) == OK)
	    {
		ಆದರೆ (is_decl)
		{
		    semsg(_(e_str_is_ಬಳಸಿದ_as_ವಾದ), lhs->lhs_name);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		lhs->lhs_lvar = &lhs->lhs_arg_lvar;
	    }
	}

	ಆದರೆ (lhs->lhs_lvar != ಖಾಲಿ)
	{
	    ಆದರೆ (is_decl)
	    {
		// if we come here with what looks like an assignment like
		// .= but which has been rejected by assignment_len() from
		// may_compile_assignment give a better error message
		char_u *p = skipwhite(lhs->lhs_ಕೊನೆ);
		ಆದರೆ (p[0] == '.' && p[1] == '=')
		    emsg(_(e_dot_equal_not_supported_with_script_ಆವೃತ್ತಿ_two));
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == ':' && !has_cmd)
		    // type specified in an assignment without "var"
		    semsg(_(e_trailing_characters_str), p);
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_variable_already_declared_str), lhs->lhs_name);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((lhs->lhs_classmember_idx = cctx_class_member_idx(
			cctx, var_ಆರಂಭ, lhs->lhs_varlen, &defcl)) >= 0)
	{
	    ಆದರೆ (compile_lhs_class_variable(cctx, lhs, defcl, is_decl)
		    == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (compile_lhs_script_var(cctx, lhs, var_ಆರಂಭ, var_ಕೊನೆ,
			is_decl) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಆದರೆ (declare_error)
    {
	vim9_declare_error(lhs->lhs_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * When compiling a LHS variable name, for a class or an object, set the LHS
 * member type.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_lhs_ಗಣ_oc_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ಆರಂಭ)
{
    class_T	*cl = lhs->lhs_type->tt_class;
    ಪೂರ್ಣಾಂಕ		is_object = lhs->lhs_type->tt_type == VAR_OBJECT;
    char_u	*name = var_ಆರಂಭ + lhs->lhs_varlen + 1;
    size_t	namelen = lhs->lhs_ಕೊನೆ - var_ಆರಂಭ - lhs->lhs_varlen - 1;

    ocmember_T	*m = member_lookup(cl, lhs->lhs_type->tt_type,
	    name, namelen, &lhs->lhs_member_idx);
    ಆದರೆ (m == ಖಾಲಿ)
    {
	member_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, lhs->lhs_type->tt_type, name, namelen);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (IS_ENUM(cl))
    {
	ಆದರೆ (!inside_class(cctx, cl))
	{
	    semsg(_(e_enumvalue_str_cannot_be_modified),
		    cl->class_name.string, m->ocm_name.string);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (lhs->lhs_type->tt_type == VAR_OBJECT &&
		lhs->lhs_member_idx < 2)
	{
	    ಅಕ್ಷರ *ಸಂದೇಶ = lhs->lhs_member_idx == 0 ?
		e_enum_str_name_cannot_be_modified :
		e_enum_str_ordinal_cannot_be_modified;
	    semsg(_(ಸಂದೇಶ), cl->class_name.string);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    // If it is private member variable, then accessing it outside the
    // class is not allowed.
    // If it is a read only class variable, then it can be modified
    // only inside the class where it is defined.
    ಆದರೆ ((m->ocm_access != VIM_ACCESS_ALL) &&
	    ((is_object && !inside_class(cctx, cl))
	     || (!is_object && cctx->ctx_ufunc->uf_defclass != cl)))
    {
	ಅಕ್ಷರ *ಸಂದೇಶ = (m->ocm_access == VIM_ACCESS_ಖಾಸಗಿ)
	    ? e_cannot_access_protected_variable_str
	    : e_variable_is_not_writable_str;
	emsg_var_cl_define(ಸಂದೇಶ, m->ocm_name.string, 0, cl);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (!IS_CONSTRUCTOR_ವಿಧಾನ(cctx->ctx_ufunc)
	    && oc_var_check_ro(cl, m))
	ಹಿಂತಿರುಗಿ FAIL;

    lhs->lhs_member_type = m->ocm_type;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * When compiling a LHS variable, set the LHS variable type.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_lhs_ಗಣ_type(cctx_T *cctx, lhs_T *lhs, char_u *var_ಕೊನೆ, ಪೂರ್ಣಾಂಕ is_decl)
{
    ಆದರೆ (is_decl && *skipwhite(var_ಕೊನೆ) == ':')
    {
	char_u *p;

	// parse optional type: "let var: type = expr"
	ಆದರೆ (VIM_ISWHITE(*var_ಕೊನೆ))
	{
	    semsg(_(e_no_white_space_allowed_before_colon_str), var_ಕೊನೆ);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (!VIM_ISWHITE(var_ಕೊನೆ[1]))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", var_ಕೊನೆ);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	p = skipwhite(var_ಕೊನೆ + 1);
	lhs->lhs_type = parse_type(&p, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ಆದರೆ (lhs->lhs_type == ಖಾಲಿ
		|| !valid_declaration_type(lhs->lhs_type))
	    ಹಿಂತಿರುಗಿ FAIL;

	lhs->lhs_has_type = TRUE;
	lhs->lhs_ಕೊನೆ = p;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lhs->lhs_lvar != ಖಾಲಿ)
	lhs->lhs_type = lhs->lhs_lvar->lv_type;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Returns TRUE if "lhs" is a concatenable string.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lhs_concatenable(lhs_T *lhs)
{
    ಹಿಂತಿರುಗಿ lhs->lhs_dest == dest_global
		|| lhs->lhs_has_ಸೂಚ್ಯಂಕ
		|| lhs->lhs_type->tt_type == VAR_STRING
		|| lhs->lhs_type->tt_type == VAR_ANY;
}

/*
 * Create a new local variable when compiling a LHS variable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_lhs_new_local_var(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		cmdidx,
    ಪೂರ್ಣಾಂಕ		oplen,
    ಪೂರ್ಣಾಂಕ		is_decl,
    ಪೂರ್ಣಾಂಕ		has_cmd,
    ಪೂರ್ಣಾಂಕ		heredoc)
{
    ಆದರೆ (oplen > 1 && !heredoc)
    {
	// +=, /=, etc. require an existing variable
	semsg(_(e_cannot_use_operator_on_new_variable_str), lhs->lhs_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (!is_decl || (lhs->lhs_has_ಸೂಚ್ಯಂಕ && !has_cmd
					&& cctx->ctx_skip != SKIP_YES))
    {
	semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Check the name is valid for a funcref.
    ಆದರೆ (lhs->lhs_type->tt_type == VAR_FUNC
				|| lhs->lhs_type->tt_type == VAR_PARTIAL)
    {
	ಆದರೆ (var_wrong_func_name(lhs->lhs_name, TRUE))
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // New local variable.
    ಪೂರ್ಣಾಂಕ ನಿಯೋಜಿಸು;
    ಆಯ್ಕೆ (cmdidx)
    {
	ಸ್ಥಿತಿ CMD_final:
	    ನಿಯೋಜಿಸು = ನಿಯೋಜಿಸು_FINAL; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CMD_const:
	    ನಿಯೋಜಿಸು = ನಿಯೋಜಿಸು_CONST; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಯೋಜಿಸು = ನಿಯೋಜಿಸು_VAR; ನಿಲ್ಲಿಸು;
    }

    lhs->lhs_lvar = reserve_local(cctx, var_ಆರಂಭ, lhs->lhs_varlen, ನಿಯೋಜಿಸು,
							lhs->lhs_type);
    ಆದರೆ (lhs->lhs_lvar == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    lhs->lhs_new_local = TRUE;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * When compiling a LHS variable name, set the LHS member type.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_lhs_ಗಣ_member_type(
    cctx_T	*cctx,
    lhs_T	*lhs,
    char_u	*var_ಆರಂಭ,
    ಪೂರ್ಣಾಂಕ		is_decl,
    ಪೂರ್ಣಾಂಕ		has_cmd)
{
    lhs->lhs_member_type = lhs->lhs_type;

    ಆದರೆ (!lhs->lhs_has_ಸೂಚ್ಯಂಕ)
	ಹಿಂತಿರುಗಿ OK;

    char_u	*after = var_ಆರಂಭ + lhs->lhs_varlen;
    char_u	*p;

    // Something follows after the variable: "var[idx]" or "var.key".
    ಆದರೆ (is_decl && cctx->ctx_skip != SKIP_YES)
    {
	ಆದರೆ (has_cmd)
	    emsg(_(e_cannot_use_ಸೂಚ್ಯಂಕ_when_declaring_variable));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_unknown_variable_str), lhs->lhs_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Now: var_start[lhs->lhs_varlen] is '[' or '.'
    // Only the last index is used below, if there are others
    // before it generate code for the expression.  Thus for
    // "ll[1][2]" the expression is "ll[1]" and "[2]" is the index.
    ಗಾಗಿ (;;)
    {
	p = skip_ಸೂಚ್ಯಂಕ(after);
	ಆದರೆ (*p != '[' && *p != '.')
	{
	    lhs->lhs_varlen_ಒಟ್ಟು = p - var_ಆರಂಭ;
	    ನಿಲ್ಲಿಸು;
	}
	after = p;
    }
    ಆದರೆ (after > var_ಆರಂಭ + lhs->lhs_varlen)
    {
	lhs->lhs_varlen = after - var_ಆರಂಭ;
	lhs->lhs_dest = dest_ಅಭಿವ್ಯಕ್ತಿ;
	// We don't know the type before evaluating the expression,
	// use "any" until then.  The member index is for the first name,
	// not for the last index.
	lhs->lhs_type = &t_any;
	lhs->lhs_member_idx = -1;
    }

    ಪೂರ್ಣಾಂಕ use_class = lhs->lhs_type != ಖಾಲಿ
	&& (lhs->lhs_type->tt_type == VAR_CLASS
		|| lhs->lhs_type->tt_type == VAR_OBJECT);

    ಆದರೆ (lhs->lhs_type == ಖಾಲಿ
	    || (use_class ? lhs->lhs_type->tt_class == ಖಾಲಿ
		: lhs->lhs_type->tt_member == ಖಾಲಿ))
    {
	lhs->lhs_member_type = &t_any;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_class)
    {
	// for an object or class member get the type of the member
	ಆದರೆ (compile_lhs_ಗಣ_oc_member_type(cctx, lhs, var_ಆರಂಭ) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	lhs->lhs_member_type = lhs->lhs_type->tt_member;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Figure out the LHS type and other properties for an assignment or one item
 * of ":unlet" with an index.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
compile_lhs(
	char_u	    *var_ಆರಂಭ,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	ಪೂರ್ಣಾಂಕ	    heredoc,
	ಪೂರ್ಣಾಂಕ	    has_cmd,	    // "var" before "var_start"
	ಪೂರ್ಣಾಂಕ	    oplen,
	cctx_T	    *cctx)
{
    char_u	*var_ಕೊನೆ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		is_decl = is_decl_command(cmdidx);

    ಆದರೆ (lhs_init(lhs, var_ಆರಂಭ, is_decl, heredoc, &var_ಕೊನೆ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	// compile the LHS destination
	ಆದರೆ (compile_lhs_var_dest(cctx, lhs, cmdidx, var_ಆರಂಭ, var_ಕೊನೆ,
						is_decl, has_cmd) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // handle "a:name" as a name, not index "name" in "a"
    ಆದರೆ (lhs->lhs_varlen > 1 || var_ಆರಂಭ[lhs->lhs_varlen] != ':')
	var_ಕೊನೆ = lhs->lhs_dest_ಕೊನೆ;

    ಆದರೆ (lhs->lhs_dest != dest_option && lhs->lhs_dest != dest_func_option)
    {
	// set the LHS variable type
	ಆದರೆ (compile_lhs_ಗಣ_type(cctx, lhs, var_ಕೊನೆ, is_decl) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (oplen == 3 && !heredoc && !lhs_concatenable(lhs))
    {
	emsg(_(e_can_only_concatenate_ಗಮ್ಯ_string));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (lhs->lhs_lvar == ಖಾಲಿ && lhs->lhs_dest == dest_local
						&& cctx->ctx_skip != SKIP_YES)
    {
	ಆದರೆ (compile_lhs_new_local_var(cctx, lhs, var_ಆರಂಭ, cmdidx, oplen,
					is_decl, has_cmd, heredoc) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (compile_lhs_ಗಣ_member_type(cctx, lhs, var_ಆರಂಭ, is_decl, has_cmd)
								== FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Figure out the LHS and check a few errors.
 */
    ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_lhs(
	char_u	    *var_ಆರಂಭ,
	lhs_T	    *lhs,
	cmdidx_T    cmdidx,
	ಪೂರ್ಣಾಂಕ	    is_decl,
	ಪೂರ್ಣಾಂಕ	    heredoc,
	ಪೂರ್ಣಾಂಕ	    has_cmd,	    // "var" before "var_start"
	ಪೂರ್ಣಾಂಕ	    oplen,
	cctx_T	    *cctx)
{
    ಆದರೆ (compile_lhs(var_ಆರಂಭ, lhs, cmdidx, heredoc, has_cmd, oplen, cctx)
								       == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (!lhs->lhs_has_ಸೂಚ್ಯಂಕ && lhs->lhs_lvar == &lhs->lhs_arg_lvar)
    {
	semsg(_(e_cannot_ನಿಯೋಜಿಸು_ಗಮ್ಯ_ವಾದ_str), lhs->lhs_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (!is_decl && lhs->lhs_lvar != ಖಾಲಿ
			   && lhs->lhs_lvar->lv_const != ನಿಯೋಜಿಸು_VAR
			   && !lhs->lhs_has_ಸೂಚ್ಯಂಕ)
    {
	semsg(_(e_cannot_ನಿಯೋಜಿಸು_ಗಮ್ಯ_constant_str), lhs->lhs_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if "lhs" has a range index: "[expr : expr]".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
has_list_ಸೂಚ್ಯಂಕ(char_u *idx_ಆರಂಭ, cctx_T *cctx)
{
    char_u  *p = idx_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ	    save_skip;

    ಆದರೆ (*p != '[')
	ಹಿಂತಿರುಗಿ FALSE;

    p = skipwhite(p + 1);
    ಆದರೆ (*p == ':')
	ಹಿಂತಿರುಗಿ TRUE;

    save_skip = cctx->ctx_skip;
    cctx->ctx_skip = SKIP_YES;
    (ಶೂನ್ಯ)compile_expr0(&p, cctx);
    cctx->ctx_skip = save_skip;
    ಹಿಂತಿರುಗಿ *skipwhite(p) == ':';
}

/*
 * For an assignment with an index, compile the "idx" in "var[idx]" or "key" in
 * "var.key".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_ಸೂಚ್ಯಂಕ(
	char_u	*var_ಆರಂಭ,
	lhs_T	*lhs,
	ಪೂರ್ಣಾಂಕ	*range,
	cctx_T	*cctx)
{
    size_t	varlen = lhs->lhs_varlen;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		r = OK;
    ಪೂರ್ಣಾಂಕ		need_white_before = TRUE;
    ಪೂರ್ಣಾಂಕ		ಖಾಲಿಗಳು_ಎರಡನೇ;

    p = var_ಆರಂಭ + varlen;
    ಆದರೆ (*p == '[')
    {
	p = skipwhite(p + 1);
	ಆದರೆ (*p == ':')
	{
	    // empty first index, push zero
	    r = generate_PUSHNR(cctx, 0);
	    need_white_before = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    r = compile_expr0(&p, cctx);

	ಆದರೆ (r == OK && *skipwhite(p) == ':')
	{
	    // unlet var[idx : idx]
	    // blob[idx : idx] = value
	    *range = TRUE;
	    p = skipwhite(p);
	    ಖಾಲಿಗಳು_ಎರಡನೇ = *skipwhite(p + 1) == ']';
	    ಆದರೆ ((need_white_before && !IS_WHITE_OR_NUL(p[-1]))
		    || (!ಖಾಲಿಗಳು_ಎರಡನೇ && !IS_WHITE_OR_NUL(p[1])))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								      ":", p);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    p = skipwhite(p + 1);
	    ಆದರೆ (*p == ']')
		// empty second index, push "none"
		r = generate_PUSHSPEC(cctx, VVAL_NONE);
	    ಇಲ್ಲದಿದ್ದರೆ
		r = compile_expr0(&p, cctx);
	}

	ಆದರೆ (r == OK && *skipwhite(p) != ']')
	{
	    // this should not happen
	    emsg(_(e_missing_closing_ವರ್ಗಫಲ_brace));
	    r = FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lhs->lhs_member_idx >= 0)
    {
	// object member index
	r = generate_PUSHNR(cctx, lhs->lhs_member_idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ // if (*p == '.')
    {
	char_u *ಕೀಲಿ_ಕೊನೆ = ಗಮ್ಯ_name_ಕೊನೆ(p + 1, TRUE);
	char_u *ಕೀಲಿ = vim_strnsave(p + 1, ಕೀಲಿ_ಕೊನೆ - p - 1);

	r = generate_PUSHS(cctx, &ಕೀಲಿ);
    }
    ಹಿಂತಿರುಗಿ r;
}

/*
 * For a LHS with an index, load the variable to be indexed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ತುಂಬು_lhs(
	lhs_T	*lhs,
	char_u	*var_ಆರಂಭ,
	type_T	*rhs_type,
	cctx_T	*cctx)
{
    ಆದರೆ (lhs->lhs_dest == dest_ಅಭಿವ್ಯಕ್ತಿ)
    {
	size_t	    varlen = lhs->lhs_varlen;
	ಪೂರ್ಣಾಂಕ	    c = var_ಆರಂಭ[varlen];
	ಪೂರ್ಣಾಂಕ	    ಸಾಲುಗಳು_len = cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len;
	ಪೂರ್ಣಾಂಕ	    res;

	// Evaluate "ll[expr]" of "ll[expr][idx]".  End the line with a NUL and
	// limit the lines array length to avoid skipping to a following line.
	var_ಆರಂಭ[varlen] = NUL;
	cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len = cctx->ctx_lnum + 1;
	char_u *p = var_ಆರಂಭ;
	res = compile_expr0(&p, cctx);
	var_ಆರಂಭ[varlen] = c;
	cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len = ಸಾಲುಗಳು_len;
	ಆದರೆ (res == FAIL || p != var_ಆರಂಭ + varlen)
	{
	    // this should not happen
	    ಆದರೆ (res != FAIL)
		emsg(_(e_missing_closing_ವರ್ಗಫಲ_brace));
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	lhs->lhs_type = cctx->ctx_type_ರಾಶಿ.ga_len == 0 ? &t_void
						  : get_type_on_ರಾಶಿ(cctx, 0);

	ಆದರೆ (lhs->lhs_type->tt_type == VAR_CLASS
		|| (lhs->lhs_type->tt_type == VAR_OBJECT
		    && lhs->lhs_type != &t_object_any))
	{
	    // Check whether the class or object variable is modifiable
	    ಆದರೆ (!lhs_class_member_modifiable(lhs, var_ಆರಂಭ, cctx))
		ಹಿಂತಿರುಗಿ FAIL;
	}

	// Now we can properly check the type.  The variable is indexed, thus
	// we need the member type.  For a class or object we don't know the
	// type yet, it depends on what member is used.
	// The top item in the stack is the Dict, followed by the key and then
	// the type of the value.
	vartype_T vartype = lhs->lhs_type->tt_type;
	type_T *member_type = lhs->lhs_type->tt_member;
	ಆದರೆ (rhs_type != ಖಾಲಿ && member_type != ಖಾಲಿ
		&& vartype != VAR_OBJECT && vartype != VAR_CLASS
		&& rhs_type != &t_void
		&& need_type(rhs_type, member_type, 0,
					    -3, 0, cctx, FALSE, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಹಿಂತಿರುಗಿ OK;
    }

    ಹಿಂತಿರುಗಿ generate_loadvar(cctx, lhs);
}

/*
 * Produce code for loading "lhs" and also take care of an index.
 * Return OK/FAIL.
 */
    ಪೂರ್ಣಾಂಕ
compile_ತುಂಬು_lhs_with_ಸೂಚ್ಯಂಕ(lhs_T *lhs, char_u *var_ಆರಂಭ, cctx_T *cctx)
{
    ಆದರೆ (lhs->lhs_type->tt_type == VAR_OBJECT)
    {
	// "this.value": load "this" object and get the value at index for an
	// object or class member get the type of the member.
	// Also for "obj.value".
	char_u *dot = vim_strchr(var_ಆರಂಭ, '.');
	ಆದರೆ (dot == ಖಾಲಿ)
	{
	    semsg(_(e_missing_dot_after_object_str), lhs->lhs_name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	type_T	*type = oc_member_type(cl, TRUE, dot + 1,
					  lhs->lhs_ಕೊನೆ, &lhs->lhs_member_idx);
	ಆದರೆ (lhs->lhs_member_idx < 0)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (dot - var_ಆರಂಭ == 4 && STRNCMP(var_ಆರಂಭ, "this", 4) == 0)
	{
	    // load "this"
	    lvar_T  *lvar = lhs->lhs_lvar;
	    ಪೂರ್ಣಾಂಕ	    rc;

	    ಆದರೆ (lvar->lv_from_outer > 0)
		rc = generate_LOADOUTER(cctx, lvar->lv_idx,
			lvar->lv_from_outer, lvar->lv_loop_ಆಳ,
			lvar->lv_loop_idx, type);
	    ಇಲ್ಲದಿದ್ದರೆ
		rc = generate_ತುಂಬು(cctx, ISN_ತುಂಬು, lvar->lv_idx, ಖಾಲಿ, type);

	    ಆದರೆ (rc == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // load object variable or argument
	    ಆದರೆ (compile_ತುಂಬು_lhs(lhs, var_ಆರಂಭ, lhs->lhs_type, cctx) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (IS_INTERFACE(cl))
	    ಹಿಂತಿರುಗಿ generate_GET_ITF_MEMBER(cctx, cl, lhs->lhs_member_idx, type);
	ಹಿಂತಿರುಗಿ generate_GET_OBJ_MEMBER(cctx, lhs->lhs_member_idx, type);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lhs->lhs_type->tt_type == VAR_CLASS)
    {
	// "<classname>.value": load class variable "classname.value"
	char_u *dot = vim_strchr(var_ಆರಂಭ, '.');
	ಆದರೆ (dot == ಖಾಲಿ)
	{
	    (ಶೂನ್ಯ)check_type_is_ಮೌಲ್ಯ(lhs->lhs_type);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	class_T	*cl = lhs->lhs_type->tt_class;
	ocmember_T *m = class_member_lookup(cl, dot + 1,
						lhs->lhs_ಕೊನೆ - dot - 1,
						&lhs->lhs_member_idx);
	ಆದರೆ (m == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಹಿಂತಿರುಗಿ generate_CLASSMEMBER(cctx, TRUE, cl, lhs->lhs_member_idx);
    }

    ಆದರೆ (compile_ತುಂಬು_lhs(lhs, var_ಆರಂಭ, ಖಾಲಿ, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (lhs->lhs_has_ಸೂಚ್ಯಂಕ)
    {
	ಪೂರ್ಣಾಂಕ	range = FALSE;
	type_T	*type = get_type_on_ರಾಶಿ(cctx, 0);

	// A member of an object or class is not obtained by indexing it.
	ಆದರೆ (type->tt_type == VAR_CLASS
		|| (type->tt_type == VAR_OBJECT && type != &t_object_any))
	{
	    char_u *p = var_ಆರಂಭ + lhs->lhs_varlen;

	    ಹಿಂತಿರುಗಿ compile_class_object_ಸೂಚ್ಯಂಕ(cctx, &p, type);
	}

	// Get member from list or dict.  First compile the
	// index value.
	ಆದರೆ (compile_ನಿಯೋಜಿಸು_ಸೂಚ್ಯಂಕ(var_ಆರಂಭ, lhs, &range, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (range)
	{
	    semsg(_(e_cannot_use_range_with_assignment_operator_str),
								    var_ಆರಂಭ);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// Get the member.
	ಆದರೆ (compile_member(FALSE, ಖಾಲಿ, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Assignment to a list or dict member, or ":unlet" for the item, using the
 * information in "lhs".
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_unlet(
    char_u	*var_ಆರಂಭ,
    lhs_T	*lhs,
    ಪೂರ್ಣಾಂಕ		is_ನಿಯೋಜಿಸು,
    type_T	*rhs_type,
    cctx_T	*cctx)
{
    vartype_T	dest_type;
    ಪೂರ್ಣಾಂಕ		range = FALSE;

    ಆದರೆ (compile_ನಿಯೋಜಿಸು_ಸೂಚ್ಯಂಕ(var_ಆರಂಭ, lhs, &range, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (is_ನಿಯೋಜಿಸು && range
	    && lhs->lhs_type->tt_type != VAR_LIST
	    && lhs->lhs_type != &t_blob
	    && lhs->lhs_type != &t_any)
    {
	ಆದರೆ (lhs->lhs_type->tt_type == VAR_TUPLE)
	    emsg(_(e_cannot_ತುಂಡು_tuple));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_cannot_use_range_with_assignment_str), var_ಆರಂಭ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // For "expr[idx]" the index is compiled before the expression, thus the
    // resulting type cannot be used here.
    ಆದರೆ (lhs->lhs_dest == dest_ಅಭಿವ್ಯಕ್ತಿ
	    || lhs->lhs_type == ಖಾಲಿ || lhs->lhs_type == &t_any)
    {
	// Index on variable of unknown type: check at runtime.
	dest_type = VAR_ANY;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	dest_type = lhs->lhs_type->tt_type;
	ಆದರೆ (dest_type == VAR_DICT && range)
	{
	    emsg(_(e_cannot_use_range_with_dictionary));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (dest_type == VAR_DICT
		&& may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (dest_type == VAR_LIST || dest_type == VAR_BLOB)
	{
	    type_T *type;

	    ಆದರೆ (range)
	    {
		type = get_type_on_ರಾಶಿ(cctx, 1);
		ಆದರೆ (need_type(type, &t_ಸಂಖ್ಯೆ, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಆದರೆ ((dest_type != VAR_BLOB && type->tt_type != VAR_SPECIAL)
		    && need_type(type, &t_ಸಂಖ್ಯೆ, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	ಹಿಂತಿರುಗಿ OK;

    // Load the dict, list or object.  On the stack we then have:
    // - value (for assignment, not for :unlet)
    // - index
    // - for [a : b] second index
    // - variable
    ಆದರೆ (compile_ತುಂಬು_lhs(lhs, var_ಆರಂಭ, rhs_type, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (dest_type == VAR_LIST
	    || dest_type == VAR_DICT
	    || dest_type == VAR_BLOB
	    || dest_type == VAR_CLASS
	    || dest_type == VAR_OBJECT
	    || dest_type == VAR_ANY)
    {
	ಆದರೆ (is_ನಿಯೋಜಿಸು)
	{
	    ಆದರೆ (range)
	    {
		ಆದರೆ (generate_instr_drop(cctx, ISN_STORERANGE, 4) == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		isn_T	*isn = generate_instr_drop(cctx, ISN_STOREINDEX, 3);

		ಆದರೆ (isn == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
		isn->isn_arg.storeindex.si_vartype = dest_type;
		isn->isn_arg.storeindex.si_class = ಖಾಲಿ;

		ಆದರೆ (dest_type == VAR_OBJECT)
		{
		    class_T *cl = lhs->lhs_type->tt_class;

		    ಆದರೆ (IS_INTERFACE(cl))
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
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (range)
	{
	    ಆದರೆ (generate_instr_drop(cctx, ISN_UNLETRANGE, 3) == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (generate_instr_drop(cctx, ISN_UNLETINDEX, 2) == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (dest_type == VAR_TUPLE)
	    emsg(_(e_tuple_is_immutable));
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg(_(e_indexable_type_required));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an instruction to push the default value for "vartype".
 * if "dest_local" is TRUE then for some types no instruction is generated.
 * "skip_store" is set to TRUE if no PUSH instruction is generated.
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ತಳ್ಳು_default_ಮೌಲ್ಯ(
	cctx_T	    *cctx,
	vartype_T   vartype,
	ಪೂರ್ಣಾಂಕ	    dest_is_local,
	ಪೂರ್ಣಾಂಕ	    *skip_store)
{
    ಪೂರ್ಣಾಂಕ r = OK;

    ಆಯ್ಕೆ (vartype)
    {
	ಸ್ಥಿತಿ VAR_BOOL:
	    r = generate_PUSHBOOL(cctx, VVAL_FALSE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    r = generate_PUSHF(cctx, 0.0);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_STRING:
	    r = generate_PUSHS(cctx, ಖಾಲಿ);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:
	    r = generate_PUSHBLOB(cctx, blob_alloc());
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FUNC:
	    r = generate_PUSHFUNC(cctx, ಖಾಲಿ, &t_func_void, TRUE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    r = generate_NEWLIST(cctx, 0, FALSE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    r = generate_NEWTUPLE(cctx, 0, FALSE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    r = generate_NEWDICT(cctx, 0, FALSE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	    r = generate_PUSHJOB(cctx);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CHANNEL:
	    r = generate_PUSHCHANNEL(cctx);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    r = generate_PUSHOBJ(cctx);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	ಸ್ಥಿತಿ VAR_SPECIAL:  // cannot happen
	    // This is skipped for local variables, they are always
	    // initialized to zero.  But in a "for" or "while" loop
	    // the value may have been changed.
	    ಆದರೆ (dest_is_local && !inside_loop_scope(cctx))
		*skip_store = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		r = generate_PUSHNR(cctx, 0);
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ r;
}

/*
 * Compile assignment context.  Used when compiling an assignment statement.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ cac_S cac_T;
ರಚನೆ cac_S
{
    cmdidx_T	cac_cmdidx;		// assignment command
    char_u	*cac_nextc;		// next character to parse
    lhs_T	cac_lhs;		// lhs of the assignment
    type_T	*cac_rhs_type;		// rhs type of an assignment
    char_u	*cac_op;		// assignment operator
    ಪೂರ್ಣಾಂಕ		cac_oplen;		// assignment operator length
    char_u	*cac_var_ಆರಂಭ;		// start of the variable names
    char_u	*cac_var_ಕೊನೆ;		// end of the variable names
    ಪೂರ್ಣಾಂಕ		cac_var_ಎಣಿಕೆ;		// number of variables in assignment
    ಪೂರ್ಣಾಂಕ		cac_var_idx;		// variable index in a list
    ಪೂರ್ಣಾಂಕ		cac_semicolon;		// semicolon in [var1, var2; var3]
    garray_T	*cac_instr;
    ಪೂರ್ಣಾಂಕ		cac_instr_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		cac_incdec;
    ಪೂರ್ಣಾಂಕ		cac_did_generate_ತುಂಡು;
    ಪೂರ್ಣಾಂಕ		cac_is_decl;
    ಪೂರ್ಣಾಂಕ		cac_is_const;
    ಪೂರ್ಣಾಂಕ		cac_ಆರಂಭ_lnum;
    type_T	*cac_inferred_type;
    ಪೂರ್ಣಾಂಕ		cac_skip_store;
};

/*
 * Initialize the compile assignment context.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compile_ನಿಯೋಜಿಸು_context_init(cac_T *cac, cctx_T *cctx, ಪೂರ್ಣಾಂಕ cmdidx, char_u *arg)
{
    CLEAR_ಕ್ಷೇತ್ರ(*cac);
    cac->cac_cmdidx = cmdidx;
    cac->cac_instr = &cctx->ctx_instr;
    cac->cac_rhs_type = &t_any;
    cac->cac_is_decl = is_decl_command(cmdidx);
    cac->cac_ಆರಂಭ_lnum = SOURCING_LNUM;
    cac->cac_instr_ಎಣಿಕೆ = -1;
    cac->cac_var_ಕೊನೆ = arg;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_obj_new_arg(char_u **argp, cctx_T *cctx)
{
    char_u *arg = *argp;

    arg += 11;	    // skip "ifargisset"
    ಪೂರ್ಣಾಂಕ def_arg_idx = getdigits(&arg);
    arg = skipwhite(arg);

    // Use a JUMP_IF_ARG_NOT_SET instruction to skip if the value was not
    // given and the default value is "v:none".
    ಪೂರ್ಣಾಂಕ ರಾಶಿ_ಸ್ಥಾನಾಂತರ = ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ +
				(cctx->ctx_ufunc->uf_va_name != ಖಾಲಿ ? 1 : 0);
    ಪೂರ್ಣಾಂಕ def_arg_ಎಣಿಕೆ = cctx->ctx_ufunc->uf_def_ವಾದಗಳು.ga_len;
    ಪೂರ್ಣಾಂಕ arg_ಸ್ಥಾನಾಂತರ = def_arg_idx - def_arg_ಎಣಿಕೆ - ರಾಶಿ_ಸ್ಥಾನಾಂತರ;

    ಆದರೆ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_NOT_ಗಣ,
			     arg_ಸ್ಥಾನಾಂತರ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    *argp = arg;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Translate the increment (++) and decrement (--) operators to the
 * corresponding compound operators (+= or -=).
 *
 * Returns OK on success and FAIL on syntax error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
translate_incdec_op(exarg_T *eap, cac_T *cac)
{
    ಆದರೆ (VIM_ISWHITE(eap->cmd[2]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str),
		eap->cmdidx == CMD_increment ? "++" : "--", eap->cmd);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    cac->cac_op = (char_u *)(eap->cmdidx == CMD_increment ? "+=" : "-=");
    cac->cac_oplen = 2;
    cac->cac_incdec = TRUE;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Process the operator in an assignment statement.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_process_operator(
    exarg_T	*eap,
    char_u	*arg,
    cac_T	*cac,
    ಪೂರ್ಣಾಂಕ		*heredoc,
    char_u	**retstr)
{
    *retstr = ಖಾಲಿ;

    ಆದರೆ (eap->cmdidx == CMD_increment || eap->cmdidx == CMD_decrement)
	// Change an unary operator to a compound operator
	ಹಿಂತಿರುಗಿ translate_incdec_op(eap, cac);

    char_u *sp = cac->cac_nextc;
    cac->cac_nextc = skipwhite(cac->cac_nextc);
    cac->cac_op = cac->cac_nextc;
    cac->cac_oplen = assignment_len(cac->cac_nextc, heredoc);

    ಆದರೆ (cac->cac_var_ಎಣಿಕೆ > 0 && cac->cac_oplen == 0)
    {
	// can be something like "[1, 2]->func()"
	*retstr = arg;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // need white space before and after the operator
    ಆದರೆ (cac->cac_oplen > 0 && (!VIM_ISWHITE(*sp)
		|| !IS_WHITE_OR_NUL(cac->cac_op[cac->cac_oplen])))
    {
	error_white_both(cac->cac_op, cac->cac_oplen);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Find the start of an assignment statement.
 */
    ಸ್ಥಿರ char_u *
compile_ನಿಯೋಜಿಸು_compute_ಆರಂಭ(char_u *arg, ಪೂರ್ಣಾಂಕ var_ಎಣಿಕೆ)
{
    ಆದರೆ (var_ಎಣಿಕೆ > 0)
	// [var1, var2] = [val1, val2]
	// skip over the "["
	ಹಿಂತಿರುಗಿ skipwhite(arg + 1);

    ಹಿಂತಿರುಗಿ arg;
}

/*
 * Parse a heredoc assignment starting at "p".  Returns a pointer to the
 * beginning of the heredoc content.
 */
    ಸ್ಥಿರ char_u *
parse_heredoc_assignment(exarg_T *eap, cctx_T *cctx, cac_T *cac)
{
    // [let] varname =<< [trim] {end}
    eap->ea_getline = exarg_getline;
    eap->cookie = cctx;

    list_T *l = heredoc_get(eap, cac->cac_nextc + 3, FALSE, TRUE);
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    list_free(l);
    cac->cac_nextc += STRLEN(cac->cac_nextc);

    ಹಿಂತಿರುಗಿ cac->cac_nextc;
}

/*
 * Check the type of a RHS expression in a list assignment statement.
 * The RHS expression is already compiled.  So the type is on the stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_list_check_rhs_type(cctx_T *cctx, cac_T *cac)
{
    type_T	*stacktype;

    stacktype = cctx->ctx_type_ರಾಶಿ.ga_len == 0 ? &t_void
						: get_type_on_ರಾಶಿ(cctx, 0);
    ಆದರೆ (stacktype->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (stacktype->tt_type != VAR_LIST && stacktype->tt_type != VAR_TUPLE
					&& stacktype->tt_type != VAR_ANY)
    {
	emsg(_(e_list_or_tuple_required));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (need_type(stacktype,
		  stacktype->tt_type == VAR_TUPLE ? &t_tuple_any : &t_list_any,
		  TYPECHK_TUPLE_OK, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // The check accepts both a list and a tuple. Keep "any", making it a list
    // would reject a tuple later on.
    ಆದರೆ (stacktype->tt_type == VAR_ANY)
	ಗಣ_type_on_ರಾಶಿ(cctx, &t_any, 0);

    ಆದರೆ (stacktype->tt_type == VAR_TUPLE)
    {
	ಆದರೆ (stacktype->tt_argcount != 1)
	    cac->cac_rhs_type = &t_any;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (stacktype->tt_flags & TTFLAG_VARARGS)
		cac->cac_rhs_type = stacktype->tt_ವಾದಗಳು[0]->tt_member;
	    ಇಲ್ಲದಿದ್ದರೆ
		cac->cac_rhs_type = stacktype->tt_ವಾದಗಳು[0];
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (stacktype->tt_member != ಖಾಲಿ)
	cac->cac_rhs_type = stacktype->tt_member;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * In a list assignment statement, if a constant list was used, check the
 * length.  Returns OK if the length check succeeds.  Returns FAIL otherwise.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_list_check_ಉದ್ದಳತೆ(cctx_T *cctx, cac_T *cac)
{
    ಪೂರ್ಣಾಂಕ	needed_list_len;
    ಪೂರ್ಣಾಂಕ	did_check = FALSE;

    needed_list_len = cac->cac_semicolon
				? cac->cac_var_ಎಣಿಕೆ - 1
				: cac->cac_var_ಎಣಿಕೆ;
    ಆದರೆ (cac->cac_instr->ga_len > 0)
    {
	isn_T	*isn = ((isn_T *)cac->cac_instr->ga_ದತ್ತಾಂಶ) +
						cac->cac_instr->ga_len - 1;

	ಆದರೆ (isn->isn_type == ISN_NEWLIST || isn->isn_type == ISN_NEWTUPLE)
	{
	    did_check = TRUE;
	    ಆದರೆ (cac->cac_semicolon ?
			isn->isn_arg.ಸಂಖ್ಯೆ < needed_list_len
			: isn->isn_arg.ಸಂಖ್ಯೆ != needed_list_len)
	    {
		semsg(_(e_expected_nr_ವಸ್ತುಗಳು_but_got_nr),
			needed_list_len, (ಪೂರ್ಣಾಂಕ)isn->isn_arg.ಸಂಖ್ಯೆ);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
    }

    ಆದರೆ (!did_check)
	generate_CHECKLEN(cctx, needed_list_len, cac->cac_semicolon);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Evaluate the expression for "[var, var] = expr" assignment.
 * A line break may follow the assignment operator "=".
 */
    ಸ್ಥಿರ char_u *
compile_ನಿಯೋಜಿಸು_list_ಅಭಿವ್ಯಕ್ತಿ(cctx_T *cctx, cac_T *cac)
{
    char_u *whitep;

    whitep = cac->cac_op + cac->cac_oplen;

    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(whitep, &cac->cac_nextc, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // compile RHS expression
    ಆದರೆ (compile_expr0(&cac->cac_nextc, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	// no need to parse more when skipping
	ಹಿಂತಿರುಗಿ cac->cac_nextc;

    ಆದರೆ (compile_ನಿಯೋಜಿಸು_list_check_rhs_type(cctx, cac) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // If a constant list was used we can check the length right here.
    ಆದರೆ (compile_ನಿಯೋಜಿಸು_list_check_ಉದ್ದಳತೆ(cctx, cac) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ cac->cac_nextc;
}

/*
 * Find and return the end of a heredoc or a list of variables assignment
 * statement.  For a single variable assignment statement, returns the current
 * end.
 * Returns NULL on failure.
 */
    ಸ್ಥಿರ char_u *
compile_ನಿಯೋಜಿಸು_compute_ಕೊನೆ(
    exarg_T	*eap,
    cctx_T	*cctx,
    cac_T	*cac,
    ಪೂರ್ಣಾಂಕ		heredoc)
{
    ಆದರೆ (heredoc)
    {
	cac->cac_nextc = parse_heredoc_assignment(eap, cctx, cac);
	ಹಿಂತಿರುಗಿ cac->cac_nextc;
    }

    ಆದರೆ (cac->cac_var_ಎಣಿಕೆ > 0)
    {
	// for "[var, var] = expr" evaluate the expression. The list of
	// variables are processed later.
	// A line break may follow the "=".
	cac->cac_nextc = compile_ನಿಯೋಜಿಸು_list_ಅಭಿವ್ಯಕ್ತಿ(cctx, cac);
	ಹಿಂತಿರುಗಿ cac->cac_nextc;
    }

    ಹಿಂತಿರುಗಿ cac->cac_var_ಕೊನೆ;
}

/*
 * For "var = expr" evaluate the expression.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_single_eval_ಅಭಿವ್ಯಕ್ತಿ(cctx_T *cctx, cac_T *cac)
{
    ಪೂರ್ಣಾಂಕ		ret = OK;
    char_u	*whitep;
    lhs_T	*lhs = &cac->cac_lhs;
    lvar_T	*lvp;
    lvar_T	save_lhs_lvar;

    // Compile the expression.
    ಆದರೆ (cac->cac_incdec)
	ಹಿಂತಿರುಗಿ generate_PUSHNR(cctx, 1);

    // Temporarily hide the new local variable here, it is
    // not available to this expression.
    ಆದರೆ (lhs->lhs_new_local)
    {
	--cctx->ctx_locals.ga_len;

	// Save the local variable value (compiling the RHS expression may
	// create new local variables).
	lvp = ((lvar_T *)cctx->ctx_locals.ga_ದತ್ತಾಂಶ) + cctx->ctx_locals.ga_len;
	save_lhs_lvar = *lvp;
    }
    whitep = cac->cac_op + cac->cac_oplen;

    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(whitep, &cac->cac_nextc, cctx) == FAIL)
    {
	ಆದರೆ (lhs->lhs_new_local)
	    ++cctx->ctx_locals.ga_len;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ret = compile_expr0_ext(&cac->cac_nextc, cctx, &cac->cac_is_const);
    ಆದರೆ (lhs->lhs_new_local)
    {
	// Restore the local variable value.  Update lhs_lvar as the index of
	// the local variable might have changed.
	lvp = ((lvar_T *)cctx->ctx_locals.ga_ದತ್ತಾಂಶ) + cctx->ctx_locals.ga_len;
	*lvp = save_lhs_lvar;
	lhs->lhs_lvar = lvp;

	++cctx->ctx_locals.ga_len;
    }

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * When compiling an assignment, set the LHS type to the RHS type.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_ಗಣ_lhs_type_from_rhs(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    ಆದರೆ (rhs_type->tt_type == VAR_VOID)
    {
	emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    type_T *type;

    // An empty list or dict has a &t_unknown member, for a variable that
    // implies &t_any.
    ಆದರೆ (rhs_type == &t_list_ಖಾಲಿಗಳು)
	type = &t_list_any;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rhs_type == &t_dict_ಖಾಲಿಗಳು)
	type = &t_dict_any;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (rhs_type == &t_unknown)
	type = &t_any;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	type = rhs_type;
	cac->cac_inferred_type = rhs_type;
    }

    ಗಣ_var_type(lhs->lhs_lvar, type, cctx);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Returns TRUE if the "rhs_type" can be assigned to the "lhs" variable.
 * Used when compiling an assignment statement.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_valid_rhs_type(
    cctx_T	*cctx,
    cac_T	*cac,
    lhs_T	*lhs,
    type_T	*rhs_type)
{
    type_T	*use_type = lhs->lhs_lvar->lv_type;
    where_T	where = WHERE_INIT;

    // Without operator check type here, otherwise below.
    // Use the line number of the assignment.
    SOURCING_LNUM = cac->cac_ಆರಂಭ_lnum;
    ಆದರೆ (cac->cac_var_ಎಣಿಕೆ > 0)
    {
	where.wt_ಸೂಚ್ಯಂಕ = cac->cac_var_idx + 1;
	where.wt_ಬಗೆ = WT_VARIABLE;
    }

    // If assigning to a list or dict member, use the member type.
    // Not for "list[:] =".
    ಆದರೆ (lhs->lhs_has_ಸೂಚ್ಯಂಕ &&
	    !has_list_ಸೂಚ್ಯಂಕ(cac->cac_var_ಆರಂಭ + lhs->lhs_varlen, cctx))
	use_type = lhs->lhs_member_type;

    ಆದರೆ (need_type_where(rhs_type, use_type, 0, -1, where, cctx, FALSE,
						cac->cac_is_const) == FAIL)
	ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Compare the LHS type with the RHS type in an assignment.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_check_type(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    type_T	*rhs_type;

    rhs_type = cctx->ctx_type_ರಾಶಿ.ga_len == 0
					? &t_void
					: get_type_on_ರಾಶಿ(cctx, 0);
    cac->cac_rhs_type = rhs_type;

    ಆದರೆ (check_type_is_ಮೌಲ್ಯ(rhs_type) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (lhs->lhs_lvar != ಖಾಲಿ && (cac->cac_is_decl || !lhs->lhs_has_type))
    {
	ಆದರೆ (rhs_type->tt_type == VAR_FUNC
					|| rhs_type->tt_type == VAR_PARTIAL)
	{
	    // Make sure the variable name can be used as a funcref
	    ಆದರೆ (!lhs->lhs_has_ಸೂಚ್ಯಂಕ
				&& var_wrong_func_name(lhs->lhs_name, TRUE))
		ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (lhs->lhs_new_local && !lhs->lhs_has_type)
	{
	    // The LHS variable doesn't have a type.  Set it to the RHS type.
	    ಆದರೆ (compile_ನಿಯೋಜಿಸು_ಗಣ_lhs_type_from_rhs(cctx, cac, lhs, rhs_type)
								== FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cac->cac_op == '=')
	{
	    ಆದರೆ (!compile_ನಿಯೋಜಿಸು_valid_rhs_type(cctx, cac, lhs, rhs_type))
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Assigning to a register using @r = "abc"

	type_T *lhs_type = lhs->lhs_member_type;

	// Special case: assigning to @# can use a number or a string.
	// Also: can assign a number to a float.
	ಆದರೆ ((lhs_type == &t_ಸಂಖ್ಯೆ_or_string || lhs_type == &t_float)
					&& rhs_type->tt_type == VAR_ಸಂಖ್ಯೆ)
	    lhs_type = &t_ಸಂಖ್ಯೆ;

	ಆದರೆ (*cac->cac_nextc != '=')
	{
	    ಆದರೆ (need_type(rhs_type, lhs_type, 0, -1, 0, cctx, FALSE,
							FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile the RHS expression in an assignment statement and generate the
 * instructions.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_rhs_ಅಭಿವ್ಯಕ್ತಿ(cctx_T *cctx, cac_T *cac)
{
    cac->cac_is_const = FALSE;

    // for "+=", "*=", "..=" etc. first load the current value
    ಆದರೆ (*cac->cac_op != '='
	    && compile_ತುಂಬು_lhs_with_ಸೂಚ್ಯಂಕ(&cac->cac_lhs, cac->cac_var_ಆರಂಭ,
								cctx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // For "var = expr" evaluate the expression.
    ಆದರೆ (cac->cac_var_ಎಣಿಕೆ == 0)
    {
	ಪೂರ್ಣಾಂಕ	ret;

	// Compile the expression.
	cac->cac_instr_ಎಣಿಕೆ = cac->cac_instr->ga_len;
	ret = compile_ನಿಯೋಜಿಸು_single_eval_ಅಭಿವ್ಯಕ್ತಿ(cctx, cac);
	ಆದರೆ (ret == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cac->cac_semicolon && cac->cac_var_idx == cac->cac_var_ಎಣಿಕೆ - 1)
    {
	// For "[var; var] = expr" get the rest of the list
	cac->cac_did_generate_ತುಂಡು = TRUE;
	ಆದರೆ (generate_ತುಂಡು(cctx, cac->cac_var_ಎಣಿಕೆ - 1) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// For "[var, var] = expr" get the "var_idx" item from the
	// list.
	ಪೂರ್ಣಾಂಕ with_op = *cac->cac_op != '=';
	ಆದರೆ (generate_GETITEM(cctx, cac->cac_var_idx, with_op) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (compile_ನಿಯೋಜಿಸು_check_type(cctx, cac) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile the RHS expression in an assignment
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_rhs(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;

    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
    {
	ಆದರೆ (cac->cac_oplen > 0 && cac->cac_var_ಎಣಿಕೆ == 0)
	{
	    // skip over the "=" and the expression
	    cac->cac_nextc = skipwhite(cac->cac_op + cac->cac_oplen);
	    (ಶೂನ್ಯ)compile_expr0(&cac->cac_nextc, cctx);
	}
	ಹಿಂತಿರುಗಿ OK;
    }

    // If RHS is specified, then generate instructions for RHS expression
    ಆದರೆ (cac->cac_oplen > 0)
	ಹಿಂತಿರುಗಿ compile_ನಿಯೋಜಿಸು_rhs_ಅಭಿವ್ಯಕ್ತಿ(cctx, cac);

    ಆದರೆ (cac->cac_cmdidx == CMD_final)
    {
	emsg(_(e_final_requires_a_ಮೌಲ್ಯ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (cac->cac_cmdidx == CMD_const)
    {
	emsg(_(e_const_requires_a_ಮೌಲ್ಯ));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (!lhs->lhs_has_type || lhs->lhs_dest == dest_option
					|| lhs->lhs_dest == dest_func_option)
    {
	emsg(_(e_type_or_initialization_required));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // variables are always initialized
    ಆದರೆ (GA_GROW_FAILS(cac->cac_instr, 1))
	ಹಿಂತಿರುಗಿ FAIL;

    cac->cac_instr_ಎಣಿಕೆ = cac->cac_instr->ga_len;

    ಹಿಂತಿರುಗಿ ತಳ್ಳು_default_ಮೌಲ್ಯ(cctx, lhs->lhs_member_type->tt_type,
			      lhs->lhs_dest == dest_local,
			      &cac->cac_skip_store);
}

/*
 * Returns OK if "type" supports compound operator "op_arg" (e.g. +=, -=, %=,
 * etc.).  Compound operators are not supported with a tuple and a dict.
 * Returns FAIL if compound operator is not supported.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_type_supports_compound_op(type_T *type, char_u op_arg)
{
    ಆದರೆ (type->tt_type == VAR_TUPLE || type->tt_type == VAR_DICT)
    {
	char_u	op[2];

	op[0] = op_arg;
	op[1] = NUL;
	semsg(_(e_wrong_variable_type_for_str_equal), op);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile a compound op assignment statement (+=, -=, *=, %=, etc.)
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_compound_op(cctx_T *cctx, cac_T *cac)
{
    lhs_T	    *lhs = &cac->cac_lhs;
    type_T	    *expected;
    type_T	    *stacktype = ಖಾಲಿ;

    ಆದರೆ (cac->cac_lhs.lhs_type->tt_type == VAR_TUPLE
	    && check_type_supports_compound_op(cac->cac_lhs.lhs_type,
							*cac->cac_op) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (*cac->cac_op == '.')
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ರಾಶಿ(cctx, 0);
	ಆದರೆ (expected != &t_string
		&& need_type(stacktype, expected, 0, -1, 0, cctx,
					FALSE, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	expected = lhs->lhs_member_type;
	stacktype = get_type_on_ರಾಶಿ(cctx, 0);

	ಆದರೆ (check_type_supports_compound_op(expected, *cac->cac_op) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (
		// If variable is float operation with number is OK.
		!(expected == &t_float && (stacktype == &t_ಸಂಖ್ಯೆ
					|| stacktype == &t_ಸಂಖ್ಯೆ_bool))
		&& need_type(stacktype, expected, TYPECHK_ಸಂಖ್ಯೆ_OK, -1, 0,
						cctx, FALSE, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (*cac->cac_op == '.')
    {
	ಆದರೆ (generate_CONCAT(cctx, 2) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*cac->cac_op == '+')
    {
	ಆದರೆ (generate_ಕೂಡಿಸು_instr(cctx,
		    operator_type(lhs->lhs_member_type, stacktype),
		    lhs->lhs_member_type, stacktype,
		    ಅಭಿವ್ಯಕ್ತಿ_APPEND) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (generate_two_op(cctx, cac->cac_op) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate the STORE and SETTYPE instructions for an assignment statement.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_generate_store(cctx_T *cctx, cac_T *cac)
{
    lhs_T	*lhs = &cac->cac_lhs;
    ಪೂರ್ಣಾಂಕ		save_lnum;

    // Use the line number of the assignment for store instruction.
    save_lnum = cctx->ctx_lnum;
    cctx->ctx_lnum = cac->cac_ಆರಂಭ_lnum - 1;

    ಆದರೆ (lhs->lhs_has_ಸೂಚ್ಯಂಕ)
    {
	// Use the info in "lhs" to store the value at the index in the
	// list, dict or object.
	ಆದರೆ (compile_ನಿಯೋಜಿಸು_unlet(cac->cac_var_ಆರಂಭ, &cac->cac_lhs,
				 TRUE, cac->cac_rhs_type, cctx) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (cac->cac_is_decl && cac->cac_cmdidx == CMD_const &&
			(lhs->lhs_dest == dest_script
			 || lhs->lhs_dest == dest_script_v9
			 || lhs->lhs_dest == dest_global
			 || lhs->lhs_dest == dest_local))
	    // ":const var": lock the value, but not referenced variables
	    generate_LOCKCONST(cctx);

	type_T	*inferred_type = cac->cac_inferred_type;

	ಆದರೆ ((lhs->lhs_type->tt_type == VAR_DICT
		    || lhs->lhs_type->tt_type == VAR_LIST)
		&& lhs->lhs_type->tt_member != ಖಾಲಿ
		&& lhs->lhs_type->tt_member != &t_any
		&& lhs->lhs_type->tt_member != &t_unknown)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, lhs->lhs_type);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lhs->lhs_type->tt_type == VAR_TUPLE
					&& lhs->lhs_type->tt_argcount != 0)
	    generate_SETTYPE(cctx, lhs->lhs_type);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (inferred_type != ಖಾಲಿ
		&& (inferred_type->tt_type == VAR_DICT
		    || inferred_type->tt_type == VAR_LIST)
		&& inferred_type->tt_member != ಖಾಲಿ
		&& inferred_type->tt_member != &t_unknown
		&& inferred_type->tt_member != &t_any)
	    // Set the type in the list or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, inferred_type);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (inferred_type != ಖಾಲಿ
				&& inferred_type->tt_type == VAR_TUPLE
				&& inferred_type->tt_argcount > 0)
	    generate_SETTYPE(cctx, inferred_type);

	ಆದರೆ (!cac->cac_skip_store &&
		generate_store_lhs(cctx, &cac->cac_lhs,
				   cac->cac_instr_ಎಣಿಕೆ,
				   cac->cac_is_decl) == FAIL)
	{
	    cctx->ctx_lnum = save_lnum;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    cctx->ctx_lnum = save_lnum;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Process the variable(s) in an assignment statement
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_ನಿಯೋಜಿಸು_process_variables(
    cctx_T	*cctx,
    cac_T	*cac,
    ಪೂರ್ಣಾಂಕ		cmdidx,
    ಪೂರ್ಣಾಂಕ		heredoc,
    ಪೂರ್ಣಾಂಕ		has_cmd,
    ಪೂರ್ಣಾಂಕ		has_argisset_ಪೂರ್ವಪ್ರತ್ಯಯ,
    ಪೂರ್ಣಾಂಕ		jump_instr_idx)
{
    /*
     * Loop over variables in "[var, var] = expr".
     * For "name = expr" and "var name: type" this is done only once.
     */
    ಗಾಗಿ (cac->cac_var_idx = 0; cac->cac_var_idx == 0 ||
	    cac->cac_var_idx < cac->cac_var_ಎಣಿಕೆ; cac->cac_var_idx++)
    {
	ಆದರೆ (cac->cac_var_ಆರಂಭ[0] == '_'
				&& !eval_isnamec(cac->cac_var_ಆರಂಭ[1]))
	{
	    // Ignore underscore in "[a, _, b] = list".
	    ಆದರೆ (cac->cac_var_ಎಣಿಕೆ > 0)
	    {
		cac->cac_var_ಆರಂಭ = skipwhite(cac->cac_var_ಆರಂಭ + 2);
		ಮುಂದುವರಿ;
	    }
	    emsg(_(e_cannot_use_underscore_ಇಲ್ಲಿ));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	vim_free(cac->cac_lhs.lhs_name);

	/*
	 * Figure out the LHS type and other properties.
	 */
	ಆದರೆ (compile_ನಿಯೋಜಿಸು_lhs(cac->cac_var_ಆರಂಭ, &cac->cac_lhs, cmdidx,
		    cac->cac_is_decl, heredoc, has_cmd,
		    cac->cac_oplen, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// Compile the RHS expression
	ಆದರೆ (heredoc)
	{
	    SOURCING_LNUM = cac->cac_ಆರಂಭ_lnum;
	    ಆದರೆ (cac->cac_lhs.lhs_has_type
		    && need_type(&t_list_string, cac->cac_lhs.lhs_type,
				0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (compile_ನಿಯೋಜಿಸು_rhs(cctx, cac) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (cac->cac_var_ಎಣಿಕೆ == 0)
		cac->cac_var_ಕೊನೆ = cac->cac_nextc;
	}

	// no need to parse more when skipping
	ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (cac->cac_oplen > 0 && *cac->cac_op != '=')
	{
	    ಆದರೆ (compile_ನಿಯೋಜಿಸು_compound_op(cctx, cac) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	// generate the store instructions
	ಆದರೆ (compile_ನಿಯೋಜಿಸು_generate_store(cctx, cac) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (cac->cac_var_idx + 1 < cac->cac_var_ಎಣಿಕೆ)
	    cac->cac_var_ಆರಂಭ = skipwhite(cac->cac_lhs.lhs_ಕೊನೆ + 1);

	ಆದರೆ (has_argisset_ಪೂರ್ವಪ್ರತ್ಯಯ)
	{
	    // set instruction index in JUMP_IF_ARG_SET to here
	    isn_T *isn = ((isn_T *)cac->cac_instr->ga_ದತ್ತಾಂಶ) + jump_instr_idx;
	    isn->isn_arg.jumparg.jump_where = cac->cac_instr->ga_len;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ char_u *
compile_assignment(
    char_u	*arg_ಆರಂಭ,
    exarg_T	*eap,
    cmdidx_T	cmdidx,
    cctx_T	*cctx)
{
    cac_T	cac;
    char_u	*arg = arg_ಆರಂಭ;
    char_u	*retstr = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		heredoc = FALSE;
    ಪೂರ್ಣಾಂಕ		jump_instr_idx;

    compile_ನಿಯೋಜಿಸು_context_init(&cac, cctx, cmdidx, arg);

    jump_instr_idx = cac.cac_instr->ga_len;

    // process object variable initialization in a new() constructor method
    ಪೂರ್ಣಾಂಕ	has_argisset_ಪೂರ್ವಪ್ರತ್ಯಯ = STRNCMP(arg, "ifargisset ", 11) == 0;
    ಆದರೆ (has_argisset_ಪೂರ್ವಪ್ರತ್ಯಯ &&
			compile_ನಿಯೋಜಿಸು_obj_new_arg(&arg, cctx) == FAIL)
	ಹೋಗು theend;

    // Skip over the "varname" or "[varname, varname]" to get to any "=".
    cac.cac_nextc = skip_var_list(arg, TRUE, &cac.cac_var_ಎಣಿಕೆ,
						&cac.cac_semicolon, TRUE);
    ಆದರೆ (cac.cac_nextc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ *arg == '[' ? arg : ಖಾಲಿ;

    ಆದರೆ (compile_ನಿಯೋಜಿಸು_process_operator(eap, arg, &cac, &heredoc,
							&retstr) == FAIL)
	ಹಿಂತಿರುಗಿ retstr;

    // Compute the start of the assignment
    cac.cac_var_ಆರಂಭ = compile_ನಿಯೋಜಿಸು_compute_ಆರಂಭ(arg, cac.cac_var_ಎಣಿಕೆ);

    // Compute the end of the assignment
    cac.cac_var_ಕೊನೆ = compile_ನಿಯೋಜಿಸು_compute_ಕೊನೆ(eap, cctx, &cac, heredoc);
    ಆದರೆ (cac.cac_var_ಕೊನೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಪೂರ್ಣಾಂಕ has_cmd = cac.cac_var_ಆರಂಭ > eap->cmd;

    /* process the variable(s) */
    ಆದರೆ (compile_ನಿಯೋಜಿಸು_process_variables(cctx, &cac, cmdidx, heredoc,
					 has_cmd, has_argisset_ಪೂರ್ವಪ್ರತ್ಯಯ,
					 jump_instr_idx) == FAIL)
	ಹೋಗು theend;

    // For "[var, var] = expr" drop the "expr" value.
    // Also for "[var, var; _] = expr".
    ಆದರೆ (cctx->ctx_skip != SKIP_YES && cac.cac_var_ಎಣಿಕೆ > 0 &&
	    (!cac.cac_semicolon || !cac.cac_did_generate_ತುಂಡು))
    {
	ಆದರೆ (generate_instr_drop(cctx, ISN_DROP, 1) == ಖಾಲಿ)
	    ಹೋಗು theend;
    }

    retstr = skipwhite(cac.cac_var_ಕೊನೆ);

theend:
    vim_free(cac.cac_lhs.lhs_name);
    ಹಿಂತಿರುಗಿ retstr;
}

/*
 * Check for an assignment at "eap->cmd", compile it if found.
 * Return NOTDONE if there is none, FAIL for failure, OK if done.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
may_compile_assignment(exarg_T *eap, char_u **ಸಾಲು, cctx_T *cctx)
{
    char_u  *pskip;
    char_u  *p;

    // Assuming the command starts with a variable or function name,
    // find what follows.
    // Skip over "var.member", "var[idx]" and the like.
    // Also "&opt = val", "$ENV = val" and "@r = val".
    pskip = (*eap->cmd == '&' || *eap->cmd == '$' || *eap->cmd == '@')
						 ? eap->cmd + 1 : eap->cmd;
    p = ಗಮ್ಯ_name_ಕೊನೆ(pskip, TRUE);
    ಆದರೆ (p > eap->cmd && *p != NUL)
    {
	char_u *var_ಕೊನೆ;
	ಪೂರ್ಣಾಂಕ	oplen;
	ಪೂರ್ಣಾಂಕ	heredoc;

	ಆದರೆ (eap->cmd[0] == '@')
	    var_ಕೊನೆ = eap->cmd + 2;
	ಇಲ್ಲದಿದ್ದರೆ
	    var_ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(pskip, ಖಾಲಿ, ಖಾಲಿ,
					FNE_CHECK_ಆರಂಭ | FNE_INCL_BR);
	oplen = assignment_len(skipwhite(var_ಕೊನೆ), &heredoc);
	ಆದರೆ (oplen > 0)
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
	    ಆದರೆ (*eap->cmd == '&'
		    || *eap->cmd == '$'
		    || *eap->cmd == '@'
		    || ((len) > 2 && eap->cmd[1] == ':')
		    || STRNCMP(eap->cmd, "g:[", 3) == 0
		    || variable_exists(eap->cmd, len, cctx))
	    {
		*ಸಾಲು = compile_assignment(eap->cmd, eap, CMD_ಗಾತ್ರ, cctx);
		ಆದರೆ (*ಸಾಲು == ಖಾಲಿ || *ಸಾಲು == eap->cmd)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಹಿಂತಿರುಗಿ OK;
	    }
	}
    }

    // might be "[var, var] = expr" or "ifargisset this.member = expr"
    ಆದರೆ (*eap->cmd == '[' || STRNCMP(eap->cmd, "ifargisset ", 11) == 0)
    {
	*ಸಾಲು = compile_assignment(eap->cmd, eap, CMD_ಗಾತ್ರ, cctx);
	ಆದರೆ (*ಸಾಲು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*ಸಾಲು != eap->cmd)
	    ಹಿಂತಿರುಗಿ OK;
    }
    ಹಿಂತಿರುಗಿ NOTDONE;
}

/*
 * Check if arguments of "ufunc" shadow variables in "cctx".
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_ವಾದಗಳು_shadowing(ufunc_T *ufunc, cctx_T *cctx)
{
    ಪೂರ್ಣಾಂಕ	    i;
    char_u  *arg;
    ಪೂರ್ಣಾಂಕ	    r = OK;

    // Make sure arguments are not found when compiling a second time.
    ufunc->uf_ವಾದಗಳು_visible = 0;

    // Check for arguments shadowing variables from the context.
    ಗಾಗಿ (i = 0; i < ufunc->uf_ವಾದಗಳು.ga_len; ++i)
    {
	arg = ((char_u **)(ufunc->uf_ವಾದಗಳು.ga_ದತ್ತಾಂಶ))[i];
	ಆದರೆ (check_defined(arg, STRLEN(arg), cctx, ಖಾಲಿ, TRUE) == FAIL)
	{
	    r = FAIL;
	    ನಿಲ್ಲಿಸು;
	}
    }
    ufunc->uf_ವಾದಗಳು_visible = ufunc->uf_ವಾದಗಳು.ga_len;
    ಹಿಂತಿರುಗಿ r;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
/*
 * Get a count before a command.  Can only be a number.
 * Returns zero if there is no count.
 * Returns -1 if there is something wrong.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
get_cmd_ಎಣಿಕೆ(char_u *ಸಾಲು, exarg_T *eap)
{
    char_u *p;

    // skip over colons and white space
    ಗಾಗಿ (p = ಸಾಲು; *p == ':' || VIM_ISWHITE(*p); ++p)
	;
    ಆದರೆ (!ಸುರಕ್ಷಿತ_isdigit(*p))
    {
	// The command or modifiers must be following.  Assume a lower case
	// character means there is a modifier.
	ಆದರೆ (p < eap->cmd && !vim_islower(*p))
	{
	    emsg(_(e_invalid_range));
	    ಹಿಂತಿರುಗಿ -1;
	}
	ಹಿಂತಿರುಗಿ 0;
    }
    ಹಿಂತಿರುಗಿ atol((ಅಕ್ಷರ *)p);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get the compilation type that should be used for "ufunc".
 * Keep in sync with INSTRUCTIONS().
 */
    compiletype_T
get_compile_type(ufunc_T *ufunc)
{
    // Update uf_has_breakpoint if needed.
    update_has_breakpoint(ufunc);

    ಆದರೆ (debug_break_level > 0 || may_break_ಒಳ_function(ufunc))
	ಹಿಂತಿರುಗಿ CT_DEBUG;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
    {
	ಆದರೆ (!ufunc->uf_profiling && has_profiling(FALSE, ufunc->uf_name, ಖಾಲಿ,
							    &ufunc->uf_hash))
	    func_do_profile(ufunc);
	ಆದರೆ (ufunc->uf_profiling)
	    ಹಿಂತಿರುಗಿ CT_PROFILE;
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ CT_NONE;
}

/*
 * Free the compiled instructions saved for a def function.  This is used when
 * compiling a def function and the function was compiled before.
 * The index is reused.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_def_function(ufunc_T *ufunc, compiletype_T compile_type)
{
    isn_T	*instr_dest = ಖಾಲಿ;
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + ufunc->uf_dfunc_idx;

    ಆಯ್ಕೆ (compile_type)
    {
	ಸ್ಥಿತಿ CT_PROFILE:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	    instr_dest = dfunc->df_instr_prof; ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ CT_NONE:   instr_dest = dfunc->df_instr; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CT_DEBUG:  instr_dest = dfunc->df_instr_debug; ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (instr_dest != ಖಾಲಿ)
	// Was compiled in this mode before: Free old instructions.
	delete_def_function_contents(dfunc, FALSE);

    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&dfunc->df_var_ಹೆಸರುಗಳು);
    dfunc->df_defer_var_idx = 0;
}

/*
 * Add a function to the list of :def functions.
 * This sets "ufunc->uf_dfunc_idx" but the function isn't compiled yet.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_def_function(ufunc_T *ufunc)
{
    dfunc_T *dfunc;

    ಆದರೆ (def_functions.ga_len == 0)
    {
	// The first position is not used, so that a zero uf_dfunc_idx means it
	// wasn't set.
	ಆದರೆ (GA_GROW_FAILS(&def_functions, 1))
	    ಹಿಂತಿರುಗಿ FAIL;
	++def_functions.ga_len;
    }

    // Add the function to "def_functions".
    ಆದರೆ (GA_GROW_FAILS(&def_functions, 1))
	ಹಿಂತಿರುಗಿ FAIL;
    dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + def_functions.ga_len;
    CLEAR_ಸೂಚಕ(dfunc);
    dfunc->df_idx = def_functions.ga_len;
    ufunc->uf_dfunc_idx = dfunc->df_idx;
    dfunc->df_ufunc = ufunc;
    dfunc->df_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ga_init2(&dfunc->df_var_ಹೆಸರುಗಳು, ಗಾತ್ರ(char_u *), 10);
    ++dfunc->df_refcount;
    ++def_functions.ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_dfunc_ufunc_init(
    ufunc_T		*ufunc,
    cctx_T		*outer_cctx,
    compiletype_T	compile_type,
    ಪೂರ್ಣಾಂಕ			*new_def_function)
{
    // When using a function that was compiled before: Free old instructions.
    // The index is reused.  Otherwise add a new entry in "def_functions".
    ಆದರೆ (ufunc->uf_dfunc_idx > 0)
	clear_def_function(ufunc, compile_type);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (ಕೂಡಿಸು_def_function(ufunc) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	*new_def_function = TRUE;
    }

    ಆದರೆ ((ufunc->uf_flags & FC_ಆವರಣ) && outer_cctx == ಖಾಲಿ)
    {
	semsg(_(e_compiling_ಆವರಣ_without_context_str),
						printable_func_name(ufunc));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ufunc->uf_def_ಸ್ಥಿತಿ = UF_COMPILING;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Initialize the compilation context for compiling a def function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compile_dfunc_cctx_init(
    cctx_T		*cctx,
    cctx_T		*outer_cctx,
    ufunc_T		*ufunc,
    compiletype_T	compile_type)
{
    CLEAR_ಕ್ಷೇತ್ರ(*cctx);

    cctx->ctx_compile_type = compile_type;
    cctx->ctx_ufunc = ufunc;
    cctx->ctx_lnum = -1;
    cctx->ctx_outer = outer_cctx;
    ga_init2(&cctx->ctx_locals, ಗಾತ್ರ(lvar_T), 10);
    // Each entry on the type stack consists of two type pointers.
    ga_init2(&cctx->ctx_type_ರಾಶಿ, ಗಾತ್ರ(type2_T), 50);
    cctx->ctx_type_list = &ufunc->uf_type_list;
    ga_init2(&cctx->ctx_instr, ಗಾತ್ರ(isn_T), 50);
}

/*
 * For an object constructor, generate instruction to setup "this" (the first
 * local variable) and to initialize the object variables.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
obj_constructor_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    generate_CONSTRUCT(cctx, ufunc->uf_class);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < ufunc->uf_class->class_obj_member_ಎಣಿಕೆ; ++i)
    {
	ocmember_T *m = &ufunc->uf_class->class_obj_members[i];

	ಆದರೆ (i < 2 && IS_ENUM(ufunc->uf_class))
	    // The first two object variables in an enum are the name
	    // and the ordinal.  These are set by the ISN_CONSTRUCT
	    // instruction.  So don't generate instructions to set
	    // these variables.
	    ಮುಂದುವರಿ;

	ಆದರೆ (m->ocm_init != ಖಾಲಿ)
	{
	    char_u	*ಅಭಿವ್ಯಕ್ತಿ = m->ocm_init;
	    sctx_T	save_current_sctx;
	    ಪೂರ್ಣಾಂಕ		change_sctx = FALSE;

	    // If the member variable initialization script context is
	    // different from the current script context, then change it.
	    ಆದರೆ (current_sctx.sc_sid != m->ocm_init_sctx.sc_sid)
		change_sctx = TRUE;

	    ಆದರೆ (change_sctx)
	    {
		// generate an instruction to change the script context to the
		// member variable initialization script context.
		save_current_sctx = current_sctx;
		current_sctx = m->ocm_init_sctx;
		generate_SCRIPTCTX_ಗಣ(cctx, current_sctx);
	    }

	    ಪೂರ್ಣಾಂಕ r = compile_expr0(&ಅಭಿವ್ಯಕ್ತಿ, cctx);

	    ಆದರೆ (change_sctx)
	    {
		// restore the previous script context
		current_sctx = save_current_sctx;
		generate_SCRIPTCTX_ಗಣ(cctx, current_sctx);
	    }

	    ಆದರೆ (r == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;

	    ಆದರೆ (!ತುದಿಗಳು_excmd2(m->ocm_init, ಅಭಿವ್ಯಕ್ತಿ))
	    {
		semsg(_(e_trailing_characters_str), ಅಭಿವ್ಯಕ್ತಿ);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    type_T	*type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಆದರೆ (m->ocm_type->tt_type == VAR_ANY
		    && !(m->ocm_flags & OCMFLAG_HAS_TYPE)
		    && type->tt_type != VAR_SPECIAL)
	    {
		// If the member variable type is not yet set, then use
		// the initialization expression type.
		m->ocm_type = type;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// The type of the member initialization expression is
		// determined at run time.  Add a runtime type check.
		where_T	where = WHERE_INIT;
		where.wt_ಬಗೆ = WT_MEMBER;
		where.wt_func_name = (ಅಕ್ಷರ *)m->ocm_name.string;
		ಆದರೆ (need_type_where(type, m->ocm_type, 0, -1,
					where, cctx, FALSE, FALSE) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ತಳ್ಳು_default_ಮೌಲ್ಯ(cctx, m->ocm_type->tt_type, FALSE, ಖಾಲಿ);

	ಆದರೆ (((m->ocm_type->tt_type == VAR_DICT
			|| m->ocm_type->tt_type == VAR_LIST)
		    && m->ocm_type->tt_member != ಖಾಲಿ
		    && m->ocm_type->tt_member != &t_any
		    && m->ocm_type->tt_member != &t_unknown)
		|| (m->ocm_type->tt_type == VAR_TUPLE
		    && m->ocm_type->tt_argcount > 0))
	    // Set the type in the list, tuple or dict, so that it can be
	    // checked, also in legacy script.
	    generate_SETTYPE(cctx, m->ocm_type);

	generate_STORE_THIS(cctx, i);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * For an object method and an constructor, generate instruction to setup
 * "this" (the first local variable).  For a constructor, generate instructions
 * to initialize the object variables.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
obj_ವಿಧಾನ_prologue(ufunc_T *ufunc, cctx_T *cctx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + ufunc->uf_dfunc_idx;

    ಆದರೆ (GA_GROW_FAILS(&dfunc->df_var_ಹೆಸರುಗಳು, 1))
	ಹಿಂತಿರುಗಿ FAIL;

    ((char_u **)dfunc->df_var_ಹೆಸರುಗಳು.ga_ದತ್ತಾಂಶ)[0] =
						vim_strsave((char_u *)"this");
    ++dfunc->df_var_ಹೆಸರುಗಳು.ga_len;

    // In the constructor allocate memory for the object and initialize the
    // object members.
    ಆದರೆ (IS_CONSTRUCTOR_ವಿಧಾನ(ufunc))
	ಹಿಂತಿರುಗಿ obj_constructor_prologue(ufunc, cctx);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Produce instructions for the default values of optional arguments.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_def_function_default_ವಾದಗಳು(
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    ಪೂರ್ಣಾಂಕ	ಎಣಿಕೆ_2 = ufunc->uf_def_ವಾದಗಳು.ga_len;
    ಪೂರ್ಣಾಂಕ	ಮೊದಲ_def_arg = ufunc->uf_ವಾದಗಳು.ga_len - ಎಣಿಕೆ_2;
    ಪೂರ್ಣಾಂಕ	i;
    ಪೂರ್ಣಾಂಕ	off = ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + (ufunc->uf_va_name != ಖಾಲಿ ? 1 : 0);
    ಪೂರ್ಣಾಂಕ	did_ಗಣ_arg_type = FALSE;

    // Produce instructions for the default values of optional arguments.
    SOURCING_LNUM = 0;  // line number unknown
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	char_u *arg = ((char_u **)(ufunc->uf_def_ವಾದಗಳು.ga_ದತ್ತಾಂಶ))[i];
	ಆದರೆ (STRCMP(arg, "v:none") == 0)
	    // "arg = v:none" means the argument is optional without
	    // setting a value when the argument is missing.
	    ಮುಂದುವರಿ;

	type_T	*val_type;
	ಪೂರ್ಣಾಂಕ		arg_idx = ಮೊದಲ_def_arg + i;
	where_T	where = WHERE_INIT;
	ಪೂರ್ಣಾಂಕ		jump_instr_idx = instr->ga_len;
	isn_T	*isn;

	// Use a JUMP_IF_ARG_SET instruction to skip if the value was given.
	ಆದರೆ (generate_JUMP_IF_ARG(cctx, ISN_JUMP_IF_ARG_ಗಣ,
						i - ಎಣಿಕೆ_2 - off) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// Make sure later arguments are not found.
	ufunc->uf_ವಾದಗಳು_visible = arg_idx;

	ಪೂರ್ಣಾಂಕ r = compile_expr0(&arg, cctx);
	ಆದರೆ (r == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// If no type specified use the type of the default value.
	// Otherwise check that the default value type matches the
	// specified type.
	val_type = get_type_on_ರಾಶಿ(cctx, 0);
	where.wt_ಸೂಚ್ಯಂಕ = arg_idx + 1;
	where.wt_ಬಗೆ = WT_ವಾದ;
	ಆದರೆ (ufunc->uf_arg_types[arg_idx] == &t_unknown)
	{
	    did_ಗಣ_arg_type = TRUE;
	    ufunc->uf_arg_types[arg_idx] = val_type;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (need_type_where(val_type, ufunc->uf_arg_types[arg_idx],
		    0, -1, where, cctx, FALSE, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (generate_STORE(cctx, ISN_STORE, i - ಎಣಿಕೆ_2 - off, ಖಾಲಿ) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// set instruction index in JUMP_IF_ARG_SET to here
	isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + jump_instr_idx;
	isn->isn_arg.jumparg.jump_where = instr->ga_len;
    }

    ಆದರೆ (did_ಗಣ_arg_type)
	ಗಣ_function_type(ufunc);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile def function body.  Loop over all the lines in the function and
 * generate instructions.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_def_function_body(
    ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_func_lnum,
    ಪೂರ್ಣಾಂಕ		check_return_type,
    garray_T	*ಸಾಲುಗಳು_ಗಮ್ಯ_free,
    ಅಕ್ಷರ	**errormsg,
    cctx_T	*cctx)
{
    char_u	*ಸಾಲು = ಖಾಲಿ;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಪೂರ್ಣಾಂಕ		prof_lnum = -1;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		debug_lnum = -1;

    ಗಾಗಿ (;;)
    {
	exarg_T	    ea;
	ಪೂರ್ಣಾಂಕ	    starts_with_colon = FALSE;
	char_u	    *cmd;
	cmdmod_T    local_cmdmod;

	// Bail out on the first error to avoid a flood of errors and report
	// the right line number when inside try/catch.
	ಆದರೆ (did_emsg_before != did_emsg)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (ಸಾಲು != ಖಾಲಿ && *ಸಾಲು == '|')
	    // the line continues after a '|'
	    ++ಸಾಲು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು != ಖಾಲಿ && *skipwhite(ಸಾಲು) != NUL
		&& !(*ಸಾಲು == '#' && (ಸಾಲು == cctx->ctx_ಸಾಲು_ಆರಂಭ
						    || VIM_ISWHITE(ಸಾಲು[-1]))))
	{
	    semsg(_(e_trailing_characters_str), ಸಾಲು);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸಾಲು != ಖಾಲಿ && vim9_bad_comment(skipwhite(ಸಾಲು)))
	    ಹಿಂತಿರುಗಿ FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಸಾಲು = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, FALSE);
	    ಆದರೆ (cctx->ctx_lnum >= ಕೊನೆಯ_func_lnum)
	    {
		// beyond the last line
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
		ಆದರೆ (cctx->ctx_skip != SKIP_YES)
		    may_generate_prof_ಕೊನೆ(cctx, prof_lnum);
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    }
	    // Make a copy, splitting off nextcmd and removing trailing spaces
	    // may change it.
	    ಆದರೆ (ಸಾಲು != ಖಾಲಿ)
	    {
		ಸಾಲು = vim_strsave(ಸಾಲು);
		ಆದರೆ (ga_ಕೂಡಿಸು_string(ಸಾಲುಗಳು_ಗಮ್ಯ_free, ಸಾಲು) == FAIL)
		{
		    vim_free(ಸಾಲು);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	}

	CLEAR_ಕ್ಷೇತ್ರ(ea);
	ea.cmdlinep = &ಸಾಲು;
	ea.cmd = skipwhite(ಸಾಲು);
	ea.skip = cctx->ctx_skip == SKIP_YES;

	ಆದರೆ (*ea.cmd == '#')
	{
	    // "#" starts a comment, but "#{" is an error
	    ಆದರೆ (vim9_bad_comment(ea.cmd))
		ಹಿಂತಿರುಗಿ FAIL;
	    ಸಾಲು = (char_u *)"";
	    ಮುಂದುವರಿ;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE && cctx->ctx_lnum != prof_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    may_generate_prof_ಕೊನೆ(cctx, prof_lnum);

	    prof_lnum = cctx->ctx_lnum;
	    generate_instr(cctx, ISN_PROF_ಆರಂಭ);
	}
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (cctx->ctx_compile_type == CT_DEBUG && cctx->ctx_lnum != debug_lnum
						  && cctx->ctx_skip != SKIP_YES)
	{
	    debug_lnum = cctx->ctx_lnum;
	    generate_instr_debug(cctx);
	}
	cctx->ctx_prev_lnum = cctx->ctx_lnum + 1;

	// Some things can be recognized by the first character.
	ಆಯ್ಕೆ (*ea.cmd)
	{
	    ಸ್ಥಿತಿ '}':
		{
		    // "}" ends a block scope
		    scopetype_T stype = cctx->ctx_scope == ಖಾಲಿ
					  ? NO_SCOPE : cctx->ctx_scope->se_type;

		    ಆದರೆ (stype == ಖಂಡ_SCOPE)
		    {
			compile_endblock(cctx);
			ಸಾಲು = ea.cmd;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			emsg(_(e_using_rcurly_outside_if_ಖಂಡ_scope));
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    ಆದರೆ (ಸಾಲು != ಖಾಲಿ)
			ಸಾಲು = skipwhite(ea.cmd + 1);
		    ಮುಂದುವರಿ;
		}

	    ಸ್ಥಿತಿ '{':
		// "{" starts a block scope
		// "{'a': 1}->func() is something else
		ಆದರೆ (ತುದಿಗಳು_excmd(*skipwhite(ea.cmd + 1)))
		{
		    ಸಾಲು = compile_ಖಂಡ(ea.cmd, cctx);
		    ಮುಂದುವರಿ;
		}
		ನಿಲ್ಲಿಸು;
	}

	/*
	 * COMMAND MODIFIERS
	 */
	cctx->ctx_has_cmdmod = FALSE;
	ಆದರೆ (parse_command_modifiers(&ea, errormsg, &local_cmdmod, FALSE)
								       == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	generate_cmdmods(cctx, &local_cmdmod);
	undo_cmdmod(&local_cmdmod);

	// Check if there was a colon after the last command modifier or before
	// the current position.
	ಗಾಗಿ (p = ea.cmd; p >= ಸಾಲು; --p)
	{
	    ಆದರೆ (*p == ':')
		starts_with_colon = TRUE;
	    ಆದರೆ (p < ea.cmd && !VIM_ISWHITE(*p))
		ನಿಲ್ಲಿಸು;
	}

	// Skip ":call" to get to the function name, unless using :legacy
	p = ea.cmd;
	ಆದರೆ (!(local_cmdmod.cmod_flags & CMOD_LEGACY))
	{
	    ಆದರೆ (checkforcmd(&ea.cmd, "call", 3))
	    {
		ಆದರೆ (*ea.cmd == '(')
		    // not for "call()"
		    ea.cmd = p;
		ಇಲ್ಲದಿದ್ದರೆ
		    ea.cmd = skipwhite(ea.cmd);
	    }

	    ಆದರೆ (!starts_with_colon)
	    {
		ಪೂರ್ಣಾಂಕ	    ನಿಯೋಜಿಸು;

		// Check for assignment after command modifiers.
		ನಿಯೋಜಿಸು = may_compile_assignment(&ea, &ಸಾಲು, cctx);
		ಆದರೆ (ನಿಯೋಜಿಸು == OK)
		    ಹೋಗು nextline;
		ಆದರೆ (ನಿಯೋಜಿಸು == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
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
	ಆದರೆ ((*cmd != '$' || starts_with_colon)
		&& (starts_with_colon
		    || !(*cmd == '\''
			|| (cmd[0] == '0' && cmd[1] == 'z')
			|| (cmd[0] != NUL && cmd[0] == cmd[1]
					    && (*cmd == '+' || *cmd == '-'))
			|| ಸಂಖ್ಯೆ_ವಿಧಾನ(cmd))))
	{
	    ea.cmd = skip_range(ea.cmd, TRUE, ಖಾಲಿ);
	    ಆದರೆ (ea.cmd > cmd)
	    {
		ಆದರೆ (!starts_with_colon
				   && !(local_cmdmod.cmod_flags & CMOD_LEGACY))
		{
		    semsg(_(e_colon_required_before_range_str), cmd);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ea.addr_ಎಣಿಕೆ = 1;
		ಆದರೆ (ತುದಿಗಳು_excmd2(ಸಾಲು, ea.cmd))
		{
		    // A range without a command: jump to the line.
		    generate_EXEC(cctx, ISN_EXECRANGE,
					      vim_strnsave(cmd, ea.cmd - cmd));
		    ಸಾಲು = ea.cmd;
		    ಹೋಗು nextline;
		}
	    }
	}
	p = ಕಂಡುಹಿಡಿ_ex_command(&ea, ಖಾಲಿ,
		starts_with_colon || (local_cmdmod.cmod_flags & CMOD_LEGACY)
						  ? ಖಾಲಿ : ಅಂಶ_exists, cctx);

	ಆದರೆ (p == ಖಾಲಿ)
	{
	    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
		semsg(_(e_ambiguous_use_ದ_user_defined_command_str), ea.cmd);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// When using ":legacy cmd" always use compile_exec().
	ಆದರೆ (local_cmdmod.cmod_flags & CMOD_LEGACY)
	{
	    char_u *ಆರಂಭ = ea.cmd;

	    ಆಯ್ಕೆ (ea.cmdidx)
	    {
		ಸ್ಥಿತಿ CMD_if:
		ಸ್ಥಿತಿ CMD_elseif:
		ಸ್ಥಿತಿ CMD_else:
		ಸ್ಥಿತಿ CMD_endif:
		ಸ್ಥಿತಿ CMD_for:
		ಸ್ಥಿತಿ CMD_endfor:
		ಸ್ಥಿತಿ CMD_continue:
		ಸ್ಥಿತಿ CMD_break:
		ಸ್ಥಿತಿ CMD_while:
		ಸ್ಥಿತಿ CMD_endwhile:
		ಸ್ಥಿತಿ CMD_try:
		ಸ್ಥಿತಿ CMD_catch:
		ಸ್ಥಿತಿ CMD_finally:
		ಸ್ಥಿತಿ CMD_endtry:
			semsg(_(e_cannot_use_legacy_with_command_str), ea.cmd);
			ಹಿಂತಿರುಗಿ FAIL;
		ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
	    }

	    // ":legacy return expr" needs to be handled differently.
	    ಆದರೆ (checkforcmd(&ಆರಂಭ, "return", 4))
		ea.cmdidx = CMD_return;
	    ಇಲ್ಲದಿದ್ದರೆ
		ea.cmdidx = CMD_legacy;
	}

	ಆದರೆ (p == ea.cmd && ea.cmdidx != CMD_ಗಾತ್ರ)
	{
	    // "eval" is used for "val->func()" and "var" for "var = val", then
	    // "p" is equal to "ea.cmd" for a valid command.
	    ಆದರೆ (ea.cmdidx == CMD_eval || ea.cmdidx == CMD_var)
		;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	    {
		ಸಾಲು += STRLEN(ಸಾಲು);
		ಹೋಗು nextline;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		semsg(_(e_command_not_recognized_str), ea.cmd);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	ಆದರೆ ((cctx->ctx_had_return || cctx->ctx_had_throw)
		&& ea.cmdidx != CMD_elseif
		&& ea.cmdidx != CMD_else
		&& ea.cmdidx != CMD_endif
		&& ea.cmdidx != CMD_endfor
		&& ea.cmdidx != CMD_endwhile
		&& ea.cmdidx != CMD_catch
		&& ea.cmdidx != CMD_finally
		&& ea.cmdidx != CMD_endtry
		&& !ignore_unreachable_ಸಂಕೇತ_for_testing)
	{
	    semsg(_(e_unreachable_ಸಂಕೇತ_after_str),
				     cctx->ctx_had_return ? "return" : "throw");
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// When processing the end of an if-else or try-catch block, don't
	// clear the "ctx_had_throw" flag.  If such a block ends in a "throw"
	// statement, then it is considered to end in a "return" statement.
	// The "ctx_had_throw" is cleared immediately after processing the
	// block ending statement.
	// Otherwise, clear the "had_throw" flag.
	ಆದರೆ (ea.cmdidx != CMD_else && ea.cmdidx != CMD_elseif
						&& ea.cmdidx != CMD_endif
						&& ea.cmdidx != CMD_catch
						&& ea.cmdidx != CMD_finally
						&& ea.cmdidx != CMD_endtry)
	    cctx->ctx_had_throw = FALSE;

	p = skipwhite(p);
	ಆದರೆ (ea.cmdidx != CMD_ಗಾತ್ರ
			    && ea.cmdidx != CMD_write && ea.cmdidx != CMD_read)
	{
	    ಆದರೆ (ea.cmdidx >= 0)
		ea.argt = excmd_get_argt(ea.cmdidx);
	    ಆದರೆ ((ea.argt & EX_BANG) && *p == '!')
	    {
		ea.forceit = TRUE;
		p = skipwhite(p + 1);
	    }
	    ಆದರೆ ((ea.argt & EX_RANGE) == 0 && ea.addr_ಎಣಿಕೆ > 0)
	    {
		emsg(_(e_no_range_allowed));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	ಆಯ್ಕೆ (ea.cmdidx)
	{
	    ಸ್ಥಿತಿ CMD_def:
	    ಸ್ಥಿತಿ CMD_function:
		    ea.arg = p;
		    ಸಾಲು = compile_nested_function(&ea, cctx, ಸಾಲುಗಳು_ಗಮ್ಯ_free);
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_return:
		    ಸಾಲು = compile_return(p, check_return_type,
				 local_cmdmod.cmod_flags & CMOD_LEGACY, cctx);
		    cctx->ctx_had_return = TRUE;
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_let:
		    emsg(_(e_cannot_use_let_ಒಳ_vim9_script));
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_var:
	    ಸ್ಥಿತಿ CMD_final:
	    ಸ್ಥಿತಿ CMD_const:
	    ಸ್ಥಿತಿ CMD_increment:
	    ಸ್ಥಿತಿ CMD_decrement:
		    ಸಾಲು = compile_assignment(p, &ea, ea.cmdidx, cctx);
		    ಆದರೆ (ಸಾಲು == p)
		    {
			emsg(_(e_invalid_assignment));
			ಸಾಲು = ಖಾಲಿ;
		    }
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_unlet:
	    ಸ್ಥಿತಿ CMD_unlockvar:
	    ಸ್ಥಿತಿ CMD_lockvar:
		    ಸಾಲು = compile_unletlock(p, &ea, cctx);
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_import:
		    emsg(_(e_import_can_only_be_ಬಳಸಿದ_ಒಳ_script));
		    ಸಾಲು = ಖಾಲಿ;
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_if:
		    ಸಾಲು = compile_if(p, cctx);
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_elseif:
		    ಸಾಲು = compile_elseif(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_else:
		    ಸಾಲು = compile_else(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_endif:
		    ಸಾಲು = compile_endif(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_while:
		    ಸಾಲು = compile_while(p, cctx);
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_endwhile:
		    ಸಾಲು = compile_endwhile(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_for:
		    ಸಾಲು = compile_for(p, cctx);
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_endfor:
		    ಸಾಲು = compile_endfor(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_continue:
		    ಸಾಲು = compile_continue(p, cctx);
		    // "continue" unconditionally jumps back to the loop,
		    // like a "return".
		    cctx->ctx_had_return = TRUE;
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_break:
		    ಸಾಲು = compile_break(p, cctx);
		    // "break" unconditionally exits the loop, like a "return".
		    cctx->ctx_had_return = TRUE;
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_try:
		    ಸಾಲು = compile_try(p, cctx);
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_catch:
		    ಸಾಲು = compile_catch(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_finally:
		    ಸಾಲು = compile_finally(p, cctx);
		    cctx->ctx_had_return = FALSE;
		    cctx->ctx_had_throw = FALSE;
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_endtry:
		    ಸಾಲು = compile_endtry(p, cctx);
		    cctx->ctx_had_throw = FALSE;
		    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ CMD_throw:
		    ಸಾಲು = compile_throw(p, cctx);
		    cctx->ctx_had_throw = TRUE;
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_eval:
		    ಸಾಲು = compile_eval(p, cctx);
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_defer:
		    ಸಾಲು = compile_defer(p, cctx);
		    ನಿಲ್ಲಿಸು;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
	    ಸ್ಥಿತಿ CMD_echowindow:
		    {
			ದೀರ್ಘ cmd_ಎಣಿಕೆ = get_cmd_ಎಣಿಕೆ(ಸಾಲು, &ea);
			ಆದರೆ (cmd_ಎಣಿಕೆ < 0)
			    ಸಾಲು = ಖಾಲಿ;
			ಇಲ್ಲದಿದ್ದರೆ
			    ಸಾಲು = compile_mult_ಅಭಿವ್ಯಕ್ತಿ(p, ea.cmdidx,
							     cmd_ಎಣಿಕೆ, cctx);
		    }
		    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    ಸ್ಥಿತಿ CMD_echo:
	    ಸ್ಥಿತಿ CMD_echon:
	    ಸ್ಥಿತಿ CMD_echoconsole:
	    ಸ್ಥಿತಿ CMD_echoerr:
	    ಸ್ಥಿತಿ CMD_echomsg:
	    ಸ್ಥಿತಿ CMD_execute:
		    ಸಾಲು = compile_mult_ಅಭಿವ್ಯಕ್ತಿ(p, ea.cmdidx, 0, cctx);
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_put:
		    ea.cmd = cmd;
		    ಸಾಲು = compile_put(p, &ea, cctx, FALSE);
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_iput:
		    ea.cmd = cmd;
		    ಸಾಲು = compile_put(p, &ea, cctx, TRUE);
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_substitute:
		    ಆದರೆ (check_global_and_subst(ea.cmd, p) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
			ಸಾಲು = (char_u *)"";
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ea.arg = p;
			ಸಾಲು = compile_substitute(ಸಾಲು, &ea, cctx);
		    }
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_redir:
		    ea.arg = p;
		    ಸಾಲು = compile_redir(ಸಾಲು, &ea, cctx);
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_cexpr:
	    ಸ್ಥಿತಿ CMD_lexpr:
	    ಸ್ಥಿತಿ CMD_caddexpr:
	    ಸ್ಥಿತಿ CMD_laddexpr:
	    ಸ್ಥಿತಿ CMD_cgetexpr:
	    ಸ್ಥಿತಿ CMD_lgetexpr:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		    ea.arg = p;
		    ಸಾಲು = compile_cexpr(ಸಾಲು, &ea, cctx);
#ಇಲ್ಲದಿದ್ದರೆ
		    ex_ni(&ea);
		    ಸಾಲು = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_append:
	    ಸ್ಥಿತಿ CMD_change:
	    ಸ್ಥಿತಿ CMD_ಸೇರಿಸು:
	    ಸ್ಥಿತಿ CMD_k:
	    ಸ್ಥಿತಿ CMD_open:
	    ಸ್ಥಿತಿ CMD_t:
	    ಸ್ಥಿತಿ CMD_xit:
		    not_ಒಳ_vim9(&ea, false);
		    ಹಿಂತಿರುಗಿ FAIL;

	    ಸ್ಥಿತಿ CMD_ಗಾತ್ರ:
		    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
		    {
			semsg(_(e_invalid_command_str), ea.cmd);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    // We don't check for a next command here.
		    ಸಾಲು = (char_u *)"";
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_lua:
	    ಸ್ಥಿತಿ CMD_mzscheme:
	    ಸ್ಥಿತಿ CMD_perl:
	    ಸ್ಥಿತಿ CMD_py3:
	    ಸ್ಥಿತಿ CMD_python3:
	    ಸ್ಥಿತಿ CMD_python:
	    ಸ್ಥಿತಿ CMD_pythonx:
	    ಸ್ಥಿತಿ CMD_ruby:
	    ಸ್ಥಿತಿ CMD_tcl:
		    ea.arg = p;
		    ಆದರೆ (vim_strchr(ಸಾಲು, '\n') == ಖಾಲಿ)
			ಸಾಲು = compile_exec(ಸಾಲು, &ea, cctx);
		    ಇಲ್ಲದಿದ್ದರೆ
			// heredoc lines have been concatenated with NL
			// characters in get_function_body()
			ಸಾಲು = compile_script(ಸಾಲು, cctx);
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_vim9script:
		    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
		    {
			emsg(_(e_vim9script_can_only_be_ಬಳಸಿದ_ಒಳ_script));
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    ಸಾಲು = (char_u *)"";
		    ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ CMD_class:
		    emsg(_(e_class_can_only_be_ಬಳಸಿದ_ಒಳ_script));
		    ಹಿಂತಿರುಗಿ FAIL;

	    ಸ್ಥಿತಿ CMD_enum:
		    emsg(_(e_enum_can_only_be_ಬಳಸಿದ_ಒಳ_script));
		    ಹಿಂತಿರುಗಿ FAIL;

	    ಸ್ಥಿತಿ CMD_interface:
		    emsg(_(e_interface_can_only_be_ಬಳಸಿದ_ಒಳ_script));
		    ಹಿಂತಿರುಗಿ FAIL;

	    ಸ್ಥಿತಿ CMD_type:
		    emsg(_(e_type_can_only_be_ಬಳಸಿದ_ಒಳ_script));
		    ಹಿಂತಿರುಗಿ FAIL;

	    ಸ್ಥಿತಿ CMD_global:
		    ಆದರೆ (check_global_and_subst(ea.cmd, p) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    // FALLTHROUGH
	    ಪೂರ್ವನಿಯೋಜಿತ:
		    // Not recognized, execute with do_cmdline_cmd().
		    ea.arg = p;
		    ಸಾಲು = compile_exec(ಸಾಲು, &ea, cctx);
		    ನಿಲ್ಲಿಸು;
	}
nextline:
	ಆದರೆ (ಸಾಲು == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಸಾಲು = skipwhite(ಸಾಲು);

	// Undo any command modifiers.
	generate_undo_cmdmods(cctx);

	ಆದರೆ (cctx->ctx_type_ರಾಶಿ.ga_len < 0)
	{
	    iemsg("Type stack underflow");
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    } // END of the loop over all the function body lines.

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Returns TRUE if the end of a scope (if, while, for, block) is missing.
 * Called after compiling a def function body.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_dfunc_scope_ಕೊನೆ_missing(cctx_T *cctx)
{
    ಆದರೆ (cctx->ctx_scope == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆಯ್ಕೆ (cctx->ctx_scope->se_type)
    {
	ಸ್ಥಿತಿ IF_SCOPE:
	    emsg(_(e_missing_endif));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ WHILE_SCOPE:
	    emsg(_(e_missing_endwhile));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ FOR_SCOPE:
	    emsg(_(e_missing_endfor));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TRY_SCOPE:
	    emsg(_(e_missing_endtry));
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ಖಂಡ_SCOPE:
	    // end block scope from :try (maybe)
	    compile_endblock(cctx);
	    ಆದರೆ (cctx->ctx_scope != ಖಾಲಿ
		    && cctx->ctx_scope->se_type == TRY_SCOPE)
		emsg(_(e_missing_endtry));
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_missing_rcurly));
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    emsg(_(e_missing_rcurly));
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * When compiling a def function, if it doesn't have an explicit return
 * statement, then generate a default return instruction.  For an object
 * constructor, return the object.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_dfunc_generate_default_return(ufunc_T *ufunc, cctx_T *cctx)
{
    // TODO: if a function ends in "throw" but there was a return elsewhere we
    // should not assume the return type is "void".
    ಆದರೆ (cctx->ctx_had_return || cctx->ctx_had_throw)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (ufunc->uf_ret_type->tt_type == VAR_UNKNOWN)
	ufunc->uf_ret_type = &t_void;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ufunc->uf_ret_type->tt_type != VAR_VOID
					&& !IS_CONSTRUCTOR_ವಿಧಾನ(ufunc))
    {
	emsg(_(e_missing_return_statement));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Return void if there is no return at the end.
    // For a constructor return the object.
    ಆದರೆ (IS_CONSTRUCTOR_ವಿಧಾನ(ufunc))
    {
	generate_instr(cctx, ISN_RETURN_OBJECT);
	ufunc->uf_ret_type = &ufunc->uf_class->class_object_type;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	generate_instr(cctx, ISN_RETURN_VOID);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Perform the chores after successfully compiling a def function.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compile_dfunc_epilogue(
    cctx_T	*outer_cctx,
    ufunc_T	*ufunc,
    garray_T	*instr,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + ufunc->uf_dfunc_idx;
    dfunc->df_deleted = FALSE;
    dfunc->df_script_seq = current_sctx.sc_seq;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE)
    {
	dfunc->df_instr_prof = instr->ga_ದತ್ತಾಂಶ;
	dfunc->df_instr_prof_ಎಣಿಕೆ = instr->ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (cctx->ctx_compile_type == CT_DEBUG)
	{
	    dfunc->df_instr_debug = instr->ga_ದತ್ತಾಂಶ;
	    dfunc->df_instr_debug_ಎಣಿಕೆ = instr->ga_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dfunc->df_instr = instr->ga_ದತ್ತಾಂಶ;
	    dfunc->df_instr_ಎಣಿಕೆ = instr->ga_len;
	}
    dfunc->df_varcount = dfunc->df_var_ಹೆಸರುಗಳು.ga_len;
    dfunc->df_has_ಆವರಣ = cctx->ctx_has_ಆವರಣ;

    ಆದರೆ (cctx->ctx_outer_ಬಳಸಿದ)
    {
	ufunc->uf_flags |= FC_ಆವರಣ;
	ಆದರೆ (outer_cctx != ಖಾಲಿ)
	    ++outer_cctx->ctx_ಆವರಣ_ಎಣಿಕೆ;
    }

    ufunc->uf_def_ಸ್ಥಿತಿ = UF_COMPILED;
}

/*
 * Perform the cleanup when a def function compilation fails.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
compile_dfunc_ufunc_cleanup(
    ufunc_T	*ufunc,
    garray_T	*instr,
    ಪೂರ್ಣಾಂಕ		new_def_function,
    ಅಕ್ಷರ	*errormsg,
    ಪೂರ್ಣಾಂಕ		did_emsg_before,
    cctx_T	*cctx)
{
    dfunc_T	*dfunc;

    dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + ufunc->uf_dfunc_idx;

    // Compiling aborted, free the generated instructions.
    clear_instr_ga(instr);
    VIM_CLEAR(dfunc->df_name);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&dfunc->df_var_ಹೆಸರುಗಳು);

    // If using the last entry in the table and it was added above, we
    // might as well remove it.
    ಆದರೆ (!dfunc->df_deleted && new_def_function
			&& ufunc->uf_dfunc_idx == def_functions.ga_len - 1)
    {
	--def_functions.ga_len;
	ufunc->uf_dfunc_idx = 0;
    }
    ufunc->uf_def_ಸ್ಥಿತಿ = UF_COMPILE_ERROR;

    ಆಗ (cctx->ctx_scope != ಖಾಲಿ)
	drop_scope(cctx);

    ಆದರೆ (errormsg != ಖಾಲಿ)
	emsg(errormsg);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (did_emsg == did_emsg_before)
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
    ಪೂರ್ಣಾಂಕ
compile_def_function(
	ufunc_T		*ufunc,
	ಪೂರ್ಣಾಂಕ		check_return_type,
	compiletype_T   compile_type,
	cctx_T		*outer_cctx)
{
    garray_T	ಸಾಲುಗಳು_ಗಮ್ಯ_free;
    ಅಕ್ಷರ	*errormsg = ಖಾಲಿ;	// error message
    cctx_T	cctx;
    garray_T	*instr;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    ಪೂರ್ಣಾಂಕ		did_emsg_silent_before = did_emsg_silent;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    sctx_T	save_current_sctx = current_sctx;
    ಪೂರ್ಣಾಂಕ		save_estack_compiling = estack_compiling;
    ಪೂರ್ಣಾಂಕ		save_cmod_flags = cmdmod.cmod_flags;
    ಪೂರ್ಣಾಂಕ		do_estack_ತಳ್ಳು;
    ಪೂರ್ಣಾಂಕ		new_def_function = FALSE;

    // allocated lines are freed at the end
    ga_init2(&ಸಾಲುಗಳು_ಗಮ್ಯ_free, ಗಾತ್ರ(char_u *), 50);

    // Initialize the ufunc and the compilation context
    ಆದರೆ (compile_dfunc_ufunc_init(ufunc, outer_cctx, compile_type,
						&new_def_function) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    compile_dfunc_cctx_init(&cctx, outer_cctx, ufunc, compile_type);

    instr = &cctx.ctx_instr;

    // Set the context to the function, it may be compiled when called from
    // another script.  Set the script version to the most modern one.
    // The line number will be set in next_line_from_context().
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;

    // Don't use the flag from ":legacy" here.
    cmdmod.cmod_flags &= ~CMOD_LEGACY;

    // Make sure error messages are OK.
    do_estack_ತಳ್ಳು = !estack_ತುದಿ_is_ufunc(ufunc, 1);
    ಆದರೆ (do_estack_ತಳ್ಳು)
	estack_ತಳ್ಳು_ufunc(ufunc, 1);
    estack_compiling = TRUE;

    // Make sure arguments don't shadow variables in the context
    ಆದರೆ (check_ವಾದಗಳು_shadowing(ufunc, &cctx) == FAIL)
	ಹೋಗು erret;

    // For an object method and a constructor generate instructions to
    // initialize "this" and the object variables.
    ಆದರೆ (ufunc->uf_flags & (FC_OBJECT|FC_NEW))
	ಆದರೆ (obj_ವಿಧಾನ_prologue(ufunc, &cctx) == FAIL)
	    ಹೋಗು erret;

    ಆದರೆ (ufunc->uf_def_ವಾದಗಳು.ga_len > 0)
	ಆದರೆ (compile_def_function_default_ವಾದಗಳು(ufunc, instr, &cctx) == FAIL)
	    ಹೋಗು erret;
    ufunc->uf_ವಾದಗಳು_visible = ufunc->uf_ವಾದಗಳು.ga_len;

    // Compiling an abstract method or a function in an interface is done to
    // get the function type.  No code is actually compiled.
    ಆದರೆ (ufunc->uf_class != ಖಾಲಿ && (IS_INTERFACE(ufunc->uf_class)
						|| IS_ABSTRACT_ವಿಧಾನ(ufunc)))
    {
	ufunc->uf_def_ಸ್ಥಿತಿ = UF_NOT_COMPILED;
	ret = OK;
	ಹೋಗು erret;
    }

    // compile the function body
    ಆದರೆ (compile_def_function_body(ufunc->uf_ಸಾಲುಗಳು.ga_len, check_return_type,
				&ಸಾಲುಗಳು_ಗಮ್ಯ_free, &errormsg, &cctx) == FAIL)
	ಹೋಗು erret;

    ಆದರೆ (compile_dfunc_scope_ಕೊನೆ_missing(&cctx))
	ಹೋಗು erret;

    ಆದರೆ (compile_dfunc_generate_default_return(ufunc, &cctx) == FAIL)
	ಹೋಗು erret;

    // When compiled with ":silent!" and there was an error don't consider the
    // function compiled.
    ಆದರೆ (emsg_silent == 0 || did_emsg_silent == did_emsg_silent_before)
	compile_dfunc_epilogue(outer_cctx, ufunc, instr, &cctx);

    ret = OK;

erret:
    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_COMPILING)
    {
	// compilation failed. do cleanup.
	compile_dfunc_ufunc_cleanup(ufunc, instr, new_def_function,
				    errormsg, did_emsg_before, &cctx);
    }

    ಆದರೆ (cctx.ctx_redir_lhs.lhs_name != ಖಾಲಿ)
    {
	ಆದರೆ (ret == OK)
	{
	    emsg(_(e_missing_redir_ಕೊನೆ));
	    ret = FAIL;
	}
	vim_free(cctx.ctx_redir_lhs.lhs_name);
	vim_free(cctx.ctx_redir_lhs.lhs_ಸಂಪೂರ್ಣ);
    }

    current_sctx = save_current_sctx;
    estack_compiling = save_estack_compiling;
    cmdmod.cmod_flags =	save_cmod_flags;
    ಆದರೆ (do_estack_ತಳ್ಳು)
	estack_ತೆಗೆ();

    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಸಾಲುಗಳು_ಗಮ್ಯ_free);
    free_locals(&cctx);
    ga_clear(&cctx.ctx_type_ರಾಶಿ);
    ಹಿಂತಿರುಗಿ ret;
}

    ಶೂನ್ಯ
ಗಣ_function_type(ufunc_T *ufunc)
{
    ಪೂರ್ಣಾಂಕ varargs = ufunc->uf_va_name != ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ argcount = ufunc->uf_ವಾದಗಳು.ga_len;

    // Create a type for the function, with the return type and any
    // argument types.
    // A vararg is included in uf_args.ga_len but not in uf_arg_types.
    // The type is included in "tt_args".
    ಆದರೆ (argcount > 0 || varargs)
    {
	ಆದರೆ (ufunc->uf_type_list.ga_itemsize == 0)
	    ga_init2(&ufunc->uf_type_list, ಗಾತ್ರ(type_T *), 10);
	ufunc->uf_func_type = alloc_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
	// Add argument types to the function type.
	ಆದರೆ (func_type_ಕೂಡಿಸು_arg_types(ufunc->uf_func_type,
				    argcount + varargs,
				    &ufunc->uf_type_list) == FAIL)
	    ಹಿಂತಿರುಗಿ;
	ufunc->uf_func_type->tt_argcount = argcount + varargs;
	ufunc->uf_func_type->tt_min_argcount =
				      argcount - ufunc->uf_def_ವಾದಗಳು.ga_len;
	ಆದರೆ (ufunc->uf_arg_types == ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ i;

	    // lambda does not have argument types.
	    ಗಾಗಿ (i = 0; i < argcount; ++i)
		ufunc->uf_func_type->tt_ವಾದಗಳು[i] = &t_any;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    mch_memmove(ufunc->uf_func_type->tt_ವಾದಗಳು,
			 ufunc->uf_arg_types, ಗಾತ್ರ(type_T *) * argcount);
	ಆದರೆ (varargs)
	{
	    ufunc->uf_func_type->tt_ವಾದಗಳು[argcount] =
		   ufunc->uf_va_type == ಖಾಲಿ ? &t_list_any : ufunc->uf_va_type;
	    ufunc->uf_func_type->tt_flags = TTFLAG_VARARGS;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// No arguments, can use a predefined type.
	ufunc->uf_func_type = get_func_type(ufunc->uf_ret_type,
					   argcount, &ufunc->uf_type_list);
}

/*
 * Free all instructions for "dfunc" except df_name.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
delete_def_function_contents(dfunc_T *dfunc, ಪೂರ್ಣಾಂಕ mark_deleted)
{
    ಪೂರ್ಣಾಂಕ idx;

    // In same cases the instructions may refer to a class in which the
    // function is defined and unreferencing the class may call back here
    // recursively.  Set the df_delete_busy to avoid problems.
    ಆದರೆ (dfunc->df_delete_busy)
	ಹಿಂತಿರುಗಿ;
    dfunc->df_delete_busy = TRUE;

    ga_clear(&dfunc->df_def_ವಾದಗಳು_isn);
    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&dfunc->df_var_ಹೆಸರುಗಳು);

    ಆದರೆ (dfunc->df_instr != ಖಾಲಿ)
    {
	ಗಾಗಿ (idx = 0; idx < dfunc->df_instr_ಎಣಿಕೆ; ++idx)
	    delete_instr(dfunc->df_instr + idx);
	VIM_CLEAR(dfunc->df_instr);
    }
    ಆದರೆ (dfunc->df_instr_debug != ಖಾಲಿ)
    {
	ಗಾಗಿ (idx = 0; idx < dfunc->df_instr_debug_ಎಣಿಕೆ; ++idx)
	    delete_instr(dfunc->df_instr_debug + idx);
	VIM_CLEAR(dfunc->df_instr_debug);
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (dfunc->df_instr_prof != ಖಾಲಿ)
    {
	ಗಾಗಿ (idx = 0; idx < dfunc->df_instr_prof_ಎಣಿಕೆ; ++idx)
	    delete_instr(dfunc->df_instr_prof + idx);
	VIM_CLEAR(dfunc->df_instr_prof);
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (mark_deleted)
	dfunc->df_deleted = TRUE;
    ಆದರೆ (dfunc->df_ufunc != ಖಾಲಿ)
	dfunc->df_ufunc->uf_def_ಸ್ಥಿತಿ = UF_NOT_COMPILED;

    dfunc->df_delete_busy = FALSE;
}

/*
 * When a user function is deleted, clear the contents of any associated def
 * function, unless another user function still uses it.
 * The position in def_functions can be re-used.
 */
    ಶೂನ್ಯ
unlink_def_function(ufunc_T *ufunc)
{
    ಆದರೆ (ufunc->uf_dfunc_idx <= 0 || def_functions.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
	+ ufunc->uf_dfunc_idx;

    ಆದರೆ (--dfunc->df_refcount <= 0)
	delete_def_function_contents(dfunc, TRUE);
    ufunc->uf_def_ಸ್ಥಿತಿ = UF_NOT_COMPILED;
    ufunc->uf_dfunc_idx = 0;
    ಆದರೆ (dfunc->df_ufunc == ufunc)
	dfunc->df_ufunc = ಖಾಲಿ;
}

/*
 * Used when a user function refers to an existing dfunc.
 */
    ಶೂನ್ಯ
link_def_function(ufunc_T *ufunc)
{
    ಆದರೆ (ufunc->uf_dfunc_idx <= 0)
	ಹಿಂತಿರುಗಿ;

    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
	+ ufunc->uf_dfunc_idx;

    ++dfunc->df_refcount;
}

#ಆದರೆ defined(EXITFREE)
/*
 * Free all functions defined with ":def".
 */
    ಶೂನ್ಯ
free_def_functions(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ idx;

    ಗಾಗಿ (idx = 0; idx < def_functions.ga_len; ++idx)
    {
	dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + idx;

	delete_def_function_contents(dfunc, TRUE);
	vim_free(dfunc->df_name);
    }

    ga_clear(&def_functions);
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
