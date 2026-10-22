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

#define USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ paren_follows_after_ಅಭಿವ್ಯಕ್ತಿ = 0;

/*
 * Generate code for any ppconst entries.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;
    ಪೂರ್ಣಾಙ್ಕ	    ret = OK;
    ಪೂರ್ಣಾಙ್ಕ	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    ಪ್ರತಿ (i = 0; i < ppconst->pp_used; ++i)
	ಯದಿ (generate_tv_ತಳ್ಳು(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_ppconst_bool(ppconst_T *ppconst)
{
    ಯದಿ (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	ಪ್ರತ್ಯಾಗಮ check_typval_type(&t_bool, tv, where);
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    ಶೂನ್ಯ
clear_ppconst(ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_tuple_member(
    type2_T	*typep,
    ಪೂರ್ಣಾಙ್ಕ		is_slice,
    cctx_T	*cctx)
{
    ಯದಿ (is_slice)
    {
	ಯದಿ (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	// a copy is made so the member type is no longer declared
	ಯದಿ (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	ಯದಿ (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    ಯದಿ (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    ಯದಿ (typep->type_curr->tt_argcount == 1)
	    {
		ಯದಿ (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		ಅನ್ಯಥಾ
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    ಅನ್ಯಥಾ
		typep->type_curr = &t_any;
	    ಯದಿ (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		ಯದಿ (typep->type_decl->tt_argcount == 1)
		{
		    ಯದಿ (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    ಅನ್ಯಥಾ
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		ಅನ್ಯಥಾ
		    typep->type_curr = &t_any;
	    }
	    ಅನ್ಯಥಾ
		typep->type_decl = typep->type_curr;
	}
	ಯದಿ (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    ಪೂರ್ಣಾಙ್ಕ
compile_member(ಪೂರ್ಣಾಙ್ಕ is_slice, ಪೂರ್ಣಾಙ್ಕ *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)ರಾಶಿ->ga_data) + ರಾಶಿ->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)ರಾಶಿ->ga_data) + ರಾಶಿ->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    ಯದಿ ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    ಯದಿ (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	ಯದಿ (need_type(idxtype, &t_ಸಂಖ್ಯೆ, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (is_slice)
	{
	    idxtype = get_type_on_ರಾಶಿ(cctx, 1);
	    ಯದಿ (need_type(idxtype, &t_ಸಂಖ್ಯೆ, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಯದಿ (vartype == VAR_DICT)
    {
	ಯದಿ (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    ಯದಿ (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    ಯದಿ (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		ಯದಿ (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    ಅನ್ಯಥಾ
		typep->type_decl = typep->type_curr;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	ಯದಿ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (keeping_dict != ರಿಕ್ತ)
	    *keeping_dict = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	ಯದಿ ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (vartype == VAR_BLOB)
    {
	ಯದಿ (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    ಯದಿ (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    typep->type_curr = &t_ಸಂಖ್ಯೆ;
	    typep->type_decl = &t_ಸಂಖ್ಯೆ;
	    ಯದಿ (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (vartype == VAR_TUPLE)
    {
	ಯದಿ (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	ಯದಿ (is_slice)
	{
	    ಯದಿ (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    // a copy is made so the member type is no longer declared
	    ಯದಿ (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    ಯದಿ (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		ಯದಿ (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		ಯದಿ (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		ಯದಿ (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    ಯದಿ (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		ಅನ್ಯಥಾ
			typep->type_decl = typep->type_curr;
	    }
	    ಯದಿ (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಅನ್ಯಥಾ
    {
	ವಿಕಲ್ಪ (vartype)
	{
	    ಪಕ್ಷ VAR_FUNC:
	    ಪಕ್ಷ VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_BOOL:
	    ಪಕ್ಷ VAR_SPECIAL:
	    ಪಕ್ಷ VAR_JOB:
	    ಪಕ್ಷ VAR_CHANNEL:
	    ಪಕ್ಷ VAR_INSTR:
	    ಪಕ್ಷ VAR_CLASS:
	    ಪಕ್ಷ VAR_OBJECT:
	    ಪಕ್ಷ VAR_TYPEALIAS:
	    ಪಕ್ಷ VAR_UNKNOWN:
	    ಪಕ್ಷ VAR_ANY:
	    ಪಕ್ಷ VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    ಪ್ರತಿ (cctx_T *cctx = cctx_arg; cctx != ರಿಕ್ತ; cctx = cctx->ctx_outer)
    {
	ಯದಿ (cctx->ctx_ufunc != ರಿಕ್ತ && cctx->ctx_ufunc->uf_class != ರಿಕ್ತ)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    ಯಾವತ್ (clp != ರಿಕ್ತ)
	    {
		ಯದಿ (clp == cl)
		    ಪ್ರತ್ಯಾಗಮ TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    ಪೂರ್ಣಾಙ್ಕ
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    ಪೂರ್ಣಾಙ್ಕ		m_idx;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;

    ಯದಿ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    class_T *cl = type->tt_class;
    ಪೂರ್ಣಾಙ್ಕ is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    ಯದಿ (type == &t_super)
    {
	ಯದಿ (cctx->ctx_ufunc == ರಿಕ್ತ || cctx->ctx_ufunc->uf_class == ರಿಕ್ತ)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_ರಾಶಿ.ga_len;
    }
    ಅನ್ಯಥಾ ಯದಿ (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	ಯದಿ (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    ಯದಿ (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    ಯದಿ (cl == ರಿಕ್ತ)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ರಿಕ್ತ, ರಿಕ್ತ, FNE_CHECK_START);
    ಯದಿ (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    ಯದಿ (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	ಯದಿ (name_end == ರಿಕ್ತ)
	    ಗಚ್ಛ done;
    }

    ಯದಿ (*name_end == '(')
    {
	ಪೂರ್ಣಾಙ್ಕ	function_ಎಣಿಕೆ;
	ಪೂರ್ಣಾಙ್ಕ	child_ಎಣಿಕೆ;
	ufunc_T	**functions;

	ಯದಿ (type->tt_type == VAR_CLASS)
	{
	    function_ಎಣಿಕೆ = cl->class_class_function_ಎಣಿಕೆ;
	    child_ಎಣಿಕೆ = cl->class_class_function_ಎಣಿಕೆ_child;
	    functions = cl->class_class_functions;
	}
	ಅನ್ಯಥಾ
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    ಯದಿ (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_ಎಣಿಕೆ = cl->class_obj_method_ಎಣಿಕೆ;
	    child_ಎಣಿಕೆ = cl->class_obj_method_ಎಣಿಕೆ_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ fi;
	ಪ್ರತಿ (fi = is_super ? child_ಎಣಿಕೆ : 0; fi < function_ಎಣಿಕೆ; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    ಯದಿ (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		ಭಙ್ಗ;
	    }
	}

	ocmember_T  *ocm = ರಿಕ್ತ;
	ಯದಿ (ufunc == ರಿಕ್ತ)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    ಯದಿ (ocm == ರಿಕ್ತ || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_ಸಂದೇಶ(cl, type->tt_type, name, len);
		ಗಚ್ಛ done;
	    }
	    ಯದಿ (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_ರಾಶಿ.ga_len;
		ಯದಿ (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    ಗಚ್ಛ done;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ status;

		ಯದಿ (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		ಅನ್ಯಥಾ
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		ಯದಿ (status == FAIL)
		    ಗಚ್ಛ done;
	    }
	}

	ಯದಿ (is_super && ufunc != ರಿಕ್ತ && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    ಗಚ್ಛ done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	ಯದಿ (ocm == ರಿಕ್ತ && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    ಗಚ್ಛ done;
	}

	// process generic function call
	ಯದಿ (ufunc != ರಿಕ್ತ)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ಯದಿ (ufunc == ರಿಕ್ತ)
		ಗಚ್ಛ done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	ಪೂರ್ಣಾಙ್ಕ argcount = 0;
	ಯದಿ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    ಗಚ್ಛ done;

	ಯದಿ (ocm != ರಿಕ್ತ)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    ಗಚ್ಛ done;
	}
	ಯದಿ (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	ಅನ್ಯಥಾ
	    ret = generate_CALL(cctx, ufunc, ರಿಕ್ತ, 0, argcount, FALSE);
	ಗಚ್ಛ done;
    }

    ಯದಿ (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	ಯದಿ (m_idx >= 0)
	{
	    ಯದಿ (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		ಗಚ್ಛ done;
	    }

	    *arg = name_end;
	    ಯದಿ (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    ಅನ್ಯಥಾ
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    ಗಚ್ಛ done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	ಯದಿ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    ಯದಿ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		ಗಚ್ಛ done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_ರಾಶಿ.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, ರಿಕ್ತ);
	    ಗಚ್ಛ done;
	}

	member_not_found_ಸಂದೇಶ(cl, VAR_OBJECT, name, len);
    }
    ಅನ್ಯಥಾ
    {
	// load class member
	ಪೂರ್ಣಾಙ್ಕ idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	ಯದಿ (m != ರಿಕ್ತ)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    ಯದಿ (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		ಗಚ್ಛ done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ರಾಶಿ.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    ಗಚ್ಛ done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	ಯದಿ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    ಯದಿ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		ಗಚ್ಛ done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ರಾಶಿ.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, ರಿಕ್ತ);
	    ಗಚ್ಛ done;
	}

	member_not_found_ಸಂದೇಶ(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    ಪೂರ್ಣಾಙ್ಕ
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    ಪೂರ್ಣಾಙ್ಕ		    idx;
    imported_T	    *imp;

    ಯದಿ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, ರಿಕ್ತ);
    ಯದಿ (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ (end == ರಿಕ್ತ)
	imp = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ (import == ರಿಕ್ತ)
	imp = find_imported(name, 0, FALSE);
    ಅನ್ಯಥಾ
	imp = import;

    ಯದಿ (imp != ರಿಕ್ತ)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	ಪೂರ್ಣಾಙ್ಕ	cc;
	ufunc_T	*ufunc = ರಿಕ್ತ;
	type_T	*type;
	ಪೂರ್ಣಾಙ್ಕ	done = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	ಯದಿ (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	++p;
	ಯದಿ (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// isolate one name
	exp_name = p;
	ಯಾವತ್ (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	ಯದಿ (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	ಯದಿ (res == OK)
	{
	    ಯದಿ (si->sn_autoload_prefix != ರಿಕ್ತ
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		ಯದಿ (cc == '(' || paren_follows_after_ಅಭಿವ್ಯಕ್ತಿ)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		ಅನ್ಯಥಾ
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		ಯದಿ (cc == '(' || paren_follows_after_ಅಭಿವ್ಯಕ್ತಿ)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		ಅನ್ಯಥಾ
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, ರಿಕ್ತ, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	ಯದಿ (done)
	    ಪ್ರತ್ಯಾಗಮ res;

	ಯದಿ (idx < 0)
	{
	    ಯದಿ (ufunc != ರಿಕ್ತ)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		ಯದಿ (IS_GENERIC_FUNC(ufunc))
		{
		    ಯದಿ (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			ಯದಿ (p != ರಿಕ್ತ)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			ಯದಿ (p == ರಿಕ್ತ || ufunc == ರಿಕ್ತ)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		    ಅನ್ಯಥಾ
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		}
		ಅನ್ಯಥಾ ಯದಿ (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, ರಿಕ್ತ, TRUE);
		ಪ್ರತ್ಯಾಗಮ OK;
	    }
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    ಪ್ರತ್ಯಾಗಮ generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    ಪೂರ್ಣಾಙ್ಕ			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    ಯದಿ (ufunc == ರಿಕ್ತ || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    ಯದಿ (ufunc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    ಯದಿ (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, ರಿಕ್ತ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    ಪ್ರತ್ಯಾಗಮ cctx->ctx_ufunc != ರಿಕ್ತ
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == ರಿಕ್ತ
		|| cctx->ctx_outer->ctx_ufunc == ರಿಕ್ತ
		|| cctx->ctx_outer->ctx_ufunc->uf_class == ರಿಕ್ತ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    ಪೂರ್ಣಾಙ್ಕ			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    ಪೂರ್ಣಾಙ್ಕ		isn_idx;

    ಯದಿ (fp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ depth = 0; depth < outer_depth; ++depth)
    {
	ಯದಿ (outer_cctx->ctx_outer == ರಿಕ್ತ)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    ಯದಿ (fp->uf_block_ids == ರಿಕ್ತ && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, ufunc->uf_block_depth);
	ಯದಿ (fp->uf_block_ids != ರಿಕ್ತ)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    ಯದಿ (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (generate_FUNCREF(cctx, fp, ರಿಕ್ತ, FALSE, 0, &isn_idx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (cctx->ctx_skip == SKIP_YES)
	ಪ್ರತ್ಯಾಗಮ OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    ಯದಿ (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	ಯದಿ (extra == ರಿಕ್ತ)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    ಯದಿ (extra == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	ಪೂರ್ಣಾಙ್ಕ	is_ಅಭಿವ್ಯಕ್ತಿ,
	ಪೂರ್ಣಾಙ್ಕ	error)
{
    type_T	*type;
    char_u	*name = ರಿಕ್ತ;
    char_u	*end = end_arg;
    ಪೂರ್ಣಾಙ್ಕ		res = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = ರಿಕ್ತ;

    generic_func_args_table_init(&gfatab);

    ಯದಿ (*(*arg + namelen) == '<')
    {
	// generic function call
	ಯದಿ (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == ರಿಕ್ತ)
	{
	    generic_func_args_table_clear(&gfatab);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    ಯದಿ (*(*arg + 1) == ':')
    {
	ಯದಿ (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    ವಿಕಲ್ಪ (**arg)
	    {
		ಪಕ್ಷ 'g': isn_type = ISN_LOADGDICT; ಭಙ್ಗ;
		ಪಕ್ಷ 'w': isn_type = ISN_LOADWDICT; ಭಙ್ಗ;
		ಪಕ್ಷ 't': isn_type = ISN_LOADTDICT; ಭಙ್ಗ;
		ಪಕ್ಷ 'b': isn_type = ISN_LOADBDICT; ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    ಗಚ್ಛ theend;
	    }
	    ಯದಿ (generate_instr_type(cctx, isn_type, &t_dict_any) == ರಿಕ್ತ)
		ಗಚ್ಛ theend;
	    res = OK;
	}
	ಅನ್ಯಥಾ
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    ಯದಿ (name == ರಿಕ್ತ)
		ಗಚ್ಛ theend;

	    ವಿಕಲ್ಪ (**arg)
	    {
		ಪಕ್ಷ 'v': res = generate_LOADV(cctx, name);
			  ಭಙ್ಗ;
		ಪಕ್ಷ 's': ಯದಿ (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      ಗಚ್ಛ theend;
			  }
			  ಯದಿ (is_ಅಭಿವ್ಯಕ್ತಿ && find_func(name, FALSE) != ರಿಕ್ತ)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  ಅನ್ಯಥಾ
			      res = compile_load_scriptvar(cctx, name,
							    ರಿಕ್ತ, &end, ರಿಕ್ತ);
			  ಭಙ್ಗ;
		ಪಕ್ಷ 'g': ಯದಿ (vim_strchr(name, AUTOLOAD_CHAR) == ರಿಕ್ತ)
			  {
			      ಯದಿ (is_ಅಭಿವ್ಯಕ್ತಿ && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != ರಿಕ್ತ
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      ಅನ್ಯಥಾ
				  isn_type = ISN_LOADG;
			  }
			  ಅನ್ಯಥಾ
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      ಯದಿ (name == ರಿಕ್ತ)
				  ಗಚ್ಛ theend;
			  }
			  ಭಙ್ಗ;
		ಪಕ್ಷ 'w': isn_type = ISN_LOADW; ಭಙ್ಗ;
		ಪಕ್ಷ 't': isn_type = ISN_LOADT; ಭಙ್ಗ;
		ಪಕ್ಷ 'b': isn_type = ISN_LOADB; ಭಙ್ಗ;
		ಸ್ವಭಾವ:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  ಗಚ್ಛ theend;
	    }
	    ಯದಿ (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	ಪೂರ್ಣಾಙ್ಕ	    idx;
	ಪೂರ್ಣಾಙ್ಕ	    method_idx;
	ಪೂರ್ಣಾಙ್ಕ	    gen_load = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	    gen_load_outer = 0;
	ಪೂರ್ಣಾಙ್ಕ	    outer_loop_depth = -1;
	ಪೂರ್ಣಾಙ್ಕ	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	ಯದಿ (name == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	ಯದಿ (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_super);
	    ಯದಿ (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr(name, AUTOLOAD_CHAR) != ರಿಕ್ತ)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	ಅನ್ಯಥಾ ಯದಿ (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    ಯದಿ (gen_load_outer == 0)
		gen_load = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    lvar_T  lvar;
	    class_T *cl = ರಿಕ್ತ;

	    ಯದಿ (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		ಯದಿ (lvar.lv_ufunc != ರಿಕ್ತ && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		ಅನ್ಯಥಾ ಯದಿ (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		ಅನ್ಯಥಾ
		    gen_load = TRUE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (cctx->ctx_ufunc->uf_defclass != ರಿಕ್ತ &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		ಯದಿ (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    ಯದಿ (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    ಅನ್ಯಥಾ
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									ರಿಕ್ತ);
		    }
		}
		ಅನ್ಯಥಾ
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		imported_T *imp = ರಿಕ್ತ;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		ಯದಿ (script_var_exists(*arg, namelen, cctx, ರಿಕ್ತ) == OK
			    || (imp = find_imported(name, 0, FALSE)) != ರಿಕ್ತ)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		ಯದಿ (res == FAIL && is_ಅಭಿವ್ಯಕ್ತಿ && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	ಯದಿ (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, ರಿಕ್ತ, type);
	ಯದಿ (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    ಯದಿ (lt != ರಿಕ್ತ)
	*lt = '<';
    ಯದಿ (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_string(isn_T *isn, cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    ಪೂರ್ಣಾಙ್ಕ		ಅಭಿವ್ಯಕ್ತಿ_res;
    ಪೂರ್ಣಾಙ್ಕ		trailing_error;
    ಪೂರ್ಣಾಙ್ಕ		instr_ಎಣಿಕೆ;
    isn_T	*instr = ರಿಕ್ತ;

    // Remove the string type from the stack.
    --cctx->ctx_type_ರಾಶಿ.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = ರಿಕ್ತ;

    // avoid peeking a next line
    ಪೂರ್ಣಾಙ್ಕ galen_save = cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len;
    cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len = 0;

    ಅಭಿವ್ಯಕ್ತಿ_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    ಯದಿ (ಅಭಿವ್ಯಕ್ತಿ_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	ಯದಿ (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_ರಾಶಿ.ga_len;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_ಎಣಿಕೆ = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_ಎಣಿಕೆ].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    ಪೂರ್ಣಾಙ್ಕ
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	ಪೂರ್ಣಾಙ್ಕ	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    ಪೂರ್ಣಾಙ್ಕ	    must_end = FALSE;
    ಪೂರ್ಣಾಙ್ಕ	    instr_ಎಣಿಕೆ;

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (may_get_next_ಸಾಲು(whitep, &p, cctx) == FAIL)
	    ಗಚ್ಛ failret;
	ಯದಿ (*p == ')')
	{
	    *arg = p + 1;
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
	ಯದಿ (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	instr_ಎಣಿಕೆ = cctx->ctx_instr.ga_len;
	ಯದಿ (compile_expr0(&p, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	++*argcount;

	ಯದಿ (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_ಎಣಿಕೆ + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_ಎಣಿಕೆ;

	    // {skip} argument of searchpair() can be compiled if not empty
	    ಯದಿ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ರಿಕ್ತ
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	ಅನ್ಯಥಾ ಯದಿ (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_ಎಣಿಕೆ + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_ಎಣಿಕೆ;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    ಯದಿ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ರಿಕ್ತ
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	ಯದಿ (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (*p == ',')
	{
	    ++p;
	    ಯದಿ (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಅನ್ಯಥಾ
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_ರಾಶಿ(cctx, 0);
    ಪೂರ್ಣಾಙ್ಕ		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    ಯದಿ (type->tt_type == VAR_OBJECT)
    {
	ಪೂರ್ಣಾಙ್ಕ	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	ಯದಿ (uf != ರಿಕ್ತ)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    ಪ್ರತ್ಯಾಗಮ res;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	ಪೂರ್ಣಾಙ್ಕ	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ರಿಕ್ತ;
    char_u	*gname = ರಿಕ್ತ;
    ufunc_T	*ufunc = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		res = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		is_autoload;
    ಪೂರ್ಣಾಙ್ಕ		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    ಯದಿ (varlen >= ಪರಿಮಾಣ(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    ಯದಿ (import != ರಿಕ್ತ)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    ಯದಿ ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	ಪೂರ್ಣಾಙ್ಕ	    is_has = **arg == 'h';
	ಪೂರ್ಣಾಙ್ಕ	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	ಯದಿ (*s == '"')
	    (ಶೂನ್ಯ)eval_string(&s, &argvars[0], TRUE, FALSE);
	ಅನ್ಯಥಾ ಯದಿ (*s == '\'')
	    (ಶೂನ್ಯ)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	ಯದಿ (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    ಯದಿ (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_ಸಂಖ್ಯೆ;
		tv->vval.v_ಸಂಖ್ಯೆ = 0;
		ಯದಿ (is_has)
		    f_has(argvars, tv);
		ಅನ್ಯಥಾ ಯದಿ (is_len)
		    f_len(argvars, tv);
		ಅನ್ಯಥಾ
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
	clear_tv(&argvars[0]);
	ಯದಿ (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    ಯದಿ ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    ಅನ್ಯಥಾ ಯದಿ (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    ಅನ್ಯಥಾ
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    ಯದಿ (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	ಯದಿ (*arg == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	ಯದಿ (gname == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;
	++*arg;			// skip '('
    }
    ಅನ್ಯಥಾ
	*arg += varlen + 1;

    ಯದಿ (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	ಗಚ್ಛ theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != ರಿಕ್ತ;
    ಯದಿ (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	ಪೂರ್ಣಾಙ್ಕ	    idx;

	// builtin function
	idx = find_internal_func(name);
	ಯದಿ (idx >= 0)
	{
	    ಯದಿ (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		ಗಚ್ಛ theend;
	    }

	    ಯದಿ (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_ರಾಶಿ(cctx, 1);
		ಯದಿ (check_type_is_ಮೌಲ್ಯ(get_type_on_ರಾಶಿ(cctx, 0)) == FAIL)
		    ಗಚ್ಛ theend;

		// add() can be compiled to instructions if we know the type
		ಯದಿ (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		ಅನ್ಯಥಾ ಯದಿ (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    ಯದಿ ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		ಯದಿ (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    ಯದಿ (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    ಅನ್ಯಥಾ ಯದಿ (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    ಯದಿ (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		ಯದಿ (res == OK)
		    idx = -1;
	    }

	    ಯದಿ (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	ಅನ್ಯಥಾ
	    emsg_funcname(e_unknown_function_str, namebuf);
	ಗಚ್ಛ theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    ಯದಿ (lookup_local(namebuf, varlen, ರಿಕ್ತ, cctx) == FAIL
	    && arg_exists(namebuf, varlen, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, cctx) != OK)
    {
	class_T		*cl = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	ಯದಿ (ufunc != ರಿಕ್ತ)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ಯದಿ (ufunc == ರಿಕ್ತ)
		ಗಚ್ಛ theend;

	    ಯದಿ (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, ರಿಕ್ತ, 0, argcount, FALSE);
		ಗಚ್ಛ theend;
	    }
	    ಯದಿ (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ರಿಕ್ತ
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		ಗಚ್ಛ theend;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    ಯದಿ (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], ರಿಕ್ತ,
							0, argcount, FALSE);
	    }
	    ಅನ್ಯಥಾ
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    ಗಚ್ಛ theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == ರಿಕ್ತ && gname != ರಿಕ್ತ ? gname : namebuf;
    ಯದಿ (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_ರಾಶಿ(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	ಗಚ್ಛ theend;
    }

    // If we can find a global function by name generate the right call.
    ಯದಿ (ufunc != ರಿಕ್ತ)
    {
	res = generate_CALL(cctx, ufunc, ರಿಕ್ತ, 0, argcount, FALSE);
	ಗಚ್ಛ theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    ಯದಿ (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    ಅನ್ಯಥಾ
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    ಪ್ರತ್ಯಾಗಮ res;
}

// like NAMESPACE_CHAR but with 'a' and 'l'.
#define VIM9_NAMESPACE_CHAR	(char_u *)"bgstvw"

/*
 * Find the end of a variable or function name.  Unlike find_name_end() this
 * does not recognize magic braces.
 * When "use_namespace" is TRUE recognize "b:", "s:", etc.
 * Return a pointer to just after the name.  Equal to "arg" if there is no
 * valid name.
 */
    char_u *
