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
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) എങ്കിൽ (cctx->ctx_skip == SKIP_YES) തിരികെ ശൂന്യ
#define RETURN_OK_IF_SKIP(cctx) എങ്കിൽ (cctx->ctx_skip == SKIP_YES) തിരികെ OK

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
    എങ്കിൽ (GA_GROW_FAILS(instr, 1))
	തിരികെ ശൂന്യ;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    തിരികെ isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, പൂർണ്ണസംഖ്യ drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_അട്ടി.ga_len -= drop;
    തിരികെ generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    സ്ഥിതം isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    എങ്കിൽ ((isn = generate_instr(cctx, isn_type)) == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (തള്ളുക_type_stack2(cctx, type == ശൂന്യ ? &t_any : type,
			       decl_type == ശൂന്യ ? &t_any : decl_type) == FAIL)
	തിരികെ ശൂന്യ;

    തിരികെ isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    തിരികെ generate_instr_type2(cctx, isn_type, type, &t_any);
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

    എങ്കിൽ ((isn = generate_instr(cctx, ISN_DEBUG)) == ശൂന്യ)
	തിരികെ ശൂന്യ;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    തിരികെ isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    പൂർണ്ണസംഖ്യ
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.construct.construct_size = വലുപ്പം(object_T)
			       + cl->class_obj_member_എണ്ണം * വലുപ്പം(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    തിരികെ OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    പൂർണ്ണസംഖ്യ
generate_GET_OBJ_MEMBER(cctx_T *cctx, പൂർണ്ണസംഖ്യ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ FAIL;

    isn->isn_arg.classmember.cm_class = ശൂന്യ;
    isn->isn_arg.classmember.cm_idx = idx;
    തിരികെ തള്ളുക_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    പൂർണ്ണസംഖ്യ
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, പൂർണ്ണസംഖ്യ idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    തിരികെ തള്ളുക_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    പൂർണ്ണസംഖ്യ
generate_STORE_THIS(cctx_T *cctx, പൂർണ്ണസംഖ്യ idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ FAIL;

    isn->isn_arg.സംഖ്യ = idx;
    തിരികെ OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    പൂർണ്ണസംഖ്യ
may_generate_2STRING(പൂർണ്ണസംഖ്യ offset, പൂർണ്ണസംഖ്യ tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_അട്ടി(cctx, -1 - offset);

    തിരഞ്ഞെടുപ്പ് (type->tt_type)
    {
	// nothing to be done
	അവസ്ഥ VAR_STRING: തിരികെ OK;

	// conversion possible
	അവസ്ഥ VAR_SPECIAL:
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_സംഖ്യ:
	അവസ്ഥ VAR_FLOAT:
			 നിർത്തുക;

	// conversion possible (with runtime check)
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 നിർത്തുക;

	// conversion possible when tolerant
	അവസ്ഥ VAR_LIST:
	അവസ്ഥ VAR_TUPLE:
	അവസ്ഥ VAR_DICT:
			 എങ്കിൽ (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     നിർത്തുക;
			 }
			 എങ്കിൽ (tostring_flags & TOSTRING_INTERPOLATE)
			     നിർത്തുക;
			 // FALLTHROUGH

	// conversion not possible
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_BLOB:
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_PARTIAL:
	അവസ്ഥ VAR_JOB:
	അവസ്ഥ VAR_CHANNEL:
	അവസ്ഥ VAR_INSTR:
	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_OBJECT:
	അവസ്ഥ VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 തിരികെ FAIL;
    }

    set_type_on_അട്ടി(cctx, &t_string, -1 - offset);
    എങ്കിൽ ((isn = generate_instr(cctx, isntype)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    തിരികെ OK;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_സംഖ്യ_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    എങ്കിൽ (!((type1 == VAR_സംഖ്യ || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_സംഖ്യ || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	എങ്കിൽ (check_type_is_മൂല്യം(typ1) == FAIL
		|| check_type_is_മൂല്യം(typ2) == FAIL)
	    തിരികെ FAIL;
	എങ്കിൽ (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	അല്ലെങ്കിൽ
	    semsg(_(e_char_requires_സംഖ്യ_or_float_arguments), *op);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < tuple_type->tt_argcount; i++)
    {
	എങ്കിൽ (ga_grow(gap, 1) == FAIL)
	    തിരികെ FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    തിരികെ OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
set_tuple_union_type_on_അട്ടി(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, വലുപ്പം(type_T *), 10);

    എങ്കിൽ (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    എങ്കിൽ (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    എങ്കിൽ ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_അട്ടി(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    തിരികെ OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    പൂർണ്ണസംഖ്യ
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  വ്യഞ്ജകം_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_സംഖ്യ ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    എങ്കിൽ (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_സംഖ്യ_or_float(type1, type2, (char_u *)"+") == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (isn != ശൂന്യ)
    {
	എങ്കിൽ (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = വ്യഞ്ജകം_type;
	അല്ലെങ്കിൽ
	    isn->isn_arg.op.op_type = വ്യഞ്ജകം_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    എങ്കിൽ (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_അട്ടി(cctx, &t_list_any, 0);
    അല്ലെങ്കിൽ എങ്കിൽ (vartype == VAR_TUPLE)
    {
	എങ്കിൽ (!check_tuples_addable(type1, type2))
	    തിരികെ FAIL;

	എങ്കിൽ (set_tuple_union_type_on_അട്ടി(type1, type2, cctx) == FAIL)
	    തിരികെ FAIL;
    }

    തിരികെ isn == ശൂന്യ ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    എങ്കിൽ (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_സംഖ്യ
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	തിരികെ type1->tt_type;
    തിരികെ VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    പൂർണ്ണസംഖ്യ
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_അട്ടി(cctx, 1);
    type2 = get_type_on_അട്ടി(cctx, 0);
    vartype = operator_type(type1, type2);

    തിരഞ്ഞെടുപ്പ് (*op)
    {
	അവസ്ഥ '+':
		  എങ്കിൽ (generate_add_instr(cctx, vartype, type1, type2,
							    വ്യഞ്ജകം_COPY) == FAIL)
		      തിരികെ FAIL;
		  നിർത്തുക;

	അവസ്ഥ '-':
	അവസ്ഥ '*':
	അവസ്ഥ '/': എങ്കിൽ (check_സംഖ്യ_or_float(type1, type2, op) == FAIL)
		      തിരികെ FAIL;
		  എങ്കിൽ (vartype == VAR_സംഖ്യ)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  അല്ലെങ്കിൽ എങ്കിൽ (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  അല്ലെങ്കിൽ
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  എങ്കിൽ (isn != ശൂന്യ)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? വ്യഞ്ജകം_MULT : *op == '/'? വ്യഞ്ജകം_DIV : വ്യഞ്ജകം_SUB;
		  നിർത്തുക;

	അവസ്ഥ '%': എങ്കിൽ ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_സംഖ്യ)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_സംഖ്യ))
		  {
		      emsg(_(e_percent_requires_സംഖ്യ_arguments));
		      തിരികെ FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_സംഖ്യ ? ISN_OPNR : ISN_OPANY, 1);
		  എങ്കിൽ (isn != ശൂന്യ)
		      isn->isn_arg.op.op_type = വ്യഞ്ജകം_REM;
		  നിർത്തുക;
    }

    // correct type of result
    എങ്കിൽ (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	എങ്കിൽ ((type1->tt_type == VAR_സംഖ്യ || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_സംഖ്യ || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_അട്ടി(cctx, type, 0);
    }

    തിരികെ OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    സ്ഥിതം isntype_T
compare_isn_not_മൂല്യങ്ങൾ(typval_T *tv, type_T *type)
{
    എങ്കിൽ (tv != ശൂന്യ)
	(ശൂന്യം)check_typval_is_മൂല്യം(tv);
    അല്ലെങ്കിൽ
	(ശൂന്യം)check_type_is_മൂല്യം(type);
    തിരികെ ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    സ്ഥിതം isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != ശൂന്യ ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != ശൂന്യ ? tv2->v_type : type2->tt_type;

    എങ്കിൽ (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	തിരികെ compare_isn_not_മൂല്യങ്ങൾ(tv1, type1);
    എങ്കിൽ (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	തിരികെ compare_isn_not_മൂല്യങ്ങൾ(tv2, type2);

    എങ്കിൽ (vartype1 == vartype2)
    {
	തിരഞ്ഞെടുപ്പ് (vartype1)
	{
	    അവസ്ഥ VAR_BOOL: isntype = ISN_COMPAREBOOL; നിർത്തുക;
	    അവസ്ഥ VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; നിർത്തുക;
	    അവസ്ഥ VAR_സംഖ്യ: isntype = ISN_COMPARENR; നിർത്തുക;
	    അവസ്ഥ VAR_FLOAT: isntype = ISN_COMPAREFLOAT; നിർത്തുക;
	    അവസ്ഥ VAR_STRING: isntype = ISN_COMPARESTRING; നിർത്തുക;
	    അവസ്ഥ VAR_BLOB: isntype = ISN_COMPAREBLOB; നിർത്തുക;
	    അവസ്ഥ VAR_LIST: isntype = ISN_COMPARELIST; നിർത്തുക;
	    അവസ്ഥ VAR_TUPLE: isntype = ISN_COMPARETUPLE; നിർത്തുക;
	    അവസ്ഥ VAR_DICT: isntype = ISN_COMPAREDICT; നിർത്തുക;
	    അവസ്ഥ VAR_FUNC: isntype = ISN_COMPAREFUNC; നിർത്തുക;
	    അവസ്ഥ VAR_OBJECT: isntype = ISN_COMPAREOBJECT; നിർത്തുക;
	    സ്ഥിരസ്ഥിതി: isntype = ISN_COMPAREANY; നിർത്തുക;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_സംഖ്യ || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_സംഖ്യ || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    അല്ലെങ്കിൽ എങ്കിൽ (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	എങ്കിൽ ((vartype1 == VAR_SPECIAL
		&& (tv1 != ശൂന്യ ? tv1->vval.v_സംഖ്യ == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != ശൂന്യ ? tv2->vval.v_സംഖ്യ == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    തിരികെ ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    എങ്കിൽ ((exprtype == വ്യഞ്ജകം_IS || exprtype == വ്യഞ്ജകം_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == വ്യഞ്ജകം_IS ? "is" : "isnot" , vartype_name(vartype1));
	തിരികെ ISN_DROP;
    }
    എങ്കിൽ (!(exprtype == വ്യഞ്ജകം_IS || exprtype == വ്യഞ്ജകം_ISNOT
		|| exprtype == വ്യഞ്ജകം_EQUAL || exprtype == വ്യഞ്ജകം_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	തിരികെ ISN_DROP;
    }
    എങ്കിൽ (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != വ്യഞ്ജകം_EQUAL
			&& exprtype != വ്യഞ്ജകം_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != വ്യഞ്ജകം_EQUAL
			 && exprtype != വ്യഞ്ജകം_NEQUAL
			 && exprtype != വ്യഞ്ജകം_IS
			 && exprtype != വ്യഞ്ജകം_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	തിരികെ ISN_DROP;
    }
    തിരികെ isntype;
}

    പൂർണ്ണസംഖ്യ
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    എങ്കിൽ (get_compare_isn(type, tv1, tv2, ശൂന്യ, ശൂന്യ) == ISN_DROP)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    പൂർണ്ണസംഖ്യ
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, പൂർണ്ണസംഖ്യ ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, ശൂന്യ, ശൂന്യ,
		       get_type_on_അട്ടി(cctx, 1), get_type_on_അട്ടി(cctx, 0));
    എങ്കിൽ (isntype == ISN_DROP)
	തിരികെ FAIL;

    എങ്കിൽ ((isn = generate_instr(cctx, isntype)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --അട്ടി->ga_len;
    set_type_on_അട്ടി(cctx, &t_bool, 0);

    തിരികെ OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    പൂർണ്ണസംഖ്യ
generate_CONCAT(cctx_T *cctx, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    isn_T	*isn;
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;

    RETURN_OK_IF_SKIP(cctx);

    എങ്കിൽ ((isn = generate_instr(cctx, ISN_CONCAT)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = എണ്ണം;

    // drop the argument types
    അട്ടി->ga_len -= എണ്ണം - 1;

    തിരികെ OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    പൂർണ്ണസംഖ്യ
generate_2BOOL(cctx_T *cctx, പൂർണ്ണസംഖ്യ invert, പൂർണ്ണസംഖ്യ offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_2BOOL)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_അട്ടി(cctx, &t_bool, -1 - offset);

    തിരികെ OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ (generate_instr(cctx, ISN_COND2BOOL) == ശൂന്യ)
	തിരികെ FAIL;

    // type becomes bool
    set_type_on_അട്ടി(cctx, &t_bool, 0);

    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	പൂർണ്ണസംഖ്യ	    typechk_flags,	// type check flags
	പൂർണ്ണസംഖ്യ	    offset,
	പൂർണ്ണസംഖ്യ	    is_var,
	പൂർണ്ണസംഖ്യ	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == ശൂന്യ)
	തിരികെ FAIL;
    type_T *tt;
    എങ്കിൽ ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_സംഖ്യ_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	എങ്കിൽ (tt != ശൂന്യ)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    എങ്കിൽ (typechk_flags & TYPECHK_സംഖ്യ_OK)
		tt->tt_flags |= TTFLAG_സംഖ്യ_OK;
	    അല്ലെങ്കിൽ
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    അല്ലെങ്കിൽ
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_അട്ടി(cctx, expected, -1 - offset);

    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_SETTYPE)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    തിരികെ OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == ശൂന്യ ? &t_any : &class->class_type);
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.classarg = class;
    എങ്കിൽ (class != ശൂന്യ)
	++class->class_refcount;
    തിരികെ OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    പൂർണ്ണസംഖ്യ
generate_tv_തള്ളുക(cctx_T *cctx, typval_T *tv)
{
    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_സംഖ്യ);
	    നിർത്തുക;
	അവസ്ഥ VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_സംഖ്യ);
	    നിർത്തുക;
	അവസ്ഥ VAR_സംഖ്യ:
	    generate_PUSHNR(cctx, tv->vval.v_സംഖ്യ);
	    നിർത്തുക;
	അവസ്ഥ VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    നിർത്തുക;
	അവസ്ഥ VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_LIST:
	    എങ്കിൽ (tv->vval.v_list != ശൂന്യ)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    എങ്കിൽ (tv->vval.v_tuple != ശൂന്യ)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    എങ്കിൽ (tv->vval.v_dict != ശൂന്യ)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    നിർത്തുക;
#ifdef FEAT_JOB_CHANNEL
	അവസ്ഥ VAR_JOB:
	    എങ്കിൽ (tv->vval.v_job != ശൂന്യ)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    നിർത്തുക;
	അവസ്ഥ VAR_CHANNEL:
	    എങ്കിൽ (tv->vval.v_channel != ശൂന്യ)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_FUNC:
	    എങ്കിൽ (tv->vval.v_string != ശൂന്യ)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, ശൂന്യ, &t_func_unknown, TRUE);
	    നിർത്തുക;
	അവസ്ഥ VAR_PARTIAL:
	    എങ്കിൽ (tv->vval.v_partial != ശൂന്യ)
		iemsg("non-null partial constant not supported");
	    എങ്കിൽ (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == ശൂന്യ)
		തിരികെ FAIL;
	    നിർത്തുക;
	അവസ്ഥ VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = ശൂന്യ;
	    നിർത്തുക;
	അവസ്ഥ VAR_OBJECT:
	    എങ്കിൽ (tv->vval.v_object != ശൂന്യ)
	    {
		emsg(_(e_cannot_use_non_null_object));
		തിരികെ FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    നിർത്തുക;
	അവസ്ഥ VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    തിരികെ FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    തിരികെ OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHNR(cctx_T *cctx, varnumber_T സംഖ്യ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_സംഖ്യ)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = സംഖ്യ;

    എങ്കിൽ (സംഖ്യ == 0 || സംഖ്യ == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_അട്ടി(cctx, &t_സംഖ്യ_bool, 0);
    തിരികെ OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHBOOL(cctx_T *cctx, varnumber_T സംഖ്യ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = സംഖ്യ;

    തിരികെ OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHSPEC(cctx_T *cctx, varnumber_T സംഖ്യ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     സംഖ്യ == VVAL_NULL ? &t_null : &t_none)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = സംഖ്യ;

    തിരികെ OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.fnumber = fnumber;

    തിരികെ OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    പൂർണ്ണസംഖ്യ		ret = OK;

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	എങ്കിൽ ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == ശൂന്യ)
	    ret = FAIL;
	അല്ലെങ്കിൽ
	{
	    isn->isn_arg.string = str == ശൂന്യ ? ശൂന്യ : *str;
	    തിരികെ OK;
	}
    }
    എങ്കിൽ (str != ശൂന്യ)
	VIM_CLEAR(*str);
    തിരികെ ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    എങ്കിൽ (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
#അല്ലെങ്കിൽ
    emsg(_(e_channel_job_feature_not_available));
    തിരികെ FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    എങ്കിൽ (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
#അല്ലെങ്കിൽ
    emsg(_(e_channel_job_feature_not_available));
    തിരികെ FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.blob = blob;

    തിരികെ OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    പൂർണ്ണസംഖ്യ
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, പൂർണ്ണസംഖ്യ may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (name == ശൂന്യ)
	funcname = ശൂന്യ;
    അല്ലെങ്കിൽ എങ്കിൽ (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != ശൂന്യ)	    // autoload
	funcname = vim_strsave(name);
    അല്ലെങ്കിൽ
    {
	funcname = alloc(STRLEN(name) + 3);
	എങ്കിൽ (funcname != ശൂന്യ)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    തിരികെ OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.string = vim_strsave(name);
    എങ്കിൽ (isn->isn_arg.string == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    പൂർണ്ണസംഖ്യ
generate_GETITEM(cctx_T *cctx, പൂർണ്ണസംഖ്യ index, പൂർണ്ണസംഖ്യ with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_അട്ടി(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_GETITEM)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    തിരികെ തള്ളുക_type_അട്ടി(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    സ്ഥിതം ശൂന്യം
set_tuple_slice_type_on_അട്ടി(type_T *type, പൂർണ്ണസംഖ്യ എണ്ണം, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    എങ്കിൽ ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= എണ്ണം)
    {
	set_type_on_അട്ടി(cctx, &t_tuple_any, 0);
	തിരികെ;
    }

    ga_init2(&tuple_types_ga, വലുപ്പം(type_T *), 10);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = എണ്ണം; i < type->tt_argcount; i++)
    {
	എങ്കിൽ (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_അട്ടി(cctx, &t_tuple_any, 0);
	    തിരികെ;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] =
							     type->tt_args[i];
	tuple_types_ga.ga_len++;
    }

    set_type_on_അട്ടി(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    പൂർണ്ണസംഖ്യ
generate_SLICE(cctx_T *cctx, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_SLICE)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = എണ്ണം;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_അട്ടി(cctx, 0);
    എങ്കിൽ (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_അട്ടി(type, എണ്ണം, cctx);

    തിരികെ OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    പൂർണ്ണസംഖ്യ
generate_CHECKLEN(cctx_T *cctx, പൂർണ്ണസംഖ്യ min_len, പൂർണ്ണസംഖ്യ more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    എങ്കിൽ ((isn = generate_instr(cctx, ISN_CHECKLEN)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    തിരികെ OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_STORE(cctx_T *cctx, isntype_T isn_type, പൂർണ്ണസംഖ്യ idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (name != ശൂന്യ)
	isn->isn_arg.string = vim_strsave(name);
    അല്ലെങ്കിൽ
	isn->isn_arg.സംഖ്യ = idx;

    തിരികെ OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	പൂർണ്ണസംഖ്യ	    load,
	class_T	    *cl,
	പൂർണ്ണസംഖ്യ	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    അല്ലെങ്കിൽ
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    തിരികെ OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
generate_STOREOUTER(
	cctx_T	    *cctx,
	പൂർണ്ണസംഖ്യ	    idx,
	പൂർണ്ണസംഖ്യ	    level,
	പൂർണ്ണസംഖ്യ	    loop_depth,
	പൂർണ്ണസംഖ്യ	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    അല്ലെങ്കിൽ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    തിരികെ OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    പൂർണ്ണസംഖ്യ
generate_STORENR(cctx_T *cctx, പൂർണ്ണസംഖ്യ idx, varnumber_T മൂല്യം)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_STORENR)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = മൂല്യം;

    തിരികെ OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_drop(cctx, isn_type, 1)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    തിരികെ OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	പൂർണ്ണസംഖ്യ	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_type2(cctx, isn_type, type, type)) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (name != ശൂന്യ)
	isn->isn_arg.string = vim_strsave(name);
    അല്ലെങ്കിൽ
	isn->isn_arg.സംഖ്യ = idx;

    തിരികെ OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    പൂർണ്ണസംഖ്യ
generate_LOADOUTER(
	cctx_T	    *cctx,
	പൂർണ്ണസംഖ്യ	    idx,
	പൂർണ്ണസംഖ്യ	    nesting,
	പൂർണ്ണസംഖ്യ	    loop_depth,
	പൂർണ്ണസംഖ്യ	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    അല്ലെങ്കിൽ
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    തിരികെ OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    പൂർണ്ണസംഖ്യ
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    പൂർണ്ണസംഖ്യ	    di_flags;
    പൂർണ്ണസംഖ്യ	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	തിരികെ FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    തിരികെ generate_LOAD(cctx, ISN_LOADV, vidx, ശൂന്യ, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, പൂർണ്ണസംഖ്യ forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, isn_type)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    തിരികെ OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ (generate_instr(cctx, ISN_LOCKCONST) == ശൂന്യ)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    അല്ലെങ്കിൽ
	isn = generate_instr_drop(cctx, isn_type, 1);
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    തിരികെ OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	പൂർണ്ണസംഖ്യ	    sid,
	പൂർണ്ണസംഖ്യ	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    അല്ലെങ്കിൽ
	isn = generate_instr_drop(cctx, isn_type, 1);
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    എങ്കിൽ (sref == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    തിരികെ OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    പൂർണ്ണസംഖ്യ
generate_NEWLIST(cctx_T *cctx, പൂർണ്ണസംഖ്യ എണ്ണം, പൂർണ്ണസംഖ്യ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_NEWLIST)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = use_null ? -1 : എണ്ണം;

    // Get the member type and the declared member type from all the items on
    // the stack.
    എങ്കിൽ ((member_type = get_member_type_from_അട്ടി(എണ്ണം, 1, cctx)) == ശൂന്യ)
	തിരികെ FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_അട്ടി.ga_len -= എണ്ണം;

    // add the list type to the type stack
    തിരികെ തള്ളുക_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    പൂർണ്ണസംഖ്യ
generate_NEWTUPLE(cctx_T *cctx, പൂർണ്ണസംഖ്യ എണ്ണം, പൂർണ്ണസംഖ്യ use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = use_null ? -1 : എണ്ണം;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, വലുപ്പം(type_T *), 10);

    എങ്കിൽ (get_tuple_type_from_അട്ടി(എണ്ണം, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	തിരികെ FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_അട്ടി.ga_len -= എണ്ണം;

    // add the tuple type to the type stack
    തിരികെ തള്ളുക_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    പൂർണ്ണസംഖ്യ
generate_NEWDICT(cctx_T *cctx, പൂർണ്ണസംഖ്യ എണ്ണം, പൂർണ്ണസംഖ്യ use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_NEWDICT)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = use_null ? -1 : എണ്ണം;

    എങ്കിൽ ((member_type = get_member_type_from_അട്ടി(എണ്ണം, 2, cctx)) == ശൂന്യ)
	തിരികെ FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_അട്ടി.ga_len -= 2 * എണ്ണം;

    // add the dict type to the type stack
    തിരികെ തള്ളുക_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    പൂർണ്ണസംഖ്യ
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	പൂർണ്ണസംഖ്യ	    object_method,
	പൂർണ്ണസംഖ്യ	    fi,
	പൂർണ്ണസംഖ്യ	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    പൂർണ്ണസംഖ്യ		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_FUNCREF)) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (isn_idx != ശൂന്യ)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    എങ്കിൽ (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != ശൂന്യ)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	എങ്കിൽ (extra == ശൂന്യ)
	    തിരികെ FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	എങ്കിൽ (cl != ശൂന്യ)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    എങ്കിൽ (ufunc->uf_def_status == UF_NOT_COMPILED || cl != ശൂന്യ)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    എങ്കിൽ (ufunc->uf_def_status != UF_NOT_COMPILED && cl == ശൂന്യ)
    {
	എങ്കിൽ (isn_idx == ശൂന്യ && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (ശൂന്യം)compile_def_function(ufunc, FALSE, CT_NONE, ശൂന്യ);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_closure = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    എങ്കിൽ ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == ശൂന്യ ? &t_func_any : ufunc->uf_func_type;
    തിരികെ തള്ളുക_type_അട്ടി(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    പൂർണ്ണസംഖ്യ
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    പൂർണ്ണസംഖ്യ		ret = OK;

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	എങ്കിൽ ((isn = generate_instr(cctx, ISN_NEWFUNC)) == ശൂന്യ)
	    ret = FAIL;
	അല്ലെങ്കിൽ
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    എങ്കിൽ (arg == ശൂന്യ)
		ret = FAIL;
	    അല്ലെങ്കിൽ
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(ശൂന്യം)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		തിരികെ OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    തിരികെ ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    പൂർണ്ണസംഖ്യ
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_DEF)) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	എങ്കിൽ (isn->isn_arg.string == ശൂന്യ)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_JUMP(cctx_T *cctx, jumpwhen_T when, പൂർണ്ണസംഖ്യ where)
{
    isn_T	*isn;
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_JUMP)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    എങ്കിൽ (when != JUMP_ALWAYS && അട്ടി->ga_len > 0)
	--അട്ടി->ga_len;

    തിരികെ OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    പൂർണ്ണസംഖ്യ
generate_WHILE(cctx_T *cctx, പൂർണ്ണസംഖ്യ funcref_idx)
{
    isn_T	*isn;
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_WHILE)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    എങ്കിൽ (അട്ടി->ga_len > 0)
	--അട്ടി->ga_len;

    തിരികെ OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, പൂർണ്ണസംഖ്യ arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, isn_type)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
generate_FOR(cctx_T *cctx, പൂർണ്ണസംഖ്യ loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_FOR)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    തിരികെ തള്ളുക_type_അട്ടി(cctx, &t_any);
}

    പൂർണ്ണസംഖ്യ
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_ENDLOOP)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_എണ്ണം;
    isn->isn_arg.endloop.end_var_എണ്ണം =
			   cctx->ctx_locals.ga_len - loop_info->li_local_എണ്ണം;
    തിരികെ OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_TRYCONT(cctx_T *cctx, പൂർണ്ണസംഖ്യ levels, പൂർണ്ണസംഖ്യ where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_TRYCONT)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    തിരികെ OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    പൂർണ്ണസംഖ്യ
check_internal_func_args(
	cctx_T	*cctx,
	പൂർണ്ണസംഖ്യ	func_idx,
	പൂർണ്ണസംഖ്യ	argcount,
	പൂർണ്ണസംഖ്യ	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;
    പൂർണ്ണസംഖ്യ		argoff = check_internal_func(func_idx, argcount);

    എങ്കിൽ (argoff < 0)
	തിരികെ FAIL;

    എങ്കിൽ (method_call && argoff > 1)
    {
	എങ്കിൽ (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    തിരികെ FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	എങ്കിൽ (isn  == ശൂന്യ)
	    തിരികെ FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    എങ്കിൽ (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)അട്ടി->ga_data) + അട്ടി->ga_len - argcount;

	// Check the types of the arguments.
	എങ്കിൽ (method_call && argoff > 1)
	{
	    പൂർണ്ണസംഖ്യ i;

	    വേണ്ടി (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ i;

	    വേണ്ടി (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	എങ്കിൽ (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    പൂർണ്ണസംഖ്യ
generate_BCALL(cctx_T *cctx, പൂർണ്ണസംഖ്യ func_idx, പൂർണ്ണസംഖ്യ argcount, പൂർണ്ണസംഖ്യ method_call)
{
    isn_T	*isn;
    garray_T	*അട്ടി = &cctx->ctx_type_അട്ടി;
    type2_T	*argtypes = ശൂന്യ;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = ശൂന്യ;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    എങ്കിൽ (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (internal_func_is_map(func_idx))
	maptype = argtypes;

    എങ്കിൽ ((isn = generate_instr(cctx, ISN_BCALL)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    അട്ടി->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    എങ്കിൽ (തള്ളുക_type_stack2(cctx, type, decl_type) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (maptype != ശൂന്യ && maptype[0].type_decl->tt_member != ശൂന്യ
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    തിരികെ OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    പൂർണ്ണസംഖ്യ
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_അട്ടി(cctx, 1);
    എങ്കിൽ (arg_type_modifiable(list_type, 1) == FAIL)
	തിരികെ FAIL;
    item_type = get_type_on_അട്ടി(cctx, 0);
    expected = list_type->tt_member;
    എങ്കിൽ (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (generate_instr(cctx, ISN_LISTAPPEND) == ശൂന്യ)
	തിരികെ FAIL;

    --cctx->ctx_type_അട്ടി.ga_len;	    // drop the argument
    തിരികെ OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    പൂർണ്ണസംഖ്യ
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    എങ്കിൽ (arg_type_modifiable(get_decl_type_on_അട്ടി(cctx, 1), 1) == FAIL)
	തിരികെ FAIL;
    item_type = get_type_on_അട്ടി(cctx, 0);
    എങ്കിൽ (need_type(item_type, &t_സംഖ്യ, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	തിരികെ FAIL;

    എങ്കിൽ (generate_instr(cctx, ISN_BLOBAPPEND) == ശൂന്യ)
	തിരികെ FAIL;

    --cctx->ctx_type_അട്ടി.ga_len;	    // drop the argument
    തിരികെ OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    സ്ഥിതം isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    എങ്കിൽ (cl != ശൂന്യ)
	തിരികെ ISN_METHODCALL;

    എങ്കിൽ (ufunc->uf_def_status == UF_NOT_COMPILED)
	തിരികെ ISN_UCALL;

    // function invoked from a lambda
    എങ്കിൽ (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	എങ്കിൽ (ufunc->uf_name[0] == K_SPECIAL)
	    തിരികെ ISN_UCALL;
    }

    തിരികെ ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    പൂർണ്ണസംഖ്യ
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	പൂർണ്ണസംഖ്യ	    mi,
	പൂർണ്ണസംഖ്യ	    pushed_argcount,
	പൂർണ്ണസംഖ്യ	    is_super)
{
    isn_T	*isn;
    പൂർണ്ണസംഖ്യ		regular_args = ufunc->uf_args.ga_len;
    പൂർണ്ണസംഖ്യ		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	തിരികെ FAIL;
    }
    എങ്കിൽ (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	തിരികെ FAIL;
    }

    എങ്കിൽ (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	പൂർണ്ണസംഖ്യ		i;
	compiletype_T	compile_type;

	വേണ്ടി (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_അട്ടി(cctx, argcount - i - 1);
	    എങ്കിൽ (check_type_is_മൂല്യം(actual) == FAIL)
		തിരികെ FAIL;
	    എങ്കിൽ (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		തുടരുക;
	    }
	    എങ്കിൽ (i < regular_args)
	    {
		എങ്കിൽ (ufunc->uf_arg_types == ശൂന്യ)
		    തുടരുക;
		expected = ufunc->uf_arg_types[i];
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (ufunc->uf_va_type == ശൂന്യ
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    അല്ലെങ്കിൽ
		expected = ufunc->uf_va_type->tt_member;
	    എങ്കിൽ (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		തിരികെ FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	എങ്കിൽ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == ശൂന്യ,
						   compile_type, ശൂന്യ) == FAIL)
	    തിരികെ FAIL;
    }
    എങ്കിൽ (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	തിരികെ FAIL;
    }

    എങ്കിൽ ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == ശൂന്യ)
	തിരികെ FAIL;
    എങ്കിൽ (cl != ശൂന്യ /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	എങ്കിൽ (isn->isn_arg.mfunc == ശൂന്യ)
	    തിരികെ FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = ശൂന്യ;
	എങ്കിൽ (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = ശൂന്യ;
	isn->isn_arg.mfunc->cmf_cache_generic = ശൂന്യ;
	isn->isn_arg.mfunc->cmf_cache_ufunc = ശൂന്യ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    അല്ലെങ്കിൽ
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_അട്ടി.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    എങ്കിൽ (ufunc->uf_class != ശൂന്യ)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_അട്ടി(cctx, 0);
	എങ്കിൽ (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_അട്ടി.ga_len--;
    }

    // add return type
    തിരികെ തള്ളുക_type_അട്ടി(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    പൂർണ്ണസംഖ്യ
generate_UCALL(cctx_T *cctx, char_u *name, പൂർണ്ണസംഖ്യ argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_UCALL)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_അട്ടി.ga_len -= argcount;

    // add return value
    തിരികെ തള്ളുക_type_അട്ടി(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    പൂർണ്ണസംഖ്യ
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	പൂർണ്ണസംഖ്യ	argcount,
	പൂർണ്ണസംഖ്യ	at_മുകൾ,
	char_u	*name)
{
    എങ്കിൽ (type->tt_argcount != -1)
    {
	പൂർണ്ണസംഖ്യ	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	എങ്കിൽ (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (type->tt_args != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ i;

	    വേണ്ടി (i = 0; i < argcount; ++i)
	    {
		പൂർണ്ണസംഖ്യ	offset = -argcount + i - (at_മുകൾ ? 0 : 1);
		type_T	*actual = get_type_on_അട്ടി(cctx, -1 - offset);
		type_T	*expected;

		എങ്കിൽ (check_type_is_മൂല്യം(actual) == FAIL)
		    തിരികെ FAIL;
		എങ്കിൽ (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    എങ്കിൽ (expected != ശൂന്യ && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    എങ്കിൽ (expected == ശൂന്യ)
			expected = &t_any;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		അല്ലെങ്കിൽ
		    expected = type->tt_args[i];
		എങ്കിൽ (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    തിരികെ FAIL;
		}
	    }
	}
    }

    തിരികെ OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    പൂർണ്ണസംഖ്യ
generate_PCALL(
	cctx_T	*cctx,
	പൂർണ്ണസംഖ്യ	argcount,
	char_u	*name,
	type_T	*type,
	പൂർണ്ണസംഖ്യ	at_മുകൾ)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    എങ്കിൽ (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    അല്ലെങ്കിൽ എങ്കിൽ (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	എങ്കിൽ (check_func_args_from_type(cctx, type, argcount, at_മുകൾ, name)
								       == FAIL)
	    തിരികെ FAIL;

	ret_type = type->tt_member;
	എങ്കിൽ (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    അല്ലെങ്കിൽ
    {
	semsg(_(e_not_callable_type_str), name);
	തിരികെ FAIL;
    }

    എങ്കിൽ ((isn = generate_instr(cctx, ISN_PCALL)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.pfunc.cpf_മുകൾ = at_മുകൾ;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_അട്ടി.ga_len -= argcount + 1;

    // push the return value
    തള്ളുക_type_അട്ടി(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    എങ്കിൽ (at_മുകൾ && generate_instr(cctx, ISN_PCALL_END) == ശൂന്യ)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_DEFER(cctx_T *cctx, പൂർണ്ണസംഖ്യ var_idx, പൂർണ്ണസംഖ്യ argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    തിരികെ OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_അട്ടി(cctx, 0);
    എങ്കിൽ (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	അക്ഷരം *tofree;

	semsg(_(e_expected_dictionary_for_using_താക്കോൽ_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	തിരികെ FAIL;
    }
    // change dict type to dict member type
    എങ്കിൽ (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_അട്ടി(cctx, ntype, 0);
    }

    തിരികെ OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_ECHO(cctx_T *cctx, പൂർണ്ണസംഖ്യ with_white, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_drop(cctx, ISN_ECHO, എണ്ണം)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_എണ്ണം = എണ്ണം;

    തിരികെ OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_MULT_വ്യഞ്ജകം(cctx_T *cctx, isntype_T isn_type, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr_drop(cctx, isn_type, എണ്ണം)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = എണ്ണം;
    തിരികെ OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    പൂർണ്ണസംഖ്യ
generate_ECHOWINDOW(cctx_T *cctx, പൂർണ്ണസംഖ്യ എണ്ണം, long time)
{
    isn_T	*isn;

    എങ്കിൽ ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, എണ്ണം)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.echowin.ewin_എണ്ണം = എണ്ണം;
    isn->isn_arg.echowin.ewin_time = time;
    തിരികെ OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    പൂർണ്ണസംഖ്യ
generate_SOURCE(cctx_T *cctx, പൂർണ്ണസംഖ്യ sid)
{
    isn_T	*isn;

    എങ്കിൽ ((isn = generate_instr(cctx, ISN_SOURCE)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = sid;

    തിരികെ OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    പൂർണ്ണസംഖ്യ
generate_PUT(cctx_T *cctx, പൂർണ്ണസംഖ്യ regname, linenr_T lnum, പൂർണ്ണസംഖ്യ fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    എങ്കിൽ (isn == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    തിരികെ OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    പൂർണ്ണസംഖ്യ
generate_LOCKUNLOCK(cctx_T *cctx, char_u *വരി, പൂർണ്ണസംഖ്യ is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == ശൂന്യ)
	തിരികെ FAIL;
    class_T *cl = cctx->ctx_ufunc != ശൂന്യ ? cctx->ctx_ufunc->uf_class : ശൂന്യ;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(വരി);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    എങ്കിൽ (cl != ശൂന്യ)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    തിരികെ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    പൂർണ്ണസംഖ്യ
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *വരി)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, isntype)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.string = vim_strsave(വരി);
    തിരികെ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    പൂർണ്ണസംഖ്യ
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    പൂർണ്ണസംഖ്യ		ret = OK;

    എങ്കിൽ (cctx->ctx_skip != SKIP_YES)
    {
	എങ്കിൽ ((isn = generate_instr(cctx, isntype)) == ശൂന്യ)
	    ret = FAIL;
	അല്ലെങ്കിൽ
	{
	    isn->isn_arg.string = str;
	    തിരികെ OK;
	}
    }
    vim_free(str);
    തിരികെ ret;
}

    പൂർണ്ണസംഖ്യ
generate_LEGACY_EVAL(cctx_T *cctx, char_u *വരി)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.string = vim_strsave(വരി);

    തിരികെ തള്ളുക_type_അട്ടി(cctx, &t_any);
}

    പൂർണ്ണസംഖ്യ
generate_EXECCONCAT(cctx_T *cctx, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    isn_T	*isn;

    എങ്കിൽ ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, എണ്ണം)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.സംഖ്യ = എണ്ണം;
    തിരികെ OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    പൂർണ്ണസംഖ്യ
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    എങ്കിൽ ((isn = generate_instr(cctx, ISN_RANGE)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.string = range;

    തിരികെ തള്ളുക_type_അട്ടി(cctx, &t_സംഖ്യ);
}

    പൂർണ്ണസംഖ്യ
generate_UNPACK(cctx_T *cctx, പൂർണ്ണസംഖ്യ var_എണ്ണം, പൂർണ്ണസംഖ്യ semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_UNPACK)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.unpack.unp_എണ്ണം = var_എണ്ണം;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    തിരികെ OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    പൂർണ്ണസംഖ്യ
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    എങ്കിൽ (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	എങ്കിൽ ((isn = generate_instr(cctx, ISN_CMDMOD)) == ശൂന്യ)
	    തിരികെ FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	എങ്കിൽ (isn->isn_arg.cmdmod.cf_cmdmod == ശൂന്യ)
	    തിരികെ FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, വലുപ്പം(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = ശൂന്യ;
    }

    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
generate_undo_cmdmods(cctx_T *cctx)
{
    എങ്കിൽ (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == ശൂന്യ)
	തിരികെ FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    തിരികെ OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	പൂർണ്ണസംഖ്യ		opt_flags,
	പൂർണ്ണസംഖ്യ		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    തിരഞ്ഞെടുപ്പ് (dest)
    {
	അവസ്ഥ dest_option:
	    തിരികെ generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	അവസ്ഥ dest_func_option:
	    തിരികെ generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	അവസ്ഥ dest_global:
	    // include g: with the name, easier to execute that way
	    തിരികെ generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == ശൂന്യ
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	അവസ്ഥ dest_buffer:
	    // include b: with the name, easier to execute that way
	    തിരികെ generate_STORE(cctx, ISN_STOREB, 0, name);
	അവസ്ഥ dest_window:
	    // include w: with the name, easier to execute that way
	    തിരികെ generate_STORE(cctx, ISN_STOREW, 0, name);
	അവസ്ഥ dest_tab:
	    // include t: with the name, easier to execute that way
	    തിരികെ generate_STORE(cctx, ISN_STORET, 0, name);
	അവസ്ഥ dest_env:
	    തിരികെ generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	അവസ്ഥ dest_reg:
	    തിരികെ generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], ശൂന്യ);
	അവസ്ഥ dest_vimvar:
	    തിരികെ generate_STORE(cctx, ISN_STOREV, vimvaridx, ശൂന്യ);
	അവസ്ഥ dest_script:
	അവസ്ഥ dest_script_v9:
	    {
		പൂർണ്ണസംഖ്യ	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		പൂർണ്ണസംഖ്യ	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		എങ്കിൽ (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    എങ്കിൽ (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == ശൂന്യ)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			എങ്കിൽ (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    തിരികെ FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    തിരികെ generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		തിരികെ generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	അവസ്ഥ dest_class_member:
	    തിരികെ generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	അവസ്ഥ dest_local:
	അവസ്ഥ dest_വ്യഞ്ജകം:
	    // cannot happen
	    നിർത്തുക;
    }
    തിരികെ FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    പൂർണ്ണസംഖ്യ
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    വേണ്ടി (;;)
    {
	എങ്കിൽ (scope == ശൂന്യ)
	    നിർത്തുക;
	എങ്കിൽ (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    തിരികെ TRUE;
	scope = scope->se_outer;
    }
    തിരികെ FALSE;
}

    പൂർണ്ണസംഖ്യ
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, പൂർണ്ണസംഖ്യ instr_എണ്ണം, പൂർണ്ണസംഖ്യ is_decl)
{
    എങ്കിൽ (lhs->lhs_dest != dest_local)
	തിരികെ generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    എങ്കിൽ (lhs->lhs_lvar == ശൂന്യ)
	തിരികെ OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    എങ്കിൽ (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_എണ്ണം + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.സംഖ്യ;
	garray_T    *അട്ടി = &cctx->ctx_type_അട്ടി;

	എങ്കിൽ (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	അല്ലെങ്കിൽ
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	എങ്കിൽ (അട്ടി->ga_len > 0)
	    --അട്ടി->ga_len;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    അല്ലെങ്കിൽ
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, ശൂന്യ);
    തിരികെ OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    പൂർണ്ണസംഖ്യ
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    എങ്കിൽ ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == ശൂന്യ)
	തിരികെ FAIL;
    isn->isn_arg.setsctx = new_sctx;
    തിരികെ OK;
}

#എങ്കിൽ defined(FEAT_PROFILE)
    ശൂന്യം
may_generate_prof_end(cctx_T *cctx, പൂർണ്ണസംഖ്യ prof_lnum)
{
    എങ്കിൽ (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_END);
}
#endif


/*
 * Delete an instruction, free what it contains.
 */
    ശൂന്യം
delete_instr(isn_T *isn)
{
    തിരഞ്ഞെടുപ്പ് (isn->isn_type)
    {
	അവസ്ഥ ISN_AUTOLOAD:
	അവസ്ഥ ISN_DEF:
	അവസ്ഥ ISN_EXEC:
	അവസ്ഥ ISN_EXECRANGE:
	അവസ്ഥ ISN_EXEC_SPLIT:
	അവസ്ഥ ISN_LEGACY_EVAL:
	അവസ്ഥ ISN_LOADAUTO:
	അവസ്ഥ ISN_LOADB:
	അവസ്ഥ ISN_LOADENV:
	അവസ്ഥ ISN_LOADG:
	അവസ്ഥ ISN_LOADOPT:
	അവസ്ഥ ISN_LOADT:
	അവസ്ഥ ISN_LOADW:
	അവസ്ഥ ISN_PUSHEXC:
	അവസ്ഥ ISN_PUSHFUNC:
	അവസ്ഥ ISN_PUSHS:
	അവസ്ഥ ISN_RANGE:
	അവസ്ഥ ISN_STOREAUTO:
	അവസ്ഥ ISN_STOREB:
	അവസ്ഥ ISN_STOREENV:
	അവസ്ഥ ISN_STOREG:
	അവസ്ഥ ISN_STORET:
	അവസ്ഥ ISN_STOREW:
	അവസ്ഥ ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    നിർത്തുക;

	അവസ്ഥ ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    നിർത്തുക;

	അവസ്ഥ ISN_SUBSTITUTE:
	    {
		പൂർണ്ണസംഖ്യ	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		വേണ്ടി (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    നിർത്തുക;

	അവസ്ഥ ISN_INSTR:
	    {
		പൂർണ്ണസംഖ്യ	idx;
		isn_T	*list = isn->isn_arg.instr;

		വേണ്ടി (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    നിർത്തുക;

	അവസ്ഥ ISN_LOADS:
	അവസ്ഥ ISN_LOADEXPORT:
	അവസ്ഥ ISN_STORES:
	അവസ്ഥ ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    നിർത്തുക;

	അവസ്ഥ ISN_UNLET:
	അവസ്ഥ ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    നിർത്തുക;

	അവസ്ഥ ISN_STOREOPT:
	അവസ്ഥ ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    നിർത്തുക;

	അവസ്ഥ ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    നിർത്തുക;

	അവസ്ഥ ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    നിർത്തുക;

	അവസ്ഥ ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    നിർത്തുക;

	അവസ്ഥ ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		എങ്കിൽ (extra == ശൂന്യ || extra->fre_func_name == ശൂന്യ)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    എങ്കിൽ (ufunc != ശൂന്യ && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		എങ്കിൽ (extra != ശൂന്യ)
		{
		    char_u *name = extra->fre_func_name;

		    എങ്കിൽ (name != ശൂന്യ)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    എങ്കിൽ (extra->fre_class != ശൂന്യ)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    നിർത്തുക;

	അവസ്ഥ ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		എങ്കിൽ (dfunc->df_ufunc != ശൂന്യ
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    നിർത്തുക;

	അവസ്ഥ ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    നിർത്തുക;

	അവസ്ഥ ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		എങ്കിൽ (arg != ശൂന്യ)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    എങ്കിൽ (ufunc != ശൂന്യ)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    നിർത്തുക;

	അവസ്ഥ ISN_CHECKTYPE:
	അവസ്ഥ ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    നിർത്തുക;

	അവസ്ഥ ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    നിർത്തുക;

	അവസ്ഥ ISN_LOADSCRIPT:
	അവസ്ഥ ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    നിർത്തുക;

	അവസ്ഥ ISN_LOAD_CLASSMEMBER:
	അവസ്ഥ ISN_STORE_CLASSMEMBER:
	അവസ്ഥ ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    നിർത്തുക;

	അവസ്ഥ ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    നിർത്തുക;

	അവസ്ഥ ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    നിർത്തുക;

	അവസ്ഥ ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    നിർത്തുക;

	അവസ്ഥ ISN_2BOOL:
	അവസ്ഥ ISN_2STRING:
	അവസ്ഥ ISN_2STRING_ANY:
	അവസ്ഥ ISN_ADDBLOB:
	അവസ്ഥ ISN_ADDLIST:
	അവസ്ഥ ISN_ADDTUPLE:
	അവസ്ഥ ISN_ANYINDEX:
	അവസ്ഥ ISN_ANYSLICE:
	അവസ്ഥ ISN_BCALL:
	അവസ്ഥ ISN_BLOBAPPEND:
	അവസ്ഥ ISN_BLOBINDEX:
	അവസ്ഥ ISN_BLOBSLICE:
	അവസ്ഥ ISN_CATCH:
	അവസ്ഥ ISN_CEXPR_AUCMD:
	അവസ്ഥ ISN_CHECKLEN:
	അവസ്ഥ ISN_CLEARDICT:
	അവസ്ഥ ISN_CMDMOD_REV:
	അവസ്ഥ ISN_COMPAREANY:
	അവസ്ഥ ISN_COMPAREBLOB:
	അവസ്ഥ ISN_COMPAREBOOL:
	അവസ്ഥ ISN_COMPAREDICT:
	അവസ്ഥ ISN_COMPAREFLOAT:
	അവസ്ഥ ISN_COMPAREFUNC:
	അവസ്ഥ ISN_COMPARELIST:
	അവസ്ഥ ISN_COMPARETUPLE:
	അവസ്ഥ ISN_COMPARENR:
	അവസ്ഥ ISN_COMPARENULL:
	അവസ്ഥ ISN_COMPAREOBJECT:
	അവസ്ഥ ISN_COMPARESPECIAL:
	അവസ്ഥ ISN_COMPARESTRING:
	അവസ്ഥ ISN_CONCAT:
	അവസ്ഥ ISN_CONSTRUCT:
	അവസ്ഥ ISN_COND2BOOL:
	അവസ്ഥ ISN_DEBUG:
	അവസ്ഥ ISN_DEFER:
	അവസ്ഥ ISN_DROP:
	അവസ്ഥ ISN_ECHO:
	അവസ്ഥ ISN_ECHOCONSOLE:
	അവസ്ഥ ISN_ECHOERR:
	അവസ്ഥ ISN_ECHOMSG:
	അവസ്ഥ ISN_ECHOWINDOW:
	അവസ്ഥ ISN_ENDLOOP:
	അവസ്ഥ ISN_ENDTRY:
	അവസ്ഥ ISN_EXECCONCAT:
	അവസ്ഥ ISN_EXECUTE:
	അവസ്ഥ ISN_FINALLY:
	അവസ്ഥ ISN_FINISH:
	അവസ്ഥ ISN_FOR:
	അവസ്ഥ ISN_GETITEM:
	അവസ്ഥ ISN_GET_OBJ_MEMBER:
	അവസ്ഥ ISN_JUMP:
	അവസ്ഥ ISN_JUMP_IF_ARG_NOT_SET:
	അവസ്ഥ ISN_JUMP_IF_ARG_SET:
	അവസ്ഥ ISN_LISTAPPEND:
	അവസ്ഥ ISN_LISTINDEX:
	അവസ്ഥ ISN_LISTSLICE:
	അവസ്ഥ ISN_TUPLEINDEX:
	അവസ്ഥ ISN_TUPLESLICE:
	അവസ്ഥ ISN_LOAD:
	അവസ്ഥ ISN_LOADBDICT:
	അവസ്ഥ ISN_LOADGDICT:
	അവസ്ഥ ISN_LOADOUTER:
	അവസ്ഥ ISN_LOADREG:
	അവസ്ഥ ISN_LOADTDICT:
	അവസ്ഥ ISN_LOADV:
	അവസ്ഥ ISN_LOADWDICT:
	അവസ്ഥ ISN_LOCKCONST:
	അവസ്ഥ ISN_MEMBER:
	അവസ്ഥ ISN_NEGATENR:
	അവസ്ഥ ISN_NEWDICT:
	അവസ്ഥ ISN_NEWLIST:
	അവസ്ഥ ISN_NEWTUPLE:
	അവസ്ഥ ISN_NEWPARTIAL:
	അവസ്ഥ ISN_OPANY:
	അവസ്ഥ ISN_OPFLOAT:
	അവസ്ഥ ISN_OPNR:
	അവസ്ഥ ISN_PCALL:
	അവസ്ഥ ISN_PCALL_END:
	അവസ്ഥ ISN_PROF_END:
	അവസ്ഥ ISN_PROF_START:
	അവസ്ഥ ISN_PUSHBOOL:
	അവസ്ഥ ISN_PUSHCHANNEL:
	അവസ്ഥ ISN_PUSHF:
	അവസ്ഥ ISN_PUSHJOB:
	അവസ്ഥ ISN_PUSHNR:
	അവസ്ഥ ISN_PUSHOBJ:
	അവസ്ഥ ISN_PUSHSPEC:
	അവസ്ഥ ISN_PUT:
	അവസ്ഥ ISN_IPUT:
	അവസ്ഥ ISN_REDIREND:
	അവസ്ഥ ISN_REDIRSTART:
	അവസ്ഥ ISN_RETURN:
	അവസ്ഥ ISN_RETURN_OBJECT:
	അവസ്ഥ ISN_RETURN_VOID:
	അവസ്ഥ ISN_SHUFFLE:
	അവസ്ഥ ISN_SLICE:
	അവസ്ഥ ISN_SOURCE:
	അവസ്ഥ ISN_STORE:
	അവസ്ഥ ISN_STORENR:
	അവസ്ഥ ISN_STOREOUTER:
	അവസ്ഥ ISN_STORE_THIS:
	അവസ്ഥ ISN_STORERANGE:
	അവസ്ഥ ISN_STOREREG:
	അവസ്ഥ ISN_STOREV:
	അവസ്ഥ ISN_STRINDEX:
	അവസ്ഥ ISN_STRSLICE:
	അവസ്ഥ ISN_THROW:
	അവസ്ഥ ISN_TRYCONT:
	അവസ്ഥ ISN_UNLETINDEX:
	അവസ്ഥ ISN_UNLETRANGE:
	അവസ്ഥ ISN_UNPACK:
	അവസ്ഥ ISN_USEDICT:
	അവസ്ഥ ISN_WHILE:
	അവസ്ഥ ISN_SCRIPTCTX_SET:
	// nothing allocated
	നിർത്തുക;
    }
}

    ശൂന്യം
clear_instr_ga(garray_T *gap)
{
    പൂർണ്ണസംഖ്യ idx;

    വേണ്ടി (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
