/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9instr.c: Dealing with instructions of a compiled function
 */

#تعریف_کرو USING_FLOAT_STUFF
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#تعریف_کرو RETURN_NULL_IF_SKIP(cctx) اگر (cctx->ctx_skip == SKIP_YES) واپس تہی
#تعریف_کرو RETURN_OK_IF_SKIP(cctx) اگر (cctx->ctx_skip == SKIP_YES) واپس OK

/*
 * Generate an instruction without arguments.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr(cctx_T *cctx, isntype_T isn_type)
{
    garray_T	*instr = &cctx->ctx_instr;
    isn_T	*isn;

    RETURN_NULL_IF_SKIP(cctx);
    اگر (GA_GROW_FAILS(instr, 1))
	واپس تہی;
    isn = ((isn_T *)instr->ga_ڈیٹا) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    واپس isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, صحیح drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_ڈھیر.ga_len -= drop;
    واپس generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    ساکن isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    اگر ((isn = generate_instr(cctx, isn_type)) == تہی)
	واپس تہی;

    اگر (ڈالو_type_stack2(cctx, type == تہی ? &t_any : type,
			       decl_type == تہی ? &t_any : decl_type) == FAIL)
	واپس تہی;

    واپس isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    واپس generate_instr_type2(cctx, isn_type, type, &t_any);
}

/*
 * Generate an ISN_DEBUG instruction.
 */
    isn_T *
