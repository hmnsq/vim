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

#ຖ້າ defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) ຖ້າ (cctx->ctx_skip == SKIP_YES) ສົ່ງຄືນ NULL
#define RETURN_OK_IF_SKIP(cctx) ຖ້າ (cctx->ctx_skip == SKIP_YES) ສົ່ງຄືນ OK

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
    ຖ້າ (GA_GROW_FAILS(instr, 1))
	ສົ່ງຄືນ NULL;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    ສົ່ງຄືນ isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, ຈຳນວນເຕັມ drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_stack.ga_len -= drop;
    ສົ່ງຄືນ generate_instr(cctx, isn_type);
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

    ຖ້າ ((isn = generate_instr(cctx, isn_type)) == NULL)
	ສົ່ງຄືນ NULL;

    ຖ້າ (push_type_stack2(cctx, type == NULL ? &t_any : type,
			       decl_type == NULL ? &t_any : decl_type) == FAIL)
	ສົ່ງຄືນ NULL;

    ສົ່ງຄືນ isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    ສົ່ງຄືນ generate_instr_type2(cctx, isn_type, type, &t_any);
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

    ຖ້າ ((isn = generate_instr(cctx, ISN_DEBUG)) == NULL)
	ສົ່ງຄືນ NULL;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    ສົ່ງຄືນ isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    ຈຳນວນເຕັມ
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.construct.construct_size = parimaa_na(object_T)
			       + cl->class_obj_member_count * parimaa_na(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    ສົ່ງຄືນ OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    ຈຳນວນເຕັມ
generate_GET_OBJ_MEMBER(cctx_T *cctx, ຈຳນວນເຕັມ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    ຖ້າ (isn == NULL)
	ສົ່ງຄືນ FAIL;

    isn->isn_arg.classmember.cm_class = NULL;
    isn->isn_arg.classmember.cm_idx = idx;
    ສົ່ງຄືນ push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    ຈຳນວນເຕັມ
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, ຈຳນວນເຕັມ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    ຖ້າ (isn == NULL)
	ສົ່ງຄືນ FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    ສົ່ງຄືນ push_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    ຈຳນວນເຕັມ
generate_STORE_THIS(cctx_T *cctx, ຈຳນວນເຕັມ idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    ຖ້າ (isn == NULL)
	ສົ່ງຄືນ FAIL;

    isn->isn_arg.number = idx;
    ສົ່ງຄືນ OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    ຈຳນວນເຕັມ
may_generate_2STRING(ຈຳນວນເຕັມ offset, ຈຳນວນເຕັມ tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_stack(cctx, -1 - offset);

    switch (type->tt_type)
    {
	// nothing to be done
	pak_sa VAR_STRING: ສົ່ງຄືນ OK;

	// conversion possible
	pak_sa VAR_SPECIAL:
	pak_sa VAR_BOOL:
	pak_sa VAR_NUMBER:
	pak_sa VAR_FLOAT:
			 bha_nga;

	// conversion possible (with runtime check)
	pak_sa VAR_ANY:
	pak_sa VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 bha_nga;

	// conversion possible when tolerant
	pak_sa VAR_LIST:
	pak_sa VAR_TUPLE:
	pak_sa VAR_DICT:
			 ຖ້າ (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     bha_nga;
			 }
			 ຖ້າ (tostring_flags & TOSTRING_INTERPOLATE)
			     bha_nga;
			 // FALLTHROUGH

	// conversion not possible
	pak_sa VAR_VOID:
	pak_sa VAR_BLOB:
	pak_sa VAR_FUNC:
	pak_sa VAR_PARTIAL:
	pak_sa VAR_JOB:
	pak_sa VAR_CHANNEL:
	pak_sa VAR_INSTR:
	pak_sa VAR_CLASS:
	pak_sa VAR_OBJECT:
	pak_sa VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 ສົ່ງຄືນ FAIL;
    }

    set_type_on_stack(cctx, &t_string, -1 - offset);
    ຖ້າ ((isn = generate_instr(cctx, isntype)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    ສົ່ງຄືນ OK;
}

    static ຈຳນວນເຕັມ
check_number_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    ຖ້າ (!((type1 == VAR_NUMBER || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_NUMBER || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	ຖ້າ (check_type_is_value(typ1) == FAIL
		|| check_type_is_value(typ2) == FAIL)
	    ສົ່ງຄືນ FAIL;
	ຖ້າ (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	ບໍ່ດັ່ງນັ້ນ
	    semsg(_(e_char_requires_number_or_float_arguments), *op);
	ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    static ຈຳນວນເຕັມ
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    ສຳລັບ (ຈຳນວນເຕັມ i = 0; i < tuple_type->tt_argcount; i++)
    {
	ຖ້າ (ga_grow(gap, 1) == FAIL)
	    ສົ່ງຄືນ FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    ສົ່ງຄືນ OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    static ຈຳນວນເຕັມ
set_tuple_union_type_on_stack(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);

    ຖ້າ (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    ຖ້າ (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    ຖ້າ ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_stack(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    ສົ່ງຄືນ OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    ຈຳນວນເຕັມ
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

    ຖ້າ (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_number_or_float(type1, type2, (char_u *)"+") == FAIL)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (isn != NULL)
    {
	ຖ້າ (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = expr_type;
	ບໍ່ດັ່ງນັ້ນ
	    isn->isn_arg.op.op_type = EXPR_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    ຖ້າ (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_stack(cctx, &t_list_any, 0);
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (vartype == VAR_TUPLE)
    {
	ຖ້າ (!check_tuples_addable(type1, type2))
	    ສົ່ງຄືນ FAIL;

	ຖ້າ (set_tuple_union_type_on_stack(type1, type2, cctx) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }

    ສົ່ງຄືນ isn == NULL ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    ຖ້າ (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_NUMBER
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	ສົ່ງຄືນ type1->tt_type;
    ສົ່ງຄືນ VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    ຈຳນວນເຕັມ
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
	pak_sa '+':
		  ຖ້າ (generate_add_instr(cctx, vartype, type1, type2,
							    EXPR_COPY) == FAIL)
		      ສົ່ງຄືນ FAIL;
		  bha_nga;

	pak_sa '-':
	pak_sa '*':
	pak_sa '/': ຖ້າ (check_number_or_float(type1, type2, op) == FAIL)
		      ສົ່ງຄືນ FAIL;
		  ຖ້າ (vartype == VAR_NUMBER)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  ບໍ່ດັ່ງນັ້ນ ຖ້າ (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  ບໍ່ດັ່ງນັ້ນ
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  ຖ້າ (isn != NULL)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? EXPR_MULT : *op == '/'? EXPR_DIV : EXPR_SUB;
		  bha_nga;

	pak_sa '%': ຖ້າ ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_NUMBER)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_NUMBER))
		  {
		      emsg(_(e_percent_requires_number_arguments));
		      ສົ່ງຄືນ FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_NUMBER ? ISN_OPNR : ISN_OPANY, 1);
		  ຖ້າ (isn != NULL)
		      isn->isn_arg.op.op_type = EXPR_REM;
		  bha_nga;
    }

    // correct type of result
    ຖ້າ (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	ຖ້າ ((type1->tt_type == VAR_NUMBER || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_NUMBER || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_stack(cctx, type, 0);
    }

    ສົ່ງຄືນ OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    static isntype_T
compare_isn_not_values(typval_T *tv, type_T *type)
{
    ຖ້າ (tv != NULL)
	(void)check_typval_is_value(tv);
    ບໍ່ດັ່ງນັ້ນ
	(void)check_type_is_value(type);
    ສົ່ງຄືນ ISN_DROP;
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

    ຖ້າ (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	ສົ່ງຄືນ compare_isn_not_values(tv1, type1);
    ຖ້າ (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	ສົ່ງຄືນ compare_isn_not_values(tv2, type2);

    ຖ້າ (vartype1 == vartype2)
    {
	switch (vartype1)
	{
	    pak_sa VAR_BOOL: isntype = ISN_COMPAREBOOL; bha_nga;
	    pak_sa VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; bha_nga;
	    pak_sa VAR_NUMBER: isntype = ISN_COMPARENR; bha_nga;
	    pak_sa VAR_FLOAT: isntype = ISN_COMPAREFLOAT; bha_nga;
	    pak_sa VAR_STRING: isntype = ISN_COMPARESTRING; bha_nga;
	    pak_sa VAR_BLOB: isntype = ISN_COMPAREBLOB; bha_nga;
	    pak_sa VAR_LIST: isntype = ISN_COMPARELIST; bha_nga;
	    pak_sa VAR_TUPLE: isntype = ISN_COMPARETUPLE; bha_nga;
	    pak_sa VAR_DICT: isntype = ISN_COMPAREDICT; bha_nga;
	    pak_sa VAR_FUNC: isntype = ISN_COMPAREFUNC; bha_nga;
	    pak_sa VAR_OBJECT: isntype = ISN_COMPAREOBJECT; bha_nga;
	    default: isntype = ISN_COMPAREANY; bha_nga;
	}
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_NUMBER || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_NUMBER || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	ຖ້າ ((vartype1 == VAR_SPECIAL
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
	    ສົ່ງຄືນ ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    ຖ້າ ((exprtype == EXPR_IS || exprtype == EXPR_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == EXPR_IS ? "is" : "isnot" , vartype_name(vartype1));
	ສົ່ງຄືນ ISN_DROP;
    }
    ຖ້າ (!(exprtype == EXPR_IS || exprtype == EXPR_ISNOT
		|| exprtype == EXPR_EQUAL || exprtype == EXPR_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	ສົ່ງຄືນ ISN_DROP;
    }
    ຖ້າ (isntype == ISN_DROP
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
	ສົ່ງຄືນ ISN_DROP;
    }
    ສົ່ງຄືນ isntype;
}

    ຈຳນວນເຕັມ
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    ຖ້າ (get_compare_isn(type, tv1, tv2, NULL, NULL) == ISN_DROP)
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    ຈຳນວນເຕັມ
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, ຈຳນວນເຕັມ ic)
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
    ຖ້າ (isntype == ISN_DROP)
	ສົ່ງຄືນ FAIL;

    ຖ້າ ((isn = generate_instr(cctx, isntype)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --stack->ga_len;
    set_type_on_stack(cctx, &t_bool, 0);

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    ຈຳນວນເຕັມ
generate_CONCAT(cctx_T *cctx, ຈຳນວນເຕັມ count)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);

    ຖ້າ ((isn = generate_instr(cctx, ISN_CONCAT)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = count;

    // drop the argument types
    stack->ga_len -= count - 1;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    ຈຳນວນເຕັມ
generate_2BOOL(cctx_T *cctx, ຈຳນວນເຕັມ invert, ຈຳນວນເຕັມ offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_2BOOL)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, -1 - offset);

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    ຈຳນວນເຕັມ
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ (generate_instr(cctx, ISN_COND2BOOL) == NULL)
	ສົ່ງຄືນ FAIL;

    // type becomes bool
    set_type_on_stack(cctx, &t_bool, 0);

    ສົ່ງຄືນ OK;
}

    ຈຳນວນເຕັມ
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	ຈຳນວນເຕັມ	    typechk_flags,	// type check flags
	ຈຳນວນເຕັມ	    offset,
	ຈຳນວນເຕັມ	    is_var,
	ຈຳນວນເຕັມ	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == NULL)
	ສົ່ງຄືນ FAIL;
    type_T *tt;
    ຖ້າ ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_NUMBER_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	ຖ້າ (tt != NULL)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    ຖ້າ (typechk_flags & TYPECHK_NUMBER_OK)
		tt->tt_flags |= TTFLAG_NUMBER_OK;
	    ບໍ່ດັ່ງນັ້ນ
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    ບໍ່ດັ່ງນັ້ນ
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_stack(cctx, expected, -1 - offset);

    ສົ່ງຄືນ OK;
}

    ຈຳນວນເຕັມ
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_SETTYPE)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    ຈຳນວນເຕັມ
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == NULL)
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    static ຈຳນວນເຕັມ
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == NULL ? &t_any : &class->class_type);
    ຖ້າ (isn == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.classarg = class;
    ຖ້າ (class != NULL)
	++class->class_refcount;
    ສົ່ງຄືນ OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    ຈຳນວນເຕັມ
generate_tv_PUSH(cctx_T *cctx, typval_T *tv)
{
    switch (tv->v_type)
    {
	pak_sa VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_number);
	    bha_nga;
	pak_sa VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_number);
	    bha_nga;
	pak_sa VAR_NUMBER:
	    generate_PUSHNR(cctx, tv->vval.v_number);
	    bha_nga;
	pak_sa VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    bha_nga;
	pak_sa VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = NULL;
	    bha_nga;
	pak_sa VAR_LIST:
	    ຖ້າ (tv->vval.v_list != NULL)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    bha_nga;
	pak_sa VAR_TUPLE:
	    ຖ້າ (tv->vval.v_tuple != NULL)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    bha_nga;
	pak_sa VAR_DICT:
	    ຖ້າ (tv->vval.v_dict != NULL)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    bha_nga;
#ifdef FEAT_JOB_CHANNEL
	pak_sa VAR_JOB:
	    ຖ້າ (tv->vval.v_job != NULL)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    bha_nga;
	pak_sa VAR_CHANNEL:
	    ຖ້າ (tv->vval.v_channel != NULL)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    bha_nga;
#endif
	pak_sa VAR_FUNC:
	    ຖ້າ (tv->vval.v_string != NULL)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, NULL, &t_func_unknown, TRUE);
	    bha_nga;
	pak_sa VAR_PARTIAL:
	    ຖ້າ (tv->vval.v_partial != NULL)
		iemsg("non-null partial constant not supported");
	    ຖ້າ (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == NULL)
		ສົ່ງຄືນ FAIL;
	    bha_nga;
	pak_sa VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = NULL;
	    bha_nga;
	pak_sa VAR_OBJECT:
	    ຖ້າ (tv->vval.v_object != NULL)
	    {
		emsg(_(e_cannot_use_non_null_object));
		ສົ່ງຄືນ FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    bha_nga;
	pak_sa VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    bha_nga;
	default:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    ສົ່ງຄືນ FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    ຈຳນວນເຕັມ
generate_PUSHNR(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_number)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = number;

    ຖ້າ (number == 0 || number == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_stack(cctx, &t_number_bool, 0);
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    ຈຳນວນເຕັມ
generate_PUSHBOOL(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = number;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    ຈຳນວນເຕັມ
generate_PUSHSPEC(cctx_T *cctx, varnumber_T number)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     number == VVAL_NULL ? &t_null : &t_none)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = number;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    ຈຳນວນເຕັມ
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.fnumber = fnumber;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    ຈຳນວນເຕັມ
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    ຈຳນວນເຕັມ		ret = OK;

    ຖ້າ (cctx->ctx_skip != SKIP_YES)
    {
	ຖ້າ ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == NULL)
	    ret = FAIL;
	ບໍ່ດັ່ງນັ້ນ
	{
	    isn->isn_arg.string = str == NULL ? NULL : *str;
	    ສົ່ງຄືນ OK;
	}
    }
    ຖ້າ (str != NULL)
	VIM_CLEAR(*str);
    ສົ່ງຄືນ ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    ຈຳນວນເຕັມ
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    ຖ້າ (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == NULL)
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
#ບໍ່ດັ່ງນັ້ນ
    emsg(_(e_channel_job_feature_not_available));
    ສົ່ງຄືນ FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    ຈຳນວນເຕັມ
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    ຖ້າ (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == NULL)
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
#ບໍ່ດັ່ງນັ້ນ
    emsg(_(e_channel_job_feature_not_available));
    ສົ່ງຄືນ FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    ຈຳນວນເຕັມ
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.blob = blob;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    ຈຳນວນເຕັມ
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, ຈຳນວນເຕັມ may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (name == NULL)
	funcname = NULL;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != NULL)	    // autoload
	funcname = vim_strsave(name);
    ບໍ່ດັ່ງນັ້ນ
    {
	funcname = alloc(STRLEN(name) + 3);
	ຖ້າ (funcname != NULL)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    ຈຳນວນເຕັມ
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.string = vim_strsave(name);
    ຖ້າ (isn->isn_arg.string == NULL)
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    ຈຳນວນເຕັມ
generate_GETITEM(cctx_T *cctx, ຈຳນວນເຕັມ index, ຈຳນວນເຕັມ with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_stack(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    ຖ້າ ((isn = generate_instr(cctx, ISN_GETITEM)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    ສົ່ງຄືນ push_type_stack(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    static void
set_tuple_slice_type_on_stack(type_T *type, ຈຳນວນເຕັມ count, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    ຖ້າ ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= count)
    {
	set_type_on_stack(cctx, &t_tuple_any, 0);
	ສົ່ງຄືນ;
    }

    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);

    ສຳລັບ (ຈຳນວນເຕັມ i = count; i < type->tt_argcount; i++)
    {
	ຖ້າ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_stack(cctx, &t_tuple_any, 0);
	    ສົ່ງຄືນ;
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
    ຈຳນວນເຕັມ
generate_SLICE(cctx_T *cctx, ຈຳນວນເຕັມ count)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_SLICE)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = count;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_stack(cctx, 0);
    ຖ້າ (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_stack(type, count, cctx);

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    ຈຳນວນເຕັມ
generate_CHECKLEN(cctx_T *cctx, ຈຳນວນເຕັມ min_len, ຈຳນວນເຕັມ more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    ຖ້າ ((isn = generate_instr(cctx, ISN_CHECKLEN)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    ຈຳນວນເຕັມ
generate_STORE(cctx_T *cctx, isntype_T isn_type, ຈຳນວນເຕັມ idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    ບໍ່ດັ່ງນັ້ນ
	isn->isn_arg.number = idx;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    ຈຳນວນເຕັມ
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	ຈຳນວນເຕັມ	    load,
	class_T	    *cl,
	ຈຳນວນເຕັມ	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    ຖ້າ (isn == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    static ຈຳນວນເຕັມ
generate_STOREOUTER(
	cctx_T	    *cctx,
	ຈຳນວນເຕັມ	    idx,
	ຈຳນວນເຕັມ	    level,
	ຈຳນວນເຕັມ	    loop_depth,
	ຈຳນວນເຕັມ	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    ຈຳນວນເຕັມ
generate_STORENR(cctx_T *cctx, ຈຳນວນເຕັມ idx, varnumber_T value)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_STORENR)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = value;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    static ຈຳນວນເຕັມ
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ຈຳນວນເຕັມ	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_drop(cctx, isn_type, 1)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    ຈຳນວນເຕັມ
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ຈຳນວນເຕັມ	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_type2(cctx, isn_type, type, type)) == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (name != NULL)
	isn->isn_arg.string = vim_strsave(name);
    ບໍ່ດັ່ງນັ້ນ
	isn->isn_arg.number = idx;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    ຈຳນວນເຕັມ
generate_LOADOUTER(
	cctx_T	    *cctx,
	ຈຳນວນເຕັມ	    idx,
	ຈຳນວນເຕັມ	    nesting,
	ຈຳນວນເຕັມ	    loop_depth,
	ຈຳນວນເຕັມ	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    ຈຳນວນເຕັມ
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    ຈຳນວນເຕັມ	    di_flags;
    ຈຳນວນເຕັມ	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	ສົ່ງຄືນ FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    ສົ່ງຄືນ generate_LOAD(cctx, ISN_LOADV, vidx, NULL, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    ຈຳນວນເຕັມ
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, ຈຳນວນເຕັມ forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, isn_type)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    ຈຳນວນເຕັມ
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ (generate_instr(cctx, ISN_LOCKCONST) == NULL)
	ສົ່ງຄືນ FAIL;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    ຈຳນວນເຕັມ
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ຈຳນວນເຕັມ	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    ບໍ່ດັ່ງນັ້ນ
	isn = generate_instr_drop(cctx, isn_type, 1);
    ຖ້າ (isn == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    ຈຳນວນເຕັມ
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ຈຳນວນເຕັມ	    sid,
	ຈຳນວນເຕັມ	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    ບໍ່ດັ່ງນັ້ນ
	isn = generate_instr_drop(cctx, isn_type, 1);
    ຖ້າ (isn == NULL)
	ສົ່ງຄືນ FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    ຖ້າ (sref == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    ຈຳນວນເຕັມ
generate_NEWLIST(cctx_T *cctx, ຈຳນວນເຕັມ count, ຈຳນວນເຕັມ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_NEWLIST)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    ຖ້າ ((member_type = get_member_type_from_stack(count, 1, cctx)) == NULL)
	ສົ່ງຄືນ FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the list type to the type stack
    ສົ່ງຄືນ push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    ຈຳນວນເຕັມ
generate_NEWTUPLE(cctx_T *cctx, ຈຳນວນເຕັມ count, ຈຳນວນເຕັມ use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, parimaa_na(type_T *), 10);

    ຖ້າ (get_tuple_type_from_stack(count, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	ສົ່ງຄືນ FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_stack.ga_len -= count;

    // add the tuple type to the type stack
    ສົ່ງຄືນ push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    ຈຳນວນເຕັມ
generate_NEWDICT(cctx_T *cctx, ຈຳນວນເຕັມ count, ຈຳນວນເຕັມ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_NEWDICT)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = use_null ? -1 : count;

    ຖ້າ ((member_type = get_member_type_from_stack(count, 2, cctx)) == NULL)
	ສົ່ງຄືນ FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_stack.ga_len -= 2 * count;

    // add the dict type to the type stack
    ສົ່ງຄືນ push_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    ຈຳນວນເຕັມ
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ຈຳນວນເຕັມ	    object_method,
	ຈຳນວນເຕັມ	    fi,
	ຈຳນວນເຕັມ	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    ຈຳນວນເຕັມ		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_FUNCREF)) == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (isn_idx != NULL)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    ຖ້າ (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != NULL)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	ຖ້າ (extra == NULL)
	    ສົ່ງຄືນ FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	ຖ້າ (cl != NULL)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    ຖ້າ (ufunc->uf_def_status == UF_NOT_COMPILED || cl != NULL)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ຖ້າ (ufunc->uf_def_status != UF_NOT_COMPILED && cl == NULL)
    {
	ຖ້າ (isn_idx == NULL && ufunc->uf_def_status == UF_TO_BE_COMPILED)
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
    ຖ້າ ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == NULL ? &t_func_any : ufunc->uf_func_type;
    ສົ່ງຄືນ push_type_stack(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    ຈຳນວນເຕັມ
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    ຈຳນວນເຕັມ		ret = OK;

    ຖ້າ (cctx->ctx_skip != SKIP_YES)
    {
	ຖ້າ ((isn = generate_instr(cctx, ISN_NEWFUNC)) == NULL)
	    ret = FAIL;
	ບໍ່ດັ່ງນັ້ນ
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    ຖ້າ (arg == NULL)
		ret = FAIL;
	    ບໍ່ດັ່ງນັ້ນ
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(void)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		ສົ່ງຄືນ OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    ສົ່ງຄືນ ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    ຈຳນວນເຕັມ
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_DEF)) == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	ຖ້າ (isn->isn_arg.string == NULL)
	    ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    ຈຳນວນເຕັມ
generate_JUMP(cctx_T *cctx, jumpwhen_T when, ຈຳນວນເຕັມ where)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_JUMP)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    ຖ້າ (when != JUMP_ALWAYS && stack->ga_len > 0)
	--stack->ga_len;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    ຈຳນວນເຕັມ
generate_WHILE(cctx_T *cctx, ຈຳນວນເຕັມ funcref_idx)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_WHILE)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    ຖ້າ (stack->ga_len > 0)
	--stack->ga_len;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    ຈຳນວນເຕັມ
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, ຈຳນວນເຕັມ arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, isn_type)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    ສົ່ງຄືນ OK;
}

    ຈຳນວນເຕັມ
generate_FOR(cctx_T *cctx, ຈຳນວນເຕັມ loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_FOR)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    ສົ່ງຄືນ push_type_stack(cctx, &t_any);
}

    ຈຳນວນເຕັມ
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_ENDLOOP)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_count;
    isn->isn_arg.endloop.end_var_count =
			   cctx->ctx_locals.ga_len - loop_info->li_local_count;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    ຈຳນວນເຕັມ
generate_TRYCONT(cctx_T *cctx, ຈຳນວນເຕັມ levels, ຈຳນວນເຕັມ where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_TRYCONT)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    ສົ່ງຄືນ OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    ຈຳນວນເຕັມ
check_internal_func_args(
	cctx_T	*cctx,
	ຈຳນວນເຕັມ	func_idx,
	ຈຳນວນເຕັມ	argcount,
	ຈຳນວນເຕັມ	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*stack = &cctx->ctx_type_stack;
    ຈຳນວນເຕັມ		argoff = check_internal_func(func_idx, argcount);

    ຖ້າ (argoff < 0)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (method_call && argoff > 1)
    {
	ຖ້າ (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    ສົ່ງຄືນ FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	ຖ້າ (isn  == NULL)
	    ສົ່ງຄືນ FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    ຖ້າ (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)stack->ga_data) + stack->ga_len - argcount;

	// Check the types of the arguments.
	ຖ້າ (method_call && argoff > 1)
	{
	    ຈຳນວນເຕັມ i;

	    ສຳລັບ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    ຈຳນວນເຕັມ i;

	    ສຳລັບ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ຖ້າ (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    ຈຳນວນເຕັມ
generate_BCALL(cctx_T *cctx, ຈຳນວນເຕັມ func_idx, ຈຳນວນເຕັມ argcount, ຈຳນວນເຕັມ method_call)
{
    isn_T	*isn;
    garray_T	*stack = &cctx->ctx_type_stack;
    type2_T	*argtypes = NULL;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = NULL;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    ຖ້າ (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (internal_func_is_map(func_idx))
	maptype = argtypes;

    ຖ້າ ((isn = generate_instr(cctx, ISN_BCALL)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    stack->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    ຖ້າ (push_type_stack2(cctx, type, decl_type) == FAIL)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (maptype != NULL && maptype[0].type_decl->tt_member != NULL
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ຈຳນວນເຕັມ
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_stack(cctx, 1);
    ຖ້າ (arg_type_modifiable(list_type, 1) == FAIL)
	ສົ່ງຄືນ FAIL;
    item_type = get_type_on_stack(cctx, 0);
    expected = list_type->tt_member;
    ຖ້າ (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (generate_instr(cctx, ISN_LISTAPPEND) == NULL)
	ສົ່ງຄືນ FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ຈຳນວນເຕັມ
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    ຖ້າ (arg_type_modifiable(get_decl_type_on_stack(cctx, 1), 1) == FAIL)
	ສົ່ງຄືນ FAIL;
    item_type = get_type_on_stack(cctx, 0);
    ຖ້າ (need_type(item_type, &t_number, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ສົ່ງຄືນ FAIL;

    ຖ້າ (generate_instr(cctx, ISN_BLOBAPPEND) == NULL)
	ສົ່ງຄືນ FAIL;

    --cctx->ctx_type_stack.ga_len;	    // drop the argument
    ສົ່ງຄືນ OK;
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
    ຖ້າ (cl != NULL)
	ສົ່ງຄືນ ISN_METHODCALL;

    ຖ້າ (ufunc->uf_def_status == UF_NOT_COMPILED)
	ສົ່ງຄືນ ISN_UCALL;

    // function invoked from a lambda
    ຖ້າ (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	ຖ້າ (ufunc->uf_name[0] == K_SPECIAL)
	    ສົ່ງຄືນ ISN_UCALL;
    }

    ສົ່ງຄືນ ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    ຈຳນວນເຕັມ
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ຈຳນວນເຕັມ	    mi,
	ຈຳນວນເຕັມ	    pushed_argcount,
	ຈຳນວນເຕັມ	    is_super)
{
    isn_T	*isn;
    ຈຳນວນເຕັມ		regular_args = ufunc->uf_args.ga_len;
    ຈຳນວນເຕັມ		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	ສົ່ງຄືນ FAIL;
    }
    ຖ້າ (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	ສົ່ງຄືນ FAIL;
    }

    ຖ້າ (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	ຈຳນວນເຕັມ		i;
	compiletype_T	compile_type;

	ສຳລັບ (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_stack(cctx, argcount - i - 1);
	    ຖ້າ (check_type_is_value(actual) == FAIL)
		ສົ່ງຄືນ FAIL;
	    ຖ້າ (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		continue;
	    }
	    ຖ້າ (i < regular_args)
	    {
		ຖ້າ (ufunc->uf_arg_types == NULL)
		    continue;
		expected = ufunc->uf_arg_types[i];
	    }
	    ບໍ່ດັ່ງນັ້ນ ຖ້າ (ufunc->uf_va_type == NULL
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    ບໍ່ດັ່ງນັ້ນ
		expected = ufunc->uf_va_type->tt_member;
	    ຖ້າ (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		ສົ່ງຄືນ FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	ຖ້າ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == NULL,
						   compile_type, NULL) == FAIL)
	    ສົ່ງຄືນ FAIL;
    }
    ຖ້າ (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	ສົ່ງຄືນ FAIL;
    }

    ຖ້າ ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == NULL)
	ສົ່ງຄືນ FAIL;
    ຖ້າ (cl != NULL /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	ຖ້າ (isn->isn_arg.mfunc == NULL)
	    ສົ່ງຄືນ FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = NULL;
	ຖ້າ (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = NULL;
	isn->isn_arg.mfunc->cmf_cache_generic = NULL;
	isn->isn_arg.mfunc->cmf_cache_ufunc = NULL;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    ຖ້າ (ufunc->uf_class != NULL)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_stack(cctx, 0);
	ຖ້າ (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_stack.ga_len--;
    }

    // add return type
    ສົ່ງຄືນ push_type_stack(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    ຈຳນວນເຕັມ
generate_UCALL(cctx_T *cctx, char_u *name, ຈຳນວນເຕັມ argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_UCALL)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_stack.ga_len -= argcount;

    // add return value
    ສົ່ງຄືນ push_type_stack(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    ຈຳນວນເຕັມ
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	ຈຳນວນເຕັມ	argcount,
	ຈຳນວນເຕັມ	at_top,
	char_u	*name)
{
    ຖ້າ (type->tt_argcount != -1)
    {
	ຈຳນວນເຕັມ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	ຖ້າ (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    ສົ່ງຄືນ FAIL;
	}
	ຖ້າ (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    ສົ່ງຄືນ FAIL;
	}
	ຖ້າ (type->tt_args != NULL)
	{
	    ຈຳນວນເຕັມ i;

	    ສຳລັບ (i = 0; i < argcount; ++i)
	    {
		ຈຳນວນເຕັມ	offset = -argcount + i - (at_top ? 0 : 1);
		type_T	*actual = get_type_on_stack(cctx, -1 - offset);
		type_T	*expected;

		ຖ້າ (check_type_is_value(actual) == FAIL)
		    ສົ່ງຄືນ FAIL;
		ຖ້າ (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    ຖ້າ (expected != NULL && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    ຖ້າ (expected == NULL)
			expected = &t_any;
		}
		ບໍ່ດັ່ງນັ້ນ ຖ້າ (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		ບໍ່ດັ່ງນັ້ນ
		    expected = type->tt_args[i];
		ຖ້າ (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    ສົ່ງຄືນ FAIL;
		}
	    }
	}
    }

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    ຈຳນວນເຕັມ
generate_PCALL(
	cctx_T	*cctx,
	ຈຳນວນເຕັມ	argcount,
	char_u	*name,
	type_T	*type,
	ຈຳນວນເຕັມ	at_top)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    ຖ້າ (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	ຖ້າ (check_func_args_from_type(cctx, type, argcount, at_top, name)
								       == FAIL)
	    ສົ່ງຄືນ FAIL;

	ret_type = type->tt_member;
	ຖ້າ (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    ບໍ່ດັ່ງນັ້ນ
    {
	semsg(_(e_not_callable_type_str), name);
	ສົ່ງຄືນ FAIL;
    }

    ຖ້າ ((isn = generate_instr(cctx, ISN_PCALL)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.pfunc.cpf_top = at_top;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_stack.ga_len -= argcount + 1;

    // push the return value
    push_type_stack(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    ຖ້າ (at_top && generate_instr(cctx, ISN_PCALL_END) == NULL)
	ສົ່ງຄືນ FAIL;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    ຈຳນວນເຕັມ
generate_DEFER(cctx_T *cctx, ຈຳນວນເຕັມ var_idx, ຈຳນວນເຕັມ argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    ຈຳນວນເຕັມ
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_stack(cctx, 0);
    ຖ້າ (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	var_na *tofree;

	semsg(_(e_expected_dictionary_for_using_key_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	ສົ່ງຄືນ FAIL;
    }
    // change dict type to dict member type
    ຖ້າ (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_stack(cctx, ntype, 0);
    }

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    ຈຳນວນເຕັມ
generate_ECHO(cctx_T *cctx, ຈຳນວນເຕັມ with_white, ຈຳນວນເຕັມ count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_drop(cctx, ISN_ECHO, count)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_count = count;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    ຈຳນວນເຕັມ
generate_MULT_EXPR(cctx_T *cctx, isntype_T isn_type, ຈຳນວນເຕັມ count)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr_drop(cctx, isn_type, count)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = count;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    ຈຳນວນເຕັມ
generate_ECHOWINDOW(cctx_T *cctx, ຈຳນວນເຕັມ count, long time)
{
    isn_T	*isn;

    ຖ້າ ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, count)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.echowin.ewin_count = count;
    isn->isn_arg.echowin.ewin_time = time;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    ຈຳນວນເຕັມ
generate_SOURCE(cctx_T *cctx, ຈຳນວນເຕັມ sid)
{
    isn_T	*isn;

    ຖ້າ ((isn = generate_instr(cctx, ISN_SOURCE)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = sid;

    ສົ່ງຄືນ OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    ຈຳນວນເຕັມ
generate_PUT(cctx_T *cctx, ຈຳນວນເຕັມ regname, linenr_T lnum, ຈຳນວນເຕັມ fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    ຖ້າ (isn == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    ສົ່ງຄືນ OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    ຈຳນວນເຕັມ
generate_LOCKUNLOCK(cctx_T *cctx, char_u *line, ຈຳນວນເຕັມ is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == NULL)
	ສົ່ງຄືນ FAIL;
    class_T *cl = cctx->ctx_ufunc != NULL ? cctx->ctx_ufunc->uf_class : NULL;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(line);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    ຖ້າ (cl != NULL)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    ຈຳນວນເຕັມ
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, isntype)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.string = vim_strsave(line);
    ສົ່ງຄືນ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    ຈຳນວນເຕັມ
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    ຈຳນວນເຕັມ		ret = OK;

    ຖ້າ (cctx->ctx_skip != SKIP_YES)
    {
	ຖ້າ ((isn = generate_instr(cctx, isntype)) == NULL)
	    ret = FAIL;
	ບໍ່ດັ່ງນັ້ນ
	{
	    isn->isn_arg.string = str;
	    ສົ່ງຄືນ OK;
	}
    }
    vim_free(str);
    ສົ່ງຄືນ ret;
}

    ຈຳນວນເຕັມ
generate_LEGACY_EVAL(cctx_T *cctx, char_u *line)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.string = vim_strsave(line);

    ສົ່ງຄືນ push_type_stack(cctx, &t_any);
}

    ຈຳນວນເຕັມ
generate_EXECCONCAT(cctx_T *cctx, ຈຳນວນເຕັມ count)
{
    isn_T	*isn;

    ຖ້າ ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, count)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.number = count;
    ສົ່ງຄືນ OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    ຈຳນວນເຕັມ
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    ຖ້າ ((isn = generate_instr(cctx, ISN_RANGE)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.string = range;

    ສົ່ງຄືນ push_type_stack(cctx, &t_number);
}

    ຈຳນວນເຕັມ
generate_UNPACK(cctx_T *cctx, ຈຳນວນເຕັມ var_count, ຈຳນວນເຕັມ semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_UNPACK)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.unpack.unp_count = var_count;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    ສົ່ງຄືນ OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    ຈຳນວນເຕັມ
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    ຖ້າ (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	ຖ້າ ((isn = generate_instr(cctx, ISN_CMDMOD)) == NULL)
	    ສົ່ງຄືນ FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	ຖ້າ (isn->isn_arg.cmdmod.cf_cmdmod == NULL)
	    ສົ່ງຄືນ FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, parimaa_na(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = NULL;
    }

    ສົ່ງຄືນ OK;
}

    ຈຳນວນເຕັມ
generate_undo_cmdmods(cctx_T *cctx)
{
    ຖ້າ (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == NULL)
	ສົ່ງຄືນ FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    ສົ່ງຄືນ OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    ຈຳນວນເຕັມ
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	ຈຳນວນເຕັມ		opt_flags,
	ຈຳນວນເຕັມ		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    switch (dest)
    {
	pak_sa dest_option:
	    ສົ່ງຄືນ generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	pak_sa dest_func_option:
	    ສົ່ງຄືນ generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	pak_sa dest_global:
	    // include g: with the name, easier to execute that way
	    ສົ່ງຄືນ generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == NULL
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	pak_sa dest_buffer:
	    // include b: with the name, easier to execute that way
	    ສົ່ງຄືນ generate_STORE(cctx, ISN_STOREB, 0, name);
	pak_sa dest_window:
	    // include w: with the name, easier to execute that way
	    ສົ່ງຄືນ generate_STORE(cctx, ISN_STOREW, 0, name);
	pak_sa dest_tab:
	    // include t: with the name, easier to execute that way
	    ສົ່ງຄືນ generate_STORE(cctx, ISN_STORET, 0, name);
	pak_sa dest_env:
	    ສົ່ງຄືນ generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	pak_sa dest_reg:
	    ສົ່ງຄືນ generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], NULL);
	pak_sa dest_vimvar:
	    ສົ່ງຄືນ generate_STORE(cctx, ISN_STOREV, vimvaridx, NULL);
	pak_sa dest_script:
	pak_sa dest_script_v9:
	    {
		ຈຳນວນເຕັມ	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		ຈຳນວນເຕັມ	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		ຖ້າ (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    ຖ້າ (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == NULL)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			ຖ້າ (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    ສົ່ງຄືນ FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    ສົ່ງຄືນ generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		ສົ່ງຄືນ generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	pak_sa dest_class_member:
	    ສົ່ງຄືນ generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	pak_sa dest_local:
	pak_sa dest_expr:
	    // cannot happen
	    bha_nga;
    }
    ສົ່ງຄືນ FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    ຈຳນວນເຕັມ
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    ສຳລັບ (;;)
    {
	ຖ້າ (scope == NULL)
	    bha_nga;
	ຖ້າ (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    ສົ່ງຄືນ TRUE;
	scope = scope->se_outer;
    }
    ສົ່ງຄືນ FALSE;
}

    ຈຳນວນເຕັມ
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, ຈຳນວນເຕັມ instr_count, ຈຳນວນເຕັມ is_decl)
{
    ຖ້າ (lhs->lhs_dest != dest_local)
	ສົ່ງຄືນ generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    ຖ້າ (lhs->lhs_lvar == NULL)
	ສົ່ງຄືນ OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    ຖ້າ (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_count + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.number;
	garray_T    *stack = &cctx->ctx_type_stack;

	ຖ້າ (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	ບໍ່ດັ່ງນັ້ນ
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	ຖ້າ (stack->ga_len > 0)
	    --stack->ga_len;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    ບໍ່ດັ່ງນັ້ນ
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, NULL);
    ສົ່ງຄືນ OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    ຈຳນວນເຕັມ
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ຖ້າ ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == NULL)
	ສົ່ງຄືນ FAIL;
    isn->isn_arg.setsctx = new_sctx;
    ສົ່ງຄືນ OK;
}

#ຖ້າ defined(FEAT_PROFILE)
    void
may_generate_prof_end(cctx_T *cctx, ຈຳນວນເຕັມ prof_lnum)
{
    ຖ້າ (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
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
	pak_sa ISN_AUTOLOAD:
	pak_sa ISN_DEF:
	pak_sa ISN_EXEC:
	pak_sa ISN_EXECRANGE:
	pak_sa ISN_EXEC_SPLIT:
	pak_sa ISN_LEGACY_EVAL:
	pak_sa ISN_LOADAUTO:
	pak_sa ISN_LOADB:
	pak_sa ISN_LOADENV:
	pak_sa ISN_LOADG:
	pak_sa ISN_LOADOPT:
	pak_sa ISN_LOADT:
	pak_sa ISN_LOADW:
	pak_sa ISN_PUSHEXC:
	pak_sa ISN_PUSHFUNC:
	pak_sa ISN_PUSHS:
	pak_sa ISN_RANGE:
	pak_sa ISN_STOREAUTO:
	pak_sa ISN_STOREB:
	pak_sa ISN_STOREENV:
	pak_sa ISN_STOREG:
	pak_sa ISN_STORET:
	pak_sa ISN_STOREW:
	pak_sa ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    bha_nga;

	pak_sa ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    bha_nga;

	pak_sa ISN_SUBSTITUTE:
	    {
		ຈຳນວນເຕັມ	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		ສຳລັບ (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    bha_nga;

	pak_sa ISN_INSTR:
	    {
		ຈຳນວນເຕັມ	idx;
		isn_T	*list = isn->isn_arg.instr;

		ສຳລັບ (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    bha_nga;

	pak_sa ISN_LOADS:
	pak_sa ISN_LOADEXPORT:
	pak_sa ISN_STORES:
	pak_sa ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    bha_nga;

	pak_sa ISN_UNLET:
	pak_sa ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    bha_nga;

	pak_sa ISN_STOREOPT:
	pak_sa ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    bha_nga;

	pak_sa ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    bha_nga;

	pak_sa ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    bha_nga;

	pak_sa ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    bha_nga;

	pak_sa ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		ຖ້າ (extra == NULL || extra->fre_func_name == NULL)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    ຖ້າ (ufunc != NULL && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		ຖ້າ (extra != NULL)
		{
		    char_u *name = extra->fre_func_name;

		    ຖ້າ (name != NULL)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    ຖ້າ (extra->fre_class != NULL)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    bha_nga;

	pak_sa ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		ຖ້າ (dfunc->df_ufunc != NULL
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    bha_nga;

	pak_sa ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    bha_nga;

	pak_sa ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		ຖ້າ (arg != NULL)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    ຖ້າ (ufunc != NULL)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    bha_nga;

	pak_sa ISN_CHECKTYPE:
	pak_sa ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    bha_nga;

	pak_sa ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    bha_nga;

	pak_sa ISN_LOADSCRIPT:
	pak_sa ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    bha_nga;

	pak_sa ISN_LOAD_CLASSMEMBER:
	pak_sa ISN_STORE_CLASSMEMBER:
	pak_sa ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    bha_nga;

	pak_sa ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    bha_nga;

	pak_sa ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    bha_nga;

	pak_sa ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    bha_nga;

	pak_sa ISN_2BOOL:
	pak_sa ISN_2STRING:
	pak_sa ISN_2STRING_ANY:
	pak_sa ISN_ADDBLOB:
	pak_sa ISN_ADDLIST:
	pak_sa ISN_ADDTUPLE:
	pak_sa ISN_ANYINDEX:
	pak_sa ISN_ANYSLICE:
	pak_sa ISN_BCALL:
	pak_sa ISN_BLOBAPPEND:
	pak_sa ISN_BLOBINDEX:
	pak_sa ISN_BLOBSLICE:
	pak_sa ISN_CATCH:
	pak_sa ISN_CEXPR_AUCMD:
	pak_sa ISN_CHECKLEN:
	pak_sa ISN_CLEARDICT:
	pak_sa ISN_CMDMOD_REV:
	pak_sa ISN_COMPAREANY:
	pak_sa ISN_COMPAREBLOB:
	pak_sa ISN_COMPAREBOOL:
	pak_sa ISN_COMPAREDICT:
	pak_sa ISN_COMPAREFLOAT:
	pak_sa ISN_COMPAREFUNC:
	pak_sa ISN_COMPARELIST:
	pak_sa ISN_COMPARETUPLE:
	pak_sa ISN_COMPARENR:
	pak_sa ISN_COMPARENULL:
	pak_sa ISN_COMPAREOBJECT:
	pak_sa ISN_COMPARESPECIAL:
	pak_sa ISN_COMPARESTRING:
	pak_sa ISN_CONCAT:
	pak_sa ISN_CONSTRUCT:
	pak_sa ISN_COND2BOOL:
	pak_sa ISN_DEBUG:
	pak_sa ISN_DEFER:
	pak_sa ISN_DROP:
	pak_sa ISN_ECHO:
	pak_sa ISN_ECHOCONSOLE:
	pak_sa ISN_ECHOERR:
	pak_sa ISN_ECHOMSG:
	pak_sa ISN_ECHOWINDOW:
	pak_sa ISN_ENDLOOP:
	pak_sa ISN_ENDTRY:
	pak_sa ISN_EXECCONCAT:
	pak_sa ISN_EXECUTE:
	pak_sa ISN_FINALLY:
	pak_sa ISN_FINISH:
	pak_sa ISN_FOR:
	pak_sa ISN_GETITEM:
	pak_sa ISN_GET_OBJ_MEMBER:
	pak_sa ISN_JUMP:
	pak_sa ISN_JUMP_IF_ARG_NOT_SET:
	pak_sa ISN_JUMP_IF_ARG_SET:
	pak_sa ISN_LISTAPPEND:
	pak_sa ISN_LISTINDEX:
	pak_sa ISN_LISTSLICE:
	pak_sa ISN_TUPLEINDEX:
	pak_sa ISN_TUPLESLICE:
	pak_sa ISN_LOAD:
	pak_sa ISN_LOADBDICT:
	pak_sa ISN_LOADGDICT:
	pak_sa ISN_LOADOUTER:
	pak_sa ISN_LOADREG:
	pak_sa ISN_LOADTDICT:
	pak_sa ISN_LOADV:
	pak_sa ISN_LOADWDICT:
	pak_sa ISN_LOCKCONST:
	pak_sa ISN_MEMBER:
	pak_sa ISN_NEGATENR:
	pak_sa ISN_NEWDICT:
	pak_sa ISN_NEWLIST:
	pak_sa ISN_NEWTUPLE:
	pak_sa ISN_NEWPARTIAL:
	pak_sa ISN_OPANY:
	pak_sa ISN_OPFLOAT:
	pak_sa ISN_OPNR:
	pak_sa ISN_PCALL:
	pak_sa ISN_PCALL_END:
	pak_sa ISN_PROF_END:
	pak_sa ISN_PROF_START:
	pak_sa ISN_PUSHBOOL:
	pak_sa ISN_PUSHCHANNEL:
	pak_sa ISN_PUSHF:
	pak_sa ISN_PUSHJOB:
	pak_sa ISN_PUSHNR:
	pak_sa ISN_PUSHOBJ:
	pak_sa ISN_PUSHSPEC:
	pak_sa ISN_PUT:
	pak_sa ISN_IPUT:
	pak_sa ISN_REDIREND:
	pak_sa ISN_REDIRSTART:
	pak_sa ISN_RETURN:
	pak_sa ISN_RETURN_OBJECT:
	pak_sa ISN_RETURN_VOID:
	pak_sa ISN_SHUFFLE:
	pak_sa ISN_SLICE:
	pak_sa ISN_SOURCE:
	pak_sa ISN_STORE:
	pak_sa ISN_STORENR:
	pak_sa ISN_STOREOUTER:
	pak_sa ISN_STORE_THIS:
	pak_sa ISN_STORERANGE:
	pak_sa ISN_STOREREG:
	pak_sa ISN_STOREV:
	pak_sa ISN_STRINDEX:
	pak_sa ISN_STRSLICE:
	pak_sa ISN_THROW:
	pak_sa ISN_TRYCONT:
	pak_sa ISN_UNLETINDEX:
	pak_sa ISN_UNLETRANGE:
	pak_sa ISN_UNPACK:
	pak_sa ISN_USEDICT:
	pak_sa ISN_WHILE:
	pak_sa ISN_SCRIPTCTX_SET:
	// nothing allocated
	bha_nga;
    }
}

    void
clear_instr_ga(garray_T *gap)
{
    ຈຳນວນເຕັມ idx;

    ສຳລັບ (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
