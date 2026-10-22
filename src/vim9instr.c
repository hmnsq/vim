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
#अन्तर्भाव "vim.h"

#यदि defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) यदि (cctx->ctx_skip == SKIP_YES) प्रत्यागम रिक्त
#define RETURN_OK_IF_SKIP(cctx) यदि (cctx->ctx_skip == SKIP_YES) प्रत्यागम OK

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
    यदि (GA_GROW_FAILS(instr, 1))
	प्रत्यागम रिक्त;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    प्रत्यागम isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, पूर्णाङ्क drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_चळत.ga_len -= drop;
    प्रत्यागम generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    स्थिर isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    यदि ((isn = generate_instr(cctx, isn_type)) == रिक्त)
	प्रत्यागम रिक्त;

    यदि (ढकला_type_stack2(cctx, type == रिक्त ? &t_any : type,
			       decl_type == रिक्त ? &t_any : decl_type) == FAIL)
	प्रत्यागम रिक्त;

    प्रत्यागम isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    प्रत्यागम generate_instr_type2(cctx, isn_type, type, &t_any);
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

    यदि ((isn = generate_instr(cctx, ISN_DEBUG)) == रिक्त)
	प्रत्यागम रिक्त;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    प्रत्यागम isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    पूर्णाङ्क
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.construct.construct_size = परिमाण(object_T)
			       + cl->class_obj_member_मोजणी * परिमाण(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    प्रत्यागम OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    पूर्णाङ्क
generate_GET_OBJ_MEMBER(cctx_T *cctx, पूर्णाङ्क idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    यदि (isn == रिक्त)
	प्रत्यागम FAIL;

    isn->isn_arg.classmember.cm_class = रिक्त;
    isn->isn_arg.classmember.cm_idx = idx;
    प्रत्यागम ढकला_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    पूर्णाङ्क
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, पूर्णाङ्क idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    यदि (isn == रिक्त)
	प्रत्यागम FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    प्रत्यागम ढकला_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    पूर्णाङ्क
generate_STORE_THIS(cctx_T *cctx, पूर्णाङ्क idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    यदि (isn == रिक्त)
	प्रत्यागम FAIL;

    isn->isn_arg.संख्या = idx;
    प्रत्यागम OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    पूर्णाङ्क
may_generate_2STRING(पूर्णाङ्क offset, पूर्णाङ्क tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_चळत(cctx, -1 - offset);

    विकल्प (type->tt_type)
    {
	// nothing to be done
	पक्ष VAR_STRING: प्रत्यागम OK;

	// conversion possible
	पक्ष VAR_SPECIAL:
	पक्ष VAR_BOOL:
	पक्ष VAR_संख्या:
	पक्ष VAR_FLOAT:
			 भङ्ग;

	// conversion possible (with runtime check)
	पक्ष VAR_ANY:
	पक्ष VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 भङ्ग;

	// conversion possible when tolerant
	पक्ष VAR_LIST:
	पक्ष VAR_TUPLE:
	पक्ष VAR_DICT:
			 यदि (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     भङ्ग;
			 }
			 यदि (tostring_flags & TOSTRING_INTERPOLATE)
			     भङ्ग;
			 // FALLTHROUGH

	// conversion not possible
	पक्ष VAR_VOID:
	पक्ष VAR_BLOB:
	पक्ष VAR_FUNC:
	पक्ष VAR_PARTIAL:
	पक्ष VAR_JOB:
	पक्ष VAR_CHANNEL:
	पक्ष VAR_INSTR:
	पक्ष VAR_CLASS:
	पक्ष VAR_OBJECT:
	पक्ष VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 प्रत्यागम FAIL;
    }

    set_type_on_चळत(cctx, &t_string, -1 - offset);
    यदि ((isn = generate_instr(cctx, isntype)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    प्रत्यागम OK;
}

    स्थिर पूर्णाङ्क
check_संख्या_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    यदि (!((type1 == VAR_संख्या || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_संख्या || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	यदि (check_type_is_मूल्य(typ1) == FAIL
		|| check_type_is_मूल्य(typ2) == FAIL)
	    प्रत्यागम FAIL;
	यदि (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	अन्यथा
	    semsg(_(e_char_requires_संख्या_or_float_arguments), *op);
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    स्थिर पूर्णाङ्क
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    प्रति (पूर्णाङ्क i = 0; i < tuple_type->tt_argcount; i++)
    {
	यदि (ga_grow(gap, 1) == FAIL)
	    प्रत्यागम FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    प्रत्यागम OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    स्थिर पूर्णाङ्क
set_tuple_union_type_on_चळत(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, परिमाण(type_T *), 10);

    यदि (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    यदि (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    यदि ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_चळत(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    प्रत्यागम OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    पूर्णाङ्क
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  अभिव्यक्ती_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_संख्या ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    यदि (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_संख्या_or_float(type1, type2, (char_u *)"+") == FAIL)
	प्रत्यागम FAIL;

    यदि (isn != रिक्त)
    {
	यदि (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = अभिव्यक्ती_type;
	अन्यथा
	    isn->isn_arg.op.op_type = अभिव्यक्ती_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    यदि (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_चळत(cctx, &t_list_any, 0);
    अन्यथा यदि (vartype == VAR_TUPLE)
    {
	यदि (!check_tuples_addable(type1, type2))
	    प्रत्यागम FAIL;

	यदि (set_tuple_union_type_on_चळत(type1, type2, cctx) == FAIL)
	    प्रत्यागम FAIL;
    }

    प्रत्यागम isn == रिक्त ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    यदि (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_संख्या
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	प्रत्यागम type1->tt_type;
    प्रत्यागम VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    पूर्णाङ्क
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_चळत(cctx, 1);
    type2 = get_type_on_चळत(cctx, 0);
    vartype = operator_type(type1, type2);

    विकल्प (*op)
    {
	पक्ष '+':
		  यदि (generate_add_instr(cctx, vartype, type1, type2,
							    अभिव्यक्ती_COPY) == FAIL)
		      प्रत्यागम FAIL;
		  भङ्ग;

	पक्ष '-':
	पक्ष '*':
	पक्ष '/': यदि (check_संख्या_or_float(type1, type2, op) == FAIL)
		      प्रत्यागम FAIL;
		  यदि (vartype == VAR_संख्या)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  अन्यथा यदि (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  अन्यथा
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  यदि (isn != रिक्त)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? अभिव्यक्ती_MULT : *op == '/'? अभिव्यक्ती_DIV : अभिव्यक्ती_SUB;
		  भङ्ग;

	पक्ष '%': यदि ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_संख्या)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_संख्या))
		  {
		      emsg(_(e_percent_requires_संख्या_arguments));
		      प्रत्यागम FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_संख्या ? ISN_OPNR : ISN_OPANY, 1);
		  यदि (isn != रिक्त)
		      isn->isn_arg.op.op_type = अभिव्यक्ती_REM;
		  भङ्ग;
    }

    // correct type of result
    यदि (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	यदि ((type1->tt_type == VAR_संख्या || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_संख्या || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_चळत(cctx, type, 0);
    }

    प्रत्यागम OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    स्थिर isntype_T
compare_isn_not_मूल्ये(typval_T *tv, type_T *type)
{
    यदि (tv != रिक्त)
	(शून्य)check_typval_is_मूल्य(tv);
    अन्यथा
	(शून्य)check_type_is_मूल्य(type);
    प्रत्यागम ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    स्थिर isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != रिक्त ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != रिक्त ? tv2->v_type : type2->tt_type;

    यदि (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	प्रत्यागम compare_isn_not_मूल्ये(tv1, type1);
    यदि (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	प्रत्यागम compare_isn_not_मूल्ये(tv2, type2);

    यदि (vartype1 == vartype2)
    {
	विकल्प (vartype1)
	{
	    पक्ष VAR_BOOL: isntype = ISN_COMPAREBOOL; भङ्ग;
	    पक्ष VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; भङ्ग;
	    पक्ष VAR_संख्या: isntype = ISN_COMPARENR; भङ्ग;
	    पक्ष VAR_FLOAT: isntype = ISN_COMPAREFLOAT; भङ्ग;
	    पक्ष VAR_STRING: isntype = ISN_COMPARESTRING; भङ्ग;
	    पक्ष VAR_BLOB: isntype = ISN_COMPAREBLOB; भङ्ग;
	    पक्ष VAR_LIST: isntype = ISN_COMPARELIST; भङ्ग;
	    पक्ष VAR_TUPLE: isntype = ISN_COMPARETUPLE; भङ्ग;
	    पक्ष VAR_DICT: isntype = ISN_COMPAREDICT; भङ्ग;
	    पक्ष VAR_FUNC: isntype = ISN_COMPAREFUNC; भङ्ग;
	    पक्ष VAR_OBJECT: isntype = ISN_COMPAREOBJECT; भङ्ग;
	    स्वभाव: isntype = ISN_COMPAREANY; भङ्ग;
	}
    }
    अन्यथा यदि (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_संख्या || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_संख्या || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    अन्यथा यदि (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	यदि ((vartype1 == VAR_SPECIAL
		&& (tv1 != रिक्त ? tv1->vval.v_संख्या == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != रिक्त ? tv2->vval.v_संख्या == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    प्रत्यागम ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    यदि ((exprtype == अभिव्यक्ती_IS || exprtype == अभिव्यक्ती_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == अभिव्यक्ती_IS ? "is" : "isnot" , vartype_name(vartype1));
	प्रत्यागम ISN_DROP;
    }
    यदि (!(exprtype == अभिव्यक्ती_IS || exprtype == अभिव्यक्ती_ISNOT
		|| exprtype == अभिव्यक्ती_EQUAL || exprtype == अभिव्यक्ती_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	प्रत्यागम ISN_DROP;
    }
    यदि (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != अभिव्यक्ती_EQUAL
			&& exprtype != अभिव्यक्ती_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != अभिव्यक्ती_EQUAL
			 && exprtype != अभिव्यक्ती_NEQUAL
			 && exprtype != अभिव्यक्ती_IS
			 && exprtype != अभिव्यक्ती_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	प्रत्यागम ISN_DROP;
    }
    प्रत्यागम isntype;
}

    पूर्णाङ्क
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    यदि (get_compare_isn(type, tv1, tv2, रिक्त, रिक्त) == ISN_DROP)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    पूर्णाङ्क
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, पूर्णाङ्क ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*चळत = &cctx->ctx_type_चळत;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, रिक्त, रिक्त,
		       get_type_on_चळत(cctx, 1), get_type_on_चळत(cctx, 0));
    यदि (isntype == ISN_DROP)
	प्रत्यागम FAIL;

    यदि ((isn = generate_instr(cctx, isntype)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --चळत->ga_len;
    set_type_on_चळत(cctx, &t_bool, 0);

    प्रत्यागम OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    पूर्णाङ्क
generate_CONCAT(cctx_T *cctx, पूर्णाङ्क मोजणी)
{
    isn_T	*isn;
    garray_T	*चळत = &cctx->ctx_type_चळत;

    RETURN_OK_IF_SKIP(cctx);

    यदि ((isn = generate_instr(cctx, ISN_CONCAT)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = मोजणी;

    // drop the argument types
    चळत->ga_len -= मोजणी - 1;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    पूर्णाङ्क
generate_2BOOL(cctx_T *cctx, पूर्णाङ्क invert, पूर्णाङ्क offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_2BOOL)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_चळत(cctx, &t_bool, -1 - offset);

    प्रत्यागम OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    पूर्णाङ्क
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    यदि (generate_instr(cctx, ISN_COND2BOOL) == रिक्त)
	प्रत्यागम FAIL;

    // type becomes bool
    set_type_on_चळत(cctx, &t_bool, 0);

    प्रत्यागम OK;
}

    पूर्णाङ्क
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	पूर्णाङ्क	    typechk_flags,	// type check flags
	पूर्णाङ्क	    offset,
	पूर्णाङ्क	    is_var,
	पूर्णाङ्क	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == रिक्त)
	प्रत्यागम FAIL;
    type_T *tt;
    यदि ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_संख्या_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	यदि (tt != रिक्त)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    यदि (typechk_flags & TYPECHK_संख्या_OK)
		tt->tt_flags |= TTFLAG_संख्या_OK;
	    अन्यथा
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    अन्यथा
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_चळत(cctx, expected, -1 - offset);

    प्रत्यागम OK;
}

    पूर्णाङ्क
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_SETTYPE)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    प्रत्यागम OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    पूर्णाङ्क
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    यदि (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == रिक्त)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    स्थिर पूर्णाङ्क
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == रिक्त ? &t_any : &class->class_type);
    यदि (isn == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.classarg = class;
    यदि (class != रिक्त)
	++class->class_refcount;
    प्रत्यागम OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    पूर्णाङ्क
generate_tv_ढकला(cctx_T *cctx, typval_T *tv)
{
    विकल्प (tv->v_type)
    {
	पक्ष VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_संख्या);
	    भङ्ग;
	पक्ष VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_संख्या);
	    भङ्ग;
	पक्ष VAR_संख्या:
	    generate_PUSHNR(cctx, tv->vval.v_संख्या);
	    भङ्ग;
	पक्ष VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    भङ्ग;
	पक्ष VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = रिक्त;
	    भङ्ग;
	पक्ष VAR_LIST:
	    यदि (tv->vval.v_list != रिक्त)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    भङ्ग;
	पक्ष VAR_TUPLE:
	    यदि (tv->vval.v_tuple != रिक्त)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    भङ्ग;
	पक्ष VAR_DICT:
	    यदि (tv->vval.v_dict != रिक्त)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    भङ्ग;
#ifdef FEAT_JOB_CHANNEL
	पक्ष VAR_JOB:
	    यदि (tv->vval.v_job != रिक्त)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    भङ्ग;
	पक्ष VAR_CHANNEL:
	    यदि (tv->vval.v_channel != रिक्त)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    भङ्ग;
#endif
	पक्ष VAR_FUNC:
	    यदि (tv->vval.v_string != रिक्त)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, रिक्त, &t_func_unknown, TRUE);
	    भङ्ग;
	पक्ष VAR_PARTIAL:
	    यदि (tv->vval.v_partial != रिक्त)
		iemsg("non-null partial constant not supported");
	    यदि (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == रिक्त)
		प्रत्यागम FAIL;
	    भङ्ग;
	पक्ष VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = रिक्त;
	    भङ्ग;
	पक्ष VAR_OBJECT:
	    यदि (tv->vval.v_object != रिक्त)
	    {
		emsg(_(e_cannot_use_non_null_object));
		प्रत्यागम FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    भङ्ग;
	पक्ष VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    भङ्ग;
	स्वभाव:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    प्रत्यागम FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    पूर्णाङ्क
generate_PUSHNR(cctx_T *cctx, varnumber_T संख्या)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_संख्या)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = संख्या;

    यदि (संख्या == 0 || संख्या == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_चळत(cctx, &t_संख्या_bool, 0);
    प्रत्यागम OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    पूर्णाङ्क
generate_PUSHBOOL(cctx_T *cctx, varnumber_T संख्या)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = संख्या;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    पूर्णाङ्क
generate_PUSHSPEC(cctx_T *cctx, varnumber_T संख्या)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     संख्या == VVAL_NULL ? &t_null : &t_none)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = संख्या;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    पूर्णाङ्क
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.fnumber = fnumber;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    पूर्णाङ्क
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    पूर्णाङ्क		ret = OK;

    यदि (cctx->ctx_skip != SKIP_YES)
    {
	यदि ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == रिक्त)
	    ret = FAIL;
	अन्यथा
	{
	    isn->isn_arg.string = str == रिक्त ? रिक्त : *str;
	    प्रत्यागम OK;
	}
    }
    यदि (str != रिक्त)
	VIM_CLEAR(*str);
    प्रत्यागम ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    पूर्णाङ्क
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    यदि (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == रिक्त)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
#अन्यथा
    emsg(_(e_channel_job_feature_not_available));
    प्रत्यागम FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    पूर्णाङ्क
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    यदि (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == रिक्त)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
#अन्यथा
    emsg(_(e_channel_job_feature_not_available));
    प्रत्यागम FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    पूर्णाङ्क
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.blob = blob;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    पूर्णाङ्क
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, पूर्णाङ्क may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == रिक्त)
	प्रत्यागम FAIL;
    यदि (name == रिक्त)
	funcname = रिक्त;
    अन्यथा यदि (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != रिक्त)	    // autoload
	funcname = vim_strsave(name);
    अन्यथा
    {
	funcname = alloc(STRLEN(name) + 3);
	यदि (funcname != रिक्त)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    पूर्णाङ्क
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.string = vim_strsave(name);
    यदि (isn->isn_arg.string == रिक्त)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    पूर्णाङ्क
generate_GETITEM(cctx_T *cctx, पूर्णाङ्क index, पूर्णाङ्क with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_चळत(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    यदि ((isn = generate_instr(cctx, ISN_GETITEM)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    प्रत्यागम ढकला_type_चळत(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    स्थिर शून्य
set_tuple_slice_type_on_चळत(type_T *type, पूर्णाङ्क मोजणी, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    यदि ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= मोजणी)
    {
	set_type_on_चळत(cctx, &t_tuple_any, 0);
	प्रत्यागम;
    }

    ga_init2(&tuple_types_ga, परिमाण(type_T *), 10);

    प्रति (पूर्णाङ्क i = मोजणी; i < type->tt_argcount; i++)
    {
	यदि (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_चळत(cctx, &t_tuple_any, 0);
	    प्रत्यागम;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] =
							     type->tt_args[i];
	tuple_types_ga.ga_len++;
    }

    set_type_on_चळत(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    पूर्णाङ्क
generate_SLICE(cctx_T *cctx, पूर्णाङ्क मोजणी)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_SLICE)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = मोजणी;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_चळत(cctx, 0);
    यदि (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_चळत(type, मोजणी, cctx);

    प्रत्यागम OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    पूर्णाङ्क
generate_CHECKLEN(cctx_T *cctx, पूर्णाङ्क min_len, पूर्णाङ्क more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    यदि ((isn = generate_instr(cctx, ISN_CHECKLEN)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    पूर्णाङ्क
generate_STORE(cctx_T *cctx, isntype_T isn_type, पूर्णाङ्क idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_drop(cctx, isn_type, 1)) == रिक्त)
	प्रत्यागम FAIL;
    यदि (name != रिक्त)
	isn->isn_arg.string = vim_strsave(name);
    अन्यथा
	isn->isn_arg.संख्या = idx;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    पूर्णाङ्क
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	पूर्णाङ्क	    load,
	class_T	    *cl,
	पूर्णाङ्क	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    अन्यथा
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    यदि (isn == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    स्थिर पूर्णाङ्क
generate_STOREOUTER(
	cctx_T	    *cctx,
	पूर्णाङ्क	    idx,
	पूर्णाङ्क	    level,
	पूर्णाङ्क	    loop_depth,
	पूर्णाङ्क	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == रिक्त)
	प्रत्यागम FAIL;
    यदि (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    अन्यथा
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    प्रत्यागम OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    पूर्णाङ्क
generate_STORENR(cctx_T *cctx, पूर्णाङ्क idx, varnumber_T मूल्य)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_STORENR)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = मूल्य;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    स्थिर पूर्णाङ्क
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	पूर्णाङ्क	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_drop(cctx, isn_type, 1)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    पूर्णाङ्क
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	पूर्णाङ्क	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_type2(cctx, isn_type, type, type)) == रिक्त)
	प्रत्यागम FAIL;
    यदि (name != रिक्त)
	isn->isn_arg.string = vim_strsave(name);
    अन्यथा
	isn->isn_arg.संख्या = idx;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    पूर्णाङ्क
generate_LOADOUTER(
	cctx_T	    *cctx,
	पूर्णाङ्क	    idx,
	पूर्णाङ्क	    nesting,
	पूर्णाङ्क	    loop_depth,
	पूर्णाङ्क	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == रिक्त)
	प्रत्यागम FAIL;
    यदि (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    अन्यथा
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    प्रत्यागम OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    पूर्णाङ्क
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    पूर्णाङ्क	    di_flags;
    पूर्णाङ्क	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    यदि (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	प्रत्यागम FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    प्रत्यागम generate_LOAD(cctx, ISN_LOADV, vidx, रिक्त, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    पूर्णाङ्क
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, पूर्णाङ्क forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, isn_type)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    पूर्णाङ्क
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    यदि (generate_instr(cctx, ISN_LOCKCONST) == रिक्त)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    पूर्णाङ्क
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	पूर्णाङ्क	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    अन्यथा
	isn = generate_instr_drop(cctx, isn_type, 1);
    यदि (isn == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    पूर्णाङ्क
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	पूर्णाङ्क	    sid,
	पूर्णाङ्क	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    यदि (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    अन्यथा
	isn = generate_instr_drop(cctx, isn_type, 1);
    यदि (isn == रिक्त)
	प्रत्यागम FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    यदि (sref == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    पूर्णाङ्क
generate_NEWLIST(cctx_T *cctx, पूर्णाङ्क मोजणी, पूर्णाङ्क use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_NEWLIST)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = use_null ? -1 : मोजणी;

    // Get the member type and the declared member type from all the items on
    // the stack.
    यदि ((member_type = get_member_type_from_चळत(मोजणी, 1, cctx)) == रिक्त)
	प्रत्यागम FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_चळत.ga_len -= मोजणी;

    // add the list type to the type stack
    प्रत्यागम ढकला_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    पूर्णाङ्क
generate_NEWTUPLE(cctx_T *cctx, पूर्णाङ्क मोजणी, पूर्णाङ्क use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = use_null ? -1 : मोजणी;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, परिमाण(type_T *), 10);

    यदि (get_tuple_type_from_चळत(मोजणी, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	प्रत्यागम FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_चळत.ga_len -= मोजणी;

    // add the tuple type to the type stack
    प्रत्यागम ढकला_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    पूर्णाङ्क
generate_NEWDICT(cctx_T *cctx, पूर्णाङ्क मोजणी, पूर्णाङ्क use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_NEWDICT)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = use_null ? -1 : मोजणी;

    यदि ((member_type = get_member_type_from_चळत(मोजणी, 2, cctx)) == रिक्त)
	प्रत्यागम FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_चळत.ga_len -= 2 * मोजणी;

    // add the dict type to the type stack
    प्रत्यागम ढकला_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    पूर्णाङ्क
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	पूर्णाङ्क	    object_method,
	पूर्णाङ्क	    fi,
	पूर्णाङ्क	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    पूर्णाङ्क		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_FUNCREF)) == रिक्त)
	प्रत्यागम FAIL;
    यदि (isn_idx != रिक्त)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    यदि (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != रिक्त)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	यदि (extra == रिक्त)
	    प्रत्यागम FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	यदि (cl != रिक्त)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    यदि (ufunc->uf_def_status == UF_NOT_COMPILED || cl != रिक्त)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    यदि (ufunc->uf_def_status != UF_NOT_COMPILED && cl == रिक्त)
    {
	यदि (isn_idx == रिक्त && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (शून्य)compile_def_function(ufunc, FALSE, CT_NONE, रिक्त);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_closure = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    यदि ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == रिक्त ? &t_func_any : ufunc->uf_func_type;
    प्रत्यागम ढकला_type_चळत(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    पूर्णाङ्क
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    पूर्णाङ्क		ret = OK;

    यदि (cctx->ctx_skip != SKIP_YES)
    {
	यदि ((isn = generate_instr(cctx, ISN_NEWFUNC)) == रिक्त)
	    ret = FAIL;
	अन्यथा
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    यदि (arg == रिक्त)
		ret = FAIL;
	    अन्यथा
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(शून्य)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		प्रत्यागम OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    प्रत्यागम ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    पूर्णाङ्क
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_DEF)) == रिक्त)
	प्रत्यागम FAIL;
    यदि (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	यदि (isn->isn_arg.string == रिक्त)
	    प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    पूर्णाङ्क
generate_JUMP(cctx_T *cctx, jumpwhen_T when, पूर्णाङ्क where)
{
    isn_T	*isn;
    garray_T	*चळत = &cctx->ctx_type_चळत;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_JUMP)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    यदि (when != JUMP_ALWAYS && चळत->ga_len > 0)
	--चळत->ga_len;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    पूर्णाङ्क
generate_WHILE(cctx_T *cctx, पूर्णाङ्क funcref_idx)
{
    isn_T	*isn;
    garray_T	*चळत = &cctx->ctx_type_चळत;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_WHILE)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    यदि (चळत->ga_len > 0)
	--चळत->ga_len;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    पूर्णाङ्क
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, पूर्णाङ्क arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, isn_type)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    प्रत्यागम OK;
}

    पूर्णाङ्क
generate_FOR(cctx_T *cctx, पूर्णाङ्क loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_FOR)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    प्रत्यागम ढकला_type_चळत(cctx, &t_any);
}

    पूर्णाङ्क
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_ENDLOOP)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_मोजणी;
    isn->isn_arg.endloop.end_var_मोजणी =
			   cctx->ctx_locals.ga_len - loop_info->li_local_मोजणी;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    पूर्णाङ्क
generate_TRYCONT(cctx_T *cctx, पूर्णाङ्क levels, पूर्णाङ्क where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_TRYCONT)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    प्रत्यागम OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    पूर्णाङ्क
check_internal_func_args(
	cctx_T	*cctx,
	पूर्णाङ्क	func_idx,
	पूर्णाङ्क	argcount,
	पूर्णाङ्क	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*चळत = &cctx->ctx_type_चळत;
    पूर्णाङ्क		argoff = check_internal_func(func_idx, argcount);

    यदि (argoff < 0)
	प्रत्यागम FAIL;

    यदि (method_call && argoff > 1)
    {
	यदि (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    प्रत्यागम FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	यदि (isn  == रिक्त)
	    प्रत्यागम FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    यदि (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)चळत->ga_data) + चळत->ga_len - argcount;

	// Check the types of the arguments.
	यदि (method_call && argoff > 1)
	{
	    पूर्णाङ्क i;

	    प्रति (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	अन्यथा
	{
	    पूर्णाङ्क i;

	    प्रति (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	यदि (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    पूर्णाङ्क
generate_BCALL(cctx_T *cctx, पूर्णाङ्क func_idx, पूर्णाङ्क argcount, पूर्णाङ्क method_call)
{
    isn_T	*isn;
    garray_T	*चळत = &cctx->ctx_type_चळत;
    type2_T	*argtypes = रिक्त;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = रिक्त;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    यदि (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	प्रत्यागम FAIL;

    यदि (internal_func_is_map(func_idx))
	maptype = argtypes;

    यदि ((isn = generate_instr(cctx, ISN_BCALL)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    चळत->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    यदि (ढकला_type_stack2(cctx, type, decl_type) == FAIL)
	प्रत्यागम FAIL;

    यदि (maptype != रिक्त && maptype[0].type_decl->tt_member != रिक्त
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    प्रत्यागम OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    पूर्णाङ्क
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_चळत(cctx, 1);
    यदि (arg_type_modifiable(list_type, 1) == FAIL)
	प्रत्यागम FAIL;
    item_type = get_type_on_चळत(cctx, 0);
    expected = list_type->tt_member;
    यदि (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	प्रत्यागम FAIL;

    यदि (generate_instr(cctx, ISN_LISTAPPEND) == रिक्त)
	प्रत्यागम FAIL;

    --cctx->ctx_type_चळत.ga_len;	    // drop the argument
    प्रत्यागम OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    पूर्णाङ्क
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    यदि (arg_type_modifiable(get_decl_type_on_चळत(cctx, 1), 1) == FAIL)
	प्रत्यागम FAIL;
    item_type = get_type_on_चळत(cctx, 0);
    यदि (need_type(item_type, &t_संख्या, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	प्रत्यागम FAIL;

    यदि (generate_instr(cctx, ISN_BLOBAPPEND) == रिक्त)
	प्रत्यागम FAIL;

    --cctx->ctx_type_चळत.ga_len;	    // drop the argument
    प्रत्यागम OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    स्थिर isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    यदि (cl != रिक्त)
	प्रत्यागम ISN_METHODCALL;

    यदि (ufunc->uf_def_status == UF_NOT_COMPILED)
	प्रत्यागम ISN_UCALL;

    // function invoked from a lambda
    यदि (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	यदि (ufunc->uf_name[0] == K_SPECIAL)
	    प्रत्यागम ISN_UCALL;
    }

    प्रत्यागम ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    पूर्णाङ्क
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	पूर्णाङ्क	    mi,
	पूर्णाङ्क	    pushed_argcount,
	पूर्णाङ्क	    is_super)
{
    isn_T	*isn;
    पूर्णाङ्क		regular_args = ufunc->uf_args.ga_len;
    पूर्णाङ्क		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    यदि (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	प्रत्यागम FAIL;
    }
    यदि (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	प्रत्यागम FAIL;
    }

    यदि (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	पूर्णाङ्क		i;
	compiletype_T	compile_type;

	प्रति (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_चळत(cctx, argcount - i - 1);
	    यदि (check_type_is_मूल्य(actual) == FAIL)
		प्रत्यागम FAIL;
	    यदि (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		अनुवर्त;
	    }
	    यदि (i < regular_args)
	    {
		यदि (ufunc->uf_arg_types == रिक्त)
		    अनुवर्त;
		expected = ufunc->uf_arg_types[i];
	    }
	    अन्यथा यदि (ufunc->uf_va_type == रिक्त
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    अन्यथा
		expected = ufunc->uf_va_type->tt_member;
	    यदि (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		प्रत्यागम FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	यदि (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == रिक्त,
						   compile_type, रिक्त) == FAIL)
	    प्रत्यागम FAIL;
    }
    यदि (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	प्रत्यागम FAIL;
    }

    यदि ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == रिक्त)
	प्रत्यागम FAIL;
    यदि (cl != रिक्त /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	यदि (isn->isn_arg.mfunc == रिक्त)
	    प्रत्यागम FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = रिक्त;
	यदि (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = रिक्त;
	isn->isn_arg.mfunc->cmf_cache_generic = रिक्त;
	isn->isn_arg.mfunc->cmf_cache_ufunc = रिक्त;
    }
    अन्यथा यदि (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    अन्यथा
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_चळत.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    यदि (ufunc->uf_class != रिक्त)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_चळत(cctx, 0);
	यदि (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_चळत.ga_len--;
    }

    // add return type
    प्रत्यागम ढकला_type_चळत(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    पूर्णाङ्क
generate_UCALL(cctx_T *cctx, char_u *name, पूर्णाङ्क argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_UCALL)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_चळत.ga_len -= argcount;

    // add return value
    प्रत्यागम ढकला_type_चळत(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    पूर्णाङ्क
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	पूर्णाङ्क	argcount,
	पूर्णाङ्क	at_शीर्ष,
	char_u	*name)
{
    यदि (type->tt_argcount != -1)
    {
	पूर्णाङ्क	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	यदि (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    प्रत्यागम FAIL;
	}
	यदि (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    प्रत्यागम FAIL;
	}
	यदि (type->tt_args != रिक्त)
	{
	    पूर्णाङ्क i;

	    प्रति (i = 0; i < argcount; ++i)
	    {
		पूर्णाङ्क	offset = -argcount + i - (at_शीर्ष ? 0 : 1);
		type_T	*actual = get_type_on_चळत(cctx, -1 - offset);
		type_T	*expected;

		यदि (check_type_is_मूल्य(actual) == FAIL)
		    प्रत्यागम FAIL;
		यदि (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    यदि (expected != रिक्त && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    यदि (expected == रिक्त)
			expected = &t_any;
		}
		अन्यथा यदि (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		अन्यथा
		    expected = type->tt_args[i];
		यदि (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    प्रत्यागम FAIL;
		}
	    }
	}
    }

    प्रत्यागम OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    पूर्णाङ्क
generate_PCALL(
	cctx_T	*cctx,
	पूर्णाङ्क	argcount,
	char_u	*name,
	type_T	*type,
	पूर्णाङ्क	at_शीर्ष)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    यदि (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    अन्यथा यदि (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	यदि (check_func_args_from_type(cctx, type, argcount, at_शीर्ष, name)
								       == FAIL)
	    प्रत्यागम FAIL;

	ret_type = type->tt_member;
	यदि (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    अन्यथा
    {
	semsg(_(e_not_callable_type_str), name);
	प्रत्यागम FAIL;
    }

    यदि ((isn = generate_instr(cctx, ISN_PCALL)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.pfunc.cpf_शीर्ष = at_शीर्ष;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_चळत.ga_len -= argcount + 1;

    // push the return value
    ढकला_type_चळत(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    यदि (at_शीर्ष && generate_instr(cctx, ISN_PCALL_END) == रिक्त)
	प्रत्यागम FAIL;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    पूर्णाङ्क
generate_DEFER(cctx_T *cctx, पूर्णाङ्क var_idx, पूर्णाङ्क argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    पूर्णाङ्क
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_चळत(cctx, 0);
    यदि (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	वर्ण *tofree;

	semsg(_(e_expected_dictionary_for_using_किल्ली_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	प्रत्यागम FAIL;
    }
    // change dict type to dict member type
    यदि (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_चळत(cctx, ntype, 0);
    }

    प्रत्यागम OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    पूर्णाङ्क
generate_ECHO(cctx_T *cctx, पूर्णाङ्क with_white, पूर्णाङ्क मोजणी)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_drop(cctx, ISN_ECHO, मोजणी)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_मोजणी = मोजणी;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    पूर्णाङ्क
generate_MULT_अभिव्यक्ती(cctx_T *cctx, isntype_T isn_type, पूर्णाङ्क मोजणी)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr_drop(cctx, isn_type, मोजणी)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = मोजणी;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    पूर्णाङ्क
generate_ECHOWINDOW(cctx_T *cctx, पूर्णाङ्क मोजणी, long time)
{
    isn_T	*isn;

    यदि ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, मोजणी)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.echowin.ewin_मोजणी = मोजणी;
    isn->isn_arg.echowin.ewin_time = time;
    प्रत्यागम OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    पूर्णाङ्क
generate_SOURCE(cctx_T *cctx, पूर्णाङ्क sid)
{
    isn_T	*isn;

    यदि ((isn = generate_instr(cctx, ISN_SOURCE)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = sid;

    प्रत्यागम OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    पूर्णाङ्क
generate_PUT(cctx_T *cctx, पूर्णाङ्क regname, linenr_T lnum, पूर्णाङ्क fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    यदि (isn == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    प्रत्यागम OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    पूर्णाङ्क
generate_LOCKUNLOCK(cctx_T *cctx, char_u *ओळ, पूर्णाङ्क is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == रिक्त)
	प्रत्यागम FAIL;
    class_T *cl = cctx->ctx_ufunc != रिक्त ? cctx->ctx_ufunc->uf_class : रिक्त;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(ओळ);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    यदि (cl != रिक्त)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    प्रत्यागम OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    पूर्णाङ्क
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *ओळ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, isntype)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.string = vim_strsave(ओळ);
    प्रत्यागम OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    पूर्णाङ्क
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    पूर्णाङ्क		ret = OK;

    यदि (cctx->ctx_skip != SKIP_YES)
    {
	यदि ((isn = generate_instr(cctx, isntype)) == रिक्त)
	    ret = FAIL;
	अन्यथा
	{
	    isn->isn_arg.string = str;
	    प्रत्यागम OK;
	}
    }
    vim_free(str);
    प्रत्यागम ret;
}

    पूर्णाङ्क
generate_LEGACY_EVAL(cctx_T *cctx, char_u *ओळ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.string = vim_strsave(ओळ);

    प्रत्यागम ढकला_type_चळत(cctx, &t_any);
}

    पूर्णाङ्क
generate_EXECCONCAT(cctx_T *cctx, पूर्णाङ्क मोजणी)
{
    isn_T	*isn;

    यदि ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, मोजणी)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.संख्या = मोजणी;
    प्रत्यागम OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    पूर्णाङ्क
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    यदि ((isn = generate_instr(cctx, ISN_RANGE)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.string = range;

    प्रत्यागम ढकला_type_चळत(cctx, &t_संख्या);
}

    पूर्णाङ्क
generate_UNPACK(cctx_T *cctx, पूर्णाङ्क var_मोजणी, पूर्णाङ्क semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_UNPACK)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.unpack.unp_मोजणी = var_मोजणी;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    प्रत्यागम OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    पूर्णाङ्क
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    यदि (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	यदि ((isn = generate_instr(cctx, ISN_CMDMOD)) == रिक्त)
	    प्रत्यागम FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	यदि (isn->isn_arg.cmdmod.cf_cmdmod == रिक्त)
	    प्रत्यागम FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, परिमाण(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = रिक्त;
    }

    प्रत्यागम OK;
}

    पूर्णाङ्क
generate_undo_cmdmods(cctx_T *cctx)
{
    यदि (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == रिक्त)
	प्रत्यागम FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    प्रत्यागम OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    पूर्णाङ्क
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	पूर्णाङ्क		opt_flags,
	पूर्णाङ्क		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    विकल्प (dest)
    {
	पक्ष dest_option:
	    प्रत्यागम generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	पक्ष dest_func_option:
	    प्रत्यागम generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	पक्ष dest_global:
	    // include g: with the name, easier to execute that way
	    प्रत्यागम generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == रिक्त
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	पक्ष dest_buffer:
	    // include b: with the name, easier to execute that way
	    प्रत्यागम generate_STORE(cctx, ISN_STOREB, 0, name);
	पक्ष dest_window:
	    // include w: with the name, easier to execute that way
	    प्रत्यागम generate_STORE(cctx, ISN_STOREW, 0, name);
	पक्ष dest_tab:
	    // include t: with the name, easier to execute that way
	    प्रत्यागम generate_STORE(cctx, ISN_STORET, 0, name);
	पक्ष dest_env:
	    प्रत्यागम generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	पक्ष dest_reg:
	    प्रत्यागम generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], रिक्त);
	पक्ष dest_vimvar:
	    प्रत्यागम generate_STORE(cctx, ISN_STOREV, vimvaridx, रिक्त);
	पक्ष dest_script:
	पक्ष dest_script_v9:
	    {
		पूर्णाङ्क	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		पूर्णाङ्क	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		यदि (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    यदि (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == रिक्त)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			यदि (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    प्रत्यागम FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    प्रत्यागम generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		प्रत्यागम generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	पक्ष dest_class_member:
	    प्रत्यागम generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	पक्ष dest_local:
	पक्ष dest_अभिव्यक्ती:
	    // cannot happen
	    भङ्ग;
    }
    प्रत्यागम FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    पूर्णाङ्क
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    प्रति (;;)
    {
	यदि (scope == रिक्त)
	    भङ्ग;
	यदि (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    प्रत्यागम TRUE;
	scope = scope->se_outer;
    }
    प्रत्यागम FALSE;
}

    पूर्णाङ्क
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, पूर्णाङ्क instr_मोजणी, पूर्णाङ्क is_decl)
{
    यदि (lhs->lhs_dest != dest_local)
	प्रत्यागम generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    यदि (lhs->lhs_lvar == रिक्त)
	प्रत्यागम OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    यदि (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_मोजणी + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.संख्या;
	garray_T    *चळत = &cctx->ctx_type_चळत;

	यदि (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	अन्यथा
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	यदि (चळत->ga_len > 0)
	    --चळत->ga_len;
    }
    अन्यथा यदि (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    अन्यथा
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, रिक्त);
    प्रत्यागम OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    पूर्णाङ्क
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    यदि ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == रिक्त)
	प्रत्यागम FAIL;
    isn->isn_arg.setsctx = new_sctx;
    प्रत्यागम OK;
}

#यदि defined(FEAT_PROFILE)
    शून्य
may_generate_prof_end(cctx_T *cctx, पूर्णाङ्क prof_lnum)
{
    यदि (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_END);
}
#endif


/*
 * Delete an instruction, free what it contains.
 */
    शून्य
delete_instr(isn_T *isn)
{
    विकल्प (isn->isn_type)
    {
	पक्ष ISN_AUTOLOAD:
	पक्ष ISN_DEF:
	पक्ष ISN_EXEC:
	पक्ष ISN_EXECRANGE:
	पक्ष ISN_EXEC_SPLIT:
	पक्ष ISN_LEGACY_EVAL:
	पक्ष ISN_LOADAUTO:
	पक्ष ISN_LOADB:
	पक्ष ISN_LOADENV:
	पक्ष ISN_LOADG:
	पक्ष ISN_LOADOPT:
	पक्ष ISN_LOADT:
	पक्ष ISN_LOADW:
	पक्ष ISN_PUSHEXC:
	पक्ष ISN_PUSHFUNC:
	पक्ष ISN_PUSHS:
	पक्ष ISN_RANGE:
	पक्ष ISN_STOREAUTO:
	पक्ष ISN_STOREB:
	पक्ष ISN_STOREENV:
	पक्ष ISN_STOREG:
	पक्ष ISN_STORET:
	पक्ष ISN_STOREW:
	पक्ष ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    भङ्ग;

	पक्ष ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    भङ्ग;

	पक्ष ISN_SUBSTITUTE:
	    {
		पूर्णाङ्क	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		प्रति (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    भङ्ग;

	पक्ष ISN_INSTR:
	    {
		पूर्णाङ्क	idx;
		isn_T	*list = isn->isn_arg.instr;

		प्रति (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    भङ्ग;

	पक्ष ISN_LOADS:
	पक्ष ISN_LOADEXPORT:
	पक्ष ISN_STORES:
	पक्ष ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    भङ्ग;

	पक्ष ISN_UNLET:
	पक्ष ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    भङ्ग;

	पक्ष ISN_STOREOPT:
	पक्ष ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    भङ्ग;

	पक्ष ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    भङ्ग;

	पक्ष ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    भङ्ग;

	पक्ष ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    भङ्ग;

	पक्ष ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		यदि (extra == रिक्त || extra->fre_func_name == रिक्त)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    यदि (ufunc != रिक्त && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		यदि (extra != रिक्त)
		{
		    char_u *name = extra->fre_func_name;

		    यदि (name != रिक्त)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    यदि (extra->fre_class != रिक्त)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    भङ्ग;

	पक्ष ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		यदि (dfunc->df_ufunc != रिक्त
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    भङ्ग;

	पक्ष ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    भङ्ग;

	पक्ष ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		यदि (arg != रिक्त)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    यदि (ufunc != रिक्त)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    भङ्ग;

	पक्ष ISN_CHECKTYPE:
	पक्ष ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    भङ्ग;

	पक्ष ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    भङ्ग;

	पक्ष ISN_LOADSCRIPT:
	पक्ष ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    भङ्ग;

	पक्ष ISN_LOAD_CLASSMEMBER:
	पक्ष ISN_STORE_CLASSMEMBER:
	पक्ष ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    भङ्ग;

	पक्ष ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    भङ्ग;

	पक्ष ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    भङ्ग;

	पक्ष ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    भङ्ग;

	पक्ष ISN_2BOOL:
	पक्ष ISN_2STRING:
	पक्ष ISN_2STRING_ANY:
	पक्ष ISN_ADDBLOB:
	पक्ष ISN_ADDLIST:
	पक्ष ISN_ADDTUPLE:
	पक्ष ISN_ANYINDEX:
	पक्ष ISN_ANYSLICE:
	पक्ष ISN_BCALL:
	पक्ष ISN_BLOBAPPEND:
	पक्ष ISN_BLOBINDEX:
	पक्ष ISN_BLOBSLICE:
	पक्ष ISN_CATCH:
	पक्ष ISN_CEXPR_AUCMD:
	पक्ष ISN_CHECKLEN:
	पक्ष ISN_CLEARDICT:
	पक्ष ISN_CMDMOD_REV:
	पक्ष ISN_COMPAREANY:
	पक्ष ISN_COMPAREBLOB:
	पक्ष ISN_COMPAREBOOL:
	पक्ष ISN_COMPAREDICT:
	पक्ष ISN_COMPAREFLOAT:
	पक्ष ISN_COMPAREFUNC:
	पक्ष ISN_COMPARELIST:
	पक्ष ISN_COMPARETUPLE:
	पक्ष ISN_COMPARENR:
	पक्ष ISN_COMPARENULL:
	पक्ष ISN_COMPAREOBJECT:
	पक्ष ISN_COMPARESPECIAL:
	पक्ष ISN_COMPARESTRING:
	पक्ष ISN_CONCAT:
	पक्ष ISN_CONSTRUCT:
	पक्ष ISN_COND2BOOL:
	पक्ष ISN_DEBUG:
	पक्ष ISN_DEFER:
	पक्ष ISN_DROP:
	पक्ष ISN_ECHO:
	पक्ष ISN_ECHOCONSOLE:
	पक्ष ISN_ECHOERR:
	पक्ष ISN_ECHOMSG:
	पक्ष ISN_ECHOWINDOW:
	पक्ष ISN_ENDLOOP:
	पक्ष ISN_ENDTRY:
	पक्ष ISN_EXECCONCAT:
	पक्ष ISN_EXECUTE:
	पक्ष ISN_FINALLY:
	पक्ष ISN_FINISH:
	पक्ष ISN_FOR:
	पक्ष ISN_GETITEM:
	पक्ष ISN_GET_OBJ_MEMBER:
	पक्ष ISN_JUMP:
	पक्ष ISN_JUMP_IF_ARG_NOT_SET:
	पक्ष ISN_JUMP_IF_ARG_SET:
	पक्ष ISN_LISTAPPEND:
	पक्ष ISN_LISTINDEX:
	पक्ष ISN_LISTSLICE:
	पक्ष ISN_TUPLEINDEX:
	पक्ष ISN_TUPLESLICE:
	पक्ष ISN_LOAD:
	पक्ष ISN_LOADBDICT:
	पक्ष ISN_LOADGDICT:
	पक्ष ISN_LOADOUTER:
	पक्ष ISN_LOADREG:
	पक्ष ISN_LOADTDICT:
	पक्ष ISN_LOADV:
	पक्ष ISN_LOADWDICT:
	पक्ष ISN_LOCKCONST:
	पक्ष ISN_MEMBER:
	पक्ष ISN_NEGATENR:
	पक्ष ISN_NEWDICT:
	पक्ष ISN_NEWLIST:
	पक्ष ISN_NEWTUPLE:
	पक्ष ISN_NEWPARTIAL:
	पक्ष ISN_OPANY:
	पक्ष ISN_OPFLOAT:
	पक्ष ISN_OPNR:
	पक्ष ISN_PCALL:
	पक्ष ISN_PCALL_END:
	पक्ष ISN_PROF_END:
	पक्ष ISN_PROF_START:
	पक्ष ISN_PUSHBOOL:
	पक्ष ISN_PUSHCHANNEL:
	पक्ष ISN_PUSHF:
	पक्ष ISN_PUSHJOB:
	पक्ष ISN_PUSHNR:
	पक्ष ISN_PUSHOBJ:
	पक्ष ISN_PUSHSPEC:
	पक्ष ISN_PUT:
	पक्ष ISN_IPUT:
	पक्ष ISN_REDIREND:
	पक्ष ISN_REDIRSTART:
	पक्ष ISN_RETURN:
	पक्ष ISN_RETURN_OBJECT:
	पक्ष ISN_RETURN_VOID:
	पक्ष ISN_SHUFFLE:
	पक्ष ISN_SLICE:
	पक्ष ISN_SOURCE:
	पक्ष ISN_STORE:
	पक्ष ISN_STORENR:
	पक्ष ISN_STOREOUTER:
	पक्ष ISN_STORE_THIS:
	पक्ष ISN_STORERANGE:
	पक्ष ISN_STOREREG:
	पक्ष ISN_STOREV:
	पक्ष ISN_STRINDEX:
	पक्ष ISN_STRSLICE:
	पक्ष ISN_THROW:
	पक्ष ISN_TRYCONT:
	पक्ष ISN_UNLETINDEX:
	पक्ष ISN_UNLETRANGE:
	पक्ष ISN_UNPACK:
	पक्ष ISN_USEDICT:
	पक्ष ISN_WHILE:
	पक्ष ISN_SCRIPTCTX_SET:
	// nothing allocated
	भङ्ग;
    }
}

    शून्य
clear_instr_ga(garray_T *gap)
{
    पूर्णाङ्क idx;

    प्रति (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
