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

#만약 defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) 만약 (cctx->ctx_skip == SKIP_YES) 반환 NULL
#define RETURN_OK_IF_SKIP(cctx) 만약 (cctx->ctx_skip == SKIP_YES) 반환 OK

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
    만약 (GA_GROW_FAILS(instr, 1))
	반환 NULL;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    반환 isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, 정수 drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_stack.ga_len -= drop;
    반환 generate_instr(cctx, isn_type);
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

    만약 ((isn = generate_instr(cctx, isn_type)) == NULL)
	반환 NULL;

    만약 (push_type_stack2(cctx, type == NULL ? &t_any : type,
			       decl_type == NULL ? &t_any : decl_type) == FAIL)
	반환 NULL;

    반환 isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    반환 generate_instr_type2(cctx, isn_type, type, &t_any);
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

    만약 ((isn = generate_instr(cctx, ISN_DEBUG)) == NULL)
	반환 NULL;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    반환 isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    정수
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == NULL)
	반환 FAIL;
    isn->isn_arg.construct.construct_size = sizeof(object_T)
			       + cl->class_obj_member_count * sizeof(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    반환 OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    정수
generate_GET_OBJ_MEMBER(cctx_T *cctx, 정수 idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    만약 (isn == NULL)
	반환 FAIL;

    isn->isn_arg.classmember.cm_class = NULL;
    isn->isn_arg.classmember.cm_idx = idx;
    반환 push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    정수
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, 정수 idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    만약 (isn == NULL)
	반환 FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    반환 push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    정수
generate_STORE_THIS(cctx_T *cctx, 정수 idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    만약 (isn == NULL)
	반환 FAIL;

    isn->isn_arg.number = idx;
    반환 OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    정수
may_generate_2STRING(정수 offset, 정수 tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_stack(cctx, -1 - offset);

    switch (type->tt_type)
    {
	// nothing to be done
	case VAR_STRING: 반환 OK;

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
			 만약 (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     break;
			 }
			 만약 (tostring_flags & TOSTRING_INTERPOLATE)
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
			 반환 FAIL;
    }

    set_type_on_stack(cctx, &t_string, -1 - offset);
    만약 ((isn = generate_instr(cctx, isntype)) == NULL)
	반환 FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    반환 OK;
}

    static 정수
check_number_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    만약 (!((type1 == VAR_NUMBER || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_NUMBER || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	만약 (check_type_is_value(typ1) == FAIL
		|| check_type_is_value(typ2) == FAIL)
	    반환 FAIL;
	만약 (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	아니면
	    semsg(_(e_char_requires_number_or_float_arguments), *op);
	반환 FAIL;
    }
    반환 OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    static 정수
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    위해 (정수 i = 0; i < tuple_type->tt_argcount; i++)
    {
	만약 (ga_grow(gap, 1) == FAIL)
	    반환 FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    반환 OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    static 정수
set_tuple_union_type_on_stack(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    만약 (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    만약 (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    만약 ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_stack(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    반환 OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    정수
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

    만약 (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_number_or_float(type1, type2, (char_u *)"+") == FAIL)
	반환 FAIL;

    만약 (isn != NULL)
    {
	만약 (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = expr_type;
	아니면
	    isn->isn_arg.op.op_type = EXPR_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    만약 (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_stack(cctx, &t_list_any, 0);
    아니면 만약 (vartype == VAR_TUPLE)
    {
	만약 (!check_tuples_addable(type1, type2))
	    반환 FAIL;

	만약 (set_tuple_union_type_on_stack(type1, type2, cctx) == FAIL)
	    반환 FAIL;
    }

    반환 isn == NULL ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    만약 (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_NUMBER
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	반환 type1->tt_type;
    반환 VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    정수
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
		  만약 (generate_add_instr(cctx, vartype, type1, type2,
							    EXPR_COPY) == FAIL)
		      반환 FAIL;
		  break;

	case '-':
	case '*':
	case '/': 만약 (check_number_or_float(type1, type2, op) == FAIL)
		      반환 FAIL;
		  만약 (vartype == VAR_NUMBER)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  아니면 만약 (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  아니면
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  만약 (isn != NULL)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? EXPR_MULT : *op == '/'? EXPR_DIV : EXPR_SUB;
		  break;

	case '%': 만약 ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_NUMBER)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_NUMBER))
		  {
		      emsg(_(e_percent_requires_number_arguments));
		      반환 FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_NUMBER ? ISN_OPNR : ISN_OPANY, 1);
		  만약 (isn != NULL)
		      isn->isn_arg.op.op_type = EXPR_REM;
		  break;
    }

    // correct type of result
    만약 (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	만약 ((type1->tt_type == VAR_NUMBER || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_NUMBER || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_stack(cctx, type, 0);
    }

    반환 OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    static isntype_T
compare_isn_not_values(typval_T *tv, type_T *type)
{
    만약 (tv != NULL)
	(void)check_typval_is_value(tv);
    아니면
	(void)check_type_is_value(type);
    반환 ISN_DROP;
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

    만약 (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	반환 compare_isn_not_values(tv1, type1);
    만약 (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	반환 compare_isn_not_values(tv2, type2);

    만약 (vartype1 == vartype2)
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
    아니면 만약 (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_NUMBER || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_NUMBER || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    아니면 만약 (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	만약 ((vartype1 == VAR_SPECIAL
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
	    반환 ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    만약 ((exprtype == EXPR_IS || exprtype == EXPR_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == EXPR_IS ? "is" : "isnot" , vartype_name(vartype1));
	반환 ISN_DROP;
    }
    만약 (!(exprtype == EXPR_IS || exprtype == EXPR_ISNOT
		|| exprtype == EXPR_EQUAL || exprtype == EXPR_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	반환 ISN_DROP;
    }
    만약 (isntype == ISN_DROP
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
	반환 ISN_DROP;
    }
    반환 isntype;
}

    정수
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    만약 (get_compare_isn(type, tv1, tv2, NULL, NULL) == ISN_DROP)
	반환 FAIL;
    반환 OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    정수
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, 정수 ic)
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
    만약 (isntype == ISN_DROP)
	반환 FAIL;

    만약 ((isn = generate_instr(cctx, isntype)) == NULL)
	반환 FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --stack->ga_len;
    set_type_on_stack(cctx, &t_bool, 0);

    반환 OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    정수
generate_CONCAT(cctx_T *cctx, 정수 count)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);

    만약 ((isn = generate_instr(cctx, ISN_CONCAT)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = count;

    // drop the argument types
    stack->ga_len -= count - 1;

    반환 OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    정수
generate_2BOOL(cctx_T *cctx, 정수 invert, 정수 offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_2BOOL)) == NULL)
	반환 FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, -1 - offset);

    반환 OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    정수
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    만약 (generate_instr(cctx, ISN_COND2BOOL) == NULL)
	반환 FAIL;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, 0);

    반환 OK;
}

    정수
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	정수	    typechk_flags,	// type check flags
	정수	    offset,
	정수	    is_var,
	정수	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == NULL)
	반환 FAIL;
    type_T *tt;
    만약 ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_NUMBER_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	만약 (tt != NULL)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    만약 (typechk_flags & TYPECHK_NUMBER_OK)
		tt->tt_flags |= TTFLAG_NUMBER_OK;
	    아니면
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    아니면
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_stack(cctx, expected, -1 - offset);

    반환 OK;
}

    정수
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_SETTYPE)) == NULL)
	반환 FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    반환 OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    정수
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    만약 (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == NULL)
	반환 FAIL;
    반환 OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    static 정수
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == NULL ? &t_any : &class->class_type);
    만약 (isn == NULL)
	반환 FAIL;
    isn->isn_arg.classarg = class;
    만약 (class != NULL)
	++class->class_refcount;
    반환 OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    정수
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
	    만약 (tv->vval.v_list != NULL)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    break;
	case VAR_TUPLE:
	    만약 (tv->vval.v_tuple != NULL)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    break;
	case VAR_DICT:
	    만약 (tv->vval.v_dict != NULL)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    break;
#ifdef FEAT_JOB_CHANNEL
	case VAR_JOB:
	    만약 (tv->vval.v_job != NULL)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    break;
	case VAR_CHANNEL:
	    만약 (tv->vval.v_channel != NULL)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    break;
#endif
	case VAR_FUNC:
	    만약 (tv->vval.v_string != NULL)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, NULL, &t_func_unknown, TRUE);
	    break;
	case VAR_PARTIAL:
	    만약 (tv->vval.v_partial != NULL)
		iemsg("non-null partial constant not supported");
	    만약 (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == NULL)
		반환 FAIL;
	    break;
	case VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = NULL;
	    break;
	case VAR_OBJECT:
	    만약 (tv->vval.v_object != NULL)
	    {
		emsg(_(e_cannot_use_non_null_object));
		반환 FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    break;
	case VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    break;
	default:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    반환 FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    반환 OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    정수
generate_PUSHNR(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_number)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = number;

    만약 (number == 0 || number == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_stack(cctx, &t_number_bool, 0);
    반환 OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    정수
generate_PUSHBOOL(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = number;

    반환 OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    정수
generate_PUSHSPEC(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     number == VVAL_NULL ? &t_null : &t_none)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = number;

    반환 OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    정수
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == NULL)
	반환 FAIL;
    isn->isn_arg.fnumber = fnumber;

    반환 OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    정수
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    정수		ret = OK;

    만약 (cctx->ctx_skip != SKIP_YES)
    {
	만약 ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == NULL)
	    ret = FAIL;
	아니면
	{
	    isn->isn_arg.string = str == NULL ? NULL : *str;
	    반환 OK;
	}
    }
    만약 (str != NULL)
	VIM_CLEAR(*str);
    반환 ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    정수
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    만약 (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == NULL)
	반환 FAIL;
    반환 OK;
#아니면
    emsg(_(e_channel_job_feature_not_available));
    반환 FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    정수
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    만약 (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == NULL)
	반환 FAIL;
    반환 OK;
#아니면
    emsg(_(e_channel_job_feature_not_available));
    반환 FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    정수
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == NULL)
	반환 FAIL;
    isn->isn_arg.blob = blob;

    반환 OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    정수
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, 정수 may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == NULL)
	반환 FAIL;
    만약 (name == NULL)
	funcname = NULL;
    아니면 만약 (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != NULL)	    // autoload
	funcname = vim_strsave(name);
    아니면
    {
	funcname = alloc(STRLEN(name) + 3);
	만약 (funcname != NULL)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    반환 OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    정수
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == NULL)
	반환 FAIL;
    isn->isn_arg.string = vim_strsave(name);
    만약 (isn->isn_arg.string == NULL)
	반환 FAIL;
    반환 OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    정수
generate_GETITEM(cctx_T *cctx, 정수 index, 정수 with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_stack(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    만약 ((isn = generate_instr(cctx, ISN_GETITEM)) == NULL)
	반환 FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    반환 push_type_stack(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    static void
set_tuple_slice_type_on_stack(type_T *type, 정수 count, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    만약 ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= count)
    {
	set_type_on_stack(cctx, &t_tuple_any, 0);
	반환;
    }

    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    위해 (정수 i = count; i < type->tt_argcount; i++)
    {
	만약 (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_stack(cctx, &t_tuple_any, 0);
	    반환;
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
    정수
generate_SLICE(cctx_T *cctx, 정수 count)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_SLICE)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = count;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_stack(cctx, 0);
    만약 (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_stack(type, count, cctx);

    반환 OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    정수
generate_CHECKLEN(cctx_T *cctx, 정수 min_len, 정수 more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    만약 ((isn = generate_instr(cctx, ISN_CHECKLEN)) == NULL)
	반환 FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    반환 OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    정수
generate_STORE(cctx_T *cctx, isntype_T isn_type, 정수 idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	반환 FAIL;
    만약 (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    아니면
	isn->isn_arg.number = idx;

    반환 OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    정수
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	정수	    load,
	class_T	    *cl,
	정수	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    아니면
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    만약 (isn == NULL)
	반환 FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    반환 OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    static 정수
generate_STOREOUTER(
	cctx_T	    *cctx,
	정수	    idx,
	정수	    level,
	정수	    loop_depth,
	정수	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == NULL)
	반환 FAIL;
    만약 (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    아니면
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    반환 OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    정수
generate_STORENR(cctx_T *cctx, 정수 idx, varnumber_T value)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_STORENR)) == NULL)
	반환 FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = value;

    반환 OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    static 정수
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	정수	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	반환 FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    반환 OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    정수
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	정수	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_type2(cctx, isn_type, type, type)) == NULL)
	반환 FAIL;
    만약 (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    아니면
	isn->isn_arg.number = idx;

    반환 OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    정수
generate_LOADOUTER(
	cctx_T	    *cctx,
	정수	    idx,
	정수	    nesting,
	정수	    loop_depth,
	정수	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == NULL)
	반환 FAIL;
    만약 (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    아니면
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    반환 OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    정수
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    정수	    di_flags;
    정수	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    만약 (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	반환 FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    반환 generate_LOAD(cctx, ISN_LOADV, vidx, NULL, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    정수
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, 정수 forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, isn_type)) == NULL)
	반환 FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    반환 OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    정수
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    만약 (generate_instr(cctx, ISN_LOCKCONST) == NULL)
	반환 FAIL;
    반환 OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    정수
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	정수	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    아니면
	isn = generate_instr_drop(cctx, isn_type, 1);
    만약 (isn == NULL)
	반환 FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    반환 OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    정수
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	정수	    sid,
	정수	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    만약 (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    아니면
	isn = generate_instr_drop(cctx, isn_type, 1);
    만약 (isn == NULL)
	반환 FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    만약 (sref == NULL)
	반환 FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    반환 OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    정수
generate_NEWLIST(cctx_T *cctx, 정수 count, 정수 use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_NEWLIST)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    만약 ((member_type = get_member_type_from_stack(count, 1, cctx)) == NULL)
	반환 FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the list type to the type stack
    반환 push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    정수
generate_NEWTUPLE(cctx_T *cctx, 정수 count, 정수 use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, sizeof(type_T *), 10);

    만약 (get_tuple_type_from_stack(count, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	반환 FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the tuple type to the type stack
    반환 push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    정수
generate_NEWDICT(cctx_T *cctx, 정수 count, 정수 use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_NEWDICT)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    만약 ((member_type = get_member_type_from_stack(count, 2, cctx)) == NULL)
	반환 FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_stack.ga_len -= 2 * count;

    // add the dict type to the type stack
    반환 push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    정수
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	정수	    object_method,
	정수	    fi,
	정수	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    정수		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_FUNCREF)) == NULL)
	반환 FAIL;
    만약 (isn_idx != NULL)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    만약 (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != NULL)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	만약 (extra == NULL)
	    반환 FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	만약 (cl != NULL)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    만약 (ufunc->uf_def_status == UF_NOT_COMPILED || cl != NULL)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    만약 (ufunc->uf_def_status != UF_NOT_COMPILED && cl == NULL)
    {
	만약 (isn_idx == NULL && ufunc->uf_def_status == UF_TO_BE_COMPILED)
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
    만약 ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == NULL ? &t_func_any : ufunc->uf_func_type;
    반환 push_type_stack(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    정수
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    정수		ret = OK;

    만약 (cctx->ctx_skip != SKIP_YES)
    {
	만약 ((isn = generate_instr(cctx, ISN_NEWFUNC)) == NULL)
	    ret = FAIL;
	아니면
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    만약 (arg == NULL)
		ret = FAIL;
	    아니면
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(void)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		반환 OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    반환 ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    정수
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_DEF)) == NULL)
	반환 FAIL;
    만약 (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	만약 (isn->isn_arg.string == NULL)
	    반환 FAIL;
    }
    반환 OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    정수
generate_JUMP(cctx_T *cctx, jumpwhen_T when, 정수 where)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_JUMP)) == NULL)
	반환 FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    만약 (when != JUMP_ALWAYS && stack->ga_len > 0)
	--stack->ga_len;

    반환 OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    정수
generate_WHILE(cctx_T *cctx, 정수 funcref_idx)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_WHILE)) == NULL)
	반환 FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    만약 (stack->ga_len > 0)
	--stack->ga_len;

    반환 OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    정수
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, 정수 arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, isn_type)) == NULL)
	반환 FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    반환 OK;
}

    정수
generate_FOR(cctx_T *cctx, 정수 loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_FOR)) == NULL)
	반환 FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    반환 push_type_stack(cctx, &t_any);
}

    정수
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_ENDLOOP)) == NULL)
	반환 FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_count;
    isn->isn_arg.endloop.end_var_count =
			   cctx->ctx_locals.ga_len - loop_info->li_local_count;
    반환 OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    정수
