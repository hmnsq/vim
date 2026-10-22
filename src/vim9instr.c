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
#include "vim.h"

#якщо defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) якщо (cctx->ctx_skip == SKIP_YES) повернути NULL
#define RETURN_OK_IF_SKIP(cctx) якщо (cctx->ctx_skip == SKIP_YES) повернути OK

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
    якщо (GA_GROW_FAILS(instr, 1))
	повернути NULL;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    повернути isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, ціле drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_stack.ga_len -= drop;
    повернути generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    static isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    якщо ((isn = generate_instr(cctx, isn_type)) == NULL)
	повернути NULL;

    якщо (push_type_stack2(cctx, type == NULL ? &t_any : type,
			       decl_type == NULL ? &t_any : decl_type) == FAIL)
	повернути NULL;

    повернути isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    повернути generate_instr_type2(cctx, isn_type, type, &t_any);
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

    якщо ((isn = generate_instr(cctx, ISN_DEBUG)) == NULL)
	повернути NULL;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    повернути isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    ціле
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == NULL)
	повернути FAIL;
    isn->isn_arg.construct.construct_size = sizeof(object_T)
			       + cl->class_obj_member_count * sizeof(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    повернути OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    ціле
generate_GET_OBJ_MEMBER(cctx_T *cctx, ціле idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    якщо (isn == NULL)
	повернути FAIL;

    isn->isn_arg.classmember.cm_class = NULL;
    isn->isn_arg.classmember.cm_idx = idx;
    повернути push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    ціле
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, ціле idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    якщо (isn == NULL)
	повернути FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    повернути push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    ціле
generate_STORE_THIS(cctx_T *cctx, ціле idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    якщо (isn == NULL)
	повернути FAIL;

    isn->isn_arg.number = idx;
    повернути OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    ціле
may_generate_2STRING(ціле offset, ціле tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_stack(cctx, -1 - offset);

    switch (type->tt_type)
    {
	// nothing to be done
	case VAR_STRING: повернути OK;

	// conversion possible
	case VAR_SPECIAL:
	case VAR_BOOL:
	case VAR_NUMBER:
	case VAR_FLOAT:
			 break;

	// conversion possible (with runtime check)
	case VAR_ANY:
	case VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 break;

	// conversion possible when tolerant
	case VAR_LIST:
	case VAR_TUPLE:
	case VAR_DICT:
			 якщо (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     break;
			 }
			 якщо (tostring_flags & TOSTRING_INTERPOLATE)
			     break;
			 // FALLTHROUGH

	// conversion not possible
	case VAR_VOID:
	case VAR_BLOB:
	case VAR_FUNC:
	case VAR_PARTIAL:
	case VAR_JOB:
	case VAR_CHANNEL:
	case VAR_INSTR:
	case VAR_CLASS:
	case VAR_OBJECT:
	case VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 повернути FAIL;
    }

    set_type_on_stack(cctx, &t_string, -1 - offset);
    якщо ((isn = generate_instr(cctx, isntype)) == NULL)
	повернути FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    повернути OK;
}

    static ціле
check_number_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    якщо (!((type1 == VAR_NUMBER || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_NUMBER || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	якщо (check_type_is_value(typ1) == FAIL
		|| check_type_is_value(typ2) == FAIL)
	    повернути FAIL;
	якщо (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	інакше
	    semsg(_(e_char_requires_number_or_float_arguments), *op);
	повернути FAIL;
    }
    повернути OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    static ціле
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    для (ціле i = 0; i < tuple_type->tt_argcount; i++)
    {
	якщо (ga_grow(gap, 1) == FAIL)
	    повернути FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    повернути OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    static ціле
set_tuple_union_type_on_stack(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    якщо (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    якщо (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    якщо ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_stack(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    повернути OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    ціле
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  expr_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_NUMBER ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    якщо (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_number_or_float(type1, type2, (char_u *)"+") == FAIL)
	повернути FAIL;

    якщо (isn != NULL)
    {
	якщо (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = expr_type;
	інакше
	    isn->isn_arg.op.op_type = EXPR_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    якщо (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_stack(cctx, &t_list_any, 0);
    інакше якщо (vartype == VAR_TUPLE)
    {
	якщо (!check_tuples_addable(type1, type2))
	    повернути FAIL;

	якщо (set_tuple_union_type_on_stack(type1, type2, cctx) == FAIL)
	    повернути FAIL;
    }

    повернути isn == NULL ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    якщо (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_NUMBER
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	повернути type1->tt_type;
    повернути VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    ціле
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_stack(cctx, 1);
    type2 = get_type_on_stack(cctx, 0);
    vartype = operator_type(type1, type2);

    switch (*op)
    {
	case '+':
		  якщо (generate_add_instr(cctx, vartype, type1, type2,
							    EXPR_COPY) == FAIL)
		      повернути FAIL;
		  break;

	case '-':
	case '*':
	case '/': якщо (check_number_or_float(type1, type2, op) == FAIL)
		      повернути FAIL;
		  якщо (vartype == VAR_NUMBER)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  інакше якщо (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  інакше
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  якщо (isn != NULL)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? EXPR_MULT : *op == '/'? EXPR_DIV : EXPR_SUB;
		  break;

	case '%': якщо ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_NUMBER)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_NUMBER))
		  {
		      emsg(_(e_percent_requires_number_arguments));
		      повернути FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_NUMBER ? ISN_OPNR : ISN_OPANY, 1);
		  якщо (isn != NULL)
		      isn->isn_arg.op.op_type = EXPR_REM;
		  break;
    }

    // correct type of result
    якщо (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	якщо ((type1->tt_type == VAR_NUMBER || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_NUMBER || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_stack(cctx, type, 0);
    }

    повернути OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    static isntype_T
compare_isn_not_values(typval_T *tv, type_T *type)
{
    якщо (tv != NULL)
	(void)check_typval_is_value(tv);
    інакше
	(void)check_type_is_value(type);
    повернути ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    static isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != NULL ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != NULL ? tv2->v_type : type2->tt_type;

    якщо (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	повернути compare_isn_not_values(tv1, type1);
    якщо (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	повернути compare_isn_not_values(tv2, type2);

    якщо (vartype1 == vartype2)
    {
	switch (vartype1)
	{
	    case VAR_BOOL: isntype = ISN_COMPAREBOOL; break;
	    case VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; break;
	    case VAR_NUMBER: isntype = ISN_COMPARENR; break;
	    case VAR_FLOAT: isntype = ISN_COMPAREFLOAT; break;
	    case VAR_STRING: isntype = ISN_COMPARESTRING; break;
	    case VAR_BLOB: isntype = ISN_COMPAREBLOB; break;
	    case VAR_LIST: isntype = ISN_COMPARELIST; break;
	    case VAR_TUPLE: isntype = ISN_COMPARETUPLE; break;
	    case VAR_DICT: isntype = ISN_COMPAREDICT; break;
	    case VAR_FUNC: isntype = ISN_COMPAREFUNC; break;
	    case VAR_OBJECT: isntype = ISN_COMPAREOBJECT; break;
	    default: isntype = ISN_COMPAREANY; break;
	}
    }
    інакше якщо (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_NUMBER || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_NUMBER || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    інакше якщо (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	якщо ((vartype1 == VAR_SPECIAL
		&& (tv1 != NULL ? tv1->vval.v_number == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != NULL ? tv2->vval.v_number == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    повернути ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    якщо ((exprtype == EXPR_IS || exprtype == EXPR_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == EXPR_IS ? "is" : "isnot" , vartype_name(vartype1));
	повернути ISN_DROP;
    }
    якщо (!(exprtype == EXPR_IS || exprtype == EXPR_ISNOT
		|| exprtype == EXPR_EQUAL || exprtype == EXPR_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	повернути ISN_DROP;
    }
    якщо (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != EXPR_EQUAL
			&& exprtype != EXPR_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != EXPR_EQUAL
			 && exprtype != EXPR_NEQUAL
			 && exprtype != EXPR_IS
			 && exprtype != EXPR_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	повернути ISN_DROP;
    }
    повернути isntype;
}

    ціле
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    якщо (get_compare_isn(type, tv1, tv2, NULL, NULL) == ISN_DROP)
	повернути FAIL;
    повернути OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    ціле
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, ціле ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, NULL, NULL,
		       get_type_on_stack(cctx, 1), get_type_on_stack(cctx, 0));
    якщо (isntype == ISN_DROP)
	повернути FAIL;

    якщо ((isn = generate_instr(cctx, isntype)) == NULL)
	повернути FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --stack->ga_len;
    set_type_on_stack(cctx, &t_bool, 0);

    повернути OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    ціле
generate_CONCAT(cctx_T *cctx, ціле count)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);

    якщо ((isn = generate_instr(cctx, ISN_CONCAT)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = count;

    // drop the argument types
    stack->ga_len -= count - 1;

    повернути OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    ціле
generate_2BOOL(cctx_T *cctx, ціле invert, ціле offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_2BOOL)) == NULL)
	повернути FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, -1 - offset);

    повернути OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    ціле
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    якщо (generate_instr(cctx, ISN_COND2BOOL) == NULL)
	повернути FAIL;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, 0);

    повернути OK;
}

    ціле
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	ціле	    typechk_flags,	// type check flags
	ціле	    offset,
	ціле	    is_var,
	ціле	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == NULL)
	повернути FAIL;
    type_T *tt;
    якщо ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_NUMBER_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	якщо (tt != NULL)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    якщо (typechk_flags & TYPECHK_NUMBER_OK)
		tt->tt_flags |= TTFLAG_NUMBER_OK;
	    інакше
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    інакше
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_stack(cctx, expected, -1 - offset);

    повернути OK;
}

    ціле
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_SETTYPE)) == NULL)
	повернути FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    повернути OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    ціле
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    якщо (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == NULL)
	повернути FAIL;
    повернути OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    static ціле
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == NULL ? &t_any : &class->class_type);
    якщо (isn == NULL)
	повернути FAIL;
    isn->isn_arg.classarg = class;
    якщо (class != NULL)
	++class->class_refcount;
    повернути OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    ціле
generate_tv_PUSH(cctx_T *cctx, typval_T *tv)
{
    switch (tv->v_type)
    {
	case VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_number);
	    break;
	case VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_number);
	    break;
	case VAR_NUMBER:
	    generate_PUSHNR(cctx, tv->vval.v_number);
	    break;
	case VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    break;
	case VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = NULL;
	    break;
	case VAR_LIST:
	    якщо (tv->vval.v_list != NULL)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    break;
	case VAR_TUPLE:
	    якщо (tv->vval.v_tuple != NULL)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    break;
	case VAR_DICT:
	    якщо (tv->vval.v_dict != NULL)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    break;
#ifdef FEAT_JOB_CHANNEL
	case VAR_JOB:
	    якщо (tv->vval.v_job != NULL)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    break;
	case VAR_CHANNEL:
	    якщо (tv->vval.v_channel != NULL)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    break;
#endif
	case VAR_FUNC:
	    якщо (tv->vval.v_string != NULL)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, NULL, &t_func_unknown, TRUE);
	    break;
	case VAR_PARTIAL:
	    якщо (tv->vval.v_partial != NULL)
		iemsg("non-null partial constant not supported");
	    якщо (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == NULL)
		повернути FAIL;
	    break;
	case VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = NULL;
	    break;
	case VAR_OBJECT:
	    якщо (tv->vval.v_object != NULL)
	    {
		emsg(_(e_cannot_use_non_null_object));
		повернути FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    break;
	case VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    break;
	default:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    повернути FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    повернути OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    ціле
generate_PUSHNR(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_number)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = number;

    якщо (number == 0 || number == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_stack(cctx, &t_number_bool, 0);
    повернути OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    ціле
generate_PUSHBOOL(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = number;

    повернути OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    ціле
generate_PUSHSPEC(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     number == VVAL_NULL ? &t_null : &t_none)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = number;

    повернути OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    ціле
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == NULL)
	повернути FAIL;
    isn->isn_arg.fnumber = fnumber;

    повернути OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    ціле
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    ціле		ret = OK;

    якщо (cctx->ctx_skip != SKIP_YES)
    {
	якщо ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == NULL)
	    ret = FAIL;
	інакше
	{
	    isn->isn_arg.string = str == NULL ? NULL : *str;
	    повернути OK;
	}
    }
    якщо (str != NULL)
	VIM_CLEAR(*str);
    повернути ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    ціле
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    якщо (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == NULL)
	повернути FAIL;
    повернути OK;
#інакше
    emsg(_(e_channel_job_feature_not_available));
    повернути FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    ціле
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    якщо (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == NULL)
	повернути FAIL;
    повернути OK;
#інакше
    emsg(_(e_channel_job_feature_not_available));
    повернути FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    ціле
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == NULL)
	повернути FAIL;
    isn->isn_arg.blob = blob;

    повернути OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    ціле
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, ціле may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == NULL)
	повернути FAIL;
    якщо (name == NULL)
	funcname = NULL;
    інакше якщо (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != NULL)	    // autoload
	funcname = vim_strsave(name);
    інакше
    {
	funcname = alloc(STRLEN(name) + 3);
	якщо (funcname != NULL)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    повернути OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    ціле
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == NULL)
	повернути FAIL;
    isn->isn_arg.string = vim_strsave(name);
    якщо (isn->isn_arg.string == NULL)
	повернути FAIL;
    повернути OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    ціле
generate_GETITEM(cctx_T *cctx, ціле index, ціле with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_stack(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    якщо ((isn = generate_instr(cctx, ISN_GETITEM)) == NULL)
	повернути FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    повернути push_type_stack(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    static void
set_tuple_slice_type_on_stack(type_T *type, ціле count, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    якщо ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= count)
    {
	set_type_on_stack(cctx, &t_tuple_any, 0);
	повернути;
    }

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    для (ціле i = count; i < type->tt_argcount; i++)
    {
	якщо (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_stack(cctx, &t_tuple_any, 0);
	    повернути;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] =
							     type->tt_args[i];
	tuple_types_ga.ga_len++;
    }

    set_type_on_stack(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    ціле
generate_SLICE(cctx_T *cctx, ціле count)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_SLICE)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = count;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_stack(cctx, 0);
    якщо (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_stack(type, count, cctx);

    повернути OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    ціле
generate_CHECKLEN(cctx_T *cctx, ціле min_len, ціле more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    якщо ((isn = generate_instr(cctx, ISN_CHECKLEN)) == NULL)
	повернути FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    повернути OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    ціле
generate_STORE(cctx_T *cctx, isntype_T isn_type, ціле idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	повернути FAIL;
    якщо (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    інакше
	isn->isn_arg.number = idx;

    повернути OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    ціле
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	ціле	    load,
	class_T	    *cl,
	ціле	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    інакше
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    якщо (isn == NULL)
	повернути FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    повернути OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    static ціле
generate_STOREOUTER(
	cctx_T	    *cctx,
	ціле	    idx,
	ціле	    level,
	ціле	    loop_depth,
	ціле	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == NULL)
	повернути FAIL;
    якщо (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    інакше
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    повернути OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    ціле
generate_STORENR(cctx_T *cctx, ціле idx, varnumber_T value)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_STORENR)) == NULL)
	повернути FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = value;

    повернути OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    static ціле
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ціле	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	повернути FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    повернути OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    ціле
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ціле	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_type2(cctx, isn_type, type, type)) == NULL)
	повернути FAIL;
    якщо (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    інакше
	isn->isn_arg.number = idx;

    повернути OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    ціле
generate_LOADOUTER(
	cctx_T	    *cctx,
	ціле	    idx,
	ціле	    nesting,
	ціле	    loop_depth,
	ціле	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == NULL)
	повернути FAIL;
    якщо (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    інакше
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    повернути OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    ціле
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    ціле	    di_flags;
    ціле	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    якщо (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	повернути FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    повернути generate_LOAD(cctx, ISN_LOADV, vidx, NULL, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    ціле
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, ціле forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, isn_type)) == NULL)
	повернути FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    повернути OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    ціле
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    якщо (generate_instr(cctx, ISN_LOCKCONST) == NULL)
	повернути FAIL;
    повернути OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    ціле
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ціле	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    інакше
	isn = generate_instr_drop(cctx, isn_type, 1);
    якщо (isn == NULL)
	повернути FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    повернути OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    ціле
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ціле	    sid,
	ціле	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    якщо (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    інакше
	isn = generate_instr_drop(cctx, isn_type, 1);
    якщо (isn == NULL)
	повернути FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    якщо (sref == NULL)
	повернути FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    повернути OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    ціле
generate_NEWLIST(cctx_T *cctx, ціле count, ціле use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_NEWLIST)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    якщо ((member_type = get_member_type_from_stack(count, 1, cctx)) == NULL)
	повернути FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the list type to the type stack
    повернути push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    ціле
generate_NEWTUPLE(cctx_T *cctx, ціле count, ціле use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    якщо (get_tuple_type_from_stack(count, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	повернути FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the tuple type to the type stack
    повернути push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    ціле
generate_NEWDICT(cctx_T *cctx, ціле count, ціле use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_NEWDICT)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    якщо ((member_type = get_member_type_from_stack(count, 2, cctx)) == NULL)
	повернути FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_stack.ga_len -= 2 * count;

    // add the dict type to the type stack
    повернути push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    ціле
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ціле	    object_method,
	ціле	    fi,
	ціле	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    ціле		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_FUNCREF)) == NULL)
	повернути FAIL;
    якщо (isn_idx != NULL)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    якщо (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != NULL)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	якщо (extra == NULL)
	    повернути FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	якщо (cl != NULL)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    якщо (ufunc->uf_def_status == UF_NOT_COMPILED || cl != NULL)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    якщо (ufunc->uf_def_status != UF_NOT_COMPILED && cl == NULL)
    {
	якщо (isn_idx == NULL && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (void)compile_def_function(ufunc, FALSE, CT_NONE, NULL);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_closure = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    якщо ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == NULL ? &t_func_any : ufunc->uf_func_type;
    повернути push_type_stack(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    ціле
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    ціле		ret = OK;

    якщо (cctx->ctx_skip != SKIP_YES)
    {
	якщо ((isn = generate_instr(cctx, ISN_NEWFUNC)) == NULL)
	    ret = FAIL;
	інакше
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    якщо (arg == NULL)
		ret = FAIL;
	    інакше
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(void)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		повернути OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    повернути ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    ціле
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_DEF)) == NULL)
	повернути FAIL;
    якщо (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	якщо (isn->isn_arg.string == NULL)
	    повернути FAIL;
    }
    повернути OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    ціле
generate_JUMP(cctx_T *cctx, jumpwhen_T when, ціле where)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_JUMP)) == NULL)
	повернути FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    якщо (when != JUMP_ALWAYS && stack->ga_len > 0)
	--stack->ga_len;

    повернути OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    ціле
generate_WHILE(cctx_T *cctx, ціле funcref_idx)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_WHILE)) == NULL)
	повернути FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    якщо (stack->ga_len > 0)
	--stack->ga_len;

    повернути OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    ціле
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, ціле arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, isn_type)) == NULL)
	повернути FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    повернути OK;
}

    ціле
generate_FOR(cctx_T *cctx, ціле loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_FOR)) == NULL)
	повернути FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    повернути push_type_stack(cctx, &t_any);
}

    ціле
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_ENDLOOP)) == NULL)
	повернути FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_count;
    isn->isn_arg.endloop.end_var_count =
			   cctx->ctx_locals.ga_len - loop_info->li_local_count;
    повернути OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    ціле
