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
#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
સ્થિર પૂર્ણાંક paren_follows_after_પદાવલિ = 0;

/*
 * Generate code for any ppconst entries.
 */
    પૂર્ણાંક
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    પૂર્ણાંક	    i;
    પૂર્ણાંક	    ret = OK;
    પૂર્ણાંક	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    માટે (i = 0; i < ppconst->pp_used; ++i)
	જો (generate_tv_ધકેલો(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    પાછા ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    સ્થિર પૂર્ણાંક
check_ppconst_bool(ppconst_T *ppconst)
{
    જો (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	પાછા check_typval_type(&t_bool, tv, where);
    }
    પાછા OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    શૂન્ય
clear_ppconst(ppconst_T *ppconst)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    સ્થિર પૂર્ણાંક
compile_tuple_member(
    type2_T	*typep,
    પૂર્ણાંક		is_slice,
    cctx_T	*cctx)
{
    જો (is_slice)
    {
	જો (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    પાછા FAIL;
	// a copy is made so the member type is no longer declared
	જો (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	જો (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    જો (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    નહીંતર
    {
	જો (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    જો (typep->type_curr->tt_argcount == 1)
	    {
		જો (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		નહીંતર
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    નહીંતર
		typep->type_curr = &t_any;
	    જો (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		જો (typep->type_decl->tt_argcount == 1)
		{
		    જો (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    નહીંતર
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		નહીંતર
		    typep->type_curr = &t_any;
	    }
	    નહીંતર
		typep->type_decl = typep->type_curr;
	}
	જો (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    પાછા FAIL;
    }

    પાછા OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    પૂર્ણાંક
compile_member(પૂર્ણાંક is_slice, પૂર્ણાંક *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*ઢગલો = &cctx->ctx_type_ઢગલો;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)ઢગલો->ga_data) + ઢગલો->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)ઢગલો->ga_data) + ઢગલો->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    જો ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    જો (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	જો (need_type(idxtype, &t_સંખ્યા, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    પાછા FAIL;
	જો (is_slice)
	{
	    idxtype = get_type_on_ઢગલો(cctx, 1);
	    જો (need_type(idxtype, &t_સંખ્યા, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		પાછા FAIL;
	}
    }

    જો (vartype == VAR_DICT)
    {
	જો (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    પાછા FAIL;
	}
	જો (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    જો (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    જો (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		જો (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    નહીંતર
		typep->type_decl = typep->type_curr;
	}
	નહીંતર
	{
	    જો (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		પાછા FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	જો (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    પાછા FAIL;
	જો (keeping_dict != ખાલી)
	    *keeping_dict = TRUE;
    }
    નહીંતર જો (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	જો ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (vartype == VAR_BLOB)
    {
	જો (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    જો (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		પાછા FAIL;
	}
	નહીંતર
	{
	    typep->type_curr = &t_સંખ્યા;
	    typep->type_decl = &t_સંખ્યા;
	    જો (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		પાછા FAIL;
	}
    }
    નહીંતર જો (vartype == VAR_TUPLE)
    {
	જો (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    પાછા FAIL;
    }
    નહીંતર જો (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	જો (is_slice)
	{
	    જો (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		પાછા FAIL;
	    // a copy is made so the member type is no longer declared
	    જો (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    જો (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		જો (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	નહીંતર
	{
	    જો (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		જો (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		જો (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    જો (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		નહીંતર
			typep->type_decl = typep->type_curr;
	    }
	    જો (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		પાછા FAIL;
	}
    }
    નહીંતર
    {
	પસંદગી (vartype)
	{
	    સ્થિતિ VAR_FUNC:
	    સ્થિતિ VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		તોડો;
	    સ્થિતિ VAR_BOOL:
	    સ્થિતિ VAR_SPECIAL:
	    સ્થિતિ VAR_JOB:
	    સ્થિતિ VAR_CHANNEL:
	    સ્થિતિ VAR_INSTR:
	    સ્થિતિ VAR_CLASS:
	    સ્થિતિ VAR_OBJECT:
	    સ્થિતિ VAR_TYPEALIAS:
	    સ્થિતિ VAR_UNKNOWN:
	    સ્થિતિ VAR_ANY:
	    સ્થિતિ VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		તોડો;
	    મૂળભૂત:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    સ્થિર પૂર્ણાંક
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    માટે (cctx_T *cctx = cctx_arg; cctx != ખાલી; cctx = cctx->ctx_outer)
    {
	જો (cctx->ctx_ufunc != ખાલી && cctx->ctx_ufunc->uf_class != ખાલી)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    જ્યારે (clp != ખાલી)
	    {
		જો (clp == cl)
		    પાછા TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    પાછા FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    પૂર્ણાંક
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    પૂર્ણાંક		m_idx;
    પૂર્ણાંક		ret = FAIL;

    જો (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	પાછા FAIL;
    }

    class_T *cl = type->tt_class;
    પૂર્ણાંક is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    જો (type == &t_super)
    {
	જો (cctx->ctx_ufunc == ખાલી || cctx->ctx_ufunc->uf_class == ખાલી)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    પાછા FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_ઢગલો.ga_len;
    }
    નહીંતર જો (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	જો (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    જો (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    જો (cl == ખાલી)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	પાછા FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ખાલી, ખાલી, FNE_CHECK_START);
    જો (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	પાછા FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    જો (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	જો (name_end == ખાલી)
	    જાઓ done;
    }

    જો (*name_end == '(')
    {
	પૂર્ણાંક	function_ગણતરી;
	પૂર્ણાંક	child_ગણતરી;
	ufunc_T	**functions;

	જો (type->tt_type == VAR_CLASS)
	{
	    function_ગણતરી = cl->class_class_function_ગણતરી;
	    child_ગણતરી = cl->class_class_function_ગણતરી_child;
	    functions = cl->class_class_functions;
	}
	નહીંતર
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    જો (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_ગણતરી = cl->class_obj_method_ગણતરી;
	    child_ગણતરી = cl->class_obj_method_ગણતરી_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = ખાલી;
	પૂર્ણાંક fi;
	માટે (fi = is_super ? child_ગણતરી : 0; fi < function_ગણતરી; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    જો (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		તોડો;
	    }
	}

	ocmember_T  *ocm = ખાલી;
	જો (ufunc == ખાલી)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    જો (ocm == ખાલી || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_સંદેશ(cl, type->tt_type, name, len);
		જાઓ done;
	    }
	    જો (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_ઢગલો.ga_len;
		જો (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    જાઓ done;
	    }
	    નહીંતર
	    {
		પૂર્ણાંક status;

		જો (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		નહીંતર
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		જો (status == FAIL)
		    જાઓ done;
	    }
	}

	જો (is_super && ufunc != ખાલી && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    જાઓ done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	જો (ocm == ખાલી && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    જાઓ done;
	}

	// process generic function call
	જો (ufunc != ખાલી)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    જો (ufunc == ખાલી)
		જાઓ done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	પૂર્ણાંક argcount = 0;
	જો (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    જાઓ done;

	જો (ocm != ખાલી)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    જાઓ done;
	}
	જો (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	નહીંતર
	    ret = generate_CALL(cctx, ufunc, ખાલી, 0, argcount, FALSE);
	જાઓ done;
    }

    જો (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	જો (m_idx >= 0)
	{
	    જો (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		જાઓ done;
	    }

	    *arg = name_end;
	    જો (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    નહીંતર
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    જાઓ done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	જો (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    જો (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		જાઓ done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_ઢગલો.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, ખાલી);
	    જાઓ done;
	}

	member_not_found_સંદેશ(cl, VAR_OBJECT, name, len);
    }
    નહીંતર
    {
	// load class member
	પૂર્ણાંક idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	જો (m != ખાલી)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    જો (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		જાઓ done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ઢગલો.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    જાઓ done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	જો (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    જો (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		જાઓ done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ઢગલો.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, ખાલી);
	    જાઓ done;
	}

	member_not_found_સંદેશ(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    પાછા ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    પૂર્ણાંક
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    પૂર્ણાંક		    idx;
    imported_T	    *imp;

    જો (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	પાછા FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, ખાલી);
    જો (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	પાછા OK;
    }

    જો (end == ખાલી)
	imp = ખાલી;
    નહીંતર જો (import == ખાલી)
	imp = find_imported(name, 0, FALSE);
    નહીંતર
	imp = import;

    જો (imp != ખાલી)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	પૂર્ણાંક	cc;
	ufunc_T	*ufunc = ખાલી;
	type_T	*type;
	પૂર્ણાંક	done = FALSE;
	પૂર્ણાંક	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	જો (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    પાછા FAIL;
	}
	++p;
	જો (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    પાછા FAIL;
	}

	// isolate one name
	exp_name = p;
	જ્યારે (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	જો (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	જો (res == OK)
	{
	    જો (si->sn_autoload_prefix != ખાલી
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		જો (cc == '(' || paren_follows_after_પદાવલિ)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		નહીંતર
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    નહીંતર જો (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		જો (cc == '(' || paren_follows_after_પદાવલિ)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		નહીંતર
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    નહીંતર
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, ખાલી, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	જો (done)
	    પાછા res;

	જો (idx < 0)
	{
	    જો (ufunc != ખાલી)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		જો (IS_GENERIC_FUNC(ufunc))
		{
		    જો (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			જો (p != ખાલી)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			જો (p == ખાલી || ufunc == ખાલી)
			    પાછા FAIL;
		    }
		    નહીંતર
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			પાછા FAIL;
		    }
		}
		નહીંતર જો (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    પાછા FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, ખાલી, TRUE);
		પાછા OK;
	    }
	    પાછા FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	પાછા OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    પાછા generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    સ્થિર પૂર્ણાંક
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    પૂર્ણાંક			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    જો (ufunc == ખાલી || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	પાછા FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    જો (ufunc == ખાલી)
	પાછા FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    જો (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, ખાલી) == FAIL)
	પાછા FAIL;
    પાછા generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    સ્થિર પૂર્ણાંક
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    પાછા cctx->ctx_ufunc != ખાલી
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == ખાલી
		|| cctx->ctx_outer->ctx_ufunc == ખાલી
		|| cctx->ctx_outer->ctx_ufunc->uf_class == ખાલી
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
    સ્થિર પૂર્ણાંક
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    પૂર્ણાંક			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    પૂર્ણાંક		isn_idx;

    જો (fp == ખાલી)
	પાછા FAIL;

    માટે (પૂર્ણાંક depth = 0; depth < outer_depth; ++depth)
    {
	જો (outer_cctx->ctx_outer == ખાલી)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    પાછા FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    જો (fp->uf_block_ids == ખાલી && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(પૂર્ણાંક, ufunc->uf_block_depth);
	જો (fp->uf_block_ids != ખાલી)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    કદ(પૂર્ણાંક) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    જો (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	પાછા FAIL;

    જો (generate_FUNCREF(cctx, fp, ખાલી, FALSE, 0, &isn_idx) == FAIL)
	પાછા FAIL;
    જો (cctx->ctx_skip == SKIP_YES)
	પાછા OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    જો (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	જો (extra == ખાલી)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    જો (extra == ખાલી)
		પાછા FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    પાછા OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    પૂર્ણાંક
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	પૂર્ણાંક	is_પદાવલિ,
	પૂર્ણાંક	error)
{
    type_T	*type;
    char_u	*name = ખાલી;
    char_u	*end = end_arg;
    પૂર્ણાંક		res = FAIL;
    પૂર્ણાંક		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = ખાલી;

    generic_func_args_table_init(&gfatab);

    જો (*(*arg + namelen) == '<')
    {
	// generic function call
	જો (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == ખાલી)
	{
	    generic_func_args_table_clear(&gfatab);
	    પાછા FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    જો (*(*arg + 1) == ':')
    {
	જો (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    પસંદગી (**arg)
	    {
		સ્થિતિ 'g': isn_type = ISN_LOADGDICT; તોડો;
		સ્થિતિ 'w': isn_type = ISN_LOADWDICT; તોડો;
		સ્થિતિ 't': isn_type = ISN_LOADTDICT; તોડો;
		સ્થિતિ 'b': isn_type = ISN_LOADBDICT; તોડો;
		મૂળભૂત:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    જાઓ theend;
	    }
	    જો (generate_instr_type(cctx, isn_type, &t_dict_any) == ખાલી)
		જાઓ theend;
	    res = OK;
	}
	નહીંતર
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    જો (name == ખાલી)
		જાઓ theend;

	    પસંદગી (**arg)
	    {
		સ્થિતિ 'v': res = generate_LOADV(cctx, name);
			  તોડો;
		સ્થિતિ 's': જો (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      જાઓ theend;
			  }
			  જો (is_પદાવલિ && find_func(name, FALSE) != ખાલી)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  નહીંતર
			      res = compile_load_scriptvar(cctx, name,
							    ખાલી, &end, ખાલી);
			  તોડો;
		સ્થિતિ 'g': જો (vim_strchr(name, AUTOLOAD_CHAR) == ખાલી)
			  {
			      જો (is_પદાવલિ && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != ખાલી
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      નહીંતર
				  isn_type = ISN_LOADG;
			  }
			  નહીંતર
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      જો (name == ખાલી)
				  જાઓ theend;
			  }
			  તોડો;
		સ્થિતિ 'w': isn_type = ISN_LOADW; તોડો;
		સ્થિતિ 't': isn_type = ISN_LOADT; તોડો;
		સ્થિતિ 'b': isn_type = ISN_LOADB; તોડો;
		મૂળભૂત:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  જાઓ theend;
	    }
	    જો (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    નહીંતર
    {
	પૂર્ણાંક	    idx;
	પૂર્ણાંક	    method_idx;
	પૂર્ણાંક	    gen_load = FALSE;
	પૂર્ણાંક	    gen_load_outer = 0;
	પૂર્ણાંક	    outer_loop_depth = -1;
	પૂર્ણાંક	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	જો (name == ખાલી)
	    જાઓ theend;

	જો (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = ધકેલો_type_ઢગલો(cctx, &t_super);
	    જો (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	નહીંતર જો (vim_strchr(name, AUTOLOAD_CHAR) != ખાલી)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	નહીંતર જો (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    જો (gen_load_outer == 0)
		gen_load = TRUE;
	}
	નહીંતર
	{
	    lvar_T  lvar;
	    class_T *cl = ખાલી;

	    જો (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		જો (lvar.lv_ufunc != ખાલી && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		નહીંતર જો (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		નહીંતર
		    gen_load = TRUE;
	    }
	    નહીંતર જો (cctx->ctx_ufunc->uf_defclass != ખાલી &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		જો (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    જો (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    નહીંતર
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									ખાલી);
		    }
		}
		નહીંતર
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    નહીંતર
	    {
		imported_T *imp = ખાલી;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		જો (script_var_exists(*arg, namelen, cctx, ખાલી) == OK
			    || (imp = find_imported(name, 0, FALSE)) != ખાલી)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		જો (res == FAIL && is_પદાવલિ && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	જો (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, ખાલી, type);
	જો (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    જો (lt != ખાલી)
	*lt = '<';
    જો (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    પાછા res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    સ્થિર પૂર્ણાંક
compile_string(isn_T *isn, cctx_T *cctx, પૂર્ણાંક str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    પૂર્ણાંક		પદાવલિ_res;
    પૂર્ણાંક		trailing_error;
    પૂર્ણાંક		instr_ગણતરી;
    isn_T	*instr = ખાલી;

    // Remove the string type from the stack.
    --cctx->ctx_type_ઢગલો.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = ખાલી;

    // avoid peeking a next line
    પૂર્ણાંક galen_save = cctx->ctx_ufunc->uf_લીટીઓ.ga_len;
    cctx->ctx_ufunc->uf_લીટીઓ.ga_len = 0;

    પદાવલિ_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_લીટીઓ.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    જો (પદાવલિ_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	જો (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_ઢગલો.ga_len;
	પાછા FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_ગણતરી = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_ગણતરી].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    પાછા OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    પૂર્ણાંક
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	પૂર્ણાંક	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    પૂર્ણાંક	    must_end = FALSE;
    પૂર્ણાંક	    instr_ગણતરી;

    માટે (;;)
    {
	જો (may_get_next_લીટી(whitep, &p, cctx) == FAIL)
	    જાઓ failret;
	જો (*p == ')')
	{
	    *arg = p + 1;
	    પાછા OK;
	}
	જો (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    પાછા FAIL;
	}

	instr_ગણતરી = cctx->ctx_instr.ga_len;
	જો (compile_expr0(&p, cctx) == FAIL)
	    પાછા FAIL;
	++*argcount;

	જો (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_ગણતરી + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_ગણતરી;

	    // {skip} argument of searchpair() can be compiled if not empty
	    જો (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ખાલી
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	નહીંતર જો (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_ગણતરી + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_ગણતરી;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    જો (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ખાલી
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	જો (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    પાછા FAIL;
	}
	જો (*p == ',')
	{
	    ++p;
	    જો (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		પાછા FAIL;
	    }
	}
	નહીંતર
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    પાછા FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    સ્થિર પૂર્ણાંક
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_ઢગલો(cctx, 0);
    પૂર્ણાંક		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    જો (type->tt_type == VAR_OBJECT)
    {
	પૂર્ણાંક	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	જો (uf != ખાલી)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    પાછા res;
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
    સ્થિર પૂર્ણાંક
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	પૂર્ણાંક	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    પૂર્ણાંક		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ખાલી;
    char_u	*gname = ખાલી;
    ufunc_T	*ufunc = ખાલી;
    પૂર્ણાંક		res = FAIL;
    પૂર્ણાંક		is_autoload;
    પૂર્ણાંક		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    જો (varlen >= કદ(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	પાછા FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    જો (import != ખાલી)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	પાછા FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    જો ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	પૂર્ણાંક	    is_has = **arg == 'h';
	પૂર્ણાંક	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	જો (*s == '"')
	    (શૂન્ય)eval_string(&s, &argvars[0], TRUE, FALSE);
	નહીંતર જો (*s == '\'')
	    (શૂન્ય)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	જો (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    જો (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_સંખ્યા;
		tv->vval.v_સંખ્યા = 0;
		જો (is_has)
		    f_has(argvars, tv);
		નહીંતર જો (is_len)
		    f_len(argvars, tv);
		નહીંતર
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    પાછા OK;
	}
	clear_tv(&argvars[0]);
	જો (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    પાછા FAIL;
	}
    }

    જો (generate_ppconst(cctx, ppconst) == FAIL)
	પાછા FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    જો ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    નહીંતર જો (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    નહીંતર
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    જો (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	જો (*arg == ખાલી)
	    જાઓ theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	જો (gname == ખાલી)
	    જાઓ theend;
	++*arg;			// skip '('
    }
    નહીંતર
	*arg += varlen + 1;

    જો (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	જાઓ theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != ખાલી;
    જો (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	પૂર્ણાંક	    idx;

	// builtin function
	idx = find_internal_func(name);
	જો (idx >= 0)
	{
	    જો (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		જાઓ theend;
	    }

	    જો (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_ઢગલો(cctx, 1);
		જો (check_type_is_કિંમત(get_type_on_ઢગલો(cctx, 0)) == FAIL)
		    જાઓ theend;

		// add() can be compiled to instructions if we know the type
		જો (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		નહીંતર જો (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    જો ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		જો (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    જો (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    નહીંતર જો (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    નહીંતર જો (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    જો (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		જો (res == OK)
		    idx = -1;
	    }

	    જો (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	નહીંતર
	    emsg_funcname(e_unknown_function_str, namebuf);
	જાઓ theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    જો (lookup_local(namebuf, varlen, ખાલી, cctx) == FAIL
	    && arg_exists(namebuf, varlen, ખાલી, ખાલી, ખાલી, cctx) != OK)
    {
	class_T		*cl = ખાલી;
	પૂર્ણાંક		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	જો (ufunc != ખાલી)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    જો (ufunc == ખાલી)
		જાઓ theend;

	    જો (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, ખાલી, 0, argcount, FALSE);
		જાઓ theend;
	    }
	    જો (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ખાલી
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		જાઓ theend;
	    }
	}
	નહીંતર જો ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    જો (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], ખાલી,
							0, argcount, FALSE);
	    }
	    નહીંતર
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    જાઓ theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == ખાલી && gname != ખાલી ? gname : namebuf;
    જો (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_ઢગલો(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	જાઓ theend;
    }

    // If we can find a global function by name generate the right call.
    જો (ufunc != ખાલી)
    {
	res = generate_CALL(cctx, ufunc, ખાલી, 0, argcount, FALSE);
	જાઓ theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    જો (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    નહીંતર
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    પાછા res;
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
to_name_end(char_u *arg, પૂર્ણાંક use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    જો (!eval_isnamec1(*arg))
	પાછા arg;

    માટે (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	જો (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == ખાલી))
	    તોડો;
    પાછા p;
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

    જો (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    જો (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	જો (eval_list(&p, &rettv, ખાલી, FALSE) == FAIL)
	    p = arg;
    }
    પાછા p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    સ્થિર પૂર્ણાંક
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    પૂર્ણાંક		ગણતરી = 0;
    પૂર્ણાંક		is_const;
    પૂર્ણાંક		is_all_const = TRUE;	// reset when non-const encountered
    પૂર્ણાંક		must_end = FALSE;

    માટે (;;)
    {
	જો (may_get_next_લીટી(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    પાછા FAIL;
	}
	જો (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    પાછા FAIL;
	}
	જો (*p == ']')
	{
	    ++p;
	    તોડો;
	}
	જો (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    પાછા FAIL;
	}
	જો (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    પાછા FAIL;
	જો (!is_const)
	    is_all_const = FALSE;
	++ગણતરી;
	જો (*p == ',')
	{
	    ++p;
	    જો (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		પાછા FAIL;
	    }
	}
	નહીંતર
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    પાછા generate_NEWLIST(cctx, ગણતરી, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    સ્થિર પૂર્ણાંક
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    પૂર્ણાંક first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    પૂર્ણાંક		ગણતરી = 0;
    પૂર્ણાંક		is_const;
    પૂર્ણાંક		is_all_const = TRUE;	// reset when non-const encountered
    પૂર્ણાંક		must_end = FALSE;

    જો (**arg != ')')
    {
	જો (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    પાછા FAIL;
	}
	ગણતરી = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	માટે (;;)
	{
	    જો (may_get_next_લીટી(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		પાછા FAIL;
	    }
	    જો (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		પાછા FAIL;
	    }
	    જો (*p == ')')
	    {
		++p;
		તોડો;
	    }
	    જો (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		પાછા FAIL;
	    }
	    જો (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		પાછા FAIL;
	    જો (!is_const)
		is_all_const = FALSE;
	    ++ગણતરી;
	    જો (*p == ',')
	    {
		++p;
		જો (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    પાછા FAIL;
		}
	    }
	    નહીંતર
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    પાછા generate_NEWTUPLE(cctx, ગણતરી, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    સ્થિર શૂન્ય
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*લીટી;
    size_t	off;

    જો (!evalarg->eval_using_cmdline || cctx == ખાલી)
	પાછા;

    gap = &evalarg->eval_tofree_ga;
    જો (gap->ga_len == 0)
	પાછા;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    લીટી = ((char_u **)cctx->ctx_ufunc->uf_લીટીઓ.ga_data)[cctx->ctx_lnum];
    *arg = લીટી + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    પૂર્ણાંક
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    પૂર્ણાંક		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    જો (r != OK)
    {
	clear_evalarg(&evalarg, ખાલી);
	પાછા r;
    }

    જો (ppconst != ખાલી && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, ખાલી);
	પાછા FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    જો (cctx->ctx_ufunc != ખાલી)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	પૂર્ણાંક block_depth = cctx->ctx_ufunc->uf_block_depth;

	જો (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(પૂર્ણાંક, block_depth);
	    જો (ufunc->uf_block_ids != ખાલી)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    કદ(પૂર્ણાંક) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    જો (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    જો (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    જો (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	જો (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, ખાલી);

    જો (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	પાછા generate_FUNCREF(cctx, ufunc, ખાલી, FALSE, 0, ખાલી);
    }

    func_ptr_unref(ufunc);
    પાછા FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    પૂર્ણાંક
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	પૂર્ણાંક	    types_optional,
	evalarg_T   *evalarg)
{
    પૂર્ણાંક		r;
    ufunc_T	*ufunc;
    પૂર્ણાંક		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, ખાલી);
    current_sctx.sc_version = save_sc_version;
    જો (r != OK)
	પાછા r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    જો (ufunc->uf_ret_type == ખાલી || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, ખાલી);

    જો (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	પાછા OK;
    }
    clear_tv(rettv);
    પાછા FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    સ્થિર પૂર્ણાંક
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    પૂર્ણાંક		ગણતરી = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    પૂર્ણાંક		is_const;
    પૂર્ણાંક		is_all_const = TRUE;	// reset when non-const encountered

    જો (d == ખાલી)
	પાછા FAIL;
    જો (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	પાછા FAIL;
    }
    માટે (;;)
    {
	char_u	    *ચાવી = ખાલી;

	જો (may_get_next_લીટી(whitep, arg, cctx) == FAIL)
	{
	    *arg = ખાલી;
	    જાઓ failret;
	}

	જો (**arg == '}')
	    તોડો;

	જો (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    જો (compile_expr0(arg, cctx) == FAIL)
		જાઓ failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    જો (isn->isn_type == ISN_PUSHNR)
	    {
		અક્ષર buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.સંખ્યા);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    જો (isn->isn_type == ISN_PUSHS)
		ચાવી = isn->isn_arg.string;
	    નહીંતર જો (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		જાઓ failret;
	    *arg = skipwhite(*arg);
	    જો (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_ચાવી));
		જાઓ failret;
	    }
	    ++*arg;
	}
	નહીંતર
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    ચાવી = get_literal_ચાવી(arg);
	    જો (ચાવી == ખાલી)
		જાઓ failret;
	    જો (generate_PUSHS(cctx, &ચાવી) == FAIL)
		જાઓ failret;
	}

	// Check for duplicate keys, if using string keys.
	જો (ચાવી != ખાલી)
	{
	    item = dict_find(d, ચાવી, -1);
	    જો (item != ખાલી)
	    {
		semsg(_(e_duplicate_ચાવી_in_dictionary_str), ચાવી);
		જાઓ failret;
	    }
	    item = dictitem_alloc(ચાવી);
	    જો (item != ખાલી)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		જો (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	જો (**arg != ':')
	{
	    જો (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    નહીંતર
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    જાઓ failret;
	}
	whitep = *arg + 1;
	જો (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    જાઓ failret;
	}

	જો (may_get_next_લીટી(whitep, arg, cctx) == FAIL)
	{
	    *arg = ખાલી;
	    જાઓ failret;
	}

	જો (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    જાઓ failret;
	જો (!is_const)
	    is_all_const = FALSE;
	++ગણતરી;

	whitep = *arg;
	જો (may_get_next_લીટી(whitep, arg, cctx) == FAIL)
	{
	    *arg = ખાલી;
	    જાઓ failret;
	}
	જો (**arg == '}')
	    તોડો;
	જો (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    જાઓ failret;
	}
	જો (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    જાઓ failret;
	}
	whitep = *arg + 1;
	જો (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    જાઓ failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    જો (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    પાછા generate_NEWDICT(cctx, ગણતરી, FALSE);

failret:
    જો (*arg == ખાલી)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    પાછા FAIL;
}

/*
 * Compile "&option".
 */
    સ્થિર પૂર્ણાંક
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    પૂર્ણાંક		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    જો (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_સંખ્યા ? &t_સંખ્યા : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    પાછા ret;
}

/*
 * Compile "$VAR".
 */
    સ્થિર પૂર્ણાંક
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    પૂર્ણાંક		len;
    પૂર્ણાંક		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    જો (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	પાછા FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    પાછા ret;
}

/*
 * Compile $"string" or $'string'.
 */
    સ્થિર પૂર્ણાંક
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    પૂર્ણાંક		ret;
    પૂર્ણાંક		quote;
    પૂર્ણાંક		evaluate = cctx->ctx_skip != SKIP_YES;
    પૂર્ણાંક		ગણતરી = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    માટે (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	જો (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	નહીંતર
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	જો (ret == FAIL)
	    તોડો;
	જો (evaluate)
	{
	    જો ((tv.vval.v_string != ખાલી && *tv.vval.v_string != NUL)
		    || (**arg != '{' && ગણતરી == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		જો (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    પાછા FAIL;
		tv.vval.v_string = ખાલી;  // don't free it now
		++ગણતરી;
	    }
	    clear_tv(&tv);
	}

	જો (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    તોડો;
	}

	p = compile_one_પદાવલિ_in_str(*arg, cctx);
	જો (p == ખાલી)
	{
	    ret = FAIL;
	    તોડો;
	}
	++ગણતરી;
	*arg = p;
    }

    જો (ret == FAIL || !evaluate)
	પાછા ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    જો (ગણતરી > 1)
	પાછા generate_CONCAT(cctx, ગણતરી);

    પાછા OK;
}

/*
 * Compile "@r".
 */
    સ્થિર પૂર્ણાંક
compile_get_register(char_u **arg, cctx_T *cctx)
{
    પૂર્ણાંક		ret;

    ++*arg;
    જો (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	પાછા FAIL;
    }
    જો (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	પાછા FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, ખાલી, &t_string);
    ++*arg;
    પાછા ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    સ્થિર પૂર્ણાંક
apply_leader(typval_T *rettv, પૂર્ણાંક numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    જ્યારે (p > start)
    {
	--p;
	જો (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    જો (rettv->v_type == VAR_FLOAT)
	    {
		જો (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    નહીંતર
	    {
		varnumber_T	val;
		પૂર્ણાંક		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		જો (check_not_string(rettv) == FAIL)
		    પાછા FAIL;
		val = tv_get_સંખ્યા_chk(rettv, &error);
		clear_tv(rettv);
		જો (error)
		    પાછા FAIL;
		જો (*p == '-')
		    val = -val;
		rettv->v_type = VAR_સંખ્યા;
		rettv->vval.v_સંખ્યા = val;
	    }
	}
	નહીંતર જો (numeric_only)
	{
	    ++p;
	    તોડો;
	}
	નહીંતર જો (*p == '!')
	{
	    પૂર્ણાંક v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_સંખ્યા = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    પાછા OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    સ્થિર શૂન્ય
get_vim_constant(char_u **arg, typval_T *rettv)
{
    જો (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_સંખ્યા = VVAL_TRUE;
	*arg += 6;
    }
    નહીંતર જો (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_સંખ્યા = VVAL_FALSE;
	*arg += 7;
    }
    નહીંતર જો (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_સંખ્યા = VVAL_NULL;
	*arg += 6;
    }
    નહીંતર જો (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_સંખ્યા = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, પૂર્ણાંક *len, પૂર્ણાંક *type_is)
{
    exprtype_T	type = પદાવલિ_UNKNOWN;
    પૂર્ણાંક		i;

    પસંદગી (p[0])
    {
	સ્થિતિ '=':   જો (p[1] == '=')
			type = પદાવલિ_EQUAL;
		    નહીંતર જો (p[1] == '~')
			type = પદાવલિ_MATCH;
		    તોડો;
	સ્થિતિ '!':   જો (p[1] == '=')
			type = પદાવલિ_NEQUAL;
		    નહીંતર જો (p[1] == '~')
			type = પદાવલિ_NOMATCH;
		    તોડો;
	સ્થિતિ '>':   જો (p[1] != '=')
		    {
			type = પદાવલિ_GREATER;
			*len = 1;
		    }
		    નહીંતર
			type = પદાવલિ_GEQUAL;
		    તોડો;
	સ્થિતિ '<':   જો (p[1] != '=')
		    {
			type = પદાવલિ_SMALLER;
			*len = 1;
		    }
		    નહીંતર
			type = પદાવલિ_SEQUAL;
		    તોડો;
	સ્થિતિ 'i':   જો (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			જો (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			જો (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? પદાવલિ_IS : પદાવલિ_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    તોડો;
    }
    પાછા type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    શૂન્ય
skip_પદાવલિ_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_પદાવલિ(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, ખાલી);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    પૂર્ણાંક
bool_on_ઢગલો(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_ઢગલો(cctx, 0);
    જો (type == &t_bool)
	પાછા OK;

    જો (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_સંખ્યા
	    || type == &t_સંખ્યા_bool
	    || type == &t_const_સંખ્યા_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	પાછા generate_COND2BOOL(cctx);

    પાછા need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    શૂન્ય
error_white_both(char_u *op, પૂર્ણાંક len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    સ્થિર પૂર્ણાંક
compile_leader(cctx_T *cctx, પૂર્ણાંક numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    જ્યારે (p > start)
    {
	--p;
	જ્યારે (VIM_ISWHITE(*p))
	    --p;
	જો (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_ઢગલો(cctx, 0);
	    જો (type->tt_type != VAR_FLOAT && need_type(type, &t_સંખ્યા,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		પાછા FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    જો (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == ખાલી)
		પાછા FAIL;
	}
	નહીંતર જો (numeric_only)
	{
	    ++p;
	    તોડો;
	}
	નહીંતર
	{
	    પૂર્ણાંક  invert = *p == '!';

	    જ્યારે (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		જો (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    જો (check_type_is_કિંમત(get_type_on_ઢગલો(cctx, 0)) == FAIL)
		પાછા FAIL;
	    જો (generate_2BOOL(cctx, invert, -1) == FAIL)
		પાછા FAIL;
	}
    }
    *end = p;
    પાછા OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    સ્થિર પૂર્ણાંક
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    પૂર્ણાંક	    ret;
    char_u  *p = *arg + 1;

    જો (may_get_next_લીટી_error(p, arg, cctx) == FAIL)
	પાછા FAIL;

    જો (**arg == ')')
	// empty tuple
	પાછા compile_tuple(arg, cctx, ppconst, FALSE);

    જો (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    નહીંતર
    {
	// Not enough space in ppconst, flush constants.
	જો (generate_ppconst(cctx, ppconst) == FAIL)
	    પાછા FAIL;
	ret = compile_expr0(arg, cctx);
    }
    જો (may_get_next_લીટી_error(*arg, arg, cctx) == FAIL)
	પાછા FAIL;
    જો (ret == OK && **arg == ',')
    {
	// tuple
	પૂર્ણાંક is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	જો (generate_ppconst(cctx, ppconst) == FAIL)
	    પાછા FAIL;
	પાછા compile_tuple(arg, cctx, ppconst, is_const);
    }

    જો (**arg == ')')
	++*arg;
    નહીંતર જો (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    પાછા ret;
}

સ્થિર પૂર્ણાંક compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    સ્થિર પૂર્ણાંક
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    પૂર્ણાંક		keeping_dict = FALSE;

    માટે (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	જો (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_લીટી_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    જો (next != ખાલી &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_લીટી_from_context(cctx, TRUE);
		જો (next == ખાલી)
		    પાછા FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	જો (**arg == '(')
	{
	    પૂર્ણાંક		argcount = 0;

	    જો (generate_ppconst(cctx, ppconst) == FAIL)
		પાછા FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_ઢગલો(cctx, 0);

	    *arg = skipwhite(p + 1);
	    જો (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		પાછા FAIL;
	    જો (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		પાછા FAIL;
	    જો (keeping_dict)
	    {
		keeping_dict = FALSE;
		જો (generate_instr(cctx, ISN_CLEARDICT) == ખાલી)
		    પાછા FAIL;
	    }
	}
	નહીંતર જો (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    પૂર્ણાંક	    alt;
	    char_u  *paren;

	    // something->method()
	    જો (generate_ppconst(cctx, ppconst) == FAIL)
		પાછા FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ઢગલો(cctx, 0);
	    જો (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_કિંમત));
		પાછા FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    જો (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		પાછા FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    જો (**arg == '(')
		alt = 2;
	    નહીંતર
	    {
		// alternative 1 or 3
		p = *arg;
		જો (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    પાછા FAIL;
		}
		જો (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		માટે ( ; eval_isnamec(*p); ++p)
		    ;
		જો (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    જો (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			પાછા FAIL;
		}
		નહીંતર
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    જો (paren == ખાલી)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			પાછા FAIL;
		    }
		}
	    }

	    જો (alt != 1)
	    {
		પૂર્ણાંક	    argcount = 1;
		garray_T    *ઢગલો = &cctx->ctx_type_ઢગલો;
		પૂર્ણાંક	    type_idx_start = ઢગલો->ga_len;
		પૂર્ણાંક	    પદાવલિ_isn_start = cctx->ctx_instr.ga_len;
		પૂર્ણાંક	    પદાવલિ_isn_end;
		પૂર્ણાંક	    arg_isn_ગણતરી;

		જો (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    જો (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			પાછા FAIL;
		}
		નહીંતર
		{
		    પૂર્ણાંક fail;
		    પૂર્ણાંક save_len = cctx->ctx_ufunc->uf_લીટીઓ.ga_len;
		    પૂર્ણાંક	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_પદાવલિ;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_લીટીઓ.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_પદાવલિ;
		    cctx->ctx_ufunc->uf_લીટીઓ.ga_len = save_len;

		    જો (fail)
		    {
			જો (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			પાછા FAIL;
		    }
		}

		// Compile the arguments.
		જો (**arg != '(')
		{
		    જો (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    નહીંતર
			semsg(_(e_missing_parenthesis_str), *arg);
		    પાછા FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		પદાવલિ_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		જો (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    પાછા FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_ગણતરી = cctx->ctx_instr.ga_len - પદાવલિ_isn_end;
		જો (arg_isn_ગણતરી > 0)
		{
		    પૂર્ણાંક	    પદાવલિ_isn_ગણતરી = પદાવલિ_isn_end - પદાવલિ_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, પદાવલિ_isn_ગણતરી);
		    type_T  *decl_type;
		    type2_T  *typep;

		    જો (isn == ખાલી)
			પાછા FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + પદાવલિ_isn_start,
					       કદ(isn_T) * પદાવલિ_isn_ગણતરી);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + પદાવલિ_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + પદાવલિ_isn_end,
						કદ(isn_T) * arg_isn_ગણતરી);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + પદાવલિ_isn_start + arg_isn_ગણતરી,
					  isn, કદ(isn_T) * પદાવલિ_isn_ગણતરી);
		    vim_free(isn);

		    typep = ((type2_T *)ઢગલો->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)ઢગલો->ga_data) + type_idx_start,
			      ((type2_T *)ઢગલો->ga_data) + type_idx_start + 1,
			      કદ(type2_T)
				       * (ઢગલો->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)ઢગલો->ga_data) + ઢગલો->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_ઢગલો(cctx, 0);
		જો (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    પાછા FAIL;
	    }

	    જો (keeping_dict)
	    {
		keeping_dict = FALSE;
		જો (generate_instr(cctx, ISN_CLEARDICT) == ખાલી)
		    પાછા FAIL;
	    }
	}
	નહીંતર જો (**arg == '[')
	{
	    પૂર્ણાંક		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    જો (generate_ppconst(cctx, ppconst) == FAIL)
		પાછા FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    જો (may_get_next_લીટી_error(p, arg, cctx) == FAIL)
		પાછા FAIL;
	    જો (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    નહીંતર
	    {
		જો (compile_expr0(arg, cctx) == FAIL)
		    પાછા FAIL;
		જો (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    પાછા FAIL;
		}
		જો (may_get_next_લીટી_error(*arg, arg, cctx) == FAIL)
		    પાછા FAIL;
		*arg = skipwhite(*arg);
	    }
	    જો (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		જો (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    પાછા FAIL;
		}
		જો (may_get_next_લીટી_error(*arg, arg, cctx) == FAIL)
		    પાછા FAIL;
		જો (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		નહીંતર
		{
		    જો (compile_expr0(arg, cctx) == FAIL)
			પાછા FAIL;
		    જો (may_get_next_લીટી_error(*arg, arg, cctx) == FAIL)
			પાછા FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    જો (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		પાછા FAIL;
	    }
	    *arg = *arg + 1;

	    જો (keeping_dict)
	    {
		keeping_dict = FALSE;
		જો (generate_instr(cctx, ISN_CLEARDICT) == ખાલી)
		    પાછા FAIL;
	    }
	    જો (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		પાછા FAIL;
	}
	નહીંતર જો (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    જો (generate_ppconst(cctx, ppconst) == FAIL)
		પાછા FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ઢગલો(cctx, 0);
	    જો (type != &t_unknown
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
		જો (compile_class_object_index(cctx, arg, type) == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર
	    {
		*arg = p + 1;
		જો (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    પાછા FAIL;
		}
		p = *arg;
		જ્યારે (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		જો (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    પાછા FAIL;
		}
		જો (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == ખાલી)
		    પાછા FAIL;
		જો (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    પાછા FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	નહીંતર
	    તોડો;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    જો (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == ખાલી)
	પાછા FAIL;

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    પૂર્ણાંક		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    પૂર્ણાંક		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    જો (eval_leader(arg, TRUE) == FAIL)
	પાછા FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    પસંદગી (**arg)
    {
	/*
	 * Number constant.
	 */
	સ્થિતિ '0':	// also for blob starting with 0z
	સ્થિતિ '1':
	સ્થિતિ '2':
	સ્થિતિ '3':
	સ્થિતિ '4':
	સ્થિતિ '5':
	સ્થિતિ '6':
	સ્થિતિ '7':
	સ્થિતિ '8':
	સ્થિતિ '9':
	સ્થિતિ '.':   જો (eval_સંખ્યા(arg, rettv, TRUE, FALSE) == FAIL)
			પાછા FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    જો (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			પાછા FAIL;
		    }
		    તોડો;

	/*
	 * String constant: "string".
	 */
	સ્થિતિ '"':   જો (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			પાછા FAIL;
		    તોડો;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	સ્થિતિ '\'':  જો (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			પાછા FAIL;
		    તોડો;

	/*
	 * Constant Vim variable.
	 */
	સ્થિતિ 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    તોડો;

	/*
	 * "true" constant
	 */
	સ્થિતિ 't':   જો (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_સંખ્યા = VVAL_TRUE;
		    }
		    નહીંતર
			ret = NOTDONE;
		    તોડો;

	/*
	 * "false" constant
	 */
	સ્થિતિ 'f':   જો (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_સંખ્યા = VVAL_FALSE;
		    }
		    નહીંતર
			ret = NOTDONE;
		    તોડો;

	/*
	 * "null" or "null_*" constant
	 */
	સ્થિતિ 'n':   જો (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			પૂર્ણાંક	len;

			માટે (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			જો (ret == FAIL)
			    ret = NOTDONE;
			નહીંતર
			    *arg += len + 4;
		    }
		    નહીંતર
			ret = NOTDONE;
		    તોડો;

	/*
	 * List: [expr, expr]
	 */
	સ્થિતિ '[':   જો (generate_ppconst(cctx, ppconst) == FAIL)
			પાછા FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    તોડો;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	સ્થિતિ '{':   જો (generate_ppconst(cctx, ppconst) == FAIL)
			પાછા FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    તોડો;

	/*
	 * Option value: &name
	 */
	સ્થિતિ '&':	જો (generate_ppconst(cctx, ppconst) == FAIL)
			    પાછા FAIL;
			ret = compile_get_option(arg, cctx);
			તોડો;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	સ્થિતિ '$':	જો (generate_ppconst(cctx, ppconst) == FAIL)
			    પાછા FAIL;
			જો ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			નહીંતર
			    ret = compile_get_env(arg, cctx);
			તોડો;

	/*
	 * Register contents: @r.
	 */
	સ્થિતિ '@':	જો (generate_ppconst(cctx, ppconst) == FAIL)
			    પાછા FAIL;
			ret = compile_get_register(arg, cctx);
			તોડો;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	સ્થિતિ '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    જો (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    તોડો;

	મૂળભૂત:    ret = NOTDONE;
		    તોડો;
    }
    જો (ret == FAIL)
	પાછા FAIL;

    જો (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	જો (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	નહીંતર
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    નહીંતર જો (ret == NOTDONE)
    {
	char_u	    *p;
	પૂર્ણાંક	    r;

	જો (!eval_isnamec1(**arg))
	{
	    જો (!vim9_bad_comment(*arg))
	    {
		જો (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		નહીંતર
		    semsg(_(e_name_expected_str), *arg);
	    }
	    પાછા FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	જો (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    પાછા FAIL;
	}

	size_t namelen = p - *arg;
	જો (*p == '<')
	{
	    જો (skip_generic_func_type_args(&p) == FAIL)
		પાછા FAIL;
	}

	જો (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	નહીંતર
	{
	    જો (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		પાછા FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	જો (r == FAIL)
	    પાછા FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    જો (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	પાછા FAIL;
    જો ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	જો (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    પાછા FAIL;
	પાછા OK;
    }
    જો (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	પાછા FAIL;
    પાછા OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    સ્થિર પૂર્ણાંક
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = ખાલી;

    // Recognize <type>
    જો (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	જો (want_type == ખાલી)
	    પાછા FAIL;

	જો (**arg != '>')
	{
	    જો (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    નહીંતર
		emsg(_(e_missing_gt));
	    પાછા FAIL;
	}
	++*arg;
	જો (may_get_next_લીટી_error(*arg, arg, cctx) == FAIL)
	    પાછા FAIL;
    }

    જો (compile_expr9(arg, cctx, ppconst) == FAIL)
	પાછા FAIL;

    જો (want_type != ખાલી)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_ઢગલો(cctx, 0);
	જો (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    જો (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		પાછા FAIL;
	}
    }

    પાછા OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    સ્થિર પૂર્ણાંક
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    પૂર્ણાંક		ppconst_used = ppconst->pp_used;

    // get the first expression
    જો (compile_expr8(arg, cctx, ppconst) == FAIL)
	પાછા FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    માટે (;;)
    {
	op = may_peek_next_લીટી(cctx, *arg, &next);
	જો (*op != '*' && *op != '/' && *op != '%')
	    તોડો;
	જો (next != ખાલી)
	{
	    *arg = next_લીટી_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	જો (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    પાછા FAIL;
	}
	જો (may_get_next_લીટી_error(op + 1, arg, cctx) == FAIL)
	    પાછા FAIL;

	// get the second expression
	જો (compile_expr8(arg, cctx, ppconst) == FAIL)
	    પાછા FAIL;

	જો (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_સંખ્યા
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_સંખ્યા)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    પૂર્ણાંક		    failed = FALSE;

	    // both are numbers: compute the result
	    પસંદગી (*op)
	    {
		સ્થિતિ '*': res = tv1->vval.v_સંખ્યા * tv2->vval.v_સંખ્યા;
			  તોડો;
		સ્થિતિ '/': res = અંક_divide(tv1->vval.v_સંખ્યા,
						  tv2->vval.v_સંખ્યા, &failed);
			  તોડો;
		સ્થિતિ '%': res = અંક_modulus(tv1->vval.v_સંખ્યા,
						  tv2->vval.v_સંખ્યા, &failed);
			  તોડો;
	    }
	    જો (failed)
		પાછા FAIL;
	    tv1->vval.v_સંખ્યા = res;
	    --ppconst->pp_used;
	}
	નહીંતર
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    પાછા OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    સ્થિર પૂર્ણાંક
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    પૂર્ણાંક		oplen;
    પૂર્ણાંક		ppconst_used = ppconst->pp_used;

    // get the first variable
    જો (compile_expr7(arg, cctx, ppconst) == FAIL)
	પાછા FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    માટે (;;)
    {
	op = may_peek_next_લીટી(cctx, *arg, &next);
	જો (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    તોડો;
	જો (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    તોડો;
	oplen = (*op == '.' ? 2 : 1);
	જો (next != ખાલી)
	{
	    *arg = next_લીટી_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	જો (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    પાછા FAIL;
	}

	જો (may_get_next_લીટી_error(op + oplen, arg, cctx) == FAIL)
	    પાછા FAIL;

	// get the second expression
	જો (compile_expr7(arg, cctx, ppconst) == FAIL)
	    પાછા FAIL;

	જો (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_સંખ્યા
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_સંખ્યા)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    જો (*op == '+')
		tv1->vval.v_સંખ્યા = tv1->vval.v_સંખ્યા + tv2->vval.v_સંખ્યા;
	    નહીંતર જો (*op == '-')
		tv1->vval.v_સંખ્યા = tv1->vval.v_સંખ્યા - tv2->vval.v_સંખ્યા;
	    નહીંતર
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		જો (tv1->vval.v_string == ખાલી)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    પાછા FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	નહીંતર
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    જો (*op == '.')
	    {
		જો (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    પાછા FAIL;
		જો (generate_CONCAT(cctx, 2) == FAIL)
		    પાછા FAIL;
	    }
	    નહીંતર
		generate_two_op(cctx, op);
	}
    }

    પાછા OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    સ્થિર પૂર્ણાંક
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = પદાવલિ_UNKNOWN;
    char_u	*p;
    char_u	*next;
    પૂર્ણાંક		len = 2;
    પૂર્ણાંક		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    જો (compile_expr6(arg, cctx, ppconst) == FAIL)
	પાછા FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    માટે (;;)
    {
	type = પદાવલિ_UNKNOWN;

	p = may_peek_next_લીટી(cctx, *arg, &next);
	જો (p[0] == '<' && p[1] == '<')
	    type = પદાવલિ_LSHIFT;
	નહીંતર જો (p[0] == '>' && p[1] == '>')
	    type = પદાવલિ_RSHIFT;

	જો (type == પદાવલિ_UNKNOWN)
	    પાછા OK;

	// Handle a bitwise left or right shift operator
	જો (ppconst->pp_used == ppconst_used + 1)
	{
	    જો (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_સંખ્યા)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_સંખ્યા));
		પાછા FAIL;
	    }
	}
	નહીંતર
	{
	    type_T	*t = get_type_on_ઢગલો(cctx, 0);

	    જો (need_type(t, &t_સંખ્યા, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_સંખ્યા));
		પાછા FAIL;
	    }
	}

	જો (next != ખાલી)
	{
	    *arg = next_લીટી_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	જો (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    પાછા FAIL;
	}

	// get the second variable
	જો (may_get_next_લીટી_error(p + len, arg, cctx) == FAIL)
	    પાછા FAIL;

	જો (compile_expr6(arg, cctx, ppconst) == FAIL)
	    પાછા FAIL;

	જો (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    જો (tv2->v_type != VAR_સંખ્યા || tv2->vval.v_સંખ્યા < 0)
	    {
		// right operand should be a positive number
		જો (tv2->v_type != VAR_સંખ્યા)
		    emsg(_(e_bitshift_ops_must_be_સંખ્યા));
		નહીંતર
		    emsg(_(e_bitshift_ops_must_be_positive));
		પાછા FAIL;
	    }

	    જો (tv2->vval.v_સંખ્યા > MAX_LSHIFT_BITS)
		tv1->vval.v_સંખ્યા = 0;
	    નહીંતર જો (type == પદાવલિ_LSHIFT)
		tv1->vval.v_સંખ્યા =
			(uvarnumber_T)tv1->vval.v_સંખ્યા << tv2->vval.v_સંખ્યા;
	    નહીંતર
		tv1->vval.v_સંખ્યા =
			(uvarnumber_T)tv1->vval.v_સંખ્યા >> tv2->vval.v_સંખ્યા;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	નહીંતર
	{
	    જો (need_type(get_type_on_ઢગલો(cctx, 0), &t_સંખ્યા, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_સંખ્યા));
		પાછા FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    જો (isn == ખાલી)
		પાછા FAIL;

	    જો (isn != ખાલી)
		isn->isn_arg.op.op_type = type;
	}
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = પદાવલિ_UNKNOWN;
    char_u	*p;
    char_u	*next;
    પૂર્ણાંક		len = 2;
    પૂર્ણાંક		type_is = FALSE;
    પૂર્ણાંક		ppconst_used = ppconst->pp_used;

    // get the first variable
    જો (compile_expr5(arg, cctx, ppconst) == FAIL)
	પાછા FAIL;

    p = may_peek_next_લીટી(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    જો (type != પદાવલિ_UNKNOWN)
    {
	પૂર્ણાંક ic = FALSE;  // Default: do not ignore case

	જો (next != ખાલી)
	{
	    *arg = next_લીટી_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	જો (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    પાછા FAIL;
	}
	// extra question mark appended: ignore case
	જો (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	નહીંતર જો (p[len] == '#')
	    ++len;
	// nothing appended: match case

	જો (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    પાછા FAIL;
	}

	// get the second variable
	જો (may_get_next_લીટી_error(p + len, arg, cctx) == FAIL)
	    પાછા FAIL;

	જો (compile_expr5(arg, cctx, ppconst) == FAIL)
	    પાછા FAIL;

	જો (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    પૂર્ણાંક		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    જો (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    નહીંતર
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_સંખ્યા = tv1->vval.v_સંખ્યા
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    પાછા ret;
	}

	generate_ppconst(cctx, ppconst);
	પાછા generate_COMPARE(cctx, type, ic);
    }

    પાછા OK;
}

સ્થિર પૂર્ણાંક compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    સ્થિર પૂર્ણાંક
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	અક્ષર	*op,
	ppconst_T *ppconst,
	પૂર્ણાંક	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_લીટી(cctx, *arg, &next);
    પૂર્ણાંક		opchar = *op;

    જો (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	પૂર્ણાંક		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, કદ(પૂર્ણાંક), 10);
	જ્યારે (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    પૂર્ણાંક		start_ctx_lnum = cctx->ctx_lnum;
	    પૂર્ણાંક		save_lnum;
	    પૂર્ણાંક		const_used;
	    પૂર્ણાંક		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    જો (next != ખાલી)
	    {
		*arg = next_લીટી_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    જો (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		પાછા FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    જો (status != FAIL)
	    {
		// Use the last ppconst if possible.
		જો (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    પૂર્ણાંક		is_true = tv2bool(tv);

		    જો ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_સંખ્યા = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    નહીંતર
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		નહીંતર
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_ઢગલો(cctx);
		}
	    }
	    જો (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    જો (status == FAIL)
	    {
		ga_clear(&end_ga);
		પાછા FAIL;
	    }

	    જો (jump_when != JUMP_NEVER)
	    {
		જો (cctx->ctx_skip != SKIP_YES)
		{
		    *(((પૂર્ણાંક *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    જો (may_get_next_લીટી_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		પાછા FAIL;
	    }

	    const_used = ppconst->pp_used;
	    જો ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		પાછા FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    જો (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		જો (tv->v_type == VAR_સંખ્યા
			 && (tv->vval.v_સંખ્યા == 1 || tv->vval.v_સંખ્યા == 0))
		{
		    tv->vval.v_સંખ્યા = tv->vval.v_સંખ્યા == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_લીટી(cctx, *arg, &next);
	}

	જો (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    પાછા FAIL;
	}

	જો (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    જો (bool_on_ઢગલો(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		પાછા FAIL;
	    }

	જો (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    જ્યારે (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((પૂર્ણાંક *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    પાછા OK;
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
    સ્થિર પૂર્ણાંક
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    પૂર્ણાંક		ppconst_used = ppconst->pp_used;

    // get the first variable
    જો (compile_expr4(arg, cctx, ppconst) == FAIL)
	પાછા FAIL;

    // || and && work almost the same
    પાછા compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    સ્થિર પૂર્ણાંક
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    પૂર્ણાંક		ppconst_used = ppconst->pp_used;

    // eval the first expression
    જો (compile_expr3(arg, cctx, ppconst) == FAIL)
	પાછા FAIL;

    // || and && work almost the same
    પાછા compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    પૂર્ણાંક
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    પૂર્ણાંક		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    જો (cctx->ctx_skip == SKIP_YES)
    {
	પૂર્ણાંક		prev_did_emsg = did_emsg;

	skip_પદાવલિ_cctx(arg, cctx);
	પાછા did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    જો (compile_expr2(arg, cctx, ppconst) == FAIL)
	પાછા FAIL;

    p = may_peek_next_લીટી(cctx, *arg, &next);
    જો (*p == '?')
    {
	પૂર્ણાંક		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*ઢગલો = &cctx->ctx_type_ઢગલો;
	પૂર્ણાંક		alt_idx = instr->ga_len;
	પૂર્ણાંક		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = ખાલી;
	પૂર્ણાંક		has_const_પદાવલિ = FALSE;
	પૂર્ણાંક		const_કિંમત = FALSE;
	પૂર્ણાંક		save_skip = cctx->ctx_skip;

	જો (next != ખાલી)
	{
	    *arg = next_લીટી_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	જો (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    પાછા FAIL;
	}

	જો (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_પદાવલિ = TRUE;
	    જો (op_falsy)
		const_કિંમત = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    નહીંતર
	    {
		પૂર્ણાંક error = FALSE;

		const_કિંમત = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		જો (error)
		    પાછા FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_કિંમત : !const_કિંમત) ? SKIP_YES : SKIP_NOT;

	    જો (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    નહીંતર
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	નહીંતર
	{
	    generate_ppconst(cctx, ppconst);
	    જો (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    જો (op_falsy)
	    {
		type1 = get_type_on_ઢગલો(cctx, -1);
		જો (check_type_is_કિંમત(type1) == FAIL)
		    પાછા FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	જો (may_get_next_લીટી_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    પાછા FAIL;
	જો (compile_expr1(arg, cctx, ppconst) == FAIL)
	    પાછા FAIL;

	જો (!has_const_પદાવલિ)
	{
	    generate_ppconst(cctx, ppconst);

	    જો (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_ઢગલો(cctx, 0);
		--ઢગલો->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	જો (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_લીટી(cctx, *arg, &next);
	    જો (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		પાછા FAIL;
	    }
	    જો (next != ખાલી)
	    {
		*arg = next_લીટી_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    જો (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		પાછા FAIL;
	    }

	    // evaluate the third expression
	    જો (has_const_પદાવલિ)
		cctx->ctx_skip = save_skip == SKIP_YES || const_કિંમત
							 ? SKIP_YES : SKIP_NOT;
	    જો (may_get_next_લીટી_error(p + 1, arg, cctx) == FAIL)
		પાછા FAIL;
	    જો (compile_expr1(arg, cctx, ppconst) == FAIL)
		પાછા FAIL;
	}

	જો (!has_const_પદાવલિ)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)ઢગલો->ga_data)
					      + ઢગલો->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    પાછા OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
compile_expr0_ext(char_u **arg,  cctx_T *cctx, પૂર્ણાંક *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    જો (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	પાછા FAIL;
    }
    જો (is_const != ખાલી)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    જો (generate_ppconst(cctx, &ppconst) == FAIL)
	પાછા FAIL;
    પાછા OK;
}

/*
 * Toplevel expression.
 */
    પૂર્ણાંક
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    પાછા compile_expr0_ext(arg, cctx, ખાલી);
}


#endif // defined(FEAT_EVAL)
