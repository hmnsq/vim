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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା paren_follows_after_ପଦସମଷ୍ଟି = 0;

/*
 * Generate code for any ppconst entries.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ret = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    ପାଇଁ (i = 0; i < ppconst->pp_used; ++i)
	ଯଦି (generate_tv_ଠେଲ(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_used = 0;
    cctx->ctx_skip = save_skip;
    ଫେରନ୍ତୁ ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_ppconst_bool(ppconst_T *ppconst)
{
    ଯଦି (ppconst->pp_used > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_used - 1];
	where_T	    where = WHERE_INIT;

	ଫେରନ୍ତୁ check_typval_type(&t_bool, tv, where);
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    ଶୂନ୍ୟ
clear_ppconst(ppconst_T *ppconst)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = 0; i < ppconst->pp_used; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_used = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_tuple_member(
    type2_T	*typep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_slice,
    cctx_T	*cctx)
{
    ଯଦି (is_slice)
    {
	ଯଦି (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	// a copy is made so the member type is no longer declared
	ଯଦି (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	ଯଦି (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    ଯଦି (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    ଯଦି (typep->type_curr->tt_argcount == 1)
	    {
		ଯଦି (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_args[0]->tt_member;
		ଅନ୍ୟଥା
		    typep->type_curr = typep->type_curr->tt_args[0];
	    }
	    ଅନ୍ୟଥା
		typep->type_curr = &t_any;
	    ଯଦି (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		ଯଦି (typep->type_decl->tt_argcount == 1)
		{
		    ଯଦି (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_args[0]->tt_member;
		    ଅନ୍ୟଥା
			typep->type_decl = typep->type_decl->tt_args[0];
		}
		ଅନ୍ୟଥା
		    typep->type_curr = &t_any;
	    }
	    ଅନ୍ୟଥା
		typep->type_decl = typep->type_curr;
	}
	ଯଦି (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_member(ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_slice, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)ଗଦା->ga_data) + ଗଦା->ga_len - (is_slice ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)ଗଦା->ga_data) + ଗଦା->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    ଯଦି ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    ଯଦି (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	ଯଦି (need_type(idxtype, &t_ସଂଖ୍ୟା, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (is_slice)
	{
	    idxtype = get_type_on_ଗଦା(cctx, 1);
	    ଯଦି (need_type(idxtype, &t_ସଂଖ୍ୟା, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
    }

    ଯଦି (vartype == VAR_DICT)
    {
	ଯଦି (is_slice)
	{
	    emsg(_(e_cannot_slice_dictionary));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    ଯଦି (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    ଯଦି (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		ଯଦି (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    ଅନ୍ୟଥା
		typep->type_decl = typep->type_curr;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	ଯଦି (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (keeping_dict != ଖାଲି)
	    *keeping_dict = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	ଯଦି ((is_slice
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (vartype == VAR_BLOB)
    {
	ଯଦି (is_slice)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    ଯଦି (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା
	{
	    typep->type_curr = &t_ସଂଖ୍ୟା;
	    typep->type_decl = &t_ସଂଖ୍ୟା;
	    ଯଦି (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
    }
    ଅନ୍ୟଥା ଯଦି (vartype == VAR_TUPLE)
    {
	ଯଦି (compile_tuple_member(typep, is_slice, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	ଯଦି (is_slice)
	{
	    ଯଦି (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    // a copy is made so the member type is no longer declared
	    ଯଦି (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    ଯଦି (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		ଯଦି (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		ଯଦି (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		ଯଦି (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    ଯଦି (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		ଅନ୍ୟଥା
			typep->type_decl = typep->type_curr;
	    }
	    ଯଦି (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
    }
    ଅନ୍ୟଥା
    {
	ବାଛ (vartype)
	{
	    ସ୍ଥିତି VAR_FUNC:
	    ସ୍ଥିତି VAR_PARTIAL:
		emsg(_(e_cannot_index_a_funcref));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_BOOL:
	    ସ୍ଥିତି VAR_SPECIAL:
	    ସ୍ଥିତି VAR_JOB:
	    ସ୍ଥିତି VAR_CHANNEL:
	    ସ୍ଥିତି VAR_INSTR:
	    ସ୍ଥିତି VAR_CLASS:
	    ସ୍ଥିତି VAR_OBJECT:
	    ସ୍ଥିତି VAR_TYPEALIAS:
	    ସ୍ଥିତି VAR_UNKNOWN:
	    ସ୍ଥିତି VAR_ANY:
	    ସ୍ଥିତି VAR_VOID:
		emsg(_(e_cannot_index_special_variable));
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    ପାଇଁ (cctx_T *cctx = cctx_arg; cctx != ଖାଲି; cctx = cctx->ctx_outer)
    {
	ଯଦି (cctx->ctx_ufunc != ଖାଲି && cctx->ctx_ufunc->uf_class != ଖାଲି)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    ଯେତେବେଳେ (clp != ଖାଲି)
	    {
		ଯଦି (clp == cl)
		    ଫେରନ୍ତୁ TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    ଫେରନ୍ତୁ FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_class_object_index(cctx_T *cctx, char_u **arg, type_T *type)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		m_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;

    ଯଦି (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	ଫେରନ୍ତୁ FAIL;
    }

    class_T *cl = type->tt_class;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    ଯଦି (type == &t_super)
    {
	ଯଦି (cctx->ctx_ufunc == ଖାଲି || cctx->ctx_ufunc->uf_class == ଖାଲି)
	{
	    emsg(_(e_using_super_not_in_class_method));
	    ଫେରନ୍ତୁ FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_ଗଦା.ga_len;
    }
    ଅନ୍ୟଥା ଯଦି (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	ଯଦି (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    ଯଦି (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    ଯଦି (cl == ଖାଲି)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	ଫେରନ୍ତୁ FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_end = find_name_end(name, ଖାଲି, ଖାଲି, FNE_CHECK_START);
    ଯଦି (name_end == name)
    {
	emsg(_(e_missing_name_after_dot));
	ଫେରନ୍ତୁ FAIL;
    }
    size_t len = name_end - name;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    ଯଦି (*name_end == '<')
    {
	// generic method call
	name_end = parse_generic_func_type_args(name, len, name_end,
							&gfatab, cctx);
	ଯଦି (name_end == ଖାଲି)
	    ଯାଅ done;
    }

    ଯଦି (*name_end == '(')
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	function_ଗଣନା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	child_ଗଣନା;
	ufunc_T	**functions;

	ଯଦି (type->tt_type == VAR_CLASS)
	{
	    function_ଗଣନା = cl->class_class_function_ଗଣନା;
	    child_ଗଣନା = cl->class_class_function_ଗଣନା_child;
	    functions = cl->class_class_functions;
	}
	ଅନ୍ୟଥା
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    ଯଦି (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_ଗଣନା = cl->class_obj_method_ଗଣନା;
	    child_ଗଣନା = cl->class_obj_method_ଗଣନା_child;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fi;
	ପାଇଁ (fi = is_super ? child_ଗଣନା : 0; fi < function_ଗଣନା; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    ଯଦି (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		ଭାଙ୍ଗ;
	    }
	}

	ocmember_T  *ocm = ଖାଲି;
	ଯଦି (ufunc == ଖାଲି)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    ଯଦି (ocm == ଖାଲି || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		method_not_found_ସନ୍ଦେଶ(cl, type->tt_type, name, len);
		ଯାଅ done;
	    }
	    ଯଦି (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_ଗଦା.ga_len;
		ଯଦି (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    ଯାଅ done;
	    }
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;

		ଯଦି (IS_INTERFACE(cl))
		    status = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		ଅନ୍ୟଥା
		    status = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		ଯଦି (status == FAIL)
		    ଯାଅ done;
	    }
	}

	ଯଦି (is_super && ufunc != ଖାଲି && IS_ABSTRACT_METHOD(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_method_str_direct), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    ଯାଅ done;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	ଯଦି (ocm == ଖାଲି && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_method_str), name);
	    ଯାଅ done;
	}

	// process generic function call
	ଯଦି (ufunc != ଖାଲି)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ଯଦି (ufunc == ଖାଲି)
		ଯାଅ done;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_end + 1);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount = 0;
	ଯଦି (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    ଯାଅ done;

	ଯଦି (ocm != ଖାଲି)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    ଯାଅ done;
	}
	ଯଦି (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	ଅନ୍ୟଥା
	    ret = generate_CALL(cctx, ufunc, ଖାଲି, 0, argcount, FALSE);
	ଯାଅ done;
    }

    ଯଦି (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	ଯଦି (m_idx >= 0)
	{
	    ଯଦି (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		ଯାଅ done;
	    }

	    *arg = name_end;
	    ଯଦି (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    ଅନ୍ୟଥା
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    ଯାଅ done;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_method_idx(cl, name, len);
	ଯଦି (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    ଯଦି (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		ଯାଅ done;
	    }
	    *arg = name_end;
	    // Remove the object type from the stack
	    --cctx->ctx_type_ଗଦା.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, ଖାଲି);
	    ଯାଅ done;
	}

	member_not_found_ସନ୍ଦେଶ(cl, VAR_OBJECT, name, len);
    }
    ଅନ୍ୟଥା
    {
	// load class member
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	ଯଦି (m != ଖାଲି)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    ଯଦି (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		ଯାଅ done;
	    }

	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ଗଦା.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    ଯାଅ done;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_method_idx(cl, name, len);
	ଯଦି (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    ଯଦି (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_method_str), fp->uf_name);
		ଯାଅ done;
	    }
	    *arg = name_end;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ଗଦା.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, ଖାଲି);
	    ଯାଅ done;
	}

	member_not_found_ସନ୍ଦେଶ(cl, VAR_CLASS, name, len);
    }

done:
    generic_func_args_table_clear(&gfatab);
    ଫେରନ୍ତୁ ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_load_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *start,	    // start of variable
	char_u **end,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx;
    imported_T	    *imp;

    ଯଦି (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	ଫେରନ୍ତୁ FAIL;
    si = SCRIPT_ITEM(current_sctx.sc_sid);
    idx = get_script_item_idx(current_sctx.sc_sid, name, 0, cctx, ଖାଲି);
    ଯଦି (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_data) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (end == ଖାଲି)
	imp = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି (import == ଖାଲି)
	imp = find_imported(name, 0, FALSE);
    ଅନ୍ୟଥା
	imp = import;

    ଯଦି (imp != ଖାଲି)
    {
	char_u	*p = skipwhite(*end);
	char_u	*exp_name;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	cc;
	ufunc_T	*ufunc = ଖାଲି;
	type_T	*type;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	done = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	ଯଦି (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), start);
	    ଫେରନ୍ତୁ FAIL;
	}
	++p;
	ଯଦି (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    ଫେରନ୍ତୁ FAIL;
	}

	// isolate one name
	exp_name = p;
	ଯେତେବେଳେ (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_ITEM(imp->imp_sid);
	ଯଦି (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_SOURCE(cctx, imp->imp_sid);

	ଯଦି (res == OK)
	{
	    ଯଦି (si->sn_autoload_prefix != ଖାଲି
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_prefix, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		ଯଦି (cc == '(' || paren_follows_after_ପଦସମଷ୍ଟି)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		ଅନ୍ୟଥା
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		done = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		ଯଦି (cc == '(' || paren_follows_after_ପଦସମଷ୍ଟି)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		ଅନ୍ୟଥା
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		done = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		idx = find_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, ଖାଲି, TRUE);
	    }
	}

	*p = cc;
	*end = p;
	ଯଦି (done)
	    ଫେରନ୍ତୁ res;

	ଯଦି (idx < 0)
	{
	    ଯଦି (ufunc != ଖାଲି)
	    {
		gfargs_tab_T	gfatab;

		generic_func_args_table_init(&gfatab);

		ଯଦି (IS_GENERIC_FUNC(ufunc))
		{
		    ଯଦି (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_args(name, STRLEN(name), p,
								&gfatab, cctx);
			ଯଦି (p != ଖାଲି)
			{
			    *end = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_args_table_clear(&gfatab);

			ଯଦି (p == ଖାଲି || ufunc == ଖାଲି)
			    ଫେରନ୍ତୁ FAIL;
		    }
		    ଅନ୍ୟଥା
		    {
			emsg_funcname(e_generic_func_missing_type_args_str,
									name);
			ଫେରନ୍ତୁ FAIL;
		    }
		}
		ଅନ୍ୟଥା ଯଦି (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    ଫେରନ୍ତୁ FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, ଖାଲି, TRUE);
		ଫେରନ୍ତୁ OK;
	    }
	    ଫେରନ୍ତୁ FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	ଫେରନ୍ତୁ OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    ଫେରନ୍ତୁ generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			has_g_prefix)
{
    ufunc_T *ufunc = find_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    ଯଦି (ufunc == ଖାଲି || (!has_g_prefix && func_requires_g_prefix(ufunc)))
	ଫେରନ୍ତୁ FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    ଯଦି (ufunc == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    ଯଦି (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, ଖାଲି) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compiling_a_class_method(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    ଫେରନ୍ତୁ cctx->ctx_ufunc != ଖାଲି
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == ଖାଲି
		|| cctx->ctx_outer->ctx_ufunc == ଖାଲି
		|| cctx->ctx_outer->ctx_ufunc->uf_class == ଖାଲି
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			outer_depth)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		isn_idx;

    ଯଦି (fp == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = 0; depth < outer_depth; ++depth)
    {
	ଯଦି (outer_cctx->ctx_outer == ଖାଲି)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    ଫେରନ୍ତୁ FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    ଯଦି (fp->uf_block_ids == ଖାଲି && ufunc->uf_block_depth > 0)
    {
	fp->uf_block_ids = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, ufunc->uf_block_depth);
	ଯଦି (fp->uf_block_ids != ଖାଲି)
	{
	    mch_memmove(fp->uf_block_ids, ufunc->uf_block_ids,
					    ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * ufunc->uf_block_depth);
	    fp->uf_block_depth = ufunc->uf_block_depth;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    ଯଦି (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (generate_FUNCREF(cctx, fp, ଖାଲି, FALSE, 0, &isn_idx) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (cctx->ctx_skip == SKIP_YES)
	ଫେରନ୍ତୁ OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    ଯଦି (outer_depth > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_data) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	ଯଦି (extra == ଖାଲି)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    ଯଦି (extra == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_depth = outer_depth;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_load(
	char_u **arg,
	size_t  namelen,
	char_u *end_arg,
	cctx_T	*cctx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_ପଦସମଷ୍ଟି,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	error)
{
    type_T	*type;
    char_u	*name = ଖାଲି;
    char_u	*end = end_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = ଖାଲି;

    generic_func_args_table_init(&gfatab);

    ଯଦି (*(*arg + namelen) == '<')
    {
	// generic function call
	ଯଦି (parse_generic_func_type_args(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == ଖାଲି)
	{
	    generic_func_args_table_clear(&gfatab);
	    ଫେରନ୍ତୁ FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    ଯଦି (*(*arg + 1) == ':')
    {
	ଯଦି (end <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    ବାଛ (**arg)
	    {
		ସ୍ଥିତି 'g': isn_type = ISN_LOADGDICT; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'w': isn_type = ISN_LOADWDICT; ଭାଙ୍ଗ;
		ସ୍ଥିତି 't': isn_type = ISN_LOADTDICT; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'b': isn_type = ISN_LOADBDICT; ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    ଯାଅ theend;
	    }
	    ଯଦି (generate_instr_type(cctx, isn_type, &t_dict_any) == ଖାଲି)
		ଯାଅ theend;
	    res = OK;
	}
	ଅନ୍ୟଥା
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, end - (*arg + 2));
	    ଯଦି (name == ଖାଲି)
		ଯାଅ theend;

	    ବାଛ (**arg)
	    {
		ସ୍ଥିତି 'v': res = generate_LOADV(cctx, name);
			  ଭାଙ୍ଗ;
		ସ୍ଥିତି 's': ଯଦି (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_in_vim9_script_str),
									 *arg);
			      ଯାଅ theend;
			  }
			  ଯଦି (is_ପଦସମଷ୍ଟି && find_func(name, FALSE) != ଖାଲି)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  ଅନ୍ୟଥା
			      res = compile_load_scriptvar(cctx, name,
							    ଖାଲି, &end, ଖାଲି);
			  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'g': ଯଦି (vim_strchr(name, AUTOLOAD_CHAR) == ଖାଲି)
			  {
			      ଯଦି (is_ପଦସମଷ୍ଟି && ASCII_ISUPPER(*name)
				       && (find_func(name, TRUE) != ଖାଲି
					   || gfatab.gfat_args.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      ଅନ୍ୟଥା
				  isn_type = ISN_LOADG;
			  }
			  ଅନ୍ୟଥା
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, end - *arg);
			      ଯଦି (name == ଖାଲି)
				  ଯାଅ theend;
			  }
			  ଭାଙ୍ଗ;
		ସ୍ଥିତି 'w': isn_type = ISN_LOADW; ଭାଙ୍ଗ;
		ସ୍ଥିତି 't': isn_type = ISN_LOADT; ଭାଙ୍ଗ;
		ସ୍ଥିତି 'b': isn_type = ISN_LOADB; ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  ଯାଅ theend;
	    }
	    ଯଦି (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_LOAD(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    method_idx;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    gen_load = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    gen_load_outer = 0;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    outer_loop_depth = -1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	ଯଦି (name == ଖାଲି)
	    ଯାଅ theend;

	ଯଦି (STRCMP(name, "super") == 0 && compiling_a_class_method(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = ଠେଲ_type_ଗଦା(cctx, &t_super);
	    ଯଦି (*end != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	ଅନ୍ୟଥା ଯଦି (vim_strchr(name, AUTOLOAD_CHAR) != ଖାଲି)
	{
	    script_autoload(name, FALSE);
	    res = generate_LOAD(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	ଅନ୍ୟଥା ଯଦି (arg_exists(*arg, namelen, &idx, &type, &gen_load_outer, cctx)
									 == OK)
	{
	    ଯଦି (gen_load_outer == 0)
		gen_load = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    lvar_T  lvar;
	    class_T *cl = ଖାଲି;

	    ଯଦି (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		ଯଦି (lvar.lv_ufunc != ଖାଲି && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_args.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		ଅନ୍ୟଥା ଯଦି (lvar.lv_from_outer != 0)
		{
		    gen_load_outer = lvar.lv_from_outer;
		    outer_loop_depth = lvar.lv_loop_depth;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		ଅନ୍ୟଥା
		    gen_load = TRUE;
	    }
	    ଅନ୍ୟଥା ଯଦି (cctx->ctx_ufunc->uf_defclass != ଖାଲି &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((method_idx =
			     cctx_class_method_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		ଯଦି (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    ଯଦି (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    ଅନ୍ୟଥା
		    {
			ufunc_T *fp = cl->class_class_functions[method_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, method_idx,
									ଖାଲି);
		    }
		}
		ଅନ୍ୟଥା
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		imported_T *imp = ଖାଲି;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		ଯଦି (script_var_exists(*arg, namelen, cctx, ଖାଲି) == OK
			    || (imp = find_imported(name, 0, FALSE)) != ଖାଲି)
		    res = compile_load_scriptvar(cctx, name, *arg, &end, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		ଯଦି (res == FAIL && is_ପଦସମଷ୍ଟି && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	ଯଦି (gen_load)
	    res = generate_LOAD(cctx, ISN_LOAD, idx, ଖାଲି, type);
	ଯଦି (gen_load_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_load_outer,
				       outer_loop_depth, outer_loop_idx, type);
	    cctx->ctx_outer_used = TRUE;
	}
    }

    *arg = end;

theend:
    ଯଦି (lt != ଖାଲି)
	*lt = '<';
    ଯଦି (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_found_str), name);
    vim_free(name);
    generic_func_args_table_clear(&gfatab);
    ଫେରନ୍ତୁ res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_string(isn_T *isn, cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା str_offset)
{
    char_u	*s = isn->isn_arg.string + str_offset;
    garray_T	save_ga = cctx->ctx_instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ପଦସମଷ୍ଟି_res;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		trailing_error;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		instr_ଗଣନା;
    isn_T	*instr = ଖାଲି;

    // Remove the string type from the stack.
    --cctx->ctx_type_ଗଦା.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_data = ଖାଲି;

    // avoid peeking a next line
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା galen_save = cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len;
    cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len = 0;

    ପଦସମଷ୍ଟି_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    ଯଦି (ପଦସମଷ୍ଟି_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	ଯଦି (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_ଗଦା.ga_len;
	ଫେରନ୍ତୁ FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_ଗଣନା = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_data;
    instr[instr_ଗଣନା].isn_type = ISN_FINISH;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    ଫେରନ୍ତୁ OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    must_end = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    instr_ଗଣନା;

    ପାଇଁ (;;)
    {
	ଯଦି (may_get_next_ଧାଡ଼ି(whitep, &p, cctx) == FAIL)
	    ଯାଅ failret;
	ଯଦି (*p == ')')
	{
	    *arg = p + 1;
	    ଫେରନ୍ତୁ OK;
	}
	ଯଦି (must_end)
	{
	    semsg(_(e_missing_comma_before_argument_str), p);
	    ଫେରନ୍ତୁ FAIL;
	}

	instr_ଗଣନା = cctx->ctx_instr.ga_len;
	ଯଦି (compile_expr0(&p, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	++*argcount;

	ଯଦି (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_ଗଣନା + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_ଗଣନା;

	    // {skip} argument of searchpair() can be compiled if not empty
	    ଯଦି (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ଖାଲି
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	ଅନ୍ୟଥା ଯଦି (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_ଗଣନା + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_data) + instr_ଗଣନା;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    ଯଦି (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ଖାଲି
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	ଯଦି (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (*p == ',')
	{
	    ++p;
	    ଯଦି (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_builtin_method_call(cctx_T *cctx, class_builtin_T builtin_method)
{
    type_T	*type = get_decl_type_on_ଗଦା(cctx, 0);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    ଯଦି (type->tt_type == VAR_OBJECT)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	method_idx;
	ufunc_T *uf = class_get_builtin_method(type->tt_class, builtin_method,
							&method_idx);
	ଯଦି (uf != ଖାଲି)
	    res = generate_CALL(cctx, uf, type->tt_class, method_idx, 0, FALSE);
    }

    ଫେରନ୍ତୁ res;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ଖାଲି;
    char_u	*gname = ଖାଲି;
    ufunc_T	*ufunc = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_autoload;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    ଯଦି (varlen >= ଆକାର(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	ଫେରନ୍ତୁ FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = find_imported(name, varlen, FALSE);
    ଯଦି (import != ଖାଲି)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	ଫେରନ୍ତୁ FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    ଯଦି ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_has = **arg == 'h';
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	ଯଦି (*s == '"')
	    (ଶୂନ୍ୟ)eval_string(&s, &argvars[0], TRUE, FALSE);
	ଅନ୍ୟଥା ଯଦି (*s == '\'')
	    (ଶୂନ୍ୟ)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	ଯଦି (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    ଯଦି (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_ସଂଖ୍ୟା;
		tv->vval.v_ସଂଖ୍ୟା = 0;
		ଯଦି (is_has)
		    f_has(argvars, tv);
		ଅନ୍ୟଥା ଯଦି (is_len)
		    f_len(argvars, tv);
		ଅନ୍ୟଥା
		    f_exists(argvars, tv);
		++ppconst->pp_used;
	    }
	    clear_tv(&argvars[0]);
	    ଫେରନ୍ତୁ OK;
	}
	clear_tv(&argvars[0]);
	ଯଦି (!is_has && !is_len)
	{
	    emsg(_(e_argument_of_exists_compiled_must_be_literal_string));
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    ଯଦି ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    ଅନ୍ୟଥା ଯଦି (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    ଅନ୍ୟଥା
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_args_table_init(&gfatab);

    ଯଦି (*(*arg + varlen) == '<')
    {
	char_u	*start = *arg;

	// generic function
	*arg = parse_generic_func_type_args(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	ଯଦି (*arg == ଖାଲି)
	    ଯାଅ theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(start, *arg - start);
	ଯଦି (gname == ଖାଲି)
	    ଯାଅ theend;
	++*arg;			// skip '('
    }
    ଅନ୍ୟଥା
	*arg += varlen + 1;

    ଯଦି (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	ଯାଅ theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != ଖାଲି;
    ଯଦି (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx;

	// builtin function
	idx = find_internal_func(name);
	ଯଦି (idx >= 0)
	{
	    ଯଦି (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_in_vim9_script));
		ଯାଅ theend;
	    }

	    ଯଦି (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_ଗଦା(cctx, 1);
		ଯଦି (check_type_is_ମୂଲ୍ୟ(get_type_on_ଗଦା(cctx, 0)) == FAIL)
		    ଯାଅ theend;

		// add() can be compiled to instructions if we know the type
		ଯଦି (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		ଅନ୍ୟଥା ଯଦି (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    ଯଦି ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		ଯଦି (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_method = CLASS_BUILTIN_INVALID;
	    ଯଦି (STRCMP(name, "string") == 0)
		builtin_method = CLASS_BUILTIN_STRING;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(name, "empty") == 0)
		builtin_method = CLASS_BUILTIN_EMPTY;
	    ଅନ୍ୟଥା ଯଦି (STRCMP(name, "len") == 0)
		builtin_method = CLASS_BUILTIN_LEN;
	    ଯଦି (builtin_method != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_method_call(cctx, builtin_method);
		ଯଦି (res == OK)
		    idx = -1;
	    }

	    ଯଦି (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	ଅନ୍ୟଥା
	    emsg_funcname(e_unknown_function_str, namebuf);
	ଯାଅ theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    ଯଦି (lookup_local(namebuf, varlen, ଖାଲି, cctx) == FAIL
	    && arg_exists(namebuf, varlen, ଖାଲି, ଖାଲି, ଖାଲି, cctx) != OK)
    {
	class_T		*cl = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = find_func(name, FALSE);
	ଯଦି (ufunc != ଖାଲି)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ଯଦି (ufunc == ଖାଲି)
		ଯାଅ theend;

	    ଯଦି (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, ଖାଲି, 0, argcount, FALSE);
		ଯାଅ theend;
	    }
	    ଯଦି (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ଖାଲି
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		ଯାଅ theend;
	    }
	}
	ଅନ୍ୟଥା ଯଦି ((mi = cctx_class_method_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    ଯଦି (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], ଖାଲି,
							0, argcount, FALSE);
	    }
	    ଅନ୍ୟଥା
	    {
		semsg(_(e_class_method_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    ଯାଅ theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == ଖାଲି && gname != ଖାଲି ? gname : namebuf;
    ଯଦି (!has_g_namespace && !is_autoload
	    && compile_load(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_ଗଦା(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	ଯାଅ theend;
    }

    // If we can find a global function by name generate the right call.
    ଯଦି (ufunc != ଖାଲି)
    {
	res = generate_CALL(cctx, ufunc, ଖାଲି, 0, argcount, FALSE);
	ଯାଅ theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    ଯଦି (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    ଅନ୍ୟଥା
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_args_table_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    ଫେରନ୍ତୁ res;
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
to_name_end(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    ଯଦି (!eval_isnamec1(*arg))
	ଫେରନ୍ତୁ arg;

    ପାଇଁ (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	ଯଦି (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == ଖାଲି))
	    ଭାଙ୍ଗ;
    ଫେରନ୍ତୁ p;
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

    ଯଦି (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = to_name_end(p, TRUE);
    ଯଦି (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	ଯଦି (eval_list(&p, &rettv, ଖାଲି, FALSE) == FAIL)
	    p = arg;
    }
    ଫେରନ୍ତୁ p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_const;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_all_const = TRUE;	// reset when non-const encountered
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		must_end = FALSE;

    ପାଇଁ (;;)
    {
	ଯଦି (may_get_next_ଧାଡ଼ି(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_end_of_list_rsb_str), *arg);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (*p == ']')
	{
	    ++p;
	    ଭାଙ୍ଗ;
	}
	ଯଦି (must_end)
	{
	    semsg(_(e_missing_comma_in_list_str), p);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (!is_const)
	    is_all_const = FALSE;
	++ଗଣନା_2;
	ଯଦି (*p == ',')
	{
	    ++p;
	    ଯଦି (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା
	    must_end = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ଫେରନ୍ତୁ generate_NEWLIST(cctx, ଗଣନା_2, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା first_item_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_const;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_all_const = TRUE;	// reset when non-const encountered
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		must_end = FALSE;

    ଯଦି (**arg != ')')
    {
	ଯଦି (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଗଣନା_2 = 1;	// the first tuple item is already processed
	is_all_const = first_item_const;
	ପାଇଁ (;;)
	{
	    ଯଦି (may_get_next_ଧାଡ଼ି(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (*p == ')')
	    {
		++p;
		ଭାଙ୍ଗ;
	    }
	    ଯଦି (must_end)
	    {
		semsg(_(e_missing_comma_in_tuple_str), p);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (!is_const)
		is_all_const = FALSE;
	    ++ଗଣନା_2;
	    ଯଦି (*p == ',')
	    {
		++p;
		ଯଦି (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	    ଅନ୍ୟଥା
		must_end = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    ଫେରନ୍ତୁ generate_NEWTUPLE(cctx, ଗଣନା_2, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*ଧାଡ଼ି;
    size_t	off;

    ଯଦି (!evalarg->eval_using_cmdline || cctx == ଖାଲି)
	ଫେରନ୍ତୁ;

    gap = &evalarg->eval_tofree_ga;
    ଯଦି (gap->ga_len == 0)
	ଫେରନ୍ତୁ;

    off = *arg - ((char_u **)gap->ga_data)[gap->ga_len - 1];
    ଧାଡ଼ି = ((char_u **)cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_data)[cctx->ctx_lnum];
    *arg = ଧାଡ଼ି + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    ଯଦି (r != OK)
    {
	clear_evalarg(&evalarg, ଖାଲି);
	ଫେରନ୍ତୁ r;
    }

    ଯଦି (ppconst != ଖାଲି && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, ଖାଲି);
	ଫେରନ୍ତୁ FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    ଯଦି (cctx->ctx_ufunc != ଖାଲି)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା block_depth = cctx->ctx_ufunc->uf_block_depth;

	ଯଦି (block_depth > 0)
	{
	    ufunc->uf_block_ids = ALLOC_MULT(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, block_depth);
	    ଯଦି (ufunc->uf_block_ids != ଖାଲି)
	    {
		mch_memmove(ufunc->uf_block_ids, cctx->ctx_ufunc->uf_block_ids,
						    ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା) * block_depth);
		ufunc->uf_block_depth = block_depth;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ଯଦି (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    ଯଦି (cctx->ctx_compile_type == CT_DEBUG
#ifdef FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#endif
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    ଯଦି (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	ଯଦି (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, ଖାଲି);

    ଯଦି (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	ଫେରନ୍ତୁ generate_FUNCREF(cctx, ufunc, ଖାଲି, FALSE, 0, ଖାଲି);
    }

    func_ptr_unref(ufunc);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    types_optional,
	evalarg_T   *evalarg)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
    ufunc_T	*ufunc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_sc_version = current_sctx.sc_version;

    // Get the funcref in "rettv".
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, ଖାଲି);
    current_sctx.sc_version = save_sc_version;
    ଯଦି (r != OK)
	ଫେରନ୍ତୁ r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ଯଦି (ufunc->uf_ret_type == ଖାଲି || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, ଖାଲି);

    ଯଦି (ufunc->uf_def_status == UF_COMPILED)
    {
	// The return type will now be known.
	set_function_type(ufunc);
	ଫେରନ୍ତୁ OK;
    }
    clear_tv(rettv);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*item;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_const;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_all_const = TRUE;	// reset when non-const encountered

    ଯଦି (d == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	ଫେରନ୍ତୁ FAIL;
    }
    ପାଇଁ (;;)
    {
	char_u	    *ଚାବି = ଖାଲି;

	ଯଦି (may_get_next_ଧାଡ଼ି(whitep, arg, cctx) == FAIL)
	{
	    *arg = ଖାଲି;
	    ଯାଅ failret;
	}

	ଯଦି (**arg == '}')
	    ଭାଙ୍ଗ;

	ଯଦି (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    ଯଦି (compile_expr0(arg, cctx) == FAIL)
		ଯାଅ failret;
	    isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;
	    ଯଦି (isn->isn_type == ISN_PUSHNR)
	    {
		ଅକ୍ଷର buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.ସଂଖ୍ୟା);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    ଯଦି (isn->isn_type == ISN_PUSHS)
		ଚାବି = isn->isn_arg.string;
	    ଅନ୍ୟଥା ଯଦି (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		ଯାଅ failret;
	    *arg = skipwhite(*arg);
	    ଯଦି (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_ଚାବି));
		ଯାଅ failret;
	    }
	    ++*arg;
	}
	ଅନ୍ୟଥା
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    ଚାବି = get_literal_ଚାବି(arg);
	    ଯଦି (ଚାବି == ଖାଲି)
		ଯାଅ failret;
	    ଯଦି (generate_PUSHS(cctx, &ଚାବି) == FAIL)
		ଯାଅ failret;
	}

	// Check for duplicate keys, if using string keys.
	ଯଦି (ଚାବି != ଖାଲି)
	{
	    item = dict_find(d, ଚାବି, -1);
	    ଯଦି (item != ଖାଲି)
	    {
		semsg(_(e_duplicate_ଚାବି_in_dictionary_str), ଚାବି);
		ଯାଅ failret;
	    }
	    item = dictitem_alloc(ଚାବି);
	    ଯଦି (item != ଖାଲି)
	    {
		item->di_tv.v_type = VAR_UNKNOWN;
		item->di_tv.v_lock = 0;
		ଯଦି (dict_add(d, item) == FAIL)
		    dictitem_free(item);
	    }
	}

	ଯଦି (**arg != ':')
	{
	    ଯଦି (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ଅନ୍ୟଥା
		semsg(_(e_missing_colon_in_dictionary_str), *arg);
	    ଯାଅ failret;
	}
	whitep = *arg + 1;
	ଯଦି (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    ଯାଅ failret;
	}

	ଯଦି (may_get_next_ଧାଡ଼ି(whitep, arg, cctx) == FAIL)
	{
	    *arg = ଖାଲି;
	    ଯାଅ failret;
	}

	ଯଦି (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    ଯାଅ failret;
	ଯଦି (!is_const)
	    is_all_const = FALSE;
	++ଗଣନା_2;

	whitep = *arg;
	ଯଦି (may_get_next_ଧାଡ଼ି(whitep, arg, cctx) == FAIL)
	{
	    *arg = ଖାଲି;
	    ଯାଅ failret;
	}
	ଯଦି (**arg == '}')
	    ଭାଙ୍ଗ;
	ଯଦି (**arg != ',')
	{
	    semsg(_(e_missing_comma_in_dictionary_str), *arg);
	    ଯାଅ failret;
	}
	ଯଦି (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    ଯାଅ failret;
	}
	whitep = *arg + 1;
	ଯଦି (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    ଯାଅ failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    ଯଦି (VIM_ISWHITE(**arg) && vim9_comment_start(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    ଫେରନ୍ତୁ generate_NEWDICT(cctx, ଗଣନା_2, FALSE);

failret:
    ଯଦି (*arg == ଖାଲି)
    {
	semsg(_(e_missing_dict_end_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Compile "&option".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*start = *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    ଯଦି (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(start, *arg - start);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_ସଂଖ୍ୟା ? &t_ସଂଖ୍ୟା : &t_string;

	ret = generate_LOAD(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    ଫେରନ୍ତୁ ret;
}

/*
 * Compile "$VAR".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*start = *arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    ଯଦି (len == 0)
    {
	semsg(_(e_syntax_error_at_str), start);
	ଫେରନ୍ତୁ FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(start, len + 1);
    ret = generate_LOAD(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    ଫେରନ୍ତୁ ret;
}

/*
 * Compile $"string" or $'string'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		quote;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = cctx->ctx_skip != SKIP_YES;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    ପାଇଁ (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ଯଦି (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ଅନ୍ୟଥା
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ଯଦି (ret == FAIL)
	    ଭାଙ୍ଗ;
	ଯଦି (evaluate)
	{
	    ଯଦି ((tv.vval.v_string != ଖାଲି && *tv.vval.v_string != NUL)
		    || (**arg != '{' && ଗଣନା_2 == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		ଯଦି (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		tv.vval.v_string = ଖାଲି;  // don't free it now
		++ଗଣନା_2;
	    }
	    clear_tv(&tv);
	}

	ଯଦି (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ଭାଙ୍ଗ;
	}

	p = compile_one_ପଦସମଷ୍ଟି_in_str(*arg, cctx);
	ଯଦି (p == ଖାଲି)
	{
	    ret = FAIL;
	    ଭାଙ୍ଗ;
	}
	++ଗଣନା_2;
	*arg = p;
    }

    ଯଦି (ret == FAIL || !evaluate)
	ଫେରନ୍ତୁ ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ଯଦି (ଗଣନା_2 > 1)
	ଫେରନ୍ତୁ generate_CONCAT(cctx, ଗଣନା_2);

    ଫେରନ୍ତୁ OK;
}

/*
 * Compile "@r".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_get_register(char_u **arg, cctx_T *cctx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

    ++*arg;
    ଯଦି (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	ଫେରନ୍ତୁ FAIL;
    }
    ret = generate_LOAD(cctx, ISN_LOADREG, **arg, ଖାଲି, &t_string);
    ++*arg;
    ଫେରନ୍ତୁ ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
apply_leader(typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା numeric_only, char_u *start, char_u **end)
{
    char_u *p = *end;

    // this works from end to start
    ଯେତେବେଳେ (p > start)
    {
	--p;
	ଯଦି (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    ଯଦି (rettv->v_type == VAR_FLOAT)
	    {
		ଯଦି (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    ଅନ୍ୟଥା
	    {
		varnumber_T	val;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		ଯଦି (check_not_string(rettv) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		val = tv_get_ସଂଖ୍ୟା_chk(rettv, &error);
		clear_tv(rettv);
		ଯଦି (error)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (*p == '-')
		    val = -val;
		rettv->v_type = VAR_ସଂଖ୍ୟା;
		rettv->vval.v_ସଂଖ୍ୟା = val;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (numeric_only)
	{
	    ++p;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '!')
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_ସଂଖ୍ୟା = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *end = p;
    ଫେରନ୍ତୁ OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_vim_constant(char_u **arg, typval_T *rettv)
{
    ଯଦି (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;
	*arg += 6;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;
	*arg += 7;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ସଂଖ୍ୟା = VVAL_NULL;
	*arg += 6;
    }
    ଅନ୍ୟଥା ଯଦି (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_ସଂଖ୍ୟା = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *type_is)
{
    exprtype_T	type = ପଦସମଷ୍ଟି_UNKNOWN;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ବାଛ (p[0])
    {
	ସ୍ଥିତି '=':   ଯଦି (p[1] == '=')
			type = ପଦସମଷ୍ଟି_EQUAL;
		    ଅନ୍ୟଥା ଯଦି (p[1] == '~')
			type = ପଦସମଷ୍ଟି_MATCH;
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି '!':   ଯଦି (p[1] == '=')
			type = ପଦସମଷ୍ଟି_NEQUAL;
		    ଅନ୍ୟଥା ଯଦି (p[1] == '~')
			type = ପଦସମଷ୍ଟି_NOMATCH;
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି '>':   ଯଦି (p[1] != '=')
		    {
			type = ପଦସମଷ୍ଟି_GREATER;
			*len = 1;
		    }
		    ଅନ୍ୟଥା
			type = ପଦସମଷ୍ଟି_GEQUAL;
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି '<':   ଯଦି (p[1] != '=')
		    {
			type = ପଦସମଷ୍ଟି_SMALLER;
			*len = 1;
		    }
		    ଅନ୍ୟଥା
			type = ପଦସମଷ୍ଟି_SEQUAL;
		    ଭାଙ୍ଗ;
	ସ୍ଥିତି 'i':   ଯଦି (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			ଯଦି (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			ଯଦି (!SAFE_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? ପଦସମଷ୍ଟି_IS : ପଦସମଷ୍ଟି_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    ଶୂନ୍ୟ
skip_ପଦସମଷ୍ଟି_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_ପଦସମଷ୍ଟି(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, ଖାଲି);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
bool_on_ଗଦା(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_ଗଦା(cctx, 0);
    ଯଦି (type == &t_bool)
	ଫେରନ୍ତୁ OK;

    ଯଦି (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_ସଂଖ୍ୟା
	    || type == &t_ସଂଖ୍ୟା_bool
	    || type == &t_const_ସଂଖ୍ୟା_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	ଫେରନ୍ତୁ generate_COND2BOOL(cctx);

    ଫେରନ୍ତୁ need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    ଶୂନ୍ୟ
error_white_both(char_u *op, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_leader(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା numeric_only, char_u *start, char_u **end)
{
    char_u	*p = *end;

    // this works from end to start
    ଯେତେବେଳେ (p > start)
    {
	--p;
	ଯେତେବେଳେ (VIM_ISWHITE(*p))
	    --p;
	ଯଦି (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_ଗଦା(cctx, 0);
	    ଯଦି (type->tt_type != VAR_FLOAT && need_type(type, &t_ସଂଖ୍ୟା,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		ଫେରନ୍ତୁ FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    ଯଦି (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (numeric_only)
	{
	    ++p;
	    ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା  invert = *p == '!';

	    ଯେତେବେଳେ (p > start && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		ଯଦି (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    ଯଦି (check_type_is_ମୂଲ୍ୟ(get_type_on_ଗଦା(cctx, 0)) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (generate_2BOOL(cctx, invert, -1) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
    }
    *end = p;
    ଫେରନ୍ତୁ OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ret;
    char_u  *p = *arg + 1;

    ଯଦି (may_get_next_ଧାଡ଼ି_error(p, arg, cctx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (**arg == ')')
	// empty tuple
	ଫେରନ୍ତୁ compile_tuple(arg, cctx, ppconst, FALSE);

    ଯଦି (ppconst->pp_used <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    ଅନ୍ୟଥା
    {
	// Not enough space in ppconst, flush constants.
	ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ret = compile_expr0(arg, cctx);
    }
    ଯଦି (may_get_next_ଧାଡ଼ି_error(*arg, arg, cctx) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (ret == OK && **arg == ',')
    {
	// tuple
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_const = ppconst->pp_used > 0 || ppconst->pp_is_const;
	ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଫେରନ୍ତୁ compile_tuple(arg, cctx, ppconst, is_const);
    }

    ଯଦି (**arg == ')')
	++*arg;
    ଅନ୍ୟଥା ଯଦି (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    ଫେରନ୍ତୁ ret;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *start_leader,
	char_u **end_leader,
	ppconst_T *ppconst)
{
    char_u	*name_start = *end_leader;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		keeping_dict = FALSE;

    ପାଇଁ (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	ଯଦି (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_start(p)))
	{
	    char_u *next = peek_next_ଧାଡ଼ି_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    ଯଦି (next != ଖାଲି &&
		    ((next[0] == '-' && next[1] == '>'
				 && (next[2] == '{'
				       || *skipwhite(next + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(next + 2))))
		    || (next[0] == '.' && eval_isdictc(next[1]))))
	    {
		next = next_ଧାଡ଼ି_from_context(cctx, TRUE);
		ଯଦି (next == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		*arg = next;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	ଯଦି (**arg == '(')
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = 0;

	    ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_ଗଦା(cctx, 0);

	    *arg = skipwhite(p + 1);
	    ଯଦି (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (generate_PCALL(cctx, argcount, name_start, type, TRUE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (keeping_dict)
	    {
		keeping_dict = FALSE;
		ଯଦି (generate_instr(cctx, ISN_CLEARDICT) == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    alt;
	    char_u  *paren;

	    // something->method()
	    ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ଗଦା(cctx, 0);
	    ଯଦି (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
		ଫେରନ୍ତୁ FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    ଯଦି (compile_leader(cctx, TRUE, start_leader, end_leader) == FAIL)
		ଫେରନ୍ତୁ FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    ଯଦି (**arg == '(')
		alt = 2;
	    ଅନ୍ୟଥା
	    {
		// alternative 1 or 3
		p = *arg;
		ଯଦି (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    ଫେରନ୍ତୁ FAIL;
		}
		ଯଦି (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		ପାଇଁ ( ; eval_isnamec(*p); ++p)
		    ;
		ଯଦି (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    ଯଦି (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    ଯଦି (paren == ଖାଲି)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			ଫେରନ୍ତୁ FAIL;
		    }
		}
	    }

	    ଯଦି (alt != 1)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount = 1;
		garray_T    *ଗଦା = &cctx->ctx_type_ଗଦା;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    type_idx_start = ଗଦା->ga_len;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ପଦସମଷ୍ଟି_isn_start = cctx->ctx_instr.ga_len;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ପଦସମଷ୍ଟି_isn_end;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    arg_isn_ଗଣନା;

		ଯଦି (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    ଯଦି (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		}
		ଅନ୍ୟଥା
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fail;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_len = cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_ପଦସମଷ୍ଟି;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_ପଦସମଷ୍ଟି;
		    cctx->ctx_ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len = save_len;

		    ଯଦି (fail)
		    {
			ଯଦି (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_expression_str), pstart);
			ଫେରନ୍ତୁ FAIL;
		    }
		}

		// Compile the arguments.
		ଯଦି (**arg != '(')
		{
		    ଯଦି (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    ଅନ୍ୟଥା
			semsg(_(e_missing_parenthesis_str), *arg);
		    ଫେରନ୍ତୁ FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		ପଦସମଷ୍ଟି_isn_end = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		ଯଦି (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    ଫେରନ୍ତୁ FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_ଗଣନା = cctx->ctx_instr.ga_len - ପଦସମଷ୍ଟି_isn_end;
		ଯଦି (arg_isn_ଗଣନା > 0)
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ପଦସମଷ୍ଟି_isn_ଗଣନା = ପଦସମଷ୍ଟି_isn_end - ପଦସମଷ୍ଟି_isn_start;
		    isn_T   *isn = ALLOC_MULT(isn_T, ପଦସମଷ୍ଟି_isn_ଗଣନା);
		    type_T  *decl_type;
		    type2_T  *typep;

		    ଯଦି (isn == ଖାଲି)
			ଫେରନ୍ତୁ FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_data)
							      + ପଦସମଷ୍ଟି_isn_start,
					       ଆକାର(isn_T) * ପଦସମଷ୍ଟି_isn_ଗଣନା);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
							      + ପଦସମଷ୍ଟି_isn_start,
			     ((isn_T *)cctx->ctx_instr.ga_data) + ପଦସମଷ୍ଟି_isn_end,
						ଆକାର(isn_T) * arg_isn_ଗଣନା);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_data)
					      + ପଦସମଷ୍ଟି_isn_start + arg_isn_ଗଣନା,
					  isn, ଆକାର(isn_T) * ପଦସମଷ୍ଟି_isn_ଗଣନା);
		    vim_free(isn);

		    typep = ((type2_T *)ଗଦା->ga_data) + type_idx_start;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)ଗଦା->ga_data) + type_idx_start,
			      ((type2_T *)ଗଦା->ga_data) + type_idx_start + 1,
			      ଆକାର(type2_T)
				       * (ଗଦା->ga_len - type_idx_start - 1));
		    typep = ((type2_T *)ଗଦା->ga_data) + ଗଦା->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_ଗଦା(cctx, 0);
		ଯଦି (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }

	    ଯଦି (keeping_dict)
	    {
		keeping_dict = FALSE;
		ଯଦି (generate_instr(cctx, ISN_CLEARDICT) == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା ଯଦି (**arg == '[')
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_slice = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    ଯଦି (may_get_next_ଧାଡ଼ି_error(p, arg, cctx) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (compile_expr0(arg, cctx) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ଫେରନ୍ତୁ FAIL;
		}
		ଯଦି (may_get_next_ଧାଡ଼ି_error(*arg, arg, cctx) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		*arg = skipwhite(*arg);
	    }
	    ଯଦି (**arg == ':')
	    {
		is_slice = TRUE;
		++*arg;
		ଯଦି (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    ଫେରନ୍ତୁ FAIL;
		}
		ଯଦି (may_get_next_ଧାଡ଼ି_error(*arg, arg, cctx) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		ଅନ୍ୟଥା
		{
		    ଯଦି (compile_expr0(arg, cctx) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    ଯଦି (may_get_next_ଧାଡ଼ି_error(*arg, arg, cctx) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    ଯଦି (**arg != ']')
	    {
		emsg(_(e_missing_closing_square_brace));
		ଫେରନ୍ତୁ FAIL;
	    }
	    *arg = *arg + 1;

	    ଯଦି (keeping_dict)
	    {
		keeping_dict = FALSE;
		ଯଦି (generate_instr(cctx, ISN_CLEARDICT) == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_slice, &keeping_dict, cctx) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ଗଦା(cctx, 0);
	    ଯଦି (type != &t_unknown
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
		ଯଦି (compile_class_object_index(cctx, arg, type) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		*arg = p + 1;
		ଯଦି (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    ଫେରନ୍ତୁ FAIL;
		}
		p = *arg;
		ଯେତେବେଳେ (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		ଯଦି (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    ଫେରନ୍ତୁ FAIL;
		}
		ଯଦି (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    ଯଦି (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*start_leader, *end_leader;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_used];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		used_before = ppconst->pp_used;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    start_leader = *arg;
    ଯଦି (eval_leader(arg, TRUE) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    end_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    ବାଛ (**arg)
    {
	/*
	 * Number constant.
	 */
	ସ୍ଥିତି '0':	// also for blob starting with 0z
	ସ୍ଥିତି '1':
	ସ୍ଥିତି '2':
	ସ୍ଥିତି '3':
	ସ୍ଥିତି '4':
	ସ୍ଥିତି '5':
	ସ୍ଥିତି '6':
	ସ୍ଥିତି '7':
	ସ୍ଥିତି '8':
	ସ୍ଥିତି '9':
	ସ୍ଥିତି '.':   ଯଦି (eval_ସଂଖ୍ୟା(arg, rettv, TRUE, FALSE) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    ଯଦି (apply_leader(rettv, TRUE,
					    start_leader, &end_leader) == FAIL)
		    {
			clear_tv(rettv);
			ଫେରନ୍ତୁ FAIL;
		    }
		    ଭାଙ୍ଗ;

	/*
	 * String constant: "string".
	 */
	ସ୍ଥିତି '"':   ଯଦି (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    ଭାଙ୍ଗ;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	ସ୍ଥିତି '\'':  ଯଦି (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    ଭାଙ୍ଗ;

	/*
	 * Constant Vim variable.
	 */
	ସ୍ଥିତି 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    ଭାଙ୍ଗ;

	/*
	 * "true" constant
	 */
	ସ୍ଥିତି 't':   ଯଦି (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_ସଂଖ୍ୟା = VVAL_TRUE;
		    }
		    ଅନ୍ୟଥା
			ret = NOTDONE;
		    ଭାଙ୍ଗ;

	/*
	 * "false" constant
	 */
	ସ୍ଥିତି 'f':   ଯଦି (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_ସଂଖ୍ୟା = VVAL_FALSE;
		    }
		    ଅନ୍ୟଥା
			ret = NOTDONE;
		    ଭାଙ୍ଗ;

	/*
	 * "null" or "null_*" constant
	 */
	ସ୍ଥିତି 'n':   ଯଦି (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len;

			ପାଇଁ (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			ଯଦି (ret == FAIL)
			    ret = NOTDONE;
			ଅନ୍ୟଥା
			    *arg += len + 4;
		    }
		    ଅନ୍ୟଥା
			ret = NOTDONE;
		    ଭାଙ୍ଗ;

	/*
	 * List: [expr, expr]
	 */
	ସ୍ଥିତି '[':   ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    ଭାଙ୍ଗ;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	ସ୍ଥିତି '{':   ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
			ଫେରନ୍ତୁ FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    ଭାଙ୍ଗ;

	/*
	 * Option value: &name
	 */
	ସ୍ଥିତି '&':	ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
			    ଫେରନ୍ତୁ FAIL;
			ret = compile_get_option(arg, cctx);
			ଭାଙ୍ଗ;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	ସ୍ଥିତି '$':	ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
			    ଫେରନ୍ତୁ FAIL;
			ଯଦି ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			ଅନ୍ୟଥା
			    ret = compile_get_env(arg, cctx);
			ଭାଙ୍ଗ;

	/*
	 * Register contents: @r.
	 */
	ସ୍ଥିତି '@':	ଯଦି (generate_ppconst(cctx, ppconst) == FAIL)
			    ଫେରନ୍ତୁ FAIL;
			ret = compile_get_register(arg, cctx);
			ଭାଙ୍ଗ;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	ସ୍ଥିତି '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    ଯଦି (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:    ret = NOTDONE;
		    ଭାଙ୍ଗ;
    }
    ଯଦି (ret == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (rettv->v_type != VAR_UNKNOWN && used_before == ppconst->pp_used)
    {
	ଯଦି (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	ଅନ୍ୟଥା
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_used;
    }
    ଅନ୍ୟଥା ଯଦି (ret == NOTDONE)
    {
	char_u	    *p;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    r;

	ଯଦି (!eval_isnamec1(**arg))
	{
	    ଯଦି (!vim9_bad_comment(*arg))
	    {
		ଯଦି (ends_excmd(*skipwhite(*arg)))
		    semsg(_(e_empty_expression_str), *arg);
		ଅନ୍ୟଥା
		    semsg(_(e_name_expected_str), *arg);
	    }
	    ଫେରନ୍ତୁ FAIL;
	}

	// "name" or "name()"
	p = to_name_end(*arg, TRUE);
	ଯଦି (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_here));
	    ଫେରନ୍ତୁ FAIL;
	}

	size_t namelen = p - *arg;
	ଯଦି (*p == '<')
	{
	    ଯଦି (skip_generic_func_type_args(&p) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    r = compile_load(arg, namelen, p, cctx, TRUE, TRUE);
	}
	ଯଦି (r == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    ଯଦି (compile_subscript(arg, cctx, start_leader, &end_leader,
							     ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି ((ppconst->pp_used > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_used - 1];
	ଯଦି (apply_leader(rettv, FALSE, start_leader, &end_leader) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଫେରନ୍ତୁ OK;
    }
    ଯଦି (compile_leader(cctx, FALSE, start_leader, &end_leader) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = ଖାଲି;

    // Recognize <type>
    ଯଦି (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ଯଦି (want_type == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (**arg != '>')
	{
	    ଯଦି (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    ଅନ୍ୟଥା
		emsg(_(e_missing_gt));
	    ଫେରନ୍ତୁ FAIL;
	}
	++*arg;
	ଯଦି (may_get_next_ଧାଡ଼ି_error(*arg, arg, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (compile_expr9(arg, cctx, ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (want_type != ଖାଲି)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_kind = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_ଗଦା(cctx, 0);
	ଯଦି (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    ଯଦି (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ppconst_used = ppconst->pp_used;

    // get the first expression
    ଯଦି (compile_expr8(arg, cctx, ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    ପାଇଁ (;;)
    {
	op = may_peek_next_ଧାଡ଼ି(cctx, *arg, &next);
	ଯଦି (*op != '*' && *op != '/' && *op != '%')
	    ଭାଙ୍ଗ;
	ଯଦି (next != ଖାଲି)
	{
	    *arg = next_ଧାଡ଼ି_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ଯଦି (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (may_get_next_ଧାଡ଼ି_error(op + 1, arg, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	// get the second expression
	ଯଦି (compile_expr8(arg, cctx, ppconst) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (ppconst->pp_used == ppconst_used + 2
		&& ppconst->pp_tv[ppconst_used].v_type == VAR_ସଂଖ୍ୟା
		&& ppconst->pp_tv[ppconst_used + 1].v_type == VAR_ସଂଖ୍ୟା)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_used + 1];
	    varnumber_T	    res = 0;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    failed = FALSE;

	    // both are numbers: compute the result
	    ବାଛ (*op)
	    {
		ସ୍ଥିତି '*': res = tv1->vval.v_ସଂଖ୍ୟା * tv2->vval.v_ସଂଖ୍ୟା;
			  ଭାଙ୍ଗ;
		ସ୍ଥିତି '/': res = ଅଙ୍କ_divide(tv1->vval.v_ସଂଖ୍ୟା,
						  tv2->vval.v_ସଂଖ୍ୟା, &failed);
			  ଭାଙ୍ଗ;
		ସ୍ଥିତି '%': res = ଅଙ୍କ_modulus(tv1->vval.v_ସଂଖ୍ୟା,
						  tv2->vval.v_ସଂଖ୍ୟା, &failed);
			  ଭାଙ୍ଗ;
	    }
	    ଯଦି (failed)
		ଫେରନ୍ତୁ FAIL;
	    tv1->vval.v_ସଂଖ୍ୟା = res;
	    --ppconst->pp_used;
	}
	ଅନ୍ୟଥା
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oplen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ppconst_used = ppconst->pp_used;

    // get the first variable
    ଯଦି (compile_expr7(arg, cctx, ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    ପାଇଁ (;;)
    {
	op = may_peek_next_ଧାଡ଼ି(cctx, *arg, &next);
	ଯଦି (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    ଭାଙ୍ଗ;
	ଯଦି (op[0] == op[1] && *op != '.' && next)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    ଭାଙ୍ଗ;
	oplen = (*op == '.' ? 2 : 1);
	ଯଦି (next != ଖାଲି)
	{
	    *arg = next_ଧାଡ଼ି_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ଯଦି (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (may_get_next_ଧାଡ଼ି_error(op + oplen, arg, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	// get the second expression
	ଯଦି (compile_expr7(arg, cctx, ppconst) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (ppconst->pp_used == ppconst_used + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_used].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_used].v_type == VAR_ସଂଖ୍ୟା
		    && ppconst->pp_tv[ppconst_used + 1].v_type == VAR_ସଂଖ୍ୟା)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_used];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_used + 1];

	    // concat/subtract/add constant numbers
	    ଯଦି (*op == '+')
		tv1->vval.v_ସଂଖ୍ୟା = tv1->vval.v_ସଂଖ୍ୟା + tv2->vval.v_ସଂଖ୍ୟା;
	    ଅନ୍ୟଥା ଯଦି (*op == '-')
		tv1->vval.v_ସଂଖ୍ୟା = tv1->vval.v_ସଂଖ୍ୟା - tv2->vval.v_ସଂଖ୍ୟା;
	    ଅନ୍ୟଥା
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		ଯଦି (tv1->vval.v_string == ଖାଲି)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    ଫେରନ୍ତୁ FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_used;
	}
	ଅନ୍ୟଥା
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    ଯଦି (*op == '.')
	    {
		ଯଦି (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (generate_CONCAT(cctx, 2) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    ଅନ୍ୟଥା
		generate_two_op(cctx, op);
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ପଦସମଷ୍ଟି_UNKNOWN;
    char_u	*p;
    char_u	*next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ppconst_used = ppconst->pp_used;
    isn_T	*isn;

    // get the first variable
    ଯଦି (compile_expr6(arg, cctx, ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    ପାଇଁ (;;)
    {
	type = ପଦସମଷ୍ଟି_UNKNOWN;

	p = may_peek_next_ଧାଡ଼ି(cctx, *arg, &next);
	ଯଦି (p[0] == '<' && p[1] == '<')
	    type = ପଦସମଷ୍ଟି_LSHIFT;
	ଅନ୍ୟଥା ଯଦି (p[0] == '>' && p[1] == '>')
	    type = ପଦସମଷ୍ଟି_RSHIFT;

	ଯଦି (type == ପଦସମଷ୍ଟି_UNKNOWN)
	    ଫେରନ୍ତୁ OK;

	// Handle a bitwise left or right shift operator
	ଯଦି (ppconst->pp_used == ppconst_used + 1)
	{
	    ଯଦି (ppconst->pp_tv[ppconst->pp_used - 1].v_type != VAR_ସଂଖ୍ୟା)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_ସଂଖ୍ୟା));
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	ଅନ୍ୟଥା
	{
	    type_T	*t = get_type_on_ଗଦା(cctx, 0);

	    ଯଦି (need_type(t, &t_ସଂଖ୍ୟା, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_ସଂଖ୍ୟା));
		ଫେରନ୍ତୁ FAIL;
	    }
	}

	ଯଦି (next != ଖାଲି)
	{
	    *arg = next_ଧାଡ଼ି_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ଯଦି (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ଫେରନ୍ତୁ FAIL;
	}

	// get the second variable
	ଯଦି (may_get_next_ଧାଡ଼ି_error(p + len, arg, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (compile_expr6(arg, cctx, ppconst) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];

	    // Both sides are a constant, compute the result now.
	    ଯଦି (tv2->v_type != VAR_ସଂଖ୍ୟା || tv2->vval.v_ସଂଖ୍ୟା < 0)
	    {
		// right operand should be a positive number
		ଯଦି (tv2->v_type != VAR_ସଂଖ୍ୟା)
		    emsg(_(e_bitshift_ops_must_be_ସଂଖ୍ୟା));
		ଅନ୍ୟଥା
		    emsg(_(e_bitshift_ops_must_be_positive));
		ଫେରନ୍ତୁ FAIL;
	    }

	    ଯଦି (tv2->vval.v_ସଂଖ୍ୟା > MAX_LSHIFT_BITS)
		tv1->vval.v_ସଂଖ୍ୟା = 0;
	    ଅନ୍ୟଥା ଯଦି (type == ପଦସମଷ୍ଟି_LSHIFT)
		tv1->vval.v_ସଂଖ୍ୟା =
			(uvarnumber_T)tv1->vval.v_ସଂଖ୍ୟା << tv2->vval.v_ସଂଖ୍ୟା;
	    ଅନ୍ୟଥା
		tv1->vval.v_ସଂଖ୍ୟା =
			(uvarnumber_T)tv1->vval.v_ସଂଖ୍ୟା >> tv2->vval.v_ସଂଖ୍ୟା;
	    clear_tv(tv2);
	    --ppconst->pp_used;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (need_type(get_type_on_ଗଦା(cctx, 0), &t_ସଂଖ୍ୟା, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_ସଂଖ୍ୟା));
		ଫେରନ୍ତୁ FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    ଯଦି (isn == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;

	    ଯଦି (isn != ଖାଲି)
		isn->isn_arg.op.op_type = type;
	}
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = ପଦସମଷ୍ଟି_UNKNOWN;
    char_u	*p;
    char_u	*next;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = 2;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type_is = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ppconst_used = ppconst->pp_used;

    // get the first variable
    ଯଦି (compile_expr5(arg, cctx, ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    p = may_peek_next_ଧାଡ଼ି(cctx, *arg, &next);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ଯଦି (type != ପଦସମଷ୍ଟି_UNKNOWN)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ic = FALSE;  // Default: do not ignore case

	ଯଦି (next != ଖାଲି)
	{
	    *arg = next_ଧାଡ଼ି_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	ଯଦି (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_expression_str), *arg);
	    ଫେରନ୍ତୁ FAIL;
	}
	// extra question mark appended: ignore case
	ଯଦି (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	ଅନ୍ୟଥା ଯଦି (p[len] == '#')
	    ++len;
	// nothing appended: match case

	ଯଦି (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    ଫେରନ୍ତୁ FAIL;
	}

	// get the second variable
	ଯଦି (may_get_next_ଧାଡ଼ି_error(p + len, arg, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (compile_expr5(arg, cctx, ppconst) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (ppconst->pp_used == ppconst_used + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_used - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_used - 1];
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    ଯଦି (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    ଅନ୍ୟଥା
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_ସଂଖ୍ୟା = tv1->vval.v_ସଂଖ୍ୟା
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_used;
	    }
	    ଫେରନ୍ତୁ ret;
	}

	generate_ppconst(cctx, ppconst);
	ଫେରନ୍ତୁ generate_COMPARE(cctx, type, ic);
    }

    ଫେରନ୍ତୁ OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	ଅକ୍ଷର	*op,
	ppconst_T *ppconst,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ppconst_used UNUSED)
{
    char_u	*next;
    char_u	*p = may_peek_next_ଧାଡ଼ି(cctx, *arg, &next);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opchar = *op;

    ଯଦି (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	end_ga;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&end_ga, ଆକାର(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), 10);
	ଯେତେବେଳେ (p[0] == opchar && p[1] == opchar)
	{
	    long	start_lnum = SOURCING_LNUM;
	    long	save_sourcing_lnum;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		start_ctx_lnum = cctx->ctx_lnum;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_lnum;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		const_used;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    ଯଦି (next != ଖାଲି)
	    {
		*arg = next_ଧାଡ଼ି_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ଯଦି (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&end_ga);
		ଫେରନ୍ତୁ FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = start_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = start_ctx_lnum;

	    status = check_ppconst_bool(ppconst);
	    ଯଦି (status != FAIL)
	    {
		// Use the last ppconst if possible.
		ଯଦି (ppconst->pp_used > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_true = tv2bool(tv);

		    ଯଦି ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_ସଂଖ୍ୟା = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ଅନ୍ୟଥା
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_used;
			jump_when = JUMP_NEVER;
		    }
		}
		ଅନ୍ୟଥା
		{
		    // Every part must evaluate to a bool.
		    status = bool_on_ଗଦା(cctx);
		}
	    }
	    ଯଦି (status != FAIL)
		status = ga_grow(&end_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    ଯଦି (status == FAIL)
	    {
		ga_clear(&end_ga);
		ଫେରନ୍ତୁ FAIL;
	    }

	    ଯଦି (jump_when != JUMP_NEVER)
	    {
		ଯଦି (cctx->ctx_skip != SKIP_YES)
		{
		    *(((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)end_ga.ga_data) + end_ga.ga_len) = instr->ga_len;
		    ++end_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    ଯଦି (may_get_next_ଧାଡ଼ି_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		ଫେରନ୍ତୁ FAIL;
	    }

	    const_used = ppconst->pp_used;
	    ଯଦି ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&end_ga);
		ଫେରନ୍ତୁ FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    ଯଦି (ppconst->pp_used == const_used + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_used - 1];

		ଯଦି (tv->v_type == VAR_ସଂଖ୍ୟା
			 && (tv->vval.v_ସଂଖ୍ୟା == 1 || tv->vval.v_ସଂଖ୍ୟା == 0))
		{
		    tv->vval.v_ସଂଖ୍ୟା = tv->vval.v_ସଂଖ୍ୟା == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_next_ଧାଡ଼ି(cctx, *arg, &next);
	}

	ଯଦି (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&end_ga);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (cctx->ctx_skip != SKIP_YES && ppconst->pp_used == 0)
	    // Every part must evaluate to a bool.
	    ଯଦି (bool_on_ଗଦା(cctx) == FAIL)
	    {
		ga_clear(&end_ga);
		ଫେରନ୍ତୁ FAIL;
	    }

	ଯଦି (end_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    ଯେତେବେଳେ (end_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--end_ga.ga_len;
		isn = ((isn_T *)instr->ga_data)
				  + *(((ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)end_ga.ga_data) + end_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&end_ga);

	cctx->ctx_skip = save_skip;
    }

    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ppconst_used = ppconst->pp_used;

    // get the first variable
    ଯଦି (compile_expr4(arg, cctx, ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // || and && work almost the same
    ଫେରନ୍ତୁ compile_and_or(arg, cctx, "&&", ppconst, ppconst_used);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ppconst_used = ppconst->pp_used;

    // eval the first expression
    ଯଦି (compile_expr3(arg, cctx, ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // || and && work almost the same
    ଫେରନ୍ତୁ compile_and_or(arg, cctx, "||", ppconst, ppconst_used);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ppconst_used = ppconst->pp_used;
    char_u	*next;

    // Ignore all kinds of errors when not producing code.
    ଯଦି (cctx->ctx_skip == SKIP_YES)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_did_emsg = did_emsg;

	skip_ପଦସମଷ୍ଟି_cctx(arg, cctx);
	ଫେରନ୍ତୁ did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    ଯଦି (compile_expr2(arg, cctx, ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    p = may_peek_next_ଧାଡ଼ି(cctx, *arg, &next);
    ଯଦି (*p == '?')
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		alt_idx = instr->ga_len;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_idx = 0;
	isn_T		*isn;
	type_T		*type1 = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		has_const_ପଦସମଷ୍ଟି = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		const_ମୂଲ୍ୟ = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_skip = cctx->ctx_skip;

	ଯଦି (next != ଖାଲି)
	{
	    *arg = next_ଧାଡ଼ି_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ଯଦି (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (ppconst->pp_used == ppconst_used + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_ପଦସମଷ୍ଟି = TRUE;
	    ଯଦି (op_falsy)
		const_ମୂଲ୍ୟ = tv2bool(&ppconst->pp_tv[ppconst_used]);
	    ଅନ୍ୟଥା
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;

		const_ମୂଲ୍ୟ = tv_get_bool_chk(&ppconst->pp_tv[ppconst_used],
								       &error);
		ଯଦି (error)
		    ଫେରନ୍ତୁ FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_ମୂଲ୍ୟ : !const_ମୂଲ୍ୟ) ? SKIP_YES : SKIP_NOT;

	    ଯଦି (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    ଅନ୍ୟଥା
	    {
		clear_tv(&ppconst->pp_tv[ppconst_used]);
		--ppconst->pp_used;
	    }
	}
	ଅନ୍ୟଥା
	{
	    generate_ppconst(cctx, ppconst);
	    ଯଦି (op_falsy)
		end_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    ଯଦି (op_falsy)
	    {
		type1 = get_type_on_ଗଦା(cctx, -1);
		ଯଦି (check_type_is_ମୂଲ୍ୟ(type1) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	ଯଦି (may_get_next_ଧାଡ଼ି_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (compile_expr1(arg, cctx, ppconst) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (!has_const_ପଦସମଷ୍ଟି)
	{
	    generate_ppconst(cctx, ppconst);

	    ଯଦି (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_ଗଦା(cctx, 0);
		--ଗଦା->ga_len;

		end_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_data) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	ଯଦି (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_next_ଧାଡ଼ି(cctx, *arg, &next);
	    ଯଦି (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (next != ଖାଲି)
	    {
		*arg = next_ଧାଡ଼ି_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ଯଦି (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		ଫେରନ୍ତୁ FAIL;
	    }

	    // evaluate the third expression
	    ଯଦି (has_const_ପଦସମଷ୍ଟି)
		cctx->ctx_skip = save_skip == SKIP_YES || const_ମୂଲ୍ୟ
							 ? SKIP_YES : SKIP_NOT;
	    ଯଦି (may_get_next_ଧାଡ଼ି_error(p + 1, arg, cctx) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (compile_expr1(arg, cctx, ppconst) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (!has_const_ପଦସମଷ୍ଟି)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)ଗଦା->ga_data)
					      + ଗଦା->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_data) + end_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr0_ext(char_u **arg,  cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *is_const)
{
    ppconst_T	ppconst;

    CLEAR_FIELD(ppconst);
    ଯଦି (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (is_const != ଖାଲି)
	*is_const = ppconst.pp_used > 0 || ppconst.pp_is_const;
    ଯଦି (generate_ppconst(cctx, &ppconst) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Toplevel expression.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    ଫେରନ୍ତୁ compile_expr0_ext(arg, cctx, ଖାଲି);
}


#endif // defined(FEAT_EVAL)
