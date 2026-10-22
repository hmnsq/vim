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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
ਸਥਿਰ ਪੂਰਨ paren_follows_after_ਵਿਅੰਜਕ = 0;

/*
 * Generate code for any ppconst entries.
 */
    ਪੂਰਨ
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    ਪੂਰਨ	    i;
    ਪੂਰਨ	    ret = OK;
    ਪੂਰਨ	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    ਲਈ (i = 0; i < ppconst->pp_ਵਰਤਿਆ; ++i)
	ਜੇ (generate_tv_ਧੱਕੋ(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_ਵਰਤਿਆ = 0;
    cctx->ctx_skip = save_skip;
    ਵਾਪਸ ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    ਸਥਿਰ ਪੂਰਨ
check_ppconst_bool(ppconst_T *ppconst)
{
    ਜੇ (ppconst->pp_ਵਰਤਿਆ > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ - 1];
	where_T	    where = WHERE_INIT;

	ਵਾਪਸ check_typval_type(&t_bool, tv, where);
    }
    ਵਾਪਸ OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    ਖਾਲੀ
clear_ppconst(ppconst_T *ppconst)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < ppconst->pp_ਵਰਤਿਆ; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_ਵਰਤਿਆ = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_tuple_member(
    type2_T	*typep,
    ਪੂਰਨ		is_ਟੁਕੜਾ,
    cctx_T	*cctx)
{
    ਜੇ (is_ਟੁਕੜਾ)
    {
	ਜੇ (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    ਵਾਪਸ FAIL;
	// a copy is made so the member type is no longer declared
	ਜੇ (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	ਜੇ (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    ਜੇ (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    ਜੇ (typep->type_curr->tt_argcount == 1)
	    {
		ਜੇ (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_ਦਲੀਲਾਂ[0]->tt_member;
		ਨਹੀਂਤਾਂ
		    typep->type_curr = typep->type_curr->tt_ਦਲੀਲਾਂ[0];
	    }
	    ਨਹੀਂਤਾਂ
		typep->type_curr = &t_any;
	    ਜੇ (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		ਜੇ (typep->type_decl->tt_argcount == 1)
		{
		    ਜੇ (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_ਦਲੀਲਾਂ[0]->tt_member;
		    ਨਹੀਂਤਾਂ
			typep->type_decl = typep->type_decl->tt_ਦਲੀਲਾਂ[0];
		}
		ਨਹੀਂਤਾਂ
		    typep->type_curr = &t_any;
	    }
	    ਨਹੀਂਤਾਂ
		typep->type_decl = typep->type_curr;
	}
	ਜੇ (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    ਪੂਰਨ
compile_member(ਪੂਰਨ is_ਟੁਕੜਾ, ਪੂਰਨ *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + ਢੇਰ->ga_len - (is_ਟੁਕੜਾ ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + ਢੇਰ->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    ਜੇ ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    ਜੇ (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	ਜੇ (need_type(idxtype, &t_ਸੰਖਿਆ, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (is_ਟੁਕੜਾ)
	{
	    idxtype = get_type_on_ਢੇਰ(cctx, 1);
	    ਜੇ (need_type(idxtype, &t_ਸੰਖਿਆ, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ਵਾਪਸ FAIL;
	}
    }

    ਜੇ (vartype == VAR_DICT)
    {
	ਜੇ (is_ਟੁਕੜਾ)
	{
	    emsg(_(e_cannot_ਟੁਕੜਾ_dictionary));
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    ਜੇ (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    ਜੇ (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		ਜੇ (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    ਨਹੀਂਤਾਂ
		typep->type_decl = typep->type_curr;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ਵਾਪਸ FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	ਜੇ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (keeping_dict != ਨਿਰਾ)
	    *keeping_dict = TRUE;
    }
    ਨਹੀਂਤਾਂ ਜੇ (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	ਜੇ ((is_ਟੁਕੜਾ
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (vartype == VAR_BLOB)
    {
	ਜੇ (is_ਟੁਕੜਾ)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    ਜੇ (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ
	{
	    typep->type_curr = &t_ਸੰਖਿਆ;
	    typep->type_decl = &t_ਸੰਖਿਆ;
	    ਜੇ (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		ਵਾਪਸ FAIL;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (vartype == VAR_TUPLE)
    {
	ਜੇ (compile_tuple_member(typep, is_ਟੁਕੜਾ, cctx) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	ਜੇ (is_ਟੁਕੜਾ)
	{
	    ਜੇ (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		ਵਾਪਸ FAIL;
	    // a copy is made so the member type is no longer declared
	    ਜੇ (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    ਜੇ (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		ਜੇ (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		ਜੇ (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		ਜੇ (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    ਜੇ (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		ਨਹੀਂਤਾਂ
			typep->type_decl = typep->type_curr;
	    }
	    ਜੇ (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		ਵਾਪਸ FAIL;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਚੋਣ (vartype)
	{
	    ਹਾਲਤ VAR_FUNC:
	    ਹਾਲਤ VAR_PARTIAL:
		emsg(_(e_cannot_ਸੂਚਕਾਂਕ_a_funcref));
		ਰੋਕੋ;
	    ਹਾਲਤ VAR_BOOL:
	    ਹਾਲਤ VAR_SPECIAL:
	    ਹਾਲਤ VAR_ਕੰਮ:
	    ਹਾਲਤ VAR_CHANNEL:
	    ਹਾਲਤ VAR_INSTR:
	    ਹਾਲਤ VAR_CLASS:
	    ਹਾਲਤ VAR_OBJECT:
	    ਹਾਲਤ VAR_TYPEALIAS:
	    ਹਾਲਤ VAR_UNKNOWN:
	    ਹਾਲਤ VAR_ANY:
	    ਹਾਲਤ VAR_VOID:
		emsg(_(e_cannot_ਸੂਚਕਾਂਕ_special_variable));
		ਰੋਕੋ;
	    ਮੂਲ:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    ਸਥਿਰ ਪੂਰਨ
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    ਲਈ (cctx_T *cctx = cctx_arg; cctx != ਨਿਰਾ; cctx = cctx->ctx_outer)
    {
	ਜੇ (cctx->ctx_ufunc != ਨਿਰਾ && cctx->ctx_ufunc->uf_class != ਨਿਰਾ)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    ਜਦੋਂ (clp != ਨਿਰਾ)
	    {
		ਜੇ (clp == cl)
		    ਵਾਪਸ TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    ਵਾਪਸ FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    ਪੂਰਨ
compile_class_object_ਸੂਚਕਾਂਕ(cctx_T *cctx, char_u **arg, type_T *type)
{
    ਪੂਰਨ		m_idx;
    ਪੂਰਨ		ret = FAIL;

    ਜੇ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ਵਾਪਸ FAIL;
    }

    class_T *cl = type->tt_class;
    ਪੂਰਨ is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    ਜੇ (type == &t_super)
    {
	ਜੇ (cctx->ctx_ufunc == ਨਿਰਾ || cctx->ctx_ufunc->uf_class == ਨਿਰਾ)
	{
	    emsg(_(e_using_super_not_ਵਿੱਚ_class_ਵਿਧੀ));
	    ਵਾਪਸ FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_ਢੇਰ.ga_len;
    }
    ਨਹੀਂਤਾਂ ਜੇ (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	ਜੇ (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_ਅੰਕੜੇ) + instr->ga_len - 1;
	    ਜੇ (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    ਜੇ (cl == ਨਿਰਾ)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	ਵਾਪਸ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_ਸਿਰਾ = ਲੱਭੋ_name_ਸਿਰਾ(name, ਨਿਰਾ, ਨਿਰਾ, FNE_CHECK_ਸ਼ੁਰੂ);
    ਜੇ (name_ਸਿਰਾ == name)
    {
	emsg(_(e_missing_name_after_dot));
	ਵਾਪਸ FAIL;
    }
    size_t len = name_ਸਿਰਾ - name;

    gfargs_tab_T    gfatab;

    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_init(&gfatab);

    ਜੇ (*name_ਸਿਰਾ == '<')
    {
	// generic method call
	name_ਸਿਰਾ = parse_generic_func_type_ਦਲੀਲਾਂ(name, len, name_ਸਿਰਾ,
							&gfatab, cctx);
	ਜੇ (name_ਸਿਰਾ == ਨਿਰਾ)
	    ਜਾਓ ਪੂਰਾ;
    }

    ਜੇ (*name_ਸਿਰਾ == '(')
    {
	ਪੂਰਨ	function_ਗਿਣਤੀ;
	ਪੂਰਨ	ਬੱਚਾ_ਗਿਣਤੀ;
	ufunc_T	**functions;

	ਜੇ (type->tt_type == VAR_CLASS)
	{
	    function_ਗਿਣਤੀ = cl->class_class_function_ਗਿਣਤੀ;
	    ਬੱਚਾ_ਗਿਣਤੀ = cl->class_class_function_ਗਿਣਤੀ_ਬੱਚਾ;
	    functions = cl->class_class_functions;
	}
	ਨਹੀਂਤਾਂ
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    ਜੇ (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_ਗਿਣਤੀ = cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ;
	    ਬੱਚਾ_ਗਿਣਤੀ = cl->class_obj_ਵਿਧੀ_ਗਿਣਤੀ_ਬੱਚਾ;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = ਨਿਰਾ;
	ਪੂਰਨ fi;
	ਲਈ (fi = is_super ? ਬੱਚਾ_ਗਿਣਤੀ : 0; fi < function_ਗਿਣਤੀ; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    ਜੇ (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		ਰੋਕੋ;
	    }
	}

	ocmember_T  *ocm = ਨਿਰਾ;
	ਜੇ (ufunc == ਨਿਰਾ)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    ਜੇ (ocm == ਨਿਰਾ || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		ਵਿਧੀ_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, type->tt_type, name, len);
		ਜਾਓ ਪੂਰਾ;
	    }
	    ਜੇ (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_ਢੇਰ.ga_len;
		ਜੇ (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    ਜਾਓ ਪੂਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ ਸਥਿਤੀ;

		ਜੇ (IS_INTERFACE(cl))
		    ਸਥਿਤੀ = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		ਨਹੀਂਤਾਂ
		    ਸਥਿਤੀ = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		ਜੇ (ਸਥਿਤੀ == FAIL)
		    ਜਾਓ ਪੂਰਾ;
	    }
	}

	ਜੇ (is_super && ufunc != ਨਿਰਾ && IS_ABSTRACT_ਵਿਧੀ(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_ਵਿਧੀ_str_ਸਿੱਧਾ), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    ਜਾਓ ਪੂਰਾ;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	ਜੇ (ocm == ਨਿਰਾ && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_ਵਿਧੀ_str), name);
	    ਜਾਓ ਪੂਰਾ;
	}

	// process generic function call
	ਜੇ (ufunc != ਨਿਰਾ)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ਜੇ (ufunc == ਨਿਰਾ)
		ਜਾਓ ਪੂਰਾ;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_ਸਿਰਾ + 1);
	ਪੂਰਨ argcount = 0;
	ਜੇ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    ਜਾਓ ਪੂਰਾ;

	ਜੇ (ocm != ਨਿਰਾ)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    ਜਾਓ ਪੂਰਾ;
	}
	ਜੇ (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	ਨਹੀਂਤਾਂ
	    ret = generate_CALL(cctx, ufunc, ਨਿਰਾ, 0, argcount, FALSE);
	ਜਾਓ ਪੂਰਾ;
    }

    ਜੇ (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	ਜੇ (m_idx >= 0)
	{
	    ਜੇ (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		ਜਾਓ ਪੂਰਾ;
	    }

	    *arg = name_ਸਿਰਾ;
	    ਜੇ (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    ਨਹੀਂਤਾਂ
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    ਜਾਓ ਪੂਰਾ;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_ਵਿਧੀ_idx(cl, name, len);
	ਜੇ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    ਜੇ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_ਵਿਧੀ_str), fp->uf_name);
		ਜਾਓ ਪੂਰਾ;
	    }
	    *arg = name_ਸਿਰਾ;
	    // Remove the object type from the stack
	    --cctx->ctx_type_ਢੇਰ.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, ਨਿਰਾ);
	    ਜਾਓ ਪੂਰਾ;
	}

	member_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, VAR_OBJECT, name, len);
    }
    ਨਹੀਂਤਾਂ
    {
	// load class member
	ਪੂਰਨ idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	ਜੇ (m != ਨਿਰਾ)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    ਜੇ (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		ਜਾਓ ਪੂਰਾ;
	    }

	    *arg = name_ਸਿਰਾ;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ਢੇਰ.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    ਜਾਓ ਪੂਰਾ;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_ਵਿਧੀ_idx(cl, name, len);
	ਜੇ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    ਜੇ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_ਵਿਧੀ_str), fp->uf_name);
		ਜਾਓ ਪੂਰਾ;
	    }
	    *arg = name_ਸਿਰਾ;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ਢੇਰ.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, ਨਿਰਾ);
	    ਜਾਓ ਪੂਰਾ;
	}

	member_not_ਮਿਲਿਆ_ਸੁਨੇਹਾ(cl, VAR_CLASS, name, len);
    }

ਪੂਰਾ:
    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_clear(&gfatab);
    ਵਾਪਸ ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    ਪੂਰਨ
compile_ਲੱਦੋ_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *ਸ਼ੁਰੂ,	    // start of variable
	char_u **ਸਿਰਾ,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    ਪੂਰਨ		    idx;
    imported_T	    *imp;

    ਜੇ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ਵਾਪਸ FAIL;
    si = SCRIPT_ਵਸਤੂ(current_sctx.sc_sid);
    idx = get_script_ਵਸਤੂ_idx(current_sctx.sc_sid, name, 0, cctx, ਨਿਰਾ);
    ਜੇ (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_ਅੰਕੜੇ) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	ਵਾਪਸ OK;
    }

    ਜੇ (ਸਿਰਾ == ਨਿਰਾ)
	imp = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ (import == ਨਿਰਾ)
	imp = ਲੱਭੋ_imported(name, 0, FALSE);
    ਨਹੀਂਤਾਂ
	imp = import;

    ਜੇ (imp != ਨਿਰਾ)
    {
	char_u	*p = skipwhite(*ਸਿਰਾ);
	char_u	*exp_name;
	ਪੂਰਨ	cc;
	ufunc_T	*ufunc = ਨਿਰਾ;
	type_T	*type;
	ਪੂਰਨ	ਪੂਰਾ = FALSE;
	ਪੂਰਨ	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	ਜੇ (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), ਸ਼ੁਰੂ);
	    ਵਾਪਸ FAIL;
	}
	++p;
	ਜੇ (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    ਵਾਪਸ FAIL;
	}

	// isolate one name
	exp_name = p;
	ਜਦੋਂ (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ਵਸਤੂ(imp->imp_sid);
	ਜੇ (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_ਸਰੋਤ(cctx, imp->imp_sid);

	ਜੇ (res == OK)
	{
	    ਜੇ (si->sn_autoload_ਅਗੇਤਰ != ਨਿਰਾ
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_ਅਗੇਤਰ, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		ਜੇ (cc == '(' || paren_follows_after_ਵਿਅੰਜਕ)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		ਨਹੀਂਤਾਂ
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		ਪੂਰਾ = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		ਜੇ (cc == '(' || paren_follows_after_ਵਿਅੰਜਕ)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		ਨਹੀਂਤਾਂ
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		ਪੂਰਾ = TRUE;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		idx = ਲੱਭੋ_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, ਨਿਰਾ, TRUE);
	    }
	}

	*p = cc;
	*ਸਿਰਾ = p;
	ਜੇ (ਪੂਰਾ)
	    ਵਾਪਸ res;

	ਜੇ (idx < 0)
	{
	    ਜੇ (ufunc != ਨਿਰਾ)
	    {
		gfargs_tab_T	gfatab;

		generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_init(&gfatab);

		ਜੇ (IS_GENERIC_FUNC(ufunc))
		{
		    ਜੇ (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_ਦਲੀਲਾਂ(name, STRLEN(name), p,
								&gfatab, cctx);
			ਜੇ (p != ਨਿਰਾ)
			{
			    *ਸਿਰਾ = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_clear(&gfatab);

			ਜੇ (p == ਨਿਰਾ || ufunc == ਨਿਰਾ)
			    ਵਾਪਸ FAIL;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			emsg_funcname(e_generic_func_missing_type_ਦਲੀਲਾਂ_str,
									name);
			ਵਾਪਸ FAIL;
		    }
		}
		ਨਹੀਂਤਾਂ ਜੇ (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    ਵਾਪਸ FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, ਨਿਰਾ, TRUE);
		ਵਾਪਸ OK;
	    }
	    ਵਾਪਸ FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	ਵਾਪਸ OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    ਵਾਪਸ generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    ਸਥਿਰ ਪੂਰਨ
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    ਪੂਰਨ			has_g_ਅਗੇਤਰ)
{
    ufunc_T *ufunc = ਲੱਭੋ_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    ਜੇ (ufunc == ਨਿਰਾ || (!has_g_ਅਗੇਤਰ && func_requires_g_ਅਗੇਤਰ(ufunc)))
	ਵਾਪਸ FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    ਜੇ (ufunc == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    ਜੇ (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, ਨਿਰਾ) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    ਸਥਿਰ ਪੂਰਨ
compiling_a_class_ਵਿਧੀ(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    ਵਾਪਸ cctx->ctx_ufunc != ਨਿਰਾ
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == ਨਿਰਾ
		|| cctx->ctx_outer->ctx_ufunc == ਨਿਰਾ
		|| cctx->ctx_outer->ctx_ufunc->uf_class == ਨਿਰਾ
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
    ਸਥਿਰ ਪੂਰਨ
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    ਪੂਰਨ			outer_ਡੂੰਘਾਈ)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    ਪੂਰਨ		isn_idx;

    ਜੇ (fp == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਲਈ (ਪੂਰਨ ਡੂੰਘਾਈ = 0; ਡੂੰਘਾਈ < outer_ਡੂੰਘਾਈ; ++ਡੂੰਘਾਈ)
    {
	ਜੇ (outer_cctx->ctx_outer == ਨਿਰਾ)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    ਵਾਪਸ FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    ਜੇ (fp->uf_ਖੰਡ_ids == ਨਿਰਾ && ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ > 0)
    {
	fp->uf_ਖੰਡ_ids = ALLOC_MULT(ਪੂਰਨ, ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ);
	ਜੇ (fp->uf_ਖੰਡ_ids != ਨਿਰਾ)
	{
	    mch_memmove(fp->uf_ਖੰਡ_ids, ufunc->uf_ਖੰਡ_ids,
					    ਆਕਾਰ(ਪੂਰਨ) * ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ);
	    fp->uf_ਖੰਡ_ਡੂੰਘਾਈ = ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    ਜੇ (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (generate_FUNCREF(cctx, fp, ਨਿਰਾ, FALSE, 0, &isn_idx) == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (cctx->ctx_skip == SKIP_YES)
	ਵਾਪਸ OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    ਜੇ (outer_ਡੂੰਘਾਈ > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_ਅੰਕੜੇ) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	ਜੇ (extra == ਨਿਰਾ)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    ਜੇ (extra == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_ਡੂੰਘਾਈ = outer_ਡੂੰਘਾਈ;
    }

    ਵਾਪਸ OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    ਪੂਰਨ
compile_ਲੱਦੋ(
	char_u **arg,
	size_t  namelen,
	char_u *ਸਿਰਾ_arg,
	cctx_T	*cctx,
	ਪੂਰਨ	is_ਵਿਅੰਜਕ,
	ਪੂਰਨ	error)
{
    type_T	*type;
    char_u	*name = ਨਿਰਾ;
    char_u	*ਸਿਰਾ = ਸਿਰਾ_arg;
    ਪੂਰਨ		res = FAIL;
    ਪੂਰਨ		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = ਨਿਰਾ;

    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_init(&gfatab);

    ਜੇ (*(*arg + namelen) == '<')
    {
	// generic function call
	ਜੇ (parse_generic_func_type_ਦਲੀਲਾਂ(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == ਨਿਰਾ)
	{
	    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_clear(&gfatab);
	    ਵਾਪਸ FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    ਜੇ (*(*arg + 1) == ':')
    {
	ਜੇ (ਸਿਰਾ <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    ਚੋਣ (**arg)
	    {
		ਹਾਲਤ 'g': isn_type = ISN_LOADGDICT; ਰੋਕੋ;
		ਹਾਲਤ 'w': isn_type = ISN_LOADWDICT; ਰੋਕੋ;
		ਹਾਲਤ 't': isn_type = ISN_LOADTDICT; ਰੋਕੋ;
		ਹਾਲਤ 'b': isn_type = ISN_LOADBDICT; ਰੋਕੋ;
		ਮੂਲ:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    ਜਾਓ theend;
	    }
	    ਜੇ (generate_instr_type(cctx, isn_type, &t_dict_any) == ਨਿਰਾ)
		ਜਾਓ theend;
	    res = OK;
	}
	ਨਹੀਂਤਾਂ
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, ਸਿਰਾ - (*arg + 2));
	    ਜੇ (name == ਨਿਰਾ)
		ਜਾਓ theend;

	    ਚੋਣ (**arg)
	    {
		ਹਾਲਤ 'v': res = generate_LOADV(cctx, name);
			  ਰੋਕੋ;
		ਹਾਲਤ 's': ਜੇ (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_ਵਿੱਚ_vim9_script_str),
									 *arg);
			      ਜਾਓ theend;
			  }
			  ਜੇ (is_ਵਿਅੰਜਕ && ਲੱਭੋ_func(name, FALSE) != ਨਿਰਾ)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  ਨਹੀਂਤਾਂ
			      res = compile_ਲੱਦੋ_scriptvar(cctx, name,
							    ਨਿਰਾ, &ਸਿਰਾ, ਨਿਰਾ);
			  ਰੋਕੋ;
		ਹਾਲਤ 'g': ਜੇ (vim_strchr(name, AUTOLOAD_CHAR) == ਨਿਰਾ)
			  {
			      ਜੇ (is_ਵਿਅੰਜਕ && ASCII_ISUPPER(*name)
				       && (ਲੱਭੋ_func(name, TRUE) != ਨਿਰਾ
					   || gfatab.gfat_ਦਲੀਲਾਂ.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      ਨਹੀਂਤਾਂ
				  isn_type = ISN_LOADG;
			  }
			  ਨਹੀਂਤਾਂ
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, ਸਿਰਾ - *arg);
			      ਜੇ (name == ਨਿਰਾ)
				  ਜਾਓ theend;
			  }
			  ਰੋਕੋ;
		ਹਾਲਤ 'w': isn_type = ISN_LOADW; ਰੋਕੋ;
		ਹਾਲਤ 't': isn_type = ISN_LOADT; ਰੋਕੋ;
		ਹਾਲਤ 'b': isn_type = ISN_LOADB; ਰੋਕੋ;
		ਮੂਲ:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  ਜਾਓ theend;
	    }
	    ਜੇ (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_ਲੱਦੋ(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	    idx;
	ਪੂਰਨ	    ਵਿਧੀ_idx;
	ਪੂਰਨ	    gen_ਲੱਦੋ = FALSE;
	ਪੂਰਨ	    gen_ਲੱਦੋ_outer = 0;
	ਪੂਰਨ	    outer_loop_ਡੂੰਘਾਈ = -1;
	ਪੂਰਨ	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	ਜੇ (name == ਨਿਰਾ)
	    ਜਾਓ theend;

	ਜੇ (STRCMP(name, "super") == 0 && compiling_a_class_ਵਿਧੀ(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = ਧੱਕੋ_type_ਢੇਰ(cctx, &t_super);
	    ਜੇ (*ਸਿਰਾ != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr(name, AUTOLOAD_CHAR) != ਨਿਰਾ)
	{
	    script_autoload(name, FALSE);
	    res = generate_ਲੱਦੋ(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	ਨਹੀਂਤਾਂ ਜੇ (arg_exists(*arg, namelen, &idx, &type, &gen_ਲੱਦੋ_outer, cctx)
									 == OK)
	{
	    ਜੇ (gen_ਲੱਦੋ_outer == 0)
		gen_ਲੱਦੋ = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    lvar_T  lvar;
	    class_T *cl = ਨਿਰਾ;

	    ਜੇ (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		ਜੇ (lvar.lv_ufunc != ਨਿਰਾ && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_ਦਲੀਲਾਂ.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		ਨਹੀਂਤਾਂ ਜੇ (lvar.lv_from_outer != 0)
		{
		    gen_ਲੱਦੋ_outer = lvar.lv_from_outer;
		    outer_loop_ਡੂੰਘਾਈ = lvar.lv_loop_ਡੂੰਘਾਈ;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		ਨਹੀਂਤਾਂ
		    gen_ਲੱਦੋ = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (cctx->ctx_ufunc->uf_defclass != ਨਿਰਾ &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((ਵਿਧੀ_idx =
			     cctx_class_ਵਿਧੀ_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		ਜੇ (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    ਜੇ (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    ਨਹੀਂਤਾਂ
		    {
			ufunc_T *fp = cl->class_class_functions[ਵਿਧੀ_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, ਵਿਧੀ_idx,
									ਨਿਰਾ);
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		imported_T *imp = ਨਿਰਾ;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		ਜੇ (script_var_exists(*arg, namelen, cctx, ਨਿਰਾ) == OK
			    || (imp = ਲੱਭੋ_imported(name, 0, FALSE)) != ਨਿਰਾ)
		    res = compile_ਲੱਦੋ_scriptvar(cctx, name, *arg, &ਸਿਰਾ, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		ਜੇ (res == FAIL && is_ਵਿਅੰਜਕ && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	ਜੇ (gen_ਲੱਦੋ)
	    res = generate_ਲੱਦੋ(cctx, ISN_ਲੱਦੋ, idx, ਨਿਰਾ, type);
	ਜੇ (gen_ਲੱਦੋ_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_ਲੱਦੋ_outer,
				       outer_loop_ਡੂੰਘਾਈ, outer_loop_idx, type);
	    cctx->ctx_outer_ਵਰਤਿਆ = TRUE;
	}
    }

    *arg = ਸਿਰਾ;

theend:
    ਜੇ (lt != ਨਿਰਾ)
	*lt = '<';
    ਜੇ (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_ਮਿਲਿਆ_str), name);
    vim_free(name);
    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_clear(&gfatab);
    ਵਾਪਸ res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_string(isn_T *isn, cctx_T *cctx, ਪੂਰਨ str_ਵਿੱਥ)
{
    char_u	*s = isn->isn_arg.string + str_ਵਿੱਥ;
    garray_T	save_ga = cctx->ctx_instr;
    ਪੂਰਨ		ਵਿਅੰਜਕ_res;
    ਪੂਰਨ		trailing_error;
    ਪੂਰਨ		instr_ਗਿਣਤੀ;
    isn_T	*instr = ਨਿਰਾ;

    // Remove the string type from the stack.
    --cctx->ctx_type_ਢੇਰ.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_ਅੰਕੜੇ = ਨਿਰਾ;

    // avoid peeking a next line
    ਪੂਰਨ galen_save = cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len;
    cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len = 0;

    ਵਿਅੰਜਕ_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    ਜੇ (ਵਿਅੰਜਕ_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	ਜੇ (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_ਢੇਰ.ga_len;
	ਵਾਪਸ FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_ਗਿਣਤੀ = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_ਅੰਕੜੇ;
    instr[instr_ਗਿਣਤੀ].isn_type = ISN_ਅੰਤ;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    ਵਾਪਸ OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    ਪੂਰਨ
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	ਪੂਰਨ	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    ਪੂਰਨ	    must_ਸਿਰਾ = FALSE;
    ਪੂਰਨ	    instr_ਗਿਣਤੀ;

    ਲਈ (;;)
    {
	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ(whitep, &p, cctx) == FAIL)
	    ਜਾਓ failret;
	ਜੇ (*p == ')')
	{
	    *arg = p + 1;
	    ਵਾਪਸ OK;
	}
	ਜੇ (must_ਸਿਰਾ)
	{
	    semsg(_(e_missing_comma_before_ਦਲੀਲ_str), p);
	    ਵਾਪਸ FAIL;
	}

	instr_ਗਿਣਤੀ = cctx->ctx_instr.ga_len;
	ਜੇ (compile_expr0(&p, cctx) == FAIL)
	    ਵਾਪਸ FAIL;
	++*argcount;

	ਜੇ (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_ਗਿਣਤੀ + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_ਅੰਕੜੇ) + instr_ਗਿਣਤੀ;

	    // {skip} argument of searchpair() can be compiled if not empty
	    ਜੇ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ਨਿਰਾ
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	ਨਹੀਂਤਾਂ ਜੇ (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_ਗਿਣਤੀ + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_ਅੰਕੜੇ) + instr_ਗਿਣਤੀ;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    ਜੇ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ਨਿਰਾ
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	ਜੇ (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (*p == ',')
	{
	    ++p;
	    ਜੇ (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ
	    must_ਸਿਰਾ = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    ਵਾਪਸ FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_builtin_ਵਿਧੀ_call(cctx_T *cctx, class_builtin_T builtin_ਵਿਧੀ)
{
    type_T	*type = get_decl_type_on_ਢੇਰ(cctx, 0);
    ਪੂਰਨ		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    ਜੇ (type->tt_type == VAR_OBJECT)
    {
	ਪੂਰਨ	ਵਿਧੀ_idx;
	ufunc_T *uf = class_get_builtin_ਵਿਧੀ(type->tt_class, builtin_ਵਿਧੀ,
							&ਵਿਧੀ_idx);
	ਜੇ (uf != ਨਿਰਾ)
	    res = generate_CALL(cctx, uf, type->tt_class, ਵਿਧੀ_idx, 0, FALSE);
    }

    ਵਾਪਸ res;
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
    ਸਥਿਰ ਪੂਰਨ
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	ਪੂਰਨ	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    ਪੂਰਨ		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ਨਿਰਾ;
    char_u	*gname = ਨਿਰਾ;
    ufunc_T	*ufunc = ਨਿਰਾ;
    ਪੂਰਨ		res = FAIL;
    ਪੂਰਨ		is_autoload;
    ਪੂਰਨ		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    ਜੇ (varlen >= ਆਕਾਰ(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	ਵਾਪਸ FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = ਲੱਭੋ_imported(name, varlen, FALSE);
    ਜੇ (import != ਨਿਰਾ)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	ਵਾਪਸ FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    ਜੇ ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	ਪੂਰਨ	    is_has = **arg == 'h';
	ਪੂਰਨ	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	ਜੇ (*s == '"')
	    (ਖਾਲੀ)eval_string(&s, &argvars[0], TRUE, FALSE);
	ਨਹੀਂਤਾਂ ਜੇ (*s == '\'')
	    (ਖਾਲੀ)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	ਜੇ (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    ਜੇ (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_ਸੰਖਿਆ;
		tv->vval.v_ਸੰਖਿਆ = 0;
		ਜੇ (is_has)
		    f_has(argvars, tv);
		ਨਹੀਂਤਾਂ ਜੇ (is_len)
		    f_len(argvars, tv);
		ਨਹੀਂਤਾਂ
		    f_exists(argvars, tv);
		++ppconst->pp_ਵਰਤਿਆ;
	    }
	    clear_tv(&argvars[0]);
	    ਵਾਪਸ OK;
	}
	clear_tv(&argvars[0]);
	ਜੇ (!is_has && !is_len)
	{
	    emsg(_(e_ਦਲੀਲ_ਦਾ_exists_compiled_must_be_literal_string));
	    ਵਾਪਸ FAIL;
	}
    }

    ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
	ਵਾਪਸ FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    ਜੇ ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    ਨਹੀਂਤਾਂ ਜੇ (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    ਨਹੀਂਤਾਂ
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_init(&gfatab);

    ਜੇ (*(*arg + varlen) == '<')
    {
	char_u	*ਸ਼ੁਰੂ = *arg;

	// generic function
	*arg = parse_generic_func_type_ਦਲੀਲਾਂ(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	ਜੇ (*arg == ਨਿਰਾ)
	    ਜਾਓ theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(ਸ਼ੁਰੂ, *arg - ਸ਼ੁਰੂ);
	ਜੇ (gname == ਨਿਰਾ)
	    ਜਾਓ theend;
	++*arg;			// skip '('
    }
    ਨਹੀਂਤਾਂ
	*arg += varlen + 1;

    ਜੇ (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	ਜਾਓ theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != ਨਿਰਾ;
    ਜੇ (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	ਪੂਰਨ	    idx;

	// builtin function
	idx = ਲੱਭੋ_ਅੰਦਰੂਨੀ_func(name);
	ਜੇ (idx >= 0)
	{
	    ਜੇ (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_ਵਿੱਚ_vim9_script));
		ਜਾਓ theend;
	    }

	    ਜੇ (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_ਢੇਰ(cctx, 1);
		ਜੇ (check_type_is_ਮੁੱਲ(get_type_on_ਢੇਰ(cctx, 0)) == FAIL)
		    ਜਾਓ theend;

		// add() can be compiled to instructions if we know the type
		ਜੇ (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		ਨਹੀਂਤਾਂ ਜੇ (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    ਜੇ ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		ਜੇ (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_ਵਿਧੀ = CLASS_BUILTIN_INVALID;
	    ਜੇ (STRCMP(name, "string") == 0)
		builtin_ਵਿਧੀ = CLASS_BUILTIN_STRING;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "empty") == 0)
		builtin_ਵਿਧੀ = CLASS_BUILTIN_ਖਾਲੀ_ਥਾਵਾਂ;
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(name, "len") == 0)
		builtin_ਵਿਧੀ = CLASS_BUILTIN_LEN;
	    ਜੇ (builtin_ਵਿਧੀ != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_ਵਿਧੀ_call(cctx, builtin_ਵਿਧੀ);
		ਜੇ (res == OK)
		    idx = -1;
	    }

	    ਜੇ (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	ਨਹੀਂਤਾਂ
	    emsg_funcname(e_unknown_function_str, namebuf);
	ਜਾਓ theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    ਜੇ (lookup_local(namebuf, varlen, ਨਿਰਾ, cctx) == FAIL
	    && arg_exists(namebuf, varlen, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, cctx) != OK)
    {
	class_T		*cl = ਨਿਰਾ;
	ਪੂਰਨ		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = ਲੱਭੋ_func(name, FALSE);
	ਜੇ (ufunc != ਨਿਰਾ)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ਜੇ (ufunc == ਨਿਰਾ)
		ਜਾਓ theend;

	    ਜੇ (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, ਨਿਰਾ, 0, argcount, FALSE);
		ਜਾਓ theend;
	    }
	    ਜੇ (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ਨਿਰਾ
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		ਜਾਓ theend;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ ((mi = cctx_class_ਵਿਧੀ_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    ਜੇ (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], ਨਿਰਾ,
							0, argcount, FALSE);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		semsg(_(e_class_ਵਿਧੀ_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    ਜਾਓ theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == ਨਿਰਾ && gname != ਨਿਰਾ ? gname : namebuf;
    ਜੇ (!has_g_namespace && !is_autoload
	    && compile_ਲੱਦੋ(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_ਢੇਰ(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	ਜਾਓ theend;
    }

    // If we can find a global function by name generate the right call.
    ਜੇ (ufunc != ਨਿਰਾ)
    {
	res = generate_CALL(cctx, ufunc, ਨਿਰਾ, 0, argcount, FALSE);
	ਜਾਓ theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    ਜੇ (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    ਨਹੀਂਤਾਂ
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_ਦਲੀਲਾਂ_ਸਾਰਣੀ_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    ਵਾਪਸ res;
}

// like NAMESPACE_CHAR but with 'a' and 'l'.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ VIM9_NAMESPACE_CHAR	(char_u *)"bgstvw"

/*
 * Find the end of a variable or function name.  Unlike find_name_end() this
 * does not recognize magic braces.
 * When "use_namespace" is TRUE recognize "b:", "s:", etc.
 * Return a pointer to just after the name.  Equal to "arg" if there is no
 * valid name.
 */
    char_u *
ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(char_u *arg, ਪੂਰਨ use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    ਜੇ (!eval_isnamec1(*arg))
	ਵਾਪਸ arg;

    ਲਈ (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	ਜੇ (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == ਨਿਰਾ))
	    ਰੋਕੋ;
    ਵਾਪਸ p;
}

/*
 * Like to_name_end() but also skip over a list or dict constant.
 * Also accept "<SNR>123_Func".
 * This intentionally does not handle line continuation.
 */
    char_u *
ਮੰਜ਼ਿਲ_name_const_ਸਿਰਾ(char_u *arg)
{
    char_u	*p = arg;
    typval_T	rettv;

    ਜੇ (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(p, TRUE);
    ਜੇ (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	ਜੇ (eval_list(&p, &rettv, ਨਿਰਾ, FALSE) == FAIL)
	    p = arg;
    }
    ਵਾਪਸ p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ		is_const;
    ਪੂਰਨ		is_all_const = TRUE;	// reset when non-const encountered
    ਪੂਰਨ		must_ਸਿਰਾ = FALSE;

    ਲਈ (;;)
    {
	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_ਸਿਰਾ_ਦਾ_list_rsb_str), *arg);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (*p == ']')
	{
	    ++p;
	    ਰੋਕੋ;
	}
	ਜੇ (must_ਸਿਰਾ)
	{
	    semsg(_(e_missing_comma_ਵਿੱਚ_list_str), p);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (!is_const)
	    is_all_const = FALSE;
	++ਗਿਣਤੀ_2;
	ਜੇ (*p == ',')
	{
	    ++p;
	    ਜੇ (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ
	    must_ਸਿਰਾ = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ਵਾਪਸ generate_NEWLIST(cctx, ਗਿਣਤੀ_2, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    ਪੂਰਨ ਪਹਿਲਾ_ਵਸਤੂ_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    ਪੂਰਨ		is_const;
    ਪੂਰਨ		is_all_const = TRUE;	// reset when non-const encountered
    ਪੂਰਨ		must_ਸਿਰਾ = FALSE;

    ਜੇ (**arg != ')')
    {
	ਜੇ (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    ਵਾਪਸ FAIL;
	}
	ਗਿਣਤੀ_2 = 1;	// the first tuple item is already processed
	is_all_const = ਪਹਿਲਾ_ਵਸਤੂ_const;
	ਲਈ (;;)
	{
	    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_ਸਿਰਾ_ਦਾ_tuple_rsp_str), *arg);
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (*p == ')')
	    {
		++p;
		ਰੋਕੋ;
	    }
	    ਜੇ (must_ਸਿਰਾ)
	    {
		semsg(_(e_missing_comma_ਵਿੱਚ_tuple_str), p);
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (!is_const)
		is_all_const = FALSE;
	    ++ਗਿਣਤੀ_2;
	    ਜੇ (*p == ',')
	    {
		++p;
		ਜੇ (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ਵਾਪਸ FAIL;
		}
	    }
	    ਨਹੀਂਤਾਂ
		must_ਸਿਰਾ = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ਵਾਪਸ generate_NEWTUPLE(cctx, ਗਿਣਤੀ_2, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    ਸਥਿਰ ਖਾਲੀ
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*ਸਤਰ;
    size_t	off;

    ਜੇ (!evalarg->eval_using_cmdline || cctx == ਨਿਰਾ)
	ਵਾਪਸ;

    gap = &evalarg->eval_tofree_ga;
    ਜੇ (gap->ga_len == 0)
	ਵਾਪਸ;

    off = *arg - ((char_u **)gap->ga_ਅੰਕੜੇ)[gap->ga_len - 1];
    ਸਤਰ = ((char_u **)cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_ਅੰਕੜੇ)[cctx->ctx_lnum];
    *arg = ਸਤਰ + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    ਪੂਰਨ
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ਪੂਰਨ		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    ਜੇ (r != OK)
    {
	clear_evalarg(&evalarg, ਨਿਰਾ);
	ਵਾਪਸ r;
    }

    ਜੇ (ppconst != ਨਿਰਾ && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, ਨਿਰਾ);
	ਵਾਪਸ FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    ਜੇ (cctx->ctx_ufunc != ਨਿਰਾ)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	ਪੂਰਨ ਖੰਡ_ਡੂੰਘਾਈ = cctx->ctx_ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ;

	ਜੇ (ਖੰਡ_ਡੂੰਘਾਈ > 0)
	{
	    ufunc->uf_ਖੰਡ_ids = ALLOC_MULT(ਪੂਰਨ, ਖੰਡ_ਡੂੰਘਾਈ);
	    ਜੇ (ufunc->uf_ਖੰਡ_ids != ਨਿਰਾ)
	    {
		mch_memmove(ufunc->uf_ਖੰਡ_ids, cctx->ctx_ufunc->uf_ਖੰਡ_ids,
						    ਆਕਾਰ(ਪੂਰਨ) * ਖੰਡ_ਡੂੰਘਾਈ);
		ufunc->uf_ਖੰਡ_ਡੂੰਘਾਈ = ਖੰਡ_ਡੂੰਘਾਈ;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ਜੇ (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    ਜੇ (cctx->ctx_compile_type == CT_DEBUG
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#ਜੇ_ਅੰਤ
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    ਜੇ (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	ਜੇ (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, ਨਿਰਾ);

    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_COMPILED)
    {
	// The return type will now be known.
	ਸੈੱਟ_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	ਵਾਪਸ generate_FUNCREF(cctx, ufunc, ਨਿਰਾ, FALSE, 0, ਨਿਰਾ);
    }

    func_ptr_unref(ufunc);
    ਵਾਪਸ FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    ਪੂਰਨ
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	ਪੂਰਨ	    types_optional,
	evalarg_T   *evalarg)
{
    ਪੂਰਨ		r;
    ufunc_T	*ufunc;
    ਪੂਰਨ		save_sc_ਸੰਸਕਰਣ = current_sctx.sc_ਸੰਸਕਰਣ;

    // Get the funcref in "rettv".
    current_sctx.sc_ਸੰਸਕਰਣ = SCRIPT_ਸੰਸਕਰਣ_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, ਨਿਰਾ);
    current_sctx.sc_ਸੰਸਕਰਣ = save_sc_ਸੰਸਕਰਣ;
    ਜੇ (r != OK)
	ਵਾਪਸ r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ਜੇ (ufunc->uf_ret_type == ਨਿਰਾ || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, ਨਿਰਾ);

    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_COMPILED)
    {
	// The return type will now be known.
	ਸੈੱਟ_function_type(ufunc);
	ਵਾਪਸ OK;
    }
    clear_tv(rettv);
    ਵਾਪਸ FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*ਵਸਤੂ;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    ਪੂਰਨ		is_const;
    ਪੂਰਨ		is_all_const = TRUE;	// reset when non-const encountered

    ਜੇ (d == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	ਵਾਪਸ FAIL;
    }
    ਲਈ (;;)
    {
	char_u	    *ਕੁੰਜੀ = ਨਿਰਾ;

	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ(whitep, arg, cctx) == FAIL)
	{
	    *arg = ਨਿਰਾ;
	    ਜਾਓ failret;
	}

	ਜੇ (**arg == '}')
	    ਰੋਕੋ;

	ਜੇ (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    ਜੇ (compile_expr0(arg, cctx) == FAIL)
		ਜਾਓ failret;
	    isn = ((isn_T *)instr->ga_ਅੰਕੜੇ) + instr->ga_len - 1;
	    ਜੇ (isn->isn_type == ISN_PUSHNR)
	    {
		ਅੱਖਰ buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.ਸੰਖਿਆ);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    ਜੇ (isn->isn_type == ISN_PUSHS)
		ਕੁੰਜੀ = isn->isn_arg.string;
	    ਨਹੀਂਤਾਂ ਜੇ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		ਜਾਓ failret;
	    *arg = skipwhite(*arg);
	    ਜੇ (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_ਕੁੰਜੀ));
		ਜਾਓ failret;
	    }
	    ++*arg;
	}
	ਨਹੀਂਤਾਂ
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    ਕੁੰਜੀ = get_literal_ਕੁੰਜੀ(arg);
	    ਜੇ (ਕੁੰਜੀ == ਨਿਰਾ)
		ਜਾਓ failret;
	    ਜੇ (generate_PUSHS(cctx, &ਕੁੰਜੀ) == FAIL)
		ਜਾਓ failret;
	}

	// Check for duplicate keys, if using string keys.
	ਜੇ (ਕੁੰਜੀ != ਨਿਰਾ)
	{
	    ਵਸਤੂ = dict_ਲੱਭੋ(d, ਕੁੰਜੀ, -1);
	    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	    {
		semsg(_(e_duplicate_ਕੁੰਜੀ_ਵਿੱਚ_dictionary_str), ਕੁੰਜੀ);
		ਜਾਓ failret;
	    }
	    ਵਸਤੂ = dictitem_alloc(ਕੁੰਜੀ);
	    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	    {
		ਵਸਤੂ->di_tv.v_type = VAR_UNKNOWN;
		ਵਸਤੂ->di_tv.v_ਤਾਲਾ = 0;
		ਜੇ (dict_ਜੋੜੋ(d, ਵਸਤੂ) == FAIL)
		    dictitem_free(ਵਸਤੂ);
	    }
	}

	ਜੇ (**arg != ':')
	{
	    ਜੇ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ਨਹੀਂਤਾਂ
		semsg(_(e_missing_colon_ਵਿੱਚ_dictionary_str), *arg);
	    ਜਾਓ failret;
	}
	whitep = *arg + 1;
	ਜੇ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    ਜਾਓ failret;
	}

	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ(whitep, arg, cctx) == FAIL)
	{
	    *arg = ਨਿਰਾ;
	    ਜਾਓ failret;
	}

	ਜੇ (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    ਜਾਓ failret;
	ਜੇ (!is_const)
	    is_all_const = FALSE;
	++ਗਿਣਤੀ_2;

	whitep = *arg;
	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ(whitep, arg, cctx) == FAIL)
	{
	    *arg = ਨਿਰਾ;
	    ਜਾਓ failret;
	}
	ਜੇ (**arg == '}')
	    ਰੋਕੋ;
	ਜੇ (**arg != ',')
	{
	    semsg(_(e_missing_comma_ਵਿੱਚ_dictionary_str), *arg);
	    ਜਾਓ failret;
	}
	ਜੇ (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    ਜਾਓ failret;
	}
	whitep = *arg + 1;
	ਜੇ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    ਜਾਓ failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    ਜੇ (VIM_ISWHITE(**arg) && vim9_comment_ਸ਼ੁਰੂ(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    ਵਾਪਸ generate_NEWDICT(cctx, ਗਿਣਤੀ_2, FALSE);

failret:
    ਜੇ (*arg == ਨਿਰਾ)
    {
	semsg(_(e_missing_dict_ਸਿਰਾ_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    ਵਾਪਸ FAIL;
}

/*
 * Compile "&option".
 */
    ਸਥਿਰ ਪੂਰਨ
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*ਸ਼ੁਰੂ = *arg;
    ਪੂਰਨ		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    ਜੇ (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(ਸ਼ੁਰੂ, *arg - ਸ਼ੁਰੂ);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_ਸੰਖਿਆ ? &t_ਸੰਖਿਆ : &t_string;

	ret = generate_ਲੱਦੋ(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    ਵਾਪਸ ret;
}

/*
 * Compile "$VAR".
 */
    ਸਥਿਰ ਪੂਰਨ
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*ਸ਼ੁਰੂ = *arg;
    ਪੂਰਨ		len;
    ਪੂਰਨ		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    ਜੇ (len == 0)
    {
	semsg(_(e_syntax_error_at_str), ਸ਼ੁਰੂ);
	ਵਾਪਸ FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(ਸ਼ੁਰੂ, len + 1);
    ret = generate_ਲੱਦੋ(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    ਵਾਪਸ ret;
}

/*
 * Compile $"string" or $'string'.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    ਪੂਰਨ		ret;
    ਪੂਰਨ		quote;
    ਪੂਰਨ		evaluate = cctx->ctx_skip != SKIP_YES;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ਲਈ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ਜੇ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ਨਹੀਂਤਾਂ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ਜੇ (ret == FAIL)
	    ਰੋਕੋ;
	ਜੇ (evaluate)
	{
	    ਜੇ ((tv.vval.v_string != ਨਿਰਾ && *tv.vval.v_string != NUL)
		    || (**arg != '{' && ਗਿਣਤੀ_2 == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		ਜੇ (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    ਵਾਪਸ FAIL;
		tv.vval.v_string = ਨਿਰਾ;  // don't free it now
		++ਗਿਣਤੀ_2;
	    }
	    clear_tv(&tv);
	}

	ਜੇ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ਰੋਕੋ;
	}

	p = compile_one_ਵਿਅੰਜਕ_ਵਿੱਚ_str(*arg, cctx);
	ਜੇ (p == ਨਿਰਾ)
	{
	    ret = FAIL;
	    ਰੋਕੋ;
	}
	++ਗਿਣਤੀ_2;
	*arg = p;
    }

    ਜੇ (ret == FAIL || !evaluate)
	ਵਾਪਸ ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ਜੇ (ਗਿਣਤੀ_2 > 1)
	ਵਾਪਸ generate_CONCAT(cctx, ਗਿਣਤੀ_2);

    ਵਾਪਸ OK;
}

/*
 * Compile "@r".
 */
    ਸਥਿਰ ਪੂਰਨ
compile_get_ਰਜਿਸਟਰ(char_u **arg, cctx_T *cctx)
{
    ਪੂਰਨ		ret;

    ++*arg;
    ਜੇ (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	ਵਾਪਸ FAIL;
    }
    ret = generate_ਲੱਦੋ(cctx, ISN_LOADREG, **arg, ਨਿਰਾ, &t_string);
    ++*arg;
    ਵਾਪਸ ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ਸਥਿਰ ਪੂਰਨ
apply_leader(typval_T *rettv, ਪੂਰਨ numeric_only, char_u *ਸ਼ੁਰੂ, char_u **ਸਿਰਾ)
{
    char_u *p = *ਸਿਰਾ;

    // this works from end to start
    ਜਦੋਂ (p > ਸ਼ੁਰੂ)
    {
	--p;
	ਜੇ (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    ਜੇ (rettv->v_type == VAR_FLOAT)
	    {
		ਜੇ (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		varnumber_T	val;
		ਪੂਰਨ		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		ਜੇ (check_not_string(rettv) == FAIL)
		    ਵਾਪਸ FAIL;
		val = tv_get_ਸੰਖਿਆ_chk(rettv, &error);
		clear_tv(rettv);
		ਜੇ (error)
		    ਵਾਪਸ FAIL;
		ਜੇ (*p == '-')
		    val = -val;
		rettv->v_type = VAR_ਸੰਖਿਆ;
		rettv->vval.v_ਸੰਖਿਆ = val;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (numeric_only)
	{
	    ++p;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '!')
	{
	    ਪੂਰਨ v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ਸੰਖਿਆ = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *ਸਿਰਾ = p;
    ਵਾਪਸ OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    ਸਥਿਰ ਖਾਲੀ
get_vim_constant(char_u **arg, typval_T *rettv)
{
    ਜੇ (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;
	*arg += 6;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_ਸੰਖਿਆ = VVAL_FALSE;
	*arg += 7;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ਸੰਖਿਆ = VVAL_NULL;
	*arg += 6;
    }
    ਨਹੀਂਤਾਂ ਜੇ (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ਸੰਖਿਆ = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, ਪੂਰਨ *len, ਪੂਰਨ *type_is)
{
    exprtype_T	type = ਵਿਅੰਜਕ_UNKNOWN;
    ਪੂਰਨ		i;

    ਚੋਣ (p[0])
    {
	ਹਾਲਤ '=':   ਜੇ (p[1] == '=')
			type = ਵਿਅੰਜਕ_EQUAL;
		    ਨਹੀਂਤਾਂ ਜੇ (p[1] == '~')
			type = ਵਿਅੰਜਕ_ਮੇਲ;
		    ਰੋਕੋ;
	ਹਾਲਤ '!':   ਜੇ (p[1] == '=')
			type = ਵਿਅੰਜਕ_NEQUAL;
		    ਨਹੀਂਤਾਂ ਜੇ (p[1] == '~')
			type = ਵਿਅੰਜਕ_NOMATCH;
		    ਰੋਕੋ;
	ਹਾਲਤ '>':   ਜੇ (p[1] != '=')
		    {
			type = ਵਿਅੰਜਕ_GREATER;
			*len = 1;
		    }
		    ਨਹੀਂਤਾਂ
			type = ਵਿਅੰਜਕ_GEQUAL;
		    ਰੋਕੋ;
	ਹਾਲਤ '<':   ਜੇ (p[1] != '=')
		    {
			type = ਵਿਅੰਜਕ_SMALLER;
			*len = 1;
		    }
		    ਨਹੀਂਤਾਂ
			type = ਵਿਅੰਜਕ_SEQUAL;
		    ਰੋਕੋ;
	ਹਾਲਤ 'i':   ਜੇ (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			ਜੇ (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			ਜੇ (!ਸੁਰੱਖਿਅਤ_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? ਵਿਅੰਜਕ_IS : ਵਿਅੰਜਕ_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    ਰੋਕੋ;
    }
    ਵਾਪਸ type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    ਖਾਲੀ
skip_ਵਿਅੰਜਕ_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_ਵਿਅੰਜਕ(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, ਨਿਰਾ);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    ਪੂਰਨ
bool_on_ਢੇਰ(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_ਢੇਰ(cctx, 0);
    ਜੇ (type == &t_bool)
	ਵਾਪਸ OK;

    ਜੇ (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_ਸੰਖਿਆ
	    || type == &t_ਸੰਖਿਆ_bool
	    || type == &t_const_ਸੰਖਿਆ_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	ਵਾਪਸ generate_COND2BOOL(cctx);

    ਵਾਪਸ need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    ਖਾਲੀ
error_white_both(char_u *op, ਪੂਰਨ len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_leader(cctx_T *cctx, ਪੂਰਨ numeric_only, char_u *ਸ਼ੁਰੂ, char_u **ਸਿਰਾ)
{
    char_u	*p = *ਸਿਰਾ;

    // this works from end to start
    ਜਦੋਂ (p > ਸ਼ੁਰੂ)
    {
	--p;
	ਜਦੋਂ (VIM_ISWHITE(*p))
	    --p;
	ਜੇ (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_ਢੇਰ(cctx, 0);
	    ਜੇ (type->tt_type != VAR_FLOAT && need_type(type, &t_ਸੰਖਿਆ,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ਵਾਪਸ FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    ਜੇ (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (numeric_only)
	{
	    ++p;
	    ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ  invert = *p == '!';

	    ਜਦੋਂ (p > ਸ਼ੁਰੂ && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		ਜੇ (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    ਜੇ (check_type_is_ਮੁੱਲ(get_type_on_ਢੇਰ(cctx, 0)) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (generate_2BOOL(cctx, invert, -1) == FAIL)
		ਵਾਪਸ FAIL;
	}
    }
    *ਸਿਰਾ = p;
    ਵਾਪਸ OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ਪੂਰਨ	    ret;
    char_u  *p = *arg + 1;

    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(p, arg, cctx) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (**arg == ')')
	// empty tuple
	ਵਾਪਸ compile_tuple(arg, cctx, ppconst, FALSE);

    ਜੇ (ppconst->pp_ਵਰਤਿਆ <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    ਨਹੀਂਤਾਂ
    {
	// Not enough space in ppconst, flush constants.
	ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
	    ਵਾਪਸ FAIL;
	ret = compile_expr0(arg, cctx);
    }
    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(*arg, arg, cctx) == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ (ret == OK && **arg == ',')
    {
	// tuple
	ਪੂਰਨ is_const = ppconst->pp_ਵਰਤਿਆ > 0 || ppconst->pp_is_const;
	ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
	    ਵਾਪਸ FAIL;
	ਵਾਪਸ compile_tuple(arg, cctx, ppconst, is_const);
    }

    ਜੇ (**arg == ')')
	++*arg;
    ਨਹੀਂਤਾਂ ਜੇ (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    ਵਾਪਸ ret;
}

ਸਥਿਰ ਪੂਰਨ compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *ਸ਼ੁਰੂ_leader,
	char_u **ਸਿਰਾ_leader,
	ppconst_T *ppconst)
{
    char_u	*name_ਸ਼ੁਰੂ = *ਸਿਰਾ_leader;
    ਪੂਰਨ		keeping_dict = FALSE;

    ਲਈ (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	ਜੇ (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_ਸ਼ੁਰੂ(p)))
	{
	    char_u *ਅਗਲਾ = peek_ਅਗਲਾ_ਸਤਰ_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    ਜੇ (ਅਗਲਾ != ਨਿਰਾ &&
		    ((ਅਗਲਾ[0] == '-' && ਅਗਲਾ[1] == '>'
				 && (ਅਗਲਾ[2] == '{'
				       || *skipwhite(ਅਗਲਾ + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(ਅਗਲਾ + 2))))
		    || (ਅਗਲਾ[0] == '.' && eval_isdictc(ਅਗਲਾ[1]))))
	    {
		ਅਗਲਾ = ਅਗਲਾ_ਸਤਰ_from_context(cctx, TRUE);
		ਜੇ (ਅਗਲਾ == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		*arg = ਅਗਲਾ;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	ਜੇ (**arg == '(')
	{
	    ਪੂਰਨ		argcount = 0;

	    ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
		ਵਾਪਸ FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_ਢੇਰ(cctx, 0);

	    *arg = skipwhite(p + 1);
	    ਜੇ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (generate_PCALL(cctx, argcount, name_ਸ਼ੁਰੂ, type, TRUE) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ਜੇ (generate_instr(cctx, ISN_CLEARDICT) == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    ਪੂਰਨ	    alt;
	    char_u  *paren;

	    // something->method()
	    ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
		ਵਾਪਸ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ਢੇਰ(cctx, 0);
	    ਜੇ (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_ਮੁੱਲ));
		ਵਾਪਸ FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    ਜੇ (compile_leader(cctx, TRUE, ਸ਼ੁਰੂ_leader, ਸਿਰਾ_leader) == FAIL)
		ਵਾਪਸ FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    ਜੇ (**arg == '(')
		alt = 2;
	    ਨਹੀਂਤਾਂ
	    {
		// alternative 1 or 3
		p = *arg;
		ਜੇ (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    ਵਾਪਸ FAIL;
		}
		ਜੇ (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		ਲਈ ( ; eval_isnamec(*p); ++p)
		    ;
		ਜੇ (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    ਜੇ (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    ਜੇ (paren == ਨਿਰਾ)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			ਵਾਪਸ FAIL;
		    }
		}
	    }

	    ਜੇ (alt != 1)
	    {
		ਪੂਰਨ	    argcount = 1;
		garray_T    *ਢੇਰ = &cctx->ctx_type_ਢੇਰ;
		ਪੂਰਨ	    type_idx_ਸ਼ੁਰੂ = ਢੇਰ->ga_len;
		ਪੂਰਨ	    ਵਿਅੰਜਕ_isn_ਸ਼ੁਰੂ = cctx->ctx_instr.ga_len;
		ਪੂਰਨ	    ਵਿਅੰਜਕ_isn_ਸਿਰਾ;
		ਪੂਰਨ	    arg_isn_ਗਿਣਤੀ;

		ਜੇ (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    ਜੇ (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			ਵਾਪਸ FAIL;
		}
		ਨਹੀਂਤਾਂ
		{
		    ਪੂਰਨ fail;
		    ਪੂਰਨ save_len = cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len;
		    ਪੂਰਨ	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_ਵਿਅੰਜਕ;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_ਵਿਅੰਜਕ;
		    cctx->ctx_ufunc->uf_ਸਤਰਾਂ.ga_len = save_len;

		    ਜੇ (fail)
		    {
			ਜੇ (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_ਵਿਅੰਜਕ_str), pstart);
			ਵਾਪਸ FAIL;
		    }
		}

		// Compile the arguments.
		ਜੇ (**arg != '(')
		{
		    ਜੇ (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    ਨਹੀਂਤਾਂ
			semsg(_(e_missing_parenthesis_str), *arg);
		    ਵਾਪਸ FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		ਵਿਅੰਜਕ_isn_ਸਿਰਾ = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		ਜੇ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    ਵਾਪਸ FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_ਗਿਣਤੀ = cctx->ctx_instr.ga_len - ਵਿਅੰਜਕ_isn_ਸਿਰਾ;
		ਜੇ (arg_isn_ਗਿਣਤੀ > 0)
		{
		    ਪੂਰਨ	    ਵਿਅੰਜਕ_isn_ਗਿਣਤੀ = ਵਿਅੰਜਕ_isn_ਸਿਰਾ - ਵਿਅੰਜਕ_isn_ਸ਼ੁਰੂ;
		    isn_T   *isn = ALLOC_MULT(isn_T, ਵਿਅੰਜਕ_isn_ਗਿਣਤੀ);
		    type_T  *decl_type;
		    type2_T  *typep;

		    ਜੇ (isn == ਨਿਰਾ)
			ਵਾਪਸ FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_ਅੰਕੜੇ)
							      + ਵਿਅੰਜਕ_isn_ਸ਼ੁਰੂ,
					       ਆਕਾਰ(isn_T) * ਵਿਅੰਜਕ_isn_ਗਿਣਤੀ);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_ਅੰਕੜੇ)
							      + ਵਿਅੰਜਕ_isn_ਸ਼ੁਰੂ,
			     ((isn_T *)cctx->ctx_instr.ga_ਅੰਕੜੇ) + ਵਿਅੰਜਕ_isn_ਸਿਰਾ,
						ਆਕਾਰ(isn_T) * arg_isn_ਗਿਣਤੀ);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_ਅੰਕੜੇ)
					      + ਵਿਅੰਜਕ_isn_ਸ਼ੁਰੂ + arg_isn_ਗਿਣਤੀ,
					  isn, ਆਕਾਰ(isn_T) * ਵਿਅੰਜਕ_isn_ਗਿਣਤੀ);
		    vim_free(isn);

		    typep = ((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + type_idx_ਸ਼ੁਰੂ;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + type_idx_ਸ਼ੁਰੂ,
			      ((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + type_idx_ਸ਼ੁਰੂ + 1,
			      ਆਕਾਰ(type2_T)
				       * (ਢੇਰ->ga_len - type_idx_ਸ਼ੁਰੂ - 1));
		    typep = ((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + ਢੇਰ->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_ਢੇਰ(cctx, 0);
		ਜੇ (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    ਵਾਪਸ FAIL;
	    }

	    ਜੇ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ਜੇ (generate_instr(cctx, ISN_CLEARDICT) == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (**arg == '[')
	{
	    ਪੂਰਨ		is_ਟੁਕੜਾ = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
		ਵਾਪਸ FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(p, arg, cctx) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਜੇ (compile_expr0(arg, cctx) == FAIL)
		    ਵਾਪਸ FAIL;
		ਜੇ (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ਵਾਪਸ FAIL;
		}
		ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(*arg, arg, cctx) == FAIL)
		    ਵਾਪਸ FAIL;
		*arg = skipwhite(*arg);
	    }
	    ਜੇ (**arg == ':')
	    {
		is_ਟੁਕੜਾ = TRUE;
		++*arg;
		ਜੇ (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ਵਾਪਸ FAIL;
		}
		ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(*arg, arg, cctx) == FAIL)
		    ਵਾਪਸ FAIL;
		ਜੇ (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (compile_expr0(arg, cctx) == FAIL)
			ਵਾਪਸ FAIL;
		    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(*arg, arg, cctx) == FAIL)
			ਵਾਪਸ FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    ਜੇ (**arg != ']')
	    {
		emsg(_(e_missing_closing_ਵਰਗਫਲ_brace));
		ਵਾਪਸ FAIL;
	    }
	    *arg = *arg + 1;

	    ਜੇ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ਜੇ (generate_instr(cctx, ISN_CLEARDICT) == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
	    }
	    ਜੇ (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_ਟੁਕੜਾ, &keeping_dict, cctx) == FAIL)
		ਵਾਪਸ FAIL;
	}
	ਨਹੀਂਤਾਂ ਜੇ (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
		ਵਾਪਸ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ਢੇਰ(cctx, 0);
	    ਜੇ (type != &t_unknown
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
		ਜੇ (compile_class_object_ਸੂਚਕਾਂਕ(cctx, arg, type) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		*arg = p + 1;
		ਜੇ (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    ਵਾਪਸ FAIL;
		}
		p = *arg;
		ਜਦੋਂ (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		ਜੇ (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    ਵਾਪਸ FAIL;
		}
		ਜੇ (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == ਨਿਰਾ)
		    ਵਾਪਸ FAIL;
		ਜੇ (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    ਵਾਪਸ FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਰੋਕੋ;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    ਜੇ (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*ਸ਼ੁਰੂ_leader, *ਸਿਰਾ_leader;
    ਪੂਰਨ		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ];
    ਪੂਰਨ		ਵਰਤਿਆ_before = ppconst->pp_ਵਰਤਿਆ;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    ਸ਼ੁਰੂ_leader = *arg;
    ਜੇ (eval_leader(arg, TRUE) == FAIL)
	ਵਾਪਸ FAIL;
    ਸਿਰਾ_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    ਚੋਣ (**arg)
    {
	/*
	 * Number constant.
	 */
	ਹਾਲਤ '0':	// also for blob starting with 0z
	ਹਾਲਤ '1':
	ਹਾਲਤ '2':
	ਹਾਲਤ '3':
	ਹਾਲਤ '4':
	ਹਾਲਤ '5':
	ਹਾਲਤ '6':
	ਹਾਲਤ '7':
	ਹਾਲਤ '8':
	ਹਾਲਤ '9':
	ਹਾਲਤ '.':   ਜੇ (eval_ਸੰਖਿਆ(arg, rettv, TRUE, FALSE) == FAIL)
			ਵਾਪਸ FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    ਜੇ (apply_leader(rettv, TRUE,
					    ਸ਼ੁਰੂ_leader, &ਸਿਰਾ_leader) == FAIL)
		    {
			clear_tv(rettv);
			ਵਾਪਸ FAIL;
		    }
		    ਰੋਕੋ;

	/*
	 * String constant: "string".
	 */
	ਹਾਲਤ '"':   ਜੇ (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			ਵਾਪਸ FAIL;
		    ਰੋਕੋ;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	ਹਾਲਤ '\'':  ਜੇ (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			ਵਾਪਸ FAIL;
		    ਰੋਕੋ;

	/*
	 * Constant Vim variable.
	 */
	ਹਾਲਤ 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    ਰੋਕੋ;

	/*
	 * "true" constant
	 */
	ਹਾਲਤ 't':   ਜੇ (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_ਸੰਖਿਆ = VVAL_TRUE;
		    }
		    ਨਹੀਂਤਾਂ
			ret = NOTDONE;
		    ਰੋਕੋ;

	/*
	 * "false" constant
	 */
	ਹਾਲਤ 'f':   ਜੇ (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_ਸੰਖਿਆ = VVAL_FALSE;
		    }
		    ਨਹੀਂਤਾਂ
			ret = NOTDONE;
		    ਰੋਕੋ;

	/*
	 * "null" or "null_*" constant
	 */
	ਹਾਲਤ 'n':   ਜੇ (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			ਪੂਰਨ	len;

			ਲਈ (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			ਜੇ (ret == FAIL)
			    ret = NOTDONE;
			ਨਹੀਂਤਾਂ
			    *arg += len + 4;
		    }
		    ਨਹੀਂਤਾਂ
			ret = NOTDONE;
		    ਰੋਕੋ;

	/*
	 * List: [expr, expr]
	 */
	ਹਾਲਤ '[':   ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
			ਵਾਪਸ FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    ਰੋਕੋ;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	ਹਾਲਤ '{':   ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
			ਵਾਪਸ FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    ਰੋਕੋ;

	/*
	 * Option value: &name
	 */
	ਹਾਲਤ '&':	ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
			    ਵਾਪਸ FAIL;
			ret = compile_get_option(arg, cctx);
			ਰੋਕੋ;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	ਹਾਲਤ '$':	ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
			    ਵਾਪਸ FAIL;
			ਜੇ ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			ਨਹੀਂਤਾਂ
			    ret = compile_get_env(arg, cctx);
			ਰੋਕੋ;

	/*
	 * Register contents: @r.
	 */
	ਹਾਲਤ '@':	ਜੇ (generate_ppconst(cctx, ppconst) == FAIL)
			    ਵਾਪਸ FAIL;
			ret = compile_get_ਰਜਿਸਟਰ(arg, cctx);
			ਰੋਕੋ;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	ਹਾਲਤ '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    ਜੇ (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    ਰੋਕੋ;

	ਮੂਲ:    ret = NOTDONE;
		    ਰੋਕੋ;
    }
    ਜੇ (ret == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (rettv->v_type != VAR_UNKNOWN && ਵਰਤਿਆ_before == ppconst->pp_ਵਰਤਿਆ)
    {
	ਜੇ (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	ਨਹੀਂਤਾਂ
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_ਵਰਤਿਆ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (ret == NOTDONE)
    {
	char_u	    *p;
	ਪੂਰਨ	    r;

	ਜੇ (!eval_isnamec1(**arg))
	{
	    ਜੇ (!vim9_bad_comment(*arg))
	    {
		ਜੇ (ਸਿਰੇ_excmd(*skipwhite(*arg)))
		    semsg(_(e_ਖਾਲੀ_ਥਾਵਾਂ_ਵਿਅੰਜਕ_str), *arg);
		ਨਹੀਂਤਾਂ
		    semsg(_(e_name_expected_str), *arg);
	    }
	    ਵਾਪਸ FAIL;
	}

	// "name" or "name()"
	p = ਮੰਜ਼ਿਲ_name_ਸਿਰਾ(*arg, TRUE);
	ਜੇ (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_ਇੱਥੇ));
	    ਵਾਪਸ FAIL;
	}

	size_t namelen = p - *arg;
	ਜੇ (*p == '<')
	{
	    ਜੇ (skip_generic_func_type_ਦਲੀਲਾਂ(&p) == FAIL)
		ਵਾਪਸ FAIL;
	}

	ਜੇ (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		ਵਾਪਸ FAIL;
	    r = compile_ਲੱਦੋ(arg, namelen, p, cctx, TRUE, TRUE);
	}
	ਜੇ (r == FAIL)
	    ਵਾਪਸ FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    ਜੇ (compile_subscript(arg, cctx, ਸ਼ੁਰੂ_leader, &ਸਿਰਾ_leader,
							     ppconst) == FAIL)
	ਵਾਪਸ FAIL;
    ਜੇ ((ppconst->pp_ਵਰਤਿਆ > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ - 1];
	ਜੇ (apply_leader(rettv, FALSE, ਸ਼ੁਰੂ_leader, &ਸਿਰਾ_leader) == FAIL)
	    ਵਾਪਸ FAIL;
	ਵਾਪਸ OK;
    }
    ਜੇ (compile_leader(cctx, FALSE, ਸ਼ੁਰੂ_leader, &ਸਿਰਾ_leader) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    ਸਥਿਰ ਪੂਰਨ
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = ਨਿਰਾ;

    // Recognize <type>
    ਜੇ (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ਜੇ (want_type == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਜੇ (**arg != '>')
	{
	    ਜੇ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    ਨਹੀਂਤਾਂ
		emsg(_(e_missing_gt));
	    ਵਾਪਸ FAIL;
	}
	++*arg;
	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(*arg, arg, cctx) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਜੇ (compile_expr9(arg, cctx, ppconst) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (want_type != ਨਿਰਾ)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_ਕਿਸਮ = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_ਢੇਰ(cctx, 0);
	ਜੇ (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    ਜੇ (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		ਵਾਪਸ FAIL;
	}
    }

    ਵਾਪਸ OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    ਸਥਿਰ ਪੂਰਨ
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*ਅਗਲਾ;
    ਪੂਰਨ		ppconst_ਵਰਤਿਆ = ppconst->pp_ਵਰਤਿਆ;

    // get the first expression
    ਜੇ (compile_expr8(arg, cctx, ppconst) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    ਲਈ (;;)
    {
	op = may_peek_ਅਗਲਾ_ਸਤਰ(cctx, *arg, &ਅਗਲਾ);
	ਜੇ (*op != '*' && *op != '/' && *op != '%')
	    ਰੋਕੋ;
	ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
	{
	    *arg = ਅਗਲਾ_ਸਤਰ_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ਜੇ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(op + 1, arg, cctx) == FAIL)
	    ਵਾਪਸ FAIL;

	// get the second expression
	ਜੇ (compile_expr8(arg, cctx, ppconst) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (ppconst->pp_ਵਰਤਿਆ == ppconst_ਵਰਤਿਆ + 2
		&& ppconst->pp_tv[ppconst_ਵਰਤਿਆ].v_type == VAR_ਸੰਖਿਆ
		&& ppconst->pp_tv[ppconst_ਵਰਤਿਆ + 1].v_type == VAR_ਸੰਖਿਆ)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_ਵਰਤਿਆ];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_ਵਰਤਿਆ + 1];
	    varnumber_T	    res = 0;
	    ਪੂਰਨ		    failed = FALSE;

	    // both are numbers: compute the result
	    ਚੋਣ (*op)
	    {
		ਹਾਲਤ '*': res = tv1->vval.v_ਸੰਖਿਆ * tv2->vval.v_ਸੰਖਿਆ;
			  ਰੋਕੋ;
		ਹਾਲਤ '/': res = ਅੰਕ_divide(tv1->vval.v_ਸੰਖਿਆ,
						  tv2->vval.v_ਸੰਖਿਆ, &failed);
			  ਰੋਕੋ;
		ਹਾਲਤ '%': res = ਅੰਕ_modulus(tv1->vval.v_ਸੰਖਿਆ,
						  tv2->vval.v_ਸੰਖਿਆ, &failed);
			  ਰੋਕੋ;
	    }
	    ਜੇ (failed)
		ਵਾਪਸ FAIL;
	    tv1->vval.v_ਸੰਖਿਆ = res;
	    --ppconst->pp_ਵਰਤਿਆ;
	}
	ਨਹੀਂਤਾਂ
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    ਵਾਪਸ OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    ਸਥਿਰ ਪੂਰਨ
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*ਅਗਲਾ;
    ਪੂਰਨ		oplen;
    ਪੂਰਨ		ppconst_ਵਰਤਿਆ = ppconst->pp_ਵਰਤਿਆ;

    // get the first variable
    ਜੇ (compile_expr7(arg, cctx, ppconst) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    ਲਈ (;;)
    {
	op = may_peek_ਅਗਲਾ_ਸਤਰ(cctx, *arg, &ਅਗਲਾ);
	ਜੇ (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    ਰੋਕੋ;
	ਜੇ (op[0] == op[1] && *op != '.' && ਅਗਲਾ)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    ਰੋਕੋ;
	oplen = (*op == '.' ? 2 : 1);
	ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
	{
	    *arg = ਅਗਲਾ_ਸਤਰ_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ਜੇ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(op + oplen, arg, cctx) == FAIL)
	    ਵਾਪਸ FAIL;

	// get the second expression
	ਜੇ (compile_expr7(arg, cctx, ppconst) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (ppconst->pp_ਵਰਤਿਆ == ppconst_ਵਰਤਿਆ + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_ਵਰਤਿਆ].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_ਵਰਤਿਆ + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_ਵਰਤਿਆ].v_type == VAR_ਸੰਖਿਆ
		    && ppconst->pp_tv[ppconst_ਵਰਤਿਆ + 1].v_type == VAR_ਸੰਖਿਆ)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_ਵਰਤਿਆ];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_ਵਰਤਿਆ + 1];

	    // concat/subtract/add constant numbers
	    ਜੇ (*op == '+')
		tv1->vval.v_ਸੰਖਿਆ = tv1->vval.v_ਸੰਖਿਆ + tv2->vval.v_ਸੰਖਿਆ;
	    ਨਹੀਂਤਾਂ ਜੇ (*op == '-')
		tv1->vval.v_ਸੰਖਿਆ = tv1->vval.v_ਸੰਖਿਆ - tv2->vval.v_ਸੰਖਿਆ;
	    ਨਹੀਂਤਾਂ
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		ਜੇ (tv1->vval.v_string == ਨਿਰਾ)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    ਵਾਪਸ FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_ਵਰਤਿਆ;
	}
	ਨਹੀਂਤਾਂ
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    ਜੇ (*op == '.')
	    {
		ਜੇ (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    ਵਾਪਸ FAIL;
		ਜੇ (generate_CONCAT(cctx, 2) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	    ਨਹੀਂਤਾਂ
		generate_two_op(cctx, op);
	}
    }

    ਵਾਪਸ OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    ਸਥਿਰ ਪੂਰਨ
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ਵਿਅੰਜਕ_UNKNOWN;
    char_u	*p;
    char_u	*ਅਗਲਾ;
    ਪੂਰਨ		len = 2;
    ਪੂਰਨ		ppconst_ਵਰਤਿਆ = ppconst->pp_ਵਰਤਿਆ;
    isn_T	*isn;

    // get the first variable
    ਜੇ (compile_expr6(arg, cctx, ppconst) == FAIL)
	ਵਾਪਸ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    ਲਈ (;;)
    {
	type = ਵਿਅੰਜਕ_UNKNOWN;

	p = may_peek_ਅਗਲਾ_ਸਤਰ(cctx, *arg, &ਅਗਲਾ);
	ਜੇ (p[0] == '<' && p[1] == '<')
	    type = ਵਿਅੰਜਕ_LSHIFT;
	ਨਹੀਂਤਾਂ ਜੇ (p[0] == '>' && p[1] == '>')
	    type = ਵਿਅੰਜਕ_RSHIFT;

	ਜੇ (type == ਵਿਅੰਜਕ_UNKNOWN)
	    ਵਾਪਸ OK;

	// Handle a bitwise left or right shift operator
	ਜੇ (ppconst->pp_ਵਰਤਿਆ == ppconst_ਵਰਤਿਆ + 1)
	{
	    ਜੇ (ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ - 1].v_type != VAR_ਸੰਖਿਆ)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_ਸੰਖਿਆ));
		ਵਾਪਸ FAIL;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    type_T	*t = get_type_on_ਢੇਰ(cctx, 0);

	    ਜੇ (need_type(t, &t_ਸੰਖਿਆ, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_ਸੰਖਿਆ));
		ਵਾਪਸ FAIL;
	    }
	}

	ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
	{
	    *arg = ਅਗਲਾ_ਸਤਰ_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ਜੇ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ਵਾਪਸ FAIL;
	}

	// get the second variable
	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(p + len, arg, cctx) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (compile_expr6(arg, cctx, ppconst) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (ppconst->pp_ਵਰਤਿਆ == ppconst_ਵਰਤਿਆ + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ - 1];

	    // Both sides are a constant, compute the result now.
	    ਜੇ (tv2->v_type != VAR_ਸੰਖਿਆ || tv2->vval.v_ਸੰਖਿਆ < 0)
	    {
		// right operand should be a positive number
		ਜੇ (tv2->v_type != VAR_ਸੰਖਿਆ)
		    emsg(_(e_bitshift_ops_must_be_ਸੰਖਿਆ));
		ਨਹੀਂਤਾਂ
		    emsg(_(e_bitshift_ops_must_be_ਸਕਾਰਾਤਮਕ));
		ਵਾਪਸ FAIL;
	    }

	    ਜੇ (tv2->vval.v_ਸੰਖਿਆ > MAX_LSHIFT_ਬਿੱਟ)
		tv1->vval.v_ਸੰਖਿਆ = 0;
	    ਨਹੀਂਤਾਂ ਜੇ (type == ਵਿਅੰਜਕ_LSHIFT)
		tv1->vval.v_ਸੰਖਿਆ =
			(uvarnumber_T)tv1->vval.v_ਸੰਖਿਆ << tv2->vval.v_ਸੰਖਿਆ;
	    ਨਹੀਂਤਾਂ
		tv1->vval.v_ਸੰਖਿਆ =
			(uvarnumber_T)tv1->vval.v_ਸੰਖਿਆ >> tv2->vval.v_ਸੰਖਿਆ;
	    clear_tv(tv2);
	    --ppconst->pp_ਵਰਤਿਆ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (need_type(get_type_on_ਢੇਰ(cctx, 0), &t_ਸੰਖਿਆ, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_ਸੰਖਿਆ));
		ਵਾਪਸ FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    ਜੇ (isn == ਨਿਰਾ)
		ਵਾਪਸ FAIL;

	    ਜੇ (isn != ਨਿਰਾ)
		isn->isn_arg.op.op_type = type;
	}
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ਵਿਅੰਜਕ_UNKNOWN;
    char_u	*p;
    char_u	*ਅਗਲਾ;
    ਪੂਰਨ		len = 2;
    ਪੂਰਨ		type_is = FALSE;
    ਪੂਰਨ		ppconst_ਵਰਤਿਆ = ppconst->pp_ਵਰਤਿਆ;

    // get the first variable
    ਜੇ (compile_expr5(arg, cctx, ppconst) == FAIL)
	ਵਾਪਸ FAIL;

    p = may_peek_ਅਗਲਾ_ਸਤਰ(cctx, *arg, &ਅਗਲਾ);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ਜੇ (type != ਵਿਅੰਜਕ_UNKNOWN)
    {
	ਪੂਰਨ ic = FALSE;  // Default: do not ignore case

	ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
	{
	    *arg = ਅਗਲਾ_ਸਤਰ_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	ਜੇ (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_ਵਿਅੰਜਕ_str), *arg);
	    ਵਾਪਸ FAIL;
	}
	// extra question mark appended: ignore case
	ਜੇ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	ਨਹੀਂਤਾਂ ਜੇ (p[len] == '#')
	    ++len;
	// nothing appended: match case

	ਜੇ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ਵਾਪਸ FAIL;
	}

	// get the second variable
	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(p + len, arg, cctx) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (compile_expr5(arg, cctx, ppconst) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (ppconst->pp_ਵਰਤਿਆ == ppconst_ਵਰਤਿਆ + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ - 1];
	    ਪੂਰਨ		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    ਜੇ (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    ਨਹੀਂਤਾਂ
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_ਸੰਖਿਆ = tv1->vval.v_ਸੰਖਿਆ
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_ਵਰਤਿਆ;
	    }
	    ਵਾਪਸ ret;
	}

	generate_ppconst(cctx, ppconst);
	ਵਾਪਸ generate_COMPARE(cctx, type, ic);
    }

    ਵਾਪਸ OK;
}

ਸਥਿਰ ਪੂਰਨ compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    ਸਥਿਰ ਪੂਰਨ
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	ਅੱਖਰ	*op,
	ppconst_T *ppconst,
	ਪੂਰਨ	ppconst_ਵਰਤਿਆ ਅਣਵਰਤਿਆ)
{
    char_u	*ਅਗਲਾ;
    char_u	*p = may_peek_ਅਗਲਾ_ਸਤਰ(cctx, *arg, &ਅਗਲਾ);
    ਪੂਰਨ		opchar = *op;

    ਜੇ (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	ਸਿਰਾ_ga;
	ਪੂਰਨ		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&ਸਿਰਾ_ga, ਆਕਾਰ(ਪੂਰਨ), 10);
	ਜਦੋਂ (p[0] == opchar && p[1] == opchar)
	{
	    ਲੰਮਾ	ਸ਼ੁਰੂ_lnum = SOURCING_LNUM;
	    ਲੰਮਾ	save_sourcing_lnum;
	    ਪੂਰਨ		ਸ਼ੁਰੂ_ctx_lnum = cctx->ctx_lnum;
	    ਪੂਰਨ		save_lnum;
	    ਪੂਰਨ		const_ਵਰਤਿਆ;
	    ਪੂਰਨ		ਸਥਿਤੀ;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
	    {
		*arg = ਅਗਲਾ_ਸਤਰ_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ਜੇ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&ਸਿਰਾ_ga);
		ਵਾਪਸ FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = ਸ਼ੁਰੂ_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = ਸ਼ੁਰੂ_ctx_lnum;

	    ਸਥਿਤੀ = check_ppconst_bool(ppconst);
	    ਜੇ (ਸਥਿਤੀ != FAIL)
	    {
		// Use the last ppconst if possible.
		ਜੇ (ppconst->pp_ਵਰਤਿਆ > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ - 1];
		    ਪੂਰਨ		is_true = tv2bool(tv);

		    ਜੇ ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_ਸੰਖਿਆ = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_ਵਰਤਿਆ;
			jump_when = JUMP_NEVER;
		    }
		}
		ਨਹੀਂਤਾਂ
		{
		    // Every part must evaluate to a bool.
		    ਸਥਿਤੀ = bool_on_ਢੇਰ(cctx);
		}
	    }
	    ਜੇ (ਸਥਿਤੀ != FAIL)
		ਸਥਿਤੀ = ga_grow(&ਸਿਰਾ_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    ਜੇ (ਸਥਿਤੀ == FAIL)
	    {
		ga_clear(&ਸਿਰਾ_ga);
		ਵਾਪਸ FAIL;
	    }

	    ਜੇ (jump_when != JUMP_NEVER)
	    {
		ਜੇ (cctx->ctx_skip != SKIP_YES)
		{
		    *(((ਪੂਰਨ *)ਸਿਰਾ_ga.ga_ਅੰਕੜੇ) + ਸਿਰਾ_ga.ga_len) = instr->ga_len;
		    ++ਸਿਰਾ_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&ਸਿਰਾ_ga);
		ਵਾਪਸ FAIL;
	    }

	    const_ਵਰਤਿਆ = ppconst->pp_ਵਰਤਿਆ;
	    ਜੇ ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&ਸਿਰਾ_ga);
		ਵਾਪਸ FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    ਜੇ (ppconst->pp_ਵਰਤਿਆ == const_ਵਰਤਿਆ + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_ਵਰਤਿਆ - 1];

		ਜੇ (tv->v_type == VAR_ਸੰਖਿਆ
			 && (tv->vval.v_ਸੰਖਿਆ == 1 || tv->vval.v_ਸੰਖਿਆ == 0))
		{
		    tv->vval.v_ਸੰਖਿਆ = tv->vval.v_ਸੰਖਿਆ == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_ਅਗਲਾ_ਸਤਰ(cctx, *arg, &ਅਗਲਾ);
	}

	ਜੇ (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&ਸਿਰਾ_ga);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (cctx->ctx_skip != SKIP_YES && ppconst->pp_ਵਰਤਿਆ == 0)
	    // Every part must evaluate to a bool.
	    ਜੇ (bool_on_ਢੇਰ(cctx) == FAIL)
	    {
		ga_clear(&ਸਿਰਾ_ga);
		ਵਾਪਸ FAIL;
	    }

	ਜੇ (ਸਿਰਾ_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    ਜਦੋਂ (ਸਿਰਾ_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--ਸਿਰਾ_ga.ga_len;
		isn = ((isn_T *)instr->ga_ਅੰਕੜੇ)
				  + *(((ਪੂਰਨ *)ਸਿਰਾ_ga.ga_ਅੰਕੜੇ) + ਸਿਰਾ_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&ਸਿਰਾ_ga);

	cctx->ctx_skip = save_skip;
    }

    ਵਾਪਸ OK;
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
    ਸਥਿਰ ਪੂਰਨ
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ਪੂਰਨ		ppconst_ਵਰਤਿਆ = ppconst->pp_ਵਰਤਿਆ;

    // get the first variable
    ਜੇ (compile_expr4(arg, cctx, ppconst) == FAIL)
	ਵਾਪਸ FAIL;

    // || and && work almost the same
    ਵਾਪਸ compile_and_or(arg, cctx, "&&", ppconst, ppconst_ਵਰਤਿਆ);
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
    ਸਥਿਰ ਪੂਰਨ
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ਪੂਰਨ		ppconst_ਵਰਤਿਆ = ppconst->pp_ਵਰਤਿਆ;

    // eval the first expression
    ਜੇ (compile_expr3(arg, cctx, ppconst) == FAIL)
	ਵਾਪਸ FAIL;

    // || and && work almost the same
    ਵਾਪਸ compile_and_or(arg, cctx, "||", ppconst, ppconst_ਵਰਤਿਆ);
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
    ਪੂਰਨ
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    ਪੂਰਨ		ppconst_ਵਰਤਿਆ = ppconst->pp_ਵਰਤਿਆ;
    char_u	*ਅਗਲਾ;

    // Ignore all kinds of errors when not producing code.
    ਜੇ (cctx->ctx_skip == SKIP_YES)
    {
	ਪੂਰਨ		prev_did_emsg = did_emsg;

	skip_ਵਿਅੰਜਕ_cctx(arg, cctx);
	ਵਾਪਸ did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    ਜੇ (compile_expr2(arg, cctx, ppconst) == FAIL)
	ਵਾਪਸ FAIL;

    p = may_peek_ਅਗਲਾ_ਸਤਰ(cctx, *arg, &ਅਗਲਾ);
    ਜੇ (*p == '?')
    {
	ਪੂਰਨ		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;
	ਪੂਰਨ		alt_idx = instr->ga_len;
	ਪੂਰਨ		ਸਿਰਾ_idx = 0;
	isn_T		*isn;
	type_T		*type1 = ਨਿਰਾ;
	ਪੂਰਨ		has_const_ਵਿਅੰਜਕ = FALSE;
	ਪੂਰਨ		const_ਮੁੱਲ = FALSE;
	ਪੂਰਨ		save_skip = cctx->ctx_skip;

	ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
	{
	    *arg = ਅਗਲਾ_ਸਤਰ_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ਜੇ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    ਵਾਪਸ FAIL;
	}

	ਜੇ (ppconst->pp_ਵਰਤਿਆ == ppconst_ਵਰਤਿਆ + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_ਵਿਅੰਜਕ = TRUE;
	    ਜੇ (op_falsy)
		const_ਮੁੱਲ = tv2bool(&ppconst->pp_tv[ppconst_ਵਰਤਿਆ]);
	    ਨਹੀਂਤਾਂ
	    {
		ਪੂਰਨ error = FALSE;

		const_ਮੁੱਲ = tv_get_bool_chk(&ppconst->pp_tv[ppconst_ਵਰਤਿਆ],
								       &error);
		ਜੇ (error)
		    ਵਾਪਸ FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_ਮੁੱਲ : !const_ਮੁੱਲ) ? SKIP_YES : SKIP_NOT;

	    ਜੇ (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    ਨਹੀਂਤਾਂ
	    {
		clear_tv(&ppconst->pp_tv[ppconst_ਵਰਤਿਆ]);
		--ppconst->pp_ਵਰਤਿਆ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    generate_ppconst(cctx, ppconst);
	    ਜੇ (op_falsy)
		ਸਿਰਾ_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    ਜੇ (op_falsy)
	    {
		type1 = get_type_on_ਢੇਰ(cctx, -1);
		ਜੇ (check_type_is_ਮੁੱਲ(type1) == FAIL)
		    ਵਾਪਸ FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (compile_expr1(arg, cctx, ppconst) == FAIL)
	    ਵਾਪਸ FAIL;

	ਜੇ (!has_const_ਵਿਅੰਜਕ)
	{
	    generate_ppconst(cctx, ppconst);

	    ਜੇ (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_ਢੇਰ(cctx, 0);
		--ਢੇਰ->ga_len;

		ਸਿਰਾ_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_ਅੰਕੜੇ) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	ਜੇ (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_ਅਗਲਾ_ਸਤਰ(cctx, *arg, &ਅਗਲਾ);
	    ਜੇ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ਵਾਪਸ FAIL;
	    }
	    ਜੇ (ਅਗਲਾ != ਨਿਰਾ)
	    {
		*arg = ਅਗਲਾ_ਸਤਰ_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ਜੇ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		ਵਾਪਸ FAIL;
	    }

	    // evaluate the third expression
	    ਜੇ (has_const_ਵਿਅੰਜਕ)
		cctx->ctx_skip = save_skip == SKIP_YES || const_ਮੁੱਲ
							 ? SKIP_YES : SKIP_NOT;
	    ਜੇ (may_get_ਅਗਲਾ_ਸਤਰ_error(p + 1, arg, cctx) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (compile_expr1(arg, cctx, ppconst) == FAIL)
		ਵਾਪਸ FAIL;
	}

	ਜੇ (!has_const_ਵਿਅੰਜਕ)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ)
					      + ਢੇਰ->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_ਅੰਕੜੇ) + ਸਿਰਾ_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    ਵਾਪਸ OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
compile_expr0_ext(char_u **arg,  cctx_T *cctx, ਪੂਰਨ *is_const)
{
    ppconst_T	ppconst;

    CLEAR_ਖੇਤਰ(ppconst);
    ਜੇ (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (is_const != ਨਿਰਾ)
	*is_const = ppconst.pp_ਵਰਤਿਆ > 0 || ppconst.pp_is_const;
    ਜੇ (generate_ppconst(cctx, &ppconst) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Toplevel expression.
 */
    ਪੂਰਨ
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    ਵਾਪਸ compile_expr0_ext(arg, cctx, ਨਿਰਾ);
}


#ਜੇ_ਅੰਤ // defined(FEAT_EVAL)
