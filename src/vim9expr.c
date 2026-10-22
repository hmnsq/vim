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

#నిర్వచించు USING_FLOAT_STUFF
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
స్థిర పూర్ణాంకం paren_follows_after_సమాసం = 0;

/*
 * Generate code for any ppconst entries.
 */
    పూర్ణాంకం
generate_ppconst(cctx_T *cctx, ppconst_T *ppconst)
{
    పూర్ణాంకం	    i;
    పూర్ణాంకం	    ret = OK;
    పూర్ణాంకం	    save_skip = cctx->ctx_skip;

    cctx->ctx_skip = SKIP_NOT;
    కొరకు (i = 0; i < ppconst->pp_వాడిన; ++i)
	ఒకవేళ (generate_tv_నెట్టు(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_వాడిన = 0;
    cctx->ctx_skip = save_skip;
    తిరిగి ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    స్థిర పూర్ణాంకం
check_ppconst_bool(ppconst_T *ppconst)
{
    ఒకవేళ (ppconst->pp_వాడిన > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_వాడిన - 1];
	where_T	    where = WHERE_INIT;

	తిరిగి check_typval_type(&t_bool, tv, where);
    }
    తిరిగి OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    శూన్య
clear_ppconst(ppconst_T *ppconst)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < ppconst->pp_వాడిన; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_వాడిన = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    స్థిర పూర్ణాంకం
compile_tuple_member(
    type2_T	*typep,
    పూర్ణాంకం		is_ముక్క,
    cctx_T	*cctx)
{
    ఒకవేళ (is_ముక్క)
    {
	ఒకవేళ (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    తిరిగి FAIL;
	// a copy is made so the member type is no longer declared
	ఒకవేళ (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	ఒకవేళ (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    ఒకవేళ (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    లేకపోతే
    {
	ఒకవేళ (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    ఒకవేళ (typep->type_curr->tt_argcount == 1)
	    {
		ఒకవేళ (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_వాదనలు[0]->tt_member;
		లేకపోతే
		    typep->type_curr = typep->type_curr->tt_వాదనలు[0];
	    }
	    లేకపోతే
		typep->type_curr = &t_any;
	    ఒకవేళ (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		ఒకవేళ (typep->type_decl->tt_argcount == 1)
		{
		    ఒకవేళ (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_వాదనలు[0]->tt_member;
		    లేకపోతే
			typep->type_decl = typep->type_decl->tt_వాదనలు[0];
		}
		లేకపోతే
		    typep->type_curr = &t_any;
	    }
	    లేకపోతే
		typep->type_decl = typep->type_curr;
	}
	ఒకవేళ (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    పూర్ణాంకం
compile_member(పూర్ణాంకం is_ముక్క, పూర్ణాంకం *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)దొంతర->ga_దత్తాంశం) + దొంతర->ga_len - (is_ముక్క ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)దొంతర->ga_దత్తాంశం) + దొంతర->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    ఒకవేళ ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    ఒకవేళ (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	ఒకవేళ (need_type(idxtype, &t_సంఖ్య, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (is_ముక్క)
	{
	    idxtype = get_type_on_దొంతర(cctx, 1);
	    ఒకవేళ (need_type(idxtype, &t_సంఖ్య, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		తిరిగి FAIL;
	}
    }

    ఒకవేళ (vartype == VAR_DICT)
    {
	ఒకవేళ (is_ముక్క)
	{
	    emsg(_(e_cannot_ముక్క_dictionary));
	    తిరిగి FAIL;
	}
	ఒకవేళ (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    ఒకవేళ (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    ఒకవేళ (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		ఒకవేళ (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    లేకపోతే
		typep->type_decl = typep->type_curr;
	}
	లేకపోతే
	{
	    ఒకవేళ (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		తిరిగి FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	ఒకవేళ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (keeping_dict != ఖాళీ)
	    *keeping_dict = TRUE;
    }
    లేకపోతే ఒకవేళ (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	ఒకవేళ ((is_ముక్క
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (vartype == VAR_BLOB)
    {
	ఒకవేళ (is_ముక్క)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    ఒకవేళ (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే
	{
	    typep->type_curr = &t_సంఖ్య;
	    typep->type_decl = &t_సంఖ్య;
	    ఒకవేళ (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		తిరిగి FAIL;
	}
    }
    లేకపోతే ఒకవేళ (vartype == VAR_TUPLE)
    {
	ఒకవేళ (compile_tuple_member(typep, is_ముక్క, cctx) == FAIL)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	ఒకవేళ (is_ముక్క)
	{
	    ఒకవేళ (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		తిరిగి FAIL;
	    // a copy is made so the member type is no longer declared
	    ఒకవేళ (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    ఒకవేళ (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		ఒకవేళ (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	లేకపోతే
	{
	    ఒకవేళ (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		ఒకవేళ (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		ఒకవేళ (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    ఒకవేళ (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		లేకపోతే
			typep->type_decl = typep->type_curr;
	    }
	    ఒకవేళ (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		తిరిగి FAIL;
	}
    }
    లేకపోతే
    {
	ఎంపిక (vartype)
	{
	    స్థితి VAR_FUNC:
	    స్థితి VAR_PARTIAL:
		emsg(_(e_cannot_సూచీ_a_funcref));
		ఆపు;
	    స్థితి VAR_BOOL:
	    స్థితి VAR_SPECIAL:
	    స్థితి VAR_పని:
	    స్థితి VAR_CHANNEL:
	    స్థితి VAR_INSTR:
	    స్థితి VAR_CLASS:
	    స్థితి VAR_OBJECT:
	    స్థితి VAR_TYPEALIAS:
	    స్థితి VAR_UNKNOWN:
	    స్థితి VAR_ANY:
	    స్థితి VAR_VOID:
		emsg(_(e_cannot_సూచీ_special_variable));
		ఆపు;
	    అప్రమేయ:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    స్థిర పూర్ణాంకం
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    కొరకు (cctx_T *cctx = cctx_arg; cctx != ఖాళీ; cctx = cctx->ctx_outer)
    {
	ఒకవేళ (cctx->ctx_ufunc != ఖాళీ && cctx->ctx_ufunc->uf_class != ఖాళీ)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    అయితే (clp != ఖాళీ)
	    {
		ఒకవేళ (clp == cl)
		    తిరిగి TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    తిరిగి FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    పూర్ణాంకం
compile_class_object_సూచీ(cctx_T *cctx, char_u **arg, type_T *type)
{
    పూర్ణాంకం		m_idx;
    పూర్ణాంకం		ret = FAIL;

    ఒకవేళ (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	తిరిగి FAIL;
    }

    class_T *cl = type->tt_class;
    పూర్ణాంకం is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    ఒకవేళ (type == &t_super)
    {
	ఒకవేళ (cctx->ctx_ufunc == ఖాళీ || cctx->ctx_ufunc->uf_class == ఖాళీ)
	{
	    emsg(_(e_using_super_not_లోపల_class_పద్ధతి));
	    తిరిగి FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_దొంతర.ga_len;
    }
    లేకపోతే ఒకవేళ (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	ఒకవేళ (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_దత్తాంశం) + instr->ga_len - 1;
	    ఒకవేళ (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    ఒకవేళ (cl == ఖాళీ)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	తిరిగి FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_చివర = కనుగొను_name_చివర(name, ఖాళీ, ఖాళీ, FNE_CHECK_ప్రారంభం);
    ఒకవేళ (name_చివర == name)
    {
	emsg(_(e_missing_name_after_dot));
	తిరిగి FAIL;
    }
    size_t len = name_చివర - name;

    gfargs_tab_T    gfatab;

    generic_func_వాదనలు_పట్టిక_init(&gfatab);

    ఒకవేళ (*name_చివర == '<')
    {
	// generic method call
	name_చివర = parse_generic_func_type_వాదనలు(name, len, name_చివర,
							&gfatab, cctx);
	ఒకవేళ (name_చివర == ఖాళీ)
	    వెళ్ళు పూర్తి;
    }

    ఒకవేళ (*name_చివర == '(')
    {
	పూర్ణాంకం	function_లెక్క;
	పూర్ణాంకం	బిడ్డ_లెక్క;
	ufunc_T	**functions;

	ఒకవేళ (type->tt_type == VAR_CLASS)
	{
	    function_లెక్క = cl->class_class_function_లెక్క;
	    బిడ్డ_లెక్క = cl->class_class_function_లెక్క_బిడ్డ;
	    functions = cl->class_class_functions;
	}
	లేకపోతే
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    ఒకవేళ (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_లెక్క = cl->class_obj_పద్ధతి_లెక్క;
	    బిడ్డ_లెక్క = cl->class_obj_పద్ధతి_లెక్క_బిడ్డ;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = ఖాళీ;
	పూర్ణాంకం fi;
	కొరకు (fi = is_super ? బిడ్డ_లెక్క : 0; fi < function_లెక్క; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    ఒకవేళ (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		ఆపు;
	    }
	}

	ocmember_T  *ocm = ఖాళీ;
	ఒకవేళ (ufunc == ఖాళీ)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    ఒకవేళ (ocm == ఖాళీ || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		పద్ధతి_not_దొరికింది_సందేశం(cl, type->tt_type, name, len);
		వెళ్ళు పూర్తి;
	    }
	    ఒకవేళ (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_దొంతర.ga_len;
		ఒకవేళ (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    వెళ్ళు పూర్తి;
	    }
	    లేకపోతే
	    {
		పూర్ణాంకం స్థితి_2;

		ఒకవేళ (IS_INTERFACE(cl))
		    స్థితి_2 = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		లేకపోతే
		    స్థితి_2 = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		ఒకవేళ (స్థితి_2 == FAIL)
		    వెళ్ళు పూర్తి;
	    }
	}

	ఒకవేళ (is_super && ufunc != ఖాళీ && IS_ABSTRACT_పద్ధతి(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_పద్ధతి_str_ప్రత్యక్ష), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    వెళ్ళు పూర్తి;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	ఒకవేళ (ocm == ఖాళీ && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_పద్ధతి_str), name);
	    వెళ్ళు పూర్తి;
	}

	// process generic function call
	ఒకవేళ (ufunc != ఖాళీ)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ఒకవేళ (ufunc == ఖాళీ)
		వెళ్ళు పూర్తి;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_చివర + 1);
	పూర్ణాంకం argcount = 0;
	ఒకవేళ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    వెళ్ళు పూర్తి;

	ఒకవేళ (ocm != ఖాళీ)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    వెళ్ళు పూర్తి;
	}
	ఒకవేళ (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	లేకపోతే
	    ret = generate_CALL(cctx, ufunc, ఖాళీ, 0, argcount, FALSE);
	వెళ్ళు పూర్తి;
    }

    ఒకవేళ (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	ఒకవేళ (m_idx >= 0)
	{
	    ఒకవేళ (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		వెళ్ళు పూర్తి;
	    }

	    *arg = name_చివర;
	    ఒకవేళ (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    లేకపోతే
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    వెళ్ళు పూర్తి;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_పద్ధతి_idx(cl, name, len);
	ఒకవేళ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    ఒకవేళ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_పద్ధతి_str), fp->uf_name);
		వెళ్ళు పూర్తి;
	    }
	    *arg = name_చివర;
	    // Remove the object type from the stack
	    --cctx->ctx_type_దొంతర.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, ఖాళీ);
	    వెళ్ళు పూర్తి;
	}

	member_not_దొరికింది_సందేశం(cl, VAR_OBJECT, name, len);
    }
    లేకపోతే
    {
	// load class member
	పూర్ణాంకం idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	ఒకవేళ (m != ఖాళీ)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    ఒకవేళ (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		వెళ్ళు పూర్తి;
	    }

	    *arg = name_చివర;
	    // Remove the class type from the stack
	    --cctx->ctx_type_దొంతర.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    వెళ్ళు పూర్తి;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_పద్ధతి_idx(cl, name, len);
	ఒకవేళ (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    ఒకవేళ (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_పద్ధతి_str), fp->uf_name);
		వెళ్ళు పూర్తి;
	    }
	    *arg = name_చివర;
	    // Remove the class type from the stack
	    --cctx->ctx_type_దొంతర.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, ఖాళీ);
	    వెళ్ళు పూర్తి;
	}

	member_not_దొరికింది_సందేశం(cl, VAR_CLASS, name, len);
    }

పూర్తి:
    generic_func_వాదనలు_పట్టిక_clear(&gfatab);
    తిరిగి ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    పూర్ణాంకం
compile_ఎక్కించు_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *ప్రారంభం,	    // start of variable
	char_u **చివర,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    పూర్ణాంకం		    idx;
    imported_T	    *imp;

    ఒకవేళ (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	తిరిగి FAIL;
    si = SCRIPT_అంశం(current_sctx.sc_sid);
    idx = get_script_అంశం_idx(current_sctx.sc_sid, name, 0, cctx, ఖాళీ);
    ఒకవేళ (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	తిరిగి OK;
    }

    ఒకవేళ (చివర == ఖాళీ)
	imp = ఖాళీ;
    లేకపోతే ఒకవేళ (import == ఖాళీ)
	imp = కనుగొను_imported(name, 0, FALSE);
    లేకపోతే
	imp = import;

    ఒకవేళ (imp != ఖాళీ)
    {
	char_u	*p = skipwhite(*చివర);
	char_u	*exp_name;
	పూర్ణాంకం	cc;
	ufunc_T	*ufunc = ఖాళీ;
	type_T	*type;
	పూర్ణాంకం	పూర్తి = FALSE;
	పూర్ణాంకం	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	ఒకవేళ (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), ప్రారంభం);
	    తిరిగి FAIL;
	}
	++p;
	ఒకవేళ (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    తిరిగి FAIL;
	}

	// isolate one name
	exp_name = p;
	అయితే (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_అంశం(imp->imp_sid);
	ఒకవేళ (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_ఆధారం(cctx, imp->imp_sid);

	ఒకవేళ (res == OK)
	{
	    ఒకవేళ (si->sn_autoload_ఉపసర్గ != ఖాళీ
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_ఉపసర్గ, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		ఒకవేళ (cc == '(' || paren_follows_after_సమాసం)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		లేకపోతే
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		పూర్తి = TRUE;
	    }
	    లేకపోతే ఒకవేళ (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		ఒకవేళ (cc == '(' || paren_follows_after_సమాసం)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		లేకపోతే
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		పూర్తి = TRUE;
	    }
	    లేకపోతే
	    {
		idx = కనుగొను_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, ఖాళీ, TRUE);
	    }
	}

	*p = cc;
	*చివర = p;
	ఒకవేళ (పూర్తి)
	    తిరిగి res;

	ఒకవేళ (idx < 0)
	{
	    ఒకవేళ (ufunc != ఖాళీ)
	    {
		gfargs_tab_T	gfatab;

		generic_func_వాదనలు_పట్టిక_init(&gfatab);

		ఒకవేళ (IS_GENERIC_FUNC(ufunc))
		{
		    ఒకవేళ (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_వాదనలు(name, STRLEN(name), p,
								&gfatab, cctx);
			ఒకవేళ (p != ఖాళీ)
			{
			    *చివర = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_వాదనలు_పట్టిక_clear(&gfatab);

			ఒకవేళ (p == ఖాళీ || ufunc == ఖాళీ)
			    తిరిగి FAIL;
		    }
		    లేకపోతే
		    {
			emsg_funcname(e_generic_func_missing_type_వాదనలు_str,
									name);
			తిరిగి FAIL;
		    }
		}
		లేకపోతే ఒకవేళ (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    తిరిగి FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, ఖాళీ, TRUE);
		తిరిగి OK;
	    }
	    తిరిగి FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	తిరిగి OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    తిరిగి generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    స్థిర పూర్ణాంకం
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    పూర్ణాంకం			has_g_ఉపసర్గ)
{
    ufunc_T *ufunc = కనుగొను_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    ఒకవేళ (ufunc == ఖాళీ || (!has_g_ఉపసర్గ && func_requires_g_ఉపసర్గ(ufunc)))
	తిరిగి FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    ఒకవేళ (ufunc == ఖాళీ)
	తిరిగి FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    ఒకవేళ (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, ఖాళీ) == FAIL)
	తిరిగి FAIL;
    తిరిగి generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    స్థిర పూర్ణాంకం
compiling_a_class_పద్ధతి(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    తిరిగి cctx->ctx_ufunc != ఖాళీ
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == ఖాళీ
		|| cctx->ctx_outer->ctx_ufunc == ఖాళీ
		|| cctx->ctx_outer->ctx_ufunc->uf_class == ఖాళీ
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
    స్థిర పూర్ణాంకం
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    పూర్ణాంకం			outer_లోతు)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    పూర్ణాంకం		isn_idx;

    ఒకవేళ (fp == ఖాళీ)
	తిరిగి FAIL;

    కొరకు (పూర్ణాంకం లోతు = 0; లోతు < outer_లోతు; ++లోతు)
    {
	ఒకవేళ (outer_cctx->ctx_outer == ఖాళీ)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    తిరిగి FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    ఒకవేళ (fp->uf_ఖండం_ids == ఖాళీ && ufunc->uf_ఖండం_లోతు > 0)
    {
	fp->uf_ఖండం_ids = ALLOC_MULT(పూర్ణాంకం, ufunc->uf_ఖండం_లోతు);
	ఒకవేళ (fp->uf_ఖండం_ids != ఖాళీ)
	{
	    mch_memmove(fp->uf_ఖండం_ids, ufunc->uf_ఖండం_ids,
					    పరిమాణం(పూర్ణాంకం) * ufunc->uf_ఖండం_లోతు);
	    fp->uf_ఖండం_లోతు = ufunc->uf_ఖండం_లోతు;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    ఒకవేళ (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (generate_FUNCREF(cctx, fp, ఖాళీ, FALSE, 0, &isn_idx) == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	తిరిగి OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    ఒకవేళ (outer_లోతు > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_దత్తాంశం) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	ఒకవేళ (extra == ఖాళీ)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    ఒకవేళ (extra == ఖాళీ)
		తిరిగి FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_లోతు = outer_లోతు;
    }

    తిరిగి OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    పూర్ణాంకం
compile_ఎక్కించు(
	char_u **arg,
	size_t  namelen,
	char_u *చివర_arg,
	cctx_T	*cctx,
	పూర్ణాంకం	is_సమాసం,
	పూర్ణాంకం	error)
{
    type_T	*type;
    char_u	*name = ఖాళీ;
    char_u	*చివర = చివర_arg;
    పూర్ణాంకం		res = FAIL;
    పూర్ణాంకం		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = ఖాళీ;

    generic_func_వాదనలు_పట్టిక_init(&gfatab);

    ఒకవేళ (*(*arg + namelen) == '<')
    {
	// generic function call
	ఒకవేళ (parse_generic_func_type_వాదనలు(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == ఖాళీ)
	{
	    generic_func_వాదనలు_పట్టిక_clear(&gfatab);
	    తిరిగి FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    ఒకవేళ (*(*arg + 1) == ':')
    {
	ఒకవేళ (చివర <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    ఎంపిక (**arg)
	    {
		స్థితి 'g': isn_type = ISN_LOADGDICT; ఆపు;
		స్థితి 'w': isn_type = ISN_LOADWDICT; ఆపు;
		స్థితి 't': isn_type = ISN_LOADTDICT; ఆపు;
		స్థితి 'b': isn_type = ISN_LOADBDICT; ఆపు;
		అప్రమేయ:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    వెళ్ళు theend;
	    }
	    ఒకవేళ (generate_instr_type(cctx, isn_type, &t_dict_any) == ఖాళీ)
		వెళ్ళు theend;
	    res = OK;
	}
	లేకపోతే
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, చివర - (*arg + 2));
	    ఒకవేళ (name == ఖాళీ)
		వెళ్ళు theend;

	    ఎంపిక (**arg)
	    {
		స్థితి 'v': res = generate_LOADV(cctx, name);
			  ఆపు;
		స్థితి 's': ఒకవేళ (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_లోపల_vim9_script_str),
									 *arg);
			      వెళ్ళు theend;
			  }
			  ఒకవేళ (is_సమాసం && కనుగొను_func(name, FALSE) != ఖాళీ)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  లేకపోతే
			      res = compile_ఎక్కించు_scriptvar(cctx, name,
							    ఖాళీ, &చివర, ఖాళీ);
			  ఆపు;
		స్థితి 'g': ఒకవేళ (vim_strchr(name, AUTOLOAD_CHAR) == ఖాళీ)
			  {
			      ఒకవేళ (is_సమాసం && ASCII_ISUPPER(*name)
				       && (కనుగొను_func(name, TRUE) != ఖాళీ
					   || gfatab.gfat_వాదనలు.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      లేకపోతే
				  isn_type = ISN_LOADG;
			  }
			  లేకపోతే
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, చివర - *arg);
			      ఒకవేళ (name == ఖాళీ)
				  వెళ్ళు theend;
			  }
			  ఆపు;
		స్థితి 'w': isn_type = ISN_LOADW; ఆపు;
		స్థితి 't': isn_type = ISN_LOADT; ఆపు;
		స్థితి 'b': isn_type = ISN_LOADB; ఆపు;
		అప్రమేయ:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  వెళ్ళు theend;
	    }
	    ఒకవేళ (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_ఎక్కించు(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    లేకపోతే
    {
	పూర్ణాంకం	    idx;
	పూర్ణాంకం	    పద్ధతి_idx;
	పూర్ణాంకం	    gen_ఎక్కించు = FALSE;
	పూర్ణాంకం	    gen_ఎక్కించు_outer = 0;
	పూర్ణాంకం	    outer_loop_లోతు = -1;
	పూర్ణాంకం	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	ఒకవేళ (name == ఖాళీ)
	    వెళ్ళు theend;

	ఒకవేళ (STRCMP(name, "super") == 0 && compiling_a_class_పద్ధతి(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = నెట్టు_type_దొంతర(cctx, &t_super);
	    ఒకవేళ (*చివర != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	లేకపోతే ఒకవేళ (vim_strchr(name, AUTOLOAD_CHAR) != ఖాళీ)
	{
	    script_autoload(name, FALSE);
	    res = generate_ఎక్కించు(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	లేకపోతే ఒకవేళ (arg_exists(*arg, namelen, &idx, &type, &gen_ఎక్కించు_outer, cctx)
									 == OK)
	{
	    ఒకవేళ (gen_ఎక్కించు_outer == 0)
		gen_ఎక్కించు = TRUE;
	}
	లేకపోతే
	{
	    lvar_T  lvar;
	    class_T *cl = ఖాళీ;

	    ఒకవేళ (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		ఒకవేళ (lvar.lv_ufunc != ఖాళీ && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_వాదనలు.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		లేకపోతే ఒకవేళ (lvar.lv_from_outer != 0)
		{
		    gen_ఎక్కించు_outer = lvar.lv_from_outer;
		    outer_loop_లోతు = lvar.lv_loop_లోతు;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		లేకపోతే
		    gen_ఎక్కించు = TRUE;
	    }
	    లేకపోతే ఒకవేళ (cctx->ctx_ufunc->uf_defclass != ఖాళీ &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((పద్ధతి_idx =
			     cctx_class_పద్ధతి_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		ఒకవేళ (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    ఒకవేళ (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    లేకపోతే
		    {
			ufunc_T *fp = cl->class_class_functions[పద్ధతి_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, పద్ధతి_idx,
									ఖాళీ);
		    }
		}
		లేకపోతే
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    లేకపోతే
	    {
		imported_T *imp = ఖాళీ;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		ఒకవేళ (script_var_exists(*arg, namelen, cctx, ఖాళీ) == OK
			    || (imp = కనుగొను_imported(name, 0, FALSE)) != ఖాళీ)
		    res = compile_ఎక్కించు_scriptvar(cctx, name, *arg, &చివర, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		ఒకవేళ (res == FAIL && is_సమాసం && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	ఒకవేళ (gen_ఎక్కించు)
	    res = generate_ఎక్కించు(cctx, ISN_ఎక్కించు, idx, ఖాళీ, type);
	ఒకవేళ (gen_ఎక్కించు_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_ఎక్కించు_outer,
				       outer_loop_లోతు, outer_loop_idx, type);
	    cctx->ctx_outer_వాడిన = TRUE;
	}
    }

    *arg = చివర;

theend:
    ఒకవేళ (lt != ఖాళీ)
	*lt = '<';
    ఒకవేళ (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_దొరికింది_str), name);
    vim_free(name);
    generic_func_వాదనలు_పట్టిక_clear(&gfatab);
    తిరిగి res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    స్థిర పూర్ణాంకం
compile_string(isn_T *isn, cctx_T *cctx, పూర్ణాంకం str_స్థానభ్రంశం)
{
    char_u	*s = isn->isn_arg.string + str_స్థానభ్రంశం;
    garray_T	save_ga = cctx->ctx_instr;
    పూర్ణాంకం		సమాసం_res;
    పూర్ణాంకం		trailing_error;
    పూర్ణాంకం		instr_లెక్క;
    isn_T	*instr = ఖాళీ;

    // Remove the string type from the stack.
    --cctx->ctx_type_దొంతర.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_దత్తాంశం = ఖాళీ;

    // avoid peeking a next line
    పూర్ణాంకం galen_save = cctx->ctx_ufunc->uf_పంక్తులు.ga_len;
    cctx->ctx_ufunc->uf_పంక్తులు.ga_len = 0;

    సమాసం_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_పంక్తులు.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    ఒకవేళ (సమాసం_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	ఒకవేళ (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_దొంతర.ga_len;
	తిరిగి FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_లెక్క = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_దత్తాంశం;
    instr[instr_లెక్క].isn_type = ISN_ముగింపు;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    తిరిగి OK;
}

/*
 * Compile the argument expressions.
 * "arg" points to just after the "(" and is advanced to after the ")"
 */
    పూర్ణాంకం
compile_arguments(
	char_u	     **arg,
	cctx_T	     *cctx,
	పూర్ణాంకం	     *argcount,
	ca_special_T special_fn)
{
    char_u  *p = *arg;
    char_u  *whitep = *arg;
    పూర్ణాంకం	    must_చివర = FALSE;
    పూర్ణాంకం	    instr_లెక్క;

    కొరకు (;;)
    {
	ఒకవేళ (may_get_తదుపరి_పంక్తి(whitep, &p, cctx) == FAIL)
	    వెళ్ళు failret;
	ఒకవేళ (*p == ')')
	{
	    *arg = p + 1;
	    తిరిగి OK;
	}
	ఒకవేళ (must_చివర)
	{
	    semsg(_(e_missing_comma_before_వాదం_str), p);
	    తిరిగి FAIL;
	}

	instr_లెక్క = cctx->ctx_instr.ga_len;
	ఒకవేళ (compile_expr0(&p, cctx) == FAIL)
	    తిరిగి FAIL;
	++*argcount;

	ఒకవేళ (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_లెక్క + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_దత్తాంశం) + instr_లెక్క;

	    // {skip} argument of searchpair() can be compiled if not empty
	    ఒకవేళ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ఖాళీ
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	లేకపోతే ఒకవేళ (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_లెక్క + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_దత్తాంశం) + instr_లెక్క;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    ఒకవేళ (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != ఖాళీ
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	ఒకవేళ (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    తిరిగి FAIL;
	}
	ఒకవేళ (*p == ',')
	{
	    ++p;
	    ఒకవేళ (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		తిరిగి FAIL;
	    }
	}
	లేకపోతే
	    must_చివర = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    తిరిగి FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    స్థిర పూర్ణాంకం
compile_builtin_పద్ధతి_call(cctx_T *cctx, class_builtin_T builtin_పద్ధతి)
{
    type_T	*type = get_decl_type_on_దొంతర(cctx, 0);
    పూర్ణాంకం		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    ఒకవేళ (type->tt_type == VAR_OBJECT)
    {
	పూర్ణాంకం	పద్ధతి_idx;
	ufunc_T *uf = class_get_builtin_పద్ధతి(type->tt_class, builtin_పద్ధతి,
							&పద్ధతి_idx);
	ఒకవేళ (uf != ఖాళీ)
	    res = generate_CALL(cctx, uf, type->tt_class, పద్ధతి_idx, 0, FALSE);
    }

    తిరిగి res;
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
    స్థిర పూర్ణాంకం
compile_call(
	char_u	    **arg,
	size_t	    varlen,
	cctx_T	    *cctx,
	ppconst_T   *ppconst,
	పూర్ణాంకం	    argcount_init)
{
    char_u	*name = *arg;
    char_u	*p;
    పూర్ణాంకం		argcount = argcount_init;
    char_u	namebuf[MAX_FUNC_NAME_LEN];
    char_u	fname_buf[FLEN_FIXED + 1];
    char_u	*tofree = ఖాళీ;
    char_u	*gname = ఖాళీ;
    ufunc_T	*ufunc = ఖాళీ;
    పూర్ణాంకం		res = FAIL;
    పూర్ణాంకం		is_autoload;
    పూర్ణాంకం		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    ఒకవేళ (varlen >= పరిమాణం(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	తిరిగి FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = కనుగొను_imported(name, varlen, FALSE);
    ఒకవేళ (import != ఖాళీ)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	తిరిగి FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    ఒకవేళ ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	పూర్ణాంకం	    is_has = **arg == 'h';
	పూర్ణాంకం	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	ఒకవేళ (*s == '"')
	    (శూన్య)eval_string(&s, &argvars[0], TRUE, FALSE);
	లేకపోతే ఒకవేళ (*s == '\'')
	    (శూన్య)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	ఒకవేళ (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_వాడిన];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_సంఖ్య;
		tv->vval.v_సంఖ్య = 0;
		ఒకవేళ (is_has)
		    f_has(argvars, tv);
		లేకపోతే ఒకవేళ (is_len)
		    f_len(argvars, tv);
		లేకపోతే
		    f_exists(argvars, tv);
		++ppconst->pp_వాడిన;
	    }
	    clear_tv(&argvars[0]);
	    తిరిగి OK;
	}
	clear_tv(&argvars[0]);
	ఒకవేళ (!is_has && !is_len)
	{
	    emsg(_(e_వాదం_యొక్క_exists_compiled_must_be_literal_string));
	    తిరిగి FAIL;
	}
    }

    ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
	తిరిగి FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    ఒకవేళ ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    లేకపోతే ఒకవేళ (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    లేకపోతే
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_వాదనలు_పట్టిక_init(&gfatab);

    ఒకవేళ (*(*arg + varlen) == '<')
    {
	char_u	*ప్రారంభం = *arg;

	// generic function
	*arg = parse_generic_func_type_వాదనలు(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	ఒకవేళ (*arg == ఖాళీ)
	    వెళ్ళు theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(ప్రారంభం, *arg - ప్రారంభం);
	ఒకవేళ (gname == ఖాళీ)
	    వెళ్ళు theend;
	++*arg;			// skip '('
    }
    లేకపోతే
	*arg += varlen + 1;

    ఒకవేళ (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	వెళ్ళు theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != ఖాళీ;
    ఒకవేళ (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	పూర్ణాంకం	    idx;

	// builtin function
	idx = కనుగొను_అంతర్గత_func(name);
	ఒకవేళ (idx >= 0)
	{
	    ఒకవేళ (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_లోపల_vim9_script));
		వెళ్ళు theend;
	    }

	    ఒకవేళ (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_దొంతర(cctx, 1);
		ఒకవేళ (check_type_is_విలువ(get_type_on_దొంతర(cctx, 0)) == FAIL)
		    వెళ్ళు theend;

		// add() can be compiled to instructions if we know the type
		ఒకవేళ (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		లేకపోతే ఒకవేళ (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    ఒకవేళ ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		ఒకవేళ (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_పద్ధతి = CLASS_BUILTIN_INVALID;
	    ఒకవేళ (STRCMP(name, "string") == 0)
		builtin_పద్ధతి = CLASS_BUILTIN_STRING;
	    లేకపోతే ఒకవేళ (STRCMP(name, "empty") == 0)
		builtin_పద్ధతి = CLASS_BUILTIN_ఖాళీలు;
	    లేకపోతే ఒకవేళ (STRCMP(name, "len") == 0)
		builtin_పద్ధతి = CLASS_BUILTIN_LEN;
	    ఒకవేళ (builtin_పద్ధతి != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_పద్ధతి_call(cctx, builtin_పద్ధతి);
		ఒకవేళ (res == OK)
		    idx = -1;
	    }

	    ఒకవేళ (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	లేకపోతే
	    emsg_funcname(e_unknown_function_str, namebuf);
	వెళ్ళు theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    ఒకవేళ (lookup_local(namebuf, varlen, ఖాళీ, cctx) == FAIL
	    && arg_exists(namebuf, varlen, ఖాళీ, ఖాళీ, ఖాళీ, cctx) != OK)
    {
	class_T		*cl = ఖాళీ;
	పూర్ణాంకం		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = కనుగొను_func(name, FALSE);
	ఒకవేళ (ufunc != ఖాళీ)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    ఒకవేళ (ufunc == ఖాళీ)
		వెళ్ళు theend;

	    ఒకవేళ (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, ఖాళీ, 0, argcount, FALSE);
		వెళ్ళు theend;
	    }
	    ఒకవేళ (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == ఖాళీ
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		వెళ్ళు theend;
	    }
	}
	లేకపోతే ఒకవేళ ((mi = cctx_class_పద్ధతి_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    ఒకవేళ (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], ఖాళీ,
							0, argcount, FALSE);
	    }
	    లేకపోతే
	    {
		semsg(_(e_class_పద్ధతి_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    వెళ్ళు theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == ఖాళీ && gname != ఖాళీ ? gname : namebuf;
    ఒకవేళ (!has_g_namespace && !is_autoload
	    && compile_ఎక్కించు(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_దొంతర(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	వెళ్ళు theend;
    }

    // If we can find a global function by name generate the right call.
    ఒకవేళ (ufunc != ఖాళీ)
    {
	res = generate_CALL(cctx, ufunc, ఖాళీ, 0, argcount, FALSE);
	వెళ్ళు theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    ఒకవేళ (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    లేకపోతే
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_వాదనలు_పట్టిక_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    తిరిగి res;
}

// like NAMESPACE_CHAR but with 'a' and 'l'.
#నిర్వచించు VIM9_NAMESPACE_CHAR	(char_u *)"bgstvw"

/*
 * Find the end of a variable or function name.  Unlike find_name_end() this
 * does not recognize magic braces.
 * When "use_namespace" is TRUE recognize "b:", "s:", etc.
 * Return a pointer to just after the name.  Equal to "arg" if there is no
 * valid name.
 */
    char_u *
గమ్యం_name_చివర(char_u *arg, పూర్ణాంకం use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    ఒకవేళ (!eval_isnamec1(*arg))
	తిరిగి arg;

    కొరకు (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	ఒకవేళ (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == ఖాళీ))
	    ఆపు;
    తిరిగి p;
}

/*
 * Like to_name_end() but also skip over a list or dict constant.
 * Also accept "<SNR>123_Func".
 * This intentionally does not handle line continuation.
 */
    char_u *
గమ్యం_name_const_చివర(char_u *arg)
{
    char_u	*p = arg;
    typval_T	rettv;

    ఒకవేళ (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = గమ్యం_name_చివర(p, TRUE);
    ఒకవేళ (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	ఒకవేళ (eval_list(&p, &rettv, ఖాళీ, FALSE) == FAIL)
	    p = arg;
    }
    తిరిగి p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    స్థిర పూర్ణాంకం
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    పూర్ణాంకం		లెక్క = 0;
    పూర్ణాంకం		is_const;
    పూర్ణాంకం		is_all_const = TRUE;	// reset when non-const encountered
    పూర్ణాంకం		must_చివర = FALSE;

    కొరకు (;;)
    {
	ఒకవేళ (may_get_తదుపరి_పంక్తి(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_చివర_యొక్క_list_rsb_str), *arg);
	    తిరిగి FAIL;
	}
	ఒకవేళ (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    తిరిగి FAIL;
	}
	ఒకవేళ (*p == ']')
	{
	    ++p;
	    ఆపు;
	}
	ఒకవేళ (must_చివర)
	{
	    semsg(_(e_missing_comma_లోపల_list_str), p);
	    తిరిగి FAIL;
	}
	ఒకవేళ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (!is_const)
	    is_all_const = FALSE;
	++లెక్క;
	ఒకవేళ (*p == ',')
	{
	    ++p;
	    ఒకవేళ (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		తిరిగి FAIL;
	    }
	}
	లేకపోతే
	    must_చివర = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    తిరిగి generate_NEWLIST(cctx, లెక్క, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    స్థిర పూర్ణాంకం
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    పూర్ణాంకం మొదటి_అంశం_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    పూర్ణాంకం		లెక్క = 0;
    పూర్ణాంకం		is_const;
    పూర్ణాంకం		is_all_const = TRUE;	// reset when non-const encountered
    పూర్ణాంకం		must_చివర = FALSE;

    ఒకవేళ (**arg != ')')
    {
	ఒకవేళ (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    తిరిగి FAIL;
	}
	లెక్క = 1;	// the first tuple item is already processed
	is_all_const = మొదటి_అంశం_const;
	కొరకు (;;)
	{
	    ఒకవేళ (may_get_తదుపరి_పంక్తి(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_చివర_యొక్క_tuple_rsp_str), *arg);
		తిరిగి FAIL;
	    }
	    ఒకవేళ (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		తిరిగి FAIL;
	    }
	    ఒకవేళ (*p == ')')
	    {
		++p;
		ఆపు;
	    }
	    ఒకవేళ (must_చివర)
	    {
		semsg(_(e_missing_comma_లోపల_tuple_str), p);
		తిరిగి FAIL;
	    }
	    ఒకవేళ (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (!is_const)
		is_all_const = FALSE;
	    ++లెక్క;
	    ఒకవేళ (*p == ',')
	    {
		++p;
		ఒకవేళ (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    తిరిగి FAIL;
		}
	    }
	    లేకపోతే
		must_చివర = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    తిరిగి generate_NEWTUPLE(cctx, లెక్క, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    స్థిర శూన్య
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*పంక్తి;
    size_t	off;

    ఒకవేళ (!evalarg->eval_using_cmdline || cctx == ఖాళీ)
	తిరిగి;

    gap = &evalarg->eval_tofree_ga;
    ఒకవేళ (gap->ga_len == 0)
	తిరిగి;

    off = *arg - ((char_u **)gap->ga_దత్తాంశం)[gap->ga_len - 1];
    పంక్తి = ((char_u **)cctx->ctx_ufunc->uf_పంక్తులు.ga_దత్తాంశం)[cctx->ctx_lnum];
    *arg = పంక్తి + off;
    evalarg->eval_using_cmdline = FALSE;
}

/*
 * Parse a lambda: "(arg, arg) => expr"
 * "*arg" points to the '('.
 * When "ppconst" is not NULL the constants in it are generated before the
 * lambda, which goes on top of them on the stack.
 * Returns OK/FAIL when a lambda is recognized, NOTDONE if it's not a lambda.
 */
    పూర్ణాంకం
compile_lambda(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    పూర్ణాంకం		r;
    typval_T	rettv;
    ufunc_T	*ufunc;
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_flags = EVAL_EVALUATE;
    evalarg.eval_cctx = cctx;

    // Get the funcref in "rettv".
    r = get_lambda_tv(arg, &rettv, TRUE, &evalarg, cctx);
    ఒకవేళ (r != OK)
    {
	clear_evalarg(&evalarg, ఖాళీ);
	తిరిగి r;
    }

    ఒకవేళ (ppconst != ఖాళీ && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, ఖాళీ);
	తిరిగి FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    ఒకవేళ (cctx->ctx_ufunc != ఖాళీ)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	పూర్ణాంకం ఖండం_లోతు = cctx->ctx_ufunc->uf_ఖండం_లోతు;

	ఒకవేళ (ఖండం_లోతు > 0)
	{
	    ufunc->uf_ఖండం_ids = ALLOC_MULT(పూర్ణాంకం, ఖండం_లోతు);
	    ఒకవేళ (ufunc->uf_ఖండం_ids != ఖాళీ)
	    {
		mch_memmove(ufunc->uf_ఖండం_ids, cctx->ctx_ufunc->uf_ఖండం_ids,
						    పరిమాణం(పూర్ణాంకం) * ఖండం_లోతు);
		ufunc->uf_ఖండం_లోతు = ఖండం_లోతు;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ఒకవేళ (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    ఒకవేళ (cctx->ctx_compile_type == CT_DEBUG
#నిర్వచితమైతే FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#అయితే_ముగింపు
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    ఒకవేళ (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	ఒకవేళ (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, ఖాళీ);

    ఒకవేళ (ufunc->uf_def_స్థితి == UF_COMPILED)
    {
	// The return type will now be known.
	సమితి_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	తిరిగి generate_FUNCREF(cctx, ufunc, ఖాళీ, FALSE, 0, ఖాళీ);
    }

    func_ptr_unref(ufunc);
    తిరిగి FAIL;
}

/*
 * Get a lambda and compile it.  Uses Vim9 syntax.
 */
    పూర్ణాంకం
get_lambda_tv_and_compile(
	char_u	    **arg,
	typval_T    *rettv,
	పూర్ణాంకం	    types_optional,
	evalarg_T   *evalarg)
{
    పూర్ణాంకం		r;
    ufunc_T	*ufunc;
    పూర్ణాంకం		save_sc_సంస్కరణ = current_sctx.sc_సంస్కరణ;

    // Get the funcref in "rettv".
    current_sctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, ఖాళీ);
    current_sctx.sc_సంస్కరణ = save_sc_సంస్కరణ;
    ఒకవేళ (r != OK)
	తిరిగి r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    ఒకవేళ (ufunc->uf_ret_type == ఖాళీ || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, ఖాళీ);

    ఒకవేళ (ufunc->uf_def_స్థితి == UF_COMPILED)
    {
	// The return type will now be known.
	సమితి_function_type(ufunc);
	తిరిగి OK;
    }
    clear_tv(rettv);
    తిరిగి FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    స్థిర పూర్ణాంకం
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    పూర్ణాంకం		లెక్క = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*అంశం;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    పూర్ణాంకం		is_const;
    పూర్ణాంకం		is_all_const = TRUE;	// reset when non-const encountered

    ఒకవేళ (d == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	తిరిగి FAIL;
    }
    కొరకు (;;)
    {
	char_u	    *కీలకం = ఖాళీ;

	ఒకవేళ (may_get_తదుపరి_పంక్తి(whitep, arg, cctx) == FAIL)
	{
	    *arg = ఖాళీ;
	    వెళ్ళు failret;
	}

	ఒకవేళ (**arg == '}')
	    ఆపు;

	ఒకవేళ (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    ఒకవేళ (compile_expr0(arg, cctx) == FAIL)
		వెళ్ళు failret;
	    isn = ((isn_T *)instr->ga_దత్తాంశం) + instr->ga_len - 1;
	    ఒకవేళ (isn->isn_type == ISN_PUSHNR)
	    {
		అక్షరం buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.సంఖ్య);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    ఒకవేళ (isn->isn_type == ISN_PUSHS)
		కీలకం = isn->isn_arg.string;
	    లేకపోతే ఒకవేళ (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		వెళ్ళు failret;
	    *arg = skipwhite(*arg);
	    ఒకవేళ (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_కీలకం));
		వెళ్ళు failret;
	    }
	    ++*arg;
	}
	లేకపోతే
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    కీలకం = get_literal_కీలకం(arg);
	    ఒకవేళ (కీలకం == ఖాళీ)
		వెళ్ళు failret;
	    ఒకవేళ (generate_PUSHS(cctx, &కీలకం) == FAIL)
		వెళ్ళు failret;
	}

	// Check for duplicate keys, if using string keys.
	ఒకవేళ (కీలకం != ఖాళీ)
	{
	    అంశం = dict_కనుగొను(d, కీలకం, -1);
	    ఒకవేళ (అంశం != ఖాళీ)
	    {
		semsg(_(e_duplicate_కీలకం_లోపల_dictionary_str), కీలకం);
		వెళ్ళు failret;
	    }
	    అంశం = dictitem_alloc(కీలకం);
	    ఒకవేళ (అంశం != ఖాళీ)
	    {
		అంశం->di_tv.v_type = VAR_UNKNOWN;
		అంశం->di_tv.v_తాళం = 0;
		ఒకవేళ (dict_కూడు(d, అంశం) == FAIL)
		    dictitem_free(అంశం);
	    }
	}

	ఒకవేళ (**arg != ':')
	{
	    ఒకవేళ (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    లేకపోతే
		semsg(_(e_missing_colon_లోపల_dictionary_str), *arg);
	    వెళ్ళు failret;
	}
	whitep = *arg + 1;
	ఒకవేళ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    వెళ్ళు failret;
	}

	ఒకవేళ (may_get_తదుపరి_పంక్తి(whitep, arg, cctx) == FAIL)
	{
	    *arg = ఖాళీ;
	    వెళ్ళు failret;
	}

	ఒకవేళ (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    వెళ్ళు failret;
	ఒకవేళ (!is_const)
	    is_all_const = FALSE;
	++లెక్క;

	whitep = *arg;
	ఒకవేళ (may_get_తదుపరి_పంక్తి(whitep, arg, cctx) == FAIL)
	{
	    *arg = ఖాళీ;
	    వెళ్ళు failret;
	}
	ఒకవేళ (**arg == '}')
	    ఆపు;
	ఒకవేళ (**arg != ',')
	{
	    semsg(_(e_missing_comma_లోపల_dictionary_str), *arg);
	    వెళ్ళు failret;
	}
	ఒకవేళ (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    వెళ్ళు failret;
	}
	whitep = *arg + 1;
	ఒకవేళ (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    వెళ్ళు failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    ఒకవేళ (VIM_ISWHITE(**arg) && vim9_comment_ప్రారంభం(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    తిరిగి generate_NEWDICT(cctx, లెక్క, FALSE);

failret:
    ఒకవేళ (*arg == ఖాళీ)
    {
	semsg(_(e_missing_dict_చివర_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    తిరిగి FAIL;
}

/*
 * Compile "&option".
 */
    స్థిర పూర్ణాంకం
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*ప్రారంభం = *arg;
    పూర్ణాంకం		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    ఒకవేళ (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(ప్రారంభం, *arg - ప్రారంభం);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_సంఖ్య ? &t_సంఖ్య : &t_string;

	ret = generate_ఎక్కించు(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    తిరిగి ret;
}

/*
 * Compile "$VAR".
 */
    స్థిర పూర్ణాంకం
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*ప్రారంభం = *arg;
    పూర్ణాంకం		len;
    పూర్ణాంకం		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    ఒకవేళ (len == 0)
    {
	semsg(_(e_syntax_error_at_str), ప్రారంభం);
	తిరిగి FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(ప్రారంభం, len + 1);
    ret = generate_ఎక్కించు(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    తిరిగి ret;
}

/*
 * Compile $"string" or $'string'.
 */
    స్థిర పూర్ణాంకం
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    పూర్ణాంకం		ret;
    పూర్ణాంకం		quote;
    పూర్ణాంకం		evaluate = cctx->ctx_skip != SKIP_YES;
    పూర్ణాంకం		లెక్క = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    కొరకు (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	ఒకవేళ (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	లేకపోతే
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	ఒకవేళ (ret == FAIL)
	    ఆపు;
	ఒకవేళ (evaluate)
	{
	    ఒకవేళ ((tv.vval.v_string != ఖాళీ && *tv.vval.v_string != NUL)
		    || (**arg != '{' && లెక్క == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		ఒకవేళ (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    తిరిగి FAIL;
		tv.vval.v_string = ఖాళీ;  // don't free it now
		++లెక్క;
	    }
	    clear_tv(&tv);
	}

	ఒకవేళ (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    ఆపు;
	}

	p = compile_one_సమాసం_లోపల_str(*arg, cctx);
	ఒకవేళ (p == ఖాళీ)
	{
	    ret = FAIL;
	    ఆపు;
	}
	++లెక్క;
	*arg = p;
    }

    ఒకవేళ (ret == FAIL || !evaluate)
	తిరిగి ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    ఒకవేళ (లెక్క > 1)
	తిరిగి generate_CONCAT(cctx, లెక్క);

    తిరిగి OK;
}

/*
 * Compile "@r".
 */
    స్థిర పూర్ణాంకం
compile_get_రిజిస్టరు(char_u **arg, cctx_T *cctx)
{
    పూర్ణాంకం		ret;

    ++*arg;
    ఒకవేళ (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	తిరిగి FAIL;
    }
    ఒకవేళ (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	తిరిగి FAIL;
    }
    ret = generate_ఎక్కించు(cctx, ISN_LOADREG, **arg, ఖాళీ, &t_string);
    ++*arg;
    తిరిగి ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    స్థిర పూర్ణాంకం
apply_leader(typval_T *rettv, పూర్ణాంకం numeric_only, char_u *ప్రారంభం, char_u **చివర)
{
    char_u *p = *చివర;

    // this works from end to start
    అయితే (p > ప్రారంభం)
    {
	--p;
	ఒకవేళ (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    ఒకవేళ (rettv->v_type == VAR_FLOAT)
	    {
		ఒకవేళ (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    లేకపోతే
	    {
		varnumber_T	val;
		పూర్ణాంకం		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		ఒకవేళ (check_not_string(rettv) == FAIL)
		    తిరిగి FAIL;
		val = tv_get_సంఖ్య_chk(rettv, &error);
		clear_tv(rettv);
		ఒకవేళ (error)
		    తిరిగి FAIL;
		ఒకవేళ (*p == '-')
		    val = -val;
		rettv->v_type = VAR_సంఖ్య;
		rettv->vval.v_సంఖ్య = val;
	    }
	}
	లేకపోతే ఒకవేళ (numeric_only)
	{
	    ++p;
	    ఆపు;
	}
	లేకపోతే ఒకవేళ (*p == '!')
	{
	    పూర్ణాంకం v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_సంఖ్య = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *చివర = p;
    తిరిగి OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    స్థిర శూన్య
get_vim_constant(char_u **arg, typval_T *rettv)
{
    ఒకవేళ (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_సంఖ్య = VVAL_TRUE;
	*arg += 6;
    }
    లేకపోతే ఒకవేళ (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_సంఖ్య = VVAL_FALSE;
	*arg += 7;
    }
    లేకపోతే ఒకవేళ (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_సంఖ్య = VVAL_NULL;
	*arg += 6;
    }
    లేకపోతే ఒకవేళ (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_సంఖ్య = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, పూర్ణాంకం *len, పూర్ణాంకం *type_is)
{
    exprtype_T	type = సమాసం_UNKNOWN;
    పూర్ణాంకం		i;

    ఎంపిక (p[0])
    {
	స్థితి '=':   ఒకవేళ (p[1] == '=')
			type = సమాసం_EQUAL;
		    లేకపోతే ఒకవేళ (p[1] == '~')
			type = సమాసం_సరిపోలిక;
		    ఆపు;
	స్థితి '!':   ఒకవేళ (p[1] == '=')
			type = సమాసం_NEQUAL;
		    లేకపోతే ఒకవేళ (p[1] == '~')
			type = సమాసం_NOMATCH;
		    ఆపు;
	స్థితి '>':   ఒకవేళ (p[1] != '=')
		    {
			type = సమాసం_GREATER;
			*len = 1;
		    }
		    లేకపోతే
			type = సమాసం_GEQUAL;
		    ఆపు;
	స్థితి '<':   ఒకవేళ (p[1] != '=')
		    {
			type = సమాసం_SMALLER;
			*len = 1;
		    }
		    లేకపోతే
			type = సమాసం_SEQUAL;
		    ఆపు;
	స్థితి 'i':   ఒకవేళ (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			ఒకవేళ (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			ఒకవేళ (!సురక్షిత_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? సమాసం_IS : సమాసం_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    ఆపు;
    }
    తిరిగి type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    శూన్య
skip_సమాసం_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_సమాసం(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, ఖాళీ);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    పూర్ణాంకం
bool_on_దొంతర(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_దొంతర(cctx, 0);
    ఒకవేళ (type == &t_bool)
	తిరిగి OK;

    ఒకవేళ (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_సంఖ్య
	    || type == &t_సంఖ్య_bool
	    || type == &t_const_సంఖ్య_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	తిరిగి generate_COND2BOOL(cctx);

    తిరిగి need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    శూన్య
error_white_both(char_u *op, పూర్ణాంకం len)
{
    char_u	buf[10];

    vim_strncpy(buf, op, len);
    semsg(_(e_white_space_required_before_and_after_str_at_str), buf, op);
}

/*
 * Compile code to apply '-', '+' and '!'.
 * When "numeric_only" is TRUE do not apply '!'.
 */
    స్థిర పూర్ణాంకం
compile_leader(cctx_T *cctx, పూర్ణాంకం numeric_only, char_u *ప్రారంభం, char_u **చివర)
{
    char_u	*p = *చివర;

    // this works from end to start
    అయితే (p > ప్రారంభం)
    {
	--p;
	అయితే (VIM_ISWHITE(*p))
	    --p;
	ఒకవేళ (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_దొంతర(cctx, 0);
	    ఒకవేళ (type->tt_type != VAR_FLOAT && need_type(type, &t_సంఖ్య,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		తిరిగి FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    ఒకవేళ (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == ఖాళీ)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (numeric_only)
	{
	    ++p;
	    ఆపు;
	}
	లేకపోతే
	{
	    పూర్ణాంకం  invert = *p == '!';

	    అయితే (p > ప్రారంభం && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		ఒకవేళ (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    ఒకవేళ (check_type_is_విలువ(get_type_on_దొంతర(cctx, 0)) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (generate_2BOOL(cctx, invert, -1) == FAIL)
		తిరిగి FAIL;
	}
    }
    *చివర = p;
    తిరిగి OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    స్థిర పూర్ణాంకం
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    పూర్ణాంకం	    ret;
    char_u  *p = *arg + 1;

    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(p, arg, cctx) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (**arg == ')')
	// empty tuple
	తిరిగి compile_tuple(arg, cctx, ppconst, FALSE);

    ఒకవేళ (ppconst->pp_వాడిన <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    లేకపోతే
    {
	// Not enough space in ppconst, flush constants.
	ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
	    తిరిగి FAIL;
	ret = compile_expr0(arg, cctx);
    }
    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(*arg, arg, cctx) == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (ret == OK && **arg == ',')
    {
	// tuple
	పూర్ణాంకం is_const = ppconst->pp_వాడిన > 0 || ppconst->pp_is_const;
	ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
	    తిరిగి FAIL;
	తిరిగి compile_tuple(arg, cctx, ppconst, is_const);
    }

    ఒకవేళ (**arg == ')')
	++*arg;
    లేకపోతే ఒకవేళ (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    తిరిగి ret;
}

స్థిర పూర్ణాంకం compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    స్థిర పూర్ణాంకం
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *ప్రారంభం_leader,
	char_u **చివర_leader,
	ppconst_T *ppconst)
{
    char_u	*name_ప్రారంభం = *చివర_leader;
    పూర్ణాంకం		keeping_dict = FALSE;

    కొరకు (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	ఒకవేళ (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_ప్రారంభం(p)))
	{
	    char_u *తదుపరి = peek_తదుపరి_పంక్తి_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    ఒకవేళ (తదుపరి != ఖాళీ &&
		    ((తదుపరి[0] == '-' && తదుపరి[1] == '>'
				 && (తదుపరి[2] == '{'
				       || *skipwhite(తదుపరి + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(తదుపరి + 2))))
		    || (తదుపరి[0] == '.' && eval_isdictc(తదుపరి[1]))))
	    {
		తదుపరి = తదుపరి_పంక్తి_from_context(cctx, TRUE);
		ఒకవేళ (తదుపరి == ఖాళీ)
		    తిరిగి FAIL;
		*arg = తదుపరి;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	ఒకవేళ (**arg == '(')
	{
	    పూర్ణాంకం		argcount = 0;

	    ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
		తిరిగి FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_దొంతర(cctx, 0);

	    *arg = skipwhite(p + 1);
	    ఒకవేళ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (generate_PCALL(cctx, argcount, name_ప్రారంభం, type, TRUE) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ఒకవేళ (generate_instr(cctx, ISN_CLEARDICT) == ఖాళీ)
		    తిరిగి FAIL;
	    }
	}
	లేకపోతే ఒకవేళ (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    పూర్ణాంకం	    alt;
	    char_u  *paren;

	    // something->method()
	    ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
		తిరిగి FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_దొంతర(cctx, 0);
	    ఒకవేళ (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_విలువ));
		తిరిగి FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    ఒకవేళ (compile_leader(cctx, TRUE, ప్రారంభం_leader, చివర_leader) == FAIL)
		తిరిగి FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    ఒకవేళ (**arg == '(')
		alt = 2;
	    లేకపోతే
	    {
		// alternative 1 or 3
		p = *arg;
		ఒకవేళ (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    తిరిగి FAIL;
		}
		ఒకవేళ (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		కొరకు ( ; eval_isnamec(*p); ++p)
		    ;
		ఒకవేళ (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    ఒకవేళ (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			తిరిగి FAIL;
		}
		లేకపోతే
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    ఒకవేళ (paren == ఖాళీ)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			తిరిగి FAIL;
		    }
		}
	    }

	    ఒకవేళ (alt != 1)
	    {
		పూర్ణాంకం	    argcount = 1;
		garray_T    *దొంతర = &cctx->ctx_type_దొంతర;
		పూర్ణాంకం	    type_idx_ప్రారంభం = దొంతర->ga_len;
		పూర్ణాంకం	    సమాసం_isn_ప్రారంభం = cctx->ctx_instr.ga_len;
		పూర్ణాంకం	    సమాసం_isn_చివర;
		పూర్ణాంకం	    arg_isn_లెక్క;

		ఒకవేళ (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    ఒకవేళ (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			తిరిగి FAIL;
		}
		లేకపోతే
		{
		    పూర్ణాంకం fail;
		    పూర్ణాంకం save_len = cctx->ctx_ufunc->uf_పంక్తులు.ga_len;
		    పూర్ణాంకం	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_సమాసం;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_పంక్తులు.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_సమాసం;
		    cctx->ctx_ufunc->uf_పంక్తులు.ga_len = save_len;

		    ఒకవేళ (fail)
		    {
			ఒకవేళ (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_వ్యక్తీకరణ_str), pstart);
			తిరిగి FAIL;
		    }
		}

		// Compile the arguments.
		ఒకవేళ (**arg != '(')
		{
		    ఒకవేళ (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    లేకపోతే
			semsg(_(e_missing_parenthesis_str), *arg);
		    తిరిగి FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		సమాసం_isn_చివర = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		ఒకవేళ (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    తిరిగి FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_లెక్క = cctx->ctx_instr.ga_len - సమాసం_isn_చివర;
		ఒకవేళ (arg_isn_లెక్క > 0)
		{
		    పూర్ణాంకం	    సమాసం_isn_లెక్క = సమాసం_isn_చివర - సమాసం_isn_ప్రారంభం;
		    isn_T   *isn = ALLOC_MULT(isn_T, సమాసం_isn_లెక్క);
		    type_T  *decl_type;
		    type2_T  *typep;

		    ఒకవేళ (isn == ఖాళీ)
			తిరిగి FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_దత్తాంశం)
							      + సమాసం_isn_ప్రారంభం,
					       పరిమాణం(isn_T) * సమాసం_isn_లెక్క);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_దత్తాంశం)
							      + సమాసం_isn_ప్రారంభం,
			     ((isn_T *)cctx->ctx_instr.ga_దత్తాంశం) + సమాసం_isn_చివర,
						పరిమాణం(isn_T) * arg_isn_లెక్క);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_దత్తాంశం)
					      + సమాసం_isn_ప్రారంభం + arg_isn_లెక్క,
					  isn, పరిమాణం(isn_T) * సమాసం_isn_లెక్క);
		    vim_free(isn);

		    typep = ((type2_T *)దొంతర->ga_దత్తాంశం) + type_idx_ప్రారంభం;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)దొంతర->ga_దత్తాంశం) + type_idx_ప్రారంభం,
			      ((type2_T *)దొంతర->ga_దత్తాంశం) + type_idx_ప్రారంభం + 1,
			      పరిమాణం(type2_T)
				       * (దొంతర->ga_len - type_idx_ప్రారంభం - 1));
		    typep = ((type2_T *)దొంతర->ga_దత్తాంశం) + దొంతర->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_దొంతర(cctx, 0);
		ఒకవేళ (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    తిరిగి FAIL;
	    }

	    ఒకవేళ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ఒకవేళ (generate_instr(cctx, ISN_CLEARDICT) == ఖాళీ)
		    తిరిగి FAIL;
	    }
	}
	లేకపోతే ఒకవేళ (**arg == '[')
	{
	    పూర్ణాంకం		is_ముక్క = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
		తిరిగి FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(p, arg, cctx) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    లేకపోతే
	    {
		ఒకవేళ (compile_expr0(arg, cctx) == FAIL)
		    తిరిగి FAIL;
		ఒకవేళ (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    తిరిగి FAIL;
		}
		ఒకవేళ (may_get_తదుపరి_పంక్తి_error(*arg, arg, cctx) == FAIL)
		    తిరిగి FAIL;
		*arg = skipwhite(*arg);
	    }
	    ఒకవేళ (**arg == ':')
	    {
		is_ముక్క = TRUE;
		++*arg;
		ఒకవేళ (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    తిరిగి FAIL;
		}
		ఒకవేళ (may_get_తదుపరి_పంక్తి_error(*arg, arg, cctx) == FAIL)
		    తిరిగి FAIL;
		ఒకవేళ (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		లేకపోతే
		{
		    ఒకవేళ (compile_expr0(arg, cctx) == FAIL)
			తిరిగి FAIL;
		    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(*arg, arg, cctx) == FAIL)
			తిరిగి FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    ఒకవేళ (**arg != ']')
	    {
		emsg(_(e_missing_closing_వర్గం_brace));
		తిరిగి FAIL;
	    }
	    *arg = *arg + 1;

	    ఒకవేళ (keeping_dict)
	    {
		keeping_dict = FALSE;
		ఒకవేళ (generate_instr(cctx, ISN_CLEARDICT) == ఖాళీ)
		    తిరిగి FAIL;
	    }
	    ఒకవేళ (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_ముక్క, &keeping_dict, cctx) == FAIL)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
		తిరిగి FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_దొంతర(cctx, 0);
	    ఒకవేళ (type != &t_unknown
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
		ఒకవేళ (compile_class_object_సూచీ(cctx, arg, type) == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే
	    {
		*arg = p + 1;
		ఒకవేళ (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    తిరిగి FAIL;
		}
		p = *arg;
		అయితే (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		ఒకవేళ (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    తిరిగి FAIL;
		}
		ఒకవేళ (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == ఖాళీ)
		    తిరిగి FAIL;
		ఒకవేళ (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    తిరిగి FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	లేకపోతే
	    ఆపు;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    ఒకవేళ (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == ఖాళీ)
	తిరిగి FAIL;

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*ప్రారంభం_leader, *చివర_leader;
    పూర్ణాంకం		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_వాడిన];
    పూర్ణాంకం		వాడిన_before = ppconst->pp_వాడిన;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    ప్రారంభం_leader = *arg;
    ఒకవేళ (eval_leader(arg, TRUE) == FAIL)
	తిరిగి FAIL;
    చివర_leader = *arg;

    rettv->v_type = VAR_UNKNOWN;
    ఎంపిక (**arg)
    {
	/*
	 * Number constant.
	 */
	స్థితి '0':	// also for blob starting with 0z
	స్థితి '1':
	స్థితి '2':
	స్థితి '3':
	స్థితి '4':
	స్థితి '5':
	స్థితి '6':
	స్థితి '7':
	స్థితి '8':
	స్థితి '9':
	స్థితి '.':   ఒకవేళ (eval_సంఖ్య(arg, rettv, TRUE, FALSE) == FAIL)
			తిరిగి FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    ఒకవేళ (apply_leader(rettv, TRUE,
					    ప్రారంభం_leader, &చివర_leader) == FAIL)
		    {
			clear_tv(rettv);
			తిరిగి FAIL;
		    }
		    ఆపు;

	/*
	 * String constant: "string".
	 */
	స్థితి '"':   ఒకవేళ (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			తిరిగి FAIL;
		    ఆపు;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	స్థితి '\'':  ఒకవేళ (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			తిరిగి FAIL;
		    ఆపు;

	/*
	 * Constant Vim variable.
	 */
	స్థితి 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    ఆపు;

	/*
	 * "true" constant
	 */
	స్థితి 't':   ఒకవేళ (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_సంఖ్య = VVAL_TRUE;
		    }
		    లేకపోతే
			ret = NOTDONE;
		    ఆపు;

	/*
	 * "false" constant
	 */
	స్థితి 'f':   ఒకవేళ (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_సంఖ్య = VVAL_FALSE;
		    }
		    లేకపోతే
			ret = NOTDONE;
		    ఆపు;

	/*
	 * "null" or "null_*" constant
	 */
	స్థితి 'n':   ఒకవేళ (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			పూర్ణాంకం	len;

			కొరకు (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			ఒకవేళ (ret == FAIL)
			    ret = NOTDONE;
			లేకపోతే
			    *arg += len + 4;
		    }
		    లేకపోతే
			ret = NOTDONE;
		    ఆపు;

	/*
	 * List: [expr, expr]
	 */
	స్థితి '[':   ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
			తిరిగి FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    ఆపు;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	స్థితి '{':   ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
			తిరిగి FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    ఆపు;

	/*
	 * Option value: &name
	 */
	స్థితి '&':	ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
			    తిరిగి FAIL;
			ret = compile_get_option(arg, cctx);
			ఆపు;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	స్థితి '$':	ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
			    తిరిగి FAIL;
			ఒకవేళ ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			లేకపోతే
			    ret = compile_get_env(arg, cctx);
			ఆపు;

	/*
	 * Register contents: @r.
	 */
	స్థితి '@':	ఒకవేళ (generate_ppconst(cctx, ppconst) == FAIL)
			    తిరిగి FAIL;
			ret = compile_get_రిజిస్టరు(arg, cctx);
			ఆపు;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	స్థితి '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    ఒకవేళ (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    ఆపు;

	అప్రమేయ:    ret = NOTDONE;
		    ఆపు;
    }
    ఒకవేళ (ret == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (rettv->v_type != VAR_UNKNOWN && వాడిన_before == ppconst->pp_వాడిన)
    {
	ఒకవేళ (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	లేకపోతే
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_వాడిన;
    }
    లేకపోతే ఒకవేళ (ret == NOTDONE)
    {
	char_u	    *p;
	పూర్ణాంకం	    r;

	ఒకవేళ (!eval_isnamec1(**arg))
	{
	    ఒకవేళ (!vim9_bad_comment(*arg))
	    {
		ఒకవేళ (చివర్లు_excmd(*skipwhite(*arg)))
		    semsg(_(e_ఖాళీలు_వ్యక్తీకరణ_str), *arg);
		లేకపోతే
		    semsg(_(e_name_expected_str), *arg);
	    }
	    తిరిగి FAIL;
	}

	// "name" or "name()"
	p = గమ్యం_name_చివర(*arg, TRUE);
	ఒకవేళ (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_ఇక్కడ));
	    తిరిగి FAIL;
	}

	size_t namelen = p - *arg;
	ఒకవేళ (*p == '<')
	{
	    ఒకవేళ (skip_generic_func_type_వాదనలు(&p) == FAIL)
		తిరిగి FAIL;
	}

	ఒకవేళ (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	లేకపోతే
	{
	    ఒకవేళ (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		తిరిగి FAIL;
	    r = compile_ఎక్కించు(arg, namelen, p, cctx, TRUE, TRUE);
	}
	ఒకవేళ (r == FAIL)
	    తిరిగి FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    ఒకవేళ (compile_subscript(arg, cctx, ప్రారంభం_leader, &చివర_leader,
							     ppconst) == FAIL)
	తిరిగి FAIL;
    ఒకవేళ ((ppconst->pp_వాడిన > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_వాడిన - 1];
	ఒకవేళ (apply_leader(rettv, FALSE, ప్రారంభం_leader, &చివర_leader) == FAIL)
	    తిరిగి FAIL;
	తిరిగి OK;
    }
    ఒకవేళ (compile_leader(cctx, FALSE, ప్రారంభం_leader, &చివర_leader) == FAIL)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    స్థిర పూర్ణాంకం
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = ఖాళీ;

    // Recognize <type>
    ఒకవేళ (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	ఒకవేళ (want_type == ఖాళీ)
	    తిరిగి FAIL;

	ఒకవేళ (**arg != '>')
	{
	    ఒకవేళ (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    లేకపోతే
		emsg(_(e_missing_gt));
	    తిరిగి FAIL;
	}
	++*arg;
	ఒకవేళ (may_get_తదుపరి_పంక్తి_error(*arg, arg, cctx) == FAIL)
	    తిరిగి FAIL;
    }

    ఒకవేళ (compile_expr9(arg, cctx, ppconst) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (want_type != ఖాళీ)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_రకం = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_దొంతర(cctx, 0);
	ఒకవేళ (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    ఒకవేళ (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		తిరిగి FAIL;
	}
    }

    తిరిగి OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    స్థిర పూర్ణాంకం
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*తదుపరి;
    పూర్ణాంకం		ppconst_వాడిన = ppconst->pp_వాడిన;

    // get the first expression
    ఒకవేళ (compile_expr8(arg, cctx, ppconst) == FAIL)
	తిరిగి FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    కొరకు (;;)
    {
	op = may_peek_తదుపరి_పంక్తి(cctx, *arg, &తదుపరి);
	ఒకవేళ (*op != '*' && *op != '/' && *op != '%')
	    ఆపు;
	ఒకవేళ (తదుపరి != ఖాళీ)
	{
	    *arg = తదుపరి_పంక్తి_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ఒకవేళ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    తిరిగి FAIL;
	}
	ఒకవేళ (may_get_తదుపరి_పంక్తి_error(op + 1, arg, cctx) == FAIL)
	    తిరిగి FAIL;

	// get the second expression
	ఒకవేళ (compile_expr8(arg, cctx, ppconst) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (ppconst->pp_వాడిన == ppconst_వాడిన + 2
		&& ppconst->pp_tv[ppconst_వాడిన].v_type == VAR_సంఖ్య
		&& ppconst->pp_tv[ppconst_వాడిన + 1].v_type == VAR_సంఖ్య)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_వాడిన];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_వాడిన + 1];
	    varnumber_T	    res = 0;
	    పూర్ణాంకం		    failed = FALSE;

	    // both are numbers: compute the result
	    ఎంపిక (*op)
	    {
		స్థితి '*': res = tv1->vval.v_సంఖ్య * tv2->vval.v_సంఖ్య;
			  ఆపు;
		స్థితి '/': res = అంకె_divide(tv1->vval.v_సంఖ్య,
						  tv2->vval.v_సంఖ్య, &failed);
			  ఆపు;
		స్థితి '%': res = అంకె_modulus(tv1->vval.v_సంఖ్య,
						  tv2->vval.v_సంఖ్య, &failed);
			  ఆపు;
	    }
	    ఒకవేళ (failed)
		తిరిగి FAIL;
	    tv1->vval.v_సంఖ్య = res;
	    --ppconst->pp_వాడిన;
	}
	లేకపోతే
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    తిరిగి OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    స్థిర పూర్ణాంకం
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*తదుపరి;
    పూర్ణాంకం		oplen;
    పూర్ణాంకం		ppconst_వాడిన = ppconst->pp_వాడిన;

    // get the first variable
    ఒకవేళ (compile_expr7(arg, cctx, ppconst) == FAIL)
	తిరిగి FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    కొరకు (;;)
    {
	op = may_peek_తదుపరి_పంక్తి(cctx, *arg, &తదుపరి);
	ఒకవేళ (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    ఆపు;
	ఒకవేళ (op[0] == op[1] && *op != '.' && తదుపరి)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    ఆపు;
	oplen = (*op == '.' ? 2 : 1);
	ఒకవేళ (తదుపరి != ఖాళీ)
	{
	    *arg = తదుపరి_పంక్తి_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	ఒకవేళ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    తిరిగి FAIL;
	}

	ఒకవేళ (may_get_తదుపరి_పంక్తి_error(op + oplen, arg, cctx) == FAIL)
	    తిరిగి FAIL;

	// get the second expression
	ఒకవేళ (compile_expr7(arg, cctx, ppconst) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (ppconst->pp_వాడిన == ppconst_వాడిన + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_వాడిన].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_వాడిన + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_వాడిన].v_type == VAR_సంఖ్య
		    && ppconst->pp_tv[ppconst_వాడిన + 1].v_type == VAR_సంఖ్య)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_వాడిన];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_వాడిన + 1];

	    // concat/subtract/add constant numbers
	    ఒకవేళ (*op == '+')
		tv1->vval.v_సంఖ్య = tv1->vval.v_సంఖ్య + tv2->vval.v_సంఖ్య;
	    లేకపోతే ఒకవేళ (*op == '-')
		tv1->vval.v_సంఖ్య = tv1->vval.v_సంఖ్య - tv2->vval.v_సంఖ్య;
	    లేకపోతే
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		ఒకవేళ (tv1->vval.v_string == ఖాళీ)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    తిరిగి FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_వాడిన;
	}
	లేకపోతే
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    ఒకవేళ (*op == '.')
	    {
		ఒకవేళ (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    తిరిగి FAIL;
		ఒకవేళ (generate_CONCAT(cctx, 2) == FAIL)
		    తిరిగి FAIL;
	    }
	    లేకపోతే
		generate_two_op(cctx, op);
	}
    }

    తిరిగి OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    స్థిర పూర్ణాంకం
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = సమాసం_UNKNOWN;
    char_u	*p;
    char_u	*తదుపరి;
    పూర్ణాంకం		len = 2;
    పూర్ణాంకం		ppconst_వాడిన = ppconst->pp_వాడిన;
    isn_T	*isn;

    // get the first variable
    ఒకవేళ (compile_expr6(arg, cctx, ppconst) == FAIL)
	తిరిగి FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    కొరకు (;;)
    {
	type = సమాసం_UNKNOWN;

	p = may_peek_తదుపరి_పంక్తి(cctx, *arg, &తదుపరి);
	ఒకవేళ (p[0] == '<' && p[1] == '<')
	    type = సమాసం_LSHIFT;
	లేకపోతే ఒకవేళ (p[0] == '>' && p[1] == '>')
	    type = సమాసం_RSHIFT;

	ఒకవేళ (type == సమాసం_UNKNOWN)
	    తిరిగి OK;

	// Handle a bitwise left or right shift operator
	ఒకవేళ (ppconst->pp_వాడిన == ppconst_వాడిన + 1)
	{
	    ఒకవేళ (ppconst->pp_tv[ppconst->pp_వాడిన - 1].v_type != VAR_సంఖ్య)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_సంఖ్య));
		తిరిగి FAIL;
	    }
	}
	లేకపోతే
	{
	    type_T	*t = get_type_on_దొంతర(cctx, 0);

	    ఒకవేళ (need_type(t, &t_సంఖ్య, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_సంఖ్య));
		తిరిగి FAIL;
	    }
	}

	ఒకవేళ (తదుపరి != ఖాళీ)
	{
	    *arg = తదుపరి_పంక్తి_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ఒకవేళ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    తిరిగి FAIL;
	}

	// get the second variable
	ఒకవేళ (may_get_తదుపరి_పంక్తి_error(p + len, arg, cctx) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (compile_expr6(arg, cctx, ppconst) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (ppconst->pp_వాడిన == ppconst_వాడిన + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_వాడిన - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_వాడిన - 1];

	    // Both sides are a constant, compute the result now.
	    ఒకవేళ (tv2->v_type != VAR_సంఖ్య || tv2->vval.v_సంఖ్య < 0)
	    {
		// right operand should be a positive number
		ఒకవేళ (tv2->v_type != VAR_సంఖ్య)
		    emsg(_(e_bitshift_ops_must_be_సంఖ్య));
		లేకపోతే
		    emsg(_(e_bitshift_ops_must_be_ధనాత్మక));
		తిరిగి FAIL;
	    }

	    ఒకవేళ (tv2->vval.v_సంఖ్య > MAX_LSHIFT_బిట్లు)
		tv1->vval.v_సంఖ్య = 0;
	    లేకపోతే ఒకవేళ (type == సమాసం_LSHIFT)
		tv1->vval.v_సంఖ్య =
			(uvarnumber_T)tv1->vval.v_సంఖ్య << tv2->vval.v_సంఖ్య;
	    లేకపోతే
		tv1->vval.v_సంఖ్య =
			(uvarnumber_T)tv1->vval.v_సంఖ్య >> tv2->vval.v_సంఖ్య;
	    clear_tv(tv2);
	    --ppconst->pp_వాడిన;
	}
	లేకపోతే
	{
	    ఒకవేళ (need_type(get_type_on_దొంతర(cctx, 0), &t_సంఖ్య, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_సంఖ్య));
		తిరిగి FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    ఒకవేళ (isn == ఖాళీ)
		తిరిగి FAIL;

	    ఒకవేళ (isn != ఖాళీ)
		isn->isn_arg.op.op_type = type;
	}
    }

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = సమాసం_UNKNOWN;
    char_u	*p;
    char_u	*తదుపరి;
    పూర్ణాంకం		len = 2;
    పూర్ణాంకం		type_is = FALSE;
    పూర్ణాంకం		ppconst_వాడిన = ppconst->pp_వాడిన;

    // get the first variable
    ఒకవేళ (compile_expr5(arg, cctx, ppconst) == FAIL)
	తిరిగి FAIL;

    p = may_peek_తదుపరి_పంక్తి(cctx, *arg, &తదుపరి);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    ఒకవేళ (type != సమాసం_UNKNOWN)
    {
	పూర్ణాంకం ic = FALSE;  // Default: do not ignore case

	ఒకవేళ (తదుపరి != ఖాళీ)
	{
	    *arg = తదుపరి_పంక్తి_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	ఒకవేళ (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_వ్యక్తీకరణ_str), *arg);
	    తిరిగి FAIL;
	}
	// extra question mark appended: ignore case
	ఒకవేళ (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	లేకపోతే ఒకవేళ (p[len] == '#')
	    ++len;
	// nothing appended: match case

	ఒకవేళ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    తిరిగి FAIL;
	}

	// get the second variable
	ఒకవేళ (may_get_తదుపరి_పంక్తి_error(p + len, arg, cctx) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (compile_expr5(arg, cctx, ppconst) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (ppconst->pp_వాడిన == ppconst_వాడిన + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_వాడిన - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_వాడిన - 1];
	    పూర్ణాంకం		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    ఒకవేళ (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    లేకపోతే
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_సంఖ్య = tv1->vval.v_సంఖ్య
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_వాడిన;
	    }
	    తిరిగి ret;
	}

	generate_ppconst(cctx, ppconst);
	తిరిగి generate_COMPARE(cctx, type, ic);
    }

    తిరిగి OK;
}

స్థిర పూర్ణాంకం compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    స్థిర పూర్ణాంకం
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	అక్షరం	*op,
	ppconst_T *ppconst,
	పూర్ణాంకం	ppconst_వాడిన వాడని)
{
    char_u	*తదుపరి;
    char_u	*p = may_peek_తదుపరి_పంక్తి(cctx, *arg, &తదుపరి);
    పూర్ణాంకం		opchar = *op;

    ఒకవేళ (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	చివర_ga;
	పూర్ణాంకం		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&చివర_ga, పరిమాణం(పూర్ణాంకం), 10);
	అయితే (p[0] == opchar && p[1] == opchar)
	{
	    దీర్ఘ	ప్రారంభం_lnum = SOURCING_LNUM;
	    దీర్ఘ	save_sourcing_lnum;
	    పూర్ణాంకం		ప్రారంభం_ctx_lnum = cctx->ctx_lnum;
	    పూర్ణాంకం		save_lnum;
	    పూర్ణాంకం		const_వాడిన;
	    పూర్ణాంకం		స్థితి_2;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    ఒకవేళ (తదుపరి != ఖాళీ)
	    {
		*arg = తదుపరి_పంక్తి_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ఒకవేళ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&చివర_ga);
		తిరిగి FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = ప్రారంభం_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = ప్రారంభం_ctx_lnum;

	    స్థితి_2 = check_ppconst_bool(ppconst);
	    ఒకవేళ (స్థితి_2 != FAIL)
	    {
		// Use the last ppconst if possible.
		ఒకవేళ (ppconst->pp_వాడిన > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_వాడిన - 1];
		    పూర్ణాంకం		is_true = tv2bool(tv);

		    ఒకవేళ ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_సంఖ్య = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    లేకపోతే
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_వాడిన;
			jump_when = JUMP_NEVER;
		    }
		}
		లేకపోతే
		{
		    // Every part must evaluate to a bool.
		    స్థితి_2 = bool_on_దొంతర(cctx);
		}
	    }
	    ఒకవేళ (స్థితి_2 != FAIL)
		స్థితి_2 = ga_grow(&చివర_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    ఒకవేళ (స్థితి_2 == FAIL)
	    {
		ga_clear(&చివర_ga);
		తిరిగి FAIL;
	    }

	    ఒకవేళ (jump_when != JUMP_NEVER)
	    {
		ఒకవేళ (cctx->ctx_skip != SKIP_YES)
		{
		    *(((పూర్ణాంకం *)చివర_ga.ga_దత్తాంశం) + చివర_ga.ga_len) = instr->ga_len;
		    ++చివర_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&చివర_ga);
		తిరిగి FAIL;
	    }

	    const_వాడిన = ppconst->pp_వాడిన;
	    ఒకవేళ ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&చివర_ga);
		తిరిగి FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    ఒకవేళ (ppconst->pp_వాడిన == const_వాడిన + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_వాడిన - 1];

		ఒకవేళ (tv->v_type == VAR_సంఖ్య
			 && (tv->vval.v_సంఖ్య == 1 || tv->vval.v_సంఖ్య == 0))
		{
		    tv->vval.v_సంఖ్య = tv->vval.v_సంఖ్య == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_తదుపరి_పంక్తి(cctx, *arg, &తదుపరి);
	}

	ఒకవేళ (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&చివర_ga);
	    తిరిగి FAIL;
	}

	ఒకవేళ (cctx->ctx_skip != SKIP_YES && ppconst->pp_వాడిన == 0)
	    // Every part must evaluate to a bool.
	    ఒకవేళ (bool_on_దొంతర(cctx) == FAIL)
	    {
		ga_clear(&చివర_ga);
		తిరిగి FAIL;
	    }

	ఒకవేళ (చివర_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    అయితే (చివర_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--చివర_ga.ga_len;
		isn = ((isn_T *)instr->ga_దత్తాంశం)
				  + *(((పూర్ణాంకం *)చివర_ga.ga_దత్తాంశం) + చివర_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&చివర_ga);

	cctx->ctx_skip = save_skip;
    }

    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    పూర్ణాంకం		ppconst_వాడిన = ppconst->pp_వాడిన;

    // get the first variable
    ఒకవేళ (compile_expr4(arg, cctx, ppconst) == FAIL)
	తిరిగి FAIL;

    // || and && work almost the same
    తిరిగి compile_and_or(arg, cctx, "&&", ppconst, ppconst_వాడిన);
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
    స్థిర పూర్ణాంకం
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    పూర్ణాంకం		ppconst_వాడిన = ppconst->pp_వాడిన;

    // eval the first expression
    ఒకవేళ (compile_expr3(arg, cctx, ppconst) == FAIL)
	తిరిగి FAIL;

    // || and && work almost the same
    తిరిగి compile_and_or(arg, cctx, "||", ppconst, ppconst_వాడిన);
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
    పూర్ణాంకం
compile_expr1(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p;
    పూర్ణాంకం		ppconst_వాడిన = ppconst->pp_వాడిన;
    char_u	*తదుపరి;

    // Ignore all kinds of errors when not producing code.
    ఒకవేళ (cctx->ctx_skip == SKIP_YES)
    {
	పూర్ణాంకం		prev_did_emsg = did_emsg;

	skip_సమాసం_cctx(arg, cctx);
	తిరిగి did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    ఒకవేళ (compile_expr2(arg, cctx, ppconst) == FAIL)
	తిరిగి FAIL;

    p = may_peek_తదుపరి_పంక్తి(cctx, *arg, &తదుపరి);
    ఒకవేళ (*p == '?')
    {
	పూర్ణాంకం		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*దొంతర = &cctx->ctx_type_దొంతర;
	పూర్ణాంకం		alt_idx = instr->ga_len;
	పూర్ణాంకం		చివర_idx = 0;
	isn_T		*isn;
	type_T		*type1 = ఖాళీ;
	పూర్ణాంకం		has_const_సమాసం = FALSE;
	పూర్ణాంకం		const_విలువ = FALSE;
	పూర్ణాంకం		save_skip = cctx->ctx_skip;

	ఒకవేళ (తదుపరి != ఖాళీ)
	{
	    *arg = తదుపరి_పంక్తి_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	ఒకవేళ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    తిరిగి FAIL;
	}

	ఒకవేళ (ppconst->pp_వాడిన == ppconst_వాడిన + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_సమాసం = TRUE;
	    ఒకవేళ (op_falsy)
		const_విలువ = tv2bool(&ppconst->pp_tv[ppconst_వాడిన]);
	    లేకపోతే
	    {
		పూర్ణాంకం error = FALSE;

		const_విలువ = tv_get_bool_chk(&ppconst->pp_tv[ppconst_వాడిన],
								       &error);
		ఒకవేళ (error)
		    తిరిగి FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_విలువ : !const_విలువ) ? SKIP_YES : SKIP_NOT;

	    ఒకవేళ (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    లేకపోతే
	    {
		clear_tv(&ppconst->pp_tv[ppconst_వాడిన]);
		--ppconst->pp_వాడిన;
	    }
	}
	లేకపోతే
	{
	    generate_ppconst(cctx, ppconst);
	    ఒకవేళ (op_falsy)
		చివర_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    ఒకవేళ (op_falsy)
	    {
		type1 = get_type_on_దొంతర(cctx, -1);
		ఒకవేళ (check_type_is_విలువ(type1) == FAIL)
		    తిరిగి FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	ఒకవేళ (may_get_తదుపరి_పంక్తి_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (compile_expr1(arg, cctx, ppconst) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (!has_const_సమాసం)
	{
	    generate_ppconst(cctx, ppconst);

	    ఒకవేళ (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_దొంతర(cctx, 0);
		--దొంతర->ga_len;

		చివర_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_దత్తాంశం) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	ఒకవేళ (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_తదుపరి_పంక్తి(cctx, *arg, &తదుపరి);
	    ఒకవేళ (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		తిరిగి FAIL;
	    }
	    ఒకవేళ (తదుపరి != ఖాళీ)
	    {
		*arg = తదుపరి_పంక్తి_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    ఒకవేళ (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		తిరిగి FAIL;
	    }

	    // evaluate the third expression
	    ఒకవేళ (has_const_సమాసం)
		cctx->ctx_skip = save_skip == SKIP_YES || const_విలువ
							 ? SKIP_YES : SKIP_NOT;
	    ఒకవేళ (may_get_తదుపరి_పంక్తి_error(p + 1, arg, cctx) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (compile_expr1(arg, cctx, ppconst) == FAIL)
		తిరిగి FAIL;
	}

	ఒకవేళ (!has_const_సమాసం)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)దొంతర->ga_దత్తాంశం)
					      + దొంతర->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_దత్తాంశం) + చివర_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    తిరిగి OK;
}

/*
 * Toplevel expression.
 * Sets "is_const" (if not NULL) to indicate the value is a constant.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
compile_expr0_ext(char_u **arg,  cctx_T *cctx, పూర్ణాంకం *is_const)
{
    ppconst_T	ppconst;

    CLEAR_క్షేత్రం(ppconst);
    ఒకవేళ (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	తిరిగి FAIL;
    }
    ఒకవేళ (is_const != ఖాళీ)
	*is_const = ppconst.pp_వాడిన > 0 || ppconst.pp_is_const;
    ఒకవేళ (generate_ppconst(cctx, &ppconst) == FAIL)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Toplevel expression.
 */
    పూర్ణాంకం
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    తిరిగి compile_expr0_ext(arg, cctx, ఖాళీ);
}


#అయితే_ముగింపు // defined(FEAT_EVAL)