generate_TRYCONT(cctx_T *cctx, 정수 levels, 정수 where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_TRYCONT)) == NULL)
	반환 FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    반환 OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    정수
check_internal_func_args(
	cctx_T	*cctx,
	정수	func_idx,
	정수	argcount,
	정수	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    정수		argoff = check_internal_func(func_idx, argcount);

    만약 (argoff < 0)
	반환 FAIL;

    만약 (method_call && argoff > 1)
    {
	만약 (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    반환 FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	만약 (isn  == NULL)
	    반환 FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    만약 (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)stack->ga_data) + stack->ga_len - argcount;

	// Check the types of the arguments.
	만약 (method_call && argoff > 1)
	{
	    정수 i;

	    위해 (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	아니면
	{
	    정수 i;

	    위해 (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	만약 (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    반환 FAIL;
    }
    반환 OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    정수
generate_BCALL(cctx_T *cctx, 정수 func_idx, 정수 argcount, 정수 method_call)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*argtypes = NULL;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = NULL;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    만약 (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	반환 FAIL;

    만약 (internal_func_is_map(func_idx))
	maptype = argtypes;

    만약 ((isn = generate_instr(cctx, ISN_BCALL)) == NULL)
	반환 FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    stack->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    만약 (push_type_stack2(cctx, type, decl_type) == FAIL)
	반환 FAIL;

    만약 (maptype != NULL && maptype[0].type_decl->tt_member != NULL
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    반환 OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    정수
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_stack(cctx, 1);
    만약 (arg_type_modifiable(list_type, 1) == FAIL)
	반환 FAIL;
    item_type = get_type_on_stack(cctx, 0);
    expected = list_type->tt_member;
    만약 (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	반환 FAIL;

    만약 (generate_instr(cctx, ISN_LISTAPPEND) == NULL)
	반환 FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    반환 OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    정수
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    만약 (arg_type_modifiable(get_decl_type_on_stack(cctx, 1), 1) == FAIL)
	반환 FAIL;
    item_type = get_type_on_stack(cctx, 0);
    만약 (need_type(item_type, &t_number, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	반환 FAIL;

    만약 (generate_instr(cctx, ISN_BLOBAPPEND) == NULL)
	반환 FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    반환 OK;
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
    만약 (cl != NULL)
	반환 ISN_METHODCALL;

    만약 (ufunc->uf_def_status == UF_NOT_COMPILED)
	반환 ISN_UCALL;

    // function invoked from a lambda
    만약 (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	만약 (ufunc->uf_name[0] == K_SPECIAL)
	    반환 ISN_UCALL;
    }

    반환 ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    정수
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	정수	    mi,
	정수	    pushed_argcount,
	정수	    is_super)
{
    isn_T	*isn;
    정수		regular_args = ufunc->uf_args.ga_len;
    정수		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    만약 (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	반환 FAIL;
    }
    만약 (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	반환 FAIL;
    }

    만약 (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	정수		i;
	compiletype_T	compile_type;

	위해 (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_stack(cctx, argcount - i - 1);
	    만약 (check_type_is_value(actual) == FAIL)
		반환 FAIL;
	    만약 (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		continue;
	    }
	    만약 (i < regular_args)
	    {
		만약 (ufunc->uf_arg_types == NULL)
		    continue;
		expected = ufunc->uf_arg_types[i];
	    }
	    아니면 만약 (ufunc->uf_va_type == NULL
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    아니면
		expected = ufunc->uf_va_type->tt_member;
	    만약 (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		반환 FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	만약 (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == NULL,
						   compile_type, NULL) == FAIL)
	    반환 FAIL;
    }
    만약 (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	반환 FAIL;
    }

    만약 ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == NULL)
	반환 FAIL;
    만약 (cl != NULL /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	만약 (isn->isn_arg.mfunc == NULL)
	    반환 FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = NULL;
	만약 (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = NULL;
	isn->isn_arg.mfunc->cmf_cache_generic = NULL;
	isn->isn_arg.mfunc->cmf_cache_ufunc = NULL;
    }
    아니면 만약 (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    아니면
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    만약 (ufunc->uf_class != NULL)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_stack(cctx, 0);
	만약 (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_stack.ga_len--;
    }

    // add return type
    반환 push_type_stack(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    정수
generate_UCALL(cctx_T *cctx, char_u *name, 정수 argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_UCALL)) == NULL)
	반환 FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // add return value
    반환 push_type_stack(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    정수
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	정수	argcount,
	정수	at_top,
	char_u	*name)
{
    만약 (type->tt_argcount != -1)
    {
	정수	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	만약 (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    반환 FAIL;
	}
	만약 (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    반환 FAIL;
	}
	만약 (type->tt_args != NULL)
	{
	    정수 i;

	    위해 (i = 0; i < argcount; ++i)
	    {
		정수	offset = -argcount + i - (at_top ? 0 : 1);
		type_T	*actual = get_type_on_stack(cctx, -1 - offset);
		type_T	*expected;

		만약 (check_type_is_value(actual) == FAIL)
		    반환 FAIL;
		만약 (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    만약 (expected != NULL && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    만약 (expected == NULL)
			expected = &t_any;
		}
		아니면 만약 (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		아니면
		    expected = type->tt_args[i];
		만약 (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    반환 FAIL;
		}
	    }
	}
    }

    반환 OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    정수
generate_PCALL(
	cctx_T	*cctx,
	정수	argcount,
	char_u	*name,
	type_T	*type,
	정수	at_top)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    만약 (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    아니면 만약 (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	만약 (check_func_args_from_type(cctx, type, argcount, at_top, name)
								       == FAIL)
	    반환 FAIL;

	ret_type = type->tt_member;
	만약 (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    아니면
    {
	semsg(_(e_not_callable_type_str), name);
	반환 FAIL;
    }

    만약 ((isn = generate_instr(cctx, ISN_PCALL)) == NULL)
	반환 FAIL;
    isn->isn_arg.pfunc.cpf_top = at_top;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_stack.ga_len -= argcount + 1;

    // push the return value
    push_type_stack(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    만약 (at_top && generate_instr(cctx, ISN_PCALL_END) == NULL)
	반환 FAIL;

    반환 OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    정수
generate_DEFER(cctx_T *cctx, 정수 var_idx, 정수 argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == NULL)
	반환 FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    반환 OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    정수
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == NULL)
	반환 FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_stack(cctx, 0);
    만약 (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	char *tofree;

	semsg(_(e_expected_dictionary_for_using_key_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	반환 FAIL;
    }
    // change dict type to dict member type
    만약 (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_stack(cctx, ntype, 0);
    }

    반환 OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    정수
generate_ECHO(cctx_T *cctx, 정수 with_white, 정수 count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_drop(cctx, ISN_ECHO, count)) == NULL)
	반환 FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_count = count;

    반환 OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    정수
generate_MULT_EXPR(cctx_T *cctx, isntype_T isn_type, 정수 count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr_drop(cctx, isn_type, count)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = count;
    반환 OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    정수
generate_ECHOWINDOW(cctx_T *cctx, 정수 count, long time)
{
    isn_T	*isn;

    만약 ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, count)) == NULL)
	반환 FAIL;
    isn->isn_arg.echowin.ewin_count = count;
    isn->isn_arg.echowin.ewin_time = time;
    반환 OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    정수
generate_SOURCE(cctx_T *cctx, 정수 sid)
{
    isn_T	*isn;

    만약 ((isn = generate_instr(cctx, ISN_SOURCE)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = sid;

    반환 OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    정수
generate_PUT(cctx_T *cctx, 정수 regname, linenr_T lnum, 정수 fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    만약 (isn == NULL)
	반환 FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    반환 OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    정수
generate_LOCKUNLOCK(cctx_T *cctx, char_u *line, 정수 is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == NULL)
	반환 FAIL;
    class_T *cl = cctx->ctx_ufunc != NULL ? cctx->ctx_ufunc->uf_class : NULL;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(line);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    만약 (cl != NULL)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    반환 OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    정수
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, isntype)) == NULL)
	반환 FAIL;
    isn->isn_arg.string = vim_strsave(line);
    반환 OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    정수
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    정수		ret = OK;

    만약 (cctx->ctx_skip != SKIP_YES)
    {
	만약 ((isn = generate_instr(cctx, isntype)) == NULL)
	    ret = FAIL;
	아니면
	{
	    isn->isn_arg.string = str;
	    반환 OK;
	}
    }
    vim_free(str);
    반환 ret;
}

    정수
generate_LEGACY_EVAL(cctx_T *cctx, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == NULL)
	반환 FAIL;
    isn->isn_arg.string = vim_strsave(line);

    반환 push_type_stack(cctx, &t_any);
}

    정수
generate_EXECCONCAT(cctx_T *cctx, 정수 count)
{
    isn_T	*isn;

    만약 ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, count)) == NULL)
	반환 FAIL;
    isn->isn_arg.number = count;
    반환 OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    정수
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    만약 ((isn = generate_instr(cctx, ISN_RANGE)) == NULL)
	반환 FAIL;
    isn->isn_arg.string = range;

    반환 push_type_stack(cctx, &t_number);
}

    정수
generate_UNPACK(cctx_T *cctx, 정수 var_count, 정수 semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_UNPACK)) == NULL)
	반환 FAIL;
    isn->isn_arg.unpack.unp_count = var_count;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    반환 OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    정수
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    만약 (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	만약 ((isn = generate_instr(cctx, ISN_CMDMOD)) == NULL)
	    반환 FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	만약 (isn->isn_arg.cmdmod.cf_cmdmod == NULL)
	    반환 FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, sizeof(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = NULL;
    }

    반환 OK;
}

    정수
generate_undo_cmdmods(cctx_T *cctx)
{
    만약 (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == NULL)
	반환 FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    반환 OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    정수
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	정수		opt_flags,
	정수		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    switch (dest)
    {
	case dest_option:
	    반환 generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	case dest_func_option:
	    반환 generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	case dest_global:
	    // include g: with the name, easier to execute that way
	    반환 generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == NULL
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	case dest_buffer:
	    // include b: with the name, easier to execute that way
	    반환 generate_STORE(cctx, ISN_STOREB, 0, name);
	case dest_window:
	    // include w: with the name, easier to execute that way
	    반환 generate_STORE(cctx, ISN_STOREW, 0, name);
	case dest_tab:
	    // include t: with the name, easier to execute that way
	    반환 generate_STORE(cctx, ISN_STORET, 0, name);
	case dest_env:
	    반환 generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	case dest_reg:
	    반환 generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], NULL);
	case dest_vimvar:
	    반환 generate_STORE(cctx, ISN_STOREV, vimvaridx, NULL);
	case dest_script:
	case dest_script_v9:
	    {
		정수	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		정수	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		만약 (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    만약 (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == NULL)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			만약 (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    반환 FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    반환 generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		반환 generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	case dest_class_member:
	    반환 generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	case dest_local:
	case dest_expr:
	    // cannot happen
	    break;
    }
    반환 FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    정수
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    위해 (;;)
    {
	만약 (scope == NULL)
	    break;
	만약 (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    반환 TRUE;
	scope = scope->se_outer;
    }
    반환 FALSE;
}

    정수
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, 정수 instr_count, 정수 is_decl)
{
    만약 (lhs->lhs_dest != dest_local)
	반환 generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    만약 (lhs->lhs_lvar == NULL)
	반환 OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    만약 (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_count + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.number;
	garray_T    *stack = &cctx->ctx_type_stack;

	만약 (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	아니면
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	만약 (stack->ga_len > 0)
	    --stack->ga_len;
    }
    아니면 만약 (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    아니면
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, NULL);
    반환 OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    정수
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    만약 ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == NULL)
	반환 FAIL;
    isn->isn_arg.setsctx = new_sctx;
    반환 OK;
}

#만약 defined(FEAT_PROFILE)
    void
may_generate_prof_end(cctx_T *cctx, 정수 prof_lnum)
{
    만약 (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
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
		정수	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		위해 (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    break;

	case ISN_INSTR:
	    {
		정수	idx;
		isn_T	*list = isn->isn_arg.instr;

		위해 (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
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

		만약 (extra == NULL || extra->fre_func_name == NULL)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    만약 (ufunc != NULL && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		만약 (extra != NULL)
		{
		    char_u *name = extra->fre_func_name;

		    만약 (name != NULL)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    만약 (extra->fre_class != NULL)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    break;

	case ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		만약 (dfunc->df_ufunc != NULL
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

		만약 (arg != NULL)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    만약 (ufunc != NULL)
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
    정수 idx;

    위해 (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
