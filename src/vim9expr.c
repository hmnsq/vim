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

#وصف_ڪريو USING_FLOAT_STUFF
#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
ساكن صحيح paren_follows_after_اظهار = 0;

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
    لكل (i = 0; i < ppconst->pp_استعمال_ٿيل; ++i)
	إذا (generate_tv_ڌڪو(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_استعمال_ٿيل = 0;
    cctx->ctx_skip = save_skip;
    أرجع ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    ساكن صحيح
check_ppconst_bool(ppconst_T *ppconst)
{
    إذا (ppconst->pp_استعمال_ٿيل > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل - 1];
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

    لكل (i = 0; i < ppconst->pp_استعمال_ٿيل; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_استعمال_ٿيل = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    ساكن صحيح
compile_tuple_member(
    type2_T	*typep,
    صحيح		is_ٽڪرو,
    cctx_T	*cctx)
{
    إذا (is_ٽڪرو)
    {
	إذا (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    أرجع FAIL;
	// a copy is made so the member type is no longer declared
	إذا (typep->type_decl->tt_type == VAR_TUPLE)
	    typep->type_decl = &t_tuple_any;

	// a copy is made, the composite is no longer "const"
	إذا (typep->type_curr->tt_flags & TTFLAG_CONST)
	{
	    type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

	    إذا (type != typep->type_curr)  // did get a copy
	    {
		type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		typep->type_curr = type;
	    }
	}
    }
    وإلا
    {
	إذا (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    إذا (typep->type_curr->tt_argcount == 1)
	    {
		إذا (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_دليلون[0]->tt_member;
		وإلا
		    typep->type_curr = typep->type_curr->tt_دليلون[0];
	    }
	    وإلا
		typep->type_curr = &t_any;
	    إذا (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		إذا (typep->type_decl->tt_argcount == 1)
		{
		    إذا (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_دليلون[0]->tt_member;
		    وإلا
			typep->type_decl = typep->type_decl->tt_دليلون[0];
		}
		وإلا
		    typep->type_curr = &t_any;
	    }
	    وإلا
		typep->type_decl = typep->type_curr;
	}
	إذا (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
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
compile_member(صحيح is_ٽڪرو, صحيح *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*ڍير = &cctx->ctx_type_ڍير;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)ڍير->ga_معلومات) + ڍير->ga_len - (is_ٽڪرو ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)ڍير->ga_معلومات) + ڍير->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    إذا ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    إذا (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	إذا (need_type(idxtype, &t_عدد, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    أرجع FAIL;
	إذا (is_ٽڪرو)
	{
	    idxtype = get_type_on_ڍير(cctx, 1);
	    إذا (need_type(idxtype, &t_عدد, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;
	}
    }

    إذا (vartype == VAR_DICT)
    {
	إذا (is_ٽڪرو)
	{
	    emsg(_(e_cannot_ٽڪرو_dictionary));
	    أرجع FAIL;
	}
	إذا (typep->type_curr->tt_type == VAR_DICT)
	{
	    typep->type_curr = typep->type_curr->tt_member;
	    إذا (typep->type_curr == &t_unknown)
		// empty dict was used
		typep->type_curr = &t_any;
	    إذا (typep->type_decl->tt_type == VAR_DICT)
	    {
		typep->type_decl = typep->type_decl->tt_member;
		إذا (typep->type_decl == &t_unknown)
		    // empty dict was used
		    typep->type_decl = &t_any;
	    }
	    وإلا
		typep->type_decl = typep->type_curr;
	}
	وإلا
	{
	    إذا (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	إذا (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    أرجع FAIL;
	إذا (keeping_dict != عدم)
	    *keeping_dict = TRUE;
    }
    وإلا إذا (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	إذا ((is_ٽڪرو
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (vartype == VAR_BLOB)
    {
	إذا (is_ٽڪرو)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    إذا (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		أرجع FAIL;
	}
	وإلا
	{
	    typep->type_curr = &t_عدد;
	    typep->type_decl = &t_عدد;
	    إذا (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		أرجع FAIL;
	}
    }
    وإلا إذا (vartype == VAR_TUPLE)
    {
	إذا (compile_tuple_member(typep, is_ٽڪرو, cctx) == FAIL)
	    أرجع FAIL;
    }
    وإلا إذا (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	إذا (is_ٽڪرو)
	{
	    إذا (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		أرجع FAIL;
	    // a copy is made so the member type is no longer declared
	    إذا (typep->type_decl->tt_type == VAR_LIST)
		typep->type_decl = &t_list_any;

	    // a copy is made, the composite is no longer "const"
	    إذا (typep->type_curr->tt_flags & TTFLAG_CONST)
	    {
		type_T *type = copy_type(typep->type_curr, cctx->ctx_type_list);

		إذا (type != typep->type_curr)  // did get a copy
		{
		    type->tt_flags &= ~(TTFLAG_CONST | TTFLAG_STATIC);
		    typep->type_curr = type;
		}
	    }
	}
	وإلا
	{
	    إذا (typep->type_curr->tt_type == VAR_LIST)
	    {
		typep->type_curr = typep->type_curr->tt_member;
		إذا (typep->type_curr == &t_unknown)
		    // empty list was used
		    typep->type_curr = &t_any;
		إذا (typep->type_decl->tt_type == VAR_LIST)
		{
		    typep->type_decl = typep->type_decl->tt_member;
		    إذا (typep->type_decl == &t_unknown)
			// empty list was used
			typep->type_decl = &t_any;
		}
		وإلا
			typep->type_decl = typep->type_curr;
	    }
	    إذا (generate_instr_drop(cctx,
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
		emsg(_(e_cannot_اشاريو_a_funcref));
		اكسر;
	    حالة VAR_BOOL:
	    حالة VAR_SPECIAL:
	    حالة VAR_ڪم:
	    حالة VAR_CHANNEL:
	    حالة VAR_INSTR:
	    حالة VAR_CLASS:
	    حالة VAR_OBJECT:
	    حالة VAR_TYPEALIAS:
	    حالة VAR_UNKNOWN:
	    حالة VAR_ANY:
	    حالة VAR_VOID:
		emsg(_(e_cannot_اشاريو_special_variable));
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
	إذا (cctx->ctx_ufunc != عدم && cctx->ctx_ufunc->uf_class != عدم)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    طالما (clp != عدم)
	    {
		إذا (clp == cl)
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
compile_class_object_اشاريو(cctx_T *cctx, char_u **arg, type_T *type)
{
    صحيح		m_idx;
    صحيح		ret = FAIL;

    إذا (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	أرجع FAIL;
    }

    class_T *cl = type->tt_class;
    صحيح is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    إذا (type == &t_super)
    {
	إذا (cctx->ctx_ufunc == عدم || cctx->ctx_ufunc->uf_class == عدم)
	{
	    emsg(_(e_using_super_not_اندريون_class_طريقو));
	    أرجع FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_ڍير.ga_len;
    }
    وإلا إذا (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	إذا (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_معلومات) + instr->ga_len - 1;
	    إذا (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    إذا (cl == عدم)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	أرجع FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_پڇاڙي = ڳوليو_name_پڇاڙي(name, عدم, عدم, FNE_CHECK_شروعات);
    إذا (name_پڇاڙي == name)
    {
	emsg(_(e_missing_name_after_dot));
	أرجع FAIL;
    }
    size_t len = name_پڇاڙي - name;

    gfargs_tab_T    gfatab;

    generic_func_دليلون_جدول_init(&gfatab);

    إذا (*name_پڇاڙي == '<')
    {
	// generic method call
	name_پڇاڙي = parse_generic_func_type_دليلون(name, len, name_پڇاڙي,
							&gfatab, cctx);
	إذا (name_پڇاڙي == عدم)
	    اذهب پورو;
    }

    إذا (*name_پڇاڙي == '(')
    {
	صحيح	function_ڳڻپ;
	صحيح	ٻار_ڳڻپ;
	ufunc_T	**functions;

	إذا (type->tt_type == VAR_CLASS)
	{
	    function_ڳڻپ = cl->class_class_function_ڳڻپ;
	    ٻار_ڳڻپ = cl->class_class_function_ڳڻپ_ٻار;
	    functions = cl->class_class_functions;
	}
	وإلا
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    إذا (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_ڳڻپ = cl->class_obj_طريقو_ڳڻپ;
	    ٻار_ڳڻپ = cl->class_obj_طريقو_ڳڻپ_ٻار;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = عدم;
	صحيح fi;
	لكل (fi = is_super ? ٻار_ڳڻپ : 0; fi < function_ڳڻپ; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    إذا (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		اكسر;
	    }
	}

	ocmember_T  *ocm = عدم;
	إذا (ufunc == عدم)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    إذا (ocm == عدم || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		طريقو_not_مليو_نياپو(cl, type->tt_type, name, len);
		اذهب پورو;
	    }
	    إذا (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_ڍير.ga_len;
		إذا (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    اذهب پورو;
	    }
	    وإلا
	    {
		صحيح حالت;

		إذا (IS_INTERFACE(cl))
		    حالت = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		وإلا
		    حالت = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		إذا (حالت == FAIL)
		    اذهب پورو;
	    }
	}

	إذا (is_super && ufunc != عدم && IS_ABSTRACT_طريقو(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_طريقو_str_سڌو), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    اذهب پورو;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	إذا (ocm == عدم && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_طريقو_str), name);
	    اذهب پورو;
	}

	// process generic function call
	إذا (ufunc != عدم)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    إذا (ufunc == عدم)
		اذهب پورو;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_پڇاڙي + 1);
	صحيح argcount = 0;
	إذا (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    اذهب پورو;

	إذا (ocm != عدم)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    اذهب پورو;
	}
	إذا (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	وإلا
	    ret = generate_CALL(cctx, ufunc, عدم, 0, argcount, FALSE);
	اذهب پورو;
    }

    إذا (type->tt_type == VAR_OBJECT)
    {
	ocmember_T *m = object_member_lookup(cl, name, len, &m_idx);
	إذا (m_idx >= 0)
	{
	    إذا (*name == '_' && !inside_class(cctx, cl))
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		اذهب پورو;
	    }

	    *arg = name_پڇاڙي;
	    إذا (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    وإلا
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    اذهب پورو;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_طريقو_idx(cl, name, len);
	إذا (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    إذا (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_طريقو_str), fp->uf_name);
		اذهب پورو;
	    }
	    *arg = name_پڇاڙي;
	    // Remove the object type from the stack
	    --cctx->ctx_type_ڍير.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, عدم);
	    اذهب پورو;
	}

	member_not_مليو_نياپو(cl, VAR_OBJECT, name, len);
    }
    وإلا
    {
	// load class member
	صحيح idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	إذا (m != عدم)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    إذا (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		اذهب پورو;
	    }

	    *arg = name_پڇاڙي;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ڍير.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    اذهب پورو;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_طريقو_idx(cl, name, len);
	إذا (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    إذا (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_طريقو_str), fp->uf_name);
		اذهب پورو;
	    }
	    *arg = name_پڇاڙي;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ڍير.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, عدم);
	    اذهب پورو;
	}

	member_not_مليو_نياپو(cl, VAR_CLASS, name, len);
    }

پورو:
    generic_func_دليلون_جدول_clear(&gfatab);
    أرجع ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    صحيح
compile_ڀريو_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *شروعات,	    // start of variable
	char_u **پڇاڙي,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    صحيح		    idx;
    imported_T	    *imp;

    إذا (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	أرجع FAIL;
    si = SCRIPT_شي(current_sctx.sc_sid);
    idx = get_script_شي_idx(current_sctx.sc_sid, name, 0, cctx, عدم);
    إذا (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_معلومات) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	أرجع OK;
    }

    إذا (پڇاڙي == عدم)
	imp = عدم;
    وإلا إذا (import == عدم)
	imp = ڳوليو_imported(name, 0, FALSE);
    وإلا
	imp = import;

    إذا (imp != عدم)
    {
	char_u	*p = skipwhite(*پڇاڙي);
	char_u	*exp_name;
	صحيح	cc;
	ufunc_T	*ufunc = عدم;
	type_T	*type;
	صحيح	پورو = FALSE;
	صحيح	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	إذا (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), شروعات);
	    أرجع FAIL;
	}
	++p;
	إذا (VIM_ISWHITE(*p))
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

	si = SCRIPT_شي(imp->imp_sid);
	إذا (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_ذريعو(cctx, imp->imp_sid);

	إذا (res == OK)
	{
	    إذا (si->sn_autoload_اڳياڙي != عدم
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_اڳياڙي, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		إذا (cc == '(' || paren_follows_after_اظهار)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		وإلا
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		پورو = TRUE;
	    }
	    وإلا إذا (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		إذا (cc == '(' || paren_follows_after_اظهار)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		وإلا
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		پورو = TRUE;
	    }
	    وإلا
	    {
		idx = ڳوليو_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, عدم, TRUE);
	    }
	}

	*p = cc;
	*پڇاڙي = p;
	إذا (پورو)
	    أرجع res;

	إذا (idx < 0)
	{
	    إذا (ufunc != عدم)
	    {
		gfargs_tab_T	gfatab;

		generic_func_دليلون_جدول_init(&gfatab);

		إذا (IS_GENERIC_FUNC(ufunc))
		{
		    إذا (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_دليلون(name, STRLEN(name), p,
								&gfatab, cctx);
			إذا (p != عدم)
			{
			    *پڇاڙي = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_دليلون_جدول_clear(&gfatab);

			إذا (p == عدم || ufunc == عدم)
			    أرجع FAIL;
		    }
		    وإلا
		    {
			emsg_funcname(e_generic_func_missing_type_دليلون_str,
									name);
			أرجع FAIL;
		    }
		}
		وإلا إذا (*p == '<')
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
    صحيح			has_g_اڳياڙي)
{
    ufunc_T *ufunc = ڳوليو_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    إذا (ufunc == عدم || (!has_g_اڳياڙي && func_requires_g_اڳياڙي(ufunc)))
	أرجع FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    إذا (ufunc == عدم)
	أرجع FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    إذا (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, عدم) == FAIL)
	أرجع FAIL;
    أرجع generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    ساكن صحيح
compiling_a_class_طريقو(cctx_T *cctx)
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
    صحيح			outer_اونهائي)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    صحيح		isn_idx;

    إذا (fp == عدم)
	أرجع FAIL;

    لكل (صحيح اونهائي = 0; اونهائي < outer_اونهائي; ++اونهائي)
    {
	إذا (outer_cctx->ctx_outer == عدم)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    أرجع FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    إذا (fp->uf_بلاڪ_ids == عدم && ufunc->uf_بلاڪ_اونهائي > 0)
    {
	fp->uf_بلاڪ_ids = ALLOC_MULT(صحيح, ufunc->uf_بلاڪ_اونهائي);
	إذا (fp->uf_بلاڪ_ids != عدم)
	{
	    mch_memmove(fp->uf_بلاڪ_ids, ufunc->uf_بلاڪ_ids,
					    حجم(صحيح) * ufunc->uf_بلاڪ_اونهائي);
	    fp->uf_بلاڪ_اونهائي = ufunc->uf_بلاڪ_اونهائي;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    إذا (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	أرجع FAIL;

    إذا (generate_FUNCREF(cctx, fp, عدم, FALSE, 0, &isn_idx) == FAIL)
	أرجع FAIL;
    إذا (cctx->ctx_skip == SKIP_YES)
	أرجع OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    إذا (outer_اونهائي > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_معلومات) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	إذا (extra == عدم)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    إذا (extra == عدم)
		أرجع FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_اونهائي = outer_اونهائي;
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
compile_ڀريو(
	char_u **arg,
	size_t  namelen,
	char_u *پڇاڙي_arg,
	cctx_T	*cctx,
	صحيح	is_اظهار,
	صحيح	error)
{
    type_T	*type;
    char_u	*name = عدم;
    char_u	*پڇاڙي = پڇاڙي_arg;
    صحيح		res = FAIL;
    صحيح		prev_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = عدم;

    generic_func_دليلون_جدول_init(&gfatab);

    إذا (*(*arg + namelen) == '<')
    {
	// generic function call
	إذا (parse_generic_func_type_دليلون(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == عدم)
	{
	    generic_func_دليلون_جدول_clear(&gfatab);
	    أرجع FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    إذا (*(*arg + 1) == ':')
    {
	إذا (پڇاڙي <= *arg + 2)
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
	    إذا (generate_instr_type(cctx, isn_type, &t_dict_any) == عدم)
		اذهب theend;
	    res = OK;
	}
	وإلا
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, پڇاڙي - (*arg + 2));
	    إذا (name == عدم)
		اذهب theend;

	    بدل (**arg)
	    {
		حالة 'v': res = generate_LOADV(cctx, name);
			  اكسر;
		حالة 's': إذا (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_اندريون_vim9_script_str),
									 *arg);
			      اذهب theend;
			  }
			  إذا (is_اظهار && ڳوليو_func(name, FALSE) != عدم)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  وإلا
			      res = compile_ڀريو_scriptvar(cctx, name,
							    عدم, &پڇاڙي, عدم);
			  اكسر;
		حالة 'g': إذا (vim_strchr(name, AUTOLOAD_CHAR) == عدم)
			  {
			      إذا (is_اظهار && ASCII_ISUPPER(*name)
				       && (ڳوليو_func(name, TRUE) != عدم
					   || gfatab.gfat_دليلون.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      وإلا
				  isn_type = ISN_LOADG;
			  }
			  وإلا
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, پڇاڙي - *arg);
			      إذا (name == عدم)
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
	    إذا (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_ڀريو(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    وإلا
    {
	صحيح	    idx;
	صحيح	    طريقو_idx;
	صحيح	    gen_ڀريو = FALSE;
	صحيح	    gen_ڀريو_outer = 0;
	صحيح	    outer_loop_اونهائي = -1;
	صحيح	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	إذا (name == عدم)
	    اذهب theend;

	إذا (STRCMP(name, "super") == 0 && compiling_a_class_طريقو(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = ڌڪو_type_ڍير(cctx, &t_super);
	    إذا (*پڇاڙي != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	وإلا إذا (vim_strchr(name, AUTOLOAD_CHAR) != عدم)
	{
	    script_autoload(name, FALSE);
	    res = generate_ڀريو(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	وإلا إذا (arg_exists(*arg, namelen, &idx, &type, &gen_ڀريو_outer, cctx)
									 == OK)
	{
	    إذا (gen_ڀريو_outer == 0)
		gen_ڀريو = TRUE;
	}
	وإلا
	{
	    lvar_T  lvar;
	    class_T *cl = عدم;

	    إذا (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		إذا (lvar.lv_ufunc != عدم && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_دليلون.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		وإلا إذا (lvar.lv_from_outer != 0)
		{
		    gen_ڀريو_outer = lvar.lv_from_outer;
		    outer_loop_اونهائي = lvar.lv_loop_اونهائي;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		وإلا
		    gen_ڀريو = TRUE;
	    }
	    وإلا إذا (cctx->ctx_ufunc->uf_defclass != عدم &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((طريقو_idx =
			     cctx_class_طريقو_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		إذا (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    إذا (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    وإلا
		    {
			ufunc_T *fp = cl->class_class_functions[طريقو_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, طريقو_idx,
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
		إذا (script_var_exists(*arg, namelen, cctx, عدم) == OK
			    || (imp = ڳوليو_imported(name, 0, FALSE)) != عدم)
		    res = compile_ڀريو_scriptvar(cctx, name, *arg, &پڇاڙي, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		إذا (res == FAIL && is_اظهار && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	إذا (gen_ڀريو)
	    res = generate_ڀريو(cctx, ISN_ڀريو, idx, عدم, type);
	إذا (gen_ڀريو_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_ڀريو_outer,
				       outer_loop_اونهائي, outer_loop_idx, type);
	    cctx->ctx_outer_استعمال_ٿيل = TRUE;
	}
    }

    *arg = پڇاڙي;

theend:
    إذا (lt != عدم)
	*lt = '<';
    إذا (res == FAIL && error && called_emsg == prev_called_emsg)
	semsg(_(e_variable_not_مليو_str), name);
    vim_free(name);
    generic_func_دليلون_جدول_clear(&gfatab);
    أرجع res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    ساكن صحيح
compile_string(isn_T *isn, cctx_T *cctx, صحيح str_وٿي)
{
    char_u	*s = isn->isn_arg.string + str_وٿي;
    garray_T	save_ga = cctx->ctx_instr;
    صحيح		اظهار_res;
    صحيح		trailing_error;
    صحيح		instr_ڳڻپ;
    isn_T	*instr = عدم;

    // Remove the string type from the stack.
    --cctx->ctx_type_ڍير.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_معلومات = عدم;

    // avoid peeking a next line
    صحيح galen_save = cctx->ctx_ufunc->uf_سٽون.ga_len;
    cctx->ctx_ufunc->uf_سٽون.ga_len = 0;

    اظهار_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_سٽون.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    إذا (اظهار_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	إذا (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_ڍير.ga_len;
	أرجع FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_ڳڻپ = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_معلومات;
    instr[instr_ڳڻپ].isn_type = ISN_پڄاڻي;

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
    صحيح	    must_پڇاڙي = FALSE;
    صحيح	    instr_ڳڻپ;

    لكل (;;)
    {
	إذا (may_get_ايندڙ_سٽ(whitep, &p, cctx) == FAIL)
	    اذهب failret;
	إذا (*p == ')')
	{
	    *arg = p + 1;
	    أرجع OK;
	}
	إذا (must_پڇاڙي)
	{
	    semsg(_(e_missing_comma_before_دليل_str), p);
	    أرجع FAIL;
	}

	instr_ڳڻپ = cctx->ctx_instr.ga_len;
	إذا (compile_expr0(&p, cctx) == FAIL)
	    أرجع FAIL;
	++*argcount;

	إذا (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_ڳڻپ + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_معلومات) + instr_ڳڻپ;

	    // {skip} argument of searchpair() can be compiled if not empty
	    إذا (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != عدم
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	وإلا إذا (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_ڳڻپ + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_معلومات) + instr_ڳڻپ;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    إذا (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != عدم
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	إذا (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    أرجع FAIL;
	}
	إذا (*p == ',')
	{
	    ++p;
	    إذا (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		أرجع FAIL;
	    }
	}
	وإلا
	    must_پڇاڙي = TRUE;
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
compile_builtin_طريقو_call(cctx_T *cctx, class_builtin_T builtin_طريقو)
{
    type_T	*type = get_decl_type_on_ڍير(cctx, 0);
    صحيح		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    إذا (type->tt_type == VAR_OBJECT)
    {
	صحيح	طريقو_idx;
	ufunc_T *uf = class_get_builtin_طريقو(type->tt_class, builtin_طريقو,
							&طريقو_idx);
	إذا (uf != عدم)
	    res = generate_CALL(cctx, uf, type->tt_class, طريقو_idx, 0, FALSE);
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

    إذا (varlen >= حجم(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	أرجع FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = ڳوليو_imported(name, varlen, FALSE);
    إذا (import != عدم)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	أرجع FAIL;
    }

    // We can evaluate "has('name')" at compile time.
    // We can evaluate "len('string')" at compile time.
    // We always evaluate "exists_compiled()" at compile time.
    إذا ((varlen == 3
	     && (STRNCMP(*arg, "has", 3) == 0 || STRNCMP(*arg, "len", 3) == 0))
	    || (varlen == 15 && STRNCMP(*arg, "exists_compiled", 6) == 0))
    {
	char_u	    *s = skipwhite(*arg + varlen + 1);
	typval_T    argvars[2];
	صحيح	    is_has = **arg == 'h';
	صحيح	    is_len = **arg == 'l';

	argvars[0].v_type = VAR_UNKNOWN;
	إذا (*s == '"')
	    (فراغ)eval_string(&s, &argvars[0], TRUE, FALSE);
	وإلا إذا (*s == '\'')
	    (فراغ)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	إذا (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    إذا (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_عدد;
		tv->vval.v_عدد = 0;
		إذا (is_has)
		    f_has(argvars, tv);
		وإلا إذا (is_len)
		    f_len(argvars, tv);
		وإلا
		    f_exists(argvars, tv);
		++ppconst->pp_استعمال_ٿيل;
	    }
	    clear_tv(&argvars[0]);
	    أرجع OK;
	}
	clear_tv(&argvars[0]);
	إذا (!is_has && !is_len)
	{
	    emsg(_(e_دليل_جو_exists_compiled_must_be_literal_string));
	    أرجع FAIL;
	}
    }

    إذا (generate_ppconst(cctx, ppconst) == FAIL)
	أرجع FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    إذا ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    وإلا إذا (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    وإلا
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_دليلون_جدول_init(&gfatab);

    إذا (*(*arg + varlen) == '<')
    {
	char_u	*شروعات = *arg;

	// generic function
	*arg = parse_generic_func_type_دليلون(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	إذا (*arg == عدم)
	    اذهب theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(شروعات, *arg - شروعات);
	إذا (gname == عدم)
	    اذهب theend;
	++*arg;			// skip '('
    }
    وإلا
	*arg += varlen + 1;

    إذا (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	اذهب theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != عدم;
    إذا (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	صحيح	    idx;

	// builtin function
	idx = ڳوليو_اندروني_func(name);
	إذا (idx >= 0)
	{
	    إذا (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_اندريون_vim9_script));
		اذهب theend;
	    }

	    إذا (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_ڍير(cctx, 1);
		إذا (check_type_is_قدر(get_type_on_ڍير(cctx, 0)) == FAIL)
		    اذهب theend;

		// add() can be compiled to instructions if we know the type
		إذا (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		وإلا إذا (type->tt_type == VAR_BLOB)
		{
		    // inline "add(blob, nr)" so that the type can be checked
		    res = generate_BLOBAPPEND(cctx);
		    idx = -1;
		}
	    }

	    إذا ((STRCMP(name, "writefile") == 0 && argcount > 2)
		    || (STRCMP(name, "mkdir") == 0 && argcount > 1))
	    {
		// May have the "D" or "R" flag, reserve a variable for a
		// deferred function call.
		إذا (get_defer_var_idx(cctx) == 0)
		    idx = -1;
	    }

	    class_builtin_T	builtin_طريقو = CLASS_BUILTIN_INVALID;
	    إذا (STRCMP(name, "string") == 0)
		builtin_طريقو = CLASS_BUILTIN_STRING;
	    وإلا إذا (STRCMP(name, "empty") == 0)
		builtin_طريقو = CLASS_BUILTIN_خالي_جايون;
	    وإلا إذا (STRCMP(name, "len") == 0)
		builtin_طريقو = CLASS_BUILTIN_LEN;
	    إذا (builtin_طريقو != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_طريقو_call(cctx, builtin_طريقو);
		إذا (res == OK)
		    idx = -1;
	    }

	    إذا (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	وإلا
	    emsg_funcname(e_unknown_function_str, namebuf);
	اذهب theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    إذا (lookup_local(namebuf, varlen, عدم, cctx) == FAIL
	    && arg_exists(namebuf, varlen, عدم, عدم, عدم, cctx) != OK)
    {
	class_T		*cl = عدم;
	صحيح		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = ڳوليو_func(name, FALSE);
	إذا (ufunc != عدم)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    إذا (ufunc == عدم)
		اذهب theend;

	    إذا (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, عدم, 0, argcount, FALSE);
		اذهب theend;
	    }
	    إذا (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == عدم
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		اذهب theend;
	    }
	}
	وإلا إذا ((mi = cctx_class_طريقو_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    إذا (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], عدم,
							0, argcount, FALSE);
	    }
	    وإلا
	    {
		semsg(_(e_class_طريقو_str_accessible_only_inside_class_str),
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
    إذا (!has_g_namespace && !is_autoload
	    && compile_ڀريو(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_ڍير(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	اذهب theend;
    }

    // If we can find a global function by name generate the right call.
    إذا (ufunc != عدم)
    {
	res = generate_CALL(cctx, ufunc, عدم, 0, argcount, FALSE);
	اذهب theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    إذا (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    وإلا
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_دليلون_جدول_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    أرجع res;
}

// like NAMESPACE_CHAR but with 'a' and 'l'.
#وصف_ڪريو VIM9_NAMESPACE_CHAR	(char_u *)"bgstvw"

/*
 * Find the end of a variable or function name.  Unlike find_name_end() this
 * does not recognize magic braces.
 * When "use_namespace" is TRUE recognize "b:", "s:", etc.
 * Return a pointer to just after the name.  Equal to "arg" if there is no
 * valid name.
 */
    char_u *
منزل_name_پڇاڙي(char_u *arg, صحيح use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    إذا (!eval_isnamec1(*arg))
	أرجع arg;

    لكل (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	إذا (*p == ':' && (p != arg + 1
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
منزل_name_const_پڇاڙي(char_u *arg)
{
    char_u	*p = arg;
    typval_T	rettv;

    إذا (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = منزل_name_پڇاڙي(p, TRUE);
    إذا (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	إذا (eval_list(&p, &rettv, عدم, FALSE) == FAIL)
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
    صحيح		ڳڻپ = 0;
    صحيح		is_const;
    صحيح		is_all_const = TRUE;	// reset when non-const encountered
    صحيح		must_پڇاڙي = FALSE;

    لكل (;;)
    {
	إذا (may_get_ايندڙ_سٽ(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_پڇاڙي_جو_list_rsb_str), *arg);
	    أرجع FAIL;
	}
	إذا (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    أرجع FAIL;
	}
	إذا (*p == ']')
	{
	    ++p;
	    اكسر;
	}
	إذا (must_پڇاڙي)
	{
	    semsg(_(e_missing_comma_اندريون_list_str), p);
	    أرجع FAIL;
	}
	إذا (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    أرجع FAIL;
	إذا (!is_const)
	    is_all_const = FALSE;
	++ڳڻپ;
	إذا (*p == ',')
	{
	    ++p;
	    إذا (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		أرجع FAIL;
	    }
	}
	وإلا
	    must_پڇاڙي = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    أرجع generate_NEWLIST(cctx, ڳڻپ, FALSE);
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
    صحيح پهريون_شي_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    صحيح		ڳڻپ = 0;
    صحيح		is_const;
    صحيح		is_all_const = TRUE;	// reset when non-const encountered
    صحيح		must_پڇاڙي = FALSE;

    إذا (**arg != ')')
    {
	إذا (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    أرجع FAIL;
	}
	ڳڻپ = 1;	// the first tuple item is already processed
	is_all_const = پهريون_شي_const;
	لكل (;;)
	{
	    إذا (may_get_ايندڙ_سٽ(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_پڇاڙي_جو_tuple_rsp_str), *arg);
		أرجع FAIL;
	    }
	    إذا (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		أرجع FAIL;
	    }
	    إذا (*p == ')')
	    {
		++p;
		اكسر;
	    }
	    إذا (must_پڇاڙي)
	    {
		semsg(_(e_missing_comma_اندريون_tuple_str), p);
		أرجع FAIL;
	    }
	    إذا (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		أرجع FAIL;
	    إذا (!is_const)
		is_all_const = FALSE;
	    ++ڳڻپ;
	    إذا (*p == ',')
	    {
		++p;
		إذا (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    أرجع FAIL;
		}
	    }
	    وإلا
		must_پڇاڙي = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    أرجع generate_NEWTUPLE(cctx, ڳڻپ, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    ساكن فراغ
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*سٽ;
    size_t	off;

    إذا (!evalarg->eval_using_cmdline || cctx == عدم)
	أرجع;

    gap = &evalarg->eval_tofree_ga;
    إذا (gap->ga_len == 0)
	أرجع;

    off = *arg - ((char_u **)gap->ga_معلومات)[gap->ga_len - 1];
    سٽ = ((char_u **)cctx->ctx_ufunc->uf_سٽون.ga_معلومات)[cctx->ctx_lnum];
    *arg = سٽ + off;
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
    إذا (r != OK)
    {
	clear_evalarg(&evalarg, عدم);
	أرجع r;
    }

    إذا (ppconst != عدم && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, عدم);
	أرجع FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    إذا (cctx->ctx_ufunc != عدم)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	صحيح بلاڪ_اونهائي = cctx->ctx_ufunc->uf_بلاڪ_اونهائي;

	إذا (بلاڪ_اونهائي > 0)
	{
	    ufunc->uf_بلاڪ_ids = ALLOC_MULT(صحيح, بلاڪ_اونهائي);
	    إذا (ufunc->uf_بلاڪ_ids != عدم)
	    {
		mch_memmove(ufunc->uf_بلاڪ_ids, cctx->ctx_ufunc->uf_بلاڪ_ids,
						    حجم(صحيح) * بلاڪ_اونهائي);
		ufunc->uf_بلاڪ_اونهائي = بلاڪ_اونهائي;
	    }
	}
    }

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    إذا (ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, cctx->ctx_compile_type, cctx);

    // When the outer function is compiled for profiling or debugging, the
    // lambda may be called without profiling or debugging.  Compile it here in
    // the right context.
    إذا (cctx->ctx_compile_type == CT_DEBUG
#جيڪڏهن_وصف_ٿيل FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#جيڪڏهن_پورو
       )
	compile_def_function(ufunc, FALSE, CT_NONE, cctx);

    // if the outer function is not compiled for debugging or profiling, this
    // one might be
    إذا (cctx->ctx_compile_type == CT_NONE)
    {
	compiletype_T compile_type = get_compile_type(ufunc);

	إذا (compile_type != CT_NONE)
	    compile_def_function(ufunc, FALSE, compile_type, cctx);
    }

    restore_cmdline_arg(&evalarg, arg, cctx);

    clear_evalarg(&evalarg, عدم);

    إذا (ufunc->uf_def_حالت == UF_COMPILED)
    {
	// The return type will now be known.
	سيٽ_function_type(ufunc);

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
    صحيح		save_sc_نسخو = current_sctx.sc_نسخو;

    // Get the funcref in "rettv".
    current_sctx.sc_نسخو = SCRIPT_نسخو_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, عدم);
    current_sctx.sc_نسخو = save_sc_نسخو;
    إذا (r != OK)
	أرجع r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    إذا (ufunc->uf_ret_type == عدم || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, عدم);

    إذا (ufunc->uf_def_حالت == UF_COMPILED)
    {
	// The return type will now be known.
	سيٽ_function_type(ufunc);
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
    صحيح		ڳڻپ = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*شي;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    صحيح		is_const;
    صحيح		is_all_const = TRUE;	// reset when non-const encountered

    إذا (d == عدم)
	أرجع FAIL;
    إذا (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	أرجع FAIL;
    }
    لكل (;;)
    {
	char_u	    *ڪنجي = عدم;

	إذا (may_get_ايندڙ_سٽ(whitep, arg, cctx) == FAIL)
	{
	    *arg = عدم;
	    اذهب failret;
	}

	إذا (**arg == '}')
	    اكسر;

	إذا (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    إذا (compile_expr0(arg, cctx) == FAIL)
		اذهب failret;
	    isn = ((isn_T *)instr->ga_معلومات) + instr->ga_len - 1;
	    إذا (isn->isn_type == ISN_PUSHNR)
	    {
		حرف buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.عدد);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    إذا (isn->isn_type == ISN_PUSHS)
		ڪنجي = isn->isn_arg.string;
	    وإلا إذا (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		اذهب failret;
	    *arg = skipwhite(*arg);
	    إذا (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_ڪنجي));
		اذهب failret;
	    }
	    ++*arg;
	}
	وإلا
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    ڪنجي = get_literal_ڪنجي(arg);
	    إذا (ڪنجي == عدم)
		اذهب failret;
	    إذا (generate_PUSHS(cctx, &ڪنجي) == FAIL)
		اذهب failret;
	}

	// Check for duplicate keys, if using string keys.
	إذا (ڪنجي != عدم)
	{
	    شي = dict_ڳوليو(d, ڪنجي, -1);
	    إذا (شي != عدم)
	    {
		semsg(_(e_duplicate_ڪنجي_اندريون_dictionary_str), ڪنجي);
		اذهب failret;
	    }
	    شي = dictitem_alloc(ڪنجي);
	    إذا (شي != عدم)
	    {
		شي->di_tv.v_type = VAR_UNKNOWN;
		شي->di_tv.v_تالو = 0;
		إذا (dict_جوڙيو(d, شي) == FAIL)
		    dictitem_free(شي);
	    }
	}

	إذا (**arg != ':')
	{
	    إذا (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    وإلا
		semsg(_(e_missing_colon_اندريون_dictionary_str), *arg);
	    اذهب failret;
	}
	whitep = *arg + 1;
	إذا (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    اذهب failret;
	}

	إذا (may_get_ايندڙ_سٽ(whitep, arg, cctx) == FAIL)
	{
	    *arg = عدم;
	    اذهب failret;
	}

	إذا (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    اذهب failret;
	إذا (!is_const)
	    is_all_const = FALSE;
	++ڳڻپ;

	whitep = *arg;
	إذا (may_get_ايندڙ_سٽ(whitep, arg, cctx) == FAIL)
	{
	    *arg = عدم;
	    اذهب failret;
	}
	إذا (**arg == '}')
	    اكسر;
	إذا (**arg != ',')
	{
	    semsg(_(e_missing_comma_اندريون_dictionary_str), *arg);
	    اذهب failret;
	}
	إذا (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    اذهب failret;
	}
	whitep = *arg + 1;
	إذا (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    اذهب failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    إذا (VIM_ISWHITE(**arg) && vim9_comment_شروعات(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    أرجع generate_NEWDICT(cctx, ڳڻپ, FALSE);

failret:
    إذا (*arg == عدم)
    {
	semsg(_(e_missing_dict_پڇاڙي_str), _("[end of lines]"));
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
    char_u	*شروعات = *arg;
    صحيح		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    إذا (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(شروعات, *arg - شروعات);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_عدد ? &t_عدد : &t_string;

	ret = generate_ڀريو(cctx, ISN_LOADOPT, 0, name, type);
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
    char_u	*شروعات = *arg;
    صحيح		len;
    صحيح		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    إذا (len == 0)
    {
	semsg(_(e_syntax_error_at_str), شروعات);
	أرجع FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(شروعات, len + 1);
    ret = generate_ڀريو(cctx, ISN_LOADENV, 0, name, &t_string);
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
    صحيح		ڳڻپ = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    لكل (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	إذا (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	وإلا
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	إذا (ret == FAIL)
	    اكسر;
	إذا (evaluate)
	{
	    إذا ((tv.vval.v_string != عدم && *tv.vval.v_string != NUL)
		    || (**arg != '{' && ڳڻپ == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		إذا (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    أرجع FAIL;
		tv.vval.v_string = عدم;  // don't free it now
		++ڳڻپ;
	    }
	    clear_tv(&tv);
	}

	إذا (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    اكسر;
	}

	p = compile_one_اظهار_اندريون_str(*arg, cctx);
	إذا (p == عدم)
	{
	    ret = FAIL;
	    اكسر;
	}
	++ڳڻپ;
	*arg = p;
    }

    إذا (ret == FAIL || !evaluate)
	أرجع ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    إذا (ڳڻپ > 1)
	أرجع generate_CONCAT(cctx, ڳڻپ);

    أرجع OK;
}

/*
 * Compile "@r".
 */
    ساكن صحيح
compile_get_رجسٽر(char_u **arg, cctx_T *cctx)
{
    صحيح		ret;

    ++*arg;
    إذا (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	أرجع FAIL;
    }
    إذا (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	أرجع FAIL;
    }
    ret = generate_ڀريو(cctx, ISN_LOADREG, **arg, عدم, &t_string);
    ++*arg;
    أرجع ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ساكن صحيح
apply_leader(typval_T *rettv, صحيح numeric_only, char_u *شروعات, char_u **پڇاڙي)
{
    char_u *p = *پڇاڙي;

    // this works from end to start
    طالما (p > شروعات)
    {
	--p;
	إذا (*p == '-' || *p == '+')
	{
	    // only '-' has an effect, for '+' we only check the type
	    إذا (rettv->v_type == VAR_FLOAT)
	    {
		إذا (*p == '-')
		    rettv->vval.v_float = -rettv->vval.v_float;
	    }
	    وإلا
	    {
		varnumber_T	val;
		صحيح		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		إذا (check_not_string(rettv) == FAIL)
		    أرجع FAIL;
		val = tv_get_عدد_chk(rettv, &error);
		clear_tv(rettv);
		إذا (error)
		    أرجع FAIL;
		إذا (*p == '-')
		    val = -val;
		rettv->v_type = VAR_عدد;
		rettv->vval.v_عدد = val;
	    }
	}
	وإلا إذا (numeric_only)
	{
	    ++p;
	    اكسر;
	}
	وإلا إذا (*p == '!')
	{
	    صحيح v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_عدد = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *پڇاڙي = p;
    أرجع OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    ساكن فراغ
get_vim_constant(char_u **arg, typval_T *rettv)
{
    إذا (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_عدد = VVAL_TRUE;
	*arg += 6;
    }
    وإلا إذا (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_عدد = VVAL_FALSE;
	*arg += 7;
    }
    وإلا إذا (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_عدد = VVAL_NULL;
	*arg += 6;
    }
    وإلا إذا (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_عدد = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, صحيح *len, صحيح *type_is)
{
    exprtype_T	type = اظهار_UNKNOWN;
    صحيح		i;

    بدل (p[0])
    {
	حالة '=':   إذا (p[1] == '=')
			type = اظهار_EQUAL;
		    وإلا إذا (p[1] == '~')
			type = اظهار_ميلاپ;
		    اكسر;
	حالة '!':   إذا (p[1] == '=')
			type = اظهار_NEQUAL;
		    وإلا إذا (p[1] == '~')
			type = اظهار_NOMATCH;
		    اكسر;
	حالة '>':   إذا (p[1] != '=')
		    {
			type = اظهار_GREATER;
			*len = 1;
		    }
		    وإلا
			type = اظهار_GEQUAL;
		    اكسر;
	حالة '<':   إذا (p[1] != '=')
		    {
			type = اظهار_SMALLER;
			*len = 1;
		    }
		    وإلا
			type = اظهار_SEQUAL;
		    اكسر;
	حالة 'i':   إذا (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			إذا (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			إذا (!محفوظ_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? اظهار_IS : اظهار_ISNOT;
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
skip_اظهار_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_اظهار(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, عدم);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    صحيح
bool_on_ڍير(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_ڍير(cctx, 0);
    إذا (type == &t_bool)
	أرجع OK;

    إذا (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_عدد
	    || type == &t_عدد_bool
	    || type == &t_const_عدد_bool)
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
compile_leader(cctx_T *cctx, صحيح numeric_only, char_u *شروعات, char_u **پڇاڙي)
{
    char_u	*p = *پڇاڙي;

    // this works from end to start
    طالما (p > شروعات)
    {
	--p;
	طالما (VIM_ISWHITE(*p))
	    --p;
	إذا (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_ڍير(cctx, 0);
	    إذا (type->tt_type != VAR_FLOAT && need_type(type, &t_عدد,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		أرجع FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    إذا (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == عدم)
		أرجع FAIL;
	}
	وإلا إذا (numeric_only)
	{
	    ++p;
	    اكسر;
	}
	وإلا
	{
	    صحيح  invert = *p == '!';

	    طالما (p > شروعات && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		إذا (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    إذا (check_type_is_قدر(get_type_on_ڍير(cctx, 0)) == FAIL)
		أرجع FAIL;
	    إذا (generate_2BOOL(cctx, invert, -1) == FAIL)
		أرجع FAIL;
	}
    }
    *پڇاڙي = p;
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

    إذا (may_get_ايندڙ_سٽ_error(p, arg, cctx) == FAIL)
	أرجع FAIL;

    إذا (**arg == ')')
	// empty tuple
	أرجع compile_tuple(arg, cctx, ppconst, FALSE);

    إذا (ppconst->pp_استعمال_ٿيل <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    وإلا
    {
	// Not enough space in ppconst, flush constants.
	إذا (generate_ppconst(cctx, ppconst) == FAIL)
	    أرجع FAIL;
	ret = compile_expr0(arg, cctx);
    }
    إذا (may_get_ايندڙ_سٽ_error(*arg, arg, cctx) == FAIL)
	أرجع FAIL;
    إذا (ret == OK && **arg == ',')
    {
	// tuple
	صحيح is_const = ppconst->pp_استعمال_ٿيل > 0 || ppconst->pp_is_const;
	إذا (generate_ppconst(cctx, ppconst) == FAIL)
	    أرجع FAIL;
	أرجع compile_tuple(arg, cctx, ppconst, is_const);
    }

    إذا (**arg == ')')
	++*arg;
    وإلا إذا (ret == OK)
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
	char_u *شروعات_leader,
	char_u **پڇاڙي_leader,
	ppconst_T *ppconst)
{
    char_u	*name_شروعات = *پڇاڙي_leader;
    صحيح		keeping_dict = FALSE;

    لكل (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	إذا (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_شروعات(p)))
	{
	    char_u *ايندڙ = peek_ايندڙ_سٽ_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    إذا (ايندڙ != عدم &&
		    ((ايندڙ[0] == '-' && ايندڙ[1] == '>'
				 && (ايندڙ[2] == '{'
				       || *skipwhite(ايندڙ + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(ايندڙ + 2))))
		    || (ايندڙ[0] == '.' && eval_isdictc(ايندڙ[1]))))
	    {
		ايندڙ = ايندڙ_سٽ_from_context(cctx, TRUE);
		إذا (ايندڙ == عدم)
		    أرجع FAIL;
		*arg = ايندڙ;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	إذا (**arg == '(')
	{
	    صحيح		argcount = 0;

	    إذا (generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_ڍير(cctx, 0);

	    *arg = skipwhite(p + 1);
	    إذا (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		أرجع FAIL;
	    إذا (generate_PCALL(cctx, argcount, name_شروعات, type, TRUE) == FAIL)
		أرجع FAIL;
	    إذا (keeping_dict)
	    {
		keeping_dict = FALSE;
		إذا (generate_instr(cctx, ISN_CLEARDICT) == عدم)
		    أرجع FAIL;
	    }
	}
	وإلا إذا (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    صحيح	    alt;
	    char_u  *paren;

	    // something->method()
	    إذا (generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ڍير(cctx, 0);
	    إذا (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_قدر));
		أرجع FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    إذا (compile_leader(cctx, TRUE, شروعات_leader, پڇاڙي_leader) == FAIL)
		أرجع FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    إذا (**arg == '(')
		alt = 2;
	    وإلا
	    {
		// alternative 1 or 3
		p = *arg;
		إذا (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    أرجع FAIL;
		}
		إذا (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		لكل ( ; eval_isnamec(*p); ++p)
		    ;
		إذا (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    إذا (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			أرجع FAIL;
		}
		وإلا
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    إذا (paren == عدم)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			أرجع FAIL;
		    }
		}
	    }

	    إذا (alt != 1)
	    {
		صحيح	    argcount = 1;
		garray_T    *ڍير = &cctx->ctx_type_ڍير;
		صحيح	    type_idx_شروعات = ڍير->ga_len;
		صحيح	    اظهار_isn_شروعات = cctx->ctx_instr.ga_len;
		صحيح	    اظهار_isn_پڇاڙي;
		صحيح	    arg_isn_ڳڻپ;

		إذا (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    إذا (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			أرجع FAIL;
		}
		وإلا
		{
		    صحيح fail;
		    صحيح save_len = cctx->ctx_ufunc->uf_سٽون.ga_len;
		    صحيح	prev_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_اظهار;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_سٽون.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_اظهار;
		    cctx->ctx_ufunc->uf_سٽون.ga_len = save_len;

		    إذا (fail)
		    {
			إذا (did_emsg == prev_did_emsg)
			    semsg(_(e_invalid_اظهار_str), pstart);
			أرجع FAIL;
		    }
		}

		// Compile the arguments.
		إذا (**arg != '(')
		{
		    إذا (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    وإلا
			semsg(_(e_missing_parenthesis_str), *arg);
		    أرجع FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		اظهار_isn_پڇاڙي = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		إذا (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    أرجع FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_ڳڻپ = cctx->ctx_instr.ga_len - اظهار_isn_پڇاڙي;
		إذا (arg_isn_ڳڻپ > 0)
		{
		    صحيح	    اظهار_isn_ڳڻپ = اظهار_isn_پڇاڙي - اظهار_isn_شروعات;
		    isn_T   *isn = ALLOC_MULT(isn_T, اظهار_isn_ڳڻپ);
		    type_T  *decl_type;
		    type2_T  *typep;

		    إذا (isn == عدم)
			أرجع FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_معلومات)
							      + اظهار_isn_شروعات,
					       حجم(isn_T) * اظهار_isn_ڳڻپ);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_معلومات)
							      + اظهار_isn_شروعات,
			     ((isn_T *)cctx->ctx_instr.ga_معلومات) + اظهار_isn_پڇاڙي,
						حجم(isn_T) * arg_isn_ڳڻپ);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_معلومات)
					      + اظهار_isn_شروعات + arg_isn_ڳڻپ,
					  isn, حجم(isn_T) * اظهار_isn_ڳڻپ);
		    vim_free(isn);

		    typep = ((type2_T *)ڍير->ga_معلومات) + type_idx_شروعات;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)ڍير->ga_معلومات) + type_idx_شروعات,
			      ((type2_T *)ڍير->ga_معلومات) + type_idx_شروعات + 1,
			      حجم(type2_T)
				       * (ڍير->ga_len - type_idx_شروعات - 1));
		    typep = ((type2_T *)ڍير->ga_معلومات) + ڍير->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_ڍير(cctx, 0);
		إذا (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    أرجع FAIL;
	    }

	    إذا (keeping_dict)
	    {
		keeping_dict = FALSE;
		إذا (generate_instr(cctx, ISN_CLEARDICT) == عدم)
		    أرجع FAIL;
	    }
	}
	وإلا إذا (**arg == '[')
	{
	    صحيح		is_ٽڪرو = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    إذا (generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    إذا (may_get_ايندڙ_سٽ_error(p, arg, cctx) == FAIL)
		أرجع FAIL;
	    إذا (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    وإلا
	    {
		إذا (compile_expr0(arg, cctx) == FAIL)
		    أرجع FAIL;
		إذا (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    أرجع FAIL;
		}
		إذا (may_get_ايندڙ_سٽ_error(*arg, arg, cctx) == FAIL)
		    أرجع FAIL;
		*arg = skipwhite(*arg);
	    }
	    إذا (**arg == ':')
	    {
		is_ٽڪرو = TRUE;
		++*arg;
		إذا (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    أرجع FAIL;
		}
		إذا (may_get_ايندڙ_سٽ_error(*arg, arg, cctx) == FAIL)
		    أرجع FAIL;
		إذا (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		وإلا
		{
		    إذا (compile_expr0(arg, cctx) == FAIL)
			أرجع FAIL;
		    إذا (may_get_ايندڙ_سٽ_error(*arg, arg, cctx) == FAIL)
			أرجع FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    إذا (**arg != ']')
	    {
		emsg(_(e_missing_closing_چورس_brace));
		أرجع FAIL;
	    }
	    *arg = *arg + 1;

	    إذا (keeping_dict)
	    {
		keeping_dict = FALSE;
		إذا (generate_instr(cctx, ISN_CLEARDICT) == عدم)
		    أرجع FAIL;
	    }
	    إذا (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_ٽڪرو, &keeping_dict, cctx) == FAIL)
		أرجع FAIL;
	}
	وإلا إذا (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    إذا (generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ڍير(cctx, 0);
	    إذا (type != &t_unknown
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
		إذا (compile_class_object_اشاريو(cctx, arg, type) == FAIL)
		    أرجع FAIL;
	    }
	    وإلا
	    {
		*arg = p + 1;
		إذا (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    أرجع FAIL;
		}
		p = *arg;
		طالما (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		إذا (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    أرجع FAIL;
		}
		إذا (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == عدم)
		    أرجع FAIL;
		إذا (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
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
    إذا (keeping_dict && cctx->ctx_skip != SKIP_YES
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
    char_u	*شروعات_leader, *پڇاڙي_leader;
    صحيح		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل];
    صحيح		استعمال_ٿيل_before = ppconst->pp_استعمال_ٿيل;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    شروعات_leader = *arg;
    إذا (eval_leader(arg, TRUE) == FAIL)
	أرجع FAIL;
    پڇاڙي_leader = *arg;

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
	حالة '.':   إذا (eval_عدد(arg, rettv, TRUE, FALSE) == FAIL)
			أرجع FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    إذا (apply_leader(rettv, TRUE,
					    شروعات_leader, &پڇاڙي_leader) == FAIL)
		    {
			clear_tv(rettv);
			أرجع FAIL;
		    }
		    اكسر;

	/*
	 * String constant: "string".
	 */
	حالة '"':   إذا (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			أرجع FAIL;
		    اكسر;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	حالة '\'':  إذا (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
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
	حالة 't':   إذا (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_عدد = VVAL_TRUE;
		    }
		    وإلا
			ret = NOTDONE;
		    اكسر;

	/*
	 * "false" constant
	 */
	حالة 'f':   إذا (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_عدد = VVAL_FALSE;
		    }
		    وإلا
			ret = NOTDONE;
		    اكسر;

	/*
	 * "null" or "null_*" constant
	 */
	حالة 'n':   إذا (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			صحيح	len;

			لكل (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			إذا (ret == FAIL)
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
	حالة '[':   إذا (generate_ppconst(cctx, ppconst) == FAIL)
			أرجع FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    اكسر;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	حالة '{':   إذا (generate_ppconst(cctx, ppconst) == FAIL)
			أرجع FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    اكسر;

	/*
	 * Option value: &name
	 */
	حالة '&':	إذا (generate_ppconst(cctx, ppconst) == FAIL)
			    أرجع FAIL;
			ret = compile_get_option(arg, cctx);
			اكسر;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	حالة '$':	إذا (generate_ppconst(cctx, ppconst) == FAIL)
			    أرجع FAIL;
			إذا ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			وإلا
			    ret = compile_get_env(arg, cctx);
			اكسر;

	/*
	 * Register contents: @r.
	 */
	حالة '@':	إذا (generate_ppconst(cctx, ppconst) == FAIL)
			    أرجع FAIL;
			ret = compile_get_رجسٽر(arg, cctx);
			اكسر;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	حالة '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    إذا (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    اكسر;

	افتراضي:    ret = NOTDONE;
		    اكسر;
    }
    إذا (ret == FAIL)
	أرجع FAIL;

    إذا (rettv->v_type != VAR_UNKNOWN && استعمال_ٿيل_before == ppconst->pp_استعمال_ٿيل)
    {
	إذا (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	وإلا
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_استعمال_ٿيل;
    }
    وإلا إذا (ret == NOTDONE)
    {
	char_u	    *p;
	صحيح	    r;

	إذا (!eval_isnamec1(**arg))
	{
	    إذا (!vim9_bad_comment(*arg))
	    {
		إذا (پڇاڙيون_excmd(*skipwhite(*arg)))
		    semsg(_(e_خالي_جايون_اظهار_str), *arg);
		وإلا
		    semsg(_(e_name_expected_str), *arg);
	    }
	    أرجع FAIL;
	}

	// "name" or "name()"
	p = منزل_name_پڇاڙي(*arg, TRUE);
	إذا (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_هتي));
	    أرجع FAIL;
	}

	size_t namelen = p - *arg;
	إذا (*p == '<')
	{
	    إذا (skip_generic_func_type_دليلون(&p) == FAIL)
		أرجع FAIL;
	}

	إذا (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	وإلا
	{
	    إذا (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		أرجع FAIL;
	    r = compile_ڀريو(arg, namelen, p, cctx, TRUE, TRUE);
	}
	إذا (r == FAIL)
	    أرجع FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    إذا (compile_subscript(arg, cctx, شروعات_leader, &پڇاڙي_leader,
							     ppconst) == FAIL)
	أرجع FAIL;
    إذا ((ppconst->pp_استعمال_ٿيل > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل - 1];
	إذا (apply_leader(rettv, FALSE, شروعات_leader, &پڇاڙي_leader) == FAIL)
	    أرجع FAIL;
	أرجع OK;
    }
    إذا (compile_leader(cctx, FALSE, شروعات_leader, &پڇاڙي_leader) == FAIL)
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
    إذا (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	إذا (want_type == عدم)
	    أرجع FAIL;

	إذا (**arg != '>')
	{
	    إذا (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    وإلا
		emsg(_(e_missing_gt));
	    أرجع FAIL;
	}
	++*arg;
	إذا (may_get_ايندڙ_سٽ_error(*arg, arg, cctx) == FAIL)
	    أرجع FAIL;
    }

    إذا (compile_expr9(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    إذا (want_type != عدم)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_قسم = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_ڍير(cctx, 0);
	إذا (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    إذا (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
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
    char_u	*ايندڙ;
    صحيح		ppconst_استعمال_ٿيل = ppconst->pp_استعمال_ٿيل;

    // get the first expression
    إذا (compile_expr8(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    لكل (;;)
    {
	op = may_peek_ايندڙ_سٽ(cctx, *arg, &ايندڙ);
	إذا (*op != '*' && *op != '/' && *op != '%')
	    اكسر;
	إذا (ايندڙ != عدم)
	{
	    *arg = ايندڙ_سٽ_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	إذا (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    أرجع FAIL;
	}
	إذا (may_get_ايندڙ_سٽ_error(op + 1, arg, cctx) == FAIL)
	    أرجع FAIL;

	// get the second expression
	إذا (compile_expr8(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	إذا (ppconst->pp_استعمال_ٿيل == ppconst_استعمال_ٿيل + 2
		&& ppconst->pp_tv[ppconst_استعمال_ٿيل].v_type == VAR_عدد
		&& ppconst->pp_tv[ppconst_استعمال_ٿيل + 1].v_type == VAR_عدد)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_استعمال_ٿيل];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_استعمال_ٿيل + 1];
	    varnumber_T	    res = 0;
	    صحيح		    failed = FALSE;

	    // both are numbers: compute the result
	    بدل (*op)
	    {
		حالة '*': res = tv1->vval.v_عدد * tv2->vval.v_عدد;
			  اكسر;
		حالة '/': res = انگ_divide(tv1->vval.v_عدد,
						  tv2->vval.v_عدد, &failed);
			  اكسر;
		حالة '%': res = انگ_modulus(tv1->vval.v_عدد,
						  tv2->vval.v_عدد, &failed);
			  اكسر;
	    }
	    إذا (failed)
		أرجع FAIL;
	    tv1->vval.v_عدد = res;
	    --ppconst->pp_استعمال_ٿيل;
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
    char_u	*ايندڙ;
    صحيح		oplen;
    صحيح		ppconst_استعمال_ٿيل = ppconst->pp_استعمال_ٿيل;

    // get the first variable
    إذا (compile_expr7(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    لكل (;;)
    {
	op = may_peek_ايندڙ_سٽ(cctx, *arg, &ايندڙ);
	إذا (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    اكسر;
	إذا (op[0] == op[1] && *op != '.' && ايندڙ)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    اكسر;
	oplen = (*op == '.' ? 2 : 1);
	إذا (ايندڙ != عدم)
	{
	    *arg = ايندڙ_سٽ_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	إذا (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    أرجع FAIL;
	}

	إذا (may_get_ايندڙ_سٽ_error(op + oplen, arg, cctx) == FAIL)
	    أرجع FAIL;

	// get the second expression
	إذا (compile_expr7(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	إذا (ppconst->pp_استعمال_ٿيل == ppconst_استعمال_ٿيل + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_استعمال_ٿيل].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_استعمال_ٿيل + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_استعمال_ٿيل].v_type == VAR_عدد
		    && ppconst->pp_tv[ppconst_استعمال_ٿيل + 1].v_type == VAR_عدد)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_استعمال_ٿيل];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_استعمال_ٿيل + 1];

	    // concat/subtract/add constant numbers
	    إذا (*op == '+')
		tv1->vval.v_عدد = tv1->vval.v_عدد + tv2->vval.v_عدد;
	    وإلا إذا (*op == '-')
		tv1->vval.v_عدد = tv1->vval.v_عدد - tv2->vval.v_عدد;
	    وإلا
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		إذا (tv1->vval.v_string == عدم)
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
	    --ppconst->pp_استعمال_ٿيل;
	}
	وإلا
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    إذا (*op == '.')
	    {
		إذا (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    أرجع FAIL;
		إذا (generate_CONCAT(cctx, 2) == FAIL)
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
    exprtype_T	type = اظهار_UNKNOWN;
    char_u	*p;
    char_u	*ايندڙ;
    صحيح		len = 2;
    صحيح		ppconst_استعمال_ٿيل = ppconst->pp_استعمال_ٿيل;
    isn_T	*isn;

    // get the first variable
    إذا (compile_expr6(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    لكل (;;)
    {
	type = اظهار_UNKNOWN;

	p = may_peek_ايندڙ_سٽ(cctx, *arg, &ايندڙ);
	إذا (p[0] == '<' && p[1] == '<')
	    type = اظهار_LSHIFT;
	وإلا إذا (p[0] == '>' && p[1] == '>')
	    type = اظهار_RSHIFT;

	إذا (type == اظهار_UNKNOWN)
	    أرجع OK;

	// Handle a bitwise left or right shift operator
	إذا (ppconst->pp_استعمال_ٿيل == ppconst_استعمال_ٿيل + 1)
	{
	    إذا (ppconst->pp_tv[ppconst->pp_استعمال_ٿيل - 1].v_type != VAR_عدد)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_عدد));
		أرجع FAIL;
	    }
	}
	وإلا
	{
	    type_T	*t = get_type_on_ڍير(cctx, 0);

	    إذا (need_type(t, &t_عدد, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_عدد));
		أرجع FAIL;
	    }
	}

	إذا (ايندڙ != عدم)
	{
	    *arg = ايندڙ_سٽ_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	إذا (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    أرجع FAIL;
	}

	// get the second variable
	إذا (may_get_ايندڙ_سٽ_error(p + len, arg, cctx) == FAIL)
	    أرجع FAIL;

	إذا (compile_expr6(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	إذا (ppconst->pp_استعمال_ٿيل == ppconst_استعمال_ٿيل + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل - 1];

	    // Both sides are a constant, compute the result now.
	    إذا (tv2->v_type != VAR_عدد || tv2->vval.v_عدد < 0)
	    {
		// right operand should be a positive number
		إذا (tv2->v_type != VAR_عدد)
		    emsg(_(e_bitshift_ops_must_be_عدد));
		وإلا
		    emsg(_(e_bitshift_ops_must_be_مثبت));
		أرجع FAIL;
	    }

	    إذا (tv2->vval.v_عدد > MAX_LSHIFT_بٽ)
		tv1->vval.v_عدد = 0;
	    وإلا إذا (type == اظهار_LSHIFT)
		tv1->vval.v_عدد =
			(uvarnumber_T)tv1->vval.v_عدد << tv2->vval.v_عدد;
	    وإلا
		tv1->vval.v_عدد =
			(uvarnumber_T)tv1->vval.v_عدد >> tv2->vval.v_عدد;
	    clear_tv(tv2);
	    --ppconst->pp_استعمال_ٿيل;
	}
	وإلا
	{
	    إذا (need_type(get_type_on_ڍير(cctx, 0), &t_عدد, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_عدد));
		أرجع FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    إذا (isn == عدم)
		أرجع FAIL;

	    إذا (isn != عدم)
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
    exprtype_T	type = اظهار_UNKNOWN;
    char_u	*p;
    char_u	*ايندڙ;
    صحيح		len = 2;
    صحيح		type_is = FALSE;
    صحيح		ppconst_استعمال_ٿيل = ppconst->pp_استعمال_ٿيل;

    // get the first variable
    إذا (compile_expr5(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    p = may_peek_ايندڙ_سٽ(cctx, *arg, &ايندڙ);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    إذا (type != اظهار_UNKNOWN)
    {
	صحيح ic = FALSE;  // Default: do not ignore case

	إذا (ايندڙ != عدم)
	{
	    *arg = ايندڙ_سٽ_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	إذا (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_اظهار_str), *arg);
	    أرجع FAIL;
	}
	// extra question mark appended: ignore case
	إذا (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	وإلا إذا (p[len] == '#')
	    ++len;
	// nothing appended: match case

	إذا (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    أرجع FAIL;
	}

	// get the second variable
	إذا (may_get_ايندڙ_سٽ_error(p + len, arg, cctx) == FAIL)
	    أرجع FAIL;

	إذا (compile_expr5(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	إذا (ppconst->pp_استعمال_ٿيل == ppconst_استعمال_ٿيل + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل - 1];
	    صحيح		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    إذا (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    وإلا
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_عدد = tv1->vval.v_عدد
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_استعمال_ٿيل;
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
	صحيح	ppconst_استعمال_ٿيل اڻ_استعمال_ٿيل)
{
    char_u	*ايندڙ;
    char_u	*p = may_peek_ايندڙ_سٽ(cctx, *arg, &ايندڙ);
    صحيح		opchar = *op;

    إذا (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	پڇاڙي_ga;
	صحيح		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&پڇاڙي_ga, حجم(صحيح), 10);
	طالما (p[0] == opchar && p[1] == opchar)
	{
	    ڊگهو	شروعات_lnum = SOURCING_LNUM;
	    ڊگهو	save_sourcing_lnum;
	    صحيح		شروعات_ctx_lnum = cctx->ctx_lnum;
	    صحيح		save_lnum;
	    صحيح		const_استعمال_ٿيل;
	    صحيح		حالت;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    إذا (ايندڙ != عدم)
	    {
		*arg = ايندڙ_سٽ_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    إذا (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&پڇاڙي_ga);
		أرجع FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = شروعات_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = شروعات_ctx_lnum;

	    حالت = check_ppconst_bool(ppconst);
	    إذا (حالت != FAIL)
	    {
		// Use the last ppconst if possible.
		إذا (ppconst->pp_استعمال_ٿيل > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل - 1];
		    صحيح		is_true = tv2bool(tv);

		    إذا ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_عدد = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    وإلا
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_استعمال_ٿيل;
			jump_when = JUMP_NEVER;
		    }
		}
		وإلا
		{
		    // Every part must evaluate to a bool.
		    حالت = bool_on_ڍير(cctx);
		}
	    }
	    إذا (حالت != FAIL)
		حالت = ga_grow(&پڇاڙي_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    إذا (حالت == FAIL)
	    {
		ga_clear(&پڇاڙي_ga);
		أرجع FAIL;
	    }

	    إذا (jump_when != JUMP_NEVER)
	    {
		إذا (cctx->ctx_skip != SKIP_YES)
		{
		    *(((صحيح *)پڇاڙي_ga.ga_معلومات) + پڇاڙي_ga.ga_len) = instr->ga_len;
		    ++پڇاڙي_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    إذا (may_get_ايندڙ_سٽ_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&پڇاڙي_ga);
		أرجع FAIL;
	    }

	    const_استعمال_ٿيل = ppconst->pp_استعمال_ٿيل;
	    إذا ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&پڇاڙي_ga);
		أرجع FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    إذا (ppconst->pp_استعمال_ٿيل == const_استعمال_ٿيل + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_استعمال_ٿيل - 1];

		إذا (tv->v_type == VAR_عدد
			 && (tv->vval.v_عدد == 1 || tv->vval.v_عدد == 0))
		{
		    tv->vval.v_عدد = tv->vval.v_عدد == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_ايندڙ_سٽ(cctx, *arg, &ايندڙ);
	}

	إذا (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&پڇاڙي_ga);
	    أرجع FAIL;
	}

	إذا (cctx->ctx_skip != SKIP_YES && ppconst->pp_استعمال_ٿيل == 0)
	    // Every part must evaluate to a bool.
	    إذا (bool_on_ڍير(cctx) == FAIL)
	    {
		ga_clear(&پڇاڙي_ga);
		أرجع FAIL;
	    }

	إذا (پڇاڙي_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    طالما (پڇاڙي_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--پڇاڙي_ga.ga_len;
		isn = ((isn_T *)instr->ga_معلومات)
				  + *(((صحيح *)پڇاڙي_ga.ga_معلومات) + پڇاڙي_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&پڇاڙي_ga);

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
    صحيح		ppconst_استعمال_ٿيل = ppconst->pp_استعمال_ٿيل;

    // get the first variable
    إذا (compile_expr4(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    // || and && work almost the same
    أرجع compile_and_or(arg, cctx, "&&", ppconst, ppconst_استعمال_ٿيل);
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
    صحيح		ppconst_استعمال_ٿيل = ppconst->pp_استعمال_ٿيل;

    // eval the first expression
    إذا (compile_expr3(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    // || and && work almost the same
    أرجع compile_and_or(arg, cctx, "||", ppconst, ppconst_استعمال_ٿيل);
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
    صحيح		ppconst_استعمال_ٿيل = ppconst->pp_استعمال_ٿيل;
    char_u	*ايندڙ;

    // Ignore all kinds of errors when not producing code.
    إذا (cctx->ctx_skip == SKIP_YES)
    {
	صحيح		prev_did_emsg = did_emsg;

	skip_اظهار_cctx(arg, cctx);
	أرجع did_emsg == prev_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    إذا (compile_expr2(arg, cctx, ppconst) == FAIL)
	أرجع FAIL;

    p = may_peek_ايندڙ_سٽ(cctx, *arg, &ايندڙ);
    إذا (*p == '?')
    {
	صحيح		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*ڍير = &cctx->ctx_type_ڍير;
	صحيح		alt_idx = instr->ga_len;
	صحيح		پڇاڙي_idx = 0;
	isn_T		*isn;
	type_T		*type1 = عدم;
	صحيح		has_const_اظهار = FALSE;
	صحيح		const_قدر = FALSE;
	صحيح		save_skip = cctx->ctx_skip;

	إذا (ايندڙ != عدم)
	{
	    *arg = ايندڙ_سٽ_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	إذا (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    أرجع FAIL;
	}

	إذا (ppconst->pp_استعمال_ٿيل == ppconst_استعمال_ٿيل + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_اظهار = TRUE;
	    إذا (op_falsy)
		const_قدر = tv2bool(&ppconst->pp_tv[ppconst_استعمال_ٿيل]);
	    وإلا
	    {
		صحيح error = FALSE;

		const_قدر = tv_get_bool_chk(&ppconst->pp_tv[ppconst_استعمال_ٿيل],
								       &error);
		إذا (error)
		    أرجع FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_قدر : !const_قدر) ? SKIP_YES : SKIP_NOT;

	    إذا (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    وإلا
	    {
		clear_tv(&ppconst->pp_tv[ppconst_استعمال_ٿيل]);
		--ppconst->pp_استعمال_ٿيل;
	    }
	}
	وإلا
	{
	    generate_ppconst(cctx, ppconst);
	    إذا (op_falsy)
		پڇاڙي_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    إذا (op_falsy)
	    {
		type1 = get_type_on_ڍير(cctx, -1);
		إذا (check_type_is_قدر(type1) == FAIL)
		    أرجع FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	إذا (may_get_ايندڙ_سٽ_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    أرجع FAIL;
	إذا (compile_expr1(arg, cctx, ppconst) == FAIL)
	    أرجع FAIL;

	إذا (!has_const_اظهار)
	{
	    generate_ppconst(cctx, ppconst);

	    إذا (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_ڍير(cctx, 0);
		--ڍير->ga_len;

		پڇاڙي_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_معلومات) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	إذا (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_ايندڙ_سٽ(cctx, *arg, &ايندڙ);
	    إذا (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		أرجع FAIL;
	    }
	    إذا (ايندڙ != عدم)
	    {
		*arg = ايندڙ_سٽ_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    إذا (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		أرجع FAIL;
	    }

	    // evaluate the third expression
	    إذا (has_const_اظهار)
		cctx->ctx_skip = save_skip == SKIP_YES || const_قدر
							 ? SKIP_YES : SKIP_NOT;
	    إذا (may_get_ايندڙ_سٽ_error(p + 1, arg, cctx) == FAIL)
		أرجع FAIL;
	    إذا (compile_expr1(arg, cctx, ppconst) == FAIL)
		أرجع FAIL;
	}

	إذا (!has_const_اظهار)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)ڍير->ga_معلومات)
					      + ڍير->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_معلومات) + پڇاڙي_idx;
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

    CLEAR_ميدان(ppconst);
    إذا (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	أرجع FAIL;
    }
    إذا (is_const != عدم)
	*is_const = ppconst.pp_استعمال_ٿيل > 0 || ppconst.pp_is_const;
    إذا (generate_ppconst(cctx, &ppconst) == FAIL)
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


#جيڪڏهن_پورو // defined(FEAT_EVAL)
