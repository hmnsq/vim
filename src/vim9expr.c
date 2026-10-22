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

#تعریف_کرو USING_FLOAT_STUFF
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// flag passed from compile_subscript() to compile_load_scriptvar()
ساکن صحیح paren_follows_after_اظہاریہ = 0;

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
    برائے (i = 0; i < ppconst->pp_مستعمل; ++i)
	اگر (generate_tv_ڈالو(cctx, &ppconst->pp_tv[i]) == FAIL)
	    ret = FAIL;
    ppconst->pp_مستعمل = 0;
    cctx->ctx_skip = save_skip;
    واپس ret;
}

/*
 * Check that the last item of "ppconst" is a bool, if there is an item.
 */
    ساکن صحیح
check_ppconst_bool(ppconst_T *ppconst)
{
    اگر (ppconst->pp_مستعمل > 0)
    {
	typval_T    *tv = &ppconst->pp_tv[ppconst->pp_مستعمل - 1];
	where_T	    where = WHERE_INIT;

	واپس check_typval_type(&t_bool, tv, where);
    }
    واپس OK;
}

/*
 * Clear ppconst constants.  Used when failing.
 */
    خالی
clear_ppconst(ppconst_T *ppconst)
{
    صحیح	    i;

    برائے (i = 0; i < ppconst->pp_مستعمل; ++i)
	clear_tv(&ppconst->pp_tv[i]);
    ppconst->pp_مستعمل = 0;
}

/*
 * Compile getting a member from a tuple.  Stack has the indexable value and
 * the index or the two indexes of a slice.
 */
    ساکن صحیح
compile_tuple_member(
    type2_T	*typep,
    صحیح		is_قاش,
    cctx_T	*cctx)
{
    اگر (is_قاش)
    {
	اگر (generate_instr_drop(cctx, ISN_TUPLESLICE, 2) == FAIL)
	    واپس FAIL;
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
    ورنہ
    {
	اگر (typep->type_curr->tt_type == VAR_TUPLE)
	{
	    اگر (typep->type_curr->tt_argcount == 1)
	    {
		اگر (typep->type_curr->tt_flags & TTFLAG_VARARGS)
		    typep->type_curr
				= typep->type_curr->tt_دلائل[0]->tt_member;
		ورنہ
		    typep->type_curr = typep->type_curr->tt_دلائل[0];
	    }
	    ورنہ
		typep->type_curr = &t_any;
	    اگر (typep->type_decl->tt_type == VAR_TUPLE)
	    {
		اگر (typep->type_decl->tt_argcount == 1)
		{
		    اگر (typep->type_decl->tt_flags & TTFLAG_VARARGS)
			typep->type_decl
				= typep->type_decl->tt_دلائل[0]->tt_member;
		    ورنہ
			typep->type_decl = typep->type_decl->tt_دلائل[0];
		}
		ورنہ
		    typep->type_curr = &t_any;
	    }
	    ورنہ
		typep->type_decl = typep->type_curr;
	}
	اگر (generate_instr_drop(cctx, ISN_TUPLEINDEX, 1) == FAIL)
	    واپس FAIL;
    }

    واپس OK;
}

/*
 * Compile getting a member from a list/tuple/dict/string/blob.  Stack has the
 * indexable value and the index or the two indexes of a slice.
 * "keeping_dict" is used for dict[func](arg) to pass dict to func.
 */
    صحیح
compile_member(صحیح is_قاش, صحیح *keeping_dict, cctx_T *cctx)
{
    type2_T	*typep;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
    vartype_T	vartype;
    type_T	*idxtype;

    // We can index a list, tuple, dict and blob.  If we don't know the type
    // we can use the index value type.  If we still don't know use an "ANY"
    // instruction.
    // TODO: what about the decl type?
    typep = (((type2_T *)ڈھیر->ga_ڈیٹا) + ڈھیر->ga_len - (is_قاش ? 3 : 2));
    vartype = typep->type_curr->tt_type;
    idxtype = (((type2_T *)ڈھیر->ga_ڈیٹا) + ڈھیر->ga_len - 1)->type_curr;
    // If the index is a string, the variable must be a Dict.
    اگر ((typep->type_curr->tt_type == VAR_ANY
		|| typep->type_curr->tt_type == VAR_UNKNOWN)
						       && idxtype == &t_string)
	vartype = VAR_DICT;
    اگر (vartype == VAR_STRING || vartype == VAR_LIST || vartype == VAR_BLOB
						|| vartype == VAR_TUPLE)
    {
	اگر (need_type(idxtype, &t_نمبر, 0,
					    -1, 0, cctx, FALSE, FALSE) == FAIL)
	    واپس FAIL;
	اگر (is_قاش)
	{
	    idxtype = get_type_on_ڈھیر(cctx, 1);
	    اگر (need_type(idxtype, &t_نمبر, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		واپس FAIL;
	}
    }

    اگر (vartype == VAR_DICT)
    {
	اگر (is_قاش)
	{
	    emsg(_(e_cannot_قاش_dictionary));
	    واپس FAIL;
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
	    ورنہ
		typep->type_decl = typep->type_curr;
	}
	ورنہ
	{
	    اگر (need_type(typep->type_curr, &t_dict_any, 0,
					    -2, 0, cctx, FALSE, FALSE) == FAIL)
		واپس FAIL;
	    typep->type_curr = &t_any;
	    typep->type_decl = &t_any;
	}
	اگر (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL
		|| generate_instr_drop(cctx, ISN_MEMBER, 1) == FAIL)
	    واپس FAIL;
	اگر (keeping_dict != تہی)
	    *keeping_dict = TRUE;
    }
    ورنہ اگر (vartype == VAR_STRING)
    {
	typep->type_curr = &t_string;
	typep->type_decl = &t_string;
	اگر ((is_قاش
		? generate_instr_drop(cctx, ISN_STRSLICE, 2)
		: generate_instr_drop(cctx, ISN_STRINDEX, 1)) == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (vartype == VAR_BLOB)
    {
	اگر (is_قاش)
	{
	    typep->type_curr = &t_blob;
	    typep->type_decl = &t_blob;
	    اگر (generate_instr_drop(cctx, ISN_BLOBSLICE, 2) == FAIL)
		واپس FAIL;
	}
	ورنہ
	{
	    typep->type_curr = &t_نمبر;
	    typep->type_decl = &t_نمبر;
	    اگر (generate_instr_drop(cctx, ISN_BLOBINDEX, 1) == FAIL)
		واپس FAIL;
	}
    }
    ورنہ اگر (vartype == VAR_TUPLE)
    {
	اگر (compile_tuple_member(typep, is_قاش, cctx) == FAIL)
	    واپس FAIL;
    }
    ورنہ اگر (vartype == VAR_LIST || typep->type_curr->tt_type == VAR_ANY
				 || typep->type_curr->tt_type == VAR_UNKNOWN)
    {
	اگر (is_قاش)
	{
	    اگر (generate_instr_drop(cctx,
		     vartype == VAR_LIST ?  ISN_LISTSLICE : ISN_ANYSLICE,
							    2) == FAIL)
		واپس FAIL;
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
	ورنہ
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
		ورنہ
			typep->type_decl = typep->type_curr;
	    }
	    اگر (generate_instr_drop(cctx,
			vartype == VAR_LIST ?  ISN_LISTINDEX : ISN_ANYINDEX, 1)
								       == FAIL)
		واپس FAIL;
	}
    }
    ورنہ
    {
	انتخاب (vartype)
	{
	    حالت VAR_FUNC:
	    حالت VAR_PARTIAL:
		emsg(_(e_cannot_اشاریہ_a_funcref));
		توڑو;
	    حالت VAR_BOOL:
	    حالت VAR_SPECIAL:
	    حالت VAR_کام:
	    حالت VAR_CHANNEL:
	    حالت VAR_INSTR:
	    حالت VAR_CLASS:
	    حالت VAR_OBJECT:
	    حالت VAR_TYPEALIAS:
	    حالت VAR_UNKNOWN:
	    حالت VAR_ANY:
	    حالت VAR_VOID:
		emsg(_(e_cannot_اشاریہ_special_variable));
		توڑو;
	    طےشدہ:
		emsg(_(e_string_list_dict_or_blob_required));
	}
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Returns TRUE if the current function is inside the class "cl" or one of the
 * parent classes.
 */
    ساکن صحیح
inside_class_hierarchy(cctx_T *cctx_arg, class_T *cl)
{
    برائے (cctx_T *cctx = cctx_arg; cctx != تہی; cctx = cctx->ctx_outer)
    {
	اگر (cctx->ctx_ufunc != تہی && cctx->ctx_ufunc->uf_class != تہی)
	{
	    class_T	*clp = cctx->ctx_ufunc->uf_class;
	    جب (clp != تہی)
	    {
		اگر (clp == cl)
		    واپس TRUE;
		clp = clp->class_extends;
	    }
	}
    }

    واپس FALSE;
}

/*
 * Compile ".member" coming after an object or class.
 */
    صحیح
compile_class_object_اشاریہ(cctx_T *cctx, char_u **arg, type_T *type)
{
    صحیح		m_idx;
    صحیح		ret = FAIL;

    اگر (VIM_ISWHITE((*arg)[1]))
    {
	semsg(_(e_no_white_space_allowed_after_str_str), ".", *arg);
	واپس FAIL;
    }

    class_T *cl = type->tt_class;
    صحیح is_super = ((type->tt_flags & TTFLAG_SUPER) == TTFLAG_SUPER);
    اگر (type == &t_super)
    {
	اگر (cctx->ctx_ufunc == تہی || cctx->ctx_ufunc->uf_class == تہی)
	{
	    emsg(_(e_using_super_not_اندر_class_طریقہ));
	    واپس FAIL;
	}
	is_super = TRUE;
	cl = cctx->ctx_ufunc->uf_class;
	// Remove &t_super from the stack.
	--cctx->ctx_type_ڈھیر.ga_len;
    }
    ورنہ اگر (type->tt_type == VAR_CLASS)
    {
	garray_T *instr = &cctx->ctx_instr;
	اگر (instr->ga_len > 0)
	{
	    isn_T *isn = ((isn_T *)instr->ga_ڈیٹا) + instr->ga_len - 1;
	    اگر (isn->isn_type == ISN_LOADSCRIPT)
	    {
		// The class was recognized as a script item.  We only need
		// to know what class it is, drop the instruction.
		--instr->ga_len;
		vim_free(isn->isn_arg.script.scriptref);
	    }
	}
    }

    اگر (cl == تہی)
    {
	// TODO: this should not give an error but be handled at runtime
	emsg(_(e_incomplete_type));
	واپس FAIL;
    }

    ++*arg;
    char_u *name = *arg;
    char_u *name_انتہا = ڈھونڈو_name_انتہا(name, تہی, تہی, FNE_CHECK_آغاز);
    اگر (name_انتہا == name)
    {
	emsg(_(e_missing_name_after_dot));
	واپس FAIL;
    }
    size_t len = name_انتہا - name;

    gfargs_tab_T    gfatab;

    generic_func_دلائل_جدول_init(&gfatab);

    اگر (*name_انتہا == '<')
    {
	// generic method call
	name_انتہا = parse_generic_func_type_دلائل(name, len, name_انتہا,
							&gfatab, cctx);
	اگر (name_انتہا == تہی)
	    جاؤ مکمل;
    }

    اگر (*name_انتہا == '(')
    {
	صحیح	function_گنتی;
	صحیح	بچہ_گنتی;
	ufunc_T	**functions;

	اگر (type->tt_type == VAR_CLASS)
	{
	    function_گنتی = cl->class_class_function_گنتی;
	    بچہ_گنتی = cl->class_class_function_گنتی_بچہ;
	    functions = cl->class_class_functions;
	}
	ورنہ
	{
	    // type->tt_type == VAR_OBJECT: method call
	    // When compiling Func and doing "super.SomeFunc()", must be in the
	    // class context that defines Func.
	    اگر (is_super)
		cl = cctx->ctx_ufunc->uf_defclass;

	    function_گنتی = cl->class_obj_طریقہ_گنتی;
	    بچہ_گنتی = cl->class_obj_طریقہ_گنتی_بچہ;
	    functions = cl->class_obj_methods;
	}

	ufunc_T *ufunc = تہی;
	صحیح fi;
	برائے (fi = is_super ? بچہ_گنتی : 0; fi < function_گنتی; ++fi)
	{
	    ufunc_T *fp = functions[fi];
	    // Use a separate pointer to avoid that ASAN complains about
	    // uf_name[] only being 4 characters.
	    char_u *ufname = (char_u *)fp->uf_name;
	    اگر (STRNCMP(name, ufname, len) == 0 && ufname[len] == NUL)
	    {
		ufunc = fp;
		توڑو;
	    }
	}

	ocmember_T  *ocm = تہی;
	اگر (ufunc == تہی)
	{
	    // could be a funcref in a member variable
	    ocm = member_lookup(cl, type->tt_type, name, len, &m_idx);
	    اگر (ocm == تہی || ocm->ocm_type->tt_type != VAR_FUNC)
	    {
		طریقہ_not_ملا_پیغام(cl, type->tt_type, name, len);
		جاؤ مکمل;
	    }
	    اگر (type->tt_type == VAR_CLASS)
	    {
		// Remove the class type from the stack
		--cctx->ctx_type_ڈھیر.ga_len;
		اگر (generate_CLASSMEMBER(cctx, TRUE, cl, m_idx) == FAIL)
		    جاؤ مکمل;
	    }
	    ورنہ
	    {
		صحیح حالت_2;

		اگر (IS_INTERFACE(cl))
		    حالت_2 = generate_GET_ITF_MEMBER(cctx, cl, m_idx,
							ocm->ocm_type);
		ورنہ
		    حالت_2 = generate_GET_OBJ_MEMBER(cctx, m_idx,
							ocm->ocm_type);
		اگر (حالت_2 == FAIL)
		    جاؤ مکمل;
	    }
	}

	اگر (is_super && ufunc != تہی && IS_ABSTRACT_طریقہ(ufunc))
	{
	    // Trying to invoke an abstract method in a super class is not
	    // allowed.
	    semsg(_(e_abstract_طریقہ_str_براہ_راست), ufunc->uf_name,
		    ufunc->uf_defclass->class_name.string);
	    جاؤ مکمل;
	}

	// A private object method can be used only inside the class where it
	// is defined or in one of the child classes.
	// A private class method can be used only in the class where it is
	// defined.
	اگر (ocm == تہی && *ufunc->uf_name == '_' &&
		((type->tt_type == VAR_OBJECT
		  && !inside_class_hierarchy(cctx, cl))
		 || (type->tt_type == VAR_CLASS
		     && cctx->ctx_ufunc->uf_defclass != cl)))
	{
	    semsg(_(e_cannot_access_protected_طریقہ_str), name);
	    جاؤ مکمل;
	}

	// process generic function call
	اگر (ufunc != تہی)
	{
	    ufunc = generic_func_get(ufunc, &gfatab);
	    اگر (ufunc == تہی)
		جاؤ مکمل;
	}

	// Compile the arguments and call the class function or object method.
	// The object method will know that the object is on the stack, just
	// before the arguments.
	*arg = skipwhite(name_انتہا + 1);
	صحیح argcount = 0;
	اگر (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
	    جاؤ مکمل;

	اگر (ocm != تہی)
	{
	    ret = generate_PCALL(cctx, argcount, name, ocm->ocm_type, TRUE);
	    جاؤ مکمل;
	}
	اگر (type->tt_type == VAR_OBJECT
		     && (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED)))
	    ret = generate_CALL(cctx, ufunc, cl, fi, argcount, is_super);
	ورنہ
	    ret = generate_CALL(cctx, ufunc, تہی, 0, argcount, FALSE);
	جاؤ مکمل;
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
		جاؤ مکمل;
	    }

	    *arg = name_انتہا;
	    اگر (cl->class_flags & (CLASS_INTERFACE | CLASS_EXTENDED))
		ret = generate_GET_ITF_MEMBER(cctx, cl, m_idx, m->ocm_type);
	    ورنہ
		ret = generate_GET_OBJ_MEMBER(cctx, m_idx, m->ocm_type);
	    جاؤ مکمل;
	}

	// Could be an object method reference: "obj.Func".
	m_idx = object_طریقہ_idx(cl, name, len);
	اگر (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_obj_methods[m_idx];
	    // Private object methods are not accessible outside the class
	    اگر (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_طریقہ_str), fp->uf_name);
		جاؤ مکمل;
	    }
	    *arg = name_انتہا;
	    // Remove the object type from the stack
	    --cctx->ctx_type_ڈھیر.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, TRUE, m_idx, تہی);
	    جاؤ مکمل;
	}

	member_not_ملا_پیغام(cl, VAR_OBJECT, name, len);
    }
    ورنہ
    {
	// load class member
	صحیح idx;
	ocmember_T *m = class_member_lookup(cl, name, len, &idx);
	اگر (m != تہی)
	{
	    // Note: type->tt_type = VAR_CLASS
	    // A private class variable can be accessed only in the class where
	    // it is defined.
	    اگر (*name == '_' && cctx->ctx_ufunc->uf_class != cl)
	    {
		emsg_var_cl_define(e_cannot_access_protected_variable_str,
							m->ocm_name.string, 0, cl);
		جاؤ مکمل;
	    }

	    *arg = name_انتہا;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ڈھیر.ga_len;
	    ret = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
	    جاؤ مکمل;
	}

	// Could be a class method reference: "class.Func".
	m_idx = class_طریقہ_idx(cl, name, len);
	اگر (m_idx >= 0)
	{
	    ufunc_T *fp = cl->class_class_functions[m_idx];
	    // Private class methods are not accessible outside the class
	    اگر (*name == '_' && !inside_class(cctx, cl))
	    {
		semsg(_(e_cannot_access_protected_طریقہ_str), fp->uf_name);
		جاؤ مکمل;
	    }
	    *arg = name_انتہا;
	    // Remove the class type from the stack
	    --cctx->ctx_type_ڈھیر.ga_len;
	    ret = generate_FUNCREF(cctx, fp, cl, FALSE, m_idx, تہی);
	    جاؤ مکمل;
	}

	member_not_ملا_پیغام(cl, VAR_CLASS, name, len);
    }

مکمل:
    generic_func_دلائل_جدول_clear(&gfatab);
    واپس ret;
}

/*
 * Generate an instruction to load script-local variable "name", without the
 * leading "s:".
 * Also finds imported variables.
 */
    صحیح
compile_بھرو_scriptvar(
	cctx_T *cctx,
	char_u *name,	    // variable NUL terminated
	char_u *آغاز,	    // start of variable
	char_u **انتہا,	    // end of variable, may be NULL
	imported_T *import) // found imported item, can be NULL
{
    scriptitem_T    *si;
    صحیح		    idx;
    imported_T	    *imp;

    اگر (!SCRIPT_ID_VALID(current_sctx.sc_sid))
	واپس FAIL;
    si = SCRIPT_شے(current_sctx.sc_sid);
    idx = get_script_شے_idx(current_sctx.sc_sid, name, 0, cctx, تہی);
    اگر (idx >= 0)
    {
	svar_T		*sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا) + idx;

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
					current_sctx.sc_sid, idx, sv->sv_type);
	واپس OK;
    }

    اگر (انتہا == تہی)
	imp = تہی;
    ورنہ اگر (import == تہی)
	imp = ڈھونڈو_imported(name, 0, FALSE);
    ورنہ
	imp = import;

    اگر (imp != تہی)
    {
	char_u	*p = skipwhite(*انتہا);
	char_u	*exp_name;
	صحیح	cc;
	ufunc_T	*ufunc = تہی;
	type_T	*type;
	صحیح	مکمل = FALSE;
	صحیح	res = OK;

	check_script_symlink(imp->imp_sid);
	import_check_sourced_sid(&imp->imp_sid);

	// Need to lookup the member.
	اگر (*p != '.')
	{
	    semsg(_(e_expected_dot_after_name_str), آغاز);
	    واپس FAIL;
	}
	++p;
	اگر (VIM_ISWHITE(*p))
	{
	    emsg(_(e_no_white_space_allowed_after_dot));
	    واپس FAIL;
	}

	// isolate one name
	exp_name = p;
	جب (eval_isnamec(*p))
	    ++p;
	cc = *p;
	*p = NUL;

	si = SCRIPT_شے(imp->imp_sid);
	اگر (si->sn_import_autoload && si->sn_state == SN_STATE_NOT_LOADED)
	    // "import autoload './dir/script.vim'" or
	    // "import autoload './autoload/script.vim'" - load script first
	    res = generate_ماخذ(cctx, imp->imp_sid);

	اگر (res == OK)
	{
	    اگر (si->sn_autoload_سابقہ != تہی
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		char_u  *auto_name =
				  concat_str(si->sn_autoload_سابقہ, exp_name);

		// autoload script must be loaded later, access by the autoload
		// name.  If a '(' follows it must be a function.  Otherwise we
		// don't know, it can be "script.Func".
		اگر (cc == '(' || paren_follows_after_اظہاریہ)
		    res = generate_PUSHFUNC(cctx, auto_name, &t_func_any, TRUE);
		ورنہ
		    res = generate_AUTOLOAD(cctx, auto_name, &t_any);
		vim_free(auto_name);
		مکمل = TRUE;
	    }
	    ورنہ اگر (si->sn_import_autoload
					&& si->sn_state == SN_STATE_NOT_LOADED)
	    {
		// If a '(' follows it must be a function.  Otherwise we don't
		// know, it can be "script.Func".
		اگر (cc == '(' || paren_follows_after_اظہاریہ)
		{
		    char_u sid_name[MAX_FUNC_NAME_LEN];

		    func_name_with_sid(exp_name, imp->imp_sid, sid_name);
		    res = generate_PUSHFUNC(cctx, sid_name, &t_func_any, TRUE);
		}
		ورنہ
		    res = generate_OLDSCRIPT(cctx, ISN_LOADEXPORT, exp_name,
						      imp->imp_sid, &t_any);
		مکمل = TRUE;
	    }
	    ورنہ
	    {
		idx = ڈھونڈو_exported(imp->imp_sid, exp_name, &ufunc, &type,
							     cctx, تہی, TRUE);
	    }
	}

	*p = cc;
	*انتہا = p;
	اگر (مکمل)
	    واپس res;

	اگر (idx < 0)
	{
	    اگر (ufunc != تہی)
	    {
		gfargs_tab_T	gfatab;

		generic_func_دلائل_جدول_init(&gfatab);

		اگر (IS_GENERIC_FUNC(ufunc))
		{
		    اگر (*p == '<')
		    {
			// generic function call
			p = parse_generic_func_type_دلائل(name, STRLEN(name), p,
								&gfatab, cctx);
			اگر (p != تہی)
			{
			    *انتہا = p;

			    // generic function call
			    ufunc = generic_func_get(ufunc, &gfatab);
			}

			generic_func_دلائل_جدول_clear(&gfatab);

			اگر (p == تہی || ufunc == تہی)
			    واپس FAIL;
		    }
		    ورنہ
		    {
			emsg_funcname(e_generic_func_missing_type_دلائل_str,
									name);
			واپس FAIL;
		    }
		}
		ورنہ اگر (*p == '<')
		{
		    emsg_funcname(e_not_a_generic_function_str, name);
		    واپس FAIL;
		}

		// function call or function reference
		generate_PUSHFUNC(cctx, ufunc->uf_name, تہی, TRUE);
		واپس OK;
	    }
	    واپس FAIL;
	}

	generate_VIM9SCRIPT(cctx, ISN_LOADSCRIPT,
		imp->imp_sid,
		idx,
		type);
	واپس OK;
    }

    // Can only get here if we know "name" is a script variable and not in a
    // Vim9 script (variable is not in sn_var_vals): old style script.
    واپس generate_OLDSCRIPT(cctx, ISN_LOADS, name, current_sctx.sc_sid,
								       &t_any);
}

    ساکن صحیح