generate_instr_debug(cctx_T *cctx)
{
    isn_T	*isn;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
					       + cctx->ctx_ufunc->uf_dfunc_idx;

    اگر ((isn = generate_instr(cctx, ISN_DEBUG)) == تہی)
	واپس تہی;
    isn->isn_arg.debug.dbg_var_نام_len = dfunc->df_var_نام.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_پچھلا_lnum;
    واپس isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    صحیح
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == تہی)
	واپس FAIL;
    isn->isn_arg.construct.construct_سائز = حجم(object_T)
			       + cl->class_obj_member_گنتی * حجم(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    واپس OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    صحیح
generate_GET_OBJ_MEMBER(cctx_T *cctx, صحیح idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    اگر (isn == تہی)
	واپس FAIL;

    isn->isn_arg.classmember.cm_class = تہی;
    isn->isn_arg.classmember.cm_idx = idx;
    واپس ڈالو_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    صحیح
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, صحیح idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    اگر (isn == تہی)
	واپس FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    واپس ڈالو_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    صحیح
generate_STORE_THIS(cctx_T *cctx, صحیح idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    اگر (isn == تہی)
	واپس FAIL;

    isn->isn_arg.نمبر = idx;
    واپس OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    صحیح
may_generate_2STRING(صحیح انحراف, صحیح tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_ڈھیر(cctx, -1 - انحراف);

    انتخاب (type->tt_type)
    {
	// nothing to be done
	حالت VAR_STRING: واپس OK;

	// conversion possible
	حالت VAR_SPECIAL:
	حالت VAR_BOOL:
	حالت VAR_نمبر:
	حالت VAR_FLOAT:
			 توڑو;

	// conversion possible (with runtime check)
	حالت VAR_ANY:
	حالت VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 توڑو;

	// conversion possible when tolerant
	حالت VAR_LIST:
	حالت VAR_TUPLE:
	حالت VAR_DICT:
			 اگر (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     توڑو;
			 }
			 اگر (tostring_flags & TOSTRING_INTERPOLATE)
			     توڑو;
			 // FALLTHROUGH

	// conversion not possible
	حالت VAR_VOID:
	حالت VAR_BLOB:
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_کام:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
			 مقصد_string_error(type->tt_type);
			 واپس FAIL;
    }

    سیٹ_type_on_ڈھیر(cctx, &t_string, -1 - انحراف);
    اگر ((isn = generate_instr(cctx, isntype)) == تہی)
	واپس FAIL;
    isn->isn_arg.tostring.انحراف = انحراف;
    isn->isn_arg.tostring.flags = tostring_flags;

    واپس OK;
}

    ساکن صحیح
check_نمبر_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    اگر (!((type1 == VAR_نمبر || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_نمبر || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	اگر (check_type_is_قدر(typ1) == FAIL
		|| check_type_is_قدر(typ2) == FAIL)
	    واپس FAIL;
	اگر (*op == '+')
	    emsg(_(e_wrong_دلیل_type_for_plus));
	ورنہ
	    semsg(_(e_char_requires_نمبر_or_float_arguments), *op);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    ساکن صحیح
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    برائے (صحیح i = 0; i < tuple_type->tt_argcount; i++)
    {
	اگر (ga_grow(gap, 1) == FAIL)
	    واپس FAIL;

	((type_T **)gap->ga_ڈیٹا)[gap->ga_len] = tuple_type->tt_دلائل[i];
	gap->ga_len++;
    }

    واپس OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ساکن صحیح
سیٹ_tuple_union_type_on_ڈھیر(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, حجم(type_T *), 10);

    اگر (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    اگر (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    اگر ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    سیٹ_type_on_ڈھیر(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    واپس OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    صحیح
generate_جوڑو_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  اظہاریہ_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_نمبر ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    اگر (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_نمبر_or_float(type1, type2, (char_u *)"+") == FAIL)
	واپس FAIL;

    اگر (isn != تہی)
    {
	اگر (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = اظہاریہ_type;
	ورنہ
	    isn->isn_arg.op.op_type = اظہاریہ_جوڑو;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    اگر (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	سیٹ_type_on_ڈھیر(cctx, &t_list_any, 0);
    ورنہ اگر (vartype == VAR_TUPLE)
    {
	اگر (!check_tuples_addable(type1, type2))
	    واپس FAIL;

	اگر (سیٹ_tuple_union_type_on_ڈھیر(type1, type2, cctx) == FAIL)
	    واپس FAIL;
    }

    واپس isn == تہی ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    اگر (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_نمبر
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	واپس type1->tt_type;
    واپس VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    صحیح
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_ڈھیر(cctx, 1);
    type2 = get_type_on_ڈھیر(cctx, 0);
    vartype = operator_type(type1, type2);

    انتخاب (*op)
    {
	حالت '+':
		  اگر (generate_جوڑو_instr(cctx, vartype, type1, type2,
							    اظہاریہ_COPY) == FAIL)
		      واپس FAIL;
		  توڑو;

	حالت '-':
	حالت '*':
	حالت '/': اگر (check_نمبر_or_float(type1, type2, op) == FAIL)
		      واپس FAIL;
		  اگر (vartype == VAR_نمبر)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  ورنہ اگر (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  ورنہ
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  اگر (isn != تہی)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? اظہاریہ_MULT : *op == '/'? اظہاریہ_DIV : اظہاریہ_SUB;
		  توڑو;

	حالت '%': اگر ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_نمبر)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_نمبر))
		  {
		      emsg(_(e_percent_requires_نمبر_arguments));
		      واپس FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_نمبر ? ISN_OPNR : ISN_OPANY, 1);
		  اگر (isn != تہی)
		      isn->isn_arg.op.op_type = اظہاریہ_REM;
		  توڑو;
    }

    // correct type of result
    اگر (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	اگر ((type1->tt_type == VAR_نمبر || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_نمبر || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	سیٹ_type_on_ڈھیر(cctx, type, 0);
    }

    واپس OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    ساکن isntype_T
compare_isn_not_اقدار(typval_T *tv, type_T *type)
{
    اگر (tv != تہی)
	(خالی)check_typval_is_قدر(tv);
    ورنہ
	(خالی)check_type_is_قدر(type);
    واپس ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    ساکن isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != تہی ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != تہی ? tv2->v_type : type2->tt_type;

    اگر (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	واپس compare_isn_not_اقدار(tv1, type1);
    اگر (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	واپس compare_isn_not_اقدار(tv2, type2);

    اگر (vartype1 == vartype2)
    {
	انتخاب (vartype1)
	{
	    حالت VAR_BOOL: isntype = ISN_COMPAREBOOL; توڑو;
	    حالت VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; توڑو;
	    حالت VAR_نمبر: isntype = ISN_COMPARENR; توڑو;
	    حالت VAR_FLOAT: isntype = ISN_COMPAREFLOAT; توڑو;
	    حالت VAR_STRING: isntype = ISN_COMPARESTRING; توڑو;
	    حالت VAR_BLOB: isntype = ISN_COMPAREBLOB; توڑو;
	    حالت VAR_LIST: isntype = ISN_COMPARELIST; توڑو;
	    حالت VAR_TUPLE: isntype = ISN_COMPARETUPLE; توڑو;
	    حالت VAR_DICT: isntype = ISN_COMPAREDICT; توڑو;
	    حالت VAR_FUNC: isntype = ISN_COMPAREFUNC; توڑو;
	    حالت VAR_OBJECT: isntype = ISN_COMPAREOBJECT; توڑو;
	    طےشدہ: isntype = ISN_COMPAREANY; توڑو;
	}
    }
    ورنہ اگر (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_نمبر || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_نمبر || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    ورنہ اگر (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	اگر ((vartype1 == VAR_SPECIAL
		&& (tv1 != تہی ? tv1->vval.v_نمبر == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != تہی ? tv2->vval.v_نمبر == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    واپس ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    اگر ((exprtype == اظہاریہ_IS || exprtype == اظہاریہ_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == اظہاریہ_IS ? "is" : "isnot" , vartype_name(vartype1));
	واپس ISN_DROP;
    }
    اگر (!(exprtype == اظہاریہ_IS || exprtype == اظہاریہ_ISNOT
		|| exprtype == اظہاریہ_EQUAL || exprtype == اظہاریہ_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	واپس ISN_DROP;
    }
    اگر (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != اظہاریہ_EQUAL
			&& exprtype != اظہاریہ_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != اظہاریہ_EQUAL
			 && exprtype != اظہاریہ_NEQUAL
			 && exprtype != اظہاریہ_IS
			 && exprtype != اظہاریہ_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	واپس ISN_DROP;
    }
    واپس isntype;
}

    صحیح
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    اگر (get_compare_isn(type, tv1, tv2, تہی, تہی) == ISN_DROP)
	واپس FAIL;
    واپس OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    صحیح
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, صحیح ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, تہی, تہی,
		       get_type_on_ڈھیر(cctx, 1), get_type_on_ڈھیر(cctx, 0));
    اگر (isntype == ISN_DROP)
	واپس FAIL;

    اگر ((isn = generate_instr(cctx, isntype)) == تہی)
	واپس FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --ڈھیر->ga_len;
    سیٹ_type_on_ڈھیر(cctx, &t_bool, 0);

    واپس OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    صحیح
generate_CONCAT(cctx_T *cctx, صحیح گنتی)
{
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    RETURN_OK_IF_SKIP(cctx);

    اگر ((isn = generate_instr(cctx, ISN_CONCAT)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = گنتی;

    // drop the argument types
    ڈھیر->ga_len -= گنتی - 1;

    واپس OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    صحیح
generate_2BOOL(cctx_T *cctx, صحیح invert, صحیح انحراف)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_2BOOL)) == تہی)
	واپس FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.انحراف = انحراف;

    // type becomes bool
    سیٹ_type_on_ڈھیر(cctx, &t_bool, -1 - انحراف);

    واپس OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    صحیح
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    اگر (generate_instr(cctx, ISN_COND2BOOL) == تہی)
	واپس FAIL;

    // type becomes bool
    سیٹ_type_on_ڈھیر(cctx, &t_bool, 0);

    واپس OK;
}

    صحیح
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	صحیح	    typechk_flags,	// type check flags
	صحیح	    انحراف,
	صحیح	    is_var,
	صحیح	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == تہی)
	واپس FAIL;
    type_T *tt;
    اگر ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_نمبر_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	اگر (tt != تہی)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    اگر (typechk_flags & TYPECHK_نمبر_OK)
		tt->tt_flags |= TTFLAG_نمبر_OK;
	    ورنہ
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    ورنہ
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)انحراف;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    سیٹ_type_on_ڈھیر(cctx, expected, -1 - انحراف);

    واپس OK;
}

    صحیح
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_SETTYPE)) == تہی)
	واپس FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    واپس OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    صحیح
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    اگر (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == تہی)
	واپس FAIL;
    واپس OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    ساکن صحیح
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == تہی ? &t_any : &class->class_type);
    اگر (isn == تہی)
	واپس FAIL;
    isn->isn_arg.classarg = class;
    اگر (class != تہی)
	++class->class_refcount;
    واپس OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    صحیح
generate_tv_ڈالو(cctx_T *cctx, typval_T *tv)
{
    انتخاب (tv->v_type)
    {
	حالت VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_نمبر);
	    توڑو;
	حالت VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_نمبر);
	    توڑو;
	حالت VAR_نمبر:
	    generate_PUSHNR(cctx, tv->vval.v_نمبر);
	    توڑو;
	حالت VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    توڑو;
	حالت VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = تہی;
	    توڑو;
	حالت VAR_LIST:
	    اگر (tv->vval.v_list != تہی)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    توڑو;
	حالت VAR_TUPLE:
	    اگر (tv->vval.v_tuple != تہی)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    توڑو;
	حالت VAR_DICT:
	    اگر (tv->vval.v_dict != تہی)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    توڑو;
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	حالت VAR_کام:
	    اگر (tv->vval.v_کام != تہی)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    توڑو;
	حالت VAR_CHANNEL:
	    اگر (tv->vval.v_channel != تہی)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    توڑو;
#اگر_ختم
	حالت VAR_FUNC:
	    اگر (tv->vval.v_string != تہی)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, تہی, &t_func_unknown, TRUE);
	    توڑو;
	حالت VAR_PARTIAL:
	    اگر (tv->vval.v_partial != تہی)
		iemsg("non-null partial constant not supported");
	    اگر (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == تہی)
		واپس FAIL;
	    توڑو;
	حالت VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = تہی;
	    توڑو;
	حالت VAR_OBJECT:
	    اگر (tv->vval.v_object != تہی)
	    {
		emsg(_(e_cannot_use_non_null_object));
		واپس FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    توڑو;
	حالت VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    توڑو;
	طےشدہ:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    واپس FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    واپس OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    صحیح
generate_PUSHNR(cctx_T *cctx, varnumber_T نمبر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_نمبر)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = نمبر;

    اگر (نمبر == 0 || نمبر == 1)
	// A 0 or 1 number can also be used as a bool.
	سیٹ_type_on_ڈھیر(cctx, &t_نمبر_bool, 0);
    واپس OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    صحیح
generate_PUSHBOOL(cctx_T *cctx, varnumber_T نمبر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = نمبر;

    واپس OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    صحیح
generate_PUSHSPEC(cctx_T *cctx, varnumber_T نمبر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     نمبر == VVAL_NULL ? &t_null : &t_none)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = نمبر;

    واپس OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    صحیح
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == تہی)
	واپس FAIL;
    isn->isn_arg.fnumber = fnumber;

    واپس OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    صحیح
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    صحیح		ret = OK;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	اگر ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == تہی)
	    ret = FAIL;
	ورنہ
	{
	    isn->isn_arg.string = str == تہی ? تہی : *str;
	    واپس OK;
	}
    }
    اگر (str != تہی)
	VIM_CLEAR(*str);
    واپس ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    صحیح
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    اگر (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == تہی)
	واپس FAIL;
    واپس OK;
#ورنہ
    emsg(_(e_channel_کام_feature_not_دستیاب));
    واپس FAIL;
#اگر_ختم
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    صحیح
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    اگر (generate_instr_type(cctx, ISN_PUSHJOB, &t_کام) == تہی)
	واپس FAIL;
    واپس OK;
#ورنہ
    emsg(_(e_channel_کام_feature_not_دستیاب));
    واپس FAIL;
#اگر_ختم
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    صحیح
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == تہی)
	واپس FAIL;
    isn->isn_arg.blob = blob;

    واپس OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    صحیح
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, صحیح may_سابقہ)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == تہی)
	واپس FAIL;
    اگر (name == تہی)
	funcname = تہی;
    ورنہ اگر (!may_سابقہ
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != تہی)	    // autoload
	funcname = vim_strsave(name);
    ورنہ
    {
	funcname = alloc(STRLEN(name) + 3);
	اگر (funcname != تہی)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    واپس OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    صحیح
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == تہی)
	واپس FAIL;
    isn->isn_arg.string = vim_strsave(name);
    اگر (isn->isn_arg.string == تہی)
	واپس FAIL;
    واپس OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    صحیح
generate_GETITEM(cctx_T *cctx, صحیح اشاریہ, صحیح with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_ڈھیر(cctx, with_op ? 1 : 0);
    type_T	*شے_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    شے_type = get_شے_type(type);
    اگر ((isn = generate_instr(cctx, ISN_GETITEM)) == تہی)
	واپس FAIL;
    isn->isn_arg.getitem.gi_اشاریہ = اشاریہ;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    واپس ڈالو_type_ڈھیر(cctx, شے_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    ساکن خالی
سیٹ_tuple_قاش_type_on_ڈھیر(type_T *type, صحیح گنتی, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    اگر ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= گنتی)
    {
	سیٹ_type_on_ڈھیر(cctx, &t_tuple_any, 0);
	واپس;
    }

    ga_init2(&tuple_types_ga, حجم(type_T *), 10);

    برائے (صحیح i = گنتی; i < type->tt_argcount; i++)
    {
	اگر (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    سیٹ_type_on_ڈھیر(cctx, &t_tuple_any, 0);
	    واپس;
	}
	((type_T **)tuple_types_ga.ga_ڈیٹا)[tuple_types_ga.ga_len] =
							     type->tt_دلائل[i];
	tuple_types_ga.ga_len++;
    }

    سیٹ_type_on_ڈھیر(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    صحیح
generate_قاش(cctx_T *cctx, صحیح گنتی)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_قاش)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = گنتی;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_ڈھیر(cctx, 0);
    اگر (type->tt_type == VAR_TUPLE)
	سیٹ_tuple_قاش_type_on_ڈھیر(type, گنتی, cctx);

    واپس OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    صحیح
generate_CHECKLEN(cctx_T *cctx, صحیح min_len, صحیح مزید_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    اگر ((isn = generate_instr(cctx, ISN_CHECKLEN)) == تہی)
	واپس FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_مزید_OK = مزید_OK;

    واپس OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    صحیح
generate_STORE(cctx_T *cctx, isntype_T isn_type, صحیح idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, isn_type, 1)) == تہی)
	واپس FAIL;
    اگر (name != تہی)
	isn->isn_arg.string = vim_strsave(name);
    ورنہ
	isn->isn_arg.نمبر = idx;

    واپس OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    صحیح
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	صحیح	    بھرو,
	class_T	    *cl,
	صحیح	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر (بھرو)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_بھرو_CLASSMEMBER, m->ocm_type);
    }
    ورنہ
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    اگر (isn == تہی)
	واپس FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    واپس OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    ساکن صحیح
generate_STOREOUTER(
	cctx_T	    *cctx,
	صحیح	    idx,
	صحیح	    level,
	صحیح	    loop_گہرائی,
	صحیح	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == تہی)
	واپس FAIL;
    اگر (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_گہرائی = -loop_گہرائی - 1;
    }
    ورنہ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_گہرائی = level;
    }

    واپس OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    صحیح
generate_STORENR(cctx_T *cctx, صحیح idx, varnumber_T قدر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_STORENR)) == تہی)
	واپس FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = قدر;

    واپس OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    ساکن صحیح
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	صحیح	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, isn_type, 1)) == تہی)
	واپس FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    واپس OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    صحیح
