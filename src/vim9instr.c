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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

/////////////////////////////////////////////////////////////////////
// Following generate_ functions expect the caller to call ga_grow().

#define RETURN_NULL_IF_SKIP(cctx) ଯଦି (cctx->ctx_skip == SKIP_YES) ଫେରନ୍ତୁ ଖାଲି
#define RETURN_OK_IF_SKIP(cctx) ଯଦି (cctx->ctx_skip == SKIP_YES) ଫେରନ୍ତୁ OK

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
    ଯଦି (GA_GROW_FAILS(instr, 1))
	ଫେରନ୍ତୁ ଖାଲି;
    isn = ((isn_T *)instr->ga_data) + instr->ga_len;
    isn->isn_type = isn_type;
    isn->isn_lnum = cctx->ctx_lnum + 1;
    ++instr->ga_len;

    ଫେରନ୍ତୁ isn;
}

/*
 * Generate an instruction without arguments.
 * "drop" will be removed from the stack.
 * Returns a pointer to the new instruction, NULL if failed.
 */
    isn_T *
generate_instr_drop(cctx_T *cctx, isntype_T isn_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା drop)
{
    RETURN_NULL_IF_SKIP(cctx);
    cctx->ctx_type_ଗଦା.ga_len -= drop;
    ଫେରନ୍ତୁ generate_instr(cctx, isn_type);
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack,
 * use "decl_type" for the declared type.
 */
    ସ୍ଥିର isn_T *
generate_instr_type2(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	type_T	    *type,
	type_T	    *decl_type)
{
    isn_T	*isn;

    ଯଦି ((isn = generate_instr(cctx, isn_type)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (ଠେଲ_type_stack2(cctx, type == ଖାଲି ? &t_any : type,
			       decl_type == ଖାଲି ? &t_any : decl_type) == FAIL)
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ isn;
}

/*
 * Generate instruction "isn_type" and put "type" on the type stack.
 * Uses "any" for the declared type, which works for constants.  For declared
 * variables use generate_instr_type2().
 */
    isn_T *
generate_instr_type(cctx_T *cctx, isntype_T isn_type, type_T *type)
{
    ଫେରନ୍ତୁ generate_instr_type2(cctx, isn_type, type, &t_any);
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

    ଯଦି ((isn = generate_instr(cctx, ISN_DEBUG)) == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    isn->isn_arg.debug.dbg_var_names_len = dfunc->df_var_names.ga_len;
    isn->isn_arg.debug.dbg_break_lnum = cctx->ctx_prev_lnum;
    ଫେରନ୍ତୁ isn;
}

/*
 * Generate an ISN_CONSTRUCT instruction.
 * The object will have "size" members.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_CONSTRUCT(cctx_T *cctx, class_T *cl)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_CONSTRUCT)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.construct.construct_size = ଆକାର(object_T)
			       + cl->class_obj_member_ଗଣନା * ଆକାର(typval_T);
    isn->isn_arg.construct.construct_class = cl;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate ISN_GET_OBJ_MEMBER - access member of object at bottom of stack by
 * index.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_GET_OBJ_MEMBER(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_OBJ_MEMBER, 1);
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    isn->isn_arg.classmember.cm_class = ଖାଲି;
    isn->isn_arg.classmember.cm_idx = idx;
    ଫେରନ୍ତୁ ଠେଲ_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_GET_ITF_MEMBER - access member of interface at bottom of stack
 * by index.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_GET_ITF_MEMBER(cctx_T *cctx, class_T *itf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, type_T *type)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the object type
    isn_T *isn = generate_instr_drop(cctx, ISN_GET_ITF_MEMBER, 1);
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    isn->isn_arg.classmember.cm_class = itf;
    ++itf->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;
    ଫେରନ୍ତୁ ଠେଲ_type_stack2(cctx, type, &t_any);
}

/*
 * Generate ISN_STORE_THIS - store value in member of "this" object with member
 * index "idx".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_STORE_THIS(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    RETURN_OK_IF_SKIP(cctx);

    // drop the value type
    isn_T *isn = generate_instr_drop(cctx, ISN_STORE_THIS, 1);
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    isn->isn_arg.ସଂଖ୍ୟା = idx;
    ଫେରନ୍ତୁ OK;
}

/*
 * If type at "offset" isn't already VAR_STRING then generate ISN_2STRING.
 * But only for simple types.
 * When tostring_flags has TOSTRING_TOLERANT, convert a List to a series of
 * strings.  When tostring_flags has TOSTRING_INTERPOLATE, convert a List or a
 * Dict to the corresponding textual representation.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_generate_2STRING(ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tostring_flags, cctx_T *cctx)
{
    isn_T	*isn;
    isntype_T	isntype = ISN_2STRING;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    type = get_type_on_ଗଦା(cctx, -1 - offset);

    ବାଛ (type->tt_type)
    {
	// nothing to be done
	ସ୍ଥିତି VAR_STRING: ଫେରନ୍ତୁ OK;

	// conversion possible
	ସ୍ଥିତି VAR_SPECIAL:
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	ସ୍ଥିତି VAR_FLOAT:
			 ଭାଙ୍ଗ;

	// conversion possible (with runtime check)
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_UNKNOWN:
			 isntype = ISN_2STRING_ANY;
			 ଭାଙ୍ଗ;

	// conversion possible when tolerant
	ସ୍ଥିତି VAR_LIST:
	ସ୍ଥିତି VAR_TUPLE:
	ସ୍ଥିତି VAR_DICT:
			 ଯଦି (tostring_flags & TOSTRING_TOLERANT)
			 {
			     isntype = ISN_2STRING_ANY;
			     ଭାଙ୍ଗ;
			 }
			 ଯଦି (tostring_flags & TOSTRING_INTERPOLATE)
			     ଭାଙ୍ଗ;
			 // FALLTHROUGH

	// conversion not possible
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_BLOB:
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_PARTIAL:
	ସ୍ଥିତି VAR_JOB:
	ସ୍ଥିତି VAR_CHANNEL:
	ସ୍ଥିତି VAR_INSTR:
	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_OBJECT:
	ସ୍ଥିତି VAR_TYPEALIAS:
			 to_string_error(type->tt_type);
			 ଫେରନ୍ତୁ FAIL;
    }

    set_type_on_ଗଦା(cctx, &t_string, -1 - offset);
    ଯଦି ((isn = generate_instr(cctx, isntype)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.tostring.offset = offset;
    isn->isn_arg.tostring.flags = tostring_flags;

    ଫେରନ୍ତୁ OK;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_ସଂଖ୍ୟା_or_float(type_T *typ1, type_T *typ2, char_u *op)
{
    vartype_T	    type1 = typ1->tt_type;
    vartype_T	    type2 = typ2->tt_type;
    ଯଦି (!((type1 == VAR_ସଂଖ୍ୟା || type1 == VAR_FLOAT
				   || type1 == VAR_ANY || type1 == VAR_UNKNOWN)
	    && (type2 == VAR_ସଂଖ୍ୟା || type2 == VAR_FLOAT
				 || type2 == VAR_ANY || type2 == VAR_UNKNOWN)))
    {
	ଯଦି (check_type_is_ମୂଲ୍ୟ(typ1) == FAIL
		|| check_type_is_ମୂଲ୍ୟ(typ2) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (*op == '+')
	    emsg(_(e_wrong_argument_type_for_plus));
	ଅନ୍ୟଥା
	    semsg(_(e_char_requires_ସଂଖ୍ୟା_or_float_arguments), *op);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Append the tuple item types from "tuple_type" to the grow array "gap".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ga_append_tuple_types(type_T *tuple_type, garray_T *gap)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < tuple_type->tt_argcount; i++)
    {
	ଯଦି (ga_grow(gap, 1) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	((type_T **)gap->ga_data)[gap->ga_len] = tuple_type->tt_args[i];
	gap->ga_len++;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * When concatenating two tuples, the resulting tuple gets a union of item
 * types from both the tuples.  This function sets the union tuple type in the
 * stack.
 *
 * Returns OK on success and FAIL on memory allocation failure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_tuple_union_type_on_ଗଦା(type_T *type1, type_T *type2, cctx_T *cctx)
{
    // The concatenated tuple has the union of types from both the tuples
    garray_T	tuple_types_ga;

    ga_init2(&tuple_types_ga, ଆକାର(type_T *), 10);

    ଯଦି (type1->tt_argcount > 0)
	ga_append_tuple_types(type1, &tuple_types_ga);
    ଯଦି (!(type1->tt_flags & TTFLAG_VARARGS) && (type2->tt_argcount > 0))
	ga_append_tuple_types(type2, &tuple_types_ga);

    type_T *new_tuple_type = get_tuple_type(&tuple_types_ga,
							cctx->ctx_type_list);
    // result inherits the variadic flag from the operands
    ଯଦି ((new_tuple_type->tt_flags & TTFLAG_STATIC) == 0)
	new_tuple_type->tt_flags |= (type1->tt_flags & TTFLAG_VARARGS)
					| (type2->tt_flags & TTFLAG_VARARGS);

    // set the type on the stack for the resulting tuple
    set_type_on_ଗଦା(cctx, new_tuple_type, 0);

    ga_clear(&tuple_types_ga);

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate instruction for "+".  For a list this creates a new list.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_add_instr(
	cctx_T	    *cctx,
	vartype_T   vartype,
	type_T	    *type1,
	type_T	    *type2,
	exprtype_T  ପଦସମଷ୍ଟି_type)
{
    isn_T	*isn = generate_instr_drop(cctx,
		      vartype == VAR_ସଂଖ୍ୟା ? ISN_OPNR
		    : vartype == VAR_LIST ? ISN_ADDLIST
		    : vartype == VAR_TUPLE ? ISN_ADDTUPLE
		    : vartype == VAR_BLOB ? ISN_ADDBLOB
		    : vartype == VAR_FLOAT ? ISN_OPFLOAT
		    : ISN_OPANY, 1);

    ଯଦି (vartype != VAR_LIST && vartype != VAR_BLOB && vartype != VAR_TUPLE
	    && type1->tt_type != VAR_ANY
	    && type1->tt_type != VAR_UNKNOWN
	    && type2->tt_type != VAR_ANY
	    && type2->tt_type != VAR_UNKNOWN
	    && check_ସଂଖ୍ୟା_or_float(type1, type2, (char_u *)"+") == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (isn != ଖାଲି)
    {
	ଯଦି (isn->isn_type == ISN_ADDLIST)
	    isn->isn_arg.op.op_type = ପଦସମଷ୍ଟି_type;
	ଅନ୍ୟଥା
	    isn->isn_arg.op.op_type = ପଦସମଷ୍ଟି_ADD;
    }

    // When concatenating two lists with different member types the member type
    // becomes "any".
    ଯଦି (vartype == VAR_LIST
	    && type1->tt_type == VAR_LIST && type2->tt_type == VAR_LIST
	    && type1->tt_member != type2->tt_member)
	set_type_on_ଗଦା(cctx, &t_list_any, 0);
    ଅନ୍ୟଥା ଯଦି (vartype == VAR_TUPLE)
    {
	ଯଦି (!check_tuples_addable(type1, type2))
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (set_tuple_union_type_on_ଗଦା(type1, type2, cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ isn == ଖାଲି ? FAIL : OK;
}

/*
 * Get the type to use for an instruction for an operation on "type1" and
 * "type2".  If they are matching use a type-specific instruction. Otherwise
 * fall back to runtime type checking.
 */
    vartype_T
operator_type(type_T *type1, type_T *type2)
{
    ଯଦି (type1->tt_type == type2->tt_type
	    && (type1->tt_type == VAR_ସଂଖ୍ୟା
		|| type1->tt_type == VAR_LIST
		|| type1->tt_type == VAR_TUPLE
		|| type1->tt_type == VAR_FLOAT
		|| type1->tt_type == VAR_BLOB))
	ଫେରନ୍ତୁ type1->tt_type;
    ଫେରନ୍ତୁ VAR_ANY;
}

/*
 * Generate an instruction with two arguments.  The instruction depends on the
 * type of the arguments.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_two_op(cctx_T *cctx, char_u *op)
{
    type_T	*type1;
    type_T	*type2;
    vartype_T	vartype;
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.
    type1 = get_type_on_ଗଦା(cctx, 1);
    type2 = get_type_on_ଗଦା(cctx, 0);
    vartype = operator_type(type1, type2);

    ବାଛ (*op)
    {
	ସ୍ଥିତି '+':
		  ଯଦି (generate_add_instr(cctx, vartype, type1, type2,
							    ପଦସମଷ୍ଟି_COPY) == FAIL)
		      ଫେରନ୍ତୁ FAIL;
		  ଭାଙ୍ଗ;

	ସ୍ଥିତି '-':
	ସ୍ଥିତି '*':
	ସ୍ଥିତି '/': ଯଦି (check_ସଂଖ୍ୟା_or_float(type1, type2, op) == FAIL)
		      ଫେରନ୍ତୁ FAIL;
		  ଯଦି (vartype == VAR_ସଂଖ୍ୟା)
		      isn = generate_instr_drop(cctx, ISN_OPNR, 1);
		  ଅନ୍ୟଥା ଯଦି (vartype == VAR_FLOAT)
		      isn = generate_instr_drop(cctx, ISN_OPFLOAT, 1);
		  ଅନ୍ୟଥା
		      isn = generate_instr_drop(cctx, ISN_OPANY, 1);
		  ଯଦି (isn != ଖାଲି)
		      isn->isn_arg.op.op_type = *op == '*'
				 ? ପଦସମଷ୍ଟି_MULT : *op == '/'? ପଦସମଷ୍ଟି_DIV : ପଦସମଷ୍ଟି_SUB;
		  ଭାଙ୍ଗ;

	ସ୍ଥିତି '%': ଯଦି ((type1->tt_type != VAR_ANY
			      && type1->tt_type != VAR_UNKNOWN
					       && type1->tt_type != VAR_ସଂଖ୍ୟା)
			  || (type2->tt_type != VAR_ANY
			      && type2->tt_type != VAR_UNKNOWN
					      && type2->tt_type != VAR_ସଂଖ୍ୟା))
		  {
		      emsg(_(e_percent_requires_ସଂଖ୍ୟା_arguments));
		      ଫେରନ୍ତୁ FAIL;
		  }
		  isn = generate_instr_drop(cctx,
			      vartype == VAR_ସଂଖ୍ୟା ? ISN_OPNR : ISN_OPANY, 1);
		  ଯଦି (isn != ଖାଲି)
		      isn->isn_arg.op.op_type = ପଦସମଷ୍ଟି_REM;
		  ଭାଙ୍ଗ;
    }

    // correct type of result
    ଯଦି (vartype == VAR_ANY)
    {
	type_T *type = &t_any;

	// float+number and number+float results in float
	ଯଦି ((type1->tt_type == VAR_ସଂଖ୍ୟା || type1->tt_type == VAR_FLOAT)
	      && (type2->tt_type == VAR_ସଂଖ୍ୟା || type2->tt_type == VAR_FLOAT))
	    type = &t_float;
	set_type_on_ଗଦା(cctx, type, 0);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Choose correct error message for the specified type information.
 */
    ସ୍ଥିର isntype_T
compare_isn_not_ମୂଲ୍ୟଗୁଡ଼ିକ(typval_T *tv, type_T *type)
{
    ଯଦି (tv != ଖାଲି)
	(ଶୂନ୍ୟ)check_typval_is_ମୂଲ୍ୟ(tv);
    ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)check_type_is_ମୂଲ୍ୟ(type);
    ଫେରନ୍ତୁ ISN_DROP;
}

/*
 * Get the instruction to use for comparing two values with specified types.
 * Either "tv1" and "tv2" are passed or "type1" and "type2".
 * Return ISN_DROP when failed.
 */
    ସ୍ଥିର isntype_T
get_compare_isn(
	exprtype_T  exprtype,
	typval_T    *tv1,
	typval_T    *tv2,
	type_T	    *type1,
	type_T	    *type2)
{
    isntype_T	isntype = ISN_DROP;
    vartype_T	vartype1 = tv1 != ଖାଲି ? tv1->v_type : type1->tt_type;
    vartype_T	vartype2 = tv2 != ଖାଲି ? tv2->v_type : type2->tt_type;

    ଯଦି (vartype1 == VAR_CLASS || vartype1 == VAR_TYPEALIAS)
	ଫେରନ୍ତୁ compare_isn_not_ମୂଲ୍ୟଗୁଡ଼ିକ(tv1, type1);
    ଯଦି (vartype2 == VAR_CLASS || vartype2 == VAR_TYPEALIAS)
	ଫେରନ୍ତୁ compare_isn_not_ମୂଲ୍ୟଗୁଡ଼ିକ(tv2, type2);

    ଯଦି (vartype1 == vartype2)
    {
	ବାଛ (vartype1)
	{
	    ସ୍ଥିତି VAR_BOOL: isntype = ISN_COMPAREBOOL; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_SPECIAL: isntype = ISN_COMPARESPECIAL; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_ସଂଖ୍ୟା: isntype = ISN_COMPARENR; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_FLOAT: isntype = ISN_COMPAREFLOAT; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_STRING: isntype = ISN_COMPARESTRING; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_BLOB: isntype = ISN_COMPAREBLOB; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_LIST: isntype = ISN_COMPARELIST; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_TUPLE: isntype = ISN_COMPARETUPLE; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_DICT: isntype = ISN_COMPAREDICT; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_FUNC: isntype = ISN_COMPAREFUNC; ଭାଙ୍ଗ;
	    ସ୍ଥିତି VAR_OBJECT: isntype = ISN_COMPAREOBJECT; ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: isntype = ISN_COMPAREANY; ଭାଙ୍ଗ;
	}
    }
    ଅନ୍ୟଥା ଯଦି (vartype1 == VAR_ANY || vartype2 == VAR_ANY
	    || ((vartype1 == VAR_ସଂଖ୍ୟା || vartype1 == VAR_FLOAT)
			  && (vartype2 == VAR_ସଂଖ୍ୟା || vartype2 == VAR_FLOAT))
	    || (vartype1 == VAR_FUNC && vartype2 == VAR_PARTIAL)
	    || (vartype1 == VAR_PARTIAL && vartype2 == VAR_FUNC))
	isntype = ISN_COMPAREANY;
    ଅନ୍ୟଥା ଯଦି (vartype1 == VAR_SPECIAL || vartype2 == VAR_SPECIAL)
    {
	ଯଦି ((vartype1 == VAR_SPECIAL
		&& (tv1 != ଖାଲି ? tv1->vval.v_ସଂଖ୍ୟା == VVAL_NONE
							    : type1 == &t_none)
		&& vartype2 != VAR_STRING)
	    || (vartype2 == VAR_SPECIAL
		&& (tv2 != ଖାଲି ? tv2->vval.v_ସଂଖ୍ୟା == VVAL_NONE
							    : type2 == &t_none)
		&& vartype1 != VAR_STRING))
	{
	    semsg(_(e_cannot_compare_str_with_str),
			       vartype_name(vartype1), vartype_name(vartype2));
	    ଫେରନ୍ତୁ ISN_DROP;
	}
	// although comparing null with number, float or bool is not useful, we
	// allow it
	isntype = ISN_COMPARENULL;
    }

    ଯଦି ((exprtype == ପଦସମଷ୍ଟି_IS || exprtype == ପଦସମଷ୍ଟି_ISNOT)
	    && (isntype == ISN_COMPAREBOOL
	    || isntype == ISN_COMPARESPECIAL
	    || isntype == ISN_COMPARENR
	    || isntype == ISN_COMPAREFLOAT))
    {
	semsg(_(e_cannot_use_str_with_str),
		exprtype == ପଦସମଷ୍ଟି_IS ? "is" : "isnot" , vartype_name(vartype1));
	ଫେରନ୍ତୁ ISN_DROP;
    }
    ଯଦି (!(exprtype == ପଦସମଷ୍ଟି_IS || exprtype == ପଦସମଷ୍ଟି_ISNOT
		|| exprtype == ପଦସମଷ୍ଟି_EQUAL || exprtype == ପଦସମଷ୍ଟି_NEQUAL)
	    && (isntype == ISN_COMPAREOBJECT))
    {
	semsg(_(e_invalid_operation_for_str), vartype_name(vartype1));
	ଫେରନ୍ତୁ ISN_DROP;
    }
    ଯଦି (isntype == ISN_DROP
	    || (isntype != ISN_COMPARENULL
		&& (((exprtype != ପଦସମଷ୍ଟି_EQUAL
			&& exprtype != ପଦସମଷ୍ଟି_NEQUAL
			&& (vartype1 == VAR_BOOL || vartype1 == VAR_SPECIAL
			  || vartype2 == VAR_BOOL || vartype2 == VAR_SPECIAL)))
		    || ((exprtype != ପଦସମଷ୍ଟି_EQUAL
			 && exprtype != ପଦସମଷ୍ଟି_NEQUAL
			 && exprtype != ପଦସମଷ୍ଟି_IS
			 && exprtype != ପଦସମଷ୍ଟି_ISNOT
			 && (vartype1 == VAR_BLOB || vartype2 == VAR_BLOB
			  || vartype1 == VAR_LIST || vartype2 == VAR_LIST))))))
    {
	semsg(_(e_cannot_compare_str_with_str),
		vartype_name(vartype1), vartype_name(vartype2));
	ଫେରନ୍ତୁ ISN_DROP;
    }
    ଫେରନ୍ତୁ isntype;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_compare_types(exprtype_T type, typval_T *tv1, typval_T *tv2)
{
    ଯଦି (get_compare_isn(type, tv1, tv2, ଖାଲି, ଖାଲି) == ISN_DROP)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_COMPARE* instruction with a boolean result.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_COMPARE(cctx_T *cctx, exprtype_T exprtype, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ic)
{
    isntype_T	isntype;
    isn_T	*isn;
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;

    RETURN_OK_IF_SKIP(cctx);

    // Get the known type of the two items on the stack.  If they are matching
    // use a type-specific instruction. Otherwise fall back to runtime type
    // checking.
    isntype = get_compare_isn(exprtype, ଖାଲି, ଖାଲି,
		       get_type_on_ଗଦା(cctx, 1), get_type_on_ଗଦା(cctx, 0));
    ଯଦି (isntype == ISN_DROP)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି ((isn = generate_instr(cctx, isntype)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.op.op_type = exprtype;
    isn->isn_arg.op.op_ic = ic;

    // takes two arguments, puts one bool back
    --ଗଦା->ga_len;
    set_type_on_ଗଦା(cctx, &t_bool, 0);

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_CONCAT instruction.
 * "count" is the number of stack elements to join together and it must be
 * greater or equal to one.
 * The caller ensures all the "count" elements on the stack have the right type.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_CONCAT(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    isn_T	*isn;
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;

    RETURN_OK_IF_SKIP(cctx);

    ଯଦି ((isn = generate_instr(cctx, ISN_CONCAT)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = ଗଣନା_2;

    // drop the argument types
    ଗଦା->ga_len -= ଗଣନା_2 - 1;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_2BOOL instruction.
 * "offset" is the offset in the type stack.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_2BOOL(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା invert, ପୂର୍ଣ୍ଣସଂଖ୍ୟା offset)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_2BOOL)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.tobool.invert = invert;
    isn->isn_arg.tobool.offset = offset;

    // type becomes bool
    set_type_on_ଗଦା(cctx, &t_bool, -1 - offset);

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_COND2BOOL instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_COND2BOOL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ଯଦି (generate_instr(cctx, ISN_COND2BOOL) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // type becomes bool
    set_type_on_ଗଦା(cctx, &t_bool, 0);

    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_TYPECHECK(
	cctx_T	    *cctx,
	type_T	    *expected,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    typechk_flags,	// type check flags
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    offset,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_var,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argidx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_CHECKTYPE)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    type_T *tt;
    ଯଦି ((expected->tt_type == VAR_FLOAT
		&& (typechk_flags & TYPECHK_ସଂଖ୍ୟା_OK))
	    || (expected->tt_type == VAR_LIST
		&& (typechk_flags & TYPECHK_TUPLE_OK)))
    {
	// always allocate, also for static types
	tt = ALLOC_ONE(type_T);
	ଯଦି (tt != ଖାଲି)
	{
	    *tt = *expected;
	    tt->tt_flags &= ~TTFLAG_STATIC;
	    ଯଦି (typechk_flags & TYPECHK_ସଂଖ୍ୟା_OK)
		tt->tt_flags |= TTFLAG_ସଂଖ୍ୟା_OK;
	    ଅନ୍ୟଥା
		tt->tt_flags |= TTFLAG_TUPLE_OK;
	}
    }
    ଅନ୍ୟଥା
	tt = alloc_type(expected);

    isn->isn_arg.type.ct_type = tt;
    isn->isn_arg.type.ct_off = (int8_T)offset;
    isn->isn_arg.type.ct_is_var = is_var;
    isn->isn_arg.type.ct_arg_idx = (int8_T)argidx;

    // type becomes expected
    set_type_on_ଗଦା(cctx, expected, -1 - offset);

    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_SETTYPE(
	cctx_T	    *cctx,
	type_T	    *expected)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_SETTYPE)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.type.ct_type = alloc_type(expected);
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PUSHOBJ instruction.  Object is always NULL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHOBJ(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ଯଦି (generate_instr_type(cctx, ISN_PUSHOBJ, &t_object_any) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PUSHCLASS instruction.  "class" can be NULL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHCLASS(cctx_T *cctx, class_T *class)
{
    RETURN_OK_IF_SKIP(cctx);
    isn_T *isn = generate_instr_type(cctx, ISN_PUSHCLASS,
				  class == ଖାଲି ? &t_any : &class->class_type);
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.classarg = class;
    ଯଦି (class != ଖାଲି)
	++class->class_refcount;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate a PUSH instruction for "tv".
 * "tv" will be consumed or cleared.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_tv_ଠେଲ(cctx_T *cctx, typval_T *tv)
{
    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_BOOL:
	    generate_PUSHBOOL(cctx, tv->vval.v_ସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_SPECIAL:
	    generate_PUSHSPEC(cctx, tv->vval.v_ସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    generate_PUSHNR(cctx, tv->vval.v_ସଂଖ୍ୟା);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_FLOAT:
	    generate_PUSHF(cctx, tv->vval.v_float);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:
	    generate_PUSHBLOB(cctx, tv->vval.v_blob);
	    tv->vval.v_blob = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_LIST:
	    ଯଦି (tv->vval.v_list != ଖାଲି)
		iemsg("non-empty list constant not supported");
	    generate_NEWLIST(cctx, 0, TRUE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    ଯଦି (tv->vval.v_tuple != ଖାଲି)
		iemsg("non-empty tuple constant not supported");
	    generate_NEWTUPLE(cctx, 0, TRUE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    ଯଦି (tv->vval.v_dict != ଖାଲି)
		iemsg("non-empty dict constant not supported");
	    generate_NEWDICT(cctx, 0, TRUE);
	    ଭାଙ୍ଗ;
#ifdef FEAT_JOB_CHANNEL
	ସ୍ଥିତି VAR_JOB:
	    ଯଦି (tv->vval.v_job != ଖାଲି)
		iemsg("non-null job constant not supported");
	    generate_PUSHJOB(cctx);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CHANNEL:
	    ଯଦି (tv->vval.v_channel != ଖାଲି)
		iemsg("non-null channel constant not supported");
	    generate_PUSHCHANNEL(cctx);
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_FUNC:
	    ଯଦି (tv->vval.v_string != ଖାଲି)
		iemsg("non-null function constant not supported");
	    generate_PUSHFUNC(cctx, ଖାଲି, &t_func_unknown, TRUE);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_PARTIAL:
	    ଯଦି (tv->vval.v_partial != ଖାଲି)
		iemsg("non-null partial constant not supported");
	    ଯଦି (generate_instr_type(cctx, ISN_NEWPARTIAL, &t_func_unknown)
								   == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_STRING:
	    generate_PUSHS(cctx, &tv->vval.v_string);
	    tv->vval.v_string = ଖାଲି;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_OBJECT:
	    ଯଦି (tv->vval.v_object != ଖାଲି)
	    {
		emsg(_(e_cannot_use_non_null_object));
		ଫେରନ୍ତୁ FAIL;
	    }
	    generate_PUSHOBJ(cctx);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_CLASS:
	    generate_PUSHCLASS(cctx, tv->vval.v_class);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    siemsg("constant type %d not supported", tv->v_type);
	    clear_tv(tv);
	    ଫେରନ୍ତୁ FAIL;
    }
    tv->v_type = VAR_UNKNOWN;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PUSHNR instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHNR(cctx_T *cctx, varnumber_T ସଂଖ୍ୟା)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_type(cctx, ISN_PUSHNR, &t_ସଂଖ୍ୟା)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = ସଂଖ୍ୟା;

    ଯଦି (ସଂଖ୍ୟା == 0 || ସଂଖ୍ୟା == 1)
	// A 0 or 1 number can also be used as a bool.
	set_type_on_ଗଦା(cctx, &t_ସଂଖ୍ୟା_bool, 0);
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PUSHBOOL instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHBOOL(cctx_T *cctx, varnumber_T ସଂଖ୍ୟା)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_type(cctx, ISN_PUSHBOOL, &t_bool)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = ସଂଖ୍ୟା;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PUSHSPEC instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHSPEC(cctx_T *cctx, varnumber_T ସଂଖ୍ୟା)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_type(cctx, ISN_PUSHSPEC,
			     ସଂଖ୍ୟା == VVAL_NULL ? &t_null : &t_none)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = ସଂଖ୍ୟା;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PUSHF instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHF(cctx_T *cctx, float_T fnumber)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_type(cctx, ISN_PUSHF, &t_float)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.fnumber = fnumber;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PUSHS instruction.
 * Consumes "*str".  When freed *str is set to NULL, unless "str" is NULL.
 * Note that if "str" is used in the instruction OK is returned and "*str" is
 * not set to NULL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHS(cctx_T *cctx, char_u **str)
{
    isn_T	*isn;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	ଯଦି ((isn = generate_instr_type(cctx, ISN_PUSHS, &t_string)) == ଖାଲି)
	    ret = FAIL;
	ଅନ୍ୟଥା
	{
	    isn->isn_arg.string = str == ଖାଲି ? ଖାଲି : *str;
	    ଫେରନ୍ତୁ OK;
	}
    }
    ଯଦି (str != ଖାଲି)
	VIM_CLEAR(*str);
    ଫେରନ୍ତୁ ret;
}

/*
 * Generate an ISN_PUSHCHANNEL instruction.  Channel is always NULL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHCHANNEL(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    ଯଦି (generate_instr_type(cctx, ISN_PUSHCHANNEL, &t_channel) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
#ଅନ୍ୟଥା
    emsg(_(e_channel_job_feature_not_available));
    ଫେରନ୍ତୁ FAIL;
#endif
}

/*
 * Generate an ISN_PUSHJOB instruction.  Job is always NULL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHJOB(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
#ifdef FEAT_JOB_CHANNEL
    ଯଦି (generate_instr_type(cctx, ISN_PUSHJOB, &t_job) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
#ଅନ୍ୟଥା
    emsg(_(e_channel_job_feature_not_available));
    ଫେରନ୍ତୁ FAIL;
#endif
}

/*
 * Generate an ISN_PUSHBLOB instruction.
 * Consumes "blob".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHBLOB(cctx_T *cctx, blob_T *blob)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_type2(cctx, ISN_PUSHBLOB,
					&t_blob, &t_blob)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.blob = blob;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PUSHFUNC instruction with name "name".
 * When "may_prefix" is TRUE prefix "g:" unless "name" is script-local or
 * autoload.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUSHFUNC(cctx_T *cctx, char_u *name, type_T *type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା may_prefix)
{
    isn_T	*isn;
    char_u	*funcname;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_type(cctx, ISN_PUSHFUNC, type)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (name == ଖାଲି)
	funcname = ଖାଲି;
    ଅନ୍ୟଥା ଯଦି (!may_prefix
	    || *name == K_SPECIAL			    // script-local
	    || vim_strchr(name, AUTOLOAD_CHAR) != ଖାଲି)	    // autoload
	funcname = vim_strsave(name);
    ଅନ୍ୟଥା
    {
	funcname = alloc(STRLEN(name) + 3);
	ଯଦି (funcname != ଖାଲି)
	{
	    STRCPY(funcname, "g:");
	    STRCPY(funcname + 2, name);
	}
    }

    isn->isn_arg.string = funcname;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_AUTOLOAD instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_AUTOLOAD(cctx_T *cctx, char_u *name, type_T *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_type(cctx, ISN_AUTOLOAD, type)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.string = vim_strsave(name);
    ଯଦି (isn->isn_arg.string == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_GETITEM instruction with "index".
 * "with_op" is TRUE for "+=" and other operators, the stack has the current
 * value below the list with values.
 * Caller must check the type is a list.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_GETITEM(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା index, ପୂର୍ଣ୍ଣସଂଖ୍ୟା with_op)
{
    isn_T	*isn;
    type_T	*type = get_type_on_ଗଦା(cctx, with_op ? 1 : 0);
    type_T	*item_type = &t_any;

    RETURN_OK_IF_SKIP(cctx);

    item_type = get_item_type(type);
    ଯଦି ((isn = generate_instr(cctx, ISN_GETITEM)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.getitem.gi_index = index;
    isn->isn_arg.getitem.gi_with_op = with_op;

    // add the item type to the type stack
    ଫେରନ୍ତୁ ଠେଲ_type_ଗଦା(cctx, item_type);
}

/*
 * Set the type of the tuple that is left after dropping the first "count"
 * items of the tuple with type "type".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_tuple_slice_type_on_ଗଦା(type_T *type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, cctx_T *cctx)
{
    garray_T	tuple_types_ga;

    // The item types are only known for a tuple with a fixed number of items.
    ଯଦି ((type->tt_flags & TTFLAG_VARARGS) || type->tt_argcount <= ଗଣନା_2)
    {
	set_type_on_ଗଦା(cctx, &t_tuple_any, 0);
	ଫେରନ୍ତୁ;
    }

    ga_init2(&tuple_types_ga, ଆକାର(type_T *), 10);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = ଗଣନା_2; i < type->tt_argcount; i++)
    {
	ଯଦି (ga_grow(&tuple_types_ga, 1) == FAIL)
	{
	    ga_clear(&tuple_types_ga);
	    set_type_on_ଗଦା(cctx, &t_tuple_any, 0);
	    ଫେରନ୍ତୁ;
	}
	((type_T **)tuple_types_ga.ga_data)[tuple_types_ga.ga_len] =
							     type->tt_args[i];
	tuple_types_ga.ga_len++;
    }

    set_type_on_ଗଦା(cctx,
		get_tuple_type(&tuple_types_ga, cctx->ctx_type_list), 0);

    ga_clear(&tuple_types_ga);
}

/*
 * Generate an ISN_SLICE instruction with "count".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_SLICE(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_SLICE)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = ଗଣନା_2;

    // Slicing a tuple leaves a tuple with the remaining item types, not the
    // type of the whole tuple.
    type = get_type_on_ଗଦା(cctx, 0);
    ଯଦି (type->tt_type == VAR_TUPLE)
	set_tuple_slice_type_on_ଗଦା(type, ଗଣନା_2, cctx);

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_CHECKLEN instruction with "min_len".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_CHECKLEN(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା more_OK)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);

    ଯଦି ((isn = generate_instr(cctx, ISN_CHECKLEN)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.checklen.cl_min_len = min_len;
    isn->isn_arg.checklen.cl_more_OK = more_OK;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_STORE instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_STORE(cctx_T *cctx, isntype_T isn_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, char_u *name)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_drop(cctx, isn_type, 1)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (name != ଖାଲି)
	isn->isn_arg.string = vim_strsave(name);
    ଅନ୍ୟଥା
	isn->isn_arg.ସଂଖ୍ୟା = idx;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_LOAD_CLASSMEMBER ("load" == TRUE) or ISN_STORE_CLASSMEMBER
 * ("load" == FALSE) instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_CLASSMEMBER(
	cctx_T	    *cctx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    load,
	class_T	    *cl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି (load)
    {
	ocmember_T *m = &cl->class_class_members[idx];
	isn = generate_instr_type(cctx, ISN_LOAD_CLASSMEMBER, m->ocm_type);
    }
    ଅନ୍ୟଥା
    {
	isn = generate_instr_drop(cctx, ISN_STORE_CLASSMEMBER, 1);
    }
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.classmember.cm_class = cl;
    ++cl->class_refcount;
    isn->isn_arg.classmember.cm_idx = idx;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_STOREOUTER instruction.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_STOREOUTER(
	cctx_T	    *cctx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    level,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    loop_depth,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_drop(cctx, ISN_STOREOUTER, 1)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (level == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Store a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    ଅନ୍ୟଥା
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = level;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_STORENR instruction (short for ISN_PUSHNR + ISN_STORE)
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_STORENR(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, varnumber_T ମୂଲ୍ୟ)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_STORENR)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.storenr.stnr_idx = idx;
    isn->isn_arg.storenr.stnr_val = ମୂଲ୍ୟ;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_STOREOPT or ISN_STOREFUNCOPT instruction
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_STOREOPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    opt_flags)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_drop(cctx, isn_type, 1)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.storeopt.so_name = vim_strsave(name);
    isn->isn_arg.storeopt.so_flags = opt_flags;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_LOAD or similar instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_LOAD(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx,
	char_u	    *name,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_type2(cctx, isn_type, type, type)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (name != ଖାଲି)
	isn->isn_arg.string = vim_strsave(name);
    ଅନ୍ୟଥା
	isn->isn_arg.ସଂଖ୍ୟା = idx;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_LOADOUTER instruction
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_LOADOUTER(
	cctx_T	    *cctx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    nesting,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    loop_depth,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    loop_idx,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_type2(cctx, ISN_LOADOUTER, type, type)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (nesting == 1 && loop_idx >= 0 && idx >= loop_idx)
    {
	// Load a variable defined in a loop.  A copy will be made at the end
	// of the loop.
	isn->isn_arg.outer.outer_idx = idx - loop_idx;
	isn->isn_arg.outer.outer_depth = -loop_depth - 1;
    }
    ଅନ୍ୟଥା
    {
	isn->isn_arg.outer.outer_idx = idx;
	isn->isn_arg.outer.outer_depth = nesting;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_LOADV instruction for v:var.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_LOADV(
	cctx_T	    *cctx,
	char_u	    *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    di_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    vidx = find_vim_var(name, &di_flags);
    type_T  *type;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି (vidx < 0)
    {
	semsg(_(e_variable_not_found_str), name);
	ଫେରନ୍ତୁ FAIL;
    }
    type = get_vim_var_type(vidx, cctx->ctx_type_list);
    ଫେରନ୍ତୁ generate_LOAD(cctx, ISN_LOADV, vidx, ଖାଲି, type);
}

/*
 * Generate an ISN_UNLET instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_UNLET(cctx_T *cctx, isntype_T isn_type, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା forceit)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, isn_type)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.unlet.ul_name = vim_strsave(name);
    isn->isn_arg.unlet.ul_forceit = forceit;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_LOCKCONST instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_LOCKCONST(cctx_T *cctx)
{
    RETURN_OK_IF_SKIP(cctx);
    ଯଦି (generate_instr(cctx, ISN_LOCKCONST) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_LOADS instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_OLDSCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sid,
	type_T	    *type)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି (isn_type == ISN_LOADS || isn_type == ISN_LOADEXPORT)
	isn = generate_instr_type(cctx, isn_type, type);
    ଅନ୍ୟଥା
	isn = generate_instr_drop(cctx, isn_type, 1);
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.loadstore.ls_name = vim_strsave(name);
    isn->isn_arg.loadstore.ls_sid = sid;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_LOADSCRIPT or ISN_STORESCRIPT instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_VIM9SCRIPT(
	cctx_T	    *cctx,
	isntype_T   isn_type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sid,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx,
	type_T	    *type)
{
    isn_T	*isn;
    scriptref_T	*sref;
    scriptitem_T *si = SCRIPT_ITEM(sid);

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି (isn_type == ISN_LOADSCRIPT)
	isn = generate_instr_type2(cctx, isn_type, type, type);
    ଅନ୍ୟଥା
	isn = generate_instr_drop(cctx, isn_type, 1);
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    // This requires three arguments, which doesn't fit in an instruction, thus
    // we need to allocate a struct for this.
    sref = ALLOC_ONE(scriptref_T);
    ଯଦି (sref == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.script.scriptref = sref;
    sref->sref_sid = sid;
    sref->sref_idx = idx;
    sref->sref_seq = si->sn_script_seq;
    sref->sref_type = type;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_NEWLIST instruction for "count" items.
 * "use_null" is TRUE for null_list.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_NEWLIST(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_NEWLIST)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = use_null ? -1 : ଗଣନା_2;

    // Get the member type and the declared member type from all the items on
    // the stack.
    ଯଦି ((member_type = get_member_type_from_ଗଦା(ଗଣନା_2, 1, cctx)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    type = get_list_type(member_type, cctx->ctx_type_list);
    decl_type = get_list_type(&t_any, cctx->ctx_type_list);

    // drop the value types
    cctx->ctx_type_ଗଦା.ga_len -= ଗଣନା_2;

    // add the list type to the type stack
    ଫେରନ୍ତୁ ଠେଲ_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWTUPLE instruction for "count" items.
 * "use_null" is TRUE for null_tuple.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_NEWTUPLE(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_null)
{
    isn_T	*isn;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_NEWTUPLE)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = use_null ? -1 : ଗଣନା_2;

    // Get the member type and the declared member type from all the items on
    // the stack.
    garray_T	tuple_types_ga;
    ga_init2(&tuple_types_ga, ଆକାର(type_T *), 10);

    ଯଦି (get_tuple_type_from_ଗଦା(ଗଣନା_2, &tuple_types_ga, cctx) < 0)
    {
	ga_clear(&tuple_types_ga);
	ଫେରନ୍ତୁ FAIL;
    }

    type = get_tuple_type(&tuple_types_ga, cctx->ctx_type_list);
    decl_type = &t_tuple_any;

    ga_clear(&tuple_types_ga);

    // drop the value types
    cctx->ctx_type_ଗଦା.ga_len -= ଗଣନା_2;

    // add the tuple type to the type stack
    ଫେରନ୍ତୁ ଠେଲ_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_NEWDICT instruction.
 * "use_null" is TRUE for null_dict.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_NEWDICT(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_null)
{
    isn_T	*isn;
    type_T	*member_type;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_NEWDICT)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = use_null ? -1 : ଗଣନା_2;

    ଯଦି ((member_type = get_member_type_from_ଗଦା(ଗଣନା_2, 2, cctx)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    type = get_dict_type(member_type, cctx->ctx_type_list);
    decl_type = get_dict_type(&t_any, cctx->ctx_type_list);

    // drop the key and value types
    cctx->ctx_type_ଗଦା.ga_len -= 2 * ଗଣନା_2;

    // add the dict type to the type stack
    ଫେରନ୍ତୁ ଠେଲ_type_stack2(cctx, type, decl_type);
}

/*
 * Generate an ISN_FUNCREF instruction.
 * For "obj.Method" "cl" is the class of the object (can be an interface or a
 * base class) and "fi" the index of the method on that class.
 * "isn_idx" is set to the index of the instruction, so that fr_dfunc_idx can
 * be set later.  The index is used instead of a pointer to the instruction
 * because the instruction memory can be reallocated.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_FUNCREF(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    object_method,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    fi,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    *isn_idx)
{
    isn_T	    *isn;
    type_T	    *type;
    funcref_extra_T *extra;
    loopvarinfo_T   loopinfo;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    has_vars;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_FUNCREF)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (isn_idx != ଖାଲି)
	// save the index of the new instruction
	*isn_idx = cctx->ctx_instr.ga_len - 1;

    has_vars = get_loop_var_info(cctx, &loopinfo);
    ଯଦି (ufunc->uf_def_status == UF_NOT_COMPILED || has_vars || cl != ଖାଲି)
    {
	extra = ALLOC_CLEAR_ONE(funcref_extra_T);
	ଯଦି (extra == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	isn->isn_arg.funcref.fr_extra = extra;
	extra->fre_loopvar_info = loopinfo;
	ଯଦି (cl != ଖାଲି)
	{
	    extra->fre_class = cl;
	    ++cl->class_refcount;
	    extra->fre_object_method = object_method;
	    extra->fre_method_idx = fi;
	}
    }
    ଯଦି (ufunc->uf_def_status == UF_NOT_COMPILED || cl != ଖାଲି)
	extra->fre_func_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
    ଯଦି (ufunc->uf_def_status != UF_NOT_COMPILED && cl == ଖାଲି)
    {
	ଯଦି (isn_idx == ଖାଲି && ufunc->uf_def_status == UF_TO_BE_COMPILED)
	    // compile the function now, we need the uf_dfunc_idx value
	    (ଶୂନ୍ୟ)compile_def_function(ufunc, FALSE, CT_NONE, ଖାଲି);
	isn->isn_arg.funcref.fr_dfunc_idx = ufunc->uf_dfunc_idx;
    }

    // Reserve an extra variable to keep track of the number of closures
    // created.
    cctx->ctx_has_closure = 1;

    // If the referenced function is a closure, it may use items further up in
    // the nested context, including this one.  But not a function defined at
    // the script level.
    ଯଦି ((ufunc->uf_flags & FC_CLOSURE)
			       && func_name_refcount(cctx->ctx_ufunc->uf_name))
	cctx->ctx_ufunc->uf_flags |= FC_CLOSURE;

    type = ufunc->uf_func_type == ଖାଲି ? &t_func_any : ufunc->uf_func_type;
    ଫେରନ୍ତୁ ଠେଲ_type_ଗଦା(cctx, type);
}

/*
 * Generate an ISN_NEWFUNC instruction.
 * "lambda_name" and "func_name" must be in allocated memory and will be
 * consumed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_NEWFUNC(
	cctx_T	*cctx,
	char_u	*lambda_name,
	char_u	*func_name)
{
    isn_T	*isn;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	ଯଦି ((isn = generate_instr(cctx, ISN_NEWFUNC)) == ଖାଲି)
	    ret = FAIL;
	ଅନ୍ୟଥା
	{
	    newfuncarg_T *arg = ALLOC_CLEAR_ONE(newfuncarg_T);

	    ଯଦି (arg == ଖାଲି)
		ret = FAIL;
	    ଅନ୍ୟଥା
	    {
		// Reserve an extra variable to keep track of the number of
		// closures created.
		cctx->ctx_has_closure = 1;

		isn->isn_arg.newfunc.nf_arg = arg;
		arg->nfa_lambda = lambda_name;
		arg->nfa_global = func_name;
		(ଶୂନ୍ୟ)get_loop_var_info(cctx, &arg->nfa_loopvar_info);
		ଫେରନ୍ତୁ OK;
	    }
	}
    }
    vim_free(lambda_name);
    vim_free(func_name);
    ଫେରନ୍ତୁ ret;
}

/*
 * Generate an ISN_DEF instruction: list functions
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_DEF(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_DEF)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (len > 0)
    {
	isn->isn_arg.string = vim_strnsave(name, len);
	ଯଦି (isn->isn_arg.string == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_JUMP instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_JUMP(cctx_T *cctx, jumpwhen_T when, ପୂର୍ଣ୍ଣସଂଖ୍ୟା where)
{
    isn_T	*isn;
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_JUMP)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.jump.jump_when = when;
    isn->isn_arg.jump.jump_where = where;

    ଯଦି (when != JUMP_ALWAYS && ଗଦା->ga_len > 0)
	--ଗଦା->ga_len;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_WHILE instruction.  Similar to ISN_JUMP for :while
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_WHILE(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା funcref_idx)
{
    isn_T	*isn;
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_WHILE)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.whileloop.while_funcref_idx = funcref_idx;
    isn->isn_arg.whileloop.while_end = 0;  // filled in later

    ଯଦି (ଗଦା->ga_len > 0)
	--ଗଦା->ga_len;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_JUMP_IF_ARG_SET or ISN_JUMP_IF_ARG_NOT_SET instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_JUMP_IF_ARG(cctx_T *cctx, isntype_T isn_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_off)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, isn_type)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.jumparg.jump_arg_off = arg_off;
    // jump_where is set later
    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_FOR(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା loop_idx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_FOR)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.forloop.for_loop_idx = loop_idx;

    // type doesn't matter, will be stored next
    ଫେରନ୍ତୁ ଠେଲ_type_ଗଦା(cctx, &t_any);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_ENDLOOP(cctx_T *cctx, loop_info_T *loop_info)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_ENDLOOP)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.endloop.end_depth = loop_info->li_depth;
    isn->isn_arg.endloop.end_funcref_idx = loop_info->li_funcref_idx;
    isn->isn_arg.endloop.end_var_idx = loop_info->li_local_ଗଣନା;
    isn->isn_arg.endloop.end_var_ଗଣନା =
			   cctx->ctx_locals.ga_len - loop_info->li_local_ଗଣନା;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_TRYCONT instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_TRYCONT(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା levels, ପୂର୍ଣ୍ଣସଂଖ୍ୟା where)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_TRYCONT)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.trycont.tct_levels = levels;
    isn->isn_arg.trycont.tct_where = where;

    ଫେରନ୍ତୁ OK;
}

/*
 * Check "argount" arguments and their types on the type stack.
 * Give an error and return FAIL if something is wrong.
 * When "method_call" is NULL no code is generated.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_internal_func_args(
	cctx_T	*cctx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	func_idx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	argcount,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	method_call,
	type2_T **argtypes,
	type2_T *shuffled_argtypes)
{
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argoff = check_internal_func(func_idx, argcount);

    ଯଦି (argoff < 0)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (method_call && argoff > 1)
    {
	ଯଦି (argcount < argoff)
	{
	    semsg(_(e_not_enough_arguments_for_function_str),
						 internal_func_name(func_idx));
	    ଫେରନ୍ତୁ FAIL;
	}

	isn_T	*isn = generate_instr(cctx, ISN_SHUFFLE);
	ଯଦି (isn  == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	isn->isn_arg.shuffle.shfl_item = argcount;
	isn->isn_arg.shuffle.shfl_up = argoff - 1;
    }

    ଯଦି (argcount > 0)
    {
	type2_T	*typep = ((type2_T *)ଗଦା->ga_data) + ଗଦା->ga_len - argcount;

	// Check the types of the arguments.
	ଯଦି (method_call && argoff > 1)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    ପାଇଁ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = (i < argoff - 1)
				    ? typep[i + 1]
				    : (i == argoff - 1) ? typep[0] : typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    ପାଇଁ (i = 0; i < argcount; ++i)
		shuffled_argtypes[i] = typep[i];
	    *argtypes = shuffled_argtypes;
	}
	ଯଦି (internal_func_check_arg_types(*argtypes, func_idx, argcount,
								 cctx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_BCALL instruction.
 * "method_call" is TRUE for "value->method()"
 * Return FAIL if the number of arguments is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_BCALL(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା func_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, ପୂର୍ଣ୍ଣସଂଖ୍ୟା method_call)
{
    isn_T	*isn;
    garray_T	*ଗଦା = &cctx->ctx_type_ଗଦା;
    type2_T	*argtypes = ଖାଲି;
    type2_T	shuffled_argtypes[MAX_FUNC_ARGS];
    type2_T	*maptype = ଖାଲି;
    type_T	*type;
    type_T	*decl_type;

    RETURN_OK_IF_SKIP(cctx);

    ଯଦି (check_internal_func_args(cctx, func_idx, argcount, method_call,
					 &argtypes, shuffled_argtypes) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (internal_func_is_map(func_idx))
	maptype = argtypes;

    ଯଦି ((isn = generate_instr(cctx, ISN_BCALL)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.bfunc.cbf_idx = func_idx;
    isn->isn_arg.bfunc.cbf_argcount = argcount;

    // Drop the argument types and push the return type.
    ଗଦା->ga_len -= argcount;
    type = internal_func_ret_type(func_idx, argcount, argtypes, &decl_type,
							  cctx->ctx_type_list);
    ଯଦି (ଠେଲ_type_stack2(cctx, type, decl_type) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (maptype != ଖାଲି && maptype[0].type_decl->tt_member != ଖାଲି
				  && maptype[0].type_decl->tt_member != &t_any)
	// Check that map() didn't change the item types.
	generate_TYPECHECK(cctx, maptype[0].type_decl, 0, -1, FALSE, 1);

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_LISTAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_LISTAPPEND(cctx_T *cctx)
{
    type_T	*list_type;
    type_T	*item_type;
    type_T	*expected;

    // Caller already checked that list_type is a list.
    // For checking the item type we use the declared type of the list and the
    // current type of the added item, adding a string to [1, 2] is OK.
    list_type = get_decl_type_on_ଗଦା(cctx, 1);
    ଯଦି (arg_type_modifiable(list_type, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    item_type = get_type_on_ଗଦା(cctx, 0);
    expected = list_type->tt_member;
    ଯଦି (need_type(item_type, expected, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (generate_instr(cctx, ISN_LISTAPPEND) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    --cctx->ctx_type_ଗଦା.ga_len;	    // drop the argument
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_BLOBAPPEND instruction.  Works like add().
 * Argument count is already checked.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_BLOBAPPEND(cctx_T *cctx)
{
    type_T	*item_type;

    // Caller already checked that blob_type is a blob, check it is modifiable.
    ଯଦି (arg_type_modifiable(get_decl_type_on_ଗଦା(cctx, 1), 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    item_type = get_type_on_ଗଦା(cctx, 0);
    ଯଦି (need_type(item_type, &t_ସଂଖ୍ୟା, 0, -1, 0, cctx, FALSE, FALSE) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (generate_instr(cctx, ISN_BLOBAPPEND) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    --cctx->ctx_type_ଗଦା.ga_len;	    // drop the argument
    ଫେରନ୍ତୁ OK;
}

/*
 * Get the instruction type for a function call:
 *   ISN_METHODCALL - object method call via interface
 *   ISN_DCALL      - compiled def function call
 *   ISN_UCALL      - uncompiled function, or
 *                    compiled script-local function called from a lambda
 */
    ସ୍ଥିର isntype_T
isn_get_calltype(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl)
{
    ଯଦି (cl != ଖାଲି)
	ଫେରନ୍ତୁ ISN_METHODCALL;

    ଯଦି (ufunc->uf_def_status == UF_NOT_COMPILED)
	ଫେରନ୍ତୁ ISN_UCALL;

    // function invoked from a lambda
    ଯଦି (cctx->ctx_ufunc->uf_flags & FC_LAMBDA)
    {
	// Script-local funcs in a lambda may be redefined by re-sourcing;
	// resolve by name at runtime.  Patched to ISN_DCALL on next call once
	// recompiled.
	ଯଦି (ufunc->uf_name[0] == K_SPECIAL)
	    ଫେରନ୍ତୁ ISN_UCALL;
    }

    ଫେରନ୍ତୁ ISN_DCALL;
}

/*
 * Generate an ISN_DCALL, ISN_UCALL or ISN_METHODCALL instruction.
 * When calling a method on an object, of which we know the interface only,
 * then "cl" is the interface and "mi" the method index on the interface.
 * save is_super in the "isn->isn_arg"; it flags execution to use mfunc
 * directly to determine ufunc.
 * Return FAIL if the number of arguments is wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_CALL(
	cctx_T	    *cctx,
	ufunc_T	    *ufunc,
	class_T	    *cl,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    mi,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    pushed_argcount,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    is_super)
{
    isn_T	*isn;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regular_args = ufunc->uf_args.ga_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = pushed_argcount;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି (argcount > regular_args && !has_varargs(ufunc))
    {
	semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (argcount < regular_args - ufunc->uf_def_args.ga_len)
    {
	semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (ufunc->uf_def_status != UF_NOT_COMPILED
	    && ufunc->uf_def_status != UF_COMPILE_ERROR)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
	compiletype_T	compile_type;

	ପାଇଁ (i = 0; i < argcount; ++i)
	{
	    type_T *expected;
	    type_T *actual;

	    actual = get_type_on_ଗଦା(cctx, argcount - i - 1);
	    ଯଦି (check_type_is_ମୂଲ୍ୟ(actual) == FAIL)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (actual->tt_type == VAR_SPECIAL
			      && i >= regular_args - ufunc->uf_def_args.ga_len)
	    {
		// assume v:none used for default argument value
		ଜାରି;
	    }
	    ଯଦି (i < regular_args)
	    {
		ଯଦି (ufunc->uf_arg_types == ଖାଲି)
		    ଜାରି;
		expected = ufunc->uf_arg_types[i];
	    }
	    ଅନ୍ୟଥା ଯଦି (ufunc->uf_va_type == ଖାଲି
					   || ufunc->uf_va_type == &t_list_any)
		// possibly a lambda or "...: any"
		expected = &t_any;
	    ଅନ୍ୟଥା
		expected = ufunc->uf_va_type->tt_member;
	    ଯଦି (need_type(actual, expected, 0,
			      -argcount + i, i + 1, cctx, TRUE, FALSE) == FAIL)
	    {
		arg_type_mismatch(expected, actual, i + 1);
		ଫେରନ୍ତୁ FAIL;
	    }
	}
	compile_type = get_compile_type(ufunc);
	ଯଦି (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, ufunc->uf_ret_type == ଖାଲି,
						   compile_type, ଖାଲି) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (ufunc->uf_def_status == UF_COMPILE_ERROR)
    {
	emsg_funcname(e_call_to_function_that_failed_to_compile_str,
							       ufunc->uf_name);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି ((isn = generate_instr(cctx, isn_get_calltype(cctx, ufunc, cl))) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (cl != ଖାଲି /* isn->isn_type == ISN_METHODCALL */)
    {
	isn->isn_arg.mfunc = ALLOC_ONE(cmfunc_T);
	ଯଦି (isn->isn_arg.mfunc == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	isn->isn_arg.mfunc->cmf_itf = cl;
	++cl->class_refcount;
	isn->isn_arg.mfunc->cmf_idx = mi;
	isn->isn_arg.mfunc->cmf_argcount = argcount;
	isn->isn_arg.mfunc->cmf_is_super = is_super;
	// For a generic method "ufunc" has the type arguments, the method of
	// the object class is created with the same type arguments when
	// called.
	isn->isn_arg.mfunc->cmf_ufunc = ଖାଲି;
	ଯଦି (ufunc != cl->class_obj_methods[mi])
	    isn->isn_arg.mfunc->cmf_ufunc = ufunc;
	isn->isn_arg.mfunc->cmf_cache_cl = ଖାଲି;
	isn->isn_arg.mfunc->cmf_cache_generic = ଖାଲି;
	isn->isn_arg.mfunc->cmf_cache_ufunc = ଖାଲି;
    }
    ଅନ୍ୟଥା ଯଦି (isn->isn_type == ISN_DCALL)
    {
	isn->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	isn->isn_arg.dfunc.cdf_argcount = argcount;
    }
    ଅନ୍ୟଥା
    {
	// A user function may be deleted and redefined later, can't use the
	// ufunc pointer, need to look it up again at runtime.
	isn->isn_arg.ufunc.cuf_name = vim_strnsave(ufunc->uf_name, ufunc->uf_namelen);
	isn->isn_arg.ufunc.cuf_argcount = argcount;
    }

    // drop the argument types
    cctx->ctx_type_ଗଦା.ga_len -= argcount;

    // For an object or class method call, drop the object/class type.
    ଯଦି (ufunc->uf_class != ଖାଲି)
    {
	// When a class method is called without the class name prefix, then
	// the type will not be in the stack.
	type_T *stype = get_type_on_ଗଦା(cctx, 0);
	ଯଦି (stype->tt_type == VAR_CLASS || stype->tt_type == VAR_OBJECT)
	    cctx->ctx_type_ଗଦା.ga_len--;
    }

    // add return type
    ଫେରନ୍ତୁ ଠେଲ_type_ଗଦା(cctx, ufunc->uf_ret_type);
}

/*
 * Generate an ISN_UCALL instruction when the function isn't defined yet.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_UCALL(cctx_T *cctx, char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_UCALL)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ufunc.cuf_name = vim_strsave(name);
    isn->isn_arg.ufunc.cuf_argcount = argcount;

    // drop the argument types
    cctx->ctx_type_ଗଦା.ga_len -= argcount;

    // add return value
    ଫେରନ୍ତୁ ଠେଲ_type_ଗଦା(cctx, &t_any);
}

/*
 * Check the arguments of function "type" against the types on the stack.
 * Returns OK or FAIL;
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_func_args_from_type(
	cctx_T	*cctx,
	type_T	*type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	argcount,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	at_ଶୀର୍ଷ,
	char_u	*name)
{
    ଯଦି (type->tt_argcount != -1)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    varargs = (type->tt_flags & TTFLAG_VARARGS) ? 1 : 0;

	ଯଦି (argcount < type->tt_min_argcount - varargs)
	{
	    emsg_funcname(e_not_enough_arguments_for_function_str, name);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (!varargs && argcount > type->tt_argcount)
	{
	    emsg_funcname(e_too_many_arguments_for_function_str, name);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (type->tt_args != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    ପାଇଁ (i = 0; i < argcount; ++i)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	offset = -argcount + i - (at_ଶୀର୍ଷ ? 0 : 1);
		type_T	*actual = get_type_on_ଗଦା(cctx, -1 - offset);
		type_T	*expected;

		ଯଦି (check_type_is_ମୂଲ୍ୟ(actual) == FAIL)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (varargs && i >= type->tt_argcount - 1)
		{
		    expected = type->tt_args[type->tt_argcount - 1];
		    ଯଦି (expected != ଖାଲି && expected->tt_type == VAR_LIST)
			expected = expected->tt_member;
		    ଯଦି (expected == ଖାଲି)
			expected = &t_any;
		}
		ଅନ୍ୟଥା ଯଦି (i >= type->tt_min_argcount
					     && actual->tt_type == VAR_SPECIAL)
		    expected = &t_any;
		ଅନ୍ୟଥା
		    expected = type->tt_args[i];
		ଯଦି (need_type(actual, expected, 0,
				     offset, i + 1, cctx, TRUE, FALSE) == FAIL)
		{
		    arg_type_mismatch(expected, actual, i + 1);
		    ଫେରନ୍ତୁ FAIL;
		}
	    }
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PCALL instruction.
 * "type" is the type of the FuncRef.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PCALL(
	cctx_T	*cctx,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	argcount,
	char_u	*name,
	type_T	*type,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	at_ଶୀର୍ଷ)
{
    isn_T	*isn;
    type_T	*ret_type;

    RETURN_OK_IF_SKIP(cctx);

    ଯଦି (type->tt_type == VAR_ANY || type->tt_type == VAR_UNKNOWN
						|| type == &t_object_any)
	ret_type = &t_any;
    ଅନ୍ୟଥା ଯଦି (type->tt_type == VAR_FUNC || type->tt_type == VAR_PARTIAL)
    {
	ଯଦି (check_func_args_from_type(cctx, type, argcount, at_ଶୀର୍ଷ, name)
								       == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ret_type = type->tt_member;
	ଯଦି (ret_type == &t_unknown)
	    // return type not known yet, use a runtime check
	    ret_type = &t_any;
    }
    ଅନ୍ୟଥା
    {
	semsg(_(e_not_callable_type_str), name);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି ((isn = generate_instr(cctx, ISN_PCALL)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.pfunc.cpf_ଶୀର୍ଷ = at_ଶୀର୍ଷ;
    isn->isn_arg.pfunc.cpf_argcount = argcount;

    // drop the arguments and the funcref/partial
    cctx->ctx_type_ଗଦା.ga_len -= argcount + 1;

    // push the return value
    ଠେଲ_type_ଗଦା(cctx, ret_type);

    // If partial is above the arguments it must be cleared and replaced with
    // the return value.
    ଯଦି (at_ଶୀର୍ଷ && generate_instr(cctx, ISN_PCALL_END) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_DEFER instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_DEFER(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount)
{
    isn_T *isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_drop(cctx, ISN_DEFER, argcount + 1)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.defer.defer_var_idx = var_idx;
    isn->isn_arg.defer.defer_argcount = argcount;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_STRINGMEMBER instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_STRINGMEMBER(cctx_T *cctx, char_u *name, size_t len)
{
    isn_T	*isn;
    type_T	*type;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_STRINGMEMBER)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.string = vim_strnsave(name, len);

    // check for dict type
    type = get_type_on_ଗଦା(cctx, 0);
    ଯଦି (type->tt_type != VAR_DICT
	    && type->tt_type != VAR_OBJECT
	    && type->tt_type != VAR_ANY
	    && type->tt_type != VAR_UNKNOWN)
    {
	ଅକ୍ଷର *tofree;

	semsg(_(e_expected_dictionary_for_using_ଚାବି_str_but_got_str),
					       name, type_name(type, &tofree));
	vim_free(tofree);
	ଫେରନ୍ତୁ FAIL;
    }
    // change dict type to dict member type
    ଯଦି (type->tt_type == VAR_DICT)
    {
	type_T *ntype = type->tt_member->tt_type == VAR_UNKNOWN
						    ? &t_any : type->tt_member;
	set_type_on_ଗଦା(cctx, ntype, 0);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_ECHO instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_ECHO(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା with_white, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_drop(cctx, ISN_ECHO, ଗଣନା_2)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.echo.echo_with_white = with_white;
    isn->isn_arg.echo.echo_ଗଣନା = ଗଣନା_2;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_EXECUTE/ISN_ECHOMSG/ISN_ECHOERR instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_MULT_ପଦସମଷ୍ଟି(cctx_T *cctx, isntype_T isn_type, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr_drop(cctx, isn_type, ଗଣନା_2)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = ଗଣନା_2;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_ECHOWINDOW instruction
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_ECHOWINDOW(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, long time)
{
    isn_T	*isn;

    ଯଦି ((isn = generate_instr_drop(cctx, ISN_ECHOWINDOW, ଗଣନା_2)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.echowin.ewin_ଗଣନା = ଗଣନା_2;
    isn->isn_arg.echowin.ewin_time = time;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_SOURCE instruction.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_SOURCE(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid)
{
    isn_T	*isn;

    ଯଦି ((isn = generate_instr(cctx, ISN_SOURCE)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = sid;

    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an ISN_PUT or ISN_IPUT instruction depending on fixindent.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_PUT(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା regname, linenr_T lnum, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixindent)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    isn = (fixindent) ? generate_instr(cctx, ISN_IPUT) :
			generate_instr(cctx, ISN_PUT);
    ଯଦି (isn == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.put.put_regname = regname;
    isn->isn_arg.put.put_lnum = lnum;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate a LOCKUNLOCK instruction.The root item, where the indexing starts
 * to find the variable, is on the stack. The instr takes
 * - the string to parse, "root.b[idx1][idx2].d.val", to find the variable
 * - the class, if any, in which the string executes.
 * - if the root item is a function argument
 * A copy is made of "line".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_LOCKUNLOCK(cctx_T *cctx, char_u *ଧାଡ଼ି, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_arg)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_LOCKUNLOCK)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    class_T *cl = cctx->ctx_ufunc != ଖାଲି ? cctx->ctx_ufunc->uf_class : ଖାଲି;
    isn->isn_arg.lockunlock.lu_string = vim_strsave(ଧାଡ଼ି);
    isn->isn_arg.lockunlock.lu_cl_exec = cl;
    ଯଦି (cl != ଖାଲି)
	++cl->class_refcount;
    isn->isn_arg.lockunlock.lu_is_arg = is_arg;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * A copy is made of "line".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_EXEC_copy(cctx_T *cctx, isntype_T isntype, char_u *ଧାଡ଼ି)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, isntype)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.string = vim_strsave(ଧାଡ଼ି);
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an EXEC instruction that takes a string argument.
 * "str" must be allocated, it is consumed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_EXEC(cctx_T *cctx, isntype_T isntype, char_u *str)
{
    isn_T	*isn;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = OK;

    ଯଦି (cctx->ctx_skip != SKIP_YES)
    {
	ଯଦି ((isn = generate_instr(cctx, isntype)) == ଖାଲି)
	    ret = FAIL;
	ଅନ୍ୟଥା
	{
	    isn->isn_arg.string = str;
	    ଫେରନ୍ତୁ OK;
	}
    }
    vim_free(str);
    ଫେରନ୍ତୁ ret;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_LEGACY_EVAL(cctx_T *cctx, char_u *ଧାଡ଼ି)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_LEGACY_EVAL)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.string = vim_strsave(ଧାଡ଼ି);

    ଫେରନ୍ତୁ ଠେଲ_type_ଗଦା(cctx, &t_any);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_EXECCONCAT(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    isn_T	*isn;

    ଯଦି ((isn = generate_instr_drop(cctx, ISN_EXECCONCAT, ଗଣନା_2)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.ସଂଖ୍ୟା = ଗଣନା_2;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate ISN_RANGE.  Consumes "range".  Return OK/FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_RANGE(cctx_T *cctx, char_u *range)
{
    isn_T	*isn;

    ଯଦି ((isn = generate_instr(cctx, ISN_RANGE)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.string = range;

    ଫେରନ୍ତୁ ଠେଲ_type_ଗଦା(cctx, &t_ସଂଖ୍ୟା);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_UNPACK(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_ଗଣନା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା semicolon)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_UNPACK)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.unpack.unp_ଗଣନା = var_ଗଣନା;
    isn->isn_arg.unpack.unp_semicolon = semicolon;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate an instruction for any command modifiers.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_cmdmods(cctx_T *cctx, cmdmod_T *cmod)
{
    isn_T	*isn;

    ଯଦି (has_cmdmod(cmod, FALSE))
    {
	cctx->ctx_has_cmdmod = TRUE;

	ଯଦି ((isn = generate_instr(cctx, ISN_CMDMOD)) == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	isn->isn_arg.cmdmod.cf_cmdmod = ALLOC_ONE(cmdmod_T);
	ଯଦି (isn->isn_arg.cmdmod.cf_cmdmod == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	mch_memmove(isn->isn_arg.cmdmod.cf_cmdmod, cmod, ଆକାର(cmdmod_T));
	// filter program now belongs to the instruction
	cmod->cmod_filter_regmatch.regprog = ଖାଲି;
    }

    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_undo_cmdmods(cctx_T *cctx)
{
    ଯଦି (cctx->ctx_has_cmdmod && generate_instr(cctx, ISN_CMDMOD_REV) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    cctx->ctx_has_cmdmod = FALSE;
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate a STORE instruction for "dest", not being "dest_local".
 * "lhs" might be NULL.
 * Return FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_store_var(
	cctx_T		*cctx,
	assign_dest_T	dest,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vimvaridx,
	type_T		*type,
	char_u		*name,
	lhs_T		*lhs)
{
    ବାଛ (dest)
    {
	ସ୍ଥିତି dest_option:
	    ଫେରନ୍ତୁ generate_STOREOPT(cctx, ISN_STOREOPT,
					skip_option_env_lead(name), opt_flags);
	ସ୍ଥିତି dest_func_option:
	    ଫେରନ୍ତୁ generate_STOREOPT(cctx, ISN_STOREFUNCOPT,
					skip_option_env_lead(name), opt_flags);
	ସ୍ଥିତି dest_global:
	    // include g: with the name, easier to execute that way
	    ଫେରନ୍ତୁ generate_STORE(cctx, vim_strchr(name, AUTOLOAD_CHAR) == ଖାଲି
					? ISN_STOREG : ISN_STOREAUTO, 0, name);
	ସ୍ଥିତି dest_buffer:
	    // include b: with the name, easier to execute that way
	    ଫେରନ୍ତୁ generate_STORE(cctx, ISN_STOREB, 0, name);
	ସ୍ଥିତି dest_window:
	    // include w: with the name, easier to execute that way
	    ଫେରନ୍ତୁ generate_STORE(cctx, ISN_STOREW, 0, name);
	ସ୍ଥିତି dest_tab:
	    // include t: with the name, easier to execute that way
	    ଫେରନ୍ତୁ generate_STORE(cctx, ISN_STORET, 0, name);
	ସ୍ଥିତି dest_env:
	    ଫେରନ୍ତୁ generate_STORE(cctx, ISN_STOREENV, 0, name + 1);
	ସ୍ଥିତି dest_reg:
	    ଫେରନ୍ତୁ generate_STORE(cctx, ISN_STOREREG,
					 name[1] == '@' ? '"' : name[1], ଖାଲି);
	ସ୍ଥିତି dest_vimvar:
	    ଫେରନ୍ତୁ generate_STORE(cctx, ISN_STOREV, vimvaridx, ଖାଲି);
	ସ୍ଥିତି dest_script:
	ସ୍ଥିତି dest_script_v9:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    scriptvar_idx = lhs->lhs_scriptvar_idx;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    scriptvar_sid = lhs->lhs_scriptvar_sid;
		ଯଦି (scriptvar_idx < 0)
		{
		    isntype_T   isn_type = ISN_STORES;

		    // If "sn_import_autoload", generate ISN_STOREEXPORT (not
		    // ISN_STORES) if destination is in a vim9script or if
		    // there is no "sn_autoload_prefix".
		    ଯଦି (SCRIPT_ID_VALID(scriptvar_sid)
			     && SCRIPT_ITEM(scriptvar_sid)->sn_import_autoload
			     && ((SCRIPT_ITEM(scriptvar_sid)
						  ->sn_autoload_prefix == ଖାଲି)
				|| lhs->lhs_dest == dest_script_v9))
		    {
			// "import autoload './dir/script.vim'" - load script
			// first
			ଯଦି (generate_SOURCE(cctx, scriptvar_sid) == FAIL)
			    ଫେରନ୍ତୁ FAIL;
			isn_type = ISN_STOREEXPORT;
		    }

		    // "s:" may be included in the name.
		    ଫେରନ୍ତୁ generate_OLDSCRIPT(cctx, isn_type, name,
							  scriptvar_sid, type);
		}
		ଫେରନ୍ତୁ generate_VIM9SCRIPT(cctx, ISN_STORESCRIPT,
					   scriptvar_sid, scriptvar_idx, type);
	    }
	ସ୍ଥିତି dest_class_member:
	    ଫେରନ୍ତୁ generate_CLASSMEMBER(cctx, FALSE,
				     lhs->lhs_class, lhs->lhs_classmember_idx);

	ସ୍ଥିତି dest_local:
	ସ୍ଥିତି dest_ପଦସମଷ୍ଟି:
	    // cannot happen
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Return TRUE when inside a "for" or "while" loop.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
inside_loop_scope(cctx_T *cctx)
{
    scope_T	*scope = cctx->ctx_scope;

    ପାଇଁ (;;)
    {
	ଯଦି (scope == ଖାଲି)
	    ଭାଙ୍ଗ;
	ଯଦି (scope->se_type == FOR_SCOPE || scope->se_type == WHILE_SCOPE)
	    ଫେରନ୍ତୁ TRUE;
	scope = scope->se_outer;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_store_lhs(cctx_T *cctx, lhs_T *lhs, ପୂର୍ଣ୍ଣସଂଖ୍ୟା instr_ଗଣନା, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_decl)
{
    ଯଦି (lhs->lhs_dest != dest_local)
	ଫେରନ୍ତୁ generate_store_var(cctx, lhs->lhs_dest,
			    lhs->lhs_opt_flags, lhs->lhs_vimvaridx,
			    lhs->lhs_type, lhs->lhs_name, lhs);

    ଯଦି (lhs->lhs_lvar == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    garray_T	*instr = &cctx->ctx_instr;
    isn_T		*isn = ((isn_T *)instr->ga_data) + instr->ga_len - 1;

    // Optimization: turn "var = 123" from ISN_PUSHNR + ISN_STORE into
    // ISN_STORENR.
    // And "var = 0" does not need any instruction.
    ଯଦି (lhs->lhs_lvar->lv_from_outer == 0
	    && instr->ga_len == instr_ଗଣନା + 1
	    && isn->isn_type == ISN_PUSHNR)
    {
	varnumber_T val = isn->isn_arg.ସଂଖ୍ୟା;
	garray_T    *ଗଦା = &cctx->ctx_type_ଗଦା;

	ଯଦି (val == 0 && is_decl && !inside_loop_scope(cctx))
	{
	    // zero is the default value, no need to do anything
	    --instr->ga_len;
	}
	ଅନ୍ୟଥା
	{
	    isn->isn_type = ISN_STORENR;
	    isn->isn_arg.storenr.stnr_idx = lhs->lhs_lvar->lv_idx;
	    isn->isn_arg.storenr.stnr_val = val;
	}
	ଯଦି (ଗଦା->ga_len > 0)
	    --ଗଦା->ga_len;
    }
    ଅନ୍ୟଥା ଯଦି (lhs->lhs_lvar->lv_from_outer > 0)
	generate_STOREOUTER(cctx, lhs->lhs_lvar->lv_idx,
		lhs->lhs_lvar->lv_from_outer, lhs->lhs_lvar->lv_loop_depth,
		lhs->lhs_lvar->lv_loop_idx);
    ଅନ୍ୟଥା
	generate_STORE(cctx, ISN_STORE, lhs->lhs_lvar->lv_idx, ଖାଲି);
    ଫେରନ୍ତୁ OK;
}

/*
 * Generate instruction to set the script context.  Used to evaluate an
 * object member variable initialization expression in the context of the
 * script where the class is defined.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
generate_SCRIPTCTX_SET(cctx_T *cctx, sctx_T new_sctx)
{
    isn_T	*isn;

    RETURN_OK_IF_SKIP(cctx);
    ଯଦି ((isn = generate_instr(cctx, ISN_SCRIPTCTX_SET)) == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    isn->isn_arg.setsctx = new_sctx;
    ଫେରନ୍ତୁ OK;
}

#ଯଦି defined(FEAT_PROFILE)
    ଶୂନ୍ୟ
may_generate_prof_end(cctx_T *cctx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା prof_lnum)
{
    ଯଦି (cctx->ctx_compile_type == CT_PROFILE && prof_lnum >= 0)
	generate_instr(cctx, ISN_PROF_END);
}
#endif


/*
 * Delete an instruction, free what it contains.
 */
    ଶୂନ୍ୟ
delete_instr(isn_T *isn)
{
    ବାଛ (isn->isn_type)
    {
	ସ୍ଥିତି ISN_AUTOLOAD:
	ସ୍ଥିତି ISN_DEF:
	ସ୍ଥିତି ISN_EXEC:
	ସ୍ଥିତି ISN_EXECRANGE:
	ସ୍ଥିତି ISN_EXEC_SPLIT:
	ସ୍ଥିତି ISN_LEGACY_EVAL:
	ସ୍ଥିତି ISN_LOADAUTO:
	ସ୍ଥିତି ISN_LOADB:
	ସ୍ଥିତି ISN_LOADENV:
	ସ୍ଥିତି ISN_LOADG:
	ସ୍ଥିତି ISN_LOADOPT:
	ସ୍ଥିତି ISN_LOADT:
	ସ୍ଥିତି ISN_LOADW:
	ସ୍ଥିତି ISN_PUSHEXC:
	ସ୍ଥିତି ISN_PUSHFUNC:
	ସ୍ଥିତି ISN_PUSHS:
	ସ୍ଥିତି ISN_RANGE:
	ସ୍ଥିତି ISN_STOREAUTO:
	ସ୍ଥିତି ISN_STOREB:
	ସ୍ଥିତି ISN_STOREENV:
	ସ୍ଥିତି ISN_STOREG:
	ସ୍ଥିତି ISN_STORET:
	ସ୍ଥିତି ISN_STOREW:
	ସ୍ଥିତି ISN_STRINGMEMBER:
	    vim_free(isn->isn_arg.string);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_LOCKUNLOCK:
	    class_unref(isn->isn_arg.lockunlock.lu_cl_exec);
	    vim_free(isn->isn_arg.lockunlock.lu_string);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_SUBSTITUTE:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx;
		isn_T	*list = isn->isn_arg.subs.subs_instr;

		vim_free(isn->isn_arg.subs.subs_cmd);
		ପାଇଁ (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_INSTR:
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx;
		isn_T	*list = isn->isn_arg.instr;

		ପାଇଁ (idx = 0; list[idx].isn_type != ISN_FINISH; ++idx)
		    delete_instr(list + idx);
		vim_free(list);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_LOADS:
	ସ୍ଥିତି ISN_LOADEXPORT:
	ସ୍ଥିତି ISN_STORES:
	ସ୍ଥିତି ISN_STOREEXPORT:
	    vim_free(isn->isn_arg.loadstore.ls_name);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_UNLET:
	ସ୍ଥିତି ISN_UNLETENV:
	    vim_free(isn->isn_arg.unlet.ul_name);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_STOREOPT:
	ସ୍ଥିତି ISN_STOREFUNCOPT:
	    vim_free(isn->isn_arg.storeopt.so_name);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_PUSHBLOB:   // push blob isn_arg.blob
	    blob_unref(isn->isn_arg.blob);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_PUSHCLASS:
	    class_unref(isn->isn_arg.classarg);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_UCALL:
	    vim_free(isn->isn_arg.ufunc.cuf_name);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_FUNCREF:
	    {
		funcref_T	*funcref = &isn->isn_arg.funcref;
		funcref_extra_T *extra = funcref->fr_extra;

		ଯଦି (extra == ଖାଲି || extra->fre_func_name == ଖାଲି)
		{
		    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
		    ufunc_T *ufunc = dfunc->df_ufunc;

		    ଯଦି (ufunc != ଖାଲି && func_name_refcount(ufunc->uf_name))
			func_ptr_unref(ufunc);
		}
		ଯଦି (extra != ଖାଲି)
		{
		    char_u *name = extra->fre_func_name;

		    ଯଦି (name != ଖାଲି)
		    {
			func_unref(name);
			vim_free(name);
		    }
		    ଯଦି (extra->fre_class != ଖାଲି)
			class_unref(extra->fre_class);
		    vim_free(extra);
		}
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_DCALL:
	    {
		dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data)
		    + isn->isn_arg.dfunc.cdf_idx;

		ଯଦି (dfunc->df_ufunc != ଖାଲି
			&& func_name_refcount(dfunc->df_ufunc->uf_name))
		    func_ptr_unref(dfunc->df_ufunc);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_METHODCALL:
	    {
		cmfunc_T  *mfunc = isn->isn_arg.mfunc;
		class_unref(mfunc->cmf_itf);
		class_unref(mfunc->cmf_cache_cl);
		vim_free(mfunc);
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_NEWFUNC:
	    {
		newfuncarg_T *arg = isn->isn_arg.newfunc.nf_arg;

		ଯଦି (arg != ଖାଲି)
		{
		    ufunc_T *ufunc = find_func_even_dead(
					      arg->nfa_lambda, FFED_IS_GLOBAL);

		    ଯଦି (ufunc != ଖାଲି)
		    {
			unlink_def_function(ufunc);
			func_ptr_unref(ufunc);
		    }

		    vim_free(arg->nfa_lambda);
		    vim_free(arg->nfa_global);
		    vim_free(arg);
		}
	    }
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_CHECKTYPE:
	ସ୍ଥିତି ISN_SETTYPE:
	    free_type(isn->isn_arg.type.ct_type);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_CMDMOD:
	    vim_regfree(isn->isn_arg.cmdmod.cf_cmdmod
		    ->cmod_filter_regmatch.regprog);
	    vim_free(isn->isn_arg.cmdmod.cf_cmdmod);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_LOADSCRIPT:
	ସ୍ଥିତି ISN_STORESCRIPT:
	    vim_free(isn->isn_arg.script.scriptref);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_LOAD_CLASSMEMBER:
	ସ୍ଥିତି ISN_STORE_CLASSMEMBER:
	ସ୍ଥିତି ISN_GET_ITF_MEMBER:
	    class_unref(isn->isn_arg.classmember.cm_class);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_STOREINDEX:
	    class_unref(isn->isn_arg.storeindex.si_class);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_TRY:
	    vim_free(isn->isn_arg.tryref.try_ref);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_CEXPR_CORE:
	    vim_free(isn->isn_arg.cexpr.cexpr_ref->cer_cmdline);
	    vim_free(isn->isn_arg.cexpr.cexpr_ref);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି ISN_2BOOL:
	ସ୍ଥିତି ISN_2STRING:
	ସ୍ଥିତି ISN_2STRING_ANY:
	ସ୍ଥିତି ISN_ADDBLOB:
	ସ୍ଥିତି ISN_ADDLIST:
	ସ୍ଥିତି ISN_ADDTUPLE:
	ସ୍ଥିତି ISN_ANYINDEX:
	ସ୍ଥିତି ISN_ANYSLICE:
	ସ୍ଥିତି ISN_BCALL:
	ସ୍ଥିତି ISN_BLOBAPPEND:
	ସ୍ଥିତି ISN_BLOBINDEX:
	ସ୍ଥିତି ISN_BLOBSLICE:
	ସ୍ଥିତି ISN_CATCH:
	ସ୍ଥିତି ISN_CEXPR_AUCMD:
	ସ୍ଥିତି ISN_CHECKLEN:
	ସ୍ଥିତି ISN_CLEARDICT:
	ସ୍ଥିତି ISN_CMDMOD_REV:
	ସ୍ଥିତି ISN_COMPAREANY:
	ସ୍ଥିତି ISN_COMPAREBLOB:
	ସ୍ଥିତି ISN_COMPAREBOOL:
	ସ୍ଥିତି ISN_COMPAREDICT:
	ସ୍ଥିତି ISN_COMPAREFLOAT:
	ସ୍ଥିତି ISN_COMPAREFUNC:
	ସ୍ଥିତି ISN_COMPARELIST:
	ସ୍ଥିତି ISN_COMPARETUPLE:
	ସ୍ଥିତି ISN_COMPARENR:
	ସ୍ଥିତି ISN_COMPARENULL:
	ସ୍ଥିତି ISN_COMPAREOBJECT:
	ସ୍ଥିତି ISN_COMPARESPECIAL:
	ସ୍ଥିତି ISN_COMPARESTRING:
	ସ୍ଥିତି ISN_CONCAT:
	ସ୍ଥିତି ISN_CONSTRUCT:
	ସ୍ଥିତି ISN_COND2BOOL:
	ସ୍ଥିତି ISN_DEBUG:
	ସ୍ଥିତି ISN_DEFER:
	ସ୍ଥିତି ISN_DROP:
	ସ୍ଥିତି ISN_ECHO:
	ସ୍ଥିତି ISN_ECHOCONSOLE:
	ସ୍ଥିତି ISN_ECHOERR:
	ସ୍ଥିତି ISN_ECHOMSG:
	ସ୍ଥିତି ISN_ECHOWINDOW:
	ସ୍ଥିତି ISN_ENDLOOP:
	ସ୍ଥିତି ISN_ENDTRY:
	ସ୍ଥିତି ISN_EXECCONCAT:
	ସ୍ଥିତି ISN_EXECUTE:
	ସ୍ଥିତି ISN_FINALLY:
	ସ୍ଥିତି ISN_FINISH:
	ସ୍ଥିତି ISN_FOR:
	ସ୍ଥିତି ISN_GETITEM:
	ସ୍ଥିତି ISN_GET_OBJ_MEMBER:
	ସ୍ଥିତି ISN_JUMP:
	ସ୍ଥିତି ISN_JUMP_IF_ARG_NOT_SET:
	ସ୍ଥିତି ISN_JUMP_IF_ARG_SET:
	ସ୍ଥିତି ISN_LISTAPPEND:
	ସ୍ଥିତି ISN_LISTINDEX:
	ସ୍ଥିତି ISN_LISTSLICE:
	ସ୍ଥିତି ISN_TUPLEINDEX:
	ସ୍ଥିତି ISN_TUPLESLICE:
	ସ୍ଥିତି ISN_LOAD:
	ସ୍ଥିତି ISN_LOADBDICT:
	ସ୍ଥିତି ISN_LOADGDICT:
	ସ୍ଥିତି ISN_LOADOUTER:
	ସ୍ଥିତି ISN_LOADREG:
	ସ୍ଥିତି ISN_LOADTDICT:
	ସ୍ଥିତି ISN_LOADV:
	ସ୍ଥିତି ISN_LOADWDICT:
	ସ୍ଥିତି ISN_LOCKCONST:
	ସ୍ଥିତି ISN_MEMBER:
	ସ୍ଥିତି ISN_NEGATENR:
	ସ୍ଥିତି ISN_NEWDICT:
	ସ୍ଥିତି ISN_NEWLIST:
	ସ୍ଥିତି ISN_NEWTUPLE:
	ସ୍ଥିତି ISN_NEWPARTIAL:
	ସ୍ଥିତି ISN_OPANY:
	ସ୍ଥିତି ISN_OPFLOAT:
	ସ୍ଥିତି ISN_OPNR:
	ସ୍ଥିତି ISN_PCALL:
	ସ୍ଥିତି ISN_PCALL_END:
	ସ୍ଥିତି ISN_PROF_END:
	ସ୍ଥିତି ISN_PROF_START:
	ସ୍ଥିତି ISN_PUSHBOOL:
	ସ୍ଥିତି ISN_PUSHCHANNEL:
	ସ୍ଥିତି ISN_PUSHF:
	ସ୍ଥିତି ISN_PUSHJOB:
	ସ୍ଥିତି ISN_PUSHNR:
	ସ୍ଥିତି ISN_PUSHOBJ:
	ସ୍ଥିତି ISN_PUSHSPEC:
	ସ୍ଥିତି ISN_PUT:
	ସ୍ଥିତି ISN_IPUT:
	ସ୍ଥିତି ISN_REDIREND:
	ସ୍ଥିତି ISN_REDIRSTART:
	ସ୍ଥିତି ISN_RETURN:
	ସ୍ଥିତି ISN_RETURN_OBJECT:
	ସ୍ଥିତି ISN_RETURN_VOID:
	ସ୍ଥିତି ISN_SHUFFLE:
	ସ୍ଥିତି ISN_SLICE:
	ସ୍ଥିତି ISN_SOURCE:
	ସ୍ଥିତି ISN_STORE:
	ସ୍ଥିତି ISN_STORENR:
	ସ୍ଥିତି ISN_STOREOUTER:
	ସ୍ଥିତି ISN_STORE_THIS:
	ସ୍ଥିତି ISN_STORERANGE:
	ସ୍ଥିତି ISN_STOREREG:
	ସ୍ଥିତି ISN_STOREV:
	ସ୍ଥିତି ISN_STRINDEX:
	ସ୍ଥିତି ISN_STRSLICE:
	ସ୍ଥିତି ISN_THROW:
	ସ୍ଥିତି ISN_TRYCONT:
	ସ୍ଥିତି ISN_UNLETINDEX:
	ସ୍ଥିତି ISN_UNLETRANGE:
	ସ୍ଥିତି ISN_UNPACK:
	ସ୍ଥିତି ISN_USEDICT:
	ସ୍ଥିତି ISN_WHILE:
	ସ୍ଥିତି ISN_SCRIPTCTX_SET:
	// nothing allocated
	ଭାଙ୍ଗ;
    }
}

    ଶୂନ୍ୟ
clear_instr_ga(garray_T *gap)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;

    ପାଇଁ (idx = 0; idx < gap->ga_len; ++idx)
	delete_instr(((isn_T *)gap->ga_data) + idx);
    ga_clear(gap);
}


#endif  // defined(FEAT_EVAL)