generate_funcref(
    cctx_T		*cctx,
    char_u		*name,
    gfargs_tab_T	*gfatab,
    صحیح			has_g_سابقہ)
{
    ufunc_T *ufunc = ڈھونڈو_func(name, FALSE);
    compiletype_T compile_type;

    // Reject a global non-autoload function found without the "g:" prefix.
    اگر (ufunc == تہی || (!has_g_سابقہ && func_requires_g_سابقہ(ufunc)))
	واپس FAIL;

    // process generic function call
    ufunc = generic_func_get(ufunc, gfatab);
    اگر (ufunc == تہی)
	واپس FAIL;

    // Need to compile any default values to get the argument types.
    compile_type = get_compile_type(ufunc);
    اگر (func_needs_compiling(ufunc, compile_type)
	      && compile_def_function(ufunc, TRUE, compile_type, تہی) == FAIL)
	واپس FAIL;
    واپس generate_PUSHFUNC(cctx, ufunc->uf_name, ufunc->uf_func_type, TRUE);
}

/*
 * Returns TRUE if compiling a class method.
 */
    ساکن صحیح
compiling_a_class_طریقہ(cctx_T *cctx)
{
    // For an object method, the FC_OBJECT flag will be set.
    // For a constructor method, the FC_NEW flag will be set.
    // Excluding these methods, the others are class methods.
    // When compiling a closure function inside an object method,
    // cctx->ctx_outer->ctx_func will point to the object method.
    واپس cctx->ctx_ufunc != تہی
	&& (cctx->ctx_ufunc->uf_flags & (FC_OBJECT|FC_NEW)) == 0
	&& (cctx->ctx_outer == تہی
		|| cctx->ctx_outer->ctx_ufunc == تہی
		|| cctx->ctx_outer->ctx_ufunc->uf_class == تہی
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
    ساکن صحیح
compile_nested_generic_func(
    cctx_T		*cctx,
    ufunc_T		*ufunc,
    gfargs_tab_T	*gfatab,
    صحیح			outer_گہرائی)
{
    ufunc_T	*fp = generic_func_get(ufunc, gfatab);
    cctx_T	*outer_cctx = cctx;
    صحیح		isn_idx;

    اگر (fp == تہی)
	واپس FAIL;

    برائے (صحیح گہرائی = 0; گہرائی < outer_گہرائی; ++گہرائی)
    {
	اگر (outer_cctx->ctx_outer == تہی)
	{
	    siemsg("generic function %s outer context missing", ufunc->uf_name);
	    واپس FAIL;
	}
	outer_cctx = outer_cctx->ctx_outer;
    }

    // copy over the block scope IDs before compiling
    اگر (fp->uf_بلاک_ids == تہی && ufunc->uf_بلاک_گہرائی > 0)
    {
	fp->uf_بلاک_ids = ALLOC_MULT(صحیح, ufunc->uf_بلاک_گہرائی);
	اگر (fp->uf_بلاک_ids != تہی)
	{
	    mch_memmove(fp->uf_بلاک_ids, ufunc->uf_بلاک_ids,
					    حجم(صحیح) * ufunc->uf_بلاک_گہرائی);
	    fp->uf_بلاک_گہرائی = ufunc->uf_بلاک_گہرائی;
	}
    }

    compiletype_T compile_type = get_compile_type(fp);
    اگر (func_needs_compiling(fp, compile_type)
	    && compile_def_function(fp, TRUE, compile_type, outer_cctx) == FAIL)
	واپس FAIL;

    اگر (generate_FUNCREF(cctx, fp, تہی, FALSE, 0, &isn_idx) == FAIL)
	واپس FAIL;
    اگر (cctx->ctx_skip == SKIP_YES)
	واپس OK;

    // The function is owned by the generic function, the reference is
    // dropped when the instruction is deleted.  It may be a recursive call
    // in the function itself.
    func_ptr_ref(fp);

    اگر (outer_گہرائی > 0)
    {
	// The closure uses the context of the function where the generic
	// function is defined, not the current function.
	isn_T		*isn = ((isn_T *)cctx->ctx_instr.ga_ڈیٹا) + isn_idx;
	funcref_extra_T *extra = isn->isn_arg.funcref.fr_extra;

	اگر (extra == تہی)
	{
	    extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	    اگر (extra == تہی)
		واپس FAIL;
	    isn->isn_arg.funcref.fr_extra = extra;
	}
	extra->fre_outer_گہرائی = outer_گہرائی;
    }

    واپس OK;
}

/*
 * Compile a variable name into a load instruction.
 * "end" points to just after the name.
 * "is_expr" is TRUE when evaluating an expression, might be a funcref.
 * When "error" is FALSE do not give an error when not found.
 */
    صحیح
compile_بھرو(
	char_u **arg,
	size_t  namelen,
	char_u *انتہا_arg,
	cctx_T	*cctx,
	صحیح	is_اظہاریہ,
	صحیح	error)
{
    type_T	*type;
    char_u	*name = تہی;
    char_u	*انتہا = انتہا_arg;
    صحیح		res = FAIL;
    صحیح		پچھلا_called_emsg = called_emsg;
    gfargs_tab_T gfatab;
    char_u	*lt = تہی;

    generic_func_دلائل_جدول_init(&gfatab);

    اگر (*(*arg + namelen) == '<')
    {
	// generic function call
	اگر (parse_generic_func_type_دلائل(*arg, namelen,
			*arg + namelen, &gfatab, cctx) == تہی)
	{
	    generic_func_دلائل_جدول_clear(&gfatab);
	    واپس FAIL;
	}
	// Temporarily terminate the name, the line may be compiled again.
	lt = *arg + namelen;
	*lt = NUL;
    }

    اگر (*(*arg + 1) == ':')
    {
	اگر (انتہا <= *arg + 2)
	{
	    isntype_T  isn_type;

	    // load dictionary of namespace
	    انتخاب (**arg)
	    {
		حالت 'g': isn_type = ISN_LOADGDICT; توڑو;
		حالت 'w': isn_type = ISN_LOADWDICT; توڑو;
		حالت 't': isn_type = ISN_LOADTDICT; توڑو;
		حالت 'b': isn_type = ISN_LOADBDICT; توڑو;
		طےشدہ:
		    semsg(_(e_namespace_not_supported_str), *arg);
		    جاؤ theend;
	    }
	    اگر (generate_instr_type(cctx, isn_type, &t_dict_any) == تہی)
		جاؤ theend;
	    res = OK;
	}
	ورنہ
	{
	    isntype_T  isn_type = ISN_DROP;

	    // load namespaced variable
	    name = vim_strnsave(*arg + 2, انتہا - (*arg + 2));
	    اگر (name == تہی)
		جاؤ theend;

	    انتخاب (**arg)
	    {
		حالت 'v': res = generate_LOADV(cctx, name);
			  توڑو;
		حالت 's': اگر (current_script_is_vim9())
			  {
			      semsg(_(e_cannot_use_s_colon_اندر_vim9_script_str),
									 *arg);
			      جاؤ theend;
			  }
			  اگر (is_اظہاریہ && ڈھونڈو_func(name, FALSE) != تہی)
			      res = generate_funcref(cctx, name, &gfatab, FALSE);
			  ورنہ
			      res = compile_بھرو_scriptvar(cctx, name,
							    تہی, &انتہا, تہی);
			  توڑو;
		حالت 'g': اگر (vim_strchr(name, AUTOLOAD_CHAR) == تہی)
			  {
			      اگر (is_اظہاریہ && ASCII_ISUPPER(*name)
				       && (ڈھونڈو_func(name, TRUE) != تہی
					   || gfatab.gfat_دلائل.ga_len > 0))
				  res = generate_funcref(cctx, name, &gfatab,
								TRUE);
			      ورنہ
				  isn_type = ISN_LOADG;
			  }
			  ورنہ
			  {
			      isn_type = ISN_LOADAUTO;
			      vim_free(name);
			      name = vim_strnsave(*arg, انتہا - *arg);
			      اگر (name == تہی)
				  جاؤ theend;
			  }
			  توڑو;
		حالت 'w': isn_type = ISN_LOADW; توڑو;
		حالت 't': isn_type = ISN_LOADT; توڑو;
		حالت 'b': isn_type = ISN_LOADB; توڑو;
		طےشدہ:  // cannot happen, just in case
			  semsg(_(e_namespace_not_supported_str), *arg);
			  جاؤ theend;
	    }
	    اگر (isn_type != ISN_DROP)
	    {
		// Global, Buffer-local, Window-local and Tabpage-local
		// variables can be defined later, thus we don't check if it
		// exists, give an error at runtime.
		res = generate_بھرو(cctx, isn_type, 0, name, &t_any);
	    }
	}
    }
    ورنہ
    {
	صحیح	    idx;
	صحیح	    طریقہ_idx;
	صحیح	    gen_بھرو = FALSE;
	صحیح	    gen_بھرو_outer = 0;
	صحیح	    outer_loop_گہرائی = -1;
	صحیح	    outer_loop_idx = -1;

	name = vim_strnsave(*arg, namelen);
	اگر (name == تہی)
	    جاؤ theend;

	اگر (STRCMP(name, "super") == 0 && compiling_a_class_طریقہ(cctx))
	{
	    // super.SomeFunc() in a class function: push &t_super type, this
	    // is recognized in compile_subscript().
	    res = ڈالو_type_ڈھیر(cctx, &t_super);
	    اگر (*انتہا != '.')
		emsg(_(e_super_must_be_followed_by_dot));
	}
	ورنہ اگر (vim_strchr(name, AUTOLOAD_CHAR) != تہی)
	{
	    script_autoload(name, FALSE);
	    res = generate_بھرو(cctx, ISN_LOADAUTO, 0, name, &t_any);
	}
	ورنہ اگر (arg_exists(*arg, namelen, &idx, &type, &gen_بھرو_outer, cctx)
									 == OK)
	{
	    اگر (gen_بھرو_outer == 0)
		gen_بھرو = TRUE;
	}
	ورنہ
	{
	    lvar_T  lvar;
	    class_T *cl = تہی;

	    اگر (lookup_local(*arg, namelen, &lvar, cctx) == OK)
	    {
		type = lvar.lv_type;
		idx = lvar.lv_idx;
		اگر (lvar.lv_ufunc != تہی && IS_GENERIC_FUNC(lvar.lv_ufunc)
			&& gfatab.gfat_دلائل.ga_len > 0)
		{
		    // The function with the types is compiled in the context
		    // of the function where it is defined.
		    res = compile_nested_generic_func(cctx, lvar.lv_ufunc,
					    &gfatab, lvar.lv_from_outer);
		}
		ورنہ اگر (lvar.lv_from_outer != 0)
		{
		    gen_بھرو_outer = lvar.lv_from_outer;
		    outer_loop_گہرائی = lvar.lv_loop_گہرائی;
		    outer_loop_idx = lvar.lv_loop_idx;
		}
		ورنہ
		    gen_بھرو = TRUE;
	    }
	    ورنہ اگر (cctx->ctx_ufunc->uf_defclass != تہی &&
		    (((idx =
		       cctx_class_member_idx(cctx, *arg, namelen, &cl)) >= 0)
		     || ((طریقہ_idx =
			     cctx_class_طریقہ_idx(cctx, *arg, namelen, &cl)) >= 0)))
	    {
		// Referencing a class variable or method without the class
		// name.  A class variable or method can be referenced without
		// the class name only in the class where the function is
		// defined.
		اگر (cctx->ctx_ufunc->uf_defclass == cl)
		{
		    اگر (idx >= 0)
			res = generate_CLASSMEMBER(cctx, TRUE, cl, idx);
		    ورنہ
		    {
			ufunc_T *fp = cl->class_class_functions[طریقہ_idx];
			res = generate_FUNCREF(cctx, fp, cl, FALSE, طریقہ_idx,
									تہی);
		    }
		}
		ورنہ
		{
		    semsg(_(e_class_variable_str_accessible_only_inside_class_str),
			    name, cl->class_name.string);
		    res = FAIL;
		}
	    }
	    ورنہ
	    {
		imported_T *imp = تہی;

		// "var" can be script-local even without using "s:" if it
		// already exists in a Vim9 script or when it's imported.
		اگر (script_var_exists(*arg, namelen, cctx, تہی) == OK
			    || (imp = ڈھونڈو_imported(name, 0, FALSE)) != تہی)
		    res = compile_بھرو_scriptvar(cctx, name, *arg, &انتہا, imp);

		// When evaluating an expression and the name starts with an
		// uppercase letter it can be a user defined function.
		// generate_funcref() will fail if the function can't be found.
		اگر (res == FAIL && is_اظہاریہ && ASCII_ISUPPER(*name))
		    res = generate_funcref(cctx, name, &gfatab, FALSE);
	    }
	}
	اگر (gen_بھرو)
	    res = generate_بھرو(cctx, ISN_بھرو, idx, تہی, type);
	اگر (gen_بھرو_outer > 0)
	{
	    res = generate_LOADOUTER(cctx, idx, gen_بھرو_outer,
				       outer_loop_گہرائی, outer_loop_idx, type);
	    cctx->ctx_outer_مستعمل = TRUE;
	}
    }

    *arg = انتہا;

theend:
    اگر (lt != تہی)
	*lt = '<';
    اگر (res == FAIL && error && called_emsg == پچھلا_called_emsg)
	semsg(_(e_variable_not_ملا_str), name);
    vim_free(name);
    generic_func_دلائل_جدول_clear(&gfatab);
    واپس res;
}

/*
 * Compile a string in a ISN_PUSHS instruction into an ISN_INSTR.
 * "str_offset" is the number of leading bytes to skip from the string.
 * Returns FAIL if compilation fails.
 */
    ساکن صحیح
compile_string(isn_T *isn, cctx_T *cctx, صحیح str_انحراف)
{
    char_u	*s = isn->isn_arg.string + str_انحراف;
    garray_T	save_ga = cctx->ctx_instr;
    صحیح		اظہاریہ_res;
    صحیح		trailing_error;
    صحیح		instr_گنتی;
    isn_T	*instr = تہی;

    // Remove the string type from the stack.
    --cctx->ctx_type_ڈھیر.ga_len;

    // Temporarily reset the list of instructions so that the jump labels are
    // correct.
    cctx->ctx_instr.ga_len = 0;
    cctx->ctx_instr.ga_maxlen = 0;
    cctx->ctx_instr.ga_ڈیٹا = تہی;

    // avoid peeking a next line
    صحیح galen_save = cctx->ctx_ufunc->uf_سطریں.ga_len;
    cctx->ctx_ufunc->uf_سطریں.ga_len = 0;

    اظہاریہ_res = compile_expr0(&s, cctx);

    cctx->ctx_ufunc->uf_سطریں.ga_len = galen_save;

    s = skipwhite(s);
    trailing_error = *s != NUL;

    اگر (اظہاریہ_res == FAIL || trailing_error
				       || GA_GROW_FAILS(&cctx->ctx_instr, 1))
    {
	اگر (trailing_error)
	    semsg(_(e_trailing_characters_str), s);
	clear_instr_ga(&cctx->ctx_instr);
	cctx->ctx_instr = save_ga;
	++cctx->ctx_type_ڈھیر.ga_len;
	واپس FAIL;
    }

    // Move the generated instructions into the ISN_INSTR instruction, then
    // restore the list of instructions.
    instr_گنتی = cctx->ctx_instr.ga_len;
    instr = cctx->ctx_instr.ga_ڈیٹا;
    instr[instr_گنتی].isn_type = ISN_اختتام;

    cctx->ctx_instr = save_ga;
    vim_free(isn->isn_arg.string);
    isn->isn_type = ISN_INSTR;
    isn->isn_arg.instr = instr;
    واپس OK;
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
    صحیح	    must_انتہا = FALSE;
    صحیح	    instr_گنتی;

    برائے (;;)
    {
	اگر (may_get_اگلا_سطر(whitep, &p, cctx) == FAIL)
	    جاؤ failret;
	اگر (*p == ')')
	{
	    *arg = p + 1;
	    واپس OK;
	}
	اگر (must_انتہا)
	{
	    semsg(_(e_missing_comma_before_دلیل_str), p);
	    واپس FAIL;
	}

	instr_گنتی = cctx->ctx_instr.ga_len;
	اگر (compile_expr0(&p, cctx) == FAIL)
	    واپس FAIL;
	++*argcount;

	اگر (special_fn == CA_SEARCHPAIR && *argcount == 5
		&& cctx->ctx_instr.ga_len == instr_گنتی + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_ڈیٹا) + instr_گنتی;

	    // {skip} argument of searchpair() can be compiled if not empty
	    اگر (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != تہی
		    && *isn->isn_arg.string != NUL)
		compile_string(isn, cctx, 0);
	}
	ورنہ اگر (special_fn == CA_SUBSTITUTE && *argcount == 3
		&& cctx->ctx_instr.ga_len == instr_گنتی + 1)
	{
	    isn_T *isn = ((isn_T *)cctx->ctx_instr.ga_ڈیٹا) + instr_گنتی;

	    // {sub} argument of substitute() can be compiled if it starts
	    // with \=
	    اگر (isn->isn_type == ISN_PUSHS
		    && isn->isn_arg.string != تہی
		    && isn->isn_arg.string[0] == '\\'
		    && isn->isn_arg.string[1] == '=')
		compile_string(isn, cctx, 2);
	}

	اگر (*p != ',' && *skipwhite(p) == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    واپس FAIL;
	}
	اگر (*p == ',')
	{
	    ++p;
	    اگر (*p != NUL && !VIM_ISWHITE(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		واپس FAIL;
	    }
	}
	ورنہ
	    must_انتہا = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
failret:
    emsg(_(e_missing_closing_paren));
    واپس FAIL;
}

/*
 * Compile a builtin method call of an object (e.g. string(), len(), empty(),
 * etc.) if the class implements it.
 */
    ساکن صحیح
compile_builtin_طریقہ_call(cctx_T *cctx, class_builtin_T builtin_طریقہ)
{
    type_T	*type = get_decl_type_on_ڈھیر(cctx, 0);
    صحیح		res = FAIL;

    // If the built in function is invoked on an object and the class
    // implements the corresponding built in method, then invoke the object
    // method.
    اگر (type->tt_type == VAR_OBJECT)
    {
	صحیح	طریقہ_idx;
	ufunc_T *uf = class_get_builtin_طریقہ(type->tt_class, builtin_طریقہ,
							&طریقہ_idx);
	اگر (uf != تہی)
	    res = generate_CALL(cctx, uf, type->tt_class, طریقہ_idx, 0, FALSE);
    }

    واپس res;
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
    ساکن صحیح
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
    char_u	*tofree = تہی;
    char_u	*gname = تہی;
    ufunc_T	*ufunc = تہی;
    صحیح		res = FAIL;
    صحیح		is_autoload;
    صحیح		has_g_namespace;
    ca_special_T special_fn;
    imported_T	*import;

    اگر (varlen >= حجم(namebuf))
    {
	semsg(_(e_name_too_long_str), name);
	واپس FAIL;
    }
    vim_strncpy(namebuf, *arg, varlen);

    import = ڈھونڈو_imported(name, varlen, FALSE);
    اگر (import != تہی)
    {
	semsg(_(e_cannot_use_str_itself_it_is_imported), namebuf);
	واپس FAIL;
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
	    (خالی)eval_string(&s, &argvars[0], TRUE, FALSE);
	ورنہ اگر (*s == '\'')
	    (خالی)eval_lit_string(&s, &argvars[0], TRUE, FALSE);
	s = skipwhite(s);
	اگر (*s == ')' && argvars[0].v_type == VAR_STRING
	       && ((is_has && !dynamic_feature(argvars[0].vval.v_string))
		    || !is_has))
	{

	    *arg = s + 1;

	    اگر (cctx->ctx_skip != SKIP_YES)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_مستعمل];

		argvars[1].v_type = VAR_UNKNOWN;
		tv->v_type = VAR_نمبر;
		tv->vval.v_نمبر = 0;
		اگر (is_has)
		    f_has(argvars, tv);
		ورنہ اگر (is_len)
		    f_len(argvars, tv);
		ورنہ
		    f_exists(argvars, tv);
		++ppconst->pp_مستعمل;
	    }
	    clear_tv(&argvars[0]);
	    واپس OK;
	}
	clear_tv(&argvars[0]);
	اگر (!is_has && !is_len)
	{
	    emsg(_(e_دلیل_کا_exists_compiled_must_be_literal_string));
	    واپس FAIL;
	}
    }

    اگر (generate_ppconst(cctx, ppconst) == FAIL)
	واپس FAIL;

    funcerror_T	error;
    name = fname_trans_sid(namebuf, fname_buf, &tofree, &error);

    // We handle the "skip" argument of searchpair() and searchpairpos()
    // differently.
    اگر ((varlen == 6 && STRNCMP(*arg, "search", 6) == 0)
	    || (varlen == 9 && STRNCMP(*arg, "searchpos", 9) == 0)
	    || (varlen == 10 && STRNCMP(*arg, "searchpair", 10) == 0)
	    || (varlen == 13 && STRNCMP(*arg, "searchpairpos", 13) == 0))
	special_fn = CA_SEARCHPAIR;
    ورنہ اگر (varlen == 10 && STRNCMP(*arg, "substitute", 10) == 0)
	special_fn = CA_SUBSTITUTE;
    ورنہ
	special_fn = CA_NOT_SPECIAL;

    gfargs_tab_T    gfatab;

    generic_func_دلائل_جدول_init(&gfatab);

    اگر (*(*arg + varlen) == '<')
    {
	char_u	*آغاز = *arg;

	// generic function
	*arg = parse_generic_func_type_دلائل(*arg, varlen, *arg + varlen,
								&gfatab, cctx);
	اگر (*arg == تہی)
	    جاؤ theend;
	// the name with the type arguments, used for a nested function
	gname = vim_strnsave(آغاز, *arg - آغاز);
	اگر (gname == تہی)
	    جاؤ theend;
	++*arg;			// skip '('
    }
    ورنہ
	*arg += varlen + 1;

    اگر (compile_arguments(arg, cctx, &argcount, special_fn) == FAIL)
	جاؤ theend;

    is_autoload = vim_strchr(name, AUTOLOAD_CHAR) != تہی;
    اگر (ASCII_ISLOWER(*name) && name[1] != ':' && !is_autoload)
    {
	صحیح	    idx;

	// builtin function
	idx = ڈھونڈو_اندرونی_func(name);
	اگر (idx >= 0)
	{
	    اگر (STRCMP(name, "flatten") == 0)
	    {
		emsg(_(e_cannot_use_flatten_اندر_vim9_script));
		جاؤ theend;
	    }

	    اگر (STRCMP(name, "add") == 0 && argcount == 2)
	    {
		type_T	    *type = get_decl_type_on_ڈھیر(cctx, 1);
		اگر (check_type_is_قدر(get_type_on_ڈھیر(cctx, 0)) == FAIL)
		    جاؤ theend;

		// add() can be compiled to instructions if we know the type
		اگر (type->tt_type == VAR_LIST)
		{
		    // inline "add(list, item)" so that the type can be checked
		    res = generate_LISTAPPEND(cctx);
		    idx = -1;
		}
		ورنہ اگر (type->tt_type == VAR_BLOB)
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

	    class_builtin_T	builtin_طریقہ = CLASS_BUILTIN_INVALID;
	    اگر (STRCMP(name, "string") == 0)
		builtin_طریقہ = CLASS_BUILTIN_STRING;
	    ورنہ اگر (STRCMP(name, "empty") == 0)
		builtin_طریقہ = CLASS_BUILTIN_خالی_خانے;
	    ورنہ اگر (STRCMP(name, "len") == 0)
		builtin_طریقہ = CLASS_BUILTIN_LEN;
	    اگر (builtin_طریقہ != CLASS_BUILTIN_INVALID)
	    {
		res = compile_builtin_طریقہ_call(cctx, builtin_طریقہ);
		اگر (res == OK)
		    idx = -1;
	    }

	    اگر (idx >= 0)
		res = generate_BCALL(cctx, idx, argcount, argcount_init == 1);
	}
	ورنہ
	    emsg_funcname(e_unknown_function_str, namebuf);
	جاؤ theend;
    }

    has_g_namespace = STRNCMP(namebuf, "g:", 2) == 0;

    // An argument or local variable can be a function reference, this
    // overrules a function name.
    اگر (lookup_local(namebuf, varlen, تہی, cctx) == FAIL
	    && arg_exists(namebuf, varlen, تہی, تہی, تہی, cctx) != OK)
    {
	class_T		*cl = تہی;
	صحیح		mi = 0;

	// If we can find the function by name generate the right call.
	// Skip global functions here, a local funcref takes precedence.
	ufunc = ڈھونڈو_func(name, FALSE);
	اگر (ufunc != تہی)
	{
	    // process generic function call
	    ufunc = generic_func_get(ufunc, &gfatab);
	    اگر (ufunc == تہی)
		جاؤ theend;

	    اگر (!func_is_global(ufunc))
	    {
		res = generate_CALL(cctx, ufunc, تہی, 0, argcount, FALSE);
		جاؤ theend;
	    }
	    اگر (!has_g_namespace
			  && vim_strchr(ufunc->uf_name, AUTOLOAD_CHAR) == تہی
			  && !IS_GENERIC_FUNC(ufunc))
	    {
		// A function name without g: prefix must be found locally
		// A generic function has the name emptied out.
		emsg_funcname(e_unknown_function_str, namebuf);
		جاؤ theend;
	    }
	}
	ورنہ اگر ((mi = cctx_class_طریقہ_idx(cctx, name, varlen, &cl)) >= 0)
	{
	    // Class method invocation without the class name.
	    // A class method can be referenced without the class name only in
	    // the class where the function is defined.
	    اگر (cctx->ctx_ufunc->uf_defclass == cl)
	    {
		res = generate_CALL(cctx, cl->class_class_functions[mi], تہی,
							0, argcount, FALSE);
	    }
	    ورنہ
	    {
		semsg(_(e_class_طریقہ_str_accessible_only_inside_class_str),
			name, cl->class_name.string);
		res = FAIL;
	    }
	    جاؤ theend;
	}
    }

    // If the name is a variable, load it and use PCALL.
    // Not for g:Func(), we don't know if it is a variable or not.
    // Not for some#Func(), it will be loaded later.
    p = ufunc == تہی && gname != تہی ? gname : namebuf;
    اگر (!has_g_namespace && !is_autoload
	    && compile_بھرو(&p, varlen, p + varlen, cctx, FALSE, FALSE) == OK)
    {
	type_T	    *s_type = get_type_on_ڈھیر(cctx, 0);

	res = generate_PCALL(cctx, argcount, namebuf, s_type, FALSE);
	جاؤ theend;
    }

    // If we can find a global function by name generate the right call.
    اگر (ufunc != تہی)
    {
	res = generate_CALL(cctx, ufunc, تہی, 0, argcount, FALSE);
	جاؤ theend;
    }

    // A global function may be defined only later.  Need to figure out at
    // runtime.  Also handles a FuncRef at runtime.
    اگر (has_g_namespace || is_autoload)
	res = generate_UCALL(cctx, name, argcount);
    ورنہ
	emsg_funcname(e_unknown_function_str, namebuf);

theend:
    generic_func_دلائل_جدول_clear(&gfatab);
    vim_free(tofree);
    vim_free(gname);
    واپس res;
}

// like NAMESPACE_CHAR but with 'a' and 'l'.
#تعریف_کرو VIM9_NAMESPACE_CHAR	(char_u *)"bgstvw"

/*
 * Find the end of a variable or function name.  Unlike find_name_end() this
 * does not recognize magic braces.
 * When "use_namespace" is TRUE recognize "b:", "s:", etc.
 * Return a pointer to just after the name.  Equal to "arg" if there is no
 * valid name.
 */
    char_u *
مقصد_name_انتہا(char_u *arg, صحیح use_namespace)
{
    char_u	*p;

    // Quick check for valid starting character.
    اگر (!eval_isnamec1(*arg))
	واپس arg;

    برائے (p = arg + 1; *p != NUL && eval_isnamec(*p); MB_PTR_ADV(p))
	// Include a namespace such as "s:var" and "v:var".  But "n:" is not
	// and can be used in slice "[n:]".
	اگر (*p == ':' && (p != arg + 1
			     || !use_namespace
			     || vim_strchr(VIM9_NAMESPACE_CHAR, *arg) == تہی))
	    توڑو;
    واپس p;
}

/*
 * Like to_name_end() but also skip over a list or dict constant.
 * Also accept "<SNR>123_Func".
 * This intentionally does not handle line continuation.
 */
    char_u *
مقصد_name_const_انتہا(char_u *arg)
{
    char_u	*p = arg;
    typval_T	rettv;

    اگر (STRNCMP(p, "<SNR>", 5) == 0)
	p = skipdigits(p + 5);
    p = مقصد_name_انتہا(p, TRUE);
    اگر (p == arg && *arg == '[')
    {

	// Can be "[1, 2, 3]->Func()".
	اگر (eval_list(&p, &rettv, تہی, FALSE) == FAIL)
	    p = arg;
    }
    واپس p;
}

/*
 * parse a list: [expr, expr]
 * "*arg" points to the '['.
 * ppconst->pp_is_const is set if all items are a constant.
 */
    ساکن صحیح
compile_list(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*p = skipwhite(*arg + 1);
    char_u	*whitep = *arg + 1;
    صحیح		گنتی = 0;
    صحیح		is_const;
    صحیح		is_all_const = TRUE;	// reset when non-const encountered
    صحیح		must_انتہا = FALSE;

    برائے (;;)
    {
	اگر (may_get_اگلا_سطر(whitep, &p, cctx) == FAIL)
	{
	    semsg(_(e_missing_انتہا_کا_list_rsb_str), *arg);
	    واپس FAIL;
	}
	اگر (*p == ',')
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
	    واپس FAIL;
	}
	اگر (*p == ']')
	{
	    ++p;
	    توڑو;
	}
	اگر (must_انتہا)
	{
	    semsg(_(e_missing_comma_اندر_list_str), p);
	    واپس FAIL;
	}
	اگر (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
	    واپس FAIL;
	اگر (!is_const)
	    is_all_const = FALSE;
	++گنتی;
	اگر (*p == ',')
	{
	    ++p;
	    اگر (*p != ']' && !IS_WHITE_OR_NUL(*p))
	    {
		semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		واپس FAIL;
	    }
	}
	ورنہ
	    must_انتہا = TRUE;
	whitep = p;
	p = skipwhite(p);
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    واپس generate_NEWLIST(cctx, گنتی, FALSE);
}

/*
 * parse a tuple: (expr, expr)
 * "*arg" points to the ','.
 * ppconst->pp_is_const is set if all the items are constants.
 */
    ساکن صحیح
compile_tuple(
    char_u **arg,
    cctx_T *cctx,
    ppconst_T *ppconst,
    صحیح پہلا_شے_const)
{
    char_u	*p = *arg + 1;
    char_u	*whitep = *arg + 1;
    صحیح		گنتی = 0;
    صحیح		is_const;
    صحیح		is_all_const = TRUE;	// reset when non-const encountered
    صحیح		must_انتہا = FALSE;

    اگر (**arg != ')')
    {
	اگر (*p != ')' && !IS_WHITE_OR_NUL(*p))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
	    واپس FAIL;
	}
	گنتی = 1;	// the first tuple item is already processed
	is_all_const = پہلا_شے_const;
	برائے (;;)
	{
	    اگر (may_get_اگلا_سطر(whitep, &p, cctx) == FAIL)
	    {
		semsg(_(e_missing_انتہا_کا_tuple_rsp_str), *arg);
		واپس FAIL;
	    }
	    اگر (*p == ',')
	    {
		semsg(_(e_no_white_space_allowed_before_str_str), ",", p);
		واپس FAIL;
	    }
	    اگر (*p == ')')
	    {
		++p;
		توڑو;
	    }
	    اگر (must_انتہا)
	    {
		semsg(_(e_missing_comma_اندر_tuple_str), p);
		واپس FAIL;
	    }
	    اگر (compile_expr0_ext(&p, cctx, &is_const) == FAIL)
		واپس FAIL;
	    اگر (!is_const)
		is_all_const = FALSE;
	    ++گنتی;
	    اگر (*p == ',')
	    {
		++p;
		اگر (*p != ')' && !IS_WHITE_OR_NUL(*p))
		{
		    semsg(_(e_white_space_required_after_str_str), ",", p - 1);
		    واپس FAIL;
		}
	    }
	    ورنہ
		must_انتہا = TRUE;
	    whitep = p;
	    p = skipwhite(p);
	}
    }
    *arg = p;

    ppconst->pp_is_const = is_all_const;
    واپس generate_NEWTUPLE(cctx, گنتی, FALSE);
}

/*
 * Restore "*arg" from a temporary cmdline copy.
 */
    ساکن خالی
restore_cmdline_arg(evalarg_T *evalarg, char_u **arg, cctx_T *cctx)
{
    garray_T    *gap;
    char_u	*سطر;
    size_t	off;

    اگر (!evalarg->eval_using_cmdline || cctx == تہی)
	واپس;

    gap = &evalarg->eval_tofree_ga;
    اگر (gap->ga_len == 0)
	واپس;

    off = *arg - ((char_u **)gap->ga_ڈیٹا)[gap->ga_len - 1];
    سطر = ((char_u **)cctx->ctx_ufunc->uf_سطریں.ga_ڈیٹا)[cctx->ctx_lnum];
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
	clear_evalarg(&evalarg, تہی);
	واپس r;
    }

    اگر (ppconst != تہی && generate_ppconst(cctx, ppconst) == FAIL)
    {
	clear_tv(&rettv);
	clear_evalarg(&evalarg, تہی);
	واپس FAIL;
    }

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv.vval.v_partial->pt_func;
    ++ufunc->uf_refcount;
    clear_tv(&rettv);

    اگر (cctx->ctx_ufunc != تہی)
    {
	// This lambda might be defined in a class method.  Inherit the class
	// from the current function.
	ufunc->uf_defclass = cctx->ctx_ufunc->uf_defclass;

	// Copy over the block scope IDs, so that a script variable declared in
	// an enclosing block can be found.
	صحیح بلاک_گہرائی = cctx->ctx_ufunc->uf_بلاک_گہرائی;

	اگر (بلاک_گہرائی > 0)
	{
	    ufunc->uf_بلاک_ids = ALLOC_MULT(صحیح, بلاک_گہرائی);
	    اگر (ufunc->uf_بلاک_ids != تہی)
	    {
		mch_memmove(ufunc->uf_بلاک_ids, cctx->ctx_ufunc->uf_بلاک_ids,
						    حجم(صحیح) * بلاک_گہرائی);
		ufunc->uf_بلاک_گہرائی = بلاک_گہرائی;
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
#اگر_تعریف_شدہ FEAT_PROFILE
	    || cctx->ctx_compile_type == CT_PROFILE
#اگر_ختم
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

    clear_evalarg(&evalarg, تہی);

    اگر (ufunc->uf_def_حالت == UF_COMPILED)
    {
	// The return type will now be known.
	سیٹ_function_type(ufunc);

	// The function reference count will be 1.  When the ISN_FUNCREF
	// instruction is deleted the reference count is decremented and the
	// function is freed.
	واپس generate_FUNCREF(cctx, ufunc, تہی, FALSE, 0, تہی);
    }

    func_ptr_unref(ufunc);
    واپس FAIL;
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
    صحیح		save_sc_نسخہ = current_sctx.sc_نسخہ;

    // Get the funcref in "rettv".
    current_sctx.sc_نسخہ = SCRIPT_نسخہ_VIM9;
    r = get_lambda_tv(arg, rettv, types_optional, evalarg, تہی);
    current_sctx.sc_نسخہ = save_sc_نسخہ;
    اگر (r != OK)
	واپس r;  // currently unreachable

    // "rettv" will now be a partial referencing the function.
    ufunc = rettv->vval.v_partial->pt_func;

    // Compile it here to get the return type.  The return type is optional,
    // when it's missing use t_unknown.  This is recognized in
    // compile_return().
    اگر (ufunc->uf_ret_type == تہی || ufunc->uf_ret_type->tt_type == VAR_VOID)
	ufunc->uf_ret_type = &t_unknown;
    compile_def_function(ufunc, FALSE, CT_NONE, تہی);

    اگر (ufunc->uf_def_حالت == UF_COMPILED)
    {
	// The return type will now be known.
	سیٹ_function_type(ufunc);
	واپس OK;
    }
    clear_tv(rettv);
    واپس FAIL;
}

/*
 * parse a dict: {key: val, [key]: val}
 * "*arg" points to the '{'.
 * ppconst->pp_is_const is set if all item values are a constant.
 */
    ساکن صحیح
compile_dict(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    garray_T	*instr = &cctx->ctx_instr;
    صحیح		گنتی = 0;
    dict_T	*d = dict_alloc();
    dictitem_T	*شے;
    char_u	*whitep = *arg + 1;
    char_u	*p;
    صحیح		is_const;
    صحیح		is_all_const = TRUE;	// reset when non-const encountered

    اگر (d == تہی)
	واپس FAIL;
    اگر (generate_ppconst(cctx, ppconst) == FAIL)
    {
	dict_unref(d);
	واپس FAIL;
    }
    برائے (;;)
    {
	char_u	    *کلید = تہی;

	اگر (may_get_اگلا_سطر(whitep, arg, cctx) == FAIL)
	{
	    *arg = تہی;
	    جاؤ failret;
	}

	اگر (**arg == '}')
	    توڑو;

	اگر (**arg == '[')
	{
	    isn_T	*isn;

	    // {[expr]: value} uses an evaluated key.
	    *arg = skipwhite(*arg + 1);
	    اگر (compile_expr0(arg, cctx) == FAIL)
		جاؤ failret;
	    isn = ((isn_T *)instr->ga_ڈیٹا) + instr->ga_len - 1;
	    اگر (isn->isn_type == ISN_PUSHNR)
	    {
		حرف buf[NUMBUFLEN];

		// Convert to string at compile time.
		vim_snprintf(buf, NUMBUFLEN, "%lld", isn->isn_arg.نمبر);
		isn->isn_type = ISN_PUSHS;
		isn->isn_arg.string = vim_strsave((char_u *)buf);
	    }
	    اگر (isn->isn_type == ISN_PUSHS)
		کلید = isn->isn_arg.string;
	    ورنہ اگر (may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		جاؤ failret;
	    *arg = skipwhite(*arg);
	    اگر (**arg != ']')
	    {
		emsg(_(e_missing_matching_bracket_after_dict_کلید));
		جاؤ failret;
	    }
	    ++*arg;
	}
	ورنہ
	{
	    // {"name": value},
	    // {'name': value},
	    // {name: value} use "name" as a literal key
	    کلید = get_literal_کلید(arg);
	    اگر (کلید == تہی)
		جاؤ failret;
	    اگر (generate_PUSHS(cctx, &کلید) == FAIL)
		جاؤ failret;
	}

	// Check for duplicate keys, if using string keys.
	اگر (کلید != تہی)
	{
	    شے = dict_ڈھونڈو(d, کلید, -1);
	    اگر (شے != تہی)
	    {
		semsg(_(e_duplicate_کلید_اندر_dictionary_str), کلید);
		جاؤ failret;
	    }
	    شے = dictitem_alloc(کلید);
	    اگر (شے != تہی)
	    {
		شے->di_tv.v_type = VAR_UNKNOWN;
		شے->di_tv.v_تالا = 0;
		اگر (dict_جوڑو(d, شے) == FAIL)
		    dictitem_free(شے);
	    }
	}

	اگر (**arg != ':')
	{
	    اگر (*skipwhite(*arg) == ':')
		semsg(_(e_no_white_space_allowed_before_str_str), ":", *arg);
	    ورنہ
		semsg(_(e_missing_colon_اندر_dictionary_str), *arg);
	    جاؤ failret;
	}
	whitep = *arg + 1;
	اگر (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ":", *arg);
	    جاؤ failret;
	}

	اگر (may_get_اگلا_سطر(whitep, arg, cctx) == FAIL)
	{
	    *arg = تہی;
	    جاؤ failret;
	}

	اگر (compile_expr0_ext(arg, cctx, &is_const) == FAIL)
	    جاؤ failret;
	اگر (!is_const)
	    is_all_const = FALSE;
	++گنتی;

	whitep = *arg;
	اگر (may_get_اگلا_سطر(whitep, arg, cctx) == FAIL)
	{
	    *arg = تہی;
	    جاؤ failret;
	}
	اگر (**arg == '}')
	    توڑو;
	اگر (**arg != ',')
	{
	    semsg(_(e_missing_comma_اندر_dictionary_str), *arg);
	    جاؤ failret;
	}
	اگر (IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_no_white_space_allowed_before_str_str), ",", whitep);
	    جاؤ failret;
	}
	whitep = *arg + 1;
	اگر (!IS_WHITE_OR_NUL(*whitep))
	{
	    semsg(_(e_white_space_required_after_str_str), ",", *arg);
	    جاؤ failret;
	}
	*arg = skipwhite(whitep);
    }

    *arg = *arg + 1;

    // Allow for following comment, after at least one space.
    p = skipwhite(*arg);
    اگر (VIM_ISWHITE(**arg) && vim9_comment_آغاز(p))
	*arg += STRLEN(*arg);

    dict_unref(d);
    ppconst->pp_is_const = is_all_const;
    واپس generate_NEWDICT(cctx, گنتی, FALSE);

failret:
    اگر (*arg == تہی)
    {
	semsg(_(e_missing_dict_انتہا_str), _("[end of lines]"));
	*arg = (char_u *)"";
    }
    dict_unref(d);
    واپس FAIL;
}

/*
 * Compile "&option".
 */
    ساکن صحیح
compile_get_option(char_u **arg, cctx_T *cctx)
{
    typval_T	rettv;
    char_u	*آغاز = *arg;
    صحیح		ret;

    // parse the option and get the current value to get the type.
    rettv.v_type = VAR_UNKNOWN;
    ret = eval_option(arg, &rettv, TRUE);
    اگر (ret == OK)
    {
	// include the '&' in the name, eval_option() expects it.
	char_u	*name = vim_strnsave(آغاز, *arg - آغاز);
	type_T	*type = rettv.v_type == VAR_BOOL ? &t_bool
			  : rettv.v_type == VAR_نمبر ? &t_نمبر : &t_string;

	ret = generate_بھرو(cctx, ISN_LOADOPT, 0, name, type);
	vim_free(name);
    }
    clear_tv(&rettv);

    واپس ret;
}

/*
 * Compile "$VAR".
 */
    ساکن صحیح
compile_get_env(char_u **arg, cctx_T *cctx)
{
    char_u	*آغاز = *arg;
    صحیح		len;
    صحیح		ret;
    char_u	*name;

    ++*arg;
    len = get_env_len(arg);
    اگر (len == 0)
    {
	semsg(_(e_syntax_error_at_str), آغاز);
	واپس FAIL;
    }

    // include the '$' in the name, eval_env_var() expects it.
    name = vim_strnsave(آغاز, len + 1);
    ret = generate_بھرو(cctx, ISN_LOADENV, 0, name, &t_string);
    vim_free(name);
    واپس ret;
}

/*
 * Compile $"string" or $'string'.
 */
    ساکن صحیح
compile_interp_string(char_u **arg, cctx_T *cctx)
{
    typval_T	tv;
    صحیح		ret;
    صحیح		quote;
    صحیح		evaluate = cctx->ctx_skip != SKIP_YES;
    صحیح		گنتی = 0;
    char_u	*p;

    // *arg is on the '$' character, move it to the first string character.
    ++*arg;
    quote = **arg;
    ++*arg;

    برائے (;;)
    {
	// Get the string up to the matching quote or to a single '{'.
	// "arg" is advanced to either the quote or the '{'.
	اگر (quote == '"')
	    ret = eval_string(arg, &tv, evaluate, TRUE);
	ورنہ
	    ret = eval_lit_string(arg, &tv, evaluate, TRUE);
	اگر (ret == FAIL)
	    توڑو;
	اگر (evaluate)
	{
	    اگر ((tv.vval.v_string != تہی && *tv.vval.v_string != NUL)
		    || (**arg != '{' && گنتی == 0))
	    {
		// generate non-empty string or empty string if it's the only
		// one
		اگر (generate_PUSHS(cctx, &tv.vval.v_string) == FAIL)
		    واپس FAIL;
		tv.vval.v_string = تہی;  // don't free it now
		++گنتی;
	    }
	    clear_tv(&tv);
	}

	اگر (**arg != '{')
	{
	    // found terminating quote
	    ++*arg;
	    توڑو;
	}

	p = compile_one_اظہاریہ_اندر_str(*arg, cctx);
	اگر (p == تہی)
	{
	    ret = FAIL;
	    توڑو;
	}
	++گنتی;
	*arg = p;
    }

    اگر (ret == FAIL || !evaluate)
	واپس ret;

    // Small optimization, if there's only a single piece skip the ISN_CONCAT.
    اگر (گنتی > 1)
	واپس generate_CONCAT(cctx, گنتی);

    واپس OK;
}

/*
 * Compile "@r".
 */
    ساکن صحیح
compile_get_رجسٹر(char_u **arg, cctx_T *cctx)
{
    صحیح		ret;

    ++*arg;
    اگر (**arg == NUL)
    {
	semsg(_(e_syntax_error_at_str), *arg - 1);
	واپس FAIL;
    }
    اگر (!valid_yank_reg(**arg, FALSE))
    {
	emsg_invreg(**arg);
	واپس FAIL;
    }
    ret = generate_بھرو(cctx, ISN_LOADREG, **arg, تہی, &t_string);
    ++*arg;
    واپس ret;
}

/*
 * Apply leading '!', '-' and '+' to constant "rettv".
 * When "numeric_only" is TRUE do not apply '!'.
 */
    ساکن صحیح
apply_leader(typval_T *rettv, صحیح numeric_only, char_u *آغاز, char_u **انتہا)
{
    char_u *p = *انتہا;

    // this works from end to start
    جب (p > آغاز)
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
	    ورنہ
	    {
		varnumber_T	val;
		صحیح		error = FALSE;

		// tv_get_number_chk() accepts a string, but we don't want that
		// here
		اگر (check_not_string(rettv) == FAIL)
		    واپس FAIL;
		val = tv_get_نمبر_chk(rettv, &error);
		clear_tv(rettv);
		اگر (error)
		    واپس FAIL;
		اگر (*p == '-')
		    val = -val;
		rettv->v_type = VAR_نمبر;
		rettv->vval.v_نمبر = val;
	    }
	}
	ورنہ اگر (numeric_only)
	{
	    ++p;
	    توڑو;
	}
	ورنہ اگر (*p == '!')
	{
	    صحیح v = tv2bool(rettv);

	    // '!' is permissive in the type.
	    clear_tv(rettv);
	    rettv->v_type = VAR_BOOL;
	    rettv->vval.v_نمبر = v ? VVAL_FALSE : VVAL_TRUE;
	}
    }
    *انتہا = p;
    واپس OK;
}