generate_TRYCONT(cctx_T *cctx, ціле levels, ціле where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_TRYCONT)) == NULL)
	повернути FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    повернути OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    ціле
check_internal_func_args(
	cctx_T	*cctx,
	ціле	func_idx,
	ціле	argcount,
	ціле	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    ціле		argoff = check_internal_func(func_idx, argcount);

    якщо (argoff < 0)
	повернути FAIL;

    якщо (method_call && argoff > 1)
    {
	якщо (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    повернути FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	якщо (isn  == NULL)
	    повернути FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    якщо (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)stack->ga_data) + stack->ga_len - argcount;

	// Check the types of the arguments.
	якщо (method_call && argoff > 1)
	{
	    ціле i;

	    для (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	інакше
	{
	    ціле i;

	    для (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	якщо (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    повернути FAIL;
    }
    повернути OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    ціле
generate_BCALL(cctx_T *cctx, ціле func_idx, ціле argcount, ціле method_call)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*argtypes = NULL;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = NULL;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    якщо (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	повернути FAIL;

    якщо (internal_func_is_map(func_idx))
	maptype = argtypes;

    якщо ((isn = generate_instr(cctx, ISN_BCALL)) == NULL)
	повернути FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    stack->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    якщо (push_type_stack2(cctx, type, decl_type) == FAIL)
	повернути FAIL;

    якщо (maptype != NULL && maptype[0].type_decl->tt_member != NULL
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    повернути OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ціле
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_stack(cctx, 1);
    якщо (arg_type_modifiable(list_type, 1) == FAIL)
	повернути FAIL;
    item_type = get_type_on_stack(cctx, 0);
    expected = list_type->tt_member;
    якщо (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	повернути FAIL;

    якщо (generate_instr(cctx, ISN_LISTAPPEND) == NULL)
	повернути FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    повернути OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ціле
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    якщо (arg_type_modifiable(get_decl_type_on_stack(cctx, 1), 1) == FAIL)
	повернути FAIL;
    item_type = get_type_on_stack(cctx, 0);
    якщо (need_type(item_type, &t_number, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	повернути FAIL;

    якщо (generate_instr(cctx, ISN_BLOBAPPEND) == NULL)
	повернути FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    повернути OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    static isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    якщо (cl != NULL)
	повернути ISN_METHODCALL;

    якщо (ufunc->uf_def_status == UF_NOT_COMPILED)
	повернути ISN_UCALL;

    // function invoked from a lambda
    якщо (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	якщо (ufunc->uf_name[0] == K_SPECIAL)
	    повернути ISN_UCALL;
    }

    повернути ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    ціле
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ціле	    mi,
	ціле	    pushed_argcount,
	ціле	    is_super)
{
    isn_T	*isn;
    ціле		regular_args = ufunc->uf_args.ga_len;
    ціле		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    якщо (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	повернути FAIL;
    }
    якщо (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	повернути FAIL;
    }

    якщо (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	ціле		i;
	compiletype_T	compile_type;

	для (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_stack(cctx, argcount - i - 1);
	    якщо (check_type_is_value(actual) == FAIL)
		повернути FAIL;
	    якщо (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		continue;
	    }
	    якщо (i < regular_args)
	    {
		якщо (ufunc->uf_arg_types == NULL)
		    continue;
		expected = ufunc->uf_arg_types[i];
	    }
	    інакше якщо (ufunc->uf_va_type == NULL
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    інакше
		expected = ufunc->uf_va_type->tt_member;
	    якщо (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		повернути FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	якщо (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == NULL,
						   compile_type, NULL) == FAIL)
	    повернути FAIL;
    }
    якщо (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	повернути FAIL;
    }

    якщо ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == NULL)
	повернути FAIL;
    якщо (cl != NULL /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	якщо (isn->isn_arg.mfunc == NULL)
	    повернути FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = NULL;
	якщо (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = NULL;
	isn->isn_arg.mfunc->cmf_cache_generic = NULL;
	isn->isn_arg.mfunc->cmf_cache_ufunc = NULL;
    }
    інакше якщо (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    інакше
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    якщо (ufunc->uf_class != NULL)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_stack(cctx, 0);
	якщо (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_stack.ga_len--;
    }

    // add return type
    повернути push_type_stack(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    ціле
generate_UCALL(cctx_T *cctx, char_u *name, ціле argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_UCALL)) == NULL)
	повернути FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // add return value
    повернути push_type_stack(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    ціле
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	ціле	argcount,
	ціле	at_top,
	char_u	*name)
{
    якщо (type->tt_argcount != -1)
    {
	ціле	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	якщо (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    повернути FAIL;
	}
	якщо (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    повернути FAIL;
	}
	якщо (type->tt_args != NULL)
	{
	    ціле i;

	    для (i = 0; i < argcount; ++i)
	    {
		ціле	offset = -argcount + i - (at_top ? 0 : 1);
		type_T	*actual = get_type_on_stack(cctx, -1 - offset);
		type_T	*expected;

		якщо (check_type_is_value(actual) == FAIL)
		    повернути FAIL;
		якщо (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    якщо (expected != NULL && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    якщо (expected == NULL)
			expected = &t_any;
		}
		інакше якщо (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		інакше
		    expected = type->tt_args[i];
		якщо (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    повернути FAIL;
		}
	    }
	}
    }

    повернути OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    ціле
generate_PCALL(
	cctx_T	*cctx,
	ціле	argcount,
	char_u	*name,
	type_T	*type,
	ціле	at_top)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    якщо (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    інакше якщо (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	якщо (check_func_args_from_type(cctx, type, argcount, at_top, name)
								       == FAIL)
	    повернути FAIL;

	ret_type = type->tt_member;
	якщо (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    інакше
    {
	semsg(_(e_not_callable_type_str), name);
	повернути FAIL;
    }

    якщо ((isn = generate_instr(cctx, ISN_PCALL)) == NULL)
	повернути FAIL;
    isn->isn_arg.pfunc.cpf_top = at_top;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_stack.ga_len -= argcount + 1;

    // push the return value
    push_type_stack(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    якщо (at_top && generate_instr(cctx, ISN_PCALL_END) == NULL)
	повернути FAIL;

    повернути OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    ціле
generate_DEFER(cctx_T *cctx, ціле var_idx, ціле argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == NULL)
	повернути FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    повернути OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    ціле
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == NULL)
	повернути FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_stack(cctx, 0);
    якщо (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	char *tofree;

	semsg(_(e_expected_dictionary_for_using_key_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	повернути FAIL;
    }
    // change dict type to dict member type
    якщо (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_stack(cctx, ntype, 0);
    }

    повернути OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    ціле
generate_ECHO(cctx_T *cctx, ціле with_white, ціле count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_drop(cctx, ISN_ECHO, count)) == NULL)
	повернути FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_count = count;

    повернути OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    ціле
generate_MULT_EXPR(cctx_T *cctx, isntype_T isn_type, ціле count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr_drop(cctx, isn_type, count)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = count;
    повернути OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    ціле
generate_ECHOWINDOW(cctx_T *cctx, ціле count, long time)
{
    isn_T	*isn;

    якщо ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, count)) == NULL)
	повернути FAIL;
    isn->isn_arg.echowin.ewin_count = count;
    isn->isn_arg.echowin.ewin_time = time;
    повернути OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    ціле
generate_SOURCE(cctx_T *cctx, ціле sid)
{
    isn_T	*isn;

    якщо ((isn = generate_instr(cctx, ISN_SOURCE)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = sid;

    повернути OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    ціле
generate_PUT(cctx_T *cctx, ціле regname, linenr_T lnum, ціле fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    якщо (isn == NULL)
	повернути FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    повернути OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    ціле
generate_LOCKUNLOCK(cctx_T *cctx, char_u *line, ціле is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == NULL)
	повернути FAIL;
    class_T *cl = cctx->ctx_ufunc != NULL ? cctx->ctx_ufunc->uf_class : NULL;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(line);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    якщо (cl != NULL)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    повернути OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    ціле
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, isntype)) == NULL)
	повернути FAIL;
    isn->isn_arg.string = vim_strsave(line);
    повернути OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    ціле
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    ціле		ret = OK;

    якщо (cctx->ctx_skip != SKIP_YES)
    {
	якщо ((isn = generate_instr(cctx, isntype)) == NULL)
	    ret = FAIL;
	інакше
	{
	    isn->isn_arg.string = str;
	    повернути OK;
	}
    }
    vim_free(str);
    повернути ret;
}

    ціле
generate_LEGACY_EVAL(cctx_T *cctx, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == NULL)
	повернути FAIL;
    isn->isn_arg.string = vim_strsave(line);

    повернути push_type_stack(cctx, &t_any);
}

    ціле
generate_EXECCONCAT(cctx_T *cctx, ціле count)
{
    isn_T	*isn;

    якщо ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, count)) == NULL)
	повернути FAIL;
    isn->isn_arg.number = count;
    повернути OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    ціле
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    якщо ((isn = generate_instr(cctx, ISN_RANGE)) == NULL)
	повернути FAIL;
    isn->isn_arg.string = range;

    повернути push_type_stack(cctx, &t_number);
}

    ціле
generate_UNPACK(cctx_T *cctx, ціле var_count, ціле semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_UNPACK)) == NULL)
	повернути FAIL;
    isn->isn_arg.unpack.unp_count = var_count;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    повернути OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    ціле
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    якщо (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	якщо ((isn = generate_instr(cctx, ISN_CMDMOD)) == NULL)
	    повернути FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	якщо (isn->isn_arg.cmdmod.cf_cmdmod == NULL)
	    повернути FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, sizeof(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = NULL;
    }

    повернути OK;
}

    ціле
generate_undo_cmdmods(cctx_T *cctx)
{
    якщо (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == NULL)
	повернути FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    повернути OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    ціле
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	ціле		opt_flags,
	ціле		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    switch (dest)
    {
	case dest_option:
	    повернути generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	case dest_func_option:
	    повернути generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	case dest_global:
	    // include g: with the name, easier to execute that way
	    повернути generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == NULL
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	case dest_buffer:
	    // include b: with the name, easier to execute that way
	    повернути generate_STORE(cctx, ISN_STOREB, 0, name);
	case dest_window:
	    // include w: with the name, easier to execute that way
	    повернути generate_STORE(cctx, ISN_STOREW, 0, name);
	case dest_tab:
	    // include t: with the name, easier to execute that way
	    повернути generate_STORE(cctx, ISN_STORET, 0, name);
	case dest_env:
	    повернути generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	case dest_reg:
	    повернути generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], NULL);
	case dest_vimvar:
	    повернути generate_STORE(cctx, ISN_STOREV, vimvaridx, NULL);
	case dest_script:
	case dest_script_v9:
	    {
		ціле	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		ціле	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		якщо (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    якщо (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == NULL)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			якщо (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    повернути FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    повернути generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		повернути generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	case dest_class_member:
	    повернути generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	case dest_local:
	case dest_expr:
	    // cannot happen
	    break;
    }
    повернути FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    ціле
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    для (;;)
    {
	якщо (scope == NULL)
	    break;
	якщо (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    повернути TRUE;
	scope = scope->se_outer;
    }
    повернути FALSE;
}

    ціле
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, ціле instr_count, ціле is_decl)
{
    якщо (lhs->lhs_dest != dest_local)
	повернути generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    якщо (lhs->lhs_lvar == NULL)
	повернути OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    якщо (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_count + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.number;
	garray_T    *stack = &cctx->ctx_type_stack;

	якщо (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	інакше
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	якщо (stack->ga_len > 0)
	    --stack->ga_len;
    }
    інакше якщо (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    інакше
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, NULL);
    повернути OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    ціле
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    якщо ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == NULL)
	повернути FAIL;
    isn->isn_arg.setsctx = new_sctx;
    повернути OK;
}

#якщо defined(FEAT_PROFILE)
    void
may_generate_prof_end(cctx_T *cctx, ціле prof_lnum)
{
    якщо (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_END);
}
#endif


/*
 * Delete an instruction, free what it contains.
 */
    void
delete_instr(isn_T *isn)
{
    switch (isn->isn_type)
    {
	case ISN_AUTOLOAD:
	case ISN_DEF:
	case ISN_EXEC:
	case ISN_EXECRANGE:
	case ISN_EXEC_SPLIT:
	case ISN_LEGACY_EVAL:
	case ISN_LOADAUTO:
	case ISN_LOADB:
	case ISN_LOADENV:
	case ISN_LOADG:
	case ISN_LOADOPT:
	case ISN_LOADT:
	case ISN_LOADW:
	case ISN_PUSHEXC:
	case ISN_PUSHFUNC:
	case ISN_PUSHS:
	case ISN_RANGE:
	case ISN_STOREAUTO:
	case ISN_STOREB:
	case ISN_STOREENV:
	case ISN_STOREG:
	case ISN_STORET:
	case ISN_STOREW:
	case ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    break;

	case ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    break;

	case ISN_SUBSTITUTE:
	    {
		ціле	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		для (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    break;

	case ISN_INSTR:
	    {
		ціле	idx;
		isn_T	*list = isn->isn_arg.instr;

		для (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    break;

	case ISN_LOADS:
	case ISN_LOADEXPORT:
	case ISN_STORES:
	case ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    break;

	case ISN_UNLET:
	case ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    break;

	case ISN_STOREOPT:
	case ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    break;

	case ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    break;

	case ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    break;

	case ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    break;

	case ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		якщо (extra == NULL || extra->fre_func_name == NULL)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    якщо (ufunc != NULL && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		якщо (extra != NULL)
		{
		    char_u *name = extra->fre_func_name;

		    якщо (name != NULL)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    якщо (extra->fre_class != NULL)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    break;

	case ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		якщо (dfunc->df_ufunc != NULL
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    break;

	case ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    break;

	case ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		якщо (arg != NULL)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    якщо (ufunc != NULL)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    break;

	case ISN_CHECKTYPE:
	case ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    break;

	case ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    break;

	case ISN_LOADSCRIPT:
	case ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    break;

	case ISN_LOAD_CLASSMEMBER:
	case ISN_STORE_CLASSMEMBER:
	case ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    break;

	case ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    break;

	case ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    break;

	case ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    break;

	case ISN_2BOOL:
	case ISN_2STRING:
	case ISN_2STRING_ANY:
	case ISN_ADDBLOB:
	case ISN_ADDLIST:
	case ISN_ADDTUPLE:
	case ISN_ANYINDEX:
	case ISN_ANYSLICE:
	case ISN_BCALL:
	case ISN_BLOBAPPEND:
	case ISN_BLOBINDEX:
	case ISN_BLOBSLICE:
	case ISN_CATCH:
	case ISN_CEXPR_AUCMD:
	case ISN_CHECKLEN:
	case ISN_CLEARDICT:
	case ISN_CMDMOD_REV:
	case ISN_COMPAREANY:
	case ISN_COMPAREBLOB:
	case ISN_COMPAREBOOL:
	case ISN_COMPAREDICT:
	case ISN_COMPAREFLOAT:
	case ISN_COMPAREFUNC:
	case ISN_COMPARELIST:
	case ISN_COMPARETUPLE:
	case ISN_COMPARENR:
	case ISN_COMPARENULL:
	case ISN_COMPAREOBJECT:
	case ISN_COMPARESPECIAL:
	case ISN_COMPARESTRING:
	case ISN_CONCAT:
	case ISN_CONSTRUCT:
	case ISN_COND2BOOL:
	case ISN_DEBUG:
	case ISN_DEFER:
	case ISN_DROP:
	case ISN_ECHO:
	case ISN_ECHOCONSOLE:
	case ISN_ECHOERR:
	case ISN_ECHOMSG:
	case ISN_ECHOWINDOW:
	case ISN_ENDLOOP:
	case ISN_ENDTRY:
	case ISN_EXECCONCAT:
	case ISN_EXECUTE:
	case ISN_FINALLY:
	case ISN_FINISH:
	case ISN_FOR:
	case ISN_GETITEM:
	case ISN_GET_OBJ_MEMBER:
	case ISN_JUMP:
	case ISN_JUMP_IF_ARG_NOT_SET:
	case ISN_JUMP_IF_ARG_SET:
	case ISN_LISTAPPEND:
	case ISN_LISTINDEX:
	case ISN_LISTSLICE:
	case ISN_TUPLEINDEX:
	case ISN_TUPLESLICE:
	case ISN_LOAD:
	case ISN_LOADBDICT:
	case ISN_LOADGDICT:
	case ISN_LOADOUTER:
	case ISN_LOADREG:
	case ISN_LOADTDICT:
	case ISN_LOADV:
	case ISN_LOADWDICT:
	case ISN_LOCKCONST:
	case ISN_MEMBER:
	case ISN_NEGATENR:
	case ISN_NEWDICT:
	case ISN_NEWLIST:
	case ISN_NEWTUPLE:
	case ISN_NEWPARTIAL:
	case ISN_OPANY:
	case ISN_OPFLOAT:
	case ISN_OPNR:
	case ISN_PCALL:
	case ISN_PCALL_END:
	case ISN_PROF_END:
	case ISN_PROF_START:
	case ISN_PUSHBOOL:
	case ISN_PUSHCHANNEL:
	case ISN_PUSHF:
	case ISN_PUSHJOB:
	case ISN_PUSHNR:
	case ISN_PUSHOBJ:
	case ISN_PUSHSPEC:
	case ISN_PUT:
	case ISN_IPUT:
	case ISN_REDIREND:
	case ISN_REDIRSTART:
	case ISN_RETURN:
	case ISN_RETURN_OBJECT:
	case ISN_RETURN_VOID:
	case ISN_SHUFFLE:
	case ISN_SLICE:
	case ISN_SOURCE:
	case ISN_STORE:
	case ISN_STORENR:
	case ISN_STOREOUTER:
	case ISN_STORE_THIS:
	case ISN_STORERANGE:
	case ISN_STOREREG:
	case ISN_STOREV:
	case ISN_STRINDEX:
	case ISN_STRSLICE:
	case ISN_THROW:
	case ISN_TRYCONT:
	case ISN_UNLETINDEX:
	case ISN_UNLETRANGE:
	case ISN_UNPACK:
	case ISN_USEDICT:
	case ISN_WHILE:
	case ISN_SCRIPTCTX_SET:
	// nothing allocated
	break;
    }
}

    void
clear_instr_ga(garray_T *gap)
{
    ціле idx;

    для (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