generate_بھرو(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	صحیح	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type2(cctx, isn_type, type, type)) == تہی)
	واپس FAIL;
    اگر (name != تہی)
	isn->isn_arg.string = vim_strsave(name);
    ورنہ
	isn->isn_arg.نمبر = idx;

    واپس OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    صحیح
generate_LOADOUTER(
	cctx_T	    *cctx,
	صحیح	    idx,
	صحیح	    nesting,
	صحیح	    loop_گہرائی,
	صحیح	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == تہی)
	واپس FAIL;
    اگر (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_گہرائی = -loop_گہرائی - 1;
    }
    ورنہ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_گہرائی = nesting;
    }

    واپس OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    صحیح
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    صحیح	    di_flags;
    صحیح	    vidx = ڈھونڈو_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    اگر (vidx < 0)
    {
	semsg(_(e_variable_not_ملا_str), name);
	واپس FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    واپس generate_بھرو(cctx, ISN_LOADV, vidx, تہی, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    صحیح
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, صحیح forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, isn_type)) == تہی)
	واپس FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    واپس OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    صحیح
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    اگر (generate_instr(cctx, ISN_LOCKCONST) == تہی)
	واپس FAIL;
    واپس OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    صحیح
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	صحیح	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    ورنہ
	isn = generate_instr_drop(cctx, isn_type, 1);
    اگر (isn == تہی)
	واپس FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    واپس OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    صحیح
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	صحیح	    sid,
	صحیح	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_شے(sid);

    RETURN_OK_IF_SKIP(cctx);
    اگر (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    ورنہ
	isn = generate_instr_drop(cctx, isn_type, 1);
    اگر (isn == تہی)
	واپس FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    اگر (sref == تہی)
	واپس FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    واپس OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    صحیح
generate_NEWLIST(cctx_T *cctx, صحیح گنتی, صحیح use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_NEWLIST)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = use_null ? -1 : گنتی;

    // Get the member type and the declared member type from all the items on
    // the stack.
    اگر ((member_type = get_member_type_from_ڈھیر(گنتی, 1, cctx)) == تہی)
	واپس FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_ڈھیر.ga_len -= گنتی;

    // add the list type to the type stack
    واپس ڈالو_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    صحیح
generate_NEWTUPLE(cctx_T *cctx, صحیح گنتی, صحیح use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = use_null ? -1 : گنتی;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, حجم(type_T *), 10);

    اگر (get_tuple_type_from_ڈھیر(گنتی, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	واپس FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_ڈھیر.ga_len -= گنتی;

    // add the tuple type to the type stack
    واپس ڈالو_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    صحیح
generate_NEWDICT(cctx_T *cctx, صحیح گنتی, صحیح use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_NEWDICT)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = use_null ? -1 : گنتی;

    اگر ((member_type = get_member_type_from_ڈھیر(گنتی, 2, cctx)) == تہی)
	واپس FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_ڈھیر.ga_len -= 2 * گنتی;

    // add the dict type to the type stack
    واپس ڈالو_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    صحیح
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	صحیح	    object_طریقہ,
	صحیح	    fi,
	صحیح	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    صحیح		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_FUNCREF)) == تہی)
	واپس FAIL;
    اگر (isn_idx != تہی)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    اگر (ufunc->uf_def_حالت == UF_NOT_COMPILED || has_vars || cl != تہی)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	اگر (extra == تہی)
	    واپس FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	اگر (cl != تہی)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_طریقہ = object_طریقہ;
	    extra->fre_طریقہ_idx = fi;
	}
    }
    اگر (ufunc->uf_def_حالت == UF_NOT_COMPILED || cl != تہی)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    اگر (ufunc->uf_def_حالت != UF_NOT_COMPILED && cl == تہی)
    {
	اگر (isn_idx == تہی && ufunc->uf_def_حالت == UF_مقصد_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (خالی)compile_def_function(ufunc, FALSE, CT_NONE, تہی);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_احاطہ = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    اگر ((ufunc->uf_flags & FC_احاطہ)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_احاطہ;

    type = ufunc->uf_func_type == تہی ? &t_func_any : ufunc->uf_func_type;
    واپس ڈالو_type_ڈھیر(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    صحیح
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    صحیح		ret = OK;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	اگر ((isn = generate_instr(cctx, ISN_NEWFUNC)) == تہی)
	    ret = FAIL;
	ورنہ
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    اگر (arg == تہی)
		ret = FAIL;
	    ورنہ
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_احاطہ = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(خالی)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		واپس OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    واپس ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    صحیح
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_DEF)) == تہی)
	واپس FAIL;
    اگر (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	اگر (isn->isn_arg.string == تہی)
	    واپس FAIL;
    }
    واپس OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    صحیح
generate_JUMP(cctx_T *cctx, jumpwhen_T when, صحیح where)
{
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_JUMP)) == تہی)
	واپس FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    اگر (when != JUMP_ALWAYS && ڈھیر->ga_len > 0)
	--ڈھیر->ga_len;

    واپس OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    صحیح