/*
 * Recognize v: variables that are constants and set "rettv".
 */
    ساکن خالی
get_vim_constant(char_u **arg, typval_T *rettv)
{
    اگر (STRNCMP(*arg, "v:true", 6) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_نمبر = VVAL_TRUE;
	*arg += 6;
    }
    ورنہ اگر (STRNCMP(*arg, "v:false", 7) == 0)
    {
	rettv->v_type = VAR_BOOL;
	rettv->vval.v_نمبر = VVAL_FALSE;
	*arg += 7;
    }
    ورنہ اگر (STRNCMP(*arg, "v:null", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_نمبر = VVAL_NULL;
	*arg += 6;
    }
    ورنہ اگر (STRNCMP(*arg, "v:none", 6) == 0)
    {
	rettv->v_type = VAR_SPECIAL;
	rettv->vval.v_نمبر = VVAL_NONE;
	*arg += 6;
    }
}

    exprtype_T
get_compare_type(char_u *p, صحیح *len, صحیح *type_is)
{
    exprtype_T	type = اظہاریہ_UNKNOWN;
    صحیح		i;

    انتخاب (p[0])
    {
	حالت '=':   اگر (p[1] == '=')
			type = اظہاریہ_EQUAL;
		    ورنہ اگر (p[1] == '~')
			type = اظہاریہ_مطابقت;
		    توڑو;
	حالت '!':   اگر (p[1] == '=')
			type = اظہاریہ_NEQUAL;
		    ورنہ اگر (p[1] == '~')
			type = اظہاریہ_NOMATCH;
		    توڑو;
	حالت '>':   اگر (p[1] != '=')
		    {
			type = اظہاریہ_GREATER;
			*len = 1;
		    }
		    ورنہ
			type = اظہاریہ_GEQUAL;
		    توڑو;
	حالت '<':   اگر (p[1] != '=')
		    {
			type = اظہاریہ_SMALLER;
			*len = 1;
		    }
		    ورنہ
			type = اظہاریہ_SEQUAL;
		    توڑو;
	حالت 'i':   اگر (p[1] == 's')
		    {
			// "is" and "isnot"; but not a prefix of a name
			اگر (p[2] == 'n' && p[3] == 'o' && p[4] == 't')
			    *len = 5;
			i = p[*len];
			اگر (!محفوظ_isalnum(i) && i != '_')
			{
			    type = *len == 2 ? اظہاریہ_IS : اظہاریہ_ISNOT;
			    *type_is = TRUE;
			}
		    }
		    توڑو;
    }
    واپس type;
}

/*
 * Skip over an expression, ignoring most errors.
 */
    خالی
skip_اظہاریہ_cctx(char_u **arg, cctx_T *cctx)
{
    evalarg_T	evalarg;

    init_evalarg(&evalarg);
    evalarg.eval_cctx = cctx;
    skip_اظہاریہ(arg, &evalarg);
    restore_cmdline_arg(&evalarg, arg, cctx);
    clear_evalarg(&evalarg, تہی);
}

/*
 * Check that the top of the type stack has a type that can be used as a
 * condition.  Give an error and return FAIL if not.
 */
    صحیح
bool_on_ڈھیر(cctx_T *cctx)
{
    type_T	*type;

    type = get_type_on_ڈھیر(cctx, 0);
    اگر (type == &t_bool)
	واپس OK;

    اگر (type->tt_type == VAR_ANY
	    || type->tt_type == VAR_UNKNOWN
	    || type->tt_type == VAR_نمبر
	    || type == &t_نمبر_bool
	    || type == &t_const_نمبر_bool)
	// Number 0 and 1 are OK to use as a bool.  "any" could also be a bool.
	// This requires a runtime type check.
	واپس generate_COND2BOOL(cctx);

    واپس need_type(type, &t_bool, 0, -1, 0, cctx, FALSE, FALSE);
}

/*
 * Give the "white on both sides" error, taking the operator from "p[len]".
 */
    خالی
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
    ساکن صحیح
compile_leader(cctx_T *cctx, صحیح numeric_only, char_u *آغاز, char_u **انتہا)
{
    char_u	*p = *انتہا;

    // this works from end to start
    جب (p > آغاز)
    {
	--p;
	جب (VIM_ISWHITE(*p))
	    --p;
	اگر (*p == '-' || *p == '+')
	{
	    type_T *type = get_type_on_ڈھیر(cctx, 0);
	    اگر (type->tt_type != VAR_FLOAT && need_type(type, &t_نمبر,
				     0, -1, 0, cctx, FALSE, FALSE) == FAIL)
		واپس FAIL;

	    // only '-' has an effect, for '+' we only check the type
	    اگر (*p == '-' && generate_instr(cctx, ISN_NEGATENR) == تہی)
		واپس FAIL;
	}
	ورنہ اگر (numeric_only)
	{
	    ++p;
	    توڑو;
	}
	ورنہ
	{
	    صحیح  invert = *p == '!';

	    جب (p > آغاز && (p[-1] == '!' || VIM_ISWHITE(p[-1])))
	    {
		اگر (p[-1] == '!')
		    invert = !invert;
		--p;
	    }
	    اگر (check_type_is_قدر(get_type_on_ڈھیر(cctx, 0)) == FAIL)
		واپس FAIL;
	    اگر (generate_2BOOL(cctx, invert, -1) == FAIL)
		واپس FAIL;
	}
    }
    *انتہا = p;
    واپس OK;
}

/*
 * Compile "(expression)": recursive!
 * Return FAIL/OK.
 */
    ساکن صحیح
compile_parenthesis(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحیح	    ret;
    char_u  *p = *arg + 1;

    اگر (may_get_اگلا_سطر_error(p, arg, cctx) == FAIL)
	واپس FAIL;

    اگر (**arg == ')')
	// empty tuple
	واپس compile_tuple(arg, cctx, ppconst, FALSE);

    اگر (ppconst->pp_مستعمل <= PPSIZE - 10)
    {
	ret = compile_expr1(arg, cctx, ppconst);
    }
    ورنہ
    {
	// Not enough space in ppconst, flush constants.
	اگر (generate_ppconst(cctx, ppconst) == FAIL)
	    واپس FAIL;
	ret = compile_expr0(arg, cctx);
    }
    اگر (may_get_اگلا_سطر_error(*arg, arg, cctx) == FAIL)
	واپس FAIL;
    اگر (ret == OK && **arg == ',')
    {
	// tuple
	صحیح is_const = ppconst->pp_مستعمل > 0 || ppconst->pp_is_const;
	اگر (generate_ppconst(cctx, ppconst) == FAIL)
	    واپس FAIL;
	واپس compile_tuple(arg, cctx, ppconst, is_const);
    }

    اگر (**arg == ')')
	++*arg;
    ورنہ اگر (ret == OK)
    {
	emsg(_(e_missing_closing_paren));
	ret = FAIL;
    }
    واپس ret;
}

ساکن صحیح compile_expr9(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile whatever comes after "name" or "name()".
 * Advances "*arg" only when something was recognized.
 */
    ساکن صحیح
compile_subscript(
	char_u **arg,
	cctx_T *cctx,
	char_u *آغاز_leader,
	char_u **انتہا_leader,
	ppconst_T *ppconst)
{
    char_u	*name_آغاز = *انتہا_leader;
    صحیح		keeping_dict = FALSE;

    برائے (;;)
    {
	char_u *p = skipwhite(*arg);
	type_T *type;

	اگر (*p == NUL || (VIM_ISWHITE(**arg) && vim9_comment_آغاز(p)))
	{
	    char_u *اگلا = peek_اگلا_سطر_from_context(cctx);

	    // If a following line starts with "->{", "->(" or "->X" advance to
	    // that line, so that a line break before "->" is allowed.
	    // Also if a following line starts with ".x".
	    اگر (اگلا != تہی &&
		    ((اگلا[0] == '-' && اگلا[1] == '>'
				 && (اگلا[2] == '{'
				       || *skipwhite(اگلا + 2) == '('
				       || ASCII_ISALPHA(*skipwhite(اگلا + 2))))
		    || (اگلا[0] == '.' && eval_isdictc(اگلا[1]))))
	    {
		اگلا = اگلا_سطر_from_context(cctx, TRUE);
		اگر (اگلا == تہی)
		    واپس FAIL;
		*arg = اگلا;
		p = skipwhite(*arg);
	    }
	}

	// Do not skip over white space to find the "(", "execute 'x' (expr)"
	// is not a function call.
	اگر (**arg == '(')
	{
	    صحیح		argcount = 0;

	    اگر (generate_ppconst(cctx, ppconst) == FAIL)
		واپس FAIL;
	    ppconst->pp_is_const = FALSE;

	    // funcref(arg)
	    type = get_type_on_ڈھیر(cctx, 0);

	    *arg = skipwhite(p + 1);
	    اگر (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL) == FAIL)
		واپس FAIL;
	    اگر (generate_PCALL(cctx, argcount, name_آغاز, type, TRUE) == FAIL)
		واپس FAIL;
	    اگر (keeping_dict)
	    {
		keeping_dict = FALSE;
		اگر (generate_instr(cctx, ISN_CLEARDICT) == تہی)
		    واپس FAIL;
	    }
	}
	ورنہ اگر (*p == '-' && p[1] == '>')
	{
	    char_u  *pstart = p;
	    صحیح	    alt;
	    char_u  *paren;

	    // something->method()
	    اگر (generate_ppconst(cctx, ppconst) == FAIL)
		واپس FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ڈھیر(cctx, 0);
	    اگر (type->tt_type == VAR_VOID)
	    {
		emsg(_(e_cannot_use_void_قدر));
		واپس FAIL;
	    }

	    // Apply the '!', '-' and '+' first:
	    //   -1.0->func() works like (-1.0)->func()
	    اگر (compile_leader(cctx, TRUE, آغاز_leader, انتہا_leader) == FAIL)
		واپس FAIL;

	    p += 2;
	    *arg = skipwhite(p);
	    // No line break supported right after "->".

	    // Three alternatives handled here:
	    // 1. "base->name("  only a name, use compile_call()
	    // 2. "base->(expr)(" evaluate "expr", then use PCALL
	    // 3. "base->expr("  Same, find the end of "expr" by "("
	    اگر (**arg == '(')
		alt = 2;
	    ورنہ
	    {
		// alternative 1 or 3
		p = *arg;
		اگر (!eval_isnamec1(*p))
		{
		    semsg(_(e_trailing_characters_str), pstart);
		    واپس FAIL;
		}
		اگر (ASCII_ISALPHA(*p) && p[1] == ':')
		    p += 2;
		برائے ( ; eval_isnamec(*p); ++p)
		    ;
		اگر (*p == '(')
		{
		    // alternative 1
		    alt = 1;
		    اگر (compile_call(arg, p - *arg, cctx, ppconst, 1) == FAIL)
			واپس FAIL;
		}
		ورنہ
		{
		    // Must be alternative 3, find the "(". Only works within
		    // one line.
		    alt = 3;
		    paren = vim_strchr(p, '(');
		    اگر (paren == تہی)
		    {
			semsg(_(e_missing_parenthesis_str), *arg);
			واپس FAIL;
		    }
		}
	    }

	    اگر (alt != 1)
	    {
		صحیح	    argcount = 1;
		garray_T    *ڈھیر = &cctx->ctx_type_ڈھیر;
		صحیح	    type_idx_آغاز = ڈھیر->ga_len;
		صحیح	    اظہاریہ_isn_آغاز = cctx->ctx_instr.ga_len;
		صحیح	    اظہاریہ_isn_انتہا;
		صحیح	    arg_isn_گنتی;

		اگر (alt == 2)
		{
		    // Funcref call:  list->(Refs[2])(arg)
		    // or lambda:	  list->((arg) => expr)(arg)
		    //
		    // Fist compile the function expression.
		    اگر (compile_parenthesis(arg, cctx, ppconst) == FAIL)
			واپس FAIL;
		}
		ورنہ
		{
		    صحیح fail;
		    صحیح save_len = cctx->ctx_ufunc->uf_سطریں.ga_len;
		    صحیح	پچھلا_did_emsg = did_emsg;

		    *paren = NUL;

		    // instead of using LOADG for "import.Func" use PUSHFUNC
		    ++paren_follows_after_اظہاریہ;

		    // do not look in the next line
		    cctx->ctx_ufunc->uf_سطریں.ga_len = 1;

		    fail = compile_expr9(arg, cctx, ppconst) == FAIL
						    || *skipwhite(*arg) != NUL;
		    *paren = '(';
		    --paren_follows_after_اظہاریہ;
		    cctx->ctx_ufunc->uf_سطریں.ga_len = save_len;

		    اگر (fail)
		    {
			اگر (did_emsg == پچھلا_did_emsg)
			    semsg(_(e_invalid_عبارت_str), pstart);
			واپس FAIL;
		    }
		}

		// Compile the arguments.
		اگر (**arg != '(')
		{
		    اگر (*skipwhite(*arg) == '(')
			emsg(_(e_no_white_space_allowed_before_parenthesis));
		    ورنہ
			semsg(_(e_missing_parenthesis_str), *arg);
		    واپس FAIL;
		}

		// Remember the next instruction index, where the instructions
		// for arguments are being written.
		اظہاریہ_isn_انتہا = cctx->ctx_instr.ga_len;

		*arg = skipwhite(*arg + 1);
		اگر (compile_arguments(arg, cctx, &argcount, CA_NOT_SPECIAL)
								       == FAIL)
		    واپس FAIL;

		// Move the instructions for the arguments to before the
		// instructions of the expression and move the type of the
		// expression after the argument types.  This is what ISN_PCALL
		// expects.
		arg_isn_گنتی = cctx->ctx_instr.ga_len - اظہاریہ_isn_انتہا;
		اگر (arg_isn_گنتی > 0)
		{
		    صحیح	    اظہاریہ_isn_گنتی = اظہاریہ_isn_انتہا - اظہاریہ_isn_آغاز;
		    isn_T   *isn = ALLOC_MULT(isn_T, اظہاریہ_isn_گنتی);
		    type_T  *decl_type;
		    type2_T  *typep;

		    اگر (isn == تہی)
			واپس FAIL;
		    mch_memmove(isn, ((isn_T *)cctx->ctx_instr.ga_ڈیٹا)
							      + اظہاریہ_isn_آغاز,
					       حجم(isn_T) * اظہاریہ_isn_گنتی);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_ڈیٹا)
							      + اظہاریہ_isn_آغاز,
			     ((isn_T *)cctx->ctx_instr.ga_ڈیٹا) + اظہاریہ_isn_انتہا,
						حجم(isn_T) * arg_isn_گنتی);
		    mch_memmove(((isn_T *)cctx->ctx_instr.ga_ڈیٹا)
					      + اظہاریہ_isn_آغاز + arg_isn_گنتی,
					  isn, حجم(isn_T) * اظہاریہ_isn_گنتی);
		    vim_free(isn);

		    typep = ((type2_T *)ڈھیر->ga_ڈیٹا) + type_idx_آغاز;
		    type = typep->type_curr;
		    decl_type = typep->type_decl;
		    mch_memmove(((type2_T *)ڈھیر->ga_ڈیٹا) + type_idx_آغاز,
			      ((type2_T *)ڈھیر->ga_ڈیٹا) + type_idx_آغاز + 1,
			      حجم(type2_T)
				       * (ڈھیر->ga_len - type_idx_آغاز - 1));
		    typep = ((type2_T *)ڈھیر->ga_ڈیٹا) + ڈھیر->ga_len - 1;
		    typep->type_curr = type;
		    typep->type_decl = decl_type;
		}

		type = get_type_on_ڈھیر(cctx, 0);
		اگر (generate_PCALL(cctx, argcount, p - 2, type, FALSE) == FAIL)
		    واپس FAIL;
	    }

	    اگر (keeping_dict)
	    {
		keeping_dict = FALSE;
		اگر (generate_instr(cctx, ISN_CLEARDICT) == تہی)
		    واپس FAIL;
	    }
	}
	ورنہ اگر (**arg == '[')
	{
	    صحیح		is_قاش = FALSE;

	    // list index: list[123]
	    // tuple index: tuple[123]
	    // dict member: dict[key]
	    // string index: text[123]
	    // blob index: blob[123]
	    اگر (generate_ppconst(cctx, ppconst) == FAIL)
		واپس FAIL;
	    ppconst->pp_is_const = FALSE;

	    ++p;
	    اگر (may_get_اگلا_سطر_error(p, arg, cctx) == FAIL)
		واپس FAIL;
	    اگر (**arg == ':')
	    {
		// missing first index is equal to zero
		generate_PUSHNR(cctx, 0);
	    }
	    ورنہ
	    {
		اگر (compile_expr0(arg, cctx) == FAIL)
		    واپس FAIL;
		اگر (**arg == ':')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    واپس FAIL;
		}
		اگر (may_get_اگلا_سطر_error(*arg, arg, cctx) == FAIL)
		    واپس FAIL;
		*arg = skipwhite(*arg);
	    }
	    اگر (**arg == ':')
	    {
		is_قاش = TRUE;
		++*arg;
		اگر (!IS_WHITE_OR_NUL(**arg) && **arg != ']')
		{
		    semsg(_(e_white_space_required_before_and_after_str_at_str),
								    ":", *arg);
		    واپس FAIL;
		}
		اگر (may_get_اگلا_سطر_error(*arg, arg, cctx) == FAIL)
		    واپس FAIL;
		اگر (**arg == ']')
		    // missing second index is equal to end of string
		    generate_PUSHNR(cctx, -1);
		ورنہ
		{
		    اگر (compile_expr0(arg, cctx) == FAIL)
			واپس FAIL;
		    اگر (may_get_اگلا_سطر_error(*arg, arg, cctx) == FAIL)
			واپس FAIL;
		    *arg = skipwhite(*arg);
		}
	    }

	    اگر (**arg != ']')
	    {
		emsg(_(e_missing_closing_مربع_brace));
		واپس FAIL;
	    }
	    *arg = *arg + 1;

	    اگر (keeping_dict)
	    {
		keeping_dict = FALSE;
		اگر (generate_instr(cctx, ISN_CLEARDICT) == تہی)
		    واپس FAIL;
	    }
	    اگر (cctx->ctx_skip != SKIP_YES
		    && compile_member(is_قاش, &keeping_dict, cctx) == FAIL)
		واپس FAIL;
	}
	ورنہ اگر (*p == '.' && p[1] != '.')
	{
	    // dictionary member: dict.name
	    اگر (generate_ppconst(cctx, ppconst) == FAIL)
		واپس FAIL;
	    ppconst->pp_is_const = FALSE;

	    type = get_type_on_ڈھیر(cctx, 0);
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
		اگر (compile_class_object_اشاریہ(cctx, arg, type) == FAIL)
		    واپس FAIL;
	    }
	    ورنہ
	    {
		*arg = p + 1;
		اگر (IS_WHITE_OR_NUL(**arg))
		{
		    emsg(_(e_missing_name_after_dot));
		    واپس FAIL;
		}
		p = *arg;
		جب (eval_isdictc(*p))
		    MB_PTR_ADV(p);
		اگر (p == *arg)
		{
		    semsg(_(e_syntax_error_at_str), *arg);
		    واپس FAIL;
		}
		اگر (keeping_dict && generate_instr(cctx, ISN_CLEARDICT) == تہی)
		    واپس FAIL;
		اگر (generate_STRINGMEMBER(cctx, *arg, p - *arg) == FAIL)
		    واپس FAIL;
		keeping_dict = TRUE;
		*arg = p;
	    }
	}
	ورنہ
	    توڑو;
    }

    // Turn "dict.Func" into a partial for "Func" bound to "dict".
    // This needs to be done at runtime to be able to check the type.
    اگر (keeping_dict && cctx->ctx_skip != SKIP_YES
				  && generate_instr(cctx, ISN_USEDICT) == تہی)
	واپس FAIL;

    واپس OK;
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
    ساکن صحیح
