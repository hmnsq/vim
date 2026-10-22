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
#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) যদি (cctx->ctx_skip == SKIP_YES) ঘূৰাই রিক্ত
#define RETURN_OK_IF_SKIP(cctx) যদি (cctx->ctx_skip == SKIP_YES) ঘূৰাই OK

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
	ঘূৰাই রিক্ত;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    ঘূৰাই isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, পূর্ণাঙ্ক drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_স্তূপ.ga_len -= drop;
    ঘূৰাই generate_instr(cctx, isn_type);
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

    যদি ((isn = generate_instr(cctx, isn_type)) == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (ঠেলো_type_stack2(cctx, type == রিক্ত ? &t_any : type,
			       decl_type == রিক্ত ? &t_any : decl_type) == FAIL)
	ঘূৰাই রিক্ত;

    ঘূৰাই isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    ঘূৰাই generate_instr_type2(cctx, isn_type, type, &t_any);
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

    যদি ((isn = generate_instr(cctx, ISN_DEBUG)) == রিক্ত)
	ঘূৰাই রিক্ত;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    ঘূৰাই isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    পূর্ণাঙ্ক
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.construct.construct_size = পরিমাণ(object_T)
			       + cl->class_obj_member_গুনতি * পরিমাণ(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    ঘূৰাই OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    পূর্ণাঙ্ক
generate_GET_OBJ_MEMBER(cctx_T *cctx, পূর্ণাঙ্ক idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    যদি (isn == রিক্ত)
	ঘূৰাই FAIL;

    isn->isn_arg.classmember.cm_class = রিক্ত;
    isn->isn_arg.classmember.cm_idx = idx;
    ঘূৰাই ঠেলো_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    পূর্ণাঙ্ক
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, পূর্ণাঙ্ক idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    যদি (isn == রিক্ত)
	ঘূৰাই FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    ঘূৰাই ঠেলো_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    পূর্ণাঙ্ক
generate_STORE_THIS(cctx_T *cctx, পূর্ণাঙ্ক idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    যদি (isn == রিক্ত)
	ঘূৰাই FAIL;

    isn->isn_arg.সংখ্যা = idx;
    ঘূৰাই OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    পূর্ণাঙ্ক
may_generate_2STRING(পূর্ণাঙ্ক offset, পূর্ণাঙ্ক tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_স্তূপ(cctx, -1 - offset);

    ৱিকল্প (type->tt_type)
    {
	// nothing to be done
	পক্ষ VAR_STRING: ঘূৰাই OK;

	// conversion possible
	পক্ষ VAR_SPECIAL:
	পক্ষ VAR_BOOL:
	পক্ষ VAR_সংখ্যা:
	পক্ষ VAR_FLOAT:
			 ভঙ্গ;

	// conversion possible (with runtime check)
	পক্ষ VAR_ANY:
	পক্ষ VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 ভঙ্গ;

	// conversion possible when tolerant
	পক্ষ VAR_LIST:
	পক্ষ VAR_TUPLE:
	পক্ষ VAR_DICT:
			 যদি (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     ভঙ্গ;
			 }
			 যদি (tostring_flags & TOSTRING_INTERPOLATE)
			     ভঙ্গ;
			 // FALLTHROUGH

	// conversion not possible
	পক্ষ VAR_VOID:
	পক্ষ VAR_BLOB:
	পক্ষ VAR_FUNC:
	পক্ষ VAR_PARTIAL:
	পক্ষ VAR_JOB:
	পক্ষ VAR_CHANNEL:
	পক্ষ VAR_INSTR:
	পক্ষ VAR_CLASS:
	পক্ষ VAR_OBJECT:
	পক্ষ VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 ঘূৰাই FAIL;
    }

    set_type_on_স্তূপ(cctx, &t_string, -1 - offset);
    যদি ((isn = generate_instr(cctx, isntype)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    ঘূৰাই OK;
}

    স্থির পূর্ণাঙ্ক
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
	    ঘূৰাই FAIL;
	যদি (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	অন্যথা
	    semsg(_(e_char_requires_সংখ্যা_or_float_arguments), *op);
	ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    স্থির পূর্ণাঙ্ক
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    বাবে (পূর্ণাঙ্ক i = 0; i < tuple_type->tt_argcount; i++)
    {
	যদি (ga_grow(gap, 1) == FAIL)
	    ঘূৰাই FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    ঘূৰাই OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    স্থির পূর্ণাঙ্ক
set_tuple_union_type_on_স্তূপ(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, পরিমাণ(type_T *), 10);

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

    ঘূৰাই OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    পূর্ণাঙ্ক
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  ৰাশিমালা_type)
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
	ঘূৰাই FAIL;

    যদি (isn != রিক্ত)
    {
	যদি (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = ৰাশিমালা_type;
	অন্যথা
	    isn->isn_arg.op.op_type = ৰাশিমালা_ADD;
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
	    ঘূৰাই FAIL;

	যদি (set_tuple_union_type_on_স্তূপ(type1, type2, cctx) == FAIL)
	    ঘূৰাই FAIL;
    }

    ঘূৰাই isn == রিক্ত ? FAIL : OK;
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
	ঘূৰাই type1->tt_type;
    ঘূৰাই VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    পূর্ণাঙ্ক
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

    ৱিকল্প (*op)
    {
	পক্ষ '+':
		  যদি (generate_add_instr(cctx, vartype, type1, type2,
							    ৰাশিমালা_COPY) == FAIL)
		      ঘূৰাই FAIL;
		  ভঙ্গ;

	পক্ষ '-':
	পক্ষ '*':
	পক্ষ '/': যদি (check_সংখ্যা_or_float(type1, type2, op) == FAIL)
		      ঘূৰাই FAIL;
		  যদি (vartype == VAR_সংখ্যা)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  অন্যথা যদি (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  অন্যথা
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  যদি (isn != রিক্ত)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? ৰাশিমালা_MULT : *op == '/'? ৰাশিমালা_DIV : ৰাশিমালা_SUB;
		  ভঙ্গ;

	পক্ষ '%': যদি ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_সংখ্যা)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_সংখ্যা))
		  {
		      emsg(_(e_percent_requires_সংখ্যা_arguments));
		      ঘূৰাই FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_সংখ্যা ? ISN_OPNR : ISN_OPANY, 1);
		  যদি (isn != রিক্ত)
		      isn->isn_arg.op.op_type = ৰাশিমালা_REM;
		  ভঙ্গ;
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

    ঘূৰাই OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    স্থির isntype_T
compare_isn_not_মানসমূহ(typval_T *tv, type_T *type)
{
    যদি (tv != রিক্ত)
	(শূন্য)check_typval_is_মান(tv);
    অন্যথা
	(শূন্য)check_type_is_মান(type);
    ঘূৰাই ISN_DROP;
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
    vartype_T	vartype1 = tv1 != রিক্ত ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != রিক্ত ? tv2->v_type : type2->tt_type;

    যদি (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	ঘূৰাই compare_isn_not_মানসমূহ(tv1, type1);
    যদি (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	ঘূৰাই compare_isn_not_মানসমূহ(tv2, type2);

    যদি (vartype1 == vartype2)
    {
	ৱিকল্প (vartype1)
	{
	    পক্ষ VAR_BOOL: isntype = ISN_COMPAREBOOL; ভঙ্গ;
	    পক্ষ VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; ভঙ্গ;
	    পক্ষ VAR_সংখ্যা: isntype = ISN_COMPARENR; ভঙ্গ;
	    পক্ষ VAR_FLOAT: isntype = ISN_COMPAREFLOAT; ভঙ্গ;
	    পক্ষ VAR_STRING: isntype = ISN_COMPARESTRING; ভঙ্গ;
	    পক্ষ VAR_BLOB: isntype = ISN_COMPAREBLOB; ভঙ্গ;
	    পক্ষ VAR_LIST: isntype = ISN_COMPARELIST; ভঙ্গ;
	    পক্ষ VAR_TUPLE: isntype = ISN_COMPARETUPLE; ভঙ্গ;
	    পক্ষ VAR_DICT: isntype = ISN_COMPAREDICT; ভঙ্গ;
	    পক্ষ VAR_FUNC: isntype = ISN_COMPAREFUNC; ভঙ্গ;
	    পক্ষ VAR_OBJECT: isntype = ISN_COMPAREOBJECT; ভঙ্গ;
	    স্ৱভাৱ: isntype = ISN_COMPAREANY; ভঙ্গ;
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
		&& (tv1 != রিক্ত ? tv1->vval.v_সংখ্যা == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != রিক্ত ? tv2->vval.v_সংখ্যা == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    ঘূৰাই ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    যদি ((exprtype == ৰাশিমালা_IS || exprtype == ৰাশিমালা_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == ৰাশিমালা_IS ? "is" : "isnot" , vartype_name(vartype1));
	ঘূৰাই ISN_DROP;
    }
    যদি (!(exprtype == ৰাশিমালা_IS || exprtype == ৰাশিমালা_ISNOT
		|| exprtype == ৰাশিমালা_EQUAL || exprtype == ৰাশিমালা_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	ঘূৰাই ISN_DROP;
    }
    যদি (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != ৰাশিমালা_EQUAL
			&& exprtype != ৰাশিমালা_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != ৰাশিমালা_EQUAL
			 && exprtype != ৰাশিমালা_NEQUAL
			 && exprtype != ৰাশিমালা_IS
			 && exprtype != ৰাশিমালা_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	ঘূৰাই ISN_DROP;
    }
    ঘূৰাই isntype;
}

    পূর্ণাঙ্ক
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    যদি (get_compare_isn(type, tv1, tv2, রিক্ত, রিক্ত) == ISN_DROP)
	ঘূৰাই FAIL;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    পূর্ণাঙ্ক
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, পূর্ণাঙ্ক ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, রিক্ত, রিক্ত,
		       get_type_on_স্তূপ(cctx, 1), get_type_on_স্তূপ(cctx, 0));
    যদি (isntype == ISN_DROP)
	ঘূৰাই FAIL;

    যদি ((isn = generate_instr(cctx, isntype)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --স্তূপ->ga_len;
    set_type_on_স্তূপ(cctx, &t_bool, 0);

    ঘূৰাই OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    পূর্ণাঙ্ক
generate_CONCAT(cctx_T *cctx, পূর্ণাঙ্ক গুনতি)
{
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    RETURN_OK_IF_SKIP(cctx);

    যদি ((isn = generate_instr(cctx, ISN_CONCAT)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = গুনতি;

    // drop the argument types
    স্তূপ->ga_len -= গুনতি - 1;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    পূর্ণাঙ্ক
generate_2BOOL(cctx_T *cctx, পূর্ণাঙ্ক invert, পূর্ণাঙ্ক offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_2BOOL)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_স্তূপ(cctx, &t_bool, -1 - offset);

    ঘূৰাই OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    পূর্ণাঙ্ক
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    যদি (generate_instr(cctx, ISN_COND2BOOL) == রিক্ত)
	ঘূৰাই FAIL;

    // type becomes bool
    set_type_on_স্তূপ(cctx, &t_bool, 0);

    ঘূৰাই OK;
}

    পূর্ণাঙ্ক
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	পূর্ণাঙ্ক	    typechk_flags,	// type check flags
	পূর্ণাঙ্ক	    offset,
	পূর্ণাঙ্ক	    is_var,
	পূর্ণাঙ্ক	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == রিক্ত)
	ঘূৰাই FAIL;
    type_T *tt;
    যদি ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_সংখ্যা_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	যদি (tt != রিক্ত)
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

    ঘূৰাই OK;
}

    পূর্ণাঙ্ক
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_SETTYPE)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    ঘূৰাই OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    পূর্ণাঙ্ক
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    যদি (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == রিক্ত)
	ঘূৰাই FAIL;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    স্থির পূর্ণাঙ্ক
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == রিক্ত ? &t_any : &class->class_type);
    যদি (isn == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.classarg = class;
    যদি (class != রিক্ত)
	++class->class_refcount;
    ঘূৰাই OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    পূর্ণাঙ্ক
generate_tv_ঠেলো(cctx_T *cctx, typval_T *tv)
{
    ৱিকল্প (tv->v_type)
    {
	পক্ষ VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_সংখ্যা);
	    ভঙ্গ;
	পক্ষ VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_সংখ্যা);
	    ভঙ্গ;
	পক্ষ VAR_সংখ্যা:
	    generate_PUSHNR(cctx, tv->vval.v_সংখ্যা);
	    ভঙ্গ;
	পক্ষ VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    ভঙ্গ;
	পক্ষ VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = রিক্ত;
	    ভঙ্গ;
	পক্ষ VAR_LIST:
	    যদি (tv->vval.v_list != রিক্ত)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    ভঙ্গ;
	পক্ষ VAR_TUPLE:
	    যদি (tv->vval.v_tuple != রিক্ত)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    ভঙ্গ;
	পক্ষ VAR_DICT:
	    যদি (tv->vval.v_dict != রিক্ত)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    ভঙ্গ;
#ifdef FEAT_JOB_CHANNEL
	পক্ষ VAR_JOB:
	    যদি (tv->vval.v_job != রিক্ত)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    ভঙ্গ;
	পক্ষ VAR_CHANNEL:
	    যদি (tv->vval.v_channel != রিক্ত)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    ভঙ্গ;
#endif
	পক্ষ VAR_FUNC:
	    যদি (tv->vval.v_string != রিক্ত)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, রিক্ত, &t_func_unknown, TRUE);
	    ভঙ্গ;
	পক্ষ VAR_PARTIAL:
	    যদি (tv->vval.v_partial != রিক্ত)
		iemsg("non-null partial constant not supported");
	    যদি (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == রিক্ত)
		ঘূৰাই FAIL;
	    ভঙ্গ;
	পক্ষ VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = রিক্ত;
	    ভঙ্গ;
	পক্ষ VAR_OBJECT:
	    যদি (tv->vval.v_object != রিক্ত)
	    {
		emsg(_(e_cannot_use_non_null_object));
		ঘূৰাই FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    ভঙ্গ;
	পক্ষ VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    ভঙ্গ;
	স্ৱভাৱ:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    ঘূৰাই FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    পূর্ণাঙ্ক
generate_PUSHNR(cctx_T *cctx, varnumber_T সংখ্যা)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_সংখ্যা)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = সংখ্যা;

    যদি (সংখ্যা == 0 || সংখ্যা == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_স্তূপ(cctx, &t_সংখ্যা_bool, 0);
    ঘূৰাই OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    পূর্ণাঙ্ক
generate_PUSHBOOL(cctx_T *cctx, varnumber_T সংখ্যা)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = সংখ্যা;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    পূর্ণাঙ্ক
generate_PUSHSPEC(cctx_T *cctx, varnumber_T সংখ্যা)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     সংখ্যা == VVAL_NULL ? &t_null : &t_none)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = সংখ্যা;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    পূর্ণাঙ্ক
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.fnumber = fnumber;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    পূর্ণাঙ্ক
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    পূর্ণাঙ্ক		ret = OK;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	যদি ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == রিক্ত)
	    ret = FAIL;
	অন্যথা
	{
	    isn->isn_arg.string = str == রিক্ত ? রিক্ত : *str;
	    ঘূৰাই OK;
	}
    }
    যদি (str != রিক্ত)
	VIM_CLEAR(*str);
    ঘূৰাই ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    পূর্ণাঙ্ক
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    যদি (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == রিক্ত)
	ঘূৰাই FAIL;
    ঘূৰাই OK;
#অন্যথা
    emsg(_(e_channel_job_feature_not_available));
    ঘূৰাই FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    পূর্ণাঙ্ক
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    যদি (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == রিক্ত)
	ঘূৰাই FAIL;
    ঘূৰাই OK;
#অন্যথা
    emsg(_(e_channel_job_feature_not_available));
    ঘূৰাই FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    পূর্ণাঙ্ক
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.blob = blob;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    পূর্ণাঙ্ক
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, পূর্ণাঙ্ক may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == রিক্ত)
	ঘূৰাই FAIL;
    যদি (name == রিক্ত)
	funcname = রিক্ত;
    অন্যথা যদি (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != রিক্ত)	    // autoload
	funcname = vim_strsave(name);
    অন্যথা
    {
	funcname = alloc(STRLEN(name) + 3);
	যদি (funcname != রিক্ত)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    পূর্ণাঙ্ক
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.string = vim_strsave(name);
    যদি (isn->isn_arg.string == রিক্ত)
	ঘূৰাই FAIL;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    পূর্ণাঙ্ক
generate_GETITEM(cctx_T *cctx, পূর্ণাঙ্ক index, পূর্ণাঙ্ক with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_স্তূপ(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    যদি ((isn = generate_instr(cctx, ISN_GETITEM)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    ঘূৰাই ঠেলো_type_স্তূপ(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    স্থির শূন্য
set_tuple_slice_type_on_স্তূপ(type_T *type, পূর্ণাঙ্ক গুনতি, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    যদি ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= গুনতি)
    {
	set_type_on_স্তূপ(cctx, &t_tuple_any, 0);
	ঘূৰাই;
    }

    ga_init2(&tuple_types_ga, পরিমাণ(type_T *), 10);

    বাবে (পূর্ণাঙ্ক i = গুনতি; i < type->tt_argcount; i++)
    {
	যদি (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_স্তূপ(cctx, &t_tuple_any, 0);
	    ঘূৰাই;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] =
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
    পূর্ণাঙ্ক
generate_SLICE(cctx_T *cctx, পূর্ণাঙ্ক গুনতি)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_SLICE)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = গুনতি;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_স্তূপ(cctx, 0);
    যদি (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_স্তূপ(type, গুনতি, cctx);

    ঘূৰাই OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    পূর্ণাঙ্ক
generate_CHECKLEN(cctx_T *cctx, পূর্ণাঙ্ক min_len, পূর্ণাঙ্ক more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    যদি ((isn = generate_instr(cctx, ISN_CHECKLEN)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    পূর্ণাঙ্ক
generate_STORE(cctx_T *cctx, isntype_T isn_type, পূর্ণাঙ্ক idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, isn_type, 1)) == রিক্ত)
	ঘূৰাই FAIL;
    যদি (name != রিক্ত)
	isn->isn_arg.string = vim_strsave(name);
    অন্যথা
	isn->isn_arg.সংখ্যা = idx;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    পূর্ণাঙ্ক
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	পূর্ণাঙ্ক	    load,
	class_T	    *cl,
	পূর্ণাঙ্ক	    idx)
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
    যদি (isn == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    স্থির পূর্ণাঙ্ক
generate_STOREOUTER(
	cctx_T	    *cctx,
	পূর্ণাঙ্ক	    idx,
	পূর্ণাঙ্ক	    level,
	পূর্ণাঙ্ক	    loop_depth,
	পূর্ণাঙ্ক	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == রিক্ত)
	ঘূৰাই FAIL;
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

    ঘূৰাই OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    পূর্ণাঙ্ক
generate_STORENR(cctx_T *cctx, পূর্ণাঙ্ক idx, varnumber_T মান)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_STORENR)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = মান;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    স্থির পূর্ণাঙ্ক
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	পূর্ণাঙ্ক	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, isn_type, 1)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    পূর্ণাঙ্ক
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	পূর্ণাঙ্ক	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type2(cctx, isn_type, type, type)) == রিক্ত)
	ঘূৰাই FAIL;
    যদি (name != রিক্ত)
	isn->isn_arg.string = vim_strsave(name);
    অন্যথা
	isn->isn_arg.সংখ্যা = idx;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    পূর্ণাঙ্ক
generate_LOADOUTER(
	cctx_T	    *cctx,
	পূর্ণাঙ্ক	    idx,
	পূর্ণাঙ্ক	    nesting,
	পূর্ণাঙ্ক	    loop_depth,
	পূর্ণাঙ্ক	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == রিক্ত)
	ঘূৰাই FAIL;
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

    ঘূৰাই OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    পূর্ণাঙ্ক
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    পূর্ণাঙ্ক	    di_flags;
    পূর্ণাঙ্ক	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    যদি (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	ঘূৰাই FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    ঘূৰাই generate_LOAD(cctx, ISN_LOADV, vidx, রিক্ত, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    পূর্ণাঙ্ক
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, পূর্ণাঙ্ক forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, isn_type)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    পূর্ণাঙ্ক
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    যদি (generate_instr(cctx, ISN_LOCKCONST) == রিক্ত)
	ঘূৰাই FAIL;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    পূর্ণাঙ্ক
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	পূর্ণাঙ্ক	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    অন্যথা
	isn = generate_instr_drop(cctx, isn_type, 1);
    যদি (isn == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    পূর্ণাঙ্ক
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	পূর্ণাঙ্ক	    sid,
	পূর্ণাঙ্ক	    idx,
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
    যদি (isn == রিক্ত)
	ঘূৰাই FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    যদি (sref == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    পূর্ণাঙ্ক
generate_NEWLIST(cctx_T *cctx, পূর্ণাঙ্ক গুনতি, পূর্ণাঙ্ক use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_NEWLIST)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = use_null ? -1 : গুনতি;

    // Get the member type and the declared member type from all the items on
    // the stack.
    যদি ((member_type = get_member_type_from_স্তূপ(গুনতি, 1, cctx)) == রিক্ত)
	ঘূৰাই FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_স্তূপ.ga_len -= গুনতি;

    // add the list type to the type stack
    ঘূৰাই ঠেলো_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    পূর্ণাঙ্ক
generate_NEWTUPLE(cctx_T *cctx, পূর্ণাঙ্ক গুনতি, পূর্ণাঙ্ক use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = use_null ? -1 : গুনতি;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, পরিমাণ(type_T *), 10);

    যদি (get_tuple_type_from_স্তূপ(গুনতি, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	ঘূৰাই FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_স্তূপ.ga_len -= গুনতি;

    // add the tuple type to the type stack
    ঘূৰাই ঠেলো_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    পূর্ণাঙ্ক
generate_NEWDICT(cctx_T *cctx, পূর্ণাঙ্ক গুনতি, পূর্ণাঙ্ক use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_NEWDICT)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = use_null ? -1 : গুনতি;

    যদি ((member_type = get_member_type_from_স্তূপ(গুনতি, 2, cctx)) == রিক্ত)
	ঘূৰাই FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_স্তূপ.ga_len -= 2 * গুনতি;

    // add the dict type to the type stack
    ঘূৰাই ঠেলো_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    পূর্ণাঙ্ক
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	পূর্ণাঙ্ক	    object_method,
	পূর্ণাঙ্ক	    fi,
	পূর্ণাঙ্ক	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    পূর্ণাঙ্ক		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_FUNCREF)) == রিক্ত)
	ঘূৰাই FAIL;
    যদি (isn_idx != রিক্ত)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    যদি (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != রিক্ত)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	যদি (extra == রিক্ত)
	    ঘূৰাই FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	যদি (cl != রিক্ত)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    যদি (ufunc->uf_def_status == UF_NOT_COMPILED || cl != রিক্ত)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    যদি (ufunc->uf_def_status != UF_NOT_COMPILED && cl == রিক্ত)
    {
	যদি (isn_idx == রিক্ত && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (শূন্য)compile_def_function(ufunc, FALSE, CT_NONE, রিক্ত);
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

    type = ufunc->uf_func_type == রিক্ত ? &t_func_any : ufunc->uf_func_type;
    ঘূৰাই ঠেলো_type_স্তূপ(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    পূর্ণাঙ্ক
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    পূর্ণাঙ্ক		ret = OK;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	যদি ((isn = generate_instr(cctx, ISN_NEWFUNC)) == রিক্ত)
	    ret = FAIL;
	অন্যথা
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    যদি (arg == রিক্ত)
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
		ঘূৰাই OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    ঘূৰাই ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    পূর্ণাঙ্ক
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_DEF)) == রিক্ত)
	ঘূৰাই FAIL;
    যদি (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	যদি (isn->isn_arg.string == রিক্ত)
	    ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    পূর্ণাঙ্ক
generate_JUMP(cctx_T *cctx, jumpwhen_T when, পূর্ণাঙ্ক where)
{
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_JUMP)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    যদি (when != JUMP_ALWAYS && স্তূপ->ga_len > 0)
	--স্তূপ->ga_len;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    পূর্ণাঙ্ক
generate_WHILE(cctx_T *cctx, পূর্ণাঙ্ক funcref_idx)
{
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_WHILE)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    যদি (স্তূপ->ga_len > 0)
	--স্তূপ->ga_len;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    পূর্ণাঙ্ক
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, পূর্ণাঙ্ক arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, isn_type)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    ঘূৰাই OK;
}

    পূর্ণাঙ্ক
generate_FOR(cctx_T *cctx, পূর্ণাঙ্ক loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_FOR)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    ঘূৰাই ঠেলো_type_স্তূপ(cctx, &t_any);
}

    পূর্ণাঙ্ক
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_ENDLOOP)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_গুনতি;
    isn->isn_arg.endloop.end_var_গুনতি =
			   cctx->ctx_locals.ga_len - loop_info->li_local_গুনতি;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    পূর্ণাঙ্ক
generate_TRYCONT(cctx_T *cctx, পূর্ণাঙ্ক levels, পূর্ণাঙ্ক where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_TRYCONT)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    ঘূৰাই OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    পূর্ণাঙ্ক
check_internal_func_args(
	cctx_T	*cctx,
	পূর্ণাঙ্ক	func_idx,
	পূর্ণাঙ্ক	argcount,
	পূর্ণাঙ্ক	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    পূর্ণাঙ্ক		argoff = check_internal_func(func_idx, argcount);

    যদি (argoff < 0)
	ঘূৰাই FAIL;

    যদি (method_call && argoff > 1)
    {
	যদি (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    ঘূৰাই FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	যদি (isn  == রিক্ত)
	    ঘূৰাই FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    যদি (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)স্তূপ->ga_data) + স্তূপ->ga_len - argcount;

	// Check the types of the arguments.
	যদি (method_call && argoff > 1)
	{
	    পূর্ণাঙ্ক i;

	    বাবে (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক i;

	    বাবে (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	যদি (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    ঘূৰাই FAIL;
    }
    ঘূৰাই OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    পূর্ণাঙ্ক
generate_BCALL(cctx_T *cctx, পূর্ণাঙ্ক func_idx, পূর্ণাঙ্ক argcount, পূর্ণাঙ্ক method_call)
{
    isn_T	*isn;
    garray_T	*স্তূপ = &cctx->ctx_type_স্তূপ;
    type2_T	*argtypes = রিক্ত;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = রিক্ত;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    যদি (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	ঘূৰাই FAIL;

    যদি (internal_func_is_map(func_idx))
	maptype = argtypes;

    যদি ((isn = generate_instr(cctx, ISN_BCALL)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    স্তূপ->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    যদি (ঠেলো_type_stack2(cctx, type, decl_type) == FAIL)
	ঘূৰাই FAIL;

    যদি (maptype != রিক্ত && maptype[0].type_decl->tt_member != রিক্ত
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    ঘূৰাই OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    পূর্ণাঙ্ক
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
	ঘূৰাই FAIL;
    item_type = get_type_on_স্তূপ(cctx, 0);
    expected = list_type->tt_member;
    যদি (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ঘূৰাই FAIL;

    যদি (generate_instr(cctx, ISN_LISTAPPEND) == রিক্ত)
	ঘূৰাই FAIL;

    --cctx->ctx_type_স্তূপ.ga_len;	    // drop the argument
    ঘূৰাই OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    পূর্ণাঙ্ক
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    যদি (arg_type_modifiable(get_decl_type_on_স্তূপ(cctx, 1), 1) == FAIL)
	ঘূৰাই FAIL;
    item_type = get_type_on_স্তূপ(cctx, 0);
    যদি (need_type(item_type, &t_সংখ্যা, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ঘূৰাই FAIL;

    যদি (generate_instr(cctx, ISN_BLOBAPPEND) == রিক্ত)
	ঘূৰাই FAIL;

    --cctx->ctx_type_স্তূপ.ga_len;	    // drop the argument
    ঘূৰাই OK;
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
    যদি (cl != রিক্ত)
	ঘূৰাই ISN_METHODCALL;

    যদি (ufunc->uf_def_status == UF_NOT_COMPILED)
	ঘূৰাই ISN_UCALL;

    // function invoked from a lambda
    যদি (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	যদি (ufunc->uf_name[0] == K_SPECIAL)
	    ঘূৰাই ISN_UCALL;
    }

    ঘূৰাই ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    পূর্ণাঙ্ক
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	পূর্ণাঙ্ক	    mi,
	পূর্ণাঙ্ক	    pushed_argcount,
	পূর্ণাঙ্ক	    is_super)
{
    isn_T	*isn;
    পূর্ণাঙ্ক		regular_args = ufunc->uf_args.ga_len;
    পূর্ণাঙ্ক		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    যদি (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	ঘূৰাই FAIL;
    }
    যদি (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	ঘূৰাই FAIL;
    }

    যদি (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	পূর্ণাঙ্ক		i;
	compiletype_T	compile_type;

	বাবে (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_স্তূপ(cctx, argcount - i - 1);
	    যদি (check_type_is_মান(actual) == FAIL)
		ঘূৰাই FAIL;
	    যদি (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		অনুৱর্ত;
	    }
	    যদি (i < regular_args)
	    {
		যদি (ufunc->uf_arg_types == রিক্ত)
		    অনুৱর্ত;
		expected = ufunc->uf_arg_types[i];
	    }
	    অন্যথা যদি (ufunc->uf_va_type == রিক্ত
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    অন্যথা
		expected = ufunc->uf_va_type->tt_member;
	    যদি (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		ঘূৰাই FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	যদি (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == রিক্ত,
						   compile_type, রিক্ত) == FAIL)
	    ঘূৰাই FAIL;
    }
    যদি (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	ঘূৰাই FAIL;
    }

    যদি ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == রিক্ত)
	ঘূৰাই FAIL;
    যদি (cl != রিক্ত /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	যদি (isn->isn_arg.mfunc == রিক্ত)
	    ঘূৰাই FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = রিক্ত;
	যদি (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = রিক্ত;
	isn->isn_arg.mfunc->cmf_cache_generic = রিক্ত;
	isn->isn_arg.mfunc->cmf_cache_ufunc = রিক্ত;
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
    যদি (ufunc->uf_class != রিক্ত)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_স্তূপ(cctx, 0);
	যদি (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_স্তূপ.ga_len--;
    }

    // add return type
    ঘূৰাই ঠেলো_type_স্তূপ(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    পূর্ণাঙ্ক
generate_UCALL(cctx_T *cctx, char_u *name, পূর্ণাঙ্ক argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_UCALL)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_স্তূপ.ga_len -= argcount;

    // add return value
    ঘূৰাই ঠেলো_type_স্তূপ(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    পূর্ণাঙ্ক
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	পূর্ণাঙ্ক	argcount,
	পূর্ণাঙ্ক	at_শীৰ্ষ,
	char_u	*name)
{
    যদি (type->tt_argcount != -1)
    {
	পূর্ণাঙ্ক	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	যদি (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    ঘূৰাই FAIL;
	}
	যদি (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    ঘূৰাই FAIL;
	}
	যদি (type->tt_args != রিক্ত)
	{
	    পূর্ণাঙ্ক i;

	    বাবে (i = 0; i < argcount; ++i)
	    {
		পূর্ণাঙ্ক	offset = -argcount + i - (at_শীৰ্ষ ? 0 : 1);
		type_T	*actual = get_type_on_স্তূপ(cctx, -1 - offset);
		type_T	*expected;

		যদি (check_type_is_মান(actual) == FAIL)
		    ঘূৰাই FAIL;
		যদি (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    যদি (expected != রিক্ত && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    যদি (expected == রিক্ত)
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
		    ঘূৰাই FAIL;
		}
	    }
	}
    }

    ঘূৰাই OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    পূর্ণাঙ্ক
generate_PCALL(
	cctx_T	*cctx,
	পূর্ণাঙ্ক	argcount,
	char_u	*name,
	type_T	*type,
	পূর্ণাঙ্ক	at_শীৰ্ষ)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    যদি (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    অন্যথা যদি (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	যদি (check_func_args_from_type(cctx, type, argcount, at_শীৰ্ষ, name)
								       == FAIL)
	    ঘূৰাই FAIL;

	ret_type = type->tt_member;
	যদি (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    অন্যথা
    {
	semsg(_(e_not_callable_type_str), name);
	ঘূৰাই FAIL;
    }

    যদি ((isn = generate_instr(cctx, ISN_PCALL)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.pfunc.cpf_শীৰ্ষ = at_শীৰ্ষ;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_স্তূপ.ga_len -= argcount + 1;

    // push the return value
    ঠেলো_type_স্তূপ(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    যদি (at_শীৰ্ষ && generate_instr(cctx, ISN_PCALL_END) == রিক্ত)
	ঘূৰাই FAIL;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    পূর্ণাঙ্ক
generate_DEFER(cctx_T *cctx, পূর্ণাঙ্ক var_idx, পূর্ণাঙ্ক argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    পূর্ণাঙ্ক
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_স্তূপ(cctx, 0);
    যদি (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	ৱর্ণ *tofree;

	semsg(_(e_expected_dictionary_for_using_চাবি_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	ঘূৰাই FAIL;
    }
    // change dict type to dict member type
    যদি (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_স্তূপ(cctx, ntype, 0);
    }

    ঘূৰাই OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    পূর্ণাঙ্ক
generate_ECHO(cctx_T *cctx, পূর্ণাঙ্ক with_white, পূর্ণাঙ্ক গুনতি)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, ISN_ECHO, গুনতি)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_গুনতি = গুনতি;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    পূর্ণাঙ্ক
generate_MULT_ৰাশিমালা(cctx_T *cctx, isntype_T isn_type, পূর্ণাঙ্ক গুনতি)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr_drop(cctx, isn_type, গুনতি)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = গুনতি;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    পূর্ণাঙ্ক
generate_ECHOWINDOW(cctx_T *cctx, পূর্ণাঙ্ক গুনতি, long time)
{
    isn_T	*isn;

    যদি ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, গুনতি)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.echowin.ewin_গুনতি = গুনতি;
    isn->isn_arg.echowin.ewin_time = time;
    ঘূৰাই OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    পূর্ণাঙ্ক
generate_SOURCE(cctx_T *cctx, পূর্ণাঙ্ক sid)
{
    isn_T	*isn;

    যদি ((isn = generate_instr(cctx, ISN_SOURCE)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = sid;

    ঘূৰাই OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    পূর্ণাঙ্ক
generate_PUT(cctx_T *cctx, পূর্ণাঙ্ক regname, linenr_T lnum, পূর্ণাঙ্ক fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    যদি (isn == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    ঘূৰাই OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    পূর্ণাঙ্ক
generate_LOCKUNLOCK(cctx_T *cctx, char_u *সাৰি, পূর্ণাঙ্ক is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == রিক্ত)
	ঘূৰাই FAIL;
    class_T *cl = cctx->ctx_ufunc != রিক্ত ? cctx->ctx_ufunc->uf_class : রিক্ত;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(সাৰি);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    যদি (cl != রিক্ত)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    ঘূৰাই OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    পূর্ণাঙ্ক
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *সাৰি)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, isntype)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.string = vim_strsave(সাৰি);
    ঘূৰাই OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    পূর্ণাঙ্ক
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    পূর্ণাঙ্ক		ret = OK;

    যদি (cctx->ctx_skip != SKIP_YES)
    {
	যদি ((isn = generate_instr(cctx, isntype)) == রিক্ত)
	    ret = FAIL;
	অন্যথা
	{
	    isn->isn_arg.string = str;
	    ঘূৰাই OK;
	}
    }
    vim_free(str);
    ঘূৰাই ret;
}

    পূর্ণাঙ্ক
generate_LEGACY_EVAL(cctx_T *cctx, char_u *সাৰি)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.string = vim_strsave(সাৰি);

    ঘূৰাই ঠেলো_type_স্তূপ(cctx, &t_any);
}

    পূর্ণাঙ্ক
generate_EXECCONCAT(cctx_T *cctx, পূর্ণাঙ্ক গুনতি)
{
    isn_T	*isn;

    যদি ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, গুনতি)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.সংখ্যা = গুনতি;
    ঘূৰাই OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    পূর্ণাঙ্ক
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    যদি ((isn = generate_instr(cctx, ISN_RANGE)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.string = range;

    ঘূৰাই ঠেলো_type_স্তূপ(cctx, &t_সংখ্যা);
}

    পূর্ণাঙ্ক
generate_UNPACK(cctx_T *cctx, পূর্ণাঙ্ক var_গুনতি, পূর্ণাঙ্ক semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_UNPACK)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.unpack.unp_গুনতি = var_গুনতি;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    ঘূৰাই OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    পূর্ণাঙ্ক
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    যদি (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	যদি ((isn = generate_instr(cctx, ISN_CMDMOD)) == রিক্ত)
	    ঘূৰাই FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	যদি (isn->isn_arg.cmdmod.cf_cmdmod == রিক্ত)
	    ঘূৰাই FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, পরিমাণ(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = রিক্ত;
    }

    ঘূৰাই OK;
}

    পূর্ণাঙ্ক
generate_undo_cmdmods(cctx_T *cctx)
{
    যদি (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == রিক্ত)
	ঘূৰাই FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    ঘূৰাই OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    পূর্ণাঙ্ক
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	পূর্ণাঙ্ক		opt_flags,
	পূর্ণাঙ্ক		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    ৱিকল্প (dest)
    {
	পক্ষ dest_option:
	    ঘূৰাই generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	পক্ষ dest_func_option:
	    ঘূৰাই generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	পক্ষ dest_global:
	    // include g: with the name, easier to execute that way
	    ঘূৰাই generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == রিক্ত
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	পক্ষ dest_buffer:
	    // include b: with the name, easier to execute that way
	    ঘূৰাই generate_STORE(cctx, ISN_STOREB, 0, name);
	পক্ষ dest_window:
	    // include w: with the name, easier to execute that way
	    ঘূৰাই generate_STORE(cctx, ISN_STOREW, 0, name);
	পক্ষ dest_tab:
	    // include t: with the name, easier to execute that way
	    ঘূৰাই generate_STORE(cctx, ISN_STORET, 0, name);
	পক্ষ dest_env:
	    ঘূৰাই generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	পক্ষ dest_reg:
	    ঘূৰাই generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], রিক্ত);
	পক্ষ dest_vimvar:
	    ঘূৰাই generate_STORE(cctx, ISN_STOREV, vimvaridx, রিক্ত);
	পক্ষ dest_script:
	পক্ষ dest_script_v9:
	    {
		পূর্ণাঙ্ক	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		পূর্ণাঙ্ক	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		যদি (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    যদি (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == রিক্ত)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			যদি (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    ঘূৰাই FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    ঘূৰাই generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		ঘূৰাই generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	পক্ষ dest_class_member:
	    ঘূৰাই generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	পক্ষ dest_local:
	পক্ষ dest_ৰাশিমালা:
	    // cannot happen
	    ভঙ্গ;
    }
    ঘূৰাই FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    পূর্ণাঙ্ক
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    বাবে (;;)
    {
	যদি (scope == রিক্ত)
	    ভঙ্গ;
	যদি (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    ঘূৰাই TRUE;
	scope = scope->se_outer;
    }
    ঘূৰাই FALSE;
}

    পূর্ণাঙ্ক
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, পূর্ণাঙ্ক instr_গুনতি, পূর্ণাঙ্ক is_decl)
{
    যদি (lhs->lhs_dest != dest_local)
	ঘূৰাই generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    যদি (lhs->lhs_lvar == রিক্ত)
	ঘূৰাই OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

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
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, রিক্ত);
    ঘূৰাই OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    পূর্ণাঙ্ক
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    যদি ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == রিক্ত)
	ঘূৰাই FAIL;
    isn->isn_arg.setsctx = new_sctx;
    ঘূৰাই OK;
}

#যদি defined(FEAT_PROFILE)
    শূন্য
may_generate_prof_end(cctx_T *cctx, পূর্ণাঙ্ক prof_lnum)
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
    ৱিকল্প (isn->isn_type)
    {
	পক্ষ ISN_AUTOLOAD:
	পক্ষ ISN_DEF:
	পক্ষ ISN_EXEC:
	পক্ষ ISN_EXECRANGE:
	পক্ষ ISN_EXEC_SPLIT:
	পক্ষ ISN_LEGACY_EVAL:
	পক্ষ ISN_LOADAUTO:
	পক্ষ ISN_LOADB:
	পক্ষ ISN_LOADENV:
	পক্ষ ISN_LOADG:
	পক্ষ ISN_LOADOPT:
	পক্ষ ISN_LOADT:
	পক্ষ ISN_LOADW:
	পক্ষ ISN_PUSHEXC:
	পক্ষ ISN_PUSHFUNC:
	পক্ষ ISN_PUSHS:
	পক্ষ ISN_RANGE:
	পক্ষ ISN_STOREAUTO:
	পক্ষ ISN_STOREB:
	পক্ষ ISN_STOREENV:
	পক্ষ ISN_STOREG:
	পক্ষ ISN_STORET:
	পক্ষ ISN_STOREW:
	পক্ষ ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    ভঙ্গ;

	পক্ষ ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    ভঙ্গ;

	পক্ষ ISN_SUBSTITUTE:
	    {
		পূর্ণাঙ্ক	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		বাবে (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ভঙ্গ;

	পক্ষ ISN_INSTR:
	    {
		পূর্ণাঙ্ক	idx;
		isn_T	*list = isn->isn_arg.instr;

		বাবে (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ভঙ্গ;

	পক্ষ ISN_LOADS:
	পক্ষ ISN_LOADEXPORT:
	পক্ষ ISN_STORES:
	পক্ষ ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    ভঙ্গ;

	পক্ষ ISN_UNLET:
	পক্ষ ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    ভঙ্গ;

	পক্ষ ISN_STOREOPT:
	পক্ষ ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    ভঙ্গ;

	পক্ষ ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    ভঙ্গ;

	পক্ষ ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    ভঙ্গ;

	পক্ষ ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    ভঙ্গ;

	পক্ষ ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		যদি (extra == রিক্ত || extra->fre_func_name == রিক্ত)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    যদি (ufunc != রিক্ত && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		যদি (extra != রিক্ত)
		{
		    char_u *name = extra->fre_func_name;

		    যদি (name != রিক্ত)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    যদি (extra->fre_class != রিক্ত)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    ভঙ্গ;

	পক্ষ ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		যদি (dfunc->df_ufunc != রিক্ত
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    ভঙ্গ;

	পক্ষ ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    ভঙ্গ;

	পক্ষ ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		যদি (arg != রিক্ত)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    যদি (ufunc != রিক্ত)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    ভঙ্গ;

	পক্ষ ISN_CHECKTYPE:
	পক্ষ ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    ভঙ্গ;

	পক্ষ ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    ভঙ্গ;

	পক্ষ ISN_LOADSCRIPT:
	পক্ষ ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    ভঙ্গ;

	পক্ষ ISN_LOAD_CLASSMEMBER:
	পক্ষ ISN_STORE_CLASSMEMBER:
	পক্ষ ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    ভঙ্গ;

	পক্ষ ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    ভঙ্গ;

	পক্ষ ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    ভঙ্গ;

	পক্ষ ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    ভঙ্গ;

	পক্ষ ISN_2BOOL:
	পক্ষ ISN_2STRING:
	পক্ষ ISN_2STRING_ANY:
	পক্ষ ISN_ADDBLOB:
	পক্ষ ISN_ADDLIST:
	পক্ষ ISN_ADDTUPLE:
	পক্ষ ISN_ANYINDEX:
	পক্ষ ISN_ANYSLICE:
	পক্ষ ISN_BCALL:
	পক্ষ ISN_BLOBAPPEND:
	পক্ষ ISN_BLOBINDEX:
	পক্ষ ISN_BLOBSLICE:
	পক্ষ ISN_CATCH:
	পক্ষ ISN_CEXPR_AUCMD:
	পক্ষ ISN_CHECKLEN:
	পক্ষ ISN_CLEARDICT:
	পক্ষ ISN_CMDMOD_REV:
	পক্ষ ISN_COMPAREANY:
	পক্ষ ISN_COMPAREBLOB:
	পক্ষ ISN_COMPAREBOOL:
	পক্ষ ISN_COMPAREDICT:
	পক্ষ ISN_COMPAREFLOAT:
	পক্ষ ISN_COMPAREFUNC:
	পক্ষ ISN_COMPARELIST:
	পক্ষ ISN_COMPARETUPLE:
	পক্ষ ISN_COMPARENR:
	পক্ষ ISN_COMPARENULL:
	পক্ষ ISN_COMPAREOBJECT:
	পক্ষ ISN_COMPARESPECIAL:
	পক্ষ ISN_COMPARESTRING:
	পক্ষ ISN_CONCAT:
	পক্ষ ISN_CONSTRUCT:
	পক্ষ ISN_COND2BOOL:
	পক্ষ ISN_DEBUG:
	পক্ষ ISN_DEFER:
	পক্ষ ISN_DROP:
	পক্ষ ISN_ECHO:
	পক্ষ ISN_ECHOCONSOLE:
	পক্ষ ISN_ECHOERR:
	পক্ষ ISN_ECHOMSG:
	পক্ষ ISN_ECHOWINDOW:
	পক্ষ ISN_ENDLOOP:
	পক্ষ ISN_ENDTRY:
	পক্ষ ISN_EXECCONCAT:
	পক্ষ ISN_EXECUTE:
	পক্ষ ISN_FINALLY:
	পক্ষ ISN_FINISH:
	পক্ষ ISN_FOR:
	পক্ষ ISN_GETITEM:
	পক্ষ ISN_GET_OBJ_MEMBER:
	পক্ষ ISN_JUMP:
	পক্ষ ISN_JUMP_IF_ARG_NOT_SET:
	পক্ষ ISN_JUMP_IF_ARG_SET:
	পক্ষ ISN_LISTAPPEND:
	পক্ষ ISN_LISTINDEX:
	পক্ষ ISN_LISTSLICE:
	পক্ষ ISN_TUPLEINDEX:
	পক্ষ ISN_TUPLESLICE:
	পক্ষ ISN_LOAD:
	পক্ষ ISN_LOADBDICT:
	পক্ষ ISN_LOADGDICT:
	পক্ষ ISN_LOADOUTER:
	পক্ষ ISN_LOADREG:
	পক্ষ ISN_LOADTDICT:
	পক্ষ ISN_LOADV:
	পক্ষ ISN_LOADWDICT:
	পক্ষ ISN_LOCKCONST:
	পক্ষ ISN_MEMBER:
	পক্ষ ISN_NEGATENR:
	পক্ষ ISN_NEWDICT:
	পক্ষ ISN_NEWLIST:
	পক্ষ ISN_NEWTUPLE:
	পক্ষ ISN_NEWPARTIAL:
	পক্ষ ISN_OPANY:
	পক্ষ ISN_OPFLOAT:
	পক্ষ ISN_OPNR:
	পক্ষ ISN_PCALL:
	পক্ষ ISN_PCALL_END:
	পক্ষ ISN_PROF_END:
	পক্ষ ISN_PROF_START:
	পক্ষ ISN_PUSHBOOL:
	পক্ষ ISN_PUSHCHANNEL:
	পক্ষ ISN_PUSHF:
	পক্ষ ISN_PUSHJOB:
	পক্ষ ISN_PUSHNR:
	পক্ষ ISN_PUSHOBJ:
	পক্ষ ISN_PUSHSPEC:
	পক্ষ ISN_PUT:
	পক্ষ ISN_IPUT:
	পক্ষ ISN_REDIREND:
	পক্ষ ISN_REDIRSTART:
	পক্ষ ISN_RETURN:
	পক্ষ ISN_RETURN_OBJECT:
	পক্ষ ISN_RETURN_VOID:
	পক্ষ ISN_SHUFFLE:
	পক্ষ ISN_SLICE:
	পক্ষ ISN_SOURCE:
	পক্ষ ISN_STORE:
	পক্ষ ISN_STORENR:
	পক্ষ ISN_STOREOUTER:
	পক্ষ ISN_STORE_THIS:
	পক্ষ ISN_STORERANGE:
	পক্ষ ISN_STOREREG:
	পক্ষ ISN_STOREV:
	পক্ষ ISN_STRINDEX:
	পক্ষ ISN_STRSLICE:
	পক্ষ ISN_THROW:
	পক্ষ ISN_TRYCONT:
	পক্ষ ISN_UNLETINDEX:
	পক্ষ ISN_UNLETRANGE:
	পক্ষ ISN_UNPACK:
	পক্ষ ISN_USEDICT:
	পক্ষ ISN_WHILE:
	পক্ষ ISN_SCRIPTCTX_SET:
	// nothing allocated
	ভঙ্গ;
    }
}

    শূন্য
clear_instr_ga(garray_T *gap)
{
    পূর্ণাঙ্ক idx;

    বাবে (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
