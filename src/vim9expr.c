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
#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
স্থির পূর্ণাঙ্ক paren_follows_after_ৰাশিমালা = 0;

/*
 * Generate code for any ppconst entries.
 */
    পূর্ণাঙ্ক
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    পূর্ণাঙ্ক	    i;
    পূর্ণাঙ্ক	    ret = OK;
    পূর্ণাঙ্ক	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    বাবে (i = 0; i < ppconst->pp_used; ++i)
	যদি (generate_tv_ঠেলো(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    ঘূৰাই ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    স্থির পূর্ণাঙ্ক
check_ppconst_bool(ppconst_T *ppconst)
{
    যদি (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	ঘূৰাই check_typval_type(&t_bool, tv, where);
    }
    ঘূৰাই OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    শূন্য
clear_ppconst(ppconst_T *ppconst)
{
    পূর্ণাঙ্ক	    i;

    বাবে (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    স্থির পূর্ণাঙ্ক
compile_tuple_member(
    type2_T	*typep,
    পূর্ণাঙ্ক		is_slice,
    cctx_T	*cctx)
{
    যদি (is_slice)
    {
	যদি (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    ঘূৰাই FAIL;
	// a copy is made so the member type is no longer declared
	যদি (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	যদি (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    যদি (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    অন্যথা
    {
	যদি (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    যদি (typep->type_curr->tt_argcount == 1)
	    {
		যদি (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		অন্যথা
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    অন্যথা
		typep->type_curr = &t_any;
	    যদি (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		যদি (typep->type_decl->tt_argcount == 1)
		{
		    যদি (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    অন্যথা
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		অন্যথা
		    typep->type_curr = &t_any;
	    }
	    অন্যথা
		typep->type_decl = typep->type_curr;
	}
	যদি (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    পূর্ণাঙ্ক
compile_member(পূর্ণাঙ্ক is_slice, পূর্ণাঙ্ক *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)স্তূপ->ga_data) + স্তূপ->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)স্তূপ->ga_data) + স্তূপ->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    যদি ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    যদি (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	যদি (need_type(idxtype, &t_সংখ্যা, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ঘূৰাই FAIL;
	যদি (is_slice)
	{
	    idxtype = get_type_on_স্তূপ(cctx, 1);
	    যদি (need_type(idxtype, &t_সংখ্যা, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ঘূৰাই FAIL;
	}
    }

    যদি (vartype == VAR_DICT)
    {
	যদি (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    ঘূৰাই FAIL;
	}
	যদি (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    যদি (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    যদি (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		যদি (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    অন্যথা
		typep->type_decl = typep->type_curr;
	}
	অন্যথা
	{
	    যদি (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ঘূৰাই FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	যদি (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    ঘূৰাই FAIL;
	যদি (keeping_dict != রিক্ত)
	    *keeping_dict = TRUE;
    }
    অন্যথা যদি (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	যদি ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    ঘূৰাই FAIL;
    }
    অন্যথা যদি (vartype == VAR_BLOB)
    {
	যদি (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    যদি (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		ঘূৰাই FAIL;
	}
	অন্যথা
	{
	    typep->type_curr = &t_সংখ্যা;
	    typep->type_decl = &t_সংখ্যা;
	    যদি (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		ঘূৰাই FAIL;
	}
    }
    অন্যথা যদি (vartype == VAR_TUPLE)
    {
	যদি (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    ঘূৰাই FAIL;
    }
    অন্যথা যদি (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	যদি (is_slice)
	{
	    যদি (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		ঘূৰাই FAIL;
	    // a copy is made so the member type is no longer declared
	    যদি (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    যদি (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		যদি (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	অন্যথা
	{
	    যদি (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		যদি (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		যদি (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    যদি (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		অন্যথা
			typep->type_decl = typep->type_curr;
	    }
	    যদি (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		ঘূৰাই FAIL;
	}
    }
    অন্যথা
    {
	ৱিকল্প (vartype)
	{
	    পক্ষ VAR_FUNC:
	    পক্ষ VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		ভঙ্গ;
	    পক্ষ VAR_BOOL:
	    পক্ষ VAR_SPECIAL:
	    পক্ষ VAR_JOB:
	    পক্ষ VAR_CHANNEL:
	    পক্ষ VAR_INSTR:
	    পক্ষ VAR_CLASS:
	    পক্ষ VAR_OBJECT:
	    পক্ষ VAR_TYPEALIAS:
	    পক্ষ VAR_UNKNOWN:
	    পক্ষ VAR_ANY:
	    পক্ষ VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		ভঙ্গ;
	    স্ৱভাৱ:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    স্থির পূর্ণাঙ্ক
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    বাবে (cctx_T *cctx = cctx_arg; cctx != রিক্ত; cctx = cctx->ctx_outer)
    {
	যদি (cctx->ctx_ufunc != রিক্ত && cctx->ctx_ufunc->uf_class != রিক্ত)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    যেতিয়ালৈ (clp != রিক্ত)
	    {
		যদি (clp == cl)
		    ঘূৰাই TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    ঘূৰাই FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    পূর্ণাঙ্ক
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    পূর্ণাঙ্ক		m_idx;
    পূর্ণাঙ্ক		ret = FAIL;

    যদি (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ঘূৰাই FAIL;
    }

    class_T *cl = type->tt_class;
    পূর্ণাঙ্ক is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    যদি (type == &t_super)
    {
	যদি (cctx->ctx_ufunc == রিক্ত || cctx->ctx_ufunc->uf_class == রিক্ত)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    ঘূৰাই FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_স্তূপ.ga_len;
    }
    অন্যথা যদি (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	যদি (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    যদি (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    যদি (cl == রিক্ত)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	ঘূৰাই FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, রিক্ত, রিক্ত, FNE_CHECK_START);
    যদি (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	ঘূৰাই FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    যদি (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	যদি (name_end == রিক্ত)
	    গচ্ছ done;
    }

    যদি (*name_end == '(')
    {
	পূর্ণাঙ্ক	function_গুনতি;
	পূর্ণাঙ্ক	child_গুনতি;
	ufunc_T	**functions;

	যদি (type->tt_type == VAR_CLASS)
	{
	    function_গুনতি = cl->class_class_function_গুনতি;
	    child_গুনতি = cl->class_class_function_গুনতি_child;
	    functions = cl->class_class_functions;
	}
	অন্যথা
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    যদি (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_গুনতি = cl->class_obj_method_গুনতি;
	    child_গুনতি = cl->class_obj_method_গুনতি_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = রিক্ত;
	পূর্ণাঙ্ক fi;
	বাবে (fi = is_super ? child_গুনতি : 0; fi < function_গুনতি; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    যদি (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		ভঙ্গ;
	    }
	}

	ocmember_T  *ocm = রিক্ত;
	যদি (ufunc == রিক্ত)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    যদি (ocm == রিক্ত || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_বাৰ্তা(cl, type->tt_type, name, len);
		গচ্ছ done;
	    }
	    যদি (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_স্তূপ.ga_len;
		যদি (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    গচ্ছ done;
	    }
	    অন্যথা
	    {
		পূর্ণাঙ্ক status;

		যদি (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		অন্যথা
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		যদি (status == FAIL)
		    গচ্ছ done;
	    }
	}

	যদি (is_super && ufunc != রিক্ত && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    গচ্ছ done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	যদি (ocm == রিক্ত && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    গচ্ছ done;
	}

	// process generic function call
	যদি (ufunc != রিক্ত)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    যদি (ufunc == রিক্ত)
		গচ্ছ done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	পূর্ণাঙ্ক argcount = 0;
	যদি (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    গচ্ছ done;

	যদি (ocm != রিক্ত)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    গচ্ছ done;
	}
	যদি (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	অন্যথা
	    ret = generate_CALL(cctx, ufunc, রিক্ত, 0, argcount, FALSE);
	গচ্ছ done;
    }

    যদি (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	যদি (m_idx >= 0)
	{
	    যদি (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		গচ্ছ done;
	    }

	    *arg = name_end;
	    যদি (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    অন্যথা
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    গচ্ছ done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	যদি (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    যদি (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		গচ্ছ done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_স্তূপ.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, রিক্ত);
	    গচ্ছ done;
	}

	member_not_found_বাৰ্তা(cl, VAR_OBJECT, name, len);
    }
    অন্যথা
    {
	// load class member
	পূর্ণাঙ্ক idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	যদি (m != রিক্ত)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    যদি (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		গচ্ছ done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_স্তূপ.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    গচ্ছ done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	যদি (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    যদি (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		গচ্ছ done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_স্তূপ.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, রিক্ত);
	    গচ্ছ done;
	}

	member_not_found_বাৰ্তা(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    ঘূৰাই ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    পূর্ণাঙ্ক
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    পূর্ণাঙ্ক		    idx;
    imported_T	    *imp;

    যদি (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ঘূৰাই FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, রিক্ত);
    যদি (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	ঘূৰাই OK;
    }

    যদি (end == রিক্ত)
	imp = রিক্ত;
    অন্যথা যদি (import == রিক্ত)
	imp = find_imported(name, 0, FALSE);
    অন্যথা
	imp = import;

    যদি (imp != রিক্ত)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	পূর্ণাঙ্ক	cc;
	ufunc_T	*ufunc = রিক্ত;
	type_T	*type;
	পূর্ণাঙ্ক	done = FALSE;
	পূর্ণাঙ্ক	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	যদি (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    ঘূৰাই FAIL;
	}
	++p;
	যদি (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    ঘূৰাই FAIL;
	}

	// isolate one name
	exp_name = p;
	যেতিয়ালৈ (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	যদি (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	যদি (res == OK)
	{
	    যদি (si->sn_autoload_prefix != রিক্ত
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		যদি (cc == '(' || paren_follows_after_ৰাশিমালা)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		অন্যথা
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    অন্যথা যদি (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		যদি (cc == '(' || paren_follows_after_ৰাশিমালা)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		অন্যথা
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    অন্যথা
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, রিক্ত, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	যদি (done)
	    ঘূৰাই res;

	যদি (idx < 0)
	{
	    যদি (ufunc != রিক্ত)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		যদি (IS_GENERIC_FUNC(ufunc))
		{
		    যদি (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			যদি (p != রিক্ত)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			যদি (p == রিক্ত || ufunc == রিক্ত)
			    ঘূৰাই FAIL;
		    }
		    অন্যথা
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			ঘূৰাই FAIL;
		    }
		}
		অন্যথা যদি (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    ঘূৰাই FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, রিক্ত, TRUE);
		ঘূৰাই OK;
	    }
	    ঘূৰাই FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	ঘূৰাই OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    ঘূৰাই generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    স্থির পূর্ণাঙ্ক
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    পূর্ণাঙ্ক			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    যদি (ufunc == রিক্ত || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	ঘূৰাই FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    যদি (ufunc == রিক্ত)
	ঘূৰাই FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    যদি (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, রিক্ত) == FAIL)
	ঘূৰাই FAIL;
    ঘূৰাই generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    স্থির পূর্ণাঙ্ক
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    ঘূৰাই cctx->ctx_ufunc != রিক্ত
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == রিক্ত
		|| cctx->ctx_outer->ctx_ufunc == রিক্ত
		|| cctx->ctx_outer->ctx_ufunc->uf_class == রিক্ত
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
    স্থির পূর্ণাঙ্ক
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    পূর্ণাঙ্ক			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    পূর্ণাঙ্ক		isn_idx;

    যদি (fp == রিক্ত)
	ঘূৰাই FAIL;

    বাবে (পূর্ণাঙ্ক depth = 0; depth < outer_depth; ++depth)
    {
	যদি (outer_cctx->ctx_outer == রিক্ত)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    ঘূৰাই FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    যদি (fp->uf_block_ids == রিক্ত && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(পূর্ণাঙ্ক, ufunc->uf_block_depth);
	যদি (fp->uf_block_ids != রিক্ত)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    পরিমাণ(পূর্ণাঙ্ক) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    যদি (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	ঘূৰাই FAIL;

    যদি (generate_FUNCREF(cctx, fp, রিক্ত, FALSE, 0, &isn_idx) == FAIL)
	ঘূৰাই FAIL;
    যদি (cctx->ctx_skip == SKIP_YES)
	ঘূৰাই OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    যদি (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	যদি (extra == রিক্ত)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    যদি (extra == রিক্ত)
		ঘূৰাই FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    ঘূৰাই OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    পূর্ণাঙ্ক
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	পূর্ণাঙ্ক	is_ৰাশিমালা,
	পূর্ণাঙ্ক	error)
{
    type_T	*type;
    char_u	*name = রিক্ত;
    char_u	*end = end_arg;
    পূর্ণাঙ্ক		res = FAIL;
    পূর্ণাঙ্ক		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = রিক্ত;

    generic_func_args_table_init(&gfatab);

    যদি (*(*arg + namelen) == '<')
    {
	// generic function call
	যদি (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == রিক্ত)
	{
	    generic_func_args_table_clear(&gfatab);
	    ঘূৰাই FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    যদি (*(*arg + 1) == ':')
    {
	যদি (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    ৱিকল্প (**arg)
	    {
		পক্ষ 'g': isn_type = ISN_LOADGDICT; ভঙ্গ;
		পক্ষ 'w': isn_type = ISN_LOADWDICT; ভঙ্গ;
		পক্ষ 't': isn_type = ISN_LOADTDICT; ভঙ্গ;
		পক্ষ 'b': isn_type = ISN_LOADBDICT; ভঙ্গ;
		স্ৱভাৱ:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    গচ্ছ theend;
	    }
	    যদি (generate_instr_type(cctx, isn_type, &t_dict_any) == রিক্ত)
		গচ্ছ theend;
	    res = OK;
	}
	অন্যথা
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    যদি (name == রিক্ত)
		গচ্ছ theend;

	    ৱিকল্প (**arg)
	    {
		পক্ষ 'v': res = generate_LOADV(cctx, name);
			  ভঙ্গ;
		পক্ষ 's': যদি (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      গচ্ছ theend;
			  }
			  যদি (is_ৰাশিমালা && find_func(name, FALSE) != রিক্ত)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  অন্যথা
			      res = compile_load_scriptvar(cctx, name,
							    রিক্ত, &end, রিক্ত);
			  ভঙ্গ;
		পক্ষ 'g': যদি (vim_strchr(name, AUTOLOAD_CHAR) == রিক্ত)
			  {
			      যদি (is_ৰাশিমালা && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != রিক্ত
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      অন্যথা
				  isn_type = ISN_LOADG;
			  }
			  অন্যথা
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      যদি (name == রিক্ত)
				  গচ্ছ theend;
			  }
			  ভঙ্গ;
		পক্ষ 'w': isn_type = ISN_LOADW; ভঙ্গ;
		পক্ষ 't': isn_type = ISN_LOADT; ভঙ্গ;
		পক্ষ 'b': isn_type = ISN_LOADB; ভঙ্গ;
		স্ৱভাৱ:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  গচ্ছ theend;
	    }
	    যদি (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    অন্যথা
    {
	পূর্ণাঙ্ক	    idx;
	পূর্ণাঙ্ক	    method_idx;
	পূর্ণাঙ্ক	    gen_load = FALSE;
	পূর্ণাঙ্ক	    gen_load_outer = 0;
	পূর্ণাঙ্ক	    outer_loop_depth = -1;
	পূর্ণাঙ্ক	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	যদি (name == রিক্ত)
	    গচ্ছ theend;

	যদি (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = ঠেলো_type_স্তূপ(cctx, &t_super);
	    যদি (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	অন্যথা যদি (vim_strchr(name, AUTOLOAD_CHAR) != রিক্ত)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	অন্যথা যদি (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    যদি (gen_load_outer == 0)
		gen_load = TRUE;
	}
	অন্যথা
	{
	    lvar_T  lvar;
	    class_T *cl = রিক্ত;

	    যদি (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		যদি (lvar.lv_ufunc != রিক্ত && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		অন্যথা যদি (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		অন্যথা
		    gen_load = TRUE;
	    }
	    অন্যথা যদি (cctx->ctx_ufunc->uf_defclass != রিক্ত &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		যদি (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    যদি (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    অন্যথা
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									রিক্ত);
		    }
		}
		অন্যথা
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    অন্যথা
	    {
		imported_T *imp = রিক্ত;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		যদি (script_var_exists(*arg, namelen, cctx, রিক্ত) == OK
			    || (imp = find_imported(name, 0, FALSE)) != রিক্ত)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		যদি (res == FAIL && is_ৰাশিমালা && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	যদি (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, রিক্ত, type);
	যদি (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    যদি (lt != রিক্ত)
	*lt = '<';
    যদি (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    ঘূৰাই res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    স্থির পূর্ণাঙ্ক
compile_string(isn_T *isn, cctx_T *cctx, পূর্ণাঙ্ক str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    পূর্ণাঙ্ক		ৰাশিমালা_res;
    পূর্ণাঙ্ক		trailing_error;
    পূর্ণাঙ্ক		instr_গুনতি;
    isn_T	*instr = রিক্ত;

    // Remove the string type from the stack.
    --cctx->ctx_type_স্তূপ.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = রিক্ত;

    // avoid peeking a next line
    পূর্ণাঙ্ক galen_save = cctx->ctx_ufunc->uf_সাৰিগুলি.ga_len;
    cctx->ctx_ufunc->uf_সাৰিগুলি.ga_len = 0;

    ৰাশিমালা_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_সাৰিগুলি.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    যদি (ৰাশিমালা_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	যদি (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_স্তূপ.ga_len;
	ঘূৰাই FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_গুনতি = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_গুনতি].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    ঘূৰাই OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    পূর্ণাঙ্ক
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	পূর্ণাঙ্ক	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    পূর্ণাঙ্ক	    must_end = FALSE;
    পূর্ণাঙ্ক	    instr_গুনতি;

    বাবে (;;)
    {
	যদি (may_get_next_সাৰি(whitep, &p, cctx) == FAIL)
	    গচ্ছ failret;
	যদি (*p == ')')
	{
	    *arg = p + 1;
	    ঘূৰাই OK;
	}
	যদি (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    ঘূৰাই FAIL;
	}

	instr_গুনতি = cctx->ctx_instr.ga_len;
	যদি (compile_expr0(&p, cctx) == FAIL)
	    ঘূৰাই FAIL;
	++*argcount;

	যদি (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_গুনতি + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_গুনতি;

	    // {skip} argument of searchpair() can be compiled if not empty
	    যদি (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != রিক্ত
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	অন্যথা যদি (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_গুনতি + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_গুনতি;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    যদি (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != রিক্ত
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	যদি (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ঘূৰাই FAIL;
	}
	যদি (*p == ',')
	{
	    ++p;
	    যদি (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ঘূৰাই FAIL;
	    }
	}
	অন্যথা
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    ঘূৰাই FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    স্থির পূর্ণাঙ্ক
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_স্তূপ(cctx, 0);
    পূর্ণাঙ্ক		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    যদি (type->tt_type == VAR_OBJECT)
    {
	পূর্ণাঙ্ক	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	যদি (uf != রিক্ত)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    ঘূৰাই res;
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
    স্থির পূর্ণাঙ্ক
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	পূর্ণাঙ্ক	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    পূর্ণাঙ্ক		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = রিক্ত;
    char_u	*gname = রিক্ত;
    ufunc_T	*ufunc = রিক্ত;
    পূর্ণাঙ্ক		res = FAIL;
    পূর্ণাঙ্ক		is_autoload;
    পূর্ণাঙ্ক		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    যদি (varlen >= পরিমাণ(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	ঘূৰাই FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    যদি (import != রিক্ত)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	ঘূৰাই FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    যদি ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	পূর্ণাঙ্ক	    is_has = **arg == 'h';
	পূর্ণাঙ্ক	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	যদি (*s == '"')
	    (শূন্য)eval_string(&s, &argvars[0], TRUE, FALSE);
	অন্যথা যদি (*s == '\'')
	    (শূন্য)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	যদি (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    যদি (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_সংখ্যা;
		tv->vval.v_সংখ্যা = 0;
		যদি (is_has)
		    f_has(argvars, tv);
		অন্যথা যদি (is_len)
		    f_len(argvars, tv);
		অন্যথা
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    ঘূৰাই OK;
	}
	clear_tv(&argvars[0]);
	যদি (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    ঘূৰাই FAIL;
	}
    }

    যদি (generate_ppconst(cctx, ppconst) == FAIL)
	ঘূৰাই FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    যদি ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    অন্যথা যদি (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    অন্যথা
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    যদি (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	যদি (*arg == রিক্ত)
	    গচ্ছ theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	যদি (gname == রিক্ত)
	    গচ্ছ theend;
	++*arg;			// skip '('
    }
    অন্যথা
	*arg += varlen + 1;

    যদি (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	গচ্ছ theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != রিক্ত;
    যদি (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	পূর্ণাঙ্ক	    idx;

	// builtin function
	idx = find_internal_func(name);
	যদি (idx >= 0)
	{
	    যদি (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		গচ্ছ theend;
	    }

	    যদি (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_স্তূপ(cctx, 1);
		যদি (check_type_is_মান(get_type_on_স্তূপ(cctx, 0)) == FAIL)
		    গচ্ছ theend;

		// add() can be compiled to instructions if we know the type
		যদি (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		অন্যথা যদি (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    যদি ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		যদি (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    যদি (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    অন্যথা যদি (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    অন্যথা যদি (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    যদি (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		যদি (res == OK)
		    idx = -1;
	    }

	    যদি (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	অন্যথা
	    emsg_funcname(e_unknown_function_str, namebuf);
	গচ্ছ theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    যদি (lookup_local(namebuf, varlen, রিক্ত, cctx) == FAIL
	    && arg_exists(namebuf, varlen, রিক্ত, রিক্ত, রিক্ত, cctx) != OK)
    {
	class_T		*cl = রিক্ত;
	পূর্ণাঙ্ক		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	যদি (ufunc != রিক্ত)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    যদি (ufunc == রিক্ত)
		গচ্ছ theend;

	    যদি (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, রিক্ত, 0, argcount, FALSE);
		গচ্ছ theend;
	    }
	    যদি (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == রিক্ত
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		গচ্ছ theend;
	    }
	}
	অন্যথা যদি ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    যদি (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], রিক্ত,
							0, argcount, FALSE);
	    }
	    অন্যথা
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    গচ্ছ theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == রিক্ত && gname != রিক্ত ? gname : namebuf;
    যদি (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_স্তূপ(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	গচ্ছ theend;
    }

    // If we can find a global function by name generate the right call.
    যদি (ufunc != রিক্ত)
    {
	res = generate_CALL(cctx, ufunc, রিক্ত, 0, argcount, FALSE);
	গচ্ছ theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    যদি (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    অন্যথা
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    ঘূৰাই res;
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
to_name_end(char_u *arg, পূর্ণাঙ্ক use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    যদি (!eval_isnamec1(*arg))
	ঘূৰাই arg;

    বাবে (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	যদি (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == রিক্ত))
	    ভঙ্গ;
    ঘূৰাই p;
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

    যদি (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    যদি (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	যদি (eval_list(&p, &rettv, রিক্ত, FALSE) == FAIL)
	    p = arg;
    }
    ঘূৰাই p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    স্থির পূর্ণাঙ্ক
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    পূর্ণাঙ্ক		গুনতি = 0;
    পূর্ণাঙ্ক		is_const;
    পূর্ণাঙ্ক		is_all_const = TRUE;	// reset when non-const encountered
    পূর্ণাঙ্ক		must_end = FALSE;

    বাবে (;;)
    {
	যদি (may_get_next_সাৰি(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    ঘূৰাই FAIL;
	}
	যদি (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ঘূৰাই FAIL;
	}
	যদি (*p == ']')
	{
	    ++p;
	    ভঙ্গ;
	}
	যদি (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    ঘূৰাই FAIL;
	}
	যদি (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    ঘূৰাই FAIL;
	যদি (!is_const)
	    is_all_const = FALSE;
	++গুনতি;
	যদি (*p == ',')
	{
	    ++p;
	    যদি (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ঘূৰাই FAIL;
	    }
	}
	অন্যথা
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ঘূৰাই generate_NEWLIST(cctx, গুনতি, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    স্থির পূর্ণাঙ্ক
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    পূর্ণাঙ্ক first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    পূর্ণাঙ্ক		গুনতি = 0;
    পূর্ণাঙ্ক		is_const;
    পূর্ণাঙ্ক		is_all_const = TRUE;	// reset when non-const encountered
    পূর্ণাঙ্ক		must_end = FALSE;

    যদি (**arg != ')')
    {
	যদি (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    ঘূৰাই FAIL;
	}
	গুনতি = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	বাবে (;;)
	{
	    যদি (may_get_next_সাৰি(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		ঘূৰাই FAIL;
	    }
	    যদি (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		ঘূৰাই FAIL;
	    }
	    যদি (*p == ')')
	    {
		++p;
		ভঙ্গ;
	    }
	    যদি (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		ঘূৰাই FAIL;
	    }
	    যদি (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		ঘূৰাই FAIL;
	    যদি (!is_const)
		is_all_const = FALSE;
	    ++গুনতি;
	    যদি (*p == ',')
	    {
		++p;
		যদি (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ঘূৰাই FAIL;
		}
	    }
	    অন্যথা
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ঘূৰাই generate_NEWTUPLE(cctx, গুনতি, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    স্থির শূন্য
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*সাৰি;
    size_t	off;

    যদি (!evalarg->eval_using_cmdline || cctx == রিক্ত)
	ঘূৰাই;

    gap = &evalarg->eval_tofree_ga;
    যদি (gap->ga_len == 0)
	ঘূৰাই;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    সাৰি = ((char_u **)cctx->ctx_ufunc->uf_সাৰিগুলি.ga_data)[cctx->ctx_lnum];
    *arg = সাৰি + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    পূর্ণাঙ্ক
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    পূর্ণাঙ্ক		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    যদি (r != OK)
    {
	clear_evalarg(&evalarg, রিক্ত);
	ঘূৰাই r;
    }

    যদি (ppconst != রিক্ত && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, রিক্ত);
	ঘূৰাই FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    যদি (cctx->ctx_ufunc != রিক্ত)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	পূর্ণাঙ্ক block_depth = cctx->ctx_ufunc->uf_block_depth;

	যদি (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(পূর্ণাঙ্ক, block_depth);
	    যদি (ufunc->uf_block_ids != রিক্ত)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    পরিমাণ(পূর্ণাঙ্ক) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    যদি (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    যদি (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    যদি (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	যদি (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, রিক্ত);

    যদি (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	ঘূৰাই generate_FUNCREF(cctx, ufunc, রিক্ত, FALSE, 0, রিক্ত);
    }

    func_ptr_unref(ufunc);
    ঘূৰাই FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    পূর্ণাঙ্ক
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	পূর্ণাঙ্ক	    types_optional,
	evalarg_T   *evalarg)
{
    পূর্ণাঙ্ক		r;
    ufunc_T	*ufunc;
    পূর্ণাঙ্ক		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, রিক্ত);
    current_sctx.sc_version = save_sc_version;
    যদি (r != OK)
	ঘূৰাই r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    যদি (ufunc->uf_ret_type == রিক্ত || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, রিক্ত);

    যদি (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	ঘূৰাই OK;
    }
    clear_tv(rettv);
    ঘূৰাই FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    স্থির পূর্ণাঙ্ক
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    পূর্ণাঙ্ক		গুনতি = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    পূর্ণাঙ্ক		is_const;
    পূর্ণাঙ্ক		is_all_const = TRUE;	// reset when non-const encountered

    যদি (d == রিক্ত)
	ঘূৰাই FAIL;
    যদি (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	ঘূৰাই FAIL;
    }
    বাবে (;;)
    {
	char_u	    *চাবি = রিক্ত;

	যদি (may_get_next_সাৰি(whitep, arg, cctx) == FAIL)
	{
	    *arg = রিক্ত;
	    গচ্ছ failret;
	}

	যদি (**arg == '}')
	    ভঙ্গ;

	যদি (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    যদি (compile_expr0(arg, cctx) == FAIL)
		গচ্ছ failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    যদি (isn->isn_type == ISN_PUSHNR)
	    {
		ৱর্ণ buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.সংখ্যা);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    যদি (isn->isn_type == ISN_PUSHS)
		চাবি = isn->isn_arg.string;
	    অন্যথা যদি (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		গচ্ছ failret;
	    *arg = skipwhite(*arg);
	    যদি (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_চাবি));
		গচ্ছ failret;
	    }
	    ++*arg;
	}
	অন্যথা
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    চাবি = get_literal_চাবি(arg);
	    যদি (চাবি == রিক্ত)
		গচ্ছ failret;
	    যদি (generate_PUSHS(cctx, &চাবি) == FAIL)
		গচ্ছ failret;
	}

	// Check for duplicate keys, if using string keys.
	যদি (চাবি != রিক্ত)
	{
	    item = dict_find(d, চাবি, -1);
	    যদি (item != রিক্ত)
	    {
		semsg(_(e_duplicate_চাবি_in_dictionary_str), চাবি);
		গচ্ছ failret;
	    }
	    item = dictitem_alloc(চাবি);
	    যদি (item != রিক্ত)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		যদি (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	যদি (**arg != ':')
	{
	    যদি (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    অন্যথা
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    গচ্ছ failret;
	}
	whitep = *arg + 1;
	যদি (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    গচ্ছ failret;
	}

	যদি (may_get_next_সাৰি(whitep, arg, cctx) == FAIL)
	{
	    *arg = রিক্ত;
	    গচ্ছ failret;
	}

	যদি (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    গচ্ছ failret;
	যদি (!is_const)
	    is_all_const = FALSE;
	++গুনতি;

	whitep = *arg;
	যদি (may_get_next_সাৰি(whitep, arg, cctx) == FAIL)
	{
	    *arg = রিক্ত;
	    গচ্ছ failret;
	}
	যদি (**arg == '}')
	    ভঙ্গ;
	যদি (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    গচ্ছ failret;
	}
	যদি (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    গচ্ছ failret;
	}
	whitep = *arg + 1;
	যদি (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    গচ্ছ failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    যদি (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    ঘূৰাই generate_NEWDICT(cctx, গুনতি, FALSE);

failret:
    যদি (*arg == রিক্ত)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    ঘূৰাই FAIL;
}

/*
 * Compile "&option".
 */
    স্থির পূর্ণাঙ্ক
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    পূর্ণাঙ্ক		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    যদি (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_সংখ্যা ? &t_সংখ্যা : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    ঘূৰাই ret;
}

/*
 * Compile "$VAR".
 */
    স্থির পূর্ণাঙ্ক
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    যদি (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	ঘূৰাই FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    ঘূৰাই ret;
}

/*
 * Compile $"string" or $'string'.
 */
    স্থির পূর্ণাঙ্ক
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    পূর্ণাঙ্ক		ret;
    পূর্ণাঙ্ক		quote;
    পূর্ণাঙ্ক		evaluate = cctx->ctx_skip != SKIP_YES;
    পূর্ণাঙ্ক		গুনতি = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    বাবে (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	যদি (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	অন্যথা
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	যদি (ret == FAIL)
	    ভঙ্গ;
	যদি (evaluate)
	{
	    যদি ((tv.vval.v_string != রিক্ত && *tv.vval.v_string != NUL)
		    || (**arg != '{' && গুনতি == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		যদি (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    ঘূৰাই FAIL;
		tv.vval.v_string = রিক্ত;  // don't free it now
		++গুনতি;
	    }
	    clear_tv(&tv);
	}

	যদি (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ভঙ্গ;
	}

	p = compile_one_ৰাশিমালা_in_str(*arg, cctx);
	যদি (p == রিক্ত)
	{
	    ret = FAIL;
	    ভঙ্গ;
	}
	++গুনতি;
	*arg = p;
    }

    যদি (ret == FAIL || !evaluate)
	ঘূৰাই ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    যদি (গুনতি > 1)
	ঘূৰাই generate_CONCAT(cctx, গুনতি);

    ঘূৰাই OK;
}

/*
 * Compile "@r".
 */
    স্থির পূর্ণাঙ্ক
compile_get_register(char_u **arg, cctx_T *cctx)
{
    পূর্ণাঙ্ক		ret;

    ++*arg;
    যদি (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	ঘূৰাই FAIL;
    }
    যদি (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	ঘূৰাই FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, রিক্ত, &t_string);
    ++*arg;
    ঘূৰাই ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    স্থির পূর্ণাঙ্ক
apply_leader(typval_T *rettv, পূর্ণাঙ্ক numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    যেতিয়ালৈ (p > start)
    {
	--p;
	যদি (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    যদি (rettv->v_type == VAR_FLOAT)
	    {
		যদি (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    অন্যথা
	    {
		varnumber_T	val;
		পূর্ণাঙ্ক		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		যদি (check_not_string(rettv) == FAIL)
		    ঘূৰাই FAIL;
		val = tv_get_সংখ্যা_chk(rettv, &error);
		clear_tv(rettv);
		যদি (error)
		    ঘূৰাই FAIL;
		যদি (*p == '-')
		    val = -val;
		rettv->v_type = VAR_সংখ্যা;
		rettv->vval.v_সংখ্যা = val;
	    }
	}
	অন্যথা যদি (numeric_only)
	{
	    ++p;
	    ভঙ্গ;
	}
	অন্যথা যদি (*p == '!')
	{
	    পূর্ণাঙ্ক v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_সংখ্যা = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    ঘূৰাই OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    স্থির শূন্য
get_vim_constant(char_u **arg, typval_T *rettv)
{
    যদি (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_সংখ্যা = VVAL_TRUE;
	*arg += 6;
    }
    অন্যথা যদি (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_সংখ্যা = VVAL_FALSE;
	*arg += 7;
    }
    অন্যথা যদি (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_সংখ্যা = VVAL_NULL;
	*arg += 6;
    }
    অন্যথা যদি (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_সংখ্যা = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, পূর্ণাঙ্ক *len, পূর্ণাঙ্ক *type_is)
{
    exprtype_T	type = ৰাশিমালা_UNKNOWN;
    পূর্ণাঙ্ক		i;

    ৱিকল্প (p[0])
    {
	পক্ষ '=':   যদি (p[1] == '=')
			type = ৰাশিমালা_EQUAL;
		    অন্যথা যদি (p[1] == '~')
			type = ৰাশিমালা_MATCH;
		    ভঙ্গ;
	পক্ষ '!':   যদি (p[1] == '=')
			type = ৰাশিমালা_NEQUAL;
		    অন্যথা যদি (p[1] == '~')
			type = ৰাশিমালা_NOMATCH;
		    ভঙ্গ;
	পক্ষ '>':   যদি (p[1] != '=')
		    {
			type = ৰাশিমালা_GREATER;
			*len = 1;
		    }
		    অন্যথা
			type = ৰাশিমালা_GEQUAL;
		    ভঙ্গ;
	পক্ষ '<':   যদি (p[1] != '=')
		    {
			type = ৰাশিমালা_SMALLER;
			*len = 1;
		    }
		    অন্যথা
			type = ৰাশিমালা_SEQUAL;
		    ভঙ্গ;
	পক্ষ 'i':   যদি (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			যদি (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			যদি (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? ৰাশিমালা_IS : ৰাশিমালা_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    ভঙ্গ;
    }
    ঘূৰাই type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    শূন্য
skip_ৰাশিমালা_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_ৰাশিমালা(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, রিক্ত);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    পূর্ণাঙ্ক
bool_on_স্তূপ(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_স্তূপ(cctx, 0);
    যদি (type == &t_bool)
	ঘূৰাই OK;

    যদি (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_সংখ্যা
	    || type == &t_সংখ্যা_bool
	    || type == &t_const_সংখ্যা_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	ঘূৰাই generate_COND2BOOL(cctx);

    ঘূৰাই need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    শূন্য
error_white_both(char_u *op, পূর্ণাঙ্ক len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    স্থির পূর্ণাঙ্ক
compile_leader(cctx_T *cctx, পূর্ণাঙ্ক numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    যেতিয়ালৈ (p > start)
    {
	--p;
	যেতিয়ালৈ (VIM_ISWHITE(*p))
	    --p;
	যদি (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_স্তূপ(cctx, 0);
	    যদি (type->tt_type != VAR_FLOAT && need_type(type, &t_সংখ্যা,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ঘূৰাই FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    যদি (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == রিক্ত)
		ঘূৰাই FAIL;
	}
	অন্যথা যদি (numeric_only)
	{
	    ++p;
	    ভঙ্গ;
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক  invert = *p == '!';

	    যেতিয়ালৈ (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		যদি (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    যদি (check_type_is_মান(get_type_on_স্তূপ(cctx, 0)) == FAIL)
		ঘূৰাই FAIL;
	    যদি (generate_2BOOL(cctx, invert, -1) == FAIL)
		ঘূৰাই FAIL;
	}
    }
    *end = p;
    ঘূৰাই OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    স্থির পূর্ণাঙ্ক
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    পূর্ণাঙ্ক	    ret;
    char_u  *p = *arg + 1;

    যদি (may_get_next_সাৰি_error(p, arg, cctx) == FAIL)
	ঘূৰাই FAIL;

    যদি (**arg == ')')
	// empty tuple
	ঘূৰাই compile_tuple(arg, cctx, ppconst, FALSE);

    যদি (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    অন্যথা
    {
	// Not enough space in ppconst, flush constants.
	যদি (generate_ppconst(cctx, ppconst) == FAIL)
	    ঘূৰাই FAIL;
	ret = compile_expr0(arg, cctx);
    }
    যদি (may_get_next_সাৰি_error(*arg, arg, cctx) == FAIL)
	ঘূৰাই FAIL;
    যদি (ret == OK && **arg == ',')
    {
	// tuple
	পূর্ণাঙ্ক is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	যদি (generate_ppconst(cctx, ppconst) == FAIL)
	    ঘূৰাই FAIL;
	ঘূৰাই compile_tuple(arg, cctx, ppconst, is_const);
    }

    যদি (**arg == ')')
	++*arg;
    অন্যথা যদি (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    ঘূৰাই ret;
}

স্থির পূর্ণাঙ্ক compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    স্থির পূর্ণাঙ্ক
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    পূর্ণাঙ্ক		keeping_dict = FALSE;

    বাবে (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	যদি (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_সাৰি_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    যদি (next != রিক্ত &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_সাৰি_from_context(cctx, TRUE);
		যদি (next == রিক্ত)
		    ঘূৰাই FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	যদি (**arg == '(')
	{
	    পূর্ণাঙ্ক		argcount = 0;

	    যদি (generate_ppconst(cctx, ppconst) == FAIL)
		ঘূৰাই FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_স্তূপ(cctx, 0);

	    *arg = skipwhite(p + 1);
	    যদি (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		ঘূৰাই FAIL;
	    যদি (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		ঘূৰাই FAIL;
	    যদি (keeping_dict)
	    {
		keeping_dict = FALSE;
		যদি (generate_instr(cctx, ISN_CLEARDICT) == রিক্ত)
		    ঘূৰাই FAIL;
	    }
	}
	অন্যথা যদি (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    পূর্ণাঙ্ক	    alt;
	    char_u  *paren;

	    // something->method()
	    যদি (generate_ppconst(cctx, ppconst) == FAIL)
		ঘূৰাই FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_স্তূপ(cctx, 0);
	    যদি (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_মান));
		ঘূৰাই FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    যদি (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		ঘূৰাই FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    যদি (**arg == '(')
		alt = 2;
	    অন্যথা
	    {
		// alternative 1 or 3
		p = *arg;
		যদি (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    ঘূৰাই FAIL;
		}
		যদি (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		বাবে ( ; eval_isnamec(*p); ++p)
		    ;
		যদি (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    যদি (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			ঘূৰাই FAIL;
		}
		অন্যথা
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    যদি (paren == রিক্ত)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			ঘূৰাই FAIL;
		    }
		}
	    }

	    যদি (alt != 1)
	    {
		পূর্ণাঙ্ক	    argcount = 1;
		garray_T    *স্তূপ = &cctx->ctx_type_স্তূপ;
		পূর্ণাঙ্ক	    type_idx_start = স্তূপ->ga_len;
		পূর্ণাঙ্ক	    ৰাশিমালা_isn_start = cctx->ctx_instr.ga_len;
		পূর্ণাঙ্ক	    ৰাশিমালা_isn_end;
		পূর্ণাঙ্ক	    arg_isn_গুনতি;

		যদি (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    যদি (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			ঘূৰাই FAIL;
		}
		অন্যথা
		{
		    পূর্ণাঙ্ক fail;
		    পূর্ণাঙ্ক save_len = cctx->ctx_ufunc->uf_সাৰিগুলি.ga_len;
		    পূর্ণাঙ্ক	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_ৰাশিমালা;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_সাৰিগুলি.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_ৰাশিমালা;
		    cctx->ctx_ufunc->uf_সাৰিগুলি.ga_len = save_len;

		    যদি (fail)
		    {
			যদি (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			ঘূৰাই FAIL;
		    }
		}

		// Compile the arguments.
		যদি (**arg != '(')
		{
		    যদি (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    অন্যথা
			semsg(_(e_missing_parenthesis_str), *arg);
		    ঘূৰাই FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		ৰাশিমালা_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		যদি (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    ঘূৰাই FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_গুনতি = cctx->ctx_instr.ga_len - ৰাশিমালা_isn_end;
		যদি (arg_isn_গুনতি > 0)
		{
		    পূর্ণাঙ্ক	    ৰাশিমালা_isn_গুনতি = ৰাশিমালা_isn_end - ৰাশিমালা_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, ৰাশিমালা_isn_গুনতি);
		    type_T  *decl_type;
		    type2_T  *typep;

		    যদি (isn == রিক্ত)
			ঘূৰাই FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + ৰাশিমালা_isn_start,
					       পরিমাণ(isn_T) * ৰাশিমালা_isn_গুনতি);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + ৰাশিমালা_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + ৰাশিমালা_isn_end,
						পরিমাণ(isn_T) * arg_isn_গুনতি);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + ৰাশিমালা_isn_start + arg_isn_গুনতি,
					  isn, পরিমাণ(isn_T) * ৰাশিমালা_isn_গুনতি);
		    vim_free(isn);

		    typep = ((type2_T *)স্তূপ->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)স্তূপ->ga_data) + type_idx_start,
			      ((type2_T *)স্তূপ->ga_data) + type_idx_start + 1,
			      পরিমাণ(type2_T)
				       * (স্তূপ->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)স্তূপ->ga_data) + স্তূপ->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_স্তূপ(cctx, 0);
		যদি (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    ঘূৰাই FAIL;
	    }

	    যদি (keeping_dict)
	    {
		keeping_dict = FALSE;
		যদি (generate_instr(cctx, ISN_CLEARDICT) == রিক্ত)
		    ঘূৰাই FAIL;
	    }
	}
	অন্যথা যদি (**arg == '[')
	{
	    পূর্ণাঙ্ক		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    যদি (generate_ppconst(cctx, ppconst) == FAIL)
		ঘূৰাই FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    যদি (may_get_next_সাৰি_error(p, arg, cctx) == FAIL)
		ঘূৰাই FAIL;
	    যদি (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    অন্যথা
	    {
		যদি (compile_expr0(arg, cctx) == FAIL)
		    ঘূৰাই FAIL;
		যদি (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ঘূৰাই FAIL;
		}
		যদি (may_get_next_সাৰি_error(*arg, arg, cctx) == FAIL)
		    ঘূৰাই FAIL;
		*arg = skipwhite(*arg);
	    }
	    যদি (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		যদি (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ঘূৰাই FAIL;
		}
		যদি (may_get_next_সাৰি_error(*arg, arg, cctx) == FAIL)
		    ঘূৰাই FAIL;
		যদি (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		অন্যথা
		{
		    যদি (compile_expr0(arg, cctx) == FAIL)
			ঘূৰাই FAIL;
		    যদি (may_get_next_সাৰি_error(*arg, arg, cctx) == FAIL)
			ঘূৰাই FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    যদি (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		ঘূৰাই FAIL;
	    }
	    *arg = *arg + 1;

	    যদি (keeping_dict)
	    {
		keeping_dict = FALSE;
		যদি (generate_instr(cctx, ISN_CLEARDICT) == রিক্ত)
		    ঘূৰাই FAIL;
	    }
	    যদি (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		ঘূৰাই FAIL;
	}
	অন্যথা যদি (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    যদি (generate_ppconst(cctx, ppconst) == FAIL)
		ঘূৰাই FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_স্তূপ(cctx, 0);
	    যদি (type != &t_unknown
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
		যদি (compile_class_object_index(cctx, arg, type) == FAIL)
		    ঘূৰাই FAIL;
	    }
	    অন্যথা
	    {
		*arg = p + 1;
		যদি (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    ঘূৰাই FAIL;
		}
		p = *arg;
		যেতিয়ালৈ (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		যদি (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    ঘূৰাই FAIL;
		}
		যদি (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == রিক্ত)
		    ঘূৰাই FAIL;
		যদি (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    ঘূৰাই FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	অন্যথা
	    ভঙ্গ;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    যদি (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == রিক্ত)
	ঘূৰাই FAIL;

    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    পূর্ণাঙ্ক		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    পূর্ণাঙ্ক		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    যদি (eval_leader(arg, TRUE) == FAIL)
	ঘূৰাই FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    ৱিকল্প (**arg)
    {
	/*
	 * Number constant.
	 */
	পক্ষ '0':	// also for blob starting with 0z
	পক্ষ '1':
	পক্ষ '2':
	পক্ষ '3':
	পক্ষ '4':
	পক্ষ '5':
	পক্ষ '6':
	পক্ষ '7':
	পক্ষ '8':
	পক্ষ '9':
	পক্ষ '.':   যদি (eval_সংখ্যা(arg, rettv, TRUE, FALSE) == FAIL)
			ঘূৰাই FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    যদি (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			ঘূৰাই FAIL;
		    }
		    ভঙ্গ;

	/*
	 * String constant: "string".
	 */
	পক্ষ '"':   যদি (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			ঘূৰাই FAIL;
		    ভঙ্গ;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	পক্ষ '\'':  যদি (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			ঘূৰাই FAIL;
		    ভঙ্গ;

	/*
	 * Constant Vim variable.
	 */
	পক্ষ 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    ভঙ্গ;

	/*
	 * "true" constant
	 */
	পক্ষ 't':   যদি (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_সংখ্যা = VVAL_TRUE;
		    }
		    অন্যথা
			ret = NOTDONE;
		    ভঙ্গ;

	/*
	 * "false" constant
	 */
	পক্ষ 'f':   যদি (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_সংখ্যা = VVAL_FALSE;
		    }
		    অন্যথা
			ret = NOTDONE;
		    ভঙ্গ;

	/*
	 * "null" or "null_*" constant
	 */
	পক্ষ 'n':   যদি (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			পূর্ণাঙ্ক	len;

			বাবে (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			যদি (ret == FAIL)
			    ret = NOTDONE;
			অন্যথা
			    *arg += len + 4;
		    }
		    অন্যথা
			ret = NOTDONE;
		    ভঙ্গ;

	/*
	 * List: [expr, expr]
	 */
	পক্ষ '[':   যদি (generate_ppconst(cctx, ppconst) == FAIL)
			ঘূৰাই FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    ভঙ্গ;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	পক্ষ '{':   যদি (generate_ppconst(cctx, ppconst) == FAIL)
			ঘূৰাই FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    ভঙ্গ;

	/*
	 * Option value: &name
	 */
	পক্ষ '&':	যদি (generate_ppconst(cctx, ppconst) == FAIL)
			    ঘূৰাই FAIL;
			ret = compile_get_option(arg, cctx);
			ভঙ্গ;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	পক্ষ '$':	যদি (generate_ppconst(cctx, ppconst) == FAIL)
			    ঘূৰাই FAIL;
			যদি ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			অন্যথা
			    ret = compile_get_env(arg, cctx);
			ভঙ্গ;

	/*
	 * Register contents: @r.
	 */
	পক্ষ '@':	যদি (generate_ppconst(cctx, ppconst) == FAIL)
			    ঘূৰাই FAIL;
			ret = compile_get_register(arg, cctx);
			ভঙ্গ;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	পক্ষ '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    যদি (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    ভঙ্গ;

	স্ৱভাৱ:    ret = NOTDONE;
		    ভঙ্গ;
    }
    যদি (ret == FAIL)
	ঘূৰাই FAIL;

    যদি (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	যদি (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	অন্যথা
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    অন্যথা যদি (ret == NOTDONE)
    {
	char_u	    *p;
	পূর্ণাঙ্ক	    r;

	যদি (!eval_isnamec1(**arg))
	{
	    যদি (!vim9_bad_comment(*arg))
	    {
		যদি (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		অন্যথা
		    semsg(_(e_name_expected_str), *arg);
	    }
	    ঘূৰাই FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	যদি (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ঘূৰাই FAIL;
	}

	size_t namelen = p - *arg;
	যদি (*p == '<')
	{
	    যদি (skip_generic_func_type_args(&p) == FAIL)
		ঘূৰাই FAIL;
	}

	যদি (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	অন্যথা
	{
	    যদি (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		ঘূৰাই FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	যদি (r == FAIL)
	    ঘূৰাই FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    যদি (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	ঘূৰাই FAIL;
    যদি ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	যদি (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    ঘূৰাই FAIL;
	ঘূৰাই OK;
    }
    যদি (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	ঘূৰাই FAIL;
    ঘূৰাই OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    স্থির পূর্ণাঙ্ক
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = রিক্ত;

    // Recognize <type>
    যদি (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	যদি (want_type == রিক্ত)
	    ঘূৰাই FAIL;

	যদি (**arg != '>')
	{
	    যদি (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    অন্যথা
		emsg(_(e_missing_gt));
	    ঘূৰাই FAIL;
	}
	++*arg;
	যদি (may_get_next_সাৰি_error(*arg, arg, cctx) == FAIL)
	    ঘূৰাই FAIL;
    }

    যদি (compile_expr9(arg, cctx, ppconst) == FAIL)
	ঘূৰাই FAIL;

    যদি (want_type != রিক্ত)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_স্তূপ(cctx, 0);
	যদি (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    যদি (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		ঘূৰাই FAIL;
	}
    }

    ঘূৰাই OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    স্থির পূর্ণাঙ্ক
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    পূর্ণাঙ্ক		ppconst_used = ppconst->pp_used;

    // get the first expression
    যদি (compile_expr8(arg, cctx, ppconst) == FAIL)
	ঘূৰাই FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    বাবে (;;)
    {
	op = may_peek_next_সাৰি(cctx, *arg, &next);
	যদি (*op != '*' && *op != '/' && *op != '%')
	    ভঙ্গ;
	যদি (next != রিক্ত)
	{
	    *arg = next_সাৰি_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	যদি (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    ঘূৰাই FAIL;
	}
	যদি (may_get_next_সাৰি_error(op + 1, arg, cctx) == FAIL)
	    ঘূৰাই FAIL;

	// get the second expression
	যদি (compile_expr8(arg, cctx, ppconst) == FAIL)
	    ঘূৰাই FAIL;

	যদি (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_সংখ্যা
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_সংখ্যা)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    পূর্ণাঙ্ক		    failed = FALSE;

	    // both are numbers: compute the result
	    ৱিকল্প (*op)
	    {
		পক্ষ '*': res = tv1->vval.v_সংখ্যা * tv2->vval.v_সংখ্যা;
			  ভঙ্গ;
		পক্ষ '/': res = অঙ্ক_divide(tv1->vval.v_সংখ্যা,
						  tv2->vval.v_সংখ্যা, &failed);
			  ভঙ্গ;
		পক্ষ '%': res = অঙ্ক_modulus(tv1->vval.v_সংখ্যা,
						  tv2->vval.v_সংখ্যা, &failed);
			  ভঙ্গ;
	    }
	    যদি (failed)
		ঘূৰাই FAIL;
	    tv1->vval.v_সংখ্যা = res;
	    --ppconst->pp_used;
	}
	অন্যথা
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    ঘূৰাই OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    স্থির পূর্ণাঙ্ক
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    পূর্ণাঙ্ক		oplen;
    পূর্ণাঙ্ক		ppconst_used = ppconst->pp_used;

    // get the first variable
    যদি (compile_expr7(arg, cctx, ppconst) == FAIL)
	ঘূৰাই FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    বাবে (;;)
    {
	op = may_peek_next_সাৰি(cctx, *arg, &next);
	যদি (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    ভঙ্গ;
	যদি (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    ভঙ্গ;
	oplen = (*op == '.' ? 2 : 1);
	যদি (next != রিক্ত)
	{
	    *arg = next_সাৰি_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	যদি (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    ঘূৰাই FAIL;
	}

	যদি (may_get_next_সাৰি_error(op + oplen, arg, cctx) == FAIL)
	    ঘূৰাই FAIL;

	// get the second expression
	যদি (compile_expr7(arg, cctx, ppconst) == FAIL)
	    ঘূৰাই FAIL;

	যদি (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_সংখ্যা
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_সংখ্যা)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    যদি (*op == '+')
		tv1->vval.v_সংখ্যা = tv1->vval.v_সংখ্যা + tv2->vval.v_সংখ্যা;
	    অন্যথা যদি (*op == '-')
		tv1->vval.v_সংখ্যা = tv1->vval.v_সংখ্যা - tv2->vval.v_সংখ্যা;
	    অন্যথা
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		যদি (tv1->vval.v_string == রিক্ত)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    ঘূৰাই FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	অন্যথা
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    যদি (*op == '.')
	    {
		যদি (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    ঘূৰাই FAIL;
		যদি (generate_CONCAT(cctx, 2) == FAIL)
		    ঘূৰাই FAIL;
	    }
	    অন্যথা
		generate_two_op(cctx, op);
	}
    }

    ঘূৰাই OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    স্থির পূর্ণাঙ্ক
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ৰাশিমালা_UNKNOWN;
    char_u	*p;
    char_u	*next;
    পূর্ণাঙ্ক		len = 2;
    পূর্ণাঙ্ক		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    যদি (compile_expr6(arg, cctx, ppconst) == FAIL)
	ঘূৰাই FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    বাবে (;;)
    {
	type = ৰাশিমালা_UNKNOWN;

	p = may_peek_next_সাৰি(cctx, *arg, &next);
	যদি (p[0] == '<' && p[1] == '<')
	    type = ৰাশিমালা_LSHIFT;
	অন্যথা যদি (p[0] == '>' && p[1] == '>')
	    type = ৰাশিমালা_RSHIFT;

	যদি (type == ৰাশিমালা_UNKNOWN)
	    ঘূৰাই OK;

	// Handle a bitwise left or right shift operator
	যদি (ppconst->pp_used == ppconst_used + 1)
	{
	    যদি (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_সংখ্যা)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_সংখ্যা));
		ঘূৰাই FAIL;
	    }
	}
	অন্যথা
	{
	    type_T	*t = get_type_on_স্তূপ(cctx, 0);

	    যদি (need_type(t, &t_সংখ্যা, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_সংখ্যা));
		ঘূৰাই FAIL;
	    }
	}

	যদি (next != রিক্ত)
	{
	    *arg = next_সাৰি_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	যদি (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ঘূৰাই FAIL;
	}

	// get the second variable
	যদি (may_get_next_সাৰি_error(p + len, arg, cctx) == FAIL)
	    ঘূৰাই FAIL;

	যদি (compile_expr6(arg, cctx, ppconst) == FAIL)
	    ঘূৰাই FAIL;

	যদি (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    যদি (tv2->v_type != VAR_সংখ্যা || tv2->vval.v_সংখ্যা < 0)
	    {
		// right operand should be a positive number
		যদি (tv2->v_type != VAR_সংখ্যা)
		    emsg(_(e_bitshift_ops_must_be_সংখ্যা));
		অন্যথা
		    emsg(_(e_bitshift_ops_must_be_positive));
		ঘূৰাই FAIL;
	    }

	    যদি (tv2->vval.v_সংখ্যা > MAX_LSHIFT_BITS)
		tv1->vval.v_সংখ্যা = 0;
	    অন্যথা যদি (type == ৰাশিমালা_LSHIFT)
		tv1->vval.v_সংখ্যা =
			(uvarnumber_T)tv1->vval.v_সংখ্যা << tv2->vval.v_সংখ্যা;
	    অন্যথা
		tv1->vval.v_সংখ্যা =
			(uvarnumber_T)tv1->vval.v_সংখ্যা >> tv2->vval.v_সংখ্যা;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	অন্যথা
	{
	    যদি (need_type(get_type_on_স্তূপ(cctx, 0), &t_সংখ্যা, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_সংখ্যা));
		ঘূৰাই FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    যদি (isn == রিক্ত)
		ঘূৰাই FAIL;

	    যদি (isn != রিক্ত)
		isn->isn_arg.op.op_type = type;
	}
    }

    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ৰাশিমালা_UNKNOWN;
    char_u	*p;
    char_u	*next;
    পূর্ণাঙ্ক		len = 2;
    পূর্ণাঙ্ক		type_is = FALSE;
    পূর্ণাঙ্ক		ppconst_used = ppconst->pp_used;

    // get the first variable
    যদি (compile_expr5(arg, cctx, ppconst) == FAIL)
	ঘূৰাই FAIL;

    p = may_peek_next_সাৰি(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    যদি (type != ৰাশিমালা_UNKNOWN)
    {
	পূর্ণাঙ্ক ic = FALSE;  // Default: do not ignore case

	যদি (next != রিক্ত)
	{
	    *arg = next_সাৰি_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	যদি (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    ঘূৰাই FAIL;
	}
	// extra question mark appended: ignore case
	যদি (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	অন্যথা যদি (p[len] == '#')
	    ++len;
	// nothing appended: match case

	যদি (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ঘূৰাই FAIL;
	}

	// get the second variable
	যদি (may_get_next_সাৰি_error(p + len, arg, cctx) == FAIL)
	    ঘূৰাই FAIL;

	যদি (compile_expr5(arg, cctx, ppconst) == FAIL)
	    ঘূৰাই FAIL;

	যদি (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    পূর্ণাঙ্ক		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    যদি (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    অন্যথা
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_সংখ্যা = tv1->vval.v_সংখ্যা
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    ঘূৰাই ret;
	}

	generate_ppconst(cctx, ppconst);
	ঘূৰাই generate_COMPARE(cctx, type, ic);
    }

    ঘূৰাই OK;
}

স্থির পূর্ণাঙ্ক compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    স্থির পূর্ণাঙ্ক
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	ৱর্ণ	*op,
	ppconst_T *ppconst,
	পূর্ণাঙ্ক	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_সাৰি(cctx, *arg, &next);
    পূর্ণাঙ্ক		opchar = *op;

    যদি (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	পূর্ণাঙ্ক		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, পরিমাণ(পূর্ণাঙ্ক), 10);
	যেতিয়ালৈ (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    পূর্ণাঙ্ক		start_ctx_lnum = cctx->ctx_lnum;
	    পূর্ণাঙ্ক		save_lnum;
	    পূর্ণাঙ্ক		const_used;
	    পূর্ণাঙ্ক		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    যদি (next != রিক্ত)
	    {
		*arg = next_সাৰি_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    যদি (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		ঘূৰাই FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    যদি (status != FAIL)
	    {
		// Use the last ppconst if possible.
		যদি (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    পূর্ণাঙ্ক		is_true = tv2bool(tv);

		    যদি ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_সংখ্যা = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    অন্যথা
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		অন্যথা
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_স্তূপ(cctx);
		}
	    }
	    যদি (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    যদি (status == FAIL)
	    {
		ga_clear(&end_ga);
		ঘূৰাই FAIL;
	    }

	    যদি (jump_when != JUMP_NEVER)
	    {
		যদি (cctx->ctx_skip != SKIP_YES)
		{
		    *(((পূর্ণাঙ্ক *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    যদি (may_get_next_সাৰি_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		ঘূৰাই FAIL;
	    }

	    const_used = ppconst->pp_used;
	    যদি ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		ঘূৰাই FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    যদি (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		যদি (tv->v_type == VAR_সংখ্যা
			 && (tv->vval.v_সংখ্যা == 1 || tv->vval.v_সংখ্যা == 0))
		{
		    tv->vval.v_সংখ্যা = tv->vval.v_সংখ্যা == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_সাৰি(cctx, *arg, &next);
	}

	যদি (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    ঘূৰাই FAIL;
	}

	যদি (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    যদি (bool_on_স্তূপ(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		ঘূৰাই FAIL;
	    }

	যদি (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    যেতিয়ালৈ (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((পূর্ণাঙ্ক *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    ঘূৰাই OK;
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
    স্থির পূর্ণাঙ্ক
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    পূর্ণাঙ্ক		ppconst_used = ppconst->pp_used;

    // get the first variable
    যদি (compile_expr4(arg, cctx, ppconst) == FAIL)
	ঘূৰাই FAIL;

    // || and && work almost the same
    ঘূৰাই compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    স্থির পূর্ণাঙ্ক
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    পূর্ণাঙ্ক		ppconst_used = ppconst->pp_used;

    // eval the first expression
    যদি (compile_expr3(arg, cctx, ppconst) == FAIL)
	ঘূৰাই FAIL;

    // || and && work almost the same
    ঘূৰাই compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    পূর্ণাঙ্ক
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    পূর্ণাঙ্ক		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    যদি (cctx->ctx_skip == SKIP_YES)
    {
	পূর্ণাঙ্ক		prev_did_emsg = did_emsg;

	skip_ৰাশিমালা_cctx(arg, cctx);
	ঘূৰাই did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    যদি (compile_expr2(arg, cctx, ppconst) == FAIL)
	ঘূৰাই FAIL;

    p = may_peek_next_সাৰি(cctx, *arg, &next);
    যদি (*p == '?')
    {
	পূর্ণাঙ্ক		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
	পূর্ণাঙ্ক		alt_idx = instr->ga_len;
	পূর্ণাঙ্ক		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = রিক্ত;
	পূর্ণাঙ্ক		has_const_ৰাশিমালা = FALSE;
	পূর্ণাঙ্ক		const_মান = FALSE;
	পূর্ণাঙ্ক		save_skip = cctx->ctx_skip;

	যদি (next != রিক্ত)
	{
	    *arg = next_সাৰি_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	যদি (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    ঘূৰাই FAIL;
	}

	যদি (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_ৰাশিমালা = TRUE;
	    যদি (op_falsy)
		const_মান = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    অন্যথা
	    {
		পূর্ণাঙ্ক error = FALSE;

		const_মান = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		যদি (error)
		    ঘূৰাই FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_মান : !const_মান) ? SKIP_YES : SKIP_NOT;

	    যদি (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    অন্যথা
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	অন্যথা
	{
	    generate_ppconst(cctx, ppconst);
	    যদি (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    যদি (op_falsy)
	    {
		type1 = get_type_on_স্তূপ(cctx, -1);
		যদি (check_type_is_মান(type1) == FAIL)
		    ঘূৰাই FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	যদি (may_get_next_সাৰি_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    ঘূৰাই FAIL;
	যদি (compile_expr1(arg, cctx, ppconst) == FAIL)
	    ঘূৰাই FAIL;

	যদি (!has_const_ৰাশিমালা)
	{
	    generate_ppconst(cctx, ppconst);

	    যদি (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_স্তূপ(cctx, 0);
		--স্তূপ->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	যদি (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_সাৰি(cctx, *arg, &next);
	    যদি (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ঘূৰাই FAIL;
	    }
	    যদি (next != রিক্ত)
	    {
		*arg = next_সাৰি_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    যদি (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		ঘূৰাই FAIL;
	    }

	    // evaluate the third expression
	    যদি (has_const_ৰাশিমালা)
		cctx->ctx_skip = save_skip == SKIP_YES || const_মান
							 ? SKIP_YES : SKIP_NOT;
	    যদি (may_get_next_সাৰি_error(p + 1, arg, cctx) == FAIL)
		ঘূৰাই FAIL;
	    যদি (compile_expr1(arg, cctx, ppconst) == FAIL)
		ঘূৰাই FAIL;
	}

	যদি (!has_const_ৰাশিমালা)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)স্তূপ->ga_data)
					      + স্তূপ->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    ঘূৰাই OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
compile_expr0_ext(char_u **arg,  cctx_T *cctx, পূর্ণাঙ্ক *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    যদি (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ঘূৰাই FAIL;
    }
    যদি (is_const != রিক্ত)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    যদি (generate_ppconst(cctx, &ppconst) == FAIL)
	ঘূৰাই FAIL;
    ঘূৰাই OK;
}

/*
 * Toplevel expression.
 */
    পূর্ণাঙ্ক
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    ঘূৰাই compile_expr0_ext(arg, cctx, রিক্ত);
}


#endif // defined(FEAT_EVAL)