compile_expr9(
	char_u **arg,
	cctx_T *cctx,
	ppconst_T *ppconst)
{
    char_u	*آغاز_leader, *انتہا_leader;
    صحیح		ret = OK;
    typval_T	*rettv = &ppconst->pp_tv[ppconst->pp_مستعمل];
    صحیح		مستعمل_before = ppconst->pp_مستعمل;

    ppconst->pp_is_const = FALSE;

    /*
     * Skip '!', '-' and '+' characters.  They are handled later.
     */
    آغاز_leader = *arg;
    اگر (eval_leader(arg, TRUE) == FAIL)
	واپس FAIL;
    انتہا_leader = *arg;

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
	حالت '.':   اگر (eval_نمبر(arg, rettv, TRUE, FALSE) == FAIL)
			واپس FAIL;
		    // Apply "-" and "+" just before the number now, right to
		    // left.  Matters especially when "->" follows.  Stops at
		    // '!'.
		    اگر (apply_leader(rettv, TRUE,
					    آغاز_leader, &انتہا_leader) == FAIL)
		    {
			clear_tv(rettv);
			واپس FAIL;
		    }
		    توڑو;

	/*
	 * String constant: "string".
	 */
	حالت '"':   اگر (eval_string(arg, rettv, TRUE, FALSE) == FAIL)
			واپس FAIL;
		    توڑو;

	/*
	 * Literal string constant: 'str''ing'.
	 */
	حالت '\'':  اگر (eval_lit_string(arg, rettv, TRUE, FALSE) == FAIL)
			واپس FAIL;
		    توڑو;

	/*
	 * Constant Vim variable.
	 */
	حالت 'v':   get_vim_constant(arg, rettv);
		    ret = NOTDONE;
		    توڑو;

	/*
	 * "true" constant
	 */
	حالت 't':   اگر (STRNCMP(*arg, "true", 4) == 0
						   && !eval_isnamec((*arg)[4]))
		    {
			*arg += 4;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_نمبر = VVAL_TRUE;
		    }
		    ورنہ
			ret = NOTDONE;
		    توڑو;

	/*
	 * "false" constant
	 */
	حالت 'f':   اگر (STRNCMP(*arg, "false", 5) == 0
						   && !eval_isnamec((*arg)[5]))
		    {
			*arg += 5;
			rettv->v_type = VAR_BOOL;
			rettv->vval.v_نمبر = VVAL_FALSE;
		    }
		    ورنہ
			ret = NOTDONE;
		    توڑو;

	/*
	 * "null" or "null_*" constant
	 */
	حالت 'n':   اگر (STRNCMP(*arg, "null", 4) == 0)
		    {
			char_u  *p = *arg + 4;
			صحیح	len;

			برائے (len = 0; eval_isnamec(p[len]); ++len)
			    ;
			ret = handle_predefined(*arg, len + 4, rettv);
			اگر (ret == FAIL)
			    ret = NOTDONE;
			ورنہ
			    *arg += len + 4;
		    }
		    ورنہ
			ret = NOTDONE;
		    توڑو;

	/*
	 * List: [expr, expr]
	 */
	حالت '[':   اگر (generate_ppconst(cctx, ppconst) == FAIL)
			واپس FAIL;
		    ret = compile_list(arg, cctx, ppconst);
		    توڑو;

	/*
	 * Dictionary: {'key': val, 'key': val}
	 */
	حالت '{':   اگر (generate_ppconst(cctx, ppconst) == FAIL)
			واپس FAIL;
		    ret = compile_dict(arg, cctx, ppconst);
		    توڑو;

	/*
	 * Option value: &name
	 */
	حالت '&':	اگر (generate_ppconst(cctx, ppconst) == FAIL)
			    واپس FAIL;
			ret = compile_get_option(arg, cctx);
			توڑو;

	/*
	 * Environment variable: $VAR.
	 * Interpolated string: $"string" or $'string'.
	 */
	حالت '$':	اگر (generate_ppconst(cctx, ppconst) == FAIL)
			    واپس FAIL;
			اگر ((*arg)[1] == '"' || (*arg)[1] == '\'')
			    ret = compile_interp_string(arg, cctx);
			ورنہ
			    ret = compile_get_env(arg, cctx);
			توڑو;

	/*
	 * Register contents: @r.
	 */
	حالت '@':	اگر (generate_ppconst(cctx, ppconst) == FAIL)
			    واپس FAIL;
			ret = compile_get_رجسٹر(arg, cctx);
			توڑو;
	/*
	 * nested expression: (expression).
	 * lambda: (arg, arg) => expr
	 * funcref: (arg, arg) => { statement }
	 */
	حالت '(':   // if compile_lambda returns NOTDONE then it must be (expr)
		    ret = compile_lambda(arg, cctx, ppconst);
		    اگر (ret == NOTDONE)
			ret = compile_parenthesis(arg, cctx, ppconst);
		    توڑو;

	طےشدہ:    ret = NOTDONE;
		    توڑو;
    }
    اگر (ret == FAIL)
	واپس FAIL;

    اگر (rettv->v_type != VAR_UNKNOWN && مستعمل_before == ppconst->pp_مستعمل)
    {
	اگر (cctx->ctx_skip == SKIP_YES)
	    clear_tv(rettv);
	ورنہ
	    // A constant expression can possibly be handled compile time,
	    // return the value instead of generating code.
	    ++ppconst->pp_مستعمل;
    }
    ورنہ اگر (ret == NOTDONE)
    {
	char_u	    *p;
	صحیح	    r;

	اگر (!eval_isnamec1(**arg))
	{
	    اگر (!vim9_bad_comment(*arg))
	    {
		اگر (سرے_excmd(*skipwhite(*arg)))
		    semsg(_(e_خالی_خانے_عبارت_str), *arg);
		ورنہ
		    semsg(_(e_name_expected_str), *arg);
	    }
	    واپس FAIL;
	}

	// "name" or "name()"
	p = مقصد_name_انتہا(*arg, TRUE);
	اگر (p - *arg == (size_t)1 && **arg == '_')
	{
	    emsg(_(e_cannot_use_underscore_یہاں));
	    واپس FAIL;
	}

	size_t namelen = p - *arg;
	اگر (*p == '<')
	{
	    اگر (skip_generic_func_type_دلائل(&p) == FAIL)
		واپس FAIL;
	}

	اگر (*p == '(')
	{
	    r = compile_call(arg, namelen, cctx, ppconst, 0);
	}
	ورنہ
	{
	    اگر (cctx->ctx_skip != SKIP_YES
				    && generate_ppconst(cctx, ppconst) == FAIL)
		واپس FAIL;
	    r = compile_بھرو(arg, namelen, p, cctx, TRUE, TRUE);
	}
	اگر (r == FAIL)
	    واپس FAIL;
    }

    // Handle following "[]", ".member", etc.
    // Then deal with prefixed '-', '+' and '!', if not done already.
    اگر (compile_subscript(arg, cctx, آغاز_leader, &انتہا_leader,
							     ppconst) == FAIL)
	واپس FAIL;
    اگر ((ppconst->pp_مستعمل > 0) && (cctx->ctx_skip != SKIP_YES))
    {
	// apply the '!', '-' and '+' before the constant
	rettv = &ppconst->pp_tv[ppconst->pp_مستعمل - 1];
	اگر (apply_leader(rettv, FALSE, آغاز_leader, &انتہا_leader) == FAIL)
	    واپس FAIL;
	واپس OK;
    }
    اگر (compile_leader(cctx, FALSE, آغاز_leader, &انتہا_leader) == FAIL)
	واپس FAIL;
    واپس OK;
}

