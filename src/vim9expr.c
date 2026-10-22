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
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
ایستا صحیح paren_follows_after_عبارت = 0;

/*
 * Generate code for any ppconst entries.
 */
    صحیح
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    صحیح	    i;
    صحیح	    ret = OK;
    صحیح	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    برای (i = 0; i < ppconst->pp_used; ++i)
	اگر (generate_tv_افزودن(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    بازگشت ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    ایستا صحیح
check_ppconst_bool(ppconst_T *ppconst)
{
    اگر (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	بازگشت check_typval_type(&t_bool, tv, where);
    }
    بازگشت OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    تهی
clear_ppconst(ppconst_T *ppconst)
{
    صحیح	    i;

    برای (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    ایستا صحیح
compile_tuple_member(
    type2_T	*typep,
    صحیح		is_slice,
    cctx_T	*cctx)
{
    اگر (is_slice)
    {
	اگر (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    بازگشت FAIL;
	// a copy is made so the member type is no longer declared
	اگر (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	اگر (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    اگر (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    وگرنه
    {
	اگر (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    اگر (typep->type_curr->tt_argcount == 1)
	    {
		اگر (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		وگرنه
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    وگرنه
		typep->type_curr = &t_any;
	    اگر (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		اگر (typep->type_decl->tt_argcount == 1)
		{
		    اگر (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    وگرنه
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		وگرنه
		    typep->type_curr = &t_any;
	    }
	    وگرنه
		typep->type_decl = typep->type_curr;
	}
	اگر (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    بازگشت FAIL;
    }

    بازگشت OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    صحیح
compile_member(صحیح is_slice, صحیح *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*پشته = &cctx->ctx_type_پشته;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)پشته->ga_data) + پشته->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)پشته->ga_data) + پشته->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    اگر ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    اگر (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	اگر (need_type(idxtype, &t_عدد, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    بازگشت FAIL;
	اگر (is_slice)
	{
	    idxtype = get_type_on_پشته(cctx, 1);
	    اگر (need_type(idxtype, &t_عدد, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		بازگشت FAIL;
	}
    }

    اگر (vartype == VAR_DICT)
    {
	اگر (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    بازگشت FAIL;
	}
	اگر (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    اگر (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    اگر (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		اگر (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    وگرنه
		typep->type_decl = typep->type_curr;
	}
	وگرنه
	{
	    اگر (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		بازگشت FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	اگر (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    بازگشت FAIL;
	اگر (keeping_dict != پوچ)
	    *keeping_dict = TRUE;
    }
    وگرنه اگر (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	اگر ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    بازگشت FAIL;
    }
    وگرنه اگر (vartype == VAR_BLOB)
    {
	اگر (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    اگر (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		بازگشت FAIL;
	}
	وگرنه
	{
	    typep->type_curr = &t_عدد;
	    typep->type_decl = &t_عدد;
	    اگر (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		بازگشت FAIL;
	}
    }
    وگرنه اگر (vartype == VAR_TUPLE)
    {
	اگر (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    بازگشت FAIL;
    }
    وگرنه اگر (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	اگر (is_slice)
	{
	    اگر (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		بازگشت FAIL;
	    // a copy is made so the member type is no longer declared
	    اگر (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    اگر (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		اگر (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	وگرنه
	{
	    اگر (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		اگر (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		اگر (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    اگر (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		وگرنه
			typep->type_decl = typep->type_curr;
	    }
	    اگر (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		بازگشت FAIL;
	}
    }
    وگرنه
    {
	انتخاب (vartype)
	{
	    حالت VAR_FUNC:
	    حالت VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		بشکن;
	    حالت VAR_BOOL:
	    حالت VAR_SPECIAL:
	    حالت VAR_JOB:
	    حالت VAR_CHANNEL:
	    حالت VAR_INSTR:
	    حالت VAR_CLASS:
	    حالت VAR_OBJECT:
	    حالت VAR_TYPEALIAS:
	    حالت VAR_UNKNOWN:
	    حالت VAR_ANY:
	    حالت VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		بشکن;
	    پیشفرض:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    ایستا صحیح
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    برای (cctx_T *cctx = cctx_arg; cctx != پوچ; cctx = cctx->ctx_outer)
    {
	اگر (cctx->ctx_ufunc != پوچ && cctx->ctx_ufunc->uf_class != پوچ)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    وقتی (clp != پوچ)
	    {
		اگر (clp == cl)
		    بازگشت TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    بازگشت FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    صحیح
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    صحیح		m_idx;
    صحیح		ret = FAIL;

    اگر (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	بازگشت FAIL;
    }

    class_T *cl = type->tt_class;
    صحیح is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    اگر (type == &t_super)
    {
	اگر (cctx->ctx_ufunc == پوچ || cctx->ctx_ufunc->uf_class == پوچ)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    بازگشت FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_پشته.ga_len;
    }
    وگرنه اگر (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	اگر (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    اگر (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    اگر (cl == پوچ)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	بازگشت FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, پوچ, پوچ, FNE_CHECK_START);
    اگر (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	بازگشت FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    اگر (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	اگر (name_end == پوچ)
	    برو done;
    }

    اگر (*name_end == '(')
    {
	صحیح	function_شمار;
	صحیح	child_شمار;
	ufunc_T	**functions;

	اگر (type->tt_type == VAR_CLASS)
	{
	    function_شمار = cl->class_class_function_شمار;
	    child_شمار = cl->class_class_function_شمار_child;
	    functions = cl->class_class_functions;
	}
	وگرنه
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    اگر (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_شمار = cl->class_obj_method_شمار;
	    child_شمار = cl->class_obj_method_شمار_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = پوچ;
	صحیح fi;
	برای (fi = is_super ? child_شمار : 0; fi < function_شمار; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    اگر (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		بشکن;
	    }
	}

	ocmember_T  *ocm = پوچ;
	اگر (ufunc == پوچ)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    اگر (ocm == پوچ || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_پیام(cl, type->tt_type, name, len);
		برو done;
	    }
	    اگر (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_پشته.ga_len;
		اگر (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    برو done;
	    }
	    وگرنه
	    {
		صحیح status;

		اگر (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		وگرنه
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		اگر (status == FAIL)
		    برو done;
	    }
	}

	اگر (is_super && ufunc != پوچ && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    برو done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	اگر (ocm == پوچ && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    برو done;
	}

	// process generic function call
	اگر (ufunc != پوچ)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    اگر (ufunc == پوچ)
		برو done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	صحیح argcount = 0;
	اگر (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    برو done;

	اگر (ocm != پوچ)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    برو done;
	}
	اگر (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	وگرنه
	    ret = generate_CALL(cctx, ufunc, پوچ, 0, argcount, FALSE);
	برو done;
    }

    اگر (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	اگر (m_idx >= 0)
	{
	    اگر (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		برو done;
	    }

	    *arg = name_end;
	    اگر (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    وگرنه
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    برو done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	اگر (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    اگر (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		برو done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_پشته.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, پوچ);
	    برو done;
	}

	member_not_found_پیام(cl, VAR_OBJECT, name, len);
    }
    وگرنه
    {
	// load class member
	صحیح idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	اگر (m != پوچ)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    اگر (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		برو done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_پشته.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    برو done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	اگر (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    اگر (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		برو done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_پشته.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, پوچ);
	    برو done;
	}

	member_not_found_پیام(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    بازگشت ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    صحیح
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    صحیح		    idx;
    imported_T	    *imp;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	بازگشت FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, پوچ);
    اگر (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	بازگشت OK;
    }

    اگر (end == پوچ)
	imp = پوچ;
    وگرنه اگر (import == پوچ)
	imp = find_imported(name, 0, FALSE);
    وگرنه
	imp = import;

    اگر (imp != پوچ)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	صحیح	cc;
	ufunc_T	*ufunc = پوچ;
	type_T	*type;
	صحیح	done = FALSE;
	صحیح	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	اگر (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    بازگشت FAIL;
	}
	++p;
	اگر (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    بازگشت FAIL;
	}

	// isolate one name
	exp_name = p;
	وقتی (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	اگر (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	اگر (res == OK)
	{
	    اگر (si->sn_autoload_prefix != پوچ
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		اگر (cc == '(' || paren_follows_after_عبارت)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		وگرنه
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    وگرنه اگر (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		اگر (cc == '(' || paren_follows_after_عبارت)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		وگرنه
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    وگرنه
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, پوچ, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	اگر (done)
	    بازگشت res;

	اگر (idx < 0)
	{
	    اگر (ufunc != پوچ)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		اگر (IS_GENERIC_FUNC(ufunc))
		{
		    اگر (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			اگر (p != پوچ)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			اگر (p == پوچ || ufunc == پوچ)
			    بازگشت FAIL;
		    }
		    وگرنه
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			بازگشت FAIL;
		    }
		}
		وگرنه اگر (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    بازگشت FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, پوچ, TRUE);
		بازگشت OK;
	    }
	    بازگشت FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	بازگشت OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    بازگشت generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    ایستا صحیح
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    صحیح			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    اگر (ufunc == پوچ || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	بازگشت FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    اگر (ufunc == پوچ)
	بازگشت FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    اگر (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, پوچ) == FAIL)
	بازگشت FAIL;
    بازگشت generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    ایستا صحیح
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    بازگشت cctx->ctx_ufunc != پوچ
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == پوچ
		|| cctx->ctx_outer->ctx_ufunc == پوچ
		|| cctx->ctx_outer->ctx_ufunc->uf_class == پوچ
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
    ایستا صحیح
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    صحیح			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    صحیح		isn_idx;

    اگر (fp == پوچ)
	بازگشت FAIL;

    برای (صحیح depth = 0; depth < outer_depth; ++depth)
    {
	اگر (outer_cctx->ctx_outer == پوچ)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    بازگشت FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    اگر (fp->uf_block_ids == پوچ && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(صحیح, ufunc->uf_block_depth);
	اگر (fp->uf_block_ids != پوچ)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    اندازه(صحیح) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    اگر (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	بازگشت FAIL;

    اگر (generate_FUNCREF(cctx, fp, پوچ, FALSE, 0, &isn_idx) == FAIL)
	بازگشت FAIL;
    اگر (cctx->ctx_skip == SKIP_YES)
	بازگشت OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    اگر (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	اگر (extra == پوچ)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    اگر (extra == پوچ)
		بازگشت FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    بازگشت OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    صحیح
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	صحیح	is_عبارت,
	صحیح	error)
{
    type_T	*type;
    char_u	*name = پوچ;
    char_u	*end = end_arg;
    صحیح		res = FAIL;
    صحیح		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = پوچ;

    generic_func_args_table_init(&gfatab);

    اگر (*(*arg + namelen) == '<')
    {
	// generic function call
	اگر (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == پوچ)
	{
	    generic_func_args_table_clear(&gfatab);
	    بازگشت FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    اگر (*(*arg + 1) == ':')
    {
	اگر (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    انتخاب (**arg)
	    {
		حالت 'g': isn_type = ISN_LOADGDICT; بشکن;
		حالت 'w': isn_type = ISN_LOADWDICT; بشکن;
		حالت 't': isn_type = ISN_LOADTDICT; بشکن;
		حالت 'b': isn_type = ISN_LOADBDICT; بشکن;
		پیشفرض:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    برو theend;
	    }
	    اگر (generate_instr_type(cctx, isn_type, &t_dict_any) == پوچ)
		برو theend;
	    res = OK;
	}
	وگرنه
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    اگر (name == پوچ)
		برو theend;

	    انتخاب (**arg)
	    {
		حالت 'v': res = generate_LOADV(cctx, name);
			  بشکن;
		حالت 's': اگر (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      برو theend;
			  }
			  اگر (is_عبارت && find_func(name, FALSE) != پوچ)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  وگرنه
			      res = compile_load_scriptvar(cctx, name,
							    پوچ, &end, پوچ);
			  بشکن;
		حالت 'g': اگر (vim_strchr(name, AUTOLOAD_CHAR) == پوچ)
			  {
			      اگر (is_عبارت && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != پوچ
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      وگرنه
				  isn_type = ISN_LOADG;
			  }
			  وگرنه
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      اگر (name == پوچ)
				  برو theend;
			  }
			  بشکن;
		حالت 'w': isn_type = ISN_LOADW; بشکن;
		حالت 't': isn_type = ISN_LOADT; بشکن;
		حالت 'b': isn_type = ISN_LOADB; بشکن;
		پیشفرض:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  برو theend;
	    }
	    اگر (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    وگرنه
    {
	صحیح	    idx;
	صحیح	    method_idx;
	صحیح	    gen_load = FALSE;
	صحیح	    gen_load_outer = 0;
	صحیح	    outer_loop_depth = -1;
	صحیح	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	اگر (name == پوچ)
	    برو theend;

	اگر (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = افزودن_type_پشته(cctx, &t_super);
	    اگر (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	وگرنه اگر (vim_strchr(name, AUTOLOAD_CHAR) != پوچ)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	وگرنه اگر (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    اگر (gen_load_outer == 0)
		gen_load = TRUE;
	}
	وگرنه
	{
	    lvar_T  lvar;
	    class_T *cl = پوچ;

	    اگر (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		اگر (lvar.lv_ufunc != پوچ && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		وگرنه اگر (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		وگرنه
		    gen_load = TRUE;
	    }
	    وگرنه اگر (cctx->ctx_ufunc->uf_defclass != پوچ &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		اگر (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    اگر (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    وگرنه
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									پوچ);
		    }
		}
		وگرنه
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    وگرنه
	    {
		imported_T *imp = پوچ;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		اگر (script_var_exists(*arg, namelen, cctx, پوچ) == OK
			    || (imp = find_imported(name, 0, FALSE)) != پوچ)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		اگر (res == FAIL && is_عبارت && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	اگر (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, پوچ, type);
	اگر (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    اگر (lt != پوچ)
	*lt = '<';
    اگر (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    بازگشت res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    ایستا صحیح
compile_string(isn_T *isn, cctx_T *cctx, صحیح str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    صحیح		عبارت_res;
    صحیح		trailing_error;
    صحیح		instr_شمار;
    isn_T	*instr = پوچ;

    // Remove the string type from the stack.
    --cctx->ctx_type_پشته.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = پوچ;

    // avoid peeking a next line
    صحیح galen_save = cctx->ctx_ufunc->uf_سطرها.ga_len;
    cctx->ctx_ufunc->uf_سطرها.ga_len = 0;

    عبارت_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_سطرها.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    اگر (عبارت_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	اگر (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_پشته.ga_len;
	بازگشت FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_شمار = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_شمار].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    بازگشت OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    صحیح
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	صحیح	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    صحیح	    must_end = FALSE;
    صحیح	    instr_شمار;

    برای (;;)
    {
	اگر (may_get_next_سطر(whitep, &p, cctx) == FAIL)
	    برو failret;
	اگر (*p == ')')
	{
	    *arg = p + 1;
	    بازگشت OK;
	}
	اگر (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    بازگشت FAIL;
	}

	instr_شمار = cctx->ctx_instr.ga_len;
	اگر (compile_expr0(&p, cctx) == FAIL)
	    بازگشت FAIL;
	++*argcount;

	اگر (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_شمار + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_شمار;

	    // {skip} argument of searchpair() can be compiled if not empty
	    اگر (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != پوچ
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	وگرنه اگر (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_شمار + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_شمار;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    اگر (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != پوچ
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	اگر (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    بازگشت FAIL;
	}
	اگر (*p == ',')
	{
	    ++p;
	    اگر (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		بازگشت FAIL;
	    }
	}
	وگرنه
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    بازگشت FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    ایستا صحیح
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_پشته(cctx, 0);
    صحیح		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    اگر (type->tt_type == VAR_OBJECT)
    {
	صحیح	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	اگر (uf != پوچ)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    بازگشت res;
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
    ایستا صحیح
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	صحیح	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    صحیح		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = پوچ;
    char_u	*gname = پوچ;
    ufunc_T	*ufunc = پوچ;
    صحیح		res = FAIL;
    صحیح		is_autoload;
    صحیح		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    اگر (varlen >= اندازه(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	بازگشت FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    اگر (import != پوچ)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	بازگشت FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    اگر ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	صحیح	    is_has = **arg == 'h';
	صحیح	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	اگر (*s == '"')
	    (تهی)eval_string(&s, &argvars[0], TRUE, FALSE);
	وگرنه اگر (*s == '\'')
	    (تهی)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	اگر (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    اگر (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_عدد;
		tv->vval.v_عدد = 0;
		اگر (is_has)
		    f_has(argvars, tv);
		وگرنه اگر (is_len)
		    f_len(argvars, tv);
		وگرنه
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    بازگشت OK;
	}
	clear_tv(&argvars[0]);
	اگر (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    بازگشت FAIL;
	}
    }

    اگر (generate_ppconst(cctx, ppconst) == FAIL)
	بازگشت FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    اگر ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    وگرنه اگر (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    وگرنه
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    اگر (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	اگر (*arg == پوچ)
	    برو theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	اگر (gname == پوچ)
	    برو theend;
	++*arg;			// skip '('
    }
    وگرنه
	*arg += varlen + 1;

    اگر (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	برو theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != پوچ;
    اگر (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	صحیح	    idx;

	// builtin function
	idx = find_internal_func(name);
	اگر (idx >= 0)
	{
	    اگر (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		برو theend;
	    }

	    اگر (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_پشته(cctx, 1);
		اگر (check_type_is_مقدار(get_type_on_پشته(cctx, 0)) == FAIL)
		    برو theend;

		// add() can be compiled to instructions if we know the type
		اگر (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		وگرنه اگر (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    اگر ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		اگر (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    اگر (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    وگرنه اگر (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    وگرنه اگر (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    اگر (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		اگر (res == OK)
		    idx = -1;
	    }

	    اگر (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	وگرنه
	    emsg_funcname(e_unknown_function_str, namebuf);
	برو theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    اگر (lookup_local(namebuf, varlen, پوچ, cctx) == FAIL
	    && arg_exists(namebuf, varlen, پوچ, پوچ, پوچ, cctx) != OK)
    {
	class_T		*cl = پوچ;
	صحیح		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	اگر (ufunc != پوچ)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    اگر (ufunc == پوچ)
		برو theend;

	    اگر (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, پوچ, 0, argcount, FALSE);
		برو theend;
	    }
	    اگر (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == پوچ
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		برو theend;
	    }
	}
	وگرنه اگر ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    اگر (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], پوچ,
							0, argcount, FALSE);
	    }
	    وگرنه
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    برو theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == پوچ && gname != پوچ ? gname : namebuf;
    اگر (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_پشته(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	برو theend;
    }

    // If we can find a global function by name generate the right call.
    اگر (ufunc != پوچ)
    {
	res = generate_CALL(cctx, ufunc, پوچ, 0, argcount, FALSE);
	برو theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    اگر (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    وگرنه
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    بازگشت res;
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
to_name_end(char_u *arg, صحیح use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    اگر (!eval_isnamec1(*arg))
	بازگشت arg;

    برای (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	اگر (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == پوچ))
	    بشکن;
    بازگشت p;
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

    اگر (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    اگر (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	اگر (eval_list(&p, &rettv, پوچ, FALSE) == FAIL)
	    p = arg;
    }
    بازگشت p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    ایستا صحیح
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    صحیح		شمار = 0;
    صحیح		is_const;
    صحیح		is_all_const = TRUE;	// reset when non-const encountered
    صحیح		must_end = FALSE;

    برای (;;)
    {
	اگر (may_get_next_سطر(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    بازگشت FAIL;
	}
	اگر (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    بازگشت FAIL;
	}
	اگر (*p == ']')
	{
	    ++p;
	    بشکن;
	}
	اگر (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    بازگشت FAIL;
	}
	اگر (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    بازگشت FAIL;
	اگر (!is_const)
	    is_all_const = FALSE;
	++شمار;
	اگر (*p == ',')
	{
	    ++p;
	    اگر (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		بازگشت FAIL;
	    }
	}
	وگرنه
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    بازگشت generate_NEWLIST(cctx, شمار, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    ایستا صحیح
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    صحیح first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    صحیح		شمار = 0;
    صحیح		is_const;
    صحیح		is_all_const = TRUE;	// reset when non-const encountered
    صحیح		must_end = FALSE;

    اگر (**arg != ')')
    {
	اگر (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    بازگشت FAIL;
	}
	شمار = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	برای (;;)
	{
	    اگر (may_get_next_سطر(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		بازگشت FAIL;
	    }
	    اگر (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		بازگشت FAIL;
	    }
	    اگر (*p == ')')
	    {
		++p;
		بشکن;
	    }
	    اگر (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		بازگشت FAIL;
	    }
	    اگر (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		بازگشت FAIL;
	    اگر (!is_const)
		is_all_const = FALSE;
	    ++شمار;
	    اگر (*p == ',')
	    {
		++p;
		اگر (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    بازگشت FAIL;
		}
	    }
	    وگرنه
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    بازگشت generate_NEWTUPLE(cctx, شمار, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    ایستا تهی
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*سطر;
    size_t	off;

    اگر (!evalarg->eval_using_cmdline || cctx == پوچ)
	بازگشت;

    gap = &evalarg->eval_tofree_ga;
    اگر (gap->ga_len == 0)
	بازگشت;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    سطر = ((char_u **)cctx->ctx_ufunc->uf_سطرها.ga_data)[cctx->ctx_lnum];
    *arg = سطر + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    صحیح
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحیح		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    اگر (r != OK)
    {
	clear_evalarg(&evalarg, پوچ);
	بازگشت r;
    }

    اگر (ppconst != پوچ && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, پوچ);
	بازگشت FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    اگر (cctx->ctx_ufunc != پوچ)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	صحیح block_depth = cctx->ctx_ufunc->uf_block_depth;

	اگر (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(صحیح, block_depth);
	    اگر (ufunc->uf_block_ids != پوچ)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    اندازه(صحیح) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    اگر (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    اگر (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    اگر (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	اگر (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, پوچ);

    اگر (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	بازگشت generate_FUNCREF(cctx, ufunc, پوچ, FALSE, 0, پوچ);
    }

    func_ptr_unref(ufunc);
    بازگشت FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    صحیح
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	صحیح	    types_optional,
	evalarg_T   *evalarg)
{
    صحیح		r;
    ufunc_T	*ufunc;
    صحیح		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, پوچ);
    current_sctx.sc_version = save_sc_version;
    اگر (r != OK)
	بازگشت r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    اگر (ufunc->uf_ret_type == پوچ || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, پوچ);

    اگر (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	بازگشت OK;
    }
    clear_tv(rettv);
    بازگشت FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    ایستا صحیح
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    صحیح		شمار = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    صحیح		is_const;
    صحیح		is_all_const = TRUE;	// reset when non-const encountered

    اگر (d == پوچ)
	بازگشت FAIL;
    اگر (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	بازگشت FAIL;
    }
    برای (;;)
    {
	char_u	    *کلید = پوچ;

	اگر (may_get_next_سطر(whitep, arg, cctx) == FAIL)
	{
	    *arg = پوچ;
	    برو failret;
	}

	اگر (**arg == '}')
	    بشکن;

	اگر (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    اگر (compile_expr0(arg, cctx) == FAIL)
		برو failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    اگر (isn->isn_type == ISN_PUSHNR)
	    {
		نویسه buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.عدد);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    اگر (isn->isn_type == ISN_PUSHS)
		کلید = isn->isn_arg.string;
	    وگرنه اگر (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		برو failret;
	    *arg = skipwhite(*arg);
	    اگر (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_کلید));
		برو failret;
	    }
	    ++*arg;
	}
	وگرنه
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    کلید = get_literal_کلید(arg);
	    اگر (کلید == پوچ)
		برو failret;
	    اگر (generate_PUSHS(cctx, &کلید) == FAIL)
		برو failret;
	}

	// Check for duplicate keys, if using string keys.
	اگر (کلید != پوچ)
	{
	    item = dict_find(d, کلید, -1);
	    اگر (item != پوچ)
	    {
		semsg(_(e_duplicate_کلید_in_dictionary_str), کلید);
		برو failret;
	    }
	    item = dictitem_alloc(کلید);
	    اگر (item != پوچ)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		اگر (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	اگر (**arg != ':')
	{
	    اگر (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    وگرنه
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    برو failret;
	}
	whitep = *arg + 1;
	اگر (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    برو failret;
	}

	اگر (may_get_next_سطر(whitep, arg, cctx) == FAIL)
	{
	    *arg = پوچ;
	    برو failret;
	}

	اگر (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    برو failret;
	اگر (!is_const)
	    is_all_const = FALSE;
	++شمار;

	whitep = *arg;
	اگر (may_get_next_سطر(whitep, arg, cctx) == FAIL)
	{
	    *arg = پوچ;
	    برو failret;
	}
	اگر (**arg == '}')
	    بشکن;
	اگر (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    برو failret;
	}
	اگر (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    برو failret;
	}
	whitep = *arg + 1;
	اگر (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    برو failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    اگر (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    بازگشت generate_NEWDICT(cctx, شمار, FALSE);

failret:
    اگر (*arg == پوچ)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    بازگشت FAIL;
}

/*
 * Compile "&option".
 */
    ایستا صحیح
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    صحیح		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    اگر (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_عدد ? &t_عدد : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    بازگشت ret;
}

/*
 * Compile "$VAR".
 */
    ایستا صحیح
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    صحیح		len;
    صحیح		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    اگر (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	بازگشت FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    بازگشت ret;
}

/*
 * Compile $"string" or $'string'.
 */
    ایستا صحیح
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    صحیح		ret;
    صحیح		quote;
    صحیح		evaluate = cctx->ctx_skip != SKIP_YES;
    صحیح		شمار = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    برای (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	اگر (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	وگرنه
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	اگر (ret == FAIL)
	    بشکن;
	اگر (evaluate)
	{
	    اگر ((tv.vval.v_string != پوچ && *tv.vval.v_string != NUL)
		    || (**arg != '{' && شمار == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		اگر (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    بازگشت FAIL;
		tv.vval.v_string = پوچ;  // don't free it now
		++شمار;
	    }
	    clear_tv(&tv);
	}

	اگر (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    بشکن;
	}

	p = compile_one_عبارت_in_str(*arg, cctx);
	اگر (p == پوچ)
	{
	    ret = FAIL;
	    بشکن;
	}
	++شمار;
	*arg = p;
    }

    اگر (ret == FAIL || !evaluate)
	بازگشت ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    اگر (شمار > 1)
	بازگشت generate_CONCAT(cctx, شمار);

    بازگشت OK;
}

/*
 * Compile "@r".
 */
    ایستا صحیح
compile_get_register(char_u **arg, cctx_T *cctx)
{
    صحیح		ret;

    ++*arg;
    اگر (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	بازگشت FAIL;
    }
    اگر (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	بازگشت FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, پوچ, &t_string);
    ++*arg;
    بازگشت ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ایستا صحیح
apply_leader(typval_T *rettv, صحیح numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    وقتی (p > start)
    {
	--p;
	اگر (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    اگر (rettv->v_type == VAR_FLOAT)
	    {
		اگر (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    وگرنه
	    {
		varnumber_T	val;
		صحیح		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		اگر (check_not_string(rettv) == FAIL)
		    بازگشت FAIL;
		val = tv_get_عدد_chk(rettv, &error);
		clear_tv(rettv);
		اگر (error)
		    بازگشت FAIL;
		اگر (*p == '-')
		    val = -val;
		rettv->v_type = VAR_عدد;
		rettv->vval.v_عدد = val;
	    }
	}
	وگرنه اگر (numeric_only)
	{
	    ++p;
	    بشکن;
	}
	وگرنه اگر (*p == '!')
	{
	    صحیح v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_عدد = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    بازگشت OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    ایستا تهی
get_vim_constant(char_u **arg, typval_T *rettv)
{
    اگر (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_عدد = VVAL_TRUE;
	*arg += 6;
    }
    وگرنه اگر (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_عدد = VVAL_FALSE;
	*arg += 7;
    }
    وگرنه اگر (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_عدد = VVAL_NULL;
	*arg += 6;
    }
    وگرنه اگر (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_عدد = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, صحیح *len, صحیح *type_is)
{
    exprtype_T	type = عبارت_UNKNOWN;
    صحیح		i;

    انتخاب (p[0])
    {
	حالت '=':   اگر (p[1] == '=')
			type = عبارت_EQUAL;
		    وگرنه اگر (p[1] == '~')
			type = عبارت_MATCH;
		    بشکن;
	حالت '!':   اگر (p[1] == '=')
			type = عبارت_NEQUAL;
		    وگرنه اگر (p[1] == '~')
			type = عبارت_NOMATCH;
		    بشکن;
	حالت '>':   اگر (p[1] != '=')
		    {
			type = عبارت_GREATER;
			*len = 1;
		    }
		    وگرنه
			type = عبارت_GEQUAL;
		    بشکن;
	حالت '<':   اگر (p[1] != '=')
		    {
			type = عبارت_SMALLER;
			*len = 1;
		    }
		    وگرنه
			type = عبارت_SEQUAL;
		    بشکن;
	حالت 'i':   اگر (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			اگر (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			اگر (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? عبارت_IS : عبارت_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    بشکن;
    }
    بازگشت type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    تهی
skip_عبارت_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_عبارت(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, پوچ);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    صحیح
bool_on_پشته(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_پشته(cctx, 0);
    اگر (type == &t_bool)
	بازگشت OK;

    اگر (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_عدد
	    || type == &t_عدد_bool
	    || type == &t_const_عدد_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	بازگشت generate_COND2BOOL(cctx);

    بازگشت need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    تهی
error_white_both(char_u *op, صحیح len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ایستا صحیح
compile_leader(cctx_T *cctx, صحیح numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    وقتی (p > start)
    {
	--p;
	وقتی (VIM_ISWHITE(*p))
	    --p;
	اگر (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_پشته(cctx, 0);
	    اگر (type->tt_type != VAR_FLOAT && need_type(type, &t_عدد,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		بازگشت FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    اگر (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == پوچ)
		بازگشت FAIL;
	}
	وگرنه اگر (numeric_only)
	{
	    ++p;
	    بشکن;
	}
	وگرنه
	{
	    صحیح  invert = *p == '!';

	    وقتی (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		اگر (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    اگر (check_type_is_مقدار(get_type_on_پشته(cctx, 0)) == FAIL)
		بازگشت FAIL;
	    اگر (generate_2BOOL(cctx, invert, -1) == FAIL)
		بازگشت FAIL;
	}
    }
    *end = p;
    بازگشت OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    ایستا صحیح
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحیح	    ret;
    char_u  *p = *arg + 1;

    اگر (may_get_next_سطر_error(p, arg, cctx) == FAIL)
	بازگشت FAIL;

    اگر (**arg == ')')
	// empty tuple
	بازگشت compile_tuple(arg, cctx, ppconst, FALSE);

    اگر (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    وگرنه
    {
	// Not enough space in ppconst, flush constants.
	اگر (generate_ppconst(cctx, ppconst) == FAIL)
	    بازگشت FAIL;
	ret = compile_expr0(arg, cctx);
    }
    اگر (may_get_next_سطر_error(*arg, arg, cctx) == FAIL)
	بازگشت FAIL;
    اگر (ret == OK && **arg == ',')
    {
	// tuple
	صحیح is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	اگر (generate_ppconst(cctx, ppconst) == FAIL)
	    بازگشت FAIL;
	بازگشت compile_tuple(arg, cctx, ppconst, is_const);
    }

    اگر (**arg == ')')
	++*arg;
    وگرنه اگر (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    بازگشت ret;
}

ایستا صحیح compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    ایستا صحیح
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    صحیح		keeping_dict = FALSE;

    برای (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	اگر (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_سطر_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    اگر (next != پوچ &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_سطر_from_context(cctx, TRUE);
		اگر (next == پوچ)
		    بازگشت FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	اگر (**arg == '(')
	{
	    صحیح		argcount = 0;

	    اگر (generate_ppconst(cctx, ppconst) == FAIL)
		بازگشت FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_پشته(cctx, 0);

	    *arg = skipwhite(p + 1);
	    اگر (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		بازگشت FAIL;
	    اگر (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		بازگشت FAIL;
	    اگر (keeping_dict)
	    {
		keeping_dict = FALSE;
		اگر (generate_instr(cctx, ISN_CLEARDICT) == پوچ)
		    بازگشت FAIL;
	    }
	}
	وگرنه اگر (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    صحیح	    alt;
	    char_u  *paren;

	    // something->method()
	    اگر (generate_ppconst(cctx, ppconst) == FAIL)
		بازگشت FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_پشته(cctx, 0);
	    اگر (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_مقدار));
		بازگشت FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    اگر (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		بازگشت FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    اگر (**arg == '(')
		alt = 2;
	    وگرنه
	    {
		// alternative 1 or 3
		p = *arg;
		اگر (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    بازگشت FAIL;
		}
		اگر (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		برای ( ; eval_isnamec(*p); ++p)
		    ;
		اگر (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    اگر (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			بازگشت FAIL;
		}
		وگرنه
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    اگر (paren == پوچ)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			بازگشت FAIL;
		    }
		}
	    }

	    اگر (alt != 1)
	    {
		صحیح	    argcount = 1;
		garray_T    *پشته = &cctx->ctx_type_پشته;
		صحیح	    type_idx_start = پشته->ga_len;
		صحیح	    عبارت_isn_start = cctx->ctx_instr.ga_len;
		صحیح	    عبارت_isn_end;
		صحیح	    arg_isn_شمار;

		اگر (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    اگر (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			بازگشت FAIL;
		}
		وگرنه
		{
		    صحیح fail;
		    صحیح save_len = cctx->ctx_ufunc->uf_سطرها.ga_len;
		    صحیح	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_عبارت;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_سطرها.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_عبارت;
		    cctx->ctx_ufunc->uf_سطرها.ga_len = save_len;

		    اگر (fail)
		    {
			اگر (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			بازگشت FAIL;
		    }
		}

		// Compile the arguments.
		اگر (**arg != '(')
		{
		    اگر (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    وگرنه
			semsg(_(e_missing_parenthesis_str), *arg);
		    بازگشت FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		عبارت_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		اگر (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    بازگشت FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_شمار = cctx->ctx_instr.ga_len - عبارت_isn_end;
		اگر (arg_isn_شمار > 0)
		{
		    صحیح	    عبارت_isn_شمار = عبارت_isn_end - عبارت_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, عبارت_isn_شمار);
		    type_T  *decl_type;
		    type2_T  *typep;

		    اگر (isn == پوچ)
			بازگشت FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + عبارت_isn_start,
					       اندازه(isn_T) * عبارت_isn_شمار);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + عبارت_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + عبارت_isn_end,
						اندازه(isn_T) * arg_isn_شمار);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + عبارت_isn_start + arg_isn_شمار,
					  isn, اندازه(isn_T) * عبارت_isn_شمار);
		    vim_free(isn);

		    typep = ((type2_T *)پشته->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)پشته->ga_data) + type_idx_start,
			      ((type2_T *)پشته->ga_data) + type_idx_start + 1,
			      اندازه(type2_T)
				       * (پشته->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)پشته->ga_data) + پشته->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_پشته(cctx, 0);
		اگر (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    بازگشت FAIL;
	    }

	    اگر (keeping_dict)
	    {
		keeping_dict = FALSE;
		اگر (generate_instr(cctx, ISN_CLEARDICT) == پوچ)
		    بازگشت FAIL;
	    }
	}
	وگرنه اگر (**arg == '[')
	{
	    صحیح		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    اگر (generate_ppconst(cctx, ppconst) == FAIL)
		بازگشت FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    اگر (may_get_next_سطر_error(p, arg, cctx) == FAIL)
		بازگشت FAIL;
	    اگر (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    وگرنه
	    {
		اگر (compile_expr0(arg, cctx) == FAIL)
		    بازگشت FAIL;
		اگر (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    بازگشت FAIL;
		}
		اگر (may_get_next_سطر_error(*arg, arg, cctx) == FAIL)
		    بازگشت FAIL;
		*arg = skipwhite(*arg);
	    }
	    اگر (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		اگر (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    بازگشت FAIL;
		}
		اگر (may_get_next_سطر_error(*arg, arg, cctx) == FAIL)
		    بازگشت FAIL;
		اگر (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		وگرنه
		{
		    اگر (compile_expr0(arg, cctx) == FAIL)
			بازگشت FAIL;
		    اگر (may_get_next_سطر_error(*arg, arg, cctx) == FAIL)
			بازگشت FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    اگر (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		بازگشت FAIL;
	    }
	    *arg = *arg + 1;

	    اگر (keeping_dict)
	    {
		keeping_dict = FALSE;
		اگر (generate_instr(cctx, ISN_CLEARDICT) == پوچ)
		    بازگشت FAIL;
	    }
	    اگر (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		بازگشت FAIL;
	}
	وگرنه اگر (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    اگر (generate_ppconst(cctx, ppconst) == FAIL)
		بازگشت FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_پشته(cctx, 0);
	    اگر (type != &t_unknown
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
		اگر (compile_class_object_index(cctx, arg, type) == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه
	    {
		*arg = p + 1;
		اگر (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    بازگشت FAIL;
		}
		p = *arg;
		وقتی (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		اگر (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    بازگشت FAIL;
		}
		اگر (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == پوچ)
		    بازگشت FAIL;
		اگر (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    بازگشت FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	وگرنه
	    بشکن;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    اگر (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == پوچ)
	بازگشت FAIL;

    بازگشت OK;
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
    ایستا صحیح
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    صحیح		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    صحیح		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    اگر (eval_leader(arg, TRUE) == FAIL)
	بازگشت FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    انتخاب (**arg)
    {
	/*
	 * Number constant.
	 */
	حالت '0':	// also for blob starting with 0z
	حالت '1':
	حالت '2':
	حالت '3':
	حالت '4':
	حالت '5':
	حالت '6':
	حالت '7':
	حالت '8':
	حالت '9':
	حالت '.':   اگر (eval_عدد(arg, rettv, TRUE, FALSE) == FAIL)
			بازگشت FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    اگر (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			بازگشت FAIL;
		    }
		    بشکن;

	/*
	 * String constant: "string".
	 */
	حالت '"':   اگر (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			بازگشت FAIL;
		    بشکن;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	حالت '\'':  اگر (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			بازگشت FAIL;
		    بشکن;

	/*
	 * Constant Vim variable.
	 */
	حالت 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    بشکن;

	/*
	 * "true" constant
	 */
	حالت 't':   اگر (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_عدد = VVAL_TRUE;
		    }
		    وگرنه
			ret = NOTDONE;
		    بشکن;

	/*
	 * "false" constant
	 */
	حالت 'f':   اگر (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_عدد = VVAL_FALSE;
		    }
		    وگرنه
			ret = NOTDONE;
		    بشکن;

	/*
	 * "null" or "null_*" constant
	 */
	حالت 'n':   اگر (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			صحیح	len;

			برای (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			اگر (ret == FAIL)
			    ret = NOTDONE;
			وگرنه
			    *arg += len + 4;
		    }
		    وگرنه
			ret = NOTDONE;
		    بشکن;

	/*
	 * List: [expr, expr]
	 */
	حالت '[':   اگر (generate_ppconst(cctx, ppconst) == FAIL)
			بازگشت FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    بشکن;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	حالت '{':   اگر (generate_ppconst(cctx, ppconst) == FAIL)
			بازگشت FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    بشکن;

	/*
	 * Option value: &name
	 */
	حالت '&':	اگر (generate_ppconst(cctx, ppconst) == FAIL)
			    بازگشت FAIL;
			ret = compile_get_option(arg, cctx);
			بشکن;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	حالت '$':	اگر (generate_ppconst(cctx, ppconst) == FAIL)
			    بازگشت FAIL;
			اگر ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			وگرنه
			    ret = compile_get_env(arg, cctx);
			بشکن;

	/*
	 * Register contents: @r.
	 */
	حالت '@':	اگر (generate_ppconst(cctx, ppconst) == FAIL)
			    بازگشت FAIL;
			ret = compile_get_register(arg, cctx);
			بشکن;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	حالت '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    اگر (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    بشکن;

	پیشفرض:    ret = NOTDONE;
		    بشکن;
    }
    اگر (ret == FAIL)
	بازگشت FAIL;

    اگر (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	اگر (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	وگرنه
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    وگرنه اگر (ret == NOTDONE)
    {
	char_u	    *p;
	صحیح	    r;

	اگر (!eval_isnamec1(**arg))
	{
	    اگر (!vim9_bad_comment(*arg))
	    {
		اگر (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		وگرنه
		    semsg(_(e_name_expected_str), *arg);
	    }
	    بازگشت FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	اگر (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    بازگشت FAIL;
	}

	size_t namelen = p - *arg;
	اگر (*p == '<')
	{
	    اگر (skip_generic_func_type_args(&p) == FAIL)
		بازگشت FAIL;
	}

	اگر (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	وگرنه
	{
	    اگر (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		بازگشت FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	اگر (r == FAIL)
	    بازگشت FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    اگر (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	بازگشت FAIL;
    اگر ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	اگر (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    بازگشت FAIL;
	بازگشت OK;
    }
    اگر (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    ایستا صحیح
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = پوچ;

    // Recognize <type>
    اگر (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	اگر (want_type == پوچ)
	    بازگشت FAIL;

	اگر (**arg != '>')
	{
	    اگر (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    وگرنه
		emsg(_(e_missing_gt));
	    بازگشت FAIL;
	}
	++*arg;
	اگر (may_get_next_سطر_error(*arg, arg, cctx) == FAIL)
	    بازگشت FAIL;
    }

    اگر (compile_expr9(arg, cctx, ppconst) == FAIL)
	بازگشت FAIL;

    اگر (want_type != پوچ)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_پشته(cctx, 0);
	اگر (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    اگر (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		بازگشت FAIL;
	}
    }

    بازگشت OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    ایستا صحیح
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    صحیح		ppconst_used = ppconst->pp_used;

    // get the first expression
    اگر (compile_expr8(arg, cctx, ppconst) == FAIL)
	بازگشت FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    برای (;;)
    {
	op = may_peek_next_سطر(cctx, *arg, &next);
	اگر (*op != '*' && *op != '/' && *op != '%')
	    بشکن;
	اگر (next != پوچ)
	{
	    *arg = next_سطر_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    بازگشت FAIL;
	}
	اگر (may_get_next_سطر_error(op + 1, arg, cctx) == FAIL)
	    بازگشت FAIL;

	// get the second expression
	اگر (compile_expr8(arg, cctx, ppconst) == FAIL)
	    بازگشت FAIL;

	اگر (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_عدد
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_عدد)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    صحیح		    failed = FALSE;

	    // both are numbers: compute the result
	    انتخاب (*op)
	    {
		حالت '*': res = tv1->vval.v_عدد * tv2->vval.v_عدد;
			  بشکن;
		حالت '/': res = رقم_divide(tv1->vval.v_عدد,
						  tv2->vval.v_عدد, &failed);
			  بشکن;
		حالت '%': res = رقم_modulus(tv1->vval.v_عدد,
						  tv2->vval.v_عدد, &failed);
			  بشکن;
	    }
	    اگر (failed)
		بازگشت FAIL;
	    tv1->vval.v_عدد = res;
	    --ppconst->pp_used;
	}
	وگرنه
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    بازگشت OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    ایستا صحیح
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    صحیح		oplen;
    صحیح		ppconst_used = ppconst->pp_used;

    // get the first variable
    اگر (compile_expr7(arg, cctx, ppconst) == FAIL)
	بازگشت FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    برای (;;)
    {
	op = may_peek_next_سطر(cctx, *arg, &next);
	اگر (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    بشکن;
	اگر (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    بشکن;
	oplen = (*op == '.' ? 2 : 1);
	اگر (next != پوچ)
	{
	    *arg = next_سطر_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    بازگشت FAIL;
	}

	اگر (may_get_next_سطر_error(op + oplen, arg, cctx) == FAIL)
	    بازگشت FAIL;

	// get the second expression
	اگر (compile_expr7(arg, cctx, ppconst) == FAIL)
	    بازگشت FAIL;

	اگر (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_عدد
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_عدد)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    اگر (*op == '+')
		tv1->vval.v_عدد = tv1->vval.v_عدد + tv2->vval.v_عدد;
	    وگرنه اگر (*op == '-')
		tv1->vval.v_عدد = tv1->vval.v_عدد - tv2->vval.v_عدد;
	    وگرنه
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		اگر (tv1->vval.v_string == پوچ)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    بازگشت FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	وگرنه
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    اگر (*op == '.')
	    {
		اگر (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    بازگشت FAIL;
		اگر (generate_CONCAT(cctx, 2) == FAIL)
		    بازگشت FAIL;
	    }
	    وگرنه
		generate_two_op(cctx, op);
	}
    }

    بازگشت OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    ایستا صحیح
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = عبارت_UNKNOWN;
    char_u	*p;
    char_u	*next;
    صحیح		len = 2;
    صحیح		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    اگر (compile_expr6(arg, cctx, ppconst) == FAIL)
	بازگشت FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    برای (;;)
    {
	type = عبارت_UNKNOWN;

	p = may_peek_next_سطر(cctx, *arg, &next);
	اگر (p[0] == '<' && p[1] == '<')
	    type = عبارت_LSHIFT;
	وگرنه اگر (p[0] == '>' && p[1] == '>')
	    type = عبارت_RSHIFT;

	اگر (type == عبارت_UNKNOWN)
	    بازگشت OK;

	// Handle a bitwise left or right shift operator
	اگر (ppconst->pp_used == ppconst_used + 1)
	{
	    اگر (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_عدد)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_عدد));
		بازگشت FAIL;
	    }
	}
	وگرنه
	{
	    type_T	*t = get_type_on_پشته(cctx, 0);

	    اگر (need_type(t, &t_عدد, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_عدد));
		بازگشت FAIL;
	    }
	}

	اگر (next != پوچ)
	{
	    *arg = next_سطر_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    بازگشت FAIL;
	}

	// get the second variable
	اگر (may_get_next_سطر_error(p + len, arg, cctx) == FAIL)
	    بازگشت FAIL;

	اگر (compile_expr6(arg, cctx, ppconst) == FAIL)
	    بازگشت FAIL;

	اگر (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    اگر (tv2->v_type != VAR_عدد || tv2->vval.v_عدد < 0)
	    {
		// right operand should be a positive number
		اگر (tv2->v_type != VAR_عدد)
		    emsg(_(e_bitshift_ops_must_be_عدد));
		وگرنه
		    emsg(_(e_bitshift_ops_must_be_positive));
		بازگشت FAIL;
	    }

	    اگر (tv2->vval.v_عدد > MAX_LSHIFT_BITS)
		tv1->vval.v_عدد = 0;
	    وگرنه اگر (type == عبارت_LSHIFT)
		tv1->vval.v_عدد =
			(uvarnumber_T)tv1->vval.v_عدد << tv2->vval.v_عدد;
	    وگرنه
		tv1->vval.v_عدد =
			(uvarnumber_T)tv1->vval.v_عدد >> tv2->vval.v_عدد;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	وگرنه
	{
	    اگر (need_type(get_type_on_پشته(cctx, 0), &t_عدد, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_عدد));
		بازگشت FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    اگر (isn == پوچ)
		بازگشت FAIL;

	    اگر (isn != پوچ)
		isn->isn_arg.op.op_type = type;
	}
    }

    بازگشت OK;
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
    ایستا صحیح
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = عبارت_UNKNOWN;
    char_u	*p;
    char_u	*next;
    صحیح		len = 2;
    صحیح		type_is = FALSE;
    صحیح		ppconst_used = ppconst->pp_used;

    // get the first variable
    اگر (compile_expr5(arg, cctx, ppconst) == FAIL)
	بازگشت FAIL;

    p = may_peek_next_سطر(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    اگر (type != عبارت_UNKNOWN)
    {
	صحیح ic = FALSE;  // Default: do not ignore case

	اگر (next != پوچ)
	{
	    *arg = next_سطر_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	اگر (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    بازگشت FAIL;
	}
	// extra question mark appended: ignore case
	اگر (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	وگرنه اگر (p[len] == '#')
	    ++len;
	// nothing appended: match case

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    بازگشت FAIL;
	}

	// get the second variable
	اگر (may_get_next_سطر_error(p + len, arg, cctx) == FAIL)
	    بازگشت FAIL;

	اگر (compile_expr5(arg, cctx, ppconst) == FAIL)
	    بازگشت FAIL;

	اگر (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    صحیح		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    اگر (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    وگرنه
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_عدد = tv1->vval.v_عدد
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    بازگشت ret;
	}

	generate_ppconst(cctx, ppconst);
	بازگشت generate_COMPARE(cctx, type, ic);
    }

    بازگشت OK;
}

ایستا صحیح compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    ایستا صحیح
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	نویسه	*op,
	ppconst_T *ppconst,
	صحیح	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_سطر(cctx, *arg, &next);
    صحیح		opchar = *op;

    اگر (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	صحیح		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, اندازه(صحیح), 10);
	وقتی (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    صحیح		start_ctx_lnum = cctx->ctx_lnum;
	    صحیح		save_lnum;
	    صحیح		const_used;
	    صحیح		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    اگر (next != پوچ)
	    {
		*arg = next_سطر_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		بازگشت FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    اگر (status != FAIL)
	    {
		// Use the last ppconst if possible.
		اگر (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    صحیح		is_true = tv2bool(tv);

		    اگر ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_عدد = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    وگرنه
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		وگرنه
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_پشته(cctx);
		}
	    }
	    اگر (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    اگر (status == FAIL)
	    {
		ga_clear(&end_ga);
		بازگشت FAIL;
	    }

	    اگر (jump_when != JUMP_NEVER)
	    {
		اگر (cctx->ctx_skip != SKIP_YES)
		{
		    *(((صحیح *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    اگر (may_get_next_سطر_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		بازگشت FAIL;
	    }

	    const_used = ppconst->pp_used;
	    اگر ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		بازگشت FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    اگر (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		اگر (tv->v_type == VAR_عدد
			 && (tv->vval.v_عدد == 1 || tv->vval.v_عدد == 0))
		{
		    tv->vval.v_عدد = tv->vval.v_عدد == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_سطر(cctx, *arg, &next);
	}

	اگر (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    بازگشت FAIL;
	}

	اگر (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    اگر (bool_on_پشته(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		بازگشت FAIL;
	    }

	اگر (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    وقتی (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((صحیح *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    بازگشت OK;
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
    ایستا صحیح
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحیح		ppconst_used = ppconst->pp_used;

    // get the first variable
    اگر (compile_expr4(arg, cctx, ppconst) == FAIL)
	بازگشت FAIL;

    // || and && work almost the same
    بازگشت compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    ایستا صحیح
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحیح		ppconst_used = ppconst->pp_used;

    // eval the first expression
    اگر (compile_expr3(arg, cctx, ppconst) == FAIL)
	بازگشت FAIL;

    // || and && work almost the same
    بازگشت compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    صحیح
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    صحیح		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    اگر (cctx->ctx_skip == SKIP_YES)
    {
	صحیح		prev_did_emsg = did_emsg;

	skip_عبارت_cctx(arg, cctx);
	بازگشت did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    اگر (compile_expr2(arg, cctx, ppconst) == FAIL)
	بازگشت FAIL;

    p = may_peek_next_سطر(cctx, *arg, &next);
    اگر (*p == '?')
    {
	صحیح		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*پشته = &cctx->ctx_type_پشته;
	صحیح		alt_idx = instr->ga_len;
	صحیح		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = پوچ;
	صحیح		has_const_عبارت = FALSE;
	صحیح		const_مقدار = FALSE;
	صحیح		save_skip = cctx->ctx_skip;

	اگر (next != پوچ)
	{
	    *arg = next_سطر_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    بازگشت FAIL;
	}

	اگر (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_عبارت = TRUE;
	    اگر (op_falsy)
		const_مقدار = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    وگرنه
	    {
		صحیح error = FALSE;

		const_مقدار = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		اگر (error)
		    بازگشت FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_مقدار : !const_مقدار) ? SKIP_YES : SKIP_NOT;

	    اگر (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    وگرنه
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	وگرنه
	{
	    generate_ppconst(cctx, ppconst);
	    اگر (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    اگر (op_falsy)
	    {
		type1 = get_type_on_پشته(cctx, -1);
		اگر (check_type_is_مقدار(type1) == FAIL)
		    بازگشت FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	اگر (may_get_next_سطر_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    بازگشت FAIL;
	اگر (compile_expr1(arg, cctx, ppconst) == FAIL)
	    بازگشت FAIL;

	اگر (!has_const_عبارت)
	{
	    generate_ppconst(cctx, ppconst);

	    اگر (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_پشته(cctx, 0);
		--پشته->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	اگر (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_سطر(cctx, *arg, &next);
	    اگر (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		بازگشت FAIL;
	    }
	    اگر (next != پوچ)
	    {
		*arg = next_سطر_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		بازگشت FAIL;
	    }

	    // evaluate the third expression
	    اگر (has_const_عبارت)
		cctx->ctx_skip = save_skip == SKIP_YES || const_مقدار
							 ? SKIP_YES : SKIP_NOT;
	    اگر (may_get_next_سطر_error(p + 1, arg, cctx) == FAIL)
		بازگشت FAIL;
	    اگر (compile_expr1(arg, cctx, ppconst) == FAIL)
		بازگشت FAIL;
	}

	اگر (!has_const_عبارت)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)پشته->ga_data)
					      + پشته->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    بازگشت OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    صحیح
compile_expr0_ext(char_u **arg,  cctx_T *cctx, صحیح *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    اگر (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	بازگشت FAIL;
    }
    اگر (is_const != پوچ)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    اگر (generate_ppconst(cctx, &ppconst) == FAIL)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Toplevel expression.
 */
    صحیح
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    بازگشت compile_expr0_ext(arg, cctx, پوچ);
}


#endif // defined(FEAT_EVAL)
