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
#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#تعریف_کرو RETURN_NULL_IF_SKIP(cctx) إذا (cctx->ctx_skip == SKIP_YES) أرجع عدم
#تعریف_کرو RETURN_OK_IF_SKIP(cctx) إذا (cctx->ctx_skip == SKIP_YES) أرجع OK

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
    إذا (GA_GROW_FAILS(instr, 1))
	أرجع عدم;
    isn = ((isn_T *)instr->ga_انکڑے) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    أرجع isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, صحيح drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_ڈھیر.ga_len -= drop;
    أرجع generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    ساكن isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    إذا ((isn = generate_instr(cctx, isn_type)) == عدم)
	أرجع عدم;

    إذا (ڈالو_type_stack2(cctx, type == عدم ? &t_any : type,
			       decl_type == عدم ? &t_any : decl_type) == FAIL)
	أرجع عدم;

    أرجع isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    أرجع generate_instr_type2(cctx, isn_type, type, &t_any);
}

/*
 * Generate an ISN_DEBUG instruction.
 */
    isn_T *
generate_instr_debug(cctx_T *cctx)
{
    isn_T	*isn;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_انکڑے)
					       + cctx->ctx_ufunc->uf_dfunc_idx;

    إذا ((isn = generate_instr(cctx, ISN_DEBUG)) == عدم)
	أرجع عدم;
    isn->isn_arg.debug.dbg_var_ناں_len = dfunc->df_var_ناں.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    أرجع isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    صحيح
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == عدم)
	أرجع FAIL;
    isn->isn_arg.construct.construct_ناپ = حجم(object_T)
			       + cl->class_obj_member_گنتی * حجم(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    أرجع OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    صحيح
generate_GET_OBJ_MEMBER(cctx_T *cctx, صحيح idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    إذا (isn == عدم)
	أرجع FAIL;

    isn->isn_arg.classmember.cm_class = عدم;
    isn->isn_arg.classmember.cm_idx = idx;
    أرجع ڈالو_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    صحيح
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, صحيح idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    إذا (isn == عدم)
	أرجع FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    أرجع ڈالو_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    صحيح
generate_STORE_THIS(cctx_T *cctx, صحيح idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    إذا (isn == عدم)
	أرجع FAIL;

    isn->isn_arg.نمبر = idx;
    أرجع OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    صحيح
may_generate_2STRING(صحيح وتھ, صحيح tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_ڈھیر(cctx, -1 - وتھ);

    بدل (type->tt_type)
    {
	// nothing to be done
	حالة VAR_STRING: أرجع OK;

	// conversion possible
	حالة VAR_SPECIAL:
	حالة VAR_BOOL:
	حالة VAR_نمبر:
	حالة VAR_FLOAT:
			 اكسر;

	// conversion possible (with runtime check)
	حالة VAR_ANY:
	حالة VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 اكسر;

	// conversion possible when tolerant
	حالة VAR_LIST:
	حالة VAR_TUPLE:
	حالة VAR_DICT:
			 إذا (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     اكسر;
			 }
			 إذا (tostring_flags & TOSTRING_INTERPOLATE)
			     اكسر;
			 // FALLTHROUGH

	// conversion not possible
	حالة VAR_VOID:
	حالة VAR_BLOB:
	حالة VAR_FUNC:
	حالة VAR_PARTIAL:
	حالة VAR_کم:
	حالة VAR_CHANNEL:
	حالة VAR_INSTR:
	حالة VAR_CLASS:
	حالة VAR_OBJECT:
	حالة VAR_TYPEALIAS:
			 منزل_string_error(type->tt_type);
			 أرجع FAIL;
    }

    سیٹ_type_on_ڈھیر(cctx, &t_string, -1 - وتھ);
    إذا ((isn = generate_instr(cctx, isntype)) == عدم)
	أرجع FAIL;
    isn->isn_arg.tostring.وتھ = وتھ;
    isn->isn_arg.tostring.flags = tostring_flags;

    أرجع OK;
}

    ساكن صحيح
check_نمبر_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    إذا (!((type1 == VAR_نمبر || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_نمبر || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	إذا (check_type_is_قدر(typ1) == FAIL
		|| check_type_is_قدر(typ2) == FAIL)
	    أرجع FAIL;
	إذا (*op == '+')
	    emsg(_(e_wrong_دلیل_type_for_plus));
	وإلا
	    semsg(_(e_char_requires_نمبر_or_float_arguments), *op);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    ساكن صحيح
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    لكل (صحيح i = 0; i < tuple_type->tt_argcount; i++)
    {
	إذا (ga_grow(gap, 1) == FAIL)
	    أرجع FAIL;

	((type_T **)gap->ga_انکڑے)[gap->ga_len] = tuple_type->tt_دلیلاں[i];
	gap->ga_len++;
    }

    أرجع OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ساكن صحيح
سیٹ_tuple_union_type_on_ڈھیر(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, حجم(type_T *), 10);

    إذا (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    إذا (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    إذا ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    سیٹ_type_on_ڈھیر(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    أرجع OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    صحيح
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

    إذا (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_نمبر_or_float(type1, type2, (char_u *)"+") == FAIL)
	أرجع FAIL;

    إذا (isn != عدم)
    {
	إذا (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = اظہاریہ_type;
	وإلا
	    isn->isn_arg.op.op_type = اظہاریہ_جوڑو;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    إذا (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	سیٹ_type_on_ڈھیر(cctx, &t_list_any, 0);
    وإلا إذا (vartype == VAR_TUPLE)
    {
	إذا (!check_tuples_addable(type1, type2))
	    أرجع FAIL;

	إذا (سیٹ_tuple_union_type_on_ڈھیر(type1, type2, cctx) == FAIL)
	    أرجع FAIL;
    }

    أرجع isn == عدم ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    إذا (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_نمبر
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	أرجع type1->tt_type;
    أرجع VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    صحيح
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

    بدل (*op)
    {
	حالة '+':
		  إذا (generate_جوڑو_instr(cctx, vartype, type1, type2,
							    اظہاریہ_COPY) == FAIL)
		      أرجع FAIL;
		  اكسر;

	حالة '-':
	حالة '*':
	حالة '/': إذا (check_نمبر_or_float(type1, type2, op) == FAIL)
		      أرجع FAIL;
		  إذا (vartype == VAR_نمبر)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  وإلا إذا (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  وإلا
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  إذا (isn != عدم)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? اظہاریہ_MULT : *op == '/'? اظہاریہ_DIV : اظہاریہ_SUB;
		  اكسر;

	حالة '%': إذا ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_نمبر)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_نمبر))
		  {
		      emsg(_(e_percent_requires_نمبر_arguments));
		      أرجع FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_نمبر ? ISN_OPNR : ISN_OPANY, 1);
		  إذا (isn != عدم)
		      isn->isn_arg.op.op_type = اظہاریہ_REM;
		  اكسر;
    }

    // correct type of result
    إذا (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	إذا ((type1->tt_type == VAR_نمبر || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_نمبر || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	سیٹ_type_on_ڈھیر(cctx, type, 0);
    }

    أرجع OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    ساكن isntype_T
compare_isn_not_اقدار(typval_T *tv, type_T *type)
{
    إذا (tv != عدم)
	(فراغ)check_typval_is_قدر(tv);
    وإلا
	(فراغ)check_type_is_قدر(type);
    أرجع ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    ساكن isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != عدم ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != عدم ? tv2->v_type : type2->tt_type;

    إذا (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	أرجع compare_isn_not_اقدار(tv1, type1);
    إذا (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	أرجع compare_isn_not_اقدار(tv2, type2);

    إذا (vartype1 == vartype2)
    {
	بدل (vartype1)
	{
	    حالة VAR_BOOL: isntype = ISN_COMPAREBOOL; اكسر;
	    حالة VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; اكسر;
	    حالة VAR_نمبر: isntype = ISN_COMPARENR; اكسر;
	    حالة VAR_FLOAT: isntype = ISN_COMPAREFLOAT; اكسر;
	    حالة VAR_STRING: isntype = ISN_COMPARESTRING; اكسر;
	    حالة VAR_BLOB: isntype = ISN_COMPAREBLOB; اكسر;
	    حالة VAR_LIST: isntype = ISN_COMPARELIST; اكسر;
	    حالة VAR_TUPLE: isntype = ISN_COMPARETUPLE; اكسر;
	    حالة VAR_DICT: isntype = ISN_COMPAREDICT; اكسر;
	    حالة VAR_FUNC: isntype = ISN_COMPAREFUNC; اكسر;
	    حالة VAR_OBJECT: isntype = ISN_COMPAREOBJECT; اكسر;
	    افتراضي: isntype = ISN_COMPAREANY; اكسر;
	}
    }
    وإلا إذا (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_نمبر || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_نمبر || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    وإلا إذا (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	إذا ((vartype1 == VAR_SPECIAL
		&& (tv1 != عدم ? tv1->vval.v_نمبر == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != عدم ? tv2->vval.v_نمبر == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    أرجع ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    إذا ((exprtype == اظہاریہ_IS || exprtype == اظہاریہ_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == اظہاریہ_IS ? "is" : "isnot" , vartype_name(vartype1));
	أرجع ISN_DROP;
    }
    إذا (!(exprtype == اظہاریہ_IS || exprtype == اظہاریہ_ISNOT
		|| exprtype == اظہاریہ_EQUAL || exprtype == اظہاریہ_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	أرجع ISN_DROP;
    }
    إذا (isntype == ISN_DROP
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
	أرجع ISN_DROP;
    }
    أرجع isntype;
}

    صحيح
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    إذا (get_compare_isn(type, tv1, tv2, عدم, عدم) == ISN_DROP)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    صحيح
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, صحيح ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, عدم, عدم,
		       get_type_on_ڈھیر(cctx, 1), get_type_on_ڈھیر(cctx, 0));
    إذا (isntype == ISN_DROP)
	أرجع FAIL;

    إذا ((isn = generate_instr(cctx, isntype)) == عدم)
	أرجع FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --ڈھیر->ga_len;
    سیٹ_type_on_ڈھیر(cctx, &t_bool, 0);

    أرجع OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    صحيح
generate_CONCAT(cctx_T *cctx, صحيح گنتی)
{
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    RETURN_OK_IF_SKIP(cctx);

    إذا ((isn = generate_instr(cctx, ISN_CONCAT)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = گنتی;

    // drop the argument types
    ڈھیر->ga_len -= گنتی - 1;

    أرجع OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    صحيح
generate_2BOOL(cctx_T *cctx, صحيح invert, صحيح وتھ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_2BOOL)) == عدم)
	أرجع FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.وتھ = وتھ;

    // type becomes bool
    سیٹ_type_on_ڈھیر(cctx, &t_bool, -1 - وتھ);

    أرجع OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    صحيح
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    إذا (generate_instr(cctx, ISN_COND2BOOL) == عدم)
	أرجع FAIL;

    // type becomes bool
    سیٹ_type_on_ڈھیر(cctx, &t_bool, 0);

    أرجع OK;
}

    صحيح
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	صحيح	    typechk_flags,	// type check flags
	صحيح	    وتھ,
	صحيح	    is_var,
	صحيح	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == عدم)
	أرجع FAIL;
    type_T *tt;
    إذا ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_نمبر_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	إذا (tt != عدم)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    إذا (typechk_flags & TYPECHK_نمبر_OK)
		tt->tt_flags |= TTFLAG_نمبر_OK;
	    وإلا
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    وإلا
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)وتھ;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    سیٹ_type_on_ڈھیر(cctx, expected, -1 - وتھ);

    أرجع OK;
}

    صحيح
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_SETTYPE)) == عدم)
	أرجع FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    أرجع OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    صحيح
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    إذا (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == عدم)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    ساكن صحيح
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == عدم ? &t_any : &class->class_type);
    إذا (isn == عدم)
	أرجع FAIL;
    isn->isn_arg.classarg = class;
    إذا (class != عدم)
	++class->class_refcount;
    أرجع OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    صحيح
generate_tv_ڈالو(cctx_T *cctx, typval_T *tv)
{
    بدل (tv->v_type)
    {
	حالة VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_نمبر);
	    اكسر;
	حالة VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_نمبر);
	    اكسر;
	حالة VAR_نمبر:
	    generate_PUSHNR(cctx, tv->vval.v_نمبر);
	    اكسر;
	حالة VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    اكسر;
	حالة VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = عدم;
	    اكسر;
	حالة VAR_LIST:
	    إذا (tv->vval.v_list != عدم)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    اكسر;
	حالة VAR_TUPLE:
	    إذا (tv->vval.v_tuple != عدم)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    اكسر;
	حالة VAR_DICT:
	    إذا (tv->vval.v_dict != عدم)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    اكسر;
#جے_تعریف_شدہ FEAT_کم_CHANNEL
	حالة VAR_کم:
	    إذا (tv->vval.v_کم != عدم)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    اكسر;
	حالة VAR_CHANNEL:
	    إذا (tv->vval.v_channel != عدم)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    اكسر;
#جے_انت
	حالة VAR_FUNC:
	    إذا (tv->vval.v_string != عدم)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, عدم, &t_func_unknown, TRUE);
	    اكسر;
	حالة VAR_PARTIAL:
	    إذا (tv->vval.v_partial != عدم)
		iemsg("non-null partial constant not supported");
	    إذا (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == عدم)
		أرجع FAIL;
	    اكسر;
	حالة VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = عدم;
	    اكسر;
	حالة VAR_OBJECT:
	    إذا (tv->vval.v_object != عدم)
	    {
		emsg(_(e_cannot_use_non_null_object));
		أرجع FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    اكسر;
	حالة VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    اكسر;
	افتراضي:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    أرجع FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    أرجع OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    صحيح
generate_PUSHNR(cctx_T *cctx, varnumber_T نمبر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_نمبر)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = نمبر;

    إذا (نمبر == 0 || نمبر == 1)
	// A 0 or 1 number can also be used as a bool.
	سیٹ_type_on_ڈھیر(cctx, &t_نمبر_bool, 0);
    أرجع OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    صحيح
generate_PUSHBOOL(cctx_T *cctx, varnumber_T نمبر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = نمبر;

    أرجع OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    صحيح
generate_PUSHSPEC(cctx_T *cctx, varnumber_T نمبر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     نمبر == VVAL_NULL ? &t_null : &t_none)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = نمبر;

    أرجع OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    صحيح
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == عدم)
	أرجع FAIL;
    isn->isn_arg.fnumber = fnumber;

    أرجع OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    صحيح
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    صحيح		ret = OK;

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	إذا ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == عدم)
	    ret = FAIL;
	وإلا
	{
	    isn->isn_arg.string = str == عدم ? عدم : *str;
	    أرجع OK;
	}
    }
    إذا (str != عدم)
	VIM_CLEAR(*str);
    أرجع ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    صحيح
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    إذا (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == عدم)
	أرجع FAIL;
    أرجع OK;
#وإلا
    emsg(_(e_channel_کم_feature_not_دستیاب));
    أرجع FAIL;
#جے_انت
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    صحيح
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    إذا (generate_instr_type(cctx, ISN_PUSHJOB, &t_کم) == عدم)
	أرجع FAIL;
    أرجع OK;
#وإلا
    emsg(_(e_channel_کم_feature_not_دستیاب));
    أرجع FAIL;
#جے_انت
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    صحيح
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == عدم)
	أرجع FAIL;
    isn->isn_arg.blob = blob;

    أرجع OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    صحيح
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, صحيح may_اگیتر)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == عدم)
	أرجع FAIL;
    إذا (name == عدم)
	funcname = عدم;
    وإلا إذا (!may_اگیتر
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != عدم)	    // autoload
	funcname = vim_strsave(name);
    وإلا
    {
	funcname = alloc(STRLEN(name) + 3);
	إذا (funcname != عدم)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    أرجع OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    صحيح
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == عدم)
	أرجع FAIL;
    isn->isn_arg.string = vim_strsave(name);
    إذا (isn->isn_arg.string == عدم)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    صحيح
generate_GETITEM(cctx_T *cctx, صحيح اشاریہ, صحيح with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_ڈھیر(cctx, with_op ? 1 : 0);
    type_T	*شے_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    شے_type = get_شے_type(type);
    إذا ((isn = generate_instr(cctx, ISN_GETITEM)) == عدم)
	أرجع FAIL;
    isn->isn_arg.getitem.gi_اشاریہ = اشاریہ;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    أرجع ڈالو_type_ڈھیر(cctx, شے_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    ساكن فراغ
سیٹ_tuple_ٹکڑا_type_on_ڈھیر(type_T *type, صحيح گنتی, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    إذا ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= گنتی)
    {
	سیٹ_type_on_ڈھیر(cctx, &t_tuple_any, 0);
	أرجع;
    }

    ga_init2(&tuple_types_ga, حجم(type_T *), 10);

    لكل (صحيح i = گنتی; i < type->tt_argcount; i++)
    {
	إذا (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    سیٹ_type_on_ڈھیر(cctx, &t_tuple_any, 0);
	    أرجع;
	}
	((type_T **)tuple_types_ga.ga_انکڑے)[tuple_types_ga.ga_len] =
							     type->tt_دلیلاں[i];
	tuple_types_ga.ga_len++;
    }

    سیٹ_type_on_ڈھیر(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    صحيح
generate_ٹکڑا(cctx_T *cctx, صحيح گنتی)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_ٹکڑا)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = گنتی;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_ڈھیر(cctx, 0);
    إذا (type->tt_type == VAR_TUPLE)
	سیٹ_tuple_ٹکڑا_type_on_ڈھیر(type, گنتی, cctx);

    أرجع OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    صحيح
generate_CHECKLEN(cctx_T *cctx, صحيح min_len, صحيح ہور_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    إذا ((isn = generate_instr(cctx, ISN_CHECKLEN)) == عدم)
	أرجع FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_ہور_OK = ہور_OK;

    أرجع OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    صحيح
generate_STORE(cctx_T *cctx, isntype_T isn_type, صحيح idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_drop(cctx, isn_type, 1)) == عدم)
	أرجع FAIL;
    إذا (name != عدم)
	isn->isn_arg.string = vim_strsave(name);
    وإلا
	isn->isn_arg.نمبر = idx;

    أرجع OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    صحيح
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	صحيح	    لدو,
	class_T	    *cl,
	صحيح	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا (لدو)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_لدو_CLASSMEMBER, m->ocm_type);
    }
    وإلا
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    إذا (isn == عدم)
	أرجع FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    أرجع OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    ساكن صحيح
generate_STOREOUTER(
	cctx_T	    *cctx,
	صحيح	    idx,
	صحيح	    level,
	صحيح	    loop_ڈونگھائی,
	صحيح	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == عدم)
	أرجع FAIL;
    إذا (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_ڈونگھائی = -loop_ڈونگھائی - 1;
    }
    وإلا
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_ڈونگھائی = level;
    }

    أرجع OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    صحيح
generate_STORENR(cctx_T *cctx, صحيح idx, varnumber_T قدر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_STORENR)) == عدم)
	أرجع FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = قدر;

    أرجع OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    ساكن صحيح
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	صحيح	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_drop(cctx, isn_type, 1)) == عدم)
	أرجع FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    أرجع OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    صحيح
generate_لدو(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	صحيح	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_type2(cctx, isn_type, type, type)) == عدم)
	أرجع FAIL;
    إذا (name != عدم)
	isn->isn_arg.string = vim_strsave(name);
    وإلا
	isn->isn_arg.نمبر = idx;

    أرجع OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    صحيح
generate_LOADOUTER(
	cctx_T	    *cctx,
	صحيح	    idx,
	صحيح	    nesting,
	صحيح	    loop_ڈونگھائی,
	صحيح	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == عدم)
	أرجع FAIL;
    إذا (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_ڈونگھائی = -loop_ڈونگھائی - 1;
    }
    وإلا
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_ڈونگھائی = nesting;
    }

    أرجع OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    صحيح
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    صحيح	    di_flags;
    صحيح	    vidx = لبھو_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    إذا (vidx < 0)
    {
	semsg(_(e_variable_not_لبھیا_str), name);
	أرجع FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    أرجع generate_لدو(cctx, ISN_LOADV, vidx, عدم, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    صحيح
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, صحيح forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, isn_type)) == عدم)
	أرجع FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    أرجع OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    صحيح
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    إذا (generate_instr(cctx, ISN_LOCKCONST) == عدم)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    صحيح
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	صحيح	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    وإلا
	isn = generate_instr_drop(cctx, isn_type, 1);
    إذا (isn == عدم)
	أرجع FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    أرجع OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    صحيح
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	صحيح	    sid,
	صحيح	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_شے(sid);

    RETURN_OK_IF_SKIP(cctx);
    إذا (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    وإلا
	isn = generate_instr_drop(cctx, isn_type, 1);
    إذا (isn == عدم)
	أرجع FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    إذا (sref == عدم)
	أرجع FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    أرجع OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    صحيح
generate_NEWLIST(cctx_T *cctx, صحيح گنتی, صحيح use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_NEWLIST)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = use_null ? -1 : گنتی;

    // Get the member type and the declared member type from all the items on
    // the stack.
    إذا ((member_type = get_member_type_from_ڈھیر(گنتی, 1, cctx)) == عدم)
	أرجع FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_ڈھیر.ga_len -= گنتی;

    // add the list type to the type stack
    أرجع ڈالو_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    صحيح
generate_NEWTUPLE(cctx_T *cctx, صحيح گنتی, صحيح use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = use_null ? -1 : گنتی;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, حجم(type_T *), 10);

    إذا (get_tuple_type_from_ڈھیر(گنتی, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	أرجع FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_ڈھیر.ga_len -= گنتی;

    // add the tuple type to the type stack
    أرجع ڈالو_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    صحيح
generate_NEWDICT(cctx_T *cctx, صحيح گنتی, صحيح use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_NEWDICT)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = use_null ? -1 : گنتی;

    إذا ((member_type = get_member_type_from_ڈھیر(گنتی, 2, cctx)) == عدم)
	أرجع FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_ڈھیر.ga_len -= 2 * گنتی;

    // add the dict type to the type stack
    أرجع ڈالو_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    صحيح
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	صحيح	    object_طریقہ,
	صحيح	    fi,
	صحيح	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    صحيح		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_FUNCREF)) == عدم)
	أرجع FAIL;
    إذا (isn_idx != عدم)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    إذا (ufunc->uf_def_حالت == UF_NOT_COMPILED || has_vars || cl != عدم)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	إذا (extra == عدم)
	    أرجع FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	إذا (cl != عدم)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_طریقہ = object_طریقہ;
	    extra->fre_طریقہ_idx = fi;
	}
    }
    إذا (ufunc->uf_def_حالت == UF_NOT_COMPILED || cl != عدم)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    إذا (ufunc->uf_def_حالت != UF_NOT_COMPILED && cl == عدم)
    {
	إذا (isn_idx == عدم && ufunc->uf_def_حالت == UF_منزل_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (فراغ)compile_def_function(ufunc, FALSE, CT_NONE, عدم);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_گھیرا = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    إذا ((ufunc->uf_flags & FC_گھیرا)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_گھیرا;

    type = ufunc->uf_func_type == عدم ? &t_func_any : ufunc->uf_func_type;
    أرجع ڈالو_type_ڈھیر(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    صحيح
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    صحيح		ret = OK;

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	إذا ((isn = generate_instr(cctx, ISN_NEWFUNC)) == عدم)
	    ret = FAIL;
	وإلا
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    إذا (arg == عدم)
		ret = FAIL;
	    وإلا
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_گھیرا = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(فراغ)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		أرجع OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    أرجع ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    صحيح
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_DEF)) == عدم)
	أرجع FAIL;
    إذا (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	إذا (isn->isn_arg.string == عدم)
	    أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    صحيح
generate_JUMP(cctx_T *cctx, jumpwhen_T when, صحيح where)
{
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_JUMP)) == عدم)
	أرجع FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    إذا (when != JUMP_ALWAYS && ڈھیر->ga_len > 0)
	--ڈھیر->ga_len;

    أرجع OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    صحيح
