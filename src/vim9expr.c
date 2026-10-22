/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9expr.c: Dealing with compiled function expressions
 */

#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ paren_follows_after_ಅಭಿವ್ಯಕ್ತಿ = 0;

/*
 * Generate code for any ppconst entries.
 */
    ಪೂರ್ಣಾಂಕ
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಂಕ	    i;
    ಪೂರ್ಣಾಂಕ	    ret = OK;
    ಪೂರ್ಣಾಂಕ	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    ಗಾಗಿ (i = 0; i < ppconst->pp_ಬಳಸಿದ; ++i)
	ಆದರೆ (generate_tv_ತಳ್ಳು(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_ಬಳಸಿದ = 0;
    cctx->ctx_skip = save_skip;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_ppconst_bool(ppconst_T *ppconst)
{
    ಆದರೆ (ppconst->pp_ಬಳಸಿದ > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ - 1];
	where_T	    where = WHERE_INIT;

	ಹಿಂತಿರುಗಿ check_typval_type(&t_bool, tv, where);
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    ಶೂನ್ಯ
clear_ppconst(ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < ppconst->pp_ಬಳಸಿದ; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_ಬಳಸಿದ = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_tuple_member(
    type2_T	*typep,
    ಪೂರ್ಣಾಂಕ		is_ತುಂಡು,
    cctx_T	*cctx)
{
    ಆದರೆ (is_ತುಂಡು)
    {
	ಆದರೆ (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	// a copy is made so the member type is no longer declared
	ಆದರೆ (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	ಆದರೆ (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    ಆದರೆ (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    ಆದರೆ (typep->type_curr->tt_argcount == 1)
	    {
		ಆದರೆ (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_ವಾದಗಳು[0]->tt_member;
		ಇಲ್ಲದಿದ್ದರೆ
		    typep->type_curr = typep->type_curr->tt_ವಾದಗಳು[0];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		typep->type_curr = &t_any;
	    ಆದರೆ (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		ಆದರೆ (typep->type_decl->tt_argcount == 1)
		{
		    ಆದರೆ (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_ವಾದಗಳು[0]->tt_member;
		    ಇಲ್ಲದಿದ್ದರೆ
			typep->type_decl = typep->type_decl->tt_ವಾದಗಳು[0];
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    typep->type_curr = &t_any;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		typep->type_decl = typep->type_curr;
	}
	ಆದರೆ (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    ಪೂರ್ಣಾಂಕ
compile_member(ಪೂರ್ಣಾಂಕ is_ತುಂಡು, ಪೂರ್ಣಾಂಕ *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + ರಾಶಿ->ga_len - (is_ತುಂಡು ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + ರಾಶಿ->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    ಆದರೆ ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    ಆದರೆ (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	ಆದರೆ (need_type(idxtype, &t_ಸಂಖ್ಯೆ, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (is_ತುಂಡು)
	{
	    idxtype = get_type_on_ರಾಶಿ(cctx, 1);
	    ಆದರೆ (need_type(idxtype, &t_ಸಂಖ್ಯೆ, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಆದರೆ (vartype == VAR_DICT)
    {
	ಆದರೆ (is_ತುಂಡು)
	{
	    emsg(_(e_cannot_ತುಂಡು_dictionary));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    ಆದರೆ (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    ಆದರೆ (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		ಆದರೆ (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		typep->type_decl = typep->type_curr;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	ಆದರೆ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (keeping_dict != ಖಾಲಿ)
	    *keeping_dict = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	ಆದರೆ ((is_ತುಂಡು
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype == VAR_BLOB)
    {
	ಆದರೆ (is_ತುಂಡು)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    ಆದರೆ (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    typep->type_curr = &t_ಸಂಖ್ಯೆ;
	    typep->type_decl = &t_ಸಂಖ್ಯೆ;
	    ಆದರೆ (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype == VAR_TUPLE)
    {
	ಆದರೆ (compile_tuple_member(typep, is_ತುಂಡು, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	ಆದರೆ (is_ತುಂಡು)
	{
	    ಆದರೆ (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    // a copy is made so the member type is no longer declared
	    ಆದರೆ (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    ಆದರೆ (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		ಆದರೆ (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		ಆದರೆ (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		ಆದರೆ (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    ಆದರೆ (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		ಇಲ್ಲದಿದ್ದರೆ
			typep->type_decl = typep->type_curr;
	    }
	    ಆದರೆ (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆಯ್ಕೆ (vartype)
	{
	    ಸ್ಥಿತಿ VAR_FUNC:
	    ಸ್ಥಿತಿ VAR_PARTIAL:
		emsg(_(e_cannot_ಸೂಚ್ಯಂಕ_a_funcref));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_BOOL:
	    ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	    ಸ್ಥಿತಿ VAR_CHANNEL:
	    ಸ್ಥಿತಿ VAR_INSTR:
	    ಸ್ಥಿತಿ VAR_CLASS:
	    ಸ್ಥಿತಿ VAR_OBJECT:
	    ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    ಸ್ಥಿತಿ VAR_UNKNOWN:
	    ಸ್ಥಿತಿ VAR_ANY:
	    ಸ್ಥಿತಿ VAR_VOID:
		emsg(_(e_cannot_ಸೂಚ್ಯಂಕ_special_variable));
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    ಗಾಗಿ (cctx_T *cctx = cctx_arg; cctx != ಖಾಲಿ; cctx = cctx->ctx_outer)
    {
	ಆದರೆ (cctx->ctx_ufunc != ಖಾಲಿ && cctx->ctx_ufunc->uf_class != ಖಾಲಿ)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    ಆಗ (clp != ಖಾಲಿ)
	    {
		ಆದರೆ (clp == cl)
		    ಹಿಂತಿರುಗಿ TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    ಪೂರ್ಣಾಂಕ
compile_class_object_ಸೂಚ್ಯಂಕ(cctx_T *cctx, char_u **arg, type_T *type)
{
    ಪೂರ್ಣಾಂಕ		m_idx;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;

    ಆದರೆ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    class_T *cl = type->tt_class;
    ಪೂರ್ಣಾಂಕ is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    ಆದರೆ (type == &t_super)
    {
	ಆದರೆ (cctx->ctx_ufunc == ಖಾಲಿ || cctx->ctx_ufunc->uf_class == ಖಾಲಿ)
	{
	    emsg(_(e_using_super_not_ಒಳ_class_ವಿಧಾನ));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_ರಾಶಿ.ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	ಆದರೆ (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + instr->ga_len - 1;
	    ಆದರೆ (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    ಆದರೆ (cl == ಖಾಲಿ)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_ಕೊನೆ = ಕಂಡುಹಿಡಿ_name_ಕೊನೆ(name, ಖಾಲಿ, ಖಾಲಿ, FNE_CHECK_ಆರಂಭ);
    ಆದರೆ (name_ಕೊನೆ == name)
    {
	emsg(_(e_missing_name_after_dot));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    size_t len = name_ಕೊನೆ - name;

    gfargs_tab_T    gfatab;

    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_init(&gfatab);

    ಆದರೆ (*name_ಕೊನೆ == '<')
    {
	// generic method call
	name_ಕೊನೆ = parse_generic_func_type_ವಾದಗಳು(name, len, name_ಕೊನೆ,
							&gfatab, cctx);
	ಆದರೆ (name_ಕೊನೆ == ಖಾಲಿ)
	    ಹೋಗು ಮುಗಿದ;
    }

    ಆದರೆ (*name_ಕೊನೆ == '(')
    {
	ಪೂರ್ಣಾಂಕ	function_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಂಕ	ಮಗು_ಎಣಿಕೆ;
	ufunc_T	**functions;

	ಆದರೆ (type->tt_type == VAR_CLASS)
	{
	    function_ಎಣಿಕೆ = cl->class_class_function_ಎಣಿಕೆ;
	    ಮಗು_ಎಣಿಕೆ = cl->class_class_function_ಎಣಿಕೆ_ಮಗು;
	    functions = cl->class_class_functions;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    ಆದರೆ (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_ಎಣಿಕೆ = cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ;
	    ಮಗು_ಎಣಿಕೆ = cl->class_obj_ವಿಧಾನ_ಎಣಿಕೆ_ಮಗು;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ fi;
	ಗಾಗಿ (fi = is_super ? ಮಗು_ಎಣಿಕೆ : 0; fi < function_ಎಣಿಕೆ; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    ಆದರೆ (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		ನಿಲ್ಲಿಸು;
	    }
	}

	ocmember_T  *ocm = ಖಾಲಿ;
	ಆದರೆ (ufunc == ಖಾಲಿ)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    ಆದರೆ (ocm == ಖಾಲಿ || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		ವಿಧಾನ_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, type->tt_type, name, len);
		ಹೋಗು ಮುಗಿದ;
	    }
	    ಆದರೆ (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_ರಾಶಿ.ga_len;
		ಆದರೆ (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    ಹೋಗು ಮುಗಿದ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;

		ಆದರೆ (IS_INTERFACE(cl))
		    ಸ್ಥಿತಿ_2 = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		ಇಲ್ಲದಿದ್ದರೆ
		    ಸ್ಥಿತಿ_2 = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
		    ಹೋಗು ಮುಗಿದ;
	    }
	}

	ಆದರೆ (is_super && ufunc != ಖಾಲಿ && IS_ABSTRACT_ವಿಧಾನ(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_ವಿಧಾನ_str_ನೇರ), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    ಹೋಗು ಮುಗಿದ;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	ಆದರೆ (ocm == ಖಾಲಿ && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_ವಿಧಾನ_str), name);
	    ಹೋಗು ಮುಗಿದ;
	}

	// process generic function call
	ಆದರೆ (ufunc != ಖಾಲಿ)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ಆದರೆ (ufunc == ಖಾಲಿ)
		ಹೋಗು ಮುಗಿದ;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_ಕೊನೆ + 1);
	ಪೂರ್ಣಾಂಕ argcount = 0;
	ಆದರೆ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    ಹೋಗು ಮುಗಿದ;

	ಆದರೆ (ocm != ಖಾಲಿ)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    ಹೋಗು ಮುಗಿದ;
	}
	ಆದರೆ (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	ಇಲ್ಲದಿದ್ದರೆ
	    ret = generate_CALL(cctx, ufunc, ಖಾಲಿ, 0, argcount, FALSE);
	ಹೋಗು ಮುಗಿದ;
    }

    ಆದರೆ (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	ಆದರೆ (m_idx >= 0)
	{
	    ಆದರೆ (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		ಹೋಗು ಮುಗಿದ;
	    }

	    *arg = name_ಕೊನೆ;
	    ಆದರೆ (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    ಇಲ್ಲದಿದ್ದರೆ
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    ಹೋಗು ಮುಗಿದ;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_ವಿಧಾನ_idx(cl, name, len);
	ಆದರೆ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    ಆದರೆ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_ವಿಧಾನ_str), fp->uf_name);
		ಹೋಗು ಮುಗಿದ;
	    }
	    *arg = name_ಕೊನೆ;
	    // Remove the object type from the stack
	    --cctx->ctx_type_ರಾಶಿ.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, ಖಾಲಿ);
	    ಹೋಗು ಮುಗಿದ;
	}

	member_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, VAR_OBJECT, name, len);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// load class member
	ಪೂರ್ಣಾಂಕ idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	ಆದರೆ (m != ಖಾಲಿ)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    ಆದರೆ (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		ಹೋಗು ಮುಗಿದ;
	    }

	    *arg = name_ಕೊನೆ;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ರಾಶಿ.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    ಹೋಗು ಮುಗಿದ;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_ವಿಧಾನ_idx(cl, name, len);
	ಆದರೆ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    ಆದರೆ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_ವಿಧಾನ_str), fp->uf_name);
		ಹೋಗು ಮುಗಿದ;
	    }
	    *arg = name_ಕೊನೆ;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ರಾಶಿ.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, ಖಾಲಿ);
	    ಹೋಗು ಮುಗಿದ;
	}

	member_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, VAR_CLASS, name, len);
    }

ಮುಗಿದ:
    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_clear(&gfatab);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    ಪೂರ್ಣಾಂಕ
compile_ತುಂಬು_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *ಆರಂಭ,	    // start of variable
	char_u **ಕೊನೆ,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    ಪೂರ್ಣಾಂಕ		    idx;
    imported_T	    *imp;

    ಆದರೆ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಹಿಂತಿರುಗಿ FAIL;
    si = SCRIPT_ಅಂಶ(current_sctx.sc_sid);
    idx = get_script_ಅಂಶ_idx(current_sctx.sc_sid, name, 0, cctx, ಖಾಲಿ);
    ಆದರೆ (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (ಕೊನೆ == ಖಾಲಿ)
	imp = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (import == ಖಾಲಿ)
	imp = ಕಂಡುಹಿಡಿ_imported(name, 0, FALSE);
    ಇಲ್ಲದಿದ್ದರೆ
	imp = import;

    ಆದರೆ (imp != ಖಾಲಿ)
    {
	char_u	*p = skipwhite(*ಕೊನೆ);
	char_u	*exp_name;
	ಪೂರ್ಣಾಂಕ	cc;
	ufunc_T	*ufunc = ಖಾಲಿ;
	type_T	*type;
	ಪೂರ್ಣಾಂಕ	ಮುಗಿದ = FALSE;
	ಪೂರ್ಣಾಂಕ	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	ಆದರೆ (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), ಆರಂಭ);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	++p;
	ಆದರೆ (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// isolate one name
	exp_name = p;
	ಆಗ (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ಅಂಶ(imp->imp_sid);
	ಆದರೆ (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_ಮೂಲ(cctx, imp->imp_sid);

	ಆದರೆ (res == OK)
	{
	    ಆದರೆ (si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ != ಖಾಲಿ
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		ಆದರೆ (cc == '(' || paren_follows_after_ಅಭಿವ್ಯಕ್ತಿ)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		ಇಲ್ಲದಿದ್ದರೆ
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		ಮುಗಿದ = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		ಆದರೆ (cc == '(' || paren_follows_after_ಅಭಿವ್ಯಕ್ತಿ)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		ಮುಗಿದ = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		idx = ಕಂಡುಹಿಡಿ_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, ಖಾಲಿ, TRUE);
	    }
	}

	*p = cc;
	*ಕೊನೆ = p;
	ಆದರೆ (ಮುಗಿದ)
	    ಹಿಂತಿರುಗಿ res;

	ಆದರೆ (idx < 0)
	{
	    ಆದರೆ (ufunc != ಖಾಲಿ)
	    {
		gfargs_tab_T	gfatab;

		generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_init(&gfatab);

		ಆದರೆ (IS_GENERIC_FUNC(ufunc))
		{
		    ಆದರೆ (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_ವಾದಗಳು(name, STRLEN(name), p,
								&gfatab, cctx);
			ಆದರೆ (p != ಖಾಲಿ)
			{
			    *ಕೊನೆ = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_clear(&gfatab);

			ಆದರೆ (p == ಖಾಲಿ || ufunc == ಖಾಲಿ)
			    ಹಿಂತಿರುಗಿ FAIL;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			emsg_funcname(e_generic_func_missing_type_ವಾದಗಳು_str,
									name);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    ಹಿಂತಿರುಗಿ FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, ಖಾಲಿ, TRUE);
		ಹಿಂತಿರುಗಿ OK;
	    }
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	ಹಿಂತಿರುಗಿ OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    ಹಿಂತಿರುಗಿ generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    ಪೂರ್ಣಾಂಕ			has_g_ಪೂರ್ವಪ್ರತ್ಯಯ)
{
    ufunc_T *ufunc = ಕಂಡುಹಿಡಿ_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    ಆದರೆ (ufunc == ಖಾಲಿ || (!has_g_ಪೂರ್ವಪ್ರತ್ಯಯ && func_requires_g_ಪೂರ್ವಪ್ರತ್ಯಯ(ufunc)))
	ಹಿಂತಿರುಗಿ FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    ಆದರೆ (ufunc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    ಆದರೆ (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, ಖಾಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compiling_a_class_ವಿಧಾನ(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    ಹಿಂತಿರುಗಿ cctx->ctx_ufunc != ಖಾಲಿ
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == ಖಾಲಿ
		|| cctx->ctx_outer->ctx_ufunc == ಖಾಲಿ
		|| cctx->ctx_outer->ctx_ufunc->uf_class == ಖಾಲಿ
		|| (cctx->ctx_outer->ctx_ufunc->uf_flags
		    & (FC_OBJECT|FC_NEW)) == 0);
}

/*
 * Generate a closure for the nested generic function "ufunc" with the type
 * arguments in "gfatab".  The function is compiled in the context of the
 * function where it is defined, which is "outer_depth" levels up from the
 * function being compiled: zero when it is defined in the current function,
 * non-zero when used in a closure or in a recursive call.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    ಪೂರ್ಣಾಂಕ			outer_ಆಳ)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    ಪೂರ್ಣಾಂಕ		isn_idx;

    ಆದರೆ (fp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಆಳ = 0; ಆಳ < outer_ಆಳ; ++ಆಳ)
    {
	ಆದರೆ (outer_cctx->ctx_outer == ಖಾಲಿ)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    ಆದರೆ (fp->uf_ಖಂಡ_ids == ಖಾಲಿ && ufunc->uf_ಖಂಡ_ಆಳ > 0)
    {
	fp->uf_ಖಂಡ_ids = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, ufunc->uf_ಖಂಡ_ಆಳ);
	ಆದರೆ (fp->uf_ಖಂಡ_ids != ಖಾಲಿ)
	{
	    mch_memmove(fp->uf_ಖಂಡ_ids, ufunc->uf_ಖಂಡ_ids,
					    ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * ufunc->uf_ಖಂಡ_ಆಳ);
	    fp->uf_ಖಂಡ_ಆಳ = ufunc->uf_ಖಂಡ_ಆಳ;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    ಆದರೆ (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (generate_FUNCREF(cctx, fp, ಖಾಲಿ, FALSE, 0, &isn_idx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	ಹಿಂತಿರುಗಿ OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    ಆದರೆ (outer_ಆಳ > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_ದತ್ತಾಂಶ) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	ಆದರೆ (extra == ಖಾಲಿ)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    ಆದರೆ (extra == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_ಆಳ = outer_ಆಳ;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    ಪೂರ್ಣಾಂಕ
compile_ತುಂಬು(
	char_u **arg,
	size_t  namelen,
	char_u *ಕೊನೆ_arg,
	cctx_T	*cctx,
	ಪೂರ್ಣಾಂಕ	is_ಅಭಿವ್ಯಕ್ತಿ,
	ಪೂರ್ಣಾಂಕ	error)
{
    type_T	*type;
    char_u	*name = ಖಾಲಿ;
    char_u	*ಕೊನೆ = ಕೊನೆ_arg;
    ಪೂರ್ಣಾಂಕ		res = FAIL;
    ಪೂರ್ಣಾಂಕ		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = ಖಾಲಿ;

    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_init(&gfatab);

    ಆದರೆ (*(*arg + namelen) == '<')
    {
	// generic function call
	ಆದರೆ (parse_generic_func_type_ವಾದಗಳು(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == ಖಾಲಿ)
	{
	    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_clear(&gfatab);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    ಆದರೆ (*(*arg + 1) == ':')
    {
	ಆದರೆ (ಕೊನೆ <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    ಆಯ್ಕೆ (**arg)
	    {
		ಸ್ಥಿತಿ 'g': isn_type = ISN_LOADGDICT; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'w': isn_type = ISN_LOADWDICT; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 't': isn_type = ISN_LOADTDICT; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'b': isn_type = ISN_LOADBDICT; ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    ಹೋಗು theend;
	    }
	    ಆದರೆ (generate_instr_type(cctx, isn_type, &t_dict_any) == ಖಾಲಿ)
		ಹೋಗು theend;
	    res = OK;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, ಕೊನೆ - (*arg + 2));
	    ಆದರೆ (name == ಖಾಲಿ)
		ಹೋಗು theend;

	    ಆಯ್ಕೆ (**arg)
	    {
		ಸ್ಥಿತಿ 'v': res = generate_LOADV(cctx, name);
			  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 's': ಆದರೆ (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_ಒಳ_vim9_script_str),
									 *arg);
			      ಹೋಗು theend;
			  }
			  ಆದರೆ (is_ಅಭಿವ್ಯಕ್ತಿ && ಕಂಡುಹಿಡಿ_func(name, FALSE) != ಖಾಲಿ)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  ಇಲ್ಲದಿದ್ದರೆ
			      res = compile_ತುಂಬು_scriptvar(cctx, name,
							    ಖಾಲಿ, &ಕೊನೆ, ಖಾಲಿ);
			  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'g': ಆದರೆ (vim_strchr(name, AUTOLOAD_CHAR) == ಖಾಲಿ)
			  {
			      ಆದರೆ (is_ಅಭಿವ್ಯಕ್ತಿ && ASCII_ISUPPER(*name)
				       && (ಕಂಡುಹಿಡಿ_func(name, TRUE) != ಖಾಲಿ
					   || gfatab.gfat_ವಾದಗಳು.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      ಇಲ್ಲದಿದ್ದರೆ
				  isn_type = ISN_LOADG;
			  }
			  ಇಲ್ಲದಿದ್ದರೆ
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, ಕೊನೆ - *arg);
			      ಆದರೆ (name == ಖಾಲಿ)
				  ಹೋಗು theend;
			  }
			  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'w': isn_type = ISN_LOADW; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 't': isn_type = ISN_LOADT; ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 'b': isn_type = ISN_LOADB; ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  ಹೋಗು theend;
	    }
	    ಆದರೆ (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_ತುಂಬು(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಪೂರ್ಣಾಂಕ	    idx;
	ಪೂರ್ಣಾಂಕ	    ವಿಧಾನ_idx;
	ಪೂರ್ಣಾಂಕ	    gen_ತುಂಬು = FALSE;
	ಪೂರ್ಣಾಂಕ	    gen_ತುಂಬು_outer = 0;
	ಪೂರ್ಣಾಂಕ	    outer_loop_ಆಳ = -1;
	ಪೂರ್ಣಾಂಕ	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	ಆದರೆ (name == ಖಾಲಿ)
	    ಹೋಗು theend;

	ಆದರೆ (STRCMP(name, "super") == 0 && compiling_a_class_ವಿಧಾನ(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_super);
	    ಆದರೆ (*ಕೊನೆ != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr(name, AUTOLOAD_CHAR) != ಖಾಲಿ)
	{
	    script_autoload(name, FALSE);
	    res = generate_ತುಂಬು(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg_exists(*arg, namelen, &idx, &type, &gen_ತುಂಬು_outer, cctx)
									 == OK)
	{
	    ಆದರೆ (gen_ತುಂಬು_outer == 0)
		gen_ತುಂಬು = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    lvar_T  lvar;
	    class_T *cl = ಖಾಲಿ;

	    ಆದರೆ (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		ಆದರೆ (lvar.lv_ufunc != ಖಾಲಿ && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_ವಾದಗಳು.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lvar.lv_from_outer != 0)
		{
		    gen_ತುಂಬು_outer = lvar.lv_from_outer;
		    outer_loop_ಆಳ = lvar.lv_loop_ಆಳ;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    gen_ತುಂಬು = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (cctx->ctx_ufunc->uf_defclass != ಖಾಲಿ &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((ವಿಧಾನ_idx =
			     cctx_class_ವಿಧಾನ_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		ಆದರೆ (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    ಆದರೆ (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ufunc_T *fp = cl->class_class_functions[ವಿಧಾನ_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, ವಿಧಾನ_idx,
									ಖಾಲಿ);
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		imported_T *imp = ಖಾಲಿ;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		ಆದರೆ (script_var_exists(*arg, namelen, cctx, ಖಾಲಿ) == OK
			    || (imp = ಕಂಡುಹಿಡಿ_imported(name, 0, FALSE)) != ಖಾಲಿ)
		    res = compile_ತುಂಬು_scriptvar(cctx, name, *arg, &ಕೊನೆ, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		ಆದರೆ (res == FAIL && is_ಅಭಿವ್ಯಕ್ತಿ && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	ಆದರೆ (gen_ತುಂಬು)
	    res = generate_ತುಂಬು(cctx, ISN_ತುಂಬು, idx, ಖಾಲಿ, type);
	ಆದರೆ (gen_ತುಂಬು_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_ತುಂಬು_outer,
				       outer_loop_ಆಳ, outer_loop_idx, type);
	    cctx->ctx_outer_ಬಳಸಿದ = TRUE;
	}
    }

    *arg = ಕೊನೆ;

theend:
    ಆದರೆ (lt != ಖಾಲಿ)
	*lt = '<';
    ಆದರೆ (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_ಸಿಕ್ಕಿತು_str), name);
    vim_free(name);
    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_clear(&gfatab);
    ಹಿಂತಿರುಗಿ res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_string(isn_T *isn, cctx_T *cctx, ಪೂರ್ಣಾಂಕ str_ಸ್ಥಾನಾಂತರ)
{
    char_u	*s = isn->isn_arg.string + str_ಸ್ಥಾನಾಂತರ;
    garray_T	save_ga = cctx->ctx_instr;
    ಪೂರ್ಣಾಂಕ		ಅಭಿವ್ಯಕ್ತಿ_res;
    ಪೂರ್ಣಾಂಕ		trailing_error;
    ಪೂರ್ಣಾಂಕ		instr_ಎಣಿಕೆ;
    isn_T	*instr = ಖಾಲಿ;

    // Remove the string type from the stack.
    --cctx->ctx_type_ರಾಶಿ.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_ದತ್ತಾಂಶ = ಖಾಲಿ;

    // avoid peeking a next line
    ಪೂರ್ಣಾಂಕ galen_save = cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len;
    cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len = 0;

    ಅಭಿವ್ಯಕ್ತಿ_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    ಆದರೆ (ಅಭಿವ್ಯಕ್ತಿ_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	ಆದರೆ (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_ರಾಶಿ.ga_len;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_ಎಣಿಕೆ = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_ದತ್ತಾಂಶ;
    instr[instr_ಎಣಿಕೆ].isn_type = ISN_ಮುಕ್ತಾಯ;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    ಪೂರ್ಣಾಂಕ
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	ಪೂರ್ಣಾಂಕ	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    ಪೂರ್ಣಾಂಕ	    must_ಕೊನೆ = FALSE;
    ಪೂರ್ಣಾಂಕ	    instr_ಎಣಿಕೆ;

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು(whitep, &p, cctx) == FAIL)
	    ಹೋಗು failret;
	ಆದರೆ (*p == ')')
	{
	    *arg = p + 1;
	    ಹಿಂತಿರುಗಿ OK;
	}
	ಆದರೆ (must_ಕೊನೆ)
	{
	    semsg(_(e_missing_comma_before_ವಾದ_str), p);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	instr_ಎಣಿಕೆ = cctx->ctx_instr.ga_len;
	ಆದರೆ (compile_expr0(&p, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	++*argcount;

	ಆದರೆ (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_ಎಣಿಕೆ + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_ದತ್ತಾಂಶ) + instr_ಎಣಿಕೆ;

	    // {skip} argument of searchpair() can be compiled if not empty
	    ಆದರೆ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ಖಾಲಿ
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_ಎಣಿಕೆ + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_ದತ್ತಾಂಶ) + instr_ಎಣಿಕೆ;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    ಆದರೆ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ಖಾಲಿ
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	ಆದರೆ (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (*p == ',')
	{
	    ++p;
	    ಆದರೆ (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    must_ಕೊನೆ = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_builtin_ವಿಧಾನ_call(cctx_T *cctx, class_builtin_T builtin_ವಿಧಾನ)
{
    type_T	*type = get_decl_type_on_ರಾಶಿ(cctx, 0);
    ಪೂರ್ಣಾಂಕ		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    ಆದರೆ (type->tt_type == VAR_OBJECT)
    {
	ಪೂರ್ಣಾಂಕ	ವಿಧಾನ_idx;
	ufunc_T *uf = class_get_builtin_ವಿಧಾನ(type->tt_class, builtin_ವಿಧಾನ,
							&ವಿಧಾನ_idx);
	ಆದರೆ (uf != ಖಾಲಿ)
	    res = generate_CALL(cctx, uf, type->tt_class, ವಿಧಾನ_idx, 0, FALSE);
    }

    ಹಿಂತಿರುಗಿ res;
}


/*
 * Compile a function call:  name(arg1, arg2)
 * "arg" points to "name", "arg + varlen" to the "(".
 * "argcount_init" is 1 for "value->method()"
 * Instructions:
 *	EVAL arg1
 *	EVAL arg2
 *	BCALL / DCALL / UCALL
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	ಪೂರ್ಣಾಂಕ	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ಖಾಲಿ;
    char_u	*gname = ಖಾಲಿ;
    ufunc_T	*ufunc = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		res = FAIL;
    ಪೂರ್ಣಾಂಕ		is_autoload;
    ಪೂರ್ಣಾಂಕ		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    ಆದರೆ (varlen >= ಗಾತ್ರ(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = ಕಂಡುಹಿಡಿ_imported(name, varlen, FALSE);
    ಆದರೆ (import != ಖಾಲಿ)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    ಆದರೆ ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	ಪೂರ್ಣಾಂಕ	    is_has = **arg == 'h';
	ಪೂರ್ಣಾಂಕ	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	ಆದರೆ (*s == '"')
	    (ಶೂನ್ಯ)eval_string(&s, &argvars[0], TRUE, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*s == '\'')
	    (ಶೂನ್ಯ)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	ಆದರೆ (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_ಸಂಖ್ಯೆ;
		tv->vval.v_ಸಂಖ್ಯೆ = 0;
		ಆದರೆ (is_has)
		    f_has(argvars, tv);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_len)
		    f_len(argvars, tv);
		ಇಲ್ಲದಿದ್ದರೆ
		    f_exists(argvars, tv);
		++ppconst->pp_ಬಳಸಿದ;
	    }
	    clear_tv(&argvars[0]);
	    ಹಿಂತಿರುಗಿ OK;
	}
	clear_tv(&argvars[0]);
	ಆದರೆ (!is_has && !is_len)
	{
	    emsg(_(e_ವಾದ_ದ_exists_compiled_must_be_literal_string));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    ಆದರೆ ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    ಇಲ್ಲದಿದ್ದರೆ
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_init(&gfatab);

    ಆದರೆ (*(*arg + varlen) == '<')
    {
	char_u	*ಆರಂಭ = *arg;

	// generic function
	*arg = parse_generic_func_type_ವಾದಗಳು(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	ಆದರೆ (*arg == ಖಾಲಿ)
	    ಹೋಗು theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(ಆರಂಭ, *arg - ಆರಂಭ);
	ಆದರೆ (gname == ಖಾಲಿ)
	    ಹೋಗು theend;
	++*arg;			// skip '('
    }
    ಇಲ್ಲದಿದ್ದರೆ
	*arg += varlen + 1;

    ಆದರೆ (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	ಹೋಗು theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != ಖಾಲಿ;
    ಆದರೆ (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	ಪೂರ್ಣಾಂಕ	    idx;

	// builtin function
	idx = ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func(name);
	ಆದರೆ (idx >= 0)
	{
	    ಆದರೆ (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_ಒಳ_vim9_script));
		ಹೋಗು theend;
	    }

	    ಆದರೆ (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_ರಾಶಿ(cctx, 1);
		ಆದರೆ (check_type_is_ಮೌಲ್ಯ(get_type_on_ರಾಶಿ(cctx, 0)) == FAIL)
		    ಹೋಗು theend;

		// add() can be compiled to instructions if we know the type
		ಆದರೆ (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    ಆದರೆ ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		ಆದರೆ (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_ವಿಧಾನ = CLASS_BUILTIN_INVALID;
	    ಆದರೆ (STRCMP(name, "string") == 0)
		builtin_ವಿಧಾನ = CLASS_BUILTIN_STRING;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "empty") == 0)
		builtin_ವಿಧಾನ = CLASS_BUILTIN_ಖಾಲಿಗಳು;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(name, "len") == 0)
		builtin_ವಿಧಾನ = CLASS_BUILTIN_LEN;
	    ಆದರೆ (builtin_ವಿಧಾನ != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_ವಿಧಾನ_call(cctx, builtin_ವಿಧಾನ);
		ಆದರೆ (res == OK)
		    idx = -1;
	    }

	    ಆದರೆ (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    emsg_funcname(e_unknown_function_str, namebuf);
	ಹೋಗು theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    ಆದರೆ (lookup_local(namebuf, varlen, ಖಾಲಿ, cctx) == FAIL
	    && arg_exists(namebuf, varlen, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, cctx) != OK)
    {
	class_T		*cl = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = ಕಂಡುಹಿಡಿ_func(name, FALSE);
	ಆದರೆ (ufunc != ಖಾಲಿ)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ಆದರೆ (ufunc == ಖಾಲಿ)
		ಹೋಗು theend;

	    ಆದರೆ (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, ಖಾಲಿ, 0, argcount, FALSE);
		ಹೋಗು theend;
	    }
	    ಆದರೆ (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ಖಾಲಿ
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		ಹೋಗು theend;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((mi = cctx_class_ವಿಧಾನ_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    ಆದರೆ (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], ಖಾಲಿ,
							0, argcount, FALSE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		semsg(_(e_class_ವಿಧಾನ_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    ಹೋಗು theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == ಖಾಲಿ && gname != ಖಾಲಿ ? gname : namebuf;
    ಆದರೆ (!has_g_namespace && !is_autoload
	    && compile_ತುಂಬು(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_ರಾಶಿ(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	ಹೋಗು theend;
    }

    // If we can find a global function by name generate the right call.
    ಆದರೆ (ufunc != ಖಾಲಿ)
    {
	res = generate_CALL(cctx, ufunc, ಖಾಲಿ, 0, argcount, FALSE);
	ಹೋಗು theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    ಆದರೆ (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    ಇಲ್ಲದಿದ್ದರೆ
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_ವಾದಗಳು_ಕೋಷ್ಟಕ_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    ಹಿಂತಿರುಗಿ res;
}

// like NAMESPACE_CHAR but with 'a' and 'l'.
#ವ್ಯಾಖ್ಯಾನಿಸು VIM9_NAMESPACE_CHAR	(char_u *)"bgstvw"

/*
 * Find the end of a variable or function name.  Unlike find_name_end() this
 * does not recognize magic braces.
 * When "use_namespace" is TRUE recognize "b:", "s:", etc.
 * Return a pointer to just after the name.  Equal to "arg" if there is no
 * valid name.
 */
    char_u *
ಗಮ್ಯ_name_ಕೊನೆ(char_u *arg, ಪೂರ್ಣಾಂಕ use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    ಆದರೆ (!eval_isnamec1(*arg))
	ಹಿಂತಿರುಗಿ arg;

    ಗಾಗಿ (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	ಆದರೆ (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == ಖಾಲಿ))
	    ನಿಲ್ಲಿಸು;
    ಹಿಂತಿರುಗಿ p;
}

/*
 * Like to_name_end() but also skip over a list or dict constant.
 * Also accept "<SNR>123_Func".
 * This intentionally does not handle line continuation.
 */
    char_u *
ಗಮ್ಯ_name_const_ಕೊನೆ(char_u *arg)
{
    char_u	*p = arg;
    typval_T	rettv;

    ಆದರೆ (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = ಗಮ್ಯ_name_ಕೊನೆ(p, TRUE);
    ಆದರೆ (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	ಆದರೆ (eval_list(&p, &rettv, ಖಾಲಿ, FALSE) == FAIL)
	    p = arg;
    }
    ಹಿಂತಿರುಗಿ p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		is_const;
    ಪೂರ್ಣಾಂಕ		is_all_const = TRUE;	// reset when non-const encountered
    ಪೂರ್ಣಾಂಕ		must_ಕೊನೆ = FALSE;

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_ಕೊನೆ_ದ_list_rsb_str), *arg);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (*p == ']')
	{
	    ++p;
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (must_ಕೊನೆ)
	{
	    semsg(_(e_missing_comma_ಒಳ_list_str), p);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (!is_const)
	    is_all_const = FALSE;
	++ಎಣಿಕೆ_2;
	ಆದರೆ (*p == ',')
	{
	    ++p;
	    ಆದರೆ (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    must_ಕೊನೆ = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ಹಿಂತಿರುಗಿ generate_NEWLIST(cctx, ಎಣಿಕೆ_2, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    ಪೂರ್ಣಾಂಕ ಮೊದಲ_ಅಂಶ_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    ಪೂರ್ಣಾಂಕ		is_const;
    ಪೂರ್ಣಾಂಕ		is_all_const = TRUE;	// reset when non-const encountered
    ಪೂರ್ಣಾಂಕ		must_ಕೊನೆ = FALSE;

    ಆದರೆ (**arg != ')')
    {
	ಆದರೆ (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಎಣಿಕೆ_2 = 1;	// the first tuple item is already processed
	is_all_const = ಮೊದಲ_ಅಂಶ_const;
	ಗಾಗಿ (;;)
	{
	    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_ಕೊನೆ_ದ_tuple_rsp_str), *arg);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (*p == ')')
	    {
		++p;
		ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (must_ಕೊನೆ)
	    {
		semsg(_(e_missing_comma_ಒಳ_tuple_str), p);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (!is_const)
		is_all_const = FALSE;
	    ++ಎಣಿಕೆ_2;
	    ಆದರೆ (*p == ',')
	    {
		++p;
		ಆದರೆ (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		must_ಕೊನೆ = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ಹಿಂತಿರುಗಿ generate_NEWTUPLE(cctx, ಎಣಿಕೆ_2, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*ಸಾಲು;
    size_t	off;

    ಆದರೆ (!evalarg->eval_using_cmdline || cctx == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    gap = &evalarg->eval_tofree_ga;
    ಆದರೆ (gap->ga_len == 0)
	ಹಿಂತಿರುಗಿ;

    off = *arg - ((char_u **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len - 1];
    ಸಾಲು = ((char_u **)cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_ದತ್ತಾಂಶ)[cctx->ctx_lnum];
    *arg = ಸಾಲು + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    ಪೂರ್ಣಾಂಕ
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಂಕ		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    ಆದರೆ (r != OK)
    {
	clear_evalarg(&evalarg, ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ r;
    }

    ಆದರೆ (ppconst != ಖಾಲಿ && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, ಖಾಲಿ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    ಆದರೆ (cctx->ctx_ufunc != ಖಾಲಿ)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	ಪೂರ್ಣಾಂಕ ಖಂಡ_ಆಳ = cctx->ctx_ufunc->uf_ಖಂಡ_ಆಳ;

	ಆದರೆ (ಖಂಡ_ಆಳ > 0)
	{
	    ufunc->uf_ಖಂಡ_ids = ALLOC_MULT(ಪೂರ್ಣಾಂಕ, ಖಂಡ_ಆಳ);
	    ಆದರೆ (ufunc->uf_ಖಂಡ_ids != ಖಾಲಿ)
	    {
		mch_memmove(ufunc->uf_ಖಂಡ_ids, cctx->ctx_ufunc->uf_ಖಂಡ_ids,
						    ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ) * ಖಂಡ_ಆಳ);
		ufunc->uf_ಖಂಡ_ಆಳ = ಖಂಡ_ಆಳ;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ಆದರೆ (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    ಆದರೆ (cctx->ctx_compile_type == CT_DEBUG
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#ಆದರೆ_ಅಂತ್ಯ
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    ಆದರೆ (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	ಆದರೆ (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, ಖಾಲಿ);

    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_COMPILED)
    {
	// The return type will now be known.
	ಗಣ_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	ಹಿಂತಿರುಗಿ generate_FUNCREF(cctx, ufunc, ಖಾಲಿ, FALSE, 0, ಖಾಲಿ);
    }

    func_ptr_unref(ufunc);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    ಪೂರ್ಣಾಂಕ
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	ಪೂರ್ಣಾಂಕ	    types_optional,
	evalarg_T   *evalarg)
{
    ಪೂರ್ಣಾಂಕ		r;
    ufunc_T	*ufunc;
    ಪೂರ್ಣಾಂಕ		save_sc_ಆವೃತ್ತಿ = current_sctx.sc_ಆವೃತ್ತಿ;

    // Get the funcref in "rettv".
    current_sctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, ಖಾಲಿ);
    current_sctx.sc_ಆವೃತ್ತಿ = save_sc_ಆವೃತ್ತಿ;
    ಆದರೆ (r != OK)
	ಹಿಂತಿರುಗಿ r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ಆದರೆ (ufunc->uf_ret_type == ಖಾಲಿ || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, ಖಾಲಿ);

    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_COMPILED)
    {
	// The return type will now be known.
	ಗಣ_function_type(ufunc);
	ಹಿಂತಿರುಗಿ OK;
    }
    clear_tv(rettv);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*ಅಂಶ;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		is_const;
    ಪೂರ್ಣಾಂಕ		is_all_const = TRUE;	// reset when non-const encountered

    ಆದರೆ (d == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಗಾಗಿ (;;)
    {
	char_u	    *ಕೀಲಿ = ಖಾಲಿ;

	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು(whitep, arg, cctx) == FAIL)
	{
	    *arg = ಖಾಲಿ;
	    ಹೋಗು failret;
	}

	ಆದರೆ (**arg == '}')
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    ಆದರೆ (compile_expr0(arg, cctx) == FAIL)
		ಹೋಗು failret;
	    isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + instr->ga_len - 1;
	    ಆದರೆ (isn->isn_type == ISN_PUSHNR)
	    {
		ಅಕ್ಷರ buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.ಸಂಖ್ಯೆ);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    ಆದರೆ (isn->isn_type == ISN_PUSHS)
		ಕೀಲಿ = isn->isn_arg.string;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		ಹೋಗು failret;
	    *arg = skipwhite(*arg);
	    ಆದರೆ (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_ಕೀಲಿ));
		ಹೋಗು failret;
	    }
	    ++*arg;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    ಕೀಲಿ = get_literal_ಕೀಲಿ(arg);
	    ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
		ಹೋಗು failret;
	    ಆದರೆ (generate_PUSHS(cctx, &ಕೀಲಿ) == FAIL)
		ಹೋಗು failret;
	}

	// Check for duplicate keys, if using string keys.
	ಆದರೆ (ಕೀಲಿ != ಖಾಲಿ)
	{
	    ಅಂಶ = dict_ಕಂಡುಹಿಡಿ(d, ಕೀಲಿ, -1);
	    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	    {
		semsg(_(e_duplicate_ಕೀಲಿ_ಒಳ_dictionary_str), ಕೀಲಿ);
		ಹೋಗು failret;
	    }
	    ಅಂಶ = dictitem_alloc(ಕೀಲಿ);
	    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	    {
		ಅಂಶ->di_tv.v_type = VAR_UNKNOWN;
		ಅಂಶ->di_tv.v_ಬೀಗ = 0;
		ಆದರೆ (dict_ಕೂಡಿಸು(d, ಅಂಶ) == FAIL)
		    dictitem_free(ಅಂಶ);
	    }
	}

	ಆದರೆ (**arg != ':')
	{
	    ಆದರೆ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_missing_colon_ಒಳ_dictionary_str), *arg);
	    ಹೋಗು failret;
	}
	whitep = *arg + 1;
	ಆದರೆ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    ಹೋಗು failret;
	}

	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು(whitep, arg, cctx) == FAIL)
	{
	    *arg = ಖಾಲಿ;
	    ಹೋಗು failret;
	}

	ಆದರೆ (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    ಹೋಗು failret;
	ಆದರೆ (!is_const)
	    is_all_const = FALSE;
	++ಎಣಿಕೆ_2;

	whitep = *arg;
	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು(whitep, arg, cctx) == FAIL)
	{
	    *arg = ಖಾಲಿ;
	    ಹೋಗು failret;
	}
	ಆದರೆ (**arg == '}')
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (**arg != ',')
	{
	    semsg(_(e_missing_comma_ಒಳ_dictionary_str), *arg);
	    ಹೋಗು failret;
	}
	ಆದರೆ (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    ಹೋಗು failret;
	}
	whitep = *arg + 1;
	ಆದರೆ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    ಹೋಗು failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    ಆದರೆ (VIM_ISWHITE(**arg) && vim9_comment_ಆರಂಭ(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    ಹಿಂತಿರುಗಿ generate_NEWDICT(cctx, ಎಣಿಕೆ_2, FALSE);

failret:
    ಆದರೆ (*arg == ಖಾಲಿ)
    {
	semsg(_(e_missing_dict_ಕೊನೆ_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Compile "&option".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*ಆರಂಭ = *arg;
    ಪೂರ್ಣಾಂಕ		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    ಆದರೆ (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(ಆರಂಭ, *arg - ಆರಂಭ);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_ಸಂಖ್ಯೆ ? &t_ಸಂಖ್ಯೆ : &t_string;

	ret = generate_ತುಂಬು(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Compile "$VAR".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*ಆರಂಭ = *arg;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    ಆದರೆ (len == 0)
    {
	semsg(_(e_syntax_error_at_str), ಆರಂಭ);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(ಆರಂಭ, len + 1);
    ret = generate_ತುಂಬು(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Compile $"string" or $'string'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    ಪೂರ್ಣಾಂಕ		ret;
    ಪೂರ್ಣಾಂಕ		quote;
    ಪೂರ್ಣಾಂಕ		evaluate = cctx->ctx_skip != SKIP_YES;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ಗಾಗಿ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ಆದರೆ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ಇಲ್ಲದಿದ್ದರೆ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ಆದರೆ (ret == FAIL)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (evaluate)
	{
	    ಆದರೆ ((tv.vval.v_string != ಖಾಲಿ && *tv.vval.v_string != NUL)
		    || (**arg != '{' && ಎಣಿಕೆ_2 == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		ಆದರೆ (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		tv.vval.v_string = ಖಾಲಿ;  // don't free it now
		++ಎಣಿಕೆ_2;
	    }
	    clear_tv(&tv);
	}

	ಆದರೆ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ನಿಲ್ಲಿಸು;
	}

	p = compile_one_ಅಭಿವ್ಯಕ್ತಿ_ಒಳ_str(*arg, cctx);
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    ret = FAIL;
	    ನಿಲ್ಲಿಸು;
	}
	++ಎಣಿಕೆ_2;
	*arg = p;
    }

    ಆದರೆ (ret == FAIL || !evaluate)
	ಹಿಂತಿರುಗಿ ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ಆದರೆ (ಎಣಿಕೆ_2 > 1)
	ಹಿಂತಿರುಗಿ generate_CONCAT(cctx, ಎಣಿಕೆ_2);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile "@r".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_get_ರಿಜಿಸ್ಟರ್(char_u **arg, cctx_T *cctx)
{
    ಪೂರ್ಣಾಂಕ		ret;

    ++*arg;
    ಆದರೆ (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ret = generate_ತುಂಬು(cctx, ISN_LOADREG, **arg, ಖಾಲಿ, &t_string);
    ++*arg;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
apply_leader(typval_T *rettv, ಪೂರ್ಣಾಂಕ numeric_only, char_u *ಆರಂಭ, char_u **ಕೊನೆ)
{
    char_u *p = *ಕೊನೆ;

    // this works from end to start
    ಆಗ (p > ಆರಂಭ)
    {
	--p;
	ಆದರೆ (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    ಆದರೆ (rettv->v_type == VAR_FLOAT)
	    {
		ಆದರೆ (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		varnumber_T	val;
		ಪೂರ್ಣಾಂಕ		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		ಆದರೆ (check_not_string(rettv) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		val = tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error);
		clear_tv(rettv);
		ಆದರೆ (error)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (*p == '-')
		    val = -val;
		rettv->v_type = VAR_ಸಂಖ್ಯೆ;
		rettv->vval.v_ಸಂಖ್ಯೆ = val;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (numeric_only)
	{
	    ++p;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '!')
	{
	    ಪೂರ್ಣಾಂಕ v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *ಕೊನೆ = p;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_vim_constant(char_u **arg, typval_T *rettv)
{
    ಆದರೆ (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
	*arg += 6;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
	*arg += 7;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	*arg += 6;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, ಪೂರ್ಣಾಂಕ *len, ಪೂರ್ಣಾಂಕ *type_is)
{
    exprtype_T	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;
    ಪೂರ್ಣಾಂಕ		i;

    ಆಯ್ಕೆ (p[0])
    {
	ಸ್ಥಿತಿ '=':   ಆದರೆ (p[1] == '=')
			type = ಅಭಿವ್ಯಕ್ತಿ_EQUAL;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[1] == '~')
			type = ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ;
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '!':   ಆದರೆ (p[1] == '=')
			type = ಅಭಿವ್ಯಕ್ತಿ_NEQUAL;
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[1] == '~')
			type = ಅಭಿವ್ಯಕ್ತಿ_NOMATCH;
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '>':   ಆದರೆ (p[1] != '=')
		    {
			type = ಅಭಿವ್ಯಕ್ತಿ_GREATER;
			*len = 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			type = ಅಭಿವ್ಯಕ್ತಿ_GEQUAL;
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ '<':   ಆದರೆ (p[1] != '=')
		    {
			type = ಅಭಿವ್ಯಕ್ತಿ_SMALLER;
			*len = 1;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			type = ಅಭಿವ್ಯಕ್ತಿ_SEQUAL;
		    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 'i':   ಆದರೆ (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			ಆದರೆ (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			ಆದರೆ (!ಸುರಕ್ಷಿತ_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? ಅಭಿವ್ಯಕ್ತಿ_IS : ಅಭಿವ್ಯಕ್ತಿ_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    ಶೂನ್ಯ
skip_ಅಭಿವ್ಯಕ್ತಿ_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_ಅಭಿವ್ಯಕ್ತಿ(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, ಖಾಲಿ);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    ಪೂರ್ಣಾಂಕ
bool_on_ರಾಶಿ(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_ರಾಶಿ(cctx, 0);
    ಆದರೆ (type == &t_bool)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type == &t_ಸಂಖ್ಯೆ_bool
	    || type == &t_const_ಸಂಖ್ಯೆ_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	ಹಿಂತಿರುಗಿ generate_COND2BOOL(cctx);

    ಹಿಂತಿರುಗಿ need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    ಶೂನ್ಯ
error_white_both(char_u *op, ಪೂರ್ಣಾಂಕ len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_leader(cctx_T *cctx, ಪೂರ್ಣಾಂಕ numeric_only, char_u *ಆರಂಭ, char_u **ಕೊನೆ)
{
    char_u	*p = *ಕೊನೆ;

    // this works from end to start
    ಆಗ (p > ಆರಂಭ)
    {
	--p;
	ಆಗ (VIM_ISWHITE(*p))
	    --p;
	ಆದರೆ (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಆದರೆ (type->tt_type != VAR_FLOAT && need_type(type, &t_ಸಂಖ್ಯೆ,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    ಆದರೆ (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (numeric_only)
	{
	    ++p;
	    ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ  invert = *p == '!';

	    ಆಗ (p > ಆರಂಭ && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		ಆದರೆ (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    ಆದರೆ (check_type_is_ಮೌಲ್ಯ(get_type_on_ರಾಶಿ(cctx, 0)) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (generate_2BOOL(cctx, invert, -1) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    *ಕೊನೆ = p;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಂಕ	    ret;
    char_u  *p = *arg + 1;

    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(p, arg, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (**arg == ')')
	// empty tuple
	ಹಿಂತಿರುಗಿ compile_tuple(arg, cctx, ppconst, FALSE);

    ಆದರೆ (ppconst->pp_ಬಳಸಿದ <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Not enough space in ppconst, flush constants.
	ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ret = compile_expr0(arg, cctx);
    }
    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (ret == OK && **arg == ',')
    {
	// tuple
	ಪೂರ್ಣಾಂಕ is_const = ppconst->pp_ಬಳಸಿದ > 0 || ppconst->pp_is_const;
	ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಹಿಂತಿರುಗಿ compile_tuple(arg, cctx, ppconst, is_const);
    }

    ಆದರೆ (**arg == ')')
	++*arg;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    ಹಿಂತಿರುಗಿ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *ಆರಂಭ_leader,
	char_u **ಕೊನೆ_leader,
	ppconst_T *ppconst)
{
    char_u	*name_ಆರಂಭ = *ಕೊನೆ_leader;
    ಪೂರ್ಣಾಂಕ		keeping_dict = FALSE;

    ಗಾಗಿ (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	ಆದರೆ (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_ಆರಂಭ(p)))
	{
	    char_u *ಮುಂದಿನ = peek_ಮುಂದಿನ_ಸಾಲು_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ &&
		    ((ಮುಂದಿನ[0] == '-' && ಮುಂದಿನ[1] == '>'
				 && (ಮುಂದಿನ[2] == '{'
				       || *skipwhite(ಮುಂದಿನ + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(ಮುಂದಿನ + 2))))
		    || (ಮುಂದಿನ[0] == '.' && eval_isdictc(ಮುಂದಿನ[1]))))
	    {
		ಮುಂದಿನ = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, TRUE);
		ಆದರೆ (ಮುಂದಿನ == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
		*arg = ಮುಂದಿನ;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	ಆದರೆ (**arg == '(')
	{
	    ಪೂರ್ಣಾಂಕ		argcount = 0;

	    ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_ರಾಶಿ(cctx, 0);

	    *arg = skipwhite(p + 1);
	    ಆದರೆ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (generate_PCALL(cctx, argcount, name_ಆರಂಭ, type, TRUE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ಆದರೆ (generate_instr(cctx, ISN_CLEARDICT) == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    ಪೂರ್ಣಾಂಕ	    alt;
	    char_u  *paren;

	    // something->method()
	    ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಆದರೆ (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    ಆದರೆ (compile_leader(cctx, TRUE, ಆರಂಭ_leader, ಕೊನೆ_leader) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    ಆದರೆ (**arg == '(')
		alt = 2;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// alternative 1 or 3
		p = *arg;
		ಆದರೆ (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಆದರೆ (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		ಗಾಗಿ ( ; eval_isnamec(*p); ++p)
		    ;
		ಆದರೆ (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    ಆದರೆ (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    ಆದರೆ (paren == ಖಾಲಿ)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		}
	    }

	    ಆದರೆ (alt != 1)
	    {
		ಪೂರ್ಣಾಂಕ	    argcount = 1;
		garray_T    *ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
		ಪೂರ್ಣಾಂಕ	    type_idx_ಆರಂಭ = ರಾಶಿ->ga_len;
		ಪೂರ್ಣಾಂಕ	    ಅಭಿವ್ಯಕ್ತಿ_isn_ಆರಂಭ = cctx->ctx_instr.ga_len;
		ಪೂರ್ಣಾಂಕ	    ಅಭಿವ್ಯಕ್ತಿ_isn_ಕೊನೆ;
		ಪೂರ್ಣಾಂಕ	    arg_isn_ಎಣಿಕೆ;

		ಆದರೆ (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    ಆದರೆ (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಪೂರ್ಣಾಂಕ fail;
		    ಪೂರ್ಣಾಂಕ save_len = cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len;
		    ಪೂರ್ಣಾಂಕ	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_ಅಭಿವ್ಯಕ್ತಿ;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_ಅಭಿವ್ಯಕ್ತಿ;
		    cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len = save_len;

		    ಆದರೆ (fail)
		    {
			ಆದರೆ (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), pstart);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		}

		// Compile the arguments.
		ಆದರೆ (**arg != '(')
		{
		    ಆದರೆ (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    ಇಲ್ಲದಿದ್ದರೆ
			semsg(_(e_missing_parenthesis_str), *arg);
		    ಹಿಂತಿರುಗಿ FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		ಅಭಿವ್ಯಕ್ತಿ_isn_ಕೊನೆ = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		ಆದರೆ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_ಎಣಿಕೆ = cctx->ctx_instr.ga_len - ಅಭಿವ್ಯಕ್ತಿ_isn_ಕೊನೆ;
		ಆದರೆ (arg_isn_ಎಣಿಕೆ > 0)
		{
		    ಪೂರ್ಣಾಂಕ	    ಅಭಿವ್ಯಕ್ತಿ_isn_ಎಣಿಕೆ = ಅಭಿವ್ಯಕ್ತಿ_isn_ಕೊನೆ - ಅಭಿವ್ಯಕ್ತಿ_isn_ಆರಂಭ;
		    isn_T   *isn = ALLOC_MULT(isn_T, ಅಭಿವ್ಯಕ್ತಿ_isn_ಎಣಿಕೆ);
		    type_T  *decl_type;
		    type2_T  *typep;

		    ಆದರೆ (isn == ಖಾಲಿ)
			ಹಿಂತಿರುಗಿ FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_ದತ್ತಾಂಶ)
							      + ಅಭಿವ್ಯಕ್ತಿ_isn_ಆರಂಭ,
					       ಗಾತ್ರ(isn_T) * ಅಭಿವ್ಯಕ್ತಿ_isn_ಎಣಿಕೆ);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_ದತ್ತಾಂಶ)
							      + ಅಭಿವ್ಯಕ್ತಿ_isn_ಆರಂಭ,
			     ((isn_T *)cctx->ctx_instr.ga_ದತ್ತಾಂಶ) + ಅಭಿವ್ಯಕ್ತಿ_isn_ಕೊನೆ,
						ಗಾತ್ರ(isn_T) * arg_isn_ಎಣಿಕೆ);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_ದತ್ತಾಂಶ)
					      + ಅಭಿವ್ಯಕ್ತಿ_isn_ಆರಂಭ + arg_isn_ಎಣಿಕೆ,
					  isn, ಗಾತ್ರ(isn_T) * ಅಭಿವ್ಯಕ್ತಿ_isn_ಎಣಿಕೆ);
		    vim_free(isn);

		    typep = ((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + type_idx_ಆರಂಭ;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + type_idx_ಆರಂಭ,
			      ((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + type_idx_ಆರಂಭ + 1,
			      ಗಾತ್ರ(type2_T)
				       * (ರಾಶಿ->ga_len - type_idx_ಆರಂಭ - 1));
		    typep = ((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + ರಾಶಿ->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_ರಾಶಿ(cctx, 0);
		ಆದರೆ (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }

	    ಆದರೆ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ಆದರೆ (generate_instr(cctx, ISN_CLEARDICT) == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (**arg == '[')
	{
	    ಪೂರ್ಣಾಂಕ		is_ತುಂಡು = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(p, arg, cctx) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (compile_expr0(arg, cctx) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		*arg = skipwhite(*arg);
	    }
	    ಆದರೆ (**arg == ':')
	    {
		is_ತುಂಡು = TRUE;
		++*arg;
		ಆದರೆ (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (compile_expr0(arg, cctx) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    ಆದರೆ (**arg != ']')
	    {
		emsg(_(e_missing_closing_ವರ್ಗಫಲ_brace));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    *arg = *arg + 1;

	    ಆದರೆ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ಆದರೆ (generate_instr(cctx, ISN_CLEARDICT) == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_ತುಂಡು, &keeping_dict, cctx) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಆದರೆ (type != &t_unknown
		    && (type->tt_type == VAR_CLASS
			|| (type->tt_type == VAR_OBJECT
			    && type != &t_object_any)))
	    {
		// class member: SomeClass.varname
		// class method: SomeClass.SomeMethod()
		// class constructor: SomeClass.new()
		// object member: someObject.varname, this.varname
		// object method: someObject.SomeMethod(), this.SomeMethod()
		*arg = p;
		ಆದರೆ (compile_class_object_ಸೂಚ್ಯಂಕ(cctx, arg, type) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*arg = p + 1;
		ಆದರೆ (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		p = *arg;
		ಆಗ (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		ಆದರೆ (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		ಆದರೆ (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    ಆದರೆ (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Compile an expression at "*arg" and add instructions to "cctx->ctx_instr".
 * "arg" is advanced until after the expression, skipping white space.
 *
 * If the value is a constant "ppconst->pp_used" will be non-zero.
 * Before instructions are generated, any values in "ppconst" will generated.
 *
 * This is the compiling equivalent of eval1(), eval2(), etc.
 */

/*
 *  number		number constant
 *  0zFFFFFFFF		Blob constant
 *  "string"		string constant
 *  'string'		literal string constant
 *  &option-name	option value
 *  @r			register contents
 *  identifier		variable value
 *  function()		function call
 *  $VAR		environment variable
 *  (expression)	nested expression
 *  [expr, expr]	List
 *  {key: val, [key]: val}   Dictionary
 *
 *  Also handle:
 *  ! in front		logical NOT
 *  - in front		unary minus
 *  + in front		unary plus (ignored)
 *  trailing (arg)	funcref/partial call
 *  trailing []		subscript in String or List
 *  trailing .name	entry in Dictionary
 *  trailing ->name()	method call
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*ಆರಂಭ_leader, *ಕೊನೆ_leader;
    ಪೂರ್ಣಾಂಕ		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ];
    ಪೂರ್ಣಾಂಕ		ಬಳಸಿದ_before = ppconst->pp_ಬಳಸಿದ;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    ಆರಂಭ_leader = *arg;
    ಆದರೆ (eval_leader(arg, TRUE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಕೊನೆ_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    ಆಯ್ಕೆ (**arg)
    {
	/*
	 * Number constant.
	 */
	ಸ್ಥಿತಿ '0':	// also for blob starting with 0z
	ಸ್ಥಿತಿ '1':
	ಸ್ಥಿತಿ '2':
	ಸ್ಥಿತಿ '3':
	ಸ್ಥಿತಿ '4':
	ಸ್ಥಿತಿ '5':
	ಸ್ಥಿತಿ '6':
	ಸ್ಥಿತಿ '7':
	ಸ್ಥಿತಿ '8':
	ಸ್ಥಿತಿ '9':
	ಸ್ಥಿತಿ '.':   ಆದರೆ (eval_ಸಂಖ್ಯೆ(arg, rettv, TRUE, FALSE) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    ಆದರೆ (apply_leader(rettv, TRUE,
					    ಆರಂಭ_leader, &ಕೊನೆ_leader) == FAIL)
		    {
			clear_tv(rettv);
			ಹಿಂತಿರುಗಿ FAIL;
		    }
		    ನಿಲ್ಲಿಸು;

	/*
	 * String constant: "string".
	 */
	ಸ್ಥಿತಿ '"':   ಆದರೆ (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    ನಿಲ್ಲಿಸು;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	ಸ್ಥಿತಿ '\'':  ಆದರೆ (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    ನಿಲ್ಲಿಸು;

	/*
	 * Constant Vim variable.
	 */
	ಸ್ಥಿತಿ 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    ನಿಲ್ಲಿಸು;

	/*
	 * "true" constant
	 */
	ಸ್ಥಿತಿ 't':   ಆದರೆ (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ret = NOTDONE;
		    ನಿಲ್ಲಿಸು;

	/*
	 * "false" constant
	 */
	ಸ್ಥಿತಿ 'f':   ಆದರೆ (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ret = NOTDONE;
		    ನಿಲ್ಲಿಸು;

	/*
	 * "null" or "null_*" constant
	 */
	ಸ್ಥಿತಿ 'n':   ಆದರೆ (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			ಪೂರ್ಣಾಂಕ	len;

			ಗಾಗಿ (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			ಆದರೆ (ret == FAIL)
			    ret = NOTDONE;
			ಇಲ್ಲದಿದ್ದರೆ
			    *arg += len + 4;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ret = NOTDONE;
		    ನಿಲ್ಲಿಸು;

	/*
	 * List: [expr, expr]
	 */
	ಸ್ಥಿತಿ '[':   ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    ನಿಲ್ಲಿಸು;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	ಸ್ಥಿತಿ '{':   ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
			ಹಿಂತಿರುಗಿ FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    ನಿಲ್ಲಿಸು;

	/*
	 * Option value: &name
	 */
	ಸ್ಥಿತಿ '&':	ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
			    ಹಿಂತಿರುಗಿ FAIL;
			ret = compile_get_option(arg, cctx);
			ನಿಲ್ಲಿಸು;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	ಸ್ಥಿತಿ '$':	ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
			    ಹಿಂತಿರುಗಿ FAIL;
			ಆದರೆ ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			ಇಲ್ಲದಿದ್ದರೆ
			    ret = compile_get_env(arg, cctx);
			ನಿಲ್ಲಿಸು;

	/*
	 * Register contents: @r.
	 */
	ಸ್ಥಿತಿ '@':	ಆದರೆ (generate_ppconst(cctx, ppconst) == FAIL)
			    ಹಿಂತಿರುಗಿ FAIL;
			ret = compile_get_ರಿಜಿಸ್ಟರ್(arg, cctx);
			ನಿಲ್ಲಿಸು;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	ಸ್ಥಿತಿ '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    ಆದರೆ (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:    ret = NOTDONE;
		    ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ (ret == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (rettv->v_type != VAR_UNKNOWN && ಬಳಸಿದ_before == ppconst->pp_ಬಳಸಿದ)
    {
	ಆದರೆ (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	ಇಲ್ಲದಿದ್ದರೆ
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_ಬಳಸಿದ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret == NOTDONE)
    {
	char_u	    *p;
	ಪೂರ್ಣಾಂಕ	    r;

	ಆದರೆ (!eval_isnamec1(**arg))
	{
	    ಆದರೆ (!vim9_bad_comment(*arg))
	    {
		ಆದರೆ (ತುದಿಗಳು_excmd(*skipwhite(*arg)))
		    semsg(_(e_ಖಾಲಿಗಳು_ಅಭಿವ್ಯಕ್ತಿ_str), *arg);
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_name_expected_str), *arg);
	    }
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// "name" or "name()"
	p = ಗಮ್ಯ_name_ಕೊನೆ(*arg, TRUE);
	ಆದರೆ (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_ಇಲ್ಲಿ));
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	size_t namelen = p - *arg;
	ಆದರೆ (*p == '<')
	{
	    ಆದರೆ (skip_generic_func_type_ವಾದಗಳು(&p) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    r = compile_ತುಂಬು(arg, namelen, p, cctx, TRUE, TRUE);
	}
	ಆದರೆ (r == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    ಆದರೆ (compile_subscript(arg, cctx, ಆರಂಭ_leader, &ಕೊನೆ_leader,
							     ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ ((ppconst->pp_ಬಳಸಿದ > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ - 1];
	ಆದರೆ (apply_leader(rettv, FALSE, ಆರಂಭ_leader, &ಕೊನೆ_leader) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಹಿಂತಿರುಗಿ OK;
    }
    ಆದರೆ (compile_leader(cctx, FALSE, ಆರಂಭ_leader, &ಕೊನೆ_leader) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = ಖಾಲಿ;

    // Recognize <type>
    ಆದರೆ (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ಆದರೆ (want_type == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (**arg != '>')
	{
	    ಆದರೆ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    ಇಲ್ಲದಿದ್ದರೆ
		emsg(_(e_missing_gt));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	++*arg;
	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (compile_expr9(arg, cctx, ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (want_type != ಖಾಲಿ)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_ಬಗೆ = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_ರಾಶಿ(cctx, 0);
	ಆದರೆ (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    ಆದರೆ (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		ppconst_ಬಳಸಿದ = ppconst->pp_ಬಳಸಿದ;

    // get the first expression
    ಆದರೆ (compile_expr8(arg, cctx, ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    ಗಾಗಿ (;;)
    {
	op = may_peek_ಮುಂದಿನ_ಸಾಲು(cctx, *arg, &ಮುಂದಿನ);
	ಆದರೆ (*op != '*' && *op != '/' && *op != '%')
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
	{
	    *arg = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ಆದರೆ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(op + 1, arg, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// get the second expression
	ಆದರೆ (compile_expr8(arg, cctx, ppconst) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (ppconst->pp_ಬಳಸಿದ == ppconst_ಬಳಸಿದ + 2
		&& ppconst->pp_tv[ppconst_ಬಳಸಿದ].v_type == VAR_ಸಂಖ್ಯೆ
		&& ppconst->pp_tv[ppconst_ಬಳಸಿದ + 1].v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_ಬಳಸಿದ];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_ಬಳಸಿದ + 1];
	    varnumber_T	    res = 0;
	    ಪೂರ್ಣಾಂಕ		    failed = FALSE;

	    // both are numbers: compute the result
	    ಆಯ್ಕೆ (*op)
	    {
		ಸ್ಥಿತಿ '*': res = tv1->vval.v_ಸಂಖ್ಯೆ * tv2->vval.v_ಸಂಖ್ಯೆ;
			  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '/': res = ಅಂಕೆ_divide(tv1->vval.v_ಸಂಖ್ಯೆ,
						  tv2->vval.v_ಸಂಖ್ಯೆ, &failed);
			  ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ '%': res = ಅಂಕೆ_modulus(tv1->vval.v_ಸಂಖ್ಯೆ,
						  tv2->vval.v_ಸಂಖ್ಯೆ, &failed);
			  ನಿಲ್ಲಿಸು;
	    }
	    ಆದರೆ (failed)
		ಹಿಂತಿರುಗಿ FAIL;
	    tv1->vval.v_ಸಂಖ್ಯೆ = res;
	    --ppconst->pp_ಬಳಸಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		oplen;
    ಪೂರ್ಣಾಂಕ		ppconst_ಬಳಸಿದ = ppconst->pp_ಬಳಸಿದ;

    // get the first variable
    ಆದರೆ (compile_expr7(arg, cctx, ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    ಗಾಗಿ (;;)
    {
	op = may_peek_ಮುಂದಿನ_ಸಾಲು(cctx, *arg, &ಮುಂದಿನ);
	ಆದರೆ (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (op[0] == op[1] && *op != '.' && ಮುಂದಿನ)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    ನಿಲ್ಲಿಸು;
	oplen = (*op == '.' ? 2 : 1);
	ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
	{
	    *arg = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ಆದರೆ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(op + oplen, arg, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	// get the second expression
	ಆದರೆ (compile_expr7(arg, cctx, ppconst) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (ppconst->pp_ಬಳಸಿದ == ppconst_ಬಳಸಿದ + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_ಬಳಸಿದ].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_ಬಳಸಿದ + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_ಬಳಸಿದ].v_type == VAR_ಸಂಖ್ಯೆ
		    && ppconst->pp_tv[ppconst_ಬಳಸಿದ + 1].v_type == VAR_ಸಂಖ್ಯೆ)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_ಬಳಸಿದ];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_ಬಳಸಿದ + 1];

	    // concat/subtract/add constant numbers
	    ಆದರೆ (*op == '+')
		tv1->vval.v_ಸಂಖ್ಯೆ = tv1->vval.v_ಸಂಖ್ಯೆ + tv2->vval.v_ಸಂಖ್ಯೆ;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*op == '-')
		tv1->vval.v_ಸಂಖ್ಯೆ = tv1->vval.v_ಸಂಖ್ಯೆ - tv2->vval.v_ಸಂಖ್ಯೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		ಆದರೆ (tv1->vval.v_string == ಖಾಲಿ)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_ಬಳಸಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    ಆದರೆ (*op == '.')
	    {
		ಆದರೆ (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (generate_CONCAT(cctx, 2) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		generate_two_op(cctx, op);
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;
    char_u	*p;
    char_u	*ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		len = 2;
    ಪೂರ್ಣಾಂಕ		ppconst_ಬಳಸಿದ = ppconst->pp_ಬಳಸಿದ;
    isn_T	*isn;

    // get the first variable
    ಆದರೆ (compile_expr6(arg, cctx, ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    ಗಾಗಿ (;;)
    {
	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;

	p = may_peek_ಮುಂದಿನ_ಸಾಲು(cctx, *arg, &ಮುಂದಿನ);
	ಆದರೆ (p[0] == '<' && p[1] == '<')
	    type = ಅಭಿವ್ಯಕ್ತಿ_LSHIFT;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[0] == '>' && p[1] == '>')
	    type = ಅಭಿವ್ಯಕ್ತಿ_RSHIFT;

	ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN)
	    ಹಿಂತಿರುಗಿ OK;

	// Handle a bitwise left or right shift operator
	ಆದರೆ (ppconst->pp_ಬಳಸಿದ == ppconst_ಬಳಸಿದ + 1)
	{
	    ಆದರೆ (ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ - 1].v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    type_T	*t = get_type_on_ರಾಶಿ(cctx, 0);

	    ಆದರೆ (need_type(t, &t_ಸಂಖ್ಯೆ, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
	{
	    *arg = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ಆದರೆ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// get the second variable
	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(p + len, arg, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (compile_expr6(arg, cctx, ppconst) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (ppconst->pp_ಬಳಸಿದ == ppconst_ಬಳಸಿದ + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ - 1];

	    // Both sides are a constant, compute the result now.
	    ಆದರೆ (tv2->v_type != VAR_ಸಂಖ್ಯೆ || tv2->vval.v_ಸಂಖ್ಯೆ < 0)
	    {
		// right operand should be a positive number
		ಆದರೆ (tv2->v_type != VAR_ಸಂಖ್ಯೆ)
		    emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
		ಇಲ್ಲದಿದ್ದರೆ
		    emsg(_(e_bitshift_ops_must_be_ಧನಾತ್ಮಕ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    ಆದರೆ (tv2->vval.v_ಸಂಖ್ಯೆ > MAX_LSHIFT_ಬಿಟ್ಟುಗಳು)
		tv1->vval.v_ಸಂಖ್ಯೆ = 0;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type == ಅಭಿವ್ಯಕ್ತಿ_LSHIFT)
		tv1->vval.v_ಸಂಖ್ಯೆ =
			(uvarnumber_T)tv1->vval.v_ಸಂಖ್ಯೆ << tv2->vval.v_ಸಂಖ್ಯೆ;
	    ಇಲ್ಲದಿದ್ದರೆ
		tv1->vval.v_ಸಂಖ್ಯೆ =
			(uvarnumber_T)tv1->vval.v_ಸಂಖ್ಯೆ >> tv2->vval.v_ಸಂಖ್ಯೆ;
	    clear_tv(tv2);
	    --ppconst->pp_ಬಳಸಿದ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (need_type(get_type_on_ರಾಶಿ(cctx, 0), &t_ಸಂಖ್ಯೆ, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    ಆದರೆ (isn == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;

	    ಆದರೆ (isn != ಖಾಲಿ)
		isn->isn_arg.op.op_type = type;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * expr5a == expr5b
 * expr5a =~ expr5b
 * expr5a != expr5b
 * expr5a !~ expr5b
 * expr5a > expr5b
 * expr5a >= expr5b
 * expr5a < expr5b
 * expr5a <= expr5b
 * expr5a is expr5b
 * expr5a isnot expr5b
 *
 * Produces instructions:
 *	EVAL expr5a		Push result of "expr5a"
 *	EVAL expr5b		Push result of "expr5b"
 *	COMPARE			one of the compare instructions
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;
    char_u	*p;
    char_u	*ಮುಂದಿನ;
    ಪೂರ್ಣಾಂಕ		len = 2;
    ಪೂರ್ಣಾಂಕ		type_is = FALSE;
    ಪೂರ್ಣಾಂಕ		ppconst_ಬಳಸಿದ = ppconst->pp_ಬಳಸಿದ;

    // get the first variable
    ಆದರೆ (compile_expr5(arg, cctx, ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    p = may_peek_ಮುಂದಿನ_ಸಾಲು(cctx, *arg, &ಮುಂದಿನ);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ಆದರೆ (type != ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN)
    {
	ಪೂರ್ಣಾಂಕ ic = FALSE;  // Default: do not ignore case

	ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
	{
	    *arg = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	ಆದರೆ (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_ಅಭಿವ್ಯಕ್ತಿ_str), *arg);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	// extra question mark appended: ignore case
	ಆದರೆ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p[len] == '#')
	    ++len;
	// nothing appended: match case

	ಆದರೆ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// get the second variable
	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(p + len, arg, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (compile_expr5(arg, cctx, ppconst) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (ppconst->pp_ಬಳಸಿದ == ppconst_ಬಳಸಿದ + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ - 1];
	    ಪೂರ್ಣಾಂಕ		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    ಆದರೆ (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_ಸಂಖ್ಯೆ = tv1->vval.v_ಸಂಖ್ಯೆ
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_ಬಳಸಿದ;
	    }
	    ಹಿಂತಿರುಗಿ ret;
	}

	generate_ppconst(cctx, ppconst);
	ಹಿಂತಿರುಗಿ generate_COMPARE(cctx, type, ic);
    }

    ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	ಅಕ್ಷರ	*op,
	ppconst_T *ppconst,
	ಪೂರ್ಣಾಂಕ	ppconst_ಬಳಸಿದ ಬಳಸದ)
{
    char_u	*ಮುಂದಿನ;
    char_u	*p = may_peek_ಮುಂದಿನ_ಸಾಲು(cctx, *arg, &ಮುಂದಿನ);
    ಪೂರ್ಣಾಂಕ		opchar = *op;

    ಆದರೆ (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	ಕೊನೆ_ga;
	ಪೂರ್ಣಾಂಕ		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&ಕೊನೆ_ga, ಗಾತ್ರ(ಪೂರ್ಣಾಂಕ), 10);
	ಆಗ (p[0] == opchar && p[1] == opchar)
	{
	    ದೀರ್ಘ	ಆರಂಭ_lnum = SOURCING_LNUM;
	    ದೀರ್ಘ	save_sourcing_lnum;
	    ಪೂರ್ಣಾಂಕ		ಆರಂಭ_ctx_lnum = cctx->ctx_lnum;
	    ಪೂರ್ಣಾಂಕ		save_lnum;
	    ಪೂರ್ಣಾಂಕ		const_ಬಳಸಿದ;
	    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
	    {
		*arg = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ಆದರೆ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&ಕೊನೆ_ga);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = ಆರಂಭ_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = ಆರಂಭ_ctx_lnum;

	    ಸ್ಥಿತಿ_2 = check_ppconst_bool(ppconst);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 != FAIL)
	    {
		// Use the last ppconst if possible.
		ಆದರೆ (ppconst->pp_ಬಳಸಿದ > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ - 1];
		    ಪೂರ್ಣಾಂಕ		is_true = tv2bool(tv);

		    ಆದರೆ ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_ಸಂಖ್ಯೆ = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_ಬಳಸಿದ;
			jump_when = JUMP_NEVER;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // Every part must evaluate to a bool.
		    ಸ್ಥಿತಿ_2 = bool_on_ರಾಶಿ(cctx);
		}
	    }
	    ಆದರೆ (ಸ್ಥಿತಿ_2 != FAIL)
		ಸ್ಥಿತಿ_2 = ga_grow(&ಕೊನೆ_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
	    {
		ga_clear(&ಕೊನೆ_ga);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    ಆದರೆ (jump_when != JUMP_NEVER)
	    {
		ಆದರೆ (cctx->ctx_skip != SKIP_YES)
		{
		    *(((ಪೂರ್ಣಾಂಕ *)ಕೊನೆ_ga.ga_ದತ್ತಾಂಶ) + ಕೊನೆ_ga.ga_len) = instr->ga_len;
		    ++ಕೊನೆ_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&ಕೊನೆ_ga);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    const_ಬಳಸಿದ = ppconst->pp_ಬಳಸಿದ;
	    ಆದರೆ ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&ಕೊನೆ_ga);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    ಆದರೆ (ppconst->pp_ಬಳಸಿದ == const_ಬಳಸಿದ + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_ಬಳಸಿದ - 1];

		ಆದರೆ (tv->v_type == VAR_ಸಂಖ್ಯೆ
			 && (tv->vval.v_ಸಂಖ್ಯೆ == 1 || tv->vval.v_ಸಂಖ್ಯೆ == 0))
		{
		    tv->vval.v_ಸಂಖ್ಯೆ = tv->vval.v_ಸಂಖ್ಯೆ == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_ಮುಂದಿನ_ಸಾಲು(cctx, *arg, &ಮುಂದಿನ);
	}

	ಆದರೆ (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&ಕೊನೆ_ga);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (cctx->ctx_skip != SKIP_YES && ppconst->pp_ಬಳಸಿದ == 0)
	    // Every part must evaluate to a bool.
	    ಆದರೆ (bool_on_ರಾಶಿ(cctx) == FAIL)
	    {
		ga_clear(&ಕೊನೆ_ga);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	ಆದರೆ (ಕೊನೆ_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    ಆಗ (ಕೊನೆ_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--ಕೊನೆ_ga.ga_len;
		isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ)
				  + *(((ಪೂರ್ಣಾಂಕ *)ಕೊನೆ_ga.ga_ದತ್ತಾಂಶ) + ಕೊನೆ_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&ಕೊನೆ_ga);

	cctx->ctx_skip = save_skip;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * expr4a && expr4a && expr4a	    logical AND
 *
 * Produces instructions:
 *	EVAL expr4a		Push result of "expr4a"
 *	COND2BOOL		convert to bool if needed
 *	JUMP_IF_COND_FALSE end
 *	EVAL expr4b		Push result of "expr4b"
 *	JUMP_IF_COND_FALSE end
 *	EVAL expr4c		Push result of "expr4c"
 * end:
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಂಕ		ppconst_ಬಳಸಿದ = ppconst->pp_ಬಳಸಿದ;

    // get the first variable
    ಆದರೆ (compile_expr4(arg, cctx, ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // || and && work almost the same
    ಹಿಂತಿರುಗಿ compile_and_or(arg, cctx, "&&", ppconst, ppconst_ಬಳಸಿದ);
}

/*
 * expr3a || expr3b || expr3c	    logical OR
 *
 * Produces instructions:
 *	EVAL expr3a		Push result of "expr3a"
 *	COND2BOOL		convert to bool if needed
 *	JUMP_IF_COND_TRUE end
 *	EVAL expr3b		Push result of "expr3b"
 *	JUMP_IF_COND_TRUE end
 *	EVAL expr3c		Push result of "expr3c"
 * end:
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಂಕ		ppconst_ಬಳಸಿದ = ppconst->pp_ಬಳಸಿದ;

    // eval the first expression
    ಆದರೆ (compile_expr3(arg, cctx, ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // || and && work almost the same
    ಹಿಂತಿರುಗಿ compile_and_or(arg, cctx, "||", ppconst, ppconst_ಬಳಸಿದ);
}

/*
 * Toplevel expression: expr2 ? expr1a : expr1b
 * Produces instructions:
 *	EVAL expr2		Push result of "expr2"
 *      JUMP_IF_FALSE alt	jump if false
 *      EVAL expr1a
 *      JUMP_ALWAYS end
 * alt:	EVAL expr1b
 * end:
 *
 * Toplevel expression: expr2 ?? expr1
 * Produces instructions:
 *	EVAL expr2		    Push result of "expr2"
 *      JUMP_AND_KEEP_IF_TRUE end   jump if true
 *      EVAL expr1
 * end:
 */
    ಪೂರ್ಣಾಂಕ
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		ppconst_ಬಳಸಿದ = ppconst->pp_ಬಳಸಿದ;
    char_u	*ಮುಂದಿನ;

    // Ignore all kinds of errors when not producing code.
    ಆದರೆ (cctx->ctx_skip == SKIP_YES)
    {
	ಪೂರ್ಣಾಂಕ		prev_did_emsg = did_emsg;

	skip_ಅಭಿವ್ಯಕ್ತಿ_cctx(arg, cctx);
	ಹಿಂತಿರುಗಿ did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    ಆದರೆ (compile_expr2(arg, cctx, ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    p = may_peek_ಮುಂದಿನ_ಸಾಲು(cctx, *arg, &ಮುಂದಿನ);
    ಆದರೆ (*p == '?')
    {
	ಪೂರ್ಣಾಂಕ		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
	ಪೂರ್ಣಾಂಕ		alt_idx = instr->ga_len;
	ಪೂರ್ಣಾಂಕ		ಕೊನೆ_idx = 0;
	isn_T		*isn;
	type_T		*type1 = ಖಾಲಿ;
	ಪೂರ್ಣಾಂಕ		has_const_ಅಭಿವ್ಯಕ್ತಿ = FALSE;
	ಪೂರ್ಣಾಂಕ		const_ಮೌಲ್ಯ = FALSE;
	ಪೂರ್ಣಾಂಕ		save_skip = cctx->ctx_skip;

	ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
	{
	    *arg = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ಆದರೆ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (ppconst->pp_ಬಳಸಿದ == ppconst_ಬಳಸಿದ + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_ಅಭಿವ್ಯಕ್ತಿ = TRUE;
	    ಆದರೆ (op_falsy)
		const_ಮೌಲ್ಯ = tv2bool(&ppconst->pp_tv[ppconst_ಬಳಸಿದ]);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಪೂರ್ಣಾಂಕ error = FALSE;

		const_ಮೌಲ್ಯ = tv_get_bool_chk(&ppconst->pp_tv[ppconst_ಬಳಸಿದ],
								       &error);
		ಆದರೆ (error)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_ಮೌಲ್ಯ : !const_ಮೌಲ್ಯ) ? SKIP_YES : SKIP_NOT;

	    ಆದರೆ (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		clear_tv(&ppconst->pp_tv[ppconst_ಬಳಸಿದ]);
		--ppconst->pp_ಬಳಸಿದ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    generate_ppconst(cctx, ppconst);
	    ಆದರೆ (op_falsy)
		ಕೊನೆ_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    ಆದರೆ (op_falsy)
	    {
		type1 = get_type_on_ರಾಶಿ(cctx, -1);
		ಆದರೆ (check_type_is_ಮೌಲ್ಯ(type1) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (compile_expr1(arg, cctx, ppconst) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (!has_const_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    generate_ppconst(cctx, ppconst);

	    ಆದರೆ (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_ರಾಶಿ(cctx, 0);
		--ರಾಶಿ->ga_len;

		ಕೊನೆ_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	ಆದರೆ (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_ಮುಂದಿನ_ಸಾಲು(cctx, *arg, &ಮುಂದಿನ);
	    ಆದರೆ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (ಮುಂದಿನ != ಖಾಲಿ)
	    {
		*arg = ಮುಂದಿನ_ಸಾಲು_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ಆದರೆ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    // evaluate the third expression
	    ಆದರೆ (has_const_ಅಭಿವ್ಯಕ್ತಿ)
		cctx->ctx_skip = save_skip == SKIP_YES || const_ಮೌಲ್ಯ
							 ? SKIP_YES : SKIP_NOT;
	    ಆದರೆ (may_get_ಮುಂದಿನ_ಸಾಲು_error(p + 1, arg, cctx) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (compile_expr1(arg, cctx, ppconst) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (!has_const_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ)
					      + ರಾಶಿ->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + ಕೊನೆ_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
compile_expr0_ext(char_u **arg,  cctx_T *cctx, ಪೂರ್ಣಾಂಕ *is_const)
{
    ppconst_T	ppconst;

    CLEAR_ಕ್ಷೇತ್ರ(ppconst);
    ಆದರೆ (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (is_const != ಖಾಲಿ)
	*is_const = ppconst.pp_ಬಳಸಿದ > 0 || ppconst.pp_is_const;
    ಆದರೆ (generate_ppconst(cctx, &ppconst) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Toplevel expression.
 */
    ಪೂರ್ಣಾಂಕ
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    ಹಿಂತಿರುಗಿ compile_expr0_ext(arg, cctx, ಖಾಲಿ);
}


#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_EVAL)