to_name_end(char_u *arg, ಪೂರ್ಣಾಙ್ಕ use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    ಯದಿ (!eval_isnamec1(*arg))
	ಪ್ರತ್ಯಾಗಮ arg;

    ಪ್ರತಿ (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	ಯದಿ (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == ರಿಕ್ತ))
	    ಭಙ್ಗ;
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * Like to_name_end() but also skip over a list or dict constant.
 * Also accept "<SNR>123_Func".
 * This intentionally does not handle line continuation.
 */
    char_u *
to_name_const_end(char_u *arg)
{
    char_u	*p = arg;
    typval_T	rettv;

    ಯದಿ (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    ಯದಿ (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	ಯದಿ (eval_list(&p, &rettv, ರಿಕ್ತ, FALSE) == FAIL)
	    p = arg;
    }
    ಪ್ರತ್ಯಾಗಮ p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		is_const;
    ಪೂರ್ಣಾಙ್ಕ		is_all_const = TRUE;	// reset when non-const encountered
    ಪೂರ್ಣಾಙ್ಕ		must_end = FALSE;

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (may_get_next_ಸಾಲು(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (*p == ']')
	{
	    ++p;
	    ಭಙ್ಗ;
	}
	ಯದಿ (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (!is_const)
	    is_all_const = FALSE;
	++ಎಣಿಕೆ;
	ಯದಿ (*p == ',')
	{
	    ++p;
	    ಯದಿ (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಅನ್ಯಥಾ
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ಪ್ರತ್ಯಾಗಮ generate_NEWLIST(cctx, ಎಣಿಕೆ, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    ಪೂರ್ಣಾಙ್ಕ first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		is_const;
    ಪೂರ್ಣಾಙ್ಕ		is_all_const = TRUE;	// reset when non-const encountered
    ಪೂರ್ಣಾಙ್ಕ		must_end = FALSE;

    ಯದಿ (**arg != ')')
    {
	ಯದಿ (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಎಣಿಕೆ = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	ಪ್ರತಿ (;;)
	{
	    ಯದಿ (may_get_next_ಸಾಲು(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (*p == ')')
	    {
		++p;
		ಭಙ್ಗ;
	    }
	    ಯದಿ (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (!is_const)
		is_all_const = FALSE;
	    ++ಎಣಿಕೆ;
	    ಯದಿ (*p == ',')
	    {
		++p;
		ಯದಿ (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	    ಅನ್ಯಥಾ
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ಪ್ರತ್ಯಾಗಮ generate_NEWTUPLE(cctx, ಎಣಿಕೆ, FALSE);
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

    ಯದಿ (!evalarg->eval_using_cmdline || cctx == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    gap = &evalarg->eval_tofree_ga;
    ಯದಿ (gap->ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    ಸಾಲು = ((char_u **)cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_data)[cctx->ctx_lnum];
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
    ಪೂರ್ಣಾಙ್ಕ
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಙ್ಕ		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    ಯದಿ (r != OK)
    {
	clear_evalarg(&evalarg, ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ r;
    }

    ಯದಿ (ppconst != ರಿಕ್ತ && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    ಯದಿ (cctx->ctx_ufunc != ರಿಕ್ತ)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	ಪೂರ್ಣಾಙ್ಕ block_depth = cctx->ctx_ufunc->uf_block_depth;

	ಯದಿ (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(ಪೂರ್ಣಾಙ್ಕ, block_depth);
	    ಯದಿ (ufunc->uf_block_ids != ರಿಕ್ತ)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ಯದಿ (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    ಯದಿ (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    ಯದಿ (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	ಯದಿ (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, ರಿಕ್ತ);

    ಯದಿ (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	ಪ್ರತ್ಯಾಗಮ generate_FUNCREF(cctx, ufunc, ರಿಕ್ತ, FALSE, 0, ರಿಕ್ತ);
    }

    func_ptr_unref(ufunc);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	ಪೂರ್ಣಾಙ್ಕ	    types_optional,
	evalarg_T   *evalarg)
{
    ಪೂರ್ಣಾಙ್ಕ		r;
    ufunc_T	*ufunc;
    ಪೂರ್ಣಾಙ್ಕ		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, ರಿಕ್ತ);
    current_sctx.sc_version = save_sc_version;
    ಯದಿ (r != OK)
	ಪ್ರತ್ಯಾಗಮ r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ಯದಿ (ufunc->uf_ret_type == ರಿಕ್ತ || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, ರಿಕ್ತ);

    ಯದಿ (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    clear_tv(rettv);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		is_const;
    ಪೂರ್ಣಾಙ್ಕ		is_all_const = TRUE;	// reset when non-const encountered

    ಯದಿ (d == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತಿ (;;)
    {
	char_u	    *ಕೀಲಿ = ರಿಕ್ತ;

	ಯದಿ (may_get_next_ಸಾಲು(whitep, arg, cctx) == FAIL)
	{
	    *arg = ರಿಕ್ತ;
	    ಗಚ್ಛ failret;
	}

	ಯದಿ (**arg == '}')
	    ಭಙ್ಗ;

	ಯದಿ (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    ಯದಿ (compile_expr0(arg, cctx) == FAIL)
		ಗಚ್ಛ failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    ಯದಿ (isn->isn_type == ISN_PUSHNR)
	    {
		ವರ್ಣ buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.ಸಂಖ್ಯೆ);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    ಯದಿ (isn->isn_type == ISN_PUSHS)
		ಕೀಲಿ = isn->isn_arg.string;
	    ಅನ್ಯಥಾ ಯದಿ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		ಗಚ್ಛ failret;
	    *arg = skipwhite(*arg);
	    ಯದಿ (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_ಕೀಲಿ));
		ಗಚ್ಛ failret;
	    }
	    ++*arg;
	}
	ಅನ್ಯಥಾ
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    ಕೀಲಿ = get_literal_ಕೀಲಿ(arg);
	    ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
		ಗಚ್ಛ failret;
	    ಯದಿ (generate_PUSHS(cctx, &ಕೀಲಿ) == FAIL)
		ಗಚ್ಛ failret;
	}

	// Check for duplicate keys, if using string keys.
	ಯದಿ (ಕೀಲಿ != ರಿಕ್ತ)
	{
	    item = dict_find(d, ಕೀಲಿ, -1);
	    ಯದಿ (item != ರಿಕ್ತ)
	    {
		semsg(_(e_duplicate_ಕೀಲಿ_in_dictionary_str), ಕೀಲಿ);
		ಗಚ್ಛ failret;
	    }
	    item = dictitem_alloc(ಕೀಲಿ);
	    ಯದಿ (item != ರಿಕ್ತ)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		ಯದಿ (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	ಯದಿ (**arg != ':')
	{
	    ಯದಿ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ಅನ್ಯಥಾ
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    ಗಚ್ಛ failret;
	}
	whitep = *arg + 1;
	ಯದಿ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    ಗಚ್ಛ failret;
	}

	ಯದಿ (may_get_next_ಸಾಲು(whitep, arg, cctx) == FAIL)
	{
	    *arg = ರಿಕ್ತ;
	    ಗಚ್ಛ failret;
	}

	ಯದಿ (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    ಗಚ್ಛ failret;
	ಯದಿ (!is_const)
	    is_all_const = FALSE;
	++ಎಣಿಕೆ;

	whitep = *arg;
	ಯದಿ (may_get_next_ಸಾಲು(whitep, arg, cctx) == FAIL)
	{
	    *arg = ರಿಕ್ತ;
	    ಗಚ್ಛ failret;
	}
	ಯದಿ (**arg == '}')
	    ಭಙ್ಗ;
	ಯದಿ (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    ಗಚ್ಛ failret;
	}
	ಯದಿ (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    ಗಚ್ಛ failret;
	}
	whitep = *arg + 1;
	ಯದಿ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    ಗಚ್ಛ failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    ಯದಿ (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    ಪ್ರತ್ಯಾಗಮ generate_NEWDICT(cctx, ಎಣಿಕೆ, FALSE);

failret:
    ಯದಿ (*arg == ರಿಕ್ತ)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Compile "&option".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    ಪೂರ್ಣಾಙ್ಕ		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    ಯದಿ (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_ಸಂಖ್ಯೆ ? &t_ಸಂಖ್ಯೆ : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Compile "$VAR".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    ಯದಿ (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Compile $"string" or $'string'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    ಪೂರ್ಣಾಙ್ಕ		quote;
    ಪೂರ್ಣಾಙ್ಕ		evaluate = cctx->ctx_skip != SKIP_YES;
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ಪ್ರತಿ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ಯದಿ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ಅನ್ಯಥಾ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ಯದಿ (ret == FAIL)
	    ಭಙ್ಗ;
	ಯದಿ (evaluate)
	{
	    ಯದಿ ((tv.vval.v_string != ರಿಕ್ತ && *tv.vval.v_string != NUL)
		    || (**arg != '{' && ಎಣಿಕೆ == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		ಯದಿ (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		tv.vval.v_string = ರಿಕ್ತ;  // don't free it now
		++ಎಣಿಕೆ;
	    }
	    clear_tv(&tv);
	}

	ಯದಿ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ಭಙ್ಗ;
	}

	p = compile_one_ಅಭಿವ್ಯಕ್ತಿ_in_str(*arg, cctx);
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    ret = FAIL;
	    ಭಙ್ಗ;
	}
	++ಎಣಿಕೆ;
	*arg = p;
    }

    ಯದಿ (ret == FAIL || !evaluate)
	ಪ್ರತ್ಯಾಗಮ ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ಯದಿ (ಎಣಿಕೆ > 1)
	ಪ್ರತ್ಯಾಗಮ generate_CONCAT(cctx, ಎಣಿಕೆ);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compile "@r".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_get_register(char_u **arg, cctx_T *cctx)
{
    ಪೂರ್ಣಾಙ್ಕ		ret;

    ++*arg;
    ಯದಿ (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, ರಿಕ್ತ, &t_string);
    ++*arg;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
apply_leader(typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    ಯಾವತ್ (p > start)
    {
	--p;
	ಯದಿ (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    ಯದಿ (rettv->v_type == VAR_FLOAT)
	    {
		ಯದಿ (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    ಅನ್ಯಥಾ
	    {
		varnumber_T	val;
		ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		ಯದಿ (check_not_string(rettv) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		val = tv_get_ಸಂಖ್ಯೆ_chk(rettv, &error);
		clear_tv(rettv);
		ಯದಿ (error)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (*p == '-')
		    val = -val;
		rettv->v_type = VAR_ಸಂಖ್ಯೆ;
		rettv->vval.v_ಸಂಖ್ಯೆ = val;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (numeric_only)
	{
	    ++p;
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '!')
	{
	    ಪೂರ್ಣಾಙ್ಕ v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ಸಂಖ್ಯೆ = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_vim_constant(char_u **arg, typval_T *rettv)
{
    ಯದಿ (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
	*arg += 6;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
	*arg += 7;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NULL;
	*arg += 6;
    }
    ಅನ್ಯಥಾ ಯದಿ (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, ಪೂರ್ಣಾಙ್ಕ *len, ಪೂರ್ಣಾಙ್ಕ *type_is)
{
    exprtype_T	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ವಿಕಲ್ಪ (p[0])
    {
	ಪಕ್ಷ '=':   ಯದಿ (p[1] == '=')
			type = ಅಭಿವ್ಯಕ್ತಿ_EQUAL;
		    ಅನ್ಯಥಾ ಯದಿ (p[1] == '~')
			type = ಅಭಿವ್ಯಕ್ತಿ_MATCH;
		    ಭಙ್ಗ;
	ಪಕ್ಷ '!':   ಯದಿ (p[1] == '=')
			type = ಅಭಿವ್ಯಕ್ತಿ_NEQUAL;
		    ಅನ್ಯಥಾ ಯದಿ (p[1] == '~')
			type = ಅಭಿವ್ಯಕ್ತಿ_NOMATCH;
		    ಭಙ್ಗ;
	ಪಕ್ಷ '>':   ಯದಿ (p[1] != '=')
		    {
			type = ಅಭಿವ್ಯಕ್ತಿ_GREATER;
			*len = 1;
		    }
		    ಅನ್ಯಥಾ
			type = ಅಭಿವ್ಯಕ್ತಿ_GEQUAL;
		    ಭಙ್ಗ;
	ಪಕ್ಷ '<':   ಯದಿ (p[1] != '=')
		    {
			type = ಅಭಿವ್ಯಕ್ತಿ_SMALLER;
			*len = 1;
		    }
		    ಅನ್ಯಥಾ
			type = ಅಭಿವ್ಯಕ್ತಿ_SEQUAL;
		    ಭಙ್ಗ;
	ಪಕ್ಷ 'i':   ಯದಿ (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			ಯದಿ (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			ಯದಿ (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? ಅಭಿವ್ಯಕ್ತಿ_IS : ಅಭಿವ್ಯಕ್ತಿ_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ type;
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
    clear_evalarg(&evalarg, ರಿಕ್ತ);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    ಪೂರ್ಣಾಙ್ಕ
bool_on_ರಾಶಿ(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_ರಾಶಿ(cctx, 0);
    ಯದಿ (type == &t_bool)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_ಸಂಖ್ಯೆ
	    || type == &t_ಸಂಖ್ಯೆ_bool
	    || type == &t_const_ಸಂಖ್ಯೆ_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	ಪ್ರತ್ಯಾಗಮ generate_COND2BOOL(cctx);

    ಪ್ರತ್ಯಾಗಮ need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    ಶೂನ್ಯ
error_white_both(char_u *op, ಪೂರ್ಣಾಙ್ಕ len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_leader(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    ಯಾವತ್ (p > start)
    {
	--p;
	ಯಾವತ್ (VIM_ISWHITE(*p))
	    --p;
	ಯದಿ (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಯದಿ (type->tt_type != VAR_FLOAT && need_type(type, &t_ಸಂಖ್ಯೆ,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    ಯದಿ (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (numeric_only)
	{
	    ++p;
	    ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ  invert = *p == '!';

	    ಯಾವತ್ (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		ಯದಿ (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    ಯದಿ (check_type_is_ಮೌಲ್ಯ(get_type_on_ರಾಶಿ(cctx, 0)) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (generate_2BOOL(cctx, invert, -1) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    *end = p;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಙ್ಕ	    ret;
    char_u  *p = *arg + 1;

    ಯದಿ (may_get_next_ಸಾಲು_error(p, arg, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (**arg == ')')
	// empty tuple
	ಪ್ರತ್ಯಾಗಮ compile_tuple(arg, cctx, ppconst, FALSE);

    ಯದಿ (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    ಅನ್ಯಥಾ
    {
	// Not enough space in ppconst, flush constants.
	ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ret = compile_expr0(arg, cctx);
    }
    ಯದಿ (may_get_next_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (ret == OK && **arg == ',')
    {
	// tuple
	ಪೂರ್ಣಾಙ್ಕ is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪ್ರತ್ಯಾಗಮ compile_tuple(arg, cctx, ppconst, is_const);
    }

    ಯದಿ (**arg == ')')
	++*arg;
    ಅನ್ಯಥಾ ಯದಿ (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ ret;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    ಪೂರ್ಣಾಙ್ಕ		keeping_dict = FALSE;

    ಪ್ರತಿ (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	ಯದಿ (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_ಸಾಲು_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    ಯದಿ (next != ರಿಕ್ತ &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_ಸಾಲು_from_context(cctx, TRUE);
		ಯದಿ (next == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	ಯದಿ (**arg == '(')
	{
	    ಪೂರ್ಣಾಙ್ಕ		argcount = 0;

	    ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_ರಾಶಿ(cctx, 0);

	    *arg = skipwhite(p + 1);
	    ಯದಿ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ಯದಿ (generate_instr(cctx, ISN_CLEARDICT) == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    ಪೂರ್ಣಾಙ್ಕ	    alt;
	    char_u  *paren;

	    // something->method()
	    ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಯದಿ (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    ಯದಿ (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    ಯದಿ (**arg == '(')
		alt = 2;
	    ಅನ್ಯಥಾ
	    {
		// alternative 1 or 3
		p = *arg;
		ಯದಿ (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಯದಿ (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		ಪ್ರತಿ ( ; eval_isnamec(*p); ++p)
		    ;
		ಯದಿ (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    ಯದಿ (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಅನ್ಯಥಾ
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    ಯದಿ (paren == ರಿಕ್ತ)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		}
	    }

	    ಯದಿ (alt != 1)
	    {
		ಪೂರ್ಣಾಙ್ಕ	    argcount = 1;
		garray_T    *ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
		ಪೂರ್ಣಾಙ್ಕ	    type_idx_start = ರಾಶಿ->ga_len;
		ಪೂರ್ಣಾಙ್ಕ	    ಅಭಿವ್ಯಕ್ತಿ_isn_start = cctx->ctx_instr.ga_len;
		ಪೂರ್ಣಾಙ್ಕ	    ಅಭಿವ್ಯಕ್ತಿ_isn_end;
		ಪೂರ್ಣಾಙ್ಕ	    arg_isn_ಎಣಿಕೆ;

		ಯದಿ (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    ಯದಿ (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಅನ್ಯಥಾ
		{
		    ಪೂರ್ಣಾಙ್ಕ fail;
		    ಪೂರ್ಣಾಙ್ಕ save_len = cctx->ctx_ufunc->uf_ಸಾಲುಗಳು.ga_len;
		    ಪೂರ್ಣಾಙ್ಕ	prev_did_emsg = did_emsg;

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

		    ಯದಿ (fail)
		    {
			ಯದಿ (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		}

		// Compile the arguments.
		ಯದಿ (**arg != '(')
		{
		    ಯದಿ (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    ಅನ್ಯಥಾ
			semsg(_(e_missing_parenthesis_str), *arg);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		ಅಭಿವ್ಯಕ್ತಿ_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		ಯದಿ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_ಎಣಿಕೆ = cctx->ctx_instr.ga_len - ಅಭಿವ್ಯಕ್ತಿ_isn_end;
		ಯದಿ (arg_isn_ಎಣಿಕೆ > 0)
		{
		    ಪೂರ್ಣಾಙ್ಕ	    ಅಭಿವ್ಯಕ್ತಿ_isn_ಎಣಿಕೆ = ಅಭಿವ್ಯಕ್ತಿ_isn_end - ಅಭಿವ್ಯಕ್ತಿ_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, ಅಭಿವ್ಯಕ್ತಿ_isn_ಎಣಿಕೆ);
		    type_T  *decl_type;
		    type2_T  *typep;

		    ಯದಿ (isn == ರಿಕ್ತ)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + ಅಭಿವ್ಯಕ್ತಿ_isn_start,
					       ಪರಿಮಾಣ(isn_T) * ಅಭಿವ್ಯಕ್ತಿ_isn_ಎಣಿಕೆ);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + ಅಭಿವ್ಯಕ್ತಿ_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + ಅಭಿವ್ಯಕ್ತಿ_isn_end,
						ಪರಿಮಾಣ(isn_T) * arg_isn_ಎಣಿಕೆ);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + ಅಭಿವ್ಯಕ್ತಿ_isn_start + arg_isn_ಎಣಿಕೆ,
					  isn, ಪರಿಮಾಣ(isn_T) * ಅಭಿವ್ಯಕ್ತಿ_isn_ಎಣಿಕೆ);
		    vim_free(isn);

		    typep = ((type2_T *)ರಾಶಿ->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)ರಾಶಿ->ga_data) + type_idx_start,
			      ((type2_T *)ರಾಶಿ->ga_data) + type_idx_start + 1,
			      ಪರಿಮಾಣ(type2_T)
				       * (ರಾಶಿ->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)ರಾಶಿ->ga_data) + ರಾಶಿ->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_ರಾಶಿ(cctx, 0);
		ಯದಿ (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    ಯದಿ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ಯದಿ (generate_instr(cctx, ISN_CLEARDICT) == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (**arg == '[')
	{
	    ಪೂರ್ಣಾಙ್ಕ		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    ಯದಿ (may_get_next_ಸಾಲು_error(p, arg, cctx) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (compile_expr0(arg, cctx) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಯದಿ (may_get_next_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		*arg = skipwhite(*arg);
	    }
	    ಯದಿ (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		ಯದಿ (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಯದಿ (may_get_next_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (compile_expr0(arg, cctx) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಯದಿ (may_get_next_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    ಯದಿ (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    *arg = *arg + 1;

	    ಯದಿ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ಯದಿ (generate_instr(cctx, ISN_CLEARDICT) == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ರಾಶಿ(cctx, 0);
	    ಯದಿ (type != &t_unknown
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
		ಯದಿ (compile_class_object_index(cctx, arg, type) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		*arg = p + 1;
		ಯದಿ (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		p = *arg;
		ಯಾವತ್ (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		ಯದಿ (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		ಯದಿ (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    ಯದಿ (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    ಪೂರ್ಣಾಙ್ಕ		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    ಯದಿ (eval_leader(arg, TRUE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    ವಿಕಲ್ಪ (**arg)
    {
	/*
	 * Number constant.
	 */
	ಪಕ್ಷ '0':	// also for blob starting with 0z
	ಪಕ್ಷ '1':
	ಪಕ್ಷ '2':
	ಪಕ್ಷ '3':
	ಪಕ್ಷ '4':
	ಪಕ್ಷ '5':
	ಪಕ್ಷ '6':
	ಪಕ್ಷ '7':
	ಪಕ್ಷ '8':
	ಪಕ್ಷ '9':
	ಪಕ್ಷ '.':   ಯದಿ (eval_ಸಂಖ್ಯೆ(arg, rettv, TRUE, FALSE) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    ಯದಿ (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    }
		    ಭಙ್ಗ;

	/*
	 * String constant: "string".
	 */
	ಪಕ್ಷ '"':   ಯದಿ (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಭಙ್ಗ;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	ಪಕ್ಷ '\'':  ಯದಿ (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ಭಙ್ಗ;

	/*
	 * Constant Vim variable.
	 */
	ಪಕ್ಷ 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    ಭಙ್ಗ;

	/*
	 * "true" constant
	 */
	ಪಕ್ಷ 't':   ಯದಿ (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_TRUE;
		    }
		    ಅನ್ಯಥಾ
			ret = NOTDONE;
		    ಭಙ್ಗ;

	/*
	 * "false" constant
	 */
	ಪಕ್ಷ 'f':   ಯದಿ (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_ಸಂಖ್ಯೆ = VVAL_FALSE;
		    }
		    ಅನ್ಯಥಾ
			ret = NOTDONE;
		    ಭಙ್ಗ;

	/*
	 * "null" or "null_*" constant
	 */
	ಪಕ್ಷ 'n':   ಯದಿ (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			ಪೂರ್ಣಾಙ್ಕ	len;

			ಪ್ರತಿ (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			ಯದಿ (ret == FAIL)
			    ret = NOTDONE;
			ಅನ್ಯಥಾ
			    *arg += len + 4;
		    }
		    ಅನ್ಯಥಾ
			ret = NOTDONE;
		    ಭಙ್ಗ;

	/*
	 * List: [expr, expr]
	 */
	ಪಕ್ಷ '[':   ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    ಭಙ್ಗ;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	ಪಕ್ಷ '{':   ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
			ಪ್ರತ್ಯಾಗಮ FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    ಭಙ್ಗ;

	/*
	 * Option value: &name
	 */
	ಪಕ್ಷ '&':	ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
			ret = compile_get_option(arg, cctx);
			ಭಙ್ಗ;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	ಪಕ್ಷ '$':	ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
			ಯದಿ ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			ಅನ್ಯಥಾ
			    ret = compile_get_env(arg, cctx);
			ಭಙ್ಗ;

	/*
	 * Register contents: @r.
	 */
	ಪಕ್ಷ '@':	ಯದಿ (generate_ppconst(cctx, ppconst) == FAIL)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
			ret = compile_get_register(arg, cctx);
			ಭಙ್ಗ;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	ಪಕ್ಷ '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    ಯದಿ (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    ಭಙ್ಗ;

	ಸ್ವಭಾವ:    ret = NOTDONE;
		    ಭಙ್ಗ;
    }
    ಯದಿ (ret == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	ಯದಿ (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	ಅನ್ಯಥಾ
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    ಅನ್ಯಥಾ ಯದಿ (ret == NOTDONE)
    {
	char_u	    *p;
	ಪೂರ್ಣಾಙ್ಕ	    r;

	ಯದಿ (!eval_isnamec1(**arg))
	{
	    ಯದಿ (!vim9_bad_comment(*arg))
	    {
		ಯದಿ (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		ಅನ್ಯಥಾ
		    semsg(_(e_name_expected_str), *arg);
	    }
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	ಯದಿ (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	size_t namelen = p - *arg;
	ಯದಿ (*p == '<')
	{
	    ಯದಿ (skip_generic_func_type_args(&p) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	ಯದಿ (r == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    ಯದಿ (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	ಯದಿ (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
    ಯದಿ (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = ರಿಕ್ತ;

    // Recognize <type>
    ಯದಿ (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ಯದಿ (want_type == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (**arg != '>')
	{
	    ಯದಿ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    ಅನ್ಯಥಾ
		emsg(_(e_missing_gt));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	++*arg;
	ಯದಿ (may_get_next_ಸಾಲು_error(*arg, arg, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (compile_expr9(arg, cctx, ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (want_type != ರಿಕ್ತ)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_ರಾಶಿ(cctx, 0);
	ಯದಿ (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    ಯದಿ (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    ಪೂರ್ಣಾಙ್ಕ		ppconst_used = ppconst->pp_used;

    // get the first expression
    ಯದಿ (compile_expr8(arg, cctx, ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    ಪ್ರತಿ (;;)
    {
	op = may_peek_next_ಸಾಲು(cctx, *arg, &next);
	ಯದಿ (*op != '*' && *op != '/' && *op != '%')
	    ಭಙ್ಗ;
	ಯದಿ (next != ರಿಕ್ತ)
	{
	    *arg = next_ಸಾಲು_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ಯದಿ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (may_get_next_ಸಾಲು_error(op + 1, arg, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// get the second expression
	ಯದಿ (compile_expr8(arg, cctx, ppconst) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_ಸಂಖ್ಯೆ
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_ಸಂಖ್ಯೆ)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    ಪೂರ್ಣಾಙ್ಕ		    failed = FALSE;

	    // both are numbers: compute the result
	    ವಿಕಲ್ಪ (*op)
	    {
		ಪಕ್ಷ '*': res = tv1->vval.v_ಸಂಖ್ಯೆ * tv2->vval.v_ಸಂಖ್ಯೆ;
			  ಭಙ್ಗ;
		ಪಕ್ಷ '/': res = ಅಂಕೆ_divide(tv1->vval.v_ಸಂಖ್ಯೆ,
						  tv2->vval.v_ಸಂಖ್ಯೆ, &failed);
			  ಭಙ್ಗ;
		ಪಕ್ಷ '%': res = ಅಂಕೆ_modulus(tv1->vval.v_ಸಂಖ್ಯೆ,
						  tv2->vval.v_ಸಂಖ್ಯೆ, &failed);
			  ಭಙ್ಗ;
	    }
	    ಯದಿ (failed)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    tv1->vval.v_ಸಂಖ್ಯೆ = res;
	    --ppconst->pp_used;
	}
	ಅನ್ಯಥಾ
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    ಪೂರ್ಣಾಙ್ಕ		oplen;
    ಪೂರ್ಣಾಙ್ಕ		ppconst_used = ppconst->pp_used;

    // get the first variable
    ಯದಿ (compile_expr7(arg, cctx, ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    ಪ್ರತಿ (;;)
    {
	op = may_peek_next_ಸಾಲು(cctx, *arg, &next);
	ಯದಿ (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    ಭಙ್ಗ;
	ಯದಿ (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    ಭಙ್ಗ;
	oplen = (*op == '.' ? 2 : 1);
	ಯದಿ (next != ರಿಕ್ತ)
	{
	    *arg = next_ಸಾಲು_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ಯದಿ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (may_get_next_ಸಾಲು_error(op + oplen, arg, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	// get the second expression
	ಯದಿ (compile_expr7(arg, cctx, ppconst) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_ಸಂಖ್ಯೆ
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_ಸಂಖ್ಯೆ)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    ಯದಿ (*op == '+')
		tv1->vval.v_ಸಂಖ್ಯೆ = tv1->vval.v_ಸಂಖ್ಯೆ + tv2->vval.v_ಸಂಖ್ಯೆ;
	    ಅನ್ಯಥಾ ಯದಿ (*op == '-')
		tv1->vval.v_ಸಂಖ್ಯೆ = tv1->vval.v_ಸಂಖ್ಯೆ - tv2->vval.v_ಸಂಖ್ಯೆ;
	    ಅನ್ಯಥಾ
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		ಯದಿ (tv1->vval.v_string == ರಿಕ್ತ)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	ಅನ್ಯಥಾ
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    ಯದಿ (*op == '.')
	    {
		ಯದಿ (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (generate_CONCAT(cctx, 2) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಅನ್ಯಥಾ
		generate_two_op(cctx, op);
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;
    char_u	*p;
    char_u	*next;
    ಪೂರ್ಣಾಙ್ಕ		len = 2;
    ಪೂರ್ಣಾಙ್ಕ		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    ಯದಿ (compile_expr6(arg, cctx, ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    ಪ್ರತಿ (;;)
    {
	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;

	p = may_peek_next_ಸಾಲು(cctx, *arg, &next);
	ಯದಿ (p[0] == '<' && p[1] == '<')
	    type = ಅಭಿವ್ಯಕ್ತಿ_LSHIFT;
	ಅನ್ಯಥಾ ಯದಿ (p[0] == '>' && p[1] == '>')
	    type = ಅಭಿವ್ಯಕ್ತಿ_RSHIFT;

	ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN)
	    ಪ್ರತ್ಯಾಗಮ OK;

	// Handle a bitwise left or right shift operator
	ಯದಿ (ppconst->pp_used == ppconst_used + 1)
	{
	    ಯದಿ (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_ಸಂಖ್ಯೆ)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    type_T	*t = get_type_on_ರಾಶಿ(cctx, 0);

	    ಯದಿ (need_type(t, &t_ಸಂಖ್ಯೆ, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	ಯದಿ (next != ರಿಕ್ತ)
	{
	    *arg = next_ಸಾಲು_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ಯದಿ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// get the second variable
	ಯದಿ (may_get_next_ಸಾಲು_error(p + len, arg, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (compile_expr6(arg, cctx, ppconst) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    ಯದಿ (tv2->v_type != VAR_ಸಂಖ್ಯೆ || tv2->vval.v_ಸಂಖ್ಯೆ < 0)
	    {
		// right operand should be a positive number
		ಯದಿ (tv2->v_type != VAR_ಸಂಖ್ಯೆ)
		    emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
		ಅನ್ಯಥಾ
		    emsg(_(e_bitshift_ops_must_be_positive));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    ಯದಿ (tv2->vval.v_ಸಂಖ್ಯೆ > MAX_LSHIFT_BITS)
		tv1->vval.v_ಸಂಖ್ಯೆ = 0;
	    ಅನ್ಯಥಾ ಯದಿ (type == ಅಭಿವ್ಯಕ್ತಿ_LSHIFT)
		tv1->vval.v_ಸಂಖ್ಯೆ =
			(uvarnumber_T)tv1->vval.v_ಸಂಖ್ಯೆ << tv2->vval.v_ಸಂಖ್ಯೆ;
	    ಅನ್ಯಥಾ
		tv1->vval.v_ಸಂಖ್ಯೆ =
			(uvarnumber_T)tv1->vval.v_ಸಂಖ್ಯೆ >> tv2->vval.v_ಸಂಖ್ಯೆ;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (need_type(get_type_on_ರಾಶಿ(cctx, 0), &t_ಸಂಖ್ಯೆ, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_ಸಂಖ್ಯೆ));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    ಯದಿ (isn == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    ಯದಿ (isn != ರಿಕ್ತ)
		isn->isn_arg.op.op_type = type;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN;
    char_u	*p;
    char_u	*next;
    ಪೂರ್ಣಾಙ್ಕ		len = 2;
    ಪೂರ್ಣಾಙ್ಕ		type_is = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ppconst_used = ppconst->pp_used;

    // get the first variable
    ಯದಿ (compile_expr5(arg, cctx, ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    p = may_peek_next_ಸಾಲು(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ಯದಿ (type != ಅಭಿವ್ಯಕ್ತಿ_UNKNOWN)
    {
	ಪೂರ್ಣಾಙ್ಕ ic = FALSE;  // Default: do not ignore case

	ಯದಿ (next != ರಿಕ್ತ)
	{
	    *arg = next_ಸಾಲು_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	ಯದಿ (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	// extra question mark appended: ignore case
	ಯದಿ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	ಅನ್ಯಥಾ ಯದಿ (p[len] == '#')
	    ++len;
	// nothing appended: match case

	ಯದಿ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// get the second variable
	ಯದಿ (may_get_next_ಸಾಲು_error(p + len, arg, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (compile_expr5(arg, cctx, ppconst) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    ಪೂರ್ಣಾಙ್ಕ		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    ಯದಿ (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    ಅನ್ಯಥಾ
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_ಸಂಖ್ಯೆ = tv1->vval.v_ಸಂಖ್ಯೆ
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    ಪ್ರತ್ಯಾಗಮ ret;
	}

	generate_ppconst(cctx, ppconst);
	ಪ್ರತ್ಯಾಗಮ generate_COMPARE(cctx, type, ic);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	ವರ್ಣ	*op,
	ppconst_T *ppconst,
	ಪೂರ್ಣಾಙ್ಕ	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_ಸಾಲು(cctx, *arg, &next);
    ಪೂರ್ಣಾಙ್ಕ		opchar = *op;

    ಯದಿ (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	ಪೂರ್ಣಾಙ್ಕ		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, ಪರಿಮಾಣ(ಪೂರ್ಣಾಙ್ಕ), 10);
	ಯಾವತ್ (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    ಪೂರ್ಣಾಙ್ಕ		start_ctx_lnum = cctx->ctx_lnum;
	    ಪೂರ್ಣಾಙ್ಕ		save_lnum;
	    ಪೂರ್ಣಾಙ್ಕ		const_used;
	    ಪೂರ್ಣಾಙ್ಕ		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    ಯದಿ (next != ರಿಕ್ತ)
	    {
		*arg = next_ಸಾಲು_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ಯದಿ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    ಯದಿ (status != FAIL)
	    {
		// Use the last ppconst if possible.
		ಯದಿ (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    ಪೂರ್ಣಾಙ್ಕ		is_true = tv2bool(tv);

		    ಯದಿ ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_ಸಂಖ್ಯೆ = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ಅನ್ಯಥಾ
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_ರಾಶಿ(cctx);
		}
	    }
	    ಯದಿ (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    ಯದಿ (status == FAIL)
	    {
		ga_clear(&end_ga);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    ಯದಿ (jump_when != JUMP_NEVER)
	    {
		ಯದಿ (cctx->ctx_skip != SKIP_YES)
		{
		    *(((ಪೂರ್ಣಾಙ್ಕ *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    ಯದಿ (may_get_next_ಸಾಲು_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    const_used = ppconst->pp_used;
	    ಯದಿ ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    ಯದಿ (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		ಯದಿ (tv->v_type == VAR_ಸಂಖ್ಯೆ
			 && (tv->vval.v_ಸಂಖ್ಯೆ == 1 || tv->vval.v_ಸಂಖ್ಯೆ == 0))
		{
		    tv->vval.v_ಸಂಖ್ಯೆ = tv->vval.v_ಸಂಖ್ಯೆ == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_ಸಾಲು(cctx, *arg, &next);
	}

	ಯದಿ (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    ಯದಿ (bool_on_ರಾಶಿ(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	ಯದಿ (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    ಯಾವತ್ (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((ಪೂರ್ಣಾಙ್ಕ *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಙ್ಕ		ppconst_used = ppconst->pp_used;

    // get the first variable
    ಯದಿ (compile_expr4(arg, cctx, ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // || and && work almost the same
    ಪ್ರತ್ಯಾಗಮ compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ಪೂರ್ಣಾಙ್ಕ		ppconst_used = ppconst->pp_used;

    // eval the first expression
    ಯದಿ (compile_expr3(arg, cctx, ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // || and && work almost the same
    ಪ್ರತ್ಯಾಗಮ compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    ಪೂರ್ಣಾಙ್ಕ
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    ಯದಿ (cctx->ctx_skip == SKIP_YES)
    {
	ಪೂರ್ಣಾಙ್ಕ		prev_did_emsg = did_emsg;

	skip_ಅಭಿವ್ಯಕ್ತಿ_cctx(arg, cctx);
	ಪ್ರತ್ಯಾಗಮ did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    ಯದಿ (compile_expr2(arg, cctx, ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    p = may_peek_next_ಸಾಲು(cctx, *arg, &next);
    ಯದಿ (*p == '?')
    {
	ಪೂರ್ಣಾಙ್ಕ		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
	ಪೂರ್ಣಾಙ್ಕ		alt_idx = instr->ga_len;
	ಪೂರ್ಣಾಙ್ಕ		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ		has_const_ಅಭಿವ್ಯಕ್ತಿ = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		const_ಮೌಲ್ಯ = FALSE;
	ಪೂರ್ಣಾಙ್ಕ		save_skip = cctx->ctx_skip;

	ಯದಿ (next != ರಿಕ್ತ)
	{
	    *arg = next_ಸಾಲು_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ಯದಿ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_ಅಭಿವ್ಯಕ್ತಿ = TRUE;
	    ಯದಿ (op_falsy)
		const_ಮೌಲ್ಯ = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    ಅನ್ಯಥಾ
	    {
		ಪೂರ್ಣಾಙ್ಕ error = FALSE;

		const_ಮೌಲ್ಯ = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		ಯದಿ (error)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_ಮೌಲ್ಯ : !const_ಮೌಲ್ಯ) ? SKIP_YES : SKIP_NOT;

	    ಯದಿ (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    ಅನ್ಯಥಾ
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    generate_ppconst(cctx, ppconst);
	    ಯದಿ (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    ಯದಿ (op_falsy)
	    {
		type1 = get_type_on_ರಾಶಿ(cctx, -1);
		ಯದಿ (check_type_is_ಮೌಲ್ಯ(type1) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	ಯದಿ (may_get_next_ಸಾಲು_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (compile_expr1(arg, cctx, ppconst) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (!has_const_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    generate_ppconst(cctx, ppconst);

	    ಯದಿ (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_ರಾಶಿ(cctx, 0);
		--ರಾಶಿ->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	ಯದಿ (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_ಸಾಲು(cctx, *arg, &next);
	    ಯದಿ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (next != ರಿಕ್ತ)
	    {
		*arg = next_ಸಾಲು_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ಯದಿ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    // evaluate the third expression
	    ಯದಿ (has_const_ಅಭಿವ್ಯಕ್ತಿ)
		cctx->ctx_skip = save_skip == SKIP_YES || const_ಮೌಲ್ಯ
							 ? SKIP_YES : SKIP_NOT;
	    ಯದಿ (may_get_next_ಸಾಲು_error(p + 1, arg, cctx) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (compile_expr1(arg, cctx, ppconst) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (!has_const_ಅಭಿವ್ಯಕ್ತಿ)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)ರಾಶಿ->ga_data)
					      + ರಾಶಿ->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
compile_expr0_ext(char_u **arg,  cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    ಯದಿ (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (is_const != ರಿಕ್ತ)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    ಯದಿ (generate_ppconst(cctx, &ppconst) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Toplevel expression.
 */
    ಪೂರ್ಣಾಙ್ಕ
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    ಪ್ರತ್ಯಾಗಮ compile_expr0_ext(arg, cctx, ರಿಕ್ತ);
}


#endif // defined(FEAT_EVAL)
