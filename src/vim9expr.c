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

#如果 defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
static 整数 paren_follows_after_expr = 0;

/*
 * Generate code for any ppconst entries.
 */
    整数
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    整数	    i;
    整数	    ret = OK;
    整数	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    为 (i = 0; i < ppconst->pp_used; ++i)
	如果 (generate_tv_PUSH(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    返回 ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    static 整数
check_ppconst_bool(ppconst_T *ppconst)
{
    如果 (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	返回 check_typval_type(&t_bool, tv, where);
    }
    返回 OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    void
clear_ppconst(ppconst_T *ppconst)
{
    整数	    i;

    为 (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    static 整数
compile_tuple_member(
    type2_T	*typep,
    整数		is_slice,
    cctx_T	*cctx)
{
    如果 (is_slice)
    {
	如果 (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    返回 FAIL;
	// a copy is made so the member type is no longer declared
	如果 (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	如果 (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    如果 (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    否则
    {
	如果 (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    如果 (typep->type_curr->tt_argcount == 1)
	    {
		如果 (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		否则
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    否则
		typep->type_curr = &t_any;
	    如果 (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		如果 (typep->type_decl->tt_argcount == 1)
		{
		    如果 (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    否则
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		否则
		    typep->type_curr = &t_any;
	    }
	    否则
		typep->type_decl = typep->type_curr;
	}
	如果 (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    返回 FAIL;
    }

    返回 OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    整数
compile_member(整数 is_slice, 整数 *keeping_dict, cctx_T *cctx)
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
    如果 ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    如果 (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	如果 (need_type(idxtype, &t_number, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    返回 FAIL;
	如果 (is_slice)
	{
	    idxtype = get_type_on_stack(cctx, 1);
	    如果 (need_type(idxtype, &t_number, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		返回 FAIL;
	}
    }

    如果 (vartype == VAR_DICT)
    {
	如果 (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    返回 FAIL;
	}
	如果 (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    如果 (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    如果 (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		如果 (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    否则
		typep->type_decl = typep->type_curr;
	}
	否则
	{
	    如果 (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		返回 FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	如果 (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    返回 FAIL;
	如果 (keeping_dict != NULL)
	    *keeping_dict = TRUE;
    }
    否则 如果 (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	如果 ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    返回 FAIL;
    }
    否则 如果 (vartype == VAR_BLOB)
    {
	如果 (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    如果 (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		返回 FAIL;
	}
	否则
	{
	    typep->type_curr = &t_number;
	    typep->type_decl = &t_number;
	    如果 (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		返回 FAIL;
	}
    }
    否则 如果 (vartype == VAR_TUPLE)
    {
	如果 (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    返回 FAIL;
    }
    否则 如果 (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	如果 (is_slice)
	{
	    如果 (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		返回 FAIL;
	    // a copy is made so the member type is no longer declared
	    如果 (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    如果 (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		如果 (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	否则
	{
	    如果 (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		如果 (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		如果 (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    如果 (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		否则
			typep->type_decl = typep->type_curr;
	    }
	    如果 (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		返回 FAIL;
	}
    }
    否则
    {
	switch (vartype)
	{
	    case VAR_FUNC:
	    case VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		break;
	    case VAR_BOOL:
	    case VAR_SPECIAL:
	    case VAR_JOB:
	    case VAR_CHANNEL:
	    case VAR_INSTR:
	    case VAR_CLASS:
	    case VAR_OBJECT:
	    case VAR_TYPEALIAS:
	    case VAR_UNKNOWN:
	    case VAR_ANY:
	    case VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		break;
	    default:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	返回 FAIL;
    }
    返回 OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    static 整数
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    为 (cctx_T *cctx = cctx_arg; cctx != NULL; cctx = cctx->ctx_outer)
    {
	如果 (cctx->ctx_ufunc != NULL && cctx->ctx_ufunc->uf_class != NULL)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    当 (clp != NULL)
	    {
		如果 (clp == cl)
		    返回 TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    返回 FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    整数
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    整数		m_idx;
    整数		ret = FAIL;

    如果 (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	返回 FAIL;
    }

    class_T *cl = type->tt_class;
    整数 is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    如果 (type == &t_super)
    {
	如果 (cctx->ctx_ufunc == NULL || cctx->ctx_ufunc->uf_class == NULL)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    返回 FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_stack.ga_len;
    }
    否则 如果 (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	如果 (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    如果 (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    如果 (cl == NULL)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	返回 FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, NULL, NULL, FNE_CHECK_START);
    如果 (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	返回 FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    如果 (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	如果 (name_end == NULL)
	    goto done;
    }

    如果 (*name_end == '(')
    {
	整数	function_count;
	整数	child_count;
	ufunc_T	**functions;

	如果 (type->tt_type == VAR_CLASS)
	{
	    function_count = cl->class_class_function_count;
	    child_count = cl->class_class_function_count_child;
	    functions = cl->class_class_functions;
	}
	否则
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    如果 (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_count = cl->class_obj_method_count;
	    child_count = cl->class_obj_method_count_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = NULL;
	整数 fi;
	为 (fi = is_super ? child_count : 0; fi < function_count; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    如果 (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		break;
	    }
	}

	ocmember_T  *ocm = NULL;
	如果 (ufunc == NULL)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    如果 (ocm == NULL || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_msg(cl, type->tt_type, name, len);
		goto done;
	    }
	    如果 (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_stack.ga_len;
		如果 (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    goto done;
	    }
	    否则
	    {
		整数 status;

		如果 (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		否则
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		如果 (status == FAIL)
		    goto done;
	    }
	}

	如果 (is_super && ufunc != NULL && IS_ABSTRACT_METHOD(ufunc))
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
	如果 (ocm == NULL && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    goto done;
	}

	// process generic function call
	如果 (ufunc != NULL)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    如果 (ufunc == NULL)
		goto done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	整数 argcount = 0;
	如果 (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    goto done;

	如果 (ocm != NULL)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    goto done;
	}
	如果 (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	否则
	    ret = generate_CALL(cctx, ufunc, NULL, 0, argcount, FALSE);
	goto done;
    }

    如果 (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	如果 (m_idx >= 0)
	{
	    如果 (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		goto done;
	    }

	    *arg = name_end;
	    如果 (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    否则
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    goto done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	如果 (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    如果 (*name == '_' && !inside_class(cctx, cl))
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
    否则
    {
	// load class member
	整数 idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	如果 (m != NULL)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    如果 (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
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
	如果 (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    如果 (*name == '_' && !inside_class(cctx, cl))
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
    返回 ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    整数
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    整数		    idx;
    imported_T	    *imp;

    如果 (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	返回 FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, NULL);
    如果 (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	返回 OK;
    }

    如果 (end == NULL)
	imp = NULL;
    否则 如果 (import == NULL)
	imp = find_imported(name, 0, FALSE);
    否则
	imp = import;

    如果 (imp != NULL)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	整数	cc;
	ufunc_T	*ufunc = NULL;
	type_T	*type;
	整数	done = FALSE;
	整数	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	如果 (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    返回 FAIL;
	}
	++p;
	如果 (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    返回 FAIL;
	}

	// isolate one name
	exp_name = p;
	当 (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	如果 (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	如果 (res == OK)
	{
	    如果 (si->sn_autoload_prefix != NULL
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		如果 (cc == '(' || paren_follows_after_expr)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		否则
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    否则 如果 (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		如果 (cc == '(' || paren_follows_after_expr)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		否则
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    否则
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, NULL, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	如果 (done)
	    返回 res;

	如果 (idx < 0)
	{
	    如果 (ufunc != NULL)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		如果 (IS_GENERIC_FUNC(ufunc))
		{
		    如果 (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			如果 (p != NULL)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			如果 (p == NULL || ufunc == NULL)
			    返回 FAIL;
		    }
		    否则
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			返回 FAIL;
		    }
		}
		否则 如果 (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    返回 FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, NULL, TRUE);
		返回 OK;
	    }
	    返回 FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	返回 OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    返回 generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    static 整数
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    整数			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    如果 (ufunc == NULL || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	返回 FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    如果 (ufunc == NULL)
	返回 FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    如果 (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, NULL) == FAIL)
	返回 FAIL;
    返回 generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    static 整数
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    返回 cctx->ctx_ufunc != NULL
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
    static 整数
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    整数			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    整数		isn_idx;

    如果 (fp == NULL)
	返回 FAIL;

    为 (整数 depth = 0; depth < outer_depth; ++depth)
    {
	如果 (outer_cctx->ctx_outer == NULL)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    返回 FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    如果 (fp->uf_block_ids == NULL && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(整数, ufunc->uf_block_depth);
	如果 (fp->uf_block_ids != NULL)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    sizeof(整数) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    如果 (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	返回 FAIL;

    如果 (generate_FUNCREF(cctx, fp, NULL, FALSE, 0, &isn_idx) == FAIL)
	返回 FAIL;
    如果 (cctx->ctx_skip == SKIP_YES)
	返回 OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    如果 (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	如果 (extra == NULL)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    如果 (extra == NULL)
		返回 FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    返回 OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    整数
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	整数	is_expr,
	整数	error)
{
    type_T	*type;
    char_u	*name = NULL;
    char_u	*end = end_arg;
    整数		res = FAIL;
    整数		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = NULL;

    generic_func_args_table_init(&gfatab);

    如果 (*(*arg + namelen) == '<')
    {
	// generic function call
	如果 (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == NULL)
	{
	    generic_func_args_table_clear(&gfatab);
	    返回 FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    如果 (*(*arg + 1) == ':')
    {
	如果 (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    switch (**arg)
	    {
		case 'g': isn_type = ISN_LOADGDICT; break;
		case 'w': isn_type = ISN_LOADWDICT; break;
		case 't': isn_type = ISN_LOADTDICT; break;
		case 'b': isn_type = ISN_LOADBDICT; break;
		default:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    goto theend;
	    }
	    如果 (generate_instr_type(cctx, isn_type, &t_dict_any) == NULL)
		goto theend;
	    res = OK;
	}
	否则
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    如果 (name == NULL)
		goto theend;

	    switch (**arg)
	    {
		case 'v': res = generate_LOADV(cctx, name);
			  break;
		case 's': 如果 (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      goto theend;
			  }
			  如果 (is_expr && find_func(name, FALSE) != NULL)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  否则
			      res = compile_load_scriptvar(cctx, name,
							    NULL, &end, NULL);
			  break;
		case 'g': 如果 (vim_strchr(name, AUTOLOAD_CHAR) == NULL)
			  {
			      如果 (is_expr && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != NULL
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      否则
				  isn_type = ISN_LOADG;
			  }
			  否则
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      如果 (name == NULL)
				  goto theend;
			  }
			  break;
		case 'w': isn_type = ISN_LOADW; break;
		case 't': isn_type = ISN_LOADT; break;
		case 'b': isn_type = ISN_LOADB; break;
		default:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  goto theend;
	    }
	    如果 (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    否则
    {
	整数	    idx;
	整数	    method_idx;
	整数	    gen_load = FALSE;
	整数	    gen_load_outer = 0;
	整数	    outer_loop_depth = -1;
	整数	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	如果 (name == NULL)
	    goto theend;

	如果 (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = push_type_stack(cctx, &t_super);
	    如果 (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	否则 如果 (vim_strchr(name, AUTOLOAD_CHAR) != NULL)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	否则 如果 (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    如果 (gen_load_outer == 0)
		gen_load = TRUE;
	}
	否则
	{
	    lvar_T  lvar;
	    class_T *cl = NULL;

	    如果 (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		如果 (lvar.lv_ufunc != NULL && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		否则 如果 (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		否则
		    gen_load = TRUE;
	    }
	    否则 如果 (cctx->ctx_ufunc->uf_defclass != NULL &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		如果 (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    如果 (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    否则
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									NULL);
		    }
		}
		否则
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    否则
	    {
		imported_T *imp = NULL;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		如果 (script_var_exists(*arg, namelen, cctx, NULL) == OK
			    || (imp = find_imported(name, 0, FALSE)) != NULL)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		如果 (res == FAIL && is_expr && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	如果 (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, NULL, type);
	如果 (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    如果 (lt != NULL)
	*lt = '<';
    如果 (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    返回 res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    static 整数
compile_string(isn_T *isn, cctx_T *cctx, 整数 str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    整数		expr_res;
    整数		trailing_error;
    整数		instr_count;
    isn_T	*instr = NULL;

    // Remove the string type from the stack.
    --cctx->ctx_type_stack.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = NULL;

    // avoid peeking a next line
    整数 galen_save = cctx->ctx_ufunc->uf_lines.ga_len;
    cctx->ctx_ufunc->uf_lines.ga_len = 0;

    expr_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_lines.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    如果 (expr_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	如果 (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_stack.ga_len;
	返回 FAIL;
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
    返回 OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    整数
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	整数	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    整数	    must_end = FALSE;
    整数	    instr_count;

    为 (;;)
    {
	如果 (may_get_next_line(whitep, &p, cctx) == FAIL)
	    goto failret;
	如果 (*p == ')')
	{
	    *arg = p + 1;
	    返回 OK;
	}
	如果 (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    返回 FAIL;
	}

	instr_count = cctx->ctx_instr.ga_len;
	如果 (compile_expr0(&p, cctx) == FAIL)
	    返回 FAIL;
	++*argcount;

	如果 (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_count + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_count;

	    // {skip} argument of searchpair() can be compiled if not empty
	    如果 (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != NULL
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	否则 如果 (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_count + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_count;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    如果 (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != NULL
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	如果 (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    返回 FAIL;
	}
	如果 (*p == ',')
	{
	    ++p;
	    如果 (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		返回 FAIL;
	    }
	}
	否则
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    返回 FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    static 整数
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_stack(cctx, 0);
    整数		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    如果 (type->tt_type == VAR_OBJECT)
    {
	整数	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	如果 (uf != NULL)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    返回 res;
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
    static 整数
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	整数	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    整数		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = NULL;
    char_u	*gname = NULL;
    ufunc_T	*ufunc = NULL;
    整数		res = FAIL;
    整数		is_autoload;
    整数		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    如果 (varlen >= sizeof(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	返回 FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    如果 (import != NULL)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	返回 FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    如果 ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	整数	    is_has = **arg == 'h';
	整数	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	如果 (*s == '"')
	    (void)eval_string(&s, &argvars[0], TRUE, FALSE);
	否则 如果 (*s == '\'')
	    (void)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	如果 (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    如果 (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_NUMBER;
		tv->vval.v_number = 0;
		如果 (is_has)
		    f_has(argvars, tv);
		否则 如果 (is_len)
		    f_len(argvars, tv);
		否则
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    返回 OK;
	}
	clear_tv(&argvars[0]);
	如果 (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    返回 FAIL;
	}
    }

    如果 (generate_ppconst(cctx, ppconst) == FAIL)
	返回 FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    如果 ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    否则 如果 (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    否则
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    如果 (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	如果 (*arg == NULL)
	    goto theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	如果 (gname == NULL)
	    goto theend;
	++*arg;			// skip '('
    }
    否则
	*arg += varlen + 1;

    如果 (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	goto theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != NULL;
    如果 (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	整数	    idx;

	// builtin function
	idx = find_internal_func(name);
	如果 (idx >= 0)
	{
	    如果 (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		goto theend;
	    }

	    如果 (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_stack(cctx, 1);
		如果 (check_type_is_value(get_type_on_stack(cctx, 0)) == FAIL)
		    goto theend;

		// add() can be compiled to instructions if we know the type
		如果 (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		否则 如果 (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    如果 ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		如果 (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    如果 (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    否则 如果 (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    否则 如果 (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    如果 (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		如果 (res == OK)
		    idx = -1;
	    }

	    如果 (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	否则
	    emsg_funcname(e_unknown_function_str, namebuf);
	goto theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    如果 (lookup_local(namebuf, varlen, NULL, cctx) == FAIL
	    && arg_exists(namebuf, varlen, NULL, NULL, NULL, cctx) != OK)
    {
	class_T		*cl = NULL;
	整数		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	如果 (ufunc != NULL)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    如果 (ufunc == NULL)
		goto theend;

	    如果 (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, NULL, 0, argcount, FALSE);
		goto theend;
	    }
	    如果 (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == NULL
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		goto theend;
	    }
	}
	否则 如果 ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    如果 (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], NULL,
							0, argcount, FALSE);
	    }
	    否则
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
    如果 (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_stack(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	goto theend;
    }

    // If we can find a global function by name generate the right call.
    如果 (ufunc != NULL)
    {
	res = generate_CALL(cctx, ufunc, NULL, 0, argcount, FALSE);
	goto theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    如果 (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    否则
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    返回 res;
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
to_name_end(char_u *arg, 整数 use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    如果 (!eval_isnamec1(*arg))
	返回 arg;

    为 (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	如果 (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == NULL))
	    break;
    返回 p;
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

    如果 (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    如果 (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	如果 (eval_list(&p, &rettv, NULL, FALSE) == FAIL)
	    p = arg;
    }
    返回 p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    static 整数
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    整数		count = 0;
    整数		is_const;
    整数		is_all_const = TRUE;	// reset when non-const encountered
    整数		must_end = FALSE;

    为 (;;)
    {
	如果 (may_get_next_line(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    返回 FAIL;
	}
	如果 (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    返回 FAIL;
	}
	如果 (*p == ']')
	{
	    ++p;
	    break;
	}
	如果 (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    返回 FAIL;
	}
	如果 (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    返回 FAIL;
	如果 (!is_const)
	    is_all_const = FALSE;
	++count;
	如果 (*p == ',')
	{
	    ++p;
	    如果 (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		返回 FAIL;
	    }
	}
	否则
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    返回 generate_NEWLIST(cctx, count, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    static 整数
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    整数 first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    整数		count = 0;
    整数		is_const;
    整数		is_all_const = TRUE;	// reset when non-const encountered
    整数		must_end = FALSE;

    如果 (**arg != ')')
    {
	如果 (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    返回 FAIL;
	}
	count = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	为 (;;)
	{
	    如果 (may_get_next_line(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		返回 FAIL;
	    }
	    如果 (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		返回 FAIL;
	    }
	    如果 (*p == ')')
	    {
		++p;
		break;
	    }
	    如果 (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		返回 FAIL;
	    }
	    如果 (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		返回 FAIL;
	    如果 (!is_const)
		is_all_const = FALSE;
	    ++count;
	    如果 (*p == ',')
	    {
		++p;
		如果 (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    返回 FAIL;
		}
	    }
	    否则
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    返回 generate_NEWTUPLE(cctx, count, FALSE);
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

    如果 (!evalarg->eval_using_cmdline || cctx == NULL)
	返回;

    gap = &evalarg->eval_tofree_ga;
    如果 (gap->ga_len == 0)
	返回;

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
    整数
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    整数		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    如果 (r != OK)
    {
	clear_evalarg(&evalarg, NULL);
	返回 r;
    }

    如果 (ppconst != NULL && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, NULL);
	返回 FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    如果 (cctx->ctx_ufunc != NULL)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	整数 block_depth = cctx->ctx_ufunc->uf_block_depth;

	如果 (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(整数, block_depth);
	    如果 (ufunc->uf_block_ids != NULL)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    sizeof(整数) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    如果 (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    如果 (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    如果 (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	如果 (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, NULL);

    如果 (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	返回 generate_FUNCREF(cctx, ufunc, NULL, FALSE, 0, NULL);
    }

    func_ptr_unref(ufunc);
    返回 FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    整数
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	整数	    types_optional,
	evalarg_T   *evalarg)
{
    整数		r;
    ufunc_T	*ufunc;
    整数		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, NULL);
    current_sctx.sc_version = save_sc_version;
    如果 (r != OK)
	返回 r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    如果 (ufunc->uf_ret_type == NULL || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, NULL);

    如果 (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	返回 OK;
    }
    clear_tv(rettv);
    返回 FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    static 整数
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    整数		count = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    整数		is_const;
    整数		is_all_const = TRUE;	// reset when non-const encountered

    如果 (d == NULL)
	返回 FAIL;
    如果 (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	返回 FAIL;
    }
    为 (;;)
    {
	char_u	    *key = NULL;

	如果 (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = NULL;
	    goto failret;
	}

	如果 (**arg == '}')
	    break;

	如果 (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    如果 (compile_expr0(arg, cctx) == FAIL)
		goto failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    如果 (isn->isn_type == ISN_PUSHNR)
	    {
		char buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.number);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    如果 (isn->isn_type == ISN_PUSHS)
		key = isn->isn_arg.string;
	    否则 如果 (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		goto failret;
	    *arg = skipwhite(*arg);
	    如果 (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_key));
		goto failret;
	    }
	    ++*arg;
	}
	否则
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    key = get_literal_key(arg);
	    如果 (key == NULL)
		goto failret;
	    如果 (generate_PUSHS(cctx, &key) == FAIL)
		goto failret;
	}

	// Check for duplicate keys, if using string keys.
	如果 (key != NULL)
	{
	    item = dict_find(d, key, -1);
	    如果 (item != NULL)
	    {
		semsg(_(e_duplicate_key_in_dictionary_str), key);
		goto failret;
	    }
	    item = dictitem_alloc(key);
	    如果 (item != NULL)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		如果 (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	如果 (**arg != ':')
	{
	    如果 (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    否则
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    goto failret;
	}
	whitep = *arg + 1;
	如果 (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    goto failret;
	}

	如果 (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = NULL;
	    goto failret;
	}

	如果 (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    goto failret;
	如果 (!is_const)
	    is_all_const = FALSE;
	++count;

	whitep = *arg;
	如果 (may_get_next_line(whitep, arg, cctx) == FAIL)
	{
	    *arg = NULL;
	    goto failret;
	}
	如果 (**arg == '}')
	    break;
	如果 (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    goto failret;
	}
	如果 (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    goto failret;
	}
	whitep = *arg + 1;
	如果 (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    goto failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    如果 (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    返回 generate_NEWDICT(cctx, count, FALSE);

failret:
    如果 (*arg == NULL)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    返回 FAIL;
}

/*
 * Compile "&option".
 */
    static 整数
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    整数		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    如果 (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_NUMBER ? &t_number : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    返回 ret;
}

/*
 * Compile "$VAR".
 */
    static 整数
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    整数		len;
    整数		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    如果 (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	返回 FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    返回 ret;
}

/*
 * Compile $"string" or $'string'.
 */
    static 整数
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    整数		ret;
    整数		quote;
    整数		evaluate = cctx->ctx_skip != SKIP_YES;
    整数		count = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    为 (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	如果 (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	否则
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	如果 (ret == FAIL)
	    break;
	如果 (evaluate)
	{
	    如果 ((tv.vval.v_string != NULL && *tv.vval.v_string != NUL)
		    || (**arg != '{' && count == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		如果 (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    返回 FAIL;
		tv.vval.v_string = NULL;  // don't free it now
		++count;
	    }
	    clear_tv(&tv);
	}

	如果 (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    break;
	}

	p = compile_one_expr_in_str(*arg, cctx);
	如果 (p == NULL)
	{
	    ret = FAIL;
	    break;
	}
	++count;
	*arg = p;
    }

    如果 (ret == FAIL || !evaluate)
	返回 ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    如果 (count > 1)
	返回 generate_CONCAT(cctx, count);

    返回 OK;
}

/*
 * Compile "@r".
 */
    static 整数
compile_get_register(char_u **arg, cctx_T *cctx)
{
    整数		ret;

    ++*arg;
    如果 (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	返回 FAIL;
    }
    如果 (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	返回 FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, NULL, &t_string);
    ++*arg;
    返回 ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    static 整数
apply_leader(typval_T *rettv, 整数 numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    当 (p > start)
    {
	--p;
	如果 (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    如果 (rettv->v_type == VAR_FLOAT)
	    {
		如果 (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    否则
	    {
		varnumber_T	val;
		整数		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		如果 (check_not_string(rettv) == FAIL)
		    返回 FAIL;
		val = tv_get_number_chk(rettv, &error);
		clear_tv(rettv);
		如果 (error)
		    返回 FAIL;
		如果 (*p == '-')
		    val = -val;
		rettv->v_type = VAR_NUMBER;
		rettv->vval.v_number = val;
	    }
	}
	否则 如果 (numeric_only)
	{
	    ++p;
	    break;
	}
	否则 如果 (*p == '!')
	{
	    整数 v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_number = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    返回 OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    static void
get_vim_constant(char_u **arg, typval_T *rettv)
{
    如果 (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_number = VVAL_TRUE;
	*arg += 6;
    }
    否则 如果 (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_number = VVAL_FALSE;
	*arg += 7;
    }
    否则 如果 (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_number = VVAL_NULL;
	*arg += 6;
    }
    否则 如果 (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_number = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, 整数 *len, 整数 *type_is)
{
    exprtype_T	type = EXPR_UNKNOWN;
    整数		i;

    switch (p[0])
    {
	case '=':   如果 (p[1] == '=')
			type = EXPR_EQUAL;
		    否则 如果 (p[1] == '~')
			type = EXPR_MATCH;
		    break;
	case '!':   如果 (p[1] == '=')
			type = EXPR_NEQUAL;
		    否则 如果 (p[1] == '~')
			type = EXPR_NOMATCH;
		    break;
	case '>':   如果 (p[1] != '=')
		    {
			type = EXPR_GREATER;
			*len = 1;
		    }
		    否则
			type = EXPR_GEQUAL;
		    break;
	case '<':   如果 (p[1] != '=')
		    {
			type = EXPR_SMALLER;
			*len = 1;
		    }
		    否则
			type = EXPR_SEQUAL;
		    break;
	case 'i':   如果 (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			如果 (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			如果 (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? EXPR_IS : EXPR_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    break;
    }
    返回 type;
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
    整数
bool_on_stack(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_stack(cctx, 0);
    如果 (type == &t_bool)
	返回 OK;

    如果 (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_NUMBER
	    || type == &t_number_bool
	    || type == &t_const_number_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	返回 generate_COND2BOOL(cctx);

    返回 need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    void
error_white_both(char_u *op, 整数 len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    static 整数
compile_leader(cctx_T *cctx, 整数 numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    当 (p > start)
    {
	--p;
	当 (VIM_ISWHITE(*p))
	    --p;
	如果 (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_stack(cctx, 0);
	    如果 (type->tt_type != VAR_FLOAT && need_type(type, &t_number,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		返回 FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    如果 (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == NULL)
		返回 FAIL;
	}
	否则 如果 (numeric_only)
	{
	    ++p;
	    break;
	}
	否则
	{
	    整数  invert = *p == '!';

	    当 (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		如果 (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    如果 (check_type_is_value(get_type_on_stack(cctx, 0)) == FAIL)
		返回 FAIL;
	    如果 (generate_2BOOL(cctx, invert, -1) == FAIL)
		返回 FAIL;
	}
    }
    *end = p;
    返回 OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    static 整数
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    整数	    ret;
    char_u  *p = *arg + 1;

    如果 (may_get_next_line_error(p, arg, cctx) == FAIL)
	返回 FAIL;

    如果 (**arg == ')')
	// empty tuple
	返回 compile_tuple(arg, cctx, ppconst, FALSE);

    如果 (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    否则
    {
	// Not enough space in ppconst, flush constants.
	如果 (generate_ppconst(cctx, ppconst) == FAIL)
	    返回 FAIL;
	ret = compile_expr0(arg, cctx);
    }
    如果 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
	返回 FAIL;
    如果 (ret == OK && **arg == ',')
    {
	// tuple
	整数 is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	如果 (generate_ppconst(cctx, ppconst) == FAIL)
	    返回 FAIL;
	返回 compile_tuple(arg, cctx, ppconst, is_const);
    }

    如果 (**arg == ')')
	++*arg;
    否则 如果 (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    返回 ret;
}

static 整数 compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    static 整数
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    整数		keeping_dict = FALSE;

    为 (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	如果 (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_line_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    如果 (next != NULL &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_line_from_context(cctx, TRUE);
		如果 (next == NULL)
		    返回 FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	如果 (**arg == '(')
	{
	    整数		argcount = 0;

	    如果 (generate_ppconst(cctx, ppconst) == FAIL)
		返回 FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_stack(cctx, 0);

	    *arg = skipwhite(p + 1);
	    如果 (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		返回 FAIL;
	    如果 (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		返回 FAIL;
	    如果 (keeping_dict)
	    {
		keeping_dict = FALSE;
		如果 (generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    返回 FAIL;
	    }
	}
	否则 如果 (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    整数	    alt;
	    char_u  *paren;

	    // something->method()
	    如果 (generate_ppconst(cctx, ppconst) == FAIL)
		返回 FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_stack(cctx, 0);
	    如果 (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_value));
		返回 FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    如果 (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		返回 FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    如果 (**arg == '(')
		alt = 2;
	    否则
	    {
		// alternative 1 or 3
		p = *arg;
		如果 (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    返回 FAIL;
		}
		如果 (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		为 ( ; eval_isnamec(*p); ++p)
		    ;
		如果 (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    如果 (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			返回 FAIL;
		}
		否则
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    如果 (paren == NULL)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			返回 FAIL;
		    }
		}
	    }

	    如果 (alt != 1)
	    {
		整数	    argcount = 1;
		garray_T    *stack = &cctx->ctx_type_stack;
		整数	    type_idx_start = stack->ga_len;
		整数	    expr_isn_start = cctx->ctx_instr.ga_len;
		整数	    expr_isn_end;
		整数	    arg_isn_count;

		如果 (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    如果 (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			返回 FAIL;
		}
		否则
		{
		    整数 fail;
		    整数 save_len = cctx->ctx_ufunc->uf_lines.ga_len;
		    整数	prev_did_emsg = did_emsg;

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

		    如果 (fail)
		    {
			如果 (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			返回 FAIL;
		    }
		}

		// Compile the arguments.
		如果 (**arg != '(')
		{
		    如果 (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    否则
			semsg(_(e_missing_parenthesis_str), *arg);
		    返回 FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		expr_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		如果 (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    返回 FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_count = cctx->ctx_instr.ga_len - expr_isn_end;
		如果 (arg_isn_count > 0)
		{
		    整数	    expr_isn_count = expr_isn_end - expr_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, expr_isn_count);
		    type_T  *decl_type;
		    type2_T  *typep;

		    如果 (isn == NULL)
			返回 FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + expr_isn_start,
					       sizeof(isn_T) * expr_isn_count);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + expr_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + expr_isn_end,
						sizeof(isn_T) * arg_isn_count);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + expr_isn_start + arg_isn_count,
					  isn, sizeof(isn_T) * expr_isn_count);
		    vim_free(isn);

		    typep = ((type2_T *)stack->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)stack->ga_data) + type_idx_start,
			      ((type2_T *)stack->ga_data) + type_idx_start + 1,
			      sizeof(type2_T)
				       * (stack->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)stack->ga_data) + stack->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_stack(cctx, 0);
		如果 (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    返回 FAIL;
	    }

	    如果 (keeping_dict)
	    {
		keeping_dict = FALSE;
		如果 (generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    返回 FAIL;
	    }
	}
	否则 如果 (**arg == '[')
	{
	    整数		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    如果 (generate_ppconst(cctx, ppconst) == FAIL)
		返回 FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    如果 (may_get_next_line_error(p, arg, cctx) == FAIL)
		返回 FAIL;
	    如果 (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    否则
	    {
		如果 (compile_expr0(arg, cctx) == FAIL)
		    返回 FAIL;
		如果 (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    返回 FAIL;
		}
		如果 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
		    返回 FAIL;
		*arg = skipwhite(*arg);
	    }
	    如果 (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		如果 (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    返回 FAIL;
		}
		如果 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
		    返回 FAIL;
		如果 (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		否则
		{
		    如果 (compile_expr0(arg, cctx) == FAIL)
			返回 FAIL;
		    如果 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
			返回 FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    如果 (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		返回 FAIL;
	    }
	    *arg = *arg + 1;

	    如果 (keeping_dict)
	    {
		keeping_dict = FALSE;
		如果 (generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    返回 FAIL;
	    }
	    如果 (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		返回 FAIL;
	}
	否则 如果 (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    如果 (generate_ppconst(cctx, ppconst) == FAIL)
		返回 FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_stack(cctx, 0);
	    如果 (type != &t_unknown
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
		如果 (compile_class_object_index(cctx, arg, type) == FAIL)
		    返回 FAIL;
	    }
	    否则
	    {
		*arg = p + 1;
		如果 (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    返回 FAIL;
		}
		p = *arg;
		当 (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		如果 (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    返回 FAIL;
		}
		如果 (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == NULL)
		    返回 FAIL;
		如果 (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    返回 FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	否则
	    break;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    如果 (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == NULL)
	返回 FAIL;

    返回 OK;
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
    static 整数
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    整数		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    整数		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    如果 (eval_leader(arg, TRUE) == FAIL)
	返回 FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    switch (**arg)
    {
	/*
	 * Number constant.
	 */
	case '0':	// also for blob starting with 0z
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case '.':   如果 (eval_number(arg, rettv, TRUE, FALSE) == FAIL)
			返回 FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    如果 (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			返回 FAIL;
		    }
		    break;

	/*
	 * String constant: "string".
	 */
	case '"':   如果 (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			返回 FAIL;
		    break;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	case '\'':  如果 (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			返回 FAIL;
		    break;

	/*
	 * Constant Vim variable.
	 */
	case 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    break;

	/*
	 * "true" constant
	 */
	case 't':   如果 (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_number = VVAL_TRUE;
		    }
		    否则
			ret = NOTDONE;
		    break;

	/*
	 * "false" constant
	 */
	case 'f':   如果 (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_number = VVAL_FALSE;
		    }
		    否则
			ret = NOTDONE;
		    break;

	/*
	 * "null" or "null_*" constant
	 */
	case 'n':   如果 (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			整数	len;

			为 (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			如果 (ret == FAIL)
			    ret = NOTDONE;
			否则
			    *arg += len + 4;
		    }
		    否则
			ret = NOTDONE;
		    break;

	/*
	 * List: [expr, expr]
	 */
	case '[':   如果 (generate_ppconst(cctx, ppconst) == FAIL)
			返回 FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    break;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	case '{':   如果 (generate_ppconst(cctx, ppconst) == FAIL)
			返回 FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    break;

	/*
	 * Option value: &name
	 */
	case '&':	如果 (generate_ppconst(cctx, ppconst) == FAIL)
			    返回 FAIL;
			ret = compile_get_option(arg, cctx);
			break;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	case '$':	如果 (generate_ppconst(cctx, ppconst) == FAIL)
			    返回 FAIL;
			如果 ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			否则
			    ret = compile_get_env(arg, cctx);
			break;

	/*
	 * Register contents: @r.
	 */
	case '@':	如果 (generate_ppconst(cctx, ppconst) == FAIL)
			    返回 FAIL;
			ret = compile_get_register(arg, cctx);
			break;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	case '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    如果 (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    break;

	default:    ret = NOTDONE;
		    break;
    }
    如果 (ret == FAIL)
	返回 FAIL;

    如果 (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	如果 (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	否则
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    否则 如果 (ret == NOTDONE)
    {
	char_u	    *p;
	整数	    r;

	如果 (!eval_isnamec1(**arg))
	{
	    如果 (!vim9_bad_comment(*arg))
	    {
		如果 (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		否则
		    semsg(_(e_name_expected_str), *arg);
	    }
	    返回 FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	如果 (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    返回 FAIL;
	}

	size_t namelen = p - *arg;
	如果 (*p == '<')
	{
	    如果 (skip_generic_func_type_args(&p) == FAIL)
		返回 FAIL;
	}

	如果 (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	否则
	{
	    如果 (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		返回 FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	如果 (r == FAIL)
	    返回 FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    如果 (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	返回 FAIL;
    如果 ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	如果 (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    返回 FAIL;
	返回 OK;
    }
    如果 (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	返回 FAIL;
    返回 OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    static 整数
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = NULL;

    // Recognize <type>
    如果 (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	如果 (want_type == NULL)
	    返回 FAIL;

	如果 (**arg != '>')
	{
	    如果 (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    否则
		emsg(_(e_missing_gt));
	    返回 FAIL;
	}
	++*arg;
	如果 (may_get_next_line_error(*arg, arg, cctx) == FAIL)
	    返回 FAIL;
    }

    如果 (compile_expr9(arg, cctx, ppconst) == FAIL)
	返回 FAIL;

    如果 (want_type != NULL)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_stack(cctx, 0);
	如果 (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    如果 (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		返回 FAIL;
	}
    }

    返回 OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    static 整数
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    整数		ppconst_used = ppconst->pp_used;

    // get the first expression
    如果 (compile_expr8(arg, cctx, ppconst) == FAIL)
	返回 FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    为 (;;)
    {
	op = may_peek_next_line(cctx, *arg, &next);
	如果 (*op != '*' && *op != '/' && *op != '%')
	    break;
	如果 (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	如果 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    返回 FAIL;
	}
	如果 (may_get_next_line_error(op + 1, arg, cctx) == FAIL)
	    返回 FAIL;

	// get the second expression
	如果 (compile_expr8(arg, cctx, ppconst) == FAIL)
	    返回 FAIL;

	如果 (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_NUMBER
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_NUMBER)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    整数		    failed = FALSE;

	    // both are numbers: compute the result
	    switch (*op)
	    {
		case '*': res = tv1->vval.v_number * tv2->vval.v_number;
			  break;
		case '/': res = num_divide(tv1->vval.v_number,
						  tv2->vval.v_number, &failed);
			  break;
		case '%': res = num_modulus(tv1->vval.v_number,
						  tv2->vval.v_number, &failed);
			  break;
	    }
	    如果 (failed)
		返回 FAIL;
	    tv1->vval.v_number = res;
	    --ppconst->pp_used;
	}
	否则
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    返回 OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    static 整数
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    整数		oplen;
    整数		ppconst_used = ppconst->pp_used;

    // get the first variable
    如果 (compile_expr7(arg, cctx, ppconst) == FAIL)
	返回 FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    为 (;;)
    {
	op = may_peek_next_line(cctx, *arg, &next);
	如果 (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    break;
	如果 (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    break;
	oplen = (*op == '.' ? 2 : 1);
	如果 (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	如果 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    返回 FAIL;
	}

	如果 (may_get_next_line_error(op + oplen, arg, cctx) == FAIL)
	    返回 FAIL;

	// get the second expression
	如果 (compile_expr7(arg, cctx, ppconst) == FAIL)
	    返回 FAIL;

	如果 (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_NUMBER
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_NUMBER)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    如果 (*op == '+')
		tv1->vval.v_number = tv1->vval.v_number + tv2->vval.v_number;
	    否则 如果 (*op == '-')
		tv1->vval.v_number = tv1->vval.v_number - tv2->vval.v_number;
	    否则
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		如果 (tv1->vval.v_string == NULL)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    返回 FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	否则
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    如果 (*op == '.')
	    {
		如果 (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    返回 FAIL;
		如果 (generate_CONCAT(cctx, 2) == FAIL)
		    返回 FAIL;
	    }
	    否则
		generate_two_op(cctx, op);
	}
    }

    返回 OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    static 整数
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = EXPR_UNKNOWN;
    char_u	*p;
    char_u	*next;
    整数		len = 2;
    整数		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    如果 (compile_expr6(arg, cctx, ppconst) == FAIL)
	返回 FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    为 (;;)
    {
	type = EXPR_UNKNOWN;

	p = may_peek_next_line(cctx, *arg, &next);
	如果 (p[0] == '<' && p[1] == '<')
	    type = EXPR_LSHIFT;
	否则 如果 (p[0] == '>' && p[1] == '>')
	    type = EXPR_RSHIFT;

	如果 (type == EXPR_UNKNOWN)
	    返回 OK;

	// Handle a bitwise left or right shift operator
	如果 (ppconst->pp_used == ppconst_used + 1)
	{
	    如果 (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_NUMBER)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_number));
		返回 FAIL;
	    }
	}
	否则
	{
	    type_T	*t = get_type_on_stack(cctx, 0);

	    如果 (need_type(t, &t_number, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_number));
		返回 FAIL;
	    }
	}

	如果 (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	如果 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    返回 FAIL;
	}

	// get the second variable
	如果 (may_get_next_line_error(p + len, arg, cctx) == FAIL)
	    返回 FAIL;

	如果 (compile_expr6(arg, cctx, ppconst) == FAIL)
	    返回 FAIL;

	如果 (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    如果 (tv2->v_type != VAR_NUMBER || tv2->vval.v_number < 0)
	    {
		// right operand should be a positive number
		如果 (tv2->v_type != VAR_NUMBER)
		    emsg(_(e_bitshift_ops_must_be_number));
		否则
		    emsg(_(e_bitshift_ops_must_be_positive));
		返回 FAIL;
	    }

	    如果 (tv2->vval.v_number > MAX_LSHIFT_BITS)
		tv1->vval.v_number = 0;
	    否则 如果 (type == EXPR_LSHIFT)
		tv1->vval.v_number =
			(uvarnumber_T)tv1->vval.v_number << tv2->vval.v_number;
	    否则
		tv1->vval.v_number =
			(uvarnumber_T)tv1->vval.v_number >> tv2->vval.v_number;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	否则
	{
	    如果 (need_type(get_type_on_stack(cctx, 0), &t_number, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_number));
		返回 FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    如果 (isn == NULL)
		返回 FAIL;

	    如果 (isn != NULL)
		isn->isn_arg.op.op_type = type;
	}
    }

    返回 OK;
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
    static 整数
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = EXPR_UNKNOWN;
    char_u	*p;
    char_u	*next;
    整数		len = 2;
    整数		type_is = FALSE;
    整数		ppconst_used = ppconst->pp_used;

    // get the first variable
    如果 (compile_expr5(arg, cctx, ppconst) == FAIL)
	返回 FAIL;

    p = may_peek_next_line(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    如果 (type != EXPR_UNKNOWN)
    {
	整数 ic = FALSE;  // Default: do not ignore case

	如果 (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	如果 (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    返回 FAIL;
	}
	// extra question mark appended: ignore case
	如果 (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	否则 如果 (p[len] == '#')
	    ++len;
	// nothing appended: match case

	如果 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    返回 FAIL;
	}

	// get the second variable
	如果 (may_get_next_line_error(p + len, arg, cctx) == FAIL)
	    返回 FAIL;

	如果 (compile_expr5(arg, cctx, ppconst) == FAIL)
	    返回 FAIL;

	如果 (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    整数		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    如果 (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    否则
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_number = tv1->vval.v_number
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    返回 ret;
	}

	generate_ppconst(cctx, ppconst);
	返回 generate_COMPARE(cctx, type, ic);
    }

    返回 OK;
}

static 整数 compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    static 整数
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	char	*op,
	ppconst_T *ppconst,
	整数	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_line(cctx, *arg, &next);
    整数		opchar = *op;

    如果 (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	整数		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, sizeof(整数), 10);
	当 (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    整数		start_ctx_lnum = cctx->ctx_lnum;
	    整数		save_lnum;
	    整数		const_used;
	    整数		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    如果 (next != NULL)
	    {
		*arg = next_line_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    如果 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		返回 FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    如果 (status != FAIL)
	    {
		// Use the last ppconst if possible.
		如果 (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    整数		is_true = tv2bool(tv);

		    如果 ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_number = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    否则
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		否则
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_stack(cctx);
		}
	    }
	    如果 (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    如果 (status == FAIL)
	    {
		ga_clear(&end_ga);
		返回 FAIL;
	    }

	    如果 (jump_when != JUMP_NEVER)
	    {
		如果 (cctx->ctx_skip != SKIP_YES)
		{
		    *(((整数 *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    如果 (may_get_next_line_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		返回 FAIL;
	    }

	    const_used = ppconst->pp_used;
	    如果 ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		返回 FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    如果 (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		如果 (tv->v_type == VAR_NUMBER
			 && (tv->vval.v_number == 1 || tv->vval.v_number == 0))
		{
		    tv->vval.v_number = tv->vval.v_number == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_line(cctx, *arg, &next);
	}

	如果 (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    返回 FAIL;
	}

	如果 (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    如果 (bool_on_stack(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		返回 FAIL;
	    }

	如果 (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    当 (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((整数 *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    返回 OK;
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
    static 整数
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    整数		ppconst_used = ppconst->pp_used;

    // get the first variable
    如果 (compile_expr4(arg, cctx, ppconst) == FAIL)
	返回 FAIL;

    // || and && work almost the same
    返回 compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    static 整数
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    整数		ppconst_used = ppconst->pp_used;

    // eval the first expression
    如果 (compile_expr3(arg, cctx, ppconst) == FAIL)
	返回 FAIL;

    // || and && work almost the same
    返回 compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    整数
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    整数		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    如果 (cctx->ctx_skip == SKIP_YES)
    {
	整数		prev_did_emsg = did_emsg;

	skip_expr_cctx(arg, cctx);
	返回 did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    如果 (compile_expr2(arg, cctx, ppconst) == FAIL)
	返回 FAIL;

    p = may_peek_next_line(cctx, *arg, &next);
    如果 (*p == '?')
    {
	整数		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*stack = &cctx->ctx_type_stack;
	整数		alt_idx = instr->ga_len;
	整数		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = NULL;
	整数		has_const_expr = FALSE;
	整数		const_value = FALSE;
	整数		save_skip = cctx->ctx_skip;

	如果 (next != NULL)
	{
	    *arg = next_line_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	如果 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    返回 FAIL;
	}

	如果 (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_expr = TRUE;
	    如果 (op_falsy)
		const_value = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    否则
	    {
		整数 error = FALSE;

		const_value = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		如果 (error)
		    返回 FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_value : !const_value) ? SKIP_YES : SKIP_NOT;

	    如果 (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    否则
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	否则
	{
	    generate_ppconst(cctx, ppconst);
	    如果 (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    如果 (op_falsy)
	    {
		type1 = get_type_on_stack(cctx, -1);
		如果 (check_type_is_value(type1) == FAIL)
		    返回 FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	如果 (may_get_next_line_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    返回 FAIL;
	如果 (compile_expr1(arg, cctx, ppconst) == FAIL)
	    返回 FAIL;

	如果 (!has_const_expr)
	{
	    generate_ppconst(cctx, ppconst);

	    如果 (!op_falsy)
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

	如果 (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_line(cctx, *arg, &next);
	    如果 (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		返回 FAIL;
	    }
	    如果 (next != NULL)
	    {
		*arg = next_line_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    如果 (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		返回 FAIL;
	    }

	    // evaluate the third expression
	    如果 (has_const_expr)
		cctx->ctx_skip = save_skip == SKIP_YES || const_value
							 ? SKIP_YES : SKIP_NOT;
	    如果 (may_get_next_line_error(p + 1, arg, cctx) == FAIL)
		返回 FAIL;
	    如果 (compile_expr1(arg, cctx, ppconst) == FAIL)
		返回 FAIL;
	}

	如果 (!has_const_expr)
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
    返回 OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    整数
compile_expr0_ext(char_u **arg,  cctx_T *cctx, 整数 *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    如果 (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	返回 FAIL;
    }
    如果 (is_const != NULL)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    如果 (generate_ppconst(cctx, &ppconst) == FAIL)
	返回 FAIL;
    返回 OK;
}

/*
 * Toplevel expression.
 */
    整数
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    返回 compile_expr0_ext(arg, cctx, NULL);
}


#endif // defined(FEAT_EVAL)
