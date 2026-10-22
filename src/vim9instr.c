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
#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) যদি (cctx->ctx_skip == SKIP_YES) ফেরত ফাঁকা
#define RETURN_OK_IF_SKIP(cctx) যদি (cctx->ctx_skip == SKIP_YES) ফেরত OK

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
    যদি (GA_GROW_FAILS(instr, 1))
	ফেরত ফাঁকা;
    isn = ((isn_T *)instr->ga_তথ্য) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    ফেরত isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, পূর্ণসংখ্যা drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_স্তূপ.ga_len -= drop;
    ফেরত generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    স্থির isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    যদি ((isn = generate_instr(cctx, isn_type)) == ফাঁকা)
	ফেরত ফাঁকা;

    যদি (ঠেলো_type_stack2(cctx, type == ফাঁকা ? &t_any : type,
			       decl_type == ফাঁকা ? &t_any : decl_type) == FAIL)
	ফেরত ফাঁকা;

    ফেরত isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    ফেরত generate_instr_type2(cctx, isn_type, type, &t_any);
}

/*
 * Generate an ISN_DEBUG instruction.
 */
    isn_T *
generate_instr_debug(cctx_T *cctx)
{
    isn_T	*isn;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_তথ্য)
					       + cctx->ctx_ufunc->uf_dfunc_idx;

    যদি ((isn = generate_instr(cctx, ISN_DEBUG)) == ফাঁকা)
	ফেরত ফাঁকা;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_পূর্ব_lnum;
    ফেরত isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    পূর্ণসংখ্যা
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.construct.construct_আকার = আকার(object_T)
			       + cl->class_obj_member_গুনতি * আকার(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    ফেরত OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    পূর্ণসংখ্যা
generate_GET_OBJ_MEMBER(cctx_T *cctx, পূর্ণসংখ্যা idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    যদি (isn == ফাঁকা)
	ফেরত FAIL;

    isn->isn_arg.classmember.cm_class = ফাঁকা;
    isn->isn_arg.classmember.cm_idx = idx;
    ফেরত ঠেলো_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    পূর্ণসংখ্যা
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, পূর্ণসংখ্যা idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    যদি (isn == ফাঁকা)
	ফেরত FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    ফেরত ঠেলো_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    পূর্ণসংখ্যা
generate_STORE_THIS(cctx_T *cctx, পূর্ণসংখ্যা idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    যদি (isn == ফাঁকা)
	ফেরত FAIL;

    isn->isn_arg.সংখ্যা = idx;
    ফেরত OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    পূর্ণসংখ্যা
may_generate_2STRING(পূর্ণসংখ্যা offset, পূর্ণসংখ্যা tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_স্তূপ(cctx, -1 - offset);

    নির্বাচন (type->tt_type)
    {
	// nothing to be done
	ক্ষেত্র VAR_STRING: ফেরত OK;

	// conversion possible
	ক্ষেত্র VAR_SPECIAL:
	ক্ষেত্র VAR_BOOL:
	ক্ষেত্র VAR_সংখ্যা:
	ক্ষেত্র VAR_FLOAT:
			 বিরতি;

	// conversion possible (with runtime check)
	ক্ষেত্র VAR_ANY:
	ক্ষেত্র VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 বিরতি;

	// conversion possible when tolerant
	ক্ষেত্র VAR_LIST:
	ক্ষেত্র VAR_TUPLE:
	ক্ষেত্র VAR_DICT:
			 যদি (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     বিরতি;
			 }
			 যদি (tostring_flags & TOSTRING_INTERPOLATE)
			     বিরতি;
			 // FALLTHROUGH

	// conversion not possible
	ক্ষেত্র VAR_VOID:
	ক্ষেত্র VAR_BLOB:
	ক্ষেত্র VAR_FUNC:
	ক্ষেত্র VAR_PARTIAL:
	ক্ষেত্র VAR_JOB:
	ক্ষেত্র VAR_CHANNEL:
	ক্ষেত্র VAR_INSTR:
	ক্ষেত্র VAR_CLASS:
	ক্ষেত্র VAR_OBJECT:
	ক্ষেত্র VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 ফেরত FAIL;
    }

    set_type_on_স্তূপ(cctx, &t_string, -1 - offset);
    যদি ((isn = generate_instr(cctx, isntype)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    ফেরত OK;
}

    স্থির পূর্ণসংখ্যা
check_সংখ্যা_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    যদি (!((type1 == VAR_সংখ্যা || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_সংখ্যা || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	যদি (check_type_is_মান(typ1) == FAIL
		|| check_type_is_মান(typ2) == FAIL)
	    ফেরত FAIL;
	যদি (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	অন্যথা
	    semsg(_(e_char_requires_সংখ্যা_or_float_arguments), *op);
	ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    স্থির পূর্ণসংখ্যা
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    জন্য (পূর্ণসংখ্যা i = 0; i < tuple_type->tt_argcount; i++)
    {
	যদি (ga_grow(gap, 1) == FAIL)
	    ফেরত FAIL;

	((type_T **)gap->ga_তথ্য)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    ফেরত OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    স্থির পূর্ণসংখ্যা
set_tuple_union_type_on_স্তূপ(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, আকার(type_T *), 10);

    যদি (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    যদি (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    যদি ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_স্তূপ(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    ফেরত OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    পূর্ণসংখ্যা
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  রাশিমালা_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_সংখ্যা ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    যদি (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_সংখ্যা_or_float(type1, type2, (char_u *)"+") == FAIL)
	ফেরত FAIL;

    যদি (isn != ফাঁকা)
    {
	যদি (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = রাশিমালা_type;
	অন্যথা
	    isn->isn_arg.op.op_type = রাশিমালা_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    যদি (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_স্তূপ(cctx, &t_list_any, 0);
    অন্যথা যদি (vartype == VAR_TUPLE)
    {
	যদি (!check_tuples_addable(type1, type2))
	    ফেরত FAIL;

	যদি (set_tuple_union_type_on_স্তূপ(type1, type2, cctx) == FAIL)
	    ফেরত FAIL;
    }

    ফেরত isn == ফাঁকা ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    যদি (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_সংখ্যা
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	ফেরত type1->tt_type;
    ফেরত VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    পূর্ণসংখ্যা
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_স্তূপ(cctx, 1);
    type2 = get_type_on_স্তূপ(cctx, 0);
    vartype = operator_type(type1, type2);

    নির্বাচন (*op)
    {
	ক্ষেত্র '+':
		  যদি (generate_add_instr(cctx, vartype, type1, type2,
							    রাশিমালা_COPY) == FAIL)
		      ফেরত FAIL;
		  বিরতি;

	ক্ষেত্র '-':
	ক্ষেত্র '*':
	ক্ষেত্র '/': যদি (check_সংখ্যা_or_float(type1, type2, op) == FAIL)
		      ফেরত FAIL;
		  যদি (vartype == VAR_সংখ্যা)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  অন্যথা যদি (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  অন্যথা
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  যদি (isn != ফাঁকা)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? রাশিমালা_MULT : *op == '/'? রাশিমালা_DIV : রাশিমালা_SUB;
		  বিরতি;

	ক্ষেত্র '%': যদি ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_সংখ্যা)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_সংখ্যা))
		  {
		      emsg(_(e_percent_requires_সংখ্যা_arguments));
		      ফেরত FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_সংখ্যা ? ISN_OPNR : ISN_OPANY, 1);
		  যদি (isn != ফাঁকা)
		      isn->isn_arg.op.op_type = রাশিমালা_REM;
		  বিরতি;
    }

    // correct type of result
    যদি (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	যদি ((type1->tt_type == VAR_সংখ্যা || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_সংখ্যা || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_স্তূপ(cctx, type, 0);
    }

    ফেরত OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    স্থির isntype_T
compare_isn_not_মানসমূহ(typval_T *tv, type_T *type)
{
    যদি (tv != ফাঁকা)
	(শূন্য)check_typval_is_মান(tv);
    অন্যথা
	(শূন্য)check_type_is_মান(type);
    ফেরত ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    স্থির isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != ফাঁকা ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != ফাঁকা ? tv2->v_type : type2->tt_type;

    যদি (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	ফেরত compare_isn_not_মানসমূহ(tv1, type1);
    যদি (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	ফেরত compare_isn_not_মানসমূহ(tv2, type2);

    যদি (vartype1 == vartype2)
    {
	নির্বাচন (vartype1)
	{
	    ক্ষেত্র VAR_BOOL: isntype = ISN_COMPAREBOOL; বিরতি;
	    ক্ষেত্র VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; বিরতি;
	    ক্ষেত্র VAR_সংখ্যা: isntype = ISN_COMPARENR; বিরতি;
	    ক্ষেত্র VAR_FLOAT: isntype = ISN_COMPAREFLOAT; বিরতি;
	    ক্ষেত্র VAR_STRING: isntype = ISN_COMPARESTRING; বিরতি;
	    ক্ষেত্র VAR_BLOB: isntype = ISN_COMPAREBLOB; বিরতি;
	    ক্ষেত্র VAR_LIST: isntype = ISN_COMPARELIST; বিরতি;
	    ক্ষেত্র VAR_TUPLE: isntype = ISN_COMPARETUPLE; বিরতি;
	    ক্ষেত্র VAR_DICT: isntype = ISN_COMPAREDICT; বিরতি;
	    ক্ষেত্র VAR_FUNC: isntype = ISN_COMPAREFUNC; বিরতি;
	    ক্ষেত্র VAR_OBJECT: isntype = ISN_COMPAREOBJECT; বিরতি;
	    পূর্বনির্ধারিত: isntype = ISN_COMPAREANY; বিরতি;
	}
    }
    অন্যথা যদি (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_সংখ্যা || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_সংখ্যা || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    অন্যথা যদি (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	যদি ((vartype1 == VAR_SPECIAL
		&& (tv1 != ফাঁকা ? tv1->vval.v_সংখ্যা == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != ফাঁকা ? tv2->vval.v_সংখ্যা == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    ফেরত ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    যদি ((exprtype == রাশিমালা_IS || exprtype == রাশিমালা_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == রাশিমালা_IS ? "is" : "isnot" , vartype_name(vartype1));
	ফেরত ISN_DROP;
    }
    যদি (!(exprtype == রাশিমালা_IS || exprtype == রাশিমালা_ISNOT
		|| exprtype == রাশিমালা_EQUAL || exprtype == রাশিমালা_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	ফেরত ISN_DROP;
    }
    যদি (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != রাশিমালা_EQUAL
			&& exprtype != রাশিমালা_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != রাশিমালা_EQUAL
			 && exprtype != রাশিমালা_NEQUAL
			 && exprtype != রাশিমালা_IS
			 && exprtype != রাশিমালা_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	ফেরত ISN_DROP;
    }
    ফেরত isntype;
}

    পূর্ণসংখ্যা
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    যদি (get_compare_isn(type, tv1, tv2, ফাঁকা, ফাঁকা) == ISN_DROP)
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    পূর্ণসংখ্যা
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, পূর্ণসংখ্যা ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, ফাঁকা, ফাঁকা,
		       get_type_on_স্তূপ(cctx, 1), get_type_on_স্তূপ(cctx, 0));
    যদি (isntype == ISN_DROP)
	ফেরত FAIL;

    যদি ((isn = generate_instr(cctx, isntype)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --স্তূপ->ga_len;
    set_type_on_স্তূপ(cctx, &t_bool, 0);

    ফেরত OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    পূর্ণসংখ্যা
generate_CONCAT(cctx_T *cctx, পূর্ণসংখ্যা গুনতি)
{
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    RETURN_OK_IF_SKIP(cctx);

    যদি ((isn = generate_instr(cctx, ISN_CONCAT)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = গুনতি;

    // drop the argument types
    স্তূপ->ga_len -= গুনতি - 1;

    ফেরত OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    পূর্ণসংখ্যা
generate_2BOOL(cctx_T *cctx, পূর্ণসংখ্যা invert, পূর্ণসংখ্যা offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_2BOOL)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_স্তূপ(cctx, &t_bool, -1 - offset);

    ফেরত OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    পূর্ণসংখ্যা
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    যদি (generate_instr(cctx, ISN_COND2BOOL) == ফাঁকা)
	ফেরত FAIL;

    // type becomes bool
    set_type_on_স্তূপ(cctx, &t_bool, 0);

    ফেরত OK;
}

    পূর্ণসংখ্যা
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	পূর্ণসংখ্যা	    typechk_flags,	// type check flags
	পূর্ণসংখ্যা	    offset,
	পূর্ণসংখ্যা	    is_var,
	পূর্ণসংখ্যা	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == ফাঁকা)
	ফেরত FAIL;
    type_T *tt;
    যদি ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_সংখ্যা_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	যদি (tt != ফাঁকা)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    যদি (typechk_flags & TYPECHK_সংখ্যা_OK)
		tt->tt_flags |= TTFLAG_সংখ্যা_OK;
	    অন্যথা
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    অন্যথা
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_স্তূপ(cctx, expected, -1 - offset);

    ফেরত OK;
}

    পূর্ণসংখ্যা
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_SETTYPE)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    ফেরত OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    পূর্ণসংখ্যা
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    যদি (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    স্থির পূর্ণসংখ্যা
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == ফাঁকা ? &t_any : &class->class_type);
    যদি (isn == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.classarg = class;
    যদি (class != ফাঁকা)
	++class->class_refcount;
    ফেরত OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    পূর্ণসংখ্যা
generate_tv_ঠেলো(cctx_T *cctx, typval_T *tv)
{
    নির্বাচন (tv->v_type)
    {
	ক্ষেত্র VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_সংখ্যা);
	    বিরতি;
	ক্ষেত্র VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_সংখ্যা);
	    বিরতি;
	ক্ষেত্র VAR_সংখ্যা:
	    generate_PUSHNR(cctx, tv->vval.v_সংখ্যা);
	    বিরতি;
	ক্ষেত্র VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    বিরতি;
	ক্ষেত্র VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_LIST:
	    যদি (tv->vval.v_list != ফাঁকা)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    বিরতি;
	ক্ষেত্র VAR_TUPLE:
	    যদি (tv->vval.v_tuple != ফাঁকা)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    বিরতি;
	ক্ষেত্র VAR_DICT:
	    যদি (tv->vval.v_dict != ফাঁকা)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    বিরতি;
#ifdef FEAT_JOB_CHANNEL
	ক্ষেত্র VAR_JOB:
	    যদি (tv->vval.v_job != ফাঁকা)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    বিরতি;
	ক্ষেত্র VAR_CHANNEL:
	    যদি (tv->vval.v_channel != ফাঁকা)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    বিরতি;
#endif
	ক্ষেত্র VAR_FUNC:
	    যদি (tv->vval.v_string != ফাঁকা)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, ফাঁকা, &t_func_unknown, TRUE);
	    বিরতি;
	ক্ষেত্র VAR_PARTIAL:
	    যদি (tv->vval.v_partial != ফাঁকা)
		iemsg("non-null partial constant not supported");
	    যদি (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == ফাঁকা)
		ফেরত FAIL;
	    বিরতি;
	ক্ষেত্র VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = ফাঁকা;
	    বিরতি;
	ক্ষেত্র VAR_OBJECT:
	    যদি (tv->vval.v_object != ফাঁকা)
	    {
		emsg(_(e_cannot_use_non_null_object));
		ফেরত FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    বিরতি;
	ক্ষেত্র VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    বিরতি;
	পূর্বনির্ধারিত:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    ফেরত FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    ফেরত OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    পূর্ণসংখ্যা
generate_PUSHNR(cctx_T *cctx, varnumber_T সংখ্যা)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_সংখ্যা)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = সংখ্যা;

    যদি (সংখ্যা == 0 || সংখ্যা == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_স্তূপ(cctx, &t_সংখ্যা_bool, 0);
    ফেরত OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    পূর্ণসংখ্যা
generate_PUSHBOOL(cctx_T *cctx, varnumber_T সংখ্যা)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = সংখ্যা;

    ফেরত OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    পূর্ণসংখ্যা
generate_PUSHSPEC(cctx_T *cctx, varnumber_T সংখ্যা)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     সংখ্যা == VVAL_NULL ? &t_null : &t_none)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = সংখ্যা;

    ফেরত OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    পূর্ণসংখ্যা
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.fnumber = fnumber;

    ফেরত OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    পূর্ণসংখ্যা
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    পূর্ণসংখ্যা		ret = OK;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	যদি ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == ফাঁকা)
	    ret = FAIL;
	অন্যথা
	{
	    isn->isn_arg.string = str == ফাঁকা ? ফাঁকা : *str;
	    ফেরত OK;
	}
    }
    যদি (str != ফাঁকা)
	VIM_CLEAR(*str);
    ফেরত ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    পূর্ণসংখ্যা
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    যদি (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
#অন্যথা
    emsg(_(e_channel_job_feature_not_available));
    ফেরত FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    পূর্ণসংখ্যা
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    যদি (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
#অন্যথা
    emsg(_(e_channel_job_feature_not_available));
    ফেরত FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    পূর্ণসংখ্যা
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.blob = blob;

    ফেরত OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    পূর্ণসংখ্যা
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, পূর্ণসংখ্যা may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == ফাঁকা)
	ফেরত FAIL;
    যদি (name == ফাঁকা)
	funcname = ফাঁকা;
    অন্যথা যদি (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != ফাঁকা)	    // autoload
	funcname = vim_strsave(name);
    অন্যথা
    {
	funcname = alloc(STRLEN(name) + 3);
	যদি (funcname != ফাঁকা)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    ফেরত OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    পূর্ণসংখ্যা
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.string = vim_strsave(name);
    যদি (isn->isn_arg.string == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    পূর্ণসংখ্যা
generate_GETITEM(cctx_T *cctx, পূর্ণসংখ্যা সূচক, পূর্ণসংখ্যা with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_স্তূপ(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    যদি ((isn = generate_instr(cctx, ISN_GETITEM)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.getitem.gi_সূচক = সূচক;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    ফেরত ঠেলো_type_স্তূপ(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    স্থির শূন্য
set_tuple_slice_type_on_স্তূপ(type_T *type, পূর্ণসংখ্যা গুনতি, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    যদি ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= গুনতি)
    {
	set_type_on_স্তূপ(cctx, &t_tuple_any, 0);
	ফেরত;
    }

    ga_init2(&tuple_types_ga, আকার(type_T *), 10);

    জন্য (পূর্ণসংখ্যা i = গুনতি; i < type->tt_argcount; i++)
    {
	যদি (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_স্তূপ(cctx, &t_tuple_any, 0);
	    ফেরত;
	}
	((type_T **)tuple_types_ga.ga_তথ্য)[tuple_types_ga.ga_len] =
							     type->tt_args[i];
	tuple_types_ga.ga_len++;
    }

    set_type_on_স্তূপ(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    পূর্ণসংখ্যা
generate_SLICE(cctx_T *cctx, পূর্ণসংখ্যা গুনতি)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_SLICE)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = গুনতি;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_স্তূপ(cctx, 0);
    যদি (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_স্তূপ(type, গুনতি, cctx);

    ফেরত OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    পূর্ণসংখ্যা
generate_CHECKLEN(cctx_T *cctx, পূর্ণসংখ্যা min_len, পূর্ণসংখ্যা more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    যদি ((isn = generate_instr(cctx, ISN_CHECKLEN)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    ফেরত OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    পূর্ণসংখ্যা
generate_STORE(cctx_T *cctx, isntype_T isn_type, পূর্ণসংখ্যা idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, isn_type, 1)) == ফাঁকা)
	ফেরত FAIL;
    যদি (name != ফাঁকা)
	isn->isn_arg.string = vim_strsave(name);
    অন্যথা
	isn->isn_arg.সংখ্যা = idx;

    ফেরত OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    পূর্ণসংখ্যা
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	পূর্ণসংখ্যা	    load,
	class_T	    *cl,
	পূর্ণসংখ্যা	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    অন্যথা
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    যদি (isn == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    ফেরত OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    স্থির পূর্ণসংখ্যা
generate_STOREOUTER(
	cctx_T	    *cctx,
	পূর্ণসংখ্যা	    idx,
	পূর্ণসংখ্যা	    level,
	পূর্ণসংখ্যা	    loop_depth,
	পূর্ণসংখ্যা	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == ফাঁকা)
	ফেরত FAIL;
    যদি (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    অন্যথা
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    ফেরত OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    পূর্ণসংখ্যা
generate_STORENR(cctx_T *cctx, পূর্ণসংখ্যা idx, varnumber_T মান)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_STORENR)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = মান;

    ফেরত OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    স্থির পূর্ণসংখ্যা
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	পূর্ণসংখ্যা	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, isn_type, 1)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    ফেরত OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    পূর্ণসংখ্যা
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	পূর্ণসংখ্যা	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type2(cctx, isn_type, type, type)) == ফাঁকা)
	ফেরত FAIL;
    যদি (name != ফাঁকা)
	isn->isn_arg.string = vim_strsave(name);
    অন্যথা
	isn->isn_arg.সংখ্যা = idx;

    ফেরত OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    পূর্ণসংখ্যা
generate_LOADOUTER(
	cctx_T	    *cctx,
	পূর্ণসংখ্যা	    idx,
	পূর্ণসংখ্যা	    nesting,
	পূর্ণসংখ্যা	    loop_depth,
	পূর্ণসংখ্যা	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == ফাঁকা)
	ফেরত FAIL;
    যদি (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    অন্যথা
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    ফেরত OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    পূর্ণসংখ্যা
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    পূর্ণসংখ্যা	    di_flags;
    পূর্ণসংখ্যা	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    যদি (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	ফেরত FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    ফেরত generate_LOAD(cctx, ISN_LOADV, vidx, ফাঁকা, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    পূর্ণসংখ্যা
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, পূর্ণসংখ্যা forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, isn_type)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    ফেরত OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    পূর্ণসংখ্যা
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    যদি (generate_instr(cctx, ISN_LOCKCONST) == ফাঁকা)
	ফেরত FAIL;
    ফেরত OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    পূর্ণসংখ্যা
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	পূর্ণসংখ্যা	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    অন্যথা
	isn = generate_instr_drop(cctx, isn_type, 1);
    যদি (isn == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    ফেরত OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    পূর্ণসংখ্যা
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	পূর্ণসংখ্যা	    sid,
	পূর্ণসংখ্যা	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    যদি (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    অন্যথা
	isn = generate_instr_drop(cctx, isn_type, 1);
    যদি (isn == ফাঁকা)
	ফেরত FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    যদি (sref == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    ফেরত OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    পূর্ণসংখ্যা
generate_NEWLIST(cctx_T *cctx, পূর্ণসংখ্যা গুনতি, পূর্ণসংখ্যা use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_NEWLIST)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = use_null ? -1 : গুনতি;

    // Get the member type and the declared member type from all the items on
    // the stack.
    যদি ((member_type = get_member_type_from_স্তূপ(গুনতি, 1, cctx)) == ফাঁকা)
	ফেরত FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_স্তূপ.ga_len -= গুনতি;

    // add the list type to the type stack
    ফেরত ঠেলো_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    পূর্ণসংখ্যা
generate_NEWTUPLE(cctx_T *cctx, পূর্ণসংখ্যা গুনতি, পূর্ণসংখ্যা use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = use_null ? -1 : গুনতি;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, আকার(type_T *), 10);

    যদি (get_tuple_type_from_স্তূপ(গুনতি, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	ফেরত FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_স্তূপ.ga_len -= গুনতি;

    // add the tuple type to the type stack
    ফেরত ঠেলো_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    পূর্ণসংখ্যা
generate_NEWDICT(cctx_T *cctx, পূর্ণসংখ্যা গুনতি, পূর্ণসংখ্যা use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_NEWDICT)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = use_null ? -1 : গুনতি;

    যদি ((member_type = get_member_type_from_স্তূপ(গুনতি, 2, cctx)) == ফাঁকা)
	ফেরত FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_স্তূপ.ga_len -= 2 * গুনতি;

    // add the dict type to the type stack
    ফেরত ঠেলো_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    পূর্ণসংখ্যা
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	পূর্ণসংখ্যা	    object_method,
	পূর্ণসংখ্যা	    fi,
	পূর্ণসংখ্যা	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    পূর্ণসংখ্যা		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_FUNCREF)) == ফাঁকা)
	ফেরত FAIL;
    যদি (isn_idx != ফাঁকা)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    যদি (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != ফাঁকা)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	যদি (extra == ফাঁকা)
	    ফেরত FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	যদি (cl != ফাঁকা)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    যদি (ufunc->uf_def_status == UF_NOT_COMPILED || cl != ফাঁকা)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    যদি (ufunc->uf_def_status != UF_NOT_COMPILED && cl == ফাঁকা)
    {
	যদি (isn_idx == ফাঁকা && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (শূন্য)compile_def_function(ufunc, FALSE, CT_NONE, ফাঁকা);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_closure = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    যদি ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == ফাঁকা ? &t_func_any : ufunc->uf_func_type;
    ফেরত ঠেলো_type_স্তূপ(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    পূর্ণসংখ্যা
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    পূর্ণসংখ্যা		ret = OK;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	যদি ((isn = generate_instr(cctx, ISN_NEWFUNC)) == ফাঁকা)
	    ret = FAIL;
	অন্যথা
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    যদি (arg == ফাঁকা)
		ret = FAIL;
	    অন্যথা
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(শূন্য)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		ফেরত OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    ফেরত ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    পূর্ণসংখ্যা
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_DEF)) == ফাঁকা)
	ফেরত FAIL;
    যদি (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	যদি (isn->isn_arg.string == ফাঁকা)
	    ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    পূর্ণসংখ্যা
generate_JUMP(cctx_T *cctx, jumpwhen_T when, পূর্ণসংখ্যা where)
{
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_JUMP)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    যদি (when != JUMP_ALWAYS && স্তূপ->ga_len > 0)
	--স্তূপ->ga_len;

    ফেরত OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    পূর্ণসংখ্যা
generate_WHILE(cctx_T *cctx, পূর্ণসংখ্যা funcref_idx)
{
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_WHILE)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    যদি (স্তূপ->ga_len > 0)
	--স্তূপ->ga_len;

    ফেরত OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    পূর্ণসংখ্যা
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, পূর্ণসংখ্যা arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, isn_type)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    ফেরত OK;
}

    পূর্ণসংখ্যা
generate_FOR(cctx_T *cctx, পূর্ণসংখ্যা loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_FOR)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    ফেরত ঠেলো_type_স্তূপ(cctx, &t_any);
}

    পূর্ণসংখ্যা
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_ENDLOOP)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_গুনতি;
    isn->isn_arg.endloop.end_var_গুনতি =
			   cctx->ctx_locals.ga_len - loop_info->li_local_গুনতি;
    ফেরত OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    পূর্ণসংখ্যা
generate_TRYCONT(cctx_T *cctx, পূর্ণসংখ্যা levels, পূর্ণসংখ্যা where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_TRYCONT)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    ফেরত OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    পূর্ণসংখ্যা
check_internal_func_args(
	cctx_T	*cctx,
	পূর্ণসংখ্যা	func_idx,
	পূর্ণসংখ্যা	argcount,
	পূর্ণসংখ্যা	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    পূর্ণসংখ্যা		argoff = check_internal_func(func_idx, argcount);

    যদি (argoff < 0)
	ফেরত FAIL;

    যদি (method_call && argoff > 1)
    {
	যদি (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    ফেরত FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	যদি (isn  == ফাঁকা)
	    ফেরত FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    যদি (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)স্তূপ->ga_তথ্য) + স্তূপ->ga_len - argcount;

	// Check the types of the arguments.
	যদি (method_call && argoff > 1)
	{
	    পূর্ণসংখ্যা i;

	    জন্য (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	অন্যথা
	{
	    পূর্ণসংখ্যা i;

	    জন্য (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	যদি (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    ফেরত FAIL;
    }
    ফেরত OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    পূর্ণসংখ্যা
generate_BCALL(cctx_T *cctx, পূর্ণসংখ্যা func_idx, পূর্ণসংখ্যা argcount, পূর্ণসংখ্যা method_call)
{
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*argtypes = ফাঁকা;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = ফাঁকা;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    যদি (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	ফেরত FAIL;

    যদি (internal_func_is_map(func_idx))
	maptype = argtypes;

    যদি ((isn = generate_instr(cctx, ISN_BCALL)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    স্তূপ->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    যদি (ঠেলো_type_stack2(cctx, type, decl_type) == FAIL)
	ফেরত FAIL;

    যদি (maptype != ফাঁকা && maptype[0].type_decl->tt_member != ফাঁকা
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    ফেরত OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    পূর্ণসংখ্যা
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_স্তূপ(cctx, 1);
    যদি (arg_type_modifiable(list_type, 1) == FAIL)
	ফেরত FAIL;
    item_type = get_type_on_স্তূপ(cctx, 0);
    expected = list_type->tt_member;
    যদি (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ফেরত FAIL;

    যদি (generate_instr(cctx, ISN_LISTAPPEND) == ফাঁকা)
	ফেরত FAIL;

    --cctx->ctx_type_স্তূপ.ga_len;	    // drop the argument
    ফেরত OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    পূর্ণসংখ্যা
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    যদি (arg_type_modifiable(get_decl_type_on_স্তূপ(cctx, 1), 1) == FAIL)
	ফেরত FAIL;
    item_type = get_type_on_স্তূপ(cctx, 0);
    যদি (need_type(item_type, &t_সংখ্যা, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ফেরত FAIL;

    যদি (generate_instr(cctx, ISN_BLOBAPPEND) == ফাঁকা)
	ফেরত FAIL;

    --cctx->ctx_type_স্তূপ.ga_len;	    // drop the argument
    ফেরত OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    স্থির isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    যদি (cl != ফাঁকা)
	ফেরত ISN_METHODCALL;

    যদি (ufunc->uf_def_status == UF_NOT_COMPILED)
	ফেরত ISN_UCALL;

    // function invoked from a lambda
    যদি (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	যদি (ufunc->uf_name[0] == K_SPECIAL)
	    ফেরত ISN_UCALL;
    }

    ফেরত ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    পূর্ণসংখ্যা
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	পূর্ণসংখ্যা	    mi,
	পূর্ণসংখ্যা	    pushed_argcount,
	পূর্ণসংখ্যা	    is_super)
{
    isn_T	*isn;
    পূর্ণসংখ্যা		regular_args = ufunc->uf_args.ga_len;
    পূর্ণসংখ্যা		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    যদি (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	ফেরত FAIL;
    }
    যদি (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	ফেরত FAIL;
    }

    যদি (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	পূর্ণসংখ্যা		i;
	compiletype_T	compile_type;

	জন্য (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_স্তূপ(cctx, argcount - i - 1);
	    যদি (check_type_is_মান(actual) == FAIL)
		ফেরত FAIL;
	    যদি (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		চালু;
	    }
	    যদি (i < regular_args)
	    {
		যদি (ufunc->uf_arg_types == ফাঁকা)
		    চালু;
		expected = ufunc->uf_arg_types[i];
	    }
	    অন্যথা যদি (ufunc->uf_va_type == ফাঁকা
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    অন্যথা
		expected = ufunc->uf_va_type->tt_member;
	    যদি (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		ফেরত FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	যদি (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == ফাঁকা,
						   compile_type, ফাঁকা) == FAIL)
	    ফেরত FAIL;
    }
    যদি (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	ফেরত FAIL;
    }

    যদি ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == ফাঁকা)
	ফেরত FAIL;
    যদি (cl != ফাঁকা /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	যদি (isn->isn_arg.mfunc == ফাঁকা)
	    ফেরত FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = ফাঁকা;
	যদি (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = ফাঁকা;
	isn->isn_arg.mfunc->cmf_cache_generic = ফাঁকা;
	isn->isn_arg.mfunc->cmf_cache_ufunc = ফাঁকা;
    }
    অন্যথা যদি (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    অন্যথা
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_স্তূপ.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    যদি (ufunc->uf_class != ফাঁকা)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_স্তূপ(cctx, 0);
	যদি (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_স্তূপ.ga_len--;
    }

    // add return type
    ফেরত ঠেলো_type_স্তূপ(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    পূর্ণসংখ্যা
generate_UCALL(cctx_T *cctx, char_u *name, পূর্ণসংখ্যা argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_UCALL)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_স্তূপ.ga_len -= argcount;

    // add return value
    ফেরত ঠেলো_type_স্তূপ(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    পূর্ণসংখ্যা
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	পূর্ণসংখ্যা	argcount,
	পূর্ণসংখ্যা	at_শীর্ষ,
	char_u	*name)
{
    যদি (type->tt_argcount != -1)
    {
	পূর্ণসংখ্যা	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	যদি (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    ফেরত FAIL;
	}
	যদি (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    ফেরত FAIL;
	}
	যদি (type->tt_args != ফাঁকা)
	{
	    পূর্ণসংখ্যা i;

	    জন্য (i = 0; i < argcount; ++i)
	    {
		পূর্ণসংখ্যা	offset = -argcount + i - (at_শীর্ষ ? 0 : 1);
		type_T	*actual = get_type_on_স্তূপ(cctx, -1 - offset);
		type_T	*expected;

		যদি (check_type_is_মান(actual) == FAIL)
		    ফেরত FAIL;
		যদি (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    যদি (expected != ফাঁকা && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    যদি (expected == ফাঁকা)
			expected = &t_any;
		}
		অন্যথা যদি (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		অন্যথা
		    expected = type->tt_args[i];
		যদি (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    ফেরত FAIL;
		}
	    }
	}
    }

    ফেরত OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    পূর্ণসংখ্যা
generate_PCALL(
	cctx_T	*cctx,
	পূর্ণসংখ্যা	argcount,
	char_u	*name,
	type_T	*type,
	পূর্ণসংখ্যা	at_শীর্ষ)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    যদি (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    অন্যথা যদি (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	যদি (check_func_args_from_type(cctx, type, argcount, at_শীর্ষ, name)
								       == FAIL)
	    ফেরত FAIL;

	ret_type = type->tt_member;
	যদি (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    অন্যথা
    {
	semsg(_(e_not_callable_type_str), name);
	ফেরত FAIL;
    }

    যদি ((isn = generate_instr(cctx, ISN_PCALL)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.pfunc.cpf_শীর্ষ = at_শীর্ষ;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_স্তূপ.ga_len -= argcount + 1;

    // push the return value
    ঠেলো_type_স্তূপ(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    যদি (at_শীর্ষ && generate_instr(cctx, ISN_PCALL_END) == ফাঁকা)
	ফেরত FAIL;

    ফেরত OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    পূর্ণসংখ্যা
generate_DEFER(cctx_T *cctx, পূর্ণসংখ্যা var_idx, পূর্ণসংখ্যা argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    ফেরত OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    পূর্ণসংখ্যা
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_স্তূপ(cctx, 0);
    যদি (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	অক্ষর *tofree;

	semsg(_(e_expected_dictionary_for_using_চাবি_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	ফেরত FAIL;
    }
    // change dict type to dict member type
    যদি (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_স্তূপ(cctx, ntype, 0);
    }

    ফেরত OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    পূর্ণসংখ্যা
generate_ECHO(cctx_T *cctx, পূর্ণসংখ্যা with_white, পূর্ণসংখ্যা গুনতি)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, ISN_ECHO, গুনতি)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_গুনতি = গুনতি;

    ফেরত OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    পূর্ণসংখ্যা
generate_MULT_রাশিমালা(cctx_T *cctx, isntype_T isn_type, পূর্ণসংখ্যা গুনতি)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, isn_type, গুনতি)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = গুনতি;
    ফেরত OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    পূর্ণসংখ্যা
generate_ECHOWINDOW(cctx_T *cctx, পূর্ণসংখ্যা গুনতি, long time)
{
    isn_T	*isn;

    যদি ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, গুনতি)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.echowin.ewin_গুনতি = গুনতি;
    isn->isn_arg.echowin.ewin_time = time;
    ফেরত OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    পূর্ণসংখ্যা
generate_SOURCE(cctx_T *cctx, পূর্ণসংখ্যা sid)
{
    isn_T	*isn;

    যদি ((isn = generate_instr(cctx, ISN_SOURCE)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = sid;

    ফেরত OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    পূর্ণসংখ্যা
generate_PUT(cctx_T *cctx, পূর্ণসংখ্যা regname, linenr_T lnum, পূর্ণসংখ্যা fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    যদি (isn == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    ফেরত OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    পূর্ণসংখ্যা
generate_LOCKUNLOCK(cctx_T *cctx, char_u *সারি, পূর্ণসংখ্যা is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == ফাঁকা)
	ফেরত FAIL;
    class_T *cl = cctx->ctx_ufunc != ফাঁকা ? cctx->ctx_ufunc->uf_class : ফাঁকা;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(সারি);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    যদি (cl != ফাঁকা)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    ফেরত OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    পূর্ণসংখ্যা
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *সারি)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, isntype)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.string = vim_strsave(সারি);
    ফেরত OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    পূর্ণসংখ্যা
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    পূর্ণসংখ্যা		ret = OK;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	যদি ((isn = generate_instr(cctx, isntype)) == ফাঁকা)
	    ret = FAIL;
	অন্যথা
	{
	    isn->isn_arg.string = str;
	    ফেরত OK;
	}
    }
    vim_free(str);
    ফেরত ret;
}

    পূর্ণসংখ্যা
generate_LEGACY_EVAL(cctx_T *cctx, char_u *সারি)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.string = vim_strsave(সারি);

    ফেরত ঠেলো_type_স্তূপ(cctx, &t_any);
}

    পূর্ণসংখ্যা
generate_EXECCONCAT(cctx_T *cctx, পূর্ণসংখ্যা গুনতি)
{
    isn_T	*isn;

    যদি ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, গুনতি)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.সংখ্যা = গুনতি;
    ফেরত OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    পূর্ণসংখ্যা
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    যদি ((isn = generate_instr(cctx, ISN_RANGE)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.string = range;

    ফেরত ঠেলো_type_স্তূপ(cctx, &t_সংখ্যা);
}

    পূর্ণসংখ্যা
generate_UNPACK(cctx_T *cctx, পূর্ণসংখ্যা var_গুনতি, পূর্ণসংখ্যা semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_UNPACK)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.unpack.unp_গুনতি = var_গুনতি;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    ফেরত OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    পূর্ণসংখ্যা
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    যদি (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	যদি ((isn = generate_instr(cctx, ISN_CMDMOD)) == ফাঁকা)
	    ফেরত FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	যদি (isn->isn_arg.cmdmod.cf_cmdmod == ফাঁকা)
	    ফেরত FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, আকার(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = ফাঁকা;
    }

    ফেরত OK;
}

    পূর্ণসংখ্যা
generate_undo_cmdmods(cctx_T *cctx)
{
    যদি (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == ফাঁকা)
	ফেরত FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    ফেরত OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    পূর্ণসংখ্যা
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	পূর্ণসংখ্যা		opt_flags,
	পূর্ণসংখ্যা		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    নির্বাচন (dest)
    {
	ক্ষেত্র dest_option:
	    ফেরত generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	ক্ষেত্র dest_func_option:
	    ফেরত generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	ক্ষেত্র dest_global:
	    // include g: with the name, easier to execute that way
	    ফেরত generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == ফাঁকা
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	ক্ষেত্র dest_বাফার:
	    // include b: with the name, easier to execute that way
	    ফেরত generate_STORE(cctx, ISN_STOREB, 0, name);
	ক্ষেত্র dest_window:
	    // include w: with the name, easier to execute that way
	    ফেরত generate_STORE(cctx, ISN_STOREW, 0, name);
	ক্ষেত্র dest_tab:
	    // include t: with the name, easier to execute that way
	    ফেরত generate_STORE(cctx, ISN_STORET, 0, name);
	ক্ষেত্র dest_env:
	    ফেরত generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	ক্ষেত্র dest_reg:
	    ফেরত generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], ফাঁকা);
	ক্ষেত্র dest_vimvar:
	    ফেরত generate_STORE(cctx, ISN_STOREV, vimvaridx, ফাঁকা);
	ক্ষেত্র dest_script:
	ক্ষেত্র dest_script_v9:
	    {
		পূর্ণসংখ্যা	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		পূর্ণসংখ্যা	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		যদি (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    যদি (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == ফাঁকা)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			যদি (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    ফেরত FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    ফেরত generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		ফেরত generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	ক্ষেত্র dest_class_member:
	    ফেরত generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	ক্ষেত্র dest_local:
	ক্ষেত্র dest_রাশিমালা:
	    // cannot happen
	    বিরতি;
    }
    ফেরত FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    পূর্ণসংখ্যা
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    জন্য (;;)
    {
	যদি (scope == ফাঁকা)
	    বিরতি;
	যদি (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    ফেরত TRUE;
	scope = scope->se_outer;
    }
    ফেরত FALSE;
}

    পূর্ণসংখ্যা
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, পূর্ণসংখ্যা instr_গুনতি, পূর্ণসংখ্যা is_decl)
{
    যদি (lhs->lhs_dest != dest_local)
	ফেরত generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    যদি (lhs->lhs_lvar == ফাঁকা)
	ফেরত OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_তথ্য) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    যদি (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_গুনতি + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.সংখ্যা;
	garray_T    *স্তূপ = &cctx->ctx_type_স্তূপ;

	যদি (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	অন্যথা
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	যদি (স্তূপ->ga_len > 0)
	    --স্তূপ->ga_len;
    }
    অন্যথা যদি (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    অন্যথা
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, ফাঁকা);
    ফেরত OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    পূর্ণসংখ্যা
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == ফাঁকা)
	ফেরত FAIL;
    isn->isn_arg.setsctx = new_sctx;
    ফেরত OK;
}

#যদি defined(FEAT_PROFILE)
    শূন্য
may_generate_prof_end(cctx_T *cctx, পূর্ণসংখ্যা prof_lnum)
{
    যদি (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_END);
}
#endif


/*
 * Delete an instruction, free what it contains.
 */
    শূন্য
delete_instr(isn_T *isn)
{
    নির্বাচন (isn->isn_type)
    {
	ক্ষেত্র ISN_AUTOLOAD:
	ক্ষেত্র ISN_DEF:
	ক্ষেত্র ISN_EXEC:
	ক্ষেত্র ISN_EXECRANGE:
	ক্ষেত্র ISN_EXEC_SPLIT:
	ক্ষেত্র ISN_LEGACY_EVAL:
	ক্ষেত্র ISN_LOADAUTO:
	ক্ষেত্র ISN_LOADB:
	ক্ষেত্র ISN_LOADENV:
	ক্ষেত্র ISN_LOADG:
	ক্ষেত্র ISN_LOADOPT:
	ক্ষেত্র ISN_LOADT:
	ক্ষেত্র ISN_LOADW:
	ক্ষেত্র ISN_PUSHEXC:
	ক্ষেত্র ISN_PUSHFUNC:
	ক্ষেত্র ISN_PUSHS:
	ক্ষেত্র ISN_RANGE:
	ক্ষেত্র ISN_STOREAUTO:
	ক্ষেত্র ISN_STOREB:
	ক্ষেত্র ISN_STOREENV:
	ক্ষেত্র ISN_STOREG:
	ক্ষেত্র ISN_STORET:
	ক্ষেত্র ISN_STOREW:
	ক্ষেত্র ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    বিরতি;

	ক্ষেত্র ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    বিরতি;

	ক্ষেত্র ISN_SUBSTITUTE:
	    {
		পূর্ণসংখ্যা	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		জন্য (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    বিরতি;

	ক্ষেত্র ISN_INSTR:
	    {
		পূর্ণসংখ্যা	idx;
		isn_T	*list = isn->isn_arg.instr;

		জন্য (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    বিরতি;

	ক্ষেত্র ISN_LOADS:
	ক্ষেত্র ISN_LOADEXPORT:
	ক্ষেত্র ISN_STORES:
	ক্ষেত্র ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    বিরতি;

	ক্ষেত্র ISN_UNLET:
	ক্ষেত্র ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    বিরতি;

	ক্ষেত্র ISN_STOREOPT:
	ক্ষেত্র ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    বিরতি;

	ক্ষেত্র ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    বিরতি;

	ক্ষেত্র ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    বিরতি;

	ক্ষেত্র ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    বিরতি;

	ক্ষেত্র ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		যদি (extra == ফাঁকা || extra->fre_func_name == ফাঁকা)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_তথ্য)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    যদি (ufunc != ফাঁকা && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		যদি (extra != ফাঁকা)
		{
		    char_u *name = extra->fre_func_name;

		    যদি (name != ফাঁকা)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    যদি (extra->fre_class != ফাঁকা)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    বিরতি;

	ক্ষেত্র ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_তথ্য)
		    + isn->isn_arg.dfunc.cdf_idx;

		যদি (dfunc->df_ufunc != ফাঁকা
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    বিরতি;

	ক্ষেত্র ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    বিরতি;

	ক্ষেত্র ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		যদি (arg != ফাঁকা)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    যদি (ufunc != ফাঁকা)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    বিরতি;

	ক্ষেত্র ISN_CHECKTYPE:
	ক্ষেত্র ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    বিরতি;

	ক্ষেত্র ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    বিরতি;

	ক্ষেত্র ISN_LOADSCRIPT:
	ক্ষেত্র ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    বিরতি;

	ক্ষেত্র ISN_LOAD_CLASSMEMBER:
	ক্ষেত্র ISN_STORE_CLASSMEMBER:
	ক্ষেত্র ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    বিরতি;

	ক্ষেত্র ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    বিরতি;

	ক্ষেত্র ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    বিরতি;

	ক্ষেত্র ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    বিরতি;

	ক্ষেত্র ISN_2BOOL:
	ক্ষেত্র ISN_2STRING:
	ক্ষেত্র ISN_2STRING_ANY:
	ক্ষেত্র ISN_ADDBLOB:
	ক্ষেত্র ISN_ADDLIST:
	ক্ষেত্র ISN_ADDTUPLE:
	ক্ষেত্র ISN_ANYINDEX:
	ক্ষেত্র ISN_ANYSLICE:
	ক্ষেত্র ISN_BCALL:
	ক্ষেত্র ISN_BLOBAPPEND:
	ক্ষেত্র ISN_BLOBINDEX:
	ক্ষেত্র ISN_BLOBSLICE:
	ক্ষেত্র ISN_CATCH:
	ক্ষেত্র ISN_CEXPR_AUCMD:
	ক্ষেত্র ISN_CHECKLEN:
	ক্ষেত্র ISN_CLEARDICT:
	ক্ষেত্র ISN_CMDMOD_REV:
	ক্ষেত্র ISN_COMPAREANY:
	ক্ষেত্র ISN_COMPAREBLOB:
	ক্ষেত্র ISN_COMPAREBOOL:
	ক্ষেত্র ISN_COMPAREDICT:
	ক্ষেত্র ISN_COMPAREFLOAT:
	ক্ষেত্র ISN_COMPAREFUNC:
	ক্ষেত্র ISN_COMPARELIST:
	ক্ষেত্র ISN_COMPARETUPLE:
	ক্ষেত্র ISN_COMPARENR:
	ক্ষেত্র ISN_COMPARENULL:
	ক্ষেত্র ISN_COMPAREOBJECT:
	ক্ষেত্র ISN_COMPARESPECIAL:
	ক্ষেত্র ISN_COMPARESTRING:
	ক্ষেত্র ISN_CONCAT:
	ক্ষেত্র ISN_CONSTRUCT:
	ক্ষেত্র ISN_COND2BOOL:
	ক্ষেত্র ISN_DEBUG:
	ক্ষেত্র ISN_DEFER:
	ক্ষেত্র ISN_DROP:
	ক্ষেত্র ISN_ECHO:
	ক্ষেত্র ISN_ECHOCONSOLE:
	ক্ষেত্র ISN_ECHOERR:
	ক্ষেত্র ISN_ECHOMSG:
	ক্ষেত্র ISN_ECHOWINDOW:
	ক্ষেত্র ISN_ENDLOOP:
	ক্ষেত্র ISN_ENDTRY:
	ক্ষেত্র ISN_EXECCONCAT:
	ক্ষেত্র ISN_EXECUTE:
	ক্ষেত্র ISN_FINALLY:
	ক্ষেত্র ISN_FINISH:
	ক্ষেত্র ISN_FOR:
	ক্ষেত্র ISN_GETITEM:
	ক্ষেত্র ISN_GET_OBJ_MEMBER:
	ক্ষেত্র ISN_JUMP:
	ক্ষেত্র ISN_JUMP_IF_ARG_NOT_SET:
	ক্ষেত্র ISN_JUMP_IF_ARG_SET:
	ক্ষেত্র ISN_LISTAPPEND:
	ক্ষেত্র ISN_LISTINDEX:
	ক্ষেত্র ISN_LISTSLICE:
	ক্ষেত্র ISN_TUPLEINDEX:
	ক্ষেত্র ISN_TUPLESLICE:
	ক্ষেত্র ISN_LOAD:
	ক্ষেত্র ISN_LOADBDICT:
	ক্ষেত্র ISN_LOADGDICT:
	ক্ষেত্র ISN_LOADOUTER:
	ক্ষেত্র ISN_LOADREG:
	ক্ষেত্র ISN_LOADTDICT:
	ক্ষেত্র ISN_LOADV:
	ক্ষেত্র ISN_LOADWDICT:
	ক্ষেত্র ISN_LOCKCONST:
	ক্ষেত্র ISN_MEMBER:
	ক্ষেত্র ISN_NEGATENR:
	ক্ষেত্র ISN_NEWDICT:
	ক্ষেত্র ISN_NEWLIST:
	ক্ষেত্র ISN_NEWTUPLE:
	ক্ষেত্র ISN_NEWPARTIAL:
	ক্ষেত্র ISN_OPANY:
	ক্ষেত্র ISN_OPFLOAT:
	ক্ষেত্র ISN_OPNR:
	ক্ষেত্র ISN_PCALL:
	ক্ষেত্র ISN_PCALL_END:
	ক্ষেত্র ISN_PROF_END:
	ক্ষেত্র ISN_PROF_START:
	ক্ষেত্র ISN_PUSHBOOL:
	ক্ষেত্র ISN_PUSHCHANNEL:
	ক্ষেত্র ISN_PUSHF:
	ক্ষেত্র ISN_PUSHJOB:
	ক্ষেত্র ISN_PUSHNR:
	ক্ষেত্র ISN_PUSHOBJ:
	ক্ষেত্র ISN_PUSHSPEC:
	ক্ষেত্র ISN_PUT:
	ক্ষেত্র ISN_IPUT:
	ক্ষেত্র ISN_REDIREND:
	ক্ষেত্র ISN_REDIRSTART:
	ক্ষেত্র ISN_RETURN:
	ক্ষেত্র ISN_RETURN_OBJECT:
	ক্ষেত্র ISN_RETURN_VOID:
	ক্ষেত্র ISN_SHUFFLE:
	ক্ষেত্র ISN_SLICE:
	ক্ষেত্র ISN_SOURCE:
	ক্ষেত্র ISN_STORE:
	ক্ষেত্র ISN_STORENR:
	ক্ষেত্র ISN_STOREOUTER:
	ক্ষেত্র ISN_STORE_THIS:
	ক্ষেত্র ISN_STORERANGE:
	ক্ষেত্র ISN_STOREREG:
	ক্ষেত্র ISN_STOREV:
	ক্ষেত্র ISN_STRINDEX:
	ক্ষেত্র ISN_STRSLICE:
	ক্ষেত্র ISN_THROW:
	ক্ষেত্র ISN_TRYCONT:
	ক্ষেত্র ISN_UNLETINDEX:
	ক্ষেত্র ISN_UNLETRANGE:
	ক্ষেত্র ISN_UNPACK:
	ক্ষেত্র ISN_USEDICT:
	ক্ষেত্র ISN_WHILE:
	ক্ষেত্র ISN_SCRIPTCTX_SET:
	// nothing allocated
	বিরতি;
    }
}

    শূন্য
clear_instr_ga(garray_T *gap)
{
    পূর্ণসংখ্যা idx;

    জন্য (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_তথ্য) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
