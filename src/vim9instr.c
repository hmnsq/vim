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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USING_FLOAT_STUFF
#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_NULL_IF_SKIP(cctx) ਜੇ (cctx->ctx_skip == SKIP_YES) ਵਾਪਸ ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RETURN_OK_IF_SKIP(cctx) ਜੇ (cctx->ctx_skip == SKIP_YES) ਵਾਪਸ OK

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
    ਜੇ (GA_GROW_FAILS(instr, 1))
	ਵਾਪਸ ਨਿਰਾ;
    isn = ((isn_T *)instr->ga_ਅੰਕੜੇ) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    ਵਾਪਸ isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, ਪੂਰਨ drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_ਢੇਰ.ga_len -= drop;
    ਵਾਪਸ generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    ਸਥਿਰ isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    ਜੇ ((isn = generate_instr(cctx, isn_type)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਧੱਕੋ_type_stack2(cctx, type == ਨਿਰਾ ? &t_any : type,
			       decl_type == ਨਿਰਾ ? &t_any : decl_type) == FAIL)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    ਵਾਪਸ generate_instr_type2(cctx, isn_type, type, &t_any);
}

/*
 * Generate an ISN_DEBUG instruction.
 */
    isn_T *
generate_instr_debug(cctx_T *cctx)
{
    isn_T	*isn;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
					       + cctx->ctx_ufunc->uf_dfunc_idx;

    ਜੇ ((isn = generate_instr(cctx, ISN_DEBUG)) == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    isn->isn_arg.debug.dbg_var_ਨਾਂ_len = dfunc->df_var_ਨਾਂ.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    ਵਾਪਸ isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    ਪੂਰਨ
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.construct.construct_ਮਾਪ = ਆਕਾਰ(object_T)
			       + cl->class_obj_member_ਗਿਣਤੀ * ਆਕਾਰ(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    ਵਾਪਸ OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    ਪੂਰਨ
generate_GET_OBJ_MEMBER(cctx_T *cctx, ਪੂਰਨ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    ਜੇ (isn == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    isn->isn_arg.classmember.cm_class = ਨਿਰਾ;
    isn->isn_arg.classmember.cm_idx = idx;
    ਵਾਪਸ ਧੱਕੋ_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    ਪੂਰਨ
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, ਪੂਰਨ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    ਜੇ (isn == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    ਵਾਪਸ ਧੱਕੋ_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    ਪੂਰਨ
generate_STORE_THIS(cctx_T *cctx, ਪੂਰਨ idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    ਜੇ (isn == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    isn->isn_arg.ਸੰਖਿਆ = idx;
    ਵਾਪਸ OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    ਪੂਰਨ
may_generate_2STRING(ਪੂਰਨ ਵਿੱਥ, ਪੂਰਨ tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_ਢੇਰ(cctx, -1 - ਵਿੱਥ);

    ਚੋਣ (type->tt_type)
    {
	// nothing to be done
	ਹਾਲਤ VAR_STRING: ਵਾਪਸ OK;

	// conversion possible
	ਹਾਲਤ VAR_SPECIAL:
	ਹਾਲਤ VAR_BOOL:
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	ਹਾਲਤ VAR_FLOAT:
			 ਰੋਕੋ;

	// conversion possible (with runtime check)
	ਹਾਲਤ VAR_ANY:
	ਹਾਲਤ VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 ਰੋਕੋ;

	// conversion possible when tolerant
	ਹਾਲਤ VAR_LIST:
	ਹਾਲਤ VAR_TUPLE:
	ਹਾਲਤ VAR_DICT:
			 ਜੇ (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     ਰੋਕੋ;
			 }
			 ਜੇ (tostring_flags & TOSTRING_INTERPOLATE)
			     ਰੋਕੋ;
			 // FALLTHROUGH

	// conversion not possible
	ਹਾਲਤ VAR_VOID:
	ਹਾਲਤ VAR_BLOB:
	ਹਾਲਤ VAR_FUNC:
	ਹਾਲਤ VAR_PARTIAL:
	ਹਾਲਤ VAR_ਕੰਮ:
	ਹਾਲਤ VAR_CHANNEL:
	ਹਾਲਤ VAR_INSTR:
	ਹਾਲਤ VAR_CLASS:
	ਹਾਲਤ VAR_OBJECT:
	ਹਾਲਤ VAR_TYPEALIAS:
			 ਮੰਜ਼ਿਲ_string_error(type->tt_type);
			 ਵਾਪਸ FAIL;
    }

    ਸੈੱਟ_type_on_ਢੇਰ(cctx, &t_string, -1 - ਵਿੱਥ);
    ਜੇ ((isn = generate_instr(cctx, isntype)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.tostring.ਵਿੱਥ = ਵਿੱਥ;
    isn->isn_arg.tostring.flags = tostring_flags;

    ਵਾਪਸ OK;
}

    ਸਥਿਰ ਪੂਰਨ
check_ਸੰਖਿਆ_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    ਜੇ (!((type1 == VAR_ਸੰਖਿਆ || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_ਸੰਖਿਆ || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	ਜੇ (check_type_is_ਮੁੱਲ(typ1) == FAIL
		|| check_type_is_ਮੁੱਲ(typ2) == FAIL)
	    ਵਾਪਸ FAIL;
	ਜੇ (*op == '+')
	    emsg(_(e_wrong_ਦਲੀਲ_type_for_plus));
	ਨਹੀਂਤਾਂ
	    semsg(_(e_char_requires_ਸੰਖਿਆ_or_float_arguments), *op);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    ਸਥਿਰ ਪੂਰਨ
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    ਲਈ (ਪੂਰਨ i = 0; i < tuple_type->tt_argcount; i++)
    {
	ਜੇ (ga_grow(gap, 1) == FAIL)
	    ਵਾਪਸ FAIL;

	((type_T **)gap->ga_ਅੰਕੜੇ)[gap->ga_len] = tuple_type->tt_ਦਲੀਲਾਂ[i];
	gap->ga_len++;
    }

    ਵਾਪਸ OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ਸਥਿਰ ਪੂਰਨ
ਸੈੱਟ_tuple_union_type_on_ਢੇਰ(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, ਆਕਾਰ(type_T *), 10);

    ਜੇ (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    ਜੇ (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    ਜੇ ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    ਸੈੱਟ_type_on_ਢੇਰ(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    ਵਾਪਸ OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    ਪੂਰਨ
generate_ਜੋੜੋ_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  ਵਿਅੰਜਕ_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_ਸੰਖਿਆ ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    ਜੇ (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_ਸੰਖਿਆ_or_float(type1, type2, (char_u *)"+") == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (isn != ਨਿਰਾ)
    {
	ਜੇ (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = ਵਿਅੰਜਕ_type;
	ਨਹੀਂਤਾਂ
	    isn->isn_arg.op.op_type = ਵਿਅੰਜਕ_ਜੋੜੋ;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    ਜੇ (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	ਸੈੱਟ_type_on_ਢੇਰ(cctx, &t_list_any, 0);
    ਨਹੀਂਤਾਂ ਜੇ (vartype == VAR_TUPLE)
    {
	ਜੇ (!check_tuples_addable(type1, type2))
	    ਵਾਪਸ FAIL;

	ਜੇ (ਸੈੱਟ_tuple_union_type_on_ਢੇਰ(type1, type2, cctx) == FAIL)
	    ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ isn == ਨਿਰਾ ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    ਜੇ (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_ਸੰਖਿਆ
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	ਵਾਪਸ type1->tt_type;
    ਵਾਪਸ VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    ਪੂਰਨ
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_ਢੇਰ(cctx, 1);
    type2 = get_type_on_ਢੇਰ(cctx, 0);
    vartype = operator_type(type1, type2);

    ਚੋਣ (*op)
    {
	ਹਾਲਤ '+':
		  ਜੇ (generate_ਜੋੜੋ_instr(cctx, vartype, type1, type2,
							    ਵਿਅੰਜਕ_COPY) == FAIL)
		      ਵਾਪਸ FAIL;
		  ਰੋਕੋ;

	ਹਾਲਤ '-':
	ਹਾਲਤ '*':
	ਹਾਲਤ '/': ਜੇ (check_ਸੰਖਿਆ_or_float(type1, type2, op) == FAIL)
		      ਵਾਪਸ FAIL;
		  ਜੇ (vartype == VAR_ਸੰਖਿਆ)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  ਨਹੀਂਤਾਂ ਜੇ (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  ਨਹੀਂਤਾਂ
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  ਜੇ (isn != ਨਿਰਾ)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? ਵਿਅੰਜਕ_MULT : *op == '/'? ਵਿਅੰਜਕ_DIV : ਵਿਅੰਜਕ_SUB;
		  ਰੋਕੋ;

	ਹਾਲਤ '%': ਜੇ ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_ਸੰਖਿਆ)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_ਸੰਖਿਆ))
		  {
		      emsg(_(e_percent_requires_ਸੰਖਿਆ_arguments));
		      ਵਾਪਸ FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_ਸੰਖਿਆ ? ISN_OPNR : ISN_OPANY, 1);
		  ਜੇ (isn != ਨਿਰਾ)
		      isn->isn_arg.op.op_type = ਵਿਅੰਜਕ_REM;
		  ਰੋਕੋ;
    }

    // correct type of result
    ਜੇ (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	ਜੇ ((type1->tt_type == VAR_ਸੰਖਿਆ || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_ਸੰਖਿਆ || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	ਸੈੱਟ_type_on_ਢੇਰ(cctx, type, 0);
    }

    ਵਾਪਸ OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    ਸਥਿਰ isntype_T
compare_isn_not_ਮੁੱਲਾਂ(typval_T *tv, type_T *type)
{
    ਜੇ (tv != ਨਿਰਾ)
	(ਖਾਲੀ)check_typval_is_ਮੁੱਲ(tv);
    ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)check_type_is_ਮੁੱਲ(type);
    ਵਾਪਸ ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    ਸਥਿਰ isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != ਨਿਰਾ ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != ਨਿਰਾ ? tv2->v_type : type2->tt_type;

    ਜੇ (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	ਵਾਪਸ compare_isn_not_ਮੁੱਲਾਂ(tv1, type1);
    ਜੇ (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	ਵਾਪਸ compare_isn_not_ਮੁੱਲਾਂ(tv2, type2);

    ਜੇ (vartype1 == vartype2)
    {
	ਚੋਣ (vartype1)
	{
	    ਹਾਲਤ VAR_BOOL: isntype = ISN_COMPAREBOOL; ਰੋਕੋ;
	    ਹਾਲਤ VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; ਰੋਕੋ;
	    ਹਾਲਤ VAR_ਸੰਖਿਆ: isntype = ISN_COMPARENR; ਰੋਕੋ;
	    ਹਾਲਤ VAR_FLOAT: isntype = ISN_COMPAREFLOAT; ਰੋਕੋ;
	    ਹਾਲਤ VAR_STRING: isntype = ISN_COMPARESTRING; ਰੋਕੋ;
	    ਹਾਲਤ VAR_BLOB: isntype = ISN_COMPAREBLOB; ਰੋਕੋ;
	    ਹਾਲਤ VAR_LIST: isntype = ISN_COMPARELIST; ਰੋਕੋ;
	    ਹਾਲਤ VAR_TUPLE: isntype = ISN_COMPARETUPLE; ਰੋਕੋ;
	    ਹਾਲਤ VAR_DICT: isntype = ISN_COMPAREDICT; ਰੋਕੋ;
	    ਹਾਲਤ VAR_FUNC: isntype = ISN_COMPAREFUNC; ਰੋਕੋ;
	    ਹਾਲਤ VAR_OBJECT: isntype = ISN_COMPAREOBJECT; ਰੋਕੋ;
	    ਮੂਲ: isntype = ISN_COMPAREANY; ਰੋਕੋ;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_ਸੰਖਿਆ || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_ਸੰਖਿਆ || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    ਨਹੀਂਤਾਂ ਜੇ (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	ਜੇ ((vartype1 == VAR_SPECIAL
		&& (tv1 != ਨਿਰਾ ? tv1->vval.v_ਸੰਖਿਆ == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != ਨਿਰਾ ? tv2->vval.v_ਸੰਖਿਆ == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    ਵਾਪਸ ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    ਜੇ ((exprtype == ਵਿਅੰਜਕ_IS || exprtype == ਵਿਅੰਜਕ_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == ਵਿਅੰਜਕ_IS ? "is" : "isnot" , vartype_name(vartype1));
	ਵਾਪਸ ISN_DROP;
    }
    ਜੇ (!(exprtype == ਵਿਅੰਜਕ_IS || exprtype == ਵਿਅੰਜਕ_ISNOT
		|| exprtype == ਵਿਅੰਜਕ_EQUAL || exprtype == ਵਿਅੰਜਕ_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	ਵਾਪਸ ISN_DROP;
    }
    ਜੇ (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != ਵਿਅੰਜਕ_EQUAL
			&& exprtype != ਵਿਅੰਜਕ_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != ਵਿਅੰਜਕ_EQUAL
			 && exprtype != ਵਿਅੰਜਕ_NEQUAL
			 && exprtype != ਵਿਅੰਜਕ_IS
			 && exprtype != ਵਿਅੰਜਕ_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	ਵਾਪਸ ISN_DROP;
    }
    ਵਾਪਸ isntype;
}

    ਪੂਰਨ
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    ਜੇ (get_compare_isn(type, tv1, tv2, ਨਿਰਾ, ਨਿਰਾ) == ISN_DROP)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    ਪੂਰਨ
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, ਪੂਰਨ ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, ਨਿਰਾ, ਨਿਰਾ,
		       get_type_on_ਢੇਰ(cctx, 1), get_type_on_ਢੇਰ(cctx, 0));
    ਜੇ (isntype == ISN_DROP)
	ਵਾਪਸ FAIL;

    ਜੇ ((isn = generate_instr(cctx, isntype)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --ਢੇਰ->ga_len;
    ਸੈੱਟ_type_on_ਢੇਰ(cctx, &t_bool, 0);

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    ਪੂਰਨ
generate_CONCAT(cctx_T *cctx, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    isn_T	*isn;
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;

    RETURN_OK_IF_SKIP(cctx);

    ਜੇ ((isn = generate_instr(cctx, ISN_CONCAT)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = ਗਿਣਤੀ_2;

    // drop the argument types
    ਢੇਰ->ga_len -= ਗਿਣਤੀ_2 - 1;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    ਪੂਰਨ
generate_2BOOL(cctx_T *cctx, ਪੂਰਨ invert, ਪੂਰਨ ਵਿੱਥ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_2BOOL)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.ਵਿੱਥ = ਵਿੱਥ;

    // type becomes bool
    ਸੈੱਟ_type_on_ਢੇਰ(cctx, &t_bool, -1 - ਵਿੱਥ);

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    ਪੂਰਨ
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ਜੇ (generate_instr(cctx, ISN_COND2BOOL) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // type becomes bool
    ਸੈੱਟ_type_on_ਢੇਰ(cctx, &t_bool, 0);

    ਵਾਪਸ OK;
}

    ਪੂਰਨ
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	ਪੂਰਨ	    typechk_flags,	// type check flags
	ਪੂਰਨ	    ਵਿੱਥ,
	ਪੂਰਨ	    is_var,
	ਪੂਰਨ	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    type_T *tt;
    ਜੇ ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_ਸੰਖਿਆ_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	ਜੇ (tt != ਨਿਰਾ)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    ਜੇ (typechk_flags & TYPECHK_ਸੰਖਿਆ_OK)
		tt->tt_flags |= TTFLAG_ਸੰਖਿਆ_OK;
	    ਨਹੀਂਤਾਂ
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    ਨਹੀਂਤਾਂ
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)ਵਿੱਥ;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    ਸੈੱਟ_type_on_ਢੇਰ(cctx, expected, -1 - ਵਿੱਥ);

    ਵਾਪਸ OK;
}

    ਪੂਰਨ
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_SETTYPE)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    ਪੂਰਨ
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ਜੇ (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    ਸਥਿਰ ਪੂਰਨ
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == ਨਿਰਾ ? &t_any : &class->class_type);
    ਜੇ (isn == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.classarg = class;
    ਜੇ (class != ਨਿਰਾ)
	++class->class_refcount;
    ਵਾਪਸ OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    ਪੂਰਨ
generate_tv_ਧੱਕੋ(cctx_T *cctx, typval_T *tv)
{
    ਚੋਣ (tv->v_type)
    {
	ਹਾਲਤ VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_ਸੰਖਿਆ);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_ਸੰਖਿਆ);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_ਸੰਖਿਆ:
	    generate_PUSHNR(cctx, tv->vval.v_ਸੰਖਿਆ);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST:
	    ਜੇ (tv->vval.v_list != ਨਿਰਾ)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE:
	    ਜੇ (tv->vval.v_tuple != ਨਿਰਾ)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT:
	    ਜੇ (tv->vval.v_dict != ਨਿਰਾ)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	ਹਾਲਤ VAR_ਕੰਮ:
	    ਜੇ (tv->vval.v_ਕੰਮ != ਨਿਰਾ)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CHANNEL:
	    ਜੇ (tv->vval.v_channel != ਨਿਰਾ)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_FUNC:
	    ਜੇ (tv->vval.v_string != ਨਿਰਾ)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, ਨਿਰਾ, &t_func_unknown, TRUE);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_PARTIAL:
	    ਜੇ (tv->vval.v_partial != ਨਿਰਾ)
		iemsg("non-null partial constant not supported");
	    ਜੇ (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == ਨਿਰਾ)
		ਵਾਪਸ FAIL;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    ਜੇ (tv->vval.v_object != ਨਿਰਾ)
	    {
		emsg(_(e_cannot_use_non_null_object));
		ਵਾਪਸ FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    ਰੋਕੋ;
	ਮੂਲ:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    ਵਾਪਸ FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    ਪੂਰਨ
generate_PUSHNR(cctx_T *cctx, varnumber_T ਸੰਖਿਆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_ਸੰਖਿਆ)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = ਸੰਖਿਆ;

    ਜੇ (ਸੰਖਿਆ == 0 || ਸੰਖਿਆ == 1)
	// A 0 or 1 number can also be used as a bool.
	ਸੈੱਟ_type_on_ਢੇਰ(cctx, &t_ਸੰਖਿਆ_bool, 0);
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    ਪੂਰਨ
generate_PUSHBOOL(cctx_T *cctx, varnumber_T ਸੰਖਿਆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = ਸੰਖਿਆ;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    ਪੂਰਨ
generate_PUSHSPEC(cctx_T *cctx, varnumber_T ਸੰਖਿਆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     ਸੰਖਿਆ == VVAL_NULL ? &t_null : &t_none)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = ਸੰਖਿਆ;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    ਪੂਰਨ
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.fnumber = fnumber;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    ਪੂਰਨ
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    ਪੂਰਨ		ret = OK;

    ਜੇ (cctx->ctx_skip != SKIP_YES)
    {
	ਜੇ ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == ਨਿਰਾ)
	    ret = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    isn->isn_arg.string = str == ਨਿਰਾ ? ਨਿਰਾ : *str;
	    ਵਾਪਸ OK;
	}
    }
    ਜੇ (str != ਨਿਰਾ)
	VIM_CLEAR(*str);
    ਵਾਪਸ ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    ਪੂਰਨ
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
#ਨਹੀਂਤਾਂ
    emsg(_(e_channel_ਕੰਮ_feature_not_ਉਪਲਬਧ));
    ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    ਪੂਰਨ
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (generate_instr_type(cctx, ISN_PUSHJOB, &t_ਕੰਮ) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
#ਨਹੀਂਤਾਂ
    emsg(_(e_channel_ਕੰਮ_feature_not_ਉਪਲਬਧ));
    ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    ਪੂਰਨ
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.blob = blob;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    ਪੂਰਨ
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, ਪੂਰਨ may_ਅਗੇਤਰ)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (name == ਨਿਰਾ)
	funcname = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ (!may_ਅਗੇਤਰ
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != ਨਿਰਾ)	    // autoload
	funcname = vim_strsave(name);
    ਨਹੀਂਤਾਂ
    {
	funcname = alloc(STRLEN(name) + 3);
	ਜੇ (funcname != ਨਿਰਾ)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    ਪੂਰਨ
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.string = vim_strsave(name);
    ਜੇ (isn->isn_arg.string == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    ਪੂਰਨ
generate_GETITEM(cctx_T *cctx, ਪੂਰਨ ਸੂਚਕਾਂਕ, ਪੂਰਨ with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_ਢੇਰ(cctx, with_op ? 1 : 0);
    type_T	*ਵਸਤੂ_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    ਵਸਤੂ_type = get_ਵਸਤੂ_type(type);
    ਜੇ ((isn = generate_instr(cctx, ISN_GETITEM)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.getitem.gi_ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    ਵਾਪਸ ਧੱਕੋ_type_ਢੇਰ(cctx, ਵਸਤੂ_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_tuple_ਟੁਕੜਾ_type_on_ਢੇਰ(type_T *type, ਪੂਰਨ ਗਿਣਤੀ_2, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    ਜੇ ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= ਗਿਣਤੀ_2)
    {
	ਸੈੱਟ_type_on_ਢੇਰ(cctx, &t_tuple_any, 0);
	ਵਾਪਸ;
    }

    ga_init2(&tuple_types_ga, ਆਕਾਰ(type_T *), 10);

    ਲਈ (ਪੂਰਨ i = ਗਿਣਤੀ_2; i < type->tt_argcount; i++)
    {
	ਜੇ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    ਸੈੱਟ_type_on_ਢੇਰ(cctx, &t_tuple_any, 0);
	    ਵਾਪਸ;
	}
	((type_T **)tuple_types_ga.ga_ਅੰਕੜੇ)[tuple_types_ga.ga_len] =
							     type->tt_ਦਲੀਲਾਂ[i];
	tuple_types_ga.ga_len++;
    }

    ਸੈੱਟ_type_on_ਢੇਰ(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    ਪੂਰਨ
generate_ਟੁਕੜਾ(cctx_T *cctx, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_ਟੁਕੜਾ)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = ਗਿਣਤੀ_2;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_ਢੇਰ(cctx, 0);
    ਜੇ (type->tt_type == VAR_TUPLE)
	ਸੈੱਟ_tuple_ਟੁਕੜਾ_type_on_ਢੇਰ(type, ਗਿਣਤੀ_2, cctx);

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    ਪੂਰਨ
generate_CHECKLEN(cctx_T *cctx, ਪੂਰਨ min_len, ਪੂਰਨ ਹੋਰ_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    ਜੇ ((isn = generate_instr(cctx, ISN_CHECKLEN)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_ਹੋਰ_OK = ਹੋਰ_OK;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    ਪੂਰਨ
generate_STORE(cctx_T *cctx, isntype_T isn_type, ਪੂਰਨ idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (name != ਨਿਰਾ)
	isn->isn_arg.string = vim_strsave(name);
    ਨਹੀਂਤਾਂ
	isn->isn_arg.ਸੰਖਿਆ = idx;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    ਪੂਰਨ
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	ਪੂਰਨ	    ਲੱਦੋ,
	class_T	    *cl,
	ਪੂਰਨ	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ (ਲੱਦੋ)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_ਲੱਦੋ_CLASSMEMBER, m->ocm_type);
    }
    ਨਹੀਂਤਾਂ
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    ਜੇ (isn == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    ਸਥਿਰ ਪੂਰਨ
generate_STOREOUTER(
	cctx_T	    *cctx,
	ਪੂਰਨ	    idx,
	ਪੂਰਨ	    level,
	ਪੂਰਨ	    loop_ਡੂੰਘਾਈ,
	ਪੂਰਨ	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_ਡੂੰਘਾਈ = -loop_ਡੂੰਘਾਈ - 1;
    }
    ਨਹੀਂਤਾਂ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_ਡੂੰਘਾਈ = level;
    }

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    ਪੂਰਨ
generate_STORENR(cctx_T *cctx, ਪੂਰਨ idx, varnumber_T ਮੁੱਲ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_STORENR)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = ਮੁੱਲ;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    ਸਥਿਰ ਪੂਰਨ
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ਪੂਰਨ	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    ਪੂਰਨ
generate_ਲੱਦੋ(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ਪੂਰਨ	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_type2(cctx, isn_type, type, type)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (name != ਨਿਰਾ)
	isn->isn_arg.string = vim_strsave(name);
    ਨਹੀਂਤਾਂ
	isn->isn_arg.ਸੰਖਿਆ = idx;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    ਪੂਰਨ
generate_LOADOUTER(
	cctx_T	    *cctx,
	ਪੂਰਨ	    idx,
	ਪੂਰਨ	    nesting,
	ਪੂਰਨ	    loop_ਡੂੰਘਾਈ,
	ਪੂਰਨ	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_ਡੂੰਘਾਈ = -loop_ਡੂੰਘਾਈ - 1;
    }
    ਨਹੀਂਤਾਂ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_ਡੂੰਘਾਈ = nesting;
    }

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    ਪੂਰਨ
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    ਪੂਰਨ	    di_flags;
    ਪੂਰਨ	    vidx = ਲੱਭੋ_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ (vidx < 0)
    {
	semsg(_(e_variable_not_ਮਿਲਿਆ_str), name);
	ਵਾਪਸ FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    ਵਾਪਸ generate_ਲੱਦੋ(cctx, ISN_LOADV, vidx, ਨਿਰਾ, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    ਪੂਰਨ
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, ਪੂਰਨ forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, isn_type)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    ਪੂਰਨ
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ਜੇ (generate_instr(cctx, ISN_LOCKCONST) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    ਪੂਰਨ
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ਪੂਰਨ	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    ਨਹੀਂਤਾਂ
	isn = generate_instr_drop(cctx, isn_type, 1);
    ਜੇ (isn == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    ਪੂਰਨ
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ਪੂਰਨ	    sid,
	ਪੂਰਨ	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ਵਸਤੂ(sid);

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    ਨਹੀਂਤਾਂ
	isn = generate_instr_drop(cctx, isn_type, 1);
    ਜੇ (isn == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    ਜੇ (sref == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    ਪੂਰਨ
generate_NEWLIST(cctx_T *cctx, ਪੂਰਨ ਗਿਣਤੀ_2, ਪੂਰਨ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_NEWLIST)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = use_null ? -1 : ਗਿਣਤੀ_2;

    // Get the member type and the declared member type from all the items on
    // the stack.
    ਜੇ ((member_type = get_member_type_from_ਢੇਰ(ਗਿਣਤੀ_2, 1, cctx)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_ਢੇਰ.ga_len -= ਗਿਣਤੀ_2;

    // add the list type to the type stack
    ਵਾਪਸ ਧੱਕੋ_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    ਪੂਰਨ
generate_NEWTUPLE(cctx_T *cctx, ਪੂਰਨ ਗਿਣਤੀ_2, ਪੂਰਨ use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = use_null ? -1 : ਗਿਣਤੀ_2;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, ਆਕਾਰ(type_T *), 10);

    ਜੇ (get_tuple_type_from_ਢੇਰ(ਗਿਣਤੀ_2, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	ਵਾਪਸ FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_ਢੇਰ.ga_len -= ਗਿਣਤੀ_2;

    // add the tuple type to the type stack
    ਵਾਪਸ ਧੱਕੋ_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    ਪੂਰਨ
generate_NEWDICT(cctx_T *cctx, ਪੂਰਨ ਗਿਣਤੀ_2, ਪੂਰਨ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_NEWDICT)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = use_null ? -1 : ਗਿਣਤੀ_2;

    ਜੇ ((member_type = get_member_type_from_ਢੇਰ(ਗਿਣਤੀ_2, 2, cctx)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_ਢੇਰ.ga_len -= 2 * ਗਿਣਤੀ_2;

    // add the dict type to the type stack
    ਵਾਪਸ ਧੱਕੋ_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    ਪੂਰਨ
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ਪੂਰਨ	    object_ਵਿਧੀ,
	ਪੂਰਨ	    fi,
	ਪੂਰਨ	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    ਪੂਰਨ		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_FUNCREF)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (isn_idx != ਨਿਰਾ)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_NOT_COMPILED || has_vars || cl != ਨਿਰਾ)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	ਜੇ (extra == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	ਜੇ (cl != ਨਿਰਾ)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_ਵਿਧੀ = object_ਵਿਧੀ;
	    extra->fre_ਵਿਧੀ_idx = fi;
	}
    }
    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_NOT_COMPILED || cl != ਨਿਰਾ)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ != UF_NOT_COMPILED && cl == ਨਿਰਾ)
    {
	ਜੇ (isn_idx == ਨਿਰਾ && ufunc->uf_def_ਸਥਿਤੀ == UF_ਮੰਜ਼ਿਲ_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (ਖਾਲੀ)compile_def_function(ufunc, FALSE, CT_NONE, ਨਿਰਾ);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_ਘੇਰਾ = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    ਜੇ ((ufunc->uf_flags & FC_ਘੇਰਾ)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_ਘੇਰਾ;

    type = ufunc->uf_func_type == ਨਿਰਾ ? &t_func_any : ufunc->uf_func_type;
    ਵਾਪਸ ਧੱਕੋ_type_ਢੇਰ(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    ਪੂਰਨ
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    ਪੂਰਨ		ret = OK;

    ਜੇ (cctx->ctx_skip != SKIP_YES)
    {
	ਜੇ ((isn = generate_instr(cctx, ISN_NEWFUNC)) == ਨਿਰਾ)
	    ret = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    ਜੇ (arg == ਨਿਰਾ)
		ret = FAIL;
	    ਨਹੀਂਤਾਂ
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_ਘੇਰਾ = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(ਖਾਲੀ)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		ਵਾਪਸ OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    ਵਾਪਸ ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    ਪੂਰਨ
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_DEF)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	ਜੇ (isn->isn_arg.string == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    ਪੂਰਨ
generate_JUMP(cctx_T *cctx, jumpwhen_T when, ਪੂਰਨ where)
{
    isn_T	*isn;
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_JUMP)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    ਜੇ (when != JUMP_ALWAYS && ਢੇਰ->ga_len > 0)
	--ਢੇਰ->ga_len;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    ਪੂਰਨ
generate_WHILE(cctx_T *cctx, ਪੂਰਨ funcref_idx)
{
    isn_T	*isn;
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_WHILE)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_ਸਿਰਾ = 0;  // filled in later

    ਜੇ (ਢੇਰ->ga_len > 0)
	--ਢੇਰ->ga_len;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    ਪੂਰਨ
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, ਪੂਰਨ arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, isn_type)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    ਵਾਪਸ OK;
}

    ਪੂਰਨ
generate_FOR(cctx_T *cctx, ਪੂਰਨ loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_FOR)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    ਵਾਪਸ ਧੱਕੋ_type_ਢੇਰ(cctx, &t_any);
}

    ਪੂਰਨ
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_ENDLOOP)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.endloop.ਸਿਰਾ_ਡੂੰਘਾਈ = loop_info->li_ਡੂੰਘਾਈ;
    isn->isn_arg.endloop.ਸਿਰਾ_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.ਸਿਰਾ_var_idx = loop_info->li_local_ਗਿਣਤੀ;
    isn->isn_arg.endloop.ਸਿਰਾ_var_ਗਿਣਤੀ =
			   cctx->ctx_locals.ga_len - loop_info->li_local_ਗਿਣਤੀ;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    ਪੂਰਨ
generate_TRYCONT(cctx_T *cctx, ਪੂਰਨ ਪੱਧਰ, ਪੂਰਨ where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_TRYCONT)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.trycont.tct_ਪੱਧਰ = ਪੱਧਰ;
    isn->isn_arg.trycont.tct_where = where;

    ਵਾਪਸ OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    ਪੂਰਨ
check_ਅੰਦਰੂਨੀ_func_ਦਲੀਲਾਂ(
	cctx_T	*cctx,
	ਪੂਰਨ	func_idx,
	ਪੂਰਨ	argcount,
	ਪੂਰਨ	ਵਿਧੀ_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;
    ਪੂਰਨ		argoff = check_ਅੰਦਰੂਨੀ_func(func_idx, argcount);

    ਜੇ (argoff < 0)
	ਵਾਪਸ FAIL;

    ਜੇ (ਵਿਧੀ_call && argoff > 1)
    {
	ਜੇ (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 ਅੰਦਰੂਨੀ_func_name(func_idx));
	    ਵਾਪਸ FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	ਜੇ (isn  == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	isn->isn_arg.shuffle.shfl_ਵਸਤੂ = argcount;
	isn->isn_arg.shuffle.shfl_ਉੱਪਰ = argoff - 1;
    }

    ਜੇ (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)ਢੇਰ->ga_ਅੰਕੜੇ) + ਢੇਰ->ga_len - argcount;

	// Check the types of the arguments.
	ਜੇ (ਵਿਧੀ_call && argoff > 1)
	{
	    ਪੂਰਨ i;

	    ਲਈ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ i;

	    ਲਈ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ਜੇ (ਅੰਦਰੂਨੀ_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    ਪੂਰਨ
generate_BCALL(cctx_T *cctx, ਪੂਰਨ func_idx, ਪੂਰਨ argcount, ਪੂਰਨ ਵਿਧੀ_call)
{
    isn_T	*isn;
    garray_T	*ਢੇਰ = &cctx->ctx_type_ਢੇਰ;
    type2_T	*argtypes = ਨਿਰਾ;
    type2_T	shuffled_argtypes[MAX_FUNC_ਦਲੀਲਾਂ];
    type2_T	*maptype = ਨਿਰਾ;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    ਜੇ (check_ਅੰਦਰੂਨੀ_func_ਦਲੀਲਾਂ(cctx, func_idx, argcount, ਵਿਧੀ_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (ਅੰਦਰੂਨੀ_func_is_map(func_idx))
	maptype = argtypes;

    ਜੇ ((isn = generate_instr(cctx, ISN_BCALL)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    ਢੇਰ->ga_len -= argcount;
    type = ਅੰਦਰੂਨੀ_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    ਜੇ (ਧੱਕੋ_type_stack2(cctx, type, decl_type) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (maptype != ਨਿਰਾ && maptype[0].type_decl->tt_member != ਨਿਰਾ
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ਪੂਰਨ
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*ਵਸਤੂ_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_ਢੇਰ(cctx, 1);
    ਜੇ (arg_type_modifiable(list_type, 1) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਸਤੂ_type = get_type_on_ਢੇਰ(cctx, 0);
    expected = list_type->tt_member;
    ਜੇ (need_type(ਵਸਤੂ_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (generate_instr(cctx, ISN_LISTAPPEND) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    --cctx->ctx_type_ਢੇਰ.ga_len;	    // drop the argument
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ਪੂਰਨ
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*ਵਸਤੂ_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    ਜੇ (arg_type_modifiable(get_decl_type_on_ਢੇਰ(cctx, 1), 1) == FAIL)
	ਵਾਪਸ FAIL;
    ਵਸਤੂ_type = get_type_on_ਢੇਰ(cctx, 0);
    ਜੇ (need_type(ਵਸਤੂ_type, &t_ਸੰਖਿਆ, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ਵਾਪਸ FAIL;

    ਜੇ (generate_instr(cctx, ISN_BLOBAPPEND) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    --cctx->ctx_type_ਢੇਰ.ga_len;	    // drop the argument
    ਵਾਪਸ OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    ਸਥਿਰ isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    ਜੇ (cl != ਨਿਰਾ)
	ਵਾਪਸ ISN_METHODCALL;

    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_NOT_COMPILED)
	ਵਾਪਸ ISN_UCALL;

    // function invoked from a lambda
    ਜੇ (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	ਜੇ (ufunc->uf_name[0] == K_SPECIAL)
	    ਵਾਪਸ ISN_UCALL;
    }

    ਵਾਪਸ ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    ਪੂਰਨ
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ਪੂਰਨ	    mi,
	ਪੂਰਨ	    pushed_argcount,
	ਪੂਰਨ	    is_super)
{
    isn_T	*isn;
    ਪੂਰਨ		regular_ਦਲੀਲਾਂ = ufunc->uf_ਦਲੀਲਾਂ.ga_len;
    ਪੂਰਨ		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ (argcount > regular_ਦਲੀਲਾਂ && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (argcount < regular_ਦਲੀਲਾਂ - ufunc->uf_def_ਦਲੀਲਾਂ.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	ਵਾਪਸ FAIL;
    }

    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ != UF_NOT_COMPILED
	    && ufunc->uf_def_ਸਥਿਤੀ != UF_COMPILE_ERROR)
    {
	ਪੂਰਨ		i;
	compiletype_T	compile_type;

	ਲਈ (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_ਢੇਰ(cctx, argcount - i - 1);
	    ਜੇ (check_type_is_ਮੁੱਲ(actual) == FAIL)
		ਵਾਪਸ FAIL;
	    ਜੇ (actual->tt_type == VAR_SPECIAL
			      && i >= regular_ਦਲੀਲਾਂ - ufunc->uf_def_ਦਲੀਲਾਂ.ga_len)
	    {
		// assume v:none used for default argument value
		ਜਾਰੀ;
	    }
	    ਜੇ (i < regular_ਦਲੀਲਾਂ)
	    {
		ਜੇ (ufunc->uf_arg_types == ਨਿਰਾ)
		    ਜਾਰੀ;
		expected = ufunc->uf_arg_types[i];
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (ufunc->uf_va_type == ਨਿਰਾ
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    ਨਹੀਂਤਾਂ
		expected = ufunc->uf_va_type->tt_member;
	    ਜੇ (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		ਵਾਪਸ FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	ਜੇ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == ਨਿਰਾ,
						   compile_type, ਨਿਰਾ) == FAIL)
	    ਵਾਪਸ FAIL;
    }
    ਜੇ (ufunc->uf_def_ਸਥਿਤੀ == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_ਮੰਜ਼ਿਲ_function_that_failed_ਮੰਜ਼ਿਲ_compile_str,
							       ufunc->uf_name);
	ਵਾਪਸ FAIL;
    }

    ਜੇ ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਜੇ (cl != ਨਿਰਾ /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	ਜੇ (isn->isn_arg.mfunc == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = ਨਿਰਾ;
	ਜੇ (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = ਨਿਰਾ;
	isn->isn_arg.mfunc->cmf_cache_generic = ਨਿਰਾ;
	isn->isn_arg.mfunc->cmf_cache_ufunc = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    ਨਹੀਂਤਾਂ
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_ਢੇਰ.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    ਜੇ (ufunc->uf_class != ਨਿਰਾ)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_ਢੇਰ(cctx, 0);
	ਜੇ (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_ਢੇਰ.ga_len--;
    }

    // add return type
    ਵਾਪਸ ਧੱਕੋ_type_ਢੇਰ(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    ਪੂਰਨ
generate_UCALL(cctx_T *cctx, char_u *name, ਪੂਰਨ argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_UCALL)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_ਢੇਰ.ga_len -= argcount;

    // add return value
    ਵਾਪਸ ਧੱਕੋ_type_ਢੇਰ(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    ਪੂਰਨ
check_func_ਦਲੀਲਾਂ_from_type(
	cctx_T	*cctx,
	type_T	*type,
	ਪੂਰਨ	argcount,
	ਪੂਰਨ	at_ਸਿਖਰ,
	char_u	*name)
{
    ਜੇ (type->tt_argcount != -1)
    {
	ਪੂਰਨ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	ਜੇ (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (type->tt_ਦਲੀਲਾਂ != ਨਿਰਾ)
	{
	    ਪੂਰਨ i;

	    ਲਈ (i = 0; i < argcount; ++i)
	    {
		ਪੂਰਨ	ਵਿੱਥ = -argcount + i - (at_ਸਿਖਰ ? 0 : 1);
		type_T	*actual = get_type_on_ਢੇਰ(cctx, -1 - ਵਿੱਥ);
		type_T	*expected;

		ਜੇ (check_type_is_ਮੁੱਲ(actual) == FAIL)
		    ਵਾਪਸ FAIL;
		ਜੇ (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_ਦਲੀਲਾਂ[type->tt_argcount - 1];
		    ਜੇ (expected != ਨਿਰਾ && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    ਜੇ (expected == ਨਿਰਾ)
			expected = &t_any;
		}
		ਨਹੀਂਤਾਂ ਜੇ (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		ਨਹੀਂਤਾਂ
		    expected = type->tt_ਦਲੀਲਾਂ[i];
		ਜੇ (need_type(actual, expected, 0,
				     ਵਿੱਥ, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    ਵਾਪਸ FAIL;
		}
	    }
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    ਪੂਰਨ
generate_PCALL(
	cctx_T	*cctx,
	ਪੂਰਨ	argcount,
	char_u	*name,
	type_T	*type,
	ਪੂਰਨ	at_ਸਿਖਰ)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    ਜੇ (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    ਨਹੀਂਤਾਂ ਜੇ (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	ਜੇ (check_func_ਦਲੀਲਾਂ_from_type(cctx, type, argcount, at_ਸਿਖਰ, name)
								       == FAIL)
	    ਵਾਪਸ FAIL;

	ret_type = type->tt_member;
	ਜੇ (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    ਨਹੀਂਤਾਂ
    {
	semsg(_(e_not_callable_type_str), name);
	ਵਾਪਸ FAIL;
    }

    ਜੇ ((isn = generate_instr(cctx, ISN_PCALL)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.pfunc.cpf_ਸਿਖਰ = at_ਸਿਖਰ;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_ਢੇਰ.ga_len -= argcount + 1;

    // push the return value
    ਧੱਕੋ_type_ਢੇਰ(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    ਜੇ (at_ਸਿਖਰ && generate_instr(cctx, ISN_PCALL_ਸਿਰਾ) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    ਪੂਰਨ
generate_DEFER(cctx_T *cctx, ਪੂਰਨ var_idx, ਪੂਰਨ argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    ਪੂਰਨ
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_ਢੇਰ(cctx, 0);
    ਜੇ (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	ਅੱਖਰ *tofree;

	semsg(_(e_expected_dictionary_for_using_ਕੁੰਜੀ_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	ਵਾਪਸ FAIL;
    }
    // change dict type to dict member type
    ਜੇ (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	ਸੈੱਟ_type_on_ਢੇਰ(cctx, ntype, 0);
    }

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    ਪੂਰਨ
generate_ECHO(cctx_T *cctx, ਪੂਰਨ with_white, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_drop(cctx, ISN_ECHO, ਗਿਣਤੀ_2)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_ਗਿਣਤੀ = ਗਿਣਤੀ_2;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    ਪੂਰਨ
generate_MULT_ਵਿਅੰਜਕ(cctx_T *cctx, isntype_T isn_type, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr_drop(cctx, isn_type, ਗਿਣਤੀ_2)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = ਗਿਣਤੀ_2;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    ਪੂਰਨ
generate_ECHOWINDOW(cctx_T *cctx, ਪੂਰਨ ਗਿਣਤੀ_2, ਲੰਮਾ ਸਮਾਂ)
{
    isn_T	*isn;

    ਜੇ ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, ਗਿਣਤੀ_2)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.echowin.ewin_ਗਿਣਤੀ = ਗਿਣਤੀ_2;
    isn->isn_arg.echowin.ewin_ਸਮਾਂ = ਸਮਾਂ;
    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    ਪੂਰਨ
generate_ਸਰੋਤ(cctx_T *cctx, ਪੂਰਨ sid)
{
    isn_T	*isn;

    ਜੇ ((isn = generate_instr(cctx, ISN_ਸਰੋਤ)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = sid;

    ਵਾਪਸ OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    ਪੂਰਨ
generate_PUT(cctx_T *cctx, ਪੂਰਨ regname, linenr_T lnum, ਪੂਰਨ fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    ਜੇ (isn == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    ਵਾਪਸ OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    ਪੂਰਨ
generate_LOCKUNLOCK(cctx_T *cctx, char_u *ਸਤਰ, ਪੂਰਨ is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    class_T *cl = cctx->ctx_ufunc != ਨਿਰਾ ? cctx->ctx_ufunc->uf_class : ਨਿਰਾ;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(ਸਤਰ);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    ਜੇ (cl != ਨਿਰਾ)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    ਵਾਪਸ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    ਪੂਰਨ
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *ਸਤਰ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, isntype)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.string = vim_strsave(ਸਤਰ);
    ਵਾਪਸ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    ਪੂਰਨ
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    ਪੂਰਨ		ret = OK;

    ਜੇ (cctx->ctx_skip != SKIP_YES)
    {
	ਜੇ ((isn = generate_instr(cctx, isntype)) == ਨਿਰਾ)
	    ret = FAIL;
	ਨਹੀਂਤਾਂ
	{
	    isn->isn_arg.string = str;
	    ਵਾਪਸ OK;
	}
    }
    vim_free(str);
    ਵਾਪਸ ret;
}

    ਪੂਰਨ
generate_LEGACY_EVAL(cctx_T *cctx, char_u *ਸਤਰ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.string = vim_strsave(ਸਤਰ);

    ਵਾਪਸ ਧੱਕੋ_type_ਢੇਰ(cctx, &t_any);
}

    ਪੂਰਨ
generate_EXECCONCAT(cctx_T *cctx, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    isn_T	*isn;

    ਜੇ ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, ਗਿਣਤੀ_2)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.ਸੰਖਿਆ = ਗਿਣਤੀ_2;
    ਵਾਪਸ OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    ਪੂਰਨ
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    ਜੇ ((isn = generate_instr(cctx, ISN_RANGE)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.string = range;

    ਵਾਪਸ ਧੱਕੋ_type_ਢੇਰ(cctx, &t_ਸੰਖਿਆ);
}

    ਪੂਰਨ
generate_UNPACK(cctx_T *cctx, ਪੂਰਨ var_ਗਿਣਤੀ, ਪੂਰਨ semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_UNPACK)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.unpack.unp_ਗਿਣਤੀ = var_ਗਿਣਤੀ;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    ਵਾਪਸ OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    ਪੂਰਨ
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    ਜੇ (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	ਜੇ ((isn = generate_instr(cctx, ISN_CMDMOD)) == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	ਜੇ (isn->isn_arg.cmdmod.cf_cmdmod == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, ਆਕਾਰ(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = ਨਿਰਾ;
    }

    ਵਾਪਸ OK;
}

    ਪੂਰਨ
generate_undo_cmdmods(cctx_T *cctx)
{
    ਜੇ (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    ਵਾਪਸ OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    ਪੂਰਨ
generate_store_var(
	cctx_T		*cctx,
	ਸੌਂਪੋ_dest_T	dest,
	ਪੂਰਨ		opt_flags,
	ਪੂਰਨ		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    ਚੋਣ (dest)
    {
	ਹਾਲਤ dest_option:
	    ਵਾਪਸ generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	ਹਾਲਤ dest_func_option:
	    ਵਾਪਸ generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	ਹਾਲਤ dest_global:
	    // include g: with the name, easier to execute that way
	    ਵਾਪਸ generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == ਨਿਰਾ
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	ਹਾਲਤ dest_ਬਫਰ:
	    // include b: with the name, easier to execute that way
	    ਵਾਪਸ generate_STORE(cctx, ISN_STOREB, 0, name);
	ਹਾਲਤ dest_ਖਿੜਕੀ:
	    // include w: with the name, easier to execute that way
	    ਵਾਪਸ generate_STORE(cctx, ISN_STOREW, 0, name);
	ਹਾਲਤ dest_tab:
	    // include t: with the name, easier to execute that way
	    ਵਾਪਸ generate_STORE(cctx, ISN_STORET, 0, name);
	ਹਾਲਤ dest_env:
	    ਵਾਪਸ generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	ਹਾਲਤ dest_reg:
	    ਵਾਪਸ generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], ਨਿਰਾ);
	ਹਾਲਤ dest_vimvar:
	    ਵਾਪਸ generate_STORE(cctx, ISN_STOREV, vimvaridx, ਨਿਰਾ);
	ਹਾਲਤ dest_script:
	ਹਾਲਤ dest_script_v9:
	    {
		ਪੂਰਨ	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		ਪੂਰਨ	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		ਜੇ (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    ਜੇ (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ਵਸਤੂ(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ਵਸਤੂ(scriptvar_sid)
						  ->sn_autoload_ਅਗੇਤਰ == ਨਿਰਾ)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			//ਪਹਿਲਾ
			ਜੇ (generate_ਸਰੋਤ(cctx, scriptvar_sid) == FAIL)
			    ਵਾਪਸ FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    ਵਾਪਸ generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		ਵਾਪਸ generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	ਹਾਲਤ dest_class_member:
	    ਵਾਪਸ generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	ਹਾਲਤ dest_local:
	ਹਾਲਤ dest_ਵਿਅੰਜਕ:
	    // cannot happen
	    ਰੋਕੋ;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    ਪੂਰਨ
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    ਲਈ (;;)
    {
	ਜੇ (scope == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    ਵਾਪਸ TRUE;
	scope = scope->se_outer;
    }
    ਵਾਪਸ FALSE;
}

    ਪੂਰਨ
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, ਪੂਰਨ instr_ਗਿਣਤੀ, ਪੂਰਨ is_decl)
{
    ਜੇ (lhs->lhs_dest != dest_local)
	ਵਾਪਸ generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    ਜੇ (lhs->lhs_lvar == ਨਿਰਾ)
	ਵਾਪਸ OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_ਅੰਕੜੇ) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    ਜੇ (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_ਗਿਣਤੀ + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.ਸੰਖਿਆ;
	garray_T    *ਢੇਰ = &cctx->ctx_type_ਢੇਰ;

	ਜੇ (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	ਨਹੀਂਤਾਂ
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	ਜੇ (ਢੇਰ->ga_len > 0)
	    --ਢੇਰ->ga_len;
    }
    ਨਹੀਂਤਾਂ ਜੇ (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_ਡੂੰਘਾਈ,
		lhs->lhs_lvar->lv_loop_idx);
    ਨਹੀਂਤਾਂ
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, ਨਿਰਾ);
    ਵਾਪਸ OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    ਪੂਰਨ
generate_SCRIPTCTX_ਸੈੱਟ(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ਜੇ ((isn = generate_instr(cctx, ISN_SCRIPTCTX_ਸੈੱਟ)) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    isn->isn_arg.setsctx = new_sctx;
    ਵਾਪਸ OK;
}

#ਜੇ defined(FEAT_PROFILE)
    ਖਾਲੀ
may_generate_prof_ਸਿਰਾ(cctx_T *cctx, ਪੂਰਨ prof_lnum)
{
    ਜੇ (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_ਸਿਰਾ);
}
#ਜੇ_ਅੰਤ


/*
 * Delete an instruction, free what it contains.
 */
    ਖਾਲੀ
delete_instr(isn_T *isn)
{
    ਚੋਣ (isn->isn_type)
    {
	ਹਾਲਤ ISN_AUTOLOAD:
	ਹਾਲਤ ISN_DEF:
	ਹਾਲਤ ISN_EXEC:
	ਹਾਲਤ ISN_EXECRANGE:
	ਹਾਲਤ ISN_EXEC_ਵੰਡੋ:
	ਹਾਲਤ ISN_LEGACY_EVAL:
	ਹਾਲਤ ISN_LOADAUTO:
	ਹਾਲਤ ISN_LOADB:
	ਹਾਲਤ ISN_LOADENV:
	ਹਾਲਤ ISN_LOADG:
	ਹਾਲਤ ISN_LOADOPT:
	ਹਾਲਤ ISN_LOADT:
	ਹਾਲਤ ISN_LOADW:
	ਹਾਲਤ ISN_PUSHEXC:
	ਹਾਲਤ ISN_PUSHFUNC:
	ਹਾਲਤ ISN_PUSHS:
	ਹਾਲਤ ISN_RANGE:
	ਹਾਲਤ ISN_STOREAUTO:
	ਹਾਲਤ ISN_STOREB:
	ਹਾਲਤ ISN_STOREENV:
	ਹਾਲਤ ISN_STOREG:
	ਹਾਲਤ ISN_STORET:
	ਹਾਲਤ ISN_STOREW:
	ਹਾਲਤ ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_SUBSTITUTE:
	    {
		ਪੂਰਨ	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		ਲਈ (idx = 0; list[idx].isn_type != ISN_ਅੰਤ; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_INSTR:
	    {
		ਪੂਰਨ	idx;
		isn_T	*list = isn->isn_arg.instr;

		ਲਈ (idx = 0; list[idx].isn_type != ISN_ਅੰਤ; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_LOADS:
	ਹਾਲਤ ISN_LOADEXPORT:
	ਹਾਲਤ ISN_STORES:
	ਹਾਲਤ ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_UNLET:
	ਹਾਲਤ ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_STOREOPT:
	ਹਾਲਤ ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		ਜੇ (extra == ਨਿਰਾ || extra->fre_func_name == ਨਿਰਾ)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    ਜੇ (ufunc != ਨਿਰਾ && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		ਜੇ (extra != ਨਿਰਾ)
		{
		    char_u *name = extra->fre_func_name;

		    ਜੇ (name != ਨਿਰਾ)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    ਜੇ (extra->fre_class != ਨਿਰਾ)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ਅੰਕੜੇ)
		    + isn->isn_arg.dfunc.cdf_idx;

		ਜੇ (dfunc->df_ufunc != ਨਿਰਾ
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		ਜੇ (arg != ਨਿਰਾ)
		{
		    ufunc_T *ufunc = ਲੱਭੋ_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    ਜੇ (ufunc != ਨਿਰਾ)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_CHECKTYPE:
	ਹਾਲਤ ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_LOADSCRIPT:
	ਹਾਲਤ ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_ਲੱਦੋ_CLASSMEMBER:
	ਹਾਲਤ ISN_STORE_CLASSMEMBER:
	ਹਾਲਤ ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    ਰੋਕੋ;

	ਹਾਲਤ ISN_2BOOL:
	ਹਾਲਤ ISN_2STRING:
	ਹਾਲਤ ISN_2STRING_ANY:
	ਹਾਲਤ ISN_ADDBLOB:
	ਹਾਲਤ ISN_ADDLIST:
	ਹਾਲਤ ISN_ADDTUPLE:
	ਹਾਲਤ ISN_ANYINDEX:
	ਹਾਲਤ ISN_ANYSLICE:
	ਹਾਲਤ ISN_BCALL:
	ਹਾਲਤ ISN_BLOBAPPEND:
	ਹਾਲਤ ISN_BLOBINDEX:
	ਹਾਲਤ ISN_BLOBSLICE:
	ਹਾਲਤ ISN_CATCH:
	ਹਾਲਤ ISN_CEXPR_AUCMD:
	ਹਾਲਤ ISN_CHECKLEN:
	ਹਾਲਤ ISN_CLEARDICT:
	ਹਾਲਤ ISN_CMDMOD_REV:
	ਹਾਲਤ ISN_COMPAREANY:
	ਹਾਲਤ ISN_COMPAREBLOB:
	ਹਾਲਤ ISN_COMPAREBOOL:
	ਹਾਲਤ ISN_COMPAREDICT:
	ਹਾਲਤ ISN_COMPAREFLOAT:
	ਹਾਲਤ ISN_COMPAREFUNC:
	ਹਾਲਤ ISN_COMPARELIST:
	ਹਾਲਤ ISN_COMPARETUPLE:
	ਹਾਲਤ ISN_COMPARENR:
	ਹਾਲਤ ISN_COMPARENULL:
	ਹਾਲਤ ISN_COMPAREOBJECT:
	ਹਾਲਤ ISN_COMPARESPECIAL:
	ਹਾਲਤ ISN_COMPARESTRING:
	ਹਾਲਤ ISN_CONCAT:
	ਹਾਲਤ ISN_CONSTRUCT:
	ਹਾਲਤ ISN_COND2BOOL:
	ਹਾਲਤ ISN_DEBUG:
	ਹਾਲਤ ISN_DEFER:
	ਹਾਲਤ ISN_DROP:
	ਹਾਲਤ ISN_ECHO:
	ਹਾਲਤ ISN_ECHOCONSOLE:
	ਹਾਲਤ ISN_ECHOERR:
	ਹਾਲਤ ISN_ECHOMSG:
	ਹਾਲਤ ISN_ECHOWINDOW:
	ਹਾਲਤ ISN_ENDLOOP:
	ਹਾਲਤ ISN_ENDTRY:
	ਹਾਲਤ ISN_EXECCONCAT:
	ਹਾਲਤ ISN_EXECUTE:
	ਹਾਲਤ ISN_FINALLY:
	ਹਾਲਤ ISN_ਅੰਤ:
	ਹਾਲਤ ISN_FOR:
	ਹਾਲਤ ISN_GETITEM:
	ਹਾਲਤ ISN_GET_OBJ_MEMBER:
	ਹਾਲਤ ISN_JUMP:
	ਹਾਲਤ ISN_JUMP_IF_ARG_NOT_ਸੈੱਟ:
	ਹਾਲਤ ISN_JUMP_IF_ARG_ਸੈੱਟ:
	ਹਾਲਤ ISN_LISTAPPEND:
	ਹਾਲਤ ISN_LISTINDEX:
	ਹਾਲਤ ISN_LISTSLICE:
	ਹਾਲਤ ISN_TUPLEINDEX:
	ਹਾਲਤ ISN_TUPLESLICE:
	ਹਾਲਤ ISN_ਲੱਦੋ:
	ਹਾਲਤ ISN_LOADBDICT:
	ਹਾਲਤ ISN_LOADGDICT:
	ਹਾਲਤ ISN_LOADOUTER:
	ਹਾਲਤ ISN_LOADREG:
	ਹਾਲਤ ISN_LOADTDICT:
	ਹਾਲਤ ISN_LOADV:
	ਹਾਲਤ ISN_LOADWDICT:
	ਹਾਲਤ ISN_LOCKCONST:
	ਹਾਲਤ ISN_MEMBER:
	ਹਾਲਤ ISN_NEGATENR:
	ਹਾਲਤ ISN_NEWDICT:
	ਹਾਲਤ ISN_NEWLIST:
	ਹਾਲਤ ISN_NEWTUPLE:
	ਹਾਲਤ ISN_NEWPARTIAL:
	ਹਾਲਤ ISN_OPANY:
	ਹਾਲਤ ISN_OPFLOAT:
	ਹਾਲਤ ISN_OPNR:
	ਹਾਲਤ ISN_PCALL:
	ਹਾਲਤ ISN_PCALL_ਸਿਰਾ:
	ਹਾਲਤ ISN_PROF_ਸਿਰਾ:
	ਹਾਲਤ ISN_PROF_ਸ਼ੁਰੂ:
	ਹਾਲਤ ISN_PUSHBOOL:
	ਹਾਲਤ ISN_PUSHCHANNEL:
	ਹਾਲਤ ISN_PUSHF:
	ਹਾਲਤ ISN_PUSHJOB:
	ਹਾਲਤ ISN_PUSHNR:
	ਹਾਲਤ ISN_PUSHOBJ:
	ਹਾਲਤ ISN_PUSHSPEC:
	ਹਾਲਤ ISN_PUT:
	ਹਾਲਤ ISN_IPUT:
	ਹਾਲਤ ISN_REDIREND:
	ਹਾਲਤ ISN_REDIRSTART:
	ਹਾਲਤ ISN_RETURN:
	ਹਾਲਤ ISN_RETURN_OBJECT:
	ਹਾਲਤ ISN_RETURN_VOID:
	ਹਾਲਤ ISN_SHUFFLE:
	ਹਾਲਤ ISN_ਟੁਕੜਾ:
	ਹਾਲਤ ISN_ਸਰੋਤ:
	ਹਾਲਤ ISN_STORE:
	ਹਾਲਤ ISN_STORENR:
	ਹਾਲਤ ISN_STOREOUTER:
	ਹਾਲਤ ISN_STORE_THIS:
	ਹਾਲਤ ISN_STORERANGE:
	ਹਾਲਤ ISN_STOREREG:
	ਹਾਲਤ ISN_STOREV:
	ਹਾਲਤ ISN_STRINDEX:
	ਹਾਲਤ ISN_STRSLICE:
	ਹਾਲਤ ISN_THROW:
	ਹਾਲਤ ISN_TRYCONT:
	ਹਾਲਤ ISN_UNLETINDEX:
	ਹਾਲਤ ISN_UNLETRANGE:
	ਹਾਲਤ ISN_UNPACK:
	ਹਾਲਤ ISN_USEDICT:
	ਹਾਲਤ ISN_WHILE:
	ਹਾਲਤ ISN_SCRIPTCTX_ਸੈੱਟ:
	// nothing allocated
	ਰੋਕੋ;
    }
}

    ਖਾਲੀ
clear_instr_ga(garray_T *gap)
{
    ਪੂਰਨ idx;

    ਲਈ (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_ਅੰਕੜੇ) + idx);
    ga_clear(gap);
}


#ਜੇ_ਅੰਤ  // defined(FEAT_EVAL)