generate_WHILE(cctx_T *cctx, صحیح funcref_idx)
{
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_WHILE)) == تہی)
	واپس FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_انتہا = 0;  // filled in later

    اگر (ڈھیر->ga_len > 0)
	--ڈھیر->ga_len;

    واپس OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    صحیح
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, صحیح arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, isn_type)) == تہی)
	واپس FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    واپس OK;
}

    صحیح
generate_FOR(cctx_T *cctx, صحیح loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_FOR)) == تہی)
	واپس FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    واپس ڈالو_type_ڈھیر(cctx, &t_any);
}

    صحیح
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_ENDLOOP)) == تہی)
	واپس FAIL;
    isn->isn_arg.endloop.انتہا_گہرائی = loop_info->li_گہرائی;
    isn->isn_arg.endloop.انتہا_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.انتہا_var_idx = loop_info->li_local_گنتی;
    isn->isn_arg.endloop.انتہا_var_گنتی =
			   cctx->ctx_locals.ga_len - loop_info->li_local_گنتی;
    واپس OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    صحیح
generate_TRYCONT(cctx_T *cctx, صحیح سطحیں, صحیح where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_TRYCONT)) == تہی)
	واپس FAIL;
    isn->isn_arg.trycont.tct_سطحیں = سطحیں;
    isn->isn_arg.trycont.tct_where = where;

    واپس OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    صحیح