generate_WHILE(cctx_T *cctx, صحيح funcref_idx)
{
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_WHILE)) == عدم)
	أرجع FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_سرا = 0;  // filled in later

    إذا (ڈھیر->ga_len > 0)
	--ڈھیر->ga_len;

    أرجع OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    صحيح
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, صحيح arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, isn_type)) == عدم)
	أرجع FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    أرجع OK;
}

    صحيح
generate_FOR(cctx_T *cctx, صحيح loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_FOR)) == عدم)
	أرجع FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    أرجع ڈالو_type_ڈھیر(cctx, &t_any);
}

    صحيح
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_ENDLOOP)) == عدم)
	أرجع FAIL;
    isn->isn_arg.endloop.سرا_ڈونگھائی = loop_info->li_ڈونگھائی;
    isn->isn_arg.endloop.سرا_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.سرا_var_idx = loop_info->li_local_گنتی;
    isn->isn_arg.endloop.سرا_var_گنتی =
			   cctx->ctx_locals.ga_len - loop_info->li_local_گنتی;
    أرجع OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    صحيح
generate_TRYCONT(cctx_T *cctx, صحيح پدھراں, صحيح where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_TRYCONT)) == عدم)
	أرجع FAIL;
    isn->isn_arg.trycont.tct_پدھراں = پدھراں;
    isn->isn_arg.trycont.tct_where = where;

    أرجع OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    صحيح