/*
 * <type>expr9: runtime type check / conversion
 */
    ساکن صحیح
compile_expr8(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    type_T *want_type = تہی;

    // Recognize <type>
    اگر (**arg == '<' && eval_isnamec1((*arg)[1]))
    {
	++*arg;
	want_type = parse_type(arg, cctx->ctx_type_list, cctx->ctx_ufunc, cctx, TRUE);
	اگر (want_type == تہی)
	    واپس FAIL;

	اگر (**arg != '>')
	{
	    اگر (*skipwhite(*arg) == '>')
		semsg(_(e_no_white_space_allowed_before_str_str), ">", *arg);
	    ورنہ
		emsg(_(e_missing_gt));
	    واپس FAIL;
	}
	++*arg;
	اگر (may_get_اگلا_سطر_error(*arg, arg, cctx) == FAIL)
	    واپس FAIL;
    }

    اگر (compile_expr9(arg, cctx, ppconst) == FAIL)
	واپس FAIL;

    اگر (want_type != تہی)
    {
	type_T	    *actual;
	where_T	    where = WHERE_INIT;

	where.wt_قسم = WT_CAST;
	generate_ppconst(cctx, ppconst);
	actual = get_type_on_ڈھیر(cctx, 0);
	اگر (check_type_maybe(want_type, actual, FALSE, where) != OK)
	{
	    اگر (need_type_where(actual, want_type, 0, -1, where, cctx, FALSE,
							FALSE) == FAIL)
		واپس FAIL;
	}
    }

    واپس OK;
}

