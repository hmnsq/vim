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

#define USING_FLOAT_STUFF
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) اگر (cctx->ctx_skip == SKIP_YES) بازگشت پوچ
#define RETURN_OK_IF_SKIP(cctx) اگر (cctx->ctx_skip == SKIP_YES) بازگشت OK

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
	بازگشت پوچ;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    بازگشت isn;
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
    cctx->ctx_type_پشته.ga_len -= drop;
    بازگشت generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    ایستا isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    اگر ((isn = generate_instr(cctx, isn_type)) == پوچ)
	بازگشت پوچ;

    اگر (افزودن_type_stack2(cctx, type == پوچ ? &t_any : type,
			       decl_type == پوچ ? &t_any : decl_type) == FAIL)
	بازگشت پوچ;

    بازگشت isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    بازگشت generate_instr_type2(cctx, isn_type, type, &t_any);
}

/*
 * Generate an ISN_DEBUG instruction.
 */
    isn_T *
generate_instr_debug(cctx_T *cctx)
{
    isn_T	*isn;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
					       + cctx->ctx_ufunc->uf_dfunc_idx;

    اگر ((isn = generate_instr(cctx, ISN_DEBUG)) == پوچ)
	بازگشت پوچ;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    بازگشت isn;
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
    اگر ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.construct.construct_size = اندازه(object_T)
			       + cl->class_obj_member_شمار * اندازه(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    بازگشت OK;
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
    اگر (isn == پوچ)
	بازگشت FAIL;

    isn->isn_arg.classmember.cm_class = پوچ;
    isn->isn_arg.classmember.cm_idx = idx;
    بازگشت افزودن_type_stack2(cctx, type, &t_any);
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
    اگر (isn == پوچ)
	بازگشت FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    بازگشت افزودن_type_stack2(cctx, type, &t_any);
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
    اگر (isn == پوچ)
	بازگشت FAIL;

    isn->isn_arg.عدد = idx;
    بازگشت OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    صحیح
may_generate_2STRING(صحیح offset, صحیح tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_پشته(cctx, -1 - offset);

    انتخاب (type->tt_type)
    {
	// nothing to be done
	حالت VAR_STRING: بازگشت OK;

	// conversion possible
	حالت VAR_SPECIAL:
	حالت VAR_BOOL:
	حالت VAR_عدد:
	حالت VAR_FLOAT:
			 بشکن;

	// conversion possible (with runtime check)
	حالت VAR_ANY:
	حالت VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 بشکن;

	// conversion possible when tolerant
	حالت VAR_LIST:
	حالت VAR_TUPLE:
	حالت VAR_DICT:
			 اگر (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     بشکن;
			 }
			 اگر (tostring_flags & TOSTRING_INTERPOLATE)
			     بشکن;
			 // FALLTHROUGH

	// conversion not possible
	حالت VAR_VOID:
	حالت VAR_BLOB:
	حالت VAR_FUNC:
	حالت VAR_PARTIAL:
	حالت VAR_JOB:
	حالت VAR_CHANNEL:
	حالت VAR_INSTR:
	حالت VAR_CLASS:
	حالت VAR_OBJECT:
	حالت VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 بازگشت FAIL;
    }

    set_type_on_پشته(cctx, &t_string, -1 - offset);
    اگر ((isn = generate_instr(cctx, isntype)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    بازگشت OK;
}

    ایستا صحیح
check_عدد_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    اگر (!((type1 == VAR_عدد || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_عدد || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	اگر (check_type_is_مقدار(typ1) == FAIL
		|| check_type_is_مقدار(typ2) == FAIL)
	    بازگشت FAIL;
	اگر (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	وگرنه
	    semsg(_(e_char_requires_عدد_or_float_arguments), *op);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    ایستا صحیح
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    برای (صحیح i = 0; i < tuple_type->tt_argcount; i++)
    {
	اگر (ga_grow(gap, 1) == FAIL)
	    بازگشت FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    بازگشت OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ایستا صحیح
set_tuple_union_type_on_پشته(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, اندازه(type_T *), 10);

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
    set_type_on_پشته(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    بازگشت OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    صحیح
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  عبارت_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_عدد ? ISN_OPNR
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
	    && check_عدد_or_float(type1, type2, (char_u *)"+") == FAIL)
	بازگشت FAIL;

    اگر (isn != پوچ)
    {
	اگر (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = عبارت_type;
	وگرنه
	    isn->isn_arg.op.op_type = عبارت_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    اگر (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_پشته(cctx, &t_list_any, 0);
    وگرنه اگر (vartype == VAR_TUPLE)
    {
	اگر (!check_tuples_addable(type1, type2))
	    بازگشت FAIL;

	اگر (set_tuple_union_type_on_پشته(type1, type2, cctx) == FAIL)
	    بازگشت FAIL;
    }

    بازگشت isn == پوچ ? FAIL : OK;
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
	    && (type1->tt_type == VAR_عدد
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	بازگشت type1->tt_type;
    بازگشت VAR_ANY;
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
    type1 = get_type_on_پشته(cctx, 1);
    type2 = get_type_on_پشته(cctx, 0);
    vartype = operator_type(type1, type2);

    انتخاب (*op)
    {
	حالت '+':
		  اگر (generate_add_instr(cctx, vartype, type1, type2,
							    عبارت_COPY) == FAIL)
		      بازگشت FAIL;
		  بشکن;

	حالت '-':
	حالت '*':
	حالت '/': اگر (check_عدد_or_float(type1, type2, op) == FAIL)
		      بازگشت FAIL;
		  اگر (vartype == VAR_عدد)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  وگرنه اگر (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  وگرنه
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  اگر (isn != پوچ)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? عبارت_MULT : *op == '/'? عبارت_DIV : عبارت_SUB;
		  بشکن;

	حالت '%': اگر ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_عدد)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_عدد))
		  {
		      emsg(_(e_percent_requires_عدد_arguments));
		      بازگشت FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_عدد ? ISN_OPNR : ISN_OPANY, 1);
		  اگر (isn != پوچ)
		      isn->isn_arg.op.op_type = عبارت_REM;
		  بشکن;
    }

    // correct type of result
    اگر (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	اگر ((type1->tt_type == VAR_عدد || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_عدد || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_پشته(cctx, type, 0);
    }

    بازگشت OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    ایستا isntype_T
compare_isn_not_مقادیر(typval_T *tv, type_T *type)
{
    اگر (tv != پوچ)
	(تهی)check_typval_is_مقدار(tv);
    وگرنه
	(تهی)check_type_is_مقدار(type);
    بازگشت ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    ایستا isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != پوچ ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != پوچ ? tv2->v_type : type2->tt_type;

    اگر (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	بازگشت compare_isn_not_مقادیر(tv1, type1);
    اگر (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	بازگشت compare_isn_not_مقادیر(tv2, type2);

    اگر (vartype1 == vartype2)
    {
	انتخاب (vartype1)
	{
	    حالت VAR_BOOL: isntype = ISN_COMPAREBOOL; بشکن;
	    حالت VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; بشکن;
	    حالت VAR_عدد: isntype = ISN_COMPARENR; بشکن;
	    حالت VAR_FLOAT: isntype = ISN_COMPAREFLOAT; بشکن;
	    حالت VAR_STRING: isntype = ISN_COMPARESTRING; بشکن;
	    حالت VAR_BLOB: isntype = ISN_COMPAREBLOB; بشکن;
	    حالت VAR_LIST: isntype = ISN_COMPARELIST; بشکن;
	    حالت VAR_TUPLE: isntype = ISN_COMPARETUPLE; بشکن;
	    حالت VAR_DICT: isntype = ISN_COMPAREDICT; بشکن;
	    حالت VAR_FUNC: isntype = ISN_COMPAREFUNC; بشکن;
	    حالت VAR_OBJECT: isntype = ISN_COMPAREOBJECT; بشکن;
	    پیشفرض: isntype = ISN_COMPAREANY; بشکن;
	}
    }
    وگرنه اگر (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_عدد || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_عدد || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    وگرنه اگر (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	اگر ((vartype1 == VAR_SPECIAL
		&& (tv1 != پوچ ? tv1->vval.v_عدد == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != پوچ ? tv2->vval.v_عدد == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    بازگشت ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    اگر ((exprtype == عبارت_IS || exprtype == عبارت_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == عبارت_IS ? "is" : "isnot" , vartype_name(vartype1));
	بازگشت ISN_DROP;
    }
    اگر (!(exprtype == عبارت_IS || exprtype == عبارت_ISNOT
		|| exprtype == عبارت_EQUAL || exprtype == عبارت_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	بازگشت ISN_DROP;
    }
    اگر (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != عبارت_EQUAL
			&& exprtype != عبارت_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != عبارت_EQUAL
			 && exprtype != عبارت_NEQUAL
			 && exprtype != عبارت_IS
			 && exprtype != عبارت_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	بازگشت ISN_DROP;
    }
    بازگشت isntype;
}

    صحیح
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    اگر (get_compare_isn(type, tv1, tv2, پوچ, پوچ) == ISN_DROP)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    صحیح
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, صحیح ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*پشته = &cctx->ctx_type_پشته;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, پوچ, پوچ,
		       get_type_on_پشته(cctx, 1), get_type_on_پشته(cctx, 0));
    اگر (isntype == ISN_DROP)
	بازگشت FAIL;

    اگر ((isn = generate_instr(cctx, isntype)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --پشته->ga_len;
    set_type_on_پشته(cctx, &t_bool, 0);

    بازگشت OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    صحیح
generate_CONCAT(cctx_T *cctx, صحیح شمار)
{
    isn_T	*isn;
    garray_T	*پشته = &cctx->ctx_type_پشته;

    RETURN_OK_IF_SKIP(cctx);

    اگر ((isn = generate_instr(cctx, ISN_CONCAT)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = شمار;

    // drop the argument types
    پشته->ga_len -= شمار - 1;

    بازگشت OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    صحیح
generate_2BOOL(cctx_T *cctx, صحیح invert, صحیح offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_2BOOL)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_پشته(cctx, &t_bool, -1 - offset);

    بازگشت OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    صحیح
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    اگر (generate_instr(cctx, ISN_COND2BOOL) == پوچ)
	بازگشت FAIL;

    // type becomes bool
    set_type_on_پشته(cctx, &t_bool, 0);

    بازگشت OK;
}

    صحیح
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	صحیح	    typechk_flags,	// type check flags
	صحیح	    offset,
	صحیح	    is_var,
	صحیح	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == پوچ)
	بازگشت FAIL;
    type_T *tt;
    اگر ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_عدد_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	اگر (tt != پوچ)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    اگر (typechk_flags & TYPECHK_عدد_OK)
		tt->tt_flags |= TTFLAG_عدد_OK;
	    وگرنه
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    وگرنه
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_پشته(cctx, expected, -1 - offset);

    بازگشت OK;
}

    صحیح
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_SETTYPE)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    بازگشت OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    صحیح
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    اگر (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == پوچ)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    ایستا صحیح
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == پوچ ? &t_any : &class->class_type);
    اگر (isn == پوچ)
	بازگشت FAIL;
    isn->isn_arg.classarg = class;
    اگر (class != پوچ)
	++class->class_refcount;
    بازگشت OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    صحیح
generate_tv_افزودن(cctx_T *cctx, typval_T *tv)
{
    انتخاب (tv->v_type)
    {
	حالت VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_عدد);
	    بشکن;
	حالت VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_عدد);
	    بشکن;
	حالت VAR_عدد:
	    generate_PUSHNR(cctx, tv->vval.v_عدد);
	    بشکن;
	حالت VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    بشکن;
	حالت VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = پوچ;
	    بشکن;
	حالت VAR_LIST:
	    اگر (tv->vval.v_list != پوچ)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    بشکن;
	حالت VAR_TUPLE:
	    اگر (tv->vval.v_tuple != پوچ)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    بشکن;
	حالت VAR_DICT:
	    اگر (tv->vval.v_dict != پوچ)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    بشکن;
#ifdef FEAT_JOB_CHANNEL
	حالت VAR_JOB:
	    اگر (tv->vval.v_job != پوچ)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    بشکن;
	حالت VAR_CHANNEL:
	    اگر (tv->vval.v_channel != پوچ)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    بشکن;
#endif
	حالت VAR_FUNC:
	    اگر (tv->vval.v_string != پوچ)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, پوچ, &t_func_unknown, TRUE);
	    بشکن;
	حالت VAR_PARTIAL:
	    اگر (tv->vval.v_partial != پوچ)
		iemsg("non-null partial constant not supported");
	    اگر (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == پوچ)
		بازگشت FAIL;
	    بشکن;
	حالت VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = پوچ;
	    بشکن;
	حالت VAR_OBJECT:
	    اگر (tv->vval.v_object != پوچ)
	    {
		emsg(_(e_cannot_use_non_null_object));
		بازگشت FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    بشکن;
	حالت VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    بشکن;
	پیشفرض:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    بازگشت FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    بازگشت OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    صحیح
generate_PUSHNR(cctx_T *cctx, varnumber_T عدد)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_عدد)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = عدد;

    اگر (عدد == 0 || عدد == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_پشته(cctx, &t_عدد_bool, 0);
    بازگشت OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    صحیح
generate_PUSHBOOL(cctx_T *cctx, varnumber_T عدد)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = عدد;

    بازگشت OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    صحیح
generate_PUSHSPEC(cctx_T *cctx, varnumber_T عدد)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     عدد == VVAL_NULL ? &t_null : &t_none)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = عدد;

    بازگشت OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    صحیح
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.fnumber = fnumber;

    بازگشت OK;
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
	اگر ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == پوچ)
	    ret = FAIL;
	وگرنه
	{
	    isn->isn_arg.string = str == پوچ ? پوچ : *str;
	    بازگشت OK;
	}
    }
    اگر (str != پوچ)
	VIM_CLEAR(*str);
    بازگشت ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    صحیح
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    اگر (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == پوچ)
	بازگشت FAIL;
    بازگشت OK;
#وگرنه
    emsg(_(e_channel_job_feature_not_available));
    بازگشت FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    صحیح
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    اگر (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == پوچ)
	بازگشت FAIL;
    بازگشت OK;
#وگرنه
    emsg(_(e_channel_job_feature_not_available));
    بازگشت FAIL;
#endif
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
					&t_blob, &t_blob)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.blob = blob;

    بازگشت OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    صحیح
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, صحیح may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == پوچ)
	بازگشت FAIL;
    اگر (name == پوچ)
	funcname = پوچ;
    وگرنه اگر (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != پوچ)	    // autoload
	funcname = vim_strsave(name);
    وگرنه
    {
	funcname = alloc(STRLEN(name) + 3);
	اگر (funcname != پوچ)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    بازگشت OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    صحیح
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.string = vim_strsave(name);
    اگر (isn->isn_arg.string == پوچ)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    صحیح
generate_GETITEM(cctx_T *cctx, صحیح index, صحیح with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_پشته(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    اگر ((isn = generate_instr(cctx, ISN_GETITEM)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    بازگشت افزودن_type_پشته(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    ایستا تهی
set_tuple_slice_type_on_پشته(type_T *type, صحیح شمار, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    اگر ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= شمار)
    {
	set_type_on_پشته(cctx, &t_tuple_any, 0);
	بازگشت;
    }

    ga_init2(&tuple_types_ga, اندازه(type_T *), 10);

    برای (صحیح i = شمار; i < type->tt_argcount; i++)
    {
	اگر (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_پشته(cctx, &t_tuple_any, 0);
	    بازگشت;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] =
							     type->tt_args[i];
	tuple_types_ga.ga_len++;
    }

    set_type_on_پشته(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    صحیح
generate_SLICE(cctx_T *cctx, صحیح شمار)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_SLICE)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = شمار;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_پشته(cctx, 0);
    اگر (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_پشته(type, شمار, cctx);

    بازگشت OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    صحیح
generate_CHECKLEN(cctx_T *cctx, صحیح min_len, صحیح more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    اگر ((isn = generate_instr(cctx, ISN_CHECKLEN)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    بازگشت OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    صحیح
generate_STORE(cctx_T *cctx, isntype_T isn_type, صحیح idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, isn_type, 1)) == پوچ)
	بازگشت FAIL;
    اگر (name != پوچ)
	isn->isn_arg.string = vim_strsave(name);
    وگرنه
	isn->isn_arg.عدد = idx;

    بازگشت OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    صحیح
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	صحیح	    load,
	class_T	    *cl,
	صحیح	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    وگرنه
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    اگر (isn == پوچ)
	بازگشت FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    بازگشت OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    ایستا صحیح
generate_STOREOUTER(
	cctx_T	    *cctx,
	صحیح	    idx,
	صحیح	    level,
	صحیح	    loop_depth,
	صحیح	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == پوچ)
	بازگشت FAIL;
    اگر (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    وگرنه
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    بازگشت OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    صحیح
generate_STORENR(cctx_T *cctx, صحیح idx, varnumber_T مقدار)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_STORENR)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = مقدار;

    بازگشت OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    ایستا صحیح
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	صحیح	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, isn_type, 1)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    بازگشت OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    صحیح
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	صحیح	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type2(cctx, isn_type, type, type)) == پوچ)
	بازگشت FAIL;
    اگر (name != پوچ)
	isn->isn_arg.string = vim_strsave(name);
    وگرنه
	isn->isn_arg.عدد = idx;

    بازگشت OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    صحیح
generate_LOADOUTER(
	cctx_T	    *cctx,
	صحیح	    idx,
	صحیح	    nesting,
	صحیح	    loop_depth,
	صحیح	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == پوچ)
	بازگشت FAIL;
    اگر (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    وگرنه
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    بازگشت OK;
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
    صحیح	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    اگر (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	بازگشت FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    بازگشت generate_LOAD(cctx, ISN_LOADV, vidx, پوچ, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    صحیح
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, صحیح forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, isn_type)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    بازگشت OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    صحیح
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    اگر (generate_instr(cctx, ISN_LOCKCONST) == پوچ)
	بازگشت FAIL;
    بازگشت OK;
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
    وگرنه
	isn = generate_instr_drop(cctx, isn_type, 1);
    اگر (isn == پوچ)
	بازگشت FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    بازگشت OK;
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
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    اگر (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    وگرنه
	isn = generate_instr_drop(cctx, isn_type, 1);
    اگر (isn == پوچ)
	بازگشت FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    اگر (sref == پوچ)
	بازگشت FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    بازگشت OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    صحیح
generate_NEWLIST(cctx_T *cctx, صحیح شمار, صحیح use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_NEWLIST)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = use_null ? -1 : شمار;

    // Get the member type and the declared member type from all the items on
    // the stack.
    اگر ((member_type = get_member_type_from_پشته(شمار, 1, cctx)) == پوچ)
	بازگشت FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_پشته.ga_len -= شمار;

    // add the list type to the type stack
    بازگشت افزودن_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    صحیح
generate_NEWTUPLE(cctx_T *cctx, صحیح شمار, صحیح use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = use_null ? -1 : شمار;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, اندازه(type_T *), 10);

    اگر (get_tuple_type_from_پشته(شمار, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	بازگشت FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_پشته.ga_len -= شمار;

    // add the tuple type to the type stack
    بازگشت افزودن_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    صحیح
generate_NEWDICT(cctx_T *cctx, صحیح شمار, صحیح use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_NEWDICT)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = use_null ? -1 : شمار;

    اگر ((member_type = get_member_type_from_پشته(شمار, 2, cctx)) == پوچ)
	بازگشت FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_پشته.ga_len -= 2 * شمار;

    // add the dict type to the type stack
    بازگشت افزودن_type_stack2(cctx, type, decl_type);
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
	صحیح	    object_method,
	صحیح	    fi,
	صحیح	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    صحیح		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_FUNCREF)) == پوچ)
	بازگشت FAIL;
    اگر (isn_idx != پوچ)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    اگر (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != پوچ)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	اگر (extra == پوچ)
	    بازگشت FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	اگر (cl != پوچ)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    اگر (ufunc->uf_def_status == UF_NOT_COMPILED || cl != پوچ)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    اگر (ufunc->uf_def_status != UF_NOT_COMPILED && cl == پوچ)
    {
	اگر (isn_idx == پوچ && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (تهی)compile_def_function(ufunc, FALSE, CT_NONE, پوچ);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_closure = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    اگر ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == پوچ ? &t_func_any : ufunc->uf_func_type;
    بازگشت افزودن_type_پشته(cctx, type);
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
	اگر ((isn = generate_instr(cctx, ISN_NEWFUNC)) == پوچ)
	    ret = FAIL;
	وگرنه
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    اگر (arg == پوچ)
		ret = FAIL;
	    وگرنه
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(تهی)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		بازگشت OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    بازگشت ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    صحیح
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_DEF)) == پوچ)
	بازگشت FAIL;
    اگر (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	اگر (isn->isn_arg.string == پوچ)
	    بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    صحیح
generate_JUMP(cctx_T *cctx, jumpwhen_T when, صحیح where)
{
    isn_T	*isn;
    garray_T	*پشته = &cctx->ctx_type_پشته;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_JUMP)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    اگر (when != JUMP_ALWAYS && پشته->ga_len > 0)
	--پشته->ga_len;

    بازگشت OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    صحیح
generate_WHILE(cctx_T *cctx, صحیح funcref_idx)
{
    isn_T	*isn;
    garray_T	*پشته = &cctx->ctx_type_پشته;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_WHILE)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    اگر (پشته->ga_len > 0)
	--پشته->ga_len;

    بازگشت OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    صحیح
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, صحیح arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, isn_type)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    بازگشت OK;
}

    صحیح
generate_FOR(cctx_T *cctx, صحیح loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_FOR)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    بازگشت افزودن_type_پشته(cctx, &t_any);
}

    صحیح
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_ENDLOOP)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_شمار;
    isn->isn_arg.endloop.end_var_شمار =
			   cctx->ctx_locals.ga_len - loop_info->li_local_شمار;
    بازگشت OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    صحیح
generate_TRYCONT(cctx_T *cctx, صحیح levels, صحیح where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_TRYCONT)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    بازگشت OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    صحیح
check_internal_func_args(
	cctx_T	*cctx,
	صحیح	func_idx,
	صحیح	argcount,
	صحیح	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*پشته = &cctx->ctx_type_پشته;
    صحیح		argoff = check_internal_func(func_idx, argcount);

    اگر (argoff < 0)
	بازگشت FAIL;

    اگر (method_call && argoff > 1)
    {
	اگر (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    بازگشت FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	اگر (isn  == پوچ)
	    بازگشت FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    اگر (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)پشته->ga_data) + پشته->ga_len - argcount;

	// Check the types of the arguments.
	اگر (method_call && argoff > 1)
	{
	    صحیح i;

	    برای (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	وگرنه
	{
	    صحیح i;

	    برای (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	اگر (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    صحیح
generate_BCALL(cctx_T *cctx, صحیح func_idx, صحیح argcount, صحیح method_call)
{
    isn_T	*isn;
    garray_T	*پشته = &cctx->ctx_type_پشته;
    type2_T	*argtypes = پوچ;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = پوچ;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    اگر (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	بازگشت FAIL;

    اگر (internal_func_is_map(func_idx))
	maptype = argtypes;

    اگر ((isn = generate_instr(cctx, ISN_BCALL)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    پشته->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    اگر (افزودن_type_stack2(cctx, type, decl_type) == FAIL)
	بازگشت FAIL;

    اگر (maptype != پوچ && maptype[0].type_decl->tt_member != پوچ
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    بازگشت OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    صحیح
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_پشته(cctx, 1);
    اگر (arg_type_modifiable(list_type, 1) == FAIL)
	بازگشت FAIL;
    item_type = get_type_on_پشته(cctx, 0);
    expected = list_type->tt_member;
    اگر (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	بازگشت FAIL;

    اگر (generate_instr(cctx, ISN_LISTAPPEND) == پوچ)
	بازگشت FAIL;

    --cctx->ctx_type_پشته.ga_len;	    // drop the argument
    بازگشت OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    صحیح
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    اگر (arg_type_modifiable(get_decl_type_on_پشته(cctx, 1), 1) == FAIL)
	بازگشت FAIL;
    item_type = get_type_on_پشته(cctx, 0);
    اگر (need_type(item_type, &t_عدد, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	بازگشت FAIL;

    اگر (generate_instr(cctx, ISN_BLOBAPPEND) == پوچ)
	بازگشت FAIL;

    --cctx->ctx_type_پشته.ga_len;	    // drop the argument
    بازگشت OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    ایستا isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    اگر (cl != پوچ)
	بازگشت ISN_METHODCALL;

    اگر (ufunc->uf_def_status == UF_NOT_COMPILED)
	بازگشت ISN_UCALL;

    // function invoked from a lambda
    اگر (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	اگر (ufunc->uf_name[0] == K_SPECIAL)
	    بازگشت ISN_UCALL;
    }

    بازگشت ISN_DCALL;
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
    صحیح		regular_args = ufunc->uf_args.ga_len;
    صحیح		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    اگر (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	بازگشت FAIL;
    }
    اگر (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	بازگشت FAIL;
    }

    اگر (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	صحیح		i;
	compiletype_T	compile_type;

	برای (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_پشته(cctx, argcount - i - 1);
	    اگر (check_type_is_مقدار(actual) == FAIL)
		بازگشت FAIL;
	    اگر (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		ادامه;
	    }
	    اگر (i < regular_args)
	    {
		اگر (ufunc->uf_arg_types == پوچ)
		    ادامه;
		expected = ufunc->uf_arg_types[i];
	    }
	    وگرنه اگر (ufunc->uf_va_type == پوچ
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    وگرنه
		expected = ufunc->uf_va_type->tt_member;
	    اگر (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		بازگشت FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	اگر (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == پوچ,
						   compile_type, پوچ) == FAIL)
	    بازگشت FAIL;
    }
    اگر (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	بازگشت FAIL;
    }

    اگر ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == پوچ)
	بازگشت FAIL;
    اگر (cl != پوچ /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	اگر (isn->isn_arg.mfunc == پوچ)
	    بازگشت FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = پوچ;
	اگر (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = پوچ;
	isn->isn_arg.mfunc->cmf_cache_generic = پوچ;
	isn->isn_arg.mfunc->cmf_cache_ufunc = پوچ;
    }
    وگرنه اگر (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    وگرنه
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_پشته.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    اگر (ufunc->uf_class != پوچ)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_پشته(cctx, 0);
	اگر (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_پشته.ga_len--;
    }

    // add return type
    بازگشت افزودن_type_پشته(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    صحیح
generate_UCALL(cctx_T *cctx, char_u *name, صحیح argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_UCALL)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_پشته.ga_len -= argcount;

    // add return value
    بازگشت افزودن_type_پشته(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    صحیح
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	صحیح	argcount,
	صحیح	at_بالاترین,
	char_u	*name)
{
    اگر (type->tt_argcount != -1)
    {
	صحیح	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	اگر (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    بازگشت FAIL;
	}
	اگر (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    بازگشت FAIL;
	}
	اگر (type->tt_args != پوچ)
	{
	    صحیح i;

	    برای (i = 0; i < argcount; ++i)
	    {
		صحیح	offset = -argcount + i - (at_بالاترین ? 0 : 1);
		type_T	*actual = get_type_on_پشته(cctx, -1 - offset);
		type_T	*expected;

		اگر (check_type_is_مقدار(actual) == FAIL)
		    بازگشت FAIL;
		اگر (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    اگر (expected != پوچ && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    اگر (expected == پوچ)
			expected = &t_any;
		}
		وگرنه اگر (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		وگرنه
		    expected = type->tt_args[i];
		اگر (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    بازگشت FAIL;
		}
	    }
	}
    }

    بازگشت OK;
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
	صحیح	at_بالاترین)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    اگر (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    وگرنه اگر (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	اگر (check_func_args_from_type(cctx, type, argcount, at_بالاترین, name)
								       == FAIL)
	    بازگشت FAIL;

	ret_type = type->tt_member;
	اگر (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    وگرنه
    {
	semsg(_(e_not_callable_type_str), name);
	بازگشت FAIL;
    }

    اگر ((isn = generate_instr(cctx, ISN_PCALL)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.pfunc.cpf_بالاترین = at_بالاترین;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_پشته.ga_len -= argcount + 1;

    // push the return value
    افزودن_type_پشته(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    اگر (at_بالاترین && generate_instr(cctx, ISN_PCALL_END) == پوچ)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    صحیح
generate_DEFER(cctx_T *cctx, صحیح var_idx, صحیح argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    بازگشت OK;
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
    اگر ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_پشته(cctx, 0);
    اگر (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	نویسه *tofree;

	semsg(_(e_expected_dictionary_for_using_کلید_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	بازگشت FAIL;
    }
    // change dict type to dict member type
    اگر (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_پشته(cctx, ntype, 0);
    }

    بازگشت OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    صحیح
generate_ECHO(cctx_T *cctx, صحیح with_white, صحیح شمار)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, ISN_ECHO, شمار)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_شمار = شمار;

    بازگشت OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    صحیح
generate_MULT_عبارت(cctx_T *cctx, isntype_T isn_type, صحیح شمار)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr_drop(cctx, isn_type, شمار)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = شمار;
    بازگشت OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    صحیح
generate_ECHOWINDOW(cctx_T *cctx, صحیح شمار, long time)
{
    isn_T	*isn;

    اگر ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, شمار)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.echowin.ewin_شمار = شمار;
    isn->isn_arg.echowin.ewin_time = time;
    بازگشت OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    صحیح
generate_SOURCE(cctx_T *cctx, صحیح sid)
{
    isn_T	*isn;

    اگر ((isn = generate_instr(cctx, ISN_SOURCE)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = sid;

    بازگشت OK;
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
    اگر (isn == پوچ)
	بازگشت FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    بازگشت OK;
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
    اگر ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == پوچ)
	بازگشت FAIL;
    class_T *cl = cctx->ctx_ufunc != پوچ ? cctx->ctx_ufunc->uf_class : پوچ;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(سطر);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    اگر (cl != پوچ)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    بازگشت OK;
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
    اگر ((isn = generate_instr(cctx, isntype)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.string = vim_strsave(سطر);
    بازگشت OK;
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
	اگر ((isn = generate_instr(cctx, isntype)) == پوچ)
	    ret = FAIL;
	وگرنه
	{
	    isn->isn_arg.string = str;
	    بازگشت OK;
	}
    }
    vim_free(str);
    بازگشت ret;
}

    صحیح
generate_LEGACY_EVAL(cctx_T *cctx, char_u *سطر)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.string = vim_strsave(سطر);

    بازگشت افزودن_type_پشته(cctx, &t_any);
}

    صحیح
generate_EXECCONCAT(cctx_T *cctx, صحیح شمار)
{
    isn_T	*isn;

    اگر ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, شمار)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.عدد = شمار;
    بازگشت OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    صحیح
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    اگر ((isn = generate_instr(cctx, ISN_RANGE)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.string = range;

    بازگشت افزودن_type_پشته(cctx, &t_عدد);
}

    صحیح
generate_UNPACK(cctx_T *cctx, صحیح var_شمار, صحیح semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_UNPACK)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.unpack.unp_شمار = var_شمار;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    بازگشت OK;
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

	اگر ((isn = generate_instr(cctx, ISN_CMDMOD)) == پوچ)
	    بازگشت FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	اگر (isn->isn_arg.cmdmod.cf_cmdmod == پوچ)
	    بازگشت FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, اندازه(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = پوچ;
    }

    بازگشت OK;
}

    صحیح
generate_undo_cmdmods(cctx_T *cctx)
{
    اگر (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == پوچ)
	بازگشت FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    بازگشت OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    صحیح
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	صحیح		opt_flags,
	صحیح		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    انتخاب (dest)
    {
	حالت dest_option:
	    بازگشت generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	حالت dest_func_option:
	    بازگشت generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	حالت dest_global:
	    // include g: with the name, easier to execute that way
	    بازگشت generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == پوچ
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	حالت dest_buffer:
	    // include b: with the name, easier to execute that way
	    بازگشت generate_STORE(cctx, ISN_STOREB, 0, name);
	حالت dest_window:
	    // include w: with the name, easier to execute that way
	    بازگشت generate_STORE(cctx, ISN_STOREW, 0, name);
	حالت dest_tab:
	    // include t: with the name, easier to execute that way
	    بازگشت generate_STORE(cctx, ISN_STORET, 0, name);
	حالت dest_env:
	    بازگشت generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	حالت dest_reg:
	    بازگشت generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], پوچ);
	حالت dest_vimvar:
	    بازگشت generate_STORE(cctx, ISN_STOREV, vimvaridx, پوچ);
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
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == پوچ)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			اگر (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    بازگشت FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    بازگشت generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		بازگشت generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	حالت dest_class_member:
	    بازگشت generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	حالت dest_local:
	حالت dest_عبارت:
	    // cannot happen
	    بشکن;
    }
    بازگشت FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    صحیح
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    برای (;;)
    {
	اگر (scope == پوچ)
	    بشکن;
	اگر (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    بازگشت TRUE;
	scope = scope->se_outer;
    }
    بازگشت FALSE;
}

    صحیح
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, صحیح instr_شمار, صحیح is_decl)
{
    اگر (lhs->lhs_dest != dest_local)
	بازگشت generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    اگر (lhs->lhs_lvar == پوچ)
	بازگشت OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    اگر (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_شمار + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.عدد;
	garray_T    *پشته = &cctx->ctx_type_پشته;

	اگر (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	وگرنه
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	اگر (پشته->ga_len > 0)
	    --پشته->ga_len;
    }
    وگرنه اگر (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    وگرنه
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, پوچ);
    بازگشت OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    صحیح
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    اگر ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == پوچ)
	بازگشت FAIL;
    isn->isn_arg.setsctx = new_sctx;
    بازگشت OK;
}

#اگر defined(FEAT_PROFILE)
    تهی
may_generate_prof_end(cctx_T *cctx, صحیح prof_lnum)
{
    اگر (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_END);
}
#endif


/*
 * Delete an instruction, free what it contains.
 */
    تهی
delete_instr(isn_T *isn)
{
    انتخاب (isn->isn_type)
    {
	حالت ISN_AUTOLOAD:
	حالت ISN_DEF:
	حالت ISN_EXEC:
	حالت ISN_EXECRANGE:
	حالت ISN_EXEC_SPLIT:
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
	    بشکن;

	حالت ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    بشکن;

	حالت ISN_SUBSTITUTE:
	    {
		صحیح	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		برای (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    بشکن;

	حالت ISN_INSTR:
	    {
		صحیح	idx;
		isn_T	*list = isn->isn_arg.instr;

		برای (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    بشکن;

	حالت ISN_LOADS:
	حالت ISN_LOADEXPORT:
	حالت ISN_STORES:
	حالت ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    بشکن;

	حالت ISN_UNLET:
	حالت ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    بشکن;

	حالت ISN_STOREOPT:
	حالت ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    بشکن;

	حالت ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    بشکن;

	حالت ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    بشکن;

	حالت ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    بشکن;

	حالت ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		اگر (extra == پوچ || extra->fre_func_name == پوچ)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    اگر (ufunc != پوچ && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		اگر (extra != پوچ)
		{
		    char_u *name = extra->fre_func_name;

		    اگر (name != پوچ)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    اگر (extra->fre_class != پوچ)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    بشکن;

	حالت ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		اگر (dfunc->df_ufunc != پوچ
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    بشکن;

	حالت ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    بشکن;

	حالت ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		اگر (arg != پوچ)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    اگر (ufunc != پوچ)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    بشکن;

	حالت ISN_CHECKTYPE:
	حالت ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    بشکن;

	حالت ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    بشکن;

	حالت ISN_LOADSCRIPT:
	حالت ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    بشکن;

	حالت ISN_LOAD_CLASSMEMBER:
	حالت ISN_STORE_CLASSMEMBER:
	حالت ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    بشکن;

	حالت ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    بشکن;

	حالت ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    بشکن;

	حالت ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    بشکن;

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
	حالت ISN_FINISH:
	حالت ISN_FOR:
	حالت ISN_GETITEM:
	حالت ISN_GET_OBJ_MEMBER:
	حالت ISN_JUMP:
	حالت ISN_JUMP_IF_ARG_NOT_SET:
	حالت ISN_JUMP_IF_ARG_SET:
	حالت ISN_LISTAPPEND:
	حالت ISN_LISTINDEX:
	حالت ISN_LISTSLICE:
	حالت ISN_TUPLEINDEX:
	حالت ISN_TUPLESLICE:
	حالت ISN_LOAD:
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
	حالت ISN_PCALL_END:
	حالت ISN_PROF_END:
	حالت ISN_PROF_START:
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
	حالت ISN_SLICE:
	حالت ISN_SOURCE:
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
	حالت ISN_SCRIPTCTX_SET:
	// nothing allocated
	بشکن;
    }
}

    تهی
clear_instr_ga(garray_T *gap)
{
    صحیح idx;

    برای (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
