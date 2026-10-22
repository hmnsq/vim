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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) எனில் (cctx->ctx_skip == SKIP_YES) திரும்பு வெற்று
#define RETURN_OK_IF_SKIP(cctx) எனில் (cctx->ctx_skip == SKIP_YES) திரும்பு OK

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
    எனில் (GA_GROW_FAILS(instr, 1))
	திரும்பு வெற்று;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    திரும்பு isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, முழு drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_அடுக்கு.ga_len -= drop;
    திரும்பு generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    நிலையான isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    எனில் ((isn = generate_instr(cctx, isn_type)) == வெற்று)
	திரும்பு வெற்று;

    எனில் (தள்ளு_type_stack2(cctx, type == வெற்று ? &t_any : type,
			       decl_type == வெற்று ? &t_any : decl_type) == FAIL)
	திரும்பு வெற்று;

    திரும்பு isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    திரும்பு generate_instr_type2(cctx, isn_type, type, &t_any);
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

    எனில் ((isn = generate_instr(cctx, ISN_DEBUG)) == வெற்று)
	திரும்பு வெற்று;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    திரும்பு isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    முழு
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.construct.construct_size = அளவு(object_T)
			       + cl->class_obj_member_மொத்தம் * அளவு(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    திரும்பு OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    முழு
generate_GET_OBJ_MEMBER(cctx_T *cctx, முழு idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    எனில் (isn == வெற்று)
	திரும்பு FAIL;

    isn->isn_arg.classmember.cm_class = வெற்று;
    isn->isn_arg.classmember.cm_idx = idx;
    திரும்பு தள்ளு_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    முழு
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, முழு idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    எனில் (isn == வெற்று)
	திரும்பு FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    திரும்பு தள்ளு_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    முழு
generate_STORE_THIS(cctx_T *cctx, முழு idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    எனில் (isn == வெற்று)
	திரும்பு FAIL;

    isn->isn_arg.எண் = idx;
    திரும்பு OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    முழு
may_generate_2STRING(முழு offset, முழு tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_அடுக்கு(cctx, -1 - offset);

    தேர்வு (type->tt_type)
    {
	// nothing to be done
	நிலை VAR_STRING: திரும்பு OK;

	// conversion possible
	நிலை VAR_SPECIAL:
	நிலை VAR_BOOL:
	நிலை VAR_எண்:
	நிலை VAR_FLOAT:
			 நிறுத்து;

	// conversion possible (with runtime check)
	நிலை VAR_ANY:
	நிலை VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 நிறுத்து;

	// conversion possible when tolerant
	நிலை VAR_LIST:
	நிலை VAR_TUPLE:
	நிலை VAR_DICT:
			 எனில் (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     நிறுத்து;
			 }
			 எனில் (tostring_flags & TOSTRING_INTERPOLATE)
			     நிறுத்து;
			 // FALLTHROUGH

	// conversion not possible
	நிலை VAR_VOID:
	நிலை VAR_BLOB:
	நிலை VAR_FUNC:
	நிலை VAR_PARTIAL:
	நிலை VAR_JOB:
	நிலை VAR_CHANNEL:
	நிலை VAR_INSTR:
	நிலை VAR_CLASS:
	நிலை VAR_OBJECT:
	நிலை VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 திரும்பு FAIL;
    }

    set_type_on_அடுக்கு(cctx, &t_string, -1 - offset);
    எனில் ((isn = generate_instr(cctx, isntype)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    திரும்பு OK;
}

    நிலையான முழு
check_எண்_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    எனில் (!((type1 == VAR_எண் || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_எண் || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	எனில் (check_type_is_மதிப்பு(typ1) == FAIL
		|| check_type_is_மதிப்பு(typ2) == FAIL)
	    திரும்பு FAIL;
	எனில் (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	இல்லையெனில்
	    semsg(_(e_char_requires_எண்_or_float_arguments), *op);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    நிலையான முழு
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    க்கு (முழு i = 0; i < tuple_type->tt_argcount; i++)
    {
	எனில் (ga_grow(gap, 1) == FAIL)
	    திரும்பு FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    திரும்பு OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    நிலையான முழு
set_tuple_union_type_on_அடுக்கு(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, அளவு(type_T *), 10);

    எனில் (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    எனில் (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    எனில் ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_அடுக்கு(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    திரும்பு OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    முழு
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  கோவை_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_எண் ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    எனில் (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_எண்_or_float(type1, type2, (char_u *)"+") == FAIL)
	திரும்பு FAIL;

    எனில் (isn != வெற்று)
    {
	எனில் (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = கோவை_type;
	இல்லையெனில்
	    isn->isn_arg.op.op_type = கோவை_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    எனில் (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_அடுக்கு(cctx, &t_list_any, 0);
    இல்லையெனில் எனில் (vartype == VAR_TUPLE)
    {
	எனில் (!check_tuples_addable(type1, type2))
	    திரும்பு FAIL;

	எனில் (set_tuple_union_type_on_அடுக்கு(type1, type2, cctx) == FAIL)
	    திரும்பு FAIL;
    }

    திரும்பு isn == வெற்று ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    எனில் (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_எண்
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	திரும்பு type1->tt_type;
    திரும்பு VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    முழு
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_அடுக்கு(cctx, 1);
    type2 = get_type_on_அடுக்கு(cctx, 0);
    vartype = operator_type(type1, type2);

    தேர்வு (*op)
    {
	நிலை '+':
		  எனில் (generate_add_instr(cctx, vartype, type1, type2,
							    கோவை_COPY) == FAIL)
		      திரும்பு FAIL;
		  நிறுத்து;

	நிலை '-':
	நிலை '*':
	நிலை '/': எனில் (check_எண்_or_float(type1, type2, op) == FAIL)
		      திரும்பு FAIL;
		  எனில் (vartype == VAR_எண்)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  இல்லையெனில் எனில் (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  இல்லையெனில்
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  எனில் (isn != வெற்று)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? கோவை_MULT : *op == '/'? கோவை_DIV : கோவை_SUB;
		  நிறுத்து;

	நிலை '%': எனில் ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_எண்)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_எண்))
		  {
		      emsg(_(e_percent_requires_எண்_arguments));
		      திரும்பு FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_எண் ? ISN_OPNR : ISN_OPANY, 1);
		  எனில் (isn != வெற்று)
		      isn->isn_arg.op.op_type = கோவை_REM;
		  நிறுத்து;
    }

    // correct type of result
    எனில் (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	எனில் ((type1->tt_type == VAR_எண் || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_எண் || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_அடுக்கு(cctx, type, 0);
    }

    திரும்பு OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    நிலையான isntype_T
compare_isn_not_மதிப்புகள்(typval_T *tv, type_T *type)
{
    எனில் (tv != வெற்று)
	(வெறுமை)check_typval_is_மதிப்பு(tv);
    இல்லையெனில்
	(வெறுமை)check_type_is_மதிப்பு(type);
    திரும்பு ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    நிலையான isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != வெற்று ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != வெற்று ? tv2->v_type : type2->tt_type;

    எனில் (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	திரும்பு compare_isn_not_மதிப்புகள்(tv1, type1);
    எனில் (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	திரும்பு compare_isn_not_மதிப்புகள்(tv2, type2);

    எனில் (vartype1 == vartype2)
    {
	தேர்வு (vartype1)
	{
	    நிலை VAR_BOOL: isntype = ISN_COMPAREBOOL; நிறுத்து;
	    நிலை VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; நிறுத்து;
	    நிலை VAR_எண்: isntype = ISN_COMPARENR; நிறுத்து;
	    நிலை VAR_FLOAT: isntype = ISN_COMPAREFLOAT; நிறுத்து;
	    நிலை VAR_STRING: isntype = ISN_COMPARESTRING; நிறுத்து;
	    நிலை VAR_BLOB: isntype = ISN_COMPAREBLOB; நிறுத்து;
	    நிலை VAR_LIST: isntype = ISN_COMPARELIST; நிறுத்து;
	    நிலை VAR_TUPLE: isntype = ISN_COMPARETUPLE; நிறுத்து;
	    நிலை VAR_DICT: isntype = ISN_COMPAREDICT; நிறுத்து;
	    நிலை VAR_FUNC: isntype = ISN_COMPAREFUNC; நிறுத்து;
	    நிலை VAR_OBJECT: isntype = ISN_COMPAREOBJECT; நிறுத்து;
	    இயல்பு: isntype = ISN_COMPAREANY; நிறுத்து;
	}
    }
    இல்லையெனில் எனில் (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_எண் || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_எண் || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    இல்லையெனில் எனில் (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	எனில் ((vartype1 == VAR_SPECIAL
		&& (tv1 != வெற்று ? tv1->vval.v_எண் == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != வெற்று ? tv2->vval.v_எண் == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    திரும்பு ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    எனில் ((exprtype == கோவை_IS || exprtype == கோவை_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == கோவை_IS ? "is" : "isnot" , vartype_name(vartype1));
	திரும்பு ISN_DROP;
    }
    எனில் (!(exprtype == கோவை_IS || exprtype == கோவை_ISNOT
		|| exprtype == கோவை_EQUAL || exprtype == கோவை_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	திரும்பு ISN_DROP;
    }
    எனில் (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != கோவை_EQUAL
			&& exprtype != கோவை_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != கோவை_EQUAL
			 && exprtype != கோவை_NEQUAL
			 && exprtype != கோவை_IS
			 && exprtype != கோவை_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	திரும்பு ISN_DROP;
    }
    திரும்பு isntype;
}

    முழு
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    எனில் (get_compare_isn(type, tv1, tv2, வெற்று, வெற்று) == ISN_DROP)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    முழு
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, முழு ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, வெற்று, வெற்று,
		       get_type_on_அடுக்கு(cctx, 1), get_type_on_அடுக்கு(cctx, 0));
    எனில் (isntype == ISN_DROP)
	திரும்பு FAIL;

    எனில் ((isn = generate_instr(cctx, isntype)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --அடுக்கு->ga_len;
    set_type_on_அடுக்கு(cctx, &t_bool, 0);

    திரும்பு OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    முழு
generate_CONCAT(cctx_T *cctx, முழு மொத்தம்)
{
    isn_T	*isn;
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;

    RETURN_OK_IF_SKIP(cctx);

    எனில் ((isn = generate_instr(cctx, ISN_CONCAT)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = மொத்தம்;

    // drop the argument types
    அடுக்கு->ga_len -= மொத்தம் - 1;

    திரும்பு OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    முழு
generate_2BOOL(cctx_T *cctx, முழு invert, முழு offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_2BOOL)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_அடுக்கு(cctx, &t_bool, -1 - offset);

    திரும்பு OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    முழு
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    எனில் (generate_instr(cctx, ISN_COND2BOOL) == வெற்று)
	திரும்பு FAIL;

    // type becomes bool
    set_type_on_அடுக்கு(cctx, &t_bool, 0);

    திரும்பு OK;
}

    முழு
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	முழு	    typechk_flags,	// type check flags
	முழு	    offset,
	முழு	    is_var,
	முழு	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == வெற்று)
	திரும்பு FAIL;
    type_T *tt;
    எனில் ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_எண்_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	எனில் (tt != வெற்று)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    எனில் (typechk_flags & TYPECHK_எண்_OK)
		tt->tt_flags |= TTFLAG_எண்_OK;
	    இல்லையெனில்
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    இல்லையெனில்
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_அடுக்கு(cctx, expected, -1 - offset);

    திரும்பு OK;
}

    முழு
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_SETTYPE)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    திரும்பு OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    முழு
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    எனில் (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    நிலையான முழு
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == வெற்று ? &t_any : &class->class_type);
    எனில் (isn == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.classarg = class;
    எனில் (class != வெற்று)
	++class->class_refcount;
    திரும்பு OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    முழு
generate_tv_தள்ளு(cctx_T *cctx, typval_T *tv)
{
    தேர்வு (tv->v_type)
    {
	நிலை VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_எண்);
	    நிறுத்து;
	நிலை VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_எண்);
	    நிறுத்து;
	நிலை VAR_எண்:
	    generate_PUSHNR(cctx, tv->vval.v_எண்);
	    நிறுத்து;
	நிலை VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    நிறுத்து;
	நிலை VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = வெற்று;
	    நிறுத்து;
	நிலை VAR_LIST:
	    எனில் (tv->vval.v_list != வெற்று)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    எனில் (tv->vval.v_tuple != வெற்று)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    நிறுத்து;
	நிலை VAR_DICT:
	    எனில் (tv->vval.v_dict != வெற்று)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    நிறுத்து;
#ifdef FEAT_JOB_CHANNEL
	நிலை VAR_JOB:
	    எனில் (tv->vval.v_job != வெற்று)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    நிறுத்து;
	நிலை VAR_CHANNEL:
	    எனில் (tv->vval.v_channel != வெற்று)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    நிறுத்து;
#endif
	நிலை VAR_FUNC:
	    எனில் (tv->vval.v_string != வெற்று)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, வெற்று, &t_func_unknown, TRUE);
	    நிறுத்து;
	நிலை VAR_PARTIAL:
	    எனில் (tv->vval.v_partial != வெற்று)
		iemsg("non-null partial constant not supported");
	    எனில் (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == வெற்று)
		திரும்பு FAIL;
	    நிறுத்து;
	நிலை VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = வெற்று;
	    நிறுத்து;
	நிலை VAR_OBJECT:
	    எனில் (tv->vval.v_object != வெற்று)
	    {
		emsg(_(e_cannot_use_non_null_object));
		திரும்பு FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    நிறுத்து;
	நிலை VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    நிறுத்து;
	இயல்பு:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    திரும்பு FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    திரும்பு OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    முழு
generate_PUSHNR(cctx_T *cctx, varnumber_T எண்)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_எண்)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = எண்;

    எனில் (எண் == 0 || எண் == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_அடுக்கு(cctx, &t_எண்_bool, 0);
    திரும்பு OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    முழு
generate_PUSHBOOL(cctx_T *cctx, varnumber_T எண்)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = எண்;

    திரும்பு OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    முழு
generate_PUSHSPEC(cctx_T *cctx, varnumber_T எண்)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     எண் == VVAL_NULL ? &t_null : &t_none)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = எண்;

    திரும்பு OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    முழு
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.fnumber = fnumber;

    திரும்பு OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    முழு
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    முழு		ret = OK;

    எனில் (cctx->ctx_skip != SKIP_YES)
    {
	எனில் ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == வெற்று)
	    ret = FAIL;
	இல்லையெனில்
	{
	    isn->isn_arg.string = str == வெற்று ? வெற்று : *str;
	    திரும்பு OK;
	}
    }
    எனில் (str != வெற்று)
	VIM_CLEAR(*str);
    திரும்பு ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    முழு
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    எனில் (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
#இல்லையெனில்
    emsg(_(e_channel_job_feature_not_available));
    திரும்பு FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    முழு
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    எனில் (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
#இல்லையெனில்
    emsg(_(e_channel_job_feature_not_available));
    திரும்பு FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    முழு
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.blob = blob;

    திரும்பு OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    முழு
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, முழு may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == வெற்று)
	திரும்பு FAIL;
    எனில் (name == வெற்று)
	funcname = வெற்று;
    இல்லையெனில் எனில் (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != வெற்று)	    // autoload
	funcname = vim_strsave(name);
    இல்லையெனில்
    {
	funcname = alloc(STRLEN(name) + 3);
	எனில் (funcname != வெற்று)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    திரும்பு OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    முழு
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.string = vim_strsave(name);
    எனில் (isn->isn_arg.string == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    முழு
generate_GETITEM(cctx_T *cctx, முழு index, முழு with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_அடுக்கு(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    எனில் ((isn = generate_instr(cctx, ISN_GETITEM)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    திரும்பு தள்ளு_type_அடுக்கு(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    நிலையான வெறுமை
set_tuple_slice_type_on_அடுக்கு(type_T *type, முழு மொத்தம், cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    எனில் ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= மொத்தம்)
    {
	set_type_on_அடுக்கு(cctx, &t_tuple_any, 0);
	திரும்பு;
    }

    ga_init2(&tuple_types_ga, அளவு(type_T *), 10);

    க்கு (முழு i = மொத்தம்; i < type->tt_argcount; i++)
    {
	எனில் (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_அடுக்கு(cctx, &t_tuple_any, 0);
	    திரும்பு;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] =
							     type->tt_args[i];
	tuple_types_ga.ga_len++;
    }

    set_type_on_அடுக்கு(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    முழு
generate_SLICE(cctx_T *cctx, முழு மொத்தம்)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_SLICE)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = மொத்தம்;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_அடுக்கு(cctx, 0);
    எனில் (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_அடுக்கு(type, மொத்தம், cctx);

    திரும்பு OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    முழு
generate_CHECKLEN(cctx_T *cctx, முழு min_len, முழு more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    எனில் ((isn = generate_instr(cctx, ISN_CHECKLEN)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    திரும்பு OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    முழு
generate_STORE(cctx_T *cctx, isntype_T isn_type, முழு idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_drop(cctx, isn_type, 1)) == வெற்று)
	திரும்பு FAIL;
    எனில் (name != வெற்று)
	isn->isn_arg.string = vim_strsave(name);
    இல்லையெனில்
	isn->isn_arg.எண் = idx;

    திரும்பு OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    முழு
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	முழு	    load,
	class_T	    *cl,
	முழு	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    இல்லையெனில்
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    எனில் (isn == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    திரும்பு OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    நிலையான முழு
generate_STOREOUTER(
	cctx_T	    *cctx,
	முழு	    idx,
	முழு	    level,
	முழு	    loop_depth,
	முழு	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == வெற்று)
	திரும்பு FAIL;
    எனில் (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    இல்லையெனில்
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    திரும்பு OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    முழு
generate_STORENR(cctx_T *cctx, முழு idx, varnumber_T மதிப்பு)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_STORENR)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = மதிப்பு;

    திரும்பு OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    நிலையான முழு
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	முழு	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_drop(cctx, isn_type, 1)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    திரும்பு OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    முழு
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	முழு	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_type2(cctx, isn_type, type, type)) == வெற்று)
	திரும்பு FAIL;
    எனில் (name != வெற்று)
	isn->isn_arg.string = vim_strsave(name);
    இல்லையெனில்
	isn->isn_arg.எண் = idx;

    திரும்பு OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    முழு
generate_LOADOUTER(
	cctx_T	    *cctx,
	முழு	    idx,
	முழு	    nesting,
	முழு	    loop_depth,
	முழு	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == வெற்று)
	திரும்பு FAIL;
    எனில் (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    இல்லையெனில்
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    திரும்பு OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    முழு
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    முழு	    di_flags;
    முழு	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    எனில் (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	திரும்பு FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    திரும்பு generate_LOAD(cctx, ISN_LOADV, vidx, வெற்று, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    முழு
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, முழு forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, isn_type)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    திரும்பு OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    முழு
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    எனில் (generate_instr(cctx, ISN_LOCKCONST) == வெற்று)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    முழு
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	முழு	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    இல்லையெனில்
	isn = generate_instr_drop(cctx, isn_type, 1);
    எனில் (isn == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    திரும்பு OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    முழு
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	முழு	    sid,
	முழு	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    எனில் (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    இல்லையெனில்
	isn = generate_instr_drop(cctx, isn_type, 1);
    எனில் (isn == வெற்று)
	திரும்பு FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    எனில் (sref == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    திரும்பு OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    முழு
generate_NEWLIST(cctx_T *cctx, முழு மொத்தம், முழு use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_NEWLIST)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = use_null ? -1 : மொத்தம்;

    // Get the member type and the declared member type from all the items on
    // the stack.
    எனில் ((member_type = get_member_type_from_அடுக்கு(மொத்தம், 1, cctx)) == வெற்று)
	திரும்பு FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_அடுக்கு.ga_len -= மொத்தம்;

    // add the list type to the type stack
    திரும்பு தள்ளு_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    முழு
generate_NEWTUPLE(cctx_T *cctx, முழு மொத்தம், முழு use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = use_null ? -1 : மொத்தம்;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, அளவு(type_T *), 10);

    எனில் (get_tuple_type_from_அடுக்கு(மொத்தம், &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	திரும்பு FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_அடுக்கு.ga_len -= மொத்தம்;

    // add the tuple type to the type stack
    திரும்பு தள்ளு_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    முழு
generate_NEWDICT(cctx_T *cctx, முழு மொத்தம், முழு use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_NEWDICT)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = use_null ? -1 : மொத்தம்;

    எனில் ((member_type = get_member_type_from_அடுக்கு(மொத்தம், 2, cctx)) == வெற்று)
	திரும்பு FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_அடுக்கு.ga_len -= 2 * மொத்தம்;

    // add the dict type to the type stack
    திரும்பு தள்ளு_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    முழு
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	முழு	    object_method,
	முழு	    fi,
	முழு	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    முழு		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_FUNCREF)) == வெற்று)
	திரும்பு FAIL;
    எனில் (isn_idx != வெற்று)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    எனில் (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != வெற்று)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	எனில் (extra == வெற்று)
	    திரும்பு FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	எனில் (cl != வெற்று)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    எனில் (ufunc->uf_def_status == UF_NOT_COMPILED || cl != வெற்று)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    எனில் (ufunc->uf_def_status != UF_NOT_COMPILED && cl == வெற்று)
    {
	எனில் (isn_idx == வெற்று && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (வெறுமை)compile_def_function(ufunc, FALSE, CT_NONE, வெற்று);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_closure = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    எனில் ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == வெற்று ? &t_func_any : ufunc->uf_func_type;
    திரும்பு தள்ளு_type_அடுக்கு(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    முழு
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    முழு		ret = OK;

    எனில் (cctx->ctx_skip != SKIP_YES)
    {
	எனில் ((isn = generate_instr(cctx, ISN_NEWFUNC)) == வெற்று)
	    ret = FAIL;
	இல்லையெனில்
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    எனில் (arg == வெற்று)
		ret = FAIL;
	    இல்லையெனில்
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(வெறுமை)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		திரும்பு OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    திரும்பு ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    முழு
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_DEF)) == வெற்று)
	திரும்பு FAIL;
    எனில் (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	எனில் (isn->isn_arg.string == வெற்று)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    முழு
generate_JUMP(cctx_T *cctx, jumpwhen_T when, முழு where)
{
    isn_T	*isn;
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_JUMP)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    எனில் (when != JUMP_ALWAYS && அடுக்கு->ga_len > 0)
	--அடுக்கு->ga_len;

    திரும்பு OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    முழு
generate_WHILE(cctx_T *cctx, முழு funcref_idx)
{
    isn_T	*isn;
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_WHILE)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    எனில் (அடுக்கு->ga_len > 0)
	--அடுக்கு->ga_len;

    திரும்பு OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    முழு
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, முழு arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, isn_type)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    திரும்பு OK;
}

    முழு
generate_FOR(cctx_T *cctx, முழு loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_FOR)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    திரும்பு தள்ளு_type_அடுக்கு(cctx, &t_any);
}

    முழு
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_ENDLOOP)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_மொத்தம்;
    isn->isn_arg.endloop.end_var_மொத்தம் =
			   cctx->ctx_locals.ga_len - loop_info->li_local_மொத்தம்;
    திரும்பு OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    முழு
generate_TRYCONT(cctx_T *cctx, முழு levels, முழு where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_TRYCONT)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    திரும்பு OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    முழு
check_internal_func_args(
	cctx_T	*cctx,
	முழு	func_idx,
	முழு	argcount,
	முழு	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;
    முழு		argoff = check_internal_func(func_idx, argcount);

    எனில் (argoff < 0)
	திரும்பு FAIL;

    எனில் (method_call && argoff > 1)
    {
	எனில் (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    திரும்பு FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	எனில் (isn  == வெற்று)
	    திரும்பு FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    எனில் (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)அடுக்கு->ga_data) + அடுக்கு->ga_len - argcount;

	// Check the types of the arguments.
	எனில் (method_call && argoff > 1)
	{
	    முழு i;

	    க்கு (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	இல்லையெனில்
	{
	    முழு i;

	    க்கு (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	எனில் (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    முழு
generate_BCALL(cctx_T *cctx, முழு func_idx, முழு argcount, முழு method_call)
{
    isn_T	*isn;
    garray_T	*அடுக்கு = &cctx->ctx_type_அடுக்கு;
    type2_T	*argtypes = வெற்று;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = வெற்று;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    எனில் (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	திரும்பு FAIL;

    எனில் (internal_func_is_map(func_idx))
	maptype = argtypes;

    எனில் ((isn = generate_instr(cctx, ISN_BCALL)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    அடுக்கு->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    எனில் (தள்ளு_type_stack2(cctx, type, decl_type) == FAIL)
	திரும்பு FAIL;

    எனில் (maptype != வெற்று && maptype[0].type_decl->tt_member != வெற்று
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    திரும்பு OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    முழு
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_அடுக்கு(cctx, 1);
    எனில் (arg_type_modifiable(list_type, 1) == FAIL)
	திரும்பு FAIL;
    item_type = get_type_on_அடுக்கு(cctx, 0);
    expected = list_type->tt_member;
    எனில் (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	திரும்பு FAIL;

    எனில் (generate_instr(cctx, ISN_LISTAPPEND) == வெற்று)
	திரும்பு FAIL;

    --cctx->ctx_type_அடுக்கு.ga_len;	    // drop the argument
    திரும்பு OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    முழு
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    எனில் (arg_type_modifiable(get_decl_type_on_அடுக்கு(cctx, 1), 1) == FAIL)
	திரும்பு FAIL;
    item_type = get_type_on_அடுக்கு(cctx, 0);
    எனில் (need_type(item_type, &t_எண், 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	திரும்பு FAIL;

    எனில் (generate_instr(cctx, ISN_BLOBAPPEND) == வெற்று)
	திரும்பு FAIL;

    --cctx->ctx_type_அடுக்கு.ga_len;	    // drop the argument
    திரும்பு OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    நிலையான isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    எனில் (cl != வெற்று)
	திரும்பு ISN_METHODCALL;

    எனில் (ufunc->uf_def_status == UF_NOT_COMPILED)
	திரும்பு ISN_UCALL;

    // function invoked from a lambda
    எனில் (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	எனில் (ufunc->uf_name[0] == K_SPECIAL)
	    திரும்பு ISN_UCALL;
    }

    திரும்பு ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    முழு
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	முழு	    mi,
	முழு	    pushed_argcount,
	முழு	    is_super)
{
    isn_T	*isn;
    முழு		regular_args = ufunc->uf_args.ga_len;
    முழு		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    எனில் (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	திரும்பு FAIL;
    }
    எனில் (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	திரும்பு FAIL;
    }

    எனில் (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	முழு		i;
	compiletype_T	compile_type;

	க்கு (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_அடுக்கு(cctx, argcount - i - 1);
	    எனில் (check_type_is_மதிப்பு(actual) == FAIL)
		திரும்பு FAIL;
	    எனில் (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		தொடர்;
	    }
	    எனில் (i < regular_args)
	    {
		எனில் (ufunc->uf_arg_types == வெற்று)
		    தொடர்;
		expected = ufunc->uf_arg_types[i];
	    }
	    இல்லையெனில் எனில் (ufunc->uf_va_type == வெற்று
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    இல்லையெனில்
		expected = ufunc->uf_va_type->tt_member;
	    எனில் (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		திரும்பு FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	எனில் (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == வெற்று,
						   compile_type, வெற்று) == FAIL)
	    திரும்பு FAIL;
    }
    எனில் (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	திரும்பு FAIL;
    }

    எனில் ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == வெற்று)
	திரும்பு FAIL;
    எனில் (cl != வெற்று /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	எனில் (isn->isn_arg.mfunc == வெற்று)
	    திரும்பு FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = வெற்று;
	எனில் (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = வெற்று;
	isn->isn_arg.mfunc->cmf_cache_generic = வெற்று;
	isn->isn_arg.mfunc->cmf_cache_ufunc = வெற்று;
    }
    இல்லையெனில் எனில் (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    இல்லையெனில்
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_அடுக்கு.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    எனில் (ufunc->uf_class != வெற்று)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_அடுக்கு(cctx, 0);
	எனில் (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_அடுக்கு.ga_len--;
    }

    // add return type
    திரும்பு தள்ளு_type_அடுக்கு(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    முழு
generate_UCALL(cctx_T *cctx, char_u *name, முழு argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_UCALL)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_அடுக்கு.ga_len -= argcount;

    // add return value
    திரும்பு தள்ளு_type_அடுக்கு(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    முழு
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	முழு	argcount,
	முழு	at_உச்சி,
	char_u	*name)
{
    எனில் (type->tt_argcount != -1)
    {
	முழு	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	எனில் (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    திரும்பு FAIL;
	}
	எனில் (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    திரும்பு FAIL;
	}
	எனில் (type->tt_args != வெற்று)
	{
	    முழு i;

	    க்கு (i = 0; i < argcount; ++i)
	    {
		முழு	offset = -argcount + i - (at_உச்சி ? 0 : 1);
		type_T	*actual = get_type_on_அடுக்கு(cctx, -1 - offset);
		type_T	*expected;

		எனில் (check_type_is_மதிப்பு(actual) == FAIL)
		    திரும்பு FAIL;
		எனில் (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    எனில் (expected != வெற்று && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    எனில் (expected == வெற்று)
			expected = &t_any;
		}
		இல்லையெனில் எனில் (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		இல்லையெனில்
		    expected = type->tt_args[i];
		எனில் (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    திரும்பு FAIL;
		}
	    }
	}
    }

    திரும்பு OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    முழு
generate_PCALL(
	cctx_T	*cctx,
	முழு	argcount,
	char_u	*name,
	type_T	*type,
	முழு	at_உச்சி)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    எனில் (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    இல்லையெனில் எனில் (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	எனில் (check_func_args_from_type(cctx, type, argcount, at_உச்சி, name)
								       == FAIL)
	    திரும்பு FAIL;

	ret_type = type->tt_member;
	எனில் (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    இல்லையெனில்
    {
	semsg(_(e_not_callable_type_str), name);
	திரும்பு FAIL;
    }

    எனில் ((isn = generate_instr(cctx, ISN_PCALL)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.pfunc.cpf_உச்சி = at_உச்சி;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_அடுக்கு.ga_len -= argcount + 1;

    // push the return value
    தள்ளு_type_அடுக்கு(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    எனில் (at_உச்சி && generate_instr(cctx, ISN_PCALL_END) == வெற்று)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    முழு
generate_DEFER(cctx_T *cctx, முழு var_idx, முழு argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    திரும்பு OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    முழு
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_அடுக்கு(cctx, 0);
    எனில் (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	எழுத்து *tofree;

	semsg(_(e_expected_dictionary_for_using_சாவி_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	திரும்பு FAIL;
    }
    // change dict type to dict member type
    எனில் (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_அடுக்கு(cctx, ntype, 0);
    }

    திரும்பு OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    முழு
generate_ECHO(cctx_T *cctx, முழு with_white, முழு மொத்தம்)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_drop(cctx, ISN_ECHO, மொத்தம்)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_மொத்தம் = மொத்தம்;

    திரும்பு OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    முழு
generate_MULT_கோவை(cctx_T *cctx, isntype_T isn_type, முழு மொத்தம்)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr_drop(cctx, isn_type, மொத்தம்)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = மொத்தம்;
    திரும்பு OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    முழு
generate_ECHOWINDOW(cctx_T *cctx, முழு மொத்தம், long time)
{
    isn_T	*isn;

    எனில் ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, மொத்தம்)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.echowin.ewin_மொத்தம் = மொத்தம்;
    isn->isn_arg.echowin.ewin_time = time;
    திரும்பு OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    முழு
generate_SOURCE(cctx_T *cctx, முழு sid)
{
    isn_T	*isn;

    எனில் ((isn = generate_instr(cctx, ISN_SOURCE)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = sid;

    திரும்பு OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    முழு
generate_PUT(cctx_T *cctx, முழு regname, linenr_T lnum, முழு fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    எனில் (isn == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    திரும்பு OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    முழு
generate_LOCKUNLOCK(cctx_T *cctx, char_u *வரி, முழு is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == வெற்று)
	திரும்பு FAIL;
    class_T *cl = cctx->ctx_ufunc != வெற்று ? cctx->ctx_ufunc->uf_class : வெற்று;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(வரி);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    எனில் (cl != வெற்று)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    திரும்பு OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    முழு
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *வரி)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, isntype)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.string = vim_strsave(வரி);
    திரும்பு OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    முழு
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    முழு		ret = OK;

    எனில் (cctx->ctx_skip != SKIP_YES)
    {
	எனில் ((isn = generate_instr(cctx, isntype)) == வெற்று)
	    ret = FAIL;
	இல்லையெனில்
	{
	    isn->isn_arg.string = str;
	    திரும்பு OK;
	}
    }
    vim_free(str);
    திரும்பு ret;
}

    முழு
generate_LEGACY_EVAL(cctx_T *cctx, char_u *வரி)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.string = vim_strsave(வரி);

    திரும்பு தள்ளு_type_அடுக்கு(cctx, &t_any);
}

    முழு
generate_EXECCONCAT(cctx_T *cctx, முழு மொத்தம்)
{
    isn_T	*isn;

    எனில் ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, மொத்தம்)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.எண் = மொத்தம்;
    திரும்பு OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    முழு
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    எனில் ((isn = generate_instr(cctx, ISN_RANGE)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.string = range;

    திரும்பு தள்ளு_type_அடுக்கு(cctx, &t_எண்);
}

    முழு
generate_UNPACK(cctx_T *cctx, முழு var_மொத்தம், முழு semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_UNPACK)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.unpack.unp_மொத்தம் = var_மொத்தம்;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    திரும்பு OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    முழு
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    எனில் (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	எனில் ((isn = generate_instr(cctx, ISN_CMDMOD)) == வெற்று)
	    திரும்பு FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	எனில் (isn->isn_arg.cmdmod.cf_cmdmod == வெற்று)
	    திரும்பு FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, அளவு(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = வெற்று;
    }

    திரும்பு OK;
}

    முழு
generate_undo_cmdmods(cctx_T *cctx)
{
    எனில் (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == வெற்று)
	திரும்பு FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    திரும்பு OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    முழு
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	முழு		opt_flags,
	முழு		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    தேர்வு (dest)
    {
	நிலை dest_option:
	    திரும்பு generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	நிலை dest_func_option:
	    திரும்பு generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	நிலை dest_global:
	    // include g: with the name, easier to execute that way
	    திரும்பு generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == வெற்று
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	நிலை dest_buffer:
	    // include b: with the name, easier to execute that way
	    திரும்பு generate_STORE(cctx, ISN_STOREB, 0, name);
	நிலை dest_window:
	    // include w: with the name, easier to execute that way
	    திரும்பு generate_STORE(cctx, ISN_STOREW, 0, name);
	நிலை dest_tab:
	    // include t: with the name, easier to execute that way
	    திரும்பு generate_STORE(cctx, ISN_STORET, 0, name);
	நிலை dest_env:
	    திரும்பு generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	நிலை dest_reg:
	    திரும்பு generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], வெற்று);
	நிலை dest_vimvar:
	    திரும்பு generate_STORE(cctx, ISN_STOREV, vimvaridx, வெற்று);
	நிலை dest_script:
	நிலை dest_script_v9:
	    {
		முழு	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		முழு	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		எனில் (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    எனில் (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == வெற்று)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			எனில் (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    திரும்பு FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    திரும்பு generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		திரும்பு generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	நிலை dest_class_member:
	    திரும்பு generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	நிலை dest_local:
	நிலை dest_கோவை:
	    // cannot happen
	    நிறுத்து;
    }
    திரும்பு FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    முழு
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    க்கு (;;)
    {
	எனில் (scope == வெற்று)
	    நிறுத்து;
	எனில் (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    திரும்பு TRUE;
	scope = scope->se_outer;
    }
    திரும்பு FALSE;
}

    முழு
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, முழு instr_மொத்தம், முழு is_decl)
{
    எனில் (lhs->lhs_dest != dest_local)
	திரும்பு generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    எனில் (lhs->lhs_lvar == வெற்று)
	திரும்பு OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    எனில் (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_மொத்தம் + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.எண்;
	garray_T    *அடுக்கு = &cctx->ctx_type_அடுக்கு;

	எனில் (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	இல்லையெனில்
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	எனில் (அடுக்கு->ga_len > 0)
	    --அடுக்கு->ga_len;
    }
    இல்லையெனில் எனில் (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    இல்லையெனில்
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, வெற்று);
    திரும்பு OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    முழு
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    எனில் ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == வெற்று)
	திரும்பு FAIL;
    isn->isn_arg.setsctx = new_sctx;
    திரும்பு OK;
}

#எனில் defined(FEAT_PROFILE)
    வெறுமை
may_generate_prof_end(cctx_T *cctx, முழு prof_lnum)
{
    எனில் (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_END);
}
#endif


/*
 * Delete an instruction, free what it contains.
 */
    வெறுமை
delete_instr(isn_T *isn)
{
    தேர்வு (isn->isn_type)
    {
	நிலை ISN_AUTOLOAD:
	நிலை ISN_DEF:
	நிலை ISN_EXEC:
	நிலை ISN_EXECRANGE:
	நிலை ISN_EXEC_SPLIT:
	நிலை ISN_LEGACY_EVAL:
	நிலை ISN_LOADAUTO:
	நிலை ISN_LOADB:
	நிலை ISN_LOADENV:
	நிலை ISN_LOADG:
	நிலை ISN_LOADOPT:
	நிலை ISN_LOADT:
	நிலை ISN_LOADW:
	நிலை ISN_PUSHEXC:
	நிலை ISN_PUSHFUNC:
	நிலை ISN_PUSHS:
	நிலை ISN_RANGE:
	நிலை ISN_STOREAUTO:
	நிலை ISN_STOREB:
	நிலை ISN_STOREENV:
	நிலை ISN_STOREG:
	நிலை ISN_STORET:
	நிலை ISN_STOREW:
	நிலை ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    நிறுத்து;

	நிலை ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    நிறுத்து;

	நிலை ISN_SUBSTITUTE:
	    {
		முழு	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		க்கு (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    நிறுத்து;

	நிலை ISN_INSTR:
	    {
		முழு	idx;
		isn_T	*list = isn->isn_arg.instr;

		க்கு (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    நிறுத்து;

	நிலை ISN_LOADS:
	நிலை ISN_LOADEXPORT:
	நிலை ISN_STORES:
	நிலை ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    நிறுத்து;

	நிலை ISN_UNLET:
	நிலை ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    நிறுத்து;

	நிலை ISN_STOREOPT:
	நிலை ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    நிறுத்து;

	நிலை ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    நிறுத்து;

	நிலை ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    நிறுத்து;

	நிலை ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    நிறுத்து;

	நிலை ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		எனில் (extra == வெற்று || extra->fre_func_name == வெற்று)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    எனில் (ufunc != வெற்று && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		எனில் (extra != வெற்று)
		{
		    char_u *name = extra->fre_func_name;

		    எனில் (name != வெற்று)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    எனில் (extra->fre_class != வெற்று)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    நிறுத்து;

	நிலை ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		எனில் (dfunc->df_ufunc != வெற்று
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    நிறுத்து;

	நிலை ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    நிறுத்து;

	நிலை ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		எனில் (arg != வெற்று)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    எனில் (ufunc != வெற்று)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    நிறுத்து;

	நிலை ISN_CHECKTYPE:
	நிலை ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    நிறுத்து;

	நிலை ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    நிறுத்து;

	நிலை ISN_LOADSCRIPT:
	நிலை ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    நிறுத்து;

	நிலை ISN_LOAD_CLASSMEMBER:
	நிலை ISN_STORE_CLASSMEMBER:
	நிலை ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    நிறுத்து;

	நிலை ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    நிறுத்து;

	நிலை ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    நிறுத்து;

	நிலை ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    நிறுத்து;

	நிலை ISN_2BOOL:
	நிலை ISN_2STRING:
	நிலை ISN_2STRING_ANY:
	நிலை ISN_ADDBLOB:
	நிலை ISN_ADDLIST:
	நிலை ISN_ADDTUPLE:
	நிலை ISN_ANYINDEX:
	நிலை ISN_ANYSLICE:
	நிலை ISN_BCALL:
	நிலை ISN_BLOBAPPEND:
	நிலை ISN_BLOBINDEX:
	நிலை ISN_BLOBSLICE:
	நிலை ISN_CATCH:
	நிலை ISN_CEXPR_AUCMD:
	நிலை ISN_CHECKLEN:
	நிலை ISN_CLEARDICT:
	நிலை ISN_CMDMOD_REV:
	நிலை ISN_COMPAREANY:
	நிலை ISN_COMPAREBLOB:
	நிலை ISN_COMPAREBOOL:
	நிலை ISN_COMPAREDICT:
	நிலை ISN_COMPAREFLOAT:
	நிலை ISN_COMPAREFUNC:
	நிலை ISN_COMPARELIST:
	நிலை ISN_COMPARETUPLE:
	நிலை ISN_COMPARENR:
	நிலை ISN_COMPARENULL:
	நிலை ISN_COMPAREOBJECT:
	நிலை ISN_COMPARESPECIAL:
	நிலை ISN_COMPARESTRING:
	நிலை ISN_CONCAT:
	நிலை ISN_CONSTRUCT:
	நிலை ISN_COND2BOOL:
	நிலை ISN_DEBUG:
	நிலை ISN_DEFER:
	நிலை ISN_DROP:
	நிலை ISN_ECHO:
	நிலை ISN_ECHOCONSOLE:
	நிலை ISN_ECHOERR:
	நிலை ISN_ECHOMSG:
	நிலை ISN_ECHOWINDOW:
	நிலை ISN_ENDLOOP:
	நிலை ISN_ENDTRY:
	நிலை ISN_EXECCONCAT:
	நிலை ISN_EXECUTE:
	நிலை ISN_FINALLY:
	நிலை ISN_FINISH:
	நிலை ISN_FOR:
	நிலை ISN_GETITEM:
	நிலை ISN_GET_OBJ_MEMBER:
	நிலை ISN_JUMP:
	நிலை ISN_JUMP_IF_ARG_NOT_SET:
	நிலை ISN_JUMP_IF_ARG_SET:
	நிலை ISN_LISTAPPEND:
	நிலை ISN_LISTINDEX:
	நிலை ISN_LISTSLICE:
	நிலை ISN_TUPLEINDEX:
	நிலை ISN_TUPLESLICE:
	நிலை ISN_LOAD:
	நிலை ISN_LOADBDICT:
	நிலை ISN_LOADGDICT:
	நிலை ISN_LOADOUTER:
	நிலை ISN_LOADREG:
	நிலை ISN_LOADTDICT:
	நிலை ISN_LOADV:
	நிலை ISN_LOADWDICT:
	நிலை ISN_LOCKCONST:
	நிலை ISN_MEMBER:
	நிலை ISN_NEGATENR:
	நிலை ISN_NEWDICT:
	நிலை ISN_NEWLIST:
	நிலை ISN_NEWTUPLE:
	நிலை ISN_NEWPARTIAL:
	நிலை ISN_OPANY:
	நிலை ISN_OPFLOAT:
	நிலை ISN_OPNR:
	நிலை ISN_PCALL:
	நிலை ISN_PCALL_END:
	நிலை ISN_PROF_END:
	நிலை ISN_PROF_START:
	நிலை ISN_PUSHBOOL:
	நிலை ISN_PUSHCHANNEL:
	நிலை ISN_PUSHF:
	நிலை ISN_PUSHJOB:
	நிலை ISN_PUSHNR:
	நிலை ISN_PUSHOBJ:
	நிலை ISN_PUSHSPEC:
	நிலை ISN_PUT:
	நிலை ISN_IPUT:
	நிலை ISN_REDIREND:
	நிலை ISN_REDIRSTART:
	நிலை ISN_RETURN:
	நிலை ISN_RETURN_OBJECT:
	நிலை ISN_RETURN_VOID:
	நிலை ISN_SHUFFLE:
	நிலை ISN_SLICE:
	நிலை ISN_SOURCE:
	நிலை ISN_STORE:
	நிலை ISN_STORENR:
	நிலை ISN_STOREOUTER:
	நிலை ISN_STORE_THIS:
	நிலை ISN_STORERANGE:
	நிலை ISN_STOREREG:
	நிலை ISN_STOREV:
	நிலை ISN_STRINDEX:
	நிலை ISN_STRSLICE:
	நிலை ISN_THROW:
	நிலை ISN_TRYCONT:
	நிலை ISN_UNLETINDEX:
	நிலை ISN_UNLETRANGE:
	நிலை ISN_UNPACK:
	நிலை ISN_USEDICT:
	நிலை ISN_WHILE:
	நிலை ISN_SCRIPTCTX_SET:
	// nothing allocated
	நிறுத்து;
    }
}

    வெறுமை
clear_instr_ga(garray_T *gap)
{
    முழு idx;

    க்கு (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
