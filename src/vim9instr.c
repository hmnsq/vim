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
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) ಯದಿ (cctx->ctx_skip == SKIP_YES) ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ
#define RETURN_OK_IF_SKIP(cctx) ಯದಿ (cctx->ctx_skip == SKIP_YES) ಪ್ರತ್ಯಾಗಮ OK

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
    ಯದಿ (GA_GROW_FAILS(instr, 1))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    ಪ್ರತ್ಯಾಗಮ isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, ಪೂರ್ಣಾಙ್ಕ drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_ರಾಶಿ.ga_len -= drop;
    ಪ್ರತ್ಯಾಗಮ generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    ಸ್ಥಿರ isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    ಯದಿ ((isn = generate_instr(cctx, isn_type)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (ತಳ್ಳು_type_stack2(cctx, type == ರಿಕ್ತ ? &t_any : type,
			       decl_type == ರಿಕ್ತ ? &t_any : decl_type) == FAIL)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    ಪ್ರತ್ಯಾಗಮ generate_instr_type2(cctx, isn_type, type, &t_any);
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

    ಯದಿ ((isn = generate_instr(cctx, ISN_DEBUG)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    ಪ್ರತ್ಯಾಗಮ isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.construct.construct_size = ಪರಿಮಾಣ(object_T)
			       + cl->class_obj_member_ಎಣಿಕೆ * ಪರಿಮಾಣ(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_GET_OBJ_MEMBER(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    isn->isn_arg.classmember.cm_class = ರಿಕ್ತ;
    isn->isn_arg.classmember.cm_idx = idx;
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, ಪೂರ್ಣಾಙ್ಕ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_STORE_THIS(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    isn->isn_arg.ಸಂಖ್ಯೆ = idx;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    ಪೂರ್ಣಾಙ್ಕ
may_generate_2STRING(ಪೂರ್ಣಾಙ್ಕ offset, ಪೂರ್ಣಾಙ್ಕ tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_ರಾಶಿ(cctx, -1 - offset);

    ವಿಕಲ್ಪ (type->tt_type)
    {
	// nothing to be done
	ಪಕ್ಷ VAR_STRING: ಪ್ರತ್ಯಾಗಮ OK;

	// conversion possible
	ಪಕ್ಷ VAR_SPECIAL:
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	ಪಕ್ಷ VAR_FLOAT:
			 ಭಙ್ಗ;

	// conversion possible (with runtime check)
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 ಭಙ್ಗ;

	// conversion possible when tolerant
	ಪಕ್ಷ VAR_LIST:
	ಪಕ್ಷ VAR_TUPLE:
	ಪಕ್ಷ VAR_DICT:
			 ಯದಿ (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     ಭಙ್ಗ;
			 }
			 ಯದಿ (tostring_flags & TOSTRING_INTERPOLATE)
			     ಭಙ್ಗ;
			 // FALLTHROUGH

	// conversion not possible
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_BLOB:
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_PARTIAL:
	ಪಕ್ಷ VAR_JOB:
	ಪಕ್ಷ VAR_CHANNEL:
	ಪಕ್ಷ VAR_INSTR:
	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_OBJECT:
	ಪಕ್ಷ VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    set_type_on_ರಾಶಿ(cctx, &t_string, -1 - offset);
    ಯದಿ ((isn = generate_instr(cctx, isntype)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_ಸಂಖ್ಯೆ_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    ಯದಿ (!((type1 == VAR_ಸಂಖ್ಯೆ || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_ಸಂಖ್ಯೆ || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	ಯದಿ (check_type_is_ಮೌಲ್ಯ(typ1) == FAIL
		|| check_type_is_ಮೌಲ್ಯ(typ2) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	ಅನ್ಯಥಾ
	    semsg(_(e_char_requires_ಸಂಖ್ಯೆ_or_float_arguments), *op);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < tuple_type->tt_argcount; i++)
    {
	ಯದಿ (ga_grow(gap, 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
set_tuple_union_type_on_ರಾಶಿ(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, ಪರಿಮಾಣ(type_T *), 10);

    ಯದಿ (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    ಯದಿ (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    ಯದಿ ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_ರಾಶಿ(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  ಅಭಿವ್ಯಕ್ತಿ_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_ಸಂಖ್ಯೆ ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    ಯದಿ (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_ಸಂಖ್ಯೆ_or_float(type1, type2, (char_u *)"+") == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (isn != ರಿಕ್ತ)
    {
	ಯದಿ (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = ಅಭಿವ್ಯಕ್ತಿ_type;
	ಅನ್ಯಥಾ
	    isn->isn_arg.op.op_type = ಅಭಿವ್ಯಕ್ತಿ_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    ಯದಿ (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_ರಾಶಿ(cctx, &t_list_any, 0);
    ಅನ್ಯಥಾ ಯದಿ (vartype == VAR_TUPLE)
    {
	ಯದಿ (!check_tuples_addable(type1, type2))
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (set_tuple_union_type_on_ರಾಶಿ(type1, type2, cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ isn == ರಿಕ್ತ ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    ಯದಿ (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_ಸಂಖ್ಯೆ
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	ಪ್ರತ್ಯಾಗಮ type1->tt_type;
    ಪ್ರತ್ಯಾಗಮ VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_ರಾಶಿ(cctx, 1);
    type2 = get_type_on_ರಾಶಿ(cctx, 0);
    vartype = operator_type(type1, type2);

    ವಿಕಲ್ಪ (*op)
    {
	ಪಕ್ಷ '+':
		  ಯದಿ (generate_add_instr(cctx, vartype, type1, type2,
							    ಅಭಿವ್ಯಕ್ತಿ_COPY) == FAIL)
		      ಪ್ರತ್ಯಾಗಮ FAIL;
		  ಭಙ್ಗ;

	ಪಕ್ಷ '-':
	ಪಕ್ಷ '*':
	ಪಕ್ಷ '/': ಯದಿ (check_ಸಂಖ್ಯೆ_or_float(type1, type2, op) == FAIL)
		      ಪ್ರತ್ಯಾಗಮ FAIL;
		  ಯದಿ (vartype == VAR_ಸಂಖ್ಯೆ)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  ಅನ್ಯಥಾ ಯದಿ (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  ಅನ್ಯಥಾ
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  ಯದಿ (isn != ರಿಕ್ತ)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? ಅಭಿವ್ಯಕ್ತಿ_MULT : *op == '/'? ಅಭಿವ್ಯಕ್ತಿ_DIV : ಅಭಿವ್ಯಕ್ತಿ_SUB;
		  ಭಙ್ಗ;

	ಪಕ್ಷ '%': ಯದಿ ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_ಸಂಖ್ಯೆ)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_ಸಂಖ್ಯೆ))
		  {
		      emsg(_(e_percent_requires_ಸಂಖ್ಯೆ_arguments));
		      ಪ್ರತ್ಯಾಗಮ FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_ಸಂಖ್ಯೆ ? ISN_OPNR : ISN_OPANY, 1);
		  ಯದಿ (isn != ರಿಕ್ತ)
		      isn->isn_arg.op.op_type = ಅಭಿವ್ಯಕ್ತಿ_REM;
		  ಭಙ್ಗ;
    }

    // correct type of result
    ಯದಿ (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	ಯದಿ ((type1->tt_type == VAR_ಸಂಖ್ಯೆ || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_ಸಂಖ್ಯೆ || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_ರಾಶಿ(cctx, type, 0);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    ಸ್ಥಿರ isntype_T
compare_isn_not_ಮೌಲ್ಯಗಳು(typval_T *tv, type_T *type)
{
    ಯದಿ (tv != ರಿಕ್ತ)
	(ಶೂನ್ಯ)check_typval_is_ಮೌಲ್ಯ(tv);
    ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)check_type_is_ಮೌಲ್ಯ(type);
    ಪ್ರತ್ಯಾಗಮ ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    ಸ್ಥಿರ isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != ರಿಕ್ತ ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != ರಿಕ್ತ ? tv2->v_type : type2->tt_type;

    ಯದಿ (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	ಪ್ರತ್ಯಾಗಮ compare_isn_not_ಮೌಲ್ಯಗಳು(tv1, type1);
    ಯದಿ (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	ಪ್ರತ್ಯಾಗಮ compare_isn_not_ಮೌಲ್ಯಗಳು(tv2, type2);

    ಯದಿ (vartype1 == vartype2)
    {
	ವಿಕಲ್ಪ (vartype1)
	{
	    ಪಕ್ಷ VAR_BOOL: isntype = ISN_COMPAREBOOL; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ: isntype = ISN_COMPARENR; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_FLOAT: isntype = ISN_COMPAREFLOAT; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_STRING: isntype = ISN_COMPARESTRING; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_BLOB: isntype = ISN_COMPAREBLOB; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_LIST: isntype = ISN_COMPARELIST; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_TUPLE: isntype = ISN_COMPARETUPLE; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_DICT: isntype = ISN_COMPAREDICT; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_FUNC: isntype = ISN_COMPAREFUNC; ಭಙ್ಗ;
	    ಪಕ್ಷ VAR_OBJECT: isntype = ISN_COMPAREOBJECT; ಭಙ್ಗ;
	    ಸ್ವಭಾವ: isntype = ISN_COMPAREANY; ಭಙ್ಗ;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_ಸಂಖ್ಯೆ || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_ಸಂಖ್ಯೆ || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    ಅನ್ಯಥಾ ಯದಿ (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	ಯದಿ ((vartype1 == VAR_SPECIAL
		&& (tv1 != ರಿಕ್ತ ? tv1->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != ರಿಕ್ತ ? tv2->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    ಪ್ರತ್ಯಾಗಮ ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    ಯದಿ ((exprtype == ಅಭಿವ್ಯಕ್ತಿ_IS || exprtype == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == ಅಭಿವ್ಯಕ್ತಿ_IS ? "is" : "isnot" , vartype_name(vartype1));
	ಪ್ರತ್ಯಾಗಮ ISN_DROP;
    }
    ಯದಿ (!(exprtype == ಅಭಿವ್ಯಕ್ತಿ_IS || exprtype == ಅಭಿವ್ಯಕ್ತಿ_ISNOT
		|| exprtype == ಅಭಿವ್ಯಕ್ತಿ_EQUAL || exprtype == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	ಪ್ರತ್ಯಾಗಮ ISN_DROP;
    }
    ಯದಿ (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != ಅಭಿವ್ಯಕ್ತಿ_EQUAL
			&& exprtype != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != ಅಭಿವ್ಯಕ್ತಿ_EQUAL
			 && exprtype != ಅಭಿವ್ಯಕ್ತಿ_NEQUAL
			 && exprtype != ಅಭಿವ್ಯಕ್ತಿ_IS
			 && exprtype != ಅಭಿವ್ಯಕ್ತಿ_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	ಪ್ರತ್ಯಾಗಮ ISN_DROP;
    }
    ಪ್ರತ್ಯಾಗಮ isntype;
}

    ಪೂರ್ಣಾಙ್ಕ
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    ಯದಿ (get_compare_isn(type, tv1, tv2, ರಿಕ್ತ, ರಿಕ್ತ) == ISN_DROP)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, ಪೂರ್ಣಾಙ್ಕ ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, ರಿಕ್ತ, ರಿಕ್ತ,
		       get_type_on_ರಾಶಿ(cctx, 1), get_type_on_ರಾಶಿ(cctx, 0));
    ಯದಿ (isntype == ISN_DROP)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ ((isn = generate_instr(cctx, isntype)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --ರಾಶಿ->ga_len;
    set_type_on_ರಾಶಿ(cctx, &t_bool, 0);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_CONCAT(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    RETURN_OK_IF_SKIP(cctx);

    ಯದಿ ((isn = generate_instr(cctx, ISN_CONCAT)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಎಣಿಕೆ;

    // drop the argument types
    ರಾಶಿ->ga_len -= ಎಣಿಕೆ - 1;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_2BOOL(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ invert, ಪೂರ್ಣಾಙ್ಕ offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_2BOOL)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_ರಾಶಿ(cctx, &t_bool, -1 - offset);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ (generate_instr(cctx, ISN_COND2BOOL) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // type becomes bool
    set_type_on_ರಾಶಿ(cctx, &t_bool, 0);

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	ಪೂರ್ಣಾಙ್ಕ	    typechk_flags,	// type check flags
	ಪೂರ್ಣಾಙ್ಕ	    offset,
	ಪೂರ್ಣಾಙ್ಕ	    is_var,
	ಪೂರ್ಣಾಙ್ಕ	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    type_T *tt;
    ಯದಿ ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_ಸಂಖ್ಯೆ_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	ಯದಿ (tt != ರಿಕ್ತ)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    ಯದಿ (typechk_flags & TYPECHK_ಸಂಖ್ಯೆ_OK)
		tt->tt_flags |= TTFLAG_ಸಂಖ್ಯೆ_OK;
	    ಅನ್ಯಥಾ
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    ಅನ್ಯಥಾ
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_ರಾಶಿ(cctx, expected, -1 - offset);

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_SETTYPE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == ರಿಕ್ತ ? &t_any : &class->class_type);
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.classarg = class;
    ಯದಿ (class != ರಿಕ್ತ)
	++class->class_refcount;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_tv_ತಳ್ಳು(cctx_T *cctx, typval_T *tv)
{
    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_ಸಂಖ್ಯೆ);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_ಸಂಖ್ಯೆ);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    generate_PUSHNR(cctx, tv->vval.v_ಸಂಖ್ಯೆ);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_LIST:
	    ಯದಿ (tv->vval.v_list != ರಿಕ್ತ)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    ಯದಿ (tv->vval.v_tuple != ರಿಕ್ತ)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    ಯದಿ (tv->vval.v_dict != ರಿಕ್ತ)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    ಭಙ್ಗ;
#ifdef FEAT_JOB_CHANNEL
	ಪಕ್ಷ VAR_JOB:
	    ಯದಿ (tv->vval.v_job != ರಿಕ್ತ)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CHANNEL:
	    ಯದಿ (tv->vval.v_channel != ರಿಕ್ತ)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_FUNC:
	    ಯದಿ (tv->vval.v_string != ರಿಕ್ತ)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, ರಿಕ್ತ, &t_func_unknown, TRUE);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_PARTIAL:
	    ಯದಿ (tv->vval.v_partial != ರಿಕ್ತ)
		iemsg("non-null partial constant not supported");
	    ಯದಿ (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = ರಿಕ್ತ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_OBJECT:
	    ಯದಿ (tv->vval.v_object != ರಿಕ್ತ)
	    {
		emsg(_(e_cannot_use_non_null_object));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHNR(cctx_T *cctx, varnumber_T ಸಂಖ್ಯೆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_ಸಂಖ್ಯೆ)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ;

    ಯದಿ (ಸಂಖ್ಯೆ == 0 || ಸಂಖ್ಯೆ == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_ರಾಶಿ(cctx, &t_ಸಂಖ್ಯೆ_bool, 0);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHBOOL(cctx_T *cctx, varnumber_T ಸಂಖ್ಯೆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHSPEC(cctx_T *cctx, varnumber_T ಸಂಖ್ಯೆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     ಸಂಖ್ಯೆ == VVAL_NULL ? &t_null : &t_none)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.fnumber = fnumber;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	ಯದಿ ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == ರಿಕ್ತ)
	    ret = FAIL;
	ಅನ್ಯಥಾ
	{
	    isn->isn_arg.string = str == ರಿಕ್ತ ? ರಿಕ್ತ : *str;
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
    }
    ಯದಿ (str != ರಿಕ್ತ)
	VIM_CLEAR(*str);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    ಯದಿ (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
#ಅನ್ಯಥಾ
    emsg(_(e_channel_job_feature_not_available));
    ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    ಯದಿ (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
#ಅನ್ಯಥಾ
    emsg(_(e_channel_job_feature_not_available));
    ಪ್ರತ್ಯಾಗಮ FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.blob = blob;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, ಪೂರ್ಣಾಙ್ಕ may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (name == ರಿಕ್ತ)
	funcname = ರಿಕ್ತ;
    ಅನ್ಯಥಾ ಯದಿ (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != ರಿಕ್ತ)	    // autoload
	funcname = vim_strsave(name);
    ಅನ್ಯಥಾ
    {
	funcname = alloc(STRLEN(name) + 3);
	ಯದಿ (funcname != ರಿಕ್ತ)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.string = vim_strsave(name);
    ಯದಿ (isn->isn_arg.string == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_GETITEM(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ index, ಪೂರ್ಣಾಙ್ಕ with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_ರಾಶಿ(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    ಯದಿ ((isn = generate_instr(cctx, ISN_GETITEM)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_ರಾಶಿ(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_tuple_slice_type_on_ರಾಶಿ(type_T *type, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    ಯದಿ ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= ಎಣಿಕೆ)
    {
	set_type_on_ರಾಶಿ(cctx, &t_tuple_any, 0);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ga_init2(&tuple_types_ga, ಪರಿಮಾಣ(type_T *), 10);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = ಎಣಿಕೆ; i < type->tt_argcount; i++)
    {
	ಯದಿ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_ರಾಶಿ(cctx, &t_tuple_any, 0);
	    ಪ್ರತ್ಯಾಗಮ;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] =
							     type->tt_args[i];
	tuple_types_ga.ga_len++;
    }

    set_type_on_ರಾಶಿ(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_SLICE(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_SLICE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಎಣಿಕೆ;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_ರಾಶಿ(cctx, 0);
    ಯದಿ (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_ರಾಶಿ(type, ಎಣಿಕೆ, cctx);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_CHECKLEN(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ min_len, ಪೂರ್ಣಾಙ್ಕ more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    ಯದಿ ((isn = generate_instr(cctx, ISN_CHECKLEN)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_STORE(cctx_T *cctx, isntype_T isn_type, ಪೂರ್ಣಾಙ್ಕ idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (name != ರಿಕ್ತ)
	isn->isn_arg.string = vim_strsave(name);
    ಅನ್ಯಥಾ
	isn->isn_arg.ಸಂಖ್ಯೆ = idx;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	ಪೂರ್ಣಾಙ್ಕ	    load,
	class_T	    *cl,
	ಪೂರ್ಣಾಙ್ಕ	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    ಅನ್ಯಥಾ
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
generate_STOREOUTER(
	cctx_T	    *cctx,
	ಪೂರ್ಣಾಙ್ಕ	    idx,
	ಪೂರ್ಣಾಙ್ಕ	    level,
	ಪೂರ್ಣಾಙ್ಕ	    loop_depth,
	ಪೂರ್ಣಾಙ್ಕ	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    ಅನ್ಯಥಾ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_STORENR(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ idx, varnumber_T ಮೌಲ್ಯ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_STORENR)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = ಮೌಲ್ಯ;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ಪೂರ್ಣಾಙ್ಕ	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ಪೂರ್ಣಾಙ್ಕ	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_type2(cctx, isn_type, type, type)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (name != ರಿಕ್ತ)
	isn->isn_arg.string = vim_strsave(name);
    ಅನ್ಯಥಾ
	isn->isn_arg.ಸಂಖ್ಯೆ = idx;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_LOADOUTER(
	cctx_T	    *cctx,
	ಪೂರ್ಣಾಙ್ಕ	    idx,
	ಪೂರ್ಣಾಙ್ಕ	    nesting,
	ಪೂರ್ಣಾಙ್ಕ	    loop_depth,
	ಪೂರ್ಣಾಙ್ಕ	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    ಅನ್ಯಥಾ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    ಪೂರ್ಣಾಙ್ಕ	    di_flags;
    ಪೂರ್ಣಾಙ್ಕ	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    ಪ್ರತ್ಯಾಗಮ generate_LOAD(cctx, ISN_LOADV, vidx, ರಿಕ್ತ, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, ಪೂರ್ಣಾಙ್ಕ forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, isn_type)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ (generate_instr(cctx, ISN_LOCKCONST) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ಪೂರ್ಣಾಙ್ಕ	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    ಅನ್ಯಥಾ
	isn = generate_instr_drop(cctx, isn_type, 1);
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ಪೂರ್ಣಾಙ್ಕ	    sid,
	ಪೂರ್ಣಾಙ್ಕ	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    ಅನ್ಯಥಾ
	isn = generate_instr_drop(cctx, isn_type, 1);
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    ಯದಿ (sref == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_NEWLIST(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_NEWLIST)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = use_null ? -1 : ಎಣಿಕೆ;

    // Get the member type and the declared member type from all the items on
    // the stack.
    ಯದಿ ((member_type = get_member_type_from_ರಾಶಿ(ಎಣಿಕೆ, 1, cctx)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_ರಾಶಿ.ga_len -= ಎಣಿಕೆ;

    // add the list type to the type stack
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_NEWTUPLE(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = use_null ? -1 : ಎಣಿಕೆ;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, ಪರಿಮಾಣ(type_T *), 10);

    ಯದಿ (get_tuple_type_from_ರಾಶಿ(ಎಣಿಕೆ, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_ರಾಶಿ.ga_len -= ಎಣಿಕೆ;

    // add the tuple type to the type stack
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_NEWDICT(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_NEWDICT)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = use_null ? -1 : ಎಣಿಕೆ;

    ಯದಿ ((member_type = get_member_type_from_ರಾಶಿ(ಎಣಿಕೆ, 2, cctx)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_ರಾಶಿ.ga_len -= 2 * ಎಣಿಕೆ;

    // add the dict type to the type stack
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ಪೂರ್ಣಾಙ್ಕ	    object_method,
	ಪೂರ್ಣಾಙ್ಕ	    fi,
	ಪೂರ್ಣಾಙ್ಕ	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    ಪೂರ್ಣಾಙ್ಕ		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_FUNCREF)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (isn_idx != ರಿಕ್ತ)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    ಯದಿ (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != ರಿಕ್ತ)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	ಯದಿ (extra == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	ಯದಿ (cl != ರಿಕ್ತ)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    ಯದಿ (ufunc->uf_def_status == UF_NOT_COMPILED || cl != ರಿಕ್ತ)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ಯದಿ (ufunc->uf_def_status != UF_NOT_COMPILED && cl == ರಿಕ್ತ)
    {
	ಯದಿ (isn_idx == ರಿಕ್ತ && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (ಶೂನ್ಯ)compile_def_function(ufunc, FALSE, CT_NONE, ರಿಕ್ತ);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_closure = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    ಯದಿ ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == ರಿಕ್ತ ? &t_func_any : ufunc->uf_func_type;
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_ರಾಶಿ(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	ಯದಿ ((isn = generate_instr(cctx, ISN_NEWFUNC)) == ರಿಕ್ತ)
	    ret = FAIL;
	ಅನ್ಯಥಾ
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    ಯದಿ (arg == ರಿಕ್ತ)
		ret = FAIL;
	    ಅನ್ಯಥಾ
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(ಶೂನ್ಯ)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		ಪ್ರತ್ಯಾಗಮ OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_DEF)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	ಯದಿ (isn->isn_arg.string == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_JUMP(cctx_T *cctx, jumpwhen_T when, ಪೂರ್ಣಾಙ್ಕ where)
{
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_JUMP)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    ಯದಿ (when != JUMP_ALWAYS && ರಾಶಿ->ga_len > 0)
	--ರಾಶಿ->ga_len;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_WHILE(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ funcref_idx)
{
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_WHILE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    ಯದಿ (ರಾಶಿ->ga_len > 0)
	--ರಾಶಿ->ga_len;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, ಪೂರ್ಣಾಙ್ಕ arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, isn_type)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
generate_FOR(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_FOR)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_any);
}

    ಪೂರ್ಣಾಙ್ಕ
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_ENDLOOP)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_ಎಣಿಕೆ;
    isn->isn_arg.endloop.end_var_ಎಣಿಕೆ =
			   cctx->ctx_locals.ga_len - loop_info->li_local_ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_TRYCONT(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ levels, ಪೂರ್ಣಾಙ್ಕ where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_TRYCONT)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_internal_func_args(
	cctx_T	*cctx,
	ಪೂರ್ಣಾಙ್ಕ	func_idx,
	ಪೂರ್ಣಾಙ್ಕ	argcount,
	ಪೂರ್ಣಾಙ್ಕ	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    ಪೂರ್ಣಾಙ್ಕ		argoff = check_internal_func(func_idx, argcount);

    ಯದಿ (argoff < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (method_call && argoff > 1)
    {
	ಯದಿ (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	ಯದಿ (isn  == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    ಯದಿ (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)ರಾಶಿ->ga_data) + ರಾಶಿ->ga_len - argcount;

	// Check the types of the arguments.
	ಯದಿ (method_call && argoff > 1)
	{
	    ಪೂರ್ಣಾಙ್ಕ i;

	    ಪ್ರತಿ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ i;

	    ಪ್ರತಿ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ಯದಿ (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_BCALL(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ func_idx, ಪೂರ್ಣಾಙ್ಕ argcount, ಪೂರ್ಣಾಙ್ಕ method_call)
{
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*argtypes = ರಿಕ್ತ;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = ರಿಕ್ತ;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    ಯದಿ (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (internal_func_is_map(func_idx))
	maptype = argtypes;

    ಯದಿ ((isn = generate_instr(cctx, ISN_BCALL)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    ರಾಶಿ->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    ಯದಿ (ತಳ್ಳು_type_stack2(cctx, type, decl_type) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (maptype != ರಿಕ್ತ && maptype[0].type_decl->tt_member != ರಿಕ್ತ
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_ರಾಶಿ(cctx, 1);
    ಯದಿ (arg_type_modifiable(list_type, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    item_type = get_type_on_ರಾಶಿ(cctx, 0);
    expected = list_type->tt_member;
    ಯದಿ (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (generate_instr(cctx, ISN_LISTAPPEND) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    --cctx->ctx_type_ರಾಶಿ.ga_len;	    // drop the argument
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    ಯದಿ (arg_type_modifiable(get_decl_type_on_ರಾಶಿ(cctx, 1), 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    item_type = get_type_on_ರಾಶಿ(cctx, 0);
    ಯದಿ (need_type(item_type, &t_ಸಂಖ್ಯೆ, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (generate_instr(cctx, ISN_BLOBAPPEND) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    --cctx->ctx_type_ರಾಶಿ.ga_len;	    // drop the argument
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    ಸ್ಥಿರ isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    ಯದಿ (cl != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ISN_METHODCALL;

    ಯದಿ (ufunc->uf_def_status == UF_NOT_COMPILED)
	ಪ್ರತ್ಯಾಗಮ ISN_UCALL;

    // function invoked from a lambda
    ಯದಿ (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	ಯದಿ (ufunc->uf_name[0] == K_SPECIAL)
	    ಪ್ರತ್ಯಾಗಮ ISN_UCALL;
    }

    ಪ್ರತ್ಯಾಗಮ ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ಪೂರ್ಣಾಙ್ಕ	    mi,
	ಪೂರ್ಣಾಙ್ಕ	    pushed_argcount,
	ಪೂರ್ಣಾಙ್ಕ	    is_super)
{
    isn_T	*isn;
    ಪೂರ್ಣಾಙ್ಕ		regular_args = ufunc->uf_args.ga_len;
    ಪೂರ್ಣಾಙ್ಕ		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	ಪೂರ್ಣಾಙ್ಕ		i;
	compiletype_T	compile_type;

	ಪ್ರತಿ (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_ರಾಶಿ(cctx, argcount - i - 1);
	    ಯದಿ (check_type_is_ಮೌಲ್ಯ(actual) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		ಅನುವರ್ತ;
	    }
	    ಯದಿ (i < regular_args)
	    {
		ಯದಿ (ufunc->uf_arg_types == ರಿಕ್ತ)
		    ಅನುವರ್ತ;
		expected = ufunc->uf_arg_types[i];
	    }
	    ಅನ್ಯಥಾ ಯದಿ (ufunc->uf_va_type == ರಿಕ್ತ
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    ಅನ್ಯಥಾ
		expected = ufunc->uf_va_type->tt_member;
	    ಯದಿ (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	ಯದಿ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == ರಿಕ್ತ,
						   compile_type, ರಿಕ್ತ) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (cl != ರಿಕ್ತ /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	ಯದಿ (isn->isn_arg.mfunc == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = ರಿಕ್ತ;
	ಯದಿ (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = ರಿಕ್ತ;
	isn->isn_arg.mfunc->cmf_cache_generic = ರಿಕ್ತ;
	isn->isn_arg.mfunc->cmf_cache_ufunc = ರಿಕ್ತ;
    }
    ಅನ್ಯಥಾ ಯದಿ (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    ಅನ್ಯಥಾ
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_ರಾಶಿ.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    ಯದಿ (ufunc->uf_class != ರಿಕ್ತ)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_ರಾಶಿ(cctx, 0);
	ಯದಿ (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_ರಾಶಿ.ga_len--;
    }

    // add return type
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_ರಾಶಿ(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_UCALL(cctx_T *cctx, char_u *name, ಪೂರ್ಣಾಙ್ಕ argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_UCALL)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_ರಾಶಿ.ga_len -= argcount;

    // add return value
    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    ಪೂರ್ಣಾಙ್ಕ
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	ಪೂರ್ಣಾಙ್ಕ	argcount,
	ಪೂರ್ಣಾಙ್ಕ	at_ತುದಿ,
	char_u	*name)
{
    ಯದಿ (type->tt_argcount != -1)
    {
	ಪೂರ್ಣಾಙ್ಕ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	ಯದಿ (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (type->tt_args != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ i;

	    ಪ್ರತಿ (i = 0; i < argcount; ++i)
	    {
		ಪೂರ್ಣಾಙ್ಕ	offset = -argcount + i - (at_ತುದಿ ? 0 : 1);
		type_T	*actual = get_type_on_ರಾಶಿ(cctx, -1 - offset);
		type_T	*expected;

		ಯದಿ (check_type_is_ಮೌಲ್ಯ(actual) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    ಯದಿ (expected != ರಿಕ್ತ && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    ಯದಿ (expected == ರಿಕ್ತ)
			expected = &t_any;
		}
		ಅನ್ಯಥಾ ಯದಿ (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		ಅನ್ಯಥಾ
		    expected = type->tt_args[i];
		ಯದಿ (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		}
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PCALL(
	cctx_T	*cctx,
	ಪೂರ್ಣಾಙ್ಕ	argcount,
	char_u	*name,
	type_T	*type,
	ಪೂರ್ಣಾಙ್ಕ	at_ತುದಿ)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    ಯದಿ (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    ಅನ್ಯಥಾ ಯದಿ (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	ಯದಿ (check_func_args_from_type(cctx, type, argcount, at_ತುದಿ, name)
								       == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ret_type = type->tt_member;
	ಯದಿ (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    ಅನ್ಯಥಾ
    {
	semsg(_(e_not_callable_type_str), name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ ((isn = generate_instr(cctx, ISN_PCALL)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.pfunc.cpf_ತುದಿ = at_ತುದಿ;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_ರಾಶಿ.ga_len -= argcount + 1;

    // push the return value
    ತಳ್ಳು_type_ರಾಶಿ(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    ಯದಿ (at_ತುದಿ && generate_instr(cctx, ISN_PCALL_END) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_DEFER(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ var_idx, ಪೂರ್ಣಾಙ್ಕ argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_ರಾಶಿ(cctx, 0);
    ಯದಿ (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	ವರ್ಣ *tofree;

	semsg(_(e_expected_dictionary_for_using_ಕೀಲಿ_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    // change dict type to dict member type
    ಯದಿ (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_ರಾಶಿ(cctx, ntype, 0);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_ECHO(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ with_white, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_drop(cctx, ISN_ECHO, ಎಣಿಕೆ)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_ಎಣಿಕೆ = ಎಣಿಕೆ;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx_T *cctx, isntype_T isn_type, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr_drop(cctx, isn_type, ಎಣಿಕೆ)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_ECHOWINDOW(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, long time)
{
    isn_T	*isn;

    ಯದಿ ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, ಎಣಿಕೆ)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.echowin.ewin_ಎಣಿಕೆ = ಎಣಿಕೆ;
    isn->isn_arg.echowin.ewin_time = time;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_SOURCE(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ sid)
{
    isn_T	*isn;

    ಯದಿ ((isn = generate_instr(cctx, ISN_SOURCE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = sid;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_PUT(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ regname, linenr_T lnum, ಪೂರ್ಣಾಙ್ಕ fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    ಯದಿ (isn == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_LOCKUNLOCK(cctx_T *cctx, char_u *ಸಾಲು, ಪೂರ್ಣಾಙ್ಕ is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    class_T *cl = cctx->ctx_ufunc != ರಿಕ್ತ ? cctx->ctx_ufunc->uf_class : ರಿಕ್ತ;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(ಸಾಲು);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    ಯದಿ (cl != ರಿಕ್ತ)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *ಸಾಲು)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, isntype)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.string = vim_strsave(ಸಾಲು);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    ಪೂರ್ಣಾಙ್ಕ		ret = OK;

    ಯದಿ (cctx->ctx_skip != SKIP_YES)
    {
	ಯದಿ ((isn = generate_instr(cctx, isntype)) == ರಿಕ್ತ)
	    ret = FAIL;
	ಅನ್ಯಥಾ
	{
	    isn->isn_arg.string = str;
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
    }
    vim_free(str);
    ಪ್ರತ್ಯಾಗಮ ret;
}

    ಪೂರ್ಣಾಙ್ಕ
generate_LEGACY_EVAL(cctx_T *cctx, char_u *ಸಾಲು)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.string = vim_strsave(ಸಾಲು);

    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_any);
}

    ಪೂರ್ಣಾಙ್ಕ
generate_EXECCONCAT(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    isn_T	*isn;

    ಯದಿ ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, ಎಣಿಕೆ)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    ಯದಿ ((isn = generate_instr(cctx, ISN_RANGE)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.string = range;

    ಪ್ರತ್ಯಾಗಮ ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_ಸಂಖ್ಯೆ);
}

    ಪೂರ್ಣಾಙ್ಕ
generate_UNPACK(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ var_ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_UNPACK)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.unpack.unp_ಎಣಿಕೆ = var_ಎಣಿಕೆ;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    ಯದಿ (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	ಯದಿ ((isn = generate_instr(cctx, ISN_CMDMOD)) == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	ಯದಿ (isn->isn_arg.cmdmod.cf_cmdmod == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, ಪರಿಮಾಣ(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
generate_undo_cmdmods(cctx_T *cctx)
{
    ಯದಿ (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	ಪೂರ್ಣಾಙ್ಕ		opt_flags,
	ಪೂರ್ಣಾಙ್ಕ		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    ವಿಕಲ್ಪ (dest)
    {
	ಪಕ್ಷ dest_option:
	    ಪ್ರತ್ಯಾಗಮ generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	ಪಕ್ಷ dest_func_option:
	    ಪ್ರತ್ಯಾಗಮ generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	ಪಕ್ಷ dest_global:
	    // include g: with the name, easier to execute that way
	    ಪ್ರತ್ಯಾಗಮ generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == ರಿಕ್ತ
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	ಪಕ್ಷ dest_buffer:
	    // include b: with the name, easier to execute that way
	    ಪ್ರತ್ಯಾಗಮ generate_STORE(cctx, ISN_STOREB, 0, name);
	ಪಕ್ಷ dest_window:
	    // include w: with the name, easier to execute that way
	    ಪ್ರತ್ಯಾಗಮ generate_STORE(cctx, ISN_STOREW, 0, name);
	ಪಕ್ಷ dest_tab:
	    // include t: with the name, easier to execute that way
	    ಪ್ರತ್ಯಾಗಮ generate_STORE(cctx, ISN_STORET, 0, name);
	ಪಕ್ಷ dest_env:
	    ಪ್ರತ್ಯಾಗಮ generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	ಪಕ್ಷ dest_reg:
	    ಪ್ರತ್ಯಾಗಮ generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], ರಿಕ್ತ);
	ಪಕ್ಷ dest_vimvar:
	    ಪ್ರತ್ಯಾಗಮ generate_STORE(cctx, ISN_STOREV, vimvaridx, ರಿಕ್ತ);
	ಪಕ್ಷ dest_script:
	ಪಕ್ಷ dest_script_v9:
	    {
		ಪೂರ್ಣಾಙ್ಕ	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		ಪೂರ್ಣಾಙ್ಕ	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		ಯದಿ (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    ಯದಿ (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == ರಿಕ್ತ)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			ಯದಿ (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    ಪ್ರತ್ಯಾಗಮ FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    ಪ್ರತ್ಯಾಗಮ generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		ಪ್ರತ್ಯಾಗಮ generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	ಪಕ್ಷ dest_class_member:
	    ಪ್ರತ್ಯಾಗಮ generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	ಪಕ್ಷ dest_local:
	ಪಕ್ಷ dest_ಅಭಿವ್ಯಕ್ತಿ:
	    // cannot happen
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    ಪೂರ್ಣಾಙ್ಕ
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    ಪ್ರತಿ (;;)
    {
	ಯದಿ (scope == ರಿಕ್ತ)
	    ಭಙ್ಗ;
	ಯದಿ (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	scope = scope->se_outer;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಪೂರ್ಣಾಙ್ಕ
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, ಪೂರ್ಣಾಙ್ಕ instr_ಎಣಿಕೆ, ಪೂರ್ಣಾಙ್ಕ is_decl)
{
    ಯದಿ (lhs->lhs_dest != dest_local)
	ಪ್ರತ್ಯಾಗಮ generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    ಯದಿ (lhs->lhs_lvar == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    ಯದಿ (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_ಎಣಿಕೆ + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.ಸಂಖ್ಯೆ;
	garray_T    *ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

	ಯದಿ (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	ಅನ್ಯಥಾ
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	ಯದಿ (ರಾಶಿ->ga_len > 0)
	    --ರಾಶಿ->ga_len;
    }
    ಅನ್ಯಥಾ ಯದಿ (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    ಅನ್ಯಥಾ
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, ರಿಕ್ತ);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    ಪೂರ್ಣಾಙ್ಕ
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಯದಿ ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    isn->isn_arg.setsctx = new_sctx;
    ಪ್ರತ್ಯಾಗಮ OK;
}

#ಯದಿ defined(FEAT_PROFILE)
    ಶೂನ್ಯ
may_generate_prof_end(cctx_T *cctx, ಪೂರ್ಣಾಙ್ಕ prof_lnum)
{
    ಯದಿ (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_END);
}
#endif


/*
 * Delete an instruction, free what it contains.
 */
    ಶೂನ್ಯ
delete_instr(isn_T *isn)
{
    ವಿಕಲ್ಪ (isn->isn_type)
    {
	ಪಕ್ಷ ISN_AUTOLOAD:
	ಪಕ್ಷ ISN_DEF:
	ಪಕ್ಷ ISN_EXEC:
	ಪಕ್ಷ ISN_EXECRANGE:
	ಪಕ್ಷ ISN_EXEC_SPLIT:
	ಪಕ್ಷ ISN_LEGACY_EVAL:
	ಪಕ್ಷ ISN_LOADAUTO:
	ಪಕ್ಷ ISN_LOADB:
	ಪಕ್ಷ ISN_LOADENV:
	ಪಕ್ಷ ISN_LOADG:
	ಪಕ್ಷ ISN_LOADOPT:
	ಪಕ್ಷ ISN_LOADT:
	ಪಕ್ಷ ISN_LOADW:
	ಪಕ್ಷ ISN_PUSHEXC:
	ಪಕ್ಷ ISN_PUSHFUNC:
	ಪಕ್ಷ ISN_PUSHS:
	ಪಕ್ಷ ISN_RANGE:
	ಪಕ್ಷ ISN_STOREAUTO:
	ಪಕ್ಷ ISN_STOREB:
	ಪಕ್ಷ ISN_STOREENV:
	ಪಕ್ಷ ISN_STOREG:
	ಪಕ್ಷ ISN_STORET:
	ಪಕ್ಷ ISN_STOREW:
	ಪಕ್ಷ ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_SUBSTITUTE:
	    {
		ಪೂರ್ಣಾಙ್ಕ	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		ಪ್ರತಿ (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_INSTR:
	    {
		ಪೂರ್ಣಾಙ್ಕ	idx;
		isn_T	*list = isn->isn_arg.instr;

		ಪ್ರತಿ (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_LOADS:
	ಪಕ್ಷ ISN_LOADEXPORT:
	ಪಕ್ಷ ISN_STORES:
	ಪಕ್ಷ ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_UNLET:
	ಪಕ್ಷ ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_STOREOPT:
	ಪಕ್ಷ ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		ಯದಿ (extra == ರಿಕ್ತ || extra->fre_func_name == ರಿಕ್ತ)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    ಯದಿ (ufunc != ರಿಕ್ತ && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		ಯದಿ (extra != ರಿಕ್ತ)
		{
		    char_u *name = extra->fre_func_name;

		    ಯದಿ (name != ರಿಕ್ತ)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    ಯದಿ (extra->fre_class != ರಿಕ್ತ)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		ಯದಿ (dfunc->df_ufunc != ರಿಕ್ತ
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		ಯದಿ (arg != ರಿಕ್ತ)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    ಯದಿ (ufunc != ರಿಕ್ತ)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_CHECKTYPE:
	ಪಕ್ಷ ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_LOADSCRIPT:
	ಪಕ್ಷ ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_LOAD_CLASSMEMBER:
	ಪಕ್ಷ ISN_STORE_CLASSMEMBER:
	ಪಕ್ಷ ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    ಭಙ್ಗ;

	ಪಕ್ಷ ISN_2BOOL:
	ಪಕ್ಷ ISN_2STRING:
	ಪಕ್ಷ ISN_2STRING_ANY:
	ಪಕ್ಷ ISN_ADDBLOB:
	ಪಕ್ಷ ISN_ADDLIST:
	ಪಕ್ಷ ISN_ADDTUPLE:
	ಪಕ್ಷ ISN_ANYINDEX:
	ಪಕ್ಷ ISN_ANYSLICE:
	ಪಕ್ಷ ISN_BCALL:
	ಪಕ್ಷ ISN_BLOBAPPEND:
	ಪಕ್ಷ ISN_BLOBINDEX:
	ಪಕ್ಷ ISN_BLOBSLICE:
	ಪಕ್ಷ ISN_CATCH:
	ಪಕ್ಷ ISN_CEXPR_AUCMD:
	ಪಕ್ಷ ISN_CHECKLEN:
	ಪಕ್ಷ ISN_CLEARDICT:
	ಪಕ್ಷ ISN_CMDMOD_REV:
	ಪಕ್ಷ ISN_COMPAREANY:
	ಪಕ್ಷ ISN_COMPAREBLOB:
	ಪಕ್ಷ ISN_COMPAREBOOL:
	ಪಕ್ಷ ISN_COMPAREDICT:
	ಪಕ್ಷ ISN_COMPAREFLOAT:
	ಪಕ್ಷ ISN_COMPAREFUNC:
	ಪಕ್ಷ ISN_COMPARELIST:
	ಪಕ್ಷ ISN_COMPARETUPLE:
	ಪಕ್ಷ ISN_COMPARENR:
	ಪಕ್ಷ ISN_COMPARENULL:
	ಪಕ್ಷ ISN_COMPAREOBJECT:
	ಪಕ್ಷ ISN_COMPARESPECIAL:
	ಪಕ್ಷ ISN_COMPARESTRING:
	ಪಕ್ಷ ISN_CONCAT:
	ಪಕ್ಷ ISN_CONSTRUCT:
	ಪಕ್ಷ ISN_COND2BOOL:
	ಪಕ್ಷ ISN_DEBUG:
	ಪಕ್ಷ ISN_DEFER:
	ಪಕ್ಷ ISN_DROP:
	ಪಕ್ಷ ISN_ECHO:
	ಪಕ್ಷ ISN_ECHOCONSOLE:
	ಪಕ್ಷ ISN_ECHOERR:
	ಪಕ್ಷ ISN_ECHOMSG:
	ಪಕ್ಷ ISN_ECHOWINDOW:
	ಪಕ್ಷ ISN_ENDLOOP:
	ಪಕ್ಷ ISN_ENDTRY:
	ಪಕ್ಷ ISN_EXECCONCAT:
	ಪಕ್ಷ ISN_EXECUTE:
	ಪಕ್ಷ ISN_FINALLY:
	ಪಕ್ಷ ISN_FINISH:
	ಪಕ್ಷ ISN_FOR:
	ಪಕ್ಷ ISN_GETITEM:
	ಪಕ್ಷ ISN_GET_OBJ_MEMBER:
	ಪಕ್ಷ ISN_JUMP:
	ಪಕ್ಷ ISN_JUMP_IF_ARG_NOT_SET:
	ಪಕ್ಷ ISN_JUMP_IF_ARG_SET:
	ಪಕ್ಷ ISN_LISTAPPEND:
	ಪಕ್ಷ ISN_LISTINDEX:
	ಪಕ್ಷ ISN_LISTSLICE:
	ಪಕ್ಷ ISN_TUPLEINDEX:
	ಪಕ್ಷ ISN_TUPLESLICE:
	ಪಕ್ಷ ISN_LOAD:
	ಪಕ್ಷ ISN_LOADBDICT:
	ಪಕ್ಷ ISN_LOADGDICT:
	ಪಕ್ಷ ISN_LOADOUTER:
	ಪಕ್ಷ ISN_LOADREG:
	ಪಕ್ಷ ISN_LOADTDICT:
	ಪಕ್ಷ ISN_LOADV:
	ಪಕ್ಷ ISN_LOADWDICT:
	ಪಕ್ಷ ISN_LOCKCONST:
	ಪಕ್ಷ ISN_MEMBER:
	ಪಕ್ಷ ISN_NEGATENR:
	ಪಕ್ಷ ISN_NEWDICT:
	ಪಕ್ಷ ISN_NEWLIST:
	ಪಕ್ಷ ISN_NEWTUPLE:
	ಪಕ್ಷ ISN_NEWPARTIAL:
	ಪಕ್ಷ ISN_OPANY:
	ಪಕ್ಷ ISN_OPFLOAT:
	ಪಕ್ಷ ISN_OPNR:
	ಪಕ್ಷ ISN_PCALL:
	ಪಕ್ಷ ISN_PCALL_END:
	ಪಕ್ಷ ISN_PROF_END:
	ಪಕ್ಷ ISN_PROF_START:
	ಪಕ್ಷ ISN_PUSHBOOL:
	ಪಕ್ಷ ISN_PUSHCHANNEL:
	ಪಕ್ಷ ISN_PUSHF:
	ಪಕ್ಷ ISN_PUSHJOB:
	ಪಕ್ಷ ISN_PUSHNR:
	ಪಕ್ಷ ISN_PUSHOBJ:
	ಪಕ್ಷ ISN_PUSHSPEC:
	ಪಕ್ಷ ISN_PUT:
	ಪಕ್ಷ ISN_IPUT:
	ಪಕ್ಷ ISN_REDIREND:
	ಪಕ್ಷ ISN_REDIRSTART:
	ಪಕ್ಷ ISN_RETURN:
	ಪಕ್ಷ ISN_RETURN_OBJECT:
	ಪಕ್ಷ ISN_RETURN_VOID:
	ಪಕ್ಷ ISN_SHUFFLE:
	ಪಕ್ಷ ISN_SLICE:
	ಪಕ್ಷ ISN_SOURCE:
	ಪಕ್ಷ ISN_STORE:
	ಪಕ್ಷ ISN_STORENR:
	ಪಕ್ಷ ISN_STOREOUTER:
	ಪಕ್ಷ ISN_STORE_THIS:
	ಪಕ್ಷ ISN_STORERANGE:
	ಪಕ್ಷ ISN_STOREREG:
	ಪಕ್ಷ ISN_STOREV:
	ಪಕ್ಷ ISN_STRINDEX:
	ಪಕ್ಷ ISN_STRSLICE:
	ಪಕ್ಷ ISN_THROW:
	ಪಕ್ಷ ISN_TRYCONT:
	ಪಕ್ಷ ISN_UNLETINDEX:
	ಪಕ್ಷ ISN_UNLETRANGE:
	ಪಕ್ಷ ISN_UNPACK:
	ಪಕ್ಷ ISN_USEDICT:
	ಪಕ್ಷ ISN_WHILE:
	ಪಕ್ಷ ISN_SCRIPTCTX_SET:
	// nothing allocated
	ಭಙ್ಗ;
    }
}

    ಶೂನ್ಯ
clear_instr_ga(garray_T *gap)
{
    ಪೂರ್ಣಾಙ್ಕ idx;

    ಪ್ರತಿ (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
