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
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
സ്ഥിതം പൂർണ്ണസംഖ്യ paren_follows_after_വ്യഞ്ജകം = 0;

/*
 * Generate code for any ppconst entries.
 */
    പൂർണ്ണസംഖ്യ
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    പൂർണ്ണസംഖ്യ	    i;
    പൂർണ്ണസംഖ്യ	    ret = OK;
    പൂർണ്ണസംഖ്യ	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    വേണ്ടി (i = 0; i < ppconst->pp_used; ++i)
	എങ്കിൽ (generate_tv_തള്ളുക(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    തിരികെ ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_ppconst_bool(ppconst_T *ppconst)
{
    എങ്കിൽ (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	തിരികെ check_typval_type(&t_bool, tv, where);
    }
    തിരികെ OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    ശൂന്യം
clear_ppconst(ppconst_T *ppconst)
{
    പൂർണ്ണസംഖ്യ	    i;

    വേണ്ടി (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_tuple_member(
    type2_T	*typep,
    പൂർണ്ണസംഖ്യ		is_slice,
    cctx_T	*cctx)
{
    എങ്കിൽ (is_slice)
    {
	എങ്കിൽ (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    തിരികെ FAIL;
	// a copy is made so the member type is no longer declared
	എങ്കിൽ (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	എങ്കിൽ (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    എങ്കിൽ (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    എങ്കിൽ (typep->type_curr->tt_argcount == 1)
	    {
		എങ്കിൽ (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		അല്ലെങ്കിൽ
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    അല്ലെങ്കിൽ
		typep->type_curr = &t_any;
	    എങ്കിൽ (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		എങ്കിൽ (typep->type_decl->tt_argcount == 1)
		{
		    എങ്കിൽ (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    അല്ലെങ്കിൽ
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		അല്ലെങ്കിൽ
		    typep->type_curr = &t_any;
	    }
	    അല്ലെങ്കിൽ
		typep->type_decl = typep->type_curr;
	}
	എങ്കിൽ (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    പൂർണ്ണസംഖ്യ
compile_member(പൂർണ്ണസംഖ്യ is_slice, പൂർണ്ണസംഖ്യ *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)അട്ടി->ga_data) + അട്ടി->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)അട്ടി->ga_data) + അട്ടി->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    എങ്കിൽ ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    എങ്കിൽ (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	എങ്കിൽ (need_type(idxtype, &t_സംഖ്യ, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (is_slice)
	{
	    idxtype = get_type_on_അട്ടി(cctx, 1);
	    എങ്കിൽ (need_type(idxtype, &t_സംഖ്യ, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		തിരികെ FAIL;
	}
    }

    എങ്കിൽ (vartype == VAR_DICT)
    {
	എങ്കിൽ (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    തിരികെ FAIL;
	}
	എങ്കിൽ (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    എങ്കിൽ (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    എങ്കിൽ (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		എങ്കിൽ (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    അല്ലെങ്കിൽ
		typep->type_decl = typep->type_curr;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		തിരികെ FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	എങ്കിൽ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (keeping_dict != ശൂന്യ)
	    *keeping_dict = TRUE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	എങ്കിൽ ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vartype == VAR_BLOB)
    {
	എങ്കിൽ (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    എങ്കിൽ (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    typep->type_curr = &t_സംഖ്യ;
	    typep->type_decl = &t_സംഖ്യ;
	    എങ്കിൽ (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		തിരികെ FAIL;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vartype == VAR_TUPLE)
    {
	എങ്കിൽ (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	എങ്കിൽ (is_slice)
	{
	    എങ്കിൽ (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		തിരികെ FAIL;
	    // a copy is made so the member type is no longer declared
	    എങ്കിൽ (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    എങ്കിൽ (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		എങ്കിൽ (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		എങ്കിൽ (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		എങ്കിൽ (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    എങ്കിൽ (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		അല്ലെങ്കിൽ
			typep->type_decl = typep->type_curr;
	    }
	    എങ്കിൽ (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		തിരികെ FAIL;
	}
    }
    അല്ലെങ്കിൽ
    {
	തിരഞ്ഞെടുപ്പ് (vartype)
	{
	    അവസ്ഥ VAR_FUNC:
	    അവസ്ഥ VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		നിർത്തുക;
	    അവസ്ഥ VAR_BOOL:
	    അവസ്ഥ VAR_SPECIAL:
	    അവസ്ഥ VAR_JOB:
	    അവസ്ഥ VAR_CHANNEL:
	    അവസ്ഥ VAR_INSTR:
	    അവസ്ഥ VAR_CLASS:
	    അവസ്ഥ VAR_OBJECT:
	    അവസ്ഥ VAR_TYPEALIAS:
	    അവസ്ഥ VAR_UNKNOWN:
	    അവസ്ഥ VAR_ANY:
	    അവസ്ഥ VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    വേണ്ടി (cctx_T *cctx = cctx_arg; cctx != ശൂന്യ; cctx = cctx->ctx_outer)
    {
	എങ്കിൽ (cctx->ctx_ufunc != ശൂന്യ && cctx->ctx_ufunc->uf_class != ശൂന്യ)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    ആകുമ്പോൾ (clp != ശൂന്യ)
	    {
		എങ്കിൽ (clp == cl)
		    തിരികെ TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    തിരികെ FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    പൂർണ്ണസംഖ്യ
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    പൂർണ്ണസംഖ്യ		m_idx;
    പൂർണ്ണസംഖ്യ		ret = FAIL;

    എങ്കിൽ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	തിരികെ FAIL;
    }

    class_T *cl = type->tt_class;
    പൂർണ്ണസംഖ്യ is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    എങ്കിൽ (type == &t_super)
    {
	എങ്കിൽ (cctx->ctx_ufunc == ശൂന്യ || cctx->ctx_ufunc->uf_class == ശൂന്യ)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    തിരികെ FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_അട്ടി.ga_len;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	എങ്കിൽ (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    എങ്കിൽ (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    എങ്കിൽ (cl == ശൂന്യ)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	തിരികെ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ശൂന്യ, ശൂന്യ, FNE_CHECK_START);
    എങ്കിൽ (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	തിരികെ FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    എങ്കിൽ (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	എങ്കിൽ (name_end == ശൂന്യ)
	    പോകുക done;
    }

    എങ്കിൽ (*name_end == '(')
    {
	പൂർണ്ണസംഖ്യ	function_എണ്ണം;
	പൂർണ്ണസംഖ്യ	child_എണ്ണം;
	ufunc_T	**functions;

	എങ്കിൽ (type->tt_type == VAR_CLASS)
	{
	    function_എണ്ണം = cl->class_class_function_എണ്ണം;
	    child_എണ്ണം = cl->class_class_function_എണ്ണം_child;
	    functions = cl->class_class_functions;
	}
	അല്ലെങ്കിൽ
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    എങ്കിൽ (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_എണ്ണം = cl->class_obj_method_എണ്ണം;
	    child_എണ്ണം = cl->class_obj_method_എണ്ണം_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = ശൂന്യ;
	പൂർണ്ണസംഖ്യ fi;
	വേണ്ടി (fi = is_super ? child_എണ്ണം : 0; fi < function_എണ്ണം; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    എങ്കിൽ (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		നിർത്തുക;
	    }
	}

	ocmember_T  *ocm = ശൂന്യ;
	എങ്കിൽ (ufunc == ശൂന്യ)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    എങ്കിൽ (ocm == ശൂന്യ || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_സന്ദേശം(cl, type->tt_type, name, len);
		പോകുക done;
	    }
	    എങ്കിൽ (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_അട്ടി.ga_len;
		എങ്കിൽ (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    പോകുക done;
	    }
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ status;

		എങ്കിൽ (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		അല്ലെങ്കിൽ
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		എങ്കിൽ (status == FAIL)
		    പോകുക done;
	    }
	}

	എങ്കിൽ (is_super && ufunc != ശൂന്യ && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    പോകുക done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	എങ്കിൽ (ocm == ശൂന്യ && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    പോകുക done;
	}

	// process generic function call
	എങ്കിൽ (ufunc != ശൂന്യ)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    എങ്കിൽ (ufunc == ശൂന്യ)
		പോകുക done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	പൂർണ്ണസംഖ്യ argcount = 0;
	എങ്കിൽ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    പോകുക done;

	എങ്കിൽ (ocm != ശൂന്യ)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    പോകുക done;
	}
	എങ്കിൽ (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	അല്ലെങ്കിൽ
	    ret = generate_CALL(cctx, ufunc, ശൂന്യ, 0, argcount, FALSE);
	പോകുക done;
    }

    എങ്കിൽ (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	എങ്കിൽ (m_idx >= 0)
	{
	    എങ്കിൽ (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		പോകുക done;
	    }

	    *arg = name_end;
	    എങ്കിൽ (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    അല്ലെങ്കിൽ
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    പോകുക done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	എങ്കിൽ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    എങ്കിൽ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		പോകുക done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_അട്ടി.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, ശൂന്യ);
	    പോകുക done;
	}

	member_not_found_സന്ദേശം(cl, VAR_OBJECT, name, len);
    }
    അല്ലെങ്കിൽ
    {
	// load class member
	പൂർണ്ണസംഖ്യ idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	എങ്കിൽ (m != ശൂന്യ)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    എങ്കിൽ (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		പോകുക done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_അട്ടി.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    പോകുക done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	എങ്കിൽ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    എങ്കിൽ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		പോകുക done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_അട്ടി.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, ശൂന്യ);
	    പോകുക done;
	}

	member_not_found_സന്ദേശം(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    തിരികെ ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    പൂർണ്ണസംഖ്യ
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    പൂർണ്ണസംഖ്യ		    idx;
    imported_T	    *imp;

    എങ്കിൽ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	തിരികെ FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, ശൂന്യ);
    എങ്കിൽ (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	തിരികെ OK;
    }

    എങ്കിൽ (end == ശൂന്യ)
	imp = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ (import == ശൂന്യ)
	imp = find_imported(name, 0, FALSE);
    അല്ലെങ്കിൽ
	imp = import;

    എങ്കിൽ (imp != ശൂന്യ)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	പൂർണ്ണസംഖ്യ	cc;
	ufunc_T	*ufunc = ശൂന്യ;
	type_T	*type;
	പൂർണ്ണസംഖ്യ	done = FALSE;
	പൂർണ്ണസംഖ്യ	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	എങ്കിൽ (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    തിരികെ FAIL;
	}
	++p;
	എങ്കിൽ (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    തിരികെ FAIL;
	}

	// isolate one name
	exp_name = p;
	ആകുമ്പോൾ (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	എങ്കിൽ (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	എങ്കിൽ (res == OK)
	{
	    എങ്കിൽ (si->sn_autoload_prefix != ശൂന്യ
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		എങ്കിൽ (cc == '(' || paren_follows_after_വ്യഞ്ജകം)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		അല്ലെങ്കിൽ
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		എങ്കിൽ (cc == '(' || paren_follows_after_വ്യഞ്ജകം)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		അല്ലെങ്കിൽ
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, ശൂന്യ, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	എങ്കിൽ (done)
	    തിരികെ res;

	എങ്കിൽ (idx < 0)
	{
	    എങ്കിൽ (ufunc != ശൂന്യ)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		എങ്കിൽ (IS_GENERIC_FUNC(ufunc))
		{
		    എങ്കിൽ (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			എങ്കിൽ (p != ശൂന്യ)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			എങ്കിൽ (p == ശൂന്യ || ufunc == ശൂന്യ)
			    തിരികെ FAIL;
		    }
		    അല്ലെങ്കിൽ
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			തിരികെ FAIL;
		    }
		}
		അല്ലെങ്കിൽ എങ്കിൽ (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    തിരികെ FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, ശൂന്യ, TRUE);
		തിരികെ OK;
	    }
	    തിരികെ FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	തിരികെ OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    തിരികെ generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    പൂർണ്ണസംഖ്യ			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    എങ്കിൽ (ufunc == ശൂന്യ || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	തിരികെ FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    എങ്കിൽ (ufunc == ശൂന്യ)
	തിരികെ FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    എങ്കിൽ (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, ശൂന്യ) == FAIL)
	തിരികെ FAIL;
    തിരികെ generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    തിരികെ cctx->ctx_ufunc != ശൂന്യ
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == ശൂന്യ
		|| cctx->ctx_outer->ctx_ufunc == ശൂന്യ
		|| cctx->ctx_outer->ctx_ufunc->uf_class == ശൂന്യ
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    പൂർണ്ണസംഖ്യ			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    പൂർണ്ണസംഖ്യ		isn_idx;

    എങ്കിൽ (fp == ശൂന്യ)
	തിരികെ FAIL;

    വേണ്ടി (പൂർണ്ണസംഖ്യ depth = 0; depth < outer_depth; ++depth)
    {
	എങ്കിൽ (outer_cctx->ctx_outer == ശൂന്യ)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    തിരികെ FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    എങ്കിൽ (fp->uf_block_ids == ശൂന്യ && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(പൂർണ്ണസംഖ്യ, ufunc->uf_block_depth);
	എങ്കിൽ (fp->uf_block_ids != ശൂന്യ)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    വലുപ്പം(പൂർണ്ണസംഖ്യ) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    എങ്കിൽ (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (generate_FUNCREF(cctx, fp, ശൂന്യ, FALSE, 0, &isn_idx) == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	തിരികെ OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    എങ്കിൽ (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	എങ്കിൽ (extra == ശൂന്യ)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    എങ്കിൽ (extra == ശൂന്യ)
		തിരികെ FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    തിരികെ OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    പൂർണ്ണസംഖ്യ
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	പൂർണ്ണസംഖ്യ	is_വ്യഞ്ജകം,
	പൂർണ്ണസംഖ്യ	error)
{
    type_T	*type;
    char_u	*name = ശൂന്യ;
    char_u	*end = end_arg;
    പൂർണ്ണസംഖ്യ		res = FAIL;
    പൂർണ്ണസംഖ്യ		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = ശൂന്യ;

    generic_func_args_table_init(&gfatab);

    എങ്കിൽ (*(*arg + namelen) == '<')
    {
	// generic function call
	എങ്കിൽ (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == ശൂന്യ)
	{
	    generic_func_args_table_clear(&gfatab);
	    തിരികെ FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    എങ്കിൽ (*(*arg + 1) == ':')
    {
	എങ്കിൽ (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    തിരഞ്ഞെടുപ്പ് (**arg)
	    {
		അവസ്ഥ 'g': isn_type = ISN_LOADGDICT; നിർത്തുക;
		അവസ്ഥ 'w': isn_type = ISN_LOADWDICT; നിർത്തുക;
		അവസ്ഥ 't': isn_type = ISN_LOADTDICT; നിർത്തുക;
		അവസ്ഥ 'b': isn_type = ISN_LOADBDICT; നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    പോകുക theend;
	    }
	    എങ്കിൽ (generate_instr_type(cctx, isn_type, &t_dict_any) == ശൂന്യ)
		പോകുക theend;
	    res = OK;
	}
	അല്ലെങ്കിൽ
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    എങ്കിൽ (name == ശൂന്യ)
		പോകുക theend;

	    തിരഞ്ഞെടുപ്പ് (**arg)
	    {
		അവസ്ഥ 'v': res = generate_LOADV(cctx, name);
			  നിർത്തുക;
		അവസ്ഥ 's': എങ്കിൽ (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      പോകുക theend;
			  }
			  എങ്കിൽ (is_വ്യഞ്ജകം && find_func(name, FALSE) != ശൂന്യ)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  അല്ലെങ്കിൽ
			      res = compile_load_scriptvar(cctx, name,
							    ശൂന്യ, &end, ശൂന്യ);
			  നിർത്തുക;
		അവസ്ഥ 'g': എങ്കിൽ (vim_strchr(name, AUTOLOAD_CHAR) == ശൂന്യ)
			  {
			      എങ്കിൽ (is_വ്യഞ്ജകം && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != ശൂന്യ
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      അല്ലെങ്കിൽ
				  isn_type = ISN_LOADG;
			  }
			  അല്ലെങ്കിൽ
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      എങ്കിൽ (name == ശൂന്യ)
				  പോകുക theend;
			  }
			  നിർത്തുക;
		അവസ്ഥ 'w': isn_type = ISN_LOADW; നിർത്തുക;
		അവസ്ഥ 't': isn_type = ISN_LOADT; നിർത്തുക;
		അവസ്ഥ 'b': isn_type = ISN_LOADB; നിർത്തുക;
		സ്ഥിരസ്ഥിതി:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  പോകുക theend;
	    }
	    എങ്കിൽ (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	പൂർണ്ണസംഖ്യ	    idx;
	പൂർണ്ണസംഖ്യ	    method_idx;
	പൂർണ്ണസംഖ്യ	    gen_load = FALSE;
	പൂർണ്ണസംഖ്യ	    gen_load_outer = 0;
	പൂർണ്ണസംഖ്യ	    outer_loop_depth = -1;
	പൂർണ്ണസംഖ്യ	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	എങ്കിൽ (name == ശൂന്യ)
	    പോകുക theend;

	എങ്കിൽ (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = തള്ളുക_type_അട്ടി(cctx, &t_super);
	    എങ്കിൽ (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr(name, AUTOLOAD_CHAR) != ശൂന്യ)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    എങ്കിൽ (gen_load_outer == 0)
		gen_load = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    lvar_T  lvar;
	    class_T *cl = ശൂന്യ;

	    എങ്കിൽ (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		എങ്കിൽ (lvar.lv_ufunc != ശൂന്യ && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		അല്ലെങ്കിൽ എങ്കിൽ (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		അല്ലെങ്കിൽ
		    gen_load = TRUE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (cctx->ctx_ufunc->uf_defclass != ശൂന്യ &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		എങ്കിൽ (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    എങ്കിൽ (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    അല്ലെങ്കിൽ
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									ശൂന്യ);
		    }
		}
		അല്ലെങ്കിൽ
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		imported_T *imp = ശൂന്യ;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		എങ്കിൽ (script_var_exists(*arg, namelen, cctx, ശൂന്യ) == OK
			    || (imp = find_imported(name, 0, FALSE)) != ശൂന്യ)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		എങ്കിൽ (res == FAIL && is_വ്യഞ്ജകം && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	എങ്കിൽ (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, ശൂന്യ, type);
	എങ്കിൽ (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    എങ്കിൽ (lt != ശൂന്യ)
	*lt = '<';
    എങ്കിൽ (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    തിരികെ res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_string(isn_T *isn, cctx_T *cctx, പൂർണ്ണസംഖ്യ str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    പൂർണ്ണസംഖ്യ		വ്യഞ്ജകം_res;
    പൂർണ്ണസംഖ്യ		trailing_error;
    പൂർണ്ണസംഖ്യ		instr_എണ്ണം;
    isn_T	*instr = ശൂന്യ;

    // Remove the string type from the stack.
    --cctx->ctx_type_അട്ടി.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = ശൂന്യ;

    // avoid peeking a next line
    പൂർണ്ണസംഖ്യ galen_save = cctx->ctx_ufunc->uf_വരികൾ.ga_len;
    cctx->ctx_ufunc->uf_വരികൾ.ga_len = 0;

    വ്യഞ്ജകം_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_വരികൾ.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    എങ്കിൽ (വ്യഞ്ജകം_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	എങ്കിൽ (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_അട്ടി.ga_len;
	തിരികെ FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_എണ്ണം = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_എണ്ണം].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    തിരികെ OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    പൂർണ്ണസംഖ്യ
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	പൂർണ്ണസംഖ്യ	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    പൂർണ്ണസംഖ്യ	    must_end = FALSE;
    പൂർണ്ണസംഖ്യ	    instr_എണ്ണം;

    വേണ്ടി (;;)
    {
	എങ്കിൽ (may_get_next_വരി(whitep, &p, cctx) == FAIL)
	    പോകുക failret;
	എങ്കിൽ (*p == ')')
	{
	    *arg = p + 1;
	    തിരികെ OK;
	}
	എങ്കിൽ (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    തിരികെ FAIL;
	}

	instr_എണ്ണം = cctx->ctx_instr.ga_len;
	എങ്കിൽ (compile_expr0(&p, cctx) == FAIL)
	    തിരികെ FAIL;
	++*argcount;

	എങ്കിൽ (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_എണ്ണം + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_എണ്ണം;

	    // {skip} argument of searchpair() can be compiled if not empty
	    എങ്കിൽ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ശൂന്യ
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_എണ്ണം + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_എണ്ണം;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    എങ്കിൽ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ശൂന്യ
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	എങ്കിൽ (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (*p == ',')
	{
	    ++p;
	    എങ്കിൽ (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    തിരികെ FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_അട്ടി(cctx, 0);
    പൂർണ്ണസംഖ്യ		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    എങ്കിൽ (type->tt_type == VAR_OBJECT)
    {
	പൂർണ്ണസംഖ്യ	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	എങ്കിൽ (uf != ശൂന്യ)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    തിരികെ res;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	പൂർണ്ണസംഖ്യ	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ശൂന്യ;
    char_u	*gname = ശൂന്യ;
    ufunc_T	*ufunc = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		res = FAIL;
    പൂർണ്ണസംഖ്യ		is_autoload;
    പൂർണ്ണസംഖ്യ		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    എങ്കിൽ (varlen >= വലുപ്പം(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	തിരികെ FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    എങ്കിൽ (import != ശൂന്യ)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	തിരികെ FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    എങ്കിൽ ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	പൂർണ്ണസംഖ്യ	    is_has = **arg == 'h';
	പൂർണ്ണസംഖ്യ	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	എങ്കിൽ (*s == '"')
	    (ശൂന്യം)eval_string(&s, &argvars[0], TRUE, FALSE);
	അല്ലെങ്കിൽ എങ്കിൽ (*s == '\'')
	    (ശൂന്യം)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	എങ്കിൽ (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_സംഖ്യ;
		tv->vval.v_സംഖ്യ = 0;
		എങ്കിൽ (is_has)
		    f_has(argvars, tv);
		അല്ലെങ്കിൽ എങ്കിൽ (is_len)
		    f_len(argvars, tv);
		അല്ലെങ്കിൽ
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    തിരികെ OK;
	}
	clear_tv(&argvars[0]);
	എങ്കിൽ (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    തിരികെ FAIL;
	}
    }

    എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
	തിരികെ FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    എങ്കിൽ ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    അല്ലെങ്കിൽ എങ്കിൽ (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    അല്ലെങ്കിൽ
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    എങ്കിൽ (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	എങ്കിൽ (*arg == ശൂന്യ)
	    പോകുക theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	എങ്കിൽ (gname == ശൂന്യ)
	    പോകുക theend;
	++*arg;			// skip '('
    }
    അല്ലെങ്കിൽ
	*arg += varlen + 1;

    എങ്കിൽ (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	പോകുക theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != ശൂന്യ;
    എങ്കിൽ (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	പൂർണ്ണസംഖ്യ	    idx;

	// builtin function
	idx = find_internal_func(name);
	എങ്കിൽ (idx >= 0)
	{
	    എങ്കിൽ (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		പോകുക theend;
	    }

	    എങ്കിൽ (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_അട്ടി(cctx, 1);
		എങ്കിൽ (check_type_is_മൂല്യം(get_type_on_അട്ടി(cctx, 0)) == FAIL)
		    പോകുക theend;

		// add() can be compiled to instructions if we know the type
		എങ്കിൽ (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    എങ്കിൽ ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		എങ്കിൽ (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    എങ്കിൽ (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    എങ്കിൽ (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		എങ്കിൽ (res == OK)
		    idx = -1;
	    }

	    എങ്കിൽ (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	അല്ലെങ്കിൽ
	    emsg_funcname(e_unknown_function_str, namebuf);
	പോകുക theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    എങ്കിൽ (lookup_local(namebuf, varlen, ശൂന്യ, cctx) == FAIL
	    && arg_exists(namebuf, varlen, ശൂന്യ, ശൂന്യ, ശൂന്യ, cctx) != OK)
    {
	class_T		*cl = ശൂന്യ;
	പൂർണ്ണസംഖ്യ		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	എങ്കിൽ (ufunc != ശൂന്യ)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    എങ്കിൽ (ufunc == ശൂന്യ)
		പോകുക theend;

	    എങ്കിൽ (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, ശൂന്യ, 0, argcount, FALSE);
		പോകുക theend;
	    }
	    എങ്കിൽ (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ശൂന്യ
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		പോകുക theend;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    എങ്കിൽ (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], ശൂന്യ,
							0, argcount, FALSE);
	    }
	    അല്ലെങ്കിൽ
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    പോകുക theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == ശൂന്യ && gname != ശൂന്യ ? gname : namebuf;
    എങ്കിൽ (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_അട്ടി(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	പോകുക theend;
    }

    // If we can find a global function by name generate the right call.
    എങ്കിൽ (ufunc != ശൂന്യ)
    {
	res = generate_CALL(cctx, ufunc, ശൂന്യ, 0, argcount, FALSE);
	പോകുക theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    എങ്കിൽ (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    അല്ലെങ്കിൽ
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    തിരികെ res;
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
to_name_end(char_u *arg, പൂർണ്ണസംഖ്യ use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    എങ്കിൽ (!eval_isnamec1(*arg))
	തിരികെ arg;

    വേണ്ടി (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	എങ്കിൽ (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == ശൂന്യ))
	    നിർത്തുക;
    തിരികെ p;
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

    എങ്കിൽ (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    എങ്കിൽ (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	എങ്കിൽ (eval_list(&p, &rettv, ശൂന്യ, FALSE) == FAIL)
	    p = arg;
    }
    തിരികെ p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		is_const;
    പൂർണ്ണസംഖ്യ		is_all_const = TRUE;	// reset when non-const encountered
    പൂർണ്ണസംഖ്യ		must_end = FALSE;

    വേണ്ടി (;;)
    {
	എങ്കിൽ (may_get_next_വരി(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (*p == ']')
	{
	    ++p;
	    നിർത്തുക;
	}
	എങ്കിൽ (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (!is_const)
	    is_all_const = FALSE;
	++എണ്ണം;
	എങ്കിൽ (*p == ',')
	{
	    ++p;
	    എങ്കിൽ (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    തിരികെ generate_NEWLIST(cctx, എണ്ണം, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    പൂർണ്ണസംഖ്യ first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		is_const;
    പൂർണ്ണസംഖ്യ		is_all_const = TRUE;	// reset when non-const encountered
    പൂർണ്ണസംഖ്യ		must_end = FALSE;

    എങ്കിൽ (**arg != ')')
    {
	എങ്കിൽ (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    തിരികെ FAIL;
	}
	എണ്ണം = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	വേണ്ടി (;;)
	{
	    എങ്കിൽ (may_get_next_വരി(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (*p == ')')
	    {
		++p;
		നിർത്തുക;
	    }
	    എങ്കിൽ (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (!is_const)
		is_all_const = FALSE;
	    ++എണ്ണം;
	    എങ്കിൽ (*p == ',')
	    {
		++p;
		എങ്കിൽ (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    തിരികെ FAIL;
		}
	    }
	    അല്ലെങ്കിൽ
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    തിരികെ generate_NEWTUPLE(cctx, എണ്ണം, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    സ്ഥിതം ശൂന്യം
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*വരി;
    size_t	off;

    എങ്കിൽ (!evalarg->eval_using_cmdline || cctx == ശൂന്യ)
	തിരികെ;

    gap = &evalarg->eval_tofree_ga;
    എങ്കിൽ (gap->ga_len == 0)
	തിരികെ;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    വരി = ((char_u **)cctx->ctx_ufunc->uf_വരികൾ.ga_data)[cctx->ctx_lnum];
    *arg = വരി + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    പൂർണ്ണസംഖ്യ
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    പൂർണ്ണസംഖ്യ		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    എങ്കിൽ (r != OK)
    {
	clear_evalarg(&evalarg, ശൂന്യ);
	തിരികെ r;
    }

    എങ്കിൽ (ppconst != ശൂന്യ && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, ശൂന്യ);
	തിരികെ FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    എങ്കിൽ (cctx->ctx_ufunc != ശൂന്യ)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	പൂർണ്ണസംഖ്യ block_depth = cctx->ctx_ufunc->uf_block_depth;

	എങ്കിൽ (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(പൂർണ്ണസംഖ്യ, block_depth);
	    എങ്കിൽ (ufunc->uf_block_ids != ശൂന്യ)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    വലുപ്പം(പൂർണ്ണസംഖ്യ) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    എങ്കിൽ (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    എങ്കിൽ (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    എങ്കിൽ (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	എങ്കിൽ (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, ശൂന്യ);

    എങ്കിൽ (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	തിരികെ generate_FUNCREF(cctx, ufunc, ശൂന്യ, FALSE, 0, ശൂന്യ);
    }

    func_ptr_unref(ufunc);
    തിരികെ FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    പൂർണ്ണസംഖ്യ
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	പൂർണ്ണസംഖ്യ	    types_optional,
	evalarg_T   *evalarg)
{
    പൂർണ്ണസംഖ്യ		r;
    ufunc_T	*ufunc;
    പൂർണ്ണസംഖ്യ		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, ശൂന്യ);
    current_sctx.sc_version = save_sc_version;
    എങ്കിൽ (r != OK)
	തിരികെ r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    എങ്കിൽ (ufunc->uf_ret_type == ശൂന്യ || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, ശൂന്യ);

    എങ്കിൽ (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	തിരികെ OK;
    }
    clear_tv(rettv);
    തിരികെ FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		is_const;
    പൂർണ്ണസംഖ്യ		is_all_const = TRUE;	// reset when non-const encountered

    എങ്കിൽ (d == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	തിരികെ FAIL;
    }
    വേണ്ടി (;;)
    {
	char_u	    *താക്കോൽ = ശൂന്യ;

	എങ്കിൽ (may_get_next_വരി(whitep, arg, cctx) == FAIL)
	{
	    *arg = ശൂന്യ;
	    പോകുക failret;
	}

	എങ്കിൽ (**arg == '}')
	    നിർത്തുക;

	എങ്കിൽ (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    എങ്കിൽ (compile_expr0(arg, cctx) == FAIL)
		പോകുക failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    എങ്കിൽ (isn->isn_type == ISN_PUSHNR)
	    {
		അക്ഷരം buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.സംഖ്യ);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    എങ്കിൽ (isn->isn_type == ISN_PUSHS)
		താക്കോൽ = isn->isn_arg.string;
	    അല്ലെങ്കിൽ എങ്കിൽ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		പോകുക failret;
	    *arg = skipwhite(*arg);
	    എങ്കിൽ (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_താക്കോൽ));
		പോകുക failret;
	    }
	    ++*arg;
	}
	അല്ലെങ്കിൽ
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    താക്കോൽ = get_literal_താക്കോൽ(arg);
	    എങ്കിൽ (താക്കോൽ == ശൂന്യ)
		പോകുക failret;
	    എങ്കിൽ (generate_PUSHS(cctx, &താക്കോൽ) == FAIL)
		പോകുക failret;
	}

	// Check for duplicate keys, if using string keys.
	എങ്കിൽ (താക്കോൽ != ശൂന്യ)
	{
	    item = dict_find(d, താക്കോൽ, -1);
	    എങ്കിൽ (item != ശൂന്യ)
	    {
		semsg(_(e_duplicate_താക്കോൽ_in_dictionary_str), താക്കോൽ);
		പോകുക failret;
	    }
	    item = dictitem_alloc(താക്കോൽ);
	    എങ്കിൽ (item != ശൂന്യ)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		എങ്കിൽ (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	എങ്കിൽ (**arg != ':')
	{
	    എങ്കിൽ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    അല്ലെങ്കിൽ
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    പോകുക failret;
	}
	whitep = *arg + 1;
	എങ്കിൽ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    പോകുക failret;
	}

	എങ്കിൽ (may_get_next_വരി(whitep, arg, cctx) == FAIL)
	{
	    *arg = ശൂന്യ;
	    പോകുക failret;
	}

	എങ്കിൽ (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    പോകുക failret;
	എങ്കിൽ (!is_const)
	    is_all_const = FALSE;
	++എണ്ണം;

	whitep = *arg;
	എങ്കിൽ (may_get_next_വരി(whitep, arg, cctx) == FAIL)
	{
	    *arg = ശൂന്യ;
	    പോകുക failret;
	}
	എങ്കിൽ (**arg == '}')
	    നിർത്തുക;
	എങ്കിൽ (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    പോകുക failret;
	}
	എങ്കിൽ (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    പോകുക failret;
	}
	whitep = *arg + 1;
	എങ്കിൽ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    പോകുക failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    എങ്കിൽ (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    തിരികെ generate_NEWDICT(cctx, എണ്ണം, FALSE);

failret:
    എങ്കിൽ (*arg == ശൂന്യ)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    തിരികെ FAIL;
}

/*
 * Compile "&option".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    പൂർണ്ണസംഖ്യ		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    എങ്കിൽ (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_സംഖ്യ ? &t_സംഖ്യ : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    തിരികെ ret;
}

/*
 * Compile "$VAR".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    എങ്കിൽ (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	തിരികെ FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    തിരികെ ret;
}

/*
 * Compile $"string" or $'string'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    പൂർണ്ണസംഖ്യ		ret;
    പൂർണ്ണസംഖ്യ		quote;
    പൂർണ്ണസംഖ്യ		evaluate = cctx->ctx_skip != SKIP_YES;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    വേണ്ടി (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	എങ്കിൽ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	അല്ലെങ്കിൽ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	എങ്കിൽ (ret == FAIL)
	    നിർത്തുക;
	എങ്കിൽ (evaluate)
	{
	    എങ്കിൽ ((tv.vval.v_string != ശൂന്യ && *tv.vval.v_string != NUL)
		    || (**arg != '{' && എണ്ണം == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		എങ്കിൽ (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    തിരികെ FAIL;
		tv.vval.v_string = ശൂന്യ;  // don't free it now
		++എണ്ണം;
	    }
	    clear_tv(&tv);
	}

	എങ്കിൽ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    നിർത്തുക;
	}

	p = compile_one_വ്യഞ്ജകം_in_str(*arg, cctx);
	എങ്കിൽ (p == ശൂന്യ)
	{
	    ret = FAIL;
	    നിർത്തുക;
	}
	++എണ്ണം;
	*arg = p;
    }

    എങ്കിൽ (ret == FAIL || !evaluate)
	തിരികെ ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    എങ്കിൽ (എണ്ണം > 1)
	തിരികെ generate_CONCAT(cctx, എണ്ണം);

    തിരികെ OK;
}

/*
 * Compile "@r".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_get_register(char_u **arg, cctx_T *cctx)
{
    പൂർണ്ണസംഖ്യ		ret;

    ++*arg;
    എങ്കിൽ (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	തിരികെ FAIL;
    }
    എങ്കിൽ (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	തിരികെ FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, ശൂന്യ, &t_string);
    ++*arg;
    തിരികെ ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
apply_leader(typval_T *rettv, പൂർണ്ണസംഖ്യ numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    ആകുമ്പോൾ (p > start)
    {
	--p;
	എങ്കിൽ (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    എങ്കിൽ (rettv->v_type == VAR_FLOAT)
	    {
		എങ്കിൽ (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    അല്ലെങ്കിൽ
	    {
		varnumber_T	val;
		പൂർണ്ണസംഖ്യ		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		എങ്കിൽ (check_not_string(rettv) == FAIL)
		    തിരികെ FAIL;
		val = tv_get_സംഖ്യ_chk(rettv, &error);
		clear_tv(rettv);
		എങ്കിൽ (error)
		    തിരികെ FAIL;
		എങ്കിൽ (*p == '-')
		    val = -val;
		rettv->v_type = VAR_സംഖ്യ;
		rettv->vval.v_സംഖ്യ = val;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (numeric_only)
	{
	    ++p;
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '!')
	{
	    പൂർണ്ണസംഖ്യ v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_സംഖ്യ = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    തിരികെ OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    സ്ഥിതം ശൂന്യം
get_vim_constant(char_u **arg, typval_T *rettv)
{
    എങ്കിൽ (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_സംഖ്യ = VVAL_TRUE;
	*arg += 6;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_സംഖ്യ = VVAL_FALSE;
	*arg += 7;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_സംഖ്യ = VVAL_NULL;
	*arg += 6;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_സംഖ്യ = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, പൂർണ്ണസംഖ്യ *len, പൂർണ്ണസംഖ്യ *type_is)
{
    exprtype_T	type = വ്യഞ്ജകം_UNKNOWN;
    പൂർണ്ണസംഖ്യ		i;

    തിരഞ്ഞെടുപ്പ് (p[0])
    {
	അവസ്ഥ '=':   എങ്കിൽ (p[1] == '=')
			type = വ്യഞ്ജകം_EQUAL;
		    അല്ലെങ്കിൽ എങ്കിൽ (p[1] == '~')
			type = വ്യഞ്ജകം_MATCH;
		    നിർത്തുക;
	അവസ്ഥ '!':   എങ്കിൽ (p[1] == '=')
			type = വ്യഞ്ജകം_NEQUAL;
		    അല്ലെങ്കിൽ എങ്കിൽ (p[1] == '~')
			type = വ്യഞ്ജകം_NOMATCH;
		    നിർത്തുക;
	അവസ്ഥ '>':   എങ്കിൽ (p[1] != '=')
		    {
			type = വ്യഞ്ജകം_GREATER;
			*len = 1;
		    }
		    അല്ലെങ്കിൽ
			type = വ്യഞ്ജകം_GEQUAL;
		    നിർത്തുക;
	അവസ്ഥ '<':   എങ്കിൽ (p[1] != '=')
		    {
			type = വ്യഞ്ജകം_SMALLER;
			*len = 1;
		    }
		    അല്ലെങ്കിൽ
			type = വ്യഞ്ജകം_SEQUAL;
		    നിർത്തുക;
	അവസ്ഥ 'i':   എങ്കിൽ (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			എങ്കിൽ (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			എങ്കിൽ (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? വ്യഞ്ജകം_IS : വ്യഞ്ജകം_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    നിർത്തുക;
    }
    തിരികെ type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    ശൂന്യം
skip_വ്യഞ്ജകം_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_വ്യഞ്ജകം(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, ശൂന്യ);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    പൂർണ്ണസംഖ്യ
bool_on_അട്ടി(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_അട്ടി(cctx, 0);
    എങ്കിൽ (type == &t_bool)
	തിരികെ OK;

    എങ്കിൽ (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_സംഖ്യ
	    || type == &t_സംഖ്യ_bool
	    || type == &t_const_സംഖ്യ_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	തിരികെ generate_COND2BOOL(cctx);

    തിരികെ need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    ശൂന്യം
error_white_both(char_u *op, പൂർണ്ണസംഖ്യ len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_leader(cctx_T *cctx, പൂർണ്ണസംഖ്യ numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    ആകുമ്പോൾ (p > start)
    {
	--p;
	ആകുമ്പോൾ (VIM_ISWHITE(*p))
	    --p;
	എങ്കിൽ (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_അട്ടി(cctx, 0);
	    എങ്കിൽ (type->tt_type != VAR_FLOAT && need_type(type, &t_സംഖ്യ,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		തിരികെ FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    എങ്കിൽ (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == ശൂന്യ)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (numeric_only)
	{
	    ++p;
	    നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ  invert = *p == '!';

	    ആകുമ്പോൾ (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		എങ്കിൽ (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    എങ്കിൽ (check_type_is_മൂല്യം(get_type_on_അട്ടി(cctx, 0)) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (generate_2BOOL(cctx, invert, -1) == FAIL)
		തിരികെ FAIL;
	}
    }
    *end = p;
    തിരികെ OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    പൂർണ്ണസംഖ്യ	    ret;
    char_u  *p = *arg + 1;

    എങ്കിൽ (may_get_next_വരി_error(p, arg, cctx) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (**arg == ')')
	// empty tuple
	തിരികെ compile_tuple(arg, cctx, ppconst, FALSE);

    എങ്കിൽ (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    അല്ലെങ്കിൽ
    {
	// Not enough space in ppconst, flush constants.
	എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
	    തിരികെ FAIL;
	ret = compile_expr0(arg, cctx);
    }
    എങ്കിൽ (may_get_next_വരി_error(*arg, arg, cctx) == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ (ret == OK && **arg == ',')
    {
	// tuple
	പൂർണ്ണസംഖ്യ is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
	    തിരികെ FAIL;
	തിരികെ compile_tuple(arg, cctx, ppconst, is_const);
    }

    എങ്കിൽ (**arg == ')')
	++*arg;
    അല്ലെങ്കിൽ എങ്കിൽ (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    തിരികെ ret;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    പൂർണ്ണസംഖ്യ		keeping_dict = FALSE;

    വേണ്ടി (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	എങ്കിൽ (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_വരി_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    എങ്കിൽ (next != ശൂന്യ &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_വരി_from_context(cctx, TRUE);
		എങ്കിൽ (next == ശൂന്യ)
		    തിരികെ FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	എങ്കിൽ (**arg == '(')
	{
	    പൂർണ്ണസംഖ്യ		argcount = 0;

	    എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
		തിരികെ FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_അട്ടി(cctx, 0);

	    *arg = skipwhite(p + 1);
	    എങ്കിൽ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (keeping_dict)
	    {
		keeping_dict = FALSE;
		എങ്കിൽ (generate_instr(cctx, ISN_CLEARDICT) == ശൂന്യ)
		    തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    പൂർണ്ണസംഖ്യ	    alt;
	    char_u  *paren;

	    // something->method()
	    എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
		തിരികെ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_അട്ടി(cctx, 0);
	    എങ്കിൽ (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_മൂല്യം));
		തിരികെ FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    എങ്കിൽ (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		തിരികെ FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    എങ്കിൽ (**arg == '(')
		alt = 2;
	    അല്ലെങ്കിൽ
	    {
		// alternative 1 or 3
		p = *arg;
		എങ്കിൽ (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    തിരികെ FAIL;
		}
		എങ്കിൽ (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		വേണ്ടി ( ; eval_isnamec(*p); ++p)
		    ;
		എങ്കിൽ (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    എങ്കിൽ (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    എങ്കിൽ (paren == ശൂന്യ)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			തിരികെ FAIL;
		    }
		}
	    }

	    എങ്കിൽ (alt != 1)
	    {
		പൂർണ്ണസംഖ്യ	    argcount = 1;
		garray_T    *അട്ടി = &cctx->ctx_type_അട്ടി;
		പൂർണ്ണസംഖ്യ	    type_idx_start = അട്ടി->ga_len;
		പൂർണ്ണസംഖ്യ	    വ്യഞ്ജകം_isn_start = cctx->ctx_instr.ga_len;
		പൂർണ്ണസംഖ്യ	    വ്യഞ്ജകം_isn_end;
		പൂർണ്ണസംഖ്യ	    arg_isn_എണ്ണം;

		എങ്കിൽ (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    എങ്കിൽ (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			തിരികെ FAIL;
		}
		അല്ലെങ്കിൽ
		{
		    പൂർണ്ണസംഖ്യ fail;
		    പൂർണ്ണസംഖ്യ save_len = cctx->ctx_ufunc->uf_വരികൾ.ga_len;
		    പൂർണ്ണസംഖ്യ	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_വ്യഞ്ജകം;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_വരികൾ.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_വ്യഞ്ജകം;
		    cctx->ctx_ufunc->uf_വരികൾ.ga_len = save_len;

		    എങ്കിൽ (fail)
		    {
			എങ്കിൽ (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			തിരികെ FAIL;
		    }
		}

		// Compile the arguments.
		എങ്കിൽ (**arg != '(')
		{
		    എങ്കിൽ (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    അല്ലെങ്കിൽ
			semsg(_(e_missing_parenthesis_str), *arg);
		    തിരികെ FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		വ്യഞ്ജകം_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		എങ്കിൽ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    തിരികെ FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_എണ്ണം = cctx->ctx_instr.ga_len - വ്യഞ്ജകം_isn_end;
		എങ്കിൽ (arg_isn_എണ്ണം > 0)
		{
		    പൂർണ്ണസംഖ്യ	    വ്യഞ്ജകം_isn_എണ്ണം = വ്യഞ്ജകം_isn_end - വ്യഞ്ജകം_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, വ്യഞ്ജകം_isn_എണ്ണം);
		    type_T  *decl_type;
		    type2_T  *typep;

		    എങ്കിൽ (isn == ശൂന്യ)
			തിരികെ FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + വ്യഞ്ജകം_isn_start,
					       വലുപ്പം(isn_T) * വ്യഞ്ജകം_isn_എണ്ണം);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + വ്യഞ്ജകം_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + വ്യഞ്ജകം_isn_end,
						വലുപ്പം(isn_T) * arg_isn_എണ്ണം);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + വ്യഞ്ജകം_isn_start + arg_isn_എണ്ണം,
					  isn, വലുപ്പം(isn_T) * വ്യഞ്ജകം_isn_എണ്ണം);
		    vim_free(isn);

		    typep = ((type2_T *)അട്ടി->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)അട്ടി->ga_data) + type_idx_start,
			      ((type2_T *)അട്ടി->ga_data) + type_idx_start + 1,
			      വലുപ്പം(type2_T)
				       * (അട്ടി->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)അട്ടി->ga_data) + അട്ടി->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_അട്ടി(cctx, 0);
		എങ്കിൽ (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    തിരികെ FAIL;
	    }

	    എങ്കിൽ (keeping_dict)
	    {
		keeping_dict = FALSE;
		എങ്കിൽ (generate_instr(cctx, ISN_CLEARDICT) == ശൂന്യ)
		    തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (**arg == '[')
	{
	    പൂർണ്ണസംഖ്യ		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
		തിരികെ FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    എങ്കിൽ (may_get_next_വരി_error(p, arg, cctx) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (compile_expr0(arg, cctx) == FAIL)
		    തിരികെ FAIL;
		എങ്കിൽ (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    തിരികെ FAIL;
		}
		എങ്കിൽ (may_get_next_വരി_error(*arg, arg, cctx) == FAIL)
		    തിരികെ FAIL;
		*arg = skipwhite(*arg);
	    }
	    എങ്കിൽ (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		എങ്കിൽ (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    തിരികെ FAIL;
		}
		എങ്കിൽ (may_get_next_വരി_error(*arg, arg, cctx) == FAIL)
		    തിരികെ FAIL;
		എങ്കിൽ (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (compile_expr0(arg, cctx) == FAIL)
			തിരികെ FAIL;
		    എങ്കിൽ (may_get_next_വരി_error(*arg, arg, cctx) == FAIL)
			തിരികെ FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    എങ്കിൽ (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		തിരികെ FAIL;
	    }
	    *arg = *arg + 1;

	    എങ്കിൽ (keeping_dict)
	    {
		keeping_dict = FALSE;
		എങ്കിൽ (generate_instr(cctx, ISN_CLEARDICT) == ശൂന്യ)
		    തിരികെ FAIL;
	    }
	    എങ്കിൽ (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
		തിരികെ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_അട്ടി(cctx, 0);
	    എങ്കിൽ (type != &t_unknown
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
		എങ്കിൽ (compile_class_object_index(cctx, arg, type) == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		*arg = p + 1;
		എങ്കിൽ (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    തിരികെ FAIL;
		}
		p = *arg;
		ആകുമ്പോൾ (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		എങ്കിൽ (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    തിരികെ FAIL;
		}
		എങ്കിൽ (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == ശൂന്യ)
		    തിരികെ FAIL;
		എങ്കിൽ (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    തിരികെ FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	അല്ലെങ്കിൽ
	    നിർത്തുക;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    എങ്കിൽ (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == ശൂന്യ)
	തിരികെ FAIL;

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    പൂർണ്ണസംഖ്യ		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    പൂർണ്ണസംഖ്യ		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    എങ്കിൽ (eval_leader(arg, TRUE) == FAIL)
	തിരികെ FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    തിരഞ്ഞെടുപ്പ് (**arg)
    {
	/*
	 * Number constant.
	 */
	അവസ്ഥ '0':	// also for blob starting with 0z
	അവസ്ഥ '1':
	അവസ്ഥ '2':
	അവസ്ഥ '3':
	അവസ്ഥ '4':
	അവസ്ഥ '5':
	അവസ്ഥ '6':
	അവസ്ഥ '7':
	അവസ്ഥ '8':
	അവസ്ഥ '9':
	അവസ്ഥ '.':   എങ്കിൽ (eval_സംഖ്യ(arg, rettv, TRUE, FALSE) == FAIL)
			തിരികെ FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    എങ്കിൽ (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			തിരികെ FAIL;
		    }
		    നിർത്തുക;

	/*
	 * String constant: "string".
	 */
	അവസ്ഥ '"':   എങ്കിൽ (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			തിരികെ FAIL;
		    നിർത്തുക;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	അവസ്ഥ '\'':  എങ്കിൽ (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			തിരികെ FAIL;
		    നിർത്തുക;

	/*
	 * Constant Vim variable.
	 */
	അവസ്ഥ 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    നിർത്തുക;

	/*
	 * "true" constant
	 */
	അവസ്ഥ 't':   എങ്കിൽ (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_സംഖ്യ = VVAL_TRUE;
		    }
		    അല്ലെങ്കിൽ
			ret = NOTDONE;
		    നിർത്തുക;

	/*
	 * "false" constant
	 */
	അവസ്ഥ 'f':   എങ്കിൽ (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_സംഖ്യ = VVAL_FALSE;
		    }
		    അല്ലെങ്കിൽ
			ret = NOTDONE;
		    നിർത്തുക;

	/*
	 * "null" or "null_*" constant
	 */
	അവസ്ഥ 'n':   എങ്കിൽ (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			പൂർണ്ണസംഖ്യ	len;

			വേണ്ടി (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			എങ്കിൽ (ret == FAIL)
			    ret = NOTDONE;
			അല്ലെങ്കിൽ
			    *arg += len + 4;
		    }
		    അല്ലെങ്കിൽ
			ret = NOTDONE;
		    നിർത്തുക;

	/*
	 * List: [expr, expr]
	 */
	അവസ്ഥ '[':   എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
			തിരികെ FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    നിർത്തുക;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	അവസ്ഥ '{':   എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
			തിരികെ FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    നിർത്തുക;

	/*
	 * Option value: &name
	 */
	അവസ്ഥ '&':	എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
			    തിരികെ FAIL;
			ret = compile_get_option(arg, cctx);
			നിർത്തുക;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	അവസ്ഥ '$':	എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
			    തിരികെ FAIL;
			എങ്കിൽ ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			അല്ലെങ്കിൽ
			    ret = compile_get_env(arg, cctx);
			നിർത്തുക;

	/*
	 * Register contents: @r.
	 */
	അവസ്ഥ '@':	എങ്കിൽ (generate_ppconst(cctx, ppconst) == FAIL)
			    തിരികെ FAIL;
			ret = compile_get_register(arg, cctx);
			നിർത്തുക;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	അവസ്ഥ '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    എങ്കിൽ (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    നിർത്തുക;

	സ്ഥിരസ്ഥിതി:    ret = NOTDONE;
		    നിർത്തുക;
    }
    എങ്കിൽ (ret == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	അല്ലെങ്കിൽ
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ret == NOTDONE)
    {
	char_u	    *p;
	പൂർണ്ണസംഖ്യ	    r;

	എങ്കിൽ (!eval_isnamec1(**arg))
	{
	    എങ്കിൽ (!vim9_bad_comment(*arg))
	    {
		എങ്കിൽ (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		അല്ലെങ്കിൽ
		    semsg(_(e_name_expected_str), *arg);
	    }
	    തിരികെ FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	എങ്കിൽ (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    തിരികെ FAIL;
	}

	size_t namelen = p - *arg;
	എങ്കിൽ (*p == '<')
	{
	    എങ്കിൽ (skip_generic_func_type_args(&p) == FAIL)
		തിരികെ FAIL;
	}

	എങ്കിൽ (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		തിരികെ FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	എങ്കിൽ (r == FAIL)
	    തിരികെ FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    എങ്കിൽ (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	എങ്കിൽ (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    തിരികെ FAIL;
	തിരികെ OK;
    }
    എങ്കിൽ (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = ശൂന്യ;

    // Recognize <type>
    എങ്കിൽ (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	എങ്കിൽ (want_type == ശൂന്യ)
	    തിരികെ FAIL;

	എങ്കിൽ (**arg != '>')
	{
	    എങ്കിൽ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    അല്ലെങ്കിൽ
		emsg(_(e_missing_gt));
	    തിരികെ FAIL;
	}
	++*arg;
	എങ്കിൽ (may_get_next_വരി_error(*arg, arg, cctx) == FAIL)
	    തിരികെ FAIL;
    }

    എങ്കിൽ (compile_expr9(arg, cctx, ppconst) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (want_type != ശൂന്യ)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_അട്ടി(cctx, 0);
	എങ്കിൽ (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    എങ്കിൽ (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		തിരികെ FAIL;
	}
    }

    തിരികെ OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    പൂർണ്ണസംഖ്യ		ppconst_used = ppconst->pp_used;

    // get the first expression
    എങ്കിൽ (compile_expr8(arg, cctx, ppconst) == FAIL)
	തിരികെ FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    വേണ്ടി (;;)
    {
	op = may_peek_next_വരി(cctx, *arg, &next);
	എങ്കിൽ (*op != '*' && *op != '/' && *op != '%')
	    നിർത്തുക;
	എങ്കിൽ (next != ശൂന്യ)
	{
	    *arg = next_വരി_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	എങ്കിൽ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (may_get_next_വരി_error(op + 1, arg, cctx) == FAIL)
	    തിരികെ FAIL;

	// get the second expression
	എങ്കിൽ (compile_expr8(arg, cctx, ppconst) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_സംഖ്യ
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_സംഖ്യ)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    പൂർണ്ണസംഖ്യ		    failed = FALSE;

	    // both are numbers: compute the result
	    തിരഞ്ഞെടുപ്പ് (*op)
	    {
		അവസ്ഥ '*': res = tv1->vval.v_സംഖ്യ * tv2->vval.v_സംഖ്യ;
			  നിർത്തുക;
		അവസ്ഥ '/': res = അക്കം_divide(tv1->vval.v_സംഖ്യ,
						  tv2->vval.v_സംഖ്യ, &failed);
			  നിർത്തുക;
		അവസ്ഥ '%': res = അക്കം_modulus(tv1->vval.v_സംഖ്യ,
						  tv2->vval.v_സംഖ്യ, &failed);
			  നിർത്തുക;
	    }
	    എങ്കിൽ (failed)
		തിരികെ FAIL;
	    tv1->vval.v_സംഖ്യ = res;
	    --ppconst->pp_used;
	}
	അല്ലെങ്കിൽ
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    തിരികെ OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    പൂർണ്ണസംഖ്യ		oplen;
    പൂർണ്ണസംഖ്യ		ppconst_used = ppconst->pp_used;

    // get the first variable
    എങ്കിൽ (compile_expr7(arg, cctx, ppconst) == FAIL)
	തിരികെ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    വേണ്ടി (;;)
    {
	op = may_peek_next_വരി(cctx, *arg, &next);
	എങ്കിൽ (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    നിർത്തുക;
	എങ്കിൽ (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    നിർത്തുക;
	oplen = (*op == '.' ? 2 : 1);
	എങ്കിൽ (next != ശൂന്യ)
	{
	    *arg = next_വരി_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	എങ്കിൽ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (may_get_next_വരി_error(op + oplen, arg, cctx) == FAIL)
	    തിരികെ FAIL;

	// get the second expression
	എങ്കിൽ (compile_expr7(arg, cctx, ppconst) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_സംഖ്യ
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_സംഖ്യ)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    എങ്കിൽ (*op == '+')
		tv1->vval.v_സംഖ്യ = tv1->vval.v_സംഖ്യ + tv2->vval.v_സംഖ്യ;
	    അല്ലെങ്കിൽ എങ്കിൽ (*op == '-')
		tv1->vval.v_സംഖ്യ = tv1->vval.v_സംഖ്യ - tv2->vval.v_സംഖ്യ;
	    അല്ലെങ്കിൽ
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		എങ്കിൽ (tv1->vval.v_string == ശൂന്യ)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    തിരികെ FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	അല്ലെങ്കിൽ
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    എങ്കിൽ (*op == '.')
	    {
		എങ്കിൽ (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    തിരികെ FAIL;
		എങ്കിൽ (generate_CONCAT(cctx, 2) == FAIL)
		    തിരികെ FAIL;
	    }
	    അല്ലെങ്കിൽ
		generate_two_op(cctx, op);
	}
    }

    തിരികെ OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = വ്യഞ്ജകം_UNKNOWN;
    char_u	*p;
    char_u	*next;
    പൂർണ്ണസംഖ്യ		len = 2;
    പൂർണ്ണസംഖ്യ		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    എങ്കിൽ (compile_expr6(arg, cctx, ppconst) == FAIL)
	തിരികെ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    വേണ്ടി (;;)
    {
	type = വ്യഞ്ജകം_UNKNOWN;

	p = may_peek_next_വരി(cctx, *arg, &next);
	എങ്കിൽ (p[0] == '<' && p[1] == '<')
	    type = വ്യഞ്ജകം_LSHIFT;
	അല്ലെങ്കിൽ എങ്കിൽ (p[0] == '>' && p[1] == '>')
	    type = വ്യഞ്ജകം_RSHIFT;

	എങ്കിൽ (type == വ്യഞ്ജകം_UNKNOWN)
	    തിരികെ OK;

	// Handle a bitwise left or right shift operator
	എങ്കിൽ (ppconst->pp_used == ppconst_used + 1)
	{
	    എങ്കിൽ (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_സംഖ്യ)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_സംഖ്യ));
		തിരികെ FAIL;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    type_T	*t = get_type_on_അട്ടി(cctx, 0);

	    എങ്കിൽ (need_type(t, &t_സംഖ്യ, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_സംഖ്യ));
		തിരികെ FAIL;
	    }
	}

	എങ്കിൽ (next != ശൂന്യ)
	{
	    *arg = next_വരി_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	എങ്കിൽ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    തിരികെ FAIL;
	}

	// get the second variable
	എങ്കിൽ (may_get_next_വരി_error(p + len, arg, cctx) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (compile_expr6(arg, cctx, ppconst) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    എങ്കിൽ (tv2->v_type != VAR_സംഖ്യ || tv2->vval.v_സംഖ്യ < 0)
	    {
		// right operand should be a positive number
		എങ്കിൽ (tv2->v_type != VAR_സംഖ്യ)
		    emsg(_(e_bitshift_ops_must_be_സംഖ്യ));
		അല്ലെങ്കിൽ
		    emsg(_(e_bitshift_ops_must_be_positive));
		തിരികെ FAIL;
	    }

	    എങ്കിൽ (tv2->vval.v_സംഖ്യ > MAX_LSHIFT_BITS)
		tv1->vval.v_സംഖ്യ = 0;
	    അല്ലെങ്കിൽ എങ്കിൽ (type == വ്യഞ്ജകം_LSHIFT)
		tv1->vval.v_സംഖ്യ =
			(uvarnumber_T)tv1->vval.v_സംഖ്യ << tv2->vval.v_സംഖ്യ;
	    അല്ലെങ്കിൽ
		tv1->vval.v_സംഖ്യ =
			(uvarnumber_T)tv1->vval.v_സംഖ്യ >> tv2->vval.v_സംഖ്യ;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (need_type(get_type_on_അട്ടി(cctx, 0), &t_സംഖ്യ, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_സംഖ്യ));
		തിരികെ FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    എങ്കിൽ (isn == ശൂന്യ)
		തിരികെ FAIL;

	    എങ്കിൽ (isn != ശൂന്യ)
		isn->isn_arg.op.op_type = type;
	}
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = വ്യഞ്ജകം_UNKNOWN;
    char_u	*p;
    char_u	*next;
    പൂർണ്ണസംഖ്യ		len = 2;
    പൂർണ്ണസംഖ്യ		type_is = FALSE;
    പൂർണ്ണസംഖ്യ		ppconst_used = ppconst->pp_used;

    // get the first variable
    എങ്കിൽ (compile_expr5(arg, cctx, ppconst) == FAIL)
	തിരികെ FAIL;

    p = may_peek_next_വരി(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    എങ്കിൽ (type != വ്യഞ്ജകം_UNKNOWN)
    {
	പൂർണ്ണസംഖ്യ ic = FALSE;  // Default: do not ignore case

	എങ്കിൽ (next != ശൂന്യ)
	{
	    *arg = next_വരി_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	എങ്കിൽ (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    തിരികെ FAIL;
	}
	// extra question mark appended: ignore case
	എങ്കിൽ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	അല്ലെങ്കിൽ എങ്കിൽ (p[len] == '#')
	    ++len;
	// nothing appended: match case

	എങ്കിൽ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    തിരികെ FAIL;
	}

	// get the second variable
	എങ്കിൽ (may_get_next_വരി_error(p + len, arg, cctx) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (compile_expr5(arg, cctx, ppconst) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    പൂർണ്ണസംഖ്യ		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    എങ്കിൽ (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    അല്ലെങ്കിൽ
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_സംഖ്യ = tv1->vval.v_സംഖ്യ
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    തിരികെ ret;
	}

	generate_ppconst(cctx, ppconst);
	തിരികെ generate_COMPARE(cctx, type, ic);
    }

    തിരികെ OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	അക്ഷരം	*op,
	ppconst_T *ppconst,
	പൂർണ്ണസംഖ്യ	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_വരി(cctx, *arg, &next);
    പൂർണ്ണസംഖ്യ		opchar = *op;

    എങ്കിൽ (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	പൂർണ്ണസംഖ്യ		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, വലുപ്പം(പൂർണ്ണസംഖ്യ), 10);
	ആകുമ്പോൾ (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    പൂർണ്ണസംഖ്യ		start_ctx_lnum = cctx->ctx_lnum;
	    പൂർണ്ണസംഖ്യ		save_lnum;
	    പൂർണ്ണസംഖ്യ		const_used;
	    പൂർണ്ണസംഖ്യ		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    എങ്കിൽ (next != ശൂന്യ)
	    {
		*arg = next_വരി_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    എങ്കിൽ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		തിരികെ FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    എങ്കിൽ (status != FAIL)
	    {
		// Use the last ppconst if possible.
		എങ്കിൽ (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    പൂർണ്ണസംഖ്യ		is_true = tv2bool(tv);

		    എങ്കിൽ ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_സംഖ്യ = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    അല്ലെങ്കിൽ
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_അട്ടി(cctx);
		}
	    }
	    എങ്കിൽ (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    എങ്കിൽ (status == FAIL)
	    {
		ga_clear(&end_ga);
		തിരികെ FAIL;
	    }

	    എങ്കിൽ (jump_when != JUMP_NEVER)
	    {
		എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
		{
		    *(((പൂർണ്ണസംഖ്യ *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    എങ്കിൽ (may_get_next_വരി_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		തിരികെ FAIL;
	    }

	    const_used = ppconst->pp_used;
	    എങ്കിൽ ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		തിരികെ FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    എങ്കിൽ (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		എങ്കിൽ (tv->v_type == VAR_സംഖ്യ
			 && (tv->vval.v_സംഖ്യ == 1 || tv->vval.v_സംഖ്യ == 0))
		{
		    tv->vval.v_സംഖ്യ = tv->vval.v_സംഖ്യ == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_വരി(cctx, *arg, &next);
	}

	എങ്കിൽ (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    എങ്കിൽ (bool_on_അട്ടി(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		തിരികെ FAIL;
	    }

	എങ്കിൽ (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    ആകുമ്പോൾ (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((പൂർണ്ണസംഖ്യ *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    പൂർണ്ണസംഖ്യ		ppconst_used = ppconst->pp_used;

    // get the first variable
    എങ്കിൽ (compile_expr4(arg, cctx, ppconst) == FAIL)
	തിരികെ FAIL;

    // || and && work almost the same
    തിരികെ compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    പൂർണ്ണസംഖ്യ		ppconst_used = ppconst->pp_used;

    // eval the first expression
    എങ്കിൽ (compile_expr3(arg, cctx, ppconst) == FAIL)
	തിരികെ FAIL;

    // || and && work almost the same
    തിരികെ compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    പൂർണ്ണസംഖ്യ
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    പൂർണ്ണസംഖ്യ		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    എങ്കിൽ (cctx->ctx_skip == SKIP_YES)
    {
	പൂർണ്ണസംഖ്യ		prev_did_emsg = did_emsg;

	skip_വ്യഞ്ജകം_cctx(arg, cctx);
	തിരികെ did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    എങ്കിൽ (compile_expr2(arg, cctx, ppconst) == FAIL)
	തിരികെ FAIL;

    p = may_peek_next_വരി(cctx, *arg, &next);
    എങ്കിൽ (*p == '?')
    {
	പൂർണ്ണസംഖ്യ		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;
	പൂർണ്ണസംഖ്യ		alt_idx = instr->ga_len;
	പൂർണ്ണസംഖ്യ		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = ശൂന്യ;
	പൂർണ്ണസംഖ്യ		has_const_വ്യഞ്ജകം = FALSE;
	പൂർണ്ണസംഖ്യ		const_മൂല്യം = FALSE;
	പൂർണ്ണസംഖ്യ		save_skip = cctx->ctx_skip;

	എങ്കിൽ (next != ശൂന്യ)
	{
	    *arg = next_വരി_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	എങ്കിൽ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_വ്യഞ്ജകം = TRUE;
	    എങ്കിൽ (op_falsy)
		const_മൂല്യം = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    അല്ലെങ്കിൽ
	    {
		പൂർണ്ണസംഖ്യ error = FALSE;

		const_മൂല്യം = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		എങ്കിൽ (error)
		    തിരികെ FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_മൂല്യം : !const_മൂല്യം) ? SKIP_YES : SKIP_NOT;

	    എങ്കിൽ (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    അല്ലെങ്കിൽ
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    generate_ppconst(cctx, ppconst);
	    എങ്കിൽ (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    എങ്കിൽ (op_falsy)
	    {
		type1 = get_type_on_അട്ടി(cctx, -1);
		എങ്കിൽ (check_type_is_മൂല്യം(type1) == FAIL)
		    തിരികെ FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	എങ്കിൽ (may_get_next_വരി_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (compile_expr1(arg, cctx, ppconst) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (!has_const_വ്യഞ്ജകം)
	{
	    generate_ppconst(cctx, ppconst);

	    എങ്കിൽ (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_അട്ടി(cctx, 0);
		--അട്ടി->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	എങ്കിൽ (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_വരി(cctx, *arg, &next);
	    എങ്കിൽ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (next != ശൂന്യ)
	    {
		*arg = next_വരി_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    എങ്കിൽ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		തിരികെ FAIL;
	    }

	    // evaluate the third expression
	    എങ്കിൽ (has_const_വ്യഞ്ജകം)
		cctx->ctx_skip = save_skip == SKIP_YES || const_മൂല്യം
							 ? SKIP_YES : SKIP_NOT;
	    എങ്കിൽ (may_get_next_വരി_error(p + 1, arg, cctx) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (compile_expr1(arg, cctx, ppconst) == FAIL)
		തിരികെ FAIL;
	}

	എങ്കിൽ (!has_const_വ്യഞ്ജകം)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)അട്ടി->ga_data)
					      + അട്ടി->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    തിരികെ OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
compile_expr0_ext(char_u **arg,  cctx_T *cctx, പൂർണ്ണസംഖ്യ *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    എങ്കിൽ (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	തിരികെ FAIL;
    }
    എങ്കിൽ (is_const != ശൂന്യ)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    എങ്കിൽ (generate_ppconst(cctx, &ppconst) == FAIL)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Toplevel expression.
 */
    പൂർണ്ണസംഖ്യ
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    തിരികെ compile_expr0_ext(arg, cctx, ശൂന്യ);
}


#endif // defined(FEAT_EVAL)