check_اندرونی_func_دلیلاں(
	cctx_T	*cctx,
	صحيح	func_idx,
	صحيح	argcount,
	صحيح	طریقہ_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
    صحيح		argoff = check_اندرونی_func(func_idx, argcount);

    إذا (argoff < 0)
	أرجع FAIL;

    إذا (طریقہ_call && argoff > 1)
    {
	إذا (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 اندرونی_func_name(func_idx));
	    أرجع FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	إذا (isn  == عدم)
	    أرجع FAIL;
	isn->isn_arg.shuffle.shfl_شے = argcount;
	isn->isn_arg.shuffle.shfl_اتے = argoff - 1;
    }

    إذا (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)ڈھیر->ga_انکڑے) + ڈھیر->ga_len - argcount;

	// Check the types of the arguments.
	إذا (طریقہ_call && argoff > 1)
	{
	    صحيح i;

	    لكل (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	وإلا
	{
	    صحيح i;

	    لكل (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	إذا (اندرونی_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    صحيح
generate_BCALL(cctx_T *cctx, صحيح func_idx, صحيح argcount, صحيح طریقہ_call)
{
    isn_T	*isn;
    garray_T	*ڈھیر = &cctx->ctx_type_ڈھیر;
    type2_T	*argtypes = عدم;
    type2_T	shuffled_argtypes[MAX_FUNC_دلیلاں];
    type2_T	*maptype = عدم;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    إذا (check_اندرونی_func_دلیلاں(cctx, func_idx, argcount, طریقہ_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	أرجع FAIL;

    إذا (اندرونی_func_is_map(func_idx))
	maptype = argtypes;

    إذا ((isn = generate_instr(cctx, ISN_BCALL)) == عدم)
	أرجع FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    ڈھیر->ga_len -= argcount;
    type = اندرونی_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    إذا (ڈالو_type_stack2(cctx, type, decl_type) == FAIL)
	أرجع FAIL;

    إذا (maptype != عدم && maptype[0].type_decl->tt_member != عدم
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    أرجع OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    صحيح
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*شے_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_ڈھیر(cctx, 1);
    إذا (arg_type_modifiable(list_type, 1) == FAIL)
	أرجع FAIL;
    شے_type = get_type_on_ڈھیر(cctx, 0);
    expected = list_type->tt_member;
    إذا (need_type(شے_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	أرجع FAIL;

    إذا (generate_instr(cctx, ISN_LISTAPPEND) == عدم)
	أرجع FAIL;

    --cctx->ctx_type_ڈھیر.ga_len;	    // drop the argument
    أرجع OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    صحيح
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*شے_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    إذا (arg_type_modifiable(get_decl_type_on_ڈھیر(cctx, 1), 1) == FAIL)
	أرجع FAIL;
    شے_type = get_type_on_ڈھیر(cctx, 0);
    إذا (need_type(شے_type, &t_نمبر, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	أرجع FAIL;

    إذا (generate_instr(cctx, ISN_BLOBAPPEND) == عدم)
	أرجع FAIL;

    --cctx->ctx_type_ڈھیر.ga_len;	    // drop the argument
    أرجع OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    ساكن isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    إذا (cl != عدم)
	أرجع ISN_METHODCALL;

    إذا (ufunc->uf_def_حالت == UF_NOT_COMPILED)
	أرجع ISN_UCALL;

    // function invoked from a lambda
    إذا (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	إذا (ufunc->uf_name[0] == K_SPECIAL)
	    أرجع ISN_UCALL;
    }

    أرجع ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    صحيح
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	صحيح	    mi,
	صحيح	    pushed_argcount,
	صحيح	    is_super)
{
    isn_T	*isn;
    صحيح		regular_دلیلاں = ufunc->uf_دلیلاں.ga_len;
    صحيح		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    إذا (argcount > regular_دلیلاں && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	أرجع FAIL;
    }
    إذا (argcount < regular_دلیلاں - ufunc->uf_def_دلیلاں.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	أرجع FAIL;
    }

    إذا (ufunc->uf_def_حالت != UF_NOT_COMPILED
	    && ufunc->uf_def_حالت != UF_COMPILE_ERROR)
    {
	صحيح		i;
	compiletype_T	compile_type;

	لكل (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_ڈھیر(cctx, argcount - i - 1);
	    إذا (check_type_is_قدر(actual) == FAIL)
		أرجع FAIL;
	    إذا (actual->tt_type == VAR_SPECIAL
			      && i >= regular_دلیلاں - ufunc->uf_def_دلیلاں.ga_len)
	    {
		// assume v:none used for default argument value
		استمر;
	    }
	    إذا (i < regular_دلیلاں)
	    {
		إذا (ufunc->uf_arg_types == عدم)
		    استمر;
		expected = ufunc->uf_arg_types[i];
	    }
	    وإلا إذا (ufunc->uf_va_type == عدم
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    وإلا
		expected = ufunc->uf_va_type->tt_member;
	    إذا (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		أرجع FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	إذا (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == عدم,
						   compile_type, عدم) == FAIL)
	    أرجع FAIL;
    }
    إذا (ufunc->uf_def_حالت == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_منزل_function_that_failed_منزل_compile_str,
							       ufunc->uf_name);
	أرجع FAIL;
    }

    إذا ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == عدم)
	أرجع FAIL;
    إذا (cl != عدم /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	إذا (isn->isn_arg.mfunc == عدم)
	    أرجع FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = عدم;
	إذا (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = عدم;
	isn->isn_arg.mfunc->cmf_cache_generic = عدم;
	isn->isn_arg.mfunc->cmf_cache_ufunc = عدم;
    }
    وإلا إذا (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    وإلا
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_ڈھیر.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    إذا (ufunc->uf_class != عدم)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_ڈھیر(cctx, 0);
	إذا (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_ڈھیر.ga_len--;
    }

    // add return type
    أرجع ڈالو_type_ڈھیر(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    صحيح
generate_UCALL(cctx_T *cctx, char_u *name, صحيح argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_UCALL)) == عدم)
	أرجع FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_ڈھیر.ga_len -= argcount;

    // add return value
    أرجع ڈالو_type_ڈھیر(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    صحيح
check_func_دلیلاں_from_type(
	cctx_T	*cctx,
	type_T	*type,
	صحيح	argcount,
	صحيح	at_چوٹی,
	char_u	*name)
{
    إذا (type->tt_argcount != -1)
    {
	صحيح	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	إذا (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    أرجع FAIL;
	}
	إذا (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    أرجع FAIL;
	}
	إذا (type->tt_دلیلاں != عدم)
	{
	    صحيح i;

	    لكل (i = 0; i < argcount; ++i)
	    {
		صحيح	وتھ = -argcount + i - (at_چوٹی ? 0 : 1);
		type_T	*actual = get_type_on_ڈھیر(cctx, -1 - وتھ);
		type_T	*expected;

		إذا (check_type_is_قدر(actual) == FAIL)
		    أرجع FAIL;
		إذا (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_دلیلاں[type->tt_argcount - 1];
		    إذا (expected != عدم && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    إذا (expected == عدم)
			expected = &t_any;
		}
		وإلا إذا (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		وإلا
		    expected = type->tt_دلیلاں[i];
		إذا (need_type(actual, expected, 0,
				     وتھ, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    أرجع FAIL;
		}
	    }
	}
    }

    أرجع OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    صحيح
generate_PCALL(
	cctx_T	*cctx,
	صحيح	argcount,
	char_u	*name,
	type_T	*type,
	صحيح	at_چوٹی)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    إذا (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    وإلا إذا (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	إذا (check_func_دلیلاں_from_type(cctx, type, argcount, at_چوٹی, name)
								       == FAIL)
	    أرجع FAIL;

	ret_type = type->tt_member;
	إذا (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    وإلا
    {
	semsg(_(e_not_callable_type_str), name);
	أرجع FAIL;
    }

    إذا ((isn = generate_instr(cctx, ISN_PCALL)) == عدم)
	أرجع FAIL;
    isn->isn_arg.pfunc.cpf_چوٹی = at_چوٹی;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_ڈھیر.ga_len -= argcount + 1;

    // push the return value
    ڈالو_type_ڈھیر(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    إذا (at_چوٹی && generate_instr(cctx, ISN_PCALL_سرا) == عدم)
	أرجع FAIL;

    أرجع OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    صحيح
generate_DEFER(cctx_T *cctx, صحيح var_idx, صحيح argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == عدم)
	أرجع FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    أرجع OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    صحيح
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == عدم)
	أرجع FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_ڈھیر(cctx, 0);
    إذا (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	حرف *tofree;

	semsg(_(e_expected_dictionary_for_using_کلید_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	أرجع FAIL;
    }
    // change dict type to dict member type
    إذا (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	سیٹ_type_on_ڈھیر(cctx, ntype, 0);
    }

    أرجع OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    صحيح
generate_ECHO(cctx_T *cctx, صحيح with_white, صحيح گنتی)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_drop(cctx, ISN_ECHO, گنتی)) == عدم)
	أرجع FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_گنتی = گنتی;

    أرجع OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    صحيح
generate_MULT_اظہاریہ(cctx_T *cctx, isntype_T isn_type, صحيح گنتی)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr_drop(cctx, isn_type, گنتی)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = گنتی;
    أرجع OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    صحيح
generate_ECHOWINDOW(cctx_T *cctx, صحيح گنتی, لما ویلا)
{
    isn_T	*isn;

    إذا ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, گنتی)) == عدم)
	أرجع FAIL;
    isn->isn_arg.echowin.ewin_گنتی = گنتی;
    isn->isn_arg.echowin.ewin_ویلا = ویلا;
    أرجع OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    صحيح
generate_سروت(cctx_T *cctx, صحيح sid)
{
    isn_T	*isn;

    إذا ((isn = generate_instr(cctx, ISN_سروت)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = sid;

    أرجع OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    صحيح
generate_PUT(cctx_T *cctx, صحيح regname, linenr_T lnum, صحيح fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    إذا (isn == عدم)
	أرجع FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    أرجع OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    صحيح
generate_LOCKUNLOCK(cctx_T *cctx, char_u *سطر, صحيح is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == عدم)
	أرجع FAIL;
    class_T *cl = cctx->ctx_ufunc != عدم ? cctx->ctx_ufunc->uf_class : عدم;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(سطر);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    إذا (cl != عدم)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    أرجع OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    صحيح
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *سطر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, isntype)) == عدم)
	أرجع FAIL;
    isn->isn_arg.string = vim_strsave(سطر);
    أرجع OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    صحيح
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    صحيح		ret = OK;

    إذا (cctx->ctx_skip != SKIP_YES)
    {
	إذا ((isn = generate_instr(cctx, isntype)) == عدم)
	    ret = FAIL;
	وإلا
	{
	    isn->isn_arg.string = str;
	    أرجع OK;
	}
    }
    vim_free(str);
    أرجع ret;
}

    صحيح
generate_LEGACY_EVAL(cctx_T *cctx, char_u *سطر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == عدم)
	أرجع FAIL;
    isn->isn_arg.string = vim_strsave(سطر);

    أرجع ڈالو_type_ڈھیر(cctx, &t_any);
}

    صحيح
generate_EXECCONCAT(cctx_T *cctx, صحيح گنتی)
{
    isn_T	*isn;

    إذا ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, گنتی)) == عدم)
	أرجع FAIL;
    isn->isn_arg.نمبر = گنتی;
    أرجع OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    صحيح
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    إذا ((isn = generate_instr(cctx, ISN_RANGE)) == عدم)
	أرجع FAIL;
    isn->isn_arg.string = range;

    أرجع ڈالو_type_ڈھیر(cctx, &t_نمبر);
}

    صحيح
generate_UNPACK(cctx_T *cctx, صحيح var_گنتی, صحيح semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_UNPACK)) == عدم)
	أرجع FAIL;
    isn->isn_arg.unpack.unp_گنتی = var_گنتی;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    أرجع OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    صحيح
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    إذا (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	إذا ((isn = generate_instr(cctx, ISN_CMDMOD)) == عدم)
	    أرجع FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	إذا (isn->isn_arg.cmdmod.cf_cmdmod == عدم)
	    أرجع FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, حجم(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = عدم;
    }

    أرجع OK;
}

    صحيح
generate_undo_cmdmods(cctx_T *cctx)
{
    إذا (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == عدم)
	أرجع FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    أرجع OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    صحيح
generate_store_var(
	cctx_T		*cctx,
	سونپو_dest_T	dest,
	صحيح		opt_flags,
	صحيح		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    بدل (dest)
    {
	حالة dest_option:
	    أرجع generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	حالة dest_func_option:
	    أرجع generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	حالة dest_global:
	    // include g: with the name, easier to execute that way
	    أرجع generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == عدم
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	حالة dest_بفر:
	    // include b: with the name, easier to execute that way
	    أرجع generate_STORE(cctx, ISN_STOREB, 0, name);
	حالة dest_کھڑکی:
	    // include w: with the name, easier to execute that way
	    أرجع generate_STORE(cctx, ISN_STOREW, 0, name);
	حالة dest_tab:
	    // include t: with the name, easier to execute that way
	    أرجع generate_STORE(cctx, ISN_STORET, 0, name);
	حالة dest_env:
	    أرجع generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	حالة dest_reg:
	    أرجع generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], عدم);
	حالة dest_vimvar:
	    أرجع generate_STORE(cctx, ISN_STOREV, vimvaridx, عدم);
	حالة dest_script:
	حالة dest_script_v9:
	    {
		صحيح	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		صحيح	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		إذا (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    إذا (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_شے(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_شے(scriptvar_sid)
						  ->sn_autoload_اگیتر == عدم)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			//پہلا
			إذا (generate_سروت(cctx, scriptvar_sid) == FAIL)
			    أرجع FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    أرجع generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		أرجع generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	حالة dest_class_member:
	    أرجع generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	حالة dest_local:
	حالة dest_اظہاریہ:
	    // cannot happen
	    اكسر;
    }
    أرجع FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    صحيح
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    لكل (;;)
    {
	إذا (scope == عدم)
	    اكسر;
	إذا (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    أرجع TRUE;
	scope = scope->se_outer;
    }
    أرجع FALSE;
}

    صحيح
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, صحيح instr_گنتی, صحيح is_decl)
{
    إذا (lhs->lhs_dest != dest_local)
	أرجع generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    إذا (lhs->lhs_lvar == عدم)
	أرجع OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_انکڑے) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    إذا (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_گنتی + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.نمبر;
	garray_T    *ڈھیر = &cctx->ctx_type_ڈھیر;

	إذا (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	وإلا
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	إذا (ڈھیر->ga_len > 0)
	    --ڈھیر->ga_len;
    }
    وإلا إذا (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_ڈونگھائی,
		lhs->lhs_lvar->lv_loop_idx);
    وإلا
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, عدم);
    أرجع OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    صحيح
generate_SCRIPTCTX_سیٹ(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    إذا ((isn = generate_instr(cctx, ISN_SCRIPTCTX_سیٹ)) == عدم)
	أرجع FAIL;
    isn->isn_arg.setsctx = new_sctx;
    أرجع OK;
}

#إذا defined(FEAT_PROFILE)
    فراغ
may_generate_prof_سرا(cctx_T *cctx, صحيح prof_lnum)
{
    إذا (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_سرا);
}
#جے_انت


/*
 * Delete an instruction, free what it contains.
 */
    فراغ
delete_instr(isn_T *isn)
{
    بدل (isn->isn_type)
    {
	حالة ISN_AUTOLOAD:
	حالة ISN_DEF:
	حالة ISN_EXEC:
	حالة ISN_EXECRANGE:
	حالة ISN_EXEC_ونڈو:
	حالة ISN_LEGACY_EVAL:
	حالة ISN_LOADAUTO:
	حالة ISN_LOADB:
	حالة ISN_LOADENV:
	حالة ISN_LOADG:
	حالة ISN_LOADOPT:
	حالة ISN_LOADT:
	حالة ISN_LOADW:
	حالة ISN_PUSHEXC:
	حالة ISN_PUSHFUNC:
	حالة ISN_PUSHS:
	حالة ISN_RANGE:
	حالة ISN_STOREAUTO:
	حالة ISN_STOREB:
	حالة ISN_STOREENV:
	حالة ISN_STOREG:
	حالة ISN_STORET:
	حالة ISN_STOREW:
	حالة ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    اكسر;

	حالة ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    اكسر;

	حالة ISN_SUBSTITUTE:
	    {
		صحيح	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		لكل (idx = 0; list[idx].isn_type != ISN_انت; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    اكسر;

	حالة ISN_INSTR:
	    {
		صحيح	idx;
		isn_T	*list = isn->isn_arg.instr;

		لكل (idx = 0; list[idx].isn_type != ISN_انت; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    اكسر;

	حالة ISN_LOADS:
	حالة ISN_LOADEXPORT:
	حالة ISN_STORES:
	حالة ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    اكسر;

	حالة ISN_UNLET:
	حالة ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    اكسر;

	حالة ISN_STOREOPT:
	حالة ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    اكسر;

	حالة ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    اكسر;

	حالة ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    اكسر;

	حالة ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    اكسر;

	حالة ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		إذا (extra == عدم || extra->fre_func_name == عدم)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_انکڑے)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    إذا (ufunc != عدم && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		إذا (extra != عدم)
		{
		    char_u *name = extra->fre_func_name;

		    إذا (name != عدم)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    إذا (extra->fre_class != عدم)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    اكسر;

	حالة ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_انکڑے)
		    + isn->isn_arg.dfunc.cdf_idx;

		إذا (dfunc->df_ufunc != عدم
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    اكسر;

	حالة ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    اكسر;

	حالة ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		إذا (arg != عدم)
		{
		    ufunc_T *ufunc = لبھو_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    إذا (ufunc != عدم)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    اكسر;

	حالة ISN_CHECKTYPE:
	حالة ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    اكسر;

	حالة ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    اكسر;

	حالة ISN_LOADSCRIPT:
	حالة ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    اكسر;

	حالة ISN_لدو_CLASSMEMBER:
	حالة ISN_STORE_CLASSMEMBER:
	حالة ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    اكسر;

	حالة ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    اكسر;

	حالة ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    اكسر;

	حالة ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    اكسر;

	حالة ISN_2BOOL:
	حالة ISN_2STRING:
	حالة ISN_2STRING_ANY:
	حالة ISN_ADDBLOB:
	حالة ISN_ADDLIST:
	حالة ISN_ADDTUPLE:
	حالة ISN_ANYINDEX:
	حالة ISN_ANYSLICE:
	حالة ISN_BCALL:
	حالة ISN_BLOBAPPEND:
	حالة ISN_BLOBINDEX:
	حالة ISN_BLOBSLICE:
	حالة ISN_CATCH:
	حالة ISN_CEXPR_AUCMD:
	حالة ISN_CHECKLEN:
	حالة ISN_CLEARDICT:
	حالة ISN_CMDMOD_REV:
	حالة ISN_COMPAREANY:
	حالة ISN_COMPAREBLOB:
	حالة ISN_COMPAREBOOL:
	حالة ISN_COMPAREDICT:
	حالة ISN_COMPAREFLOAT:
	حالة ISN_COMPAREFUNC:
	حالة ISN_COMPARELIST:
	حالة ISN_COMPARETUPLE:
	حالة ISN_COMPARENR:
	حالة ISN_COMPARENULL:
	حالة ISN_COMPAREOBJECT:
	حالة ISN_COMPARESPECIAL:
	حالة ISN_COMPARESTRING:
	حالة ISN_CONCAT:
	حالة ISN_CONSTRUCT:
	حالة ISN_COND2BOOL:
	حالة ISN_DEBUG:
	حالة ISN_DEFER:
	حالة ISN_DROP:
	حالة ISN_ECHO:
	حالة ISN_ECHOCONSOLE:
	حالة ISN_ECHOERR:
	حالة ISN_ECHOMSG:
	حالة ISN_ECHOWINDOW:
	حالة ISN_ENDLOOP:
	حالة ISN_ENDTRY:
	حالة ISN_EXECCONCAT:
	حالة ISN_EXECUTE:
	حالة ISN_FINALLY:
	حالة ISN_انت:
	حالة ISN_FOR:
	حالة ISN_GETITEM:
	حالة ISN_GET_OBJ_MEMBER:
	حالة ISN_JUMP:
	حالة ISN_JUMP_IF_ARG_NOT_سیٹ:
	حالة ISN_JUMP_IF_ARG_سیٹ:
	حالة ISN_LISTAPPEND:
	حالة ISN_LISTINDEX:
	حالة ISN_LISTSLICE:
	حالة ISN_TUPLEINDEX:
	حالة ISN_TUPLESLICE:
	حالة ISN_لدو:
	حالة ISN_LOADBDICT:
	حالة ISN_LOADGDICT:
	حالة ISN_LOADOUTER:
	حالة ISN_LOADREG:
	حالة ISN_LOADTDICT:
	حالة ISN_LOADV:
	حالة ISN_LOADWDICT:
	حالة ISN_LOCKCONST:
	حالة ISN_MEMBER:
	حالة ISN_NEGATENR:
	حالة ISN_NEWDICT:
	حالة ISN_NEWLIST:
	حالة ISN_NEWTUPLE:
	حالة ISN_NEWPARTIAL:
	حالة ISN_OPANY:
	حالة ISN_OPFLOAT:
	حالة ISN_OPNR:
	حالة ISN_PCALL:
	حالة ISN_PCALL_سرا:
	حالة ISN_PROF_سرا:
	حالة ISN_PROF_شروع:
	حالة ISN_PUSHBOOL:
	حالة ISN_PUSHCHANNEL:
	حالة ISN_PUSHF:
	حالة ISN_PUSHJOB:
	حالة ISN_PUSHNR:
	حالة ISN_PUSHOBJ:
	حالة ISN_PUSHSPEC:
	حالة ISN_PUT:
	حالة ISN_IPUT:
	حالة ISN_REDIREND:
	حالة ISN_REDIRSTART:
	حالة ISN_RETURN:
	حالة ISN_RETURN_OBJECT:
	حالة ISN_RETURN_VOID:
	حالة ISN_SHUFFLE:
	حالة ISN_ٹکڑا:
	حالة ISN_سروت:
	حالة ISN_STORE:
	حالة ISN_STORENR:
	حالة ISN_STOREOUTER:
	حالة ISN_STORE_THIS:
	حالة ISN_STORERANGE:
	حالة ISN_STOREREG:
	حالة ISN_STOREV:
	حالة ISN_STRINDEX:
	حالة ISN_STRSLICE:
	حالة ISN_THROW:
	حالة ISN_TRYCONT:
	حالة ISN_UNLETINDEX:
	حالة ISN_UNLETRANGE:
	حالة ISN_UNPACK:
	حالة ISN_USEDICT:
	حالة ISN_WHILE:
	حالة ISN_SCRIPTCTX_سیٹ:
	// nothing allocated
	اكسر;
    }
}

    فراغ
clear_instr_ga(garray_T *gap)
{
    صحيح idx;

    لكل (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_انکڑے) + idx);
    ga_clear(gap);
}


#جے_انت  // defined(FEAT_EVAL)
