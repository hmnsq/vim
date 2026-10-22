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

#ако defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) ако (cctx->ctx_skip == SKIP_YES) върни NULL
#define RETURN_OK_IF_SKIP(cctx) ако (cctx->ctx_skip == SKIP_YES) върни OK

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
    ако (GA_GROW_FAILS(instr, 1))
	върни NULL;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    върни isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, цяло drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_stack.ga_len -= drop;
    върни generate_instr(cctx, isn_type);
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

    ако ((isn = generate_instr(cctx, isn_type)) == NULL)
	върни NULL;

    ако (push_type_stack2(cctx, type == NULL ? &t_any : type,
			       decl_type == NULL ? &t_any : decl_type) == FAIL)
	върни NULL;

    върни isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    върни generate_instr_type2(cctx, isn_type, type, &t_any);
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

    ако ((isn = generate_instr(cctx, ISN_DEBUG)) == NULL)
	върни NULL;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    върни isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    цяло
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == NULL)
	върни FAIL;
    isn->isn_arg.construct.construct_size = sizeof(object_T)
			       + cl->class_obj_member_count * sizeof(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    върни OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    цяло
generate_GET_OBJ_MEMBER(cctx_T *cctx, цяло idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    ако (isn == NULL)
	върни FAIL;

    isn->isn_arg.classmember.cm_class = NULL;
    isn->isn_arg.classmember.cm_idx = idx;
    върни push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    цяло
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, цяло idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    ако (isn == NULL)
	върни FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    върни push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    цяло
generate_STORE_THIS(cctx_T *cctx, цяло idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    ако (isn == NULL)
	върни FAIL;

    isn->isn_arg.number = idx;
    върни OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    цяло
may_generate_2STRING(цяло offset, цяло tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_stack(cctx, -1 - offset);

    switch (type->tt_type)
    {
	// nothing to be done
	case VAR_STRING: върни OK;

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
			 ако (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     break;
			 }
			 ако (tostring_flags & TOSTRING_INTERPOLATE)
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
			 върни FAIL;
    }

    set_type_on_stack(cctx, &t_string, -1 - offset);
    ако ((isn = generate_instr(cctx, isntype)) == NULL)
	върни FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    върни OK;
}

    static цяло
check_number_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    ако (!((type1 == VAR_NUMBER || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_NUMBER || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	ако (check_type_is_value(typ1) == FAIL
		|| check_type_is_value(typ2) == FAIL)
	    върни FAIL;
	ако (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	иначе
	    semsg(_(e_char_requires_number_or_float_arguments), *op);
	върни FAIL;
    }
    върни OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    static цяло
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    за (цяло i = 0; i < tuple_type->tt_argcount; i++)
    {
	ако (ga_grow(gap, 1) == FAIL)
	    върни FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    върни OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    static цяло
set_tuple_union_type_on_stack(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    ако (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    ако (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    ако ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_stack(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    върни OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    цяло
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

    ако (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_number_or_float(type1, type2, (char_u *)"+") == FAIL)
	върни FAIL;

    ако (isn != NULL)
    {
	ако (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = expr_type;
	иначе
	    isn->isn_arg.op.op_type = EXPR_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    ако (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_stack(cctx, &t_list_any, 0);
    иначе ако (vartype == VAR_TUPLE)
    {
	ако (!check_tuples_addable(type1, type2))
	    върни FAIL;

	ако (set_tuple_union_type_on_stack(type1, type2, cctx) == FAIL)
	    върни FAIL;
    }

    върни isn == NULL ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    ако (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_NUMBER
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	върни type1->tt_type;
    върни VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    цяло
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
		  ако (generate_add_instr(cctx, vartype, type1, type2,
							    EXPR_COPY) == FAIL)
		      върни FAIL;
		  break;

	case '-':
	case '*':
	case '/': ако (check_number_or_float(type1, type2, op) == FAIL)
		      върни FAIL;
		  ако (vartype == VAR_NUMBER)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  иначе ако (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  иначе
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  ако (isn != NULL)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? EXPR_MULT : *op == '/'? EXPR_DIV : EXPR_SUB;
		  break;

	case '%': ако ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_NUMBER)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_NUMBER))
		  {
		      emsg(_(e_percent_requires_number_arguments));
		      върни FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_NUMBER ? ISN_OPNR : ISN_OPANY, 1);
		  ако (isn != NULL)
		      isn->isn_arg.op.op_type = EXPR_REM;
		  break;
    }

    // correct type of result
    ако (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	ако ((type1->tt_type == VAR_NUMBER || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_NUMBER || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_stack(cctx, type, 0);
    }

    върни OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    static isntype_T
compare_isn_not_values(typval_T *tv, type_T *type)
{
    ако (tv != NULL)
	(void)check_typval_is_value(tv);
    иначе
	(void)check_type_is_value(type);
    върни ISN_DROP;
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

    ако (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	върни compare_isn_not_values(tv1, type1);
    ако (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	върни compare_isn_not_values(tv2, type2);

    ако (vartype1 == vartype2)
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
    иначе ако (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_NUMBER || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_NUMBER || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    иначе ако (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	ако ((vartype1 == VAR_SPECIAL
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
	    върни ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    ако ((exprtype == EXPR_IS || exprtype == EXPR_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == EXPR_IS ? "is" : "isnot" , vartype_name(vartype1));
	върни ISN_DROP;
    }
    ако (!(exprtype == EXPR_IS || exprtype == EXPR_ISNOT
		|| exprtype == EXPR_EQUAL || exprtype == EXPR_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	върни ISN_DROP;
    }
    ако (isntype == ISN_DROP
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
	върни ISN_DROP;
    }
    върни isntype;
}

    цяло
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    ако (get_compare_isn(type, tv1, tv2, NULL, NULL) == ISN_DROP)
	върни FAIL;
    върни OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    цяло
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, цяло ic)
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
    ако (isntype == ISN_DROP)
	върни FAIL;

    ако ((isn = generate_instr(cctx, isntype)) == NULL)
	върни FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --stack->ga_len;
    set_type_on_stack(cctx, &t_bool, 0);

    върни OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    цяло
generate_CONCAT(cctx_T *cctx, цяло count)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);

    ако ((isn = generate_instr(cctx, ISN_CONCAT)) == NULL)
	върни FAIL;
    isn->isn_arg.number = count;

    // drop the argument types
    stack->ga_len -= count - 1;

    върни OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    цяло
generate_2BOOL(cctx_T *cctx, цяло invert, цяло offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_2BOOL)) == NULL)
	върни FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, -1 - offset);

    върни OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    цяло
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ако (generate_instr(cctx, ISN_COND2BOOL) == NULL)
	върни FAIL;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, 0);

    върни OK;
}

    цяло
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	цяло	    typechk_flags,	// type check flags
	цяло	    offset,
	цяло	    is_var,
	цяло	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == NULL)
	върни FAIL;
    type_T *tt;
    ако ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_NUMBER_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	ако (tt != NULL)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    ако (typechk_flags & TYPECHK_NUMBER_OK)
		tt->tt_flags |= TTFLAG_NUMBER_OK;
	    иначе
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    иначе
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_stack(cctx, expected, -1 - offset);

    върни OK;
}

    цяло
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_SETTYPE)) == NULL)
	върни FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    върни OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    цяло
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ако (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == NULL)
	върни FAIL;
    върни OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    static цяло
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == NULL ? &t_any : &class->class_type);
    ако (isn == NULL)
	върни FAIL;
    isn->isn_arg.classarg = class;
    ако (class != NULL)
	++class->class_refcount;
    върни OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    цяло
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
	    ако (tv->vval.v_list != NULL)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    break;
	case VAR_TUPLE:
	    ако (tv->vval.v_tuple != NULL)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    break;
	case VAR_DICT:
	    ако (tv->vval.v_dict != NULL)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    break;
#ifdef FEAT_JOB_CHANNEL
	case VAR_JOB:
	    ако (tv->vval.v_job != NULL)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    break;
	case VAR_CHANNEL:
	    ако (tv->vval.v_channel != NULL)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    break;
#endif
	case VAR_FUNC:
	    ако (tv->vval.v_string != NULL)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, NULL, &t_func_unknown, TRUE);
	    break;
	case VAR_PARTIAL:
	    ако (tv->vval.v_partial != NULL)
		iemsg("non-null partial constant not supported");
	    ако (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == NULL)
		върни FAIL;
	    break;
	case VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = NULL;
	    break;
	case VAR_OBJECT:
	    ако (tv->vval.v_object != NULL)
	    {
		emsg(_(e_cannot_use_non_null_object));
		върни FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    break;
	case VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    break;
	default:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    върни FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    върни OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    цяло
generate_PUSHNR(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_number)) == NULL)
	върни FAIL;
    isn->isn_arg.number = number;

    ако (number == 0 || number == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_stack(cctx, &t_number_bool, 0);
    върни OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    цяло
generate_PUSHBOOL(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == NULL)
	върни FAIL;
    isn->isn_arg.number = number;

    върни OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    цяло
generate_PUSHSPEC(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     number == VVAL_NULL ? &t_null : &t_none)) == NULL)
	върни FAIL;
    isn->isn_arg.number = number;

    върни OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    цяло
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == NULL)
	върни FAIL;
    isn->isn_arg.fnumber = fnumber;

    върни OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    цяло
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    цяло		ret = OK;

    ако (cctx->ctx_skip != SKIP_YES)
    {
	ако ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == NULL)
	    ret = FAIL;
	иначе
	{
	    isn->isn_arg.string = str == NULL ? NULL : *str;
	    върни OK;
	}
    }
    ако (str != NULL)
	VIM_CLEAR(*str);
    върни ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    цяло
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    ако (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == NULL)
	върни FAIL;
    върни OK;
#иначе
    emsg(_(e_channel_job_feature_not_available));
    върни FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    цяло
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    ако (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == NULL)
	върни FAIL;
    върни OK;
#иначе
    emsg(_(e_channel_job_feature_not_available));
    върни FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    цяло
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == NULL)
	върни FAIL;
    isn->isn_arg.blob = blob;

    върни OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    цяло
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, цяло may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == NULL)
	върни FAIL;
    ако (name == NULL)
	funcname = NULL;
    иначе ако (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != NULL)	    // autoload
	funcname = vim_strsave(name);
    иначе
    {
	funcname = alloc(STRLEN(name) + 3);
	ако (funcname != NULL)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    върни OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    цяло
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == NULL)
	върни FAIL;
    isn->isn_arg.string = vim_strsave(name);
    ако (isn->isn_arg.string == NULL)
	върни FAIL;
    върни OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    цяло
generate_GETITEM(cctx_T *cctx, цяло index, цяло with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_stack(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    ако ((isn = generate_instr(cctx, ISN_GETITEM)) == NULL)
	върни FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    върни push_type_stack(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    static void
set_tuple_slice_type_on_stack(type_T *type, цяло count, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    ако ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= count)
    {
	set_type_on_stack(cctx, &t_tuple_any, 0);
	върни;
    }

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    за (цяло i = count; i < type->tt_argcount; i++)
    {
	ако (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_stack(cctx, &t_tuple_any, 0);
	    върни;
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
    цяло
generate_SLICE(cctx_T *cctx, цяло count)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_SLICE)) == NULL)
	върни FAIL;
    isn->isn_arg.number = count;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_stack(cctx, 0);
    ако (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_stack(type, count, cctx);

    върни OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    цяло
generate_CHECKLEN(cctx_T *cctx, цяло min_len, цяло more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    ако ((isn = generate_instr(cctx, ISN_CHECKLEN)) == NULL)
	върни FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    върни OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    цяло
generate_STORE(cctx_T *cctx, isntype_T isn_type, цяло idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	върни FAIL;
    ако (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    иначе
	isn->isn_arg.number = idx;

    върни OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    цяло
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	цяло	    load,
	class_T	    *cl,
	цяло	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    иначе
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    ако (isn == NULL)
	върни FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    върни OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    static цяло
generate_STOREOUTER(
	cctx_T	    *cctx,
	цяло	    idx,
	цяло	    level,
	цяло	    loop_depth,
	цяло	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == NULL)
	върни FAIL;
    ако (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    иначе
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    върни OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    цяло
generate_STORENR(cctx_T *cctx, цяло idx, varnumber_T value)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_STORENR)) == NULL)
	върни FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = value;

    върни OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    static цяло
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	цяло	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	върни FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    върни OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    цяло
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	цяло	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_type2(cctx, isn_type, type, type)) == NULL)
	върни FAIL;
    ако (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    иначе
	isn->isn_arg.number = idx;

    върни OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    цяло
generate_LOADOUTER(
	cctx_T	    *cctx,
	цяло	    idx,
	цяло	    nesting,
	цяло	    loop_depth,
	цяло	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == NULL)
	върни FAIL;
    ако (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    иначе
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    върни OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    цяло
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    цяло	    di_flags;
    цяло	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    ако (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	върни FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    върни generate_LOAD(cctx, ISN_LOADV, vidx, NULL, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    цяло
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, цяло forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, isn_type)) == NULL)
	върни FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    върни OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    цяло
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ако (generate_instr(cctx, ISN_LOCKCONST) == NULL)
	върни FAIL;
    върни OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    цяло
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	цяло	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    иначе
	isn = generate_instr_drop(cctx, isn_type, 1);
    ако (isn == NULL)
	върни FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    върни OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    цяло
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	цяло	    sid,
	цяло	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    ако (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    иначе
	isn = generate_instr_drop(cctx, isn_type, 1);
    ако (isn == NULL)
	върни FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    ако (sref == NULL)
	върни FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    върни OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    цяло
generate_NEWLIST(cctx_T *cctx, цяло count, цяло use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_NEWLIST)) == NULL)
	върни FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    ако ((member_type = get_member_type_from_stack(count, 1, cctx)) == NULL)
	върни FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the list type to the type stack
    върни push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    цяло
generate_NEWTUPLE(cctx_T *cctx, цяло count, цяло use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == NULL)
	върни FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    ако (get_tuple_type_from_stack(count, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	върни FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the tuple type to the type stack
    върни push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    цяло
generate_NEWDICT(cctx_T *cctx, цяло count, цяло use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_NEWDICT)) == NULL)
	върни FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    ако ((member_type = get_member_type_from_stack(count, 2, cctx)) == NULL)
	върни FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_stack.ga_len -= 2 * count;

    // add the dict type to the type stack
    върни push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    цяло
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	цяло	    object_method,
	цяло	    fi,
	цяло	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    цяло		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_FUNCREF)) == NULL)
	върни FAIL;
    ако (isn_idx != NULL)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    ако (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != NULL)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	ако (extra == NULL)
	    върни FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	ако (cl != NULL)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    ако (ufunc->uf_def_status == UF_NOT_COMPILED || cl != NULL)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ако (ufunc->uf_def_status != UF_NOT_COMPILED && cl == NULL)
    {
	ако (isn_idx == NULL && ufunc->uf_def_status == UF_TO_BE_COMPILED)
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
    ако ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == NULL ? &t_func_any : ufunc->uf_func_type;
    върни push_type_stack(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    цяло
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    цяло		ret = OK;

    ако (cctx->ctx_skip != SKIP_YES)
    {
	ако ((isn = generate_instr(cctx, ISN_NEWFUNC)) == NULL)
	    ret = FAIL;
	иначе
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    ако (arg == NULL)
		ret = FAIL;
	    иначе
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(void)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		върни OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    върни ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    цяло
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_DEF)) == NULL)
	върни FAIL;
    ако (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	ако (isn->isn_arg.string == NULL)
	    върни FAIL;
    }
    върни OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    цяло