/*
 *	*	number multiplication
 *	/	number division
 *	%	number modulo
 */
    ساکن صحیح
compile_expr7(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*اگلا;
    صحیح		ppconst_مستعمل = ppconst->pp_مستعمل;

    // get the first expression
    اگر (compile_expr8(arg, cctx, ppconst) == FAIL)
	واپس FAIL;

    /*
     * Repeat computing, until no "*", "/" or "%" is following.
     */
    برائے (;;)
    {
	op = may_peek_اگلا_سطر(cctx, *arg, &اگلا);
	اگر (*op != '*' && *op != '/' && *op != '%')
	    توڑو;
	اگر (اگلا != تہی)
	{
	    *arg = اگلا_سطر_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[1]))
	{
	    error_white_both(op, 1);
	    واپس FAIL;
	}
	اگر (may_get_اگلا_سطر_error(op + 1, arg, cctx) == FAIL)
	    واپس FAIL;

	// get the second expression
	اگر (compile_expr8(arg, cctx, ppconst) == FAIL)
	    واپس FAIL;

	اگر (ppconst->pp_مستعمل == ppconst_مستعمل + 2
		&& ppconst->pp_tv[ppconst_مستعمل].v_type == VAR_نمبر
		&& ppconst->pp_tv[ppconst_مستعمل + 1].v_type == VAR_نمبر)
	{
	    typval_T	    *tv1 = &ppconst->pp_tv[ppconst_مستعمل];
	    typval_T	    *tv2 = &ppconst->pp_tv[ppconst_مستعمل + 1];
	    varnumber_T	    res = 0;
	    صحیح		    failed = FALSE;

	    // both are numbers: compute the result
	    انتخاب (*op)
	    {
		حالت '*': res = tv1->vval.v_نمبر * tv2->vval.v_نمبر;
			  توڑو;
		حالت '/': res = عدد_divide(tv1->vval.v_نمبر,
						  tv2->vval.v_نمبر, &failed);
			  توڑو;
		حالت '%': res = عدد_modulus(tv1->vval.v_نمبر,
						  tv2->vval.v_نمبر, &failed);
			  توڑو;
	    }
	    اگر (failed)
		واپس FAIL;
	    tv1->vval.v_نمبر = res;
	    --ppconst->pp_مستعمل;
	}
	ورنہ
	{
	    generate_ppconst(cctx, ppconst);
	    generate_two_op(cctx, op);
	}
    }

    واپس OK;
}

