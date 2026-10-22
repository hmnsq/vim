/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * vim9execute.c: execute Vim9 script instructions
 */

#define USING_FLOAT_STUFF
#अन्तर्भाव "vim.h"

#सचे defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
प्रकारनिर्देश संरचना {
    पूर्णाङ्क	    tcd_frame_idx;	// ec_frame_idx at ISN_TRY
    पूर्णाङ्क	    tcd_राशिः_len;	// size of ectx.ec_stack at ISN_TRY
    पूर्णाङ्क	    tcd_in_catch;	// in catch or finally block
    पूर्णाङ्क	    tcd_did_throw;	// set did_throw in :endtry
    पूर्णाङ्क	    tcd_catch_idx;	// instruction of the first :catch or :finally
    पूर्णाङ्क	    tcd_finally_idx;	// instruction of the :finally block or zero
    पूर्णाङ्क	    tcd_endtry_idx;	// instruction of the :endtry
    पूर्णाङ्क	    tcd_caught;		// catch block entered
    पूर्णाङ्क	    tcd_cont;		// :continue encountered, jump here (minus one)
    पूर्णाङ्क	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
प्रकारनिर्देश संरचना {
    पूर्णाङ्क		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    पूर्णाङ्क		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
प्रकारनिर्देश संरचना {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    पूर्णाङ्क		or_outer_allocated;  // free "or_outer" later
} outer_ref_T;

// A stack is used to store:
// - arguments passed to a :def function
// - info about the calling function, to use when returning
// - local variables
// - temporary values
//
// In detail (FP == Frame Pointer):
//	  arg1		first argument from caller (if present)
//	  arg2		second argument from caller (if present)
//	  extra_arg1	any missing optional argument default value
// FP ->  cur_func	calling function
//	  current	previous instruction pointer
//	  frame_ptr	previous Frame Pointer
//	  var1		space for local variable
//	  var2		space for local variable
//	  ....		fixed space for max. number of local variables
//	  temp		temporary values
//	  ....		flexible space for temporary values (can grow big)

/*
 * Execution context.
 */
संरचना ectx_S {
    garray_T	ec_राशिः;	// stack of typval_T values
    पूर्णाङ्क		ec_frame_idx;	// index in ec_stack: context of ec_dfunc_idx
    पूर्णाङ्क		ec_initial_frame_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    पूर्णाङ्क		ec_dfunc_idx;	// current function index
    पूर्णाङ्क		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    पूर्णाङ्क		ec_did_emsg_before;
    पूर्णाङ्क		ec_trylevel_at_start;
    where_T	ec_where;
};

#ifdef FEAT_PROFILE
// stack of profinfo_T used when profiling.
स्थिर garray_T profile_info_ga = {0, 0, परिमाण(profinfo_T), 20, रिक्त};
#endif

// Get pointer to item in the stack.
#define राशिः_TV(idx) (((typval_T *)ectx->ec_राशिः.ga_data) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#define राशिः_TV_BOT(idx) (((typval_T *)ectx->ec_राशिः.ga_data) + ectx->ec_राशिः.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#define राशिः_TV_VAR(idx) (((typval_T *)ectx->ec_राशिः.ga_data) + ectx->ec_frame_idx + राशिः_FRAME_SIZE + idx)

// Return value for functions used to execute instructions
#define EXEC_FAIL	0
#define EXEC_OK		1
#define EXEC_DONE	2

    शून्य
to_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_to_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    स्थिर पूर्णाङ्क
ufunc_argcount(ufunc_T *ufunc)
{
    पटिनिवत्त ufunc->uf_args.ga_len + (ufunc->uf_va_name != रिक्त ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    स्थिर पूर्णाङ्क
exe_concat(पूर्णाङ्क गणनम्, ectx_T *ectx)
{
    पूर्णाङ्क		idx;
    size_t	len = 0;
    garray_T	ga;
    प्रकारनिर्देश संरचना
    {
	typval_T    *tv;
	size_t	    length;
    } string_segment_T;
    गणना
    {
	STRING_SEGMENT_CACHE_SIZE = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_SIZE];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    सचे (गणनम् > (पूर्णाङ्क)ARRAY_LENGTH(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, गणनम्);
	सचे (string_segment_tab == रिक्त)
	    पटिनिवत्त FAIL;
    }

    ga_init2(&ga, परिमाण(वर्ण), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    अत्थाय (idx = 0; idx < गणनम्; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = राशिः_TV_BOT(idx - गणनम्);
	सचे (segment->tv->vval.v_string != रिक्त)
	{
	    segment->length = STRLEN(segment->tv->vval.v_string);
	    len += segment->length;
	}
	अञ्ञथा
	    segment->length = 0;    // Ensure clean state for the second pass
    }

    सचे (ga_grow(&ga, (पूर्णाङ्क)len + 1) == FAIL)
    {
	सचे (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	पटिनिवत्त FAIL;
    }

    अत्थाय (idx = 0; idx < गणनम्; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->length);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_राशिः.ga_len -= गणनम् - 1;
    राशिः_TV_BOT(-1)->vval.v_string = ga.ga_data;

    सचे (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    पटिनिवत्त OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    स्थिर पूर्णाङ्क
exe_newlist(पूर्णाङ्क गणनम्, ectx_T *ectx)
{
    list_T	*list = रिक्त;
    पूर्णाङ्क		idx;
    typval_T	*tv;

    सचे (गणनम् >= 0)
    {
	list = list_alloc_with_वस्तूनि(गणनम्);
	सचे (list == रिक्त)
	    पटिनिवत्त FAIL;
	अत्थाय (idx = 0; idx < गणनम्; ++idx)
	    list_set_item(list, idx, राशिः_TV_BOT(idx - गणनम्));
    }

    सचे (गणनम् > 0)
	ectx->ec_राशिः.ga_len -= गणनम् - 1;
    अञ्ञथा सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
    {
	list_unref(list);
	पटिनिवत्त FAIL;
    }
    अञ्ञथा
	++ectx->ec_राशिः.ga_len;
    tv = राशिः_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_lock = 0;
    सचे (list != रिक्त)
	++list->lv_refcount;
    पटिनिवत्त OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    स्थिर पूर्णाङ्क
exe_newtuple(पूर्णाङ्क गणनम्, ectx_T *ectx)
{
    tuple_T	*tuple = रिक्त;
    पूर्णाङ्क		idx;
    typval_T	*tv;

    सचे (गणनम् >= 0)
    {
	tuple = tuple_alloc_with_वस्तूनि(गणनम्);
	सचे (tuple == रिक्त)
	    पटिनिवत्त FAIL;
	अत्थाय (idx = 0; idx < गणनम्; ++idx)
	    tuple_set_item(tuple, idx, राशिः_TV_BOT(idx - गणनम्));
    }

    सचे (गणनम् > 0)
	ectx->ec_राशिः.ga_len -= गणनम् - 1;
    अञ्ञथा सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
    {
	tuple_unref(tuple);
	पटिनिवत्त FAIL;
    }
    अञ्ञथा
	++ectx->ec_राशिः.ga_len;
    tv = राशिः_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_lock = 0;
    सचे (tuple != रिक्त)
	++tuple->tv_refcount;

    पटिनिवत्त OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    स्थिर पूर्णाङ्क
exe_newdict(पूर्णाङ्क गणनम्, ectx_T *ectx)
{
    dict_T	*dict = रिक्त;
    dictitem_T	*item;
    char_u	*कुञ्चिका;
    पूर्णाङ्क		idx;
    typval_T	*tv;

    सचे (गणनम् >= 0)
    {
	dict = dict_alloc();
	सचे (unlikely(dict == रिक्त))
	    पटिनिवत्त FAIL;
	अत्थाय (idx = 0; idx < गणनम्; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = राशिः_TV_BOT(2 * (idx - गणनम्));
	    // check key is unique
	    कुञ्चिका = tv->vval.v_string == रिक्त ? (char_u *)"" : tv->vval.v_string;
	    item = dict_find(dict, कुञ्चिका, -1);
	    सचे (item != रिक्त)
	    {
		semsg(_(e_duplicate_कुञ्चिका_in_dictionary_str), कुञ्चिका);
		dict_unref(dict);
		पटिनिवत्त MAYBE;
	    }
	    item = dictitem_alloc(कुञ्चिका);
	    clear_tv(tv);
	    सचे (unlikely(item == रिक्त))
	    {
		dict_unref(dict);
		पटिनिवत्त FAIL;
	    }
	    tv = राशिः_TV_BOT(2 * (idx - गणनम्) + 1);
	    item->di_tv = *tv;
	    item->di_tv.v_lock = 0;
	    tv->v_type = VAR_UNKNOWN;
	    सचे (dict_add(dict, item) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(item);
		dict_unref(dict);
		पटिनिवत्त FAIL;
	    }
	}
    }

    सचे (गणनम् > 0)
	ectx->ec_राशिः.ga_len -= 2 * गणनम् - 1;
    अञ्ञथा सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
    {
	dict_unref(dict);
	पटिनिवत्त FAIL;
    }
    अञ्ञथा
	++ectx->ec_राशिः.ga_len;
    tv = राशिः_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_lock = 0;
    tv->vval.v_dict = dict;
    सचे (dict != रिक्त)
	++dict->dv_refcount;
    पटिनिवत्त OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    शून्य
update_has_breakpoint(ufunc_T *ufunc)
{
    सचे (ufunc->uf_debug_tick == debug_tick)
	पटिनिवत्त;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

स्थिर garray_T dict_राशिः = GA_EMPTY;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    स्थिर पूर्णाङ्क
dict_राशिः_save(typval_T *tv)
{
    सचे (dict_राशिः.ga_growsize == 0)
	ga_init2(&dict_राशिः, परिमाण(typval_T), 10);
    सचे (ga_grow(&dict_राशिः, 1) == FAIL)
	पटिनिवत्त FAIL;
    ((typval_T *)dict_राशिः.ga_data)[dict_राशिः.ga_len] = *tv;
    ++dict_राशिः.ga_len;
    पटिनिवत्त OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    स्थिर typval_T *
dict_राशिः_get_tv(शून्य)
{
    सचे (dict_राशिः.ga_len == 0)
	पटिनिवत्त रिक्त;
    पटिनिवत्त ((typval_T *)dict_राशिः.ga_data) + dict_राशिः.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    स्थिर dict_T *
dict_राशिः_get_dict(शून्य)
{
    typval_T *tv;

    सचे (dict_राशिः.ga_len == 0)
	पटिनिवत्त रिक्त;
    tv = ((typval_T *)dict_राशिः.ga_data) + dict_राशिः.ga_len - 1;
    सचे (tv->v_type == VAR_DICT)
	पटिनिवत्त tv->vval.v_dict;
    पटिनिवत्त रिक्त;
}

/*
 * Drop an item from the dict stack.
 */
    स्थिर शून्य
dict_राशिः_drop(शून्य)
{
    सचे (dict_राशिः.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	पटिनिवत्त;
    }
    --dict_राशिः.ga_len;
    clear_tv(((typval_T *)dict_राशिः.ga_data) + dict_राशिः.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    स्थिर शून्य
dict_राशिः_clear(पूर्णाङ्क len)
{
    याव (dict_राशिः.ga_len > len)
	dict_राशिः_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    स्थिर outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    सचे (ptref == रिक्त)
	पटिनिवत्त &pt->pt_outer;

    // partial using partial (recursively)
    याव (ptref->pt_outer_partial != रिक्त)
	ptref = ptref->pt_outer_partial;
    पटिनिवत्त &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
check_ufunc_arg_types(ufunc_T *ufunc, पूर्णाङ्क argcount, पूर्णाङ्क off, ectx_T *ectx)
{
    सचे (ufunc->uf_arg_types == रिक्त && ufunc->uf_va_type == रिक्त)
	पटिनिवत्त OK;

    typval_T	*argv = राशिः_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    अत्थाय (पूर्णाङ्क i = 0; i < argcount; ++i)
    {
	type_T *type = रिक्त;

	// assume a v:none argument, using the default value, is always OK
	सचे (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_सङ्ख्या == VVAL_NONE)
	    अनुवर्त;
	// only pass values to user functions, never types
	सचे (check_typval_is_मानम्(&argv[i]) == FAIL)
	    पटिनिवत्त FAIL;

	सचे (i < ufunc->uf_args.ga_len && ufunc->uf_arg_types != रिक्त)
	    type = ufunc->uf_arg_types[i];
	अञ्ञथा सचे (ufunc->uf_va_type != रिक्त)
	    type = ufunc->uf_va_type->tt_member;
	सचे (type != रिक्त && check_typval_arg_type(type,
		    &argv[i], रिक्त, i + 1) == FAIL)
	    पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

/*
 * Call compiled function "cdf_idx" from compiled code.
 * This adds a stack frame and sets the instruction pointer to the start of the
 * called function.
 * If "pt_arg" is not NULL use "pt_arg->pt_outer" for ec_outer_ref->or_outer.
 *
 * Stack has:
 * - current arguments (already there)
 * - omitted optional argument (default values) added here
 * - stack frame:
 *	- pointer to calling function
 *	- Index of next instruction in calling function
 *	- previous frame pointer
 * - reserved space for local variables
 */
    स्थिर पूर्णाङ्क
call_dfunc(
	पूर्णाङ्क		cdf_idx,
	partial_T	*pt_arg,
	पूर्णाङ्क		argcount_arg,
	ectx_T		*ectx)
{
    पूर्णाङ्क		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    पूर्णाङ्क		did_emsg_before = did_emsg_cumul + did_emsg;
    पूर्णाङ्क		arg_to_add;
    पूर्णाङ्क		vararg_गणनम् = 0;
    पूर्णाङ्क		varcount;
    पूर्णाङ्क		idx;
    estack_T	*entry;
    funclocal_T	*floc = रिक्त;
    पूर्णाङ्क		res = OK;
    compiletype_T compile_type;

    सचे (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == रिक्त ? (char_u *)"unknown" : dfunc->df_name);
	पटिनिवत्त FAIL;
    }

#ifdef FEAT_PROFILE
    सचे (do_profiling == PROF_YES)
    {
	सचे (GA_GROW_OK(&profile_info_ga, 1))
	{
	    profinfo_T *info = ((profinfo_T *)profile_info_ga.ga_data)
						      + profile_info_ga.ga_len;
	    ++profile_info_ga.ga_len;
	    CLEAR_POINTER(info);
	    profile_may_start_func(info, ufunc,
			(((dfunc_T *)def_functions.ga_data)
					      + ectx->ec_dfunc_idx)->df_ufunc);
	}
    }
#endif

    // When debugging and using "cont" switches to the not-debugged
    // instructions, may need to still compile them.
    compile_type = get_compile_type(ufunc);
    सचे (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, रिक्त);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    }
    सचे (res == FAIL || INSTRUCTIONS(dfunc) == रिक्त)
    {
	सचे (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	पटिनिवत्त FAIL;
    }

    सचे (ufunc->uf_va_name != रिक्त)
    {
	// Need to make a list out of the vararg arguments.
	// Stack at time of call with 2 varargs:
	//   normal_arg
	//   optional_arg
	//   vararg_1
	//   vararg_2
	// After creating the list:
	//   normal_arg
	//   optional_arg
	//   vararg-list
	// With missing optional arguments we get:
	//    normal_arg
	// After creating the list
	//    normal_arg
	//    (space for optional_arg)
	//    vararg-list
	vararg_गणनम् = argcount - ufunc->uf_args.ga_len;
	सचे (vararg_गणनम् < 0)
	    vararg_गणनम् = 0;
	अञ्ञथा
	    argcount -= vararg_गणनम्;
	सचे (exe_newlist(vararg_गणनम्, ectx) == FAIL)
	    पटिनिवत्त FAIL;

	vararg_गणनम् = 1;
    }

    arg_to_add = ufunc->uf_args.ga_len - argcount;
    सचे (arg_to_add < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
			-arg_to_add), -arg_to_add);
	पटिनिवत्त FAIL;
    }
    अञ्ञथा सचे (arg_to_add > ufunc->uf_def_args.ga_len)
    {
	पूर्णाङ्क missing = arg_to_add - ufunc->uf_def_args.ga_len;

	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
			missing), missing);
	पटिनिवत्त FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = राशिः_TV_BOT(0) - argcount - vararg_गणनम् - 1;

    सचे (IS_OBJECT_METHOD(ufunc) && !IS_CONSTRUCTOR_METHOD(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == रिक्त)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	पटिनिवत्त FAIL;
    }

    // Check the argument types.
    सचे (check_ufunc_arg_types(ufunc, argcount, vararg_गणनम्, ectx) == FAIL)
	पटिनिवत्त FAIL;

    // While check_ufunc_arg_types call, def function compilation process may
    // run.  If so many def functions are compiled, def_functions array may be
    // reallocated and dfunc may no longer have valid pointer.  Get the object
    // pointer from def_functions again here.
    dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;

    // Reserve space for:
    // - missing arguments
    // - stack frame
    // - local variables
    // - if needed: a counter for number of closures created in
    //   ectx->ec_funcrefs.
    varcount = dfunc->df_varcount + dfunc->df_has_closure;
    सचे (GA_GROW_FAILS(&ectx->ec_राशिः,
				     arg_to_add + राशिः_FRAME_SIZE + varcount))
	पटिनिवत्त FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = राशिः_TV_BOT(0) - argcount - vararg_गणनम् - 1;

    // If depth of calling is getting too high, don't execute the function.
    सचे (funcdepth_increment() == FAIL)
	पटिनिवत्त FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    सचे (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	सचे (floc == रिक्त)
	    पटिनिवत्त FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    सचे (vararg_गणनम् > 0 && arg_to_add > 0)
	*राशिः_TV_BOT(arg_to_add - 1) = *राशिः_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    अत्थाय (idx = 0; idx < arg_to_add; ++idx)
	राशिः_TV_BOT(idx - vararg_गणनम्)->v_type = VAR_UNKNOWN;
    ectx->ec_राशिः.ga_len += arg_to_add;

    // Store current execution state in stack frame for ISN_RETURN.
    राशिः_TV_BOT(राशिः_FRAME_FUNC_OFF)->vval.v_सङ्ख्या = ectx->ec_dfunc_idx;
    राशिः_TV_BOT(राशिः_FRAME_IIDX_OFF)->vval.v_सङ्ख्या = ectx->ec_iidx;
    राशिः_TV_BOT(राशिः_FRAME_INSTR_OFF)->vval.v_string = (शून्य *)ectx->ec_instr;
    राशिः_TV_BOT(राशिः_FRAME_OUTER_OFF)->vval.v_string =
						    (शून्य *)ectx->ec_outer_ref;
    राशिः_TV_BOT(राशिः_FRAME_FUNCLOCAL_OFF)->vval.v_string = (शून्य *)floc;
    राशिः_TV_BOT(राशिः_FRAME_IDX_OFF)->vval.v_सङ्ख्या = ectx->ec_frame_idx;
    ectx->ec_frame_idx = ectx->ec_राशिः.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    पूर्णाङ्क initcount = dfunc->df_varcount + (dfunc->df_has_closure ? 1 : 0);
    अत्थाय (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = राशिः_TV_BOT(राशिः_FRAME_SIZE + idx);

	tv->v_type = VAR_सङ्ख्या;
	tv->vval.v_सङ्ख्या = 0;
    }
    ectx->ec_राशिः.ga_len += राशिः_FRAME_SIZE + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    सचे (IS_OBJECT_METHOD(ufunc))
    {
	सचे (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_internal_error_str), "type in stack is not an object");
	    पटिनिवत्त FAIL;
	}

	*राशिः_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != रिक्त ? pt_arg : ufunc->uf_partial;
    सचे (pt != रिक्त || (ufunc->uf_flags & FC_CLOSURE))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	सचे (ref == रिक्त)
	    पटिनिवत्त FAIL;
	सचे (pt != रिक्त)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	अञ्ञथा
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    सचे (unlikely(ref->or_outer == रिक्त))
	    {
		vim_free(ref);
		पटिनिवत्त FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->out_राशिः = &ectx->ec_राशिः;
	    ref->or_outer->out_frame_idx = ectx->ec_frame_idx;
	    सचे (ectx->ec_outer_ref != रिक्त)
		ref->or_outer->out_up = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    अञ्ञथा
	ectx->ec_outer_ref = रिक्त;

    सचे (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    entry = estack_प्रक्षिप_ufunc(ufunc, 1);
    सचे (entry != रिक्त)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	entry->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    पटिनिवत्त OK;
}

// Double linked list of funcstack_T in use.
स्थिर funcstack_T *first_funcstack = रिक्त;

    स्थिर शून्य
add_funcstack_to_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    सचे (first_funcstack != रिक्त)
	first_funcstack->fs_prev = funcstack;
    funcstack->fs_next = first_funcstack;
    funcstack->fs_prev = रिक्त;
    first_funcstack = funcstack;
}

    स्थिर शून्य
remove_funcstack_from_list(funcstack_T *funcstack)
{
    सचे (funcstack->fs_prev == रिक्त)
	first_funcstack = funcstack->fs_next;
    अञ्ञथा
	funcstack->fs_prev->fs_next = funcstack->fs_next;
    सचे (funcstack->fs_next != रिक्त)
	funcstack->fs_next->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    स्थिर पूर्णाङ्क
handle_closure_in_use(ectx_T *ectx, पूर्णाङ्क free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    पूर्णाङ्क		argcount;
    पूर्णाङ्क		शिखरम्;
    पूर्णाङ्क		idx;
    typval_T	*tv;
    पूर्णाङ्क		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_गणनम्;

    सचे (dfunc->df_ufunc == रिक्त)
	पटिनिवत्त OK;  // function was freed
    सचे (dfunc->df_has_closure == 0)
	पटिनिवत्त OK;  // no closures
    tv = राशिः_TV(ectx->ec_frame_idx + राशिः_FRAME_SIZE + dfunc->df_varcount);
    closure_गणनम् = tv->vval.v_सङ्ख्या;
    सचे (closure_गणनम् == 0)
	पटिनिवत्त OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    शिखरम् = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    अत्थाय (idx = 0; idx < closure_गणनम्; ++idx)
    {
	partial_T   *pt;
	पूर्णाङ्क	    off = gap->ga_len - closure_गणनम् + idx;

	सचे (off < 0)
	    अनुवर्त;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	सचे (pt->pt_refcount > 1)
	{
	    पूर्णाङ्क refcount = pt->pt_refcount;
	    पूर्णाङ्क i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    अत्थाय (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = राशिः_TV(ectx->ec_frame_idx
						       + राशिः_FRAME_SIZE + i);
		सचे (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    सचे (refcount > 1)
	    {
		closure_in_use = TRUE;
		भङ्ग;
	    }
	}
    }

    सचे (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *राशिः;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	सचे (funcstack == रिक्त)
	    पटिनिवत्त FAIL;

	funcstack->fs_var_offset = argcount + राशिः_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset
							  + dfunc->df_varcount;
	राशिः = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = राशिः;
	सचे (राशिः == रिक्त)
	{
	    vim_free(funcstack);
	    पटिनिवत्त FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	अत्थाय (idx = 0; idx < argcount; ++idx)
	{
	    tv = राशिः_TV(शिखरम् + idx);
	    सचे (free_arguments)
	    {
		*(राशिः + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    अञ्ञथा सचे (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, राशिः + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	अत्थाय (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = राशिः_TV(ectx->ec_frame_idx + राशिः_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    सचे (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != रिक्त)
	    {
		पूर्णाङ्क	    i;

		अत्थाय (i = 0; i < closure_गणनम्; ++i)
		    सचे (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_गणनम् + i])
			++funcstack->fs_min_refcount;
	    }

	    *(राशिः + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	अत्थाय (idx = 0; idx < closure_गणनम्; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_गणनम् + idx];
	    सचे (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_राशिः = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - शिखरम्;
	    }
	}
    }

    अत्थाय (idx = 0; idx < closure_गणनम्; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_गणनम् + idx]);
    gap->ga_len -= closure_गणनम्;
    सचे (gap->ga_len == 0)
	ga_clear(gap);

    पटिनिवत्त OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    पूर्णाङ्क
funcstack_check_refcount(funcstack_T *funcstack)
{
    पूर्णाङ्क		i;
    garray_T	*gap = &funcstack->fs_ga;
    पूर्णाङ्क		done = 0;
    typval_T	*राशिः;

    सचे (funcstack->fs_refcount > funcstack->fs_min_refcount)
	पटिनिवत्त FALSE;
    अत्थाय (i = funcstack->fs_var_offset; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_data) + i;

	सचे (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != रिक्त
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++done;
    }
    सचे (done != funcstack->fs_min_refcount)
	पटिनिवत्त FALSE;

    राशिः = gap->ga_data;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    अत्थाय (i = 0; i < gap->ga_len; ++i)
	clear_tv(राशिः + i);
    vim_free(राशिः);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    पटिनिवत्त TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    पूर्णाङ्क
set_ref_in_funcstacks(पूर्णाङ्क copyID)
{
    funcstack_T *funcstack;

    अत्थाय (funcstack = first_funcstack; funcstack != रिक्त;
						funcstack = funcstack->fs_next)
    {
	typval_T    *राशिः = funcstack->fs_ga.ga_data;
	पूर्णाङ्क	    i;

	अत्थाय (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    सचे (set_ref_in_item(राशिः + i, copyID, रिक्त, रिक्त, रिक्त))
		पटिनिवत्त TRUE;  // abort
    }
    पटिनिवत्त FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
स्थिर ectx_T *current_ectx = रिक्त;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    पूर्णाङ्क
in_def_function(शून्य)
{
    पटिनिवत्त current_ectx != रिक्त;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    पूर्णाङ्क
fill_exec_lval_मूलम्(lval_मूलम्_T *मूलम्)
{
    ectx_T *ectx = current_ectx;
    सचे (ectx != रिक्त)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	सचे (ufunc->uf_class != रिक्त)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    सचे (tv != रिक्त)
	    {
		CLEAR_POINTER(मूलम्);
		मूलम्->lr_tv = tv;
		copy_tv(राशिः_TV_VAR(0), मूलम्->lr_tv);
		मूलम्->lr_cl_exec = ufunc->uf_class;
		++मूलम्->lr_cl_exec->class_refcount;
		पटिनिवत्त OK;
	    }
	}
    }

    पटिनिवत्त FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(शून्य)
{
    ectx_T *r = current_ectx;

    current_ectx = रिक्त;
    पटिनिवत्त r;
}

    शून्य
restore_current_ectx(ectx_T *ectx)
{
    सचे (current_ectx != रिक्त)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    स्थिर list_T *
add_defer_item(पूर्णाङ्क var_idx, पूर्णाङ्क argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = राशिः_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    सचे (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	सचे (rettv_list_alloc(defer_tv) == FAIL)
	    पटिनिवत्त रिक्त;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_वस्तूनि(argcount + 1);
    सचे (l == रिक्त)
	पटिनिवत्त रिक्त;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_lock = 0;
    सचे (list_निवेशय_tv(defer_l, &listval, defer_l->lv_first) == FAIL)
    {
	list_free(l);
	पटिनिवत्त रिक्त;
    }

    पटिनिवत्त l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
defer_command(पूर्णाङ्क var_idx, पूर्णाङ्क argcount, ectx_T *ectx)
{
    list_T	*l = add_defer_item(var_idx, argcount, ectx);
    पूर्णाङ्क		i;
    typval_T	*func_tv;

    सचे (l == रिक्त)
	पटिनिवत्त FAIL;

    func_tv = राशिः_TV_BOT(-argcount - 1);
    सचे (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	पटिनिवत्त FAIL;
    }
    list_set_item(l, 0, func_tv);

    अत्थाय (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, राशिः_TV_BOT(-argcount + i));
    ectx->ec_राशिः.ga_len -= argcount + 1;
    पटिनिवत्त OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    पूर्णाङ्क
add_defer_function(char_u *name, पूर्णाङ्क argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    पूर्णाङ्क		i;

    सचे (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	पटिनिवत्त FAIL;
    }

    l = add_defer_item(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    सचे (l == रिक्त)
    {
	vim_free(name);
	पटिनिवत्त FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_lock = 0;
    func_tv.vval.v_string = name;
    list_set_item(l, 0, &func_tv);

    अत्थाय (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, argvars + i);
    पटिनिवत्त OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    स्थिर शून्य
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = राशिः_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    सचे (defer_tv->v_type != VAR_LIST)
	पटिनिवत्त;	 // no function added
    FOR_ALL_LIST_वस्तूनि(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ARGS];
	पूर्णाङ्क	    i;
	listitem_T  *arg_li = l->lv_first;
	typval_T    *functv = &l->lv_first->li_tv;
	पूर्णाङ्क	    argcount = l->lv_len - 1;

	सचे (functv->vval.v_string == रिक्त)
	    // already being called, can happen if function does ":qa"
	    अनुवर्त;

	अत्थाय (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_next;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = रिक्त;
	partial_T	*pt = रिक्त;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	सचे (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = रिक्त;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    सचे (funcexe.fe_object != रिक्त)
		++funcexe.fe_object->obj_refcount;
	}
	अञ्ञथा
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = रिक्त;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(शून्य)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	सचे (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	अञ्ञथा
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    स्थिर पूर्णाङ्क
func_return(ectx_T *ectx)
{
    पूर्णाङ्क		idx;
    पूर्णाङ्क		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    पूर्णाङ्क		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*entry;
    पूर्णाङ्क		prev_dfunc_idx = राशिः_TV(ectx->ec_frame_idx
					+ राशिः_FRAME_FUNC_OFF)->vval.v_सङ्ख्या;
    funclocal_T	*floc;
#ifdef FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_data)
							      + prev_dfunc_idx;

    सचे (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	सचे (dfunc->df_ufunc->uf_profiling
				   || (caller != रिक्त && caller->uf_profiling))
	{
	    profile_may_end_func(((profinfo_T *)profile_info_ga.ga_data)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#endif

    सचे (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    सचे (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    entry = estack_निष्कासय();
    सचे (entry != रिक्त)
	current_sctx = entry->es_save_sctx;

    सचे (handle_closure_in_use(ectx, TRUE) == FAIL)
	पटिनिवत्त FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    पूर्णाङ्क शिखरम् = ectx->ec_frame_idx - argcount;
    सचे (IS_OBJECT_METHOD(dfunc->df_ufunc))
	--शिखरम्;
    अत्थाय (idx = शिखरम्; idx < ectx->ec_frame_idx; ++idx)
	clear_tv(राशिः_TV(idx));

    // Clear local variables and temp values, but not the return value.
    अत्थाय (idx = ectx->ec_frame_idx + राशिः_FRAME_SIZE;
					idx < ectx->ec_राशिः.ga_len - 1; ++idx)
	clear_tv(राशिः_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_राशिः.ga_len - 1;
    सचे (ret_idx == ectx->ec_frame_idx + राशिः_FRAME_IDX_OFF)
	ret_idx = 0;

    सचे (ectx->ec_outer_ref != रिक्त)
    {
	सचे (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = राशिः_TV(ectx->ec_frame_idx
					+ राशिः_FRAME_IIDX_OFF)->vval.v_सङ्ख्या;
    ectx->ec_instr = (शून्य *)राशिः_TV(ectx->ec_frame_idx
				       + राशिः_FRAME_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (शून्य *)राशिः_TV(ectx->ec_frame_idx
				       + राशिः_FRAME_OUTER_OFF)->vval.v_string;
    floc = (शून्य *)राशिः_TV(ectx->ec_frame_idx
				   + राशिः_FRAME_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_frame_idx = राशिः_TV(ectx->ec_frame_idx
				       + राशिः_FRAME_IDX_OFF)->vval.v_सङ्ख्या;

    सचे (floc == रिक्त)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    अञ्ञथा
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    सचे (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_राशिः.ga_len = शिखरम् + 1;
	*राशिः_TV_BOT(-1) = *राशिः_TV(ret_idx);
    }
    अञ्ञथा
	// Reset the stack to the position before the call.
	ectx->ec_राशिः.ga_len = शिखरम्;

    funcdepth_decrement();
    --ex_nesting_level;
    पटिनिवत्त OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    स्थिर पूर्णाङ्क
call_prepare(पूर्णाङ्क argcount, typval_T *argvars, ectx_T *ectx)
{
    पूर्णाङ्क		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    अत्थाय (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *राशिः_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    सचे (argcount > 0)
	ectx->ec_राशिः.ga_len -= argcount - 1;
    अञ्ञथा सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
	पटिनिवत्त FAIL;
    अञ्ञथा
	++ectx->ec_राशिः.ga_len;

    // Default return value is zero.
    tv = राशिः_TV_BOT(-1);
    tv->v_type = VAR_सङ्ख्या;
    tv->vval.v_सङ्ख्या = 0;
    tv->v_lock = 0;

    पटिनिवत्त OK;
}

/*
 * Call a builtin function by index.
 */
    स्थिर पूर्णाङ्क
call_bfunc(पूर्णाङ्क func_idx, पूर्णाङ्क argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    पूर्णाङ्क		idx;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    वर्ण	*save_func_name = ectx->ec_where.wt_func_name;

    सचे (call_prepare(argcount, argvars, ectx) == FAIL)
	पटिनिवत्त FAIL;

    // Check for void value being passed as an argument.
    अत्थाय (idx = 0; idx < argcount; ++idx)
	सचे (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_मानम्));
	    अत्थाय (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    पटिनिवत्त FAIL;
	}

    ectx->ec_where.wt_func_name = internal_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_internal_func_by_idx(func_idx, argvars, राशिः_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    अत्थाय (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    सचे (did_emsg > did_emsg_before)
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    स्थिर पूर्णाङ्क
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	पूर्णाङ्क	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    funcexe_T   funcexe;
    funcerror_T	error;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    सचे (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, रिक्त)
								       == FAIL)
	पटिनिवत्त FAIL;
    सचे (ufunc->uf_def_status == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	सचे (error != FCERR_UNKNOWN)
	{
	    सचे (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    अञ्ञथा
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    पटिनिवत्त FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	सचे (iptr != रिक्त)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	पटिनिवत्त call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    सचे (call_prepare(argcount, argvars, ectx) == FAIL)
	पटिनिवत्त FAIL;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != रिक्त ? selfdict : dict_राशिः_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      राशिः_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    अत्थाय (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    सचे (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_found_var);
	पटिनिवत्त FAIL;
    }
    सचे (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

/*
 * If command modifiers were applied restore them.
 */
    स्थिर शून्य
may_restore_cmdmod(funclocal_T *funclocal)
{
    सचे (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = रिक्त;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    स्थिर पूर्णाङ्क
vim9_aborting(पूर्णाङ्क prev_uncaught_emsg)
{
    पटिनिवत्त uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    स्थिर पूर्णाङ्क
call_by_name(
	char_u	    *name,
	पूर्णाङ्क	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    सचे (builtin_function(name, -1))
    {
	पूर्णाङ्क func_idx = find_internal_func(name);

	सचे (func_idx < 0)  // Impossible?
	    पटिनिवत्त FAIL;
	सचे (check_internal_func(func_idx, argcount) < 0)
	    पटिनिवत्त FAIL;
	पटिनिवत्त call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*start_bracket = generic_func_find_open_bracket(name);
    सचे (start_bracket != रिक्त)
    {
	cc = *start_bracket;
	*start_bracket = NUL;
    }

    ufunc = find_func(name, FALSE);

    सचे (ufunc == रिक्त)
    {
	पूर्णाङ्क prev_uncaught_emsg = uncaught_emsg;

	सचे (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = find_func(name, FALSE);

	सचे (vim9_aborting(prev_uncaught_emsg))
	{
	    सचे (start_bracket != रिक्त)
		*start_bracket = cc;
	    पटिनिवत्त FAIL;  // bail out if loading the script caused an error
	}
    }

    सचे (start_bracket != रिक्त)
	*start_bracket = cc;

    सचे (ufunc != रिक्त)
    {
	सचे (IS_GENERIC_FUNC(ufunc))
	{
	    सचे (start_bracket != रिक्त)
		ufunc = find_generic_func(ufunc, name, &start_bracket);
	    अञ्ञथा
	    {
		emsg_funcname(e_generic_func_missing_type_args_str, name);
		ufunc = रिक्त;
	    }
	    सचे (ufunc == रिक्त)
		पटिनिवत्त FAIL;
	}
	अञ्ञथा सचे (start_bracket != रिक्त)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    पटिनिवत्त FAIL;
	}

	सचे (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    पटिनिवत्त FAIL;

	पटिनिवत्त call_ufunc(ufunc, रिक्त, argcount, ectx, iptr, selfdict);
    }

    पटिनिवत्त FAIL;
}

    स्थिर पूर्णाङ्क
call_partial(
	typval_T    *tv,
	पूर्णाङ्क	    argcount_arg,
	ectx_T	    *ectx)
{
    पूर्णाङ्क		argcount = argcount_arg;
    char_u	*name = रिक्त;
    पूर्णाङ्क		called_emsg_before = called_emsg;
    पूर्णाङ्क		res = FAIL;
    dict_T	*selfdict = रिक्त;

    सचे (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	पूर्णाङ्क	    i;

	सचे (pt->pt_obj != रिक्त)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		पटिनिवत्त FAIL;
	    अत्थाय (i = 1; i <= argcount; ++i)
		*राशिः_TV_BOT(-i + 1) = *राशिः_TV_BOT(-i);

	    typval_T *obj_tv = राशिः_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_lock = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_राशिः.ga_len;
	}

	सचे (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, pt->pt_argc))
		पटिनिवत्त FAIL;
	    अत्थाय (i = 1; i <= argcount; ++i)
		*राशिः_TV_BOT(-i + pt->pt_argc) = *राशिः_TV_BOT(-i);
	    ectx->ec_राशिः.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    अत्थाय (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], राशिः_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	सचे (pt->pt_func != रिक्त)
	    पटिनिवत्त call_ufunc(pt->pt_func, pt, argcount, ectx, रिक्त, selfdict);

	name = pt->pt_name;
    }
    अञ्ञथा सचे (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    सचे (name != रिक्त)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = रिक्त;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	सचे (error != FCERR_NONE)
	    res = FAIL;
	अञ्ञथा
	    res = call_by_name(fname, argcount, ectx, रिक्त, selfdict);
	vim_free(tofree);
    }

    सचे (res == FAIL)
    {
	सचे (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == रिक्त ? (char_u *)"[unknown]" : name);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    स्थिर पूर्णाङ्क
error_if_locked(पूर्णाङ्क lock, वर्ण *error)
{
    सचे (lock & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	पटिनिवत्त TRUE;
    }
    पटिनिवत्त FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    स्थिर पूर्णाङ्क
check_for_सङ्ख्या(typval_T *tv)
{
    सचे (tv->v_type != VAR_सङ्ख्या)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_सङ्ख्या), vartype_name(tv->v_type));
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    स्थिर शून्य
store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    पूर्णाङ्क		    flags = ASSIGN_DECL;

    सचे (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, रिक्त, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    स्थिर पूर्णाङ्क
do_2string(typval_T *tv, पूर्णाङ्क is_2string_any, पूर्णाङ्क tostring_flags)
{
    सचे (tv->v_type == VAR_STRING)
	पटिनिवत्त OK;

    char_u *str;

    सचे (is_2string_any)
    {
	विकल्प (tv->v_type)
	{
	    पक्ष VAR_SPECIAL:
	    पक्ष VAR_BOOL:
	    पक्ष VAR_सङ्ख्या:
	    पक्ष VAR_FLOAT:
	    पक्ष VAR_DICT:
	    पक्ष VAR_BLOB:	भङ्ग;

	    पक्ष VAR_LIST:
				सचे (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, परिमाण(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    सचे (str == रिक्त)
					पटिनिवत्त FAIL;
				    s = str;
				    याव ((e = vim_strchr(s, '\n')) != रिक्त)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					सचे (p != रिक्त)
					{
					    ga_concat(&ga, p);
					    GA_CONCAT_LITERAL(&ga, " ");
					    vim_free(p);
					}
					s = e + 1;
				    }
				    vim_free(str);
				    clear_tv(tv);
				    tv->v_type = VAR_STRING;
				    tv->vval.v_string = ga.ga_data;
				    पटिनिवत्त OK;
				}
				सचे (tostring_flags & TOSTRING_INTERPOLATE)
				    भङ्ग;
				// FALLTHROUGH
	    स्वभाव:	to_string_error(tv->v_type);
			पटिनिवत्त FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    पटिनिवत्त OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    स्थिर शून्य
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    विकल्प (tv->v_type)
    {
	पक्ष VAR_LIST:
	    सचे (tv->vval.v_list == रिक्त && sv->sv_type != &t_list_empty)
		(शून्य)rettv_list_alloc(tv);
	    भङ्ग;
	पक्ष VAR_TUPLE:
	    सचे (tv->vval.v_tuple == रिक्त && sv->sv_type != &t_tuple_empty)
		(शून्य)rettv_tuple_alloc(tv);
	    भङ्ग;
	पक्ष VAR_DICT:
	    सचे (tv->vval.v_dict == रिक्त && sv->sv_type != &t_dict_empty)
		(शून्य)rettv_dict_alloc(tv);
	    भङ्ग;
	पक्ष VAR_BLOB:
	    सचे (tv->vval.v_blob == रिक्त && sv->sv_type != &t_blob_null)
		(शून्य)rettv_blob_alloc(tv);
	    भङ्ग;
	स्वभाव:
	    भङ्ग;
    }
}

/*
 * Return the character "str[index]" where "index" is the character index,
 * including composing characters.
 * If "index" is out of range NULL is returned.
 */
    char_u *
char_from_string(char_u *str, varnumber_T index)
{
    size_t	    nbyte = 0;
    varnumber_T	    nchar = index;
    size_t	    slen;

    सचे (str == रिक्त)
	पटिनिवत्त रिक्त;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    सचे (index < 0)
    {
	पूर्णाङ्क	clen = 0;

	अत्थाय (nbyte = 0; nbyte < slen; ++clen)
	{
	    सचे (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    अञ्ञथा सचे (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    अञ्ञथा
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + index;
	सचे (nchar < 0)
	    // unlike list: index out of range results in empty string
	    पटिनिवत्त रिक्त;
    }

    अत्थाय (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	सचे (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	अञ्ञथा सचे (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	अञ्ञथा
	    nbyte += mb_ptr2len(str + nbyte);
    }
    सचे (nbyte >= slen)
	पटिनिवत्त रिक्त;
    पटिनिवत्त vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    स्थिर long
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    सचे (nchar >= 0)
    {
	याव (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    अञ्ञथा
    {
	nbyte = str_len;
	याव (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_head_off(str, str + nbyte);
	    ++nchar;
	}
	सचे (nchar < 0)
	    पटिनिवत्त -1;
    }
    पटिनिवत्त (long)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_slice(char_u *str, varnumber_T first, varnumber_T last, पूर्णाङ्क exclusive)
{
    long	start_byte, end_byte;
    size_t	slen;

    सचे (str == रिक्त)
	पटिनिवत्त रिक्त;
    slen = STRLEN(str);
    start_byte = char_idx2byte(str, slen, first);
    सचे (start_byte < 0)
	start_byte = 0; // first index very negative: use zero
    सचे ((last == -1 && !exclusive) || last == VARNUM_MAX)
	end_byte = (long)slen;
    अञ्ञथा
    {
	end_byte = char_idx2byte(str, slen, last);
	सचे (!exclusive && end_byte >= 0 && end_byte < (long)slen)
	    // end index is inclusive
	    end_byte += mb_ptr2len(str + end_byte);
    }

    सचे (start_byte >= (long)slen || end_byte <= start_byte)
	पटिनिवत्त रिक्त;
    पटिनिवत्त vim_strnsave(str + start_byte, end_byte - start_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    स्थिर svar_T *
get_script_svar(scriptref_T *sref, पूर्णाङ्क dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? रिक्त
			      : ((dfunc_T *)def_functions.ga_data) + dfunc_idx;
    svar_T	    *sv;

    सचे (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	सचे (dfunc != रिक्त)
	    semsg(_(e_script_variable_invalid_after_reload_in_function_str),
					 printable_func_name(dfunc->df_ufunc));
	पटिनिवत्त रिक्त;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_data) + sref->sref_idx;
    सचे (sv->sv_name == रिक्त)
    {
	सचे (dfunc != रिक्त)
	    emsg(_(e_script_variable_was_deleted));
	पटिनिवत्त रिक्त;
    }
    सचे (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	सचे (dfunc != रिक्त)
	    emsg(_(e_script_variable_type_changed));
	पटिनिवत्त रिक्त;
    }

    सचे ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	सचे (dfunc != रिक्त)
	    semsg(_(e_item_not_exported_in_script_str), sv->sv_name);
	पटिनिवत्त रिक्त;
    }
    पटिनिवत्त sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    स्थिर char_u *
get_split_sourceline(
	पूर्णाङ्क c UNUSED,
	शून्य *cookie,
	पूर्णाङ्क indent UNUSED,
	getline_opt_T options UNUSED)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*p;
    char_u		*पङ्क्तिः;

    p = vim_strchr(sp->nextline, '\n');
    सचे (p == रिक्त)
    {
	पङ्क्तिः = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    अञ्ञथा
    {
	पङ्क्तिः = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    पटिनिवत्त पङ्क्तिः;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    स्थिर पूर्णाङ्क
call_eval_func(
	char_u	    *name,
	पूर्णाङ्क	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    पूर्णाङ्क	    called_emsg_before = called_emsg;
    पूर्णाङ्क	    res;

    res = call_by_name(name, argcount, ectx, iptr, रिक्त);
    सचे (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = find_var(name, रिक्त, FALSE);
	सचे (v == रिक्त || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    पटिनिवत्त FAIL;
	}
	पटिनिवत्त call_partial(&v->di_tv, argcount, ectx);
    }
    पटिनिवत्त res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    पूर्णाङ्क
fill_partial_and_closure(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    सचे (ufunc->uf_flags & FC_CLOSURE)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.out_राशिः = &ectx->ec_राशिः;
	pt->pt_outer.out_frame_idx = ectx->ec_frame_idx;
	सचे (ectx->ec_outer_ref != रिक्त)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.out_up = ectx->ec_outer_ref->or_outer;
	    सचे (ectx->ec_outer_ref->or_partial != रिक्त)
	    {
		pt->pt_outer.out_up_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.out_up_partial->pt_refcount;
	    }
	}

	सचे (lvi != रिक्त)
	{
	    पूर्णाङ्क	depth;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    अत्थाय (depth = 0; depth < lvi->lvi_depth; ++depth)
	    {
		pt->pt_outer.out_loop[depth].राशिः = &ectx->ec_राशिः;
		pt->pt_outer.out_loop[depth].var_idx = ectx->ec_frame_idx
			 + राशिः_FRAME_SIZE + lvi->lvi_loop[depth].var_idx;
		pt->pt_outer.out_loop[depth].var_गणनम् =
					    lvi->lvi_loop[depth].var_गणनम्;
	    }
	    pt->pt_outer.out_loop_size = lvi->lvi_depth;
	}
	अञ्ञथा
	    pt->pt_outer.out_loop_size = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	सचे (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    पटिनिवत्त FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_राशिः.ga_data) + ectx->ec_frame_idx
		       + राशिः_FRAME_SIZE + dfunc->df_varcount)->vval.v_सङ्ख्या;

	((partial_T **)ectx->ec_funcrefs.ga_data)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    पटिनिवत्त OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    स्थिर पूर्णाङ्क
exec_command(isn_T *iptr, char_u *cmd_string)
{
    source_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_FIELD(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    सचे (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	पटिनिवत्त FAIL;
    पटिनिवत्त OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    पूर्णाङ्क
may_load_script(पूर्णाङ्क sid, पूर्णाङ्क *loaded)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);

    सचे (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	सचे (do_source(si->sn_name, FALSE, DOSO_NONE, रिक्त) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    पटिनिवत्त FAIL;
	}
    }
    पटिनिवत्त OK;
}

// used for v_instr of typval of VAR_INSTR
संरचना instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
प्रकारनिर्देश संरचना subs_व्यञ्जकम्_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    पूर्णाङ्क		subs_status;
} subs_व्यञ्जकम्_T;

// Set when calling do_debug().
स्थिर ectx_T	*debug_context = रिक्त;
स्थिर पूर्णाङ्क	debug_var_गणनम्;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    पूर्णाङ्क		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    पूर्णाङ्क		    varargs_off;

    सचे (ectx == रिक्त)
	पटिनिवत्त रिक्त;
    dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    अत्थाय (idx = debug_var_गणनम् - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_names.ga_data)[idx];

	// the variable name may be NULL when not available in this block
	सचे (varname != रिक्त && STRCMP(varname, name) == 0)
	    पटिनिवत्त राशिः_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == रिक्त ? 0 : 1;
    अत्थाय (idx = 0; idx < ufunc->uf_args.ga_len; ++idx)
	सचे (STRCMP(((char_u **)(ufunc->uf_args.ga_data))[idx], name) == 0)
	    पटिनिवत्त राशिः_TV(ectx->ec_frame_idx - ufunc->uf_args.ga_len
							  - varargs_off + idx);
    सचे (ufunc->uf_va_name != रिक्त && STRCMP(ufunc->uf_va_name, name) == 0)
	पटिनिवत्त राशिः_TV(ectx->ec_frame_idx - 1);

    पटिनिवत्त रिक्त;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    पूर्णाङ्क
may_break_in_function(ufunc_T *ufunc)
{
    पटिनिवत्त ufunc->uf_has_breakpoint || debug_has_व्यञ्जकम्_breakpoint();
}

    स्थिर शून्य
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*पङ्क्तिः;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    पूर्णाङ्क		end_lnum = iptr->isn_lnum;
    garray_T	ga;
    पूर्णाङ्क		lnum;

    सचे (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	सचे (!may_break_in_function(ufunc))
	    पटिनिवत्त;

	// check for the next breakpoint if needed
	breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	सचे (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    पटिनिवत्त;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_गणनम् = iptr->isn_arg.debug.dbg_var_names_len;

    अत्थाय (ni = iptr + 1; ni->isn_type != ISN_FINISH; ++ni)
	सचे (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    end_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    भङ्ग;
	}

    सचे (end_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, परिमाण(char_u *), 10);
	अत्थाय (lnum = iptr->isn_lnum; lnum < end_lnum
				     && lnum <= ufunc->uf_पङ्क्तयः.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_पङ्क्तयः.ga_data)[lnum - 1];

	    सचे (p == रिक्त)
		अनुवर्त;  // left over from continuation line
	    p = skipwhite(p);
	    सचे (*p == '#')
		भङ्ग;
	    सचे (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_data))[ga.ga_len++] = p;
	    सचे (STRNCMP(p, "def ", 4) == 0)
		भङ्ग;
	}
	पङ्क्तिः = ga_concat_strings(&ga, "  ");
	vim_free(ga.ga_data);
    }
    अञ्ञथा
	पङ्क्तिः = ((char_u **)ufunc->uf_पङ्क्तयः.ga_data)[iptr->isn_lnum - 1];

    do_debug(पङ्क्तिः == रिक्त ? (char_u *)"[empty]" : पङ्क्तिः);
    debug_context = रिक्त;

    सचे (end_lnum > iptr->isn_lnum)
	vim_free(पङ्क्तिः);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    स्थिर पूर्णाङ्क
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    सचे (lhs_list->lv_type == रिक्त || lhs_list->lv_type->tt_member == रिक्त)
	पटिनिवत्त OK;

    पटिनिवत्त check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    स्थिर पूर्णाङ्क
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = राशिः_TV_BOT(-2);
    long	lidx = 0;
    typval_T	*tv_dest = राशिः_TV_BOT(-1);
    पूर्णाङ्क		status = OK;
    पूर्णाङ्क		check_rhs_type = FALSE;

    सचे (tv_idx->v_type == VAR_सङ्ख्या)
	lidx = (long)tv_idx->vval.v_सङ्ख्या;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = राशिः_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    सचे (dest_type == VAR_OBJECT && tv_dest->vval.v_object == रिक्त)
    {
	emsg(_(e_using_null_object));
	status = FAIL;
    }
    अञ्ञथा सचे (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	सचे (dest_type == VAR_DICT)
	    status = do_2string(tv_idx, TRUE, FALSE);
	अञ्ञथा सचे (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    सचे (obj == रिक्त)
	    {
		emsg(_(e_using_null_object));
		status = FAIL;
	    }
	    अञ्ञथा
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		पूर्णाङ्क		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		सचे (m != रिक्त)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    सचे ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == रिक्त ||
			     !class_instance_of(ufunc->uf_class, cl)))
		    {
			वर्ण *सन्देशः = (m->ocm_access == VIM_ACCESS_PRIVATE)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(सन्देशः, m->ocm_name.string, 0, cl);
			status = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    अञ्ञथा सचे (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			status = FAIL;
		    अञ्ञथा
			lidx = m_idx;
		}
		अञ्ञथा
		{
		    member_not_found_सन्देशः(cl, VAR_OBJECT, member, 0);
		    status = FAIL;
		}
	    }
	}
	अञ्ञथा सचे (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    status = FAIL;
	}
	अञ्ञथा सचे (dest_type == VAR_LIST && tv_idx->v_type != VAR_सङ्ख्या)
	{
	    emsg(_(e_सङ्ख्या_expected));
	    status = FAIL;
	}
    }

    सचे (status == OK)
    {
	सचे (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    सचे (list == रिक्त)
	    {
		emsg(_(e_list_not_set));
		पटिनिवत्त FAIL;
	    }
	    सचे (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    सचे (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_index_out_of_range_nr), lidx);
		पटिनिवत्त FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    सचे (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		पटिनिवत्त FAIL;

	    सचे (lidx < list->lv_len)
	    {
		listitem_T *li = list_find(list, lidx);
		सचे (li == रिक्त)
		    पटिनिवत्त FAIL;
		सचे (error_if_locked(li->li_tv.v_lock,
					     e_cannot_change_locked_list_item))
		    पटिनिवत्त FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    अञ्ञथा
	    {
		सचे (error_if_locked(list->lv_lock, e_cannot_change_locked_list))
		    पटिनिवत्त FAIL;
		// append to list, only fails when out of memory
		सचे (list_append_tv(list, tv) == FAIL)
		    पटिनिवत्त NOTDONE;
		clear_tv(tv);
	    }
	}
	अञ्ञथा सचे (dest_type == VAR_DICT)
	{
	    char_u		*कुञ्चिका = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    सचे (dict == रिक्त)
	    {
		emsg(_(e_dictionary_not_set));
		पटिनिवत्त FAIL;
	    }
	    सचे (कुञ्चिका == रिक्त)
		कुञ्चिका = (char_u *)"";
	    di = dict_find(dict, कुञ्चिका, -1);
	    सचे (di != रिक्त)
	    {
		सचे (error_if_locked(di->di_tv.v_lock,
						    e_cannot_change_dict_item))
		    पटिनिवत्त FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    अञ्ञथा
	    {
		सचे (error_if_locked(dict->dv_lock, e_cannot_change_dict))
		    पटिनिवत्त FAIL;
		// add to dict, only fails when out of memory
		सचे (dict_add_tv(dict, (वर्ण *)कुञ्चिका, tv) == FAIL)
		    पटिनिवत्त NOTDONE;
		clear_tv(tv);
	    }
	}
	अञ्ञथा सचे (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    पूर्णाङ्क		    error = FALSE;
	    पूर्णाङ्क		    len;

	    सचे (blob == रिक्त)
	    {
		emsg(_(e_blob_not_set));
		पटिनिवत्त FAIL;
	    }
	    len = blob_len(blob);
	    सचे (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    सचे (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), lidx);
		पटिनिवत्त FAIL;
	    }
	    सचे (मानम्_check_lock(blob->bv_lock, (char_u *)"blob", FALSE))
		पटिनिवत्त FAIL;
	    nr = tv_get_सङ्ख्या_chk(tv, &error);
	    सचे (error)
		पटिनिवत्त FAIL;
	    सचे (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_मानम्_for_blob_nr), nr);
		पटिनिवत्त FAIL;
	    }
	    blob_set_append(blob, lidx, nr);
	}
	अञ्ञथा सचे (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    सचे (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		सचे (itf != रिक्त)
		    // convert interface member index to class member index
		    lidx = object_index_from_itf_index(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    अञ्ञथा
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	अञ्ञथा सचे (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    status = FAIL;
	}
	अञ्ञथा
	{
	    status = FAIL;
	    semsg(_(e_cannot_index_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_राशिः.ga_len -= 3;
    सचे (status == FAIL)
    {
	clear_tv(tv);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

/*
 * Store a value in a list or blob range.
 */
    स्थिर पूर्णाङ्क
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = राशिः_TV_BOT(-3);
    typval_T	*tv_idx2 = राशिः_TV_BOT(-2);
    typval_T	*tv_dest = राशिः_TV_BOT(-1);
    पूर्णाङ्क		status = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = राशिः_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    सचे (tv_dest->v_type == VAR_LIST)
    {
	long	    n1;
	long	    n2;
	listitem_T  *li1;

	n1 = (long)tv_get_सङ्ख्या_chk(tv_idx1, रिक्त);
	सचे (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_सङ्ख्या == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	अञ्ञथा
	    n2 = (long)tv_get_सङ्ख्या_chk(tv_idx2, रिक्त);

	li1 = check_range_index_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	सचे (li1 == रिक्त)
	    status = FAIL;
	अञ्ञथा
	{
	    status = check_range_index_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    सचे (status != FAIL)
		status = list_assign_range(
			tv_dest->vval.v_list,
			tv->vval.v_list,
			n1,
			n2,
			tv_idx2->v_type == VAR_SPECIAL,
			(char_u *)"=",
			(char_u *)"[unknown]");
	}
    }
    अञ्ञथा सचे (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	long	    bloblen;

	n1 = tv_get_सङ्ख्या_chk(tv_idx1, रिक्त);
	सचे (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_सङ्ख्या == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	अञ्ञथा
	    n2 = tv_get_सङ्ख्या_chk(tv_idx2, रिक्त);
	bloblen = blob_len(tv_dest->vval.v_blob);

	सचे (check_blob_index(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    status = FAIL;
	अञ्ञथा
	    status = blob_set_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    अञ्ञथा
    {
	status = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_राशिः.ga_len -= 4;
    clear_tv(tv);

    पटिनिवत्त status;
}

/*
 * Unlet item in list or dict variable.
 */
    स्थिर पूर्णाङ्क
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = राशिः_TV_BOT(-2);
    typval_T	*tv_dest = राशिः_TV_BOT(-1);
    पूर्णाङ्क		status = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    सचे (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	सचे (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_सङ्ख्या)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    status = FAIL;
	}
	अञ्ञथा
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*कुञ्चिका;
	    dictitem_T  *di = रिक्त;

	    सचे (d != रिक्त && मानम्_check_lock(
				      d->dv_lock, रिक्त, FALSE))
		status = FAIL;
	    अञ्ञथा
	    {
		सचे (tv_idx->v_type == VAR_STRING)
		{
		    कुञ्चिका = tv_idx->vval.v_string;
		    सचे (कुञ्चिका == रिक्त)
			कुञ्चिका = (char_u *)"";
		}
		अञ्ञथा
		{
		    कुञ्चिका = tv_get_string(tv_idx);
		}
		सचे (d != रिक्त)
		    di = dict_find(d, कुञ्चिका, (पूर्णाङ्क)STRLEN(कुञ्चिका));
		सचे (di == रिक्त)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_कुञ्चिका_not_present_in_dictionary_str), कुञ्चिका);
		    status = FAIL;
		}
		अञ्ञथा सचे (var_check_fixed(di->di_flags,
						   रिक्त, FALSE)
			|| var_check_ro(di->di_flags,
						  रिक्त, FALSE))
		    status = FAIL;
		अञ्ञथा
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    अञ्ञथा सचे (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	सचे (check_for_सङ्ख्या(tv_idx) == FAIL)
	{
	    status = FAIL;
	}
	अञ्ञथा
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n = (long)tv_idx->vval.v_सङ्ख्या;

	    सचे (l != रिक्त && मानम्_check_lock(
				      l->lv_lock, रिक्त, FALSE))
		status = FAIL;
	    अञ्ञथा
	    {
		listitem_T	*li = list_find(l, n);

		सचे (li == रिक्त)
		{
		    semsg(_(e_list_index_out_of_range_nr), n);
		    status = FAIL;
		}
		अञ्ञथा
		    listitem_remove(l, li);
	    }
	}
    }
    अञ्ञथा
    {
	status = FAIL;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_राशिः.ga_len -= 2;

    पटिनिवत्त status;
}

/*
 * Unlet a range of items in a list variable.
 */
    स्थिर पूर्णाङ्क
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = राशिः_TV_BOT(-3);
    typval_T	*tv_idx2 = राशिः_TV_BOT(-2);
    typval_T	*tv_dest = राशिः_TV_BOT(-1);
    पूर्णाङ्क		status = OK;

    सचे (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	सचे (check_for_सङ्ख्या(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_सङ्ख्या(tv_idx2) == FAIL))
	{
	    status = FAIL;
	}
	अञ्ञथा
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n1 = (long)tv_idx1->vval.v_सङ्ख्या;
	    long	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (long)tv_idx2->vval.v_सङ्ख्या;
	    listitem_T	*li;

	    li = list_find_index(l, &n1);
	    सचे (li == रिक्त)
	    {
		semsg(_(e_list_index_out_of_range_nr),
						 (long)tv_idx1->vval.v_सङ्ख्या);
		status = FAIL;
	    }
	    अञ्ञथा
	    {
		सचे (n1 < 0)
		    n1 = list_idx_of_item(l, li);
		सचे (n2 < 0)
		{
		    listitem_T *li2 = list_find(l, n2);

		    सचे (li2 == रिक्त)
		    {
			semsg(_(e_list_index_out_of_range_nr), n2);
			status = FAIL;
		    }
		    अञ्ञथा
			n2 = list_idx_of_item(l, li2);
		}
		सचे (status != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_index_out_of_range_nr), n2);
		    status = FAIL;
		}
		सचे (status != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    अञ्ञथा
    {
	status = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_राशिः.ga_len -= 3;

    पटिनिवत्त status;
}

/*
 * Top of a for loop.
 */
    स्थिर पूर्णाङ्क
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    पूर्णाङ्क		jump = FALSE;
    typval_T	*ltv = राशिः_TV_BOT(-1);
    typval_T	*idxtv =
		   राशिः_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
	पटिनिवत्त FAIL;
    सचे (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_सङ्ख्या;
	सचे (list == रिक्त
		       || idxtv->vval.v_सङ्ख्या >= list->lv_len)
	{
	    jump = TRUE;
	}
	अञ्ञथा सचे (list->lv_first == &range_list_item)
	{
	    // non-materialized range() list
	    tv = राशिः_TV_BOT(0);
	    tv->v_type = VAR_सङ्ख्या;
	    tv->v_lock = 0;
	    tv->vval.v_सङ्ख्या = list_find_nr(
			     list, idxtv->vval.v_सङ्ख्या, रिक्त);
	    ++ectx->ec_राशिः.ga_len;
	}
	अञ्ञथा
	{
	    listitem_T *li = list_find(list,
					 idxtv->vval.v_सङ्ख्या);
	    सचे (li == रिक्त)
		पटिनिवत्त FAIL;
	    copy_tv(&li->li_tv, राशिः_TV_BOT(0));
	    ++ectx->ec_राशिः.ga_len;
	}
    }
    अञ्ञथा सचे (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_सङ्ख्या;
	सचे (tuple == रिक्त || idxtv->vval.v_सङ्ख्या >= TUPLE_LEN(tuple))
	    jump = TRUE;
	अञ्ञथा
	{
	    copy_tv(TUPLE_ITEM(tuple, idxtv->vval.v_सङ्ख्या), राशिः_TV_BOT(0));
	    ++ectx->ec_राशिः.ga_len;
	}
    }
    अञ्ञथा सचे (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_सङ्ख्या;
	सचे (str == रिक्त || str[idxtv->vval.v_सङ्ख्या] == NUL)
	{
	    jump = TRUE;
	}
	अञ्ञथा
	{
	    पूर्णाङ्क	clen = mb_ptr2len(str + idxtv->vval.v_सङ्ख्या);

	    // Push the next character from the string.
	    tv = राशिः_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_सङ्ख्या, clen);
	    ++ectx->ec_राशिः.ga_len;
	    idxtv->vval.v_सङ्ख्या += clen - 1;
	}
    }
    अञ्ञथा सचे (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	सचे (idxtv->vval.v_सङ्ख्या == -1 && blob != रिक्त)
	{
	    ltv->v_lock = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_सङ्ख्या;
	सचे (blob == रिक्त || idxtv->vval.v_सङ्ख्या >= blob_len(blob))
	{
	    jump = TRUE;
	}
	अञ्ञथा
	{
	    // Push the next byte from the blob.
	    tv = राशिः_TV_BOT(0);
	    tv->v_type = VAR_सङ्ख्या;
	    tv->vval.v_सङ्ख्या = blob_get(blob,
					 idxtv->vval.v_सङ्ख्या);
	    ++ectx->ec_राशिः.ga_len;
	}
    }
    अञ्ञथा
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	पटिनिवत्त FAIL;
    }

    सचे (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_end;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    अञ्ञथा
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = राशिः_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_सङ्ख्या = ectx->ec_funcrefs.ga_len;
    }

    पटिनिवत्त OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
स्थिर loopvars_T *first_loopvars = रिक्त;

    स्थिर शून्य
add_loopvars_to_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    सचे (first_loopvars != रिक्त)
	first_loopvars->lvs_prev = loopvars;
    loopvars->lvs_next = first_loopvars;
    loopvars->lvs_prev = रिक्त;
    first_loopvars = loopvars;
}

    स्थिर शून्य
remove_loopvars_from_list(loopvars_T *loopvars)
{
    सचे (loopvars->lvs_prev == रिक्त)
	first_loopvars = loopvars->lvs_next;
    अञ्ञथा
	loopvars->lvs_prev->lvs_next = loopvars->lvs_next;
    सचे (loopvars->lvs_next != रिक्त)
	loopvars->lvs_next->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    स्थिर पूर्णाङ्क
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = राशिः_TV_VAR(endloop->end_funcref_idx);
    पूर्णाङ्क		prev_closure_गणनम् = tv_refcount->vval.v_सङ्ख्या;
    पूर्णाङ्क		depth = endloop->end_depth;
    garray_T	*gap = &ectx->ec_funcrefs;
    पूर्णाङ्क		closure_in_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *राशिः;
    पूर्णाङ्क		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    अत्थाय (idx = prev_closure_गणनम्; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	सचे (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == रिक्त)
	{
	    पूर्णाङ्क refcount = pt->pt_refcount;
	    पूर्णाङ्क i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    अत्थाय (i = 0; i < endloop->end_var_गणनम्; ++i)
	    {
		typval_T *stv = राशिः_TV_VAR(endloop->end_var_idx + i);

		सचे (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    सचे (refcount > 1)
	    {
		closure_in_use = TRUE;
		भङ्ग;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    सचे (!closure_in_use)
	पटिनिवत्त OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    सचे (loopvars == रिक्त)
	पटिनिवत्त FAIL;

    loopvars->lvs_ga.ga_len = endloop->end_var_गणनम्;
    राशिः = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_data = राशिः;
    सचे (राशिः == रिक्त)
    {
	vim_free(loopvars);
	पटिनिवत्त FAIL;
    }
    add_loopvars_to_list(loopvars);

    // Move the variable values.
    अत्थाय (idx = 0; idx < endloop->end_var_गणनम्; ++idx)
    {
	typval_T *tv = राशिः_TV_VAR(endloop->end_var_idx + idx);

	*(राशिः + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    अत्थाय (idx = prev_closure_गणनम्; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	सचे (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == रिक्त)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[depth] = loopvars;

	    pt->pt_outer.out_loop[depth].राशिः = &loopvars->lvs_ga;
	    pt->pt_outer.out_loop[depth].var_idx -=
		  ectx->ec_frame_idx + राशिः_FRAME_SIZE + endloop->end_var_idx;
	}
    }

    पटिनिवत्त OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    पूर्णाङ्क
loopvars_check_refcount(loopvars_T *loopvars)
{
    पूर्णाङ्क		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    पूर्णाङ्क		    done = 0;
	typval_T	*राशिः = gap->ga_data;

    सचे (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	पटिनिवत्त FALSE;
    अत्थाय (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_data) + i;

	सचे (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != रिक्त
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    पूर्णाङ्क	    depth;

	    अत्थाय (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		सचे (tv->vval.v_partial->pt_loopvars[depth] == loopvars)
		    ++done;
	}
    }
    सचे (done != loopvars->lvs_min_refcount)
	पटिनिवत्त FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    राशिः = gap->ga_data;
    अत्थाय (i = 0; i < gap->ga_len; ++i)
	clear_tv(राशिः + i);
    vim_free(राशिः);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    पटिनिवत्त TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    पूर्णाङ्क
set_ref_in_loopvars(पूर्णाङ्क copyID)
{
    loopvars_T *loopvars;

    अत्थाय (loopvars = first_loopvars; loopvars != रिक्त;
						 loopvars = loopvars->lvs_next)
    {
	typval_T    *राशिः = loopvars->lvs_ga.ga_data;
	पूर्णाङ्क	    i;

	अत्थाय (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    सचे (set_ref_in_item(राशिः + i, copyID, रिक्त, रिक्त, रिक्त))
		पटिनिवत्त TRUE;  // abort
    }
    पटिनिवत्त FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    स्थिर पूर्णाङ्क
load_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = रिक्त;
    hashtab_T	*ht = रिक्त;
    वर्ण	namespace;

    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
	पटिनिवत्त NOTDONE;
    विकल्प (isn_type)
    {
	पक्ष ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    भङ्ग;
	पक्ष ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    भङ्ग;
	पक्ष ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    भङ्ग;
	पक्ष ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    भङ्ग;
	स्वभाव:  // Cannot reach here
	    पटिनिवत्त NOTDONE;
    }
    di = find_var_in_ht(ht, 0, iptr->isn_arg.string, TRUE);

    सचे (di == रिक्त)
    {
	सचे (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = find_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    सचे (ufunc != रिक्त)
	    {
		typval_T    *tv = राशिः_TV_BOT(0);

		++ectx->ec_राशिः.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		सचे (tv->vval.v_string == रिक्त)
		    पटिनिवत्त FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		पटिनिवत्त OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	सचे (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != रिक्त)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_item_not_found_in_script_str), iptr->isn_arg.string);
	अञ्ञथा
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = राशिः_TV_BOT(0);
	tv->v_type = VAR_सङ्ख्या;
	tv->v_lock = 0;
	tv->vval.v_सङ्ख्या = 0;
	++ectx->ec_राशिः.ga_len;
	पटिनिवत्त FAIL;
    }
    अञ्ञथा
    {
	copy_tv(&di->di_tv, राशिः_TV_BOT(0));
	++ectx->ec_राशिः.ga_len;
    }
    पटिनिवत्त OK;
}


    स्थिर शून्य
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, परिमाण(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    वर्ण *tofree = रिक्त;
    वर्ण *typename = type_name(type, &tofree);
    semsg(_(e_object_required_found_str), typename);
    vim_free(tofree);
    clear_type_list(&type_list);
}

/*
 * Accessing the variable or method of an object or a class stored in a
 * variable of type "any".
 * Returns OK if the member variable is present.
 * Returns FAIL if the variable is not found.
 */
    स्थिर पूर्णाङ्क
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    पूर्णाङ्क		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = रिक्त;
    typval_T	mtv;

    सचे (is_object)
    {
	obj = tv->vval.v_object;
	सचे (obj == रिक्त)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    पटिनिवत्त FAIL;
	}
	tv_cl = obj->obj_class;
    }
    अञ्ञथा
    {
	tv_cl = tv->vval.v_class;
	सचे (tv_cl == रिक्त)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    पटिनिवत्त FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    पूर्णाङ्क		namelen = (पूर्णाङ्क)STRLEN(iptr->isn_arg.string);
    पूर्णाङ्क		save_did_emsg = did_emsg;

    सचे (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	पटिनिवत्त OK;
    }

    सचे (did_emsg != save_did_emsg)
	पटिनिवत्त FAIL;

    // could be a class or instance method
    ufunc_T	*oc_method;
    पूर्णाङ्क		oc_method_idx;

    oc_method = method_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_method_idx);
    सचे (oc_method == रिक्त)
    {
	वर्ण	*सन्देशः;

	SOURCING_LNUM = iptr->isn_lnum;
	सचे (is_object)
	    सन्देशः = e_variable_not_found_on_object_str_str;
	अञ्ञथा
	    सन्देशः = e_class_variable_str_not_found_in_class_str;
	semsg(_(सन्देशः), iptr->isn_arg.string, tv_cl->class_name.string);
	पटिनिवत्त FAIL;
    }

    // Protected methods are not accessible outside the class
    सचे (*oc_method->uf_name == '_'
			&& !class_instance_of(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_method_str), oc_method->uf_name);
	पटिनिवत्त FAIL;
    }

    // Create a partial for the instance or class method
    सचे (obj_method_to_partial_tv(is_object ? obj : रिक्त, oc_method, tv)
								== FAIL)
	पटिनिवत्त FAIL;

    पटिनिवत्त OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    स्थिर शून्य
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, पूर्णाङ्क fixindent)
{
    पूर्णाङ्क		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*व्यञ्जकम् = रिक्त;
    पूर्णाङ्क		dir = FORWARD;

    सचे (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = राशिः_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_सङ्ख्या;
	सचे (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_राशिः.ga_len;
    }
    अञ्ञथा सचे (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    अञ्ञथा सचे (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	सचे (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    सचे (regname == '=')
    {
	tv = राशिः_TV_BOT(-1);
	सचे (tv->v_type == VAR_STRING)
	    व्यञ्जकम् = tv->vval.v_string;
	अञ्ञथा
	{
	    व्यञ्जकम् = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_राशिः.ga_len;
    }
    check_cursor();

    सचे (fixindent)
	do_put(regname, व्यञ्जकम्, dir, 1L, PUT_पङ्क्तिः|PUT_CURSLINE|PUT_FIXINDENT);
    अञ्ञथा
	do_put(regname, व्यञ्जकम्, dir, 1L, PUT_पङ्क्तिः|PUT_CURSLINE);
    vim_free(व्यञ्जकम्);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    स्थिर पूर्णाङ्क
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    पूर्णाङ्क		गणनम् = iptr->isn_arg.unpack.unp_गणनम्;
    पूर्णाङ्क		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    पूर्णाङ्क		i;

    l = tv->vval.v_list;
    सचे (l == रिक्त
	    || l->lv_len < (semicolon ? गणनम् - 1 : गणनम्))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_मानम्_does_not_have_enough_वस्तूनि));
	पटिनिवत्त EXEC_FAIL;
    }
    अञ्ञथा सचे (!semicolon && l->lv_len > गणनम्)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_मानम्_has_more_वस्तूनि_than_targets));
	पटिनिवत्त EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, गणनम् - 1))
	पटिनिवत्त EXEC_DONE;
    ectx->ec_राशिः.ga_len += गणनम् - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    सचे (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_वस्तूनि(l->lv_len - गणनम् + 1);

	सचे (rem_list == रिक्त)
	    पटिनिवत्त EXEC_DONE;
	tv = राशिः_TV_BOT(-गणनम्);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	li = l->lv_first;
	अत्थाय (i = 0; i < गणनम् - 1; ++i)
	    li = li->li_next;
	अत्थाय (i = 0; li != रिक्त; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_set_item(rem_list, i, &tvcopy);
	    li = li->li_next;
	}
	--गणनम्;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_first;
    अत्थाय (i = 0; i < गणनम्; ++i)
    {
	tv = राशिः_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_next;
    }

    list_unref(l);

    पटिनिवत्त EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    स्थिर पूर्णाङ्क
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    पूर्णाङ्क		गणनम् = iptr->isn_arg.unpack.unp_गणनम्;
    पूर्णाङ्क		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    पूर्णाङ्क		i;

    tuple = tv->vval.v_tuple;
    सचे (tuple == रिक्त
	    || TUPLE_LEN(tuple) < (semicolon ? गणनम् - 1 : गणनम्))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_more_targets_than_tuple_वस्तूनि));
	पटिनिवत्त EXEC_FAIL;
    }
    अञ्ञथा सचे (!semicolon && TUPLE_LEN(tuple) > गणनम्)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_वस्तूनि));
	पटिनिवत्त EXEC_FAIL;
    }

    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, गणनम् - 1))
	पटिनिवत्त EXEC_DONE;
    ectx->ec_राशिः.ga_len += गणनम् - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    सचे (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_वस्तूनि(TUPLE_LEN(tuple) - गणनम् + 1);

	सचे (rem_list == रिक्त)
	    पटिनिवत्त EXEC_DONE;
	tv = राशिः_TV_BOT(-गणनम्);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	अत्थाय (i = गणनम् - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ITEM(tuple, i), &tvcopy);
	    list_set_item(rem_list, i - (गणनम् - 1), &tvcopy);
	}
	--गणनम्;
    }

    // Produce the values in reverse order, first item last.
    अत्थाय (i = 0; i < गणनम्; ++i)
    {
	tv = राशिः_TV_BOT(-i - 1);
	copy_tv(TUPLE_ITEM(tuple, i), tv);
    }

    tuple_unref(tuple);

    पटिनिवत्त EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    स्थिर पूर्णाङ्क
exec_instructions(ectx_T *ectx)
{
    पूर्णाङ्क		ret = FAIL;
    पूर्णाङ्क		save_trylevel_at_start = ectx->ec_trylevel_at_start;
    पूर्णाङ्क		dict_राशिः_len_at_start = dict_राशिः.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_start = trylevel;

    अत्थाय (;;)
    {
	स्थिर पूर्णाङ्क  breakcheck_गणनम् = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	सचे (unlikely(++breakcheck_गणनम् >= 100))
	{
	    पङ्क्तिः_breakcheck();
	    breakcheck_गणनम् = 0;
	}
	सचे (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    सचे (throw_exception("Vim:Interrupt", ET_INTERRUPT, रिक्त) == FAIL)
		गच्छ theend;
	    did_throw = TRUE;
	}

	सचे (unlikely(did_emsg && सन्देशः_list != रिक्त && *सन्देशः_list != रिक्त))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    सचे (throw_exception(*सन्देशः_list, ET_ERROR, रिक्त) == FAIL)
		गच्छ theend;
	    did_throw = TRUE;
	    *सन्देशः_list = रिक्त;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    सचे (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		सचे (trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		{
		    सचे (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			finish_exception(caught_राशिः);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	सचे (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = रिक्त;
	    पूर्णाङ्क		index = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    याव (index > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_data) + index - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		सचे (!trycmd->tcd_in_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    भङ्ग;
		// In the catch and finally block of this try we have to go up
		// one level.
		--index;
		trycmd = रिक्त;
	    }
	    सचे (trycmd != रिक्त && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
	    {
		सचे (trycmd->tcd_in_catch)
		{
		    सचे (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    अञ्ञथा
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		अञ्ञथा
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_in_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    अञ्ञथा
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    गच्छ theend;
		tv = राशिः_TV_BOT(0);
		tv->v_type = VAR_सङ्ख्या;
		tv->vval.v_सङ्ख्या = 0;
		++ectx->ec_राशिः.ga_len;
		सचे (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    सचे (handle_closure_in_use(ectx, FALSE) == FAIL)
			गच्छ theend;
		    गच्छ done;
		}

		सचे (func_return(ectx) == FAIL)
		    गच्छ theend;
	    }
	    अनुवर्त;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	विकल्प (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    पक्ष ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = राशिः_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		भङ्ग;

	    // execute Ex command line
	    पक्ष ISN_EXEC:
		सचे (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    गच्छ on_error;
		भङ्ग;

	    // execute Ex command line split at NL characters.
	    पक्ष ISN_EXEC_SPLIT:
		{
		    source_cookie_T cookie;
		    char_u	    *पङ्क्तिः;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    पङ्क्तिः = get_split_sourceline(0, &cookie, 0, 0);
		    सचे (do_cmdline(पङ्क्तिः,
				get_split_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(पङ्क्तिः);
			गच्छ on_error;
		    }
		    vim_free(पङ्क्तिः);
		}
		भङ्ग;

	    // execute Ex command line that is only a range
	    पक्ष ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    वर्ण	*error = रिक्त;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = CMD_SIZE;
		    ea.addr_type = ADDR_पङ्क्तयः;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_address(&ea, &error, FALSE);
		    सचे (ea.cmd == रिक्त)
			गच्छ on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    सचे (error != रिक्त)
		    {
			emsg(error);
			गच्छ on_error;
		    }
		}
		भङ्ग;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    पक्ष ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    पूर्णाङ्क	    res;
		    पूर्णाङ्क	    save_flags = cmdmod.cmod_flags;

		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    tv = राशिः_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, रिक्त, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    सचे (res == FAIL)
			गच्छ on_error;
		    ++ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    // push typeval VAR_INSTR with instructions to be executed
	    पक्ष ISN_INSTR:
		{
		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    tv = राशिः_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    सचे (tv->vval.v_instr == रिक्त)
			गच्छ on_error;
		    ++ectx->ec_राशिः.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		भङ्ग;

	    पक्ष ISN_SOURCE:
		{
		    पूर्णाङ्क notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (may_load_script((पूर्णाङ्क)iptr->isn_arg.सङ्ख्या, &notused)
								       == FAIL)
			गच्छ on_error;
		}
		भङ्ग;

	    // execute :substitute with an expression
	    पक्ष ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    source_cookie_T	cookie;
		    संरचना subs_व्यञ्जकम्_S	*save_instr = substitute_instr;
		    संरचना subs_व्यञ्जकम्_S	subs_instr;
		    पूर्णाङ्क			res;

		    subs_instr.subs_ectx = ectx;
		    subs_instr.subs_instr = subs->subs_instr;
		    subs_instr.subs_status = OK;
		    substitute_instr = &subs_instr;

		    SOURCING_LNUM = iptr->isn_lnum;
		    // This is very much like ISN_EXEC
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    res = do_cmdline(subs->subs_cmd,
				getsourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
		    substitute_instr = save_instr;

		    सचे (res == FAIL || did_emsg
					     || subs_instr.subs_status == FAIL)
			गच्छ on_error;
		}
		भङ्ग;

	    पक्ष ISN_FINISH:
		गच्छ done;

	    पक्ष ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		सचे (alloc_redir_lval() == FAIL)
		    गच्छ on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		भङ्ग;

	    पक्ष ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    {
			vim_free(res);
			गच्छ theend;
		    }
		    tv = राशिः_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    पक्ष ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		force_abort = TRUE;
		सचे (trigger_cexpr_autocmd(iptr->isn_arg.सङ्ख्या) == FAIL)
		    गच्छ on_error;
		force_abort = FALSE;
#endif
		भङ्ग;

	    पक्ष ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    exarg_T ea;
		    पूर्णाङ्क	    res;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_राशिः.ga_len;
		    tv = राशिः_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    सचे (res == FAIL)
			गच्छ on_error;
		}
#endif
		भङ्ग;

	    // execute Ex command from pieces on the stack
	    पक्ष ISN_EXECCONCAT:
		{
		    पूर्णाङ्क	    गणनम् = iptr->isn_arg.सङ्ख्या;
		    size_t  len = 0;
		    पूर्णाङ्क	    pass;
		    पूर्णाङ्क	    i;
		    char_u  *cmd = रिक्त;
		    char_u  *str;

		    अत्थाय (pass = 1; pass <= 2; ++pass)
		    {
			अत्थाय (i = 0; i < गणनम्; ++i)
			{
			    tv = राशिः_TV_BOT(i - गणनम्);
			    str = tv->vval.v_string;
			    सचे (str != रिक्त && *str != NUL)
			    {
				सचे (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    सचे (pass == 2)
				clear_tv(tv);
			}
			सचे (pass == 1)
			{
			    cmd = alloc(len + 1);
			    सचे (unlikely(cmd == रिक्त))
				गच्छ theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		भङ्ग;

	    // execute :echo {string} ...
	    पक्ष ISN_ECHO:
		{
		    पूर्णाङ्क गणनम् = iptr->isn_arg.echo.echo_गणनम्;
		    पूर्णाङ्क	atstart = TRUE;
		    पूर्णाङ्क needclr = TRUE;
		    पूर्णाङ्क	idx;

		    अत्थाय (idx = 0; idx < गणनम्; ++idx)
		    {
			tv = राशिः_TV_BOT(idx - गणनम्);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    सचे (needclr)
			सन्देशः_clr_eos();
		    ectx->ec_राशिः.ga_len -= गणनम्;
		}
		भङ्ग;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    पक्ष ISN_EXECUTE:
	    पक्ष ISN_ECHOMSG:
	    पक्ष ISN_ECHOWINDOW:
	    पक्ष ISN_ECHOCONSOLE:
	    पक्ष ISN_ECHOERR:
		{
		    पूर्णाङ्क		गणनम्;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    पूर्णाङ्क		len;
		    पूर्णाङ्क		failed = FALSE;
		    पूर्णाङ्क		idx;

		    सचे (iptr->isn_type == ISN_ECHOWINDOW)
			गणनम् = iptr->isn_arg.echowin.ewin_गणनम्;
		    अञ्ञथा
			गणनम् = iptr->isn_arg.सङ्ख्या;
		    ga_init2(&ga, 1, 80);
		    अत्थाय (idx = 0; idx < गणनम्; ++idx)
		    {
			tv = राशिः_TV_BOT(idx - गणनम्);
			सचे (iptr->isn_type == ISN_EXECUTE)
			{
			    सचे (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_JOB)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_मानम्_as_string_str),
						    vartype_name(tv->v_type));
				भङ्ग;
			    }
			    अञ्ञथा
				p = tv_get_string_buf(tv, buf);
			}
			अञ्ञथा
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (पूर्णाङ्क)STRLEN(p);
			सचे (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			अञ्ञथा
			{
			    सचे (ga.ga_len > 0)
				((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_राशिः.ga_len -= गणनम्;
		    सचे (failed)
		    {
			ga_clear(&ga);
			गच्छ on_error;
		    }

		    सचे (ga.ga_data != रिक्त)
		    {
			सचे (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_data);
			    सचे (did_emsg)
			    {
				ga_clear(&ga);
				गच्छ on_error;
			    }
			}
			अञ्ञथा
			{
			    सन्देशः_sb_eol();
			    सचे (iptr->isn_type == ISN_ECHOMSG)
			    {
				सन्देशः_attr(ga.ga_data, echo_attr);
				out_flush();
			    }
#ifdef HAS_MESSAGE_WINDOW
			    अञ्ञथा सचे (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				start_echowindow(
					      iptr->isn_arg.echowin.ewin_time);
				सन्देशः_attr(ga.ga_data, echo_attr);
				end_echowindow();
			    }
#endif
			    अञ्ञथा सचे (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_data, (पूर्णाङ्क)STRLEN(ga.ga_data),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    अञ्ञथा
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_data);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		भङ्ग;

	    // load local variable or argument
	    पक्ष ISN_LOAD:
		सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    गच्छ theend;
		tv = राशिः_TV_VAR(iptr->isn_arg.सङ्ख्या);
		सचे (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    राशिः_TV_BOT(0)->v_type = VAR_SPECIAL;
		    राशिः_TV_BOT(0)->vval.v_सङ्ख्या = VVAL_NONE;
		}
		अञ्ञथा
		    copy_tv(tv, राशिः_TV_BOT(0));
		++ectx->ec_राशिः.ga_len;
		भङ्ग;

	    // load v: variable
	    पक्ष ISN_LOADV:
		सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    गच्छ theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.सङ्ख्या), राशिः_TV_BOT(0));
		++ectx->ec_राशिः.ga_len;
		भङ्ग;

	    // load s: variable in Vim9 script
	    पक्ष ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    सचे (sv == रिक्त)
			गच्छ theend;
		    allocate_if_null(sv);
		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    copy_tv(sv->sv_tv, राशिः_TV_BOT(0));
		    ++ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    // load s: variable in old script or autoload import
	    पक्ष ISN_LOADS:
	    पक्ष ISN_LOADEXPORT:
		{
		    पूर्णाङ्क		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = find_var_in_ht(ht, 0, name, TRUE);

		    सचे (di == रिक्त)
		    {
			ufunc_T	*ufunc = रिक्त;

			सचे (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (शून्य)find_exported(sid, name, &ufunc, &type,
							 रिक्त, रिक्त, FALSE);
			}
			सचे (ufunc == रिक्त)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    गच्छ on_error;
			}
			सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			    गच्छ theend;
			tv = राशिः_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_राशिः.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    अञ्ञथा
		    {
			सचे (iptr->isn_type == ISN_LOADEXPORT)
			{
			    पूर्णाङ्क idx = get_script_item_idx(sid, name, 0,
								   रिक्त, रिक्त);
			    svar_T	*sv;

			    सचे (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;
				सचे ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    गच्छ on_error;
				}
			    }
			}
			सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			    गच्छ theend;
			copy_tv(&di->di_tv, राशिः_TV_BOT(0));
			++ectx->ec_राशिः.ga_len;
		    }
		}
		भङ्ग;

	    // load g:/b:/w:/t: variable
	    पक्ष ISN_LOADG:
	    पक्ष ISN_LOADB:
	    पक्ष ISN_LOADW:
	    पक्ष ISN_LOADT:
		{
		    पूर्णाङ्क res = load_namespace_var(ectx, iptr->isn_type, iptr);

		    सचे (res == NOTDONE)
			गच्छ theend;
		    सचे (res == FAIL)
			गच्छ on_error;
		}

		भङ्ग;

	    // load autoload variable
	    पक्ष ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (eval_variable(name, (पूर्णाङ्क)STRLEN(name), 0,
			      राशिः_TV_BOT(0), रिक्त, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = राशिः_TV_BOT(0);
			tv->v_type = VAR_सङ्ख्या;
			tv->v_lock = 0;
			tv->vval.v_सङ्ख्या = 0;
			++ectx->ec_राशिः.ga_len;
			गच्छ on_error;
		    }
		    ++ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    // load g:/b:/w:/t: namespace
	    पक्ष ISN_LOADGDICT:
	    पक्ष ISN_LOADBDICT:
	    पक्ष ISN_LOADWDICT:
	    पक्ष ISN_LOADTDICT:
		{
		    dict_T *d = रिक्त;

		    विकल्प (iptr->isn_type)
		    {
			पक्ष ISN_LOADGDICT: d = get_globvar_dict(); भङ्ग;
			पक्ष ISN_LOADBDICT: d = curbuf->b_vars; भङ्ग;
			पक्ष ISN_LOADWDICT: d = curwin->w_vars; भङ्ग;
			पक्ष ISN_LOADTDICT: d = curtab->tp_vars; भङ्ग;
			स्वभाव:  // Cannot reach here
			    गच्छ theend;
		    }
		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    tv = राशिः_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_lock = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    // load &option
	    पक्ष ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    सचे (eval_option(&name, &optval, TRUE) == FAIL)
			गच्छ theend;
		    *राशिः_TV_BOT(0) = optval;
		    ++ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    // load $ENV
	    पक्ष ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    // name is always valid, checked when compiling
		    (शून्य)eval_env_var(&name, &optval, TRUE);
		    *राशिः_TV_BOT(0) = optval;
		    ++ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    // load @register
	    पक्ष ISN_LOADREG:
		सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    गच्छ theend;
		tv = राशिः_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.सङ्ख्या, GREG_व्यञ्जकम्_SRC);
		++ectx->ec_राशिः.ga_len;
		भङ्ग;

	    // store local variable
	    पक्ष ISN_STORE:
		--ectx->ec_राशिः.ga_len;
		tv = राशिः_TV_VAR(iptr->isn_arg.सङ्ख्या);
		सचे (check_typval_is_मानम्(राशिः_TV_BOT(0)) == FAIL
			|| राशिः_TV_BOT(0)->v_type == VAR_VOID)
		{
		    सचे (राशिः_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_मानम्));
		    clear_tv(राशिः_TV_BOT(0));
		    गच्छ on_error;
		}
		clear_tv(tv);
		*tv = *राशिः_TV_BOT(0);
		भङ्ग;

	    // store s: variable in old script or autoload import
	    पक्ष ISN_STORES:
	    पक्ष ISN_STOREEXPORT:
		{
		    पूर्णाङ्क		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = रिक्त;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    सचे (iptr->isn_type == ISN_STOREEXPORT)
			di = find_var_autoload_prefix(name, sid, रिक्त, रिक्त);
		    // Then look for a variable in the script's variables.
		    सचे (di == रिक्त)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = find_var_in_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_राशिः.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (di == रिक्त)
		    {
			सचे (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(राशिः_TV_BOT(0));
			    गच्छ on_error;
			}
			store_var(name, राशिः_TV_BOT(0));
		    }
		    अञ्ञथा
		    {
			सचे (iptr->isn_type == ISN_STOREEXPORT)
			{
			    पूर्णाङ्क idx = get_script_item_idx(sid, name, 0,
								   रिक्त, रिक्त);

			    // can this ever fail?
			    सचे (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;

				सचे ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    clear_tv(राशिः_TV_BOT(0));
				    गच्छ on_error;
				}
			    }
			}
			सचे (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(राशिः_TV_BOT(0));
			    गच्छ on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *राशिः_TV_BOT(0);
		    }
		}
		भङ्ग;

	    // store script-local variable in Vim9 script
	    पक्ष ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    सचे (sv == रिक्त)
			गच्छ theend;
		    --ectx->ec_राशिः.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (मानम्_check_lock(sv->sv_tv->v_lock, sv->sv_name, FALSE))
		    {
			clear_tv(राशिः_TV_BOT(0));
			गच्छ on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *राशिः_TV_BOT(0);
		}
		भङ्ग;

	    // store option
	    पक्ष ISN_STOREOPT:
	    पक्ष ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    पूर्णाङ्क		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    long	n = 0;
		    char_u	*s = रिक्त;
		    वर्ण	*सन्देशः;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = रिक्त;

		    --ectx->ec_राशिः.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = राशिः_TV_BOT(0);
		    सचे (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			सचे (s == रिक्त)
			    s = (char_u *)"";
		    }
		    अञ्ञथा सचे (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			सचे (s == रिक्त || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    गच्छ on_error;
			}
		    }
		    अञ्ञथा
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_सङ्ख्या;
		    सन्देशः = set_option_मानम्(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    सचे (सन्देशः != रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(सन्देशः));
			गच्छ on_error;
		    }
		}
		भङ्ग;

	    // store $ENV
	    पक्ष ISN_STOREENV:
		सचे (check_secure() || check_restricted())
		    गच्छ theend;
		--ectx->ec_राशिः.ga_len;
		tv = राशिः_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		भङ्ग;

	    // store @r
	    पक्ष ISN_STOREREG:
		{
		    पूर्णाङ्क	reg = iptr->isn_arg.सङ्ख्या;

		    --ectx->ec_राशिः.ga_len;
		    tv = राशिः_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		भङ्ग;

	    // store v: variable
	    पक्ष ISN_STOREV:
		--ectx->ec_राशिः.ga_len;
		सचे (set_vim_var_tv(iptr->isn_arg.सङ्ख्या, राशिः_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    गच्छ on_error;
		भङ्ग;

	    // store g:/b:/w:/t: variable
	    पक्ष ISN_STOREG:
	    पक्ष ISN_STOREB:
	    पक्ष ISN_STOREW:
	    पक्ष ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    विकल्प (iptr->isn_type)
		    {
			पक्ष ISN_STOREG:
			    ht = get_globvar_ht();
			    भङ्ग;
			पक्ष ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    भङ्ग;
			पक्ष ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    भङ्ग;
			पक्ष ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    भङ्ग;
			स्वभाव:  // Cannot reach here
			    गच्छ theend;
		    }

		    --ectx->ec_राशिः.ga_len;
		    di = find_var_in_ht(ht, 0, name, TRUE);
		    सचे (di == रिक्त)
			store_var(iptr->isn_arg.string, राशिः_TV_BOT(0));
		    अञ्ञथा
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			सचे (var_check_permission(di, name) == FAIL)
			    गच्छ on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *राशिः_TV_BOT(0);
		    }
		}
		भङ्ग;

	    // store an autoload variable
	    पक्ष ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		set_var(iptr->isn_arg.string, राशिः_TV_BOT(-1), TRUE);
		clear_tv(राशिः_TV_BOT(-1));
		--ectx->ec_राशिः.ga_len;
		भङ्ग;

	    // store number in local variable
	    पक्ष ISN_STORENR:
		tv = राशिः_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_सङ्ख्या;
		tv->vval.v_सङ्ख्या = iptr->isn_arg.storenr.stnr_val;
		भङ्ग;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    पक्ष ISN_STOREINDEX:
		{
		    पूर्णाङ्क res = execute_storeindex(iptr, ectx);

		    सचे (res == FAIL)
			गच्छ on_error;
		    सचे (res == NOTDONE)
			गच्छ theend;
		}
		भङ्ग;

	    // store value in list or blob range
	    पक्ष ISN_STORERANGE:
		सचे (execute_storerange(iptr, ectx) == FAIL)
		    गच्छ on_error;
		भङ्ग;

	    पक्ष ISN_LOAD_CLASSMEMBER:
		{
		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      राशिः_TV_BOT(0));
		    ++ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    पक्ष ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *राशिः_TV_BOT(-1);
		    --ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    // Load or store variable or argument from outer scope.
	    पक्ष ISN_LOADOUTER:
	    पक्ष ISN_STOREOUTER:
		{
		    पूर्णाङ्क		depth = iptr->isn_arg.outer.outer_depth;
		    outer_T	*outer = ectx->ec_outer_ref == रिक्त ? रिक्त
						: ectx->ec_outer_ref->or_outer;

		    याव (depth > 1 && outer != रिक्त)
		    {
			outer = outer->out_up;
			--depth;
		    }
		    सचे (outer == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			सचे (ectx->ec_frame_idx == ectx->ec_initial_frame_idx
						 || ectx->ec_outer_ref == रिक्त)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_closure_called_from_invalid_context));
			अञ्ञथा
			    iemsg("LOADOUTER depth more than scope levels");
			गच्छ theend;
		    }
		    सचे (depth < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->out_loop[-depth - 1]
							       .राशिः->ga_data)
					  + outer->out_loop[-depth - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    अञ्ञथा
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->out_राशिः->ga_data)
				      + outer->out_frame_idx + राशिः_FRAME_SIZE
				      + iptr->isn_arg.outer.outer_idx;
		    सचे (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			    गच्छ theend;
			// careful: ga_grow_inner may re-alloc the stack
			सचे (depth < 0)
			    copy = ((typval_T *)outer->out_loop[-depth - 1]
								   .राशिः->ga_data)
					      + outer->out_loop[-depth - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			अञ्ञथा
			    copy = ((typval_T *)outer->out_राशिः->ga_data)
					  + outer->out_frame_idx + राशिः_FRAME_SIZE
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			सचे (copy != tv)
			    tv = copy;
			copy_tv(tv, राशिः_TV_BOT(0));
			++ectx->ec_राशिः.ga_len;
		    }
		    अञ्ञथा
		    {
			--ectx->ec_राशिः.ga_len;
			clear_tv(tv);
			*tv = *राशिः_TV_BOT(0);
		    }
		}
		भङ्ग;

	    // unlet item in list or dict variable
	    पक्ष ISN_UNLETINDEX:
		सचे (execute_unletindex(iptr, ectx) == FAIL)
		    गच्छ on_error;
		भङ्ग;

	    // unlet range of items in list variable
	    पक्ष ISN_UNLETRANGE:
		सचे (execute_unletrange(iptr, ectx) == FAIL)
		    गच्छ on_error;
		भङ्ग;

	    // push constant
	    पक्ष ISN_PUSHNR:
	    पक्ष ISN_PUSHBOOL:
	    पक्ष ISN_PUSHSPEC:
	    पक्ष ISN_PUSHF:
	    पक्ष ISN_PUSHS:
	    पक्ष ISN_PUSHBLOB:
	    पक्ष ISN_PUSHFUNC:
	    पक्ष ISN_PUSHCHANNEL:
	    पक्ष ISN_PUSHJOB:
	    पक्ष ISN_PUSHOBJ:
	    पक्ष ISN_PUSHCLASS:
		सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    गच्छ theend;
		tv = राशिः_TV_BOT(0);
		tv->v_lock = 0;
		++ectx->ec_राशिः.ga_len;
		विकल्प (iptr->isn_type)
		{
		    पक्ष ISN_PUSHNR:
			tv->v_type = VAR_सङ्ख्या;
			tv->vval.v_सङ्ख्या = iptr->isn_arg.सङ्ख्या;
			भङ्ग;
		    पक्ष ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_सङ्ख्या = iptr->isn_arg.सङ्ख्या;
			भङ्ग;
		    पक्ष ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_सङ्ख्या = iptr->isn_arg.सङ्ख्या;
			भङ्ग;
		    पक्ष ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			भङ्ग;
		    पक्ष ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			भङ्ग;
		    पक्ष ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			सचे (iptr->isn_arg.string == रिक्त)
			    tv->vval.v_string = रिक्त;
			अञ्ञथा
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			भङ्ग;
		    पक्ष ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = रिक्त;
#endif
			भङ्ग;
		    पक्ष ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_JOB;
			tv->vval.v_job = रिक्त;
#endif
			भङ्ग;
		    पक्ष ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = रिक्त;
			भङ्ग;
		    पक्ष ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			भङ्ग;
		    स्वभाव:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == रिक्त
				    ? रिक्त : vim_strsave(iptr->isn_arg.string);
		}
		भङ्ग;

	    पक्ष ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (शून्य)script_autoload(name, FALSE);
		    सचे (find_func(name, TRUE))
		    {
			सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			    गच्छ theend;
			tv = राशिः_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_राशिः.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    अञ्ञथा
		    {
			पूर्णाङ्क res = load_namespace_var(ectx, ISN_LOADG, iptr);

			सचे (res == NOTDONE)
			    गच्छ theend;
			सचे (res == FAIL)
			    गच्छ on_error;
		    }
		}
		भङ्ग;

	    पक्ष ISN_UNLET:
		सचे (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    गच्छ on_error;
		भङ्ग;
	    पक्ष ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		भङ्ग;

	    पक्ष ISN_LOCKUNLOCK:
		{
#ifdef LOG_LOCKVAR
		    ch_log(रिक्त, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#endif
		    lval_मूलम्_T	*lval_मूलम्_save = lval_मूलम्;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_राशिः.ga_len;
		    lval_मूलम्_T मूलम्;
		    मूलम्.lr_tv      = राशिः_TV_BOT(0);
		    मूलम्.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    मूलम्.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_मूलम् = &मूलम्;
		    पूर्णाङ्क res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(मूलम्.lr_tv);
		    lval_मूलम् = lval_मूलम्_save;
		    सचे (res == FAIL)
			गच्छ on_error;
		}
		भङ्ग;

	    पक्ष ISN_LOCKCONST:
		item_lock(राशिः_TV_BOT(-1), 100, TRUE, TRUE);
		भङ्ग;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    पक्ष ISN_NEWLIST:
		सचे (exe_newlist(iptr->isn_arg.सङ्ख्या, ectx) == FAIL)
		    गच्छ theend;
		भङ्ग;

	    // create a tuple from items on the stack
	    पक्ष ISN_NEWTUPLE:
		सचे (exe_newtuple(iptr->isn_arg.सङ्ख्या, ectx) == FAIL)
		    गच्छ theend;
		भङ्ग;

	    // create a dict from items on the stack
	    पक्ष ISN_NEWDICT:
		{
		    पूर्णाङ्क res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.सङ्ख्या, ectx);
		    सचे (res == FAIL)
			गच्छ theend;
		    सचे (res == MAYBE)
			गच्छ on_error;
		}
		भङ्ग;

	    पक्ष ISN_CONCAT:
		सचे (exe_concat(iptr->isn_arg.सङ्ख्या, ectx) == FAIL)
		    गच्छ theend;
		भङ्ग;

	    // create a partial with NULL value
	    पक्ष ISN_NEWPARTIAL:
		सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    गच्छ theend;
		++ectx->ec_राशिः.ga_len;
		tv = राशिः_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_lock = 0;
		tv->vval.v_partial = रिक्त;
		भङ्ग;

	    // call a :def function
	    पक्ष ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		सचे (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				रिक्त,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    गच्छ on_error;
		भङ्ग;

	    // call a method on an interface
	    पक्ष ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = राशिः_TV_BOT(-1 - mfunc->cmf_argcount);
		    सचे (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			गच्छ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    सचे (obj == रिक्त)
		    {
			emsg(_(e_using_null_object));
			गच्छ on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    सचे (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    अञ्ञथा
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			पूर्णाङ्क idx = object_index_from_itf_index(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    सचे (mfunc->cmf_ufunc != रिक्त && IS_GENERIC_FUNC(ufunc))
		    {
			सचे (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			अञ्ञथा
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    सचे (ufunc == रिक्त)
				गच्छ on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    सचे (call_ufunc(ufunc, रिक्त, mfunc->cmf_argcount, ectx,
							   रिक्त, रिक्त) == FAIL)
			गच्छ on_error;
		}
		भङ्ग;

	    // call a builtin function
	    पक्ष ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		सचे (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    गच्छ on_error;
		भङ्ग;

	    // call a funcref or partial
	    पक्ष ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    पूर्णाङ्क		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (pfunc->cpf_शिखरम्)
		    {
			// funcref is above the arguments
			tv = राशिः_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    अञ्ञथा
		    {
			// Get the funcref from the stack.
			--ectx->ec_राशिः.ga_len;
			partial_tv = *राशिः_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    सचे (tv == &partial_tv)
			clear_tv(&partial_tv);
		    सचे (r == FAIL)
			गच्छ on_error;
		}
		भङ्ग;

	    पक्ष ISN_PCALL_END:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_राशिः.ga_len;
		clear_tv(राशिः_TV_BOT(-1));
		*राशिः_TV_BOT(-1) = *राशिः_TV_BOT(0);
		भङ्ग;

	    // call a user defined function or funcref/partial
	    पक्ष ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			गच्छ on_error;
		}
		भङ्ग;

	    // :defer func(arg)
	    पक्ष ISN_DEFER:
		सचे (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    गच्छ on_error;
		भङ्ग;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    पक्ष ISN_RETURN_VOID:
	    पक्ष ISN_RETURN_OBJECT:
		सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    गच्छ theend;
		tv = राशिः_TV_BOT(0);
		++ectx->ec_राशिः.ga_len;
		सचे (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_सङ्ख्या = 0;
		    tv->v_lock = 0;
		}
		अञ्ञथा
		{
		    *tv = *राशिः_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_lock_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    पक्ष ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = रिक्त;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    सचे (check_typval_is_मानम्(राशिः_TV_BOT(-1)) == FAIL)
			गच्छ theend;

		    सचे (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    सचे (trycmd != रिक्त
				 && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    {
			// jump to ":finally" or ":endtry"
			सचे (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			अञ्ञथा
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    अञ्ञथा
			गच्छ func_return;
		}
		भङ्ग;

	    // push a partial, a reference to a compiled function
	    पक्ष ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    सचे (pt == रिक्त)
			गच्छ theend;
		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    {
			vim_free(pt);
			गच्छ theend;
		    }
		    सचे (extra != रिक्त && extra->fre_class != रिक्त)
		    {
			class_T	*cl;
			सचे (extra->fre_object_method)
			{
			    tv = राशिः_TV_BOT(-1);
			    सचे (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				गच्छ on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    सचे (obj == रिक्त)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				गच्छ on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_राशिः.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			अञ्ञथा
			    cl = extra->fre_class;

			सचे (extra->fre_object_method)
			{
			    // object method
			    // convert the interface index to the object index
			    पूर्णाङ्क idx =
				object_index_from_itf_index(extra->fre_class,
					TRUE, extra->fre_method_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			अञ्ञथा
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_method_idx];
			}
		    }
		    अञ्ञथा सचे (extra == रिक्त || extra->fre_func_name == रिक्त)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    अञ्ञथा
		    {
			ufunc = find_func(extra->fre_func_name, FALSE);
		    }
		    सचे (ufunc == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			गच्छ theend;
		    }
		    सचे (extra != रिक्त && extra->fre_outer_depth > 0
					   && (ufunc->uf_flags & FC_CLOSURE))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == रिक्त
				    ? रिक्त : ectx->ec_outer_ref->or_partial;

			अत्थाय (पूर्णाङ्क depth = 1; outer_pt != रिक्त
				    && depth < extra->fre_outer_depth; ++depth)
			    outer_pt = get_pt_outer(outer_pt)->out_up_partial;
			सचे (outer_pt == रिक्त)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    गच्छ theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    अञ्ञथा सचे (fill_partial_and_closure(pt, ufunc,
			       extra == रिक्त ? रिक्त : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			गच्छ theend;
		    }
		    tv = राशिः_TV_BOT(0);
		    ++ectx->ec_राशिः.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_lock = 0;
		}
		भङ्ग;

	    // Create a global function from a lambda.
	    पक्ष ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    सचे (copy_lambda_to_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			गच्छ theend;
		}
		भङ्ग;

	    // List functions
	    पक्ष ISN_DEF:
		सचे (iptr->isn_arg.string == रिक्त)
		    list_functions(रिक्त);
		अञ्ञथा
		{
		    exarg_T	ea;
		    garray_T	पङ्क्तयः_to_free;

		    CLEAR_FIELD(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&पङ्क्तयः_to_free, परिमाण(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, रिक्त, &पङ्क्तयः_to_free, 0, रिक्त, 0, रिक्त);
		    ga_clear_strings(&पङ्क्तयः_to_free);
		}
		भङ्ग;

	    // jump if a condition is met
	    पक्ष ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    पूर्णाङ्क		error = FALSE;
		    पूर्णाङ्क		jump = TRUE;

		    सचे (when != JUMP_ALWAYS)
		    {
			tv = राशिः_TV_BOT(-1);
			सचे (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    सचे (error)
				गच्छ on_error;
			}
			अञ्ञथा
			    jump = tv2bool(tv);
			सचे (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			सचे (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_राशिः.ga_len;
			}
		    }
		    सचे (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		भङ्ग;

	    // "while": jump to end if a condition is false
	    पक्ष ISN_WHILE:
		{
		    पूर्णाङ्क		error = FALSE;
		    पूर्णाङ्क		jump = TRUE;

		    tv = राशिः_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    सचे (error)
			गच्छ on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_राशिः.ga_len;
		    सचे (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_end;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = राशिः_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_सङ्ख्या = ectx->ec_funcrefs.ga_len;
		}
		भङ्ग;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    पक्ष ISN_JUMP_IF_ARG_SET:
	    पक्ष ISN_JUMP_IF_ARG_NOT_SET:
		tv = राशिः_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		पूर्णाङ्क arg_set = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_सङ्ख्या == VVAL_NONE);

		सचे (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_SET && !arg_set)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_data)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_args.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ राशिः_FRAME_SIZE;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_POINTER(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    सचे (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		सचे (iptr->isn_type == ISN_JUMP_IF_ARG_SET ? arg_set : !arg_set)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		भङ्ग;

	    // top of a for loop
	    पक्ष ISN_FOR:
		सचे (execute_for(iptr, ectx) == FAIL)
		    गच्छ theend;
		भङ्ग;

	    // end of a for or while loop
	    पक्ष ISN_ENDLOOP:
		सचे (execute_endloop(iptr, ectx) == FAIL)
		    गच्छ theend;
		भङ्ग;

	    // start of ":try" block
	    पक्ष ISN_TRY:
		{
		    trycmd_T    *trycmd = रिक्त;

		    सचे (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			गच्छ theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_data)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_POINTER(trycmd);
		    trycmd->tcd_frame_idx = ectx->ec_frame_idx;
		    trycmd->tcd_राशिः_len = ectx->ec_राशिः.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		भङ्ग;

	    पक्ष ISN_PUSHEXC:
		सचे (current_exception == रिक्त)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    गच्छ theend;
		}
		सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
		    गच्छ theend;
		tv = राशिः_TV_BOT(0);
		++ectx->ec_राशिः.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->मानम्);
		भङ्ग;

	    पक्ष ISN_CATCH:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    may_restore_cmdmod(&ectx->ec_funclocal);
		    trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    trycmd->tcd_caught = TRUE;
		    trycmd->tcd_did_throw = FALSE;

		    did_emsg = got_int = did_throw = FALSE;
		    force_abort = need_rethrow = FALSE;
		    catch_exception(current_exception);
		}
		भङ्ग;

	    पक्ष ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    पूर्णाङ्क		i;
		    trycmd_T    *trycmd;
		    पूर्णाङ्क		iidx = trycont->tct_where;

		    सचे (trystack->ga_len < trycont->tct_levels)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_levels, trystack->ga_len);
			गच्छ theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    अत्थाय (i = trycont->tct_levels; i > 0; --i)
		    {
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - i;
			// Add one to tcd_cont to be able to jump to
			// instruction with index zero.
			trycmd->tcd_cont = iidx + 1;
			iidx = trycmd->tcd_finally_idx == 0
			    ? trycmd->tcd_endtry_idx : trycmd->tcd_finally_idx;
		    }
		    // jump to :finally or :endtry of current try statement
		    ectx->ec_iidx = iidx;
		}
		भङ्ग;

	    पक्ष ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    सचे (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_राशिः);
			trycmd->tcd_caught = FALSE;
		    }
		    भङ्ग;
		}

	    // end of ":try" block
	    पक्ष ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_data) + trystack->ga_len;
		    सचे (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    सचे (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_राशिः);
		    }

		    सचे (trycmd->tcd_return)
			गच्छ func_return;

		    याव (ectx->ec_राशिः.ga_len > trycmd->tcd_राशिः_len)
		    {
			--ectx->ec_राशिः.ga_len;
			clear_tv(राशिः_TV_BOT(0));
		    }
		    सचे (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		भङ्ग;

	    पक्ष ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    सचे (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = राशिः_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_सङ्ख्या;
			tv->vval.v_सङ्ख्या = 0;
			गच्छ done;
		    }
		    --ectx->ec_राशिः.ga_len;
		    tv = राशिः_TV_BOT(0);
		    सचे (tv->vval.v_string == रिक्त
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_empty_string));
			गच्छ theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    सचे (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
			सचे (trycmd->tcd_caught)
			{
			    // discard the exception
			    finish_exception(caught_राशिः);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (throw_exception(tv->vval.v_string, ET_USER, रिक्त)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			गच्छ theend;
		    }
		    did_throw = TRUE;
		}
		भङ्ग;

	    // compare with special values
	    पक्ष ISN_COMPAREBOOL:
	    पक्ष ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = राशिः_TV_BOT(-2);
		    typval_T	*tv2 = राशिः_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_सङ्ख्या;
		    varnumber_T arg2 = tv2->vval.v_सङ्ख्या;
		    पूर्णाङ्क		res;

		    सचे (iptr->isn_arg.op.op_type == व्यञ्जकम्_EQUAL)
			res = arg1 == arg2;
		    अञ्ञथा
			res = arg1 != arg2;

		    --ectx->ec_राशिः.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_सङ्ख्या = res ? VVAL_TRUE : VVAL_FALSE;
		}
		भङ्ग;

	    पक्ष ISN_COMPARENULL:
		{
		    typval_T	*tv1 = राशिः_TV_BOT(-2);
		    typval_T	*tv2 = राशिः_TV_BOT(-1);
		    पूर्णाङ्क		res;

		    res = typval_compare_null(tv1, tv2);
		    सचे (res == MAYBE)
			गच्छ on_error;
		    सचे (iptr->isn_arg.op.op_type == व्यञ्जकम्_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_राशिः.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_सङ्ख्या = res ? VVAL_TRUE : VVAL_FALSE;
		}
		भङ्ग;

	    // Operation with two number arguments
	    पक्ष ISN_OPNR:
	    पक्ष ISN_COMPARENR:
		{
		    typval_T	*tv1 = राशिः_TV_BOT(-2);
		    typval_T	*tv2 = राशिः_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_सङ्ख्या;
		    varnumber_T arg2 = tv2->vval.v_सङ्ख्या;
		    varnumber_T res = 0;
		    पूर्णाङ्क		div_zero = FALSE;

		    सचे (iptr->isn_arg.op.op_type == व्यञ्जकम्_LSHIFT
			    || iptr->isn_arg.op.op_type == व्यञ्जकम्_RSHIFT)
		    {
			सचे (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_positive));
			    गच्छ on_error;
			}
		    }

		    विकल्प (iptr->isn_arg.op.op_type)
		    {
			पक्ष व्यञ्जकम्_MULT: res = arg1 * arg2; भङ्ग;
			पक्ष व्यञ्जकम्_DIV:  सचे (arg2 == 0)
					    div_zero = TRUE;
					अञ्ञथा
					    res = arg1 / arg2;
					भङ्ग;
			पक्ष व्यञ्जकम्_REM:  सचे (arg2 == 0)
					    div_zero = TRUE;
					अञ्ञथा
					    res = arg1 % arg2;
					भङ्ग;
			पक्ष व्यञ्जकम्_SUB: res = arg1 - arg2; भङ्ग;
			पक्ष व्यञ्जकम्_ADD: res = arg1 + arg2; भङ्ग;

			पक्ष व्यञ्जकम्_EQUAL: res = arg1 == arg2; भङ्ग;
			पक्ष व्यञ्जकम्_NEQUAL: res = arg1 != arg2; भङ्ग;
			पक्ष व्यञ्जकम्_GREATER: res = arg1 > arg2; भङ्ग;
			पक्ष व्यञ्जकम्_GEQUAL: res = arg1 >= arg2; भङ्ग;
			पक्ष व्यञ्जकम्_SMALLER: res = arg1 < arg2; भङ्ग;
			पक्ष व्यञ्जकम्_SEQUAL: res = arg1 <= arg2; भङ्ग;
			पक्ष व्यञ्जकम्_LSHIFT: सचे (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  अञ्ञथा
					      res = (uvarnumber_T)arg1 << arg2;
					  भङ्ग;
			पक्ष व्यञ्जकम्_RSHIFT: सचे (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  अञ्ञथा
					      res = (uvarnumber_T)arg1 >> arg2;
					  भङ्ग;
			स्वभाव: भङ्ग;
		    }

		    --ectx->ec_राशिः.ga_len;
		    सचे (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_सङ्ख्या = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    अञ्ञथा
			tv1->vval.v_सङ्ख्या = res;
		    सचे (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			गच्छ on_error;
		    }
		}
		भङ्ग;

	    // Computation with two float arguments
	    पक्ष ISN_OPFLOAT:
	    पक्ष ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = राशिः_TV_BOT(-2);
		    typval_T	*tv2 = राशिः_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    पूर्णाङ्क		cmp = FALSE;

		    विकल्प (iptr->isn_arg.op.op_type)
		    {
			पक्ष व्यञ्जकम्_MULT: res = arg1 * arg2; भङ्ग;
			पक्ष व्यञ्जकम्_DIV: res = arg1 / arg2; भङ्ग;
			पक्ष व्यञ्जकम्_SUB: res = arg1 - arg2; भङ्ग;
			पक्ष व्यञ्जकम्_ADD: res = arg1 + arg2; भङ्ग;

			पक्ष व्यञ्जकम्_EQUAL: cmp = arg1 == arg2; भङ्ग;
			पक्ष व्यञ्जकम्_NEQUAL: cmp = arg1 != arg2; भङ्ग;
			पक्ष व्यञ्जकम्_GREATER: cmp = arg1 > arg2; भङ्ग;
			पक्ष व्यञ्जकम्_GEQUAL: cmp = arg1 >= arg2; भङ्ग;
			पक्ष व्यञ्जकम्_SMALLER: cmp = arg1 < arg2; भङ्ग;
			पक्ष व्यञ्जकम्_SEQUAL: cmp = arg1 <= arg2; भङ्ग;
			स्वभाव: cmp = 0; भङ्ग;
		    }
		    --ectx->ec_राशिः.ga_len;
		    सचे (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_सङ्ख्या = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    अञ्ञथा
			tv1->vval.v_float = res;
		}
		भङ्ग;

	    पक्ष ISN_COMPARELIST:
	    पक्ष ISN_COMPARETUPLE:
	    पक्ष ISN_COMPAREDICT:
	    पक्ष ISN_COMPAREFUNC:
	    पक्ष ISN_COMPARESTRING:
	    पक्ष ISN_COMPAREBLOB:
	    पक्ष ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = राशिः_TV_BOT(-2);
		    typval_T	*tv2 = राशिः_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    पूर्णाङ्क		ic = iptr->isn_arg.op.op_ic;
		    पूर्णाङ्क		res = FALSE;
		    पूर्णाङ्क		status = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (iptr->isn_type == ISN_COMPARELIST)
		    {
			status = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अञ्ञथा सचे (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			status = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अञ्ञथा सचे (iptr->isn_type == ISN_COMPAREDICT)
		    {
			status = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अञ्ञथा सचे (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			status = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अञ्ञथा सचे (iptr->isn_type == ISN_COMPARESTRING)
		    {
			status = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अञ्ञथा सचे (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			status = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    अञ्ञथा // ISN_COMPAREOBJECT
		    {
			status = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_राशिः.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_सङ्ख्या = res ? VVAL_TRUE : VVAL_FALSE;
		    सचे (status == FAIL)
			गच्छ theend;
		}
		भङ्ग;

	    पक्ष ISN_COMPAREANY:
		{
		    typval_T	*tv1 = राशिः_TV_BOT(-2);
		    typval_T	*tv2 = राशिः_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    पूर्णाङ्क		ic = iptr->isn_arg.op.op_ic;
		    पूर्णाङ्क		status;

		    SOURCING_LNUM = iptr->isn_lnum;
		    status = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_राशिः.ga_len;
		    सचे (status == FAIL)
			गच्छ theend;
		}
		भङ्ग;

	    पक्ष ISN_ADDLIST:
	    पक्ष ISN_ADDTUPLE:
	    पक्ष ISN_ADDBLOB:
		{
		    typval_T *tv1 = राशिः_TV_BOT(-2);
		    typval_T *tv2 = राशिः_TV_BOT(-1);

		    // add two lists or blobs
		    सचे (iptr->isn_type == ISN_ADDLIST)
		    {
			सचे (iptr->isn_arg.op.op_type == व्यञ्जकम्_APPEND
						   && tv1->vval.v_list != रिक्त)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 रिक्त);
			अञ्ञथा
			    eval_addlist(tv1, tv2);
		    }
		    अञ्ञथा सचे (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    अञ्ञथा
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    पक्ष ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = राशिः_TV_BOT(-2);
		    typval_T	*tv2 = राशिः_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (l == रिक्त)
		    {
			emsg(_(e_cannot_add_to_null_list));
			गच्छ on_error;
		    }
		    सचे (मानम्_check_lock(l->lv_lock, रिक्त, FALSE))
			गच्छ on_error;
		    सचे (list_append_tv(l, tv2) == FAIL)
			गच्छ theend;
		    clear_tv(tv2);
		    --ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    पक्ष ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = राशिः_TV_BOT(-2);
		    typval_T	*tv2 = राशिः_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    पूर्णाङ्क		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    सचे (b == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_add_to_null_blob));
			गच्छ on_error;
		    }
		    n = tv_get_सङ्ख्या_chk(tv2, &error);
		    सचे (error)
			गच्छ on_error;
		    ga_append(&b->bv_ga, (पूर्णाङ्क)n);
		    --ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    // Computation with two arguments of unknown type
	    पक्ष ISN_OPANY:
		{
		    typval_T	*tv1 = राशिः_TV_BOT(-2);
		    typval_T	*tv2 = राशिः_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    पूर्णाङ्क		error = FALSE;

		    सचे (iptr->isn_arg.op.op_type == व्यञ्जकम्_ADD)
		    {
			सचे (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_राशिः.ga_len;
			    भङ्ग;
			}
			अञ्ञथा सचे (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_राशिः.ga_len;
			    भङ्ग;
			}
			अञ्ञथा सचे (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_राशिः.ga_len;
			    भङ्ग;
			}
		    }
		    सचे (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    अञ्ञथा
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_सङ्ख्या_chk(tv1, &error);
			सचे (error)
			    गच्छ on_error;
			सचे (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    सचे (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    अञ्ञथा
		    {
			n2 = tv_get_सङ्ख्या_chk(tv2, &error);
			सचे (error)
			    गच्छ on_error;
			सचे (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    सचे (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			विकल्प (iptr->isn_arg.op.op_type)
			{
			    पक्ष व्यञ्जकम्_MULT: f1 = f1 * f2; भङ्ग;
			    पक्ष व्यञ्जकम्_DIV:  f1 = f1 / f2; भङ्ग;
			    पक्ष व्यञ्जकम्_SUB:  f1 = f1 - f2; भङ्ग;
			    पक्ष व्यञ्जकम्_ADD:  f1 = f1 + f2; भङ्ग;
			    स्वभाव: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     गच्छ on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_राशिः.ga_len;
		    }
		    अञ्ञथा
		    {
			पूर्णाङ्क failed = FALSE;

			विकल्प (iptr->isn_arg.op.op_type)
			{
			    पक्ष व्यञ्जकम्_MULT: n1 = n1 * n2; भङ्ग;
			    पक्ष व्यञ्जकम्_DIV:  n1 = अङ्कः_divide(n1, n2, &failed);
					    सचे (failed)
						गच्छ on_error;
					    भङ्ग;
			    पक्ष व्यञ्जकम्_SUB:  n1 = n1 - n2; भङ्ग;
			    पक्ष व्यञ्जकम्_ADD:  n1 = n1 + n2; भङ्ग;
			    स्वभाव:	    n1 = अङ्कः_modulus(n1, n2, &failed);
					    सचे (failed)
						गच्छ on_error;
					    भङ्ग;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_सङ्ख्या;
			tv1->vval.v_सङ्ख्या = n1;
			--ectx->ec_राशिः.ga_len;
		    }
		}
		भङ्ग;

	    पक्ष ISN_STRINDEX:
	    पक्ष ISN_STRSLICE:
		{
		    पूर्णाङ्क		is_slice = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    सचे (is_slice)
		    {
			tv = राशिः_TV_BOT(-2);
			n1 = tv->vval.v_सङ्ख्या;
		    }

		    tv = राशिः_TV_BOT(-1);
		    n2 = tv->vval.v_सङ्ख्या;

		    ectx->ec_राशिः.ga_len -= is_slice ? 2 : 1;
		    tv = राशिः_TV_BOT(-1);
		    सचे (is_slice)
			// Slice: Select the characters from the string
			res = string_slice(tv->vval.v_string, n1, n2, FALSE);
		    अञ्ञथा
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		भङ्ग;

	    पक्ष ISN_LISTINDEX:
	    पक्ष ISN_LISTSLICE:
	    पक्ष ISN_TUPLEINDEX:
	    पक्ष ISN_TUPLESLICE:
	    पक्ष ISN_BLOBINDEX:
	    पक्ष ISN_BLOBSLICE:
		{
		    पूर्णाङ्क		is_slice = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    पूर्णाङ्क		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    पूर्णाङ्क		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_slice ? राशिः_TV_BOT(-3) : राशिः_TV_BOT(-2);

		    tv = राशिः_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_सङ्ख्या;
		    clear_tv(tv);

		    सचे (is_slice)
		    {
			tv = राशिः_TV_BOT(-2);
			n1 = tv->vval.v_सङ्ख्या;
			clear_tv(tv);
		    }

		    ectx->ec_राशिः.ga_len -= is_slice ? 2 : 1;
		    tv = राशिः_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (is_blob)
		    {
			सचे (blob_slice_or_index(val_tv->vval.v_blob, is_slice,
						    n1, n2, FALSE, tv) == FAIL)
			    गच्छ on_error;
		    }
		    अञ्ञथा सचे (is_tuple)
		    {
			सचे (tuple_slice_or_index(val_tv->vval.v_tuple,
				is_slice, n1, n2, FALSE, tv, TRUE) == FAIL)
			    गच्छ on_error;
		    }
		    अञ्ञथा
		    {
			सचे (list_slice_or_index(val_tv->vval.v_list, is_slice,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    गच्छ on_error;
		    }
		}
		भङ्ग;

	    पक्ष ISN_ANYINDEX:
	    पक्ष ISN_ANYSLICE:
		{
		    पूर्णाङ्क		is_slice = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    पूर्णाङ्क		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_slice ? राशिः_TV_BOT(-3) : राशिः_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (check_can_index(tv, TRUE, TRUE) == FAIL)
			गच्छ on_error;
		    var1 = is_slice ? राशिः_TV_BOT(-2) : राशिः_TV_BOT(-1);
		    var2 = is_slice ? राशिः_TV_BOT(-1) : रिक्त;
		    res = eval_index_inner(tv, is_slice, var1, var2,
							FALSE, रिक्त, -1, TRUE);
		    clear_tv(var1);
		    सचे (is_slice)
			clear_tv(var2);
		    ectx->ec_राशिः.ga_len -= is_slice ? 2 : 1;
		    सचे (res == FAIL)
			गच्छ on_error;
		}
		भङ्ग;

	    पक्ष ISN_SLICE:
		{
		    पूर्णाङ्क		गणनम् = iptr->isn_arg.सङ्ख्या;

		    // type will have been checked to be a list
		    tv = राशिः_TV_BOT(-1);
		    सचे (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			सचे (list != रिक्त && list->lv_len >= गणनम्)
			{
			    list_T	*newlist = list_slice(list,
				    गणनम्, list->lv_len - 1);

			    सचे (newlist != रिक्त)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    अञ्ञथा
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			सचे (tuple != रिक्त && TUPLE_LEN(tuple) >= गणनम्)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_slice(tuple, गणनम्,
							TUPLE_LEN(tuple) - 1);
			    सचे (newtuple != रिक्त)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		भङ्ग;

	    पक्ष ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*item_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = राशिः_TV_BOT(-1 - gi->gi_with_op);
		    सचे (tv->v_type == VAR_LIST)
		    {
			li = list_find(tv->vval.v_list, gi->gi_index);
			सचे (li == रिक्त)
			    गच्छ on_error;
			item_tv = &li->li_tv;
		    }
		    अञ्ञथा
			item_tv = TUPLE_ITEM(tv->vval.v_tuple, gi->gi_index);

		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    ++ectx->ec_राशिः.ga_len;
		    copy_tv(item_tv, राशिः_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_index = gi->gi_index + 1;
		    ectx->ec_where.wt_kind = WT_VARIABLE;
		}
		भङ्ग;

	    // dict member with string key (dict['member'])
	    पक्ष ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*कुञ्चिका;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = राशिः_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = राशिः_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    कुञ्चिका = tv->vval.v_string;
		    सचे (कुञ्चिका == रिक्त)
			कुञ्चिका = (char_u *)"";

		    सचे ((di = dict_find(dict, कुञ्चिका, -1)) == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_कुञ्चिका_not_present_in_dictionary_str), कुञ्चिका);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_राशिः.ga_len;
			tv = राशिः_TV_BOT(-1);
			(शून्य) dict_राशिः_save(tv);
			tv->v_type = VAR_सङ्ख्या;
			tv->vval.v_सङ्ख्या = 0;
			गच्छ on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_राशिः.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = राशिः_TV_BOT(-1);
		    सचे (dict_राशिः_save(tv) == FAIL)
			गच्छ on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		भङ्ग;

	    // dict member with string key (dict.member)
	    // or can be an object
	    पक्ष ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = राशिः_TV_BOT(-1);

		    सचे (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			सचे (dict_राशिः_save(tv) == FAIL)
			    गच्छ on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			सचे (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    गच्छ on_error;
		    }
		    अञ्ञथा
		    {
			सचे (tv->v_type != VAR_DICT || tv->vval.v_dict == रिक्त)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    गच्छ on_error;
			}
			dict = tv->vval.v_dict;

			सचे ((di = dict_find(dict, iptr->isn_arg.string, -1))
								   == रिक्त)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_कुञ्चिका_not_present_in_dictionary_str),
						     iptr->isn_arg.string);
			    गच्छ on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			सचे (dict_राशिः_save(tv) == FAIL)
			    गच्छ on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		भङ्ग;

	    पक्ष ISN_GET_OBJ_MEMBER:
	    पक्ष ISN_GET_ITF_MEMBER:
		{
		    tv = राशिः_TV_BOT(-1);
		    सचे (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			गच्छ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    सचे (obj == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			गच्छ on_error;
		    }

		    पूर्णाङ्क idx;
		    सचे (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    अञ्ञथा
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_index_from_itf_index(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    सचे (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			गच्छ on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		भङ्ग;

	    पक्ष ISN_STORE_THIS:
		{
		    पूर्णाङ्क idx = iptr->isn_arg.सङ्ख्या;
		    object_T *obj = राशिः_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *राशिः_TV_BOT(-1);
		    --ectx->ec_राशिः.ga_len;
		}
		भङ्ग;

	    पक्ष ISN_CLEARDICT:
		dict_राशिः_drop();
		भङ्ग;

	    पक्ष ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_राशिः_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = राशिः_TV_BOT(-1);
		    सचे (dict_tv != रिक्त
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != रिक्त
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == रिक्त))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_राशिः_drop();
		}
		भङ्ग;

	    पक्ष ISN_NEGATENR:
		tv = राशिः_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		सचे (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		अञ्ञथा
		    tv->vval.v_सङ्ख्या = -tv->vval.v_सङ्ख्या;
		भङ्ग;

	    पक्ष ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    पूर्णाङ्क		r;
		    where_T	where = WHERE_INIT;

		    tv = राशिः_TV_BOT((पूर्णाङ्क)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    सचे (ct->ct_arg_idx > 0)
		    {
			where.wt_index = ct->ct_arg_idx;
			where.wt_kind = ct->ct_is_var ? WT_VARIABLE : WT_ARGUMENT;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    सचे (r == FAIL)
			गच्छ on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    सचे (tv->v_type == VAR_सङ्ख्या
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_सङ्ख्या == 0
						|| tv->vval.v_सङ्ख्या == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_सङ्ख्या = tv->vval.v_सङ्ख्या
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		भङ्ग;

	    पक्ष ISN_CHECKLEN:
		{
		    पूर्णाङ्क	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = रिक्त;
		    tuple_T *tuple = रिक्त;
		    पूर्णाङ्क	    len = 0;

		    tv = राशिः_TV_BOT(-1);

		    पूर्णाङ्क		len_check_failed = FALSE;
		    सचे (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			सचे (list == रिक्त || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			सचे (list != रिक्त)
			    len = list->lv_len;
		    }
		    अञ्ञथा सचे (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			सचे (tuple == रिक्त || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			सचे (tuple != रिक्त)
			    len = TUPLE_LEN(tuple);
		    }
		    अञ्ञथा
			len_check_failed = TRUE;

		    सचे (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_वस्तूनि_but_got_nr),
				     min_len, len);
			गच्छ on_error;
		    }
		}
		भङ्ग;

	    पक्ष ISN_SETTYPE:
		set_tv_type(राशिः_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		भङ्ग;

	    पक्ष ISN_2BOOL:
	    पक्ष ISN_COND2BOOL:
		{
		    पूर्णाङ्क n;
		    पूर्णाङ्क error = FALSE;

		    सचे (iptr->isn_type == ISN_2BOOL)
		    {
			tv = राशिः_TV_BOT(iptr->isn_arg.tobool.offset);
			n = tv2bool(tv);
			सचे (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    अञ्ञथा
		    {
			tv = राशिः_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			सचे (error)
			    गच्छ on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_सङ्ख्या = n ? VVAL_TRUE : VVAL_FALSE;
		}
		भङ्ग;

	    पक्ष ISN_2STRING:
	    पक्ष ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		सचे (do_2string(राशिः_TV_BOT(iptr->isn_arg.tostring.offset),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    गच्छ on_error;
		भङ्ग;

	    पक्ष ISN_RANGE:
		{
		    exarg_T	ea;
		    वर्ण	*errormsg;

		    ea.line2 = 0;
		    ea.addr_गणनम् = 0;
		    ea.addr_type = ADDR_पङ्क्तयः;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    सचे (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
			गच्छ on_error;

		    सचे (GA_GROW_FAILS(&ectx->ec_राशिः, 1))
			गच्छ theend;
		    ++ectx->ec_राशिः.ga_len;
		    tv = राशिः_TV_BOT(-1);
		    tv->v_type = VAR_सङ्ख्या;
		    tv->v_lock = 0;
		    tv->vval.v_सङ्ख्या = ea.line2;
		}
		भङ्ग;

	    पक्ष ISN_PUT:
		tv = रिक्त; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		भङ्ग;
	    पक्ष ISN_IPUT:
		tv = रिक्त;
		isn_put_do(ectx, iptr, tv, TRUE);
		भङ्ग;

	    पक्ष ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_राशिः.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		भङ्ग;

	    पक्ष ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = रिक्त;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		भङ्ग;

	    पक्ष ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = राशिः_TV_BOT(-1);
		सचे (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_argument_must_be_sequence_of_lists_or_tuples));
		    गच्छ on_error;
		}

		पूर्णाङ्क rc;
		सचे (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		अञ्ञथा
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		सचे (rc != EXEC_OK)
		{
		    सचे (rc == EXEC_FAIL)
			गच्छ on_error;
		    गच्छ theend;
		}
		भङ्ग;

	    पक्ष ISN_PROF_START:
	    पक्ष ISN_PROF_END:
		{
#ifdef FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    सचे (iptr->isn_type == ISN_PROF_START)
		    {
			func_पङ्क्तिः_start(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_पङ्क्तिः_exec(&cookie);
		    }
		    अञ्ञथा
			func_पङ्क्तिः_end(&cookie);
#endif
		}
		भङ्ग;

	    पक्ष ISN_DEBUG:
		handle_debug(iptr, ectx);
		भङ्ग;

	    पक्ष ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    पूर्णाङ्क		item = iptr->isn_arg.shuffle.shfl_item;
		    पूर्णाङ्क		up = iptr->isn_arg.shuffle.shfl_up;

		    tmp_tv = *राशिः_TV_BOT(-item);
		    अत्थाय ( ; up > 0 && item > 1; --up)
		    {
			*राशिः_TV_BOT(-item) = *राशिः_TV_BOT(-item + 1);
			--item;
		    }
		    *राशिः_TV_BOT(-item) = tmp_tv;
		}
		भङ्ग;

	    पक्ष ISN_DROP:
		--ectx->ec_राशिः.ga_len;
		clear_tv(राशिः_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		भङ्ग;

	    पक्ष ISN_SCRIPTCTX_SET:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		भङ्ग;
	}
	अनुवर्त;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	सचे (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
	    गच्छ done;

	सचे (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    गच्छ theend;
	अनुवर्त;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	सचे (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    सचे (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		याव (ectx->ec_राशिः.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_राशिः.ga_len;
		    clear_tv(राशिः_TV_BOT(0));
		}
		याव (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    अनुवर्त;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	सचे (trylevel <= ectx->ec_trylevel_at_start)
	    गच्छ theend;
    }

done:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_राशिः_clear(dict_राशिः_len_at_start);
    ectx->ec_trylevel_at_start = save_trylevel_at_start;
    पटिनिवत्त ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    पूर्णाङ्क
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    पूर्णाङ्क		save_iidx = ectx->ec_iidx;
    पूर्णाङ्क		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    सचे (res == OK)
    {
	*rettv = *राशिः_TV_BOT(-1);
	--ectx->ec_राशिः.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    पटिनिवत्त res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(शून्य)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    पूर्णाङ्क		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    सचे (exec_instructions(ectx) == OK)
    {
	typval_T *tv = राशिः_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_राशिः.ga_len;
	clear_tv(tv);
    }
    अञ्ञथा
    {
	substitute_instr->subs_status = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    पटिनिवत्त res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    पूर्णाङ्क
call_def_function(
    ufunc_T	*ufunc,
    पूर्णाङ्क		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    पूर्णाङ्क		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    पूर्णाङ्क		argc = argc_arg;
    पूर्णाङ्क		partial_argc = partial == रिक्त
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    पूर्णाङ्क		total_argc = argc + partial_argc;
    typval_T	*tv;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		ret = FAIL;
    पूर्णाङ्क		defcount = ufunc->uf_args.ga_len - total_argc;
    sctx_T	save_current_sctx = current_sctx;
    पूर्णाङ्क		did_emsg_before = did_emsg_cumul + did_emsg;
    पूर्णाङ्क		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_सन्देशः_list = रिक्त;
    msglist_T	*private_सन्देशः_list = रिक्त;
    पूर्णाङ्क		save_emsg_silent_def = emsg_silent_def;
    पूर्णाङ्क		save_did_emsg_def = did_emsg_def;
    पूर्णाङ्क		orig_funcdepth;
    पूर्णाङ्क		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#undef राशिः_TV
#define राशिः_TV(idx) (((typval_T *)ectx.ec_राशिः.ga_data) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#undef राशिः_TV_BOT
#define राशिः_TV_BOT(idx) (((typval_T *)ectx.ec_राशिः.ga_data) + ectx.ec_राशिः.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#undef राशिः_TV_VAR
#define राशिः_TV_VAR(idx) (((typval_T *)ectx.ec_राशिः.ga_data) + ectx.ec_frame_idx + राशिः_FRAME_SIZE + idx)

    सचे (ufunc->uf_def_status == UF_NOT_COMPILED
	    || ufunc->uf_def_status == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), रिक्त) == FAIL))
    {
	सचे (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	पटिनिवत्त FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	सचे (dfunc->df_ufunc == रिक्त)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    पटिनिवत्त FAIL;
	}
	सचे (INSTRUCTIONS(dfunc) == रिक्त)
	{
	    iemsg("using call_def_function() on not compiled function");
	    पटिनिवत्त FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    सचे (funcdepth_increment() == FAIL)
	पटिनिवत्त FAIL;

    CLEAR_FIELD(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_राशिः, परिमाण(typval_T), 500);
    सचे (GA_GROW_FAILS(&ectx.ec_राशिः, 20))
    {
	funcdepth_decrement();
	पटिनिवत्त FAIL;
    }
    ga_init2(&ectx.ec_trystack, परिमाण(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, परिमाण(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = total_argc - ufunc->uf_args.ga_len;
    सचे (idx > 0 && ufunc->uf_va_name == रिक्त)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
								    idx), idx);
	गच्छ failed_early;
    }
    idx = total_argc - ufunc->uf_args.ga_len + ufunc->uf_def_args.ga_len;
    सचे (idx < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	गच्छ failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    अत्थाय (idx = 0; idx < total_argc
	    && (ufunc->uf_va_name != रिक्त || idx < ufunc->uf_args.ga_len);
									 ++idx)
    {
	पूर्णाङ्क argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	सचे (idx >= ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_सङ्ख्या == VVAL_NONE)
	{
	    // Use the default value.
	    राशिः_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	अञ्ञथा
	{
	    पूर्णाङ्क done = FALSE;
	    सचे (ufunc->uf_arg_types != रिक्त && idx < ufunc->uf_args.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		सचे (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_सङ्ख्या)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    राशिः_TV_BOT(0)->v_type = VAR_FLOAT;
		    राशिः_TV_BOT(0)->v_lock = 0;
		    राशिः_TV_BOT(0)->vval.v_float = tv->vval.v_सङ्ख्या;
		    done = TRUE;
		}
		अञ्ञथा सचे (check_typval_arg_type(expected, tv,
						   रिक्त, argv_idx + 1) == FAIL)
		    गच्छ failed_early;
	    }
	    सचे (!done)
		copy_tv(tv, राशिः_TV_BOT(0));
	}
	++ectx.ec_राशिः.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    सचे (ufunc->uf_va_name != रिक्त)
    {
	पूर्णाङ्क vararg_गणनम् = argc - ufunc->uf_args.ga_len;

	सचे (vararg_गणनम् < 0)
	    vararg_गणनम् = 0;
	अञ्ञथा
	    argc -= vararg_गणनम्;
	सचे (exe_newlist(vararg_गणनम्, &ectx) == FAIL)
	    गच्छ failed_early;

	// Check the type of the list items.
	tv = राशिः_TV_BOT(-1);
	सचे (ufunc->uf_va_type != रिक्त
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != रिक्त)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_first;

	    अत्थाय (idx = 0; idx < vararg_गणनम्; ++idx)
	    {
		सचे (check_typval_arg_type(expected, &li->li_tv,
						 रिक्त, argc + idx + 1) == FAIL)
		    गच्छ failed_early;
		li = li->li_next;
	    }
	}

	सचे (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *राशिः_TV_BOT(defcount - 1) = *राशिः_TV_BOT(-1);
	--ectx.ec_राशिः.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    सचे (defcount > 0)
	अत्थाय (idx = 0; idx < defcount; ++idx)
	{
	    राशिः_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_राशिः.ga_len;
	}
    सचे (ufunc->uf_va_name != रिक्त)
	++ectx.ec_राशिः.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_frame_idx = ectx.ec_राशिः.ga_len;
    ectx.ec_initial_frame_idx = ectx.ec_frame_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *base_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	सचे (partial != रिक्त || base_ufunc->uf_partial != रिक्त)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    सचे (ectx.ec_outer_ref == रिक्त)
		गच्छ failed_early;
	    सचे (partial != रिक्त)
	    {
		outer_T *outer = get_pt_outer(partial);

		सचे (outer->out_राशिः == रिक्त && outer->out_loop_size == 0)
		{
		    // no stack was set
		    सचे (current_ectx != रिक्त)
		    {
			सचे (current_ectx->ec_outer_ref != रिक्त
			       && current_ectx->ec_outer_ref->or_outer != रिक्त)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		अञ्ञथा
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    अञ्ञथा
	    {
		ectx.ec_outer_ref->or_outer = &base_ufunc->uf_partial->pt_outer;
		++base_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = base_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    अत्थाय (idx = 0; idx < राशिः_FRAME_SIZE; ++idx)
    {
	राशिः_TV(ectx.ec_राशिः.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_राशिः.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	अत्थाय (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    राशिः_TV_VAR(idx)->v_type = VAR_सङ्ख्या;
	    राशिः_TV_VAR(idx)->vval.v_सङ्ख्या = 0;
	}
	ectx.ec_राशिः.ga_len += dfunc->df_varcount;

	सचे (object != रिक्त)
	{
	    // the object is always the variable at index zero
	    tv = राशिः_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	सचे (dfunc->df_has_closure)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    राशिः_TV_VAR(idx)->v_type = VAR_सङ्ख्या;
	    राशिः_TV_VAR(idx)->vval.v_सङ्ख्या = 0;
	    ++ectx.ec_राशिः.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    सचे (funccal != रिक्त)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_प्रक्षिप_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_सन्देशः_list = सन्देशः_list;
    सन्देशः_list = &private_सन्देशः_list;

    // Do turn errors into exceptions.
    suppress_errthrow = FALSE;

    // Do not delete the function while executing it.
    ++ufunc->uf_calls;

    // When ":silent!" was used before calling then we still abort the
    // function.  If ":silent!" is used in the function then we don't.
    emsg_silent_def = emsg_silent;
    did_emsg_def = 0;

    ectx.ec_where = (where_T)WHERE_INIT;

    /*
     * Execute the instructions until done.
     */
    ret = exec_instructions(&ectx);
    सचे (ret == OK)
    {
	// function finished, get result from the stack.
	सचे (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	अञ्ञथा
	{
	    tv = राशिः_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    सचे (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_closure_in_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_निष्कासय();
    current_sctx = save_current_sctx;

    सचे (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    सचे (*सन्देशः_list != रिक्त && saved_सन्देशः_list != रिक्त)
    {
	msglist_T **plist = saved_सन्देशः_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	याव (*plist != रिक्त)
	    plist = &(*plist)->next;

	*plist = *सन्देशः_list;
    }
    सन्देशः_list = saved_सन्देशः_list;

    सचे (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = रिक्त;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    अत्थाय (idx = 0; idx < ectx.ec_राशिः.ga_len; ++idx)
    {
	tv = राशिः_TV(idx);
	सचे (tv->v_type != VAR_सङ्ख्या && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_राशिः.ga_data);
    vim_free(ectx.ec_trystack.ga_data);
    सचे (ectx.ec_outer_ref != रिक्त)
    {
	सचे (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    सचे (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    पटिनिवत्त ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    शून्य
unwind_def_callstack(ectx_T *ectx)
{
    याव (ectx->ec_frame_idx != ectx->ec_initial_frame_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    शून्य
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    सचे (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    स्थिर char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    पूर्णाङ्क		depth;

    ga_init2(&ga, 1, 100);
    अत्थाय (depth = 0; depth < lvi->lvi_depth; ++depth)
    {
	सचे (ga_grow(&ga, 50) == FAIL)
	    भङ्ग;
	सचे (lvi->lvi_loop[depth].var_idx == 0)
	    STRCPY((वर्ण *)ga.ga_data + ga.ga_len, " -");
	अञ्ञथा
	    vim_snprintf((वर्ण *)ga.ga_data + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[depth].var_idx,
			    lvi->lvi_loop[depth].var_idx
					 + lvi->lvi_loop[depth].var_गणनम् - 1);
	ga.ga_len = (पूर्णाङ्क)STRLEN(ga.ga_data);
    }
    पटिनिवत्त ga.ga_data;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    स्थिर शून्य
list_instructions(वर्ण *pfx, isn_T *instr, पूर्णाङ्क instr_गणनम्, ufunc_T *ufunc)
{
    पूर्णाङ्क		पङ्क्तिः_idx = 0;
    पूर्णाङ्क		prev_current = 0;
    पूर्णाङ्क		current;
    पूर्णाङ्क		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    अत्थाय (current = 0; current < instr_गणनम्; ++current)
    {
	isn_T	    *iptr = &instr[current];
	वर्ण	    *पङ्क्तिः;

	सचे (ufunc != रिक्त)
	{
	    याव (पङ्क्तिः_idx < iptr->isn_lnum
					  && पङ्क्तिः_idx < ufunc->uf_पङ्क्तयः.ga_len)
	    {
		सचे (current > prev_current)
		{
		    सन्देशः_puts("\n\n");
		    prev_current = current;
		}
		पङ्क्तिः = ((वर्ण **)ufunc->uf_पङ्क्तयः.ga_data)[पङ्क्तिः_idx++];
		सचे (पङ्क्तिः != रिक्त)
		    सन्देशः(पङ्क्तिः);
	    }
	    सचे (iptr->isn_type == ISN_JUMP_IF_ARG_SET)
	    {
		पूर्णाङ्क	first_def_arg = ufunc->uf_args.ga_len
						   - ufunc->uf_def_args.ga_len;

		सचे (def_arg_idx > 0)
		    सन्देशः_puts("\n\n");
		सन्देशः_start();
		सन्देशः_puts("  ");
		सन्देशः_puts(((वर्ण **)(ufunc->uf_args.ga_data))[
						 first_def_arg + def_arg_idx]);
		सन्देशः_puts(" = ");
		सन्देशः_puts(((वर्ण **)(ufunc->uf_def_args.ga_data))[def_arg_idx++]);
		सन्देशः_clr_eos();
		सन्देशः_end();
	    }
	}

	विकल्प (iptr->isn_type)
	{
	    पक्ष ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (पूर्णाङ्क)iptr->isn_arg.construct.construct_size);
		भङ्ग;
	    पक्ष ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_EXEC_SPLIT:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		भङ्ग;
	    पक्ष ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.सङ्ख्या ? " append" : "");
		भङ्ग;
	    पक्ष ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.सङ्ख्या));
#endif
		भङ्ग;
	    पक्ष ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#endif
		भङ्ग;
	    पक्ष ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, INT_MAX, रिक्त);
		सन्देशः("     -------------");
		भङ्ग;
	    पक्ष ISN_SOURCE:
		{
		    scriptitem_T    *si = SCRIPT_ITEM(iptr->isn_arg.सङ्ख्या);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		भङ्ग;
	    पक्ष ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, INT_MAX, रिक्त);
		    सन्देशः("     -------------");
		}
		भङ्ग;
	    पक्ष ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.सङ्ख्या);
		भङ्ग;
	    पक्ष ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_गणनम्);
		}
		भङ्ग;
	    पक्ष ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_ECHOWINDOW:
		सचे (iptr->isn_arg.echowin.ewin_time > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_गणनम्,
				      iptr->isn_arg.echowin.ewin_time);
		अञ्ञथा
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_गणनम्);
		भङ्ग;
	    पक्ष ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_LOAD:
		{
		    सचे (iptr->isn_arg.सङ्ख्या < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.सङ्ख्या
							  + राशिः_FRAME_SIZE));
		    अञ्ञथा
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		}
		भङ्ग;
	    पक्ष ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    सचे (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_depth,
					outer->outer_idx + राशिः_FRAME_SIZE);
		    अञ्ञथा सचे (outer->outer_depth < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_depth);
		    अञ्ञथा
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_depth,
					      outer->outer_idx);
		}
		भङ्ग;
	    पक्ष ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    सचे (sv == रिक्त)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    अञ्ञथा
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		भङ्ग;
	    पक्ष ISN_LOADS:
	    पक्ष ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		भङ्ग;
	    पक्ष ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		भङ्ग;
	    पक्ष ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		भङ्ग;
	    पक्ष ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		भङ्ग;
	    पक्ष ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		भङ्ग;
	    पक्ष ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (पूर्णाङ्क)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;

	    पक्ष ISN_STORE:
		सचे (iptr->isn_arg.सङ्ख्या < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.सङ्ख्या + राशिः_FRAME_SIZE);
		अञ्ञथा
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.सङ्ख्या);
		भङ्ग;
	    पक्ष ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    सचे (outer->outer_depth < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_depth);
		    अञ्ञथा
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_depth, outer->outer_idx);
		}
		भङ्ग;
	    पक्ष ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_STORES:
	    पक्ष ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		भङ्ग;
	    पक्ष ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    सचे (sv == रिक्त)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    अञ्ञथा
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		भङ्ग;
	    पक्ष ISN_STOREOPT:
	    पक्ष ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		भङ्ग;
	    पक्ष ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (पूर्णाङ्क)iptr->isn_arg.सङ्ख्या);
		भङ्ग;
	    पक्ष ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		भङ्ग;

	    पक्ष ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		भङ्ग;

	    पक्ष ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		भङ्ग;

	    पक्ष ISN_LOAD_CLASSMEMBER:
	    पक्ष ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    पूर्णाङ्क	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_LOAD_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		भङ्ग;

	    // constants
	    पक्ष ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_PUSHBOOL:
	    पक्ष ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		भङ्ग;
	    पक्ष ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		भङ्ग;
	    पक्ष ISN_PUSHFUNC:
		{
		    वर्ण *name = (वर्ण *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == रिक्त ? "[none]" : name);
		}
		भङ्ग;
	    पक्ष ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#endif
		भङ्ग;
	    पक्ष ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#endif
		भङ्ग;
	    पक्ष ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		भङ्ग;
	    पक्ष ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == रिक्त ? "null"
				 : (वर्ण *)iptr->isn_arg.classarg->class_name.string);
		भङ्ग;
	    पक्ष ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		भङ्ग;
	    पक्ष ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		भङ्ग;
	    पक्ष ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		भङ्ग;
	    पक्ष ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		भङ्ग;
	    पक्ष ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		भङ्ग;
	    पक्ष ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		भङ्ग;
	    पक्ष ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		भङ्ग;
	    पक्ष ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		भङ्ग;

	    // function call
	    पक्ष ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    internal_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		भङ्ग;
	    पक्ष ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		भङ्ग;
	    पक्ष ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != रिक्त
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		भङ्ग;
	    पक्ष ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		भङ्ग;
	    पक्ष ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_शिखरम् ? " top" : "", cpfunc->cpf_argcount);
		}
		भङ्ग;
	    पक्ष ISN_PCALL_END:
		smsg("%s%4d PCALL end", pfx, current);
		भङ्ग;
	    पक्ष ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (पूर्णाङ्क)iptr->isn_arg.defer.defer_argcount);
		भङ्ग;
	    पक्ष ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		भङ्ग;
	    पक्ष ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		भङ्ग;
	    पक्ष ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		भङ्ग;
	    पक्ष ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    सचे (extra == रिक्त || extra->fre_func_name == रिक्त)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    अञ्ञथा
			name = extra->fre_func_name;
		    सचे (extra != रिक्त && extra->fre_class != रिक्त)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    अञ्ञथा सचे (extra == रिक्त
				     || extra->fre_loopvar_info.lvi_depth == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    अञ्ञथा
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		भङ्ग;

	    पक्ष ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    सचे (arg->nfa_loopvar_info.lvi_depth == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    अञ्ञथा
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		भङ्ग;

	    पक्ष ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == रिक्त ? (char_u *)"" : name);
		}
		भङ्ग;

	    पक्ष ISN_JUMP:
		{
		    वर्ण *when = "?";

		    विकल्प (iptr->isn_arg.jump.jump_when)
		    {
			पक्ष JUMP_ALWAYS:
			    when = "JUMP";
			    भङ्ग;
			पक्ष JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    भङ्ग;
			पक्ष JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    भङ्ग;
			पक्ष JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    भङ्ग;
			पक्ष JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  // unused
			    भङ्ग;
			पक्ष JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    भङ्ग;
			पक्ष JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    भङ्ग;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		भङ्ग;

	    पक्ष ISN_JUMP_IF_ARG_SET:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + राशिः_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		भङ्ग;

	    पक्ष ISN_JUMP_IF_ARG_NOT_SET:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + राशिः_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		भङ्ग;

	    पक्ष ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_end);
		}
		भङ्ग;

	    पक्ष ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->end_funcref_idx,
			    endloop->end_var_idx,
			    endloop->end_var_idx + endloop->end_var_गणनम् - 1,
			    endloop->end_depth);
		}
		भङ्ग;

	    पक्ष ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_end);
		}
		भङ्ग;

	    पक्ष ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    सचे (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    अञ्ञथा
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		भङ्ग;
	    पक्ष ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		भङ्ग;
	    पक्ष ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_levels,
				      trycont->tct_levels == 1 ? "" : "s",
				      trycont->tct_where);
		}
		भङ्ग;
	    पक्ष ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		भङ्ग;
	    पक्ष ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		भङ्ग;
	    पक्ष ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		भङ्ग;

	    // expression operations on number
	    पक्ष ISN_OPNR:
	    पक्ष ISN_OPFLOAT:
	    पक्ष ISN_OPANY:
		{
		    वर्ण *what;
		    वर्ण *ins;

		    विकल्प (iptr->isn_arg.op.op_type)
		    {
			पक्ष व्यञ्जकम्_MULT: what = "*"; भङ्ग;
			पक्ष व्यञ्जकम्_DIV: what = "/"; भङ्ग;
			पक्ष व्यञ्जकम्_REM: what = "%"; भङ्ग;
			पक्ष व्यञ्जकम्_SUB: what = "-"; भङ्ग;
			पक्ष व्यञ्जकम्_ADD: what = "+"; भङ्ग;
			पक्ष व्यञ्जकम्_LSHIFT: what = "<<"; भङ्ग;
			पक्ष व्यञ्जकम्_RSHIFT: what = ">>"; भङ्ग;
			स्वभाव:       what = "???"; भङ्ग;
		    }
		    विकल्प (iptr->isn_type)
		    {
			पक्ष ISN_OPNR: ins = "OPNR"; भङ्ग;
			पक्ष ISN_OPFLOAT: ins = "OPFLOAT"; भङ्ग;
			पक्ष ISN_OPANY: ins = "OPANY"; भङ्ग;
			स्वभाव: ins = "???"; भङ्ग;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		भङ्ग;

	    पक्ष ISN_COMPAREBOOL:
	    पक्ष ISN_COMPARESPECIAL:
	    पक्ष ISN_COMPARENULL:
	    पक्ष ISN_COMPARENR:
	    पक्ष ISN_COMPAREFLOAT:
	    पक्ष ISN_COMPARESTRING:
	    पक्ष ISN_COMPAREBLOB:
	    पक्ष ISN_COMPARELIST:
	    पक्ष ISN_COMPARETUPLE:
	    पक्ष ISN_COMPAREDICT:
	    पक्ष ISN_COMPAREFUNC:
	    पक्ष ISN_COMPAREOBJECT:
	    पक्ष ISN_COMPAREANY:
		   {
		       वर्ण *p;
		       वर्ण buf[10];
		       वर्ण *type;

		       विकल्प (iptr->isn_arg.op.op_type)
		       {
			   पक्ष व्यञ्जकम्_EQUAL:	 p = "=="; भङ्ग;
			   पक्ष व्यञ्जकम्_NEQUAL:    p = "!="; भङ्ग;
			   पक्ष व्यञ्जकम्_GREATER:   p = ">"; भङ्ग;
			   पक्ष व्यञ्जकम्_GEQUAL:    p = ">="; भङ्ग;
			   पक्ष व्यञ्जकम्_SMALLER:   p = "<"; भङ्ग;
			   पक्ष व्यञ्जकम्_SEQUAL:    p = "<="; भङ्ग;
			   पक्ष व्यञ्जकम्_MATCH:	 p = "=~"; भङ्ग;
			   पक्ष व्यञ्जकम्_IS:	 p = "is"; भङ्ग;
			   पक्ष व्यञ्जकम्_ISNOT:	 p = "isnot"; भङ्ग;
			   पक्ष व्यञ्जकम्_NOMATCH:	 p = "!~"; भङ्ग;
			   स्वभाव:  p = "???"; भङ्ग;
		       }
		       STRCPY(buf, p);
		       सचे (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       विकल्प(iptr->isn_type)
		       {
			   पक्ष ISN_COMPAREBOOL: type = "COMPAREBOOL"; भङ्ग;
			   पक्ष ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; भङ्ग;
			   पक्ष ISN_COMPARENULL: type = "COMPARENULL"; भङ्ग;
			   पक्ष ISN_COMPARENR: type = "COMPARENR"; भङ्ग;
			   पक्ष ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; भङ्ग;
			   पक्ष ISN_COMPARESTRING:
						  type = "COMPARESTRING"; भङ्ग;
			   पक्ष ISN_COMPAREBLOB: type = "COMPAREBLOB"; भङ्ग;
			   पक्ष ISN_COMPARELIST: type = "COMPARELIST"; भङ्ग;
			   पक्ष ISN_COMPARETUPLE: type = "COMPARETUPLE"; भङ्ग;
			   पक्ष ISN_COMPAREDICT: type = "COMPAREDICT"; भङ्ग;
			   पक्ष ISN_COMPAREFUNC: type = "COMPAREFUNC"; भङ्ग;
			   पक्ष ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; भङ्ग;
			   पक्ष ISN_COMPAREANY: type = "COMPAREANY"; भङ्ग;
			   स्वभाव: type = "???"; भङ्ग;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   भङ्ग;

	    पक्ष ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); भङ्ग;
	    पक्ष ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); भङ्ग;
	    पक्ष ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); भङ्ग;

	    // expression operations
	    पक्ष ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		भङ्ग;
	    पक्ष ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); भङ्ग;
	    पक्ष ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); भङ्ग;
	    पक्ष ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); भङ्ग;
	    पक्ष ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); भङ्ग;
	    पक्ष ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); भङ्ग;
	    पक्ष ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); भङ्ग;
	    पक्ष ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); भङ्ग;
	    पक्ष ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); भङ्ग;
	    पक्ष ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); भङ्ग;
	    पक्ष ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); भङ्ग;
	    पक्ष ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); भङ्ग;
	    पक्ष ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); भङ्ग;
	    पक्ष ISN_SLICE: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.सङ्ख्या); भङ्ग;
	    पक्ष ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_index,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); भङ्ग;
	    पक्ष ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); भङ्ग;
	    पक्ष ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); भङ्ग;
	    पक्ष ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (पूर्णाङ्क)iptr->isn_arg.classmember.cm_idx);
				     भङ्ग;
	    पक्ष ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (पूर्णाङ्क)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     भङ्ग;
	    पक्ष ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (पूर्णाङ्क)iptr->isn_arg.सङ्ख्या); भङ्ग;
	    पक्ष ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); भङ्ग;
	    पक्ष ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); भङ्ग;

	    पक्ष ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); भङ्ग;

	    पक्ष ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      वर्ण	    *tofree = रिक्त;
		      वर्ण	    *typename;

		      सचे (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_सङ्ख्या_OK))
			  typename = "float|number";
		      अञ्ञथा सचे (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      अञ्ञथा
			  typename = type_name(ct->ct_type, &tofree);

		      सचे (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (पूर्णाङ्क)ct->ct_off);
		      अञ्ञथा
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (पूर्णाङ्क)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (पूर्णाङ्क)ct->ct_arg_idx);
		      vim_free(tofree);
		      भङ्ग;
		  }
	    पक्ष ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_more_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       भङ्ग;
	    पक्ष ISN_SETTYPE:
		  {
		      वर्ण *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      भङ्ग;
		  }
	    पक्ष ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); भङ्ग;
	    पक्ष ISN_2BOOL: सचे (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    अञ्ञथा
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    भङ्ग;
	    पक्ष ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      भङ्ग;
	    पक्ष ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      भङ्ग;
	    पक्ष ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    भङ्ग;
	    पक्ष ISN_PUT:
		सचे (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		अञ्ञथा सचे (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		अञ्ञथा
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		भङ्ग;
	    पक्ष ISN_IPUT:
		सचे (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		अञ्ञथा सचे (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		अञ्ञथा
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		भङ्ग;
	    पक्ष ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  रिक्त, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    सचे (likely(buf != रिक्त))
		    {
			(शून्य)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    भङ्ग;
		}
	    पक्ष ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); भङ्ग;

	    पक्ष ISN_PROF_START:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 भङ्ग;

	    पक्ष ISN_PROF_END:
		smsg("%s%4d PROFILE END", pfx, current);
		भङ्ग;

	    पक्ष ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_names_len);
		भङ्ग;

	    पक्ष ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_गणनम्,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      भङ्ग;
	    पक्ष ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_item,
					 iptr->isn_arg.shuffle.shfl_up);
			      भङ्ग;
	    पक्ष ISN_DROP: smsg("%s%4d DROP", pfx, current); भङ्ग;

	    पक्ष ISN_SCRIPTCTX_SET:
		{
		    पूर्णाङ्क	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ITEM(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		भङ्ग;

	    पक्ष ISN_FINISH: // End of list of instructions for ISN_SUBSTITUTE.
			   पटिनिवत्त;
	}

	out_flush();	    // output one line at a time
	ui_breakcheck();
	सचे (got_int)
	    भङ्ग;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    शून्य
set_context_in_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_pattern = arg;

    // first argument already typed: only user function names
    सचे (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_pattern = skipwhite(p);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the list of :disassemble
 * arguments.
 */
    char_u *
get_disassemble_argument(expand_T *xp, पूर्णाङ्क idx)
{
    सचे (idx == 0)
	पटिनिवत्त (char_u *)"debug";
    सचे (idx == 1)
	पटिनिवत्त (char_u *)"profile";
    पटिनिवत्त get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    शून्य
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = रिक्त;  // init to shut up gcc warning
    पूर्णाङ्क		instr_गणनम् = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = find_func_by_name(arg, &compile_type);
    सचे (ufunc == रिक्त)
	पटिनिवत्त;
    सचे (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, रिक्त) == FAIL)
	पटिनिवत्त;
    सचे (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	पटिनिवत्त;
    }
    सन्देशः((वर्ण *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    विकल्प (compile_type)
    {
	पक्ष CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_गणनम् = dfunc->df_instr_prof_गणनम्;
	    भङ्ग;
#endif
	    // FALLTHROUGH
	पक्ष CT_NONE:
	    instr = dfunc->df_instr;
	    instr_गणनम् = dfunc->df_instr_गणनम्;
	    भङ्ग;
	पक्ष CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_गणनम् = dfunc->df_instr_debug_गणनम्;
	    भङ्ग;
    }

    list_instructions("", instr, instr_गणनम्, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    पूर्णाङ्क
tv2bool(typval_T *tv)
{
    विकल्प (tv->v_type)
    {
	पक्ष VAR_सङ्ख्या:
	    पटिनिवत्त tv->vval.v_सङ्ख्या != 0;
	पक्ष VAR_FLOAT:
	    पटिनिवत्त tv->vval.v_float != 0.0;
	पक्ष VAR_PARTIAL:
	    पटिनिवत्त tv->vval.v_partial != रिक्त;
	पक्ष VAR_FUNC:
	पक्ष VAR_STRING:
	    पटिनिवत्त tv->vval.v_string != रिक्त && *tv->vval.v_string != NUL;
	पक्ष VAR_LIST:
	    पटिनिवत्त tv->vval.v_list != रिक्त && tv->vval.v_list->lv_len > 0;
	पक्ष VAR_TUPLE:
	    पटिनिवत्त tuple_len(tv->vval.v_tuple) > 0;
	पक्ष VAR_DICT:
	    पटिनिवत्त tv->vval.v_dict != रिक्त
				    && tv->vval.v_dict->dv_hashtab.ht_used > 0;
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    पटिनिवत्त tv->vval.v_सङ्ख्या == VVAL_TRUE ? TRUE : FALSE;
	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    पटिनिवत्त tv->vval.v_job != रिक्त;
#अञ्ञथा
	    भङ्ग;
#endif
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    पटिनिवत्त tv->vval.v_channel != रिक्त;
#अञ्ञथा
	    भङ्ग;
#endif
	पक्ष VAR_BLOB:
	    पटिनिवत्त tv->vval.v_blob != रिक्त && tv->vval.v_blob->bv_ga.ga_len > 0;

	पक्ष VAR_OBJECT:
	    पटिनिवत्त tv->vval.v_object != रिक्त;

	पक्ष VAR_CLASS:
	पक्ष VAR_TYPEALIAS:
	    check_typval_is_मानम्(tv);
	    भङ्ग;

	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_INSTR:
	    भङ्ग;
    }
    पटिनिवत्त FALSE;
}

    शून्य
emsg_using_string_as(typval_T *tv, पूर्णाङ्क as_सङ्ख्या)
{
    semsg(_(as_सङ्ख्या ? e_using_string_as_सङ्ख्या_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == रिक्त
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    पूर्णाङ्क
check_not_string(typval_T *tv)
{
    सचे (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	पटिनिवत्त FAIL;
    }
    पटिनिवत्त OK;
}

#endif // FEAT_EVAL