generate_JUMP(cctx_T *cctx, jumpwhen_T when, цяло where)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_JUMP)) == NULL)
	върни FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    ако (when != JUMP_ALWAYS && stack->ga_len > 0)
	--stack->ga_len;

    върни OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    цяло
generate_WHILE(cctx_T *cctx, цяло funcref_idx)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_WHILE)) == NULL)
	върни FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    ако (stack->ga_len > 0)
	--stack->ga_len;

    върни OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    цяло
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, цяло arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, isn_type)) == NULL)
	върни FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    върни OK;
}

    цяло
generate_FOR(cctx_T *cctx, цяло loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_FOR)) == NULL)
	върни FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    върни push_type_stack(cctx, &t_any);
}

    цяло
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_ENDLOOP)) == NULL)
	върни FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_count;
    isn->isn_arg.endloop.end_var_count =
			   cctx->ctx_locals.ga_len - loop_info->li_local_count;
    върни OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    цяло
generate_TRYCONT(cctx_T *cctx, цяло levels, цяло where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_TRYCONT)) == NULL)
	върни FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    върни OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    цяло
check_internal_func_args(
	cctx_T	*cctx,
	цяло	func_idx,
	цяло	argcount,
	цяло	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    цяло		argoff = check_internal_func(func_idx, argcount);

    ако (argoff < 0)
	върни FAIL;

    ако (method_call && argoff > 1)
    {
	ако (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    върни FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	ако (isn  == NULL)
	    върни FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    ако (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)stack->ga_data) + stack->ga_len - argcount;

	// Check the types of the arguments.
	ако (method_call && argoff > 1)
	{
	    цяло i;

	    за (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	иначе
	{
	    цяло i;

	    за (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ако (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    върни FAIL;
    }
    върни OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    цяло
generate_BCALL(cctx_T *cctx, цяло func_idx, цяло argcount, цяло method_call)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*argtypes = NULL;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = NULL;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    ако (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	върни FAIL;

    ако (internal_func_is_map(func_idx))
	maptype = argtypes;

    ако ((isn = generate_instr(cctx, ISN_BCALL)) == NULL)
	върни FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    stack->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    ако (push_type_stack2(cctx, type, decl_type) == FAIL)
	върни FAIL;

    ако (maptype != NULL && maptype[0].type_decl->tt_member != NULL
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    върни OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    цяло
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_stack(cctx, 1);
    ако (arg_type_modifiable(list_type, 1) == FAIL)
	върни FAIL;
    item_type = get_type_on_stack(cctx, 0);
    expected = list_type->tt_member;
    ако (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	върни FAIL;

    ако (generate_instr(cctx, ISN_LISTAPPEND) == NULL)
	върни FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    върни OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    цяло
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    ако (arg_type_modifiable(get_decl_type_on_stack(cctx, 1), 1) == FAIL)
	върни FAIL;
    item_type = get_type_on_stack(cctx, 0);
    ако (need_type(item_type, &t_number, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	върни FAIL;

    ако (generate_instr(cctx, ISN_BLOBAPPEND) == NULL)
	върни FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    върни OK;
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
    ако (cl != NULL)
	върни ISN_METHODCALL;

    ако (ufunc->uf_def_status == UF_NOT_COMPILED)
	върни ISN_UCALL;

    // function invoked from a lambda
    ако (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	ако (ufunc->uf_name[0] == K_SPECIAL)
	    върни ISN_UCALL;
    }

    върни ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    цяло
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	цяло	    mi,
	цяло	    pushed_argcount,
	цяло	    is_super)
{
    isn_T	*isn;
    цяло		regular_args = ufunc->uf_args.ga_len;
    цяло		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    ако (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	върни FAIL;
    }
    ако (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	върни FAIL;
    }

    ако (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	цяло		i;
	compiletype_T	compile_type;

	за (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_stack(cctx, argcount - i - 1);
	    ако (check_type_is_value(actual) == FAIL)
		върни FAIL;
	    ако (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		continue;
	    }
	    ако (i < regular_args)
	    {
		ако (ufunc->uf_arg_types == NULL)
		    continue;
		expected = ufunc->uf_arg_types[i];
	    }
	    иначе ако (ufunc->uf_va_type == NULL
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    иначе
		expected = ufunc->uf_va_type->tt_member;
	    ако (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		върни FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	ако (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == NULL,
						   compile_type, NULL) == FAIL)
	    върни FAIL;
    }
    ако (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	върни FAIL;
    }

    ако ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == NULL)
	върни FAIL;
    ако (cl != NULL /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	ако (isn->isn_arg.mfunc == NULL)
	    върни FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = NULL;
	ако (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = NULL;
	isn->isn_arg.mfunc->cmf_cache_generic = NULL;
	isn->isn_arg.mfunc->cmf_cache_ufunc = NULL;
    }
    иначе ако (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    иначе
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    ако (ufunc->uf_class != NULL)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_stack(cctx, 0);
	ако (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_stack.ga_len--;
    }

    // add return type
    върни push_type_stack(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    цяло
generate_UCALL(cctx_T *cctx, char_u *name, цяло argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_UCALL)) == NULL)
	върни FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // add return value
    върни push_type_stack(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    цяло
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	цяло	argcount,
	цяло	at_top,
	char_u	*name)
{
    ако (type->tt_argcount != -1)
    {
	цяло	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	ако (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    върни FAIL;
	}
	ако (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    върни FAIL;
	}
	ако (type->tt_args != NULL)
	{
	    цяло i;

	    за (i = 0; i < argcount; ++i)
	    {
		цяло	offset = -argcount + i - (at_top ? 0 : 1);
		type_T	*actual = get_type_on_stack(cctx, -1 - offset);
		type_T	*expected;

		ако (check_type_is_value(actual) == FAIL)
		    върни FAIL;
		ако (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    ако (expected != NULL && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    ако (expected == NULL)
			expected = &t_any;
		}
		иначе ако (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		иначе
		    expected = type->tt_args[i];
		ако (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    върни FAIL;
		}
	    }
	}
    }

    върни OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    цяло
generate_PCALL(
	cctx_T	*cctx,
	цяло	argcount,
	char_u	*name,
	type_T	*type,
	цяло	at_top)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    ако (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    иначе ако (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	ако (check_func_args_from_type(cctx, type, argcount, at_top, name)
								       == FAIL)
	    върни FAIL;

	ret_type = type->tt_member;
	ако (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    иначе
    {
	semsg(_(e_not_callable_type_str), name);
	върни FAIL;
    }

    ако ((isn = generate_instr(cctx, ISN_PCALL)) == NULL)
	върни FAIL;
    isn->isn_arg.pfunc.cpf_top = at_top;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_stack.ga_len -= argcount + 1;

    // push the return value
    push_type_stack(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    ако (at_top && generate_instr(cctx, ISN_PCALL_END) == NULL)
	върни FAIL;

    върни OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    цяло
generate_DEFER(cctx_T *cctx, цяло var_idx, цяло argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == NULL)
	върни FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    върни OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    цяло
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == NULL)
	върни FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_stack(cctx, 0);
    ако (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	char *tofree;

	semsg(_(e_expected_dictionary_for_using_key_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	върни FAIL;
    }
    // change dict type to dict member type
    ако (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_stack(cctx, ntype, 0);
    }

    върни OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    цяло
generate_ECHO(cctx_T *cctx, цяло with_white, цяло count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_drop(cctx, ISN_ECHO, count)) == NULL)
	върни FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_count = count;

    върни OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    цяло
generate_MULT_EXPR(cctx_T *cctx, isntype_T isn_type, цяло count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr_drop(cctx, isn_type, count)) == NULL)
	върни FAIL;
    isn->isn_arg.number = count;
    върни OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    цяло
generate_ECHOWINDOW(cctx_T *cctx, цяло count, long time)
{
    isn_T	*isn;

    ако ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, count)) == NULL)
	върни FAIL;
    isn->isn_arg.echowin.ewin_count = count;
    isn->isn_arg.echowin.ewin_time = time;
    върни OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    цяло
generate_SOURCE(cctx_T *cctx, цяло sid)
{
    isn_T	*isn;

    ако ((isn = generate_instr(cctx, ISN_SOURCE)) == NULL)
	върни FAIL;
    isn->isn_arg.number = sid;

    върни OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    цяло
generate_PUT(cctx_T *cctx, цяло regname, linenr_T lnum, цяло fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    ако (isn == NULL)
	върни FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    върни OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    цяло
generate_LOCKUNLOCK(cctx_T *cctx, char_u *line, цяло is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == NULL)
	върни FAIL;
    class_T *cl = cctx->ctx_ufunc != NULL ? cctx->ctx_ufunc->uf_class : NULL;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(line);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    ако (cl != NULL)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    върни OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    цяло
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, isntype)) == NULL)
	върни FAIL;
    isn->isn_arg.string = vim_strsave(line);
    върни OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    цяло
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    цяло		ret = OK;

    ако (cctx->ctx_skip != SKIP_YES)
    {
	ако ((isn = generate_instr(cctx, isntype)) == NULL)
	    ret = FAIL;
	иначе
	{
	    isn->isn_arg.string = str;
	    върни OK;
	}
    }
    vim_free(str);
    върни ret;
}

    цяло
generate_LEGACY_EVAL(cctx_T *cctx, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == NULL)
	върни FAIL;
    isn->isn_arg.string = vim_strsave(line);

    върни push_type_stack(cctx, &t_any);
}

    цяло
generate_EXECCONCAT(cctx_T *cctx, цяло count)
{
    isn_T	*isn;

    ако ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, count)) == NULL)
	върни FAIL;
    isn->isn_arg.number = count;
    върни OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    цяло
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    ако ((isn = generate_instr(cctx, ISN_RANGE)) == NULL)
	върни FAIL;
    isn->isn_arg.string = range;

    върни push_type_stack(cctx, &t_number);
}

    цяло
generate_UNPACK(cctx_T *cctx, цяло var_count, цяло semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_UNPACK)) == NULL)
	върни FAIL;
    isn->isn_arg.unpack.unp_count = var_count;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    върни OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    цяло
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    ако (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	ако ((isn = generate_instr(cctx, ISN_CMDMOD)) == NULL)
	    върни FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	ако (isn->isn_arg.cmdmod.cf_cmdmod == NULL)
	    върни FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, sizeof(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = NULL;
    }

    върни OK;
}

    цяло
generate_undo_cmdmods(cctx_T *cctx)
{
    ако (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == NULL)
	върни FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    върни OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    цяло
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	цяло		opt_flags,
	цяло		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    switch (dest)
    {
	case dest_option:
	    върни generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	case dest_func_option:
	    върни generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	case dest_global:
	    // include g: with the name, easier to execute that way
	    върни generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == NULL
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	case dest_buffer:
	    // include b: with the name, easier to execute that way
	    върни generate_STORE(cctx, ISN_STOREB, 0, name);
	case dest_window:
	    // include w: with the name, easier to execute that way
	    върни generate_STORE(cctx, ISN_STOREW, 0, name);
	case dest_tab:
	    // include t: with the name, easier to execute that way
	    върни generate_STORE(cctx, ISN_STORET, 0, name);
	case dest_env:
	    върни generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	case dest_reg:
	    върни generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], NULL);
	case dest_vimvar:
	    върни generate_STORE(cctx, ISN_STOREV, vimvaridx, NULL);
	case dest_script:
	case dest_script_v9:
	    {
		цяло	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		цяло	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		ако (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    ако (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == NULL)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			ако (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    върни FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    върни generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		върни generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	case dest_class_member:
	    върни generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	case dest_local:
	case dest_expr:
	    // cannot happen
	    break;
    }
    върни FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    цяло
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    за (;;)
    {
	ако (scope == NULL)
	    break;
	ако (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    върни TRUE;
	scope = scope->se_outer;
    }
    върни FALSE;
}

    цяло
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, цяло instr_count, цяло is_decl)
{
    ако (lhs->lhs_dest != dest_local)
	върни generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    ако (lhs->lhs_lvar == NULL)
	върни OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    ако (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_count + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.number;
	garray_T    *stack = &cctx->ctx_type_stack;

	ако (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	иначе
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	ако (stack->ga_len > 0)
	    --stack->ga_len;
    }
    иначе ако (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    иначе
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, NULL);
    върни OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    цяло
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ако ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == NULL)
	върни FAIL;
    isn->isn_arg.setsctx = new_sctx;
    върни OK;
}

#ако defined(FEAT_PROFILE)
    void
may_generate_prof_end(cctx_T *cctx, цяло prof_lnum)
{
    ако (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
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
		цяло	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		за (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    break;

	case ISN_INSTR:
	    {
		цяло	idx;
		isn_T	*list = isn->isn_arg.instr;

		за (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
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

		ако (extra == NULL || extra->fre_func_name == NULL)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    ако (ufunc != NULL && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		ако (extra != NULL)
		{
		    char_u *name = extra->fre_func_name;

		    ако (name != NULL)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    ако (extra->fre_class != NULL)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    break;

	case ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		ако (dfunc->df_ufunc != NULL
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

		ако (arg != NULL)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    ако (ufunc != NULL)
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
    цяло idx;

    за (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