/*
 *      +	number addition or list/blobl concatenation
 *      -	number subtraction
 *      ..	string concatenation
 */
    ساکن صحیح
compile_expr6(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    char_u	*op;
    char_u	*اگلا;
    صحیح		oplen;
    صحیح		ppconst_مستعمل = ppconst->pp_مستعمل;

    // get the first variable
    اگر (compile_expr7(arg, cctx, ppconst) == FAIL)
	واپس FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    برائے (;;)
    {
	op = may_peek_اگلا_سطر(cctx, *arg, &اگلا);
	اگر (*op != '+' && *op != '-' && !(*op == '.' && *(op + 1) == '.'))
	    توڑو;
	اگر (op[0] == op[1] && *op != '.' && اگلا)
	    // Finding "++" or "--" on the next line is a separate command.
	    // But ".." is concatenation.
	    توڑو;
	oplen = (*op == '.' ? 2 : 1);
	اگر (اگلا != تہی)
	{
	    *arg = اگلا_سطر_from_context(cctx, TRUE);
	    op = skipwhite(*arg);
	}

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(op[oplen]))
	{
	    error_white_both(op, oplen);
	    واپس FAIL;
	}

	اگر (may_get_اگلا_سطر_error(op + oplen, arg, cctx) == FAIL)
	    واپس FAIL;

	// get the second expression
	اگر (compile_expr7(arg, cctx, ppconst) == FAIL)
	    واپس FAIL;

	اگر (ppconst->pp_مستعمل == ppconst_مستعمل + 2
		&& (*op == '.'
		    ? (ppconst->pp_tv[ppconst_مستعمل].v_type == VAR_STRING
		    && ppconst->pp_tv[ppconst_مستعمل + 1].v_type == VAR_STRING)
		    : (ppconst->pp_tv[ppconst_مستعمل].v_type == VAR_نمبر
		    && ppconst->pp_tv[ppconst_مستعمل + 1].v_type == VAR_نمبر)))
	{
	    typval_T *tv1 = &ppconst->pp_tv[ppconst_مستعمل];
	    typval_T *tv2 = &ppconst->pp_tv[ppconst_مستعمل + 1];

	    // concat/subtract/add constant numbers
	    اگر (*op == '+')
		tv1->vval.v_نمبر = tv1->vval.v_نمبر + tv2->vval.v_نمبر;
	    ورنہ اگر (*op == '-')
		tv1->vval.v_نمبر = tv1->vval.v_نمبر - tv2->vval.v_نمبر;
	    ورنہ
	    {
		// concatenate constant strings
		char_u *s1 = tv1->vval.v_string;
		char_u *s2 = tv2->vval.v_string;
		size_t len1 = STRLEN(s1);

		tv1->vval.v_string = alloc(len1 + STRLEN(s2) + 1);
		اگر (tv1->vval.v_string == تہی)
		{
		    vim_free(s1);
		    clear_ppconst(ppconst);
		    واپس FAIL;
		}
		mch_memmove(tv1->vval.v_string, s1, len1);
		STRCPY(tv1->vval.v_string + len1, s2);
		vim_free(s1);
		vim_free(s2);
	    }
	    --ppconst->pp_مستعمل;
	}
	ورنہ
	{
	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;
	    اگر (*op == '.')
	    {
		اگر (may_generate_2STRING(-2, TOSTRING_NONE, cctx) == FAIL
			|| may_generate_2STRING(-1, TOSTRING_NONE, cctx) == FAIL)
		    واپس FAIL;
		اگر (generate_CONCAT(cctx, 2) == FAIL)
		    واپس FAIL;
	    }
	    ورنہ
		generate_two_op(cctx, op);
	}
    }

    واپس OK;
}

/*
 * expr6a >> expr6b
 * expr6a << expr6b
 *
 * Produces instructions:
 *	OPNR			bitwise left or right shift
 */
    ساکن صحیح
compile_expr5(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = اظہاریہ_UNKNOWN;
    char_u	*p;
    char_u	*اگلا;
    صحیح		len = 2;
    صحیح		ppconst_مستعمل = ppconst->pp_مستعمل;
    isn_T	*isn;

    // get the first variable
    اگر (compile_expr6(arg, cctx, ppconst) == FAIL)
	واپس FAIL;

    /*
     * Repeat computing, until no "+", "-" or ".." is following.
     */
    برائے (;;)
    {
	type = اظہاریہ_UNKNOWN;

	p = may_peek_اگلا_سطر(cctx, *arg, &اگلا);
	اگر (p[0] == '<' && p[1] == '<')
	    type = اظہاریہ_LSHIFT;
	ورنہ اگر (p[0] == '>' && p[1] == '>')
	    type = اظہاریہ_RSHIFT;

	اگر (type == اظہاریہ_UNKNOWN)
	    واپس OK;

	// Handle a bitwise left or right shift operator
	اگر (ppconst->pp_مستعمل == ppconst_مستعمل + 1)
	{
	    اگر (ppconst->pp_tv[ppconst->pp_مستعمل - 1].v_type != VAR_نمبر)
	    {
		// left operand should be a number
		emsg(_(e_bitshift_ops_must_be_نمبر));
		واپس FAIL;
	    }
	}
	ورنہ
	{
	    type_T	*t = get_type_on_ڈھیر(cctx, 0);

	    اگر (need_type(t, &t_نمبر, 0, 0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_نمبر));
		واپس FAIL;
	    }
	}

	اگر (اگلا != تہی)
	{
	    *arg = اگلا_سطر_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    واپس FAIL;
	}

	// get the second variable
	اگر (may_get_اگلا_سطر_error(p + len, arg, cctx) == FAIL)
	    واپس FAIL;

	اگر (compile_expr6(arg, cctx, ppconst) == FAIL)
	    واپس FAIL;

	اگر (ppconst->pp_مستعمل == ppconst_مستعمل + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_مستعمل - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_مستعمل - 1];

	    // Both sides are a constant, compute the result now.
	    اگر (tv2->v_type != VAR_نمبر || tv2->vval.v_نمبر < 0)
	    {
		// right operand should be a positive number
		اگر (tv2->v_type != VAR_نمبر)
		    emsg(_(e_bitshift_ops_must_be_نمبر));
		ورنہ
		    emsg(_(e_bitshift_ops_must_be_مثبت));
		واپس FAIL;
	    }

	    اگر (tv2->vval.v_نمبر > MAX_LSHIFT_بٹ)
		tv1->vval.v_نمبر = 0;
	    ورنہ اگر (type == اظہاریہ_LSHIFT)
		tv1->vval.v_نمبر =
			(uvarnumber_T)tv1->vval.v_نمبر << tv2->vval.v_نمبر;
	    ورنہ
		tv1->vval.v_نمبر =
			(uvarnumber_T)tv1->vval.v_نمبر >> tv2->vval.v_نمبر;
	    clear_tv(tv2);
	    --ppconst->pp_مستعمل;
	}
	ورنہ
	{
	    اگر (need_type(get_type_on_ڈھیر(cctx, 0), &t_نمبر, 0,
					     0, 0, cctx, FALSE, FALSE) == FAIL)
	    {
		emsg(_(e_bitshift_ops_must_be_نمبر));
		واپس FAIL;
	    }

	    generate_ppconst(cctx, ppconst);

	    isn = generate_instr_drop(cctx, ISN_OPNR, 1);
	    اگر (isn == تہی)
		واپس FAIL;

	    اگر (isn != تہی)
		isn->isn_arg.op.op_type = type;
	}
    }

    واپس OK;
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
    ساکن صحیح
