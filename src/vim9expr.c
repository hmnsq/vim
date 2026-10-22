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
#ضمن "vim.h"

#𐡄𐡍 defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
ساكن صحيح paren_follows_after_expr = 0;

/*
 * Generate code for any ppconst entries.
 */
    صحيح
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    صحيح	    i;
    صحيح	    ret = OK;
    صحيح	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    لكل (i = 0; i < ppconst->pp_used; ++i)
	𐡄𐡍 (generate_tv_PUSH(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    أرجع ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    ساكن صحيح
check_ppconst_bool(ppconst_T *ppconst)
{
    𐡄𐡍 (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	أرجع check_typval_type(&t_bool, tv, where);
    }
    أرجع OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    فراغ
clear_ppconst(ppconst_T *ppconst)
{
    صحيح	    i;

    لكل (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    ساكن صحيح
compile_tuple_member(
    type2_T	*typep,
    صحيح		is_slice,
    cctx_T	*cctx)
{
    𐡄𐡍 (is_slice)
    {
	𐡄𐡍 (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    أرجع FAIL;
	// a copy is made so the member type is no longer declared
	𐡄𐡍 (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	𐡄𐡍 (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    𐡄𐡍 (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    وإلا
    {
	𐡄𐡍 (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    𐡄𐡍 (typep->type_curr->tt_argcount == 1)
	    {
		𐡄𐡍 (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		وإلا
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    وإلا
		typep->type_curr = &t_any;
	    𐡄𐡍 (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		𐡄𐡍 (typep->type_decl->tt_argcount == 1)
		{
		    𐡄𐡍 (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    وإلا
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		وإلا
		    typep->type_curr = &t_any;
	    }
	    وإلا
		typep->type_decl = typep->type_curr;
	}
	𐡄𐡍 (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    أرجع FAIL;
    }

    أرجع OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    صحيح
compile_member(صحيح is_slice, صحيح *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*stack = &cctx->ctx_type_stack;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)stack->ga_data) + stack->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)stack->ga_data) + stack->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    𐡄𐡍 ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    𐡄𐡍 (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	𐡄𐡍 (need_type(idxtype, &t_number, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    أرجع FAIL;
	𐡄𐡍 (is_slice)
	{
	    idxtype = get_type_on_stack(cctx, 1);
	    𐡄𐡍 (need_type(idxtype, &t_number, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;
	}
    }

    𐡄𐡍 (vartype == VAR_DICT)
    {
	𐡄𐡍 (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    أرجع FAIL;
	}
	𐡄𐡍 (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    𐡄𐡍 (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    𐡄𐡍 (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		𐡄𐡍 (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    وإلا
		typep->type_decl = typep->type_curr;
	}
	وإلا
	{
	    𐡄𐡍 (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	𐡄𐡍 (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    أرجع FAIL;
	𐡄𐡍 (keeping_dict != عدم)
	    *keeping_dict = TRUE;
    }
    وإلا 𐡄𐡍 (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	𐡄𐡍 ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    أرجع FAIL;
    }
    وإلا 𐡄𐡍 (vartype == VAR_BLOB)
    {
	𐡄𐡍 (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    𐡄𐡍 (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    typep->type_curr = &t_number;
	    typep->type_decl = &t_number;
	    𐡄𐡍 (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		أرجع FAIL;
	}
    }
    وإلا 𐡄𐡍 (vartype == VAR_TUPLE)
    {
	𐡄𐡍 (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    أرجع FAIL;
    }
    وإلا 𐡄𐡍 (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	𐡄𐡍 (is_slice)
	{
	    𐡄𐡍 (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		أرجع FAIL;
	    // a copy is made so the member type is no longer declared
	    𐡄𐡍 (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    𐡄𐡍 (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		𐡄𐡍 (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	وإلا
	{
	    𐡄𐡍 (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		𐡄𐡍 (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		𐡄𐡍 (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    𐡄𐡍 (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		وإلا
			typep->type_decl = typep->type_curr;
	    }
	    𐡄𐡍 (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		أرجع FAIL;
	}
    }
    وإلا
    {
	بدل (vartype)
	{
	    حالة VAR_FUNC:
	    حالة VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		اكسر;
	    حالة VAR_BOOL:
	    حالة VAR_SPECIAL:
	    حالة VAR_JOB:
	    حالة VAR_CHANNEL:
	    حالة VAR_INSTR:
	    حالة VAR_CLASS:
	    حالة VAR_OBJECT:
	    حالة VAR_TYPEALIAS:
	    حالة VAR_UNKNOWN:
	    حالة VAR_ANY:
	    حالة VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		اكسر;
	    افتراضي:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    ساكن صحيح
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    لكل (cctx_T *cctx = cctx_arg; cctx != عدم; cctx = cctx->ctx_outer)
    {
	𐡄𐡍 (cctx->ctx_ufunc != عدم && cctx->ctx_ufunc->uf_class != عدم)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    طالما (clp != عدم)
	    {
		𐡄𐡍 (clp == cl)
		    أرجع TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    أرجع FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    صحيح
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    صحيح		m_idx;
    صحيح		ret = FAIL;

    𐡄𐡍 (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	أرجع FAIL;
    }

    class_T *cl = type->tt_class;
    صحيح is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    𐡄𐡍 (type == &t_super)
    {
	𐡄𐡍 (cctx->ctx_ufunc == عدم || cctx->ctx_ufunc->uf_class == عدم)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    أرجع FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_stack.ga_len;
    }
    وإلا 𐡄𐡍 (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	𐡄𐡍 (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    𐡄𐡍 (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    𐡄𐡍 (cl == عدم)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	أرجع FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, عدم, عدم, FNE_CHECK_START);
    𐡄𐡍 (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	أرجع FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    𐡄𐡍 (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	𐡄𐡍 (name_end == عدم)
	    اذهب done;
    }

    𐡄𐡍 (*name_end == '(')
    {
	صحيح	function_count;
	صحيح	child_count;
	ufunc_T	**functions;

	𐡄𐡍 (type->tt_type == VAR_CLASS)
	{
	    function_count = cl->class_class_function_count;
	    child_count = cl->class_class_function_count_child;
	    functions = cl->class_class_functions;
	}
	وإلا
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    𐡄𐡍 (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_count = cl->class_obj_method_count;
	    child_count = cl->class_obj_method_count_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = عدم;
	صحيح fi;
	لكل (fi = is_super ? child_count : 0; fi < function_count; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    𐡄𐡍 (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		اكسر;
	    }
	}

	ocmember_T  *ocm = عدم;
	𐡄𐡍 (ufunc == عدم)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    𐡄𐡍 (ocm == عدم || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_msg(cl, type->tt_type, name, len);
		اذهب done;
	    }
	    𐡄𐡍 (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_stack.ga_len;
		𐡄𐡍 (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    اذهب done;
	    }
	    وإلا
	    {
		صحيح status;

		𐡄𐡍 (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		وإلا
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		𐡄𐡍 (status == FAIL)
		    اذهب done;
	    }
	}

	𐡄𐡍 (is_super && ufunc != عدم && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    اذهب done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	𐡄𐡍 (ocm == عدم && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    اذهب done;
	}

	// process generic function call
	𐡄𐡍 (ufunc != عدم)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    𐡄𐡍 (ufunc == عدم)
		اذهب done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	صحيح argcount = 0;
	𐡄𐡍 (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    اذهب done;

	𐡄𐡍 (ocm != عدم)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    اذهب done;
	}
	𐡄𐡍 (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	وإلا
	    ret = generate_CALL(cctx, ufunc, عدم, 0, argcount, FALSE);
	اذهب done;
    }

    𐡄𐡍 (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	𐡄𐡍 (m_idx >= 0)
	{
	    𐡄𐡍 (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		اذهب done;
	    }

	    *arg = name_end;
	    𐡄𐡍 (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    وإلا
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    اذهب done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	𐡄𐡍 (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    𐡄𐡍 (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		اذهب done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_stack.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, عدم);
	    اذهب done;
	}

	member_not_found_msg(cl, VAR_OBJECT, name, len);
    }
    وإلا
    {
	// load class member
	صحيح idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	𐡄𐡍 (m != عدم)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    𐡄𐡍 (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		اذهب done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_stack.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    اذهب done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	𐡄𐡍 (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    𐡄𐡍 (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		اذهب done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_stack.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, عدم);
	    اذهب done;
	}

	member_not_found_msg(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    أرجع ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    صحيح
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    صحيح		    idx;
    imported_T	    *imp;

    𐡄𐡍 (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	أرجع FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, عدم);
    𐡄𐡍 (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	أرجع OK;
    }

    𐡄𐡍 (end == عدم)
	imp = عدم;
    وإلا 𐡄𐡍 (import == عدم)
	imp = find_imported(name, 0, FALSE);
    وإلا
	imp = import;

    𐡄𐡍 (imp != عدم)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	صحيح	cc;
	ufunc_T	*ufunc = عدم;
	type_T	*type;
	صحيح	done = FALSE;
	صحيح	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	𐡄𐡍 (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    أرجع FAIL;
	}
	++p;
	𐡄𐡍 (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    أرجع FAIL;
	}

	// isolate one name
	exp_name = p;
	طالما (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	𐡄𐡍 (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	𐡄𐡍 (res == OK)
	{
	    𐡄𐡍 (si->sn_autoload_prefix != عدم
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		𐡄𐡍 (cc == '(' || paren_follows_after_expr)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		وإلا
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    وإلا 𐡄𐡍 (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		𐡄𐡍 (cc == '(' || paren_follows_after_expr)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		وإلا
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    وإلا
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, عدم, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	𐡄𐡍 (done)
	    أرجع res;

	𐡄𐡍 (idx < 0)
	{
	    𐡄𐡍 (ufunc != عدم)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		𐡄𐡍 (IS_GENERIC_FUNC(ufunc))
		{
		    𐡄𐡍 (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			𐡄𐡍 (p != عدم)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			𐡄𐡍 (p == عدم || ufunc == عدم)
			    أرجع FAIL;
		    }
		    وإلا
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			أرجع FAIL;
		    }
		}
		وإلا 𐡄𐡍 (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    أرجع FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, عدم, TRUE);
		أرجع OK;
	    }
	    أرجع FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	أرجع OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    أرجع generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    ساكن صحيح
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    صحيح			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    𐡄𐡍 (ufunc == عدم || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	أرجع FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    𐡄𐡍 (ufunc == عدم)
	أرجع FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    𐡄𐡍 (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, عدم) == FAIL)
	أرجع FAIL;
    أرجع generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    ساكن صحيح
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    أرجع cctx->ctx_ufunc != عدم
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == عدم
		|| cctx->ctx_outer->ctx_ufunc == عدم
		|| cctx->ctx_outer->ctx_ufunc->uf_class == عدم
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
    ساكن صحيح
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    صحيح			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    صحيح		isn_idx;

    𐡄𐡍 (fp == عدم)
	أرجع FAIL;

    لكل (صحيح depth = 0; depth < outer_depth; ++depth)
    {
	𐡄𐡍 (outer_cctx->ctx_outer == عدم)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    أرجع FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    𐡄𐡍 (fp->uf_block_ids == عدم && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(صحيح, ufunc->uf_block_depth);
	𐡄𐡍 (fp->uf_block_ids != عدم)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    حجم(صحيح) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    𐡄𐡍 (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	أرجع FAIL;

    𐡄𐡍 (generate_FUNCREF(cctx, fp, عدم, FALSE, 0, &isn_idx) == FAIL)
	أرجع FAIL;
    𐡄𐡍 (cctx->ctx_skip == SKIP_YES)
	أرجع OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    𐡄𐡍 (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	𐡄𐡍 (extra == عدم)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    𐡄𐡍 (extra == عدم)
		أرجع FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    أرجع OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    صحيح
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	صحيح	is_expr,
	صحيح	error)
{
    type_T	*type;
    char_u	*name = عدم;
    char_u	*end = end_arg;
    صحيح		res = FAIL;
    صحيح		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = عدم;

    generic_func_args_table_init(&gfatab);

    𐡄𐡍 (*(*arg + namelen) == '<')
    {
	// generic function call
	𐡄𐡍 (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == عدم)
	{
	    generic_func_args_table_clear(&gfatab);
	    أرجع FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    𐡄𐡍 (*(*arg + 1) == ':')
    {
	𐡄𐡍 (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    بدل (**arg)
	    {
		حالة 'g': isn_type = ISN_LOADGDICT; اكسر;
		حالة 'w': isn_type = ISN_LOADWDICT; اكسر;
		حالة 't': isn_type = ISN_LOADTDICT; اكسر;
		حالة 'b': isn_type = ISN_LOADBDICT; اكسر;
		افتراضي:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    اذهب theend;
	    }
	    𐡄𐡍 (generate_instr_type(cctx, isn_type, &t_dict_any) == عدم)
		اذهب theend;
	    res = OK;
	}
	وإلا
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    𐡄𐡍 (name == عدم)
		اذهب theend;

	    بدل (**arg)
	    {
		حالة 'v': res = generate_LOADV(cctx, name);
			  اكسر;
		حالة 's': 𐡄𐡍 (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      اذهب theend;
			  }
			  𐡄𐡍 (is_expr && find_func(name, FALSE) != عدم)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  وإلا
			      res = compile_load_scriptvar(cctx, name,
							    عدم, &end, عدم);
			  اكسر;
		حالة 'g': 𐡄𐡍 (vim_strchr(name, AUTOLOAD_CHAR) == عدم)
			  {
			      𐡄𐡍 (is_expr && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != عدم
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      وإلا
				  isn_type = ISN_LOADG;
			  }
			  وإلا
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      𐡄𐡍 (name == عدم)
				  اذهب theend;
			  }
			  اكسر;
		حالة 'w': isn_type = ISN_LOADW; اكسر;
		حالة 't': isn_type = ISN_LOADT; اكسر;
		حالة 'b': isn_type = ISN_LOADB; اكسر;
		افتراضي:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  اذهب theend;
	    }
	    𐡄𐡍 (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    وإلا
    {
	صحيح	    idx;
	صحيح	    method_idx;
	صحيح	    gen_load = FALSE;
	صحيح	    gen_load_outer = 0;
	صحيح	    outer_loop_depth = -1;
	صحيح	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	𐡄𐡍 (name == عدم)
	    اذهب theend;

	𐡄𐡍 (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = push_type_stack(cctx, &t_super);
	    𐡄𐡍 (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	وإلا 𐡄𐡍 (vim_strchr(name, AUTOLOAD_CHAR) != عدم)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	وإلا 𐡄𐡍 (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    𐡄𐡍 (gen_load_outer == 0)
		gen_load = TRUE;
	}
	وإلا
	{
	    lvar_T  lvar;
	    class_T *cl = عدم;

	    𐡄𐡍 (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		𐡄𐡍 (lvar.lv_ufunc != عدم && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		وإلا 𐡄𐡍 (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		وإلا
		    gen_load = TRUE;
	    }
	    وإلا 𐡄𐡍 (cctx->ctx_ufunc->uf_defclass != عدم &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		𐡄𐡍 (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    𐡄𐡍 (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    وإلا
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									عدم);
		    }
		}
		وإلا
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    وإلا
	    {
		imported_T *imp = عدم;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		𐡄𐡍 (script_var_exists(*arg, namelen, cctx, عدم) == OK
			    || (imp = find_imported(name, 0, FALSE)) != عدم)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		𐡄𐡍 (res == FAIL && is_expr && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	𐡄𐡍 (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, عدم, type);
	𐡄𐡍 (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    𐡄𐡍 (lt != عدم)
	*lt = '<';
    𐡄𐡍 (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    أرجع res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    ساكن صحيح
compile_string(isn_T *isn, cctx_T *cctx, صحيح str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    صحيح		expr_res;
    صحيح		trailing_error;
    صحيح		instr_count;
    isn_T	*instr = عدم;

    // Remove the string type from the stack.
    --cctx->ctx_type_stack.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = عدم;

    // avoid peeking a next line
    صحيح galen_save = cctx->ctx_ufunc->uf_lines.ga_len;
    cctx->ctx_ufunc->uf_lines.ga_len = 0;

    expr_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_lines.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    𐡄𐡍 (expr_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	𐡄𐡍 (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_stack.ga_len;
	أرجع FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_count = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_count].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    أرجع OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    صحيح
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	صحيح	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    صحيح	    must_end = FALSE;
    صحيح	    instr_count;

    لكل (;;)
    {
	𐡄𐡍 (may_get_next_line(whitep, &p, cctx) == FAIL)
	    اذهب failret;
	𐡄𐡍 (*p == ')')
	{
	    *arg = p + 1;
	    أرجع OK;
	}
	𐡄𐡍 (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    أرجع FAIL;
	}

	instr_count = cctx->ctx_instr.ga_len;
	𐡄𐡍 (compile_expr0(&p, cctx) == FAIL)
	    أرجع FAIL;
	++*argcount;

	𐡄𐡍 (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_count + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_count;

	    // {skip} argument of searchpair() can be compiled if not empty
	    𐡄𐡍 (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != عدم
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	وإلا 𐡄𐡍 (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_count + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_count;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    𐡄𐡍 (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != عدم
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	𐡄𐡍 (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    أرجع FAIL;
	}
	𐡄𐡍 (*p == ',')
	{
	    ++p;
	    𐡄𐡍 (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		أرجع FAIL;
	    }
	}
	وإلا
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    أرجع FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    ساكن صحيح
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_stack(cctx, 0);
    صحيح		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    𐡄𐡍 (type->tt_type == VAR_OBJECT)
    {
	صحيح	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	𐡄𐡍 (uf != عدم)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    أرجع res;
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
    ساكن صحيح
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	صحيح	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    صحيح		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = عدم;
    char_u	*gname = عدم;
    ufunc_T	*ufunc = عدم;
    صحيح		res = FAIL;
    صحيح		is_autoload;
    صحيح		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    𐡄𐡍 (varlen >= حجم(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	أرجع FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    𐡄𐡍 (import != عدم)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	أرجع FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    𐡄𐡍 ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	صحيح	    is_has = **arg == 'h';
	صحيح	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	𐡄𐡍 (*s == '"')
	    (فراغ)eval_string(&s, &argvars[0], TRUE, FALSE);
	وإلا 𐡄𐡍 (*s == '\'')
	    (فراغ)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	𐡄𐡍 (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    𐡄𐡍 (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_NUMBER;
		tv->vval.v_number = 0;
		𐡄𐡍 (is_has)
		    f_has(argvars, tv);
		وإلا 𐡄𐡍 (is_len)
		    f_len(argvars, tv);
		وإلا
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    أرجع OK;
	}
	clear_tv(&argvars[0]);
	𐡄𐡍 (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    أرجع FAIL;
	}
    }

    𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
	أرجع FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    𐡄𐡍 ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    وإلا 𐡄𐡍 (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    وإلا
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    𐡄𐡍 (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	𐡄𐡍 (*arg == عدم)
	    اذهب theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	𐡄𐡍 (gname == عدم)
	    اذهب theend;
	++*arg;			// skip '('
    }
    وإلا
	*arg += varlen + 1;

    𐡄𐡍 (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	اذهب theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != عدم;
    𐡄𐡍 (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	صحيح	    idx;

	// builtin function
	idx = find_internal_func(name);
	𐡄𐡍 (idx >= 0)
	{
	    𐡄𐡍 (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		اذهب theend;
	    }

	    𐡄𐡍 (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_stack(cctx, 1);
		𐡄𐡍 (check_type_is_value(get_type_on_stack(cctx, 0)) == FAIL)
		    اذهب theend;

		// add() can be compiled to instructions if we know the type
		𐡄𐡍 (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		وإلا 𐡄𐡍 (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    𐡄𐡍 ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		𐡄𐡍 (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    𐡄𐡍 (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    وإلا 𐡄𐡍 (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    وإلا 𐡄𐡍 (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    𐡄𐡍 (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		𐡄𐡍 (res == OK)
		    idx = -1;
	    }

	    𐡄𐡍 (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	وإلا
	    emsg_funcname(e_unknown_function_str, namebuf);
	اذهب theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    𐡄𐡍 (lookup_local(namebuf, varlen, عدم, cctx) == FAIL
	    && arg_exists(namebuf, varlen, عدم, عدم, عدم, cctx) != OK)
    {
	class_T		*cl = عدم;
	صحيح		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	𐡄𐡍 (ufunc != عدم)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    𐡄𐡍 (ufunc == عدم)
		اذهب theend;

	    𐡄𐡍 (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, عدم, 0, argcount, FALSE);
		اذهب theend;
	    }
	    𐡄𐡍 (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == عدم
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		اذهب theend;
	    }
	}
	وإلا 𐡄𐡍 ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    𐡄𐡍 (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], عدم,
							0, argcount, FALSE);
	    }
	    وإلا
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    اذهب theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == عدم && gname != عدم ? gname : namebuf;
    𐡄𐡍 (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_stack(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	اذهب theend;
    }

    // If we can find a global function by name generate the right call.
    𐡄𐡍 (ufunc != عدم)
    {
	res = generate_CALL(cctx, ufunc, عدم, 0, argcount, FALSE);
	اذهب theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    𐡄𐡍 (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    وإلا
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    أرجع res;
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
to_name_end(char_u *arg, صحيح use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    𐡄𐡍 (!eval_isnamec1(*arg))
	أرجع arg;

    لكل (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	𐡄𐡍 (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == عدم))
	    اكسر;
    أرجع p;
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

    𐡄𐡍 (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    𐡄𐡍 (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	𐡄𐡍 (eval_list(&p, &rettv, عدم, FALSE) == FAIL)
	    p = arg;
    }
    أرجع p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    ساكن صحيح
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    صحيح		count = 0;
    صحيح		is_const;
    صحيح		is_all_const = TRUE;	// reset when non-const encountered
    صحيح		must_end = FALSE;

    لكل (;;)
    {
	𐡄𐡍 (may_get_next_line(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    أرجع FAIL;
	}
	𐡄𐡍 (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    أرجع FAIL;
	}
	𐡄𐡍 (*p == ']')
	{
	    ++p;
	    اكسر;
	}
	𐡄𐡍 (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    أرجع FAIL;
	}
	𐡄𐡍 (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    أرجع FAIL;
	𐡄𐡍 (!is_const)
	    is_all_const = FALSE;
	++count;
	𐡄𐡍 (*p == ',')
	{
	    ++p;
	    𐡄𐡍 (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		أرجع FAIL;
	    }
	}
	وإلا
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    أرجع generate_NEWLIST(cctx, count, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    ساكن صحيح
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    صحيح first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    صحيح		count = 0;
    صحيح		is_const;
    صحيح		is_all_const = TRUE;	// reset when non-const encountered
    صحيح		must_end = FALSE;

    𐡄𐡍 (**arg != ')')
    {
	𐡄𐡍 (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    أرجع FAIL;
	}
	count = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	لكل (;;)
	{
	    𐡄𐡍 (may_get_next_line(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		أرجع FAIL;
	    }
	    𐡄𐡍 (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		أرجع FAIL;
	    }
	    𐡄𐡍 (*p == ')')
	    {
		++p;
		اكسر;
	    }
	    𐡄𐡍 (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		أرجع FAIL;
	    }
	    𐡄𐡍 (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		أرجع FAIL;
	    𐡄𐡍 (!is_const)
		is_all_const = FALSE;
	    ++count;
	    𐡄𐡍 (*p == ',')
	    {
		++p;
		𐡄𐡍 (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    أرجع FAIL;
		}
	    }
	    وإلا
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    أرجع generate_NEWTUPLE(cctx, count, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    ساكن فراغ
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*line;
    size_t	off;

    𐡄𐡍 (!evalarg->eval_using_cmdline || cctx == عدم)
	أرجع;

    gap = &evalarg->eval_tofree_ga;
    𐡄𐡍 (gap->ga_len == 0)
	أرجع;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    line = ((char_u **)cctx->ctx_ufunc->uf_lines.ga_data)[cctx->ctx_lnum];
    *arg = line + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    صحيح
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحيح		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    𐡄𐡍 (r != OK)
    {
	clear_evalarg(&evalarg, عدم);
	أرجع r;
    }

    𐡄𐡍 (ppconst != عدم && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, عدم);
	أرجع FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    𐡄𐡍 (cctx->ctx_ufunc != عدم)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	صحيح block_depth = cctx->ctx_ufunc->uf_block_depth;

	𐡄𐡍 (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(صحيح, block_depth);
	    𐡄𐡍 (ufunc->uf_block_ids != عدم)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    حجم(صحيح) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    𐡄𐡍 (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    𐡄𐡍 (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    𐡄𐡍 (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	𐡄𐡍 (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, عدم);

    𐡄𐡍 (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	أرجع generate_FUNCREF(cctx, ufunc, عدم, FALSE, 0, عدم);
    }

    func_ptr_unref(ufunc);
    أرجع FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    صحيح
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	صحيح	    types_optional,
	evalarg_T   *evalarg)
{
    صحيح		r;
    ufunc_T	*ufunc;
    صحيح		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, عدم);
    current_sctx.sc_version = save_sc_version;
    𐡄𐡍 (r != OK)
	أرجع r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    𐡄𐡍 (ufunc->uf_ret_type == عدم || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, عدم);

    𐡄𐡍 (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	أرجع OK;
    }
    clear_tv(rettv);
    أرجع FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    ساكن صحيح
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    صحيح		count = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    صحيح		is_const;
    صحيح		is_all_const = TRUE;	// reset when non-const encountered

    𐡄𐡍 (d == عدم)
	أرجع FAIL;
    𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	أرجع FAIL;
    }
    لكل (;;)
    {
	char_u	    *key = عدم;

	𐡄𐡍 (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = عدم;
	    اذهب failret;
	}

	𐡄𐡍 (**arg == '}')
	    اكسر;

	𐡄𐡍 (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    𐡄𐡍 (compile_expr0(arg, cctx) == FAIL)
		اذهب failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    𐡄𐡍 (isn->isn_type == ISN_PUSHNR)
	    {
		حرف buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.number);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    𐡄𐡍 (isn->isn_type == ISN_PUSHS)
		key = isn->isn_arg.string;
	    وإلا 𐡄𐡍 (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		اذهب failret;
	    *arg = skipwhite(*arg);
	    𐡄𐡍 (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_key));
		اذهب failret;
	    }
	    ++*arg;
	}
	وإلا
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    key = get_literal_key(arg);
	    𐡄𐡍 (key == عدم)
		اذهب failret;
	    𐡄𐡍 (generate_PUSHS(cctx, &key) == FAIL)
		اذهب failret;
	}

	// Check for duplicate keys, if using string keys.
	𐡄𐡍 (key != عدم)
	{
	    item = dict_find(d, key, -1);
	    𐡄𐡍 (item != عدم)
	    {
		semsg(_(e_duplicate_key_in_dictionary_str), key);
		اذهب failret;
	    }
	    item = dictitem_alloc(key);
	    𐡄𐡍 (item != عدم)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		𐡄𐡍 (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	𐡄𐡍 (**arg != ':')
	{
	    𐡄𐡍 (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    وإلا
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    اذهب failret;
	}
	whitep = *arg + 1;
	𐡄𐡍 (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    اذهب failret;
	}

	𐡄𐡍 (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = عدم;
	    اذهب failret;
	}

	𐡄𐡍 (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    اذهب failret;
	𐡄𐡍 (!is_const)
	    is_all_const = FALSE;
	++count;

	whitep = *arg;
	𐡄𐡍 (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = عدم;
	    اذهب failret;
	}
	𐡄𐡍 (**arg == '}')
	    اكسر;
	𐡄𐡍 (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    اذهب failret;
	}
	𐡄𐡍 (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    اذهب failret;
	}
	whitep = *arg + 1;
	𐡄𐡍 (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    اذهب failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    𐡄𐡍 (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    أرجع generate_NEWDICT(cctx, count, FALSE);

failret:
    𐡄𐡍 (*arg == عدم)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    أرجع FAIL;
}

/*
 * Compile "&option".
 */
    ساكن صحيح
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    صحيح		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    𐡄𐡍 (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_NUMBER ? &t_number : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    أرجع ret;
}

/*
 * Compile "$VAR".
 */
    ساكن صحيح
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    صحيح		len;
    صحيح		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    𐡄𐡍 (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	أرجع FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    أرجع ret;
}

/*
 * Compile $"string" or $'string'.
 */
    ساكن صحيح
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    صحيح		ret;
    صحيح		quote;
    صحيح		evaluate = cctx->ctx_skip != SKIP_YES;
    صحيح		count = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    لكل (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	𐡄𐡍 (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	وإلا
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	𐡄𐡍 (ret == FAIL)
	    اكسر;
	𐡄𐡍 (evaluate)
	{
	    𐡄𐡍 ((tv.vval.v_string != عدم && *tv.vval.v_string != NUL)
		    || (**arg != '{' && count == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		𐡄𐡍 (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    أرجع FAIL;
		tv.vval.v_string = عدم;  // don't free it now
		++count;
	    }
	    clear_tv(&tv);
	}

	𐡄𐡍 (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    اكسر;
	}

	p = compile_one_expr_in_str(*arg, cctx);
	𐡄𐡍 (p == عدم)
	{
	    ret = FAIL;
	    اكسر;
	}
	++count;
	*arg = p;
    }

    𐡄𐡍 (ret == FAIL || !evaluate)
	أرجع ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    𐡄𐡍 (count > 1)
	أرجع generate_CONCAT(cctx, count);

    أرجع OK;
}

/*
 * Compile "@r".
 */
    ساكن صحيح
compile_get_register(char_u **arg, cctx_T *cctx)
{
    صحيح		ret;

    ++*arg;
    𐡄𐡍 (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	أرجع FAIL;
    }
    𐡄𐡍 (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	أرجع FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, عدم, &t_string);
    ++*arg;
    أرجع ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ساكن صحيح
apply_leader(typval_T *rettv, صحيح numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    طالما (p > start)
    {
	--p;
	𐡄𐡍 (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    𐡄𐡍 (rettv->v_type == VAR_FLOAT)
	    {
		𐡄𐡍 (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    وإلا
	    {
		varnumber_T	val;
		صحيح		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		𐡄𐡍 (check_not_string(rettv) == FAIL)
		    أرجع FAIL;
		val = tv_get_number_chk(rettv, &error);
		clear_tv(rettv);
		𐡄𐡍 (error)
		    أرجع FAIL;
		𐡄𐡍 (*p == '-')
		    val = -val;
		rettv->v_type = VAR_NUMBER;
		rettv->vval.v_number = val;
	    }
	}
	وإلا 𐡄𐡍 (numeric_only)
	{
	    ++p;
	    اكسر;
	}
	وإلا 𐡄𐡍 (*p == '!')
	{
	    صحيح v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_number = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    أرجع OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    ساكن فراغ
get_vim_constant(char_u **arg, typval_T *rettv)
{
    𐡄𐡍 (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_number = VVAL_TRUE;
	*arg += 6;
    }
    وإلا 𐡄𐡍 (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_number = VVAL_FALSE;
	*arg += 7;
    }
    وإلا 𐡄𐡍 (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_number = VVAL_NULL;
	*arg += 6;
    }
    وإلا 𐡄𐡍 (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_number = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, صحيح *len, صحيح *type_is)
{
    exprtype_T	type = EXPR_UNKNOWN;
    صحيح		i;

    بدل (p[0])
    {
	حالة '=':   𐡄𐡍 (p[1] == '=')
			type = EXPR_EQUAL;
		    وإلا 𐡄𐡍 (p[1] == '~')
			type = EXPR_MATCH;
		    اكسر;
	حالة '!':   𐡄𐡍 (p[1] == '=')
			type = EXPR_NEQUAL;
		    وإلا 𐡄𐡍 (p[1] == '~')
			type = EXPR_NOMATCH;
		    اكسر;
	حالة '>':   𐡄𐡍 (p[1] != '=')
		    {
			type = EXPR_GREATER;
			*len = 1;
		    }
		    وإلا
			type = EXPR_GEQUAL;
		    اكسر;
	حالة '<':   𐡄𐡍 (p[1] != '=')
		    {
			type = EXPR_SMALLER;
			*len = 1;
		    }
		    وإلا
			type = EXPR_SEQUAL;
		    اكسر;
	حالة 'i':   𐡄𐡍 (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			𐡄𐡍 (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			𐡄𐡍 (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? EXPR_IS : EXPR_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    اكسر;
    }
    أرجع type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    فراغ
skip_expr_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_expr(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, عدم);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    صحيح
bool_on_stack(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_stack(cctx, 0);
    𐡄𐡍 (type == &t_bool)
	أرجع OK;

    𐡄𐡍 (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_NUMBER
	    || type == &t_number_bool
	    || type == &t_const_number_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	أرجع generate_COND2BOOL(cctx);

    أرجع need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    فراغ
error_white_both(char_u *op, صحيح len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ساكن صحيح
compile_leader(cctx_T *cctx, صحيح numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    طالما (p > start)
    {
	--p;
	طالما (VIM_ISWHITE(*p))
	    --p;
	𐡄𐡍 (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_stack(cctx, 0);
	    𐡄𐡍 (type->tt_type != VAR_FLOAT && need_type(type, &t_number,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    𐡄𐡍 (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == عدم)
		أرجع FAIL;
	}
	وإلا 𐡄𐡍 (numeric_only)
	{
	    ++p;
	    اكسر;
	}
	وإلا
	{
	    صحيح  invert = *p == '!';

	    طالما (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		𐡄𐡍 (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    𐡄𐡍 (check_type_is_value(get_type_on_stack(cctx, 0)) == FAIL)
		أرجع FAIL;
	    𐡄𐡍 (generate_2BOOL(cctx, invert, -1) == FAIL)
		أرجع FAIL;
	}
    }
    *end = p;
    أرجع OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    ساكن صحيح
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحيح	    ret;
    char_u  *p = *arg + 1;

    𐡄𐡍 (may_get_next_line_error(p, arg, cctx) == FAIL)
	أرجع FAIL;

    𐡄𐡍 (**arg == ')')
	// empty tuple
	أرجع compile_tuple(arg, cctx, ppconst, FALSE);

    𐡄𐡍 (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    وإلا
    {
	// Not enough space in ppconst, flush constants.
	𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
	    أرجع FAIL;
	ret = compile_expr0(arg, cctx);
    }
    𐡄𐡍 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
	أرجع FAIL;
    𐡄𐡍 (ret == OK && **arg == ',')
    {
	// tuple
	صحيح is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
	    أرجع FAIL;
	أرجع compile_tuple(arg, cctx, ppconst, is_const);
    }

    𐡄𐡍 (**arg == ')')
	++*arg;
    وإلا 𐡄𐡍 (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    أرجع ret;
}

ساكن صحيح compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    ساكن صحيح
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    صحيح		keeping_dict = FALSE;

    لكل (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	𐡄𐡍 (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_line_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    𐡄𐡍 (next != عدم &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_line_from_context(cctx, TRUE);
		𐡄𐡍 (next == عدم)
		    أرجع FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	𐡄𐡍 (**arg == '(')
	{
	    صحيح		argcount = 0;

	    𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_stack(cctx, 0);

	    *arg = skipwhite(p + 1);
	    𐡄𐡍 (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		أرجع FAIL;
	    𐡄𐡍 (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		أرجع FAIL;
	    𐡄𐡍 (keeping_dict)
	    {
		keeping_dict = FALSE;
		𐡄𐡍 (generate_instr(cctx, ISN_CLEARDICT) == عدم)
		    أرجع FAIL;
	    }
	}
	وإلا 𐡄𐡍 (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    صحيح	    alt;
	    char_u  *paren;

	    // something->method()
	    𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_stack(cctx, 0);
	    𐡄𐡍 (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_value));
		أرجع FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    𐡄𐡍 (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		أرجع FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    𐡄𐡍 (**arg == '(')
		alt = 2;
	    وإلا
	    {
		// alternative 1 or 3
		p = *arg;
		𐡄𐡍 (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    أرجع FAIL;
		}
		𐡄𐡍 (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		لكل ( ; eval_isnamec(*p); ++p)
		    ;
		𐡄𐡍 (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    𐡄𐡍 (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			أرجع FAIL;
		}
		وإلا
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    𐡄𐡍 (paren == عدم)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			أرجع FAIL;
		    }
		}
	    }

	    𐡄𐡍 (alt != 1)
	    {
		صحيح	    argcount = 1;
		garray_T    *stack = &cctx->ctx_type_stack;
		صحيح	    type_idx_start = stack->ga_len;
		صحيح	    expr_isn_start = cctx->ctx_instr.ga_len;
		صحيح	    expr_isn_end;
		صحيح	    arg_isn_count;

		𐡄𐡍 (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    𐡄𐡍 (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			أرجع FAIL;
		}
		وإلا
		{
		    صحيح fail;
		    صحيح save_len = cctx->ctx_ufunc->uf_lines.ga_len;
		    صحيح	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_expr;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_lines.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_expr;
		    cctx->ctx_ufunc->uf_lines.ga_len = save_len;

		    𐡄𐡍 (fail)
		    {
			𐡄𐡍 (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			أرجع FAIL;
		    }
		}

		// Compile the arguments.
		𐡄𐡍 (**arg != '(')
		{
		    𐡄𐡍 (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    وإلا
			semsg(_(e_missing_parenthesis_str), *arg);
		    أرجع FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		expr_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		𐡄𐡍 (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    أرجع FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_count = cctx->ctx_instr.ga_len - expr_isn_end;
		𐡄𐡍 (arg_isn_count > 0)
		{
		    صحيح	    expr_isn_count = expr_isn_end - expr_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, expr_isn_count);
		    type_T  *decl_type;
		    type2_T  *typep;

		    𐡄𐡍 (isn == عدم)
			أرجع FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + expr_isn_start,
					       حجم(isn_T) * expr_isn_count);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + expr_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + expr_isn_end,
						حجم(isn_T) * arg_isn_count);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + expr_isn_start + arg_isn_count,
					  isn, حجم(isn_T) * expr_isn_count);
		    vim_free(isn);

		    typep = ((type2_T *)stack->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)stack->ga_data) + type_idx_start,
			      ((type2_T *)stack->ga_data) + type_idx_start + 1,
			      حجم(type2_T)
				       * (stack->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)stack->ga_data) + stack->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_stack(cctx, 0);
		𐡄𐡍 (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    أرجع FAIL;
	    }

	    𐡄𐡍 (keeping_dict)
	    {
		keeping_dict = FALSE;
		𐡄𐡍 (generate_instr(cctx, ISN_CLEARDICT) == عدم)
		    أرجع FAIL;
	    }
	}
	وإلا 𐡄𐡍 (**arg == '[')
	{
	    صحيح		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    𐡄𐡍 (may_get_next_line_error(p, arg, cctx) == FAIL)
		أرجع FAIL;
	    𐡄𐡍 (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    وإلا
	    {
		𐡄𐡍 (compile_expr0(arg, cctx) == FAIL)
		    أرجع FAIL;
		𐡄𐡍 (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    أرجع FAIL;
		}
		𐡄𐡍 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
		    أرجع FAIL;
		*arg = skipwhite(*arg);
	    }
	    𐡄𐡍 (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		𐡄𐡍 (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    أرجع FAIL;
		}
		𐡄𐡍 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
		    أرجع FAIL;
		𐡄𐡍 (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		وإلا
		{
		    𐡄𐡍 (compile_expr0(arg, cctx) == FAIL)
			أرجع FAIL;
		    𐡄𐡍 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
			أرجع FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    𐡄𐡍 (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		أرجع FAIL;
	    }
	    *arg = *arg + 1;

	    𐡄𐡍 (keeping_dict)
	    {
		keeping_dict = FALSE;
		𐡄𐡍 (generate_instr(cctx, ISN_CLEARDICT) == عدم)
		    أرجع FAIL;
	    }
	    𐡄𐡍 (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		أرجع FAIL;
	}
	وإلا 𐡄𐡍 (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_stack(cctx, 0);
	    𐡄𐡍 (type != &t_unknown
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
		𐡄𐡍 (compile_class_object_index(cctx, arg, type) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا
	    {
		*arg = p + 1;
		𐡄𐡍 (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    أرجع FAIL;
		}
		p = *arg;
		طالما (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		𐡄𐡍 (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    أرجع FAIL;
		}
		𐡄𐡍 (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == عدم)
		    أرجع FAIL;
		𐡄𐡍 (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    أرجع FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	وإلا
	    اكسر;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    𐡄𐡍 (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == عدم)
	أرجع FAIL;

    أرجع OK;
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
    ساكن صحيح
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    صحيح		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    صحيح		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    𐡄𐡍 (eval_leader(arg, TRUE) == FAIL)
	أرجع FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    بدل (**arg)
    {
	/*
	 * Number constant.
	 */
	حالة '0':	// also for blob starting with 0z
	حالة '1':
	حالة '2':
	حالة '3':
	حالة '4':
	حالة '5':
	حالة '6':
	حالة '7':
	حالة '8':
	حالة '9':
	حالة '.':   𐡄𐡍 (eval_number(arg, rettv, TRUE, FALSE) == FAIL)
			أرجع FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    𐡄𐡍 (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			أرجع FAIL;
		    }
		    اكسر;

	/*
	 * String constant: "string".
	 */
	حالة '"':   𐡄𐡍 (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			أرجع FAIL;
		    اكسر;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	حالة '\'':  𐡄𐡍 (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			أرجع FAIL;
		    اكسر;

	/*
	 * Constant Vim variable.
	 */
	حالة 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    اكسر;

	/*
	 * "true" constant
	 */
	حالة 't':   𐡄𐡍 (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_number = VVAL_TRUE;
		    }
		    وإلا
			ret = NOTDONE;
		    اكسر;

	/*
	 * "false" constant
	 */
	حالة 'f':   𐡄𐡍 (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_number = VVAL_FALSE;
		    }
		    وإلا
			ret = NOTDONE;
		    اكسر;

	/*
	 * "null" or "null_*" constant
	 */
	حالة 'n':   𐡄𐡍 (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			صحيح	len;

			لكل (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			𐡄𐡍 (ret == FAIL)
			    ret = NOTDONE;
			وإلا
			    *arg += len + 4;
		    }
		    وإلا
			ret = NOTDONE;
		    اكسر;

	/*
	 * List: [expr, expr]
	 */
	حالة '[':   𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
			أرجع FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    اكسر;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	حالة '{':   𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
			أرجع FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    اكسر;

	/*
	 * Option value: &name
	 */
	حالة '&':	𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
			    أرجع FAIL;
			ret = compile_get_option(arg, cctx);
			اكسر;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	حالة '$':	𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
			    أرجع FAIL;
			𐡄𐡍 ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			وإلا
			    ret = compile_get_env(arg, cctx);
			اكسر;

	/*
	 * Register contents: @r.
	 */
	حالة '@':	𐡄𐡍 (generate_ppconst(cctx, ppconst) == FAIL)
			    أرجع FAIL;
			ret = compile_get_register(arg, cctx);
			اكسر;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	حالة '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    𐡄𐡍 (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    اكسر;

	افتراضي:    ret = NOTDONE;
		    اكسر;
    }
    𐡄𐡍 (ret == FAIL)
	أرجع FAIL;

    𐡄𐡍 (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	𐡄𐡍 (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	وإلا
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    وإلا 𐡄𐡍 (ret == NOTDONE)
    {
	char_u	    *p;
	صحيح	    r;

	𐡄𐡍 (!eval_isnamec1(**arg))
	{
	    𐡄𐡍 (!vim9_bad_comment(*arg))
	    {
		𐡄𐡍 (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		وإلا
		    semsg(_(e_name_expected_str), *arg);
	    }
	    أرجع FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	𐡄𐡍 (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    أرجع FAIL;
	}

	size_t namelen = p - *arg;
	𐡄𐡍 (*p == '<')
	{
	    𐡄𐡍 (skip_generic_func_type_args(&p) == FAIL)
		أرجع FAIL;
	}

	𐡄𐡍 (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	وإلا
	{
	    𐡄𐡍 (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	𐡄𐡍 (r == FAIL)
	    أرجع FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    𐡄𐡍 (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	أرجع FAIL;
    𐡄𐡍 ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	𐡄𐡍 (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    أرجع FAIL;
	أرجع OK;
    }
    𐡄𐡍 (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	أرجع FAIL;
    أرجع OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    ساكن صحيح
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = عدم;

    // Recognize <type>
    𐡄𐡍 (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	𐡄𐡍 (want_type == عدم)
	    أرجع FAIL;

	𐡄𐡍 (**arg != '>')
	{
	    𐡄𐡍 (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    وإلا
		emsg(_(e_missing_gt));
	    أرجع FAIL;
	}
	++*arg;
	𐡄𐡍 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
	    أرجع FAIL;
    }

    𐡄𐡍 (compile_expr9(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    𐡄𐡍 (want_type != عدم)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_stack(cctx, 0);
	𐡄𐡍 (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    𐡄𐡍 (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		أرجع FAIL;
	}
    }

    أرجع OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    ساكن صحيح
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    صحيح		ppconst_used = ppconst->pp_used;

    // get the first expression
    𐡄𐡍 (compile_expr8(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    لكل (;;)
    {
	op = may_peek_next_line(cctx, *arg, &next);
	𐡄𐡍 (*op != '*' && *op != '/' && *op != '%')
	    اكسر;
	𐡄𐡍 (next != عدم)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	𐡄𐡍 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    أرجع FAIL;
	}
	𐡄𐡍 (may_get_next_line_error(op + 1, arg, cctx) == FAIL)
	    أرجع FAIL;

	// get the second expression
	𐡄𐡍 (compile_expr8(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	𐡄𐡍 (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_NUMBER
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_NUMBER)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    صحيح		    failed = FALSE;

	    // both are numbers: compute the result
	    بدل (*op)
	    {
		حالة '*': res = tv1->vval.v_number * tv2->vval.v_number;
			  اكسر;
		حالة '/': res = num_divide(tv1->vval.v_number,
						  tv2->vval.v_number, &failed);
			  اكسر;
		حالة '%': res = num_modulus(tv1->vval.v_number,
						  tv2->vval.v_number, &failed);
			  اكسر;
	    }
	    𐡄𐡍 (failed)
		أرجع FAIL;
	    tv1->vval.v_number = res;
	    --ppconst->pp_used;
	}
	وإلا
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    أرجع OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    ساكن صحيح
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    صحيح		oplen;
    صحيح		ppconst_used = ppconst->pp_used;

    // get the first variable
    𐡄𐡍 (compile_expr7(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    لكل (;;)
    {
	op = may_peek_next_line(cctx, *arg, &next);
	𐡄𐡍 (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    اكسر;
	𐡄𐡍 (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    اكسر;
	oplen = (*op == '.' ? 2 : 1);
	𐡄𐡍 (next != عدم)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	𐡄𐡍 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    أرجع FAIL;
	}

	𐡄𐡍 (may_get_next_line_error(op + oplen, arg, cctx) == FAIL)
	    أرجع FAIL;

	// get the second expression
	𐡄𐡍 (compile_expr7(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	𐡄𐡍 (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_NUMBER
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_NUMBER)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    𐡄𐡍 (*op == '+')
		tv1->vval.v_number = tv1->vval.v_number + tv2->vval.v_number;
	    وإلا 𐡄𐡍 (*op == '-')
		tv1->vval.v_number = tv1->vval.v_number - tv2->vval.v_number;
	    وإلا
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		𐡄𐡍 (tv1->vval.v_string == عدم)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    أرجع FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	وإلا
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    𐡄𐡍 (*op == '.')
	    {
		𐡄𐡍 (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    أرجع FAIL;
		𐡄𐡍 (generate_CONCAT(cctx, 2) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا
		generate_two_op(cctx, op);
	}
    }

    أرجع OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    ساكن صحيح
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = EXPR_UNKNOWN;
    char_u	*p;
    char_u	*next;
    صحيح		len = 2;
    صحيح		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    𐡄𐡍 (compile_expr6(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    لكل (;;)
    {
	type = EXPR_UNKNOWN;

	p = may_peek_next_line(cctx, *arg, &next);
	𐡄𐡍 (p[0] == '<' && p[1] == '<')
	    type = EXPR_LSHIFT;
	وإلا 𐡄𐡍 (p[0] == '>' && p[1] == '>')
	    type = EXPR_RSHIFT;

	𐡄𐡍 (type == EXPR_UNKNOWN)
	    أرجع OK;

	// Handle a bitwise left or right shift operator
	𐡄𐡍 (ppconst->pp_used == ppconst_used + 1)
	{
	    𐡄𐡍 (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_NUMBER)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_number));
		أرجع FAIL;
	    }
	}
	وإلا
	{
	    type_T	*t = get_type_on_stack(cctx, 0);

	    𐡄𐡍 (need_type(t, &t_number, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_number));
		أرجع FAIL;
	    }
	}

	𐡄𐡍 (next != عدم)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	𐡄𐡍 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    أرجع FAIL;
	}

	// get the second variable
	𐡄𐡍 (may_get_next_line_error(p + len, arg, cctx) == FAIL)
	    أرجع FAIL;

	𐡄𐡍 (compile_expr6(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	𐡄𐡍 (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    𐡄𐡍 (tv2->v_type != VAR_NUMBER || tv2->vval.v_number < 0)
	    {
		// right operand should be a positive number
		𐡄𐡍 (tv2->v_type != VAR_NUMBER)
		    emsg(_(e_bitshift_ops_must_be_number));
		وإلا
		    emsg(_(e_bitshift_ops_must_be_positive));
		أرجع FAIL;
	    }

	    𐡄𐡍 (tv2->vval.v_number > MAX_LSHIFT_BITS)
		tv1->vval.v_number = 0;
	    وإلا 𐡄𐡍 (type == EXPR_LSHIFT)
		tv1->vval.v_number =
			(uvarnumber_T)tv1->vval.v_number << tv2->vval.v_number;
	    وإلا
		tv1->vval.v_number =
			(uvarnumber_T)tv1->vval.v_number >> tv2->vval.v_number;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	وإلا
	{
	    𐡄𐡍 (need_type(get_type_on_stack(cctx, 0), &t_number, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_number));
		أرجع FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    𐡄𐡍 (isn == عدم)
		أرجع FAIL;

	    𐡄𐡍 (isn != عدم)
		isn->isn_arg.op.op_type = type;
	}
    }

    أرجع OK;
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
    ساكن صحيح
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = EXPR_UNKNOWN;
    char_u	*p;
    char_u	*next;
    صحيح		len = 2;
    صحيح		type_is = FALSE;
    صحيح		ppconst_used = ppconst->pp_used;

    // get the first variable
    𐡄𐡍 (compile_expr5(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    p = may_peek_next_line(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    𐡄𐡍 (type != EXPR_UNKNOWN)
    {
	صحيح ic = FALSE;  // Default: do not ignore case

	𐡄𐡍 (next != عدم)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	𐡄𐡍 (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    أرجع FAIL;
	}
	// extra question mark appended: ignore case
	𐡄𐡍 (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	وإلا 𐡄𐡍 (p[len] == '#')
	    ++len;
	// nothing appended: match case

	𐡄𐡍 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    أرجع FAIL;
	}

	// get the second variable
	𐡄𐡍 (may_get_next_line_error(p + len, arg, cctx) == FAIL)
	    أرجع FAIL;

	𐡄𐡍 (compile_expr5(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	𐡄𐡍 (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    صحيح		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    𐡄𐡍 (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    وإلا
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_number = tv1->vval.v_number
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    أرجع ret;
	}

	generate_ppconst(cctx, ppconst);
	أرجع generate_COMPARE(cctx, type, ic);
    }

    أرجع OK;
}

ساكن صحيح compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    ساكن صحيح
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	حرف	*op,
	ppconst_T *ppconst,
	صحيح	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_line(cctx, *arg, &next);
    صحيح		opchar = *op;

    𐡄𐡍 (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	صحيح		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, حجم(صحيح), 10);
	طالما (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    صحيح		start_ctx_lnum = cctx->ctx_lnum;
	    صحيح		save_lnum;
	    صحيح		const_used;
	    صحيح		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    𐡄𐡍 (next != عدم)
	    {
		*arg = next_line_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    𐡄𐡍 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		أرجع FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    𐡄𐡍 (status != FAIL)
	    {
		// Use the last ppconst if possible.
		𐡄𐡍 (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    صحيح		is_true = tv2bool(tv);

		    𐡄𐡍 ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_number = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    وإلا
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		وإلا
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_stack(cctx);
		}
	    }
	    𐡄𐡍 (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    𐡄𐡍 (status == FAIL)
	    {
		ga_clear(&end_ga);
		أرجع FAIL;
	    }

	    𐡄𐡍 (jump_when != JUMP_NEVER)
	    {
		𐡄𐡍 (cctx->ctx_skip != SKIP_YES)
		{
		    *(((صحيح *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    𐡄𐡍 (may_get_next_line_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		أرجع FAIL;
	    }

	    const_used = ppconst->pp_used;
	    𐡄𐡍 ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		أرجع FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    𐡄𐡍 (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		𐡄𐡍 (tv->v_type == VAR_NUMBER
			 && (tv->vval.v_number == 1 || tv->vval.v_number == 0))
		{
		    tv->vval.v_number = tv->vval.v_number == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_line(cctx, *arg, &next);
	}

	𐡄𐡍 (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    أرجع FAIL;
	}

	𐡄𐡍 (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    𐡄𐡍 (bool_on_stack(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		أرجع FAIL;
	    }

	𐡄𐡍 (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    طالما (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((صحيح *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    أرجع OK;
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
    ساكن صحيح
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحيح		ppconst_used = ppconst->pp_used;

    // get the first variable
    𐡄𐡍 (compile_expr4(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    // || and && work almost the same
    أرجع compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    ساكن صحيح
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحيح		ppconst_used = ppconst->pp_used;

    // eval the first expression
    𐡄𐡍 (compile_expr3(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    // || and && work almost the same
    أرجع compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    صحيح
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    صحيح		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    𐡄𐡍 (cctx->ctx_skip == SKIP_YES)
    {
	صحيح		prev_did_emsg = did_emsg;

	skip_expr_cctx(arg, cctx);
	أرجع did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    𐡄𐡍 (compile_expr2(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    p = may_peek_next_line(cctx, *arg, &next);
    𐡄𐡍 (*p == '?')
    {
	صحيح		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*stack = &cctx->ctx_type_stack;
	صحيح		alt_idx = instr->ga_len;
	صحيح		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = عدم;
	صحيح		has_const_expr = FALSE;
	صحيح		const_value = FALSE;
	صحيح		save_skip = cctx->ctx_skip;

	𐡄𐡍 (next != عدم)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	𐡄𐡍 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    أرجع FAIL;
	}

	𐡄𐡍 (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_expr = TRUE;
	    𐡄𐡍 (op_falsy)
		const_value = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    وإلا
	    {
		صحيح error = FALSE;

		const_value = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		𐡄𐡍 (error)
		    أرجع FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_value : !const_value) ? SKIP_YES : SKIP_NOT;

	    𐡄𐡍 (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    وإلا
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	وإلا
	{
	    generate_ppconst(cctx, ppconst);
	    𐡄𐡍 (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    𐡄𐡍 (op_falsy)
	    {
		type1 = get_type_on_stack(cctx, -1);
		𐡄𐡍 (check_type_is_value(type1) == FAIL)
		    أرجع FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	𐡄𐡍 (may_get_next_line_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    أرجع FAIL;
	𐡄𐡍 (compile_expr1(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	𐡄𐡍 (!has_const_expr)
	{
	    generate_ppconst(cctx, ppconst);

	    𐡄𐡍 (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_stack(cctx, 0);
		--stack->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	𐡄𐡍 (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_line(cctx, *arg, &next);
	    𐡄𐡍 (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		أرجع FAIL;
	    }
	    𐡄𐡍 (next != عدم)
	    {
		*arg = next_line_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    𐡄𐡍 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		أرجع FAIL;
	    }

	    // evaluate the third expression
	    𐡄𐡍 (has_const_expr)
		cctx->ctx_skip = save_skip == SKIP_YES || const_value
							 ? SKIP_YES : SKIP_NOT;
	    𐡄𐡍 (may_get_next_line_error(p + 1, arg, cctx) == FAIL)
		أرجع FAIL;
	    𐡄𐡍 (compile_expr1(arg, cctx, ppconst) == FAIL)
		أرجع FAIL;
	}

	𐡄𐡍 (!has_const_expr)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)stack->ga_data)
					      + stack->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    أرجع OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    صحيح
compile_expr0_ext(char_u **arg,  cctx_T *cctx, صحيح *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    𐡄𐡍 (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	أرجع FAIL;
    }
    𐡄𐡍 (is_const != عدم)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    𐡄𐡍 (generate_ppconst(cctx, &ppconst) == FAIL)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Toplevel expression.
 */
    صحيح
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    أرجع compile_expr0_ext(arg, cctx, عدم);
}


#endif // defined(FEAT_EVAL)
