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

#nếu defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) nếu (cctx->ctx_skip == SKIP_YES) trả_về NULL
#define RETURN_OK_IF_SKIP(cctx) nếu (cctx->ctx_skip == SKIP_YES) trả_về OK

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
    nếu (GA_GROW_FAILS(instr, 1))
	trả_về NULL;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    trả_về isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, số_nguyên drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_stack.ga_len -= drop;
    trả_về generate_instr(cctx, isn_type);
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

    nếu ((isn = generate_instr(cctx, isn_type)) == NULL)
	trả_về NULL;

    nếu (push_type_stack2(cctx, type == NULL ? &t_any : type,
			       decl_type == NULL ? &t_any : decl_type) == FAIL)
	trả_về NULL;

    trả_về isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    trả_về generate_instr_type2(cctx, isn_type, type, &t_any);
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

    nếu ((isn = generate_instr(cctx, ISN_DEBUG)) == NULL)
	trả_về NULL;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    trả_về isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    số_nguyên
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == NULL)
	trả_về FAIL;
    isn->isn_arg.construct.construct_size = sizeof(object_T)
			       + cl->class_obj_member_count * sizeof(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    trả_về OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    số_nguyên
generate_GET_OBJ_MEMBER(cctx_T *cctx, số_nguyên idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    nếu (isn == NULL)
	trả_về FAIL;

    isn->isn_arg.classmember.cm_class = NULL;
    isn->isn_arg.classmember.cm_idx = idx;
    trả_về push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    số_nguyên
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, số_nguyên idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    nếu (isn == NULL)
	trả_về FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    trả_về push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    số_nguyên
generate_STORE_THIS(cctx_T *cctx, số_nguyên idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    nếu (isn == NULL)
	trả_về FAIL;

    isn->isn_arg.number = idx;
    trả_về OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    số_nguyên
may_generate_2STRING(số_nguyên offset, số_nguyên tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_stack(cctx, -1 - offset);

    switch (type->tt_type)
    {
	// nothing to be done
	case VAR_STRING: trả_về OK;

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
			 nếu (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     break;
			 }
			 nếu (tostring_flags & TOSTRING_INTERPOLATE)
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
			 trả_về FAIL;
    }

    set_type_on_stack(cctx, &t_string, -1 - offset);
    nếu ((isn = generate_instr(cctx, isntype)) == NULL)
	trả_về FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    trả_về OK;
}

    static số_nguyên
check_number_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    nếu (!((type1 == VAR_NUMBER || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_NUMBER || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	nếu (check_type_is_value(typ1) == FAIL
		|| check_type_is_value(typ2) == FAIL)
	    trả_về FAIL;
	nếu (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	không_thì
	    semsg(_(e_char_requires_number_or_float_arguments), *op);
	trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    static số_nguyên
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    cho (số_nguyên i = 0; i < tuple_type->tt_argcount; i++)
    {
	nếu (ga_grow(gap, 1) == FAIL)
	    trả_về FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    trả_về OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    static số_nguyên
set_tuple_union_type_on_stack(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    nếu (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    nếu (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    nếu ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_stack(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    trả_về OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    số_nguyên
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

    nếu (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_number_or_float(type1, type2, (char_u *)"+") == FAIL)
	trả_về FAIL;

    nếu (isn != NULL)
    {
	nếu (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = expr_type;
	không_thì
	    isn->isn_arg.op.op_type = EXPR_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    nếu (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_stack(cctx, &t_list_any, 0);
    không_thì nếu (vartype == VAR_TUPLE)
    {
	nếu (!check_tuples_addable(type1, type2))
	    trả_về FAIL;

	nếu (set_tuple_union_type_on_stack(type1, type2, cctx) == FAIL)
	    trả_về FAIL;
    }

    trả_về isn == NULL ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    nếu (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_NUMBER
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	trả_về type1->tt_type;
    trả_về VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    số_nguyên
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
		  nếu (generate_add_instr(cctx, vartype, type1, type2,
							    EXPR_COPY) == FAIL)
		      trả_về FAIL;
		  break;

	case '-':
	case '*':
	case '/': nếu (check_number_or_float(type1, type2, op) == FAIL)
		      trả_về FAIL;
		  nếu (vartype == VAR_NUMBER)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  không_thì nếu (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  không_thì
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  nếu (isn != NULL)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? EXPR_MULT : *op == '/'? EXPR_DIV : EXPR_SUB;
		  break;

	case '%': nếu ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_NUMBER)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_NUMBER))
		  {
		      emsg(_(e_percent_requires_number_arguments));
		      trả_về FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_NUMBER ? ISN_OPNR : ISN_OPANY, 1);
		  nếu (isn != NULL)
		      isn->isn_arg.op.op_type = EXPR_REM;
		  break;
    }

    // correct type of result
    nếu (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	nếu ((type1->tt_type == VAR_NUMBER || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_NUMBER || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_stack(cctx, type, 0);
    }

    trả_về OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    static isntype_T
compare_isn_not_values(typval_T *tv, type_T *type)
{
    nếu (tv != NULL)
	(void)check_typval_is_value(tv);
    không_thì
	(void)check_type_is_value(type);
    trả_về ISN_DROP;
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

    nếu (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	trả_về compare_isn_not_values(tv1, type1);
    nếu (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	trả_về compare_isn_not_values(tv2, type2);

    nếu (vartype1 == vartype2)
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
    không_thì nếu (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_NUMBER || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_NUMBER || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    không_thì nếu (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	nếu ((vartype1 == VAR_SPECIAL
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
	    trả_về ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    nếu ((exprtype == EXPR_IS || exprtype == EXPR_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == EXPR_IS ? "is" : "isnot" , vartype_name(vartype1));
	trả_về ISN_DROP;
    }
    nếu (!(exprtype == EXPR_IS || exprtype == EXPR_ISNOT
		|| exprtype == EXPR_EQUAL || exprtype == EXPR_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	trả_về ISN_DROP;
    }
    nếu (isntype == ISN_DROP
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
	trả_về ISN_DROP;
    }
    trả_về isntype;
}

    số_nguyên
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    nếu (get_compare_isn(type, tv1, tv2, NULL, NULL) == ISN_DROP)
	trả_về FAIL;
    trả_về OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    số_nguyên
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, số_nguyên ic)
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
    nếu (isntype == ISN_DROP)
	trả_về FAIL;

    nếu ((isn = generate_instr(cctx, isntype)) == NULL)
	trả_về FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --stack->ga_len;
    set_type_on_stack(cctx, &t_bool, 0);

    trả_về OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    số_nguyên
generate_CONCAT(cctx_T *cctx, số_nguyên count)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);

    nếu ((isn = generate_instr(cctx, ISN_CONCAT)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = count;

    // drop the argument types
    stack->ga_len -= count - 1;

    trả_về OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    số_nguyên
generate_2BOOL(cctx_T *cctx, số_nguyên invert, số_nguyên offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_2BOOL)) == NULL)
	trả_về FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, -1 - offset);

    trả_về OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    số_nguyên
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    nếu (generate_instr(cctx, ISN_COND2BOOL) == NULL)
	trả_về FAIL;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, 0);

    trả_về OK;
}

    số_nguyên
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	số_nguyên	    typechk_flags,	// type check flags
	số_nguyên	    offset,
	số_nguyên	    is_var,
	số_nguyên	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == NULL)
	trả_về FAIL;
    type_T *tt;
    nếu ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_NUMBER_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	nếu (tt != NULL)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    nếu (typechk_flags & TYPECHK_NUMBER_OK)
		tt->tt_flags |= TTFLAG_NUMBER_OK;
	    không_thì
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    không_thì
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_stack(cctx, expected, -1 - offset);

    trả_về OK;
}

    số_nguyên
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_SETTYPE)) == NULL)
	trả_về FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    trả_về OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    số_nguyên
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    nếu (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == NULL)
	trả_về FAIL;
    trả_về OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    static số_nguyên
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == NULL ? &t_any : &class->class_type);
    nếu (isn == NULL)
	trả_về FAIL;
    isn->isn_arg.classarg = class;
    nếu (class != NULL)
	++class->class_refcount;
    trả_về OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    số_nguyên
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
	    nếu (tv->vval.v_list != NULL)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    break;
	case VAR_TUPLE:
	    nếu (tv->vval.v_tuple != NULL)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    break;
	case VAR_DICT:
	    nếu (tv->vval.v_dict != NULL)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    break;
#ifdef FEAT_JOB_CHANNEL
	case VAR_JOB:
	    nếu (tv->vval.v_job != NULL)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    break;
	case VAR_CHANNEL:
	    nếu (tv->vval.v_channel != NULL)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    break;
#endif
	case VAR_FUNC:
	    nếu (tv->vval.v_string != NULL)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, NULL, &t_func_unknown, TRUE);
	    break;
	case VAR_PARTIAL:
	    nếu (tv->vval.v_partial != NULL)
		iemsg("non-null partial constant not supported");
	    nếu (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == NULL)
		trả_về FAIL;
	    break;
	case VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = NULL;
	    break;
	case VAR_OBJECT:
	    nếu (tv->vval.v_object != NULL)
	    {
		emsg(_(e_cannot_use_non_null_object));
		trả_về FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    break;
	case VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    break;
	default:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    trả_về FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    trả_về OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    số_nguyên
generate_PUSHNR(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_number)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = number;

    nếu (number == 0 || number == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_stack(cctx, &t_number_bool, 0);
    trả_về OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    số_nguyên
generate_PUSHBOOL(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = number;

    trả_về OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    số_nguyên
generate_PUSHSPEC(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     number == VVAL_NULL ? &t_null : &t_none)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = number;

    trả_về OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    số_nguyên
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == NULL)
	trả_về FAIL;
    isn->isn_arg.fnumber = fnumber;

    trả_về OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    số_nguyên
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    số_nguyên		ret = OK;

    nếu (cctx->ctx_skip != SKIP_YES)
    {
	nếu ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == NULL)
	    ret = FAIL;
	không_thì
	{
	    isn->isn_arg.string = str == NULL ? NULL : *str;
	    trả_về OK;
	}
    }
    nếu (str != NULL)
	VIM_CLEAR(*str);
    trả_về ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    số_nguyên
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    nếu (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == NULL)
	trả_về FAIL;
    trả_về OK;
#không_thì
    emsg(_(e_channel_job_feature_not_available));
    trả_về FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    số_nguyên
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    nếu (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == NULL)
	trả_về FAIL;
    trả_về OK;
#không_thì
    emsg(_(e_channel_job_feature_not_available));
    trả_về FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    số_nguyên
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == NULL)
	trả_về FAIL;
    isn->isn_arg.blob = blob;

    trả_về OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    số_nguyên
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, số_nguyên may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == NULL)
	trả_về FAIL;
    nếu (name == NULL)
	funcname = NULL;
    không_thì nếu (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != NULL)	    // autoload
	funcname = vim_strsave(name);
    không_thì
    {
	funcname = alloc(STRLEN(name) + 3);
	nếu (funcname != NULL)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    trả_về OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    số_nguyên
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == NULL)
	trả_về FAIL;
    isn->isn_arg.string = vim_strsave(name);
    nếu (isn->isn_arg.string == NULL)
	trả_về FAIL;
    trả_về OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    số_nguyên
generate_GETITEM(cctx_T *cctx, số_nguyên index, số_nguyên with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_stack(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    nếu ((isn = generate_instr(cctx, ISN_GETITEM)) == NULL)
	trả_về FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    trả_về push_type_stack(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    static void
set_tuple_slice_type_on_stack(type_T *type, số_nguyên count, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    nếu ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= count)
    {
	set_type_on_stack(cctx, &t_tuple_any, 0);
	trả_về;
    }

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    cho (số_nguyên i = count; i < type->tt_argcount; i++)
    {
	nếu (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_stack(cctx, &t_tuple_any, 0);
	    trả_về;
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
    số_nguyên
generate_SLICE(cctx_T *cctx, số_nguyên count)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_SLICE)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = count;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_stack(cctx, 0);
    nếu (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_stack(type, count, cctx);

    trả_về OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    số_nguyên
generate_CHECKLEN(cctx_T *cctx, số_nguyên min_len, số_nguyên more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    nếu ((isn = generate_instr(cctx, ISN_CHECKLEN)) == NULL)
	trả_về FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    trả_về OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    số_nguyên
generate_STORE(cctx_T *cctx, isntype_T isn_type, số_nguyên idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	trả_về FAIL;
    nếu (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    không_thì
	isn->isn_arg.number = idx;

    trả_về OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    số_nguyên
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	số_nguyên	    load,
	class_T	    *cl,
	số_nguyên	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    không_thì
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    nếu (isn == NULL)
	trả_về FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    trả_về OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    static số_nguyên
generate_STOREOUTER(
	cctx_T	    *cctx,
	số_nguyên	    idx,
	số_nguyên	    level,
	số_nguyên	    loop_depth,
	số_nguyên	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == NULL)
	trả_về FAIL;
    nếu (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    không_thì
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    trả_về OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    số_nguyên
generate_STORENR(cctx_T *cctx, số_nguyên idx, varnumber_T value)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_STORENR)) == NULL)
	trả_về FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = value;

    trả_về OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    static số_nguyên
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	số_nguyên	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	trả_về FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    trả_về OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    số_nguyên
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	số_nguyên	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_type2(cctx, isn_type, type, type)) == NULL)
	trả_về FAIL;
    nếu (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    không_thì
	isn->isn_arg.number = idx;

    trả_về OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    số_nguyên
generate_LOADOUTER(
	cctx_T	    *cctx,
	số_nguyên	    idx,
	số_nguyên	    nesting,
	số_nguyên	    loop_depth,
	số_nguyên	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == NULL)
	trả_về FAIL;
    nếu (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    không_thì
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    trả_về OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    số_nguyên
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    số_nguyên	    di_flags;
    số_nguyên	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    nếu (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	trả_về FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    trả_về generate_LOAD(cctx, ISN_LOADV, vidx, NULL, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    số_nguyên
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, số_nguyên forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, isn_type)) == NULL)
	trả_về FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    trả_về OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    số_nguyên
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    nếu (generate_instr(cctx, ISN_LOCKCONST) == NULL)
	trả_về FAIL;
    trả_về OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    số_nguyên
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	số_nguyên	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    không_thì
	isn = generate_instr_drop(cctx, isn_type, 1);
    nếu (isn == NULL)
	trả_về FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    trả_về OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    số_nguyên
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	số_nguyên	    sid,
	số_nguyên	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    nếu (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    không_thì
	isn = generate_instr_drop(cctx, isn_type, 1);
    nếu (isn == NULL)
	trả_về FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    nếu (sref == NULL)
	trả_về FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    trả_về OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    số_nguyên
generate_NEWLIST(cctx_T *cctx, số_nguyên count, số_nguyên use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_NEWLIST)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    nếu ((member_type = get_member_type_from_stack(count, 1, cctx)) == NULL)
	trả_về FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the list type to the type stack
    trả_về push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    số_nguyên
generate_NEWTUPLE(cctx_T *cctx, số_nguyên count, số_nguyên use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    nếu (get_tuple_type_from_stack(count, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	trả_về FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the tuple type to the type stack
    trả_về push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    số_nguyên
generate_NEWDICT(cctx_T *cctx, số_nguyên count, số_nguyên use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_NEWDICT)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    nếu ((member_type = get_member_type_from_stack(count, 2, cctx)) == NULL)
	trả_về FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_stack.ga_len -= 2 * count;

    // add the dict type to the type stack
    trả_về push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    số_nguyên
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	số_nguyên	    object_method,
	số_nguyên	    fi,
	số_nguyên	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    số_nguyên		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_FUNCREF)) == NULL)
	trả_về FAIL;
    nếu (isn_idx != NULL)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    nếu (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != NULL)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	nếu (extra == NULL)
	    trả_về FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	nếu (cl != NULL)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    nếu (ufunc->uf_def_status == UF_NOT_COMPILED || cl != NULL)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    nếu (ufunc->uf_def_status != UF_NOT_COMPILED && cl == NULL)
    {
	nếu (isn_idx == NULL && ufunc->uf_def_status == UF_TO_BE_COMPILED)
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
    nếu ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == NULL ? &t_func_any : ufunc->uf_func_type;
    trả_về push_type_stack(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    số_nguyên
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    số_nguyên		ret = OK;

    nếu (cctx->ctx_skip != SKIP_YES)
    {
	nếu ((isn = generate_instr(cctx, ISN_NEWFUNC)) == NULL)
	    ret = FAIL;
	không_thì
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    nếu (arg == NULL)
		ret = FAIL;
	    không_thì
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(void)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		trả_về OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    trả_về ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    số_nguyên
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_DEF)) == NULL)
	trả_về FAIL;
    nếu (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	nếu (isn->isn_arg.string == NULL)
	    trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    số_nguyên
generate_JUMP(cctx_T *cctx, jumpwhen_T when, số_nguyên where)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_JUMP)) == NULL)
	trả_về FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    nếu (when != JUMP_ALWAYS && stack->ga_len > 0)
	--stack->ga_len;

    trả_về OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    số_nguyên
generate_WHILE(cctx_T *cctx, số_nguyên funcref_idx)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_WHILE)) == NULL)
	trả_về FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    nếu (stack->ga_len > 0)
	--stack->ga_len;

    trả_về OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    số_nguyên
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, số_nguyên arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, isn_type)) == NULL)
	trả_về FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    trả_về OK;
}

    số_nguyên
generate_FOR(cctx_T *cctx, số_nguyên loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_FOR)) == NULL)
	trả_về FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    trả_về push_type_stack(cctx, &t_any);
}

    số_nguyên
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_ENDLOOP)) == NULL)
	trả_về FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_count;
    isn->isn_arg.endloop.end_var_count =
			   cctx->ctx_locals.ga_len - loop_info->li_local_count;
    trả_về OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    số_nguyên