compile_expr4(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    exprtype_T	type = اظہاریہ_UNKNOWN;
    char_u	*p;
    char_u	*اگلا;
    صحیح		len = 2;
    صحیح		type_is = FALSE;
    صحیح		ppconst_مستعمل = ppconst->pp_مستعمل;

    // get the first variable
    اگر (compile_expr5(arg, cctx, ppconst) == FAIL)
	واپس FAIL;

    p = may_peek_اگلا_سطر(cctx, *arg, &اگلا);

    type = get_compare_type(p, &len, &type_is);

    /*
     * If there is a comparative operator, use it.
     */
    اگر (type != اظہاریہ_UNKNOWN)
    {
	صحیح ic = FALSE;  // Default: do not ignore case

	اگر (اگلا != تہی)
	{
	    *arg = اگلا_سطر_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}
	اگر (type_is && (p[len] == '?' || p[len] == '#'))
	{
	    semsg(_(e_invalid_عبارت_str), *arg);
	    واپس FAIL;
	}
	// extra question mark appended: ignore case
	اگر (p[len] == '?')
	{
	    ic = TRUE;
	    ++len;
	}
	// extra '#' appended: match case (ignored)
	ورنہ اگر (p[len] == '#')
	    ++len;
	// nothing appended: match case

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[len]))
	{
	    error_white_both(p, len);
	    واپس FAIL;
	}

	// get the second variable
	اگر (may_get_اگلا_سطر_error(p + len, arg, cctx) == FAIL)
	    واپس FAIL;

	اگر (compile_expr5(arg, cctx, ppconst) == FAIL)
	    واپس FAIL;

	اگر (ppconst->pp_مستعمل == ppconst_مستعمل + 2)
	{
	    typval_T	*tv1 = &ppconst->pp_tv[ppconst->pp_مستعمل - 2];
	    typval_T	*tv2 = &ppconst->pp_tv[ppconst->pp_مستعمل - 1];
	    صحیح		ret;

	    // Both sides are a constant, compute the result now.
	    // First check for a valid combination of types, this is more
	    // strict than typval_compare().
	    اگر (check_compare_types(type, tv1, tv2) == FAIL)
		ret = FAIL;
	    ورنہ
	    {
		ret = typval_compare(tv1, tv2, type, ic);
		tv1->v_type = VAR_BOOL;
		tv1->vval.v_نمبر = tv1->vval.v_نمبر
						      ? VVAL_TRUE : VVAL_FALSE;
		clear_tv(tv2);
		--ppconst->pp_مستعمل;
	    }
	    واپس ret;
	}

	generate_ppconst(cctx, ppconst);
	واپس generate_COMPARE(cctx, type, ic);
    }

    واپس OK;
}

ساکن صحیح compile_expr3(char_u **arg,  cctx_T *cctx, ppconst_T *ppconst);

/*
 * Compile || or &&.
 */
    ساکن صحیح
compile_and_or(
	char_u **arg,
	cctx_T	*cctx,
	حرف	*op,
	ppconst_T *ppconst,
	صحیح	ppconst_مستعمل غیر_مستعمل)
{
    char_u	*اگلا;
    char_u	*p = may_peek_اگلا_سطر(cctx, *arg, &اگلا);
    صحیح		opchar = *op;

    اگر (p[0] == opchar && p[1] == opchar)
    {
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	انتہا_ga;
	صحیح		save_skip = cctx->ctx_skip;

	/*
	 * Repeat until there is no following "||" or "&&"
	 */
	ga_init2(&انتہا_ga, حجم(صحیح), 10);
	جب (p[0] == opchar && p[1] == opchar)
	{
	    طویل	آغاز_lnum = SOURCING_LNUM;
	    طویل	save_sourcing_lnum;
	    صحیح		آغاز_ctx_lnum = cctx->ctx_lnum;
	    صحیح		save_lnum;
	    صحیح		const_مستعمل;
	    صحیح		حالت_2;
	    jumpwhen_T	jump_when = opchar == '|'
				      ? JUMP_IF_COND_TRUE : JUMP_IF_COND_FALSE;

	    اگر (اگلا != تہی)
	    {
		*arg = اگلا_سطر_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[2]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
									op, p);
		ga_clear(&انتہا_ga);
		واپس FAIL;
	    }

	    save_sourcing_lnum = SOURCING_LNUM;
	    SOURCING_LNUM = آغاز_lnum;
	    save_lnum = cctx->ctx_lnum;
	    cctx->ctx_lnum = آغاز_ctx_lnum;

	    حالت_2 = check_ppconst_bool(ppconst);
	    اگر (حالت_2 != FAIL)
	    {
		// Use the last ppconst if possible.
		اگر (ppconst->pp_مستعمل > 0)
		{
		    typval_T	*tv = &ppconst->pp_tv[ppconst->pp_مستعمل - 1];
		    صحیح		is_true = tv2bool(tv);

		    اگر ((is_true && opchar == '|')
						|| (!is_true && opchar == '&'))
		    {
			// For "false && expr" and "true || expr" the "expr"
			// does not need to be evaluated.
			cctx->ctx_skip = SKIP_YES;
			clear_tv(tv);
			tv->v_type = VAR_BOOL;
			tv->vval.v_نمبر = is_true ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ورنہ
		    {
			// For "true && expr" and "false || expr" only "expr"
			// needs to be evaluated.
			--ppconst->pp_مستعمل;
			jump_when = JUMP_NEVER;
		    }
		}
		ورنہ
		{
		    // Every part must evaluate to a bool.
		    حالت_2 = bool_on_ڈھیر(cctx);
		}
	    }
	    اگر (حالت_2 != FAIL)
		حالت_2 = ga_grow(&انتہا_ga, 1);
	    cctx->ctx_lnum = save_lnum;
	    اگر (حالت_2 == FAIL)
	    {
		ga_clear(&انتہا_ga);
		واپس FAIL;
	    }

	    اگر (jump_when != JUMP_NEVER)
	    {
		اگر (cctx->ctx_skip != SKIP_YES)
		{
		    *(((صحیح *)انتہا_ga.ga_ڈیٹا) + انتہا_ga.ga_len) = instr->ga_len;
		    ++انتہا_ga.ga_len;
		}
		generate_JUMP(cctx, jump_when, 0);
	    }

	    // eval the next expression
	    SOURCING_LNUM = save_sourcing_lnum;
	    اگر (may_get_اگلا_سطر_error(p + 2, arg, cctx) == FAIL)
	    {
		ga_clear(&انتہا_ga);
		واپس FAIL;
	    }

	    const_مستعمل = ppconst->pp_مستعمل;
	    اگر ((opchar == '|' ? compile_expr3(arg, cctx, ppconst)
				  : compile_expr4(arg, cctx, ppconst)) == FAIL)
	    {
		ga_clear(&انتہا_ga);
		واپس FAIL;
	    }

	    // "0 || 1" results in true, "1 && 0" results in false.
	    اگر (ppconst->pp_مستعمل == const_مستعمل + 1)
	    {
		typval_T	*tv = &ppconst->pp_tv[ppconst->pp_مستعمل - 1];

		اگر (tv->v_type == VAR_نمبر
			 && (tv->vval.v_نمبر == 1 || tv->vval.v_نمبر == 0))
		{
		    tv->vval.v_نمبر = tv->vval.v_نمبر == 1
						      ? VVAL_TRUE : VVAL_FALSE;
		    tv->v_type = VAR_BOOL;
		}
	    }

	    p = may_peek_اگلا_سطر(cctx, *arg, &اگلا);
	}

	اگر (check_ppconst_bool(ppconst) == FAIL)
	{
	    ga_clear(&انتہا_ga);
	    واپس FAIL;
	}

	اگر (cctx->ctx_skip != SKIP_YES && ppconst->pp_مستعمل == 0)
	    // Every part must evaluate to a bool.
	    اگر (bool_on_ڈھیر(cctx) == FAIL)
	    {
		ga_clear(&انتہا_ga);
		واپس FAIL;
	    }

	اگر (انتہا_ga.ga_len > 0)
	{
	    // Fill in the end label in all jumps.
	    generate_ppconst(cctx, ppconst);
	    جب (انتہا_ga.ga_len > 0)
	    {
		isn_T	*isn;

		--انتہا_ga.ga_len;
		isn = ((isn_T *)instr->ga_ڈیٹا)
				  + *(((صحیح *)انتہا_ga.ga_ڈیٹا) + انتہا_ga.ga_len);
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}
	ga_clear(&انتہا_ga);

	cctx->ctx_skip = save_skip;
    }

    واپس OK;
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
    ساکن صحیح
compile_expr3(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحیح		ppconst_مستعمل = ppconst->pp_مستعمل;

    // get the first variable
    اگر (compile_expr4(arg, cctx, ppconst) == FAIL)
	واپس FAIL;

    // || and && work almost the same
    واپس compile_and_or(arg, cctx, "&&", ppconst, ppconst_مستعمل);
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
    ساکن صحیح
compile_expr2(char_u **arg, cctx_T *cctx, ppconst_T *ppconst)
{
    صحیح		ppconst_مستعمل = ppconst->pp_مستعمل;

    // eval the first expression
    اگر (compile_expr3(arg, cctx, ppconst) == FAIL)
	واپس FAIL;

    // || and && work almost the same
    واپس compile_and_or(arg, cctx, "||", ppconst, ppconst_مستعمل);
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
    صحیح		ppconst_مستعمل = ppconst->pp_مستعمل;
    char_u	*اگلا;

    // Ignore all kinds of errors when not producing code.
    اگر (cctx->ctx_skip == SKIP_YES)
    {
	صحیح		پچھلا_did_emsg = did_emsg;

	skip_اظہاریہ_cctx(arg, cctx);
	واپس did_emsg == پچھلا_did_emsg ? OK : FAIL;
    }

    // Evaluate the first expression.
    اگر (compile_expr2(arg, cctx, ppconst) == FAIL)
	واپس FAIL;

    p = may_peek_اگلا_سطر(cctx, *arg, &اگلا);
    اگر (*p == '?')
    {
	صحیح		op_falsy = p[1] == '?';
	garray_T	*instr = &cctx->ctx_instr;
	garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
	صحیح		alt_idx = instr->ga_len;
	صحیح		انتہا_idx = 0;
	isn_T		*isn;
	type_T		*type1 = تہی;
	صحیح		has_const_اظہاریہ = FALSE;
	صحیح		const_قدر = FALSE;
	صحیح		save_skip = cctx->ctx_skip;

	اگر (اگلا != تہی)
	{
	    *arg = اگلا_سطر_from_context(cctx, TRUE);
	    p = skipwhite(*arg);
	}

	اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1 + op_falsy]))
	{
	    semsg(_(e_white_space_required_before_and_after_str_at_str),
						     op_falsy ? "??" : "?", p);
	    واپس FAIL;
	}

	اگر (ppconst->pp_مستعمل == ppconst_مستعمل + 1)
	{
	    // the condition is a constant, we know whether the ? or the :
	    // expression is to be evaluated.
	    has_const_اظہاریہ = TRUE;
	    اگر (op_falsy)
		const_قدر = tv2bool(&ppconst->pp_tv[ppconst_مستعمل]);
	    ورنہ
	    {
		صحیح error = FALSE;

		const_قدر = tv_get_bool_chk(&ppconst->pp_tv[ppconst_مستعمل],
								       &error);
		اگر (error)
		    واپس FAIL;
	    }
	    cctx->ctx_skip = save_skip == SKIP_YES ||
		 (op_falsy ? const_قدر : !const_قدر) ? SKIP_YES : SKIP_NOT;

	    اگر (op_falsy && cctx->ctx_skip == SKIP_YES)
		// "left ?? right" and "left" is truthy: produce "left"
		generate_ppconst(cctx, ppconst);
	    ورنہ
	    {
		clear_tv(&ppconst->pp_tv[ppconst_مستعمل]);
		--ppconst->pp_مستعمل;
	    }
	}
	ورنہ
	{
	    generate_ppconst(cctx, ppconst);
	    اگر (op_falsy)
		انتہا_idx = instr->ga_len;
	    generate_JUMP(cctx, op_falsy
				   ? JUMP_AND_KEEP_IF_TRUE : JUMP_IF_FALSE, 0);
	    اگر (op_falsy)
	    {
		type1 = get_type_on_ڈھیر(cctx, -1);
		اگر (check_type_is_قدر(type1) == FAIL)
		    واپس FAIL;
	    }
	}

	// evaluate the second expression; any type is accepted
	اگر (may_get_اگلا_سطر_error(p + 1 + op_falsy, arg, cctx) == FAIL)
	    واپس FAIL;
	اگر (compile_expr1(arg, cctx, ppconst) == FAIL)
	    واپس FAIL;

	اگر (!has_const_اظہاریہ)
	{
	    generate_ppconst(cctx, ppconst);

	    اگر (!op_falsy)
	    {
		// remember the type and drop it
		type1 = get_type_on_ڈھیر(cctx, 0);
		--ڈھیر->ga_len;

		انتہا_idx = instr->ga_len;
		generate_JUMP(cctx, JUMP_ALWAYS, 0);

		// jump here from JUMP_IF_FALSE
		isn = ((isn_T *)instr->ga_ڈیٹا) + alt_idx;
		isn->isn_arg.jump.jump_where = instr->ga_len;
	    }
	}

	اگر (!op_falsy)
	{
	    // Check for the ":".
	    p = may_peek_اگلا_سطر(cctx, *arg, &اگلا);
	    اگر (*p != ':')
	    {
		emsg(_(e_missing_colon_after_questionmark));
		واپس FAIL;
	    }
	    اگر (اگلا != تہی)
	    {
		*arg = اگلا_سطر_from_context(cctx, TRUE);
		p = skipwhite(*arg);
	    }

	    اگر (!IS_WHITE_OR_NUL(**arg) || !IS_WHITE_OR_NUL(p[1]))
	    {
		semsg(_(e_white_space_required_before_and_after_str_at_str),
								       ":", p);
		واپس FAIL;
	    }

	    // evaluate the third expression
	    اگر (has_const_اظہاریہ)
		cctx->ctx_skip = save_skip == SKIP_YES || const_قدر
							 ? SKIP_YES : SKIP_NOT;
	    اگر (may_get_اگلا_سطر_error(p + 1, arg, cctx) == FAIL)
		واپس FAIL;
	    اگر (compile_expr1(arg, cctx, ppconst) == FAIL)
		واپس FAIL;
	}

	اگر (!has_const_اظہاریہ)
	{
	    type_T	**typep;

	    generate_ppconst(cctx, ppconst);
	    ppconst->pp_is_const = FALSE;

	    // If the types differ, the result has a more generic type.
	    typep = &((((type2_T *)ڈھیر->ga_ڈیٹا)
					      + ڈھیر->ga_len - 1)->type_curr);
	    common_type(type1, *typep, typep, cctx->ctx_type_list);

	    // jump here from JUMP_ALWAYS or JUMP_AND_KEEP_IF_TRUE
	    isn = ((isn_T *)instr->ga_ڈیٹا) + انتہا_idx;
	    isn->isn_arg.jump.jump_where = instr->ga_len;
	}

	cctx->ctx_skip = save_skip;
    }
    واپس OK;
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

    CLEAR_میدان(ppconst);
    اگر (compile_expr1(arg, cctx, &ppconst) == FAIL)
    {
	clear_ppconst(&ppconst);
	واپس FAIL;
    }
    اگر (is_const != تہی)
	*is_const = ppconst.pp_مستعمل > 0 || ppconst.pp_is_const;
    اگر (generate_ppconst(cctx, &ppconst) == FAIL)
	واپس FAIL;
    واپس OK;
}

/*
 * Toplevel expression.
 */
    صحیح
compile_expr0(char_u **arg,  cctx_T *cctx)
{
    واپس compile_expr0_ext(arg, cctx, تہی);
}


#اگر_ختم // defined(FEAT_EVAL)
