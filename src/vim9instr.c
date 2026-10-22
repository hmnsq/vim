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

#నిర్వచించు USING_FLOAT_STUFF
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#నిర్వచించు RETURN_NULL_IF_SKIP(cctx) ఒకవేళ (cctx->ctx_skip == SKIP_YES) తిరిగి ఖాళీ
#నిర్వచించు RETURN_OK_IF_SKIP(cctx) ఒకవేళ (cctx->ctx_skip == SKIP_YES) తిరిగి OK

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
    ఒకవేళ (GA_GROW_FAILS(instr, 1))
	తిరిగి ఖాళీ;
    isn = ((isn_T *)instr->ga_దత్తాంశం) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    తిరిగి isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, పూర్ణాంకం drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_దొంతర.ga_len -= drop;
    తిరిగి generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    స్థిర isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    ఒకవేళ ((isn = generate_instr(cctx, isn_type)) == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (నెట్టు_type_stack2(cctx, type == ఖాళీ ? &t_any : type,
			       decl_type == ఖాళీ ? &t_any : decl_type) == FAIL)
	తిరిగి ఖాళీ;

    తిరిగి isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    తిరిగి generate_instr_type2(cctx, isn_type, type, &t_any);
}

/*
 * Generate an ISN_DEBUG instruction.
 */
    isn_T *