check_اندرونی_func_دلائل(
	cctx_T	*cctx,
	صحیح	func_idx,
	صحیح	argcount,
	صحیح	طریقہ_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
    صحیح		argoff = check_اندرونی_func(func_idx, argcount);

    اگر (argoff < 0)
	واپس FAIL;

    اگر (طریقہ_call && argoff > 1)
    {
	اگر (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 اندرونی_func_name(func_idx));
	    واپس FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	اگر (isn  == تہی)
	    واپس FAIL;
	isn->isn_arg.shuffle.shfl_شے = argcount;
	isn->isn_arg.shuffle.shfl_اوپر = argoff - 1;
    }

    اگر (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)ڈھیر->ga_ڈیٹا) + ڈھیر->ga_len - argcount;

	// Check the types of the arguments.
	اگر (طریقہ_call && argoff > 1)
	{
	    صحیح i;

	    برائے (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ورنہ
	{
	    صحیح i;

	    برائے (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	اگر (اندرونی_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    واپس FAIL;
    }
    واپس OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    صحیح
generate_BCALL(cctx_T *cctx, صحیح func_idx, صحیح argcount, صحیح طریقہ_call)
{
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
    type2_T	*argtypes = تہی;
    type2_T	shuffled_argtypes[MAX_FUNC_دلائل];
    type2_T	*maptype = تہی;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    اگر (check_اندرونی_func_دلائل(cctx, func_idx, argcount, طریقہ_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	واپس FAIL;

    اگر (اندرونی_func_is_map(func_idx))
	maptype = argtypes;

    اگر ((isn = generate_instr(cctx, ISN_BCALL)) == تہی)
	واپس FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    ڈھیر->ga_len -= argcount;
    type = اندرونی_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    اگر (ڈالو_type_stack2(cctx, type, decl_type) == FAIL)
	واپس FAIL;

    اگر (maptype != تہی && maptype[0].type_decl->tt_member != تہی
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    واپس OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    صحیح
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*شے_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_ڈھیر(cctx, 1);
    اگر (arg_type_modifiable(list_type, 1) == FAIL)
	واپس FAIL;
    شے_type = get_type_on_ڈھیر(cctx, 0);
    expected = list_type->tt_member;
    اگر (need_type(شے_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	واپس FAIL;

    اگر (generate_instr(cctx, ISN_LISTAPPEND) == تہی)
	واپس FAIL;

    --cctx->ctx_type_ڈھیر.ga_len;	    // drop the argument
    واپس OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    صحیح
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*شے_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    اگر (arg_type_modifiable(get_decl_type_on_ڈھیر(cctx, 1), 1) == FAIL)
	واپس FAIL;
    شے_type = get_type_on_ڈھیر(cctx, 0);
    اگر (need_type(شے_type, &t_نمبر, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	واپس FAIL;

    اگر (generate_instr(cctx, ISN_BLOBAPPEND) == تہی)
	واپس FAIL;

    --cctx->ctx_type_ڈھیر.ga_len;	    // drop the argument
    واپس OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    ساکن isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    اگر (cl != تہی)
	واپس ISN_METHODCALL;

    اگر (ufunc->uf_def_حالت == UF_NOT_COMPILED)
	واپس ISN_UCALL;

    // function invoked from a lambda
    اگر (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	اگر (ufunc->uf_name[0] == K_SPECIAL)
	    واپس ISN_UCALL;
    }

    واپس ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    صحیح
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	صحیح	    mi,
	صحیح	    pushed_argcount,
	صحیح	    is_super)
{
    isn_T	*isn;
    صحیح		regular_دلائل = ufunc->uf_دلائل.ga_len;
    صحیح		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    اگر (argcount > regular_دلائل && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	واپس FAIL;
    }
    اگر (argcount < regular_دلائل - ufunc->uf_def_دلائل.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	واپس FAIL;
    }

    اگر (ufunc->uf_def_حالت != UF_NOT_COMPILED
	    && ufunc->uf_def_حالت != UF_COMPILE_ERROR)
    {
	صحیح		i;
	compiletype_T	compile_type;

	برائے (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_ڈھیر(cctx, argcount - i - 1);
	    اگر (check_type_is_قدر(actual) == FAIL)
		واپس FAIL;
	    اگر (actual->tt_type == VAR_SPECIAL
			      && i >= regular_دلائل - ufunc->uf_def_دلائل.ga_len)
	    {
		// assume v:none used for default argument value
		جاری;
	    }
	    اگر (i < regular_دلائل)
	    {
		اگر (ufunc->uf_arg_types == تہی)
		    جاری;
		expected = ufunc->uf_arg_types[i];
	    }
	    ورنہ اگر (ufunc->uf_va_type == تہی
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    ورنہ
		expected = ufunc->uf_va_type->tt_member;
	    اگر (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		واپس FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	اگر (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == تہی,
						   compile_type, تہی) == FAIL)
	    واپس FAIL;
    }
    اگر (ufunc->uf_def_حالت == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_مقصد_function_that_failed_مقصد_compile_str,
							       ufunc->uf_name);
	واپس FAIL;
    }

    اگر ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == تہی)
	واپس FAIL;
    اگر (cl != تہی /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	اگر (isn->isn_arg.mfunc == تہی)
	    واپس FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = تہی;
	اگر (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = تہی;
	isn->isn_arg.mfunc->cmf_cache_generic = تہی;
	isn->isn_arg.mfunc->cmf_cache_ufunc = تہی;
    }
    ورنہ اگر (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    ورنہ
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_ڈھیر.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    اگر (ufunc->uf_class != تہی)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_ڈھیر(cctx, 0);
	اگر (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_ڈھیر.ga_len--;
    }

    // add return type
    واپس ڈالو_type_ڈھیر(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    صحیح
generate_UCALL(cctx_T *cctx, char_u *name, صحیح argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_UCALL)) == تہی)
	واپس FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_ڈھیر.ga_len -= argcount;

    // add return value
    واپس ڈالو_type_ڈھیر(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    صحیح
check_func_دلائل_from_type(
	cctx_T	*cctx,
	type_T	*type,
	صحیح	argcount,
	صحیح	at_چوٹی,
	char_u	*name)
{
    اگر (type->tt_argcount != -1)
    {
	صحیح	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	اگر (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    واپس FAIL;
	}
	اگر (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    واپس FAIL;
	}
	اگر (type->tt_دلائل != تہی)
	{
	    صحیح i;

	    برائے (i = 0; i < argcount; ++i)
	    {
		صحیح	انحراف = -argcount + i - (at_چوٹی ? 0 : 1);
		type_T	*actual = get_type_on_ڈھیر(cctx, -1 - انحراف);
		type_T	*expected;

		اگر (check_type_is_قدر(actual) == FAIL)
		    واپس FAIL;
		اگر (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_دلائل[type->tt_argcount - 1];
		    اگر (expected != تہی && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    اگر (expected == تہی)
			expected = &t_any;
		}
		ورنہ اگر (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		ورنہ
		    expected = type->tt_دلائل[i];
		اگر (need_type(actual, expected, 0,
				     انحراف, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    واپس FAIL;
		}
	    }
	}
    }

    واپس OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    صحیح
generate_PCALL(
	cctx_T	*cctx,
	صحیح	argcount,
	char_u	*name,
	type_T	*type,
	صحیح	at_چوٹی)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    اگر (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    ورنہ اگر (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	اگر (check_func_دلائل_from_type(cctx, type, argcount, at_چوٹی, name)
								       == FAIL)
	    واپس FAIL;

	ret_type = type->tt_member;
	اگر (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    ورنہ
    {
	semsg(_(e_not_callable_type_str), name);
	واپس FAIL;
    }

    اگر ((isn = generate_instr(cctx, ISN_PCALL)) == تہی)
	واپس FAIL;
    isn->isn_arg.pfunc.cpf_چوٹی = at_چوٹی;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_ڈھیر.ga_len -= argcount + 1;

    // push the return value
    ڈالو_type_ڈھیر(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    اگر (at_چوٹی && generate_instr(cctx, ISN_PCALL_انتہا) == تہی)
	واپس FAIL;

    واپس OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    صحیح
generate_DEFER(cctx_T *cctx, صحیح var_idx, صحیح argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == تہی)
	واپس FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    واپس OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    صحیح
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == تہی)
	واپس FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_ڈھیر(cctx, 0);
    اگر (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	حرف *tofree;

	semsg(_(e_expected_dictionary_for_using_کلید_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	واپس FAIL;
    }
    // change dict type to dict member type
    اگر (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	سیٹ_type_on_ڈھیر(cctx, ntype, 0);
    }

    واپس OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    صحیح
generate_ECHO(cctx_T *cctx, صحیح with_white, صحیح گنتی)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, ISN_ECHO, گنتی)) == تہی)
	واپس FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_گنتی = گنتی;

    واپس OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    صحیح
generate_MULT_اظہاریہ(cctx_T *cctx, isntype_T isn_type, صحیح گنتی)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, isn_type, گنتی)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = گنتی;
    واپس OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    صحیح
generate_ECHOWINDOW(cctx_T *cctx, صحیح گنتی, طویل وقت)
{
    isn_T	*isn;

    اگر ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, گنتی)) == تہی)
	واپس FAIL;
    isn->isn_arg.echowin.ewin_گنتی = گنتی;
    isn->isn_arg.echowin.ewin_وقت = وقت;
    واپس OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    صحیح
generate_ماخذ(cctx_T *cctx, صحیح sid)
{
    isn_T	*isn;

    اگر ((isn = generate_instr(cctx, ISN_ماخذ)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = sid;

    واپس OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    صحیح
generate_PUT(cctx_T *cctx, صحیح regname, linenr_T lnum, صحیح fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    اگر (isn == تہی)
	واپس FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    واپس OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    صحیح
generate_LOCKUNLOCK(cctx_T *cctx, char_u *سطر, صحیح is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == تہی)
	واپس FAIL;
    class_T *cl = cctx->ctx_ufunc != تہی ? cctx->ctx_ufunc->uf_class : تہی;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(سطر);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    اگر (cl != تہی)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    واپس OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    صحیح
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *سطر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, isntype)) == تہی)
	واپس FAIL;
    isn->isn_arg.string = vim_strsave(سطر);
    واپس OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    صحیح
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    صحیح		ret = OK;

    اگر (cctx->ctx_skip != SKIP_YES)
    {
	اگر ((isn = generate_instr(cctx, isntype)) == تہی)
	    ret = FAIL;
	ورنہ
	{
	    isn->isn_arg.string = str;
	    واپس OK;
	}
    }
    vim_free(str);
    واپس ret;
}

    صحیح
generate_LEGACY_EVAL(cctx_T *cctx, char_u *سطر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == تہی)
	واپس FAIL;
    isn->isn_arg.string = vim_strsave(سطر);

    واپس ڈالو_type_ڈھیر(cctx, &t_any);
}

    صحیح
generate_EXECCONCAT(cctx_T *cctx, صحیح گنتی)
{
    isn_T	*isn;

    اگر ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, گنتی)) == تہی)
	واپس FAIL;
    isn->isn_arg.نمبر = گنتی;
    واپس OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    صحیح
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    اگر ((isn = generate_instr(cctx, ISN_RANGE)) == تہی)
	واپس FAIL;
    isn->isn_arg.string = range;

    واپس ڈالو_type_ڈھیر(cctx, &t_نمبر);
}

    صحیح
generate_UNPACK(cctx_T *cctx, صحیح var_گنتی, صحیح semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_UNPACK)) == تہی)
	واپس FAIL;
    isn->isn_arg.unpack.unp_گنتی = var_گنتی;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    واپس OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    صحیح
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    اگر (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	اگر ((isn = generate_instr(cctx, ISN_CMDMOD)) == تہی)
	    واپس FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	اگر (isn->isn_arg.cmdmod.cf_cmdmod == تہی)
	    واپس FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, حجم(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = تہی;
    }

    واپس OK;
}

    صحیح
generate_undo_cmdmods(cctx_T *cctx)
{
    اگر (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == تہی)
	واپس FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    واپس OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    صحیح
generate_store_var(
	cctx_T		*cctx,
	تفویض_dest_T	dest,
	صحیح		opt_flags,
	صحیح		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    انتخاب (dest)
    {
	حالت dest_option:
	    واپس generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	حالت dest_func_option:
	    واپس generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	حالت dest_global:
	    // include g: with the name, easier to execute that way
	    واپس generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == تہی
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	حالت dest_بفر:
	    // include b: with the name, easier to execute that way
	    واپس generate_STORE(cctx, ISN_STOREB, 0, name);
	حالت dest_کھڑکی:
	    // include w: with the name, easier to execute that way
	    واپس generate_STORE(cctx, ISN_STOREW, 0, name);
	حالت dest_tab:
	    // include t: with the name, easier to execute that way
	    واپس generate_STORE(cctx, ISN_STORET, 0, name);
	حالت dest_env:
	    واپس generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	حالت dest_reg:
	    واپس generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], تہی);
	حالت dest_vimvar:
	    واپس generate_STORE(cctx, ISN_STOREV, vimvaridx, تہی);
	حالت dest_script:
	حالت dest_script_v9:
	    {
		صحیح	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		صحیح	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		اگر (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    اگر (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_شے(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_شے(scriptvar_sid)
						  ->sn_autoload_سابقہ == تہی)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			//پہلا
			اگر (generate_ماخذ(cctx, scriptvar_sid) == FAIL)
			    واپس FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    واپس generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		واپس generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	حالت dest_class_member:
	    واپس generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	حالت dest_local:
	حالت dest_اظہاریہ:
	    // cannot happen
	    توڑو;
    }
    واپس FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    صحیح
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    برائے (;;)
    {
	اگر (scope == تہی)
	    توڑو;
	اگر (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    واپس TRUE;
	scope = scope->se_outer;
    }
    واپس FALSE;
}

    صحیح
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, صحیح instr_گنتی, صحیح is_decl)
{
    اگر (lhs->lhs_dest != dest_local)
	واپس generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    اگر (lhs->lhs_lvar == تہی)
	واپس OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_ڈیٹا) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    اگر (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_گنتی + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.نمبر;
	garray_T    *ڈھیر = &cctx->ctx_type_ڈھیر;

	اگر (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	ورنہ
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	اگر (ڈھیر->ga_len > 0)
	    --ڈھیر->ga_len;
    }
    ورنہ اگر (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_گہرائی,
		lhs->lhs_lvar->lv_loop_idx);
    ورنہ
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, تہی);
    واپس OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    صحیح
generate_SCRIPTCTX_سیٹ(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_SCRIPTCTX_سیٹ)) == تہی)
	واپس FAIL;
    isn->isn_arg.setsctx = new_sctx;
    واپس OK;
}

#اگر defined(FEAT_PROFILE)
    خالی
may_generate_prof_انتہا(cctx_T *cctx, صحیح prof_lnum)
{
    اگر (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_انتہا);
}
#اگر_ختم


/*
 * Delete an instruction, free what it contains.
 */
    خالی
delete_instr(isn_T *isn)
{
    انتخاب (isn->isn_type)
    {
	حالت ISN_AUTOLOAD:
	حالت ISN_DEF:
	حالت ISN_EXEC:
	حالت ISN_EXECRANGE:
	حالت ISN_EXEC_بانٹو:
	حالت ISN_LEGACY_EVAL:
	حالت ISN_LOADAUTO:
	حالت ISN_LOADB:
	حالت ISN_LOADENV:
	حالت ISN_LOADG:
	حالت ISN_LOADOPT:
	حالت ISN_LOADT:
	حالت ISN_LOADW:
	حالت ISN_PUSHEXC:
	حالت ISN_PUSHFUNC:
	حالت ISN_PUSHS:
	حالت ISN_RANGE:
	حالت ISN_STOREAUTO:
	حالت ISN_STOREB:
	حالت ISN_STOREENV:
	حالت ISN_STOREG:
	حالت ISN_STORET:
	حالت ISN_STOREW:
	حالت ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    توڑو;

	حالت ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    توڑو;

	حالت ISN_SUBSTITUTE:
	    {
		صحیح	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		برائے (idx = 0; list[idx].isn_type != ISN_اختتام; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    توڑو;

	حالت ISN_INSTR:
	    {
		صحیح	idx;
		isn_T	*list = isn->isn_arg.instr;

		برائے (idx = 0; list[idx].isn_type != ISN_اختتام; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    توڑو;

	حالت ISN_LOADS:
	حالت ISN_LOADEXPORT:
	حالت ISN_STORES:
	حالت ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    توڑو;

	حالت ISN_UNLET:
	حالت ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    توڑو;

	حالت ISN_STOREOPT:
	حالت ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    توڑو;

	حالت ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    توڑو;

	حالت ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    توڑو;

	حالت ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    توڑو;

	حالت ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		اگر (extra == تہی || extra->fre_func_name == تہی)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    اگر (ufunc != تہی && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		اگر (extra != تہی)
		{
		    char_u *name = extra->fre_func_name;

		    اگر (name != تہی)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    اگر (extra->fre_class != تہی)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    توڑو;

	حالت ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
		    + isn->isn_arg.dfunc.cdf_idx;

		اگر (dfunc->df_ufunc != تہی
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    توڑو;

	حالت ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    توڑو;

	حالت ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		اگر (arg != تہی)
		{
		    ufunc_T *ufunc = ڈھونڈو_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    اگر (ufunc != تہی)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    توڑو;

	حالت ISN_CHECKTYPE:
	حالت ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    توڑو;

	حالت ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    توڑو;

	حالت ISN_LOADSCRIPT:
	حالت ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    توڑو;

	حالت ISN_بھرو_CLASSMEMBER:
	حالت ISN_STORE_CLASSMEMBER:
	حالت ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    توڑو;

	حالت ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    توڑو;

	حالت ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    توڑو;

	حالت ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    توڑو;

	حالت ISN_2BOOL:
	حالت ISN_2STRING:
	حالت ISN_2STRING_ANY:
	حالت ISN_ADDBLOB:
	حالت ISN_ADDLIST:
	حالت ISN_ADDTUPLE:
	حالت ISN_ANYINDEX:
	حالت ISN_ANYSLICE:
	حالت ISN_BCALL:
	حالت ISN_BLOBAPPEND:
	حالت ISN_BLOBINDEX:
	حالت ISN_BLOBSLICE:
	حالت ISN_CATCH:
	حالت ISN_CEXPR_AUCMD:
	حالت ISN_CHECKLEN:
	حالت ISN_CLEARDICT:
	حالت ISN_CMDMOD_REV:
	حالت ISN_COMPAREANY:
	حالت ISN_COMPAREBLOB:
	حالت ISN_COMPAREBOOL:
	حالت ISN_COMPAREDICT:
	حالت ISN_COMPAREFLOAT:
	حالت ISN_COMPAREFUNC:
	حالت ISN_COMPARELIST:
	حالت ISN_COMPARETUPLE:
	حالت ISN_COMPARENR:
	حالت ISN_COMPARENULL:
	حالت ISN_COMPAREOBJECT:
	حالت ISN_COMPARESPECIAL:
	حالت ISN_COMPARESTRING:
	حالت ISN_CONCAT:
	حالت ISN_CONSTRUCT:
	حالت ISN_COND2BOOL:
	حالت ISN_DEBUG:
	حالت ISN_DEFER:
	حالت ISN_DROP:
	حالت ISN_ECHO:
	حالت ISN_ECHOCONSOLE:
	حالت ISN_ECHOERR:
	حالت ISN_ECHOMSG:
	حالت ISN_ECHOWINDOW:
	حالت ISN_ENDLOOP:
	حالت ISN_ENDTRY:
	حالت ISN_EXECCONCAT:
	حالت ISN_EXECUTE:
	حالت ISN_FINALLY:
	حالت ISN_اختتام:
	حالت ISN_FOR:
	حالت ISN_GETITEM:
	حالت ISN_GET_OBJ_MEMBER:
	حالت ISN_JUMP:
	حالت ISN_JUMP_IF_ARG_NOT_سیٹ:
	حالت ISN_JUMP_IF_ARG_سیٹ:
	حالت ISN_LISTAPPEND:
	حالت ISN_LISTINDEX:
	حالت ISN_LISTSLICE:
	حالت ISN_TUPLEINDEX:
	حالت ISN_TUPLESLICE:
	حالت ISN_بھرو:
	حالت ISN_LOADBDICT:
	حالت ISN_LOADGDICT:
	حالت ISN_LOADOUTER:
	حالت ISN_LOADREG:
	حالت ISN_LOADTDICT:
	حالت ISN_LOADV:
	حالت ISN_LOADWDICT:
	حالت ISN_LOCKCONST:
	حالت ISN_MEMBER:
	حالت ISN_NEGATENR:
	حالت ISN_NEWDICT:
	حالت ISN_NEWLIST:
	حالت ISN_NEWTUPLE:
	حالت ISN_NEWPARTIAL:
	حالت ISN_OPANY:
	حالت ISN_OPFLOAT:
	حالت ISN_OPNR:
	حالت ISN_PCALL:
	حالت ISN_PCALL_انتہا:
	حالت ISN_PROF_انتہا:
	حالت ISN_PROF_آغاز:
	حالت ISN_PUSHBOOL:
	حالت ISN_PUSHCHANNEL:
	حالت ISN_PUSHF:
	حالت ISN_PUSHJOB:
	حالت ISN_PUSHNR:
	حالت ISN_PUSHOBJ:
	حالت ISN_PUSHSPEC:
	حالت ISN_PUT:
	حالت ISN_IPUT:
	حالت ISN_REDIREND:
	حالت ISN_REDIRSTART:
	حالت ISN_RETURN:
	حالت ISN_RETURN_OBJECT:
	حالت ISN_RETURN_VOID:
	حالت ISN_SHUFFLE:
	حالت ISN_قاش:
	حالت ISN_ماخذ:
	حالت ISN_STORE:
	حالت ISN_STORENR:
	حالت ISN_STOREOUTER:
	حالت ISN_STORE_THIS:
	حالت ISN_STORERANGE:
	حالت ISN_STOREREG:
	حالت ISN_STOREV:
	حالت ISN_STRINDEX:
	حالت ISN_STRSLICE:
	حالت ISN_THROW:
	حالت ISN_TRYCONT:
	حالت ISN_UNLETINDEX:
	حالت ISN_UNLETRANGE:
	حالت ISN_UNPACK:
	حالت ISN_USEDICT:
	حالت ISN_WHILE:
	حالت ISN_SCRIPTCTX_سیٹ:
	// nothing allocated
	توڑو;
    }
}

    خالی
clear_instr_ga(garray_T *gap)
{
    صحیح idx;

    برائے (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_ڈیٹا) + idx);
    ga_clear(gap);
}


#اگر_ختم  // defined(FEAT_EVAL)
