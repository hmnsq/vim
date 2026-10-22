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
#include "vim.h"

#ប្រសិនបើ defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
static ចំនួនគត់ paren_follows_after_expr = 0;

/*
 * Generate code for any ppconst entries.
 */
    ចំនួនគត់
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    ចំនួនគត់	    i;
    ចំនួនគត់	    ret = OK;
    ចំនួនគត់	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    សម្រាប់ (i = 0; i < ppconst->pp_used; ++i)
	ប្រសិនបើ (generate_tv_PUSH(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    ត្រឡប់ ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    static ចំនួនគត់
check_ppconst_bool(ppconst_T *ppconst)
{
    ប្រសិនបើ (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	ត្រឡប់ check_typval_type(&t_bool, tv, where);
    }
    ត្រឡប់ OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    void
clear_ppconst(ppconst_T *ppconst)
{
    ចំនួនគត់	    i;

    សម្រាប់ (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    static ចំនួនគត់
compile_tuple_member(
    type2_T	*typep,
    ចំនួនគត់		is_slice,
    cctx_T	*cctx)
{
    ប្រសិនបើ (is_slice)
    {
	ប្រសិនបើ (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    ត្រឡប់ FAIL;
	// a copy is made so the member type is no longer declared
	ប្រសិនបើ (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	ប្រសិនបើ (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    ប្រសិនបើ (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    បើមិនដូច្នេះ
    {
	ប្រសិនបើ (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    ប្រសិនបើ (typep->type_curr->tt_argcount == 1)
	    {
		ប្រសិនបើ (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		បើមិនដូច្នេះ
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    បើមិនដូច្នេះ
		typep->type_curr = &t_any;
	    ប្រសិនបើ (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		ប្រសិនបើ (typep->type_decl->tt_argcount == 1)
		{
		    ប្រសិនបើ (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    បើមិនដូច្នេះ
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		បើមិនដូច្នេះ
		    typep->type_curr = &t_any;
	    }
	    បើមិនដូច្នេះ
		typep->type_decl = typep->type_curr;
	}
	ប្រសិនបើ (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    ត្រឡប់ FAIL;
    }

    ត្រឡប់ OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    ចំនួនគត់
compile_member(ចំនួនគត់ is_slice, ចំនួនគត់ *keeping_dict, cctx_T *cctx)
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
    ប្រសិនបើ ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    ប្រសិនបើ (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	ប្រសិនបើ (need_type(idxtype, &t_number, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ត្រឡប់ FAIL;
	ប្រសិនបើ (is_slice)
	{
	    idxtype = get_type_on_stack(cctx, 1);
	    ប្រសិនបើ (need_type(idxtype, &t_number, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ត្រឡប់ FAIL;
	}
    }

    ប្រសិនបើ (vartype == VAR_DICT)
    {
	ប្រសិនបើ (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    ប្រសិនបើ (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    ប្រសិនបើ (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		ប្រសិនបើ (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    បើមិនដូច្នេះ
		typep->type_decl = typep->type_curr;
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ត្រឡប់ FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	ប្រសិនបើ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    ត្រឡប់ FAIL;
	ប្រសិនបើ (keeping_dict != NULL)
	    *keeping_dict = TRUE;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	ប្រសិនបើ ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (vartype == VAR_BLOB)
    {
	ប្រសិនបើ (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    ប្រសិនបើ (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ
	{
	    typep->type_curr = &t_number;
	    typep->type_decl = &t_number;
	    ប្រសិនបើ (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		ត្រឡប់ FAIL;
	}
    }
    បើមិនដូច្នេះ ប្រសិនបើ (vartype == VAR_TUPLE)
    {
	ប្រសិនបើ (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    ត្រឡប់ FAIL;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	ប្រសិនបើ (is_slice)
	{
	    ប្រសិនបើ (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		ត្រឡប់ FAIL;
	    // a copy is made so the member type is no longer declared
	    ប្រសិនបើ (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    ប្រសិនបើ (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		ប្រសិនបើ (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		ប្រសិនបើ (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		ប្រសិនបើ (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    ប្រសិនបើ (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		បើមិនដូច្នេះ
			typep->type_decl = typep->type_curr;
	    }
	    ប្រសិនបើ (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		ត្រឡប់ FAIL;
	}
    }
    បើមិនដូច្នេះ
    {
	switch (vartype)
	{
	    pak_sa VAR_FUNC:
	    pak_sa VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		bha_nga;
	    pak_sa VAR_BOOL:
	    pak_sa VAR_SPECIAL:
	    pak_sa VAR_JOB:
	    pak_sa VAR_CHANNEL:
	    pak_sa VAR_INSTR:
	    pak_sa VAR_CLASS:
	    pak_sa VAR_OBJECT:
	    pak_sa VAR_TYPEALIAS:
	    pak_sa VAR_UNKNOWN:
	    pak_sa VAR_ANY:
	    pak_sa VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		bha_nga;
	    default:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	ត្រឡប់ FAIL;
    }
    ត្រឡប់ OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    static ចំនួនគត់
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    សម្រាប់ (cctx_T *cctx = cctx_arg; cctx != NULL; cctx = cctx->ctx_outer)
    {
	ប្រសិនបើ (cctx->ctx_ufunc != NULL && cctx->ctx_ufunc->uf_class != NULL)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    ខណៈពេល (clp != NULL)
	    {
		ប្រសិនបើ (clp == cl)
		    ត្រឡប់ TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    ត្រឡប់ FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    ចំនួនគត់
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    ចំនួនគត់		m_idx;
    ចំនួនគត់		ret = FAIL;

    ប្រសិនបើ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ត្រឡប់ FAIL;
    }

    class_T *cl = type->tt_class;
    ចំនួនគត់ is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    ប្រសិនបើ (type == &t_super)
    {
	ប្រសិនបើ (cctx->ctx_ufunc == NULL || cctx->ctx_ufunc->uf_class == NULL)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    ត្រឡប់ FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_stack.ga_len;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	ប្រសិនបើ (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    ប្រសិនបើ (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    ប្រសិនបើ (cl == NULL)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	ត្រឡប់ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, NULL, NULL, FNE_CHECK_START);
    ប្រសិនបើ (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	ត្រឡប់ FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    ប្រសិនបើ (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	ប្រសិនបើ (name_end == NULL)
	    goto done;
    }

    ប្រសិនបើ (*name_end == '(')
    {
	ចំនួនគត់	function_count;
	ចំនួនគត់	child_count;
	ufunc_T	**functions;

	ប្រសិនបើ (type->tt_type == VAR_CLASS)
	{
	    function_count = cl->class_class_function_count;
	    child_count = cl->class_class_function_count_child;
	    functions = cl->class_class_functions;
	}
	បើមិនដូច្នេះ
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    ប្រសិនបើ (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_count = cl->class_obj_method_count;
	    child_count = cl->class_obj_method_count_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = NULL;
	ចំនួនគត់ fi;
	សម្រាប់ (fi = is_super ? child_count : 0; fi < function_count; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    ប្រសិនបើ (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		bha_nga;
	    }
	}

	ocmember_T  *ocm = NULL;
	ប្រសិនបើ (ufunc == NULL)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    ប្រសិនបើ (ocm == NULL || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_msg(cl, type->tt_type, name, len);
		goto done;
	    }
	    ប្រសិនបើ (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_stack.ga_len;
		ប្រសិនបើ (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    goto done;
	    }
	    បើមិនដូច្នេះ
	    {
		ចំនួនគត់ status;

		ប្រសិនបើ (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		បើមិនដូច្នេះ
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		ប្រសិនបើ (status == FAIL)
		    goto done;
	    }
	}

	ប្រសិនបើ (is_super && ufunc != NULL && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    goto done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	ប្រសិនបើ (ocm == NULL && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    goto done;
	}

	// process generic function call
	ប្រសិនបើ (ufunc != NULL)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ប្រសិនបើ (ufunc == NULL)
		goto done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	ចំនួនគត់ argcount = 0;
	ប្រសិនបើ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    goto done;

	ប្រសិនបើ (ocm != NULL)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    goto done;
	}
	ប្រសិនបើ (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	បើមិនដូច្នេះ
	    ret = generate_CALL(cctx, ufunc, NULL, 0, argcount, FALSE);
	goto done;
    }

    ប្រសិនបើ (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	ប្រសិនបើ (m_idx >= 0)
	{
	    ប្រសិនបើ (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		goto done;
	    }

	    *arg = name_end;
	    ប្រសិនបើ (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    បើមិនដូច្នេះ
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    goto done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	ប្រសិនបើ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    ប្រសិនបើ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		goto done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_stack.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, NULL);
	    goto done;
	}

	member_not_found_msg(cl, VAR_OBJECT, name, len);
    }
    បើមិនដូច្នេះ
    {
	// load class member
	ចំនួនគត់ idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	ប្រសិនបើ (m != NULL)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    ប្រសិនបើ (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		goto done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_stack.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    goto done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	ប្រសិនបើ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    ប្រសិនបើ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		goto done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_stack.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, NULL);
	    goto done;
	}

	member_not_found_msg(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    ត្រឡប់ ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    ចំនួនគត់
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    ចំនួនគត់		    idx;
    imported_T	    *imp;

    ប្រសិនបើ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ត្រឡប់ FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, NULL);
    ប្រសិនបើ (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	ត្រឡប់ OK;
    }

    ប្រសិនបើ (end == NULL)
	imp = NULL;
    បើមិនដូច្នេះ ប្រសិនបើ (import == NULL)
	imp = find_imported(name, 0, FALSE);
    បើមិនដូច្នេះ
	imp = import;

    ប្រសិនបើ (imp != NULL)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	ចំនួនគត់	cc;
	ufunc_T	*ufunc = NULL;
	type_T	*type;
	ចំនួនគត់	done = FALSE;
	ចំនួនគត់	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	ប្រសិនបើ (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    ត្រឡប់ FAIL;
	}
	++p;
	ប្រសិនបើ (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    ត្រឡប់ FAIL;
	}

	// isolate one name
	exp_name = p;
	ខណៈពេល (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	ប្រសិនបើ (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	ប្រសិនបើ (res == OK)
	{
	    ប្រសិនបើ (si->sn_autoload_prefix != NULL
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		ប្រសិនបើ (cc == '(' || paren_follows_after_expr)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		បើមិនដូច្នេះ
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		ប្រសិនបើ (cc == '(' || paren_follows_after_expr)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		បើមិនដូច្នេះ
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    បើមិនដូច្នេះ
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, NULL, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	ប្រសិនបើ (done)
	    ត្រឡប់ res;

	ប្រសិនបើ (idx < 0)
	{
	    ប្រសិនបើ (ufunc != NULL)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		ប្រសិនបើ (IS_GENERIC_FUNC(ufunc))
		{
		    ប្រសិនបើ (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			ប្រសិនបើ (p != NULL)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			ប្រសិនបើ (p == NULL || ufunc == NULL)
			    ត្រឡប់ FAIL;
		    }
		    បើមិនដូច្នេះ
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			ត្រឡប់ FAIL;
		    }
		}
		បើមិនដូច្នេះ ប្រសិនបើ (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    ត្រឡប់ FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, NULL, TRUE);
		ត្រឡប់ OK;
	    }
	    ត្រឡប់ FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	ត្រឡប់ OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    ត្រឡប់ generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    static ចំនួនគត់
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    ចំនួនគត់			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    ប្រសិនបើ (ufunc == NULL || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	ត្រឡប់ FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    ប្រសិនបើ (ufunc == NULL)
	ត្រឡប់ FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    ប្រសិនបើ (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, NULL) == FAIL)
	ត្រឡប់ FAIL;
    ត្រឡប់ generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    static ចំនួនគត់
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    ត្រឡប់ cctx->ctx_ufunc != NULL
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == NULL
		|| cctx->ctx_outer->ctx_ufunc == NULL
		|| cctx->ctx_outer->ctx_ufunc->uf_class == NULL
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
    static ចំនួនគត់
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    ចំនួនគត់			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    ចំនួនគត់		isn_idx;

    ប្រសិនបើ (fp == NULL)
	ត្រឡប់ FAIL;

    សម្រាប់ (ចំនួនគត់ depth = 0; depth < outer_depth; ++depth)
    {
	ប្រសិនបើ (outer_cctx->ctx_outer == NULL)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    ត្រឡប់ FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    ប្រសិនបើ (fp->uf_block_ids == NULL && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(ចំនួនគត់, ufunc->uf_block_depth);
	ប្រសិនបើ (fp->uf_block_ids != NULL)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    parimaa_na(ចំនួនគត់) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    ប្រសិនបើ (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (generate_FUNCREF(cctx, fp, NULL, FALSE, 0, &isn_idx) == FAIL)
	ត្រឡប់ FAIL;
    ប្រសិនបើ (cctx->ctx_skip == SKIP_YES)
	ត្រឡប់ OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    ប្រសិនបើ (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	ប្រសិនបើ (extra == NULL)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    ប្រសិនបើ (extra == NULL)
		ត្រឡប់ FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    ត្រឡប់ OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    ចំនួនគត់
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	ចំនួនគត់	is_expr,
	ចំនួនគត់	error)
{
    type_T	*type;
    char_u	*name = NULL;
    char_u	*end = end_arg;
    ចំនួនគត់		res = FAIL;
    ចំនួនគត់		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = NULL;

    generic_func_args_table_init(&gfatab);

    ប្រសិនបើ (*(*arg + namelen) == '<')
    {
	// generic function call
	ប្រសិនបើ (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == NULL)
	{
	    generic_func_args_table_clear(&gfatab);
	    ត្រឡប់ FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    ប្រសិនបើ (*(*arg + 1) == ':')
    {
	ប្រសិនបើ (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    switch (**arg)
	    {
		pak_sa 'g': isn_type = ISN_LOADGDICT; bha_nga;
		pak_sa 'w': isn_type = ISN_LOADWDICT; bha_nga;
		pak_sa 't': isn_type = ISN_LOADTDICT; bha_nga;
		pak_sa 'b': isn_type = ISN_LOADBDICT; bha_nga;
		default:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    goto theend;
	    }
	    ប្រសិនបើ (generate_instr_type(cctx, isn_type, &t_dict_any) == NULL)
		goto theend;
	    res = OK;
	}
	បើមិនដូច្នេះ
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    ប្រសិនបើ (name == NULL)
		goto theend;

	    switch (**arg)
	    {
		pak_sa 'v': res = generate_LOADV(cctx, name);
			  bha_nga;
		pak_sa 's': ប្រសិនបើ (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      goto theend;
			  }
			  ប្រសិនបើ (is_expr && find_func(name, FALSE) != NULL)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  បើមិនដូច្នេះ
			      res = compile_load_scriptvar(cctx, name,
							    NULL, &end, NULL);
			  bha_nga;
		pak_sa 'g': ប្រសិនបើ (vim_strchr(name, AUTOLOAD_CHAR) == NULL)
			  {
			      ប្រសិនបើ (is_expr && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != NULL
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      បើមិនដូច្នេះ
				  isn_type = ISN_LOADG;
			  }
			  បើមិនដូច្នេះ
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      ប្រសិនបើ (name == NULL)
				  goto theend;
			  }
			  bha_nga;
		pak_sa 'w': isn_type = ISN_LOADW; bha_nga;
		pak_sa 't': isn_type = ISN_LOADT; bha_nga;
		pak_sa 'b': isn_type = ISN_LOADB; bha_nga;
		default:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  goto theend;
	    }
	    ប្រសិនបើ (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    បើមិនដូច្នេះ
    {
	ចំនួនគត់	    idx;
	ចំនួនគត់	    method_idx;
	ចំនួនគត់	    gen_load = FALSE;
	ចំនួនគត់	    gen_load_outer = 0;
	ចំនួនគត់	    outer_loop_depth = -1;
	ចំនួនគត់	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	ប្រសិនបើ (name == NULL)
	    goto theend;

	ប្រសិនបើ (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = push_type_stack(cctx, &t_super);
	    ប្រសិនបើ (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	បើមិនដូច្នេះ ប្រសិនបើ (vim_strchr(name, AUTOLOAD_CHAR) != NULL)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    ប្រសិនបើ (gen_load_outer == 0)
		gen_load = TRUE;
	}
	បើមិនដូច្នេះ
	{
	    lvar_T  lvar;
	    class_T *cl = NULL;

	    ប្រសិនបើ (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		ប្រសិនបើ (lvar.lv_ufunc != NULL && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		បើមិនដូច្នេះ ប្រសិនបើ (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		បើមិនដូច្នេះ
		    gen_load = TRUE;
	    }
	    បើមិនដូច្នេះ ប្រសិនបើ (cctx->ctx_ufunc->uf_defclass != NULL &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		ប្រសិនបើ (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    ប្រសិនបើ (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    បើមិនដូច្នេះ
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									NULL);
		    }
		}
		បើមិនដូច្នេះ
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    បើមិនដូច្នេះ
	    {
		imported_T *imp = NULL;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		ប្រសិនបើ (script_var_exists(*arg, namelen, cctx, NULL) == OK
			    || (imp = find_imported(name, 0, FALSE)) != NULL)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		ប្រសិនបើ (res == FAIL && is_expr && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	ប្រសិនបើ (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, NULL, type);
	ប្រសិនបើ (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    ប្រសិនបើ (lt != NULL)
	*lt = '<';
    ប្រសិនបើ (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    ត្រឡប់ res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    static ចំនួនគត់
compile_string(isn_T *isn, cctx_T *cctx, ចំនួនគត់ str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    ចំនួនគត់		expr_res;
    ចំនួនគត់		trailing_error;
    ចំនួនគត់		instr_count;
    isn_T	*instr = NULL;

    // Remove the string type from the stack.
    --cctx->ctx_type_stack.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = NULL;

    // avoid peeking a next line
    ចំនួនគត់ galen_save = cctx->ctx_ufunc->uf_lines.ga_len;
    cctx->ctx_ufunc->uf_lines.ga_len = 0;

    expr_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_lines.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    ប្រសិនបើ (expr_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	ប្រសិនបើ (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_stack.ga_len;
	ត្រឡប់ FAIL;
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
    ត្រឡប់ OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    ចំនួនគត់
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	ចំនួនគត់	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    ចំនួនគត់	    must_end = FALSE;
    ចំនួនគត់	    instr_count;

    សម្រាប់ (;;)
    {
	ប្រសិនបើ (may_get_next_line(whitep, &p, cctx) == FAIL)
	    goto failret;
	ប្រសិនបើ (*p == ')')
	{
	    *arg = p + 1;
	    ត្រឡប់ OK;
	}
	ប្រសិនបើ (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    ត្រឡប់ FAIL;
	}

	instr_count = cctx->ctx_instr.ga_len;
	ប្រសិនបើ (compile_expr0(&p, cctx) == FAIL)
	    ត្រឡប់ FAIL;
	++*argcount;

	ប្រសិនបើ (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_count + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_count;

	    // {skip} argument of searchpair() can be compiled if not empty
	    ប្រសិនបើ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != NULL
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	បើមិនដូច្នេះ ប្រសិនបើ (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_count + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_count;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    ប្រសិនបើ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != NULL
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	ប្រសិនបើ (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (*p == ',')
	{
	    ++p;
	    ប្រសិនបើ (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ត្រឡប់ FAIL;
	    }
	}
	បើមិនដូច្នេះ
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    ត្រឡប់ FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    static ចំនួនគត់
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_stack(cctx, 0);
    ចំនួនគត់		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    ប្រសិនបើ (type->tt_type == VAR_OBJECT)
    {
	ចំនួនគត់	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	ប្រសិនបើ (uf != NULL)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    ត្រឡប់ res;
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
    static ចំនួនគត់
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	ចំនួនគត់	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    ចំនួនគត់		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = NULL;
    char_u	*gname = NULL;
    ufunc_T	*ufunc = NULL;
    ចំនួនគត់		res = FAIL;
    ចំនួនគត់		is_autoload;
    ចំនួនគត់		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    ប្រសិនបើ (varlen >= parimaa_na(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	ត្រឡប់ FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    ប្រសិនបើ (import != NULL)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	ត្រឡប់ FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    ប្រសិនបើ ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	ចំនួនគត់	    is_has = **arg == 'h';
	ចំនួនគត់	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	ប្រសិនបើ (*s == '"')
	    (void)eval_string(&s, &argvars[0], TRUE, FALSE);
	បើមិនដូច្នេះ ប្រសិនបើ (*s == '\'')
	    (void)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	ប្រសិនបើ (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    ប្រសិនបើ (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_NUMBER;
		tv->vval.v_number = 0;
		ប្រសិនបើ (is_has)
		    f_has(argvars, tv);
		បើមិនដូច្នេះ ប្រសិនបើ (is_len)
		    f_len(argvars, tv);
		បើមិនដូច្នេះ
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    ត្រឡប់ OK;
	}
	clear_tv(&argvars[0]);
	ប្រសិនបើ (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    ត្រឡប់ FAIL;
	}
    }

    ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
	ត្រឡប់ FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    ប្រសិនបើ ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    បើមិនដូច្នេះ ប្រសិនបើ (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    បើមិនដូច្នេះ
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    ប្រសិនបើ (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	ប្រសិនបើ (*arg == NULL)
	    goto theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	ប្រសិនបើ (gname == NULL)
	    goto theend;
	++*arg;			// skip '('
    }
    បើមិនដូច្នេះ
	*arg += varlen + 1;

    ប្រសិនបើ (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	goto theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != NULL;
    ប្រសិនបើ (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	ចំនួនគត់	    idx;

	// builtin function
	idx = find_internal_func(name);
	ប្រសិនបើ (idx >= 0)
	{
	    ប្រសិនបើ (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		goto theend;
	    }

	    ប្រសិនបើ (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_stack(cctx, 1);
		ប្រសិនបើ (check_type_is_value(get_type_on_stack(cctx, 0)) == FAIL)
		    goto theend;

		// add() can be compiled to instructions if we know the type
		ប្រសិនបើ (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		បើមិនដូច្នេះ ប្រសិនបើ (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    ប្រសិនបើ ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		ប្រសិនបើ (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    ប្រសិនបើ (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    បើមិនដូច្នេះ ប្រសិនបើ (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    ប្រសិនបើ (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		ប្រសិនបើ (res == OK)
		    idx = -1;
	    }

	    ប្រសិនបើ (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	បើមិនដូច្នេះ
	    emsg_funcname(e_unknown_function_str, namebuf);
	goto theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    ប្រសិនបើ (lookup_local(namebuf, varlen, NULL, cctx) == FAIL
	    && arg_exists(namebuf, varlen, NULL, NULL, NULL, cctx) != OK)
    {
	class_T		*cl = NULL;
	ចំនួនគត់		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	ប្រសិនបើ (ufunc != NULL)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ប្រសិនបើ (ufunc == NULL)
		goto theend;

	    ប្រសិនបើ (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, NULL, 0, argcount, FALSE);
		goto theend;
	    }
	    ប្រសិនបើ (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == NULL
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		goto theend;
	    }
	}
	បើមិនដូច្នេះ ប្រសិនបើ ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    ប្រសិនបើ (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], NULL,
							0, argcount, FALSE);
	    }
	    បើមិនដូច្នេះ
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    goto theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == NULL && gname != NULL ? gname : namebuf;
    ប្រសិនបើ (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_stack(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	goto theend;
    }

    // If we can find a global function by name generate the right call.
    ប្រសិនបើ (ufunc != NULL)
    {
	res = generate_CALL(cctx, ufunc, NULL, 0, argcount, FALSE);
	goto theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    ប្រសិនបើ (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    បើមិនដូច្នេះ
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    ត្រឡប់ res;
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
to_name_end(char_u *arg, ចំនួនគត់ use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    ប្រសិនបើ (!eval_isnamec1(*arg))
	ត្រឡប់ arg;

    សម្រាប់ (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	ប្រសិនបើ (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == NULL))
	    bha_nga;
    ត្រឡប់ p;
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

    ប្រសិនបើ (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    ប្រសិនបើ (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	ប្រសិនបើ (eval_list(&p, &rettv, NULL, FALSE) == FAIL)
	    p = arg;
    }
    ត្រឡប់ p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    static ចំនួនគត់
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    ចំនួនគត់		count = 0;
    ចំនួនគត់		is_const;
    ចំនួនគត់		is_all_const = TRUE;	// reset when non-const encountered
    ចំនួនគត់		must_end = FALSE;

    សម្រាប់ (;;)
    {
	ប្រសិនបើ (may_get_next_line(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (*p == ']')
	{
	    ++p;
	    bha_nga;
	}
	ប្រសិនបើ (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    ត្រឡប់ FAIL;
	ប្រសិនបើ (!is_const)
	    is_all_const = FALSE;
	++count;
	ប្រសិនបើ (*p == ',')
	{
	    ++p;
	    ប្រសិនបើ (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ត្រឡប់ FAIL;
	    }
	}
	បើមិនដូច្នេះ
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ត្រឡប់ generate_NEWLIST(cctx, count, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    static ចំនួនគត់
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    ចំនួនគត់ first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    ចំនួនគត់		count = 0;
    ចំនួនគត់		is_const;
    ចំនួនគត់		is_all_const = TRUE;	// reset when non-const encountered
    ចំនួនគត់		must_end = FALSE;

    ប្រសិនបើ (**arg != ')')
    {
	ប្រសិនបើ (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    ត្រឡប់ FAIL;
	}
	count = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	សម្រាប់ (;;)
	{
	    ប្រសិនបើ (may_get_next_line(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		ត្រឡប់ FAIL;
	    }
	    ប្រសិនបើ (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		ត្រឡប់ FAIL;
	    }
	    ប្រសិនបើ (*p == ')')
	    {
		++p;
		bha_nga;
	    }
	    ប្រសិនបើ (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		ត្រឡប់ FAIL;
	    }
	    ប្រសិនបើ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		ត្រឡប់ FAIL;
	    ប្រសិនបើ (!is_const)
		is_all_const = FALSE;
	    ++count;
	    ប្រសិនបើ (*p == ',')
	    {
		++p;
		ប្រសិនបើ (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ត្រឡប់ FAIL;
		}
	    }
	    បើមិនដូច្នេះ
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ត្រឡប់ generate_NEWTUPLE(cctx, count, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    static void
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*line;
    size_t	off;

    ប្រសិនបើ (!evalarg->eval_using_cmdline || cctx == NULL)
	ត្រឡប់;

    gap = &evalarg->eval_tofree_ga;
    ប្រសិនបើ (gap->ga_len == 0)
	ត្រឡប់;

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
    ចំនួនគត់
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ចំនួនគត់		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    ប្រសិនបើ (r != OK)
    {
	clear_evalarg(&evalarg, NULL);
	ត្រឡប់ r;
    }

    ប្រសិនបើ (ppconst != NULL && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, NULL);
	ត្រឡប់ FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    ប្រសិនបើ (cctx->ctx_ufunc != NULL)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	ចំនួនគត់ block_depth = cctx->ctx_ufunc->uf_block_depth;

	ប្រសិនបើ (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(ចំនួនគត់, block_depth);
	    ប្រសិនបើ (ufunc->uf_block_ids != NULL)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    parimaa_na(ចំនួនគត់) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ប្រសិនបើ (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    ប្រសិនបើ (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    ប្រសិនបើ (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	ប្រសិនបើ (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, NULL);

    ប្រសិនបើ (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	ត្រឡប់ generate_FUNCREF(cctx, ufunc, NULL, FALSE, 0, NULL);
    }

    func_ptr_unref(ufunc);
    ត្រឡប់ FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    ចំនួនគត់
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	ចំនួនគត់	    types_optional,
	evalarg_T   *evalarg)
{
    ចំនួនគត់		r;
    ufunc_T	*ufunc;
    ចំនួនគត់		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, NULL);
    current_sctx.sc_version = save_sc_version;
    ប្រសិនបើ (r != OK)
	ត្រឡប់ r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ប្រសិនបើ (ufunc->uf_ret_type == NULL || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, NULL);

    ប្រសិនបើ (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	ត្រឡប់ OK;
    }
    clear_tv(rettv);
    ត្រឡប់ FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    static ចំនួនគត់
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    ចំនួនគត់		count = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    ចំនួនគត់		is_const;
    ចំនួនគត់		is_all_const = TRUE;	// reset when non-const encountered

    ប្រសិនបើ (d == NULL)
	ត្រឡប់ FAIL;
    ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	ត្រឡប់ FAIL;
    }
    សម្រាប់ (;;)
    {
	char_u	    *key = NULL;

	ប្រសិនបើ (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = NULL;
	    goto failret;
	}

	ប្រសិនបើ (**arg == '}')
	    bha_nga;

	ប្រសិនបើ (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    ប្រសិនបើ (compile_expr0(arg, cctx) == FAIL)
		goto failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    ប្រសិនបើ (isn->isn_type == ISN_PUSHNR)
	    {
		var_na buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.number);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    ប្រសិនបើ (isn->isn_type == ISN_PUSHS)
		key = isn->isn_arg.string;
	    បើមិនដូច្នេះ ប្រសិនបើ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		goto failret;
	    *arg = skipwhite(*arg);
	    ប្រសិនបើ (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_key));
		goto failret;
	    }
	    ++*arg;
	}
	បើមិនដូច្នេះ
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    key = get_literal_key(arg);
	    ប្រសិនបើ (key == NULL)
		goto failret;
	    ប្រសិនបើ (generate_PUSHS(cctx, &key) == FAIL)
		goto failret;
	}

	// Check for duplicate keys, if using string keys.
	ប្រសិនបើ (key != NULL)
	{
	    item = dict_find(d, key, -1);
	    ប្រសិនបើ (item != NULL)
	    {
		semsg(_(e_duplicate_key_in_dictionary_str), key);
		goto failret;
	    }
	    item = dictitem_alloc(key);
	    ប្រសិនបើ (item != NULL)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		ប្រសិនបើ (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	ប្រសិនបើ (**arg != ':')
	{
	    ប្រសិនបើ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    បើមិនដូច្នេះ
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    goto failret;
	}
	whitep = *arg + 1;
	ប្រសិនបើ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    goto failret;
	}

	ប្រសិនបើ (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = NULL;
	    goto failret;
	}

	ប្រសិនបើ (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    goto failret;
	ប្រសិនបើ (!is_const)
	    is_all_const = FALSE;
	++count;

	whitep = *arg;
	ប្រសិនបើ (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = NULL;
	    goto failret;
	}
	ប្រសិនបើ (**arg == '}')
	    bha_nga;
	ប្រសិនបើ (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    goto failret;
	}
	ប្រសិនបើ (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    goto failret;
	}
	whitep = *arg + 1;
	ប្រសិនបើ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    goto failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    ប្រសិនបើ (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    ត្រឡប់ generate_NEWDICT(cctx, count, FALSE);

failret:
    ប្រសិនបើ (*arg == NULL)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    ត្រឡប់ FAIL;
}

/*
 * Compile "&option".
 */
    static ចំនួនគត់
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    ចំនួនគត់		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    ប្រសិនបើ (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_NUMBER ? &t_number : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    ត្រឡប់ ret;
}

/*
 * Compile "$VAR".
 */
    static ចំនួនគត់
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    ចំនួនគត់		len;
    ចំនួនគត់		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    ប្រសិនបើ (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	ត្រឡប់ FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    ត្រឡប់ ret;
}

/*
 * Compile $"string" or $'string'.
 */
    static ចំនួនគត់
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    ចំនួនគត់		ret;
    ចំនួនគត់		quote;
    ចំនួនគត់		evaluate = cctx->ctx_skip != SKIP_YES;
    ចំនួនគត់		count = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    សម្រាប់ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ប្រសិនបើ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	បើមិនដូច្នេះ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ប្រសិនបើ (ret == FAIL)
	    bha_nga;
	ប្រសិនបើ (evaluate)
	{
	    ប្រសិនបើ ((tv.vval.v_string != NULL && *tv.vval.v_string != NUL)
		    || (**arg != '{' && count == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		ប្រសិនបើ (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    ត្រឡប់ FAIL;
		tv.vval.v_string = NULL;  // don't free it now
		++count;
	    }
	    clear_tv(&tv);
	}

	ប្រសិនបើ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    bha_nga;
	}

	p = compile_one_expr_in_str(*arg, cctx);
	ប្រសិនបើ (p == NULL)
	{
	    ret = FAIL;
	    bha_nga;
	}
	++count;
	*arg = p;
    }

    ប្រសិនបើ (ret == FAIL || !evaluate)
	ត្រឡប់ ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ប្រសិនបើ (count > 1)
	ត្រឡប់ generate_CONCAT(cctx, count);

    ត្រឡប់ OK;
}

/*
 * Compile "@r".
 */
    static ចំនួនគត់
compile_get_register(char_u **arg, cctx_T *cctx)
{
    ចំនួនគត់		ret;

    ++*arg;
    ប្រសិនបើ (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	ត្រឡប់ FAIL;
    }
    ប្រសិនបើ (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	ត្រឡប់ FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, NULL, &t_string);
    ++*arg;
    ត្រឡប់ ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    static ចំនួនគត់
apply_leader(typval_T *rettv, ចំនួនគត់ numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    ខណៈពេល (p > start)
    {
	--p;
	ប្រសិនបើ (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    ប្រសិនបើ (rettv->v_type == VAR_FLOAT)
	    {
		ប្រសិនបើ (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    បើមិនដូច្នេះ
	    {
		varnumber_T	val;
		ចំនួនគត់		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		ប្រសិនបើ (check_not_string(rettv) == FAIL)
		    ត្រឡប់ FAIL;
		val = tv_get_number_chk(rettv, &error);
		clear_tv(rettv);
		ប្រសិនបើ (error)
		    ត្រឡប់ FAIL;
		ប្រសិនបើ (*p == '-')
		    val = -val;
		rettv->v_type = VAR_NUMBER;
		rettv->vval.v_number = val;
	    }
	}
	បើមិនដូច្នេះ ប្រសិនបើ (numeric_only)
	{
	    ++p;
	    bha_nga;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (*p == '!')
	{
	    ចំនួនគត់ v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_number = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    ត្រឡប់ OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    static void
get_vim_constant(char_u **arg, typval_T *rettv)
{
    ប្រសិនបើ (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_number = VVAL_TRUE;
	*arg += 6;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_number = VVAL_FALSE;
	*arg += 7;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_number = VVAL_NULL;
	*arg += 6;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_number = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, ចំនួនគត់ *len, ចំនួនគត់ *type_is)
{
    exprtype_T	type = EXPR_UNKNOWN;
    ចំនួនគត់		i;

    switch (p[0])
    {
	pak_sa '=':   ប្រសិនបើ (p[1] == '=')
			type = EXPR_EQUAL;
		    បើមិនដូច្នេះ ប្រសិនបើ (p[1] == '~')
			type = EXPR_MATCH;
		    bha_nga;
	pak_sa '!':   ប្រសិនបើ (p[1] == '=')
			type = EXPR_NEQUAL;
		    បើមិនដូច្នេះ ប្រសិនបើ (p[1] == '~')
			type = EXPR_NOMATCH;
		    bha_nga;
	pak_sa '>':   ប្រសិនបើ (p[1] != '=')
		    {
			type = EXPR_GREATER;
			*len = 1;
		    }
		    បើមិនដូច្នេះ
			type = EXPR_GEQUAL;
		    bha_nga;
	pak_sa '<':   ប្រសិនបើ (p[1] != '=')
		    {
			type = EXPR_SMALLER;
			*len = 1;
		    }
		    បើមិនដូច្នេះ
			type = EXPR_SEQUAL;
		    bha_nga;
	pak_sa 'i':   ប្រសិនបើ (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			ប្រសិនបើ (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			ប្រសិនបើ (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? EXPR_IS : EXPR_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    bha_nga;
    }
    ត្រឡប់ type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    void
skip_expr_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_expr(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, NULL);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    ចំនួនគត់
bool_on_stack(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_stack(cctx, 0);
    ប្រសិនបើ (type == &t_bool)
	ត្រឡប់ OK;

    ប្រសិនបើ (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_NUMBER
	    || type == &t_number_bool
	    || type == &t_const_number_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	ត្រឡប់ generate_COND2BOOL(cctx);

    ត្រឡប់ need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    void
error_white_both(char_u *op, ចំនួនគត់ len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    static ចំនួនគត់
compile_leader(cctx_T *cctx, ចំនួនគត់ numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    ខណៈពេល (p > start)
    {
	--p;
	ខណៈពេល (VIM_ISWHITE(*p))
	    --p;
	ប្រសិនបើ (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_stack(cctx, 0);
	    ប្រសិនបើ (type->tt_type != VAR_FLOAT && need_type(type, &t_number,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ត្រឡប់ FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    ប្រសិនបើ (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == NULL)
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (numeric_only)
	{
	    ++p;
	    bha_nga;
	}
	បើមិនដូច្នេះ
	{
	    ចំនួនគត់  invert = *p == '!';

	    ខណៈពេល (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		ប្រសិនបើ (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    ប្រសិនបើ (check_type_is_value(get_type_on_stack(cctx, 0)) == FAIL)
		ត្រឡប់ FAIL;
	    ប្រសិនបើ (generate_2BOOL(cctx, invert, -1) == FAIL)
		ត្រឡប់ FAIL;
	}
    }
    *end = p;
    ត្រឡប់ OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    static ចំនួនគត់
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ចំនួនគត់	    ret;
    char_u  *p = *arg + 1;

    ប្រសិនបើ (may_get_next_line_error(p, arg, cctx) == FAIL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (**arg == ')')
	// empty tuple
	ត្រឡប់ compile_tuple(arg, cctx, ppconst, FALSE);

    ប្រសិនបើ (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    បើមិនដូច្នេះ
    {
	// Not enough space in ppconst, flush constants.
	ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
	    ត្រឡប់ FAIL;
	ret = compile_expr0(arg, cctx);
    }
    ប្រសិនបើ (may_get_next_line_error(*arg, arg, cctx) == FAIL)
	ត្រឡប់ FAIL;
    ប្រសិនបើ (ret == OK && **arg == ',')
    {
	// tuple
	ចំនួនគត់ is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
	    ត្រឡប់ FAIL;
	ត្រឡប់ compile_tuple(arg, cctx, ppconst, is_const);
    }

    ប្រសិនបើ (**arg == ')')
	++*arg;
    បើមិនដូច្នេះ ប្រសិនបើ (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    ត្រឡប់ ret;
}

static ចំនួនគត់ compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    static ចំនួនគត់
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    ចំនួនគត់		keeping_dict = FALSE;

    សម្រាប់ (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	ប្រសិនបើ (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_line_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    ប្រសិនបើ (next != NULL &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_line_from_context(cctx, TRUE);
		ប្រសិនបើ (next == NULL)
		    ត្រឡប់ FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	ប្រសិនបើ (**arg == '(')
	{
	    ចំនួនគត់		argcount = 0;

	    ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
		ត្រឡប់ FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_stack(cctx, 0);

	    *arg = skipwhite(p + 1);
	    ប្រសិនបើ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		ត្រឡប់ FAIL;
	    ប្រសិនបើ (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		ត្រឡប់ FAIL;
	    ប្រសិនបើ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ប្រសិនបើ (generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    ត្រឡប់ FAIL;
	    }
	}
	បើមិនដូច្នេះ ប្រសិនបើ (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    ចំនួនគត់	    alt;
	    char_u  *paren;

	    // something->method()
	    ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
		ត្រឡប់ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_stack(cctx, 0);
	    ប្រសិនបើ (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_value));
		ត្រឡប់ FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    ប្រសិនបើ (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		ត្រឡប់ FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    ប្រសិនបើ (**arg == '(')
		alt = 2;
	    បើមិនដូច្នេះ
	    {
		// alternative 1 or 3
		p = *arg;
		ប្រសិនបើ (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    ត្រឡប់ FAIL;
		}
		ប្រសិនបើ (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		សម្រាប់ ( ; eval_isnamec(*p); ++p)
		    ;
		ប្រសិនបើ (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    ប្រសិនបើ (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			ត្រឡប់ FAIL;
		}
		បើមិនដូច្នេះ
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    ប្រសិនបើ (paren == NULL)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			ត្រឡប់ FAIL;
		    }
		}
	    }

	    ប្រសិនបើ (alt != 1)
	    {
		ចំនួនគត់	    argcount = 1;
		garray_T    *stack = &cctx->ctx_type_stack;
		ចំនួនគត់	    type_idx_start = stack->ga_len;
		ចំនួនគត់	    expr_isn_start = cctx->ctx_instr.ga_len;
		ចំនួនគត់	    expr_isn_end;
		ចំនួនគត់	    arg_isn_count;

		ប្រសិនបើ (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    ប្រសិនបើ (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			ត្រឡប់ FAIL;
		}
		បើមិនដូច្នេះ
		{
		    ចំនួនគត់ fail;
		    ចំនួនគត់ save_len = cctx->ctx_ufunc->uf_lines.ga_len;
		    ចំនួនគត់	prev_did_emsg = did_emsg;

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

		    ប្រសិនបើ (fail)
		    {
			ប្រសិនបើ (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			ត្រឡប់ FAIL;
		    }
		}

		// Compile the arguments.
		ប្រសិនបើ (**arg != '(')
		{
		    ប្រសិនបើ (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    បើមិនដូច្នេះ
			semsg(_(e_missing_parenthesis_str), *arg);
		    ត្រឡប់ FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		expr_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		ប្រសិនបើ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    ត្រឡប់ FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_count = cctx->ctx_instr.ga_len - expr_isn_end;
		ប្រសិនបើ (arg_isn_count > 0)
		{
		    ចំនួនគត់	    expr_isn_count = expr_isn_end - expr_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, expr_isn_count);
		    type_T  *decl_type;
		    type2_T  *typep;

		    ប្រសិនបើ (isn == NULL)
			ត្រឡប់ FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + expr_isn_start,
					       parimaa_na(isn_T) * expr_isn_count);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + expr_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + expr_isn_end,
						parimaa_na(isn_T) * arg_isn_count);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + expr_isn_start + arg_isn_count,
					  isn, parimaa_na(isn_T) * expr_isn_count);
		    vim_free(isn);

		    typep = ((type2_T *)stack->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)stack->ga_data) + type_idx_start,
			      ((type2_T *)stack->ga_data) + type_idx_start + 1,
			      parimaa_na(type2_T)
				       * (stack->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)stack->ga_data) + stack->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_stack(cctx, 0);
		ប្រសិនបើ (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    ត្រឡប់ FAIL;
	    }

	    ប្រសិនបើ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ប្រសិនបើ (generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    ត្រឡប់ FAIL;
	    }
	}
	បើមិនដូច្នេះ ប្រសិនបើ (**arg == '[')
	{
	    ចំនួនគត់		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
		ត្រឡប់ FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    ប្រសិនបើ (may_get_next_line_error(p, arg, cctx) == FAIL)
		ត្រឡប់ FAIL;
	    ប្រសិនបើ (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    បើមិនដូច្នេះ
	    {
		ប្រសិនបើ (compile_expr0(arg, cctx) == FAIL)
		    ត្រឡប់ FAIL;
		ប្រសិនបើ (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ត្រឡប់ FAIL;
		}
		ប្រសិនបើ (may_get_next_line_error(*arg, arg, cctx) == FAIL)
		    ត្រឡប់ FAIL;
		*arg = skipwhite(*arg);
	    }
	    ប្រសិនបើ (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		ប្រសិនបើ (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ត្រឡប់ FAIL;
		}
		ប្រសិនបើ (may_get_next_line_error(*arg, arg, cctx) == FAIL)
		    ត្រឡប់ FAIL;
		ប្រសិនបើ (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		បើមិនដូច្នេះ
		{
		    ប្រសិនបើ (compile_expr0(arg, cctx) == FAIL)
			ត្រឡប់ FAIL;
		    ប្រសិនបើ (may_get_next_line_error(*arg, arg, cctx) == FAIL)
			ត្រឡប់ FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    ប្រសិនបើ (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		ត្រឡប់ FAIL;
	    }
	    *arg = *arg + 1;

	    ប្រសិនបើ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ប្រសិនបើ (generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    ត្រឡប់ FAIL;
	    }
	    ប្រសិនបើ (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		ត្រឡប់ FAIL;
	}
	បើមិនដូច្នេះ ប្រសិនបើ (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
		ត្រឡប់ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_stack(cctx, 0);
	    ប្រសិនបើ (type != &t_unknown
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
		ប្រសិនបើ (compile_class_object_index(cctx, arg, type) == FAIL)
		    ត្រឡប់ FAIL;
	    }
	    បើមិនដូច្នេះ
	    {
		*arg = p + 1;
		ប្រសិនបើ (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    ត្រឡប់ FAIL;
		}
		p = *arg;
		ខណៈពេល (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		ប្រសិនបើ (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    ត្រឡប់ FAIL;
		}
		ប្រសិនបើ (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    ត្រឡប់ FAIL;
		ប្រសិនបើ (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    ត្រឡប់ FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	បើមិនដូច្នេះ
	    bha_nga;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    ប្រសិនបើ (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == NULL)
	ត្រឡប់ FAIL;

    ត្រឡប់ OK;
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
    static ចំនួនគត់
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    ចំនួនគត់		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    ចំនួនគត់		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    ប្រសិនបើ (eval_leader(arg, TRUE) == FAIL)
	ត្រឡប់ FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    switch (**arg)
    {
	/*
	 * Number constant.
	 */
	pak_sa '0':	// also for blob starting with 0z
	pak_sa '1':
	pak_sa '2':
	pak_sa '3':
	pak_sa '4':
	pak_sa '5':
	pak_sa '6':
	pak_sa '7':
	pak_sa '8':
	pak_sa '9':
	pak_sa '.':   ប្រសិនបើ (eval_number(arg, rettv, TRUE, FALSE) == FAIL)
			ត្រឡប់ FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    ប្រសិនបើ (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			ត្រឡប់ FAIL;
		    }
		    bha_nga;

	/*
	 * String constant: "string".
	 */
	pak_sa '"':   ប្រសិនបើ (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			ត្រឡប់ FAIL;
		    bha_nga;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	pak_sa '\'':  ប្រសិនបើ (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			ត្រឡប់ FAIL;
		    bha_nga;

	/*
	 * Constant Vim variable.
	 */
	pak_sa 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    bha_nga;

	/*
	 * "true" constant
	 */
	pak_sa 't':   ប្រសិនបើ (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_number = VVAL_TRUE;
		    }
		    បើមិនដូច្នេះ
			ret = NOTDONE;
		    bha_nga;

	/*
	 * "false" constant
	 */
	pak_sa 'f':   ប្រសិនបើ (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_number = VVAL_FALSE;
		    }
		    បើមិនដូច្នេះ
			ret = NOTDONE;
		    bha_nga;

	/*
	 * "null" or "null_*" constant
	 */
	pak_sa 'n':   ប្រសិនបើ (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			ចំនួនគត់	len;

			សម្រាប់ (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			ប្រសិនបើ (ret == FAIL)
			    ret = NOTDONE;
			បើមិនដូច្នេះ
			    *arg += len + 4;
		    }
		    បើមិនដូច្នេះ
			ret = NOTDONE;
		    bha_nga;

	/*
	 * List: [expr, expr]
	 */
	pak_sa '[':   ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
			ត្រឡប់ FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    bha_nga;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	pak_sa '{':   ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
			ត្រឡប់ FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    bha_nga;

	/*
	 * Option value: &name
	 */
	pak_sa '&':	ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
			    ត្រឡប់ FAIL;
			ret = compile_get_option(arg, cctx);
			bha_nga;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	pak_sa '$':	ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
			    ត្រឡប់ FAIL;
			ប្រសិនបើ ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			បើមិនដូច្នេះ
			    ret = compile_get_env(arg, cctx);
			bha_nga;

	/*
	 * Register contents: @r.
	 */
	pak_sa '@':	ប្រសិនបើ (generate_ppconst(cctx, ppconst) == FAIL)
			    ត្រឡប់ FAIL;
			ret = compile_get_register(arg, cctx);
			bha_nga;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	pak_sa '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    ប្រសិនបើ (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    bha_nga;

	default:    ret = NOTDONE;
		    bha_nga;
    }
    ប្រសិនបើ (ret == FAIL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	ប្រសិនបើ (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	បើមិនដូច្នេះ
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    បើមិនដូច្នេះ ប្រសិនបើ (ret == NOTDONE)
    {
	char_u	    *p;
	ចំនួនគត់	    r;

	ប្រសិនបើ (!eval_isnamec1(**arg))
	{
	    ប្រសិនបើ (!vim9_bad_comment(*arg))
	    {
		ប្រសិនបើ (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		បើមិនដូច្នេះ
		    semsg(_(e_name_expected_str), *arg);
	    }
	    ត្រឡប់ FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	ប្រសិនបើ (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ត្រឡប់ FAIL;
	}

	size_t namelen = p - *arg;
	ប្រសិនបើ (*p == '<')
	{
	    ប្រសិនបើ (skip_generic_func_type_args(&p) == FAIL)
		ត្រឡប់ FAIL;
	}

	ប្រសិនបើ (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		ត្រឡប់ FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	ប្រសិនបើ (r == FAIL)
	    ត្រឡប់ FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    ប្រសិនបើ (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	ត្រឡប់ FAIL;
    ប្រសិនបើ ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	ប្រសិនបើ (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    ត្រឡប់ FAIL;
	ត្រឡប់ OK;
    }
    ប្រសិនបើ (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	ត្រឡប់ FAIL;
    ត្រឡប់ OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    static ចំនួនគត់
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = NULL;

    // Recognize <type>
    ប្រសិនបើ (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ប្រសិនបើ (want_type == NULL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (**arg != '>')
	{
	    ប្រសិនបើ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    បើមិនដូច្នេះ
		emsg(_(e_missing_gt));
	    ត្រឡប់ FAIL;
	}
	++*arg;
	ប្រសិនបើ (may_get_next_line_error(*arg, arg, cctx) == FAIL)
	    ត្រឡប់ FAIL;
    }

    ប្រសិនបើ (compile_expr9(arg, cctx, ppconst) == FAIL)
	ត្រឡប់ FAIL;

    ប្រសិនបើ (want_type != NULL)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_stack(cctx, 0);
	ប្រសិនបើ (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    ប្រសិនបើ (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		ត្រឡប់ FAIL;
	}
    }

    ត្រឡប់ OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    static ចំនួនគត់
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    ចំនួនគត់		ppconst_used = ppconst->pp_used;

    // get the first expression
    ប្រសិនបើ (compile_expr8(arg, cctx, ppconst) == FAIL)
	ត្រឡប់ FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    សម្រាប់ (;;)
    {
	op = may_peek_next_line(cctx, *arg, &next);
	ប្រសិនបើ (*op != '*' && *op != '/' && *op != '%')
	    bha_nga;
	ប្រសិនបើ (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ប្រសិនបើ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (may_get_next_line_error(op + 1, arg, cctx) == FAIL)
	    ត្រឡប់ FAIL;

	// get the second expression
	ប្រសិនបើ (compile_expr8(arg, cctx, ppconst) == FAIL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_NUMBER
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_NUMBER)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    ចំនួនគត់		    failed = FALSE;

	    // both are numbers: compute the result
	    switch (*op)
	    {
		pak_sa '*': res = tv1->vval.v_number * tv2->vval.v_number;
			  bha_nga;
		pak_sa '/': res = num_divide(tv1->vval.v_number,
						  tv2->vval.v_number, &failed);
			  bha_nga;
		pak_sa '%': res = num_modulus(tv1->vval.v_number,
						  tv2->vval.v_number, &failed);
			  bha_nga;
	    }
	    ប្រសិនបើ (failed)
		ត្រឡប់ FAIL;
	    tv1->vval.v_number = res;
	    --ppconst->pp_used;
	}
	បើមិនដូច្នេះ
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    ត្រឡប់ OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    static ចំនួនគត់
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    ចំនួនគត់		oplen;
    ចំនួនគត់		ppconst_used = ppconst->pp_used;

    // get the first variable
    ប្រសិនបើ (compile_expr7(arg, cctx, ppconst) == FAIL)
	ត្រឡប់ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    សម្រាប់ (;;)
    {
	op = may_peek_next_line(cctx, *arg, &next);
	ប្រសិនបើ (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    bha_nga;
	ប្រសិនបើ (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    bha_nga;
	oplen = (*op == '.' ? 2 : 1);
	ប្រសិនបើ (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ប្រសិនបើ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    ត្រឡប់ FAIL;
	}

	ប្រសិនបើ (may_get_next_line_error(op + oplen, arg, cctx) == FAIL)
	    ត្រឡប់ FAIL;

	// get the second expression
	ប្រសិនបើ (compile_expr7(arg, cctx, ppconst) == FAIL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_NUMBER
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_NUMBER)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    ប្រសិនបើ (*op == '+')
		tv1->vval.v_number = tv1->vval.v_number + tv2->vval.v_number;
	    បើមិនដូច្នេះ ប្រសិនបើ (*op == '-')
		tv1->vval.v_number = tv1->vval.v_number - tv2->vval.v_number;
	    បើមិនដូច្នេះ
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		ប្រសិនបើ (tv1->vval.v_string == NULL)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    ត្រឡប់ FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	បើមិនដូច្នេះ
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    ប្រសិនបើ (*op == '.')
	    {
		ប្រសិនបើ (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    ត្រឡប់ FAIL;
		ប្រសិនបើ (generate_CONCAT(cctx, 2) == FAIL)
		    ត្រឡប់ FAIL;
	    }
	    បើមិនដូច្នេះ
		generate_two_op(cctx, op);
	}
    }

    ត្រឡប់ OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    static ចំនួនគត់
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = EXPR_UNKNOWN;
    char_u	*p;
    char_u	*next;
    ចំនួនគត់		len = 2;
    ចំនួនគត់		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    ប្រសិនបើ (compile_expr6(arg, cctx, ppconst) == FAIL)
	ត្រឡប់ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    សម្រាប់ (;;)
    {
	type = EXPR_UNKNOWN;

	p = may_peek_next_line(cctx, *arg, &next);
	ប្រសិនបើ (p[0] == '<' && p[1] == '<')
	    type = EXPR_LSHIFT;
	បើមិនដូច្នេះ ប្រសិនបើ (p[0] == '>' && p[1] == '>')
	    type = EXPR_RSHIFT;

	ប្រសិនបើ (type == EXPR_UNKNOWN)
	    ត្រឡប់ OK;

	// Handle a bitwise left or right shift operator
	ប្រសិនបើ (ppconst->pp_used == ppconst_used + 1)
	{
	    ប្រសិនបើ (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_NUMBER)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_number));
		ត្រឡប់ FAIL;
	    }
	}
	បើមិនដូច្នេះ
	{
	    type_T	*t = get_type_on_stack(cctx, 0);

	    ប្រសិនបើ (need_type(t, &t_number, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_number));
		ត្រឡប់ FAIL;
	    }
	}

	ប្រសិនបើ (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ប្រសិនបើ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ត្រឡប់ FAIL;
	}

	// get the second variable
	ប្រសិនបើ (may_get_next_line_error(p + len, arg, cctx) == FAIL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (compile_expr6(arg, cctx, ppconst) == FAIL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    ប្រសិនបើ (tv2->v_type != VAR_NUMBER || tv2->vval.v_number < 0)
	    {
		// right operand should be a positive number
		ប្រសិនបើ (tv2->v_type != VAR_NUMBER)
		    emsg(_(e_bitshift_ops_must_be_number));
		បើមិនដូច្នេះ
		    emsg(_(e_bitshift_ops_must_be_positive));
		ត្រឡប់ FAIL;
	    }

	    ប្រសិនបើ (tv2->vval.v_number > MAX_LSHIFT_BITS)
		tv1->vval.v_number = 0;
	    បើមិនដូច្នេះ ប្រសិនបើ (type == EXPR_LSHIFT)
		tv1->vval.v_number =
			(uvarnumber_T)tv1->vval.v_number << tv2->vval.v_number;
	    បើមិនដូច្នេះ
		tv1->vval.v_number =
			(uvarnumber_T)tv1->vval.v_number >> tv2->vval.v_number;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (need_type(get_type_on_stack(cctx, 0), &t_number, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_number));
		ត្រឡប់ FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    ប្រសិនបើ (isn == NULL)
		ត្រឡប់ FAIL;

	    ប្រសិនបើ (isn != NULL)
		isn->isn_arg.op.op_type = type;
	}
    }

    ត្រឡប់ OK;
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
    static ចំនួនគត់
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = EXPR_UNKNOWN;
    char_u	*p;
    char_u	*next;
    ចំនួនគត់		len = 2;
    ចំនួនគត់		type_is = FALSE;
    ចំនួនគត់		ppconst_used = ppconst->pp_used;

    // get the first variable
    ប្រសិនបើ (compile_expr5(arg, cctx, ppconst) == FAIL)
	ត្រឡប់ FAIL;

    p = may_peek_next_line(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ប្រសិនបើ (type != EXPR_UNKNOWN)
    {
	ចំនួនគត់ ic = FALSE;  // Default: do not ignore case

	ប្រសិនបើ (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	ប្រសិនបើ (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    ត្រឡប់ FAIL;
	}
	// extra question mark appended: ignore case
	ប្រសិនបើ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	បើមិនដូច្នេះ ប្រសិនបើ (p[len] == '#')
	    ++len;
	// nothing appended: match case

	ប្រសិនបើ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ត្រឡប់ FAIL;
	}

	// get the second variable
	ប្រសិនបើ (may_get_next_line_error(p + len, arg, cctx) == FAIL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (compile_expr5(arg, cctx, ppconst) == FAIL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    ចំនួនគត់		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    ប្រសិនបើ (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    បើមិនដូច្នេះ
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_number = tv1->vval.v_number
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    ត្រឡប់ ret;
	}

	generate_ppconst(cctx, ppconst);
	ត្រឡប់ generate_COMPARE(cctx, type, ic);
    }

    ត្រឡប់ OK;
}

static ចំនួនគត់ compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    static ចំនួនគត់
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	var_na	*op,
	ppconst_T *ppconst,
	ចំនួនគត់	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_line(cctx, *arg, &next);
    ចំនួនគត់		opchar = *op;

    ប្រសិនបើ (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	ចំនួនគត់		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, parimaa_na(ចំនួនគត់), 10);
	ខណៈពេល (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    ចំនួនគត់		start_ctx_lnum = cctx->ctx_lnum;
	    ចំនួនគត់		save_lnum;
	    ចំនួនគត់		const_used;
	    ចំនួនគត់		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    ប្រសិនបើ (next != NULL)
	    {
		*arg = next_line_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ប្រសិនបើ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		ត្រឡប់ FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    ប្រសិនបើ (status != FAIL)
	    {
		// Use the last ppconst if possible.
		ប្រសិនបើ (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    ចំនួនគត់		is_true = tv2bool(tv);

		    ប្រសិនបើ ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_number = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    បើមិនដូច្នេះ
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		បើមិនដូច្នេះ
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_stack(cctx);
		}
	    }
	    ប្រសិនបើ (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    ប្រសិនបើ (status == FAIL)
	    {
		ga_clear(&end_ga);
		ត្រឡប់ FAIL;
	    }

	    ប្រសិនបើ (jump_when != JUMP_NEVER)
	    {
		ប្រសិនបើ (cctx->ctx_skip != SKIP_YES)
		{
		    *(((ចំនួនគត់ *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    ប្រសិនបើ (may_get_next_line_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		ត្រឡប់ FAIL;
	    }

	    const_used = ppconst->pp_used;
	    ប្រសិនបើ ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		ត្រឡប់ FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    ប្រសិនបើ (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		ប្រសិនបើ (tv->v_type == VAR_NUMBER
			 && (tv->vval.v_number == 1 || tv->vval.v_number == 0))
		{
		    tv->vval.v_number = tv->vval.v_number == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_line(cctx, *arg, &next);
	}

	ប្រសិនបើ (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    ត្រឡប់ FAIL;
	}

	ប្រសិនបើ (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    ប្រសិនបើ (bool_on_stack(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		ត្រឡប់ FAIL;
	    }

	ប្រសិនបើ (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    ខណៈពេល (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((ចំនួនគត់ *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    ត្រឡប់ OK;
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
    static ចំនួនគត់
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ចំនួនគត់		ppconst_used = ppconst->pp_used;

    // get the first variable
    ប្រសិនបើ (compile_expr4(arg, cctx, ppconst) == FAIL)
	ត្រឡប់ FAIL;

    // || and && work almost the same
    ត្រឡប់ compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    static ចំនួនគត់
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ចំនួនគត់		ppconst_used = ppconst->pp_used;

    // eval the first expression
    ប្រសិនបើ (compile_expr3(arg, cctx, ppconst) == FAIL)
	ត្រឡប់ FAIL;

    // || and && work almost the same
    ត្រឡប់ compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    ចំនួនគត់
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    ចំនួនគត់		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    ប្រសិនបើ (cctx->ctx_skip == SKIP_YES)
    {
	ចំនួនគត់		prev_did_emsg = did_emsg;

	skip_expr_cctx(arg, cctx);
	ត្រឡប់ did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    ប្រសិនបើ (compile_expr2(arg, cctx, ppconst) == FAIL)
	ត្រឡប់ FAIL;

    p = may_peek_next_line(cctx, *arg, &next);
    ប្រសិនបើ (*p == '?')
    {
	ចំនួនគត់		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*stack = &cctx->ctx_type_stack;
	ចំនួនគត់		alt_idx = instr->ga_len;
	ចំនួនគត់		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = NULL;
	ចំនួនគត់		has_const_expr = FALSE;
	ចំនួនគត់		const_value = FALSE;
	ចំនួនគត់		save_skip = cctx->ctx_skip;

	ប្រសិនបើ (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ប្រសិនបើ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    ត្រឡប់ FAIL;
	}

	ប្រសិនបើ (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_expr = TRUE;
	    ប្រសិនបើ (op_falsy)
		const_value = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    បើមិនដូច្នេះ
	    {
		ចំនួនគត់ error = FALSE;

		const_value = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		ប្រសិនបើ (error)
		    ត្រឡប់ FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_value : !const_value) ? SKIP_YES : SKIP_NOT;

	    ប្រសិនបើ (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    បើមិនដូច្នេះ
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	បើមិនដូច្នេះ
	{
	    generate_ppconst(cctx, ppconst);
	    ប្រសិនបើ (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    ប្រសិនបើ (op_falsy)
	    {
		type1 = get_type_on_stack(cctx, -1);
		ប្រសិនបើ (check_type_is_value(type1) == FAIL)
		    ត្រឡប់ FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	ប្រសិនបើ (may_get_next_line_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    ត្រឡប់ FAIL;
	ប្រសិនបើ (compile_expr1(arg, cctx, ppconst) == FAIL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (!has_const_expr)
	{
	    generate_ppconst(cctx, ppconst);

	    ប្រសិនបើ (!op_falsy)
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

	ប្រសិនបើ (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_line(cctx, *arg, &next);
	    ប្រសិនបើ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ត្រឡប់ FAIL;
	    }
	    ប្រសិនបើ (next != NULL)
	    {
		*arg = next_line_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ប្រសិនបើ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		ត្រឡប់ FAIL;
	    }

	    // evaluate the third expression
	    ប្រសិនបើ (has_const_expr)
		cctx->ctx_skip = save_skip == SKIP_YES || const_value
							 ? SKIP_YES : SKIP_NOT;
	    ប្រសិនបើ (may_get_next_line_error(p + 1, arg, cctx) == FAIL)
		ត្រឡប់ FAIL;
	    ប្រសិនបើ (compile_expr1(arg, cctx, ppconst) == FAIL)
		ត្រឡប់ FAIL;
	}

	ប្រសិនបើ (!has_const_expr)
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
    ត្រឡប់ OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    ចំនួនគត់
compile_expr0_ext(char_u **arg,  cctx_T *cctx, ចំនួនគត់ *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    ប្រសិនបើ (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ត្រឡប់ FAIL;
    }
    ប្រសិនបើ (is_const != NULL)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    ប្រសិនបើ (generate_ppconst(cctx, &ppconst) == FAIL)
	ត្រឡប់ FAIL;
    ត្រឡប់ OK;
}

/*
 * Toplevel expression.
 */
    ចំនួនគត់
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    ត្រឡប់ compile_expr0_ext(arg, cctx, NULL);
}


#endif // defined(FEAT_EVAL)