generate_instr_debug(cctx_T *cctx)
{
    isn_T	*isn;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
					       + cctx->ctx_ufunc->uf_dfunc_idx;

    ఒకవేళ ((isn = generate_instr(cctx, ISN_DEBUG)) == ఖాళీ)
	తిరిగి ఖాళీ;
    isn->isn_arg.debug.dbg_var_పేర్లు_len = dfunc->df_var_పేర్లు.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    తిరిగి isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    పూర్ణాంకం
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.construct.construct_కొలత = పరిమాణం(object_T)
			       + cl->class_obj_member_లెక్క * పరిమాణం(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    తిరిగి OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    పూర్ణాంకం
generate_GET_OBJ_MEMBER(cctx_T *cctx, పూర్ణాంకం idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి FAIL;

    isn->isn_arg.classmember.cm_class = ఖాళీ;
    isn->isn_arg.classmember.cm_idx = idx;
    తిరిగి నెట్టు_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    పూర్ణాంకం
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, పూర్ణాంకం idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    తిరిగి నెట్టు_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    పూర్ణాంకం
generate_STORE_THIS(cctx_T *cctx, పూర్ణాంకం idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి FAIL;

    isn->isn_arg.సంఖ్య = idx;
    తిరిగి OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    పూర్ణాంకం
may_generate_2STRING(పూర్ణాంకం స్థానభ్రంశం, పూర్ణాంకం tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_దొంతర(cctx, -1 - స్థానభ్రంశం);

    ఎంపిక (type->tt_type)
    {
	// nothing to be done
	స్థితి VAR_STRING: తిరిగి OK;

	// conversion possible
	స్థితి VAR_SPECIAL:
	స్థితి VAR_BOOL:
	స్థితి VAR_సంఖ్య:
	స్థితి VAR_FLOAT:
			 ఆపు;

	// conversion possible (with runtime check)
	స్థితి VAR_ANY:
	స్థితి VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 ఆపు;

	// conversion possible when tolerant
	స్థితి VAR_LIST:
	స్థితి VAR_TUPLE:
	స్థితి VAR_DICT:
			 ఒకవేళ (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     ఆపు;
			 }
			 ఒకవేళ (tostring_flags & TOSTRING_INTERPOLATE)
			     ఆపు;
			 // FALLTHROUGH

	// conversion not possible
	స్థితి VAR_VOID:
	స్థితి VAR_BLOB:
	స్థితి VAR_FUNC:
	స్థితి VAR_PARTIAL:
	స్థితి VAR_పని:
	స్థితి VAR_CHANNEL:
	స్థితి VAR_INSTR:
	స్థితి VAR_CLASS:
	స్థితి VAR_OBJECT:
	స్థితి VAR_TYPEALIAS:
			 గమ్యం_string_error(type->tt_type);
			 తిరిగి FAIL;
    }

    సమితి_type_on_దొంతర(cctx, &t_string, -1 - స్థానభ్రంశం);
    ఒకవేళ ((isn = generate_instr(cctx, isntype)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.tostring.స్థానభ్రంశం = స్థానభ్రంశం;
    isn->isn_arg.tostring.flags = tostring_flags;

    తిరిగి OK;
}

    స్థిర పూర్ణాంకం
check_సంఖ్య_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    ఒకవేళ (!((type1 == VAR_సంఖ్య || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_సంఖ్య || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	ఒకవేళ (check_type_is_విలువ(typ1) == FAIL
		|| check_type_is_విలువ(typ2) == FAIL)
	    తిరిగి FAIL;
	ఒకవేళ (*op == '+')
	    emsg(_(e_wrong_వాదం_type_for_plus));
	లేకపోతే
	    semsg(_(e_char_requires_సంఖ్య_or_float_arguments), *op);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    స్థిర పూర్ణాంకం
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    కొరకు (పూర్ణాంకం i = 0; i < tuple_type->tt_argcount; i++)
    {
	ఒకవేళ (ga_grow(gap, 1) == FAIL)
	    తిరిగి FAIL;

	((type_T **)gap->ga_దత్తాంశం)[gap->ga_len] = tuple_type->tt_వాదనలు[i];
	gap->ga_len++;
    }

    తిరిగి OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    స్థిర పూర్ణాంకం
సమితి_tuple_union_type_on_దొంతర(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, పరిమాణం(type_T *), 10);

    ఒకవేళ (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    ఒకవేళ (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    ఒకవేళ ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    సమితి_type_on_దొంతర(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    తిరిగి OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    పూర్ణాంకం
generate_కూడు_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  సమాసం_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_సంఖ్య ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    ఒకవేళ (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_సంఖ్య_or_float(type1, type2, (char_u *)"+") == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (isn != ఖాళీ)
    {
	ఒకవేళ (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = సమాసం_type;
	లేకపోతే
	    isn->isn_arg.op.op_type = సమాసం_కూడు;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    ఒకవేళ (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	సమితి_type_on_దొంతర(cctx, &t_list_any, 0);
    లేకపోతే ఒకవేళ (vartype == VAR_TUPLE)
    {
	ఒకవేళ (!check_tuples_addable(type1, type2))
	    తిరిగి FAIL;

	ఒకవేళ (సమితి_tuple_union_type_on_దొంతర(type1, type2, cctx) == FAIL)
	    తిరిగి FAIL;
    }

    తిరిగి isn == ఖాళీ ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    ఒకవేళ (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_సంఖ్య
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	తిరిగి type1->tt_type;
    తిరిగి VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    పూర్ణాంకం
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_దొంతర(cctx, 1);
    type2 = get_type_on_దొంతర(cctx, 0);
    vartype = operator_type(type1, type2);

    ఎంపిక (*op)
    {
	స్థితి '+':
		  ఒకవేళ (generate_కూడు_instr(cctx, vartype, type1, type2,
							    సమాసం_COPY) == FAIL)
		      తిరిగి FAIL;
		  ఆపు;

	స్థితి '-':
	స్థితి '*':
	స్థితి '/': ఒకవేళ (check_సంఖ్య_or_float(type1, type2, op) == FAIL)
		      తిరిగి FAIL;
		  ఒకవేళ (vartype == VAR_సంఖ్య)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  లేకపోతే ఒకవేళ (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  లేకపోతే
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  ఒకవేళ (isn != ఖాళీ)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? సమాసం_MULT : *op == '/'? సమాసం_DIV : సమాసం_SUB;
		  ఆపు;

	స్థితి '%': ఒకవేళ ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_సంఖ్య)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_సంఖ్య))
		  {
		      emsg(_(e_percent_requires_సంఖ్య_arguments));
		      తిరిగి FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_సంఖ్య ? ISN_OPNR : ISN_OPANY, 1);
		  ఒకవేళ (isn != ఖాళీ)
		      isn->isn_arg.op.op_type = సమాసం_REM;
		  ఆపు;
    }

    // correct type of result
    ఒకవేళ (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	ఒకవేళ ((type1->tt_type == VAR_సంఖ్య || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_సంఖ్య || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	సమితి_type_on_దొంతర(cctx, type, 0);
    }

    తిరిగి OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    స్థిర isntype_T
compare_isn_not_విలువలు(typval_T *tv, type_T *type)
{
    ఒకవేళ (tv != ఖాళీ)
	(శూన్య)check_typval_is_విలువ(tv);
    లేకపోతే
	(శూన్య)check_type_is_విలువ(type);
    తిరిగి ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    స్థిర isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != ఖాళీ ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != ఖాళీ ? tv2->v_type : type2->tt_type;

    ఒకవేళ (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	తిరిగి compare_isn_not_విలువలు(tv1, type1);
    ఒకవేళ (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	తిరిగి compare_isn_not_విలువలు(tv2, type2);

    ఒకవేళ (vartype1 == vartype2)
    {
	ఎంపిక (vartype1)
	{
	    స్థితి VAR_BOOL: isntype = ISN_COMPAREBOOL; ఆపు;
	    స్థితి VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; ఆపు;
	    స్థితి VAR_సంఖ్య: isntype = ISN_COMPARENR; ఆపు;
	    స్థితి VAR_FLOAT: isntype = ISN_COMPAREFLOAT; ఆపు;
	    స్థితి VAR_STRING: isntype = ISN_COMPARESTRING; ఆపు;
	    స్థితి VAR_BLOB: isntype = ISN_COMPAREBLOB; ఆపు;
	    స్థితి VAR_LIST: isntype = ISN_COMPARELIST; ఆపు;
	    స్థితి VAR_TUPLE: isntype = ISN_COMPARETUPLE; ఆపు;
	    స్థితి VAR_DICT: isntype = ISN_COMPAREDICT; ఆపు;
	    స్థితి VAR_FUNC: isntype = ISN_COMPAREFUNC; ఆపు;
	    స్థితి VAR_OBJECT: isntype = ISN_COMPAREOBJECT; ఆపు;
	    అప్రమేయ: isntype = ISN_COMPAREANY; ఆపు;
	}
    }
    లేకపోతే ఒకవేళ (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_సంఖ్య || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_సంఖ్య || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    లేకపోతే ఒకవేళ (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	ఒకవేళ ((vartype1 == VAR_SPECIAL
		&& (tv1 != ఖాళీ ? tv1->vval.v_సంఖ్య == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != ఖాళీ ? tv2->vval.v_సంఖ్య == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    తిరిగి ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    ఒకవేళ ((exprtype == సమాసం_IS || exprtype == సమాసం_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == సమాసం_IS ? "is" : "isnot" , vartype_name(vartype1));
	తిరిగి ISN_DROP;
    }
    ఒకవేళ (!(exprtype == సమాసం_IS || exprtype == సమాసం_ISNOT
		|| exprtype == సమాసం_EQUAL || exprtype == సమాసం_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	తిరిగి ISN_DROP;
    }
    ఒకవేళ (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != సమాసం_EQUAL
			&& exprtype != సమాసం_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != సమాసం_EQUAL
			 && exprtype != సమాసం_NEQUAL
			 && exprtype != సమాసం_IS
			 && exprtype != సమాసం_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	తిరిగి ISN_DROP;
    }
    తిరిగి isntype;
}

    పూర్ణాంకం
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    ఒకవేళ (get_compare_isn(type, tv1, tv2, ఖాళీ, ఖాళీ) == ISN_DROP)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    పూర్ణాంకం
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, పూర్ణాంకం ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, ఖాళీ, ఖాళీ,
		       get_type_on_దొంతర(cctx, 1), get_type_on_దొంతర(cctx, 0));
    ఒకవేళ (isntype == ISN_DROP)
	తిరిగి FAIL;

    ఒకవేళ ((isn = generate_instr(cctx, isntype)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --దొంతర->ga_len;
    సమితి_type_on_దొంతర(cctx, &t_bool, 0);

    తిరిగి OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    పూర్ణాంకం
generate_CONCAT(cctx_T *cctx, పూర్ణాంకం లెక్క)
{
    isn_T	*isn;
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;

    RETURN_OK_IF_SKIP(cctx);

    ఒకవేళ ((isn = generate_instr(cctx, ISN_CONCAT)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = లెక్క;

    // drop the argument types
    దొంతర->ga_len -= లెక్క - 1;

    తిరిగి OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    పూర్ణాంకం
generate_2BOOL(cctx_T *cctx, పూర్ణాంకం invert, పూర్ణాంకం స్థానభ్రంశం)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_2BOOL)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.స్థానభ్రంశం = స్థానభ్రంశం;

    // type becomes bool
    సమితి_type_on_దొంతర(cctx, &t_bool, -1 - స్థానభ్రంశం);

    తిరిగి OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    పూర్ణాంకం
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ (generate_instr(cctx, ISN_COND2BOOL) == ఖాళీ)
	తిరిగి FAIL;

    // type becomes bool
    సమితి_type_on_దొంతర(cctx, &t_bool, 0);

    తిరిగి OK;
}

    పూర్ణాంకం
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	పూర్ణాంకం	    typechk_flags,	// type check flags
	పూర్ణాంకం	    స్థానభ్రంశం,
	పూర్ణాంకం	    is_var,
	పూర్ణాంకం	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == ఖాళీ)
	తిరిగి FAIL;
    type_T *tt;
    ఒకవేళ ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_సంఖ్య_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	ఒకవేళ (tt != ఖాళీ)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    ఒకవేళ (typechk_flags & TYPECHK_సంఖ్య_OK)
		tt->tt_flags |= TTFLAG_సంఖ్య_OK;
	    లేకపోతే
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    లేకపోతే
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)స్థానభ్రంశం;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    సమితి_type_on_దొంతర(cctx, expected, -1 - స్థానభ్రంశం);

    తిరిగి OK;
}

    పూర్ణాంకం
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_SETTYPE)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    తిరిగి OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    పూర్ణాంకం
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    స్థిర పూర్ణాంకం
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == ఖాళీ ? &t_any : &class->class_type);
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.classarg = class;
    ఒకవేళ (class != ఖాళీ)
	++class->class_refcount;
    తిరిగి OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    పూర్ణాంకం
generate_tv_నెట్టు(cctx_T *cctx, typval_T *tv)
{
    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_సంఖ్య);
	    ఆపు;
	స్థితి VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_సంఖ్య);
	    ఆపు;
	స్థితి VAR_సంఖ్య:
	    generate_PUSHNR(cctx, tv->vval.v_సంఖ్య);
	    ఆపు;
	స్థితి VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    ఆపు;
	స్థితి VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = ఖాళీ;
	    ఆపు;
	స్థితి VAR_LIST:
	    ఒకవేళ (tv->vval.v_list != ఖాళీ)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    ఆపు;
	స్థితి VAR_TUPLE:
	    ఒకవేళ (tv->vval.v_tuple != ఖాళీ)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    ఆపు;
	స్థితి VAR_DICT:
	    ఒకవేళ (tv->vval.v_dict != ఖాళీ)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    ఆపు;
#నిర్వచితమైతే FEAT_పని_CHANNEL
	స్థితి VAR_పని:
	    ఒకవేళ (tv->vval.v_పని != ఖాళీ)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    ఆపు;
	స్థితి VAR_CHANNEL:
	    ఒకవేళ (tv->vval.v_channel != ఖాళీ)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_FUNC:
	    ఒకవేళ (tv->vval.v_string != ఖాళీ)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, ఖాళీ, &t_func_unknown, TRUE);
	    ఆపు;
	స్థితి VAR_PARTIAL:
	    ఒకవేళ (tv->vval.v_partial != ఖాళీ)
		iemsg("non-null partial constant not supported");
	    ఒకవేళ (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == ఖాళీ)
		తిరిగి FAIL;
	    ఆపు;
	స్థితి VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = ఖాళీ;
	    ఆపు;
	స్థితి VAR_OBJECT:
	    ఒకవేళ (tv->vval.v_object != ఖాళీ)
	    {
		emsg(_(e_cannot_use_non_null_object));
		తిరిగి FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    ఆపు;
	స్థితి VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    ఆపు;
	అప్రమేయ:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    తిరిగి FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    తిరిగి OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    పూర్ణాంకం
generate_PUSHNR(cctx_T *cctx, varnumber_T సంఖ్య)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_సంఖ్య)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = సంఖ్య;

    ఒకవేళ (సంఖ్య == 0 || సంఖ్య == 1)
	// A 0 or 1 number can also be used as a bool.
	సమితి_type_on_దొంతర(cctx, &t_సంఖ్య_bool, 0);
    తిరిగి OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    పూర్ణాంకం
generate_PUSHBOOL(cctx_T *cctx, varnumber_T సంఖ్య)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = సంఖ్య;

    తిరిగి OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    పూర్ణాంకం
generate_PUSHSPEC(cctx_T *cctx, varnumber_T సంఖ్య)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     సంఖ్య == VVAL_NULL ? &t_null : &t_none)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = సంఖ్య;

    తిరిగి OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    పూర్ణాంకం
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.fnumber = fnumber;

    తిరిగి OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    పూర్ణాంకం
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    పూర్ణాంకం		ret = OK;

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	ఒకవేళ ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == ఖాళీ)
	    ret = FAIL;
	లేకపోతే
	{
	    isn->isn_arg.string = str == ఖాళీ ? ఖాళీ : *str;
	    తిరిగి OK;
	}
    }
    ఒకవేళ (str != ఖాళీ)
	VIM_CLEAR(*str);
    తిరిగి ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    పూర్ణాంకం
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#నిర్వచితమైతే FEAT_పని_CHANNEL
    ఒకవేళ (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
#లేకపోతే
    emsg(_(e_channel_పని_feature_not_అందుబాటు));
    తిరిగి FAIL;
#అయితే_ముగింపు
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    పూర్ణాంకం
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#నిర్వచితమైతే FEAT_పని_CHANNEL
    ఒకవేళ (generate_instr_type(cctx, ISN_PUSHJOB, &t_పని) == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
#లేకపోతే
    emsg(_(e_channel_పని_feature_not_అందుబాటు));
    తిరిగి FAIL;
#అయితే_ముగింపు
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    పూర్ణాంకం
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.blob = blob;

    తిరిగి OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    పూర్ణాంకం
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, పూర్ణాంకం may_ఉపసర్గ)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (name == ఖాళీ)
	funcname = ఖాళీ;
    లేకపోతే ఒకవేళ (!may_ఉపసర్గ
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != ఖాళీ)	    // autoload
	funcname = vim_strsave(name);
    లేకపోతే
    {
	funcname = alloc(STRLEN(name) + 3);
	ఒకవేళ (funcname != ఖాళీ)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    తిరిగి OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    పూర్ణాంకం
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.string = vim_strsave(name);
    ఒకవేళ (isn->isn_arg.string == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    పూర్ణాంకం
generate_GETITEM(cctx_T *cctx, పూర్ణాంకం సూచీ, పూర్ణాంకం with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_దొంతర(cctx, with_op ? 1 : 0);
    type_T	*అంశం_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    అంశం_type = get_అంశం_type(type);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_GETITEM)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.getitem.gi_సూచీ = సూచీ;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    తిరిగి నెట్టు_type_దొంతర(cctx, అంశం_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    స్థిర శూన్య
సమితి_tuple_ముక్క_type_on_దొంతర(type_T *type, పూర్ణాంకం లెక్క, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    ఒకవేళ ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= లెక్క)
    {
	సమితి_type_on_దొంతర(cctx, &t_tuple_any, 0);
	తిరిగి;
    }

    ga_init2(&tuple_types_ga, పరిమాణం(type_T *), 10);

    కొరకు (పూర్ణాంకం i = లెక్క; i < type->tt_argcount; i++)
    {
	ఒకవేళ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    సమితి_type_on_దొంతర(cctx, &t_tuple_any, 0);
	    తిరిగి;
	}
	((type_T **)tuple_types_ga.ga_దత్తాంశం)[tuple_types_ga.ga_len] =
							     type->tt_వాదనలు[i];
	tuple_types_ga.ga_len++;
    }

    సమితి_type_on_దొంతర(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    పూర్ణాంకం
generate_ముక్క(cctx_T *cctx, పూర్ణాంకం లెక్క)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_ముక్క)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = లెక్క;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_దొంతర(cctx, 0);
    ఒకవేళ (type->tt_type == VAR_TUPLE)
	సమితి_tuple_ముక్క_type_on_దొంతర(type, లెక్క, cctx);

    తిరిగి OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    పూర్ణాంకం
generate_CHECKLEN(cctx_T *cctx, పూర్ణాంకం min_len, పూర్ణాంకం మరిన్ని_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    ఒకవేళ ((isn = generate_instr(cctx, ISN_CHECKLEN)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_మరిన్ని_OK = మరిన్ని_OK;

    తిరిగి OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    పూర్ణాంకం
generate_STORE(cctx_T *cctx, isntype_T isn_type, పూర్ణాంకం idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (name != ఖాళీ)
	isn->isn_arg.string = vim_strsave(name);
    లేకపోతే
	isn->isn_arg.సంఖ్య = idx;

    తిరిగి OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    పూర్ణాంకం
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	పూర్ణాంకం	    ఎక్కించు,
	class_T	    *cl,
	పూర్ణాంకం	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ (ఎక్కించు)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_ఎక్కించు_CLASSMEMBER, m->ocm_type);
    }
    లేకపోతే
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    తిరిగి OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    స్థిర పూర్ణాంకం
generate_STOREOUTER(
	cctx_T	    *cctx,
	పూర్ణాంకం	    idx,
	పూర్ణాంకం	    level,
	పూర్ణాంకం	    loop_లోతు,
	పూర్ణాంకం	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_లోతు = -loop_లోతు - 1;
    }
    లేకపోతే
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_లోతు = level;
    }

    తిరిగి OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    పూర్ణాంకం
generate_STORENR(cctx_T *cctx, పూర్ణాంకం idx, varnumber_T విలువ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_STORENR)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = విలువ;

    తిరిగి OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    స్థిర పూర్ణాంకం
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	పూర్ణాంకం	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    తిరిగి OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    పూర్ణాంకం
generate_ఎక్కించు(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	పూర్ణాంకం	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_type2(cctx, isn_type, type, type)) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (name != ఖాళీ)
	isn->isn_arg.string = vim_strsave(name);
    లేకపోతే
	isn->isn_arg.సంఖ్య = idx;

    తిరిగి OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    పూర్ణాంకం
generate_LOADOUTER(
	cctx_T	    *cctx,
	పూర్ణాంకం	    idx,
	పూర్ణాంకం	    nesting,
	పూర్ణాంకం	    loop_లోతు,
	పూర్ణాంకం	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_లోతు = -loop_లోతు - 1;
    }
    లేకపోతే
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_లోతు = nesting;
    }

    తిరిగి OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    పూర్ణాంకం
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    పూర్ణాంకం	    di_flags;
    పూర్ణాంకం	    vidx = కనుగొను_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ (vidx < 0)
    {
	semsg(_(e_variable_not_దొరికింది_str), name);
	తిరిగి FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    తిరిగి generate_ఎక్కించు(cctx, ISN_LOADV, vidx, ఖాళీ, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    పూర్ణాంకం
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, పూర్ణాంకం forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, isn_type)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    తిరిగి OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    పూర్ణాంకం
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ (generate_instr(cctx, ISN_LOCKCONST) == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    పూర్ణాంకం
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	పూర్ణాంకం	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    లేకపోతే
	isn = generate_instr_drop(cctx, isn_type, 1);
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    తిరిగి OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    పూర్ణాంకం
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	పూర్ణాంకం	    sid,
	పూర్ణాంకం	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_అంశం(sid);

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    లేకపోతే
	isn = generate_instr_drop(cctx, isn_type, 1);
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    ఒకవేళ (sref == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    తిరిగి OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    పూర్ణాంకం
generate_NEWLIST(cctx_T *cctx, పూర్ణాంకం లెక్క, పూర్ణాంకం use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_NEWLIST)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = use_null ? -1 : లెక్క;

    // Get the member type and the declared member type from all the items on
    // the stack.
    ఒకవేళ ((member_type = get_member_type_from_దొంతర(లెక్క, 1, cctx)) == ఖాళీ)
	తిరిగి FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_దొంతర.ga_len -= లెక్క;

    // add the list type to the type stack
    తిరిగి నెట్టు_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    పూర్ణాంకం
generate_NEWTUPLE(cctx_T *cctx, పూర్ణాంకం లెక్క, పూర్ణాంకం use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = use_null ? -1 : లెక్క;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, పరిమాణం(type_T *), 10);

    ఒకవేళ (get_tuple_type_from_దొంతర(లెక్క, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	తిరిగి FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_దొంతర.ga_len -= లెక్క;

    // add the tuple type to the type stack
    తిరిగి నెట్టు_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    పూర్ణాంకం
generate_NEWDICT(cctx_T *cctx, పూర్ణాంకం లెక్క, పూర్ణాంకం use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_NEWDICT)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = use_null ? -1 : లెక్క;

    ఒకవేళ ((member_type = get_member_type_from_దొంతర(లెక్క, 2, cctx)) == ఖాళీ)
	తిరిగి FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_దొంతర.ga_len -= 2 * లెక్క;

    // add the dict type to the type stack
    తిరిగి నెట్టు_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    పూర్ణాంకం
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	పూర్ణాంకం	    object_పద్ధతి,
	పూర్ణాంకం	    fi,
	పూర్ణాంకం	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    పూర్ణాంకం		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_FUNCREF)) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (isn_idx != ఖాళీ)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    ఒకవేళ (ufunc->uf_def_స్థితి == UF_NOT_COMPILED || has_vars || cl != ఖాళీ)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	ఒకవేళ (extra == ఖాళీ)
	    తిరిగి FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	ఒకవేళ (cl != ఖాళీ)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_పద్ధతి = object_పద్ధతి;
	    extra->fre_పద్ధతి_idx = fi;
	}
    }
    ఒకవేళ (ufunc->uf_def_స్థితి == UF_NOT_COMPILED || cl != ఖాళీ)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ఒకవేళ (ufunc->uf_def_స్థితి != UF_NOT_COMPILED && cl == ఖాళీ)
    {
	ఒకవేళ (isn_idx == ఖాళీ && ufunc->uf_def_స్థితి == UF_గమ్యం_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (శూన్య)compile_def_function(ufunc, FALSE, CT_NONE, ఖాళీ);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_సంవృతం = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    ఒకవేళ ((ufunc->uf_flags & FC_సంవృతం)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_సంవృతం;

    type = ufunc->uf_func_type == ఖాళీ ? &t_func_any : ufunc->uf_func_type;
    తిరిగి నెట్టు_type_దొంతర(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    పూర్ణాంకం
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    పూర్ణాంకం		ret = OK;

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	ఒకవేళ ((isn = generate_instr(cctx, ISN_NEWFUNC)) == ఖాళీ)
	    ret = FAIL;
	లేకపోతే
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    ఒకవేళ (arg == ఖాళీ)
		ret = FAIL;
	    లేకపోతే
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_సంవృతం = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(శూన్య)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		తిరిగి OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    తిరిగి ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    పూర్ణాంకం
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_DEF)) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	ఒకవేళ (isn->isn_arg.string == ఖాళీ)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    పూర్ణాంకం
generate_JUMP(cctx_T *cctx, jumpwhen_T when, పూర్ణాంకం where)
{
    isn_T	*isn;
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_JUMP)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    ఒకవేళ (when != JUMP_ALWAYS && దొంతర->ga_len > 0)
	--దొంతర->ga_len;

    తిరిగి OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    పూర్ణాంకం
generate_WHILE(cctx_T *cctx, పూర్ణాంకం funcref_idx)
{
    isn_T	*isn;
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_WHILE)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_చివర = 0;  // filled in later

    ఒకవేళ (దొంతర->ga_len > 0)
	--దొంతర->ga_len;

    తిరిగి OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    పూర్ణాంకం
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, పూర్ణాంకం arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, isn_type)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    తిరిగి OK;
}

    పూర్ణాంకం
generate_FOR(cctx_T *cctx, పూర్ణాంకం loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_FOR)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    తిరిగి నెట్టు_type_దొంతర(cctx, &t_any);
}

    పూర్ణాంకం
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_ENDLOOP)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.endloop.చివర_లోతు = loop_info->li_లోతు;
    isn->isn_arg.endloop.చివర_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.చివర_var_idx = loop_info->li_local_లెక్క;
    isn->isn_arg.endloop.చివర_var_లెక్క =
			   cctx->ctx_locals.ga_len - loop_info->li_local_లెక్క;
    తిరిగి OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    పూర్ణాంకం
generate_TRYCONT(cctx_T *cctx, పూర్ణాంకం స్థాయిలు, పూర్ణాంకం where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_TRYCONT)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.trycont.tct_స్థాయిలు = స్థాయిలు;
    isn->isn_arg.trycont.tct_where = where;

    తిరిగి OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    పూర్ణాంకం
check_అంతర్గత_func_వాదనలు(
	cctx_T	*cctx,
	పూర్ణాంకం	func_idx,
	పూర్ణాంకం	argcount,
	పూర్ణాంకం	పద్ధతి_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;
    పూర్ణాంకం		argoff = check_అంతర్గత_func(func_idx, argcount);

    ఒకవేళ (argoff < 0)
	తిరిగి FAIL;

    ఒకవేళ (పద్ధతి_call && argoff > 1)
    {
	ఒకవేళ (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 అంతర్గత_func_name(func_idx));
	    తిరిగి FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	ఒకవేళ (isn  == ఖాళీ)
	    తిరిగి FAIL;
	isn->isn_arg.shuffle.shfl_అంశం = argcount;
	isn->isn_arg.shuffle.shfl_పైకి = argoff - 1;
    }

    ఒకవేళ (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)దొంతర->ga_దత్తాంశం) + దొంతర->ga_len - argcount;

	// Check the types of the arguments.
	ఒకవేళ (పద్ధతి_call && argoff > 1)
	{
	    పూర్ణాంకం i;

	    కొరకు (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	లేకపోతే
	{
	    పూర్ణాంకం i;

	    కొరకు (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ఒకవేళ (అంతర్గత_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    పూర్ణాంకం
generate_BCALL(cctx_T *cctx, పూర్ణాంకం func_idx, పూర్ణాంకం argcount, పూర్ణాంకం పద్ధతి_call)
{
    isn_T	*isn;
    garray_T	*దొంతర = &cctx->ctx_type_దొంతర;
    type2_T	*argtypes = ఖాళీ;
    type2_T	shuffled_argtypes[MAX_FUNC_వాదనలు];
    type2_T	*maptype = ఖాళీ;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    ఒకవేళ (check_అంతర్గత_func_వాదనలు(cctx, func_idx, argcount, పద్ధతి_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (అంతర్గత_func_is_map(func_idx))
	maptype = argtypes;

    ఒకవేళ ((isn = generate_instr(cctx, ISN_BCALL)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    దొంతర->ga_len -= argcount;
    type = అంతర్గత_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    ఒకవేళ (నెట్టు_type_stack2(cctx, type, decl_type) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (maptype != ఖాళీ && maptype[0].type_decl->tt_member != ఖాళీ
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    తిరిగి OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    పూర్ణాంకం
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*అంశం_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_దొంతర(cctx, 1);
    ఒకవేళ (arg_type_modifiable(list_type, 1) == FAIL)
	తిరిగి FAIL;
    అంశం_type = get_type_on_దొంతర(cctx, 0);
    expected = list_type->tt_member;
    ఒకవేళ (need_type(అంశం_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (generate_instr(cctx, ISN_LISTAPPEND) == ఖాళీ)
	తిరిగి FAIL;

    --cctx->ctx_type_దొంతర.ga_len;	    // drop the argument
    తిరిగి OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    పూర్ణాంకం
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*అంశం_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    ఒకవేళ (arg_type_modifiable(get_decl_type_on_దొంతర(cctx, 1), 1) == FAIL)
	తిరిగి FAIL;
    అంశం_type = get_type_on_దొంతర(cctx, 0);
    ఒకవేళ (need_type(అంశం_type, &t_సంఖ్య, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	తిరిగి FAIL;

    ఒకవేళ (generate_instr(cctx, ISN_BLOBAPPEND) == ఖాళీ)
	తిరిగి FAIL;

    --cctx->ctx_type_దొంతర.ga_len;	    // drop the argument
    తిరిగి OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    స్థిర isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    ఒకవేళ (cl != ఖాళీ)
	తిరిగి ISN_METHODCALL;

    ఒకవేళ (ufunc->uf_def_స్థితి == UF_NOT_COMPILED)
	తిరిగి ISN_UCALL;

    // function invoked from a lambda
    ఒకవేళ (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	ఒకవేళ (ufunc->uf_name[0] == K_SPECIAL)
	    తిరిగి ISN_UCALL;
    }

    తిరిగి ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    పూర్ణాంకం
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	పూర్ణాంకం	    mi,
	పూర్ణాంకం	    pushed_argcount,
	పూర్ణాంకం	    is_super)
{
    isn_T	*isn;
    పూర్ణాంకం		regular_వాదనలు = ufunc->uf_వాదనలు.ga_len;
    పూర్ణాంకం		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ (argcount > regular_వాదనలు && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	తిరిగి FAIL;
    }
    ఒకవేళ (argcount < regular_వాదనలు - ufunc->uf_def_వాదనలు.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	తిరిగి FAIL;
    }

    ఒకవేళ (ufunc->uf_def_స్థితి != UF_NOT_COMPILED
	    && ufunc->uf_def_స్థితి != UF_COMPILE_ERROR)
    {
	పూర్ణాంకం		i;
	compiletype_T	compile_type;

	కొరకు (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_దొంతర(cctx, argcount - i - 1);
	    ఒకవేళ (check_type_is_విలువ(actual) == FAIL)
		తిరిగి FAIL;
	    ఒకవేళ (actual->tt_type == VAR_SPECIAL
			      && i >= regular_వాదనలు - ufunc->uf_def_వాదనలు.ga_len)
	    {
		// assume v:none used for default argument value
		కొనసాగు;
	    }
	    ఒకవేళ (i < regular_వాదనలు)
	    {
		ఒకవేళ (ufunc->uf_arg_types == ఖాళీ)
		    కొనసాగు;
		expected = ufunc->uf_arg_types[i];
	    }
	    లేకపోతే ఒకవేళ (ufunc->uf_va_type == ఖాళీ
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    లేకపోతే
		expected = ufunc->uf_va_type->tt_member;
	    ఒకవేళ (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		తిరిగి FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	ఒకవేళ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == ఖాళీ,
						   compile_type, ఖాళీ) == FAIL)
	    తిరిగి FAIL;
    }
    ఒకవేళ (ufunc->uf_def_స్థితి == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_గమ్యం_function_that_failed_గమ్యం_compile_str,
							       ufunc->uf_name);
	తిరిగి FAIL;
    }

    ఒకవేళ ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == ఖాళీ)
	తిరిగి FAIL;
    ఒకవేళ (cl != ఖాళీ /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	ఒకవేళ (isn->isn_arg.mfunc == ఖాళీ)
	    తిరిగి FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = ఖాళీ;
	ఒకవేళ (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = ఖాళీ;
	isn->isn_arg.mfunc->cmf_cache_generic = ఖాళీ;
	isn->isn_arg.mfunc->cmf_cache_ufunc = ఖాళీ;
    }
    లేకపోతే ఒకవేళ (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    లేకపోతే
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_దొంతర.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    ఒకవేళ (ufunc->uf_class != ఖాళీ)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_దొంతర(cctx, 0);
	ఒకవేళ (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_దొంతర.ga_len--;
    }

    // add return type
    తిరిగి నెట్టు_type_దొంతర(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    పూర్ణాంకం
generate_UCALL(cctx_T *cctx, char_u *name, పూర్ణాంకం argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_UCALL)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_దొంతర.ga_len -= argcount;

    // add return value
    తిరిగి నెట్టు_type_దొంతర(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    పూర్ణాంకం
check_func_వాదనలు_from_type(
	cctx_T	*cctx,
	type_T	*type,
	పూర్ణాంకం	argcount,
	పూర్ణాంకం	at_శిఖరం,
	char_u	*name)
{
    ఒకవేళ (type->tt_argcount != -1)
    {
	పూర్ణాంకం	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	ఒకవేళ (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    తిరిగి FAIL;
	}
	ఒకవేళ (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    తిరిగి FAIL;
	}
	ఒకవేళ (type->tt_వాదనలు != ఖాళీ)
	{
	    పూర్ణాంకం i;

	    కొరకు (i = 0; i < argcount; ++i)
	    {
		పూర్ణాంకం	స్థానభ్రంశం = -argcount + i - (at_శిఖరం ? 0 : 1);
		type_T	*actual = get_type_on_దొంతర(cctx, -1 - స్థానభ్రంశం);
		type_T	*expected;

		ఒకవేళ (check_type_is_విలువ(actual) == FAIL)
		    తిరిగి FAIL;
		ఒకవేళ (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_వాదనలు[type->tt_argcount - 1];
		    ఒకవేళ (expected != ఖాళీ && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    ఒకవేళ (expected == ఖాళీ)
			expected = &t_any;
		}
		లేకపోతే ఒకవేళ (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		లేకపోతే
		    expected = type->tt_వాదనలు[i];
		ఒకవేళ (need_type(actual, expected, 0,
				     స్థానభ్రంశం, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    తిరిగి FAIL;
		}
	    }
	}
    }

    తిరిగి OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    పూర్ణాంకం
generate_PCALL(
	cctx_T	*cctx,
	పూర్ణాంకం	argcount,
	char_u	*name,
	type_T	*type,
	పూర్ణాంకం	at_శిఖరం)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    ఒకవేళ (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    లేకపోతే ఒకవేళ (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	ఒకవేళ (check_func_వాదనలు_from_type(cctx, type, argcount, at_శిఖరం, name)
								       == FAIL)
	    తిరిగి FAIL;

	ret_type = type->tt_member;
	ఒకవేళ (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    లేకపోతే
    {
	semsg(_(e_not_callable_type_str), name);
	తిరిగి FAIL;
    }

    ఒకవేళ ((isn = generate_instr(cctx, ISN_PCALL)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.pfunc.cpf_శిఖరం = at_శిఖరం;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_దొంతర.ga_len -= argcount + 1;

    // push the return value
    నెట్టు_type_దొంతర(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    ఒకవేళ (at_శిఖరం && generate_instr(cctx, ISN_PCALL_చివర) == ఖాళీ)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    పూర్ణాంకం
generate_DEFER(cctx_T *cctx, పూర్ణాంకం var_idx, పూర్ణాంకం argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    తిరిగి OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    పూర్ణాంకం
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_దొంతర(cctx, 0);
    ఒకవేళ (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	అక్షరం *tofree;

	semsg(_(e_expected_dictionary_for_using_కీలకం_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	తిరిగి FAIL;
    }
    // change dict type to dict member type
    ఒకవేళ (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	సమితి_type_on_దొంతర(cctx, ntype, 0);
    }

    తిరిగి OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    పూర్ణాంకం
generate_ECHO(cctx_T *cctx, పూర్ణాంకం with_white, పూర్ణాంకం లెక్క)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_drop(cctx, ISN_ECHO, లెక్క)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_లెక్క = లెక్క;

    తిరిగి OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    పూర్ణాంకం
generate_MULT_సమాసం(cctx_T *cctx, isntype_T isn_type, పూర్ణాంకం లెక్క)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr_drop(cctx, isn_type, లెక్క)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = లెక్క;
    తిరిగి OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    పూర్ణాంకం
generate_ECHOWINDOW(cctx_T *cctx, పూర్ణాంకం లెక్క, దీర్ఘ సమయం)
{
    isn_T	*isn;

    ఒకవేళ ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, లెక్క)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.echowin.ewin_లెక్క = లెక్క;
    isn->isn_arg.echowin.ewin_సమయం = సమయం;
    తిరిగి OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    పూర్ణాంకం
generate_ఆధారం(cctx_T *cctx, పూర్ణాంకం sid)
{
    isn_T	*isn;

    ఒకవేళ ((isn = generate_instr(cctx, ISN_ఆధారం)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = sid;

    తిరిగి OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    పూర్ణాంకం
generate_PUT(cctx_T *cctx, పూర్ణాంకం regname, linenr_T lnum, పూర్ణాంకం fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    ఒకవేళ (isn == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    తిరిగి OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    పూర్ణాంకం
generate_LOCKUNLOCK(cctx_T *cctx, char_u *పంక్తి, పూర్ణాంకం is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == ఖాళీ)
	తిరిగి FAIL;
    class_T *cl = cctx->ctx_ufunc != ఖాళీ ? cctx->ctx_ufunc->uf_class : ఖాళీ;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(పంక్తి);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    ఒకవేళ (cl != ఖాళీ)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    తిరిగి OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    పూర్ణాంకం
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *పంక్తి)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, isntype)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.string = vim_strsave(పంక్తి);
    తిరిగి OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    పూర్ణాంకం
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    పూర్ణాంకం		ret = OK;

    ఒకవేళ (cctx->ctx_skip != SKIP_YES)
    {
	ఒకవేళ ((isn = generate_instr(cctx, isntype)) == ఖాళీ)
	    ret = FAIL;
	లేకపోతే
	{
	    isn->isn_arg.string = str;
	    తిరిగి OK;
	}
    }
    vim_free(str);
    తిరిగి ret;
}

    పూర్ణాంకం
generate_LEGACY_EVAL(cctx_T *cctx, char_u *పంక్తి)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.string = vim_strsave(పంక్తి);

    తిరిగి నెట్టు_type_దొంతర(cctx, &t_any);
}

    పూర్ణాంకం
generate_EXECCONCAT(cctx_T *cctx, పూర్ణాంకం లెక్క)
{
    isn_T	*isn;

    ఒకవేళ ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, లెక్క)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.సంఖ్య = లెక్క;
    తిరిగి OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    పూర్ణాంకం
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    ఒకవేళ ((isn = generate_instr(cctx, ISN_RANGE)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.string = range;

    తిరిగి నెట్టు_type_దొంతర(cctx, &t_సంఖ్య);
}

    పూర్ణాంకం
generate_UNPACK(cctx_T *cctx, పూర్ణాంకం var_లెక్క, పూర్ణాంకం semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_UNPACK)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.unpack.unp_లెక్క = var_లెక్క;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    తిరిగి OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    పూర్ణాంకం
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    ఒకవేళ (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	ఒకవేళ ((isn = generate_instr(cctx, ISN_CMDMOD)) == ఖాళీ)
	    తిరిగి FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	ఒకవేళ (isn->isn_arg.cmdmod.cf_cmdmod == ఖాళీ)
	    తిరిగి FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, పరిమాణం(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = ఖాళీ;
    }

    తిరిగి OK;
}

    పూర్ణాంకం
generate_undo_cmdmods(cctx_T *cctx)
{
    ఒకవేళ (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == ఖాళీ)
	తిరిగి FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    తిరిగి OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    పూర్ణాంకం
generate_store_var(
	cctx_T		*cctx,
	కేటాయించు_dest_T	dest,
	పూర్ణాంకం		opt_flags,
	పూర్ణాంకం		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    ఎంపిక (dest)
    {
	స్థితి dest_option:
	    తిరిగి generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	స్థితి dest_func_option:
	    తిరిగి generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	స్థితి dest_global:
	    // include g: with the name, easier to execute that way
	    తిరిగి generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == ఖాళీ
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	స్థితి dest_బఫరు:
	    // include b: with the name, easier to execute that way
	    తిరిగి generate_STORE(cctx, ISN_STOREB, 0, name);
	స్థితి dest_కిటికీ:
	    // include w: with the name, easier to execute that way
	    తిరిగి generate_STORE(cctx, ISN_STOREW, 0, name);
	స్థితి dest_tab:
	    // include t: with the name, easier to execute that way
	    తిరిగి generate_STORE(cctx, ISN_STORET, 0, name);
	స్థితి dest_env:
	    తిరిగి generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	స్థితి dest_reg:
	    తిరిగి generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], ఖాళీ);
	స్థితి dest_vimvar:
	    తిరిగి generate_STORE(cctx, ISN_STOREV, vimvaridx, ఖాళీ);
	స్థితి dest_script:
	స్థితి dest_script_v9:
	    {
		పూర్ణాంకం	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		పూర్ణాంకం	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		ఒకవేళ (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    ఒకవేళ (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_అంశం(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_అంశం(scriptvar_sid)
						  ->sn_autoload_ఉపసర్గ == ఖాళీ)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			//మొదటి
			ఒకవేళ (generate_ఆధారం(cctx, scriptvar_sid) == FAIL)
			    తిరిగి FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    తిరిగి generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		తిరిగి generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	స్థితి dest_class_member:
	    తిరిగి generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	స్థితి dest_local:
	స్థితి dest_సమాసం:
	    // cannot happen
	    ఆపు;
    }
    తిరిగి FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    పూర్ణాంకం
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    కొరకు (;;)
    {
	ఒకవేళ (scope == ఖాళీ)
	    ఆపు;
	ఒకవేళ (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    తిరిగి TRUE;
	scope = scope->se_outer;
    }
    తిరిగి FALSE;
}

    పూర్ణాంకం
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, పూర్ణాంకం instr_లెక్క, పూర్ణాంకం is_decl)
{
    ఒకవేళ (lhs->lhs_dest != dest_local)
	తిరిగి generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    ఒకవేళ (lhs->lhs_lvar == ఖాళీ)
	తిరిగి OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_దత్తాంశం) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    ఒకవేళ (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_లెక్క + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.సంఖ్య;
	garray_T    *దొంతర = &cctx->ctx_type_దొంతర;

	ఒకవేళ (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	లేకపోతే
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	ఒకవేళ (దొంతర->ga_len > 0)
	    --దొంతర->ga_len;
    }
    లేకపోతే ఒకవేళ (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_లోతు,
		lhs->lhs_lvar->lv_loop_idx);
    లేకపోతే
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, ఖాళీ);
    తిరిగి OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    పూర్ణాంకం
generate_SCRIPTCTX_సమితి(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ఒకవేళ ((isn = generate_instr(cctx, ISN_SCRIPTCTX_సమితి)) == ఖాళీ)
	తిరిగి FAIL;
    isn->isn_arg.setsctx = new_sctx;
    తిరిగి OK;
}

#ఒకవేళ defined(FEAT_PROFILE)
    శూన్య
may_generate_prof_చివర(cctx_T *cctx, పూర్ణాంకం prof_lnum)
{
    ఒకవేళ (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_చివర);
}
#అయితే_ముగింపు


/*
 * Delete an instruction, free what it contains.
 */
    శూన్య
delete_instr(isn_T *isn)
{
    ఎంపిక (isn->isn_type)
    {
	స్థితి ISN_AUTOLOAD:
	స్థితి ISN_DEF:
	స్థితి ISN_EXEC:
	స్థితి ISN_EXECRANGE:
	స్థితి ISN_EXEC_విభజించు:
	స్థితి ISN_LEGACY_EVAL:
	స్థితి ISN_LOADAUTO:
	స్థితి ISN_LOADB:
	స్థితి ISN_LOADENV:
	స్థితి ISN_LOADG:
	స్థితి ISN_LOADOPT:
	స్థితి ISN_LOADT:
	స్థితి ISN_LOADW:
	స్థితి ISN_PUSHEXC:
	స్థితి ISN_PUSHFUNC:
	స్థితి ISN_PUSHS:
	స్థితి ISN_RANGE:
	స్థితి ISN_STOREAUTO:
	స్థితి ISN_STOREB:
	స్థితి ISN_STOREENV:
	స్థితి ISN_STOREG:
	స్థితి ISN_STORET:
	స్థితి ISN_STOREW:
	స్థితి ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    ఆపు;

	స్థితి ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    ఆపు;

	స్థితి ISN_SUBSTITUTE:
	    {
		పూర్ణాంకం	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		కొరకు (idx = 0; list[idx].isn_type != ISN_ముగింపు; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ఆపు;

	స్థితి ISN_INSTR:
	    {
		పూర్ణాంకం	idx;
		isn_T	*list = isn->isn_arg.instr;

		కొరకు (idx = 0; list[idx].isn_type != ISN_ముగింపు; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ఆపు;

	స్థితి ISN_LOADS:
	స్థితి ISN_LOADEXPORT:
	స్థితి ISN_STORES:
	స్థితి ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    ఆపు;

	స్థితి ISN_UNLET:
	స్థితి ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    ఆపు;

	స్థితి ISN_STOREOPT:
	స్థితి ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    ఆపు;

	స్థితి ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    ఆపు;

	స్థితి ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    ఆపు;

	స్థితి ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    ఆపు;

	స్థితి ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		ఒకవేళ (extra == ఖాళీ || extra->fre_func_name == ఖాళీ)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    ఒకవేళ (ufunc != ఖాళీ && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		ఒకవేళ (extra != ఖాళీ)
		{
		    char_u *name = extra->fre_func_name;

		    ఒకవేళ (name != ఖాళీ)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    ఒకవేళ (extra->fre_class != ఖాళీ)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    ఆపు;

	స్థితి ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
		    + isn->isn_arg.dfunc.cdf_idx;

		ఒకవేళ (dfunc->df_ufunc != ఖాళీ
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    ఆపు;

	స్థితి ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    ఆపు;

	స్థితి ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		ఒకవేళ (arg != ఖాళీ)
		{
		    ufunc_T *ufunc = కనుగొను_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    ఒకవేళ (ufunc != ఖాళీ)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    ఆపు;

	స్థితి ISN_CHECKTYPE:
	స్థితి ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    ఆపు;

	స్థితి ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    ఆపు;

	స్థితి ISN_LOADSCRIPT:
	స్థితి ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    ఆపు;

	స్థితి ISN_ఎక్కించు_CLASSMEMBER:
	స్థితి ISN_STORE_CLASSMEMBER:
	స్థితి ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    ఆపు;

	స్థితి ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    ఆపు;

	స్థితి ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    ఆపు;

	స్థితి ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    ఆపు;

	స్థితి ISN_2BOOL:
	స్థితి ISN_2STRING:
	స్థితి ISN_2STRING_ANY:
	స్థితి ISN_ADDBLOB:
	స్థితి ISN_ADDLIST:
	స్థితి ISN_ADDTUPLE:
	స్థితి ISN_ANYINDEX:
	స్థితి ISN_ANYSLICE:
	స్థితి ISN_BCALL:
	స్థితి ISN_BLOBAPPEND:
	స్థితి ISN_BLOBINDEX:
	స్థితి ISN_BLOBSLICE:
	స్థితి ISN_CATCH:
	స్థితి ISN_CEXPR_AUCMD:
	స్థితి ISN_CHECKLEN:
	స్థితి ISN_CLEARDICT:
	స్థితి ISN_CMDMOD_REV:
	స్థితి ISN_COMPAREANY:
	స్థితి ISN_COMPAREBLOB:
	స్థితి ISN_COMPAREBOOL:
	స్థితి ISN_COMPAREDICT:
	స్థితి ISN_COMPAREFLOAT:
	స్థితి ISN_COMPAREFUNC:
	స్థితి ISN_COMPARELIST:
	స్థితి ISN_COMPARETUPLE:
	స్థితి ISN_COMPARENR:
	స్థితి ISN_COMPARENULL:
	స్థితి ISN_COMPAREOBJECT:
	స్థితి ISN_COMPARESPECIAL:
	స్థితి ISN_COMPARESTRING:
	స్థితి ISN_CONCAT:
	స్థితి ISN_CONSTRUCT:
	స్థితి ISN_COND2BOOL:
	స్థితి ISN_DEBUG:
	స్థితి ISN_DEFER:
	స్థితి ISN_DROP:
	స్థితి ISN_ECHO:
	స్థితి ISN_ECHOCONSOLE:
	స్థితి ISN_ECHOERR:
	స్థితి ISN_ECHOMSG:
	స్థితి ISN_ECHOWINDOW:
	స్థితి ISN_ENDLOOP:
	స్థితి ISN_ENDTRY:
	స్థితి ISN_EXECCONCAT:
	స్థితి ISN_EXECUTE:
	స్థితి ISN_FINALLY:
	స్థితి ISN_ముగింపు:
	స్థితి ISN_FOR:
	స్థితి ISN_GETITEM:
	స్థితి ISN_GET_OBJ_MEMBER:
	స్థితి ISN_JUMP:
	స్థితి ISN_JUMP_IF_ARG_NOT_సమితి:
	స్థితి ISN_JUMP_IF_ARG_సమితి:
	స్థితి ISN_LISTAPPEND:
	స్థితి ISN_LISTINDEX:
	స్థితి ISN_LISTSLICE:
	స్థితి ISN_TUPLEINDEX:
	స్థితి ISN_TUPLESLICE:
	స్థితి ISN_ఎక్కించు:
	స్థితి ISN_LOADBDICT:
	స్థితి ISN_LOADGDICT:
	స్థితి ISN_LOADOUTER:
	స్థితి ISN_LOADREG:
	స్థితి ISN_LOADTDICT:
	స్థితి ISN_LOADV:
	స్థితి ISN_LOADWDICT:
	స్థితి ISN_LOCKCONST:
	స్థితి ISN_MEMBER:
	స్థితి ISN_NEGATENR:
	స్థితి ISN_NEWDICT:
	స్థితి ISN_NEWLIST:
	స్థితి ISN_NEWTUPLE:
	స్థితి ISN_NEWPARTIAL:
	స్థితి ISN_OPANY:
	స్థితి ISN_OPFLOAT:
	స్థితి ISN_OPNR:
	స్థితి ISN_PCALL:
	స్థితి ISN_PCALL_చివర:
	స్థితి ISN_PROF_చివర:
	స్థితి ISN_PROF_ప్రారంభం:
	స్థితి ISN_PUSHBOOL:
	స్థితి ISN_PUSHCHANNEL:
	స్థితి ISN_PUSHF:
	స్థితి ISN_PUSHJOB:
	స్థితి ISN_PUSHNR:
	స్థితి ISN_PUSHOBJ:
	స్థితి ISN_PUSHSPEC:
	స్థితి ISN_PUT:
	స్థితి ISN_IPUT:
	స్థితి ISN_REDIREND:
	స్థితి ISN_REDIRSTART:
	స్థితి ISN_RETURN:
	స్థితి ISN_RETURN_OBJECT:
	స్థితి ISN_RETURN_VOID:
	స్థితి ISN_SHUFFLE:
	స్థితి ISN_ముక్క:
	స్థితి ISN_ఆధారం:
	స్థితి ISN_STORE:
	స్థితి ISN_STORENR:
	స్థితి ISN_STOREOUTER:
	స్థితి ISN_STORE_THIS:
	స్థితి ISN_STORERANGE:
	స్థితి ISN_STOREREG:
	స్థితి ISN_STOREV:
	స్థితి ISN_STRINDEX:
	స్థితి ISN_STRSLICE:
	స్థితి ISN_THROW:
	స్థితి ISN_TRYCONT:
	స్థితి ISN_UNLETINDEX:
	స్థితి ISN_UNLETRANGE:
	స్థితి ISN_UNPACK:
	స్థితి ISN_USEDICT:
	స్థితి ISN_WHILE:
	స్థితి ISN_SCRIPTCTX_సమితి:
	// nothing allocated
	ఆపు;
    }
}

    శూన్య
clear_instr_ga(garray_T *gap)
{
    పూర్ణాంకం idx;

    కొరకు (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_దత్తాంశం) + idx);
    ga_clear(gap);
}


#అయితే_ముగింపు  // defined(FEAT_EVAL)
