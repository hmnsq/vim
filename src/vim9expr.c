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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
நிலையான முழு paren_follows_after_கோவை = 0;

/*
 * Generate code for any ppconst entries.
 */
    முழு
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    முழு	    i;
    முழு	    ret = OK;
    முழு	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    க்கு (i = 0; i < ppconst->pp_used; ++i)
	எனில் (generate_tv_தள்ளு(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    திரும்பு ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    நிலையான முழு
check_ppconst_bool(ppconst_T *ppconst)
{
    எனில் (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	திரும்பு check_typval_type(&t_bool, tv, where);
    }
    திரும்பு OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    வெறுமை
clear_ppconst(ppconst_T *ppconst)
{
    முழு	    i;

    க்கு (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    நிலையான முழு
compile_tuple_member(
    type2_T	*typep,
    முழு		is_slice,
    cctx_T	*cctx)
{
    எனில் (is_slice)
    {
	எனில் (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    திரும்பு FAIL;
	// a copy is made so the member type is no longer declared
	எனில் (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	எனில் (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    எனில் (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    இல்லையெனில்
    {
	எனில் (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    எனில் (typep->type_curr->tt_argcount == 1)
	    {
		எனில் (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		இல்லையெனில்
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    இல்லையெனில்
		typep->type_curr = &t_any;
	    எனில் (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		எனில் (typep->type_decl->tt_argcount == 1)
		{
		    எனில் (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    இல்லையெனில்
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		இல்லையெனில்
		    typep->type_curr = &t_any;
	    }
	    இல்லையெனில்
		typep->type_decl = typep->type_curr;
	}
	எனில் (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    திரும்பு FAIL;
    }

    திரும்பு OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    முழு
compile_member(முழு is_slice, முழு *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)அடுக்கு->ga_data) + அடுக்கு->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)அடுக்கு->ga_data) + அடுக்கு->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    எனில் ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    எனில் (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	எனில் (need_type(idxtype, &t_எண், 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    திரும்பு FAIL;
	எனில் (is_slice)
	{
	    idxtype = get_type_on_அடுக்கு(cctx, 1);
	    எனில் (need_type(idxtype, &t_எண், 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		திரும்பு FAIL;
	}
    }

    எனில் (vartype == VAR_DICT)
    {
	எனில் (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    திரும்பு FAIL;
	}
	எனில் (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    எனில் (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    எனில் (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		எனில் (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    இல்லையெனில்
		typep->type_decl = typep->type_curr;
	}
	இல்லையெனில்
	{
	    எனில் (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		திரும்பு FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	எனில் (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    திரும்பு FAIL;
	எனில் (keeping_dict != வெற்று)
	    *keeping_dict = TRUE;
    }
    இல்லையெனில் எனில் (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	எனில் ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (vartype == VAR_BLOB)
    {
	எனில் (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    எனில் (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில்
	{
	    typep->type_curr = &t_எண்;
	    typep->type_decl = &t_எண்;
	    எனில் (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		திரும்பு FAIL;
	}
    }
    இல்லையெனில் எனில் (vartype == VAR_TUPLE)
    {
	எனில் (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	எனில் (is_slice)
	{
	    எனில் (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		திரும்பு FAIL;
	    // a copy is made so the member type is no longer declared
	    எனில் (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    எனில் (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		எனில் (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	இல்லையெனில்
	{
	    எனில் (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		எனில் (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		எனில் (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    எனில் (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		இல்லையெனில்
			typep->type_decl = typep->type_curr;
	    }
	    எனில் (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		திரும்பு FAIL;
	}
    }
    இல்லையெனில்
    {
	தேர்வு (vartype)
	{
	    நிலை VAR_FUNC:
	    நிலை VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		நிறுத்து;
	    நிலை VAR_BOOL:
	    நிலை VAR_SPECIAL:
	    நிலை VAR_JOB:
	    நிலை VAR_CHANNEL:
	    நிலை VAR_INSTR:
	    நிலை VAR_CLASS:
	    நிலை VAR_OBJECT:
	    நிலை VAR_TYPEALIAS:
	    நிலை VAR_UNKNOWN:
	    நிலை VAR_ANY:
	    நிலை VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		நிறுத்து;
	    இயல்பு:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    நிலையான முழு
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    க்கு (cctx_T *cctx = cctx_arg; cctx != வெற்று; cctx = cctx->ctx_outer)
    {
	எனில் (cctx->ctx_ufunc != வெற்று && cctx->ctx_ufunc->uf_class != வெற்று)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    போது (clp != வெற்று)
	    {
		எனில் (clp == cl)
		    திரும்பு TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    திரும்பு FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    முழு
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    முழு		m_idx;
    முழு		ret = FAIL;

    எனில் (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	திரும்பு FAIL;
    }

    class_T *cl = type->tt_class;
    முழு is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    எனில் (type == &t_super)
    {
	எனில் (cctx->ctx_ufunc == வெற்று || cctx->ctx_ufunc->uf_class == வெற்று)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    திரும்பு FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_அடுக்கு.ga_len;
    }
    இல்லையெனில் எனில் (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	எனில் (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    எனில் (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    எனில் (cl == வெற்று)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	திரும்பு FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, வெற்று, வெற்று, FNE_CHECK_START);
    எனில் (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	திரும்பு FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    எனில் (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	எனில் (name_end == வெற்று)
	    செல் done;
    }

    எனில் (*name_end == '(')
    {
	முழு	function_மொத்தம்;
	முழு	child_மொத்தம்;
	ufunc_T	**functions;

	எனில் (type->tt_type == VAR_CLASS)
	{
	    function_மொத்தம் = cl->class_class_function_மொத்தம்;
	    child_மொத்தம் = cl->class_class_function_மொத்தம்_child;
	    functions = cl->class_class_functions;
	}
	இல்லையெனில்
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    எனில் (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_மொத்தம் = cl->class_obj_method_மொத்தம்;
	    child_மொத்தம் = cl->class_obj_method_மொத்தம்_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = வெற்று;
	முழு fi;
	க்கு (fi = is_super ? child_மொத்தம் : 0; fi < function_மொத்தம்; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    எனில் (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		நிறுத்து;
	    }
	}

	ocmember_T  *ocm = வெற்று;
	எனில் (ufunc == வெற்று)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    எனில் (ocm == வெற்று || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_செய்தி(cl, type->tt_type, name, len);
		செல் done;
	    }
	    எனில் (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_அடுக்கு.ga_len;
		எனில் (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    செல் done;
	    }
	    இல்லையெனில்
	    {
		முழு status;

		எனில் (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		இல்லையெனில்
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		எனில் (status == FAIL)
		    செல் done;
	    }
	}

	எனில் (is_super && ufunc != வெற்று && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    செல் done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	எனில் (ocm == வெற்று && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    செல் done;
	}

	// process generic function call
	எனில் (ufunc != வெற்று)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    எனில் (ufunc == வெற்று)
		செல் done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	முழு argcount = 0;
	எனில் (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    செல் done;

	எனில் (ocm != வெற்று)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    செல் done;
	}
	எனில் (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	இல்லையெனில்
	    ret = generate_CALL(cctx, ufunc, வெற்று, 0, argcount, FALSE);
	செல் done;
    }

    எனில் (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	எனில் (m_idx >= 0)
	{
	    எனில் (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		செல் done;
	    }

	    *arg = name_end;
	    எனில் (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    இல்லையெனில்
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    செல் done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	எனில் (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    எனில் (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		செல் done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_அடுக்கு.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, வெற்று);
	    செல் done;
	}

	member_not_found_செய்தி(cl, VAR_OBJECT, name, len);
    }
    இல்லையெனில்
    {
	// load class member
	முழு idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	எனில் (m != வெற்று)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    எனில் (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		செல் done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_அடுக்கு.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    செல் done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	எனில் (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    எனில் (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		செல் done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_அடுக்கு.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, வெற்று);
	    செல் done;
	}

	member_not_found_செய்தி(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    திரும்பு ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    முழு
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    முழு		    idx;
    imported_T	    *imp;

    எனில் (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	திரும்பு FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, வெற்று);
    எனில் (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	திரும்பு OK;
    }

    எனில் (end == வெற்று)
	imp = வெற்று;
    இல்லையெனில் எனில் (import == வெற்று)
	imp = find_imported(name, 0, FALSE);
    இல்லையெனில்
	imp = import;

    எனில் (imp != வெற்று)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	முழு	cc;
	ufunc_T	*ufunc = வெற்று;
	type_T	*type;
	முழு	done = FALSE;
	முழு	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	எனில் (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    திரும்பு FAIL;
	}
	++p;
	எனில் (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    திரும்பு FAIL;
	}

	// isolate one name
	exp_name = p;
	போது (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	எனில் (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	எனில் (res == OK)
	{
	    எனில் (si->sn_autoload_prefix != வெற்று
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		எனில் (cc == '(' || paren_follows_after_கோவை)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		இல்லையெனில்
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    இல்லையெனில் எனில் (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		எனில் (cc == '(' || paren_follows_after_கோவை)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		இல்லையெனில்
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    இல்லையெனில்
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, வெற்று, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	எனில் (done)
	    திரும்பு res;

	எனில் (idx < 0)
	{
	    எனில் (ufunc != வெற்று)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		எனில் (IS_GENERIC_FUNC(ufunc))
		{
		    எனில் (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			எனில் (p != வெற்று)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			எனில் (p == வெற்று || ufunc == வெற்று)
			    திரும்பு FAIL;
		    }
		    இல்லையெனில்
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			திரும்பு FAIL;
		    }
		}
		இல்லையெனில் எனில் (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    திரும்பு FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, வெற்று, TRUE);
		திரும்பு OK;
	    }
	    திரும்பு FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	திரும்பு OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    திரும்பு generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    நிலையான முழு
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    முழு			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    எனில் (ufunc == வெற்று || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	திரும்பு FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    எனில் (ufunc == வெற்று)
	திரும்பு FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    எனில் (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, வெற்று) == FAIL)
	திரும்பு FAIL;
    திரும்பு generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    நிலையான முழு
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    திரும்பு cctx->ctx_ufunc != வெற்று
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == வெற்று
		|| cctx->ctx_outer->ctx_ufunc == வெற்று
		|| cctx->ctx_outer->ctx_ufunc->uf_class == வெற்று
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
    நிலையான முழு
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    முழு			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    முழு		isn_idx;

    எனில் (fp == வெற்று)
	திரும்பு FAIL;

    க்கு (முழு depth = 0; depth < outer_depth; ++depth)
    {
	எனில் (outer_cctx->ctx_outer == வெற்று)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    திரும்பு FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    எனில் (fp->uf_block_ids == வெற்று && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(முழு, ufunc->uf_block_depth);
	எனில் (fp->uf_block_ids != வெற்று)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    அளவு(முழு) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    எனில் (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	திரும்பு FAIL;

    எனில் (generate_FUNCREF(cctx, fp, வெற்று, FALSE, 0, &isn_idx) == FAIL)
	திரும்பு FAIL;
    எனில் (cctx->ctx_skip == SKIP_YES)
	திரும்பு OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    எனில் (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	எனில் (extra == வெற்று)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    எனில் (extra == வெற்று)
		திரும்பு FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    திரும்பு OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    முழு
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	முழு	is_கோவை,
	முழு	error)
{
    type_T	*type;
    char_u	*name = வெற்று;
    char_u	*end = end_arg;
    முழு		res = FAIL;
    முழு		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = வெற்று;

    generic_func_args_table_init(&gfatab);

    எனில் (*(*arg + namelen) == '<')
    {
	// generic function call
	எனில் (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == வெற்று)
	{
	    generic_func_args_table_clear(&gfatab);
	    திரும்பு FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    எனில் (*(*arg + 1) == ':')
    {
	எனில் (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    தேர்வு (**arg)
	    {
		நிலை 'g': isn_type = ISN_LOADGDICT; நிறுத்து;
		நிலை 'w': isn_type = ISN_LOADWDICT; நிறுத்து;
		நிலை 't': isn_type = ISN_LOADTDICT; நிறுத்து;
		நிலை 'b': isn_type = ISN_LOADBDICT; நிறுத்து;
		இயல்பு:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    செல் theend;
	    }
	    எனில் (generate_instr_type(cctx, isn_type, &t_dict_any) == வெற்று)
		செல் theend;
	    res = OK;
	}
	இல்லையெனில்
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    எனில் (name == வெற்று)
		செல் theend;

	    தேர்வு (**arg)
	    {
		நிலை 'v': res = generate_LOADV(cctx, name);
			  நிறுத்து;
		நிலை 's': எனில் (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      செல் theend;
			  }
			  எனில் (is_கோவை && find_func(name, FALSE) != வெற்று)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  இல்லையெனில்
			      res = compile_load_scriptvar(cctx, name,
							    வெற்று, &end, வெற்று);
			  நிறுத்து;
		நிலை 'g': எனில் (vim_strchr(name, AUTOLOAD_CHAR) == வெற்று)
			  {
			      எனில் (is_கோவை && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != வெற்று
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      இல்லையெனில்
				  isn_type = ISN_LOADG;
			  }
			  இல்லையெனில்
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      எனில் (name == வெற்று)
				  செல் theend;
			  }
			  நிறுத்து;
		நிலை 'w': isn_type = ISN_LOADW; நிறுத்து;
		நிலை 't': isn_type = ISN_LOADT; நிறுத்து;
		நிலை 'b': isn_type = ISN_LOADB; நிறுத்து;
		இயல்பு:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  செல் theend;
	    }
	    எனில் (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    இல்லையெனில்
    {
	முழு	    idx;
	முழு	    method_idx;
	முழு	    gen_load = FALSE;
	முழு	    gen_load_outer = 0;
	முழு	    outer_loop_depth = -1;
	முழு	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	எனில் (name == வெற்று)
	    செல் theend;

	எனில் (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = தள்ளு_type_அடுக்கு(cctx, &t_super);
	    எனில் (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	இல்லையெனில் எனில் (vim_strchr(name, AUTOLOAD_CHAR) != வெற்று)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	இல்லையெனில் எனில் (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    எனில் (gen_load_outer == 0)
		gen_load = TRUE;
	}
	இல்லையெனில்
	{
	    lvar_T  lvar;
	    class_T *cl = வெற்று;

	    எனில் (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		எனில் (lvar.lv_ufunc != வெற்று && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		இல்லையெனில் எனில் (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		இல்லையெனில்
		    gen_load = TRUE;
	    }
	    இல்லையெனில் எனில் (cctx->ctx_ufunc->uf_defclass != வெற்று &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		எனில் (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    எனில் (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    இல்லையெனில்
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									வெற்று);
		    }
		}
		இல்லையெனில்
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    இல்லையெனில்
	    {
		imported_T *imp = வெற்று;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		எனில் (script_var_exists(*arg, namelen, cctx, வெற்று) == OK
			    || (imp = find_imported(name, 0, FALSE)) != வெற்று)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		எனில் (res == FAIL && is_கோவை && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	எனில் (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, வெற்று, type);
	எனில் (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    எனில் (lt != வெற்று)
	*lt = '<';
    எனில் (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    திரும்பு res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    நிலையான முழு
compile_string(isn_T *isn, cctx_T *cctx, முழு str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    முழு		கோவை_res;
    முழு		trailing_error;
    முழு		instr_மொத்தம்;
    isn_T	*instr = வெற்று;

    // Remove the string type from the stack.
    --cctx->ctx_type_அடுக்கு.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = வெற்று;

    // avoid peeking a next line
    முழு galen_save = cctx->ctx_ufunc->uf_வரிகள்.ga_len;
    cctx->ctx_ufunc->uf_வரிகள்.ga_len = 0;

    கோவை_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_வரிகள்.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    எனில் (கோவை_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	எனில் (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_அடுக்கு.ga_len;
	திரும்பு FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_மொத்தம் = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_மொத்தம்].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    திரும்பு OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    முழு
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	முழு	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    முழு	    must_end = FALSE;
    முழு	    instr_மொத்தம்;

    க்கு (;;)
    {
	எனில் (may_get_next_வரி(whitep, &p, cctx) == FAIL)
	    செல் failret;
	எனில் (*p == ')')
	{
	    *arg = p + 1;
	    திரும்பு OK;
	}
	எனில் (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    திரும்பு FAIL;
	}

	instr_மொத்தம் = cctx->ctx_instr.ga_len;
	எனில் (compile_expr0(&p, cctx) == FAIL)
	    திரும்பு FAIL;
	++*argcount;

	எனில் (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_மொத்தம் + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_மொத்தம்;

	    // {skip} argument of searchpair() can be compiled if not empty
	    எனில் (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != வெற்று
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	இல்லையெனில் எனில் (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_மொத்தம் + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_மொத்தம்;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    எனில் (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != வெற்று
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	எனில் (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    திரும்பு FAIL;
	}
	எனில் (*p == ',')
	{
	    ++p;
	    எனில் (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		திரும்பு FAIL;
	    }
	}
	இல்லையெனில்
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    திரும்பு FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    நிலையான முழு
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_அடுக்கு(cctx, 0);
    முழு		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    எனில் (type->tt_type == VAR_OBJECT)
    {
	முழு	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	எனில் (uf != வெற்று)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    திரும்பு res;
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
    நிலையான முழு
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	முழு	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    முழு		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = வெற்று;
    char_u	*gname = வெற்று;
    ufunc_T	*ufunc = வெற்று;
    முழு		res = FAIL;
    முழு		is_autoload;
    முழு		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    எனில் (varlen >= அளவு(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	திரும்பு FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    எனில் (import != வெற்று)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	திரும்பு FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    எனில் ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	முழு	    is_has = **arg == 'h';
	முழு	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	எனில் (*s == '"')
	    (வெறுமை)eval_string(&s, &argvars[0], TRUE, FALSE);
	இல்லையெனில் எனில் (*s == '\'')
	    (வெறுமை)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	எனில் (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    எனில் (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_எண்;
		tv->vval.v_எண் = 0;
		எனில் (is_has)
		    f_has(argvars, tv);
		இல்லையெனில் எனில் (is_len)
		    f_len(argvars, tv);
		இல்லையெனில்
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    திரும்பு OK;
	}
	clear_tv(&argvars[0]);
	எனில் (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    திரும்பு FAIL;
	}
    }

    எனில் (generate_ppconst(cctx, ppconst) == FAIL)
	திரும்பு FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    எனில் ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    இல்லையெனில் எனில் (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    இல்லையெனில்
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    எனில் (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	எனில் (*arg == வெற்று)
	    செல் theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	எனில் (gname == வெற்று)
	    செல் theend;
	++*arg;			// skip '('
    }
    இல்லையெனில்
	*arg += varlen + 1;

    எனில் (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	செல் theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != வெற்று;
    எனில் (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	முழு	    idx;

	// builtin function
	idx = find_internal_func(name);
	எனில் (idx >= 0)
	{
	    எனில் (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		செல் theend;
	    }

	    எனில் (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_அடுக்கு(cctx, 1);
		எனில் (check_type_is_மதிப்பு(get_type_on_அடுக்கு(cctx, 0)) == FAIL)
		    செல் theend;

		// add() can be compiled to instructions if we know the type
		எனில் (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		இல்லையெனில் எனில் (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    எனில் ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		எனில் (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    எனில் (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    இல்லையெனில் எனில் (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    இல்லையெனில் எனில் (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    எனில் (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		எனில் (res == OK)
		    idx = -1;
	    }

	    எனில் (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	இல்லையெனில்
	    emsg_funcname(e_unknown_function_str, namebuf);
	செல் theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    எனில் (lookup_local(namebuf, varlen, வெற்று, cctx) == FAIL
	    && arg_exists(namebuf, varlen, வெற்று, வெற்று, வெற்று, cctx) != OK)
    {
	class_T		*cl = வெற்று;
	முழு		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	எனில் (ufunc != வெற்று)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    எனில் (ufunc == வெற்று)
		செல் theend;

	    எனில் (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, வெற்று, 0, argcount, FALSE);
		செல் theend;
	    }
	    எனில் (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == வெற்று
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		செல் theend;
	    }
	}
	இல்லையெனில் எனில் ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    எனில் (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], வெற்று,
							0, argcount, FALSE);
	    }
	    இல்லையெனில்
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    செல் theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == வெற்று && gname != வெற்று ? gname : namebuf;
    எனில் (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_அடுக்கு(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	செல் theend;
    }

    // If we can find a global function by name generate the right call.
    எனில் (ufunc != வெற்று)
    {
	res = generate_CALL(cctx, ufunc, வெற்று, 0, argcount, FALSE);
	செல் theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    எனில் (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    இல்லையெனில்
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    திரும்பு res;
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
to_name_end(char_u *arg, முழு use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    எனில் (!eval_isnamec1(*arg))
	திரும்பு arg;

    க்கு (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	எனில் (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == வெற்று))
	    நிறுத்து;
    திரும்பு p;
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

    எனில் (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    எனில் (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	எனில் (eval_list(&p, &rettv, வெற்று, FALSE) == FAIL)
	    p = arg;
    }
    திரும்பு p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    நிலையான முழு
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    முழு		மொத்தம் = 0;
    முழு		is_const;
    முழு		is_all_const = TRUE;	// reset when non-const encountered
    முழு		must_end = FALSE;

    க்கு (;;)
    {
	எனில் (may_get_next_வரி(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    திரும்பு FAIL;
	}
	எனில் (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    திரும்பு FAIL;
	}
	எனில் (*p == ']')
	{
	    ++p;
	    நிறுத்து;
	}
	எனில் (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    திரும்பு FAIL;
	}
	எனில் (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    திரும்பு FAIL;
	எனில் (!is_const)
	    is_all_const = FALSE;
	++மொத்தம்;
	எனில் (*p == ',')
	{
	    ++p;
	    எனில் (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		திரும்பு FAIL;
	    }
	}
	இல்லையெனில்
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    திரும்பு generate_NEWLIST(cctx, மொத்தம், FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    நிலையான முழு
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    முழு first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    முழு		மொத்தம் = 0;
    முழு		is_const;
    முழு		is_all_const = TRUE;	// reset when non-const encountered
    முழு		must_end = FALSE;

    எனில் (**arg != ')')
    {
	எனில் (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    திரும்பு FAIL;
	}
	மொத்தம் = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	க்கு (;;)
	{
	    எனில் (may_get_next_வரி(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		திரும்பு FAIL;
	    }
	    எனில் (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		திரும்பு FAIL;
	    }
	    எனில் (*p == ')')
	    {
		++p;
		நிறுத்து;
	    }
	    எனில் (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		திரும்பு FAIL;
	    }
	    எனில் (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		திரும்பு FAIL;
	    எனில் (!is_const)
		is_all_const = FALSE;
	    ++மொத்தம்;
	    எனில் (*p == ',')
	    {
		++p;
		எனில் (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    திரும்பு FAIL;
		}
	    }
	    இல்லையெனில்
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    திரும்பு generate_NEWTUPLE(cctx, மொத்தம், FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    நிலையான வெறுமை
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*வரி;
    size_t	off;

    எனில் (!evalarg->eval_using_cmdline || cctx == வெற்று)
	திரும்பு;

    gap = &evalarg->eval_tofree_ga;
    எனில் (gap->ga_len == 0)
	திரும்பு;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    வரி = ((char_u **)cctx->ctx_ufunc->uf_வரிகள்.ga_data)[cctx->ctx_lnum];
    *arg = வரி + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    முழு
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    முழு		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    எனில் (r != OK)
    {
	clear_evalarg(&evalarg, வெற்று);
	திரும்பு r;
    }

    எனில் (ppconst != வெற்று && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, வெற்று);
	திரும்பு FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    எனில் (cctx->ctx_ufunc != வெற்று)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	முழு block_depth = cctx->ctx_ufunc->uf_block_depth;

	எனில் (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(முழு, block_depth);
	    எனில் (ufunc->uf_block_ids != வெற்று)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    அளவு(முழு) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    எனில் (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    எனில் (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    எனில் (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	எனில் (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, வெற்று);

    எனில் (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	திரும்பு generate_FUNCREF(cctx, ufunc, வெற்று, FALSE, 0, வெற்று);
    }

    func_ptr_unref(ufunc);
    திரும்பு FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    முழு
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	முழு	    types_optional,
	evalarg_T   *evalarg)
{
    முழு		r;
    ufunc_T	*ufunc;
    முழு		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, வெற்று);
    current_sctx.sc_version = save_sc_version;
    எனில் (r != OK)
	திரும்பு r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    எனில் (ufunc->uf_ret_type == வெற்று || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, வெற்று);

    எனில் (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	திரும்பு OK;
    }
    clear_tv(rettv);
    திரும்பு FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    நிலையான முழு
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    முழு		மொத்தம் = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    முழு		is_const;
    முழு		is_all_const = TRUE;	// reset when non-const encountered

    எனில் (d == வெற்று)
	திரும்பு FAIL;
    எனில் (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	திரும்பு FAIL;
    }
    க்கு (;;)
    {
	char_u	    *சாவி = வெற்று;

	எனில் (may_get_next_வரி(whitep, arg, cctx) == FAIL)
	{
	    *arg = வெற்று;
	    செல் failret;
	}

	எனில் (**arg == '}')
	    நிறுத்து;

	எனில் (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    எனில் (compile_expr0(arg, cctx) == FAIL)
		செல் failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    எனில் (isn->isn_type == ISN_PUSHNR)
	    {
		எழுத்து buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.எண்);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    எனில் (isn->isn_type == ISN_PUSHS)
		சாவி = isn->isn_arg.string;
	    இல்லையெனில் எனில் (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		செல் failret;
	    *arg = skipwhite(*arg);
	    எனில் (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_சாவி));
		செல் failret;
	    }
	    ++*arg;
	}
	இல்லையெனில்
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    சாவி = get_literal_சாவி(arg);
	    எனில் (சாவி == வெற்று)
		செல் failret;
	    எனில் (generate_PUSHS(cctx, &சாவி) == FAIL)
		செல் failret;
	}

	// Check for duplicate keys, if using string keys.
	எனில் (சாவி != வெற்று)
	{
	    item = dict_find(d, சாவி, -1);
	    எனில் (item != வெற்று)
	    {
		semsg(_(e_duplicate_சாவி_in_dictionary_str), சாவி);
		செல் failret;
	    }
	    item = dictitem_alloc(சாவி);
	    எனில் (item != வெற்று)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		எனில் (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	எனில் (**arg != ':')
	{
	    எனில் (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    இல்லையெனில்
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    செல் failret;
	}
	whitep = *arg + 1;
	எனில் (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    செல் failret;
	}

	எனில் (may_get_next_வரி(whitep, arg, cctx) == FAIL)
	{
	    *arg = வெற்று;
	    செல் failret;
	}

	எனில் (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    செல் failret;
	எனில் (!is_const)
	    is_all_const = FALSE;
	++மொத்தம்;

	whitep = *arg;
	எனில் (may_get_next_வரி(whitep, arg, cctx) == FAIL)
	{
	    *arg = வெற்று;
	    செல் failret;
	}
	எனில் (**arg == '}')
	    நிறுத்து;
	எனில் (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    செல் failret;
	}
	எனில் (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    செல் failret;
	}
	whitep = *arg + 1;
	எனில் (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    செல் failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    எனில் (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    திரும்பு generate_NEWDICT(cctx, மொத்தம், FALSE);

failret:
    எனில் (*arg == வெற்று)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    திரும்பு FAIL;
}

/*
 * Compile "&option".
 */
    நிலையான முழு
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    முழு		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    எனில் (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_எண் ? &t_எண் : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    திரும்பு ret;
}

/*
 * Compile "$VAR".
 */
    நிலையான முழு
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    முழு		len;
    முழு		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    எனில் (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	திரும்பு FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    திரும்பு ret;
}

/*
 * Compile $"string" or $'string'.
 */
    நிலையான முழு
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    முழு		ret;
    முழு		quote;
    முழு		evaluate = cctx->ctx_skip != SKIP_YES;
    முழு		மொத்தம் = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    க்கு (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	எனில் (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	இல்லையெனில்
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	எனில் (ret == FAIL)
	    நிறுத்து;
	எனில் (evaluate)
	{
	    எனில் ((tv.vval.v_string != வெற்று && *tv.vval.v_string != NUL)
		    || (**arg != '{' && மொத்தம் == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		எனில் (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    திரும்பு FAIL;
		tv.vval.v_string = வெற்று;  // don't free it now
		++மொத்தம்;
	    }
	    clear_tv(&tv);
	}

	எனில் (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    நிறுத்து;
	}

	p = compile_one_கோவை_in_str(*arg, cctx);
	எனில் (p == வெற்று)
	{
	    ret = FAIL;
	    நிறுத்து;
	}
	++மொத்தம்;
	*arg = p;
    }

    எனில் (ret == FAIL || !evaluate)
	திரும்பு ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    எனில் (மொத்தம் > 1)
	திரும்பு generate_CONCAT(cctx, மொத்தம்);

    திரும்பு OK;
}

/*
 * Compile "@r".
 */
    நிலையான முழு
compile_get_register(char_u **arg, cctx_T *cctx)
{
    முழு		ret;

    ++*arg;
    எனில் (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	திரும்பு FAIL;
    }
    எனில் (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	திரும்பு FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, வெற்று, &t_string);
    ++*arg;
    திரும்பு ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    நிலையான முழு
apply_leader(typval_T *rettv, முழு numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    போது (p > start)
    {
	--p;
	எனில் (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    எனில் (rettv->v_type == VAR_FLOAT)
	    {
		எனில் (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    இல்லையெனில்
	    {
		varnumber_T	val;
		முழு		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		எனில் (check_not_string(rettv) == FAIL)
		    திரும்பு FAIL;
		val = tv_get_எண்_chk(rettv, &error);
		clear_tv(rettv);
		எனில் (error)
		    திரும்பு FAIL;
		எனில் (*p == '-')
		    val = -val;
		rettv->v_type = VAR_எண்;
		rettv->vval.v_எண் = val;
	    }
	}
	இல்லையெனில் எனில் (numeric_only)
	{
	    ++p;
	    நிறுத்து;
	}
	இல்லையெனில் எனில் (*p == '!')
	{
	    முழு v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_எண் = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    திரும்பு OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    நிலையான வெறுமை
get_vim_constant(char_u **arg, typval_T *rettv)
{
    எனில் (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_எண் = VVAL_TRUE;
	*arg += 6;
    }
    இல்லையெனில் எனில் (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_எண் = VVAL_FALSE;
	*arg += 7;
    }
    இல்லையெனில் எனில் (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_எண் = VVAL_NULL;
	*arg += 6;
    }
    இல்லையெனில் எனில் (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_எண் = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, முழு *len, முழு *type_is)
{
    exprtype_T	type = கோவை_UNKNOWN;
    முழு		i;

    தேர்வு (p[0])
    {
	நிலை '=':   எனில் (p[1] == '=')
			type = கோவை_EQUAL;
		    இல்லையெனில் எனில் (p[1] == '~')
			type = கோவை_MATCH;
		    நிறுத்து;
	நிலை '!':   எனில் (p[1] == '=')
			type = கோவை_NEQUAL;
		    இல்லையெனில் எனில் (p[1] == '~')
			type = கோவை_NOMATCH;
		    நிறுத்து;
	நிலை '>':   எனில் (p[1] != '=')
		    {
			type = கோவை_GREATER;
			*len = 1;
		    }
		    இல்லையெனில்
			type = கோவை_GEQUAL;
		    நிறுத்து;
	நிலை '<':   எனில் (p[1] != '=')
		    {
			type = கோவை_SMALLER;
			*len = 1;
		    }
		    இல்லையெனில்
			type = கோவை_SEQUAL;
		    நிறுத்து;
	நிலை 'i':   எனில் (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			எனில் (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			எனில் (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? கோவை_IS : கோவை_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    நிறுத்து;
    }
    திரும்பு type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    வெறுமை
skip_கோவை_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_கோவை(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, வெற்று);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    முழு
bool_on_அடுக்கு(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_அடுக்கு(cctx, 0);
    எனில் (type == &t_bool)
	திரும்பு OK;

    எனில் (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_எண்
	    || type == &t_எண்_bool
	    || type == &t_const_எண்_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	திரும்பு generate_COND2BOOL(cctx);

    திரும்பு need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    வெறுமை
error_white_both(char_u *op, முழு len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    நிலையான முழு
compile_leader(cctx_T *cctx, முழு numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    போது (p > start)
    {
	--p;
	போது (VIM_ISWHITE(*p))
	    --p;
	எனில் (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_அடுக்கு(cctx, 0);
	    எனில் (type->tt_type != VAR_FLOAT && need_type(type, &t_எண்,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		திரும்பு FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    எனில் (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == வெற்று)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (numeric_only)
	{
	    ++p;
	    நிறுத்து;
	}
	இல்லையெனில்
	{
	    முழு  invert = *p == '!';

	    போது (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		எனில் (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    எனில் (check_type_is_மதிப்பு(get_type_on_அடுக்கு(cctx, 0)) == FAIL)
		திரும்பு FAIL;
	    எனில் (generate_2BOOL(cctx, invert, -1) == FAIL)
		திரும்பு FAIL;
	}
    }
    *end = p;
    திரும்பு OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    நிலையான முழு
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    முழு	    ret;
    char_u  *p = *arg + 1;

    எனில் (may_get_next_வரி_error(p, arg, cctx) == FAIL)
	திரும்பு FAIL;

    எனில் (**arg == ')')
	// empty tuple
	திரும்பு compile_tuple(arg, cctx, ppconst, FALSE);

    எனில் (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    இல்லையெனில்
    {
	// Not enough space in ppconst, flush constants.
	எனில் (generate_ppconst(cctx, ppconst) == FAIL)
	    திரும்பு FAIL;
	ret = compile_expr0(arg, cctx);
    }
    எனில் (may_get_next_வரி_error(*arg, arg, cctx) == FAIL)
	திரும்பு FAIL;
    எனில் (ret == OK && **arg == ',')
    {
	// tuple
	முழு is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	எனில் (generate_ppconst(cctx, ppconst) == FAIL)
	    திரும்பு FAIL;
	திரும்பு compile_tuple(arg, cctx, ppconst, is_const);
    }

    எனில் (**arg == ')')
	++*arg;
    இல்லையெனில் எனில் (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    திரும்பு ret;
}

நிலையான முழு compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    நிலையான முழு
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    முழு		keeping_dict = FALSE;

    க்கு (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	எனில் (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_வரி_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    எனில் (next != வெற்று &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_வரி_from_context(cctx, TRUE);
		எனில் (next == வெற்று)
		    திரும்பு FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	எனில் (**arg == '(')
	{
	    முழு		argcount = 0;

	    எனில் (generate_ppconst(cctx, ppconst) == FAIL)
		திரும்பு FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_அடுக்கு(cctx, 0);

	    *arg = skipwhite(p + 1);
	    எனில் (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		திரும்பு FAIL;
	    எனில் (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		திரும்பு FAIL;
	    எனில் (keeping_dict)
	    {
		keeping_dict = FALSE;
		எனில் (generate_instr(cctx, ISN_CLEARDICT) == வெற்று)
		    திரும்பு FAIL;
	    }
	}
	இல்லையெனில் எனில் (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    முழு	    alt;
	    char_u  *paren;

	    // something->method()
	    எனில் (generate_ppconst(cctx, ppconst) == FAIL)
		திரும்பு FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_அடுக்கு(cctx, 0);
	    எனில் (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_மதிப்பு));
		திரும்பு FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    எனில் (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		திரும்பு FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    எனில் (**arg == '(')
		alt = 2;
	    இல்லையெனில்
	    {
		// alternative 1 or 3
		p = *arg;
		எனில் (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    திரும்பு FAIL;
		}
		எனில் (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		க்கு ( ; eval_isnamec(*p); ++p)
		    ;
		எனில் (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    எனில் (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			திரும்பு FAIL;
		}
		இல்லையெனில்
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    எனில் (paren == வெற்று)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			திரும்பு FAIL;
		    }
		}
	    }

	    எனில் (alt != 1)
	    {
		முழு	    argcount = 1;
		garray_T    *அடுக்கு = &cctx->ctx_type_அடுக்கு;
		முழு	    type_idx_start = அடுக்கு->ga_len;
		முழு	    கோவை_isn_start = cctx->ctx_instr.ga_len;
		முழு	    கோவை_isn_end;
		முழு	    arg_isn_மொத்தம்;

		எனில் (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    எனில் (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			திரும்பு FAIL;
		}
		இல்லையெனில்
		{
		    முழு fail;
		    முழு save_len = cctx->ctx_ufunc->uf_வரிகள்.ga_len;
		    முழு	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_கோவை;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_வரிகள்.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_கோவை;
		    cctx->ctx_ufunc->uf_வரிகள்.ga_len = save_len;

		    எனில் (fail)
		    {
			எனில் (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			திரும்பு FAIL;
		    }
		}

		// Compile the arguments.
		எனில் (**arg != '(')
		{
		    எனில் (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    இல்லையெனில்
			semsg(_(e_missing_parenthesis_str), *arg);
		    திரும்பு FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		கோவை_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		எனில் (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    திரும்பு FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_மொத்தம் = cctx->ctx_instr.ga_len - கோவை_isn_end;
		எனில் (arg_isn_மொத்தம் > 0)
		{
		    முழு	    கோவை_isn_மொத்தம் = கோவை_isn_end - கோவை_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, கோவை_isn_மொத்தம்);
		    type_T  *decl_type;
		    type2_T  *typep;

		    எனில் (isn == வெற்று)
			திரும்பு FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + கோவை_isn_start,
					       அளவு(isn_T) * கோவை_isn_மொத்தம்);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + கோவை_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + கோவை_isn_end,
						அளவு(isn_T) * arg_isn_மொத்தம்);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + கோவை_isn_start + arg_isn_மொத்தம்,
					  isn, அளவு(isn_T) * கோவை_isn_மொத்தம்);
		    vim_free(isn);

		    typep = ((type2_T *)அடுக்கு->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)அடுக்கு->ga_data) + type_idx_start,
			      ((type2_T *)அடுக்கு->ga_data) + type_idx_start + 1,
			      அளவு(type2_T)
				       * (அடுக்கு->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)அடுக்கு->ga_data) + அடுக்கு->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_அடுக்கு(cctx, 0);
		எனில் (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    திரும்பு FAIL;
	    }

	    எனில் (keeping_dict)
	    {
		keeping_dict = FALSE;
		எனில் (generate_instr(cctx, ISN_CLEARDICT) == வெற்று)
		    திரும்பு FAIL;
	    }
	}
	இல்லையெனில் எனில் (**arg == '[')
	{
	    முழு		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    எனில் (generate_ppconst(cctx, ppconst) == FAIL)
		திரும்பு FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    எனில் (may_get_next_வரி_error(p, arg, cctx) == FAIL)
		திரும்பு FAIL;
	    எனில் (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    இல்லையெனில்
	    {
		எனில் (compile_expr0(arg, cctx) == FAIL)
		    திரும்பு FAIL;
		எனில் (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    திரும்பு FAIL;
		}
		எனில் (may_get_next_வரி_error(*arg, arg, cctx) == FAIL)
		    திரும்பு FAIL;
		*arg = skipwhite(*arg);
	    }
	    எனில் (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		எனில் (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    திரும்பு FAIL;
		}
		எனில் (may_get_next_வரி_error(*arg, arg, cctx) == FAIL)
		    திரும்பு FAIL;
		எனில் (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		இல்லையெனில்
		{
		    எனில் (compile_expr0(arg, cctx) == FAIL)
			திரும்பு FAIL;
		    எனில் (may_get_next_வரி_error(*arg, arg, cctx) == FAIL)
			திரும்பு FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    எனில் (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		திரும்பு FAIL;
	    }
	    *arg = *arg + 1;

	    எனில் (keeping_dict)
	    {
		keeping_dict = FALSE;
		எனில் (generate_instr(cctx, ISN_CLEARDICT) == வெற்று)
		    திரும்பு FAIL;
	    }
	    எனில் (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    எனில் (generate_ppconst(cctx, ppconst) == FAIL)
		திரும்பு FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_அடுக்கு(cctx, 0);
	    எனில் (type != &t_unknown
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
		எனில் (compile_class_object_index(cctx, arg, type) == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில்
	    {
		*arg = p + 1;
		எனில் (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    திரும்பு FAIL;
		}
		p = *arg;
		போது (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		எனில் (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    திரும்பு FAIL;
		}
		எனில் (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == வெற்று)
		    திரும்பு FAIL;
		எனில் (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    திரும்பு FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	இல்லையெனில்
	    நிறுத்து;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    எனில் (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == வெற்று)
	திரும்பு FAIL;

    திரும்பு OK;
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
    நிலையான முழு
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    முழு		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    முழு		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    எனில் (eval_leader(arg, TRUE) == FAIL)
	திரும்பு FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    தேர்வு (**arg)
    {
	/*
	 * Number constant.
	 */
	நிலை '0':	// also for blob starting with 0z
	நிலை '1':
	நிலை '2':
	நிலை '3':
	நிலை '4':
	நிலை '5':
	நிலை '6':
	நிலை '7':
	நிலை '8':
	நிலை '9':
	நிலை '.':   எனில் (eval_எண்(arg, rettv, TRUE, FALSE) == FAIL)
			திரும்பு FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    எனில் (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			திரும்பு FAIL;
		    }
		    நிறுத்து;

	/*
	 * String constant: "string".
	 */
	நிலை '"':   எனில் (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			திரும்பு FAIL;
		    நிறுத்து;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	நிலை '\'':  எனில் (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			திரும்பு FAIL;
		    நிறுத்து;

	/*
	 * Constant Vim variable.
	 */
	நிலை 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    நிறுத்து;

	/*
	 * "true" constant
	 */
	நிலை 't':   எனில் (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_எண் = VVAL_TRUE;
		    }
		    இல்லையெனில்
			ret = NOTDONE;
		    நிறுத்து;

	/*
	 * "false" constant
	 */
	நிலை 'f':   எனில் (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_எண் = VVAL_FALSE;
		    }
		    இல்லையெனில்
			ret = NOTDONE;
		    நிறுத்து;

	/*
	 * "null" or "null_*" constant
	 */
	நிலை 'n':   எனில் (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			முழு	len;

			க்கு (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			எனில் (ret == FAIL)
			    ret = NOTDONE;
			இல்லையெனில்
			    *arg += len + 4;
		    }
		    இல்லையெனில்
			ret = NOTDONE;
		    நிறுத்து;

	/*
	 * List: [expr, expr]
	 */
	நிலை '[':   எனில் (generate_ppconst(cctx, ppconst) == FAIL)
			திரும்பு FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    நிறுத்து;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	நிலை '{':   எனில் (generate_ppconst(cctx, ppconst) == FAIL)
			திரும்பு FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    நிறுத்து;

	/*
	 * Option value: &name
	 */
	நிலை '&':	எனில் (generate_ppconst(cctx, ppconst) == FAIL)
			    திரும்பு FAIL;
			ret = compile_get_option(arg, cctx);
			நிறுத்து;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	நிலை '$':	எனில் (generate_ppconst(cctx, ppconst) == FAIL)
			    திரும்பு FAIL;
			எனில் ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			இல்லையெனில்
			    ret = compile_get_env(arg, cctx);
			நிறுத்து;

	/*
	 * Register contents: @r.
	 */
	நிலை '@':	எனில் (generate_ppconst(cctx, ppconst) == FAIL)
			    திரும்பு FAIL;
			ret = compile_get_register(arg, cctx);
			நிறுத்து;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	நிலை '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    எனில் (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    நிறுத்து;

	இயல்பு:    ret = NOTDONE;
		    நிறுத்து;
    }
    எனில் (ret == FAIL)
	திரும்பு FAIL;

    எனில் (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	எனில் (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	இல்லையெனில்
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    இல்லையெனில் எனில் (ret == NOTDONE)
    {
	char_u	    *p;
	முழு	    r;

	எனில் (!eval_isnamec1(**arg))
	{
	    எனில் (!vim9_bad_comment(*arg))
	    {
		எனில் (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		இல்லையெனில்
		    semsg(_(e_name_expected_str), *arg);
	    }
	    திரும்பு FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	எனில் (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    திரும்பு FAIL;
	}

	size_t namelen = p - *arg;
	எனில் (*p == '<')
	{
	    எனில் (skip_generic_func_type_args(&p) == FAIL)
		திரும்பு FAIL;
	}

	எனில் (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	இல்லையெனில்
	{
	    எனில் (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		திரும்பு FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	எனில் (r == FAIL)
	    திரும்பு FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    எனில் (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	திரும்பு FAIL;
    எனில் ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	எனில் (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    திரும்பு FAIL;
	திரும்பு OK;
    }
    எனில் (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    நிலையான முழு
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = வெற்று;

    // Recognize <type>
    எனில் (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	எனில் (want_type == வெற்று)
	    திரும்பு FAIL;

	எனில் (**arg != '>')
	{
	    எனில் (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    இல்லையெனில்
		emsg(_(e_missing_gt));
	    திரும்பு FAIL;
	}
	++*arg;
	எனில் (may_get_next_வரி_error(*arg, arg, cctx) == FAIL)
	    திரும்பு FAIL;
    }

    எனில் (compile_expr9(arg, cctx, ppconst) == FAIL)
	திரும்பு FAIL;

    எனில் (want_type != வெற்று)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_அடுக்கு(cctx, 0);
	எனில் (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    எனில் (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		திரும்பு FAIL;
	}
    }

    திரும்பு OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    நிலையான முழு
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    முழு		ppconst_used = ppconst->pp_used;

    // get the first expression
    எனில் (compile_expr8(arg, cctx, ppconst) == FAIL)
	திரும்பு FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    க்கு (;;)
    {
	op = may_peek_next_வரி(cctx, *arg, &next);
	எனில் (*op != '*' && *op != '/' && *op != '%')
	    நிறுத்து;
	எனில் (next != வெற்று)
	{
	    *arg = next_வரி_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	எனில் (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    திரும்பு FAIL;
	}
	எனில் (may_get_next_வரி_error(op + 1, arg, cctx) == FAIL)
	    திரும்பு FAIL;

	// get the second expression
	எனில் (compile_expr8(arg, cctx, ppconst) == FAIL)
	    திரும்பு FAIL;

	எனில் (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_எண்
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_எண்)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    முழு		    failed = FALSE;

	    // both are numbers: compute the result
	    தேர்வு (*op)
	    {
		நிலை '*': res = tv1->vval.v_எண் * tv2->vval.v_எண்;
			  நிறுத்து;
		நிலை '/': res = இலக்கம்_divide(tv1->vval.v_எண்,
						  tv2->vval.v_எண், &failed);
			  நிறுத்து;
		நிலை '%': res = இலக்கம்_modulus(tv1->vval.v_எண்,
						  tv2->vval.v_எண், &failed);
			  நிறுத்து;
	    }
	    எனில் (failed)
		திரும்பு FAIL;
	    tv1->vval.v_எண் = res;
	    --ppconst->pp_used;
	}
	இல்லையெனில்
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    திரும்பு OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    நிலையான முழு
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    முழு		oplen;
    முழு		ppconst_used = ppconst->pp_used;

    // get the first variable
    எனில் (compile_expr7(arg, cctx, ppconst) == FAIL)
	திரும்பு FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    க்கு (;;)
    {
	op = may_peek_next_வரி(cctx, *arg, &next);
	எனில் (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    நிறுத்து;
	எனில் (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    நிறுத்து;
	oplen = (*op == '.' ? 2 : 1);
	எனில் (next != வெற்று)
	{
	    *arg = next_வரி_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	எனில் (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    திரும்பு FAIL;
	}

	எனில் (may_get_next_வரி_error(op + oplen, arg, cctx) == FAIL)
	    திரும்பு FAIL;

	// get the second expression
	எனில் (compile_expr7(arg, cctx, ppconst) == FAIL)
	    திரும்பு FAIL;

	எனில் (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_எண்
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_எண்)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    எனில் (*op == '+')
		tv1->vval.v_எண் = tv1->vval.v_எண் + tv2->vval.v_எண்;
	    இல்லையெனில் எனில் (*op == '-')
		tv1->vval.v_எண் = tv1->vval.v_எண் - tv2->vval.v_எண்;
	    இல்லையெனில்
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		எனில் (tv1->vval.v_string == வெற்று)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    திரும்பு FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	இல்லையெனில்
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    எனில் (*op == '.')
	    {
		எனில் (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    திரும்பு FAIL;
		எனில் (generate_CONCAT(cctx, 2) == FAIL)
		    திரும்பு FAIL;
	    }
	    இல்லையெனில்
		generate_two_op(cctx, op);
	}
    }

    திரும்பு OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    நிலையான முழு
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = கோவை_UNKNOWN;
    char_u	*p;
    char_u	*next;
    முழு		len = 2;
    முழு		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    எனில் (compile_expr6(arg, cctx, ppconst) == FAIL)
	திரும்பு FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    க்கு (;;)
    {
	type = கோவை_UNKNOWN;

	p = may_peek_next_வரி(cctx, *arg, &next);
	எனில் (p[0] == '<' && p[1] == '<')
	    type = கோவை_LSHIFT;
	இல்லையெனில் எனில் (p[0] == '>' && p[1] == '>')
	    type = கோவை_RSHIFT;

	எனில் (type == கோவை_UNKNOWN)
	    திரும்பு OK;

	// Handle a bitwise left or right shift operator
	எனில் (ppconst->pp_used == ppconst_used + 1)
	{
	    எனில் (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_எண்)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_எண்));
		திரும்பு FAIL;
	    }
	}
	இல்லையெனில்
	{
	    type_T	*t = get_type_on_அடுக்கு(cctx, 0);

	    எனில் (need_type(t, &t_எண், 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_எண்));
		திரும்பு FAIL;
	    }
	}

	எனில் (next != வெற்று)
	{
	    *arg = next_வரி_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	எனில் (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    திரும்பு FAIL;
	}

	// get the second variable
	எனில் (may_get_next_வரி_error(p + len, arg, cctx) == FAIL)
	    திரும்பு FAIL;

	எனில் (compile_expr6(arg, cctx, ppconst) == FAIL)
	    திரும்பு FAIL;

	எனில் (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    எனில் (tv2->v_type != VAR_எண் || tv2->vval.v_எண் < 0)
	    {
		// right operand should be a positive number
		எனில் (tv2->v_type != VAR_எண்)
		    emsg(_(e_bitshift_ops_must_be_எண்));
		இல்லையெனில்
		    emsg(_(e_bitshift_ops_must_be_positive));
		திரும்பு FAIL;
	    }

	    எனில் (tv2->vval.v_எண் > MAX_LSHIFT_BITS)
		tv1->vval.v_எண் = 0;
	    இல்லையெனில் எனில் (type == கோவை_LSHIFT)
		tv1->vval.v_எண் =
			(uvarnumber_T)tv1->vval.v_எண் << tv2->vval.v_எண்;
	    இல்லையெனில்
		tv1->vval.v_எண் =
			(uvarnumber_T)tv1->vval.v_எண் >> tv2->vval.v_எண்;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	இல்லையெனில்
	{
	    எனில் (need_type(get_type_on_அடுக்கு(cctx, 0), &t_எண், 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_எண்));
		திரும்பு FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    எனில் (isn == வெற்று)
		திரும்பு FAIL;

	    எனில் (isn != வெற்று)
		isn->isn_arg.op.op_type = type;
	}
    }

    திரும்பு OK;
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
    நிலையான முழு
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = கோவை_UNKNOWN;
    char_u	*p;
    char_u	*next;
    முழு		len = 2;
    முழு		type_is = FALSE;
    முழு		ppconst_used = ppconst->pp_used;

    // get the first variable
    எனில் (compile_expr5(arg, cctx, ppconst) == FAIL)
	திரும்பு FAIL;

    p = may_peek_next_வரி(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    எனில் (type != கோவை_UNKNOWN)
    {
	முழு ic = FALSE;  // Default: do not ignore case

	எனில் (next != வெற்று)
	{
	    *arg = next_வரி_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	எனில் (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    திரும்பு FAIL;
	}
	// extra question mark appended: ignore case
	எனில் (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	இல்லையெனில் எனில் (p[len] == '#')
	    ++len;
	// nothing appended: match case

	எனில் (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    திரும்பு FAIL;
	}

	// get the second variable
	எனில் (may_get_next_வரி_error(p + len, arg, cctx) == FAIL)
	    திரும்பு FAIL;

	எனில் (compile_expr5(arg, cctx, ppconst) == FAIL)
	    திரும்பு FAIL;

	எனில் (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    முழு		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    எனில் (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    இல்லையெனில்
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_எண் = tv1->vval.v_எண்
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    திரும்பு ret;
	}

	generate_ppconst(cctx, ppconst);
	திரும்பு generate_COMPARE(cctx, type, ic);
    }

    திரும்பு OK;
}

நிலையான முழு compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    நிலையான முழு
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	எழுத்து	*op,
	ppconst_T *ppconst,
	முழு	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_வரி(cctx, *arg, &next);
    முழு		opchar = *op;

    எனில் (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	முழு		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, அளவு(முழு), 10);
	போது (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    முழு		start_ctx_lnum = cctx->ctx_lnum;
	    முழு		save_lnum;
	    முழு		const_used;
	    முழு		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    எனில் (next != வெற்று)
	    {
		*arg = next_வரி_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    எனில் (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		திரும்பு FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    எனில் (status != FAIL)
	    {
		// Use the last ppconst if possible.
		எனில் (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    முழு		is_true = tv2bool(tv);

		    எனில் ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_எண் = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    இல்லையெனில்
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		இல்லையெனில்
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_அடுக்கு(cctx);
		}
	    }
	    எனில் (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    எனில் (status == FAIL)
	    {
		ga_clear(&end_ga);
		திரும்பு FAIL;
	    }

	    எனில் (jump_when != JUMP_NEVER)
	    {
		எனில் (cctx->ctx_skip != SKIP_YES)
		{
		    *(((முழு *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    எனில் (may_get_next_வரி_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		திரும்பு FAIL;
	    }

	    const_used = ppconst->pp_used;
	    எனில் ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		திரும்பு FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    எனில் (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		எனில் (tv->v_type == VAR_எண்
			 && (tv->vval.v_எண் == 1 || tv->vval.v_எண் == 0))
		{
		    tv->vval.v_எண் = tv->vval.v_எண் == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_வரி(cctx, *arg, &next);
	}

	எனில் (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    திரும்பு FAIL;
	}

	எனில் (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    எனில் (bool_on_அடுக்கு(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		திரும்பு FAIL;
	    }

	எனில் (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    போது (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((முழு *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    திரும்பு OK;
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
    நிலையான முழு
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    முழு		ppconst_used = ppconst->pp_used;

    // get the first variable
    எனில் (compile_expr4(arg, cctx, ppconst) == FAIL)
	திரும்பு FAIL;

    // || and && work almost the same
    திரும்பு compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    நிலையான முழு
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    முழு		ppconst_used = ppconst->pp_used;

    // eval the first expression
    எனில் (compile_expr3(arg, cctx, ppconst) == FAIL)
	திரும்பு FAIL;

    // || and && work almost the same
    திரும்பு compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    முழு
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    முழு		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    எனில் (cctx->ctx_skip == SKIP_YES)
    {
	முழு		prev_did_emsg = did_emsg;

	skip_கோவை_cctx(arg, cctx);
	திரும்பு did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    எனில் (compile_expr2(arg, cctx, ppconst) == FAIL)
	திரும்பு FAIL;

    p = may_peek_next_வரி(cctx, *arg, &next);
    எனில் (*p == '?')
    {
	முழு		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;
	முழு		alt_idx = instr->ga_len;
	முழு		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = வெற்று;
	முழு		has_const_கோவை = FALSE;
	முழு		const_மதிப்பு = FALSE;
	முழு		save_skip = cctx->ctx_skip;

	எனில் (next != வெற்று)
	{
	    *arg = next_வரி_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	எனில் (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    திரும்பு FAIL;
	}

	எனில் (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_கோவை = TRUE;
	    எனில் (op_falsy)
		const_மதிப்பு = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    இல்லையெனில்
	    {
		முழு error = FALSE;

		const_மதிப்பு = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		எனில் (error)
		    திரும்பு FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_மதிப்பு : !const_மதிப்பு) ? SKIP_YES : SKIP_NOT;

	    எனில் (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    இல்லையெனில்
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	இல்லையெனில்
	{
	    generate_ppconst(cctx, ppconst);
	    எனில் (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    எனில் (op_falsy)
	    {
		type1 = get_type_on_அடுக்கு(cctx, -1);
		எனில் (check_type_is_மதிப்பு(type1) == FAIL)
		    திரும்பு FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	எனில் (may_get_next_வரி_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    திரும்பு FAIL;
	எனில் (compile_expr1(arg, cctx, ppconst) == FAIL)
	    திரும்பு FAIL;

	எனில் (!has_const_கோவை)
	{
	    generate_ppconst(cctx, ppconst);

	    எனில் (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_அடுக்கு(cctx, 0);
		--அடுக்கு->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	எனில் (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_வரி(cctx, *arg, &next);
	    எனில் (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		திரும்பு FAIL;
	    }
	    எனில் (next != வெற்று)
	    {
		*arg = next_வரி_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    எனில் (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		திரும்பு FAIL;
	    }

	    // evaluate the third expression
	    எனில் (has_const_கோவை)
		cctx->ctx_skip = save_skip == SKIP_YES || const_மதிப்பு
							 ? SKIP_YES : SKIP_NOT;
	    எனில் (may_get_next_வரி_error(p + 1, arg, cctx) == FAIL)
		திரும்பு FAIL;
	    எனில் (compile_expr1(arg, cctx, ppconst) == FAIL)
		திரும்பு FAIL;
	}

	எனில் (!has_const_கோவை)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)அடுக்கு->ga_data)
					      + அடுக்கு->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    திரும்பு OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    முழு
compile_expr0_ext(char_u **arg,  cctx_T *cctx, முழு *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    எனில் (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	திரும்பு FAIL;
    }
    எனில் (is_const != வெற்று)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    எனில் (generate_ppconst(cctx, &ppconst) == FAIL)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Toplevel expression.
 */
    முழு
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    திரும்பு compile_expr0_ext(arg, cctx, வெற்று);
}


#endif // defined(FEAT_EVAL)