generate_TRYCONT(cctx_T *cctx, số_nguyên levels, số_nguyên where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_TRYCONT)) == NULL)
	trả_về FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    trả_về OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    số_nguyên
check_internal_func_args(
	cctx_T	*cctx,
	số_nguyên	func_idx,
	số_nguyên	argcount,
	số_nguyên	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    số_nguyên		argoff = check_internal_func(func_idx, argcount);

    nếu (argoff < 0)
	trả_về FAIL;

    nếu (method_call && argoff > 1)
    {
	nếu (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    trả_về FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	nếu (isn  == NULL)
	    trả_về FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    nếu (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)stack->ga_data) + stack->ga_len - argcount;

	// Check the types of the arguments.
	nếu (method_call && argoff > 1)
	{
	    số_nguyên i;

	    cho (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	không_thì
	{
	    số_nguyên i;

	    cho (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	nếu (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    trả_về FAIL;
    }
    trả_về OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    số_nguyên
generate_BCALL(cctx_T *cctx, số_nguyên func_idx, số_nguyên argcount, số_nguyên method_call)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*argtypes = NULL;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = NULL;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    nếu (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	trả_về FAIL;

    nếu (internal_func_is_map(func_idx))
	maptype = argtypes;

    nếu ((isn = generate_instr(cctx, ISN_BCALL)) == NULL)
	trả_về FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    stack->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    nếu (push_type_stack2(cctx, type, decl_type) == FAIL)
	trả_về FAIL;

    nếu (maptype != NULL && maptype[0].type_decl->tt_member != NULL
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    trả_về OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    số_nguyên
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_stack(cctx, 1);
    nếu (arg_type_modifiable(list_type, 1) == FAIL)
	trả_về FAIL;
    item_type = get_type_on_stack(cctx, 0);
    expected = list_type->tt_member;
    nếu (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	trả_về FAIL;

    nếu (generate_instr(cctx, ISN_LISTAPPEND) == NULL)
	trả_về FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    trả_về OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    số_nguyên
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    nếu (arg_type_modifiable(get_decl_type_on_stack(cctx, 1), 1) == FAIL)
	trả_về FAIL;
    item_type = get_type_on_stack(cctx, 0);
    nếu (need_type(item_type, &t_number, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	trả_về FAIL;

    nếu (generate_instr(cctx, ISN_BLOBAPPEND) == NULL)
	trả_về FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    trả_về OK;
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
    nếu (cl != NULL)
	trả_về ISN_METHODCALL;

    nếu (ufunc->uf_def_status == UF_NOT_COMPILED)
	trả_về ISN_UCALL;

    // function invoked from a lambda
    nếu (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	nếu (ufunc->uf_name[0] == K_SPECIAL)
	    trả_về ISN_UCALL;
    }

    trả_về ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    số_nguyên
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	số_nguyên	    mi,
	số_nguyên	    pushed_argcount,
	số_nguyên	    is_super)
{
    isn_T	*isn;
    số_nguyên		regular_args = ufunc->uf_args.ga_len;
    số_nguyên		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    nếu (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	trả_về FAIL;
    }
    nếu (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	trả_về FAIL;
    }

    nếu (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	số_nguyên		i;
	compiletype_T	compile_type;

	cho (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_stack(cctx, argcount - i - 1);
	    nếu (check_type_is_value(actual) == FAIL)
		trả_về FAIL;
	    nếu (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		continue;
	    }
	    nếu (i < regular_args)
	    {
		nếu (ufunc->uf_arg_types == NULL)
		    continue;
		expected = ufunc->uf_arg_types[i];
	    }
	    không_thì nếu (ufunc->uf_va_type == NULL
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    không_thì
		expected = ufunc->uf_va_type->tt_member;
	    nếu (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		trả_về FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	nếu (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == NULL,
						   compile_type, NULL) == FAIL)
	    trả_về FAIL;
    }
    nếu (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	trả_về FAIL;
    }

    nếu ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == NULL)
	trả_về FAIL;
    nếu (cl != NULL /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	nếu (isn->isn_arg.mfunc == NULL)
	    trả_về FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = NULL;
	nếu (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = NULL;
	isn->isn_arg.mfunc->cmf_cache_generic = NULL;
	isn->isn_arg.mfunc->cmf_cache_ufunc = NULL;
    }
    không_thì nếu (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    không_thì
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    nếu (ufunc->uf_class != NULL)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_stack(cctx, 0);
	nếu (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_stack.ga_len--;
    }

    // add return type
    trả_về push_type_stack(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    số_nguyên
generate_UCALL(cctx_T *cctx, char_u *name, số_nguyên argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_UCALL)) == NULL)
	trả_về FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // add return value
    trả_về push_type_stack(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    số_nguyên
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	số_nguyên	argcount,
	số_nguyên	at_top,
	char_u	*name)
{
    nếu (type->tt_argcount != -1)
    {
	số_nguyên	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	nếu (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    trả_về FAIL;
	}
	nếu (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    trả_về FAIL;
	}
	nếu (type->tt_args != NULL)
	{
	    số_nguyên i;

	    cho (i = 0; i < argcount; ++i)
	    {
		số_nguyên	offset = -argcount + i - (at_top ? 0 : 1);
		type_T	*actual = get_type_on_stack(cctx, -1 - offset);
		type_T	*expected;

		nếu (check_type_is_value(actual) == FAIL)
		    trả_về FAIL;
		nếu (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    nếu (expected != NULL && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    nếu (expected == NULL)
			expected = &t_any;
		}
		không_thì nếu (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		không_thì
		    expected = type->tt_args[i];
		nếu (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    trả_về FAIL;
		}
	    }
	}
    }

    trả_về OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    số_nguyên
generate_PCALL(
	cctx_T	*cctx,
	số_nguyên	argcount,
	char_u	*name,
	type_T	*type,
	số_nguyên	at_top)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    nếu (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    không_thì nếu (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	nếu (check_func_args_from_type(cctx, type, argcount, at_top, name)
								       == FAIL)
	    trả_về FAIL;

	ret_type = type->tt_member;
	nếu (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    không_thì
    {
	semsg(_(e_not_callable_type_str), name);
	trả_về FAIL;
    }

    nếu ((isn = generate_instr(cctx, ISN_PCALL)) == NULL)
	trả_về FAIL;
    isn->isn_arg.pfunc.cpf_top = at_top;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_stack.ga_len -= argcount + 1;

    // push the return value
    push_type_stack(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    nếu (at_top && generate_instr(cctx, ISN_PCALL_END) == NULL)
	trả_về FAIL;

    trả_về OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    số_nguyên
generate_DEFER(cctx_T *cctx, số_nguyên var_idx, số_nguyên argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == NULL)
	trả_về FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    trả_về OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    số_nguyên
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == NULL)
	trả_về FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_stack(cctx, 0);
    nếu (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	char *tofree;

	semsg(_(e_expected_dictionary_for_using_key_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	trả_về FAIL;
    }
    // change dict type to dict member type
    nếu (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_stack(cctx, ntype, 0);
    }

    trả_về OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    số_nguyên
generate_ECHO(cctx_T *cctx, số_nguyên with_white, số_nguyên count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_drop(cctx, ISN_ECHO, count)) == NULL)
	trả_về FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_count = count;

    trả_về OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    số_nguyên
generate_MULT_EXPR(cctx_T *cctx, isntype_T isn_type, số_nguyên count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr_drop(cctx, isn_type, count)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = count;
    trả_về OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    số_nguyên
generate_ECHOWINDOW(cctx_T *cctx, số_nguyên count, long time)
{
    isn_T	*isn;

    nếu ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, count)) == NULL)
	trả_về FAIL;
    isn->isn_arg.echowin.ewin_count = count;
    isn->isn_arg.echowin.ewin_time = time;
    trả_về OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    số_nguyên
generate_SOURCE(cctx_T *cctx, số_nguyên sid)
{
    isn_T	*isn;

    nếu ((isn = generate_instr(cctx, ISN_SOURCE)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = sid;

    trả_về OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    số_nguyên
generate_PUT(cctx_T *cctx, số_nguyên regname, linenr_T lnum, số_nguyên fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    nếu (isn == NULL)
	trả_về FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    trả_về OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    số_nguyên
generate_LOCKUNLOCK(cctx_T *cctx, char_u *line, số_nguyên is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == NULL)
	trả_về FAIL;
    class_T *cl = cctx->ctx_ufunc != NULL ? cctx->ctx_ufunc->uf_class : NULL;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(line);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    nếu (cl != NULL)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    trả_về OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    số_nguyên
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, isntype)) == NULL)
	trả_về FAIL;
    isn->isn_arg.string = vim_strsave(line);
    trả_về OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    số_nguyên
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    số_nguyên		ret = OK;

    nếu (cctx->ctx_skip != SKIP_YES)
    {
	nếu ((isn = generate_instr(cctx, isntype)) == NULL)
	    ret = FAIL;
	không_thì
	{
	    isn->isn_arg.string = str;
	    trả_về OK;
	}
    }
    vim_free(str);
    trả_về ret;
}

    số_nguyên
generate_LEGACY_EVAL(cctx_T *cctx, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == NULL)
	trả_về FAIL;
    isn->isn_arg.string = vim_strsave(line);

    trả_về push_type_stack(cctx, &t_any);
}

    số_nguyên
generate_EXECCONCAT(cctx_T *cctx, số_nguyên count)
{
    isn_T	*isn;

    nếu ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, count)) == NULL)
	trả_về FAIL;
    isn->isn_arg.number = count;
    trả_về OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    số_nguyên
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    nếu ((isn = generate_instr(cctx, ISN_RANGE)) == NULL)
	trả_về FAIL;
    isn->isn_arg.string = range;

    trả_về push_type_stack(cctx, &t_number);
}

    số_nguyên
generate_UNPACK(cctx_T *cctx, số_nguyên var_count, số_nguyên semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_UNPACK)) == NULL)
	trả_về FAIL;
    isn->isn_arg.unpack.unp_count = var_count;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    trả_về OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    số_nguyên
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    nếu (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	nếu ((isn = generate_instr(cctx, ISN_CMDMOD)) == NULL)
	    trả_về FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	nếu (isn->isn_arg.cmdmod.cf_cmdmod == NULL)
	    trả_về FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, sizeof(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = NULL;
    }

    trả_về OK;
}

    số_nguyên
generate_undo_cmdmods(cctx_T *cctx)
{
    nếu (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == NULL)
	trả_về FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    trả_về OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    số_nguyên
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	số_nguyên		opt_flags,
	số_nguyên		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    switch (dest)
    {
	case dest_option:
	    trả_về generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	case dest_func_option:
	    trả_về generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	case dest_global:
	    // include g: with the name, easier to execute that way
	    trả_về generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == NULL
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	case dest_buffer:
	    // include b: with the name, easier to execute that way
	    trả_về generate_STORE(cctx, ISN_STOREB, 0, name);
	case dest_window:
	    // include w: with the name, easier to execute that way
	    trả_về generate_STORE(cctx, ISN_STOREW, 0, name);
	case dest_tab:
	    // include t: with the name, easier to execute that way
	    trả_về generate_STORE(cctx, ISN_STORET, 0, name);
	case dest_env:
	    trả_về generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	case dest_reg:
	    trả_về generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], NULL);
	case dest_vimvar:
	    trả_về generate_STORE(cctx, ISN_STOREV, vimvaridx, NULL);
	case dest_script:
	case dest_script_v9:
	    {
		số_nguyên	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		số_nguyên	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		nếu (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    nếu (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == NULL)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			nếu (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    trả_về FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    trả_về generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		trả_về generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	case dest_class_member:
	    trả_về generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	case dest_local:
	case dest_expr:
	    // cannot happen
	    break;
    }
    trả_về FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    số_nguyên
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    cho (;;)
    {
	nếu (scope == NULL)
	    break;
	nếu (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    trả_về TRUE;
	scope = scope->se_outer;
    }
    trả_về FALSE;
}

    số_nguyên
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, số_nguyên instr_count, số_nguyên is_decl)
{
    nếu (lhs->lhs_dest != dest_local)
	trả_về generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    nếu (lhs->lhs_lvar == NULL)
	trả_về OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    nếu (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_count + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.number;
	garray_T    *stack = &cctx->ctx_type_stack;

	nếu (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	không_thì
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	nếu (stack->ga_len > 0)
	    --stack->ga_len;
    }
    không_thì nếu (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    không_thì
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, NULL);
    trả_về OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    số_nguyên
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    nếu ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == NULL)
	trả_về FAIL;
    isn->isn_arg.setsctx = new_sctx;
    trả_về OK;
}

#nếu defined(FEAT_PROFILE)
    void
may_generate_prof_end(cctx_T *cctx, số_nguyên prof_lnum)
{
    nếu (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
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
		số_nguyên	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		cho (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    break;

	case ISN_INSTR:
	    {
		số_nguyên	idx;
		isn_T	*list = isn->isn_arg.instr;

		cho (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
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

		nếu (extra == NULL || extra->fre_func_name == NULL)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    nếu (ufunc != NULL && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		nếu (extra != NULL)
		{
		    char_u *name = extra->fre_func_name;

		    nếu (name != NULL)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    nếu (extra->fre_class != NULL)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    break;

	case ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		nếu (dfunc->df_ufunc != NULL
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

		nếu (arg != NULL)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    nếu (ufunc != NULL)
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
    số_nguyên idx;

    cho (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
