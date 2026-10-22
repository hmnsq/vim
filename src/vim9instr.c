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

#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_NULL_IF_SKIP(cctx) ಆದರೆ (cctx->ctx_skip == SKIP_YES) ಹಿಂತಿರುಗಿ ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು RETURN_OK_IF_SKIP(cctx) ಆದರೆ (cctx->ctx_skip == SKIP_YES) ಹಿಂತಿರುಗಿ OK

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
    ಆದರೆ (GA_GROW_FAILS(instr, 1))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    ಹಿಂತಿರುಗಿ isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, ಪೂರ್ಣಾಂಕ drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_ರಾಶಿ.ga_len -= drop;
    ಹಿಂತಿರುಗಿ generate_instr(cctx, isn_type);
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

    ಆದರೆ ((isn = generate_instr(cctx, isn_type)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ತಳ್ಳು_type_stack2(cctx, type == ಖಾಲಿ ? &t_any : type,
			       decl_type == ಖಾಲಿ ? &t_any : decl_type) == FAIL)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    ಹಿಂತಿರುಗಿ generate_instr_type2(cctx, isn_type, type, &t_any);
}

/*
 * Generate an ISN_DEBUG instruction.
 */
    isn_T *
generate_instr_debug(cctx_T *cctx)
{
    isn_T	*isn;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
					       + cctx->ctx_ufunc->uf_dfunc_idx;

    ಆದರೆ ((isn = generate_instr(cctx, ISN_DEBUG)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    isn->isn_arg.debug.dbg_var_ಹೆಸರುಗಳು_len = dfunc->df_var_ಹೆಸರುಗಳು.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    ಹಿಂತಿರುಗಿ isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    ಪೂರ್ಣಾಂಕ
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.construct.construct_ಗಾತ್ರ = ಗಾತ್ರ(object_T)
			       + cl->class_obj_member_ಎಣಿಕೆ * ಗಾತ್ರ(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    ಪೂರ್ಣಾಂಕ
generate_GET_OBJ_MEMBER(cctx_T *cctx, ಪೂರ್ಣಾಂಕ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    isn->isn_arg.classmember.cm_class = ಖಾಲಿ;
    isn->isn_arg.classmember.cm_idx = idx;
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    ಪೂರ್ಣಾಂಕ
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, ಪೂರ್ಣಾಂಕ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    ಪೂರ್ಣಾಂಕ
generate_STORE_THIS(cctx_T *cctx, ಪೂರ್ಣಾಂಕ idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    isn->isn_arg.ಸಂಖ್ಯೆ = idx;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    ಪೂರ್ಣಾಂಕ
may_generate_2STRING(ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ, ಪೂರ್ಣಾಂಕ tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_ರಾಶಿ(cctx, -1 - ಸ್ಥಾನಾಂತರ);

    ಆಯ್ಕೆ (type->tt_type)
    {
	// nothing to be done
	ಸ್ಥಿತಿ VAR_STRING: ಹಿಂತಿರುಗಿ OK;

	// conversion possible
	ಸ್ಥಿತಿ VAR_SPECIAL:
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	ಸ್ಥಿತಿ VAR_FLOAT:
			 ನಿಲ್ಲಿಸು;

	// conversion possible (with runtime check)
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 ನಿಲ್ಲಿಸು;

	// conversion possible when tolerant
	ಸ್ಥಿತಿ VAR_LIST:
	ಸ್ಥಿತಿ VAR_TUPLE:
	ಸ್ಥಿತಿ VAR_DICT:
			 ಆದರೆ (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     ನಿಲ್ಲಿಸು;
			 }
			 ಆದರೆ (tostring_flags & TOSTRING_INTERPOLATE)
			     ನಿಲ್ಲಿಸು;
			 // FALLTHROUGH

	// conversion not possible
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_BLOB:
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_PARTIAL:
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	ಸ್ಥಿತಿ VAR_CHANNEL:
	ಸ್ಥಿತಿ VAR_INSTR:
	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_OBJECT:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
			 ಗಮ್ಯ_string_error(type->tt_type);
			 ಹಿಂತಿರುಗಿ FAIL;
    }

    ಗಣ_type_on_ರಾಶಿ(cctx, &t_string, -1 - ಸ್ಥಾನಾಂತರ);
    ಆದರೆ ((isn = generate_instr(cctx, isntype)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.tostring.ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ;
    isn->isn_arg.tostring.flags = tostring_flags;

    ಹಿಂತಿರುಗಿ OK;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_ಸಂಖ್ಯೆ_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    ಆದರೆ (!((type1 == VAR_ಸಂಖ್ಯೆ || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_ಸಂಖ್ಯೆ || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	ಆದರೆ (check_type_is_ಮೌಲ್ಯ(typ1) == FAIL
		|| check_type_is_ಮೌಲ್ಯ(typ2) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (*op == '+')
	    emsg(_(e_wrong_ವಾದ_type_for_plus));
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_char_requires_ಸಂಖ್ಯೆ_or_float_arguments), *op);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < tuple_type->tt_argcount; i++)
    {
	ಆದರೆ (ga_grow(gap, 1) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	((type_T **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len] = tuple_type->tt_ವಾದಗಳು[i];
	gap->ga_len++;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಗಣ_tuple_union_type_on_ರಾಶಿ(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, ಗಾತ್ರ(type_T *), 10);

    ಆದರೆ (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    ಆದರೆ (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    ಆದರೆ ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    ಗಣ_type_on_ರಾಶಿ(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    ಪೂರ್ಣಾಂಕ
generate_ಕೂಡಿಸು_instr(
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

    ಆದರೆ (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_ಸಂಖ್ಯೆ_or_float(type1, type2, (char_u *)"+") == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (isn != ಖಾಲಿ)
    {
	ಆದರೆ (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = ಅಭಿವ್ಯಕ್ತಿ_type;
	ಇಲ್ಲದಿದ್ದರೆ
	    isn->isn_arg.op.op_type = ಅಭಿವ್ಯಕ್ತಿ_ಕೂಡಿಸು;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    ಆದರೆ (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	ಗಣ_type_on_ರಾಶಿ(cctx, &t_list_any, 0);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype == VAR_TUPLE)
    {
	ಆದರೆ (!check_tuples_addable(type1, type2))
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (ಗಣ_tuple_union_type_on_ರಾಶಿ(type1, type2, cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ isn == ಖಾಲಿ ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    ಆದರೆ (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_ಸಂಖ್ಯೆ
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	ಹಿಂತಿರುಗಿ type1->tt_type;
    ಹಿಂತಿರುಗಿ VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    ಪೂರ್ಣಾಂಕ
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

    ಆಯ್ಕೆ (*op)
    {
	ಸ್ಥಿತಿ '+':
		  ಆದರೆ (generate_ಕೂಡಿಸು_instr(cctx, vartype, type1, type2,
							    ಅಭಿವ್ಯಕ್ತಿ_COPY) == FAIL)
		      ಹಿಂತಿರುಗಿ FAIL;
		  ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ '-':
	ಸ್ಥಿತಿ '*':
	ಸ್ಥಿತಿ '/': ಆದರೆ (check_ಸಂಖ್ಯೆ_or_float(type1, type2, op) == FAIL)
		      ಹಿಂತಿರುಗಿ FAIL;
		  ಆದರೆ (vartype == VAR_ಸಂಖ್ಯೆ)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  ಇಲ್ಲದಿದ್ದರೆ
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  ಆದರೆ (isn != ಖಾಲಿ)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? ಅಭಿವ್ಯಕ್ತಿ_MULT : *op == '/'? ಅಭಿವ್ಯಕ್ತಿ_DIV : ಅಭಿವ್ಯಕ್ತಿ_SUB;
		  ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ '%': ಆದರೆ ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_ಸಂಖ್ಯೆ)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_ಸಂಖ್ಯೆ))
		  {
		      emsg(_(e_percent_requires_ಸಂಖ್ಯೆ_arguments));
		      ಹಿಂತಿರುಗಿ FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_ಸಂಖ್ಯೆ ? ISN_OPNR : ISN_OPANY, 1);
		  ಆದರೆ (isn != ಖಾಲಿ)
		      isn->isn_arg.op.op_type = ಅಭಿವ್ಯಕ್ತಿ_REM;
		  ನಿಲ್ಲಿಸು;
    }

    // correct type of result
    ಆದರೆ (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	ಆದರೆ ((type1->tt_type == VAR_ಸಂಖ್ಯೆ || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_ಸಂಖ್ಯೆ || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	ಗಣ_type_on_ರಾಶಿ(cctx, type, 0);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    ಸ್ಥಿರ isntype_T
compare_isn_not_ಮೌಲ್ಯಗಳು(typval_T *tv, type_T *type)
{
    ಆದರೆ (tv != ಖಾಲಿ)
	(ಶೂನ್ಯ)check_typval_is_ಮೌಲ್ಯ(tv);
    ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)check_type_is_ಮೌಲ್ಯ(type);
    ಹಿಂತಿರುಗಿ ISN_DROP;
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
    vartype_T	vartype1 = tv1 != ಖಾಲಿ ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != ಖಾಲಿ ? tv2->v_type : type2->tt_type;

    ಆದರೆ (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	ಹಿಂತಿರುಗಿ compare_isn_not_ಮೌಲ್ಯಗಳು(tv1, type1);
    ಆದರೆ (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	ಹಿಂತಿರುಗಿ compare_isn_not_ಮೌಲ್ಯಗಳು(tv2, type2);

    ಆದರೆ (vartype1 == vartype2)
    {
	ಆಯ್ಕೆ (vartype1)
	{
	    ಸ್ಥಿತಿ VAR_BOOL: isntype = ISN_COMPAREBOOL; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ: isntype = ISN_COMPARENR; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_FLOAT: isntype = ISN_COMPAREFLOAT; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_STRING: isntype = ISN_COMPARESTRING; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_BLOB: isntype = ISN_COMPAREBLOB; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_LIST: isntype = ISN_COMPARELIST; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_TUPLE: isntype = ISN_COMPARETUPLE; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_DICT: isntype = ISN_COMPAREDICT; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_FUNC: isntype = ISN_COMPAREFUNC; ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ VAR_OBJECT: isntype = ISN_COMPAREOBJECT; ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ: isntype = ISN_COMPAREANY; ನಿಲ್ಲಿಸು;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_ಸಂಖ್ಯೆ || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_ಸಂಖ್ಯೆ || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	ಆದರೆ ((vartype1 == VAR_SPECIAL
		&& (tv1 != ಖಾಲಿ ? tv1->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != ಖಾಲಿ ? tv2->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    ಹಿಂತಿರುಗಿ ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    ಆದರೆ ((exprtype == ಅಭಿವ್ಯಕ್ತಿ_IS || exprtype == ಅಭಿವ್ಯಕ್ತಿ_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == ಅಭಿವ್ಯಕ್ತಿ_IS ? "is" : "isnot" , vartype_name(vartype1));
	ಹಿಂತಿರುಗಿ ISN_DROP;
    }
    ಆದರೆ (!(exprtype == ಅಭಿವ್ಯಕ್ತಿ_IS || exprtype == ಅಭಿವ್ಯಕ್ತಿ_ISNOT
		|| exprtype == ಅಭಿವ್ಯಕ್ತಿ_EQUAL || exprtype == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	ಹಿಂತಿರುಗಿ ISN_DROP;
    }
    ಆದರೆ (isntype == ISN_DROP
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
	ಹಿಂತಿರುಗಿ ISN_DROP;
    }
    ಹಿಂತಿರುಗಿ isntype;
}

    ಪೂರ್ಣಾಂಕ
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    ಆದರೆ (get_compare_isn(type, tv1, tv2, ಖಾಲಿ, ಖಾಲಿ) == ISN_DROP)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    ಪೂರ್ಣಾಂಕ
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, ಪೂರ್ಣಾಂಕ ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, ಖಾಲಿ, ಖಾಲಿ,
		       get_type_on_ರಾಶಿ(cctx, 1), get_type_on_ರಾಶಿ(cctx, 0));
    ಆದರೆ (isntype == ISN_DROP)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ ((isn = generate_instr(cctx, isntype)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --ರಾಶಿ->ga_len;
    ಗಣ_type_on_ರಾಶಿ(cctx, &t_bool, 0);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    ಪೂರ್ಣಾಂಕ
generate_CONCAT(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    RETURN_OK_IF_SKIP(cctx);

    ಆದರೆ ((isn = generate_instr(cctx, ISN_CONCAT)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಎಣಿಕೆ_2;

    // drop the argument types
    ರಾಶಿ->ga_len -= ಎಣಿಕೆ_2 - 1;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    ಪೂರ್ಣಾಂಕ
generate_2BOOL(cctx_T *cctx, ಪೂರ್ಣಾಂಕ invert, ಪೂರ್ಣಾಂಕ ಸ್ಥಾನಾಂತರ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_2BOOL)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.ಸ್ಥಾನಾಂತರ = ಸ್ಥಾನಾಂತರ;

    // type becomes bool
    ಗಣ_type_on_ರಾಶಿ(cctx, &t_bool, -1 - ಸ್ಥಾನಾಂತರ);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ (generate_instr(cctx, ISN_COND2BOOL) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // type becomes bool
    ಗಣ_type_on_ರಾಶಿ(cctx, &t_bool, 0);

    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	ಪೂರ್ಣಾಂಕ	    typechk_flags,	// type check flags
	ಪೂರ್ಣಾಂಕ	    ಸ್ಥಾನಾಂತರ,
	ಪೂರ್ಣಾಂಕ	    is_var,
	ಪೂರ್ಣಾಂಕ	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    type_T *tt;
    ಆದರೆ ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_ಸಂಖ್ಯೆ_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	ಆದರೆ (tt != ಖಾಲಿ)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    ಆದರೆ (typechk_flags & TYPECHK_ಸಂಖ್ಯೆ_OK)
		tt->tt_flags |= TTFLAG_ಸಂಖ್ಯೆ_OK;
	    ಇಲ್ಲದಿದ್ದರೆ
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)ಸ್ಥಾನಾಂತರ;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    ಗಣ_type_on_ರಾಶಿ(cctx, expected, -1 - ಸ್ಥಾನಾಂತರ);

    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_SETTYPE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == ಖಾಲಿ ? &t_any : &class->class_type);
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.classarg = class;
    ಆದರೆ (class != ಖಾಲಿ)
	++class->class_refcount;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    ಪೂರ್ಣಾಂಕ
generate_tv_ತಳ್ಳು(cctx_T *cctx, typval_T *tv)
{
    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_ಸಂಖ್ಯೆ);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_ಸಂಖ್ಯೆ);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    generate_PUSHNR(cctx, tv->vval.v_ಸಂಖ್ಯೆ);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_LIST:
	    ಆದರೆ (tv->vval.v_list != ಖಾಲಿ)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಆದರೆ (tv->vval.v_tuple != ಖಾಲಿ)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    ಆದರೆ (tv->vval.v_dict != ಖಾಲಿ)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
	    ಆದರೆ (tv->vval.v_ಕೆಲಸ != ಖಾಲಿ)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CHANNEL:
	    ಆದರೆ (tv->vval.v_channel != ಖಾಲಿ)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_FUNC:
	    ಆದರೆ (tv->vval.v_string != ಖಾಲಿ)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, ಖಾಲಿ, &t_func_unknown, TRUE);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಆದರೆ (tv->vval.v_partial != ಖಾಲಿ)
		iemsg("non-null partial constant not supported");
	    ಆದರೆ (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = ಖಾಲಿ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_OBJECT:
	    ಆದರೆ (tv->vval.v_object != ಖಾಲಿ)
	    {
		emsg(_(e_cannot_use_non_null_object));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHNR(cctx_T *cctx, varnumber_T ಸಂಖ್ಯೆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_ಸಂಖ್ಯೆ)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ;

    ಆದರೆ (ಸಂಖ್ಯೆ == 0 || ಸಂಖ್ಯೆ == 1)
	// A 0 or 1 number can also be used as a bool.
	ಗಣ_type_on_ರಾಶಿ(cctx, &t_ಸಂಖ್ಯೆ_bool, 0);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHBOOL(cctx_T *cctx, varnumber_T ಸಂಖ್ಯೆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHSPEC(cctx_T *cctx, varnumber_T ಸಂಖ್ಯೆ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     ಸಂಖ್ಯೆ == VVAL_NULL ? &t_null : &t_none)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಸಂಖ್ಯೆ;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.fnumber = fnumber;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    ಪೂರ್ಣಾಂಕ		ret = OK;

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	ಆದರೆ ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == ಖಾಲಿ)
	    ret = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    isn->isn_arg.string = str == ಖಾಲಿ ? ಖಾಲಿ : *str;
	    ಹಿಂತಿರುಗಿ OK;
	}
    }
    ಆದರೆ (str != ಖಾಲಿ)
	VIM_CLEAR(*str);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
#ಇಲ್ಲದಿದ್ದರೆ
    emsg(_(e_channel_ಕೆಲಸ_feature_not_ಲಭ್ಯ));
    ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (generate_instr_type(cctx, ISN_PUSHJOB, &t_ಕೆಲಸ) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
#ಇಲ್ಲದಿದ್ದರೆ
    emsg(_(e_channel_ಕೆಲಸ_feature_not_ಲಭ್ಯ));
    ಹಿಂತಿರುಗಿ FAIL;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.blob = blob;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, ಪೂರ್ಣಾಂಕ may_ಪೂರ್ವಪ್ರತ್ಯಯ)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (name == ಖಾಲಿ)
	funcname = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!may_ಪೂರ್ವಪ್ರತ್ಯಯ
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != ಖಾಲಿ)	    // autoload
	funcname = vim_strsave(name);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	funcname = alloc(STRLEN(name) + 3);
	ಆದರೆ (funcname != ಖಾಲಿ)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.string = vim_strsave(name);
    ಆದರೆ (isn->isn_arg.string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    ಪೂರ್ಣಾಂಕ
generate_GETITEM(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಸೂಚ್ಯಂಕ, ಪೂರ್ಣಾಂಕ with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_ರಾಶಿ(cctx, with_op ? 1 : 0);
    type_T	*ಅಂಶ_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    ಅಂಶ_type = get_ಅಂಶ_type(type);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_GETITEM)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.getitem.gi_ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_ರಾಶಿ(cctx, ಅಂಶ_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_tuple_ತುಂಡು_type_on_ರಾಶಿ(type_T *type, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    ಆದರೆ ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= ಎಣಿಕೆ_2)
    {
	ಗಣ_type_on_ರಾಶಿ(cctx, &t_tuple_any, 0);
	ಹಿಂತಿರುಗಿ;
    }

    ga_init2(&tuple_types_ga, ಗಾತ್ರ(type_T *), 10);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = ಎಣಿಕೆ_2; i < type->tt_argcount; i++)
    {
	ಆದರೆ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    ಗಣ_type_on_ರಾಶಿ(cctx, &t_tuple_any, 0);
	    ಹಿಂತಿರುಗಿ;
	}
	((type_T **)tuple_types_ga.ga_ದತ್ತಾಂಶ)[tuple_types_ga.ga_len] =
							     type->tt_ವಾದಗಳು[i];
	tuple_types_ga.ga_len++;
    }

    ಗಣ_type_on_ರಾಶಿ(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    ಪೂರ್ಣಾಂಕ
generate_ತುಂಡು(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_ತುಂಡು)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಎಣಿಕೆ_2;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_ರಾಶಿ(cctx, 0);
    ಆದರೆ (type->tt_type == VAR_TUPLE)
	ಗಣ_tuple_ತುಂಡು_type_on_ರಾಶಿ(type, ಎಣಿಕೆ_2, cctx);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    ಪೂರ್ಣಾಂಕ
generate_CHECKLEN(cctx_T *cctx, ಪೂರ್ಣಾಂಕ min_len, ಪೂರ್ಣಾಂಕ ಇನ್ನಷ್ಟು_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    ಆದರೆ ((isn = generate_instr(cctx, ISN_CHECKLEN)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_ಇನ್ನಷ್ಟು_OK = ಇನ್ನಷ್ಟು_OK;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_STORE(cctx_T *cctx, isntype_T isn_type, ಪೂರ್ಣಾಂಕ idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (name != ಖಾಲಿ)
	isn->isn_arg.string = vim_strsave(name);
    ಇಲ್ಲದಿದ್ದರೆ
	isn->isn_arg.ಸಂಖ್ಯೆ = idx;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	ಪೂರ್ಣಾಂಕ	    ತುಂಬು,
	class_T	    *cl,
	ಪೂರ್ಣಾಂಕ	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ (ತುಂಬು)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_ತುಂಬು_CLASSMEMBER, m->ocm_type);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
generate_STOREOUTER(
	cctx_T	    *cctx,
	ಪೂರ್ಣಾಂಕ	    idx,
	ಪೂರ್ಣಾಂಕ	    level,
	ಪೂರ್ಣಾಂಕ	    loop_ಆಳ,
	ಪೂರ್ಣಾಂಕ	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_ಆಳ = -loop_ಆಳ - 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_ಆಳ = level;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    ಪೂರ್ಣಾಂಕ
generate_STORENR(cctx_T *cctx, ಪೂರ್ಣಾಂಕ idx, varnumber_T ಮೌಲ್ಯ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_STORENR)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = ಮೌಲ್ಯ;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_ತುಂಬು(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ಪೂರ್ಣಾಂಕ	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_type2(cctx, isn_type, type, type)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (name != ಖಾಲಿ)
	isn->isn_arg.string = vim_strsave(name);
    ಇಲ್ಲದಿದ್ದರೆ
	isn->isn_arg.ಸಂಖ್ಯೆ = idx;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    ಪೂರ್ಣಾಂಕ
generate_LOADOUTER(
	cctx_T	    *cctx,
	ಪೂರ್ಣಾಂಕ	    idx,
	ಪೂರ್ಣಾಂಕ	    nesting,
	ಪೂರ್ಣಾಂಕ	    loop_ಆಳ,
	ಪೂರ್ಣಾಂಕ	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_ಆಳ = -loop_ಆಳ - 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_ಆಳ = nesting;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    ಪೂರ್ಣಾಂಕ
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    ಪೂರ್ಣಾಂಕ	    di_flags;
    ಪೂರ್ಣಾಂಕ	    vidx = ಕಂಡುಹಿಡಿ_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ (vidx < 0)
    {
	semsg(_(e_variable_not_ಸಿಕ್ಕಿತು_str), name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    ಹಿಂತಿರುಗಿ generate_ತುಂಬು(cctx, ISN_LOADV, vidx, ಖಾಲಿ, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, ಪೂರ್ಣಾಂಕ forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, isn_type)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ (generate_instr(cctx, ISN_LOCKCONST) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    ಇಲ್ಲದಿದ್ದರೆ
	isn = generate_instr_drop(cctx, isn_type, 1);
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ಪೂರ್ಣಾಂಕ	    sid,
	ಪೂರ್ಣಾಂಕ	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ಅಂಶ(sid);

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    ಇಲ್ಲದಿದ್ದರೆ
	isn = generate_instr_drop(cctx, isn_type, 1);
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    ಆದರೆ (sref == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    ಪೂರ್ಣಾಂಕ
generate_NEWLIST(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_NEWLIST)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = use_null ? -1 : ಎಣಿಕೆ_2;

    // Get the member type and the declared member type from all the items on
    // the stack.
    ಆದರೆ ((member_type = get_member_type_from_ರಾಶಿ(ಎಣಿಕೆ_2, 1, cctx)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_ರಾಶಿ.ga_len -= ಎಣಿಕೆ_2;

    // add the list type to the type stack
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    ಪೂರ್ಣಾಂಕ
generate_NEWTUPLE(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = use_null ? -1 : ಎಣಿಕೆ_2;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, ಗಾತ್ರ(type_T *), 10);

    ಆದರೆ (get_tuple_type_from_ರಾಶಿ(ಎಣಿಕೆ_2, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_ರಾಶಿ.ga_len -= ಎಣಿಕೆ_2;

    // add the tuple type to the type stack
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    ಪೂರ್ಣಾಂಕ
generate_NEWDICT(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ಪೂರ್ಣಾಂಕ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_NEWDICT)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = use_null ? -1 : ಎಣಿಕೆ_2;

    ಆದರೆ ((member_type = get_member_type_from_ರಾಶಿ(ಎಣಿಕೆ_2, 2, cctx)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_ರಾಶಿ.ga_len -= 2 * ಎಣಿಕೆ_2;

    // add the dict type to the type stack
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    ಪೂರ್ಣಾಂಕ
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ಪೂರ್ಣಾಂಕ	    object_ವಿಧಾನ,
	ಪೂರ್ಣಾಂಕ	    fi,
	ಪೂರ್ಣಾಂಕ	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    ಪೂರ್ಣಾಂಕ		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_FUNCREF)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (isn_idx != ಖಾಲಿ)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_NOT_COMPILED || has_vars || cl != ಖಾಲಿ)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	ಆದರೆ (extra == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	ಆದರೆ (cl != ಖಾಲಿ)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_ವಿಧಾನ = object_ವಿಧಾನ;
	    extra->fre_ವಿಧಾನ_idx = fi;
	}
    }
    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_NOT_COMPILED || cl != ಖಾಲಿ)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ != UF_NOT_COMPILED && cl == ಖಾಲಿ)
    {
	ಆದರೆ (isn_idx == ಖಾಲಿ && ufunc->uf_def_ಸ್ಥಿತಿ == UF_ಗಮ್ಯ_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (ಶೂನ್ಯ)compile_def_function(ufunc, FALSE, CT_NONE, ಖಾಲಿ);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_ಆವರಣ = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    ಆದರೆ ((ufunc->uf_flags & FC_ಆವರಣ)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_ಆವರಣ;

    type = ufunc->uf_func_type == ಖಾಲಿ ? &t_func_any : ufunc->uf_func_type;
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_ರಾಶಿ(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    ಪೂರ್ಣಾಂಕ
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    ಪೂರ್ಣಾಂಕ		ret = OK;

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	ಆದರೆ ((isn = generate_instr(cctx, ISN_NEWFUNC)) == ಖಾಲಿ)
	    ret = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    ಆದರೆ (arg == ಖಾಲಿ)
		ret = FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_ಆವರಣ = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(ಶೂನ್ಯ)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		ಹಿಂತಿರುಗಿ OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    ಪೂರ್ಣಾಂಕ
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_DEF)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	ಆದರೆ (isn->isn_arg.string == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_JUMP(cctx_T *cctx, jumpwhen_T when, ಪೂರ್ಣಾಂಕ where)
{
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_JUMP)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    ಆದರೆ (when != JUMP_ALWAYS && ರಾಶಿ->ga_len > 0)
	--ರಾಶಿ->ga_len;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    ಪೂರ್ಣಾಂಕ
generate_WHILE(cctx_T *cctx, ಪೂರ್ಣಾಂಕ funcref_idx)
{
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_WHILE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_ಕೊನೆ = 0;  // filled in later

    ಆದರೆ (ರಾಶಿ->ga_len > 0)
	--ರಾಶಿ->ga_len;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, ಪೂರ್ಣಾಂಕ arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, isn_type)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
generate_FOR(cctx_T *cctx, ಪೂರ್ಣಾಂಕ loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_FOR)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_any);
}

    ಪೂರ್ಣಾಂಕ
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_ENDLOOP)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.endloop.ಕೊನೆ_ಆಳ = loop_info->li_ಆಳ;
    isn->isn_arg.endloop.ಕೊನೆ_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.ಕೊನೆ_var_idx = loop_info->li_local_ಎಣಿಕೆ;
    isn->isn_arg.endloop.ಕೊನೆ_var_ಎಣಿಕೆ =
			   cctx->ctx_locals.ga_len - loop_info->li_local_ಎಣಿಕೆ;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_TRYCONT(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಮಟ್ಟಗಳು, ಪೂರ್ಣಾಂಕ where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_TRYCONT)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.trycont.tct_ಮಟ್ಟಗಳು = ಮಟ್ಟಗಳು;
    isn->isn_arg.trycont.tct_where = where;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    ಪೂರ್ಣಾಂಕ
check_ಆಂತರಿಕ_func_ವಾದಗಳು(
	cctx_T	*cctx,
	ಪೂರ್ಣಾಂಕ	func_idx,
	ಪೂರ್ಣಾಂಕ	argcount,
	ಪೂರ್ಣಾಂಕ	ವಿಧಾನ_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    ಪೂರ್ಣಾಂಕ		argoff = check_ಆಂತರಿಕ_func(func_idx, argcount);

    ಆದರೆ (argoff < 0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ವಿಧಾನ_call && argoff > 1)
    {
	ಆದರೆ (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 ಆಂತರಿಕ_func_name(func_idx));
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	ಆದರೆ (isn  == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	isn->isn_arg.shuffle.shfl_ಅಂಶ = argcount;
	isn->isn_arg.shuffle.shfl_ಮೇಲೆ = argoff - 1;
    }

    ಆದರೆ (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)ರಾಶಿ->ga_ದತ್ತಾಂಶ) + ರಾಶಿ->ga_len - argcount;

	// Check the types of the arguments.
	ಆದರೆ (ವಿಧಾನ_call && argoff > 1)
	{
	    ಪೂರ್ಣಾಂಕ i;

	    ಗಾಗಿ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ i;

	    ಗಾಗಿ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ಆದರೆ (ಆಂತರಿಕ_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    ಪೂರ್ಣಾಂಕ
generate_BCALL(cctx_T *cctx, ಪೂರ್ಣಾಂಕ func_idx, ಪೂರ್ಣಾಂಕ argcount, ಪೂರ್ಣಾಂಕ ವಿಧಾನ_call)
{
    isn_T	*isn;
    garray_T	*ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;
    type2_T	*argtypes = ಖಾಲಿ;
    type2_T	shuffled_argtypes[MAX_FUNC_ವಾದಗಳು];
    type2_T	*maptype = ಖಾಲಿ;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    ಆದರೆ (check_ಆಂತರಿಕ_func_ವಾದಗಳು(cctx, func_idx, argcount, ವಿಧಾನ_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (ಆಂತರಿಕ_func_is_map(func_idx))
	maptype = argtypes;

    ಆದರೆ ((isn = generate_instr(cctx, ISN_BCALL)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    ರಾಶಿ->ga_len -= argcount;
    type = ಆಂತರಿಕ_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    ಆದರೆ (ತಳ್ಳು_type_stack2(cctx, type, decl_type) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (maptype != ಖಾಲಿ && maptype[0].type_decl->tt_member != ಖಾಲಿ
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ಪೂರ್ಣಾಂಕ
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*ಅಂಶ_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_ರಾಶಿ(cctx, 1);
    ಆದರೆ (arg_type_modifiable(list_type, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಅಂಶ_type = get_type_on_ರಾಶಿ(cctx, 0);
    expected = list_type->tt_member;
    ಆದರೆ (need_type(ಅಂಶ_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (generate_instr(cctx, ISN_LISTAPPEND) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    --cctx->ctx_type_ರಾಶಿ.ga_len;	    // drop the argument
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ಪೂರ್ಣಾಂಕ
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*ಅಂಶ_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    ಆದರೆ (arg_type_modifiable(get_decl_type_on_ರಾಶಿ(cctx, 1), 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಅಂಶ_type = get_type_on_ರಾಶಿ(cctx, 0);
    ಆದರೆ (need_type(ಅಂಶ_type, &t_ಸಂಖ್ಯೆ, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (generate_instr(cctx, ISN_BLOBAPPEND) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    --cctx->ctx_type_ರಾಶಿ.ga_len;	    // drop the argument
    ಹಿಂತಿರುಗಿ OK;
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
    ಆದರೆ (cl != ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ISN_METHODCALL;

    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_NOT_COMPILED)
	ಹಿಂತಿರುಗಿ ISN_UCALL;

    // function invoked from a lambda
    ಆದರೆ (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	ಆದರೆ (ufunc->uf_name[0] == K_SPECIAL)
	    ಹಿಂತಿರುಗಿ ISN_UCALL;
    }

    ಹಿಂತಿರುಗಿ ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    ಪೂರ್ಣಾಂಕ
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ಪೂರ್ಣಾಂಕ	    mi,
	ಪೂರ್ಣಾಂಕ	    pushed_argcount,
	ಪೂರ್ಣಾಂಕ	    is_super)
{
    isn_T	*isn;
    ಪೂರ್ಣಾಂಕ		regular_ವಾದಗಳು = ufunc->uf_ವಾದಗಳು.ga_len;
    ಪೂರ್ಣಾಂಕ		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ (argcount > regular_ವಾದಗಳು && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (argcount < regular_ವಾದಗಳು - ufunc->uf_def_ವಾದಗಳು.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ != UF_NOT_COMPILED
	    && ufunc->uf_def_ಸ್ಥಿತಿ != UF_COMPILE_ERROR)
    {
	ಪೂರ್ಣಾಂಕ		i;
	compiletype_T	compile_type;

	ಗಾಗಿ (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_ರಾಶಿ(cctx, argcount - i - 1);
	    ಆದರೆ (check_type_is_ಮೌಲ್ಯ(actual) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (actual->tt_type == VAR_SPECIAL
			      && i >= regular_ವಾದಗಳು - ufunc->uf_def_ವಾದಗಳು.ga_len)
	    {
		// assume v:none used for default argument value
		ಮುಂದುವರಿ;
	    }
	    ಆದರೆ (i < regular_ವಾದಗಳು)
	    {
		ಆದರೆ (ufunc->uf_arg_types == ಖಾಲಿ)
		    ಮುಂದುವರಿ;
		expected = ufunc->uf_arg_types[i];
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ufunc->uf_va_type == ಖಾಲಿ
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    ಇಲ್ಲದಿದ್ದರೆ
		expected = ufunc->uf_va_type->tt_member;
	    ಆದರೆ (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	ಆದರೆ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == ಖಾಲಿ,
						   compile_type, ಖಾಲಿ) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_ಗಮ್ಯ_function_that_failed_ಗಮ್ಯ_compile_str,
							       ufunc->uf_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (cl != ಖಾಲಿ /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	ಆದರೆ (isn->isn_arg.mfunc == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = ಖಾಲಿ;
	ಆದರೆ (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = ಖಾಲಿ;
	isn->isn_arg.mfunc->cmf_cache_generic = ಖಾಲಿ;
	isn->isn_arg.mfunc->cmf_cache_ufunc = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_ರಾಶಿ.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    ಆದರೆ (ufunc->uf_class != ಖಾಲಿ)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_ರಾಶಿ(cctx, 0);
	ಆದರೆ (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_ರಾಶಿ.ga_len--;
    }

    // add return type
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_ರಾಶಿ(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    ಪೂರ್ಣಾಂಕ
generate_UCALL(cctx_T *cctx, char_u *name, ಪೂರ್ಣಾಂಕ argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_UCALL)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_ರಾಶಿ.ga_len -= argcount;

    // add return value
    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    ಪೂರ್ಣಾಂಕ
check_func_ವಾದಗಳು_from_type(
	cctx_T	*cctx,
	type_T	*type,
	ಪೂರ್ಣಾಂಕ	argcount,
	ಪೂರ್ಣಾಂಕ	at_ತುದಿ,
	char_u	*name)
{
    ಆದರೆ (type->tt_argcount != -1)
    {
	ಪೂರ್ಣಾಂಕ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	ಆದರೆ (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (type->tt_ವಾದಗಳು != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ i;

	    ಗಾಗಿ (i = 0; i < argcount; ++i)
	    {
		ಪೂರ್ಣಾಂಕ	ಸ್ಥಾನಾಂತರ = -argcount + i - (at_ತುದಿ ? 0 : 1);
		type_T	*actual = get_type_on_ರಾಶಿ(cctx, -1 - ಸ್ಥಾನಾಂತರ);
		type_T	*expected;

		ಆದರೆ (check_type_is_ಮೌಲ್ಯ(actual) == FAIL)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_ವಾದಗಳು[type->tt_argcount - 1];
		    ಆದರೆ (expected != ಖಾಲಿ && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    ಆದರೆ (expected == ಖಾಲಿ)
			expected = &t_any;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		ಇಲ್ಲದಿದ್ದರೆ
		    expected = type->tt_ವಾದಗಳು[i];
		ಆದರೆ (need_type(actual, expected, 0,
				     ಸ್ಥಾನಾಂತರ, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    ಹಿಂತಿರುಗಿ FAIL;
		}
	    }
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    ಪೂರ್ಣಾಂಕ
generate_PCALL(
	cctx_T	*cctx,
	ಪೂರ್ಣಾಂಕ	argcount,
	char_u	*name,
	type_T	*type,
	ಪೂರ್ಣಾಂಕ	at_ತುದಿ)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    ಆದರೆ (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	ಆದರೆ (check_func_ವಾದಗಳು_from_type(cctx, type, argcount, at_ತುದಿ, name)
								       == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ret_type = type->tt_member;
	ಆದರೆ (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_not_callable_type_str), name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ ((isn = generate_instr(cctx, ISN_PCALL)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.pfunc.cpf_ತುದಿ = at_ತುದಿ;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_ರಾಶಿ.ga_len -= argcount + 1;

    // push the return value
    ತಳ್ಳು_type_ರಾಶಿ(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    ಆದರೆ (at_ತುದಿ && generate_instr(cctx, ISN_PCALL_ಕೊನೆ) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_DEFER(cctx_T *cctx, ಪೂರ್ಣಾಂಕ var_idx, ಪೂರ್ಣಾಂಕ argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_ರಾಶಿ(cctx, 0);
    ಆದರೆ (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	ಅಕ್ಷರ *tofree;

	semsg(_(e_expected_dictionary_for_using_ಕೀಲಿ_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    // change dict type to dict member type
    ಆದರೆ (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	ಗಣ_type_on_ರಾಶಿ(cctx, ntype, 0);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_ECHO(cctx_T *cctx, ಪೂರ್ಣಾಂಕ with_white, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_drop(cctx, ISN_ECHO, ಎಣಿಕೆ_2)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_ಎಣಿಕೆ = ಎಣಿಕೆ_2;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_MULT_ಅಭಿವ್ಯಕ್ತಿ(cctx_T *cctx, isntype_T isn_type, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr_drop(cctx, isn_type, ಎಣಿಕೆ_2)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    ಪೂರ್ಣಾಂಕ
generate_ECHOWINDOW(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ದೀರ್ಘ ಸಮಯ)
{
    isn_T	*isn;

    ಆದರೆ ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, ಎಣಿಕೆ_2)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.echowin.ewin_ಎಣಿಕೆ = ಎಣಿಕೆ_2;
    isn->isn_arg.echowin.ewin_ಸಮಯ = ಸಮಯ;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    ಪೂರ್ಣಾಂಕ
generate_ಮೂಲ(cctx_T *cctx, ಪೂರ್ಣಾಂಕ sid)
{
    isn_T	*isn;

    ಆದರೆ ((isn = generate_instr(cctx, ISN_ಮೂಲ)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = sid;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    ಪೂರ್ಣಾಂಕ
generate_PUT(cctx_T *cctx, ಪೂರ್ಣಾಂಕ regname, linenr_T lnum, ಪೂರ್ಣಾಂಕ fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    ಆದರೆ (isn == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    ಪೂರ್ಣಾಂಕ
generate_LOCKUNLOCK(cctx_T *cctx, char_u *ಸಾಲು, ಪೂರ್ಣಾಂಕ is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    class_T *cl = cctx->ctx_ufunc != ಖಾಲಿ ? cctx->ctx_ufunc->uf_class : ಖಾಲಿ;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(ಸಾಲು);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    ಆದರೆ (cl != ಖಾಲಿ)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    ಪೂರ್ಣಾಂಕ
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *ಸಾಲು)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, isntype)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.string = vim_strsave(ಸಾಲು);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    ಪೂರ್ಣಾಂಕ
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    ಪೂರ್ಣಾಂಕ		ret = OK;

    ಆದರೆ (cctx->ctx_skip != SKIP_YES)
    {
	ಆದರೆ ((isn = generate_instr(cctx, isntype)) == ಖಾಲಿ)
	    ret = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    isn->isn_arg.string = str;
	    ಹಿಂತಿರುಗಿ OK;
	}
    }
    vim_free(str);
    ಹಿಂತಿರುಗಿ ret;
}

    ಪೂರ್ಣಾಂಕ
generate_LEGACY_EVAL(cctx_T *cctx, char_u *ಸಾಲು)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.string = vim_strsave(ಸಾಲು);

    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_any);
}

    ಪೂರ್ಣಾಂಕ
generate_EXECCONCAT(cctx_T *cctx, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    isn_T	*isn;

    ಆದರೆ ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, ಎಣಿಕೆ_2)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.ಸಂಖ್ಯೆ = ಎಣಿಕೆ_2;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    ಪೂರ್ಣಾಂಕ
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    ಆದರೆ ((isn = generate_instr(cctx, ISN_RANGE)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.string = range;

    ಹಿಂತಿರುಗಿ ತಳ್ಳು_type_ರಾಶಿ(cctx, &t_ಸಂಖ್ಯೆ);
}

    ಪೂರ್ಣಾಂಕ
generate_UNPACK(cctx_T *cctx, ಪೂರ್ಣಾಂಕ var_ಎಣಿಕೆ, ಪೂರ್ಣಾಂಕ semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_UNPACK)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.unpack.unp_ಎಣಿಕೆ = var_ಎಣಿಕೆ;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    ಪೂರ್ಣಾಂಕ
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    ಆದರೆ (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	ಆದರೆ ((isn = generate_instr(cctx, ISN_CMDMOD)) == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	ಆದರೆ (isn->isn_arg.cmdmod.cf_cmdmod == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, ಗಾತ್ರ(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
generate_undo_cmdmods(cctx_T *cctx)
{
    ಆದರೆ (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
generate_store_var(
	cctx_T		*cctx,
	ನಿಯೋಜಿಸು_dest_T	dest,
	ಪೂರ್ಣಾಂಕ		opt_flags,
	ಪೂರ್ಣಾಂಕ		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    ಆಯ್ಕೆ (dest)
    {
	ಸ್ಥಿತಿ dest_option:
	    ಹಿಂತಿರುಗಿ generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	ಸ್ಥಿತಿ dest_func_option:
	    ಹಿಂತಿರುಗಿ generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	ಸ್ಥಿತಿ dest_global:
	    // include g: with the name, easier to execute that way
	    ಹಿಂತಿರುಗಿ generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == ಖಾಲಿ
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	ಸ್ಥಿತಿ dest_ಬಫರ್:
	    // include b: with the name, easier to execute that way
	    ಹಿಂತಿರುಗಿ generate_STORE(cctx, ISN_STOREB, 0, name);
	ಸ್ಥಿತಿ dest_ಕಿಟಕಿ:
	    // include w: with the name, easier to execute that way
	    ಹಿಂತಿರುಗಿ generate_STORE(cctx, ISN_STOREW, 0, name);
	ಸ್ಥಿತಿ dest_tab:
	    // include t: with the name, easier to execute that way
	    ಹಿಂತಿರುಗಿ generate_STORE(cctx, ISN_STORET, 0, name);
	ಸ್ಥಿತಿ dest_env:
	    ಹಿಂತಿರುಗಿ generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	ಸ್ಥಿತಿ dest_reg:
	    ಹಿಂತಿರುಗಿ generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], ಖಾಲಿ);
	ಸ್ಥಿತಿ dest_vimvar:
	    ಹಿಂತಿರುಗಿ generate_STORE(cctx, ISN_STOREV, vimvaridx, ಖಾಲಿ);
	ಸ್ಥಿತಿ dest_script:
	ಸ್ಥಿತಿ dest_script_v9:
	    {
		ಪೂರ್ಣಾಂಕ	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		ಪೂರ್ಣಾಂಕ	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		ಆದರೆ (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    ಆದರೆ (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ಅಂಶ(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ಅಂಶ(scriptvar_sid)
						  ->sn_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ == ಖಾಲಿ)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			//ಮೊದಲ
			ಆದರೆ (generate_ಮೂಲ(cctx, scriptvar_sid) == FAIL)
			    ಹಿಂತಿರುಗಿ FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    ಹಿಂತಿರುಗಿ generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		ಹಿಂತಿರುಗಿ generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	ಸ್ಥಿತಿ dest_class_member:
	    ಹಿಂತಿರುಗಿ generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	ಸ್ಥಿತಿ dest_local:
	ಸ್ಥಿತಿ dest_ಅಭಿವ್ಯಕ್ತಿ:
	    // cannot happen
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    ಪೂರ್ಣಾಂಕ
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    ಗಾಗಿ (;;)
    {
	ಆದರೆ (scope == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    ಹಿಂತಿರುಗಿ TRUE;
	scope = scope->se_outer;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಪೂರ್ಣಾಂಕ
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, ಪೂರ್ಣಾಂಕ instr_ಎಣಿಕೆ, ಪೂರ್ಣಾಂಕ is_decl)
{
    ಆದರೆ (lhs->lhs_dest != dest_local)
	ಹಿಂತಿರುಗಿ generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    ಆದರೆ (lhs->lhs_lvar == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_ದತ್ತಾಂಶ) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    ಆದರೆ (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_ಎಣಿಕೆ + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.ಸಂಖ್ಯೆ;
	garray_T    *ರಾಶಿ = &cctx->ctx_type_ರಾಶಿ;

	ಆದರೆ (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	ಆದರೆ (ರಾಶಿ->ga_len > 0)
	    --ರಾಶಿ->ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_ಆಳ,
		lhs->lhs_lvar->lv_loop_idx);
    ಇಲ್ಲದಿದ್ದರೆ
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, ಖಾಲಿ);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    ಪೂರ್ಣಾಂಕ
generate_SCRIPTCTX_ಗಣ(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ಆದರೆ ((isn = generate_instr(cctx, ISN_SCRIPTCTX_ಗಣ)) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    isn->isn_arg.setsctx = new_sctx;
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(FEAT_PROFILE)
    ಶೂನ್ಯ
may_generate_prof_ಕೊನೆ(cctx_T *cctx, ಪೂರ್ಣಾಂಕ prof_lnum)
{
    ಆದರೆ (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_ಕೊನೆ);
}
#ಆದರೆ_ಅಂತ್ಯ


/*
 * Delete an instruction, free what it contains.
 */
    ಶೂನ್ಯ
delete_instr(isn_T *isn)
{
    ಆಯ್ಕೆ (isn->isn_type)
    {
	ಸ್ಥಿತಿ ISN_AUTOLOAD:
	ಸ್ಥಿತಿ ISN_DEF:
	ಸ್ಥಿತಿ ISN_EXEC:
	ಸ್ಥಿತಿ ISN_EXECRANGE:
	ಸ್ಥಿತಿ ISN_EXEC_ವಿಭಜಿಸು:
	ಸ್ಥಿತಿ ISN_LEGACY_EVAL:
	ಸ್ಥಿತಿ ISN_LOADAUTO:
	ಸ್ಥಿತಿ ISN_LOADB:
	ಸ್ಥಿತಿ ISN_LOADENV:
	ಸ್ಥಿತಿ ISN_LOADG:
	ಸ್ಥಿತಿ ISN_LOADOPT:
	ಸ್ಥಿತಿ ISN_LOADT:
	ಸ್ಥಿತಿ ISN_LOADW:
	ಸ್ಥಿತಿ ISN_PUSHEXC:
	ಸ್ಥಿತಿ ISN_PUSHFUNC:
	ಸ್ಥಿತಿ ISN_PUSHS:
	ಸ್ಥಿತಿ ISN_RANGE:
	ಸ್ಥಿತಿ ISN_STOREAUTO:
	ಸ್ಥಿತಿ ISN_STOREB:
	ಸ್ಥಿತಿ ISN_STOREENV:
	ಸ್ಥಿತಿ ISN_STOREG:
	ಸ್ಥಿತಿ ISN_STORET:
	ಸ್ಥಿತಿ ISN_STOREW:
	ಸ್ಥಿತಿ ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_SUBSTITUTE:
	    {
		ಪೂರ್ಣಾಂಕ	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		ಗಾಗಿ (idx = 0; list[idx].isn_type != ISN_ಮುಕ್ತಾಯ; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_INSTR:
	    {
		ಪೂರ್ಣಾಂಕ	idx;
		isn_T	*list = isn->isn_arg.instr;

		ಗಾಗಿ (idx = 0; list[idx].isn_type != ISN_ಮುಕ್ತಾಯ; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_LOADS:
	ಸ್ಥಿತಿ ISN_LOADEXPORT:
	ಸ್ಥಿತಿ ISN_STORES:
	ಸ್ಥಿತಿ ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_UNLET:
	ಸ್ಥಿತಿ ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_STOREOPT:
	ಸ್ಥಿತಿ ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		ಆದರೆ (extra == ಖಾಲಿ || extra->fre_func_name == ಖಾಲಿ)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    ಆದರೆ (ufunc != ಖಾಲಿ && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		ಆದರೆ (extra != ಖಾಲಿ)
		{
		    char_u *name = extra->fre_func_name;

		    ಆದರೆ (name != ಖಾಲಿ)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    ಆದರೆ (extra->fre_class != ಖಾಲಿ)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
		    + isn->isn_arg.dfunc.cdf_idx;

		ಆದರೆ (dfunc->df_ufunc != ಖಾಲಿ
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		ಆದರೆ (arg != ಖಾಲಿ)
		{
		    ufunc_T *ufunc = ಕಂಡುಹಿಡಿ_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    ಆದರೆ (ufunc != ಖಾಲಿ)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_CHECKTYPE:
	ಸ್ಥಿತಿ ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_LOADSCRIPT:
	ಸ್ಥಿತಿ ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_ತುಂಬು_CLASSMEMBER:
	ಸ್ಥಿತಿ ISN_STORE_CLASSMEMBER:
	ಸ್ಥಿತಿ ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ ISN_2BOOL:
	ಸ್ಥಿತಿ ISN_2STRING:
	ಸ್ಥಿತಿ ISN_2STRING_ANY:
	ಸ್ಥಿತಿ ISN_ADDBLOB:
	ಸ್ಥಿತಿ ISN_ADDLIST:
	ಸ್ಥಿತಿ ISN_ADDTUPLE:
	ಸ್ಥಿತಿ ISN_ANYINDEX:
	ಸ್ಥಿತಿ ISN_ANYSLICE:
	ಸ್ಥಿತಿ ISN_BCALL:
	ಸ್ಥಿತಿ ISN_BLOBAPPEND:
	ಸ್ಥಿತಿ ISN_BLOBINDEX:
	ಸ್ಥಿತಿ ISN_BLOBSLICE:
	ಸ್ಥಿತಿ ISN_CATCH:
	ಸ್ಥಿತಿ ISN_CEXPR_AUCMD:
	ಸ್ಥಿತಿ ISN_CHECKLEN:
	ಸ್ಥಿತಿ ISN_CLEARDICT:
	ಸ್ಥಿತಿ ISN_CMDMOD_REV:
	ಸ್ಥಿತಿ ISN_COMPAREANY:
	ಸ್ಥಿತಿ ISN_COMPAREBLOB:
	ಸ್ಥಿತಿ ISN_COMPAREBOOL:
	ಸ್ಥಿತಿ ISN_COMPAREDICT:
	ಸ್ಥಿತಿ ISN_COMPAREFLOAT:
	ಸ್ಥಿತಿ ISN_COMPAREFUNC:
	ಸ್ಥಿತಿ ISN_COMPARELIST:
	ಸ್ಥಿತಿ ISN_COMPARETUPLE:
	ಸ್ಥಿತಿ ISN_COMPARENR:
	ಸ್ಥಿತಿ ISN_COMPARENULL:
	ಸ್ಥಿತಿ ISN_COMPAREOBJECT:
	ಸ್ಥಿತಿ ISN_COMPARESPECIAL:
	ಸ್ಥಿತಿ ISN_COMPARESTRING:
	ಸ್ಥಿತಿ ISN_CONCAT:
	ಸ್ಥಿತಿ ISN_CONSTRUCT:
	ಸ್ಥಿತಿ ISN_COND2BOOL:
	ಸ್ಥಿತಿ ISN_DEBUG:
	ಸ್ಥಿತಿ ISN_DEFER:
	ಸ್ಥಿತಿ ISN_DROP:
	ಸ್ಥಿತಿ ISN_ECHO:
	ಸ್ಥಿತಿ ISN_ECHOCONSOLE:
	ಸ್ಥಿತಿ ISN_ECHOERR:
	ಸ್ಥಿತಿ ISN_ECHOMSG:
	ಸ್ಥಿತಿ ISN_ECHOWINDOW:
	ಸ್ಥಿತಿ ISN_ENDLOOP:
	ಸ್ಥಿತಿ ISN_ENDTRY:
	ಸ್ಥಿತಿ ISN_EXECCONCAT:
	ಸ್ಥಿತಿ ISN_EXECUTE:
	ಸ್ಥಿತಿ ISN_FINALLY:
	ಸ್ಥಿತಿ ISN_ಮುಕ್ತಾಯ:
	ಸ್ಥಿತಿ ISN_FOR:
	ಸ್ಥಿತಿ ISN_GETITEM:
	ಸ್ಥಿತಿ ISN_GET_OBJ_MEMBER:
	ಸ್ಥಿತಿ ISN_JUMP:
	ಸ್ಥಿತಿ ISN_JUMP_IF_ARG_NOT_ಗಣ:
	ಸ್ಥಿತಿ ISN_JUMP_IF_ARG_ಗಣ:
	ಸ್ಥಿತಿ ISN_LISTAPPEND:
	ಸ್ಥಿತಿ ISN_LISTINDEX:
	ಸ್ಥಿತಿ ISN_LISTSLICE:
	ಸ್ಥಿತಿ ISN_TUPLEINDEX:
	ಸ್ಥಿತಿ ISN_TUPLESLICE:
	ಸ್ಥಿತಿ ISN_ತುಂಬು:
	ಸ್ಥಿತಿ ISN_LOADBDICT:
	ಸ್ಥಿತಿ ISN_LOADGDICT:
	ಸ್ಥಿತಿ ISN_LOADOUTER:
	ಸ್ಥಿತಿ ISN_LOADREG:
	ಸ್ಥಿತಿ ISN_LOADTDICT:
	ಸ್ಥಿತಿ ISN_LOADV:
	ಸ್ಥಿತಿ ISN_LOADWDICT:
	ಸ್ಥಿತಿ ISN_LOCKCONST:
	ಸ್ಥಿತಿ ISN_MEMBER:
	ಸ್ಥಿತಿ ISN_NEGATENR:
	ಸ್ಥಿತಿ ISN_NEWDICT:
	ಸ್ಥಿತಿ ISN_NEWLIST:
	ಸ್ಥಿತಿ ISN_NEWTUPLE:
	ಸ್ಥಿತಿ ISN_NEWPARTIAL:
	ಸ್ಥಿತಿ ISN_OPANY:
	ಸ್ಥಿತಿ ISN_OPFLOAT:
	ಸ್ಥಿತಿ ISN_OPNR:
	ಸ್ಥಿತಿ ISN_PCALL:
	ಸ್ಥಿತಿ ISN_PCALL_ಕೊನೆ:
	ಸ್ಥಿತಿ ISN_PROF_ಕೊನೆ:
	ಸ್ಥಿತಿ ISN_PROF_ಆರಂಭ:
	ಸ್ಥಿತಿ ISN_PUSHBOOL:
	ಸ್ಥಿತಿ ISN_PUSHCHANNEL:
	ಸ್ಥಿತಿ ISN_PUSHF:
	ಸ್ಥಿತಿ ISN_PUSHJOB:
	ಸ್ಥಿತಿ ISN_PUSHNR:
	ಸ್ಥಿತಿ ISN_PUSHOBJ:
	ಸ್ಥಿತಿ ISN_PUSHSPEC:
	ಸ್ಥಿತಿ ISN_PUT:
	ಸ್ಥಿತಿ ISN_IPUT:
	ಸ್ಥಿತಿ ISN_REDIREND:
	ಸ್ಥಿತಿ ISN_REDIRSTART:
	ಸ್ಥಿತಿ ISN_RETURN:
	ಸ್ಥಿತಿ ISN_RETURN_OBJECT:
	ಸ್ಥಿತಿ ISN_RETURN_VOID:
	ಸ್ಥಿತಿ ISN_SHUFFLE:
	ಸ್ಥಿತಿ ISN_ತುಂಡು:
	ಸ್ಥಿತಿ ISN_ಮೂಲ:
	ಸ್ಥಿತಿ ISN_STORE:
	ಸ್ಥಿತಿ ISN_STORENR:
	ಸ್ಥಿತಿ ISN_STOREOUTER:
	ಸ್ಥಿತಿ ISN_STORE_THIS:
	ಸ್ಥಿತಿ ISN_STORERANGE:
	ಸ್ಥಿತಿ ISN_STOREREG:
	ಸ್ಥಿತಿ ISN_STOREV:
	ಸ್ಥಿತಿ ISN_STRINDEX:
	ಸ್ಥಿತಿ ISN_STRSLICE:
	ಸ್ಥಿತಿ ISN_THROW:
	ಸ್ಥಿತಿ ISN_TRYCONT:
	ಸ್ಥಿತಿ ISN_UNLETINDEX:
	ಸ್ಥಿತಿ ISN_UNLETRANGE:
	ಸ್ಥಿತಿ ISN_UNPACK:
	ಸ್ಥಿತಿ ISN_USEDICT:
	ಸ್ಥಿತಿ ISN_WHILE:
	ಸ್ಥಿತಿ ISN_SCRIPTCTX_ಗಣ:
	// nothing allocated
	ನಿಲ್ಲಿಸು;
    }
}

    ಶೂನ್ಯ
clear_instr_ga(garray_T *gap)
{
    ಪೂರ್ಣಾಂಕ idx;

    ಗಾಗಿ (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_ದತ್ತಾಂಶ) + idx);
    ga_clear(gap);
}


#ಆದರೆ_ಅಂತ್ಯ  // defined(FEAT_EVAL)
