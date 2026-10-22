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
#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) જો (cctx->ctx_skip == SKIP_YES) પાછા ખાલી
#define RETURN_OK_IF_SKIP(cctx) જો (cctx->ctx_skip == SKIP_YES) પાછા OK

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
    જો (GA_GROW_FAILS(instr, 1))
	પાછા ખાલી;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    પાછા isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, પૂર્ણાંક drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_ઢગલો.ga_len -= drop;
    પાછા generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    સ્થિર isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    જો ((isn = generate_instr(cctx, isn_type)) == ખાલી)
	પાછા ખાલી;

    જો (ધકેલો_type_stack2(cctx, type == ખાલી ? &t_any : type,
			       decl_type == ખાલી ? &t_any : decl_type) == FAIL)
	પાછા ખાલી;

    પાછા isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    પાછા generate_instr_type2(cctx, isn_type, type, &t_any);
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

    જો ((isn = generate_instr(cctx, ISN_DEBUG)) == ખાલી)
	પાછા ખાલી;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    પાછા isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    પૂર્ણાંક
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.construct.construct_size = કદ(object_T)
			       + cl->class_obj_member_ગણતરી * કદ(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    પાછા OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    પૂર્ણાંક
generate_GET_OBJ_MEMBER(cctx_T *cctx, પૂર્ણાંક idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    જો (isn == ખાલી)
	પાછા FAIL;

    isn->isn_arg.classmember.cm_class = ખાલી;
    isn->isn_arg.classmember.cm_idx = idx;
    પાછા ધકેલો_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    પૂર્ણાંક
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, પૂર્ણાંક idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    જો (isn == ખાલી)
	પાછા FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    પાછા ધકેલો_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    પૂર્ણાંક
generate_STORE_THIS(cctx_T *cctx, પૂર્ણાંક idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    જો (isn == ખાલી)
	પાછા FAIL;

    isn->isn_arg.સંખ્યા = idx;
    પાછા OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    પૂર્ણાંક
may_generate_2STRING(પૂર્ણાંક offset, પૂર્ણાંક tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_ઢગલો(cctx, -1 - offset);

    પસંદગી (type->tt_type)
    {
	// nothing to be done
	સ્થિતિ VAR_STRING: પાછા OK;

	// conversion possible
	સ્થિતિ VAR_SPECIAL:
	સ્થિતિ VAR_BOOL:
	સ્થિતિ VAR_સંખ્યા:
	સ્થિતિ VAR_FLOAT:
			 તોડો;

	// conversion possible (with runtime check)
	સ્થિતિ VAR_ANY:
	સ્થિતિ VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 તોડો;

	// conversion possible when tolerant
	સ્થિતિ VAR_LIST:
	સ્થિતિ VAR_TUPLE:
	સ્થિતિ VAR_DICT:
			 જો (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     તોડો;
			 }
			 જો (tostring_flags & TOSTRING_INTERPOLATE)
			     તોડો;
			 // FALLTHROUGH

	// conversion not possible
	સ્થિતિ VAR_VOID:
	સ્થિતિ VAR_BLOB:
	સ્થિતિ VAR_FUNC:
	સ્થિતિ VAR_PARTIAL:
	સ્થિતિ VAR_JOB:
	સ્થિતિ VAR_CHANNEL:
	સ્થિતિ VAR_INSTR:
	સ્થિતિ VAR_CLASS:
	સ્થિતિ VAR_OBJECT:
	સ્થિતિ VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 પાછા FAIL;
    }

    set_type_on_ઢગલો(cctx, &t_string, -1 - offset);
    જો ((isn = generate_instr(cctx, isntype)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    પાછા OK;
}

    સ્થિર પૂર્ણાંક
check_સંખ્યા_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    જો (!((type1 == VAR_સંખ્યા || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_સંખ્યા || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	જો (check_type_is_કિંમત(typ1) == FAIL
		|| check_type_is_કિંમત(typ2) == FAIL)
	    પાછા FAIL;
	જો (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	નહીંતર
	    semsg(_(e_char_requires_સંખ્યા_or_float_arguments), *op);
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    સ્થિર પૂર્ણાંક
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    માટે (પૂર્ણાંક i = 0; i < tuple_type->tt_argcount; i++)
    {
	જો (ga_grow(gap, 1) == FAIL)
	    પાછા FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    પાછા OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    સ્થિર પૂર્ણાંક
set_tuple_union_type_on_ઢગલો(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, કદ(type_T *), 10);

    જો (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    જો (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    જો ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_ઢગલો(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    પાછા OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    પૂર્ણાંક
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  પદાવલિ_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_સંખ્યા ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    જો (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_સંખ્યા_or_float(type1, type2, (char_u *)"+") == FAIL)
	પાછા FAIL;

    જો (isn != ખાલી)
    {
	જો (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = પદાવલિ_type;
	નહીંતર
	    isn->isn_arg.op.op_type = પદાવલિ_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    જો (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_ઢગલો(cctx, &t_list_any, 0);
    નહીંતર જો (vartype == VAR_TUPLE)
    {
	જો (!check_tuples_addable(type1, type2))
	    પાછા FAIL;

	જો (set_tuple_union_type_on_ઢગલો(type1, type2, cctx) == FAIL)
	    પાછા FAIL;
    }

    પાછા isn == ખાલી ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    જો (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_સંખ્યા
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	પાછા type1->tt_type;
    પાછા VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    પૂર્ણાંક
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_ઢગલો(cctx, 1);
    type2 = get_type_on_ઢગલો(cctx, 0);
    vartype = operator_type(type1, type2);

    પસંદગી (*op)
    {
	સ્થિતિ '+':
		  જો (generate_add_instr(cctx, vartype, type1, type2,
							    પદાવલિ_COPY) == FAIL)
		      પાછા FAIL;
		  તોડો;

	સ્થિતિ '-':
	સ્થિતિ '*':
	સ્થિતિ '/': જો (check_સંખ્યા_or_float(type1, type2, op) == FAIL)
		      પાછા FAIL;
		  જો (vartype == VAR_સંખ્યા)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  નહીંતર જો (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  નહીંતર
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  જો (isn != ખાલી)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? પદાવલિ_MULT : *op == '/'? પદાવલિ_DIV : પદાવલિ_SUB;
		  તોડો;

	સ્થિતિ '%': જો ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_સંખ્યા)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_સંખ્યા))
		  {
		      emsg(_(e_percent_requires_સંખ્યા_arguments));
		      પાછા FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_સંખ્યા ? ISN_OPNR : ISN_OPANY, 1);
		  જો (isn != ખાલી)
		      isn->isn_arg.op.op_type = પદાવલિ_REM;
		  તોડો;
    }

    // correct type of result
    જો (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	જો ((type1->tt_type == VAR_સંખ્યા || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_સંખ્યા || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_ઢગલો(cctx, type, 0);
    }

    પાછા OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    સ્થિર isntype_T
compare_isn_not_કિંમતો(typval_T *tv, type_T *type)
{
    જો (tv != ખાલી)
	(શૂન્ય)check_typval_is_કિંમત(tv);
    નહીંતર
	(શૂન્ય)check_type_is_કિંમત(type);
    પાછા ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    સ્થિર isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != ખાલી ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != ખાલી ? tv2->v_type : type2->tt_type;

    જો (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	પાછા compare_isn_not_કિંમતો(tv1, type1);
    જો (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	પાછા compare_isn_not_કિંમતો(tv2, type2);

    જો (vartype1 == vartype2)
    {
	પસંદગી (vartype1)
	{
	    સ્થિતિ VAR_BOOL: isntype = ISN_COMPAREBOOL; તોડો;
	    સ્થિતિ VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; તોડો;
	    સ્થિતિ VAR_સંખ્યા: isntype = ISN_COMPARENR; તોડો;
	    સ્થિતિ VAR_FLOAT: isntype = ISN_COMPAREFLOAT; તોડો;
	    સ્થિતિ VAR_STRING: isntype = ISN_COMPARESTRING; તોડો;
	    સ્થિતિ VAR_BLOB: isntype = ISN_COMPAREBLOB; તોડો;
	    સ્થિતિ VAR_LIST: isntype = ISN_COMPARELIST; તોડો;
	    સ્થિતિ VAR_TUPLE: isntype = ISN_COMPARETUPLE; તોડો;
	    સ્થિતિ VAR_DICT: isntype = ISN_COMPAREDICT; તોડો;
	    સ્થિતિ VAR_FUNC: isntype = ISN_COMPAREFUNC; તોડો;
	    સ્થિતિ VAR_OBJECT: isntype = ISN_COMPAREOBJECT; તોડો;
	    મૂળભૂત: isntype = ISN_COMPAREANY; તોડો;
	}
    }
    નહીંતર જો (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_સંખ્યા || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_સંખ્યા || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    નહીંતર જો (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	જો ((vartype1 == VAR_SPECIAL
		&& (tv1 != ખાલી ? tv1->vval.v_સંખ્યા == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != ખાલી ? tv2->vval.v_સંખ્યા == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    પાછા ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    જો ((exprtype == પદાવલિ_IS || exprtype == પદાવલિ_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == પદાવલિ_IS ? "is" : "isnot" , vartype_name(vartype1));
	પાછા ISN_DROP;
    }
    જો (!(exprtype == પદાવલિ_IS || exprtype == પદાવલિ_ISNOT
		|| exprtype == પદાવલિ_EQUAL || exprtype == પદાવલિ_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	પાછા ISN_DROP;
    }
    જો (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != પદાવલિ_EQUAL
			&& exprtype != પદાવલિ_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != પદાવલિ_EQUAL
			 && exprtype != પદાવલિ_NEQUAL
			 && exprtype != પદાવલિ_IS
			 && exprtype != પદાવલિ_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	પાછા ISN_DROP;
    }
    પાછા isntype;
}

    પૂર્ણાંક
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    જો (get_compare_isn(type, tv1, tv2, ખાલી, ખાલી) == ISN_DROP)
	પાછા FAIL;
    પાછા OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    પૂર્ણાંક
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, પૂર્ણાંક ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*ઢગલો = &cctx->ctx_type_ઢગલો;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, ખાલી, ખાલી,
		       get_type_on_ઢગલો(cctx, 1), get_type_on_ઢગલો(cctx, 0));
    જો (isntype == ISN_DROP)
	પાછા FAIL;

    જો ((isn = generate_instr(cctx, isntype)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --ઢગલો->ga_len;
    set_type_on_ઢગલો(cctx, &t_bool, 0);

    પાછા OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    પૂર્ણાંક
generate_CONCAT(cctx_T *cctx, પૂર્ણાંક ગણતરી)
{
    isn_T	*isn;
    garray_T	*ઢગલો = &cctx->ctx_type_ઢગલો;

    RETURN_OK_IF_SKIP(cctx);

    જો ((isn = generate_instr(cctx, ISN_CONCAT)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = ગણતરી;

    // drop the argument types
    ઢગલો->ga_len -= ગણતરી - 1;

    પાછા OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    પૂર્ણાંક
generate_2BOOL(cctx_T *cctx, પૂર્ણાંક invert, પૂર્ણાંક offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_2BOOL)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_ઢગલો(cctx, &t_bool, -1 - offset);

    પાછા OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    પૂર્ણાંક
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    જો (generate_instr(cctx, ISN_COND2BOOL) == ખાલી)
	પાછા FAIL;

    // type becomes bool
    set_type_on_ઢગલો(cctx, &t_bool, 0);

    પાછા OK;
}

    પૂર્ણાંક
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	પૂર્ણાંક	    typechk_flags,	// type check flags
	પૂર્ણાંક	    offset,
	પૂર્ણાંક	    is_var,
	પૂર્ણાંક	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == ખાલી)
	પાછા FAIL;
    type_T *tt;
    જો ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_સંખ્યા_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	જો (tt != ખાલી)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    જો (typechk_flags & TYPECHK_સંખ્યા_OK)
		tt->tt_flags |= TTFLAG_સંખ્યા_OK;
	    નહીંતર
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    નહીંતર
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_ઢગલો(cctx, expected, -1 - offset);

    પાછા OK;
}

    પૂર્ણાંક
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_SETTYPE)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    પાછા OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    પૂર્ણાંક
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    જો (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == ખાલી)
	પાછા FAIL;
    પાછા OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    સ્થિર પૂર્ણાંક
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == ખાલી ? &t_any : &class->class_type);
    જો (isn == ખાલી)
	પાછા FAIL;
    isn->isn_arg.classarg = class;
    જો (class != ખાલી)
	++class->class_refcount;
    પાછા OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    પૂર્ણાંક
generate_tv_ધકેલો(cctx_T *cctx, typval_T *tv)
{
    પસંદગી (tv->v_type)
    {
	સ્થિતિ VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_સંખ્યા);
	    તોડો;
	સ્થિતિ VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_સંખ્યા);
	    તોડો;
	સ્થિતિ VAR_સંખ્યા:
	    generate_PUSHNR(cctx, tv->vval.v_સંખ્યા);
	    તોડો;
	સ્થિતિ VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    તોડો;
	સ્થિતિ VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = ખાલી;
	    તોડો;
	સ્થિતિ VAR_LIST:
	    જો (tv->vval.v_list != ખાલી)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    તોડો;
	સ્થિતિ VAR_TUPLE:
	    જો (tv->vval.v_tuple != ખાલી)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    તોડો;
	સ્થિતિ VAR_DICT:
	    જો (tv->vval.v_dict != ખાલી)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    તોડો;
#ifdef FEAT_JOB_CHANNEL
	સ્થિતિ VAR_JOB:
	    જો (tv->vval.v_job != ખાલી)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    તોડો;
	સ્થિતિ VAR_CHANNEL:
	    જો (tv->vval.v_channel != ખાલી)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    તોડો;
#endif
	સ્થિતિ VAR_FUNC:
	    જો (tv->vval.v_string != ખાલી)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, ખાલી, &t_func_unknown, TRUE);
	    તોડો;
	સ્થિતિ VAR_PARTIAL:
	    જો (tv->vval.v_partial != ખાલી)
		iemsg("non-null partial constant not supported");
	    જો (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == ખાલી)
		પાછા FAIL;
	    તોડો;
	સ્થિતિ VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = ખાલી;
	    તોડો;
	સ્થિતિ VAR_OBJECT:
	    જો (tv->vval.v_object != ખાલી)
	    {
		emsg(_(e_cannot_use_non_null_object));
		પાછા FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    તોડો;
	સ્થિતિ VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    તોડો;
	મૂળભૂત:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    પાછા FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    પાછા OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    પૂર્ણાંક
generate_PUSHNR(cctx_T *cctx, varnumber_T સંખ્યા)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_સંખ્યા)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = સંખ્યા;

    જો (સંખ્યા == 0 || સંખ્યા == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_ઢગલો(cctx, &t_સંખ્યા_bool, 0);
    પાછા OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    પૂર્ણાંક
generate_PUSHBOOL(cctx_T *cctx, varnumber_T સંખ્યા)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = સંખ્યા;

    પાછા OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    પૂર્ણાંક
generate_PUSHSPEC(cctx_T *cctx, varnumber_T સંખ્યા)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     સંખ્યા == VVAL_NULL ? &t_null : &t_none)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = સંખ્યા;

    પાછા OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    પૂર્ણાંક
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.fnumber = fnumber;

    પાછા OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    પૂર્ણાંક
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    પૂર્ણાંક		ret = OK;

    જો (cctx->ctx_skip != SKIP_YES)
    {
	જો ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == ખાલી)
	    ret = FAIL;
	નહીંતર
	{
	    isn->isn_arg.string = str == ખાલી ? ખાલી : *str;
	    પાછા OK;
	}
    }
    જો (str != ખાલી)
	VIM_CLEAR(*str);
    પાછા ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    પૂર્ણાંક
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    જો (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == ખાલી)
	પાછા FAIL;
    પાછા OK;
#નહીંતર
    emsg(_(e_channel_job_feature_not_available));
    પાછા FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    પૂર્ણાંક
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    જો (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == ખાલી)
	પાછા FAIL;
    પાછા OK;
#નહીંતર
    emsg(_(e_channel_job_feature_not_available));
    પાછા FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    પૂર્ણાંક
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.blob = blob;

    પાછા OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    પૂર્ણાંક
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, પૂર્ણાંક may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == ખાલી)
	પાછા FAIL;
    જો (name == ખાલી)
	funcname = ખાલી;
    નહીંતર જો (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != ખાલી)	    // autoload
	funcname = vim_strsave(name);
    નહીંતર
    {
	funcname = alloc(STRLEN(name) + 3);
	જો (funcname != ખાલી)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    પાછા OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    પૂર્ણાંક
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.string = vim_strsave(name);
    જો (isn->isn_arg.string == ખાલી)
	પાછા FAIL;
    પાછા OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    પૂર્ણાંક
generate_GETITEM(cctx_T *cctx, પૂર્ણાંક index, પૂર્ણાંક with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_ઢગલો(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    જો ((isn = generate_instr(cctx, ISN_GETITEM)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    પાછા ધકેલો_type_ઢગલો(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    સ્થિર શૂન્ય
set_tuple_slice_type_on_ઢગલો(type_T *type, પૂર્ણાંક ગણતરી, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    જો ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= ગણતરી)
    {
	set_type_on_ઢગલો(cctx, &t_tuple_any, 0);
	પાછા;
    }

    ga_init2(&tuple_types_ga, કદ(type_T *), 10);

    માટે (પૂર્ણાંક i = ગણતરી; i < type->tt_argcount; i++)
    {
	જો (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_ઢગલો(cctx, &t_tuple_any, 0);
	    પાછા;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] =
							     type->tt_args[i];
	tuple_types_ga.ga_len++;
    }

    set_type_on_ઢગલો(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    પૂર્ણાંક
generate_SLICE(cctx_T *cctx, પૂર્ણાંક ગણતરી)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_SLICE)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = ગણતરી;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_ઢગલો(cctx, 0);
    જો (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_ઢગલો(type, ગણતરી, cctx);

    પાછા OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    પૂર્ણાંક
generate_CHECKLEN(cctx_T *cctx, પૂર્ણાંક min_len, પૂર્ણાંક more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    જો ((isn = generate_instr(cctx, ISN_CHECKLEN)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    પાછા OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    પૂર્ણાંક
generate_STORE(cctx_T *cctx, isntype_T isn_type, પૂર્ણાંક idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_drop(cctx, isn_type, 1)) == ખાલી)
	પાછા FAIL;
    જો (name != ખાલી)
	isn->isn_arg.string = vim_strsave(name);
    નહીંતર
	isn->isn_arg.સંખ્યા = idx;

    પાછા OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    પૂર્ણાંક
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	પૂર્ણાંક	    load,
	class_T	    *cl,
	પૂર્ણાંક	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    નહીંતર
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    જો (isn == ખાલી)
	પાછા FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    પાછા OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    સ્થિર પૂર્ણાંક
generate_STOREOUTER(
	cctx_T	    *cctx,
	પૂર્ણાંક	    idx,
	પૂર્ણાંક	    level,
	પૂર્ણાંક	    loop_depth,
	પૂર્ણાંક	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == ખાલી)
	પાછા FAIL;
    જો (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    નહીંતર
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    પાછા OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    પૂર્ણાંક
generate_STORENR(cctx_T *cctx, પૂર્ણાંક idx, varnumber_T કિંમત)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_STORENR)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = કિંમત;

    પાછા OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    સ્થિર પૂર્ણાંક
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	પૂર્ણાંક	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_drop(cctx, isn_type, 1)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    પાછા OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    પૂર્ણાંક
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	પૂર્ણાંક	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_type2(cctx, isn_type, type, type)) == ખાલી)
	પાછા FAIL;
    જો (name != ખાલી)
	isn->isn_arg.string = vim_strsave(name);
    નહીંતર
	isn->isn_arg.સંખ્યા = idx;

    પાછા OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    પૂર્ણાંક
generate_LOADOUTER(
	cctx_T	    *cctx,
	પૂર્ણાંક	    idx,
	પૂર્ણાંક	    nesting,
	પૂર્ણાંક	    loop_depth,
	પૂર્ણાંક	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == ખાલી)
	પાછા FAIL;
    જો (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    નહીંતર
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    પાછા OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    પૂર્ણાંક
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    પૂર્ણાંક	    di_flags;
    પૂર્ણાંક	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    જો (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	પાછા FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    પાછા generate_LOAD(cctx, ISN_LOADV, vidx, ખાલી, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    પૂર્ણાંક
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, પૂર્ણાંક forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, isn_type)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    પાછા OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    પૂર્ણાંક
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    જો (generate_instr(cctx, ISN_LOCKCONST) == ખાલી)
	પાછા FAIL;
    પાછા OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    પૂર્ણાંક
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	પૂર્ણાંક	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    નહીંતર
	isn = generate_instr_drop(cctx, isn_type, 1);
    જો (isn == ખાલી)
	પાછા FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    પાછા OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    પૂર્ણાંક
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	પૂર્ણાંક	    sid,
	પૂર્ણાંક	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    જો (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    નહીંતર
	isn = generate_instr_drop(cctx, isn_type, 1);
    જો (isn == ખાલી)
	પાછા FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    જો (sref == ખાલી)
	પાછા FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    પાછા OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    પૂર્ણાંક
generate_NEWLIST(cctx_T *cctx, પૂર્ણાંક ગણતરી, પૂર્ણાંક use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_NEWLIST)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = use_null ? -1 : ગણતરી;

    // Get the member type and the declared member type from all the items on
    // the stack.
    જો ((member_type = get_member_type_from_ઢગલો(ગણતરી, 1, cctx)) == ખાલી)
	પાછા FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_ઢગલો.ga_len -= ગણતરી;

    // add the list type to the type stack
    પાછા ધકેલો_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    પૂર્ણાંક
generate_NEWTUPLE(cctx_T *cctx, પૂર્ણાંક ગણતરી, પૂર્ણાંક use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = use_null ? -1 : ગણતરી;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, કદ(type_T *), 10);

    જો (get_tuple_type_from_ઢગલો(ગણતરી, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	પાછા FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_ઢગલો.ga_len -= ગણતરી;

    // add the tuple type to the type stack
    પાછા ધકેલો_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    પૂર્ણાંક
generate_NEWDICT(cctx_T *cctx, પૂર્ણાંક ગણતરી, પૂર્ણાંક use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_NEWDICT)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = use_null ? -1 : ગણતરી;

    જો ((member_type = get_member_type_from_ઢગલો(ગણતરી, 2, cctx)) == ખાલી)
	પાછા FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_ઢગલો.ga_len -= 2 * ગણતરી;

    // add the dict type to the type stack
    પાછા ધકેલો_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    પૂર્ણાંક
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	પૂર્ણાંક	    object_method,
	પૂર્ણાંક	    fi,
	પૂર્ણાંક	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    પૂર્ણાંક		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_FUNCREF)) == ખાલી)
	પાછા FAIL;
    જો (isn_idx != ખાલી)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    જો (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != ખાલી)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	જો (extra == ખાલી)
	    પાછા FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	જો (cl != ખાલી)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    જો (ufunc->uf_def_status == UF_NOT_COMPILED || cl != ખાલી)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    જો (ufunc->uf_def_status != UF_NOT_COMPILED && cl == ખાલી)
    {
	જો (isn_idx == ખાલી && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (શૂન્ય)compile_def_function(ufunc, FALSE, CT_NONE, ખાલી);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_closure = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    જો ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == ખાલી ? &t_func_any : ufunc->uf_func_type;
    પાછા ધકેલો_type_ઢગલો(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    પૂર્ણાંક
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    પૂર્ણાંક		ret = OK;

    જો (cctx->ctx_skip != SKIP_YES)
    {
	જો ((isn = generate_instr(cctx, ISN_NEWFUNC)) == ખાલી)
	    ret = FAIL;
	નહીંતર
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    જો (arg == ખાલી)
		ret = FAIL;
	    નહીંતર
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(શૂન્ય)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		પાછા OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    પાછા ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    પૂર્ણાંક
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_DEF)) == ખાલી)
	પાછા FAIL;
    જો (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	જો (isn->isn_arg.string == ખાલી)
	    પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    પૂર્ણાંક
generate_JUMP(cctx_T *cctx, jumpwhen_T when, પૂર્ણાંક where)
{
    isn_T	*isn;
    garray_T	*ઢગલો = &cctx->ctx_type_ઢગલો;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_JUMP)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    જો (when != JUMP_ALWAYS && ઢગલો->ga_len > 0)
	--ઢગલો->ga_len;

    પાછા OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    પૂર્ણાંક
generate_WHILE(cctx_T *cctx, પૂર્ણાંક funcref_idx)
{
    isn_T	*isn;
    garray_T	*ઢગલો = &cctx->ctx_type_ઢગલો;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_WHILE)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    જો (ઢગલો->ga_len > 0)
	--ઢગલો->ga_len;

    પાછા OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    પૂર્ણાંક
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, પૂર્ણાંક arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, isn_type)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    પાછા OK;
}

    પૂર્ણાંક
generate_FOR(cctx_T *cctx, પૂર્ણાંક loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_FOR)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    પાછા ધકેલો_type_ઢગલો(cctx, &t_any);
}

    પૂર્ણાંક
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_ENDLOOP)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_ગણતરી;
    isn->isn_arg.endloop.end_var_ગણતરી =
			   cctx->ctx_locals.ga_len - loop_info->li_local_ગણતરી;
    પાછા OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    પૂર્ણાંક
generate_TRYCONT(cctx_T *cctx, પૂર્ણાંક levels, પૂર્ણાંક where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_TRYCONT)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    પાછા OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    પૂર્ણાંક
check_internal_func_args(
	cctx_T	*cctx,
	પૂર્ણાંક	func_idx,
	પૂર્ણાંક	argcount,
	પૂર્ણાંક	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*ઢગલો = &cctx->ctx_type_ઢગલો;
    પૂર્ણાંક		argoff = check_internal_func(func_idx, argcount);

    જો (argoff < 0)
	પાછા FAIL;

    જો (method_call && argoff > 1)
    {
	જો (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    પાછા FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	જો (isn  == ખાલી)
	    પાછા FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    જો (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)ઢગલો->ga_data) + ઢગલો->ga_len - argcount;

	// Check the types of the arguments.
	જો (method_call && argoff > 1)
	{
	    પૂર્ણાંક i;

	    માટે (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	નહીંતર
	{
	    પૂર્ણાંક i;

	    માટે (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	જો (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    પૂર્ણાંક
generate_BCALL(cctx_T *cctx, પૂર્ણાંક func_idx, પૂર્ણાંક argcount, પૂર્ણાંક method_call)
{
    isn_T	*isn;
    garray_T	*ઢગલો = &cctx->ctx_type_ઢગલો;
    type2_T	*argtypes = ખાલી;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = ખાલી;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    જો (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	પાછા FAIL;

    જો (internal_func_is_map(func_idx))
	maptype = argtypes;

    જો ((isn = generate_instr(cctx, ISN_BCALL)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    ઢગલો->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    જો (ધકેલો_type_stack2(cctx, type, decl_type) == FAIL)
	પાછા FAIL;

    જો (maptype != ખાલી && maptype[0].type_decl->tt_member != ખાલી
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    પાછા OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    પૂર્ણાંક
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_ઢગલો(cctx, 1);
    જો (arg_type_modifiable(list_type, 1) == FAIL)
	પાછા FAIL;
    item_type = get_type_on_ઢગલો(cctx, 0);
    expected = list_type->tt_member;
    જો (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	પાછા FAIL;

    જો (generate_instr(cctx, ISN_LISTAPPEND) == ખાલી)
	પાછા FAIL;

    --cctx->ctx_type_ઢગલો.ga_len;	    // drop the argument
    પાછા OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    પૂર્ણાંક
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    જો (arg_type_modifiable(get_decl_type_on_ઢગલો(cctx, 1), 1) == FAIL)
	પાછા FAIL;
    item_type = get_type_on_ઢગલો(cctx, 0);
    જો (need_type(item_type, &t_સંખ્યા, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	પાછા FAIL;

    જો (generate_instr(cctx, ISN_BLOBAPPEND) == ખાલી)
	પાછા FAIL;

    --cctx->ctx_type_ઢગલો.ga_len;	    // drop the argument
    પાછા OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    સ્થિર isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    જો (cl != ખાલી)
	પાછા ISN_METHODCALL;

    જો (ufunc->uf_def_status == UF_NOT_COMPILED)
	પાછા ISN_UCALL;

    // function invoked from a lambda
    જો (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	જો (ufunc->uf_name[0] == K_SPECIAL)
	    પાછા ISN_UCALL;
    }

    પાછા ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    પૂર્ણાંક
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	પૂર્ણાંક	    mi,
	પૂર્ણાંક	    pushed_argcount,
	પૂર્ણાંક	    is_super)
{
    isn_T	*isn;
    પૂર્ણાંક		regular_args = ufunc->uf_args.ga_len;
    પૂર્ણાંક		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    જો (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	પાછા FAIL;
    }
    જો (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	પાછા FAIL;
    }

    જો (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	પૂર્ણાંક		i;
	compiletype_T	compile_type;

	માટે (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_ઢગલો(cctx, argcount - i - 1);
	    જો (check_type_is_કિંમત(actual) == FAIL)
		પાછા FAIL;
	    જો (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		ચાલુ;
	    }
	    જો (i < regular_args)
	    {
		જો (ufunc->uf_arg_types == ખાલી)
		    ચાલુ;
		expected = ufunc->uf_arg_types[i];
	    }
	    નહીંતર જો (ufunc->uf_va_type == ખાલી
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    નહીંતર
		expected = ufunc->uf_va_type->tt_member;
	    જો (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		પાછા FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	જો (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == ખાલી,
						   compile_type, ખાલી) == FAIL)
	    પાછા FAIL;
    }
    જો (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	પાછા FAIL;
    }

    જો ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == ખાલી)
	પાછા FAIL;
    જો (cl != ખાલી /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	જો (isn->isn_arg.mfunc == ખાલી)
	    પાછા FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = ખાલી;
	જો (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = ખાલી;
	isn->isn_arg.mfunc->cmf_cache_generic = ખાલી;
	isn->isn_arg.mfunc->cmf_cache_ufunc = ખાલી;
    }
    નહીંતર જો (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    નહીંતર
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_ઢગલો.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    જો (ufunc->uf_class != ખાલી)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_ઢગલો(cctx, 0);
	જો (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_ઢગલો.ga_len--;
    }

    // add return type
    પાછા ધકેલો_type_ઢગલો(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    પૂર્ણાંક
generate_UCALL(cctx_T *cctx, char_u *name, પૂર્ણાંક argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_UCALL)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_ઢગલો.ga_len -= argcount;

    // add return value
    પાછા ધકેલો_type_ઢગલો(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    પૂર્ણાંક
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	પૂર્ણાંક	argcount,
	પૂર્ણાંક	at_ટોચ,
	char_u	*name)
{
    જો (type->tt_argcount != -1)
    {
	પૂર્ણાંક	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	જો (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    પાછા FAIL;
	}
	જો (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    પાછા FAIL;
	}
	જો (type->tt_args != ખાલી)
	{
	    પૂર્ણાંક i;

	    માટે (i = 0; i < argcount; ++i)
	    {
		પૂર્ણાંક	offset = -argcount + i - (at_ટોચ ? 0 : 1);
		type_T	*actual = get_type_on_ઢગલો(cctx, -1 - offset);
		type_T	*expected;

		જો (check_type_is_કિંમત(actual) == FAIL)
		    પાછા FAIL;
		જો (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    જો (expected != ખાલી && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    જો (expected == ખાલી)
			expected = &t_any;
		}
		નહીંતર જો (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		નહીંતર
		    expected = type->tt_args[i];
		જો (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    પાછા FAIL;
		}
	    }
	}
    }

    પાછા OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    પૂર્ણાંક
generate_PCALL(
	cctx_T	*cctx,
	પૂર્ણાંક	argcount,
	char_u	*name,
	type_T	*type,
	પૂર્ણાંક	at_ટોચ)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    જો (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    નહીંતર જો (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	જો (check_func_args_from_type(cctx, type, argcount, at_ટોચ, name)
								       == FAIL)
	    પાછા FAIL;

	ret_type = type->tt_member;
	જો (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    નહીંતર
    {
	semsg(_(e_not_callable_type_str), name);
	પાછા FAIL;
    }

    જો ((isn = generate_instr(cctx, ISN_PCALL)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.pfunc.cpf_ટોચ = at_ટોચ;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_ઢગલો.ga_len -= argcount + 1;

    // push the return value
    ધકેલો_type_ઢગલો(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    જો (at_ટોચ && generate_instr(cctx, ISN_PCALL_END) == ખાલી)
	પાછા FAIL;

    પાછા OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    પૂર્ણાંક
generate_DEFER(cctx_T *cctx, પૂર્ણાંક var_idx, પૂર્ણાંક argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    પાછા OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    પૂર્ણાંક
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_ઢગલો(cctx, 0);
    જો (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	અક્ષર *tofree;

	semsg(_(e_expected_dictionary_for_using_ચાવી_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	પાછા FAIL;
    }
    // change dict type to dict member type
    જો (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_ઢગલો(cctx, ntype, 0);
    }

    પાછા OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    પૂર્ણાંક
generate_ECHO(cctx_T *cctx, પૂર્ણાંક with_white, પૂર્ણાંક ગણતરી)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_drop(cctx, ISN_ECHO, ગણતરી)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_ગણતરી = ગણતરી;

    પાછા OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    પૂર્ણાંક
generate_MULT_પદાવલિ(cctx_T *cctx, isntype_T isn_type, પૂર્ણાંક ગણતરી)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr_drop(cctx, isn_type, ગણતરી)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = ગણતરી;
    પાછા OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    પૂર્ણાંક
generate_ECHOWINDOW(cctx_T *cctx, પૂર્ણાંક ગણતરી, long time)
{
    isn_T	*isn;

    જો ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, ગણતરી)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.echowin.ewin_ગણતરી = ગણતરી;
    isn->isn_arg.echowin.ewin_time = time;
    પાછા OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    પૂર્ણાંક
generate_SOURCE(cctx_T *cctx, પૂર્ણાંક sid)
{
    isn_T	*isn;

    જો ((isn = generate_instr(cctx, ISN_SOURCE)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = sid;

    પાછા OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    પૂર્ણાંક
generate_PUT(cctx_T *cctx, પૂર્ણાંક regname, linenr_T lnum, પૂર્ણાંક fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    જો (isn == ખાલી)
	પાછા FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    પાછા OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    પૂર્ણાંક
generate_LOCKUNLOCK(cctx_T *cctx, char_u *લીટી, પૂર્ણાંક is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == ખાલી)
	પાછા FAIL;
    class_T *cl = cctx->ctx_ufunc != ખાલી ? cctx->ctx_ufunc->uf_class : ખાલી;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(લીટી);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    જો (cl != ખાલી)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    પાછા OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    પૂર્ણાંક
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *લીટી)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, isntype)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.string = vim_strsave(લીટી);
    પાછા OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    પૂર્ણાંક
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    પૂર્ણાંક		ret = OK;

    જો (cctx->ctx_skip != SKIP_YES)
    {
	જો ((isn = generate_instr(cctx, isntype)) == ખાલી)
	    ret = FAIL;
	નહીંતર
	{
	    isn->isn_arg.string = str;
	    પાછા OK;
	}
    }
    vim_free(str);
    પાછા ret;
}

    પૂર્ણાંક
generate_LEGACY_EVAL(cctx_T *cctx, char_u *લીટી)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.string = vim_strsave(લીટી);

    પાછા ધકેલો_type_ઢગલો(cctx, &t_any);
}

    પૂર્ણાંક
generate_EXECCONCAT(cctx_T *cctx, પૂર્ણાંક ગણતરી)
{
    isn_T	*isn;

    જો ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, ગણતરી)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.સંખ્યા = ગણતરી;
    પાછા OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    પૂર્ણાંક
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    જો ((isn = generate_instr(cctx, ISN_RANGE)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.string = range;

    પાછા ધકેલો_type_ઢગલો(cctx, &t_સંખ્યા);
}

    પૂર્ણાંક
generate_UNPACK(cctx_T *cctx, પૂર્ણાંક var_ગણતરી, પૂર્ણાંક semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_UNPACK)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.unpack.unp_ગણતરી = var_ગણતરી;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    પાછા OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    પૂર્ણાંક
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    જો (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	જો ((isn = generate_instr(cctx, ISN_CMDMOD)) == ખાલી)
	    પાછા FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	જો (isn->isn_arg.cmdmod.cf_cmdmod == ખાલી)
	    પાછા FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, કદ(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = ખાલી;
    }

    પાછા OK;
}

    પૂર્ણાંક
generate_undo_cmdmods(cctx_T *cctx)
{
    જો (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == ખાલી)
	પાછા FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    પાછા OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    પૂર્ણાંક
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	પૂર્ણાંક		opt_flags,
	પૂર્ણાંક		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    પસંદગી (dest)
    {
	સ્થિતિ dest_option:
	    પાછા generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	સ્થિતિ dest_func_option:
	    પાછા generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	સ્થિતિ dest_global:
	    // include g: with the name, easier to execute that way
	    પાછા generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == ખાલી
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	સ્થિતિ dest_buffer:
	    // include b: with the name, easier to execute that way
	    પાછા generate_STORE(cctx, ISN_STOREB, 0, name);
	સ્થિતિ dest_window:
	    // include w: with the name, easier to execute that way
	    પાછા generate_STORE(cctx, ISN_STOREW, 0, name);
	સ્થિતિ dest_tab:
	    // include t: with the name, easier to execute that way
	    પાછા generate_STORE(cctx, ISN_STORET, 0, name);
	સ્થિતિ dest_env:
	    પાછા generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	સ્થિતિ dest_reg:
	    પાછા generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], ખાલી);
	સ્થિતિ dest_vimvar:
	    પાછા generate_STORE(cctx, ISN_STOREV, vimvaridx, ખાલી);
	સ્થિતિ dest_script:
	સ્થિતિ dest_script_v9:
	    {
		પૂર્ણાંક	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		પૂર્ણાંક	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		જો (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    જો (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == ખાલી)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			જો (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    પાછા FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    પાછા generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		પાછા generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	સ્થિતિ dest_class_member:
	    પાછા generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	સ્થિતિ dest_local:
	સ્થિતિ dest_પદાવલિ:
	    // cannot happen
	    તોડો;
    }
    પાછા FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    પૂર્ણાંક
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    માટે (;;)
    {
	જો (scope == ખાલી)
	    તોડો;
	જો (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    પાછા TRUE;
	scope = scope->se_outer;
    }
    પાછા FALSE;
}

    પૂર્ણાંક
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, પૂર્ણાંક instr_ગણતરી, પૂર્ણાંક is_decl)
{
    જો (lhs->lhs_dest != dest_local)
	પાછા generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    જો (lhs->lhs_lvar == ખાલી)
	પાછા OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    જો (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_ગણતરી + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.સંખ્યા;
	garray_T    *ઢગલો = &cctx->ctx_type_ઢગલો;

	જો (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	નહીંતર
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	જો (ઢગલો->ga_len > 0)
	    --ઢગલો->ga_len;
    }
    નહીંતર જો (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    નહીંતર
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, ખાલી);
    પાછા OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    પૂર્ણાંક
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    જો ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == ખાલી)
	પાછા FAIL;
    isn->isn_arg.setsctx = new_sctx;
    પાછા OK;
}

#જો defined(FEAT_PROFILE)
    શૂન્ય
may_generate_prof_end(cctx_T *cctx, પૂર્ણાંક prof_lnum)
{
    જો (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_END);
}
#endif


/*
 * Delete an instruction, free what it contains.
 */
    શૂન્ય
delete_instr(isn_T *isn)
{
    પસંદગી (isn->isn_type)
    {
	સ્થિતિ ISN_AUTOLOAD:
	સ્થિતિ ISN_DEF:
	સ્થિતિ ISN_EXEC:
	સ્થિતિ ISN_EXECRANGE:
	સ્થિતિ ISN_EXEC_SPLIT:
	સ્થિતિ ISN_LEGACY_EVAL:
	સ્થિતિ ISN_LOADAUTO:
	સ્થિતિ ISN_LOADB:
	સ્થિતિ ISN_LOADENV:
	સ્થિતિ ISN_LOADG:
	સ્થિતિ ISN_LOADOPT:
	સ્થિતિ ISN_LOADT:
	સ્થિતિ ISN_LOADW:
	સ્થિતિ ISN_PUSHEXC:
	સ્થિતિ ISN_PUSHFUNC:
	સ્થિતિ ISN_PUSHS:
	સ્થિતિ ISN_RANGE:
	સ્થિતિ ISN_STOREAUTO:
	સ્થિતિ ISN_STOREB:
	સ્થિતિ ISN_STOREENV:
	સ્થિતિ ISN_STOREG:
	સ્થિતિ ISN_STORET:
	સ્થિતિ ISN_STOREW:
	સ્થિતિ ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    તોડો;

	સ્થિતિ ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    તોડો;

	સ્થિતિ ISN_SUBSTITUTE:
	    {
		પૂર્ણાંક	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		માટે (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    તોડો;

	સ્થિતિ ISN_INSTR:
	    {
		પૂર્ણાંક	idx;
		isn_T	*list = isn->isn_arg.instr;

		માટે (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    તોડો;

	સ્થિતિ ISN_LOADS:
	સ્થિતિ ISN_LOADEXPORT:
	સ્થિતિ ISN_STORES:
	સ્થિતિ ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    તોડો;

	સ્થિતિ ISN_UNLET:
	સ્થિતિ ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    તોડો;

	સ્થિતિ ISN_STOREOPT:
	સ્થિતિ ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    તોડો;

	સ્થિતિ ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    તોડો;

	સ્થિતિ ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    તોડો;

	સ્થિતિ ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    તોડો;

	સ્થિતિ ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		જો (extra == ખાલી || extra->fre_func_name == ખાલી)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    જો (ufunc != ખાલી && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		જો (extra != ખાલી)
		{
		    char_u *name = extra->fre_func_name;

		    જો (name != ખાલી)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    જો (extra->fre_class != ખાલી)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    તોડો;

	સ્થિતિ ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		જો (dfunc->df_ufunc != ખાલી
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    તોડો;

	સ્થિતિ ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    તોડો;

	સ્થિતિ ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		જો (arg != ખાલી)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    જો (ufunc != ખાલી)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    તોડો;

	સ્થિતિ ISN_CHECKTYPE:
	સ્થિતિ ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    તોડો;

	સ્થિતિ ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    તોડો;

	સ્થિતિ ISN_LOADSCRIPT:
	સ્થિતિ ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    તોડો;

	સ્થિતિ ISN_LOAD_CLASSMEMBER:
	સ્થિતિ ISN_STORE_CLASSMEMBER:
	સ્થિતિ ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    તોડો;

	સ્થિતિ ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    તોડો;

	સ્થિતિ ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    તોડો;

	સ્થિતિ ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    તોડો;

	સ્થિતિ ISN_2BOOL:
	સ્થિતિ ISN_2STRING:
	સ્થિતિ ISN_2STRING_ANY:
	સ્થિતિ ISN_ADDBLOB:
	સ્થિતિ ISN_ADDLIST:
	સ્થિતિ ISN_ADDTUPLE:
	સ્થિતિ ISN_ANYINDEX:
	સ્થિતિ ISN_ANYSLICE:
	સ્થિતિ ISN_BCALL:
	સ્થિતિ ISN_BLOBAPPEND:
	સ્થિતિ ISN_BLOBINDEX:
	સ્થિતિ ISN_BLOBSLICE:
	સ્થિતિ ISN_CATCH:
	સ્થિતિ ISN_CEXPR_AUCMD:
	સ્થિતિ ISN_CHECKLEN:
	સ્થિતિ ISN_CLEARDICT:
	સ્થિતિ ISN_CMDMOD_REV:
	સ્થિતિ ISN_COMPAREANY:
	સ્થિતિ ISN_COMPAREBLOB:
	સ્થિતિ ISN_COMPAREBOOL:
	સ્થિતિ ISN_COMPAREDICT:
	સ્થિતિ ISN_COMPAREFLOAT:
	સ્થિતિ ISN_COMPAREFUNC:
	સ્થિતિ ISN_COMPARELIST:
	સ્થિતિ ISN_COMPARETUPLE:
	સ્થિતિ ISN_COMPARENR:
	સ્થિતિ ISN_COMPARENULL:
	સ્થિતિ ISN_COMPAREOBJECT:
	સ્થિતિ ISN_COMPARESPECIAL:
	સ્થિતિ ISN_COMPARESTRING:
	સ્થિતિ ISN_CONCAT:
	સ્થિતિ ISN_CONSTRUCT:
	સ્થિતિ ISN_COND2BOOL:
	સ્થિતિ ISN_DEBUG:
	સ્થિતિ ISN_DEFER:
	સ્થિતિ ISN_DROP:
	સ્થિતિ ISN_ECHO:
	સ્થિતિ ISN_ECHOCONSOLE:
	સ્થિતિ ISN_ECHOERR:
	સ્થિતિ ISN_ECHOMSG:
	સ્થિતિ ISN_ECHOWINDOW:
	સ્થિતિ ISN_ENDLOOP:
	સ્થિતિ ISN_ENDTRY:
	સ્થિતિ ISN_EXECCONCAT:
	સ્થિતિ ISN_EXECUTE:
	સ્થિતિ ISN_FINALLY:
	સ્થિતિ ISN_FINISH:
	સ્થિતિ ISN_FOR:
	સ્થિતિ ISN_GETITEM:
	સ્થિતિ ISN_GET_OBJ_MEMBER:
	સ્થિતિ ISN_JUMP:
	સ્થિતિ ISN_JUMP_IF_ARG_NOT_SET:
	સ્થિતિ ISN_JUMP_IF_ARG_SET:
	સ્થિતિ ISN_LISTAPPEND:
	સ્થિતિ ISN_LISTINDEX:
	સ્થિતિ ISN_LISTSLICE:
	સ્થિતિ ISN_TUPLEINDEX:
	સ્થિતિ ISN_TUPLESLICE:
	સ્થિતિ ISN_LOAD:
	સ્થિતિ ISN_LOADBDICT:
	સ્થિતિ ISN_LOADGDICT:
	સ્થિતિ ISN_LOADOUTER:
	સ્થિતિ ISN_LOADREG:
	સ્થિતિ ISN_LOADTDICT:
	સ્થિતિ ISN_LOADV:
	સ્થિતિ ISN_LOADWDICT:
	સ્થિતિ ISN_LOCKCONST:
	સ્થિતિ ISN_MEMBER:
	સ્થિતિ ISN_NEGATENR:
	સ્થિતિ ISN_NEWDICT:
	સ્થિતિ ISN_NEWLIST:
	સ્થિતિ ISN_NEWTUPLE:
	સ્થિતિ ISN_NEWPARTIAL:
	સ્થિતિ ISN_OPANY:
	સ્થિતિ ISN_OPFLOAT:
	સ્થિતિ ISN_OPNR:
	સ્થિતિ ISN_PCALL:
	સ્થિતિ ISN_PCALL_END:
	સ્થિતિ ISN_PROF_END:
	સ્થિતિ ISN_PROF_START:
	સ્થિતિ ISN_PUSHBOOL:
	સ્થિતિ ISN_PUSHCHANNEL:
	સ્થિતિ ISN_PUSHF:
	સ્થિતિ ISN_PUSHJOB:
	સ્થિતિ ISN_PUSHNR:
	સ્થિતિ ISN_PUSHOBJ:
	સ્થિતિ ISN_PUSHSPEC:
	સ્થિતિ ISN_PUT:
	સ્થિતિ ISN_IPUT:
	સ્થિતિ ISN_REDIREND:
	સ્થિતિ ISN_REDIRSTART:
	સ્થિતિ ISN_RETURN:
	સ્થિતિ ISN_RETURN_OBJECT:
	સ્થિતિ ISN_RETURN_VOID:
	સ્થિતિ ISN_SHUFFLE:
	સ્થિતિ ISN_SLICE:
	સ્થિતિ ISN_SOURCE:
	સ્થિતિ ISN_STORE:
	સ્થિતિ ISN_STORENR:
	સ્થિતિ ISN_STOREOUTER:
	સ્થિતિ ISN_STORE_THIS:
	સ્થિતિ ISN_STORERANGE:
	સ્થિતિ ISN_STOREREG:
	સ્થિતિ ISN_STOREV:
	સ્થિતિ ISN_STRINDEX:
	સ્થિતિ ISN_STRSLICE:
	સ્થિતિ ISN_THROW:
	સ્થિતિ ISN_TRYCONT:
	સ્થિતિ ISN_UNLETINDEX:
	સ્થિતિ ISN_UNLETRANGE:
	સ્થિતિ ISN_UNPACK:
	સ્થિતિ ISN_USEDICT:
	સ્થિતિ ISN_WHILE:
	સ્થિતિ ISN_SCRIPTCTX_SET:
	// nothing allocated
	તોડો;
    }
}

    શૂન્ય
clear_instr_ga(garray_T *gap)
{
    પૂર્ણાંક idx;

    માટે (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
