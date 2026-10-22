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

#यदि defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
स्थिर पूर्णाङ्क paren_follows_after_अभिव्यक्ती = 0;

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
	यदि (generate_tv_ढकला(cctx, &ppconst->pp_tv[i]) == FAIL)
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
    यदि (ppconst->pp_used > 0)
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
    यदि (is_slice)
    {
	यदि (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    प्रत्यागम FAIL;
	// a copy is made so the member type is no longer declared
	यदि (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	यदि (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    यदि (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    अन्यथा
    {
	यदि (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    यदि (typep->type_curr->tt_argcount == 1)
	    {
		यदि (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		अन्यथा
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    अन्यथा
		typep->type_curr = &t_any;
	    यदि (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		यदि (typep->type_decl->tt_argcount == 1)
		{
		    यदि (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    अन्यथा
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		अन्यथा
		    typep->type_curr = &t_any;
	    }
	    अन्यथा
		typep->type_decl = typep->type_curr;
	}
	यदि (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
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
    garray_T	*चळत = &cctx->ctx_type_चळत;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)चळत->ga_data) + चळत->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)चळत->ga_data) + चळत->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    यदि ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    यदि (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	यदि (need_type(idxtype, &t_संख्या, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    प्रत्यागम FAIL;
	यदि (is_slice)
	{
	    idxtype = get_type_on_चळत(cctx, 1);
	    यदि (need_type(idxtype, &t_संख्या, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;
	}
    }

    यदि (vartype == VAR_DICT)
    {
	यदि (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    प्रत्यागम FAIL;
	}
	यदि (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    यदि (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    यदि (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		यदि (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    अन्यथा
		typep->type_decl = typep->type_curr;
	}
	अन्यथा
	{
	    यदि (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	यदि (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    प्रत्यागम FAIL;
	यदि (keeping_dict != रिक्त)
	    *keeping_dict = TRUE;
    }
    अन्यथा यदि (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	यदि ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    प्रत्यागम FAIL;
    }
    अन्यथा यदि (vartype == VAR_BLOB)
    {
	यदि (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    यदि (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		प्रत्यागम FAIL;
	}
	अन्यथा
	{
	    typep->type_curr = &t_संख्या;
	    typep->type_decl = &t_संख्या;
	    यदि (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		प्रत्यागम FAIL;
	}
    }
    अन्यथा यदि (vartype == VAR_TUPLE)
    {
	यदि (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    प्रत्यागम FAIL;
    }
    अन्यथा यदि (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	यदि (is_slice)
	{
	    यदि (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		प्रत्यागम FAIL;
	    // a copy is made so the member type is no longer declared
	    यदि (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    यदि (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		यदि (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	अन्यथा
	{
	    यदि (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		यदि (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		यदि (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    यदि (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		अन्यथा
			typep->type_decl = typep->type_curr;
	    }
	    यदि (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		प्रत्यागम FAIL;
	}
    }
    अन्यथा
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
	यदि (cctx->ctx_ufunc != रिक्त && cctx->ctx_ufunc->uf_class != रिक्त)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    यावत् (clp != रिक्त)
	    {
		यदि (clp == cl)
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

    यदि (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	प्रत्यागम FAIL;
    }

    class_T *cl = type->tt_class;
    पूर्णाङ्क is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    यदि (type == &t_super)
    {
	यदि (cctx->ctx_ufunc == रिक्त || cctx->ctx_ufunc->uf_class == रिक्त)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    प्रत्यागम FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_चळत.ga_len;
    }
    अन्यथा यदि (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	यदि (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    यदि (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    यदि (cl == रिक्त)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	प्रत्यागम FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, रिक्त, रिक्त, FNE_CHECK_START);
    यदि (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	प्रत्यागम FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    यदि (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	यदि (name_end == रिक्त)
	    गच्छ done;
    }

    यदि (*name_end == '(')
    {
	पूर्णाङ्क	function_मोजणी;
	पूर्णाङ्क	child_मोजणी;
	ufunc_T	**functions;

	यदि (type->tt_type == VAR_CLASS)
	{
	    function_मोजणी = cl->class_class_function_मोजणी;
	    child_मोजणी = cl->class_class_function_मोजणी_child;
	    functions = cl->class_class_functions;
	}
	अन्यथा
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    यदि (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_मोजणी = cl->class_obj_method_मोजणी;
	    child_मोजणी = cl->class_obj_method_मोजणी_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = रिक्त;
	पूर्णाङ्क fi;
	प्रति (fi = is_super ? child_मोजणी : 0; fi < function_मोजणी; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    यदि (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		भङ्ग;
	    }
	}

	ocmember_T  *ocm = रिक्त;
	यदि (ufunc == रिक्त)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    यदि (ocm == रिक्त || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_संदेश(cl, type->tt_type, name, len);
		गच्छ done;
	    }
	    यदि (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_चळत.ga_len;
		यदि (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    गच्छ done;
	    }
	    अन्यथा
	    {
		पूर्णाङ्क status;

		यदि (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		अन्यथा
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		यदि (status == FAIL)
		    गच्छ done;
	    }
	}

	यदि (is_super && ufunc != रिक्त && IS_ABSTRACT_METHOD(ufunc))
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
	यदि (ocm == रिक्त && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    गच्छ done;
	}

	// process generic function call
	यदि (ufunc != रिक्त)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    यदि (ufunc == रिक्त)
		गच्छ done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	पूर्णाङ्क argcount = 0;
	यदि (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    गच्छ done;

	यदि (ocm != रिक्त)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    गच्छ done;
	}
	यदि (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	अन्यथा
	    ret = generate_CALL(cctx, ufunc, रिक्त, 0, argcount, FALSE);
	गच्छ done;
    }

    यदि (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	यदि (m_idx >= 0)
	{
	    यदि (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		गच्छ done;
	    }

	    *arg = name_end;
	    यदि (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    अन्यथा
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    गच्छ done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	यदि (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    यदि (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		गच्छ done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_चळत.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, रिक्त);
	    गच्छ done;
	}

	member_not_found_संदेश(cl, VAR_OBJECT, name, len);
    }
    अन्यथा
    {
	// load class member
	पूर्णाङ्क idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	यदि (m != रिक्त)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    यदि (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		गच्छ done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_चळत.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    गच्छ done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	यदि (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    यदि (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		गच्छ done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_चळत.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, रिक्त);
	    गच्छ done;
	}

	member_not_found_संदेश(cl, VAR_CLASS, name, len);
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

    यदि (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	प्रत्यागम FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, रिक्त);
    यदि (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	प्रत्यागम OK;
    }

    यदि (end == रिक्त)
	imp = रिक्त;
    अन्यथा यदि (import == रिक्त)
	imp = find_imported(name, 0, FALSE);
    अन्यथा
	imp = import;

    यदि (imp != रिक्त)
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
	यदि (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    प्रत्यागम FAIL;
	}
	++p;
	यदि (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    प्रत्यागम FAIL;
	}

	// isolate one name
	exp_name = p;
	यावत् (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	यदि (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	यदि (res == OK)
	{
	    यदि (si->sn_autoload_prefix != रिक्त
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		यदि (cc == '(' || paren_follows_after_अभिव्यक्ती)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		अन्यथा
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    अन्यथा यदि (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		यदि (cc == '(' || paren_follows_after_अभिव्यक्ती)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		अन्यथा
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    अन्यथा
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, रिक्त, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	यदि (done)
	    प्रत्यागम res;

	यदि (idx < 0)
	{
	    यदि (ufunc != रिक्त)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		यदि (IS_GENERIC_FUNC(ufunc))
		{
		    यदि (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			यदि (p != रिक्त)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			यदि (p == रिक्त || ufunc == रिक्त)
			    प्रत्यागम FAIL;
		    }
		    अन्यथा
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			प्रत्यागम FAIL;
		    }
		}
		अन्यथा यदि (*p == '<')
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
    यदि (ufunc == रिक्त || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	प्रत्यागम FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    यदि (ufunc == रिक्त)
	प्रत्यागम FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    यदि (func_needs_compiling(ufunc, compile_type)
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

    यदि (fp == रिक्त)
	प्रत्यागम FAIL;

    प्रति (पूर्णाङ्क depth = 0; depth < outer_depth; ++depth)
    {
	यदि (outer_cctx->ctx_outer == रिक्त)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    प्रत्यागम FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    यदि (fp->uf_block_ids == रिक्त && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(पूर्णाङ्क, ufunc->uf_block_depth);
	यदि (fp->uf_block_ids != रिक्त)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    परिमाण(पूर्णाङ्क) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    यदि (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	प्रत्यागम FAIL;

    यदि (generate_FUNCREF(cctx, fp, रिक्त, FALSE, 0, &isn_idx) == FAIL)
	प्रत्यागम FAIL;
    यदि (cctx->ctx_skip == SKIP_YES)
	प्रत्यागम OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    यदि (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	यदि (extra == रिक्त)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    यदि (extra == रिक्त)
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
	पूर्णाङ्क	is_अभिव्यक्ती,
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

    यदि (*(*arg + namelen) == '<')
    {
	// generic function call
	यदि (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == रिक्त)
	{
	    generic_func_args_table_clear(&gfatab);
	    प्रत्यागम FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    यदि (*(*arg + 1) == ':')
    {
	यदि (end <= *arg + 2)
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
	    यदि (generate_instr_type(cctx, isn_type, &t_dict_any) == रिक्त)
		गच्छ theend;
	    res = OK;
	}
	अन्यथा
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    यदि (name == रिक्त)
		गच्छ theend;

	    विकल्प (**arg)
	    {
		पक्ष 'v': res = generate_LOADV(cctx, name);
			  भङ्ग;
		पक्ष 's': यदि (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      गच्छ theend;
			  }
			  यदि (is_अभिव्यक्ती && find_func(name, FALSE) != रिक्त)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  अन्यथा
			      res = compile_load_scriptvar(cctx, name,
							    रिक्त, &end, रिक्त);
			  भङ्ग;
		पक्ष 'g': यदि (vim_strchr(name, AUTOLOAD_CHAR) == रिक्त)
			  {
			      यदि (is_अभिव्यक्ती && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != रिक्त
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      अन्यथा
				  isn_type = ISN_LOADG;
			  }
			  अन्यथा
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      यदि (name == रिक्त)
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
	    यदि (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    अन्यथा
    {
	पूर्णाङ्क	    idx;
	पूर्णाङ्क	    method_idx;
	पूर्णाङ्क	    gen_load = FALSE;
	पूर्णाङ्क	    gen_load_outer = 0;
	पूर्णाङ्क	    outer_loop_depth = -1;
	पूर्णाङ्क	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	यदि (name == रिक्त)
	    गच्छ theend;

	यदि (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = ढकला_type_चळत(cctx, &t_super);
	    यदि (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	अन्यथा यदि (vim_strchr(name, AUTOLOAD_CHAR) != रिक्त)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	अन्यथा यदि (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    यदि (gen_load_outer == 0)
		gen_load = TRUE;
	}
	अन्यथा
	{
	    lvar_T  lvar;
	    class_T *cl = रिक्त;

	    यदि (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		यदि (lvar.lv_ufunc != रिक्त && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		अन्यथा यदि (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		अन्यथा
		    gen_load = TRUE;
	    }
	    अन्यथा यदि (cctx->ctx_ufunc->uf_defclass != रिक्त &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		यदि (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    यदि (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    अन्यथा
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									रिक्त);
		    }
		}
		अन्यथा
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    अन्यथा
	    {
		imported_T *imp = रिक्त;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		यदि (script_var_exists(*arg, namelen, cctx, रिक्त) == OK
			    || (imp = find_imported(name, 0, FALSE)) != रिक्त)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		यदि (res == FAIL && is_अभिव्यक्ती && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	यदि (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, रिक्त, type);
	यदि (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    यदि (lt != रिक्त)
	*lt = '<';
    यदि (res == FAIL && error && called_emsg == prev_called_emsg)
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
    पूर्णाङ्क		अभिव्यक्ती_res;
    पूर्णाङ्क		trailing_error;
    पूर्णाङ्क		instr_मोजणी;
    isn_T	*instr = रिक्त;

    // Remove the string type from the stack.
    --cctx->ctx_type_चळत.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = रिक्त;

    // avoid peeking a next line
    पूर्णाङ्क galen_save = cctx->ctx_ufunc->uf_ओळी.ga_len;
    cctx->ctx_ufunc->uf_ओळी.ga_len = 0;

    अभिव्यक्ती_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_ओळी.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    यदि (अभिव्यक्ती_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	यदि (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_चळत.ga_len;
	प्रत्यागम FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_मोजणी = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_मोजणी].isn_type = ISN_FINISH;

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
    पूर्णाङ्क	    instr_मोजणी;

    प्रति (;;)
    {
	यदि (may_get_next_ओळ(whitep, &p, cctx) == FAIL)
	    गच्छ failret;
	यदि (*p == ')')
	{
	    *arg = p + 1;
	    प्रत्यागम OK;
	}
	यदि (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    प्रत्यागम FAIL;
	}

	instr_मोजणी = cctx->ctx_instr.ga_len;
	यदि (compile_expr0(&p, cctx) == FAIL)
	    प्रत्यागम FAIL;
	++*argcount;

	यदि (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_मोजणी + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_मोजणी;

	    // {skip} argument of searchpair() can be compiled if not empty
	    यदि (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != रिक्त
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	अन्यथा यदि (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_मोजणी + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_मोजणी;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    यदि (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != रिक्त
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	यदि (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    प्रत्यागम FAIL;
	}
	यदि (*p == ',')
	{
	    ++p;
	    यदि (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		प्रत्यागम FAIL;
	    }
	}
	अन्यथा
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
    type_T	*type = get_decl_type_on_चळत(cctx, 0);
    पूर्णाङ्क		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    यदि (type->tt_type == VAR_OBJECT)
    {
	पूर्णाङ्क	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	यदि (uf != रिक्त)
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

    यदि (varlen >= परिमाण(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	प्रत्यागम FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    यदि (import != रिक्त)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	प्रत्यागम FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    यदि ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	पूर्णाङ्क	    is_has = **arg == 'h';
	पूर्णाङ्क	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	यदि (*s == '"')
	    (शून्य)eval_string(&s, &argvars[0], TRUE, FALSE);
	अन्यथा यदि (*s == '\'')
	    (शून्य)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	यदि (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    यदि (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_संख्या;
		tv->vval.v_संख्या = 0;
		यदि (is_has)
		    f_has(argvars, tv);
		अन्यथा यदि (is_len)
		    f_len(argvars, tv);
		अन्यथा
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    प्रत्यागम OK;
	}
	clear_tv(&argvars[0]);
	यदि (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    प्रत्यागम FAIL;
	}
    }

    यदि (generate_ppconst(cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    यदि ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    अन्यथा यदि (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    अन्यथा
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    यदि (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	यदि (*arg == रिक्त)
	    गच्छ theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	यदि (gname == रिक्त)
	    गच्छ theend;
	++*arg;			// skip '('
    }
    अन्यथा
	*arg += varlen + 1;

    यदि (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	गच्छ theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != रिक्त;
    यदि (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	पूर्णाङ्क	    idx;

	// builtin function
	idx = find_internal_func(name);
	यदि (idx >= 0)
	{
	    यदि (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		गच्छ theend;
	    }

	    यदि (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_चळत(cctx, 1);
		यदि (check_type_is_मूल्य(get_type_on_चळत(cctx, 0)) == FAIL)
		    गच्छ theend;

		// add() can be compiled to instructions if we know the type
		यदि (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		अन्यथा यदि (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    यदि ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		यदि (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    यदि (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    अन्यथा यदि (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    अन्यथा यदि (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    यदि (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		यदि (res == OK)
		    idx = -1;
	    }

	    यदि (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	अन्यथा
	    emsg_funcname(e_unknown_function_str, namebuf);
	गच्छ theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    यदि (lookup_local(namebuf, varlen, रिक्त, cctx) == FAIL
	    && arg_exists(namebuf, varlen, रिक्त, रिक्त, रिक्त, cctx) != OK)
    {
	class_T		*cl = रिक्त;
	पूर्णाङ्क		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	यदि (ufunc != रिक्त)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    यदि (ufunc == रिक्त)
		गच्छ theend;

	    यदि (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, रिक्त, 0, argcount, FALSE);
		गच्छ theend;
	    }
	    यदि (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == रिक्त
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		गच्छ theend;
	    }
	}
	अन्यथा यदि ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    यदि (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], रिक्त,
							0, argcount, FALSE);
	    }
	    अन्यथा
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
    यदि (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_चळत(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	गच्छ theend;
    }

    // If we can find a global function by name generate the right call.
    यदि (ufunc != रिक्त)
    {
	res = generate_CALL(cctx, ufunc, रिक्त, 0, argcount, FALSE);
	गच्छ theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    यदि (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    अन्यथा
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
    यदि (!eval_isnamec1(*arg))
	प्रत्यागम arg;

    प्रति (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	यदि (*p == ':' && (p != arg + 1
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

    यदि (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    यदि (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	यदि (eval_list(&p, &rettv, रिक्त, FALSE) == FAIL)
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
    पूर्णाङ्क		मोजणी = 0;
    पूर्णाङ्क		is_const;
    पूर्णाङ्क		is_all_const = TRUE;	// reset when non-const encountered
    पूर्णाङ्क		must_end = FALSE;

    प्रति (;;)
    {
	यदि (may_get_next_ओळ(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    प्रत्यागम FAIL;
	}
	यदि (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    प्रत्यागम FAIL;
	}
	यदि (*p == ']')
	{
	    ++p;
	    भङ्ग;
	}
	यदि (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    प्रत्यागम FAIL;
	}
	यदि (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    प्रत्यागम FAIL;
	यदि (!is_const)
	    is_all_const = FALSE;
	++मोजणी;
	यदि (*p == ',')
	{
	    ++p;
	    यदि (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		प्रत्यागम FAIL;
	    }
	}
	अन्यथा
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    प्रत्यागम generate_NEWLIST(cctx, मोजणी, FALSE);
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
    पूर्णाङ्क		मोजणी = 0;
    पूर्णाङ्क		is_const;
    पूर्णाङ्क		is_all_const = TRUE;	// reset when non-const encountered
    पूर्णाङ्क		must_end = FALSE;

    यदि (**arg != ')')
    {
	यदि (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    प्रत्यागम FAIL;
	}
	मोजणी = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	प्रति (;;)
	{
	    यदि (may_get_next_ओळ(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		प्रत्यागम FAIL;
	    }
	    यदि (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		प्रत्यागम FAIL;
	    }
	    यदि (*p == ')')
	    {
		++p;
		भङ्ग;
	    }
	    यदि (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		प्रत्यागम FAIL;
	    }
	    यदि (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		प्रत्यागम FAIL;
	    यदि (!is_const)
		is_all_const = FALSE;
	    ++मोजणी;
	    यदि (*p == ',')
	    {
		++p;
		यदि (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    प्रत्यागम FAIL;
		}
	    }
	    अन्यथा
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    प्रत्यागम generate_NEWTUPLE(cctx, मोजणी, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    स्थिर शून्य
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*ओळ;
    size_t	off;

    यदि (!evalarg->eval_using_cmdline || cctx == रिक्त)
	प्रत्यागम;

    gap = &evalarg->eval_tofree_ga;
    यदि (gap->ga_len == 0)
	प्रत्यागम;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    ओळ = ((char_u **)cctx->ctx_ufunc->uf_ओळी.ga_data)[cctx->ctx_lnum];
    *arg = ओळ + off;
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
    यदि (r != OK)
    {
	clear_evalarg(&evalarg, रिक्त);
	प्रत्यागम r;
    }

    यदि (ppconst != रिक्त && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, रिक्त);
	प्रत्यागम FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    यदि (cctx->ctx_ufunc != रिक्त)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	पूर्णाङ्क block_depth = cctx->ctx_ufunc->uf_block_depth;

	यदि (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(पूर्णाङ्क, block_depth);
	    यदि (ufunc->uf_block_ids != रिक्त)
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
    यदि (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    यदि (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    यदि (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	यदि (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, रिक्त);

    यदि (ufunc->uf_def_status == UF_COMPILED)
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
    यदि (r != OK)
	प्रत्यागम r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    यदि (ufunc->uf_ret_type == रिक्त || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, रिक्त);

    यदि (ufunc->uf_def_status == UF_COMPILED)
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
    पूर्णाङ्क		मोजणी = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    पूर्णाङ्क		is_const;
    पूर्णाङ्क		is_all_const = TRUE;	// reset when non-const encountered

    यदि (d == रिक्त)
	प्रत्यागम FAIL;
    यदि (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	प्रत्यागम FAIL;
    }
    प्रति (;;)
    {
	char_u	    *किल्ली = रिक्त;

	यदि (may_get_next_ओळ(whitep, arg, cctx) == FAIL)
	{
	    *arg = रिक्त;
	    गच्छ failret;
	}

	यदि (**arg == '}')
	    भङ्ग;

	यदि (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    यदि (compile_expr0(arg, cctx) == FAIL)
		गच्छ failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    यदि (isn->isn_type == ISN_PUSHNR)
	    {
		वर्ण buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.संख्या);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    यदि (isn->isn_type == ISN_PUSHS)
		किल्ली = isn->isn_arg.string;
	    अन्यथा यदि (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		गच्छ failret;
	    *arg = skipwhite(*arg);
	    यदि (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_किल्ली));
		गच्छ failret;
	    }
	    ++*arg;
	}
	अन्यथा
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    किल्ली = get_literal_किल्ली(arg);
	    यदि (किल्ली == रिक्त)
		गच्छ failret;
	    यदि (generate_PUSHS(cctx, &किल्ली) == FAIL)
		गच्छ failret;
	}

	// Check for duplicate keys, if using string keys.
	यदि (किल्ली != रिक्त)
	{
	    item = dict_find(d, किल्ली, -1);
	    यदि (item != रिक्त)
	    {
		semsg(_(e_duplicate_किल्ली_in_dictionary_str), किल्ली);
		गच्छ failret;
	    }
	    item = dictitem_alloc(किल्ली);
	    यदि (item != रिक्त)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		यदि (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	यदि (**arg != ':')
	{
	    यदि (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    अन्यथा
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    गच्छ failret;
	}
	whitep = *arg + 1;
	यदि (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    गच्छ failret;
	}

	यदि (may_get_next_ओळ(whitep, arg, cctx) == FAIL)
	{
	    *arg = रिक्त;
	    गच्छ failret;
	}

	यदि (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    गच्छ failret;
	यदि (!is_const)
	    is_all_const = FALSE;
	++मोजणी;

	whitep = *arg;
	यदि (may_get_next_ओळ(whitep, arg, cctx) == FAIL)
	{
	    *arg = रिक्त;
	    गच्छ failret;
	}
	यदि (**arg == '}')
	    भङ्ग;
	यदि (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    गच्छ failret;
	}
	यदि (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    गच्छ failret;
	}
	whitep = *arg + 1;
	यदि (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    गच्छ failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    यदि (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    प्रत्यागम generate_NEWDICT(cctx, मोजणी, FALSE);

failret:
    यदि (*arg == रिक्त)
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
    यदि (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_संख्या ? &t_संख्या : &t_string;

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
    यदि (len == 0)
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
    पूर्णाङ्क		मोजणी = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    प्रति (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	यदि (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	अन्यथा
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	यदि (ret == FAIL)
	    भङ्ग;
	यदि (evaluate)
	{
	    यदि ((tv.vval.v_string != रिक्त && *tv.vval.v_string != NUL)
		    || (**arg != '{' && मोजणी == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		यदि (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    प्रत्यागम FAIL;
		tv.vval.v_string = रिक्त;  // don't free it now
		++मोजणी;
	    }
	    clear_tv(&tv);
	}

	यदि (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    भङ्ग;
	}

	p = compile_one_अभिव्यक्ती_in_str(*arg, cctx);
	यदि (p == रिक्त)
	{
	    ret = FAIL;
	    भङ्ग;
	}
	++मोजणी;
	*arg = p;
    }

    यदि (ret == FAIL || !evaluate)
	प्रत्यागम ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    यदि (मोजणी > 1)
	प्रत्यागम generate_CONCAT(cctx, मोजणी);

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
    यदि (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	प्रत्यागम FAIL;
    }
    यदि (!valid_yank_reg(**arg, FALSE))
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
    यावत् (p > start)
    {
	--p;
	यदि (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    यदि (rettv->v_type == VAR_FLOAT)
	    {
		यदि (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    अन्यथा
	    {
		varnumber_T	val;
		पूर्णाङ्क		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		यदि (check_not_string(rettv) == FAIL)
		    प्रत्यागम FAIL;
		val = tv_get_संख्या_chk(rettv, &error);
		clear_tv(rettv);
		यदि (error)
		    प्रत्यागम FAIL;
		यदि (*p == '-')
		    val = -val;
		rettv->v_type = VAR_संख्या;
		rettv->vval.v_संख्या = val;
	    }
	}
	अन्यथा यदि (numeric_only)
	{
	    ++p;
	    भङ्ग;
	}
	अन्यथा यदि (*p == '!')
	{
	    पूर्णाङ्क v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_संख्या = v ? VVAL_FALSE : VVAL_TRUE;
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
    यदि (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_संख्या = VVAL_TRUE;
	*arg += 6;
    }
    अन्यथा यदि (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_संख्या = VVAL_FALSE;
	*arg += 7;
    }
    अन्यथा यदि (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_संख्या = VVAL_NULL;
	*arg += 6;
    }
    अन्यथा यदि (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_संख्या = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, पूर्णाङ्क *len, पूर्णाङ्क *type_is)
{
    exprtype_T	type = अभिव्यक्ती_UNKNOWN;
    पूर्णाङ्क		i;

    विकल्प (p[0])
    {
	पक्ष '=':   यदि (p[1] == '=')
			type = अभिव्यक्ती_EQUAL;
		    अन्यथा यदि (p[1] == '~')
			type = अभिव्यक्ती_MATCH;
		    भङ्ग;
	पक्ष '!':   यदि (p[1] == '=')
			type = अभिव्यक्ती_NEQUAL;
		    अन्यथा यदि (p[1] == '~')
			type = अभिव्यक्ती_NOMATCH;
		    भङ्ग;
	पक्ष '>':   यदि (p[1] != '=')
		    {
			type = अभिव्यक्ती_GREATER;
			*len = 1;
		    }
		    अन्यथा
			type = अभिव्यक्ती_GEQUAL;
		    भङ्ग;
	पक्ष '<':   यदि (p[1] != '=')
		    {
			type = अभिव्यक्ती_SMALLER;
			*len = 1;
		    }
		    अन्यथा
			type = अभिव्यक्ती_SEQUAL;
		    भङ्ग;
	पक्ष 'i':   यदि (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			यदि (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			यदि (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? अभिव्यक्ती_IS : अभिव्यक्ती_ISNOT;
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
skip_अभिव्यक्ती_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_अभिव्यक्ती(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, रिक्त);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    पूर्णाङ्क
bool_on_चळत(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_चळत(cctx, 0);
    यदि (type == &t_bool)
	प्रत्यागम OK;

    यदि (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_संख्या
	    || type == &t_संख्या_bool
	    || type == &t_const_संख्या_bool)
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
    यावत् (p > start)
    {
	--p;
	यावत् (VIM_ISWHITE(*p))
	    --p;
	यदि (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_चळत(cctx, 0);
	    यदि (type->tt_type != VAR_FLOAT && need_type(type, &t_संख्या,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		प्रत्यागम FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    यदि (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == रिक्त)
		प्रत्यागम FAIL;
	}
	अन्यथा यदि (numeric_only)
	{
	    ++p;
	    भङ्ग;
	}
	अन्यथा
	{
	    पूर्णाङ्क  invert = *p == '!';

	    यावत् (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		यदि (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    यदि (check_type_is_मूल्य(get_type_on_चळत(cctx, 0)) == FAIL)
		प्रत्यागम FAIL;
	    यदि (generate_2BOOL(cctx, invert, -1) == FAIL)
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

    यदि (may_get_next_ओळ_error(p, arg, cctx) == FAIL)
	प्रत्यागम FAIL;

    यदि (**arg == ')')
	// empty tuple
	प्रत्यागम compile_tuple(arg, cctx, ppconst, FALSE);

    यदि (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    अन्यथा
    {
	// Not enough space in ppconst, flush constants.
	यदि (generate_ppconst(cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;
	ret = compile_expr0(arg, cctx);
    }
    यदि (may_get_next_ओळ_error(*arg, arg, cctx) == FAIL)
	प्रत्यागम FAIL;
    यदि (ret == OK && **arg == ',')
    {
	// tuple
	पूर्णाङ्क is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	यदि (generate_ppconst(cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;
	प्रत्यागम compile_tuple(arg, cctx, ppconst, is_const);
    }

    यदि (**arg == ')')
	++*arg;
    अन्यथा यदि (ret == OK)
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

	यदि (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_ओळ_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    यदि (next != रिक्त &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_ओळ_from_context(cctx, TRUE);
		यदि (next == रिक्त)
		    प्रत्यागम FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	यदि (**arg == '(')
	{
	    पूर्णाङ्क		argcount = 0;

	    यदि (generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_चळत(cctx, 0);

	    *arg = skipwhite(p + 1);
	    यदि (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		प्रत्यागम FAIL;
	    यदि (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		प्रत्यागम FAIL;
	    यदि (keeping_dict)
	    {
		keeping_dict = FALSE;
		यदि (generate_instr(cctx, ISN_CLEARDICT) == रिक्त)
		    प्रत्यागम FAIL;
	    }
	}
	अन्यथा यदि (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    पूर्णाङ्क	    alt;
	    char_u  *paren;

	    // something->method()
	    यदि (generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_चळत(cctx, 0);
	    यदि (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_मूल्य));
		प्रत्यागम FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    यदि (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		प्रत्यागम FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    यदि (**arg == '(')
		alt = 2;
	    अन्यथा
	    {
		// alternative 1 or 3
		p = *arg;
		यदि (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    प्रत्यागम FAIL;
		}
		यदि (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		प्रति ( ; eval_isnamec(*p); ++p)
		    ;
		यदि (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    यदि (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			प्रत्यागम FAIL;
		}
		अन्यथा
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    यदि (paren == रिक्त)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			प्रत्यागम FAIL;
		    }
		}
	    }

	    यदि (alt != 1)
	    {
		पूर्णाङ्क	    argcount = 1;
		garray_T    *चळत = &cctx->ctx_type_चळत;
		पूर्णाङ्क	    type_idx_start = चळत->ga_len;
		पूर्णाङ्क	    अभिव्यक्ती_isn_start = cctx->ctx_instr.ga_len;
		पूर्णाङ्क	    अभिव्यक्ती_isn_end;
		पूर्णाङ्क	    arg_isn_मोजणी;

		यदि (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    यदि (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			प्रत्यागम FAIL;
		}
		अन्यथा
		{
		    पूर्णाङ्क fail;
		    पूर्णाङ्क save_len = cctx->ctx_ufunc->uf_ओळी.ga_len;
		    पूर्णाङ्क	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_अभिव्यक्ती;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_ओळी.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_अभिव्यक्ती;
		    cctx->ctx_ufunc->uf_ओळी.ga_len = save_len;

		    यदि (fail)
		    {
			यदि (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			प्रत्यागम FAIL;
		    }
		}

		// Compile the arguments.
		यदि (**arg != '(')
		{
		    यदि (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    अन्यथा
			semsg(_(e_missing_parenthesis_str), *arg);
		    प्रत्यागम FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		अभिव्यक्ती_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		यदि (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    प्रत्यागम FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_मोजणी = cctx->ctx_instr.ga_len - अभिव्यक्ती_isn_end;
		यदि (arg_isn_मोजणी > 0)
		{
		    पूर्णाङ्क	    अभिव्यक्ती_isn_मोजणी = अभिव्यक्ती_isn_end - अभिव्यक्ती_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, अभिव्यक्ती_isn_मोजणी);
		    type_T  *decl_type;
		    type2_T  *typep;

		    यदि (isn == रिक्त)
			प्रत्यागम FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + अभिव्यक्ती_isn_start,
					       परिमाण(isn_T) * अभिव्यक्ती_isn_मोजणी);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + अभिव्यक्ती_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + अभिव्यक्ती_isn_end,
						परिमाण(isn_T) * arg_isn_मोजणी);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + अभिव्यक्ती_isn_start + arg_isn_मोजणी,
					  isn, परिमाण(isn_T) * अभिव्यक्ती_isn_मोजणी);
		    vim_free(isn);

		    typep = ((type2_T *)चळत->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)चळत->ga_data) + type_idx_start,
			      ((type2_T *)चळत->ga_data) + type_idx_start + 1,
			      परिमाण(type2_T)
				       * (चळत->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)चळत->ga_data) + चळत->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_चळत(cctx, 0);
		यदि (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    प्रत्यागम FAIL;
	    }

	    यदि (keeping_dict)
	    {
		keeping_dict = FALSE;
		यदि (generate_instr(cctx, ISN_CLEARDICT) == रिक्त)
		    प्रत्यागम FAIL;
	    }
	}
	अन्यथा यदि (**arg == '[')
	{
	    पूर्णाङ्क		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    यदि (generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    यदि (may_get_next_ओळ_error(p, arg, cctx) == FAIL)
		प्रत्यागम FAIL;
	    यदि (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    अन्यथा
	    {
		यदि (compile_expr0(arg, cctx) == FAIL)
		    प्रत्यागम FAIL;
		यदि (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    प्रत्यागम FAIL;
		}
		यदि (may_get_next_ओळ_error(*arg, arg, cctx) == FAIL)
		    प्रत्यागम FAIL;
		*arg = skipwhite(*arg);
	    }
	    यदि (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		यदि (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    प्रत्यागम FAIL;
		}
		यदि (may_get_next_ओळ_error(*arg, arg, cctx) == FAIL)
		    प्रत्यागम FAIL;
		यदि (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		अन्यथा
		{
		    यदि (compile_expr0(arg, cctx) == FAIL)
			प्रत्यागम FAIL;
		    यदि (may_get_next_ओळ_error(*arg, arg, cctx) == FAIL)
			प्रत्यागम FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    यदि (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		प्रत्यागम FAIL;
	    }
	    *arg = *arg + 1;

	    यदि (keeping_dict)
	    {
		keeping_dict = FALSE;
		यदि (generate_instr(cctx, ISN_CLEARDICT) == रिक्त)
		    प्रत्यागम FAIL;
	    }
	    यदि (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		प्रत्यागम FAIL;
	}
	अन्यथा यदि (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    यदि (generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_चळत(cctx, 0);
	    यदि (type != &t_unknown
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
		यदि (compile_class_object_index(cctx, arg, type) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अन्यथा
	    {
		*arg = p + 1;
		यदि (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    प्रत्यागम FAIL;
		}
		p = *arg;
		यावत् (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		यदि (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    प्रत्यागम FAIL;
		}
		यदि (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == रिक्त)
		    प्रत्यागम FAIL;
		यदि (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    प्रत्यागम FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	अन्यथा
	    भङ्ग;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    यदि (keeping_dict && cctx->ctx_skip != SKIP_YES
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
    यदि (eval_leader(arg, TRUE) == FAIL)
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
	पक्ष '.':   यदि (eval_संख्या(arg, rettv, TRUE, FALSE) == FAIL)
			प्रत्यागम FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    यदि (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			प्रत्यागम FAIL;
		    }
		    भङ्ग;

	/*
	 * String constant: "string".
	 */
	पक्ष '"':   यदि (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			प्रत्यागम FAIL;
		    भङ्ग;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	पक्ष '\'':  यदि (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
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
	पक्ष 't':   यदि (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_संख्या = VVAL_TRUE;
		    }
		    अन्यथा
			ret = NOTDONE;
		    भङ्ग;

	/*
	 * "false" constant
	 */
	पक्ष 'f':   यदि (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_संख्या = VVAL_FALSE;
		    }
		    अन्यथा
			ret = NOTDONE;
		    भङ्ग;

	/*
	 * "null" or "null_*" constant
	 */
	पक्ष 'n':   यदि (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			पूर्णाङ्क	len;

			प्रति (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			यदि (ret == FAIL)
			    ret = NOTDONE;
			अन्यथा
			    *arg += len + 4;
		    }
		    अन्यथा
			ret = NOTDONE;
		    भङ्ग;

	/*
	 * List: [expr, expr]
	 */
	पक्ष '[':   यदि (generate_ppconst(cctx, ppconst) == FAIL)
			प्रत्यागम FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    भङ्ग;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	पक्ष '{':   यदि (generate_ppconst(cctx, ppconst) == FAIL)
			प्रत्यागम FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    भङ्ग;

	/*
	 * Option value: &name
	 */
	पक्ष '&':	यदि (generate_ppconst(cctx, ppconst) == FAIL)
			    प्रत्यागम FAIL;
			ret = compile_get_option(arg, cctx);
			भङ्ग;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	पक्ष '$':	यदि (generate_ppconst(cctx, ppconst) == FAIL)
			    प्रत्यागम FAIL;
			यदि ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			अन्यथा
			    ret = compile_get_env(arg, cctx);
			भङ्ग;

	/*
	 * Register contents: @r.
	 */
	पक्ष '@':	यदि (generate_ppconst(cctx, ppconst) == FAIL)
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
		    यदि (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    भङ्ग;

	स्वभाव:    ret = NOTDONE;
		    भङ्ग;
    }
    यदि (ret == FAIL)
	प्रत्यागम FAIL;

    यदि (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	यदि (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	अन्यथा
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    अन्यथा यदि (ret == NOTDONE)
    {
	char_u	    *p;
	पूर्णाङ्क	    r;

	यदि (!eval_isnamec1(**arg))
	{
	    यदि (!vim9_bad_comment(*arg))
	    {
		यदि (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		अन्यथा
		    semsg(_(e_name_expected_str), *arg);
	    }
	    प्रत्यागम FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	यदि (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    प्रत्यागम FAIL;
	}

	size_t namelen = p - *arg;
	यदि (*p == '<')
	{
	    यदि (skip_generic_func_type_args(&p) == FAIL)
		प्रत्यागम FAIL;
	}

	यदि (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	अन्यथा
	{
	    यदि (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	यदि (r == FAIL)
	    प्रत्यागम FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    यदि (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	प्रत्यागम FAIL;
    यदि ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	यदि (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    प्रत्यागम FAIL;
	प्रत्यागम OK;
    }
    यदि (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
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
    यदि (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	यदि (want_type == रिक्त)
	    प्रत्यागम FAIL;

	यदि (**arg != '>')
	{
	    यदि (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    अन्यथा
		emsg(_(e_missing_gt));
	    प्रत्यागम FAIL;
	}
	++*arg;
	यदि (may_get_next_ओळ_error(*arg, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;
    }

    यदि (compile_expr9(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    यदि (want_type != रिक्त)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_चळत(cctx, 0);
	यदि (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    यदि (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
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
    यदि (compile_expr8(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    प्रति (;;)
    {
	op = may_peek_next_ओळ(cctx, *arg, &next);
	यदि (*op != '*' && *op != '/' && *op != '%')
	    भङ्ग;
	यदि (next != रिक्त)
	{
	    *arg = next_ओळ_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	यदि (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    प्रत्यागम FAIL;
	}
	यदि (may_get_next_ओळ_error(op + 1, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;

	// get the second expression
	यदि (compile_expr8(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	यदि (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_संख्या
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_संख्या)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    पूर्णाङ्क		    failed = FALSE;

	    // both are numbers: compute the result
	    विकल्प (*op)
	    {
		पक्ष '*': res = tv1->vval.v_संख्या * tv2->vval.v_संख्या;
			  भङ्ग;
		पक्ष '/': res = अंक_divide(tv1->vval.v_संख्या,
						  tv2->vval.v_संख्या, &failed);
			  भङ्ग;
		पक्ष '%': res = अंक_modulus(tv1->vval.v_संख्या,
						  tv2->vval.v_संख्या, &failed);
			  भङ्ग;
	    }
	    यदि (failed)
		प्रत्यागम FAIL;
	    tv1->vval.v_संख्या = res;
	    --ppconst->pp_used;
	}
	अन्यथा
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
    यदि (compile_expr7(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    प्रति (;;)
    {
	op = may_peek_next_ओळ(cctx, *arg, &next);
	यदि (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    भङ्ग;
	यदि (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    भङ्ग;
	oplen = (*op == '.' ? 2 : 1);
	यदि (next != रिक्त)
	{
	    *arg = next_ओळ_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	यदि (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    प्रत्यागम FAIL;
	}

	यदि (may_get_next_ओळ_error(op + oplen, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;

	// get the second expression
	यदि (compile_expr7(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	यदि (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_संख्या
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_संख्या)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    यदि (*op == '+')
		tv1->vval.v_संख्या = tv1->vval.v_संख्या + tv2->vval.v_संख्या;
	    अन्यथा यदि (*op == '-')
		tv1->vval.v_संख्या = tv1->vval.v_संख्या - tv2->vval.v_संख्या;
	    अन्यथा
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		यदि (tv1->vval.v_string == रिक्त)
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
	अन्यथा
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    यदि (*op == '.')
	    {
		यदि (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    प्रत्यागम FAIL;
		यदि (generate_CONCAT(cctx, 2) == FAIL)
		    प्रत्यागम FAIL;
	    }
	    अन्यथा
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
    exprtype_T	type = अभिव्यक्ती_UNKNOWN;
    char_u	*p;
    char_u	*next;
    पूर्णाङ्क		len = 2;
    पूर्णाङ्क		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    यदि (compile_expr6(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    प्रति (;;)
    {
	type = अभिव्यक्ती_UNKNOWN;

	p = may_peek_next_ओळ(cctx, *arg, &next);
	यदि (p[0] == '<' && p[1] == '<')
	    type = अभिव्यक्ती_LSHIFT;
	अन्यथा यदि (p[0] == '>' && p[1] == '>')
	    type = अभिव्यक्ती_RSHIFT;

	यदि (type == अभिव्यक्ती_UNKNOWN)
	    प्रत्यागम OK;

	// Handle a bitwise left or right shift operator
	यदि (ppconst->pp_used == ppconst_used + 1)
	{
	    यदि (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_संख्या)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_संख्या));
		प्रत्यागम FAIL;
	    }
	}
	अन्यथा
	{
	    type_T	*t = get_type_on_चळत(cctx, 0);

	    यदि (need_type(t, &t_संख्या, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_संख्या));
		प्रत्यागम FAIL;
	    }
	}

	यदि (next != रिक्त)
	{
	    *arg = next_ओळ_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	यदि (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    प्रत्यागम FAIL;
	}

	// get the second variable
	यदि (may_get_next_ओळ_error(p + len, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;

	यदि (compile_expr6(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	यदि (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    यदि (tv2->v_type != VAR_संख्या || tv2->vval.v_संख्या < 0)
	    {
		// right operand should be a positive number
		यदि (tv2->v_type != VAR_संख्या)
		    emsg(_(e_bitshift_ops_must_be_संख्या));
		अन्यथा
		    emsg(_(e_bitshift_ops_must_be_positive));
		प्रत्यागम FAIL;
	    }

	    यदि (tv2->vval.v_संख्या > MAX_LSHIFT_BITS)
		tv1->vval.v_संख्या = 0;
	    अन्यथा यदि (type == अभिव्यक्ती_LSHIFT)
		tv1->vval.v_संख्या =
			(uvarnumber_T)tv1->vval.v_संख्या << tv2->vval.v_संख्या;
	    अन्यथा
		tv1->vval.v_संख्या =
			(uvarnumber_T)tv1->vval.v_संख्या >> tv2->vval.v_संख्या;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	अन्यथा
	{
	    यदि (need_type(get_type_on_चळत(cctx, 0), &t_संख्या, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_संख्या));
		प्रत्यागम FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    यदि (isn == रिक्त)
		प्रत्यागम FAIL;

	    यदि (isn != रिक्त)
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
    exprtype_T	type = अभिव्यक्ती_UNKNOWN;
    char_u	*p;
    char_u	*next;
    पूर्णाङ्क		len = 2;
    पूर्णाङ्क		type_is = FALSE;
    पूर्णाङ्क		ppconst_used = ppconst->pp_used;

    // get the first variable
    यदि (compile_expr5(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    p = may_peek_next_ओळ(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    यदि (type != अभिव्यक्ती_UNKNOWN)
    {
	पूर्णाङ्क ic = FALSE;  // Default: do not ignore case

	यदि (next != रिक्त)
	{
	    *arg = next_ओळ_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	यदि (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    प्रत्यागम FAIL;
	}
	// extra question mark appended: ignore case
	यदि (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	अन्यथा यदि (p[len] == '#')
	    ++len;
	// nothing appended: match case

	यदि (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    प्रत्यागम FAIL;
	}

	// get the second variable
	यदि (may_get_next_ओळ_error(p + len, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;

	यदि (compile_expr5(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	यदि (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    पूर्णाङ्क		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    यदि (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    अन्यथा
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_संख्या = tv1->vval.v_संख्या
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
    char_u	*p = may_peek_next_ओळ(cctx, *arg, &next);
    पूर्णाङ्क		opchar = *op;

    यदि (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	पूर्णाङ्क		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, परिमाण(पूर्णाङ्क), 10);
	यावत् (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    पूर्णाङ्क		start_ctx_lnum = cctx->ctx_lnum;
	    पूर्णाङ्क		save_lnum;
	    पूर्णाङ्क		const_used;
	    पूर्णाङ्क		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    यदि (next != रिक्त)
	    {
		*arg = next_ओळ_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    यदि (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
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
	    यदि (status != FAIL)
	    {
		// Use the last ppconst if possible.
		यदि (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    पूर्णाङ्क		is_true = tv2bool(tv);

		    यदि ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_संख्या = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    अन्यथा
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		अन्यथा
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_चळत(cctx);
		}
	    }
	    यदि (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    यदि (status == FAIL)
	    {
		ga_clear(&end_ga);
		प्रत्यागम FAIL;
	    }

	    यदि (jump_when != JUMP_NEVER)
	    {
		यदि (cctx->ctx_skip != SKIP_YES)
		{
		    *(((पूर्णाङ्क *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    यदि (may_get_next_ओळ_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		प्रत्यागम FAIL;
	    }

	    const_used = ppconst->pp_used;
	    यदि ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		प्रत्यागम FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    यदि (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		यदि (tv->v_type == VAR_संख्या
			 && (tv->vval.v_संख्या == 1 || tv->vval.v_संख्या == 0))
		{
		    tv->vval.v_संख्या = tv->vval.v_संख्या == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_ओळ(cctx, *arg, &next);
	}

	यदि (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    प्रत्यागम FAIL;
	}

	यदि (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    यदि (bool_on_चळत(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		प्रत्यागम FAIL;
	    }

	यदि (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    यावत् (end_ga.ga_len > 0)
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
    यदि (compile_expr4(arg, cctx, ppconst) == FAIL)
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
    यदि (compile_expr3(arg, cctx, ppconst) == FAIL)
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
    यदि (cctx->ctx_skip == SKIP_YES)
    {
	पूर्णाङ्क		prev_did_emsg = did_emsg;

	skip_अभिव्यक्ती_cctx(arg, cctx);
	प्रत्यागम did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    यदि (compile_expr2(arg, cctx, ppconst) == FAIL)
	प्रत्यागम FAIL;

    p = may_peek_next_ओळ(cctx, *arg, &next);
    यदि (*p == '?')
    {
	पूर्णाङ्क		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*चळत = &cctx->ctx_type_चळत;
	पूर्णाङ्क		alt_idx = instr->ga_len;
	पूर्णाङ्क		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = रिक्त;
	पूर्णाङ्क		has_const_अभिव्यक्ती = FALSE;
	पूर्णाङ्क		const_मूल्य = FALSE;
	पूर्णाङ्क		save_skip = cctx->ctx_skip;

	यदि (next != रिक्त)
	{
	    *arg = next_ओळ_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	यदि (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    प्रत्यागम FAIL;
	}

	यदि (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_अभिव्यक्ती = TRUE;
	    यदि (op_falsy)
		const_मूल्य = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    अन्यथा
	    {
		पूर्णाङ्क error = FALSE;

		const_मूल्य = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		यदि (error)
		    प्रत्यागम FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_मूल्य : !const_मूल्य) ? SKIP_YES : SKIP_NOT;

	    यदि (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    अन्यथा
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	अन्यथा
	{
	    generate_ppconst(cctx, ppconst);
	    यदि (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    यदि (op_falsy)
	    {
		type1 = get_type_on_चळत(cctx, -1);
		यदि (check_type_is_मूल्य(type1) == FAIL)
		    प्रत्यागम FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	यदि (may_get_next_ओळ_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    प्रत्यागम FAIL;
	यदि (compile_expr1(arg, cctx, ppconst) == FAIL)
	    प्रत्यागम FAIL;

	यदि (!has_const_अभिव्यक्ती)
	{
	    generate_ppconst(cctx, ppconst);

	    यदि (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_चळत(cctx, 0);
		--चळत->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	यदि (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_ओळ(cctx, *arg, &next);
	    यदि (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		प्रत्यागम FAIL;
	    }
	    यदि (next != रिक्त)
	    {
		*arg = next_ओळ_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    यदि (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		प्रत्यागम FAIL;
	    }

	    // evaluate the third expression
	    यदि (has_const_अभिव्यक्ती)
		cctx->ctx_skip = save_skip == SKIP_YES || const_मूल्य
							 ? SKIP_YES : SKIP_NOT;
	    यदि (may_get_next_ओळ_error(p + 1, arg, cctx) == FAIL)
		प्रत्यागम FAIL;
	    यदि (compile_expr1(arg, cctx, ppconst) == FAIL)
		प्रत्यागम FAIL;
	}

	यदि (!has_const_अभिव्यक्ती)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)चळत->ga_data)
					      + चळत->ga_len - 1)->type_curr);
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
    यदि (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	प्रत्यागम FAIL;
    }
    यदि (is_const != रिक्त)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    यदि (generate_ppconst(cctx, &ppconst) == FAIL)
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
