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
#अन्तर्भाव "vim.h"

#जइ defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
स्थिर पूर्णाङ्क paren_follows_after_व्यञ्जकम् = 0;

/*
 * Generate code for any ppconst entries.
 */
    पूर्णाङ्क
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    पूर्णाङ्क	    i;
    पूर्णाङ्क	    ret = OK;
    पूर्णाङ्क	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    प्रति (i = 0; i < ppconst->pp_used; ++i)
	जइ (generate_tv_प्रक्षिप(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    प्रत्यागम ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    स्थिर पूर्णाङ्क
check_ppconst_bool(ppconst_T *ppconst)
{
    जइ (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	प्रत्यागम check_typval_type(&t_bool, tv, where);
    }
    प्रत्यागम OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    शून्य
clear_ppconst(ppconst_T *ppconst)
{
    पूर्णाङ्क	    i;

    प्रति (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    स्थिर पूर्णाङ्क
compile_tuple_member(
    type2_T	*typep,
    पूर्णाङ्क		is_slice,
    cctx_T	*cctx)
{
    जइ (is_slice)
    {
	जइ (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    प्रत्यागम FAIL;
	// a copy is made so the member type is no longer declared
	जइ (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	जइ (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    जइ (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    अण्णहा
    {
	जइ (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    जइ (typep->type_curr->tt_argcount == 1)
	    {
		जइ (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		अण्णहा
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    अण्णहा
		typep->type_curr = &t_any;
	    जइ (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		जइ (typep->type_decl->tt_argcount == 1)
		{
		    जइ (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    अण्णहा
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		अण्णहा
		    typep->type_curr = &t_any;
	    }
	    अण्णहा
		typep->type_decl = typep->type_curr;
	}
	जइ (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    पूर्णाङ्क
compile_member(पूर्णाङ्क is_slice, पूर्णाङ्क *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*राशिः = &cctx->ctx_type_राशिः;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)राशिः->ga_data) + राशिः->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)राशिः->ga_data) + राशिः->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    जइ ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    जइ (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	जइ (need_type(idxtype, &t_सङ्ख्या, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    प्रत्यागम FAIL;
	जइ (is_slice)
	{
	    idxtype = get_type_on_राशिः(cctx, 1);
	    जइ (need_type(idxtype, &t_सङ्ख्या, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;
	}
    }

    जइ (vartype == VAR_DICT)
    {
	जइ (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    प्रत्यागम FAIL;
	}
	जइ (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    जइ (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    जइ (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		जइ (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    अण्णहा
		typep->type_decl = typep->type_curr;
	}
	अण्णहा
	{
	    जइ (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	जइ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    प्रत्यागम FAIL;
	जइ (keeping_dict != रिक्त)
	    *keeping_dict = TRUE;
    }
    अण्णहा जइ (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	जइ ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा जइ (vartype == VAR_BLOB)
    {
	जइ (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    जइ (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		प्रत्यागम FAIL;
	}
	अण्णहा
	{
	    typep->type_curr = &t_सङ्ख्या;
	    typep->type_decl = &t_सङ्ख्या;
	    जइ (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		प्रत्यागम FAIL;
	}
    }
    अण्णहा जइ (vartype == VAR_TUPLE)
    {
	जइ (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    प्रत्यागम FAIL;
    }
    अण्णहा जइ (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	जइ (is_slice)
	{
	    जइ (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		प्रत्यागम FAIL;
	    // a copy is made so the member type is no longer declared
	    जइ (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    जइ (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		जइ (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	अण्णहा
	{
	    जइ (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		जइ (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		जइ (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    जइ (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		अण्णहा
			typep->type_decl = typep->type_curr;
	    }
	    जइ (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		प्रत्यागम FAIL;
	}
    }
    अण्णहा
    {
	विकल्प (vartype)
	{
	    पक्ष VAR_FUNC:
	    पक्ष VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		भङ्ग;
	    पक्ष VAR_BOOL:
	    पक्ष VAR_SPECIAL:
	    पक्ष VAR_JOB:
	    पक्ष VAR_CHANNEL:
	    पक्ष VAR_INSTR:
	    पक्ष VAR_CLASS:
	    पक्ष VAR_OBJECT:
	    पक्ष VAR_TYPEALIAS:
	    पक्ष VAR_UNKNOWN:
	    पक्ष VAR_ANY:
	    पक्ष VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		भङ्ग;
	    स्वभाव:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    स्थिर पूर्णाङ्क
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    प्रति (cctx_T *cctx = cctx_arg; cctx != रिक्त; cctx = cctx->ctx_outer)
    {
	जइ (cctx->ctx_ufunc != रिक्त && cctx->ctx_ufunc->uf_class != रिक्त)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    जाव (clp != रिक्त)
	    {
		जइ (clp == cl)
		    प्रत्यागम TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    प्रत्यागम FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    पूर्णाङ्क
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    पूर्णाङ्क		m_idx;
    पूर्णाङ्क		ret = FAIL;

    जइ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	प्रत्यागम FAIL;
    }

    class_T *cl = type->tt_class;
    पूर्णाङ्क is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    जइ (type == &t_super)
    {
	जइ (cctx->ctx_ufunc == रिक्त || cctx->ctx_ufunc->uf_class == रिक्त)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    प्रत्यागम FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_राशिः.ga_len;
    }
    अण्णहा जइ (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	जइ (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    जइ (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    जइ (cl == रिक्त)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	प्रत्यागम FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, रिक्त, रिक्त, FNE_CHECK_START);
    जइ (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	प्रत्यागम FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    जइ (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	जइ (name_end == रिक्त)
	    गच्छ done;
    }

    जइ (*name_end == '(')
    {
	पूर्णाङ्क	function_गणनम्;
	पूर्णाङ्क	child_गणनम्;
	ufunc_T	**functions;

	जइ (type->tt_type == VAR_CLASS)
	{
	    function_गणनम् = cl->class_class_function_गणनम्;
	    child_गणनम् = cl->class_class_function_गणनम्_child;
	    functions = cl->class_class_functions;
	}
	अण्णहा
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    जइ (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_गणनम् = cl->class_obj_method_गणनम्;
	    child_गणनम् = cl->class_obj_method_गणनम्_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = रिक्त;
	पूर्णाङ्क fi;
	प्रति (fi = is_super ? child_गणनम् : 0; fi < function_गणनम्; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    जइ (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		भङ्ग;
	    }
	}

	ocmember_T  *ocm = रिक्त;
	जइ (ufunc == रिक्त)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    जइ (ocm == रिक्त || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_सन्देशः(cl, type->tt_type, name, len);
		गच्छ done;
	    }
	    जइ (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_राशिः.ga_len;
		जइ (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    गच्छ done;
	    }
	    अण्णहा
	    {
		पूर्णाङ्क status;

		जइ (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		अण्णहा
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		जइ (status == FAIL)
		    गच्छ done;
	    }
	}

	जइ (is_super && ufunc != रिक्त && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    गच्छ done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	जइ (ocm == रिक्त && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    गच्छ done;
	}

	// process generic function call
	जइ (ufunc != रिक्त)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    जइ (ufunc == रिक्त)
		गच्छ done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	पूर्णाङ्क argcount = 0;
	जइ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    गच्छ done;

	जइ (ocm != रिक्त)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    गच्छ done;
	}
	जइ (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	अण्णहा
	    ret = generate_CALL(cctx, ufunc, रिक्त, 0, argcount, FALSE);
	गच्छ done;
    }

    जइ (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	जइ (m_idx >= 0)
	{
	    जइ (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		गच्छ done;
	    }

	    *arg = name_end;
	    जइ (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    अण्णहा
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    गच्छ done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	जइ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    जइ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		गच्छ done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_राशिः.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, रिक्त);
	    गच्छ done;
	}

	member_not_found_सन्देशः(cl, VAR_OBJECT, name, len);
    }
    अण्णहा
    {
	// load class member
	पूर्णाङ्क idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	जइ (m != रिक्त)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    जइ (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		गच्छ done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_राशिः.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    गच्छ done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	जइ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    जइ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		गच्छ done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_राशिः.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, रिक्त);
	    गच्छ done;
	}

	member_not_found_सन्देशः(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    प्रत्यागम ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    पूर्णाङ्क
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    पूर्णाङ्क		    idx;
    imported_T	    *imp;

    जइ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	प्रत्यागम FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, रिक्त);
    जइ (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	प्रत्यागम OK;
    }

    जइ (end == रिक्त)
	imp = रिक्त;
    अण्णहा जइ (import == रिक्त)
	imp = find_imported(name, 0, FALSE);
    अण्णहा
	imp = import;

    जइ (imp != रिक्त)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	पूर्णाङ्क	cc;
	ufunc_T	*ufunc = रिक्त;
	type_T	*type;
	पूर्णाङ्क	done = FALSE;
	पूर्णाङ्क	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	जइ (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    प्रत्यागम FAIL;
	}
	++p;
	जइ (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    प्रत्यागम FAIL;
	}

	// isolate one name
	exp_name = p;
	जाव (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	जइ (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	जइ (res == OK)
	{
	    जइ (si->sn_autoload_prefix != रिक्त
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		जइ (cc == '(' || paren_follows_after_व्यञ्जकम्)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		अण्णहा
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    अण्णहा जइ (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		जइ (cc == '(' || paren_follows_after_व्यञ्जकम्)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		अण्णहा
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    अण्णहा
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, रिक्त, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	जइ (done)
	    प्रत्यागम res;

	जइ (idx < 0)
	{
	    जइ (ufunc != रिक्त)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		जइ (IS_GENERIC_FUNC(ufunc))
		{
		    जइ (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			जइ (p != रिक्त)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			जइ (p == रिक्त || ufunc == रिक्त)
			    प्रत्यागम FAIL;
		    }
		    अण्णहा
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			प्रत्यागम FAIL;
		    }
		}
		अण्णहा जइ (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    प्रत्यागम FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, रिक्त, TRUE);
		प्रत्यागम OK;
	    }
	    प्रत्यागम FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	प्रत्यागम OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    प्रत्यागम generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    स्थिर पूर्णाङ्क
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    पूर्णाङ्क			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    जइ (ufunc == रिक्त || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	प्रत्यागम FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    जइ (ufunc == रिक्त)
	प्रत्यागम FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    जइ (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, रिक्त) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    स्थिर पूर्णाङ्क
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    प्रत्यागम cctx->ctx_ufunc != रिक्त
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == रिक्त
		|| cctx->ctx_outer->ctx_ufunc == रिक्त
		|| cctx->ctx_outer->ctx_ufunc->uf_class == रिक्त
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
    स्थिर पूर्णाङ्क
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    पूर्णाङ्क			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    पूर्णाङ्क		isn_idx;

    जइ (fp == रिक्त)
	प्रत्यागम FAIL;

    प्रति (पूर्णाङ्क depth = 0; depth < outer_depth; ++depth)
    {
	जइ (outer_cctx->ctx_outer == रिक्त)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    प्रत्यागम FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    जइ (fp->uf_block_ids == रिक्त && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(पूर्णाङ्क, ufunc->uf_block_depth);
	जइ (fp->uf_block_ids != रिक्त)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    परिमाण(पूर्णाङ्क) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    जइ (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	प्रत्यागम FAIL;

    जइ (generate_FUNCREF(cctx, fp, रिक्त, FALSE, 0, &isn_idx) == FAIL)
	प्रत्यागम FAIL;
    जइ (cctx->ctx_skip == SKIP_YES)
	प्रत्यागम OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    जइ (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	जइ (extra == रिक्त)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    जइ (extra == रिक्त)
		प्रत्यागम FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    प्रत्यागम OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    पूर्णाङ्क
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	पूर्णाङ्क	is_व्यञ्जकम्,
	पूर्णाङ्क	error)
{
    type_T	*type;
    char_u	*name = रिक्त;
    char_u	*end = end_arg;
    पूर्णाङ्क		res = FAIL;
    पूर्णाङ्क		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = रिक्त;

    generic_func_args_table_init(&gfatab);

    जइ (*(*arg + namelen) == '<')
    {
	// generic function call
	जइ (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == रिक्त)
	{
	    generic_func_args_table_clear(&gfatab);
	    प्रत्यागम FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    जइ (*(*arg + 1) == ':')
    {
	जइ (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    विकल्प (**arg)
	    {
		पक्ष 'g': isn_type = ISN_LOADGDICT; भङ्ग;
		पक्ष 'w': isn_type = ISN_LOADWDICT; भङ्ग;
		पक्ष 't': isn_type = ISN_LOADTDICT; भङ्ग;
		पक्ष 'b': isn_type = ISN_LOADBDICT; भङ्ग;
		स्वभाव:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    गच्छ theend;
	    }
	    जइ (generate_instr_type(cctx, isn_type, &t_dict_any) == रिक्त)
		गच्छ theend;
	    res = OK;
	}
	अण्णहा
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    जइ (name == रिक्त)
		गच्छ theend;

	    विकल्प (**arg)
	    {
		पक्ष 'v': res = generate_LOADV(cctx, name);
			  भङ्ग;
		पक्ष 's': जइ (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      गच्छ theend;
			  }
			  जइ (is_व्यञ्जकम् && find_func(name, FALSE) != रिक्त)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  अण्णहा
			      res = compile_load_scriptvar(cctx, name,
							    रिक्त, &end, रिक्त);
			  भङ्ग;
		पक्ष 'g': जइ (vim_strchr(name, AUTOLOAD_CHAR) == रिक्त)
			  {
			      जइ (is_व्यञ्जकम् && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != रिक्त
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      अण्णहा
				  isn_type = ISN_LOADG;
			  }
			  अण्णहा
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      जइ (name == रिक्त)
				  गच्छ theend;
			  }
			  भङ्ग;
		पक्ष 'w': isn_type = ISN_LOADW; भङ्ग;
		पक्ष 't': isn_type = ISN_LOADT; भङ्ग;
		पक्ष 'b': isn_type = ISN_LOADB; भङ्ग;
		स्वभाव:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  गच्छ theend;
	    }
	    जइ (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    अण्णहा
    {
	पूर्णाङ्क	    idx;
	पूर्णाङ्क	    method_idx;
	पूर्णाङ्क	    gen_load = FALSE;
	पूर्णाङ्क	    gen_load_outer = 0;
	पूर्णाङ्क	    outer_loop_depth = -1;
	पूर्णाङ्क	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	जइ (name == रिक्त)
	    गच्छ theend;

	जइ (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = प्रक्षिप_type_राशिः(cctx, &t_super);
	    जइ (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	अण्णहा जइ (vim_strchr(name, AUTOLOAD_CHAR) != रिक्त)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	अण्णहा जइ (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    जइ (gen_load_outer == 0)
		gen_load = TRUE;
	}
	अण्णहा
	{
	    lvar_T  lvar;
	    class_T *cl = रिक्त;

	    जइ (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		जइ (lvar.lv_ufunc != रिक्त && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		अण्णहा जइ (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		अण्णहा
		    gen_load = TRUE;
	    }
	    अण्णहा जइ (cctx->ctx_ufunc->uf_defclass != रिक्त &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		जइ (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    जइ (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    अण्णहा
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									रिक्त);
		    }
		}
		अण्णहा
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    अण्णहा
	    {
		imported_T *imp = रिक्त;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		जइ (script_var_exists(*arg, namelen, cctx, रिक्त) == OK
			    || (imp = find_imported(name, 0, FALSE)) != रिक्त)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		जइ (res == FAIL && is_व्यञ्जकम् && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	जइ (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, रिक्त, type);
	जइ (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    जइ (lt != रिक्त)
	*lt = '<';
    जइ (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    प्रत्यागम res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    स्थिर पूर्णाङ्क
compile_string(isn_T *isn, cctx_T *cctx, पूर्णाङ्क str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    पूर्णाङ्क		व्यञ्जकम्_res;
    पूर्णाङ्क		trailing_error;
    पूर्णाङ्क		instr_गणनम्;
    isn_T	*instr = रिक्त;

    // Remove the string type from the stack.
    --cctx->ctx_type_राशिः.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = रिक्त;

    // avoid peeking a next line
    पूर्णाङ्क galen_save = cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len;
    cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len = 0;

    व्यञ्जकम्_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    जइ (व्यञ्जकम्_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	जइ (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_राशिः.ga_len;
	प्रत्यागम FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_गणनम् = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_गणनम्].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    प्रत्यागम OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    पूर्णाङ्क
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	पूर्णाङ्क	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    पूर्णाङ्क	    must_end = FALSE;
    पूर्णाङ्क	    instr_गणनम्;

    प्रति (;;)
    {
	जइ (may_get_next_पङ्क्तिः(whitep, &p, cctx) == FAIL)
	    गच्छ failret;
	जइ (*p == ')')
	{
	    *arg = p + 1;
	    प्रत्यागम OK;
	}
	जइ (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    प्रत्यागम FAIL;
	}

	instr_गणनम् = cctx->ctx_instr.ga_len;
	जइ (compile_expr0(&p, cctx) == FAIL)
	    प्रत्यागम FAIL;
	++*argcount;

	जइ (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_गणनम् + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_गणनम्;

	    // {skip} argument of searchpair() can be compiled if not empty
	    जइ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != रिक्त
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	अण्णहा जइ (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_गणनम् + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_गणनम्;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    जइ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != रिक्त
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	जइ (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    प्रत्यागम FAIL;
	}
	जइ (*p == ',')
	{
	    ++p;
	    जइ (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		प्रत्यागम FAIL;
	    }
	}
	अण्णहा
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    प्रत्यागम FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    स्थिर पूर्णाङ्क
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_राशिः(cctx, 0);
    पूर्णाङ्क		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    जइ (type->tt_type == VAR_OBJECT)
    {
	पूर्णाङ्क	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	जइ (uf != रिक्त)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    प्रत्यागम res;
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
    स्थिर पूर्णाङ्क
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	पूर्णाङ्क	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    पूर्णाङ्क		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = रिक्त;
    char_u	*gname = रिक्त;
    ufunc_T	*ufunc = रिक्त;
    पूर्णाङ्क		res = FAIL;
    पूर्णाङ्क		is_autoload;
    पूर्णाङ्क		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    जइ (varlen >= परिमाण(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	प्रत्यागम FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    जइ (import != रिक्त)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	प्रत्यागम FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    जइ ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	पूर्णाङ्क	    is_has = **arg == 'h';
	पूर्णाङ्क	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	जइ (*s == '"')
	    (शून्य)eval_string(&s, &argvars[0], TRUE, FALSE);
	अण्णहा जइ (*s == '\'')
	    (शून्य)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	जइ (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    जइ (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_सङ्ख्या;
		tv->vval.v_सङ्ख्या = 0;
		जइ (is_has)
		    f_has(argvars, tv);
		अण्णहा जइ (is_len)
		    f_len(argvars, tv);
		अण्णहा
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    प्रत्यागम OK;
	}
	clear_tv(&argvars[0]);
	जइ (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    प्रत्यागम FAIL;
	}
    }

    जइ (generate_ppconst(cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    जइ ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    अण्णहा जइ (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    अण्णहा
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    जइ (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	जइ (*arg == रिक्त)
	    गच्छ theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	जइ (gname == रिक्त)
	    गच्छ theend;
	++*arg;			// skip '('
    }
    अण्णहा
	*arg += varlen + 1;

    जइ (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	गच्छ theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != रिक्त;
    जइ (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	पूर्णाङ्क	    idx;

	// builtin function
	idx = find_internal_func(name);
	जइ (idx >= 0)
	{
	    जइ (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		गच्छ theend;
	    }

	    जइ (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_राशिः(cctx, 1);
		जइ (check_type_is_मानम्(get_type_on_राशिः(cctx, 0)) == FAIL)
		    गच्छ theend;

		// add() can be compiled to instructions if we know the type
		जइ (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		अण्णहा जइ (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    जइ ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		जइ (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    जइ (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    अण्णहा जइ (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    अण्णहा जइ (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    जइ (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		जइ (res == OK)
		    idx = -1;
	    }

	    जइ (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	अण्णहा
	    emsg_funcname(e_unknown_function_str, namebuf);
	गच्छ theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    जइ (lookup_local(namebuf, varlen, रिक्त, cctx) == FAIL
	    && arg_exists(namebuf, varlen, रिक्त, रिक्त, रिक्त, cctx) != OK)
    {
	class_T		*cl = रिक्त;
	पूर्णाङ्क		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	जइ (ufunc != रिक्त)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    जइ (ufunc == रिक्त)
		गच्छ theend;

	    जइ (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, रिक्त, 0, argcount, FALSE);
		गच्छ theend;
	    }
	    जइ (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == रिक्त
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		गच्छ theend;
	    }
	}
	अण्णहा जइ ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    जइ (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], रिक्त,
							0, argcount, FALSE);
	    }
	    अण्णहा
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    गच्छ theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == रिक्त && gname != रिक्त ? gname : namebuf;
    जइ (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_राशिः(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	गच्छ theend;
    }

    // If we can find a global function by name generate the right call.
    जइ (ufunc != रिक्त)
    {
	res = generate_CALL(cctx, ufunc, रिक्त, 0, argcount, FALSE);
	गच्छ theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    जइ (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    अण्णहा
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    प्रत्यागम res;
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
to_name_end(char_u *arg, पूर्णाङ्क use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    जइ (!eval_isnamec1(*arg))
	प्रत्यागम arg;

    प्रति (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	जइ (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == रिक्त))
	    भङ्ग;
    प्रत्यागम p;
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

    जइ (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    जइ (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	जइ (eval_list(&p, &rettv, रिक्त, FALSE) == FAIL)
	    p = arg;
    }
    प्रत्यागम p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    स्थिर पूर्णाङ्क
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    पूर्णाङ्क		गणनम् = 0;
    पूर्णाङ्क		is_const;
    पूर्णाङ्क		is_all_const = TRUE;	// reset when non-const encountered
    पूर्णाङ्क		must_end = FALSE;

    प्रति (;;)
    {
	जइ (may_get_next_पङ्क्तिः(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    प्रत्यागम FAIL;
	}
	जइ (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    प्रत्यागम FAIL;
	}
	जइ (*p == ']')
	{
	    ++p;
	    भङ्ग;
	}
	जइ (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    प्रत्यागम FAIL;
	}
	जइ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    प्रत्यागम FAIL;
	जइ (!is_const)
	    is_all_const = FALSE;
	++गणनम्;
	जइ (*p == ',')
	{
	    ++p;
	    जइ (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		प्रत्यागम FAIL;
	    }
	}
	अण्णहा
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    प्रत्यागम generate_NEWLIST(cctx, गणनम्, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    स्थिर पूर्णाङ्क
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    पूर्णाङ्क first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    पूर्णाङ्क		गणनम् = 0;
    पूर्णाङ्क		is_const;
    पूर्णाङ्क		is_all_const = TRUE;	// reset when non-const encountered
    पूर्णाङ्क		must_end = FALSE;

    जइ (**arg != ')')
    {
	जइ (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    प्रत्यागम FAIL;
	}
	गणनम् = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	प्रति (;;)
	{
	    जइ (may_get_next_पङ्क्तिः(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		प्रत्यागम FAIL;
	    }
	    जइ (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		प्रत्यागम FAIL;
	    }
	    जइ (*p == ')')
	    {
		++p;
		भङ्ग;
	    }
	    जइ (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		प्रत्यागम FAIL;
	    }
	    जइ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		प्रत्यागम FAIL;
	    जइ (!is_const)
		is_all_const = FALSE;
	    ++गणनम्;
	    जइ (*p == ',')
	    {
		++p;
		जइ (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    प्रत्यागम FAIL;
		}
	    }
	    अण्णहा
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    प्रत्यागम generate_NEWTUPLE(cctx, गणनम्, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    स्थिर शून्य
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*पङ्क्तिः;
    size_t	off;

    जइ (!evalarg->eval_using_cmdline || cctx == रिक्त)
	प्रत्यागम;

    gap = &evalarg->eval_tofree_ga;
    जइ (gap->ga_len == 0)
	प्रत्यागम;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    पङ्क्तिः = ((char_u **)cctx->ctx_ufunc->uf_पङ्क्तयः.ga_data)[cctx->ctx_lnum];
    *arg = पङ्क्तिः + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    पूर्णाङ्क
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    पूर्णाङ्क		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    जइ (r != OK)
    {
	clear_evalarg(&evalarg, रिक्त);
	प्रत्यागम r;
    }

    जइ (ppconst != रिक्त && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, रिक्त);
	प्रत्यागम FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    जइ (cctx->ctx_ufunc != रिक्त)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	पूर्णाङ्क block_depth = cctx->ctx_ufunc->uf_block_depth;

	जइ (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(पूर्णाङ्क, block_depth);
	    जइ (ufunc->uf_block_ids != रिक्त)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    परिमाण(पूर्णाङ्क) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    जइ (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    जइ (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    जइ (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	जइ (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, रिक्त);

    जइ (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	प्रत्यागम generate_FUNCREF(cctx, ufunc, रिक्त, FALSE, 0, रिक्त);
    }

    func_ptr_unref(ufunc);
    प्रत्यागम FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    पूर्णाङ्क
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	पूर्णाङ्क	    types_optional,
	evalarg_T   *evalarg)
{
    पूर्णाङ्क		r;
    ufunc_T	*ufunc;
    पूर्णाङ्क		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, रिक्त);
    current_sctx.sc_version = save_sc_version;
    जइ (r != OK)
	प्रत्यागम r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    जइ (ufunc->uf_ret_type == रिक्त || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, रिक्त);

    जइ (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	प्रत्यागम OK;
    }
    clear_tv(rettv);
    प्रत्यागम FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    स्थिर पूर्णाङ्क
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    पूर्णाङ्क		गणनम् = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    पूर्णाङ्क		is_const;
    पूर्णाङ्क		is_all_const = TRUE;	// reset when non-const encountered

    जइ (d == रिक्त)
	प्रत्यागम FAIL;
    जइ (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	प्रत्यागम FAIL;
    }
    प्रति (;;)
    {
	char_u	    *कुञ्चिका = रिक्त;

	जइ (may_get_next_पङ्क्तिः(whitep, arg, cctx) == FAIL)
	{
	    *arg = रिक्त;
	    गच्छ failret;
	}

	जइ (**arg == '}')
	    भङ्ग;

	जइ (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    जइ (compile_expr0(arg, cctx) == FAIL)
		गच्छ failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    जइ (isn->isn_type == ISN_PUSHNR)
	    {
		वर्ण buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.सङ्ख्या);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    जइ (isn->isn_type == ISN_PUSHS)
		कुञ्चिका = isn->isn_arg.string;
	    अण्णहा जइ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		गच्छ failret;
	    *arg = skipwhite(*arg);
	    जइ (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_कुञ्चिका));
		गच्छ failret;
	    }
	    ++*arg;
	}
	अण्णहा
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    कुञ्चिका = get_literal_कुञ्चिका(arg);
	    जइ (कुञ्चिका == रिक्त)
		गच्छ failret;
	    जइ (generate_PUSHS(cctx, &कुञ्चिका) == FAIL)
		गच्छ failret;
	}

	// Check for duplicate keys, if using string keys.
	जइ (कुञ्चिका != रिक्त)
	{
	    item = dict_find(d, कुञ्चिका, -1);
	    जइ (item != रिक्त)
	    {
		semsg(_(e_duplicate_कुञ्चिका_in_dictionary_str), कुञ्चिका);
		गच्छ failret;
	    }
	    item = dictitem_alloc(कुञ्चिका);
	    जइ (item != रिक्त)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		जइ (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	जइ (**arg != ':')
	{
	    जइ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    अण्णहा
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    गच्छ failret;
	}
	whitep = *arg + 1;
	जइ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    गच्छ failret;
	}

	जइ (may_get_next_पङ्क्तिः(whitep, arg, cctx) == FAIL)
	{
	    *arg = रिक्त;
	    गच्छ failret;
	}

	जइ (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    गच्छ failret;
	जइ (!is_const)
	    is_all_const = FALSE;
	++गणनम्;

	whitep = *arg;
	जइ (may_get_next_पङ्क्तिः(whitep, arg, cctx) == FAIL)
	{
	    *arg = रिक्त;
	    गच्छ failret;
	}
	जइ (**arg == '}')
	    भङ्ग;
	जइ (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    गच्छ failret;
	}
	जइ (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    गच्छ failret;
	}
	whitep = *arg + 1;
	जइ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    गच्छ failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    जइ (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    प्रत्यागम generate_NEWDICT(cctx, गणनम्, FALSE);

failret:
    जइ (*arg == रिक्त)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    प्रत्यागम FAIL;
}

/*
 * Compile "&option".
 */
    स्थिर पूर्णाङ्क
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    पूर्णाङ्क		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    जइ (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_सङ्ख्या ? &t_सङ्ख्या : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    प्रत्यागम ret;
}

/*
 * Compile "$VAR".
 */
    स्थिर पूर्णाङ्क
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    पूर्णाङ्क		len;
    पूर्णाङ्क		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    जइ (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	प्रत्यागम FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    प्रत्यागम ret;
}

/*
 * Compile $"string" or $'string'.
 */
    स्थिर पूर्णाङ्क
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    पूर्णाङ्क		ret;
    पूर्णाङ्क		quote;
    पूर्णाङ्क		evaluate = cctx->ctx_skip != SKIP_YES;
    पूर्णाङ्क		गणनम् = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    प्रति (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	जइ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	अण्णहा
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	जइ (ret == FAIL)
	    भङ्ग;
	जइ (evaluate)
	{
	    जइ ((tv.vval.v_string != रिक्त && *tv.vval.v_string != NUL)
		    || (**arg != '{' && गणनम् == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		जइ (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    प्रत्यागम FAIL;
		tv.vval.v_string = रिक्त;  // don't free it now
		++गणनम्;
	    }
	    clear_tv(&tv);
	}

	जइ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    भङ्ग;
	}

	p = compile_one_व्यञ्जकम्_in_str(*arg, cctx);
	जइ (p == रिक्त)
	{
	    ret = FAIL;
	    भङ्ग;
	}
	++गणनम्;
	*arg = p;
    }

    जइ (ret == FAIL || !evaluate)
	प्रत्यागम ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    जइ (गणनम् > 1)
	प्रत्यागम generate_CONCAT(cctx, गणनम्);

    प्रत्यागम OK;
}

/*
 * Compile "@r".
 */
    स्थिर पूर्णाङ्क
compile_get_register(char_u **arg, cctx_T *cctx)
{
    पूर्णाङ्क		ret;

    ++*arg;
    जइ (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	प्रत्यागम FAIL;
    }
    जइ (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	प्रत्यागम FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, रिक्त, &t_string);
    ++*arg;
    प्रत्यागम ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    स्थिर पूर्णाङ्क
apply_leader(typval_T *rettv, पूर्णाङ्क numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    जाव (p > start)
    {
	--p;
	जइ (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    जइ (rettv->v_type == VAR_FLOAT)
	    {
		जइ (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    अण्णहा
	    {
		varnumber_T	val;
		पूर्णाङ्क		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		जइ (check_not_string(rettv) == FAIL)
		    प्रत्यागम FAIL;
		val = tv_get_सङ्ख्या_chk(rettv, &error);
		clear_tv(rettv);
		जइ (error)
		    प्रत्यागम FAIL;
		जइ (*p == '-')
		    val = -val;
		rettv->v_type = VAR_सङ्ख्या;
		rettv->vval.v_सङ्ख्या = val;
	    }
	}
	अण्णहा जइ (numeric_only)
	{
	    ++p;
	    भङ्ग;
	}
	अण्णहा जइ (*p == '!')
	{
	    पूर्णाङ्क v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_सङ्ख्या = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    प्रत्यागम OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    स्थिर शून्य
get_vim_constant(char_u **arg, typval_T *rettv)
{
    जइ (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_सङ्ख्या = VVAL_TRUE;
	*arg += 6;
    }
    अण्णहा जइ (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_सङ्ख्या = VVAL_FALSE;
	*arg += 7;
    }
    अण्णहा जइ (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_सङ्ख्या = VVAL_NULL;
	*arg += 6;
    }
    अण्णहा जइ (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_सङ्ख्या = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, पूर्णाङ्क *len, पूर्णाङ्क *type_is)
{
    exprtype_T	type = व्यञ्जकम्_UNKNOWN;
    पूर्णाङ्क		i;

    विकल्प (p[0])
    {
	पक्ष '=':   जइ (p[1] == '=')
			type = व्यञ्जकम्_EQUAL;
		    अण्णहा जइ (p[1] == '~')
			type = व्यञ्जकम्_MATCH;
		    भङ्ग;
	पक्ष '!':   जइ (p[1] == '=')
			type = व्यञ्जकम्_NEQUAL;
		    अण्णहा जइ (p[1] == '~')
			type = व्यञ्जकम्_NOMATCH;
		    भङ्ग;
	पक्ष '>':   जइ (p[1] != '=')
		    {
			type = व्यञ्जकम्_GREATER;
			*len = 1;
		    }
		    अण्णहा
			type = व्यञ्जकम्_GEQUAL;
		    भङ्ग;
	पक्ष '<':   जइ (p[1] != '=')
		    {
			type = व्यञ्जकम्_SMALLER;
			*len = 1;
		    }
		    अण्णहा
			type = व्यञ्जकम्_SEQUAL;
		    भङ्ग;
	पक्ष 'i':   जइ (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			जइ (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			जइ (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? व्यञ्जकम्_IS : व्यञ्जकम्_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    भङ्ग;
    }
    प्रत्यागम type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    शून्य
skip_व्यञ्जकम्_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_व्यञ्जकम्(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, रिक्त);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    पूर्णाङ्क
bool_on_राशिः(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_राशिः(cctx, 0);
    जइ (type == &t_bool)
	प्रत्यागम OK;

    जइ (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_सङ्ख्या
	    || type == &t_सङ्ख्या_bool
	    || type == &t_const_सङ्ख्या_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	प्रत्यागम generate_COND2BOOL(cctx);

    प्रत्यागम need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    शून्य
error_white_both(char_u *op, पूर्णाङ्क len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    स्थिर पूर्णाङ्क
compile_leader(cctx_T *cctx, पूर्णाङ्क numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    जाव (p > start)
    {
	--p;
	जाव (VIM_ISWHITE(*p))
	    --p;
	जइ (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_राशिः(cctx, 0);
	    जइ (type->tt_type != VAR_FLOAT && need_type(type, &t_सङ्ख्या,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    जइ (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == रिक्त)
		प्रत्यागम FAIL;
	}
	अण्णहा जइ (numeric_only)
	{
	    ++p;
	    भङ्ग;
	}
	अण्णहा
	{
	    पूर्णाङ्क  invert = *p == '!';

	    जाव (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		जइ (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    जइ (check_type_is_मानम्(get_type_on_राशिः(cctx, 0)) == FAIL)
		प्रत्यागम FAIL;
	    जइ (generate_2BOOL(cctx, invert, -1) == FAIL)
		प्रत्यागम FAIL;
	}
    }
    *end = p;
    प्रत्यागम OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    स्थिर पूर्णाङ्क
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    पूर्णाङ्क	    ret;
    char_u  *p = *arg + 1;

    जइ (may_get_next_पङ्क्तिः_error(p, arg, cctx) == FAIL)
	प्रत्यागम FAIL;

    जइ (**arg == ')')
	// empty tuple
	प्रत्यागम compile_tuple(arg, cctx, ppconst, FALSE);

    जइ (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    अण्णहा
    {
	// Not enough space in ppconst, flush constants.
	जइ (generate_ppconst(cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;
	ret = compile_expr0(arg, cctx);
    }
    जइ (may_get_next_पङ्क्तिः_error(*arg, arg, cctx) == FAIL)
	प्रत्यागम FAIL;
    जइ (ret == OK && **arg == ',')
    {
	// tuple
	पूर्णाङ्क is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	जइ (generate_ppconst(cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;
	प्रत्यागम compile_tuple(arg, cctx, ppconst, is_const);
    }

    जइ (**arg == ')')
	++*arg;
    अण्णहा जइ (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    प्रत्यागम ret;
}

स्थिर पूर्णाङ्क compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    स्थिर पूर्णाङ्क
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    पूर्णाङ्क		keeping_dict = FALSE;

    प्रति (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	जइ (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_पङ्क्तिः_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    जइ (next != रिक्त &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_पङ्क्तिः_from_context(cctx, TRUE);
		जइ (next == रिक्त)
		    प्रत्यागम FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	जइ (**arg == '(')
	{
	    पूर्णाङ्क		argcount = 0;

	    जइ (generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_राशिः(cctx, 0);

	    *arg = skipwhite(p + 1);
	    जइ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		प्रत्यागम FAIL;
	    जइ (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		प्रत्यागम FAIL;
	    जइ (keeping_dict)
	    {
		keeping_dict = FALSE;
		जइ (generate_instr(cctx, ISN_CLEARDICT) == रिक्त)
		    प्रत्यागम FAIL;
	    }
	}
	अण्णहा जइ (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    पूर्णाङ्क	    alt;
	    char_u  *paren;

	    // something->method()
	    जइ (generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_राशिः(cctx, 0);
	    जइ (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_मानम्));
		प्रत्यागम FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    जइ (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		प्रत्यागम FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    जइ (**arg == '(')
		alt = 2;
	    अण्णहा
	    {
		// alternative 1 or 3
		p = *arg;
		जइ (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    प्रत्यागम FAIL;
		}
		जइ (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		प्रति ( ; eval_isnamec(*p); ++p)
		    ;
		जइ (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    जइ (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			प्रत्यागम FAIL;
		}
		अण्णहा
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    जइ (paren == रिक्त)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			प्रत्यागम FAIL;
		    }
		}
	    }

	    जइ (alt != 1)
	    {
		पूर्णाङ्क	    argcount = 1;
		garray_T    *राशिः = &cctx->ctx_type_राशिः;
		पूर्णाङ्क	    type_idx_start = राशिः->ga_len;
		पूर्णाङ्क	    व्यञ्जकम्_isn_start = cctx->ctx_instr.ga_len;
		पूर्णाङ्क	    व्यञ्जकम्_isn_end;
		पूर्णाङ्क	    arg_isn_गणनम्;

		जइ (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    जइ (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			प्रत्यागम FAIL;
		}
		अण्णहा
		{
		    पूर्णाङ्क fail;
		    पूर्णाङ्क save_len = cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len;
		    पूर्णाङ्क	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_व्यञ्जकम्;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_व्यञ्जकम्;
		    cctx->ctx_ufunc->uf_पङ्क्तयः.ga_len = save_len;

		    जइ (fail)
		    {
			जइ (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			प्रत्यागम FAIL;
		    }
		}

		// Compile the arguments.
		जइ (**arg != '(')
		{
		    जइ (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    अण्णहा
			semsg(_(e_missing_parenthesis_str), *arg);
		    प्रत्यागम FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		व्यञ्जकम्_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		जइ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    प्रत्यागम FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_गणनम् = cctx->ctx_instr.ga_len - व्यञ्जकम्_isn_end;
		जइ (arg_isn_गणनम् > 0)
		{
		    पूर्णाङ्क	    व्यञ्जकम्_isn_गणनम् = व्यञ्जकम्_isn_end - व्यञ्जकम्_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, व्यञ्जकम्_isn_गणनम्);
		    type_T  *decl_type;
		    type2_T  *typep;

		    जइ (isn == रिक्त)
			प्रत्यागम FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + व्यञ्जकम्_isn_start,
					       परिमाण(isn_T) * व्यञ्जकम्_isn_गणनम्);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + व्यञ्जकम्_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + व्यञ्जकम्_isn_end,
						परिमाण(isn_T) * arg_isn_गणनम्);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + व्यञ्जकम्_isn_start + arg_isn_गणनम्,
					  isn, परिमाण(isn_T) * व्यञ्जकम्_isn_गणनम्);
		    vim_free(isn);

		    typep = ((type2_T *)राशिः->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)राशिः->ga_data) + type_idx_start,
			      ((type2_T *)राशिः->ga_data) + type_idx_start + 1,
			      परिमाण(type2_T)
				       * (राशिः->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)राशिः->ga_data) + राशिः->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_राशिः(cctx, 0);
		जइ (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    प्रत्यागम FAIL;
	    }

	    जइ (keeping_dict)
	    {
		keeping_dict = FALSE;
		जइ (generate_instr(cctx, ISN_CLEARDICT) == रिक्त)
		    प्रत्यागम FAIL;
	    }
	}
	अण्णहा जइ (**arg == '[')
	{
	    पूर्णाङ्क		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    जइ (generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    जइ (may_get_next_पङ्क्तिः_error(p, arg, cctx) == FAIL)
		प्रत्यागम FAIL;
	    जइ (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    अण्णहा
	    {
		जइ (compile_expr0(arg, cctx) == FAIL)
		    प्रत्यागम FAIL;
		जइ (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    प्रत्यागम FAIL;
		}
		जइ (may_get_next_पङ्क्तिः_error(*arg, arg, cctx) == FAIL)
		    प्रत्यागम FAIL;
		*arg = skipwhite(*arg);
	    }
	    जइ (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		जइ (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    प्रत्यागम FAIL;
		}
		जइ (may_get_next_पङ्क्तिः_error(*arg, arg, cctx) == FAIL)
		    प्रत्यागम FAIL;
		जइ (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		अण्णहा
		{
		    जइ (compile_expr0(arg, cctx) == FAIL)
			प्रत्यागम FAIL;
		    जइ (may_get_next_पङ्क्तिः_error(*arg, arg, cctx) == FAIL)
			प्रत्यागम FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    जइ (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		प्रत्यागम FAIL;
	    }
	    *arg = *arg + 1;

	    जइ (keeping_dict)
	    {
		keeping_dict = FALSE;
		जइ (generate_instr(cctx, ISN_CLEARDICT) == रिक्त)
		    प्रत्यागम FAIL;
	    }
	    जइ (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		प्रत्यागम FAIL;
	}
	अण्णहा जइ (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    जइ (generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_राशिः(cctx, 0);
	    जइ (type != &t_unknown
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
		जइ (compile_class_object_index(cctx, arg, type) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अण्णहा
	    {
		*arg = p + 1;
		जइ (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    प्रत्यागम FAIL;
		}
		p = *arg;
		जाव (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		जइ (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    प्रत्यागम FAIL;
		}
		जइ (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == रिक्त)
		    प्रत्यागम FAIL;
		जइ (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    प्रत्यागम FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	अण्णहा
	    भङ्ग;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    जइ (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == रिक्त)
	प्रत्यागम FAIL;

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    पूर्णाङ्क		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    पूर्णाङ्क		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    जइ (eval_leader(arg, TRUE) == FAIL)
	प्रत्यागम FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    विकल्प (**arg)
    {
	/*
	 * Number constant.
	 */
	पक्ष '0':	// also for blob starting with 0z
	पक्ष '1':
	पक्ष '2':
	पक्ष '3':
	पक्ष '4':
	पक्ष '5':
	पक्ष '6':
	पक्ष '7':
	पक्ष '8':
	पक्ष '9':
	पक्ष '.':   जइ (eval_सङ्ख्या(arg, rettv, TRUE, FALSE) == FAIL)
			प्रत्यागम FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    जइ (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			प्रत्यागम FAIL;
		    }
		    भङ्ग;

	/*
	 * String constant: "string".
	 */
	पक्ष '"':   जइ (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			प्रत्यागम FAIL;
		    भङ्ग;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	पक्ष '\'':  जइ (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			प्रत्यागम FAIL;
		    भङ्ग;

	/*
	 * Constant Vim variable.
	 */
	पक्ष 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    भङ्ग;

	/*
	 * "true" constant
	 */
	पक्ष 't':   जइ (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_सङ्ख्या = VVAL_TRUE;
		    }
		    अण्णहा
			ret = NOTDONE;
		    भङ्ग;

	/*
	 * "false" constant
	 */
	पक्ष 'f':   जइ (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_सङ्ख्या = VVAL_FALSE;
		    }
		    अण्णहा
			ret = NOTDONE;
		    भङ्ग;

	/*
	 * "null" or "null_*" constant
	 */
	पक्ष 'n':   जइ (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			पूर्णाङ्क	len;

			प्रति (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			जइ (ret == FAIL)
			    ret = NOTDONE;
			अण्णहा
			    *arg += len + 4;
		    }
		    अण्णहा
			ret = NOTDONE;
		    भङ्ग;

	/*
	 * List: [expr, expr]
	 */
	पक्ष '[':   जइ (generate_ppconst(cctx, ppconst) == FAIL)
			प्रत्यागम FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    भङ्ग;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	पक्ष '{':   जइ (generate_ppconst(cctx, ppconst) == FAIL)
			प्रत्यागम FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    भङ्ग;

	/*
	 * Option value: &name
	 */
	पक्ष '&':	जइ (generate_ppconst(cctx, ppconst) == FAIL)
			    प्रत्यागम FAIL;
			ret = compile_get_option(arg, cctx);
			भङ्ग;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	पक्ष '$':	जइ (generate_ppconst(cctx, ppconst) == FAIL)
			    प्रत्यागम FAIL;
			जइ ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			अण्णहा
			    ret = compile_get_env(arg, cctx);
			भङ्ग;

	/*
	 * Register contents: @r.
	 */
	पक्ष '@':	जइ (generate_ppconst(cctx, ppconst) == FAIL)
			    प्रत्यागम FAIL;
			ret = compile_get_register(arg, cctx);
			भङ्ग;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	पक्ष '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    जइ (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    भङ्ग;

	स्वभाव:    ret = NOTDONE;
		    भङ्ग;
    }
    जइ (ret == FAIL)
	प्रत्यागम FAIL;

    जइ (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	जइ (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	अण्णहा
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    अण्णहा जइ (ret == NOTDONE)
    {
	char_u	    *p;
	पूर्णाङ्क	    r;

	जइ (!eval_isnamec1(**arg))
	{
	    जइ (!vim9_bad_comment(*arg))
	    {
		जइ (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		अण्णहा
		    semsg(_(e_name_expected_str), *arg);
	    }
	    प्रत्यागम FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	जइ (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    प्रत्यागम FAIL;
	}

	size_t namelen = p - *arg;
	जइ (*p == '<')
	{
	    जइ (skip_generic_func_type_args(&p) == FAIL)
		प्रत्यागम FAIL;
	}

	जइ (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	अण्णहा
	{
	    जइ (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	जइ (r == FAIL)
	    प्रत्यागम FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    जइ (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	प्रत्यागम FAIL;
    जइ ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	जइ (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    प्रत्यागम FAIL;
	प्रत्यागम OK;
    }
    जइ (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    स्थिर पूर्णाङ्क
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = रिक्त;

    // Recognize <type>
    जइ (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	जइ (want_type == रिक्त)
	    प्रत्यागम FAIL;

	जइ (**arg != '>')
	{
	    जइ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    अण्णहा
		emsg(_(e_missing_gt));
	    प्रत्यागम FAIL;
	}
	++*arg;
	जइ (may_get_next_पङ्क्तिः_error(*arg, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;
    }

    जइ (compile_expr9(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    जइ (want_type != रिक्त)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_राशिः(cctx, 0);
	जइ (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    जइ (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		प्रत्यागम FAIL;
	}
    }

    प्रत्यागम OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    स्थिर पूर्णाङ्क
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    पूर्णाङ्क		ppconst_used = ppconst->pp_used;

    // get the first expression
    जइ (compile_expr8(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    प्रति (;;)
    {
	op = may_peek_next_पङ्क्तिः(cctx, *arg, &next);
	जइ (*op != '*' && *op != '/' && *op != '%')
	    भङ्ग;
	जइ (next != रिक्त)
	{
	    *arg = next_पङ्क्तिः_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	जइ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    प्रत्यागम FAIL;
	}
	जइ (may_get_next_पङ्क्तिः_error(op + 1, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;

	// get the second expression
	जइ (compile_expr8(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	जइ (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_सङ्ख्या
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_सङ्ख्या)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    पूर्णाङ्क		    failed = FALSE;

	    // both are numbers: compute the result
	    विकल्प (*op)
	    {
		पक्ष '*': res = tv1->vval.v_सङ्ख्या * tv2->vval.v_सङ्ख्या;
			  भङ्ग;
		पक्ष '/': res = अङ्कः_divide(tv1->vval.v_सङ्ख्या,
						  tv2->vval.v_सङ्ख्या, &failed);
			  भङ्ग;
		पक्ष '%': res = अङ्कः_modulus(tv1->vval.v_सङ्ख्या,
						  tv2->vval.v_सङ्ख्या, &failed);
			  भङ्ग;
	    }
	    जइ (failed)
		प्रत्यागम FAIL;
	    tv1->vval.v_सङ्ख्या = res;
	    --ppconst->pp_used;
	}
	अण्णहा
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    प्रत्यागम OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    स्थिर पूर्णाङ्क
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    पूर्णाङ्क		oplen;
    पूर्णाङ्क		ppconst_used = ppconst->pp_used;

    // get the first variable
    जइ (compile_expr7(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    प्रति (;;)
    {
	op = may_peek_next_पङ्क्तिः(cctx, *arg, &next);
	जइ (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    भङ्ग;
	जइ (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    भङ्ग;
	oplen = (*op == '.' ? 2 : 1);
	जइ (next != रिक्त)
	{
	    *arg = next_पङ्क्तिः_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	जइ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    प्रत्यागम FAIL;
	}

	जइ (may_get_next_पङ्क्तिः_error(op + oplen, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;

	// get the second expression
	जइ (compile_expr7(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	जइ (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_सङ्ख्या
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_सङ्ख्या)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    जइ (*op == '+')
		tv1->vval.v_सङ्ख्या = tv1->vval.v_सङ्ख्या + tv2->vval.v_सङ्ख्या;
	    अण्णहा जइ (*op == '-')
		tv1->vval.v_सङ्ख्या = tv1->vval.v_सङ्ख्या - tv2->vval.v_सङ्ख्या;
	    अण्णहा
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		जइ (tv1->vval.v_string == रिक्त)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    प्रत्यागम FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	अण्णहा
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    जइ (*op == '.')
	    {
		जइ (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    प्रत्यागम FAIL;
		जइ (generate_CONCAT(cctx, 2) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अण्णहा
		generate_two_op(cctx, op);
	}
    }

    प्रत्यागम OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    स्थिर पूर्णाङ्क
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = व्यञ्जकम्_UNKNOWN;
    char_u	*p;
    char_u	*next;
    पूर्णाङ्क		len = 2;
    पूर्णाङ्क		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    जइ (compile_expr6(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    प्रति (;;)
    {
	type = व्यञ्जकम्_UNKNOWN;

	p = may_peek_next_पङ्क्तिः(cctx, *arg, &next);
	जइ (p[0] == '<' && p[1] == '<')
	    type = व्यञ्जकम्_LSHIFT;
	अण्णहा जइ (p[0] == '>' && p[1] == '>')
	    type = व्यञ्जकम्_RSHIFT;

	जइ (type == व्यञ्जकम्_UNKNOWN)
	    प्रत्यागम OK;

	// Handle a bitwise left or right shift operator
	जइ (ppconst->pp_used == ppconst_used + 1)
	{
	    जइ (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_सङ्ख्या)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_सङ्ख्या));
		प्रत्यागम FAIL;
	    }
	}
	अण्णहा
	{
	    type_T	*t = get_type_on_राशिः(cctx, 0);

	    जइ (need_type(t, &t_सङ्ख्या, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_सङ्ख्या));
		प्रत्यागम FAIL;
	    }
	}

	जइ (next != रिक्त)
	{
	    *arg = next_पङ्क्तिः_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	जइ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    प्रत्यागम FAIL;
	}

	// get the second variable
	जइ (may_get_next_पङ्क्तिः_error(p + len, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;

	जइ (compile_expr6(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	जइ (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    जइ (tv2->v_type != VAR_सङ्ख्या || tv2->vval.v_सङ्ख्या < 0)
	    {
		// right operand should be a positive number
		जइ (tv2->v_type != VAR_सङ्ख्या)
		    emsg(_(e_bitshift_ops_must_be_सङ्ख्या));
		अण्णहा
		    emsg(_(e_bitshift_ops_must_be_positive));
		प्रत्यागम FAIL;
	    }

	    जइ (tv2->vval.v_सङ्ख्या > MAX_LSHIFT_BITS)
		tv1->vval.v_सङ्ख्या = 0;
	    अण्णहा जइ (type == व्यञ्जकम्_LSHIFT)
		tv1->vval.v_सङ्ख्या =
			(uvarnumber_T)tv1->vval.v_सङ्ख्या << tv2->vval.v_सङ्ख्या;
	    अण्णहा
		tv1->vval.v_सङ्ख्या =
			(uvarnumber_T)tv1->vval.v_सङ्ख्या >> tv2->vval.v_सङ्ख्या;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	अण्णहा
	{
	    जइ (need_type(get_type_on_राशिः(cctx, 0), &t_सङ्ख्या, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_सङ्ख्या));
		प्रत्यागम FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    जइ (isn == रिक्त)
		प्रत्यागम FAIL;

	    जइ (isn != रिक्त)
		isn->isn_arg.op.op_type = type;
	}
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = व्यञ्जकम्_UNKNOWN;
    char_u	*p;
    char_u	*next;
    पूर्णाङ्क		len = 2;
    पूर्णाङ्क		type_is = FALSE;
    पूर्णाङ्क		ppconst_used = ppconst->pp_used;

    // get the first variable
    जइ (compile_expr5(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    p = may_peek_next_पङ्क्तिः(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    जइ (type != व्यञ्जकम्_UNKNOWN)
    {
	पूर्णाङ्क ic = FALSE;  // Default: do not ignore case

	जइ (next != रिक्त)
	{
	    *arg = next_पङ्क्तिः_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	जइ (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    प्रत्यागम FAIL;
	}
	// extra question mark appended: ignore case
	जइ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	अण्णहा जइ (p[len] == '#')
	    ++len;
	// nothing appended: match case

	जइ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    प्रत्यागम FAIL;
	}

	// get the second variable
	जइ (may_get_next_पङ्क्तिः_error(p + len, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;

	जइ (compile_expr5(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	जइ (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    पूर्णाङ्क		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    जइ (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    अण्णहा
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_सङ्ख्या = tv1->vval.v_सङ्ख्या
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    प्रत्यागम ret;
	}

	generate_ppconst(cctx, ppconst);
	प्रत्यागम generate_COMPARE(cctx, type, ic);
    }

    प्रत्यागम OK;
}

स्थिर पूर्णाङ्क compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    स्थिर पूर्णाङ्क
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	वर्ण	*op,
	ppconst_T *ppconst,
	पूर्णाङ्क	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_पङ्क्तिः(cctx, *arg, &next);
    पूर्णाङ्क		opchar = *op;

    जइ (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	पूर्णाङ्क		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, परिमाण(पूर्णाङ्क), 10);
	जाव (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    पूर्णाङ्क		start_ctx_lnum = cctx->ctx_lnum;
	    पूर्णाङ्क		save_lnum;
	    पूर्णाङ्क		const_used;
	    पूर्णाङ्क		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    जइ (next != रिक्त)
	    {
		*arg = next_पङ्क्तिः_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    जइ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		प्रत्यागम FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    जइ (status != FAIL)
	    {
		// Use the last ppconst if possible.
		जइ (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    पूर्णाङ्क		is_true = tv2bool(tv);

		    जइ ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_सङ्ख्या = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    अण्णहा
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		अण्णहा
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_राशिः(cctx);
		}
	    }
	    जइ (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    जइ (status == FAIL)
	    {
		ga_clear(&end_ga);
		प्रत्यागम FAIL;
	    }

	    जइ (jump_when != JUMP_NEVER)
	    {
		जइ (cctx->ctx_skip != SKIP_YES)
		{
		    *(((पूर्णाङ्क *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    जइ (may_get_next_पङ्क्तिः_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		प्रत्यागम FAIL;
	    }

	    const_used = ppconst->pp_used;
	    जइ ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		प्रत्यागम FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    जइ (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		जइ (tv->v_type == VAR_सङ्ख्या
			 && (tv->vval.v_सङ्ख्या == 1 || tv->vval.v_सङ्ख्या == 0))
		{
		    tv->vval.v_सङ्ख्या = tv->vval.v_सङ्ख्या == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_पङ्क्तिः(cctx, *arg, &next);
	}

	जइ (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    प्रत्यागम FAIL;
	}

	जइ (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    जइ (bool_on_राशिः(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		प्रत्यागम FAIL;
	    }

	जइ (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    जाव (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((पूर्णाङ्क *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    प्रत्यागम OK;
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
    स्थिर पूर्णाङ्क
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    पूर्णाङ्क		ppconst_used = ppconst->pp_used;

    // get the first variable
    जइ (compile_expr4(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    // || and && work almost the same
    प्रत्यागम compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    स्थिर पूर्णाङ्क
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    पूर्णाङ्क		ppconst_used = ppconst->pp_used;

    // eval the first expression
    जइ (compile_expr3(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    // || and && work almost the same
    प्रत्यागम compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    पूर्णाङ्क
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    पूर्णाङ्क		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    जइ (cctx->ctx_skip == SKIP_YES)
    {
	पूर्णाङ्क		prev_did_emsg = did_emsg;

	skip_व्यञ्जकम्_cctx(arg, cctx);
	प्रत्यागम did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    जइ (compile_expr2(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    p = may_peek_next_पङ्क्तिः(cctx, *arg, &next);
    जइ (*p == '?')
    {
	पूर्णाङ्क		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*राशिः = &cctx->ctx_type_राशिः;
	पूर्णाङ्क		alt_idx = instr->ga_len;
	पूर्णाङ्क		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = रिक्त;
	पूर्णाङ्क		has_const_व्यञ्जकम् = FALSE;
	पूर्णाङ्क		const_मानम् = FALSE;
	पूर्णाङ्क		save_skip = cctx->ctx_skip;

	जइ (next != रिक्त)
	{
	    *arg = next_पङ्क्तिः_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	जइ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    प्रत्यागम FAIL;
	}

	जइ (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_व्यञ्जकम् = TRUE;
	    जइ (op_falsy)
		const_मानम् = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    अण्णहा
	    {
		पूर्णाङ्क error = FALSE;

		const_मानम् = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		जइ (error)
		    प्रत्यागम FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_मानम् : !const_मानम्) ? SKIP_YES : SKIP_NOT;

	    जइ (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    अण्णहा
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	अण्णहा
	{
	    generate_ppconst(cctx, ppconst);
	    जइ (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    जइ (op_falsy)
	    {
		type1 = get_type_on_राशिः(cctx, -1);
		जइ (check_type_is_मानम्(type1) == FAIL)
		    प्रत्यागम FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	जइ (may_get_next_पङ्क्तिः_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;
	जइ (compile_expr1(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	जइ (!has_const_व्यञ्जकम्)
	{
	    generate_ppconst(cctx, ppconst);

	    जइ (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_राशिः(cctx, 0);
		--राशिः->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	जइ (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_पङ्क्तिः(cctx, *arg, &next);
	    जइ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		प्रत्यागम FAIL;
	    }
	    जइ (next != रिक्त)
	    {
		*arg = next_पङ्क्तिः_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    जइ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		प्रत्यागम FAIL;
	    }

	    // evaluate the third expression
	    जइ (has_const_व्यञ्जकम्)
		cctx->ctx_skip = save_skip == SKIP_YES || const_मानम्
							 ? SKIP_YES : SKIP_NOT;
	    जइ (may_get_next_पङ्क्तिः_error(p + 1, arg, cctx) == FAIL)
		प्रत्यागम FAIL;
	    जइ (compile_expr1(arg, cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	}

	जइ (!has_const_व्यञ्जकम्)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)राशिः->ga_data)
					      + राशिः->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    प्रत्यागम OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
compile_expr0_ext(char_u **arg,  cctx_T *cctx, पूर्णाङ्क *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    जइ (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	प्रत्यागम FAIL;
    }
    जइ (is_const != रिक्त)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    जइ (generate_ppconst(cctx, &ppconst) == FAIL)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Toplevel expression.
 */
    पूर्णाङ्क
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    प्रत्यागम compile_expr0_ext(arg, cctx, रिक्त);
}


#endif // defined(FEAT_EVAL)
