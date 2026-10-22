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
#অন্তর্ভাব "vim.h"

#যদি defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক	    tcd_frame_idx;	// ec_frame_idx at ISN_TRY
    পূর্ণাঙ্ক	    tcd_স্তূপ_len;	// size of ectx.ec_stack at ISN_TRY
    পূর্ণাঙ্ক	    tcd_in_catch;	// in catch or finally block
    পূর্ণাঙ্ক	    tcd_did_throw;	// set did_throw in :endtry
    পূর্ণাঙ্ক	    tcd_catch_idx;	// instruction of the first :catch or :finally
    পূর্ণাঙ্ক	    tcd_finally_idx;	// instruction of the :finally block or zero
    পূর্ণাঙ্ক	    tcd_endtry_idx;	// instruction of the :endtry
    পূর্ণাঙ্ক	    tcd_caught;		// catch block entered
    পূর্ণাঙ্ক	    tcd_cont;		// :continue encountered, jump here (minus one)
    পূর্ণাঙ্ক	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
প্রকারনির্দেশ সংরচনা {
    পূর্ণাঙ্ক		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    পূর্ণাঙ্ক		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
প্রকারনির্দেশ সংরচনা {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    পূর্ণাঙ্ক		or_outer_allocated;  // free "or_outer" later
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
সংরচনা ectx_S {
    garray_T	ec_স্তূপ;	// stack of typval_T values
    পূর্ণাঙ্ক		ec_frame_idx;	// index in ec_stack: context of ec_dfunc_idx
    পূর্ণাঙ্ক		ec_initial_frame_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    পূর্ণাঙ্ক		ec_dfunc_idx;	// current function index
    পূর্ণাঙ্ক		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    পূর্ণাঙ্ক		ec_did_emsg_before;
    পূর্ণাঙ্ক		ec_trylevel_at_start;
    where_T	ec_where;
};

#ifdef FEAT_PROFILE
// stack of profinfo_T used when profiling.
স্থির garray_T profile_info_ga = {0, 0, পরিমাণ(profinfo_T), 20, রিক্ত};
#endif

// Get pointer to item in the stack.
#define স্তূপ_TV(idx) (((typval_T *)ectx->ec_স্তূপ.ga_data) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#define স্তূপ_TV_BOT(idx) (((typval_T *)ectx->ec_স্তূপ.ga_data) + ectx->ec_স্তূপ.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#define স্তূপ_TV_VAR(idx) (((typval_T *)ectx->ec_স্তূপ.ga_data) + ectx->ec_frame_idx + স্তূপ_FRAME_SIZE + idx)

// Return value for functions used to execute instructions
#define EXEC_FAIL	0
#define EXEC_OK		1
#define EXEC_DONE	2

    শূন্য
to_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_to_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    স্থির পূর্ণাঙ্ক
ufunc_argcount(ufunc_T *ufunc)
{
    প্রত্যাগম ufunc->uf_args.ga_len + (ufunc->uf_va_name != রিক্ত ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    স্থির পূর্ণাঙ্ক
exe_concat(পূর্ণাঙ্ক গুনতি, ectx_T *ectx)
{
    পূর্ণাঙ্ক		idx;
    size_t	len = 0;
    garray_T	ga;
    প্রকারনির্দেশ সংরচনা
    {
	typval_T    *tv;
	size_t	    length;
    } string_segment_T;
    গণনা
    {
	STRING_SEGMENT_CACHE_SIZE = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_SIZE];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    যদি (গুনতি > (পূর্ণাঙ্ক)ARRAY_LENGTH(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, গুনতি);
	যদি (string_segment_tab == রিক্ত)
	    প্রত্যাগম FAIL;
    }

    ga_init2(&ga, পরিমাণ(বর্ণ), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    প্রতি (idx = 0; idx < গুনতি; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = স্তূপ_TV_BOT(idx - গুনতি);
	যদি (segment->tv->vval.v_string != রিক্ত)
	{
	    segment->length = STRLEN(segment->tv->vval.v_string);
	    len += segment->length;
	}
	অন্যথা
	    segment->length = 0;    // Ensure clean state for the second pass
    }

    যদি (ga_grow(&ga, (পূর্ণাঙ্ক)len + 1) == FAIL)
    {
	যদি (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	প্রত্যাগম FAIL;
    }

    প্রতি (idx = 0; idx < গুনতি; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->length);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_স্তূপ.ga_len -= গুনতি - 1;
    স্তূপ_TV_BOT(-1)->vval.v_string = ga.ga_data;

    যদি (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    প্রত্যাগম OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    স্থির পূর্ণাঙ্ক
exe_newlist(পূর্ণাঙ্ক গুনতি, ectx_T *ectx)
{
    list_T	*list = রিক্ত;
    পূর্ণাঙ্ক		idx;
    typval_T	*tv;

    যদি (গুনতি >= 0)
    {
	list = list_alloc_with_বস্তুসমূহ(গুনতি);
	যদি (list == রিক্ত)
	    প্রত্যাগম FAIL;
	প্রতি (idx = 0; idx < গুনতি; ++idx)
	    list_set_item(list, idx, স্তূপ_TV_BOT(idx - গুনতি));
    }

    যদি (গুনতি > 0)
	ectx->ec_স্তূপ.ga_len -= গুনতি - 1;
    অন্যথা যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
    {
	list_unref(list);
	প্রত্যাগম FAIL;
    }
    অন্যথা
	++ectx->ec_স্তূপ.ga_len;
    tv = স্তূপ_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_lock = 0;
    যদি (list != রিক্ত)
	++list->lv_refcount;
    প্রত্যাগম OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    স্থির পূর্ণাঙ্ক
exe_newtuple(পূর্ণাঙ্ক গুনতি, ectx_T *ectx)
{
    tuple_T	*tuple = রিক্ত;
    পূর্ণাঙ্ক		idx;
    typval_T	*tv;

    যদি (গুনতি >= 0)
    {
	tuple = tuple_alloc_with_বস্তুসমূহ(গুনতি);
	যদি (tuple == রিক্ত)
	    প্রত্যাগম FAIL;
	প্রতি (idx = 0; idx < গুনতি; ++idx)
	    tuple_set_item(tuple, idx, স্তূপ_TV_BOT(idx - গুনতি));
    }

    যদি (গুনতি > 0)
	ectx->ec_স্তূপ.ga_len -= গুনতি - 1;
    অন্যথা যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
    {
	tuple_unref(tuple);
	প্রত্যাগম FAIL;
    }
    অন্যথা
	++ectx->ec_স্তূপ.ga_len;
    tv = স্তূপ_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_lock = 0;
    যদি (tuple != রিক্ত)
	++tuple->tv_refcount;

    প্রত্যাগম OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    স্থির পূর্ণাঙ্ক
exe_newdict(পূর্ণাঙ্ক গুনতি, ectx_T *ectx)
{
    dict_T	*dict = রিক্ত;
    dictitem_T	*item;
    char_u	*চাবি;
    পূর্ণাঙ্ক		idx;
    typval_T	*tv;

    যদি (গুনতি >= 0)
    {
	dict = dict_alloc();
	যদি (unlikely(dict == রিক্ত))
	    প্রত্যাগম FAIL;
	প্রতি (idx = 0; idx < গুনতি; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = স্তূপ_TV_BOT(2 * (idx - গুনতি));
	    // check key is unique
	    চাবি = tv->vval.v_string == রিক্ত ? (char_u *)"" : tv->vval.v_string;
	    item = dict_find(dict, চাবি, -1);
	    যদি (item != রিক্ত)
	    {
		semsg(_(e_duplicate_চাবি_in_dictionary_str), চাবি);
		dict_unref(dict);
		প্রত্যাগম MAYBE;
	    }
	    item = dictitem_alloc(চাবি);
	    clear_tv(tv);
	    যদি (unlikely(item == রিক্ত))
	    {
		dict_unref(dict);
		প্রত্যাগম FAIL;
	    }
	    tv = স্তূপ_TV_BOT(2 * (idx - গুনতি) + 1);
	    item->di_tv = *tv;
	    item->di_tv.v_lock = 0;
	    tv->v_type = VAR_UNKNOWN;
	    যদি (dict_add(dict, item) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(item);
		dict_unref(dict);
		প্রত্যাগম FAIL;
	    }
	}
    }

    যদি (গুনতি > 0)
	ectx->ec_স্তূপ.ga_len -= 2 * গুনতি - 1;
    অন্যথা যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
    {
	dict_unref(dict);
	প্রত্যাগম FAIL;
    }
    অন্যথা
	++ectx->ec_স্তূপ.ga_len;
    tv = স্তূপ_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_lock = 0;
    tv->vval.v_dict = dict;
    যদি (dict != রিক্ত)
	++dict->dv_refcount;
    প্রত্যাগম OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    শূন্য
update_has_breakpoint(ufunc_T *ufunc)
{
    যদি (ufunc->uf_debug_tick == debug_tick)
	প্রত্যাগম;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

স্থির garray_T dict_স্তূপ = GA_EMPTY;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    স্থির পূর্ণাঙ্ক
dict_স্তূপ_save(typval_T *tv)
{
    যদি (dict_স্তূপ.ga_growsize == 0)
	ga_init2(&dict_স্তূপ, পরিমাণ(typval_T), 10);
    যদি (ga_grow(&dict_স্তূপ, 1) == FAIL)
	প্রত্যাগম FAIL;
    ((typval_T *)dict_স্তূপ.ga_data)[dict_স্তূপ.ga_len] = *tv;
    ++dict_স্তূপ.ga_len;
    প্রত্যাগম OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    স্থির typval_T *
dict_স্তূপ_get_tv(শূন্য)
{
    যদি (dict_স্তূপ.ga_len == 0)
	প্রত্যাগম রিক্ত;
    প্রত্যাগম ((typval_T *)dict_স্তূপ.ga_data) + dict_স্তূপ.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    স্থির dict_T *
dict_স্তূপ_get_dict(শূন্য)
{
    typval_T *tv;

    যদি (dict_স্তূপ.ga_len == 0)
	প্রত্যাগম রিক্ত;
    tv = ((typval_T *)dict_স্তূপ.ga_data) + dict_স্তূপ.ga_len - 1;
    যদি (tv->v_type == VAR_DICT)
	প্রত্যাগম tv->vval.v_dict;
    প্রত্যাগম রিক্ত;
}

/*
 * Drop an item from the dict stack.
 */
    স্থির শূন্য
dict_স্তূপ_drop(শূন্য)
{
    যদি (dict_স্তূপ.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	প্রত্যাগম;
    }
    --dict_স্তূপ.ga_len;
    clear_tv(((typval_T *)dict_স্তূপ.ga_data) + dict_স্তূপ.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    স্থির শূন্য
dict_স্তূপ_clear(পূর্ণাঙ্ক len)
{
    যাবত্ (dict_স্তূপ.ga_len > len)
	dict_স্তূপ_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    স্থির outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    যদি (ptref == রিক্ত)
	প্রত্যাগম &pt->pt_outer;

    // partial using partial (recursively)
    যাবত্ (ptref->pt_outer_partial != রিক্ত)
	ptref = ptref->pt_outer_partial;
    প্রত্যাগম &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
check_ufunc_arg_types(ufunc_T *ufunc, পূর্ণাঙ্ক argcount, পূর্ণাঙ্ক off, ectx_T *ectx)
{
    যদি (ufunc->uf_arg_types == রিক্ত && ufunc->uf_va_type == রিক্ত)
	প্রত্যাগম OK;

    typval_T	*argv = স্তূপ_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    প্রতি (পূর্ণাঙ্ক i = 0; i < argcount; ++i)
    {
	type_T *type = রিক্ত;

	// assume a v:none argument, using the default value, is always OK
	যদি (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_সংখ্যা == VVAL_NONE)
	    অনুবর্ত;
	// only pass values to user functions, never types
	যদি (check_typval_is_মান(&argv[i]) == FAIL)
	    প্রত্যাগম FAIL;

	যদি (i < ufunc->uf_args.ga_len && ufunc->uf_arg_types != রিক্ত)
	    type = ufunc->uf_arg_types[i];
	অন্যথা যদি (ufunc->uf_va_type != রিক্ত)
	    type = ufunc->uf_va_type->tt_member;
	যদি (type != রিক্ত && check_typval_arg_type(type,
		    &argv[i], রিক্ত, i + 1) == FAIL)
	    প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
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
    স্থির পূর্ণাঙ্ক
call_dfunc(
	পূর্ণাঙ্ক		cdf_idx,
	partial_T	*pt_arg,
	পূর্ণাঙ্ক		argcount_arg,
	ectx_T		*ectx)
{
    পূর্ণাঙ্ক		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg_cumul + did_emsg;
    পূর্ণাঙ্ক		arg_to_add;
    পূর্ণাঙ্ক		vararg_গুনতি = 0;
    পূর্ণাঙ্ক		varcount;
    পূর্ণাঙ্ক		idx;
    estack_T	*entry;
    funclocal_T	*floc = রিক্ত;
    পূর্ণাঙ্ক		res = OK;
    compiletype_T compile_type;

    যদি (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == রিক্ত ? (char_u *)"unknown" : dfunc->df_name);
	প্রত্যাগম FAIL;
    }

#ifdef FEAT_PROFILE
    যদি (do_profiling == PROF_YES)
    {
	যদি (GA_GROW_OK(&profile_info_ga, 1))
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
    যদি (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, রিক্ত);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    }
    যদি (res == FAIL || INSTRUCTIONS(dfunc) == রিক্ত)
    {
	যদি (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	প্রত্যাগম FAIL;
    }

    যদি (ufunc->uf_va_name != রিক্ত)
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
	vararg_গুনতি = argcount - ufunc->uf_args.ga_len;
	যদি (vararg_গুনতি < 0)
	    vararg_গুনতি = 0;
	অন্যথা
	    argcount -= vararg_গুনতি;
	যদি (exe_newlist(vararg_গুনতি, ectx) == FAIL)
	    প্রত্যাগম FAIL;

	vararg_গুনতি = 1;
    }

    arg_to_add = ufunc->uf_args.ga_len - argcount;
    যদি (arg_to_add < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
			-arg_to_add), -arg_to_add);
	প্রত্যাগম FAIL;
    }
    অন্যথা যদি (arg_to_add > ufunc->uf_def_args.ga_len)
    {
	পূর্ণাঙ্ক missing = arg_to_add - ufunc->uf_def_args.ga_len;

	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
			missing), missing);
	প্রত্যাগম FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = স্তূপ_TV_BOT(0) - argcount - vararg_গুনতি - 1;

    যদি (IS_OBJECT_METHOD(ufunc) && !IS_CONSTRUCTOR_METHOD(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == রিক্ত)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	প্রত্যাগম FAIL;
    }

    // Check the argument types.
    যদি (check_ufunc_arg_types(ufunc, argcount, vararg_গুনতি, ectx) == FAIL)
	প্রত্যাগম FAIL;

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
    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ,
				     arg_to_add + স্তূপ_FRAME_SIZE + varcount))
	প্রত্যাগম FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = স্তূপ_TV_BOT(0) - argcount - vararg_গুনতি - 1;

    // If depth of calling is getting too high, don't execute the function.
    যদি (funcdepth_increment() == FAIL)
	প্রত্যাগম FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    যদি (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	যদি (floc == রিক্ত)
	    প্রত্যাগম FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    যদি (vararg_গুনতি > 0 && arg_to_add > 0)
	*স্তূপ_TV_BOT(arg_to_add - 1) = *স্তূপ_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    প্রতি (idx = 0; idx < arg_to_add; ++idx)
	স্তূপ_TV_BOT(idx - vararg_গুনতি)->v_type = VAR_UNKNOWN;
    ectx->ec_স্তূপ.ga_len += arg_to_add;

    // Store current execution state in stack frame for ISN_RETURN.
    স্তূপ_TV_BOT(স্তূপ_FRAME_FUNC_OFF)->vval.v_সংখ্যা = ectx->ec_dfunc_idx;
    স্তূপ_TV_BOT(স্তূপ_FRAME_IIDX_OFF)->vval.v_সংখ্যা = ectx->ec_iidx;
    স্তূপ_TV_BOT(স্তূপ_FRAME_INSTR_OFF)->vval.v_string = (শূন্য *)ectx->ec_instr;
    স্তূপ_TV_BOT(স্তূপ_FRAME_OUTER_OFF)->vval.v_string =
						    (শূন্য *)ectx->ec_outer_ref;
    স্তূপ_TV_BOT(স্তূপ_FRAME_FUNCLOCAL_OFF)->vval.v_string = (শূন্য *)floc;
    স্তূপ_TV_BOT(স্তূপ_FRAME_IDX_OFF)->vval.v_সংখ্যা = ectx->ec_frame_idx;
    ectx->ec_frame_idx = ectx->ec_স্তূপ.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    পূর্ণাঙ্ক initcount = dfunc->df_varcount + (dfunc->df_has_closure ? 1 : 0);
    প্রতি (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = স্তূপ_TV_BOT(স্তূপ_FRAME_SIZE + idx);

	tv->v_type = VAR_সংখ্যা;
	tv->vval.v_সংখ্যা = 0;
    }
    ectx->ec_স্তূপ.ga_len += স্তূপ_FRAME_SIZE + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    যদি (IS_OBJECT_METHOD(ufunc))
    {
	যদি (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_internal_error_str), "type in stack is not an object");
	    প্রত্যাগম FAIL;
	}

	*স্তূপ_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != রিক্ত ? pt_arg : ufunc->uf_partial;
    যদি (pt != রিক্ত || (ufunc->uf_flags & FC_CLOSURE))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	যদি (ref == রিক্ত)
	    প্রত্যাগম FAIL;
	যদি (pt != রিক্ত)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	অন্যথা
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    যদি (unlikely(ref->or_outer == রিক্ত))
	    {
		vim_free(ref);
		প্রত্যাগম FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->out_স্তূপ = &ectx->ec_স্তূপ;
	    ref->or_outer->out_frame_idx = ectx->ec_frame_idx;
	    যদি (ectx->ec_outer_ref != রিক্ত)
		ref->or_outer->out_up = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    অন্যথা
	ectx->ec_outer_ref = রিক্ত;

    যদি (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    entry = estack_ঠেলো_ufunc(ufunc, 1);
    যদি (entry != রিক্ত)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	entry->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    প্রত্যাগম OK;
}

// Double linked list of funcstack_T in use.
স্থির funcstack_T *first_funcstack = রিক্ত;

    স্থির শূন্য
add_funcstack_to_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    যদি (first_funcstack != রিক্ত)
	first_funcstack->fs_prev = funcstack;
    funcstack->fs_next = first_funcstack;
    funcstack->fs_prev = রিক্ত;
    first_funcstack = funcstack;
}

    স্থির শূন্য
remove_funcstack_from_list(funcstack_T *funcstack)
{
    যদি (funcstack->fs_prev == রিক্ত)
	first_funcstack = funcstack->fs_next;
    অন্যথা
	funcstack->fs_prev->fs_next = funcstack->fs_next;
    যদি (funcstack->fs_next != রিক্ত)
	funcstack->fs_next->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    স্থির পূর্ণাঙ্ক
handle_closure_in_use(ectx_T *ectx, পূর্ণাঙ্ক free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    পূর্ণাঙ্ক		argcount;
    পূর্ণাঙ্ক		শীর্ষ;
    পূর্ণাঙ্ক		idx;
    typval_T	*tv;
    পূর্ণাঙ্ক		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_গুনতি;

    যদি (dfunc->df_ufunc == রিক্ত)
	প্রত্যাগম OK;  // function was freed
    যদি (dfunc->df_has_closure == 0)
	প্রত্যাগম OK;  // no closures
    tv = স্তূপ_TV(ectx->ec_frame_idx + স্তূপ_FRAME_SIZE + dfunc->df_varcount);
    closure_গুনতি = tv->vval.v_সংখ্যা;
    যদি (closure_গুনতি == 0)
	প্রত্যাগম OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    শীর্ষ = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    প্রতি (idx = 0; idx < closure_গুনতি; ++idx)
    {
	partial_T   *pt;
	পূর্ণাঙ্ক	    off = gap->ga_len - closure_গুনতি + idx;

	যদি (off < 0)
	    অনুবর্ত;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	যদি (pt->pt_refcount > 1)
	{
	    পূর্ণাঙ্ক refcount = pt->pt_refcount;
	    পূর্ণাঙ্ক i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    প্রতি (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = স্তূপ_TV(ectx->ec_frame_idx
						       + স্তূপ_FRAME_SIZE + i);
		যদি (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    যদি (refcount > 1)
	    {
		closure_in_use = TRUE;
		ভঙ্গ;
	    }
	}
    }

    যদি (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *স্তূপ;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	যদি (funcstack == রিক্ত)
	    প্রত্যাগম FAIL;

	funcstack->fs_var_offset = argcount + স্তূপ_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset
							  + dfunc->df_varcount;
	স্তূপ = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = স্তূপ;
	যদি (স্তূপ == রিক্ত)
	{
	    vim_free(funcstack);
	    প্রত্যাগম FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	প্রতি (idx = 0; idx < argcount; ++idx)
	{
	    tv = স্তূপ_TV(শীর্ষ + idx);
	    যদি (free_arguments)
	    {
		*(স্তূপ + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    অন্যথা যদি (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, স্তূপ + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	প্রতি (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = স্তূপ_TV(ectx->ec_frame_idx + স্তূপ_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    যদি (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != রিক্ত)
	    {
		পূর্ণাঙ্ক	    i;

		প্রতি (i = 0; i < closure_গুনতি; ++i)
		    যদি (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_গুনতি + i])
			++funcstack->fs_min_refcount;
	    }

	    *(স্তূপ + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	প্রতি (idx = 0; idx < closure_গুনতি; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_গুনতি + idx];
	    যদি (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_স্তূপ = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - শীর্ষ;
	    }
	}
    }

    প্রতি (idx = 0; idx < closure_গুনতি; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_গুনতি + idx]);
    gap->ga_len -= closure_গুনতি;
    যদি (gap->ga_len == 0)
	ga_clear(gap);

    প্রত্যাগম OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    পূর্ণাঙ্ক
funcstack_check_refcount(funcstack_T *funcstack)
{
    পূর্ণাঙ্ক		i;
    garray_T	*gap = &funcstack->fs_ga;
    পূর্ণাঙ্ক		done = 0;
    typval_T	*স্তূপ;

    যদি (funcstack->fs_refcount > funcstack->fs_min_refcount)
	প্রত্যাগম FALSE;
    প্রতি (i = funcstack->fs_var_offset; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_data) + i;

	যদি (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != রিক্ত
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++done;
    }
    যদি (done != funcstack->fs_min_refcount)
	প্রত্যাগম FALSE;

    স্তূপ = gap->ga_data;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    প্রতি (i = 0; i < gap->ga_len; ++i)
	clear_tv(স্তূপ + i);
    vim_free(স্তূপ);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    প্রত্যাগম TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    পূর্ণাঙ্ক
set_ref_in_funcstacks(পূর্ণাঙ্ক copyID)
{
    funcstack_T *funcstack;

    প্রতি (funcstack = first_funcstack; funcstack != রিক্ত;
						funcstack = funcstack->fs_next)
    {
	typval_T    *স্তূপ = funcstack->fs_ga.ga_data;
	পূর্ণাঙ্ক	    i;

	প্রতি (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    যদি (set_ref_in_item(স্তূপ + i, copyID, রিক্ত, রিক্ত, রিক্ত))
		প্রত্যাগম TRUE;  // abort
    }
    প্রত্যাগম FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
স্থির ectx_T *current_ectx = রিক্ত;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    পূর্ণাঙ্ক
in_def_function(শূন্য)
{
    প্রত্যাগম current_ectx != রিক্ত;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    পূর্ণাঙ্ক
fill_exec_lval_মূল(lval_মূল_T *মূল)
{
    ectx_T *ectx = current_ectx;
    যদি (ectx != রিক্ত)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	যদি (ufunc->uf_class != রিক্ত)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    যদি (tv != রিক্ত)
	    {
		CLEAR_POINTER(মূল);
		মূল->lr_tv = tv;
		copy_tv(স্তূপ_TV_VAR(0), মূল->lr_tv);
		মূল->lr_cl_exec = ufunc->uf_class;
		++মূল->lr_cl_exec->class_refcount;
		প্রত্যাগম OK;
	    }
	}
    }

    প্রত্যাগম FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(শূন্য)
{
    ectx_T *r = current_ectx;

    current_ectx = রিক্ত;
    প্রত্যাগম r;
}

    শূন্য
restore_current_ectx(ectx_T *ectx)
{
    যদি (current_ectx != রিক্ত)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    স্থির list_T *
add_defer_item(পূর্ণাঙ্ক var_idx, পূর্ণাঙ্ক argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = স্তূপ_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    যদি (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	যদি (rettv_list_alloc(defer_tv) == FAIL)
	    প্রত্যাগম রিক্ত;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_বস্তুসমূহ(argcount + 1);
    যদি (l == রিক্ত)
	প্রত্যাগম রিক্ত;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_lock = 0;
    যদি (list_ঢোকাও_tv(defer_l, &listval, defer_l->lv_first) == FAIL)
    {
	list_free(l);
	প্রত্যাগম রিক্ত;
    }

    প্রত্যাগম l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    স্থির পূর্ণাঙ্ক
defer_command(পূর্ণাঙ্ক var_idx, পূর্ণাঙ্ক argcount, ectx_T *ectx)
{
    list_T	*l = add_defer_item(var_idx, argcount, ectx);
    পূর্ণাঙ্ক		i;
    typval_T	*func_tv;

    যদি (l == রিক্ত)
	প্রত্যাগম FAIL;

    func_tv = স্তূপ_TV_BOT(-argcount - 1);
    যদি (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	প্রত্যাগম FAIL;
    }
    list_set_item(l, 0, func_tv);

    প্রতি (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, স্তূপ_TV_BOT(-argcount + i));
    ectx->ec_স্তূপ.ga_len -= argcount + 1;
    প্রত্যাগম OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    পূর্ণাঙ্ক
add_defer_function(char_u *name, পূর্ণাঙ্ক argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    পূর্ণাঙ্ক		i;

    যদি (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	প্রত্যাগম FAIL;
    }

    l = add_defer_item(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    যদি (l == রিক্ত)
    {
	vim_free(name);
	প্রত্যাগম FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_lock = 0;
    func_tv.vval.v_string = name;
    list_set_item(l, 0, &func_tv);

    প্রতি (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, argvars + i);
    প্রত্যাগম OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    স্থির শূন্য
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = স্তূপ_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    যদি (defer_tv->v_type != VAR_LIST)
	প্রত্যাগম;	 // no function added
    FOR_ALL_LIST_বস্তুসমূহ(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ARGS];
	পূর্ণাঙ্ক	    i;
	listitem_T  *arg_li = l->lv_first;
	typval_T    *functv = &l->lv_first->li_tv;
	পূর্ণাঙ্ক	    argcount = l->lv_len - 1;

	যদি (functv->vval.v_string == রিক্ত)
	    // already being called, can happen if function does ":qa"
	    অনুবর্ত;

	প্রতি (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_next;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = রিক্ত;
	partial_T	*pt = রিক্ত;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	যদি (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = রিক্ত;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    যদি (funcexe.fe_object != রিক্ত)
		++funcexe.fe_object->obj_refcount;
	}
	অন্যথা
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = রিক্ত;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(শূন্য)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	যদি (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	অন্যথা
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    স্থির পূর্ণাঙ্ক
func_return(ectx_T *ectx)
{
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    পূর্ণাঙ্ক		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*entry;
    পূর্ণাঙ্ক		prev_dfunc_idx = স্তূপ_TV(ectx->ec_frame_idx
					+ স্তূপ_FRAME_FUNC_OFF)->vval.v_সংখ্যা;
    funclocal_T	*floc;
#ifdef FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_data)
							      + prev_dfunc_idx;

    যদি (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	যদি (dfunc->df_ufunc->uf_profiling
				   || (caller != রিক্ত && caller->uf_profiling))
	{
	    profile_may_end_func(((profinfo_T *)profile_info_ga.ga_data)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#endif

    যদি (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    যদি (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    entry = estack_বের_করো();
    যদি (entry != রিক্ত)
	current_sctx = entry->es_save_sctx;

    যদি (handle_closure_in_use(ectx, TRUE) == FAIL)
	প্রত্যাগম FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    পূর্ণাঙ্ক শীর্ষ = ectx->ec_frame_idx - argcount;
    যদি (IS_OBJECT_METHOD(dfunc->df_ufunc))
	--শীর্ষ;
    প্রতি (idx = শীর্ষ; idx < ectx->ec_frame_idx; ++idx)
	clear_tv(স্তূপ_TV(idx));

    // Clear local variables and temp values, but not the return value.
    প্রতি (idx = ectx->ec_frame_idx + স্তূপ_FRAME_SIZE;
					idx < ectx->ec_স্তূপ.ga_len - 1; ++idx)
	clear_tv(স্তূপ_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_স্তূপ.ga_len - 1;
    যদি (ret_idx == ectx->ec_frame_idx + স্তূপ_FRAME_IDX_OFF)
	ret_idx = 0;

    যদি (ectx->ec_outer_ref != রিক্ত)
    {
	যদি (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = স্তূপ_TV(ectx->ec_frame_idx
					+ স্তূপ_FRAME_IIDX_OFF)->vval.v_সংখ্যা;
    ectx->ec_instr = (শূন্য *)স্তূপ_TV(ectx->ec_frame_idx
				       + স্তূপ_FRAME_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (শূন্য *)স্তূপ_TV(ectx->ec_frame_idx
				       + স্তূপ_FRAME_OUTER_OFF)->vval.v_string;
    floc = (শূন্য *)স্তূপ_TV(ectx->ec_frame_idx
				   + স্তূপ_FRAME_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_frame_idx = স্তূপ_TV(ectx->ec_frame_idx
				       + স্তূপ_FRAME_IDX_OFF)->vval.v_সংখ্যা;

    যদি (floc == রিক্ত)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    অন্যথা
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    যদি (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_স্তূপ.ga_len = শীর্ষ + 1;
	*স্তূপ_TV_BOT(-1) = *স্তূপ_TV(ret_idx);
    }
    অন্যথা
	// Reset the stack to the position before the call.
	ectx->ec_স্তূপ.ga_len = শীর্ষ;

    funcdepth_decrement();
    --ex_nesting_level;
    প্রত্যাগম OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    স্থির পূর্ণাঙ্ক
call_prepare(পূর্ণাঙ্ক argcount, typval_T *argvars, ectx_T *ectx)
{
    পূর্ণাঙ্ক		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    প্রতি (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *স্তূপ_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    যদি (argcount > 0)
	ectx->ec_স্তূপ.ga_len -= argcount - 1;
    অন্যথা যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
	প্রত্যাগম FAIL;
    অন্যথা
	++ectx->ec_স্তূপ.ga_len;

    // Default return value is zero.
    tv = স্তূপ_TV_BOT(-1);
    tv->v_type = VAR_সংখ্যা;
    tv->vval.v_সংখ্যা = 0;
    tv->v_lock = 0;

    প্রত্যাগম OK;
}

/*
 * Call a builtin function by index.
 */
    স্থির পূর্ণাঙ্ক
call_bfunc(পূর্ণাঙ্ক func_idx, পূর্ণাঙ্ক argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    বর্ণ	*save_func_name = ectx->ec_where.wt_func_name;

    যদি (call_prepare(argcount, argvars, ectx) == FAIL)
	প্রত্যাগম FAIL;

    // Check for void value being passed as an argument.
    প্রতি (idx = 0; idx < argcount; ++idx)
	যদি (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_মান));
	    প্রতি (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    প্রত্যাগম FAIL;
	}

    ectx->ec_where.wt_func_name = internal_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_internal_func_by_idx(func_idx, argvars, স্তূপ_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    প্রতি (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    যদি (did_emsg > did_emsg_before)
	প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    স্থির পূর্ণাঙ্ক
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	পূর্ণাঙ্ক	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    funcexe_T   funcexe;
    funcerror_T	error;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    যদি (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, রিক্ত)
								       == FAIL)
	প্রত্যাগম FAIL;
    যদি (ufunc->uf_def_status == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	যদি (error != FCERR_UNKNOWN)
	{
	    যদি (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    অন্যথা
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    প্রত্যাগম FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	যদি (iptr != রিক্ত)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	প্রত্যাগম call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    যদি (call_prepare(argcount, argvars, ectx) == FAIL)
	প্রত্যাগম FAIL;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != রিক্ত ? selfdict : dict_স্তূপ_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      স্তূপ_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    প্রতি (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    যদি (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_found_var);
	প্রত্যাগম FAIL;
    }
    যদি (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}

/*
 * If command modifiers were applied restore them.
 */
    স্থির শূন্য
may_restore_cmdmod(funclocal_T *funclocal)
{
    যদি (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = রিক্ত;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    স্থির পূর্ণাঙ্ক
vim9_aborting(পূর্ণাঙ্ক prev_uncaught_emsg)
{
    প্রত্যাগম uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    স্থির পূর্ণাঙ্ক
call_by_name(
	char_u	    *name,
	পূর্ণাঙ্ক	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    যদি (builtin_function(name, -1))
    {
	পূর্ণাঙ্ক func_idx = find_internal_func(name);

	যদি (func_idx < 0)  // Impossible?
	    প্রত্যাগম FAIL;
	যদি (check_internal_func(func_idx, argcount) < 0)
	    প্রত্যাগম FAIL;
	প্রত্যাগম call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*start_bracket = generic_func_find_open_bracket(name);
    যদি (start_bracket != রিক্ত)
    {
	cc = *start_bracket;
	*start_bracket = NUL;
    }

    ufunc = find_func(name, FALSE);

    যদি (ufunc == রিক্ত)
    {
	পূর্ণাঙ্ক prev_uncaught_emsg = uncaught_emsg;

	যদি (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = find_func(name, FALSE);

	যদি (vim9_aborting(prev_uncaught_emsg))
	{
	    যদি (start_bracket != রিক্ত)
		*start_bracket = cc;
	    প্রত্যাগম FAIL;  // bail out if loading the script caused an error
	}
    }

    যদি (start_bracket != রিক্ত)
	*start_bracket = cc;

    যদি (ufunc != রিক্ত)
    {
	যদি (IS_GENERIC_FUNC(ufunc))
	{
	    যদি (start_bracket != রিক্ত)
		ufunc = find_generic_func(ufunc, name, &start_bracket);
	    অন্যথা
	    {
		emsg_funcname(e_generic_func_missing_type_args_str, name);
		ufunc = রিক্ত;
	    }
	    যদি (ufunc == রিক্ত)
		প্রত্যাগম FAIL;
	}
	অন্যথা যদি (start_bracket != রিক্ত)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    প্রত্যাগম FAIL;
	}

	যদি (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    প্রত্যাগম FAIL;

	প্রত্যাগম call_ufunc(ufunc, রিক্ত, argcount, ectx, iptr, selfdict);
    }

    প্রত্যাগম FAIL;
}

    স্থির পূর্ণাঙ্ক
call_partial(
	typval_T    *tv,
	পূর্ণাঙ্ক	    argcount_arg,
	ectx_T	    *ectx)
{
    পূর্ণাঙ্ক		argcount = argcount_arg;
    char_u	*name = রিক্ত;
    পূর্ণাঙ্ক		called_emsg_before = called_emsg;
    পূর্ণাঙ্ক		res = FAIL;
    dict_T	*selfdict = রিক্ত;

    যদি (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	পূর্ণাঙ্ক	    i;

	যদি (pt->pt_obj != রিক্ত)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		প্রত্যাগম FAIL;
	    প্রতি (i = 1; i <= argcount; ++i)
		*স্তূপ_TV_BOT(-i + 1) = *স্তূপ_TV_BOT(-i);

	    typval_T *obj_tv = স্তূপ_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_lock = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_স্তূপ.ga_len;
	}

	যদি (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, pt->pt_argc))
		প্রত্যাগম FAIL;
	    প্রতি (i = 1; i <= argcount; ++i)
		*স্তূপ_TV_BOT(-i + pt->pt_argc) = *স্তূপ_TV_BOT(-i);
	    ectx->ec_স্তূপ.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    প্রতি (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], স্তূপ_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	যদি (pt->pt_func != রিক্ত)
	    প্রত্যাগম call_ufunc(pt->pt_func, pt, argcount, ectx, রিক্ত, selfdict);

	name = pt->pt_name;
    }
    অন্যথা যদি (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    যদি (name != রিক্ত)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = রিক্ত;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	যদি (error != FCERR_NONE)
	    res = FAIL;
	অন্যথা
	    res = call_by_name(fname, argcount, ectx, রিক্ত, selfdict);
	vim_free(tofree);
    }

    যদি (res == FAIL)
    {
	যদি (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == রিক্ত ? (char_u *)"[unknown]" : name);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    স্থির পূর্ণাঙ্ক
error_if_locked(পূর্ণাঙ্ক lock, বর্ণ *error)
{
    যদি (lock & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    স্থির পূর্ণাঙ্ক
check_for_সংখ্যা(typval_T *tv)
{
    যদি (tv->v_type != VAR_সংখ্যা)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_সংখ্যা), vartype_name(tv->v_type));
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    স্থির শূন্য
store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    পূর্ণাঙ্ক		    flags = ASSIGN_DECL;

    যদি (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, রিক্ত, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    স্থির পূর্ণাঙ্ক
do_2string(typval_T *tv, পূর্ণাঙ্ক is_2string_any, পূর্ণাঙ্ক tostring_flags)
{
    যদি (tv->v_type == VAR_STRING)
	প্রত্যাগম OK;

    char_u *str;

    যদি (is_2string_any)
    {
	বিকল্প (tv->v_type)
	{
	    পক্ষ VAR_SPECIAL:
	    পক্ষ VAR_BOOL:
	    পক্ষ VAR_সংখ্যা:
	    পক্ষ VAR_FLOAT:
	    পক্ষ VAR_DICT:
	    পক্ষ VAR_BLOB:	ভঙ্গ;

	    পক্ষ VAR_LIST:
				যদি (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, পরিমাণ(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    যদি (str == রিক্ত)
					প্রত্যাগম FAIL;
				    s = str;
				    যাবত্ ((e = vim_strchr(s, '\n')) != রিক্ত)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					যদি (p != রিক্ত)
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
				    প্রত্যাগম OK;
				}
				যদি (tostring_flags & TOSTRING_INTERPOLATE)
				    ভঙ্গ;
				// FALLTHROUGH
	    স্বভাব:	to_string_error(tv->v_type);
			প্রত্যাগম FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    প্রত্যাগম OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    স্থির শূন্য
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    বিকল্প (tv->v_type)
    {
	পক্ষ VAR_LIST:
	    যদি (tv->vval.v_list == রিক্ত && sv->sv_type != &t_list_empty)
		(শূন্য)rettv_list_alloc(tv);
	    ভঙ্গ;
	পক্ষ VAR_TUPLE:
	    যদি (tv->vval.v_tuple == রিক্ত && sv->sv_type != &t_tuple_empty)
		(শূন্য)rettv_tuple_alloc(tv);
	    ভঙ্গ;
	পক্ষ VAR_DICT:
	    যদি (tv->vval.v_dict == রিক্ত && sv->sv_type != &t_dict_empty)
		(শূন্য)rettv_dict_alloc(tv);
	    ভঙ্গ;
	পক্ষ VAR_BLOB:
	    যদি (tv->vval.v_blob == রিক্ত && sv->sv_type != &t_blob_null)
		(শূন্য)rettv_blob_alloc(tv);
	    ভঙ্গ;
	স্বভাব:
	    ভঙ্গ;
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

    যদি (str == রিক্ত)
	প্রত্যাগম রিক্ত;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    যদি (index < 0)
    {
	পূর্ণাঙ্ক	clen = 0;

	প্রতি (nbyte = 0; nbyte < slen; ++clen)
	{
	    যদি (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    অন্যথা যদি (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    অন্যথা
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + index;
	যদি (nchar < 0)
	    // unlike list: index out of range results in empty string
	    প্রত্যাগম রিক্ত;
    }

    প্রতি (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	যদি (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	অন্যথা যদি (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	অন্যথা
	    nbyte += mb_ptr2len(str + nbyte);
    }
    যদি (nbyte >= slen)
	প্রত্যাগম রিক্ত;
    প্রত্যাগম vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    স্থির long
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    যদি (nchar >= 0)
    {
	যাবত্ (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    অন্যথা
    {
	nbyte = str_len;
	যাবত্ (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_head_off(str, str + nbyte);
	    ++nchar;
	}
	যদি (nchar < 0)
	    প্রত্যাগম -1;
    }
    প্রত্যাগম (long)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_slice(char_u *str, varnumber_T first, varnumber_T last, পূর্ণাঙ্ক exclusive)
{
    long	start_byte, end_byte;
    size_t	slen;

    যদি (str == রিক্ত)
	প্রত্যাগম রিক্ত;
    slen = STRLEN(str);
    start_byte = char_idx2byte(str, slen, first);
    যদি (start_byte < 0)
	start_byte = 0; // first index very negative: use zero
    যদি ((last == -1 && !exclusive) || last == VARNUM_MAX)
	end_byte = (long)slen;
    অন্যথা
    {
	end_byte = char_idx2byte(str, slen, last);
	যদি (!exclusive && end_byte >= 0 && end_byte < (long)slen)
	    // end index is inclusive
	    end_byte += mb_ptr2len(str + end_byte);
    }

    যদি (start_byte >= (long)slen || end_byte <= start_byte)
	প্রত্যাগম রিক্ত;
    প্রত্যাগম vim_strnsave(str + start_byte, end_byte - start_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    স্থির svar_T *
get_script_svar(scriptref_T *sref, পূর্ণাঙ্ক dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? রিক্ত
			      : ((dfunc_T *)def_functions.ga_data) + dfunc_idx;
    svar_T	    *sv;

    যদি (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	যদি (dfunc != রিক্ত)
	    semsg(_(e_script_variable_invalid_after_reload_in_function_str),
					 printable_func_name(dfunc->df_ufunc));
	প্রত্যাগম রিক্ত;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_data) + sref->sref_idx;
    যদি (sv->sv_name == রিক্ত)
    {
	যদি (dfunc != রিক্ত)
	    emsg(_(e_script_variable_was_deleted));
	প্রত্যাগম রিক্ত;
    }
    যদি (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	যদি (dfunc != রিক্ত)
	    emsg(_(e_script_variable_type_changed));
	প্রত্যাগম রিক্ত;
    }

    যদি ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	যদি (dfunc != রিক্ত)
	    semsg(_(e_item_not_exported_in_script_str), sv->sv_name);
	প্রত্যাগম রিক্ত;
    }
    প্রত্যাগম sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    স্থির char_u *
get_split_sourceline(
	পূর্ণাঙ্ক c UNUSED,
	শূন্য *cookie,
	পূর্ণাঙ্ক indent UNUSED,
	getline_opt_T options UNUSED)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*p;
    char_u		*সারি;

    p = vim_strchr(sp->nextline, '\n');
    যদি (p == রিক্ত)
    {
	সারি = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    অন্যথা
    {
	সারি = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    প্রত্যাগম সারি;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    স্থির পূর্ণাঙ্ক
call_eval_func(
	char_u	    *name,
	পূর্ণাঙ্ক	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    পূর্ণাঙ্ক	    called_emsg_before = called_emsg;
    পূর্ণাঙ্ক	    res;

    res = call_by_name(name, argcount, ectx, iptr, রিক্ত);
    যদি (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = find_var(name, রিক্ত, FALSE);
	যদি (v == রিক্ত || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    প্রত্যাগম FAIL;
	}
	প্রত্যাগম call_partial(&v->di_tv, argcount, ectx);
    }
    প্রত্যাগম res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    পূর্ণাঙ্ক
fill_partial_and_closure(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    যদি (ufunc->uf_flags & FC_CLOSURE)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.out_স্তূপ = &ectx->ec_স্তূপ;
	pt->pt_outer.out_frame_idx = ectx->ec_frame_idx;
	যদি (ectx->ec_outer_ref != রিক্ত)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.out_up = ectx->ec_outer_ref->or_outer;
	    যদি (ectx->ec_outer_ref->or_partial != রিক্ত)
	    {
		pt->pt_outer.out_up_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.out_up_partial->pt_refcount;
	    }
	}

	যদি (lvi != রিক্ত)
	{
	    পূর্ণাঙ্ক	depth;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    প্রতি (depth = 0; depth < lvi->lvi_depth; ++depth)
	    {
		pt->pt_outer.out_loop[depth].স্তূপ = &ectx->ec_স্তূপ;
		pt->pt_outer.out_loop[depth].var_idx = ectx->ec_frame_idx
			 + স্তূপ_FRAME_SIZE + lvi->lvi_loop[depth].var_idx;
		pt->pt_outer.out_loop[depth].var_গুনতি =
					    lvi->lvi_loop[depth].var_গুনতি;
	    }
	    pt->pt_outer.out_loop_size = lvi->lvi_depth;
	}
	অন্যথা
	    pt->pt_outer.out_loop_size = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	যদি (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    প্রত্যাগম FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_স্তূপ.ga_data) + ectx->ec_frame_idx
		       + স্তূপ_FRAME_SIZE + dfunc->df_varcount)->vval.v_সংখ্যা;

	((partial_T **)ectx->ec_funcrefs.ga_data)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    প্রত্যাগম OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    স্থির পূর্ণাঙ্ক
exec_command(isn_T *iptr, char_u *cmd_string)
{
    source_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_FIELD(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    যদি (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    পূর্ণাঙ্ক
may_load_script(পূর্ণাঙ্ক sid, পূর্ণাঙ্ক *loaded)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);

    যদি (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	যদি (do_source(si->sn_name, FALSE, DOSO_NONE, রিক্ত) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    প্রত্যাগম FAIL;
	}
    }
    প্রত্যাগম OK;
}

// used for v_instr of typval of VAR_INSTR
সংরচনা instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
প্রকারনির্দেশ সংরচনা subs_রাশিমালা_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    পূর্ণাঙ্ক		subs_status;
} subs_রাশিমালা_T;

// Set when calling do_debug().
স্থির ectx_T	*debug_context = রিক্ত;
স্থির পূর্ণাঙ্ক	debug_var_গুনতি;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    পূর্ণাঙ্ক		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    পূর্ণাঙ্ক		    varargs_off;

    যদি (ectx == রিক্ত)
	প্রত্যাগম রিক্ত;
    dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    প্রতি (idx = debug_var_গুনতি - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_names.ga_data)[idx];

	// the variable name may be NULL when not available in this block
	যদি (varname != রিক্ত && STRCMP(varname, name) == 0)
	    প্রত্যাগম স্তূপ_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == রিক্ত ? 0 : 1;
    প্রতি (idx = 0; idx < ufunc->uf_args.ga_len; ++idx)
	যদি (STRCMP(((char_u **)(ufunc->uf_args.ga_data))[idx], name) == 0)
	    প্রত্যাগম স্তূপ_TV(ectx->ec_frame_idx - ufunc->uf_args.ga_len
							  - varargs_off + idx);
    যদি (ufunc->uf_va_name != রিক্ত && STRCMP(ufunc->uf_va_name, name) == 0)
	প্রত্যাগম স্তূপ_TV(ectx->ec_frame_idx - 1);

    প্রত্যাগম রিক্ত;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    পূর্ণাঙ্ক
may_break_in_function(ufunc_T *ufunc)
{
    প্রত্যাগম ufunc->uf_has_breakpoint || debug_has_রাশিমালা_breakpoint();
}

    স্থির শূন্য
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*সারি;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    পূর্ণাঙ্ক		end_lnum = iptr->isn_lnum;
    garray_T	ga;
    পূর্ণাঙ্ক		lnum;

    যদি (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	যদি (!may_break_in_function(ufunc))
	    প্রত্যাগম;

	// check for the next breakpoint if needed
	breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	যদি (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    প্রত্যাগম;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_গুনতি = iptr->isn_arg.debug.dbg_var_names_len;

    প্রতি (ni = iptr + 1; ni->isn_type != ISN_FINISH; ++ni)
	যদি (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    end_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    ভঙ্গ;
	}

    যদি (end_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, পরিমাণ(char_u *), 10);
	প্রতি (lnum = iptr->isn_lnum; lnum < end_lnum
				     && lnum <= ufunc->uf_সারিগুলি.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_সারিগুলি.ga_data)[lnum - 1];

	    যদি (p == রিক্ত)
		অনুবর্ত;  // left over from continuation line
	    p = skipwhite(p);
	    যদি (*p == '#')
		ভঙ্গ;
	    যদি (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_data))[ga.ga_len++] = p;
	    যদি (STRNCMP(p, "def ", 4) == 0)
		ভঙ্গ;
	}
	সারি = ga_concat_strings(&ga, "  ");
	vim_free(ga.ga_data);
    }
    অন্যথা
	সারি = ((char_u **)ufunc->uf_সারিগুলি.ga_data)[iptr->isn_lnum - 1];

    do_debug(সারি == রিক্ত ? (char_u *)"[empty]" : সারি);
    debug_context = রিক্ত;

    যদি (end_lnum > iptr->isn_lnum)
	vim_free(সারি);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    স্থির পূর্ণাঙ্ক
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    যদি (lhs_list->lv_type == রিক্ত || lhs_list->lv_type->tt_member == রিক্ত)
	প্রত্যাগম OK;

    প্রত্যাগম check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    স্থির পূর্ণাঙ্ক
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = স্তূপ_TV_BOT(-2);
    long	lidx = 0;
    typval_T	*tv_dest = স্তূপ_TV_BOT(-1);
    পূর্ণাঙ্ক		status = OK;
    পূর্ণাঙ্ক		check_rhs_type = FALSE;

    যদি (tv_idx->v_type == VAR_সংখ্যা)
	lidx = (long)tv_idx->vval.v_সংখ্যা;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = স্তূপ_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    যদি (dest_type == VAR_OBJECT && tv_dest->vval.v_object == রিক্ত)
    {
	emsg(_(e_using_null_object));
	status = FAIL;
    }
    অন্যথা যদি (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	যদি (dest_type == VAR_DICT)
	    status = do_2string(tv_idx, TRUE, FALSE);
	অন্যথা যদি (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    যদি (obj == রিক্ত)
	    {
		emsg(_(e_using_null_object));
		status = FAIL;
	    }
	    অন্যথা
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		পূর্ণাঙ্ক		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		যদি (m != রিক্ত)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    যদি ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == রিক্ত ||
			     !class_instance_of(ufunc->uf_class, cl)))
		    {
			বর্ণ *বার্তা = (m->ocm_access == VIM_ACCESS_PRIVATE)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(বার্তা, m->ocm_name.string, 0, cl);
			status = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    অন্যথা যদি (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			status = FAIL;
		    অন্যথা
			lidx = m_idx;
		}
		অন্যথা
		{
		    member_not_found_বার্তা(cl, VAR_OBJECT, member, 0);
		    status = FAIL;
		}
	    }
	}
	অন্যথা যদি (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    status = FAIL;
	}
	অন্যথা যদি (dest_type == VAR_LIST && tv_idx->v_type != VAR_সংখ্যা)
	{
	    emsg(_(e_সংখ্যা_expected));
	    status = FAIL;
	}
    }

    যদি (status == OK)
    {
	যদি (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    যদি (list == রিক্ত)
	    {
		emsg(_(e_list_not_set));
		প্রত্যাগম FAIL;
	    }
	    যদি (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    যদি (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_index_out_of_range_nr), lidx);
		প্রত্যাগম FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    যদি (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		প্রত্যাগম FAIL;

	    যদি (lidx < list->lv_len)
	    {
		listitem_T *li = list_find(list, lidx);
		যদি (li == রিক্ত)
		    প্রত্যাগম FAIL;
		যদি (error_if_locked(li->li_tv.v_lock,
					     e_cannot_change_locked_list_item))
		    প্রত্যাগম FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    অন্যথা
	    {
		যদি (error_if_locked(list->lv_lock, e_cannot_change_locked_list))
		    প্রত্যাগম FAIL;
		// append to list, only fails when out of memory
		যদি (list_append_tv(list, tv) == FAIL)
		    প্রত্যাগম NOTDONE;
		clear_tv(tv);
	    }
	}
	অন্যথা যদি (dest_type == VAR_DICT)
	{
	    char_u		*চাবি = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    যদি (dict == রিক্ত)
	    {
		emsg(_(e_dictionary_not_set));
		প্রত্যাগম FAIL;
	    }
	    যদি (চাবি == রিক্ত)
		চাবি = (char_u *)"";
	    di = dict_find(dict, চাবি, -1);
	    যদি (di != রিক্ত)
	    {
		যদি (error_if_locked(di->di_tv.v_lock,
						    e_cannot_change_dict_item))
		    প্রত্যাগম FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    অন্যথা
	    {
		যদি (error_if_locked(dict->dv_lock, e_cannot_change_dict))
		    প্রত্যাগম FAIL;
		// add to dict, only fails when out of memory
		যদি (dict_add_tv(dict, (বর্ণ *)চাবি, tv) == FAIL)
		    প্রত্যাগম NOTDONE;
		clear_tv(tv);
	    }
	}
	অন্যথা যদি (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    পূর্ণাঙ্ক		    error = FALSE;
	    পূর্ণাঙ্ক		    len;

	    যদি (blob == রিক্ত)
	    {
		emsg(_(e_blob_not_set));
		প্রত্যাগম FAIL;
	    }
	    len = blob_len(blob);
	    যদি (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    যদি (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), lidx);
		প্রত্যাগম FAIL;
	    }
	    যদি (মান_check_lock(blob->bv_lock, (char_u *)"blob", FALSE))
		প্রত্যাগম FAIL;
	    nr = tv_get_সংখ্যা_chk(tv, &error);
	    যদি (error)
		প্রত্যাগম FAIL;
	    যদি (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_মান_for_blob_nr), nr);
		প্রত্যাগম FAIL;
	    }
	    blob_set_append(blob, lidx, nr);
	}
	অন্যথা যদি (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    যদি (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		যদি (itf != রিক্ত)
		    // convert interface member index to class member index
		    lidx = object_index_from_itf_index(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    অন্যথা
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	অন্যথা যদি (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    status = FAIL;
	}
	অন্যথা
	{
	    status = FAIL;
	    semsg(_(e_cannot_index_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_স্তূপ.ga_len -= 3;
    যদি (status == FAIL)
    {
	clear_tv(tv);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

/*
 * Store a value in a list or blob range.
 */
    স্থির পূর্ণাঙ্ক
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = স্তূপ_TV_BOT(-3);
    typval_T	*tv_idx2 = স্তূপ_TV_BOT(-2);
    typval_T	*tv_dest = স্তূপ_TV_BOT(-1);
    পূর্ণাঙ্ক		status = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = স্তূপ_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    যদি (tv_dest->v_type == VAR_LIST)
    {
	long	    n1;
	long	    n2;
	listitem_T  *li1;

	n1 = (long)tv_get_সংখ্যা_chk(tv_idx1, রিক্ত);
	যদি (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_সংখ্যা == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	অন্যথা
	    n2 = (long)tv_get_সংখ্যা_chk(tv_idx2, রিক্ত);

	li1 = check_range_index_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	যদি (li1 == রিক্ত)
	    status = FAIL;
	অন্যথা
	{
	    status = check_range_index_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    যদি (status != FAIL)
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
    অন্যথা যদি (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	long	    bloblen;

	n1 = tv_get_সংখ্যা_chk(tv_idx1, রিক্ত);
	যদি (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_সংখ্যা == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	অন্যথা
	    n2 = tv_get_সংখ্যা_chk(tv_idx2, রিক্ত);
	bloblen = blob_len(tv_dest->vval.v_blob);

	যদি (check_blob_index(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    status = FAIL;
	অন্যথা
	    status = blob_set_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    অন্যথা
    {
	status = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_স্তূপ.ga_len -= 4;
    clear_tv(tv);

    প্রত্যাগম status;
}

/*
 * Unlet item in list or dict variable.
 */
    স্থির পূর্ণাঙ্ক
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = স্তূপ_TV_BOT(-2);
    typval_T	*tv_dest = স্তূপ_TV_BOT(-1);
    পূর্ণাঙ্ক		status = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    যদি (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	যদি (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_সংখ্যা)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    status = FAIL;
	}
	অন্যথা
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*চাবি;
	    dictitem_T  *di = রিক্ত;

	    যদি (d != রিক্ত && মান_check_lock(
				      d->dv_lock, রিক্ত, FALSE))
		status = FAIL;
	    অন্যথা
	    {
		যদি (tv_idx->v_type == VAR_STRING)
		{
		    চাবি = tv_idx->vval.v_string;
		    যদি (চাবি == রিক্ত)
			চাবি = (char_u *)"";
		}
		অন্যথা
		{
		    চাবি = tv_get_string(tv_idx);
		}
		যদি (d != রিক্ত)
		    di = dict_find(d, চাবি, (পূর্ণাঙ্ক)STRLEN(চাবি));
		যদি (di == রিক্ত)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_চাবি_not_present_in_dictionary_str), চাবি);
		    status = FAIL;
		}
		অন্যথা যদি (var_check_fixed(di->di_flags,
						   রিক্ত, FALSE)
			|| var_check_ro(di->di_flags,
						  রিক্ত, FALSE))
		    status = FAIL;
		অন্যথা
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    অন্যথা যদি (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	যদি (check_for_সংখ্যা(tv_idx) == FAIL)
	{
	    status = FAIL;
	}
	অন্যথা
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n = (long)tv_idx->vval.v_সংখ্যা;

	    যদি (l != রিক্ত && মান_check_lock(
				      l->lv_lock, রিক্ত, FALSE))
		status = FAIL;
	    অন্যথা
	    {
		listitem_T	*li = list_find(l, n);

		যদি (li == রিক্ত)
		{
		    semsg(_(e_list_index_out_of_range_nr), n);
		    status = FAIL;
		}
		অন্যথা
		    listitem_remove(l, li);
	    }
	}
    }
    অন্যথা
    {
	status = FAIL;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_স্তূপ.ga_len -= 2;

    প্রত্যাগম status;
}

/*
 * Unlet a range of items in a list variable.
 */
    স্থির পূর্ণাঙ্ক
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = স্তূপ_TV_BOT(-3);
    typval_T	*tv_idx2 = স্তূপ_TV_BOT(-2);
    typval_T	*tv_dest = স্তূপ_TV_BOT(-1);
    পূর্ণাঙ্ক		status = OK;

    যদি (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	যদি (check_for_সংখ্যা(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_সংখ্যা(tv_idx2) == FAIL))
	{
	    status = FAIL;
	}
	অন্যথা
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n1 = (long)tv_idx1->vval.v_সংখ্যা;
	    long	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (long)tv_idx2->vval.v_সংখ্যা;
	    listitem_T	*li;

	    li = list_find_index(l, &n1);
	    যদি (li == রিক্ত)
	    {
		semsg(_(e_list_index_out_of_range_nr),
						 (long)tv_idx1->vval.v_সংখ্যা);
		status = FAIL;
	    }
	    অন্যথা
	    {
		যদি (n1 < 0)
		    n1 = list_idx_of_item(l, li);
		যদি (n2 < 0)
		{
		    listitem_T *li2 = list_find(l, n2);

		    যদি (li2 == রিক্ত)
		    {
			semsg(_(e_list_index_out_of_range_nr), n2);
			status = FAIL;
		    }
		    অন্যথা
			n2 = list_idx_of_item(l, li2);
		}
		যদি (status != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_index_out_of_range_nr), n2);
		    status = FAIL;
		}
		যদি (status != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    অন্যথা
    {
	status = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_স্তূপ.ga_len -= 3;

    প্রত্যাগম status;
}

/*
 * Top of a for loop.
 */
    স্থির পূর্ণাঙ্ক
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    পূর্ণাঙ্ক		jump = FALSE;
    typval_T	*ltv = স্তূপ_TV_BOT(-1);
    typval_T	*idxtv =
		   স্তূপ_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
	প্রত্যাগম FAIL;
    যদি (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_সংখ্যা;
	যদি (list == রিক্ত
		       || idxtv->vval.v_সংখ্যা >= list->lv_len)
	{
	    jump = TRUE;
	}
	অন্যথা যদি (list->lv_first == &range_list_item)
	{
	    // non-materialized range() list
	    tv = স্তূপ_TV_BOT(0);
	    tv->v_type = VAR_সংখ্যা;
	    tv->v_lock = 0;
	    tv->vval.v_সংখ্যা = list_find_nr(
			     list, idxtv->vval.v_সংখ্যা, রিক্ত);
	    ++ectx->ec_স্তূপ.ga_len;
	}
	অন্যথা
	{
	    listitem_T *li = list_find(list,
					 idxtv->vval.v_সংখ্যা);
	    যদি (li == রিক্ত)
		প্রত্যাগম FAIL;
	    copy_tv(&li->li_tv, স্তূপ_TV_BOT(0));
	    ++ectx->ec_স্তূপ.ga_len;
	}
    }
    অন্যথা যদি (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_সংখ্যা;
	যদি (tuple == রিক্ত || idxtv->vval.v_সংখ্যা >= TUPLE_LEN(tuple))
	    jump = TRUE;
	অন্যথা
	{
	    copy_tv(TUPLE_ITEM(tuple, idxtv->vval.v_সংখ্যা), স্তূপ_TV_BOT(0));
	    ++ectx->ec_স্তূপ.ga_len;
	}
    }
    অন্যথা যদি (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_সংখ্যা;
	যদি (str == রিক্ত || str[idxtv->vval.v_সংখ্যা] == NUL)
	{
	    jump = TRUE;
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক	clen = mb_ptr2len(str + idxtv->vval.v_সংখ্যা);

	    // Push the next character from the string.
	    tv = স্তূপ_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_সংখ্যা, clen);
	    ++ectx->ec_স্তূপ.ga_len;
	    idxtv->vval.v_সংখ্যা += clen - 1;
	}
    }
    অন্যথা যদি (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	যদি (idxtv->vval.v_সংখ্যা == -1 && blob != রিক্ত)
	{
	    ltv->v_lock = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_সংখ্যা;
	যদি (blob == রিক্ত || idxtv->vval.v_সংখ্যা >= blob_len(blob))
	{
	    jump = TRUE;
	}
	অন্যথা
	{
	    // Push the next byte from the blob.
	    tv = স্তূপ_TV_BOT(0);
	    tv->v_type = VAR_সংখ্যা;
	    tv->vval.v_সংখ্যা = blob_get(blob,
					 idxtv->vval.v_সংখ্যা);
	    ++ectx->ec_স্তূপ.ga_len;
	}
    }
    অন্যথা
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	প্রত্যাগম FAIL;
    }

    যদি (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_end;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    অন্যথা
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = স্তূপ_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_সংখ্যা = ectx->ec_funcrefs.ga_len;
    }

    প্রত্যাগম OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
স্থির loopvars_T *first_loopvars = রিক্ত;

    স্থির শূন্য
add_loopvars_to_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    যদি (first_loopvars != রিক্ত)
	first_loopvars->lvs_prev = loopvars;
    loopvars->lvs_next = first_loopvars;
    loopvars->lvs_prev = রিক্ত;
    first_loopvars = loopvars;
}

    স্থির শূন্য
remove_loopvars_from_list(loopvars_T *loopvars)
{
    যদি (loopvars->lvs_prev == রিক্ত)
	first_loopvars = loopvars->lvs_next;
    অন্যথা
	loopvars->lvs_prev->lvs_next = loopvars->lvs_next;
    যদি (loopvars->lvs_next != রিক্ত)
	loopvars->lvs_next->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    স্থির পূর্ণাঙ্ক
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = স্তূপ_TV_VAR(endloop->end_funcref_idx);
    পূর্ণাঙ্ক		prev_closure_গুনতি = tv_refcount->vval.v_সংখ্যা;
    পূর্ণাঙ্ক		depth = endloop->end_depth;
    garray_T	*gap = &ectx->ec_funcrefs;
    পূর্ণাঙ্ক		closure_in_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *স্তূপ;
    পূর্ণাঙ্ক		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    প্রতি (idx = prev_closure_গুনতি; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	যদি (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == রিক্ত)
	{
	    পূর্ণাঙ্ক refcount = pt->pt_refcount;
	    পূর্ণাঙ্ক i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    প্রতি (i = 0; i < endloop->end_var_গুনতি; ++i)
	    {
		typval_T *stv = স্তূপ_TV_VAR(endloop->end_var_idx + i);

		যদি (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    যদি (refcount > 1)
	    {
		closure_in_use = TRUE;
		ভঙ্গ;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    যদি (!closure_in_use)
	প্রত্যাগম OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    যদি (loopvars == রিক্ত)
	প্রত্যাগম FAIL;

    loopvars->lvs_ga.ga_len = endloop->end_var_গুনতি;
    স্তূপ = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_data = স্তূপ;
    যদি (স্তূপ == রিক্ত)
    {
	vim_free(loopvars);
	প্রত্যাগম FAIL;
    }
    add_loopvars_to_list(loopvars);

    // Move the variable values.
    প্রতি (idx = 0; idx < endloop->end_var_গুনতি; ++idx)
    {
	typval_T *tv = স্তূপ_TV_VAR(endloop->end_var_idx + idx);

	*(স্তূপ + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    প্রতি (idx = prev_closure_গুনতি; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	যদি (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == রিক্ত)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[depth] = loopvars;

	    pt->pt_outer.out_loop[depth].স্তূপ = &loopvars->lvs_ga;
	    pt->pt_outer.out_loop[depth].var_idx -=
		  ectx->ec_frame_idx + স্তূপ_FRAME_SIZE + endloop->end_var_idx;
	}
    }

    প্রত্যাগম OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    পূর্ণাঙ্ক
loopvars_check_refcount(loopvars_T *loopvars)
{
    পূর্ণাঙ্ক		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    পূর্ণাঙ্ক		    done = 0;
	typval_T	*স্তূপ = gap->ga_data;

    যদি (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	প্রত্যাগম FALSE;
    প্রতি (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_data) + i;

	যদি (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != রিক্ত
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    পূর্ণাঙ্ক	    depth;

	    প্রতি (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		যদি (tv->vval.v_partial->pt_loopvars[depth] == loopvars)
		    ++done;
	}
    }
    যদি (done != loopvars->lvs_min_refcount)
	প্রত্যাগম FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    স্তূপ = gap->ga_data;
    প্রতি (i = 0; i < gap->ga_len; ++i)
	clear_tv(স্তূপ + i);
    vim_free(স্তূপ);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    প্রত্যাগম TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    পূর্ণাঙ্ক
set_ref_in_loopvars(পূর্ণাঙ্ক copyID)
{
    loopvars_T *loopvars;

    প্রতি (loopvars = first_loopvars; loopvars != রিক্ত;
						 loopvars = loopvars->lvs_next)
    {
	typval_T    *স্তূপ = loopvars->lvs_ga.ga_data;
	পূর্ণাঙ্ক	    i;

	প্রতি (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    যদি (set_ref_in_item(স্তূপ + i, copyID, রিক্ত, রিক্ত, রিক্ত))
		প্রত্যাগম TRUE;  // abort
    }
    প্রত্যাগম FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    স্থির পূর্ণাঙ্ক
load_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = রিক্ত;
    hashtab_T	*ht = রিক্ত;
    বর্ণ	namespace;

    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
	প্রত্যাগম NOTDONE;
    বিকল্প (isn_type)
    {
	পক্ষ ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    ভঙ্গ;
	পক্ষ ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    ভঙ্গ;
	পক্ষ ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    ভঙ্গ;
	পক্ষ ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    ভঙ্গ;
	স্বভাব:  // Cannot reach here
	    প্রত্যাগম NOTDONE;
    }
    di = find_var_in_ht(ht, 0, iptr->isn_arg.string, TRUE);

    যদি (di == রিক্ত)
    {
	যদি (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = find_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    যদি (ufunc != রিক্ত)
	    {
		typval_T    *tv = স্তূপ_TV_BOT(0);

		++ectx->ec_স্তূপ.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		যদি (tv->vval.v_string == রিক্ত)
		    প্রত্যাগম FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		প্রত্যাগম OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	যদি (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != রিক্ত)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_item_not_found_in_script_str), iptr->isn_arg.string);
	অন্যথা
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = স্তূপ_TV_BOT(0);
	tv->v_type = VAR_সংখ্যা;
	tv->v_lock = 0;
	tv->vval.v_সংখ্যা = 0;
	++ectx->ec_স্তূপ.ga_len;
	প্রত্যাগম FAIL;
    }
    অন্যথা
    {
	copy_tv(&di->di_tv, স্তূপ_TV_BOT(0));
	++ectx->ec_স্তূপ.ga_len;
    }
    প্রত্যাগম OK;
}


    স্থির শূন্য
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, পরিমাণ(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    বর্ণ *tofree = রিক্ত;
    বর্ণ *typename = type_name(type, &tofree);
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
    স্থির পূর্ণাঙ্ক
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    পূর্ণাঙ্ক		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = রিক্ত;
    typval_T	mtv;

    যদি (is_object)
    {
	obj = tv->vval.v_object;
	যদি (obj == রিক্ত)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    প্রত্যাগম FAIL;
	}
	tv_cl = obj->obj_class;
    }
    অন্যথা
    {
	tv_cl = tv->vval.v_class;
	যদি (tv_cl == রিক্ত)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    প্রত্যাগম FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    পূর্ণাঙ্ক		namelen = (পূর্ণাঙ্ক)STRLEN(iptr->isn_arg.string);
    পূর্ণাঙ্ক		save_did_emsg = did_emsg;

    যদি (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	প্রত্যাগম OK;
    }

    যদি (did_emsg != save_did_emsg)
	প্রত্যাগম FAIL;

    // could be a class or instance method
    ufunc_T	*oc_method;
    পূর্ণাঙ্ক		oc_method_idx;

    oc_method = method_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_method_idx);
    যদি (oc_method == রিক্ত)
    {
	বর্ণ	*বার্তা;

	SOURCING_LNUM = iptr->isn_lnum;
	যদি (is_object)
	    বার্তা = e_variable_not_found_on_object_str_str;
	অন্যথা
	    বার্তা = e_class_variable_str_not_found_in_class_str;
	semsg(_(বার্তা), iptr->isn_arg.string, tv_cl->class_name.string);
	প্রত্যাগম FAIL;
    }

    // Protected methods are not accessible outside the class
    যদি (*oc_method->uf_name == '_'
			&& !class_instance_of(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_method_str), oc_method->uf_name);
	প্রত্যাগম FAIL;
    }

    // Create a partial for the instance or class method
    যদি (obj_method_to_partial_tv(is_object ? obj : রিক্ত, oc_method, tv)
								== FAIL)
	প্রত্যাগম FAIL;

    প্রত্যাগম OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    স্থির শূন্য
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, পূর্ণাঙ্ক fixindent)
{
    পূর্ণাঙ্ক		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*রাশিমালা = রিক্ত;
    পূর্ণাঙ্ক		dir = FORWARD;

    যদি (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = স্তূপ_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_সংখ্যা;
	যদি (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_স্তূপ.ga_len;
    }
    অন্যথা যদি (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    অন্যথা যদি (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	যদি (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    যদি (regname == '=')
    {
	tv = স্তূপ_TV_BOT(-1);
	যদি (tv->v_type == VAR_STRING)
	    রাশিমালা = tv->vval.v_string;
	অন্যথা
	{
	    রাশিমালা = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_স্তূপ.ga_len;
    }
    check_cursor();

    যদি (fixindent)
	do_put(regname, রাশিমালা, dir, 1L, PUT_সারি|PUT_CURSLINE|PUT_FIXINDENT);
    অন্যথা
	do_put(regname, রাশিমালা, dir, 1L, PUT_সারি|PUT_CURSLINE);
    vim_free(রাশিমালা);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    স্থির পূর্ণাঙ্ক
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    পূর্ণাঙ্ক		গুনতি = iptr->isn_arg.unpack.unp_গুনতি;
    পূর্ণাঙ্ক		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    পূর্ণাঙ্ক		i;

    l = tv->vval.v_list;
    যদি (l == রিক্ত
	    || l->lv_len < (semicolon ? গুনতি - 1 : গুনতি))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_মান_does_not_have_enough_বস্তুসমূহ));
	প্রত্যাগম EXEC_FAIL;
    }
    অন্যথা যদি (!semicolon && l->lv_len > গুনতি)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_মান_has_more_বস্তুসমূহ_than_targets));
	প্রত্যাগম EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, গুনতি - 1))
	প্রত্যাগম EXEC_DONE;
    ectx->ec_স্তূপ.ga_len += গুনতি - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    যদি (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_বস্তুসমূহ(l->lv_len - গুনতি + 1);

	যদি (rem_list == রিক্ত)
	    প্রত্যাগম EXEC_DONE;
	tv = স্তূপ_TV_BOT(-গুনতি);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	li = l->lv_first;
	প্রতি (i = 0; i < গুনতি - 1; ++i)
	    li = li->li_next;
	প্রতি (i = 0; li != রিক্ত; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_set_item(rem_list, i, &tvcopy);
	    li = li->li_next;
	}
	--গুনতি;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_first;
    প্রতি (i = 0; i < গুনতি; ++i)
    {
	tv = স্তূপ_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_next;
    }

    list_unref(l);

    প্রত্যাগম EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    স্থির পূর্ণাঙ্ক
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    পূর্ণাঙ্ক		গুনতি = iptr->isn_arg.unpack.unp_গুনতি;
    পূর্ণাঙ্ক		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    পূর্ণাঙ্ক		i;

    tuple = tv->vval.v_tuple;
    যদি (tuple == রিক্ত
	    || TUPLE_LEN(tuple) < (semicolon ? গুনতি - 1 : গুনতি))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_more_targets_than_tuple_বস্তুসমূহ));
	প্রত্যাগম EXEC_FAIL;
    }
    অন্যথা যদি (!semicolon && TUPLE_LEN(tuple) > গুনতি)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_বস্তুসমূহ));
	প্রত্যাগম EXEC_FAIL;
    }

    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, গুনতি - 1))
	প্রত্যাগম EXEC_DONE;
    ectx->ec_স্তূপ.ga_len += গুনতি - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    যদি (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_বস্তুসমূহ(TUPLE_LEN(tuple) - গুনতি + 1);

	যদি (rem_list == রিক্ত)
	    প্রত্যাগম EXEC_DONE;
	tv = স্তূপ_TV_BOT(-গুনতি);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	প্রতি (i = গুনতি - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ITEM(tuple, i), &tvcopy);
	    list_set_item(rem_list, i - (গুনতি - 1), &tvcopy);
	}
	--গুনতি;
    }

    // Produce the values in reverse order, first item last.
    প্রতি (i = 0; i < গুনতি; ++i)
    {
	tv = স্তূপ_TV_BOT(-i - 1);
	copy_tv(TUPLE_ITEM(tuple, i), tv);
    }

    tuple_unref(tuple);

    প্রত্যাগম EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    স্থির পূর্ণাঙ্ক
exec_instructions(ectx_T *ectx)
{
    পূর্ণাঙ্ক		ret = FAIL;
    পূর্ণাঙ্ক		save_trylevel_at_start = ectx->ec_trylevel_at_start;
    পূর্ণাঙ্ক		dict_স্তূপ_len_at_start = dict_স্তূপ.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_start = trylevel;

    প্রতি (;;)
    {
	স্থির পূর্ণাঙ্ক  breakcheck_গুনতি = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	যদি (unlikely(++breakcheck_গুনতি >= 100))
	{
	    সারি_breakcheck();
	    breakcheck_গুনতি = 0;
	}
	যদি (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    যদি (throw_exception("Vim:Interrupt", ET_INTERRUPT, রিক্ত) == FAIL)
		গচ্ছ theend;
	    did_throw = TRUE;
	}

	যদি (unlikely(did_emsg && বার্তা_list != রিক্ত && *বার্তা_list != রিক্ত))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    যদি (throw_exception(*বার্তা_list, ET_ERROR, রিক্ত) == FAIL)
		গচ্ছ theend;
	    did_throw = TRUE;
	    *বার্তা_list = রিক্ত;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    যদি (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		যদি (trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		{
		    যদি (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			finish_exception(caught_স্তূপ);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	যদি (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = রিক্ত;
	    পূর্ণাঙ্ক		index = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    যাবত্ (index > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_data) + index - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		যদি (!trycmd->tcd_in_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    ভঙ্গ;
		// In the catch and finally block of this try we have to go up
		// one level.
		--index;
		trycmd = রিক্ত;
	    }
	    যদি (trycmd != রিক্ত && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
	    {
		যদি (trycmd->tcd_in_catch)
		{
		    যদি (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    অন্যথা
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		অন্যথা
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_in_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    অন্যথা
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    গচ্ছ theend;
		tv = স্তূপ_TV_BOT(0);
		tv->v_type = VAR_সংখ্যা;
		tv->vval.v_সংখ্যা = 0;
		++ectx->ec_স্তূপ.ga_len;
		যদি (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    যদি (handle_closure_in_use(ectx, FALSE) == FAIL)
			গচ্ছ theend;
		    গচ্ছ done;
		}

		যদি (func_return(ectx) == FAIL)
		    গচ্ছ theend;
	    }
	    অনুবর্ত;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	বিকল্প (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    পক্ষ ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = স্তূপ_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		ভঙ্গ;

	    // execute Ex command line
	    পক্ষ ISN_EXEC:
		যদি (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    গচ্ছ on_error;
		ভঙ্গ;

	    // execute Ex command line split at NL characters.
	    পক্ষ ISN_EXEC_SPLIT:
		{
		    source_cookie_T cookie;
		    char_u	    *সারি;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    সারি = get_split_sourceline(0, &cookie, 0, 0);
		    যদি (do_cmdline(সারি,
				get_split_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(সারি);
			গচ্ছ on_error;
		    }
		    vim_free(সারি);
		}
		ভঙ্গ;

	    // execute Ex command line that is only a range
	    পক্ষ ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    বর্ণ	*error = রিক্ত;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = CMD_SIZE;
		    ea.addr_type = ADDR_সারিগুলি;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_address(&ea, &error, FALSE);
		    যদি (ea.cmd == রিক্ত)
			গচ্ছ on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    যদি (error != রিক্ত)
		    {
			emsg(error);
			গচ্ছ on_error;
		    }
		}
		ভঙ্গ;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    পক্ষ ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    পূর্ণাঙ্ক	    res;
		    পূর্ণাঙ্ক	    save_flags = cmdmod.cmod_flags;

		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    tv = স্তূপ_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, রিক্ত, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    যদি (res == FAIL)
			গচ্ছ on_error;
		    ++ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    // push typeval VAR_INSTR with instructions to be executed
	    পক্ষ ISN_INSTR:
		{
		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    tv = স্তূপ_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    যদি (tv->vval.v_instr == রিক্ত)
			গচ্ছ on_error;
		    ++ectx->ec_স্তূপ.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		ভঙ্গ;

	    পক্ষ ISN_SOURCE:
		{
		    পূর্ণাঙ্ক notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (may_load_script((পূর্ণাঙ্ক)iptr->isn_arg.সংখ্যা, &notused)
								       == FAIL)
			গচ্ছ on_error;
		}
		ভঙ্গ;

	    // execute :substitute with an expression
	    পক্ষ ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    source_cookie_T	cookie;
		    সংরচনা subs_রাশিমালা_S	*save_instr = substitute_instr;
		    সংরচনা subs_রাশিমালা_S	subs_instr;
		    পূর্ণাঙ্ক			res;

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

		    যদি (res == FAIL || did_emsg
					     || subs_instr.subs_status == FAIL)
			গচ্ছ on_error;
		}
		ভঙ্গ;

	    পক্ষ ISN_FINISH:
		গচ্ছ done;

	    পক্ষ ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		যদি (alloc_redir_lval() == FAIL)
		    গচ্ছ on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		ভঙ্গ;

	    পক্ষ ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    {
			vim_free(res);
			গচ্ছ theend;
		    }
		    tv = স্তূপ_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    পক্ষ ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		force_abort = TRUE;
		যদি (trigger_cexpr_autocmd(iptr->isn_arg.সংখ্যা) == FAIL)
		    গচ্ছ on_error;
		force_abort = FALSE;
#endif
		ভঙ্গ;

	    পক্ষ ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    exarg_T ea;
		    পূর্ণাঙ্ক	    res;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_স্তূপ.ga_len;
		    tv = স্তূপ_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    যদি (res == FAIL)
			গচ্ছ on_error;
		}
#endif
		ভঙ্গ;

	    // execute Ex command from pieces on the stack
	    পক্ষ ISN_EXECCONCAT:
		{
		    পূর্ণাঙ্ক	    গুনতি = iptr->isn_arg.সংখ্যা;
		    size_t  len = 0;
		    পূর্ণাঙ্ক	    pass;
		    পূর্ণাঙ্ক	    i;
		    char_u  *cmd = রিক্ত;
		    char_u  *str;

		    প্রতি (pass = 1; pass <= 2; ++pass)
		    {
			প্রতি (i = 0; i < গুনতি; ++i)
			{
			    tv = স্তূপ_TV_BOT(i - গুনতি);
			    str = tv->vval.v_string;
			    যদি (str != রিক্ত && *str != NUL)
			    {
				যদি (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    যদি (pass == 2)
				clear_tv(tv);
			}
			যদি (pass == 1)
			{
			    cmd = alloc(len + 1);
			    যদি (unlikely(cmd == রিক্ত))
				গচ্ছ theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		ভঙ্গ;

	    // execute :echo {string} ...
	    পক্ষ ISN_ECHO:
		{
		    পূর্ণাঙ্ক গুনতি = iptr->isn_arg.echo.echo_গুনতি;
		    পূর্ণাঙ্ক	atstart = TRUE;
		    পূর্ণাঙ্ক needclr = TRUE;
		    পূর্ণাঙ্ক	idx;

		    প্রতি (idx = 0; idx < গুনতি; ++idx)
		    {
			tv = স্তূপ_TV_BOT(idx - গুনতি);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    যদি (needclr)
			বার্তা_clr_eos();
		    ectx->ec_স্তূপ.ga_len -= গুনতি;
		}
		ভঙ্গ;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    পক্ষ ISN_EXECUTE:
	    পক্ষ ISN_ECHOMSG:
	    পক্ষ ISN_ECHOWINDOW:
	    পক্ষ ISN_ECHOCONSOLE:
	    পক্ষ ISN_ECHOERR:
		{
		    পূর্ণাঙ্ক		গুনতি;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    পূর্ণাঙ্ক		len;
		    পূর্ণাঙ্ক		failed = FALSE;
		    পূর্ণাঙ্ক		idx;

		    যদি (iptr->isn_type == ISN_ECHOWINDOW)
			গুনতি = iptr->isn_arg.echowin.ewin_গুনতি;
		    অন্যথা
			গুনতি = iptr->isn_arg.সংখ্যা;
		    ga_init2(&ga, 1, 80);
		    প্রতি (idx = 0; idx < গুনতি; ++idx)
		    {
			tv = স্তূপ_TV_BOT(idx - গুনতি);
			যদি (iptr->isn_type == ISN_EXECUTE)
			{
			    যদি (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_JOB)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_মান_as_string_str),
						    vartype_name(tv->v_type));
				ভঙ্গ;
			    }
			    অন্যথা
				p = tv_get_string_buf(tv, buf);
			}
			অন্যথা
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (পূর্ণাঙ্ক)STRLEN(p);
			যদি (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			অন্যথা
			{
			    যদি (ga.ga_len > 0)
				((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_স্তূপ.ga_len -= গুনতি;
		    যদি (failed)
		    {
			ga_clear(&ga);
			গচ্ছ on_error;
		    }

		    যদি (ga.ga_data != রিক্ত)
		    {
			যদি (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_data);
			    যদি (did_emsg)
			    {
				ga_clear(&ga);
				গচ্ছ on_error;
			    }
			}
			অন্যথা
			{
			    বার্তা_sb_eol();
			    যদি (iptr->isn_type == ISN_ECHOMSG)
			    {
				বার্তা_attr(ga.ga_data, echo_attr);
				out_flush();
			    }
#ifdef HAS_MESSAGE_WINDOW
			    অন্যথা যদি (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				start_echowindow(
					      iptr->isn_arg.echowin.ewin_time);
				বার্তা_attr(ga.ga_data, echo_attr);
				end_echowindow();
			    }
#endif
			    অন্যথা যদি (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_data, (পূর্ণাঙ্ক)STRLEN(ga.ga_data),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    অন্যথা
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_data);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		ভঙ্গ;

	    // load local variable or argument
	    পক্ষ ISN_LOAD:
		যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    গচ্ছ theend;
		tv = স্তূপ_TV_VAR(iptr->isn_arg.সংখ্যা);
		যদি (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    স্তূপ_TV_BOT(0)->v_type = VAR_SPECIAL;
		    স্তূপ_TV_BOT(0)->vval.v_সংখ্যা = VVAL_NONE;
		}
		অন্যথা
		    copy_tv(tv, স্তূপ_TV_BOT(0));
		++ectx->ec_স্তূপ.ga_len;
		ভঙ্গ;

	    // load v: variable
	    পক্ষ ISN_LOADV:
		যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    গচ্ছ theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.সংখ্যা), স্তূপ_TV_BOT(0));
		++ectx->ec_স্তূপ.ga_len;
		ভঙ্গ;

	    // load s: variable in Vim9 script
	    পক্ষ ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    যদি (sv == রিক্ত)
			গচ্ছ theend;
		    allocate_if_null(sv);
		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    copy_tv(sv->sv_tv, স্তূপ_TV_BOT(0));
		    ++ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    // load s: variable in old script or autoload import
	    পক্ষ ISN_LOADS:
	    পক্ষ ISN_LOADEXPORT:
		{
		    পূর্ণাঙ্ক		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = find_var_in_ht(ht, 0, name, TRUE);

		    যদি (di == রিক্ত)
		    {
			ufunc_T	*ufunc = রিক্ত;

			যদি (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (শূন্য)find_exported(sid, name, &ufunc, &type,
							 রিক্ত, রিক্ত, FALSE);
			}
			যদি (ufunc == রিক্ত)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    গচ্ছ on_error;
			}
			যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			    গচ্ছ theend;
			tv = স্তূপ_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_স্তূপ.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    অন্যথা
		    {
			যদি (iptr->isn_type == ISN_LOADEXPORT)
			{
			    পূর্ণাঙ্ক idx = get_script_item_idx(sid, name, 0,
								   রিক্ত, রিক্ত);
			    svar_T	*sv;

			    যদি (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;
				যদি ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    গচ্ছ on_error;
				}
			    }
			}
			যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			    গচ্ছ theend;
			copy_tv(&di->di_tv, স্তূপ_TV_BOT(0));
			++ectx->ec_স্তূপ.ga_len;
		    }
		}
		ভঙ্গ;

	    // load g:/b:/w:/t: variable
	    পক্ষ ISN_LOADG:
	    পক্ষ ISN_LOADB:
	    পক্ষ ISN_LOADW:
	    পক্ষ ISN_LOADT:
		{
		    পূর্ণাঙ্ক res = load_namespace_var(ectx, iptr->isn_type, iptr);

		    যদি (res == NOTDONE)
			গচ্ছ theend;
		    যদি (res == FAIL)
			গচ্ছ on_error;
		}

		ভঙ্গ;

	    // load autoload variable
	    পক্ষ ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (eval_variable(name, (পূর্ণাঙ্ক)STRLEN(name), 0,
			      স্তূপ_TV_BOT(0), রিক্ত, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = স্তূপ_TV_BOT(0);
			tv->v_type = VAR_সংখ্যা;
			tv->v_lock = 0;
			tv->vval.v_সংখ্যা = 0;
			++ectx->ec_স্তূপ.ga_len;
			গচ্ছ on_error;
		    }
		    ++ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    // load g:/b:/w:/t: namespace
	    পক্ষ ISN_LOADGDICT:
	    পক্ষ ISN_LOADBDICT:
	    পক্ষ ISN_LOADWDICT:
	    পক্ষ ISN_LOADTDICT:
		{
		    dict_T *d = রিক্ত;

		    বিকল্প (iptr->isn_type)
		    {
			পক্ষ ISN_LOADGDICT: d = get_globvar_dict(); ভঙ্গ;
			পক্ষ ISN_LOADBDICT: d = curbuf->b_vars; ভঙ্গ;
			পক্ষ ISN_LOADWDICT: d = curwin->w_vars; ভঙ্গ;
			পক্ষ ISN_LOADTDICT: d = curtab->tp_vars; ভঙ্গ;
			স্বভাব:  // Cannot reach here
			    গচ্ছ theend;
		    }
		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    tv = স্তূপ_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_lock = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    // load &option
	    পক্ষ ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    যদি (eval_option(&name, &optval, TRUE) == FAIL)
			গচ্ছ theend;
		    *স্তূপ_TV_BOT(0) = optval;
		    ++ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    // load $ENV
	    পক্ষ ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    // name is always valid, checked when compiling
		    (শূন্য)eval_env_var(&name, &optval, TRUE);
		    *স্তূপ_TV_BOT(0) = optval;
		    ++ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    // load @register
	    পক্ষ ISN_LOADREG:
		যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    গচ্ছ theend;
		tv = স্তূপ_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.সংখ্যা, GREG_রাশিমালা_SRC);
		++ectx->ec_স্তূপ.ga_len;
		ভঙ্গ;

	    // store local variable
	    পক্ষ ISN_STORE:
		--ectx->ec_স্তূপ.ga_len;
		tv = স্তূপ_TV_VAR(iptr->isn_arg.সংখ্যা);
		যদি (check_typval_is_মান(স্তূপ_TV_BOT(0)) == FAIL
			|| স্তূপ_TV_BOT(0)->v_type == VAR_VOID)
		{
		    যদি (স্তূপ_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_মান));
		    clear_tv(স্তূপ_TV_BOT(0));
		    গচ্ছ on_error;
		}
		clear_tv(tv);
		*tv = *স্তূপ_TV_BOT(0);
		ভঙ্গ;

	    // store s: variable in old script or autoload import
	    পক্ষ ISN_STORES:
	    পক্ষ ISN_STOREEXPORT:
		{
		    পূর্ণাঙ্ক		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = রিক্ত;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    যদি (iptr->isn_type == ISN_STOREEXPORT)
			di = find_var_autoload_prefix(name, sid, রিক্ত, রিক্ত);
		    // Then look for a variable in the script's variables.
		    যদি (di == রিক্ত)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = find_var_in_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_স্তূপ.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (di == রিক্ত)
		    {
			যদি (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(স্তূপ_TV_BOT(0));
			    গচ্ছ on_error;
			}
			store_var(name, স্তূপ_TV_BOT(0));
		    }
		    অন্যথা
		    {
			যদি (iptr->isn_type == ISN_STOREEXPORT)
			{
			    পূর্ণাঙ্ক idx = get_script_item_idx(sid, name, 0,
								   রিক্ত, রিক্ত);

			    // can this ever fail?
			    যদি (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;

				যদি ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    clear_tv(স্তূপ_TV_BOT(0));
				    গচ্ছ on_error;
				}
			    }
			}
			যদি (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(স্তূপ_TV_BOT(0));
			    গচ্ছ on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *স্তূপ_TV_BOT(0);
		    }
		}
		ভঙ্গ;

	    // store script-local variable in Vim9 script
	    পক্ষ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    যদি (sv == রিক্ত)
			গচ্ছ theend;
		    --ectx->ec_স্তূপ.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (মান_check_lock(sv->sv_tv->v_lock, sv->sv_name, FALSE))
		    {
			clear_tv(স্তূপ_TV_BOT(0));
			গচ্ছ on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *স্তূপ_TV_BOT(0);
		}
		ভঙ্গ;

	    // store option
	    পক্ষ ISN_STOREOPT:
	    পক্ষ ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    পূর্ণাঙ্ক		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    long	n = 0;
		    char_u	*s = রিক্ত;
		    বর্ণ	*বার্তা;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = রিক্ত;

		    --ectx->ec_স্তূপ.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = স্তূপ_TV_BOT(0);
		    যদি (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			যদি (s == রিক্ত)
			    s = (char_u *)"";
		    }
		    অন্যথা যদি (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			যদি (s == রিক্ত || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    গচ্ছ on_error;
			}
		    }
		    অন্যথা
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_সংখ্যা;
		    বার্তা = set_option_মান(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    যদি (বার্তা != রিক্ত)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(বার্তা));
			গচ্ছ on_error;
		    }
		}
		ভঙ্গ;

	    // store $ENV
	    পক্ষ ISN_STOREENV:
		যদি (check_secure() || check_restricted())
		    গচ্ছ theend;
		--ectx->ec_স্তূপ.ga_len;
		tv = স্তূপ_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		ভঙ্গ;

	    // store @r
	    পক্ষ ISN_STOREREG:
		{
		    পূর্ণাঙ্ক	reg = iptr->isn_arg.সংখ্যা;

		    --ectx->ec_স্তূপ.ga_len;
		    tv = স্তূপ_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		ভঙ্গ;

	    // store v: variable
	    পক্ষ ISN_STOREV:
		--ectx->ec_স্তূপ.ga_len;
		যদি (set_vim_var_tv(iptr->isn_arg.সংখ্যা, স্তূপ_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    গচ্ছ on_error;
		ভঙ্গ;

	    // store g:/b:/w:/t: variable
	    পক্ষ ISN_STOREG:
	    পক্ষ ISN_STOREB:
	    পক্ষ ISN_STOREW:
	    পক্ষ ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    বিকল্প (iptr->isn_type)
		    {
			পক্ষ ISN_STOREG:
			    ht = get_globvar_ht();
			    ভঙ্গ;
			পক্ষ ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    ভঙ্গ;
			পক্ষ ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    ভঙ্গ;
			পক্ষ ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    ভঙ্গ;
			স্বভাব:  // Cannot reach here
			    গচ্ছ theend;
		    }

		    --ectx->ec_স্তূপ.ga_len;
		    di = find_var_in_ht(ht, 0, name, TRUE);
		    যদি (di == রিক্ত)
			store_var(iptr->isn_arg.string, স্তূপ_TV_BOT(0));
		    অন্যথা
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			যদি (var_check_permission(di, name) == FAIL)
			    গচ্ছ on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *স্তূপ_TV_BOT(0);
		    }
		}
		ভঙ্গ;

	    // store an autoload variable
	    পক্ষ ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		set_var(iptr->isn_arg.string, স্তূপ_TV_BOT(-1), TRUE);
		clear_tv(স্তূপ_TV_BOT(-1));
		--ectx->ec_স্তূপ.ga_len;
		ভঙ্গ;

	    // store number in local variable
	    পক্ষ ISN_STORENR:
		tv = স্তূপ_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_সংখ্যা;
		tv->vval.v_সংখ্যা = iptr->isn_arg.storenr.stnr_val;
		ভঙ্গ;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    পক্ষ ISN_STOREINDEX:
		{
		    পূর্ণাঙ্ক res = execute_storeindex(iptr, ectx);

		    যদি (res == FAIL)
			গচ্ছ on_error;
		    যদি (res == NOTDONE)
			গচ্ছ theend;
		}
		ভঙ্গ;

	    // store value in list or blob range
	    পক্ষ ISN_STORERANGE:
		যদি (execute_storerange(iptr, ectx) == FAIL)
		    গচ্ছ on_error;
		ভঙ্গ;

	    পক্ষ ISN_LOAD_CLASSMEMBER:
		{
		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      স্তূপ_TV_BOT(0));
		    ++ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    পক্ষ ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *স্তূপ_TV_BOT(-1);
		    --ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    // Load or store variable or argument from outer scope.
	    পক্ষ ISN_LOADOUTER:
	    পক্ষ ISN_STOREOUTER:
		{
		    পূর্ণাঙ্ক		depth = iptr->isn_arg.outer.outer_depth;
		    outer_T	*outer = ectx->ec_outer_ref == রিক্ত ? রিক্ত
						: ectx->ec_outer_ref->or_outer;

		    যাবত্ (depth > 1 && outer != রিক্ত)
		    {
			outer = outer->out_up;
			--depth;
		    }
		    যদি (outer == রিক্ত)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			যদি (ectx->ec_frame_idx == ectx->ec_initial_frame_idx
						 || ectx->ec_outer_ref == রিক্ত)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_closure_called_from_invalid_context));
			অন্যথা
			    iemsg("LOADOUTER depth more than scope levels");
			গচ্ছ theend;
		    }
		    যদি (depth < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->out_loop[-depth - 1]
							       .স্তূপ->ga_data)
					  + outer->out_loop[-depth - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    অন্যথা
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->out_স্তূপ->ga_data)
				      + outer->out_frame_idx + স্তূপ_FRAME_SIZE
				      + iptr->isn_arg.outer.outer_idx;
		    যদি (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			    গচ্ছ theend;
			// careful: ga_grow_inner may re-alloc the stack
			যদি (depth < 0)
			    copy = ((typval_T *)outer->out_loop[-depth - 1]
								   .স্তূপ->ga_data)
					      + outer->out_loop[-depth - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			অন্যথা
			    copy = ((typval_T *)outer->out_স্তূপ->ga_data)
					  + outer->out_frame_idx + স্তূপ_FRAME_SIZE
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			যদি (copy != tv)
			    tv = copy;
			copy_tv(tv, স্তূপ_TV_BOT(0));
			++ectx->ec_স্তূপ.ga_len;
		    }
		    অন্যথা
		    {
			--ectx->ec_স্তূপ.ga_len;
			clear_tv(tv);
			*tv = *স্তূপ_TV_BOT(0);
		    }
		}
		ভঙ্গ;

	    // unlet item in list or dict variable
	    পক্ষ ISN_UNLETINDEX:
		যদি (execute_unletindex(iptr, ectx) == FAIL)
		    গচ্ছ on_error;
		ভঙ্গ;

	    // unlet range of items in list variable
	    পক্ষ ISN_UNLETRANGE:
		যদি (execute_unletrange(iptr, ectx) == FAIL)
		    গচ্ছ on_error;
		ভঙ্গ;

	    // push constant
	    পক্ষ ISN_PUSHNR:
	    পক্ষ ISN_PUSHBOOL:
	    পক্ষ ISN_PUSHSPEC:
	    পক্ষ ISN_PUSHF:
	    পক্ষ ISN_PUSHS:
	    পক্ষ ISN_PUSHBLOB:
	    পক্ষ ISN_PUSHFUNC:
	    পক্ষ ISN_PUSHCHANNEL:
	    পক্ষ ISN_PUSHJOB:
	    পক্ষ ISN_PUSHOBJ:
	    পক্ষ ISN_PUSHCLASS:
		যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    গচ্ছ theend;
		tv = স্তূপ_TV_BOT(0);
		tv->v_lock = 0;
		++ectx->ec_স্তূপ.ga_len;
		বিকল্প (iptr->isn_type)
		{
		    পক্ষ ISN_PUSHNR:
			tv->v_type = VAR_সংখ্যা;
			tv->vval.v_সংখ্যা = iptr->isn_arg.সংখ্যা;
			ভঙ্গ;
		    পক্ষ ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_সংখ্যা = iptr->isn_arg.সংখ্যা;
			ভঙ্গ;
		    পক্ষ ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_সংখ্যা = iptr->isn_arg.সংখ্যা;
			ভঙ্গ;
		    পক্ষ ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			ভঙ্গ;
		    পক্ষ ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			ভঙ্গ;
		    পক্ষ ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			যদি (iptr->isn_arg.string == রিক্ত)
			    tv->vval.v_string = রিক্ত;
			অন্যথা
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			ভঙ্গ;
		    পক্ষ ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = রিক্ত;
#endif
			ভঙ্গ;
		    পক্ষ ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_JOB;
			tv->vval.v_job = রিক্ত;
#endif
			ভঙ্গ;
		    পক্ষ ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = রিক্ত;
			ভঙ্গ;
		    পক্ষ ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			ভঙ্গ;
		    স্বভাব:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == রিক্ত
				    ? রিক্ত : vim_strsave(iptr->isn_arg.string);
		}
		ভঙ্গ;

	    পক্ষ ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (শূন্য)script_autoload(name, FALSE);
		    যদি (find_func(name, TRUE))
		    {
			যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			    গচ্ছ theend;
			tv = স্তূপ_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_স্তূপ.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    অন্যথা
		    {
			পূর্ণাঙ্ক res = load_namespace_var(ectx, ISN_LOADG, iptr);

			যদি (res == NOTDONE)
			    গচ্ছ theend;
			যদি (res == FAIL)
			    গচ্ছ on_error;
		    }
		}
		ভঙ্গ;

	    পক্ষ ISN_UNLET:
		যদি (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    গচ্ছ on_error;
		ভঙ্গ;
	    পক্ষ ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		ভঙ্গ;

	    পক্ষ ISN_LOCKUNLOCK:
		{
#ifdef LOG_LOCKVAR
		    ch_log(রিক্ত, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#endif
		    lval_মূল_T	*lval_মূল_save = lval_মূল;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_স্তূপ.ga_len;
		    lval_মূল_T মূল;
		    মূল.lr_tv      = স্তূপ_TV_BOT(0);
		    মূল.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    মূল.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_মূল = &মূল;
		    পূর্ণাঙ্ক res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(মূল.lr_tv);
		    lval_মূল = lval_মূল_save;
		    যদি (res == FAIL)
			গচ্ছ on_error;
		}
		ভঙ্গ;

	    পক্ষ ISN_LOCKCONST:
		item_lock(স্তূপ_TV_BOT(-1), 100, TRUE, TRUE);
		ভঙ্গ;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    পক্ষ ISN_NEWLIST:
		যদি (exe_newlist(iptr->isn_arg.সংখ্যা, ectx) == FAIL)
		    গচ্ছ theend;
		ভঙ্গ;

	    // create a tuple from items on the stack
	    পক্ষ ISN_NEWTUPLE:
		যদি (exe_newtuple(iptr->isn_arg.সংখ্যা, ectx) == FAIL)
		    গচ্ছ theend;
		ভঙ্গ;

	    // create a dict from items on the stack
	    পক্ষ ISN_NEWDICT:
		{
		    পূর্ণাঙ্ক res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.সংখ্যা, ectx);
		    যদি (res == FAIL)
			গচ্ছ theend;
		    যদি (res == MAYBE)
			গচ্ছ on_error;
		}
		ভঙ্গ;

	    পক্ষ ISN_CONCAT:
		যদি (exe_concat(iptr->isn_arg.সংখ্যা, ectx) == FAIL)
		    গচ্ছ theend;
		ভঙ্গ;

	    // create a partial with NULL value
	    পক্ষ ISN_NEWPARTIAL:
		যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    গচ্ছ theend;
		++ectx->ec_স্তূপ.ga_len;
		tv = স্তূপ_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_lock = 0;
		tv->vval.v_partial = রিক্ত;
		ভঙ্গ;

	    // call a :def function
	    পক্ষ ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		যদি (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				রিক্ত,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    গচ্ছ on_error;
		ভঙ্গ;

	    // call a method on an interface
	    পক্ষ ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = স্তূপ_TV_BOT(-1 - mfunc->cmf_argcount);
		    যদি (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			গচ্ছ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    যদি (obj == রিক্ত)
		    {
			emsg(_(e_using_null_object));
			গচ্ছ on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    যদি (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    অন্যথা
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			পূর্ণাঙ্ক idx = object_index_from_itf_index(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    যদি (mfunc->cmf_ufunc != রিক্ত && IS_GENERIC_FUNC(ufunc))
		    {
			যদি (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			অন্যথা
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    যদি (ufunc == রিক্ত)
				গচ্ছ on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    যদি (call_ufunc(ufunc, রিক্ত, mfunc->cmf_argcount, ectx,
							   রিক্ত, রিক্ত) == FAIL)
			গচ্ছ on_error;
		}
		ভঙ্গ;

	    // call a builtin function
	    পক্ষ ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		যদি (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    গচ্ছ on_error;
		ভঙ্গ;

	    // call a funcref or partial
	    পক্ষ ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    পূর্ণাঙ্ক		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (pfunc->cpf_শীর্ষ)
		    {
			// funcref is above the arguments
			tv = স্তূপ_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    অন্যথা
		    {
			// Get the funcref from the stack.
			--ectx->ec_স্তূপ.ga_len;
			partial_tv = *স্তূপ_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    যদি (tv == &partial_tv)
			clear_tv(&partial_tv);
		    যদি (r == FAIL)
			গচ্ছ on_error;
		}
		ভঙ্গ;

	    পক্ষ ISN_PCALL_END:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_স্তূপ.ga_len;
		clear_tv(স্তূপ_TV_BOT(-1));
		*স্তূপ_TV_BOT(-1) = *স্তূপ_TV_BOT(0);
		ভঙ্গ;

	    // call a user defined function or funcref/partial
	    পক্ষ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			গচ্ছ on_error;
		}
		ভঙ্গ;

	    // :defer func(arg)
	    পক্ষ ISN_DEFER:
		যদি (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    গচ্ছ on_error;
		ভঙ্গ;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    পক্ষ ISN_RETURN_VOID:
	    পক্ষ ISN_RETURN_OBJECT:
		যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    গচ্ছ theend;
		tv = স্তূপ_TV_BOT(0);
		++ectx->ec_স্তূপ.ga_len;
		যদি (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_সংখ্যা = 0;
		    tv->v_lock = 0;
		}
		অন্যথা
		{
		    *tv = *স্তূপ_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_lock_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    পক্ষ ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = রিক্ত;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    যদি (check_typval_is_মান(স্তূপ_TV_BOT(-1)) == FAIL)
			গচ্ছ theend;

		    যদি (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    যদি (trycmd != রিক্ত
				 && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    {
			// jump to ":finally" or ":endtry"
			যদি (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			অন্যথা
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    অন্যথা
			গচ্ছ func_return;
		}
		ভঙ্গ;

	    // push a partial, a reference to a compiled function
	    পক্ষ ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    যদি (pt == রিক্ত)
			গচ্ছ theend;
		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    {
			vim_free(pt);
			গচ্ছ theend;
		    }
		    যদি (extra != রিক্ত && extra->fre_class != রিক্ত)
		    {
			class_T	*cl;
			যদি (extra->fre_object_method)
			{
			    tv = স্তূপ_TV_BOT(-1);
			    যদি (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				গচ্ছ on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    যদি (obj == রিক্ত)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				গচ্ছ on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_স্তূপ.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			অন্যথা
			    cl = extra->fre_class;

			যদি (extra->fre_object_method)
			{
			    // object method
			    // convert the interface index to the object index
			    পূর্ণাঙ্ক idx =
				object_index_from_itf_index(extra->fre_class,
					TRUE, extra->fre_method_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			অন্যথা
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_method_idx];
			}
		    }
		    অন্যথা যদি (extra == রিক্ত || extra->fre_func_name == রিক্ত)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    অন্যথা
		    {
			ufunc = find_func(extra->fre_func_name, FALSE);
		    }
		    যদি (ufunc == রিক্ত)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			গচ্ছ theend;
		    }
		    যদি (extra != রিক্ত && extra->fre_outer_depth > 0
					   && (ufunc->uf_flags & FC_CLOSURE))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == রিক্ত
				    ? রিক্ত : ectx->ec_outer_ref->or_partial;

			প্রতি (পূর্ণাঙ্ক depth = 1; outer_pt != রিক্ত
				    && depth < extra->fre_outer_depth; ++depth)
			    outer_pt = get_pt_outer(outer_pt)->out_up_partial;
			যদি (outer_pt == রিক্ত)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    গচ্ছ theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    অন্যথা যদি (fill_partial_and_closure(pt, ufunc,
			       extra == রিক্ত ? রিক্ত : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			গচ্ছ theend;
		    }
		    tv = স্তূপ_TV_BOT(0);
		    ++ectx->ec_স্তূপ.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_lock = 0;
		}
		ভঙ্গ;

	    // Create a global function from a lambda.
	    পক্ষ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    যদি (copy_lambda_to_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			গচ্ছ theend;
		}
		ভঙ্গ;

	    // List functions
	    পক্ষ ISN_DEF:
		যদি (iptr->isn_arg.string == রিক্ত)
		    list_functions(রিক্ত);
		অন্যথা
		{
		    exarg_T	ea;
		    garray_T	সারিগুলি_to_free;

		    CLEAR_FIELD(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&সারিগুলি_to_free, পরিমাণ(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, রিক্ত, &সারিগুলি_to_free, 0, রিক্ত, 0, রিক্ত);
		    ga_clear_strings(&সারিগুলি_to_free);
		}
		ভঙ্গ;

	    // jump if a condition is met
	    পক্ষ ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    পূর্ণাঙ্ক		error = FALSE;
		    পূর্ণাঙ্ক		jump = TRUE;

		    যদি (when != JUMP_ALWAYS)
		    {
			tv = স্তূপ_TV_BOT(-1);
			যদি (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    যদি (error)
				গচ্ছ on_error;
			}
			অন্যথা
			    jump = tv2bool(tv);
			যদি (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			যদি (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_স্তূপ.ga_len;
			}
		    }
		    যদি (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		ভঙ্গ;

	    // "while": jump to end if a condition is false
	    পক্ষ ISN_WHILE:
		{
		    পূর্ণাঙ্ক		error = FALSE;
		    পূর্ণাঙ্ক		jump = TRUE;

		    tv = স্তূপ_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    যদি (error)
			গচ্ছ on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_স্তূপ.ga_len;
		    যদি (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_end;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = স্তূপ_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_সংখ্যা = ectx->ec_funcrefs.ga_len;
		}
		ভঙ্গ;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    পক্ষ ISN_JUMP_IF_ARG_SET:
	    পক্ষ ISN_JUMP_IF_ARG_NOT_SET:
		tv = স্তূপ_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		পূর্ণাঙ্ক arg_set = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_সংখ্যা == VVAL_NONE);

		যদি (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_SET && !arg_set)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_data)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_args.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ স্তূপ_FRAME_SIZE;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_POINTER(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    যদি (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		যদি (iptr->isn_type == ISN_JUMP_IF_ARG_SET ? arg_set : !arg_set)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		ভঙ্গ;

	    // top of a for loop
	    পক্ষ ISN_FOR:
		যদি (execute_for(iptr, ectx) == FAIL)
		    গচ্ছ theend;
		ভঙ্গ;

	    // end of a for or while loop
	    পক্ষ ISN_ENDLOOP:
		যদি (execute_endloop(iptr, ectx) == FAIL)
		    গচ্ছ theend;
		ভঙ্গ;

	    // start of ":try" block
	    পক্ষ ISN_TRY:
		{
		    trycmd_T    *trycmd = রিক্ত;

		    যদি (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			গচ্ছ theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_data)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_POINTER(trycmd);
		    trycmd->tcd_frame_idx = ectx->ec_frame_idx;
		    trycmd->tcd_স্তূপ_len = ectx->ec_স্তূপ.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		ভঙ্গ;

	    পক্ষ ISN_PUSHEXC:
		যদি (current_exception == রিক্ত)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    গচ্ছ theend;
		}
		যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
		    গচ্ছ theend;
		tv = স্তূপ_TV_BOT(0);
		++ectx->ec_স্তূপ.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->মান);
		ভঙ্গ;

	    পক্ষ ISN_CATCH:
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
		ভঙ্গ;

	    পক্ষ ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    পূর্ণাঙ্ক		i;
		    trycmd_T    *trycmd;
		    পূর্ণাঙ্ক		iidx = trycont->tct_where;

		    যদি (trystack->ga_len < trycont->tct_levels)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_levels, trystack->ga_len);
			গচ্ছ theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    প্রতি (i = trycont->tct_levels; i > 0; --i)
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
		ভঙ্গ;

	    পক্ষ ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    যদি (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_স্তূপ);
			trycmd->tcd_caught = FALSE;
		    }
		    ভঙ্গ;
		}

	    // end of ":try" block
	    পক্ষ ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_data) + trystack->ga_len;
		    যদি (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    যদি (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_স্তূপ);
		    }

		    যদি (trycmd->tcd_return)
			গচ্ছ func_return;

		    যাবত্ (ectx->ec_স্তূপ.ga_len > trycmd->tcd_স্তূপ_len)
		    {
			--ectx->ec_স্তূপ.ga_len;
			clear_tv(স্তূপ_TV_BOT(0));
		    }
		    যদি (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		ভঙ্গ;

	    পক্ষ ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    যদি (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = স্তূপ_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_সংখ্যা;
			tv->vval.v_সংখ্যা = 0;
			গচ্ছ done;
		    }
		    --ectx->ec_স্তূপ.ga_len;
		    tv = স্তূপ_TV_BOT(0);
		    যদি (tv->vval.v_string == রিক্ত
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_empty_string));
			গচ্ছ theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    যদি (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
			যদি (trycmd->tcd_caught)
			{
			    // discard the exception
			    finish_exception(caught_স্তূপ);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (throw_exception(tv->vval.v_string, ET_USER, রিক্ত)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			গচ্ছ theend;
		    }
		    did_throw = TRUE;
		}
		ভঙ্গ;

	    // compare with special values
	    পক্ষ ISN_COMPAREBOOL:
	    পক্ষ ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = স্তূপ_TV_BOT(-2);
		    typval_T	*tv2 = স্তূপ_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_সংখ্যা;
		    varnumber_T arg2 = tv2->vval.v_সংখ্যা;
		    পূর্ণাঙ্ক		res;

		    যদি (iptr->isn_arg.op.op_type == রাশিমালা_EQUAL)
			res = arg1 == arg2;
		    অন্যথা
			res = arg1 != arg2;

		    --ectx->ec_স্তূপ.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_সংখ্যা = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ভঙ্গ;

	    পক্ষ ISN_COMPARENULL:
		{
		    typval_T	*tv1 = স্তূপ_TV_BOT(-2);
		    typval_T	*tv2 = স্তূপ_TV_BOT(-1);
		    পূর্ণাঙ্ক		res;

		    res = typval_compare_null(tv1, tv2);
		    যদি (res == MAYBE)
			গচ্ছ on_error;
		    যদি (iptr->isn_arg.op.op_type == রাশিমালা_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_স্তূপ.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_সংখ্যা = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ভঙ্গ;

	    // Operation with two number arguments
	    পক্ষ ISN_OPNR:
	    পক্ষ ISN_COMPARENR:
		{
		    typval_T	*tv1 = স্তূপ_TV_BOT(-2);
		    typval_T	*tv2 = স্তূপ_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_সংখ্যা;
		    varnumber_T arg2 = tv2->vval.v_সংখ্যা;
		    varnumber_T res = 0;
		    পূর্ণাঙ্ক		div_zero = FALSE;

		    যদি (iptr->isn_arg.op.op_type == রাশিমালা_LSHIFT
			    || iptr->isn_arg.op.op_type == রাশিমালা_RSHIFT)
		    {
			যদি (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_positive));
			    গচ্ছ on_error;
			}
		    }

		    বিকল্প (iptr->isn_arg.op.op_type)
		    {
			পক্ষ রাশিমালা_MULT: res = arg1 * arg2; ভঙ্গ;
			পক্ষ রাশিমালা_DIV:  যদি (arg2 == 0)
					    div_zero = TRUE;
					অন্যথা
					    res = arg1 / arg2;
					ভঙ্গ;
			পক্ষ রাশিমালা_REM:  যদি (arg2 == 0)
					    div_zero = TRUE;
					অন্যথা
					    res = arg1 % arg2;
					ভঙ্গ;
			পক্ষ রাশিমালা_SUB: res = arg1 - arg2; ভঙ্গ;
			পক্ষ রাশিমালা_ADD: res = arg1 + arg2; ভঙ্গ;

			পক্ষ রাশিমালা_EQUAL: res = arg1 == arg2; ভঙ্গ;
			পক্ষ রাশিমালা_NEQUAL: res = arg1 != arg2; ভঙ্গ;
			পক্ষ রাশিমালা_GREATER: res = arg1 > arg2; ভঙ্গ;
			পক্ষ রাশিমালা_GEQUAL: res = arg1 >= arg2; ভঙ্গ;
			পক্ষ রাশিমালা_SMALLER: res = arg1 < arg2; ভঙ্গ;
			পক্ষ রাশিমালা_SEQUAL: res = arg1 <= arg2; ভঙ্গ;
			পক্ষ রাশিমালা_LSHIFT: যদি (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  অন্যথা
					      res = (uvarnumber_T)arg1 << arg2;
					  ভঙ্গ;
			পক্ষ রাশিমালা_RSHIFT: যদি (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  অন্যথা
					      res = (uvarnumber_T)arg1 >> arg2;
					  ভঙ্গ;
			স্বভাব: ভঙ্গ;
		    }

		    --ectx->ec_স্তূপ.ga_len;
		    যদি (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_সংখ্যা = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    অন্যথা
			tv1->vval.v_সংখ্যা = res;
		    যদি (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			গচ্ছ on_error;
		    }
		}
		ভঙ্গ;

	    // Computation with two float arguments
	    পক্ষ ISN_OPFLOAT:
	    পক্ষ ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = স্তূপ_TV_BOT(-2);
		    typval_T	*tv2 = স্তূপ_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    পূর্ণাঙ্ক		cmp = FALSE;

		    বিকল্প (iptr->isn_arg.op.op_type)
		    {
			পক্ষ রাশিমালা_MULT: res = arg1 * arg2; ভঙ্গ;
			পক্ষ রাশিমালা_DIV: res = arg1 / arg2; ভঙ্গ;
			পক্ষ রাশিমালা_SUB: res = arg1 - arg2; ভঙ্গ;
			পক্ষ রাশিমালা_ADD: res = arg1 + arg2; ভঙ্গ;

			পক্ষ রাশিমালা_EQUAL: cmp = arg1 == arg2; ভঙ্গ;
			পক্ষ রাশিমালা_NEQUAL: cmp = arg1 != arg2; ভঙ্গ;
			পক্ষ রাশিমালা_GREATER: cmp = arg1 > arg2; ভঙ্গ;
			পক্ষ রাশিমালা_GEQUAL: cmp = arg1 >= arg2; ভঙ্গ;
			পক্ষ রাশিমালা_SMALLER: cmp = arg1 < arg2; ভঙ্গ;
			পক্ষ রাশিমালা_SEQUAL: cmp = arg1 <= arg2; ভঙ্গ;
			স্বভাব: cmp = 0; ভঙ্গ;
		    }
		    --ectx->ec_স্তূপ.ga_len;
		    যদি (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_সংখ্যা = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    অন্যথা
			tv1->vval.v_float = res;
		}
		ভঙ্গ;

	    পক্ষ ISN_COMPARELIST:
	    পক্ষ ISN_COMPARETUPLE:
	    পক্ষ ISN_COMPAREDICT:
	    পক্ষ ISN_COMPAREFUNC:
	    পক্ষ ISN_COMPARESTRING:
	    পক্ষ ISN_COMPAREBLOB:
	    পক্ষ ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = স্তূপ_TV_BOT(-2);
		    typval_T	*tv2 = স্তূপ_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    পূর্ণাঙ্ক		ic = iptr->isn_arg.op.op_ic;
		    পূর্ণাঙ্ক		res = FALSE;
		    পূর্ণাঙ্ক		status = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (iptr->isn_type == ISN_COMPARELIST)
		    {
			status = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    অন্যথা যদি (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			status = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    অন্যথা যদি (iptr->isn_type == ISN_COMPAREDICT)
		    {
			status = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    অন্যথা যদি (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			status = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    অন্যথা যদি (iptr->isn_type == ISN_COMPARESTRING)
		    {
			status = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    অন্যথা যদি (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			status = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    অন্যথা // ISN_COMPAREOBJECT
		    {
			status = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_স্তূপ.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_সংখ্যা = res ? VVAL_TRUE : VVAL_FALSE;
		    যদি (status == FAIL)
			গচ্ছ theend;
		}
		ভঙ্গ;

	    পক্ষ ISN_COMPAREANY:
		{
		    typval_T	*tv1 = স্তূপ_TV_BOT(-2);
		    typval_T	*tv2 = স্তূপ_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    পূর্ণাঙ্ক		ic = iptr->isn_arg.op.op_ic;
		    পূর্ণাঙ্ক		status;

		    SOURCING_LNUM = iptr->isn_lnum;
		    status = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_স্তূপ.ga_len;
		    যদি (status == FAIL)
			গচ্ছ theend;
		}
		ভঙ্গ;

	    পক্ষ ISN_ADDLIST:
	    পক্ষ ISN_ADDTUPLE:
	    পক্ষ ISN_ADDBLOB:
		{
		    typval_T *tv1 = স্তূপ_TV_BOT(-2);
		    typval_T *tv2 = স্তূপ_TV_BOT(-1);

		    // add two lists or blobs
		    যদি (iptr->isn_type == ISN_ADDLIST)
		    {
			যদি (iptr->isn_arg.op.op_type == রাশিমালা_APPEND
						   && tv1->vval.v_list != রিক্ত)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 রিক্ত);
			অন্যথা
			    eval_addlist(tv1, tv2);
		    }
		    অন্যথা যদি (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    অন্যথা
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    পক্ষ ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = স্তূপ_TV_BOT(-2);
		    typval_T	*tv2 = স্তূপ_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (l == রিক্ত)
		    {
			emsg(_(e_cannot_add_to_null_list));
			গচ্ছ on_error;
		    }
		    যদি (মান_check_lock(l->lv_lock, রিক্ত, FALSE))
			গচ্ছ on_error;
		    যদি (list_append_tv(l, tv2) == FAIL)
			গচ্ছ theend;
		    clear_tv(tv2);
		    --ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    পক্ষ ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = স্তূপ_TV_BOT(-2);
		    typval_T	*tv2 = স্তূপ_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    পূর্ণাঙ্ক		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    যদি (b == রিক্ত)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_add_to_null_blob));
			গচ্ছ on_error;
		    }
		    n = tv_get_সংখ্যা_chk(tv2, &error);
		    যদি (error)
			গচ্ছ on_error;
		    ga_append(&b->bv_ga, (পূর্ণাঙ্ক)n);
		    --ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    // Computation with two arguments of unknown type
	    পক্ষ ISN_OPANY:
		{
		    typval_T	*tv1 = স্তূপ_TV_BOT(-2);
		    typval_T	*tv2 = স্তূপ_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    পূর্ণাঙ্ক		error = FALSE;

		    যদি (iptr->isn_arg.op.op_type == রাশিমালা_ADD)
		    {
			যদি (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_স্তূপ.ga_len;
			    ভঙ্গ;
			}
			অন্যথা যদি (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_স্তূপ.ga_len;
			    ভঙ্গ;
			}
			অন্যথা যদি (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_স্তূপ.ga_len;
			    ভঙ্গ;
			}
		    }
		    যদি (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    অন্যথা
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_সংখ্যা_chk(tv1, &error);
			যদি (error)
			    গচ্ছ on_error;
			যদি (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    যদি (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    অন্যথা
		    {
			n2 = tv_get_সংখ্যা_chk(tv2, &error);
			যদি (error)
			    গচ্ছ on_error;
			যদি (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    যদি (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			বিকল্প (iptr->isn_arg.op.op_type)
			{
			    পক্ষ রাশিমালা_MULT: f1 = f1 * f2; ভঙ্গ;
			    পক্ষ রাশিমালা_DIV:  f1 = f1 / f2; ভঙ্গ;
			    পক্ষ রাশিমালা_SUB:  f1 = f1 - f2; ভঙ্গ;
			    পক্ষ রাশিমালা_ADD:  f1 = f1 + f2; ভঙ্গ;
			    স্বভাব: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     গচ্ছ on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_স্তূপ.ga_len;
		    }
		    অন্যথা
		    {
			পূর্ণাঙ্ক failed = FALSE;

			বিকল্প (iptr->isn_arg.op.op_type)
			{
			    পক্ষ রাশিমালা_MULT: n1 = n1 * n2; ভঙ্গ;
			    পক্ষ রাশিমালা_DIV:  n1 = অঙ্ক_divide(n1, n2, &failed);
					    যদি (failed)
						গচ্ছ on_error;
					    ভঙ্গ;
			    পক্ষ রাশিমালা_SUB:  n1 = n1 - n2; ভঙ্গ;
			    পক্ষ রাশিমালা_ADD:  n1 = n1 + n2; ভঙ্গ;
			    স্বভাব:	    n1 = অঙ্ক_modulus(n1, n2, &failed);
					    যদি (failed)
						গচ্ছ on_error;
					    ভঙ্গ;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_সংখ্যা;
			tv1->vval.v_সংখ্যা = n1;
			--ectx->ec_স্তূপ.ga_len;
		    }
		}
		ভঙ্গ;

	    পক্ষ ISN_STRINDEX:
	    পক্ষ ISN_STRSLICE:
		{
		    পূর্ণাঙ্ক		is_slice = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    যদি (is_slice)
		    {
			tv = স্তূপ_TV_BOT(-2);
			n1 = tv->vval.v_সংখ্যা;
		    }

		    tv = স্তূপ_TV_BOT(-1);
		    n2 = tv->vval.v_সংখ্যা;

		    ectx->ec_স্তূপ.ga_len -= is_slice ? 2 : 1;
		    tv = স্তূপ_TV_BOT(-1);
		    যদি (is_slice)
			// Slice: Select the characters from the string
			res = string_slice(tv->vval.v_string, n1, n2, FALSE);
		    অন্যথা
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		ভঙ্গ;

	    পক্ষ ISN_LISTINDEX:
	    পক্ষ ISN_LISTSLICE:
	    পক্ষ ISN_TUPLEINDEX:
	    পক্ষ ISN_TUPLESLICE:
	    পক্ষ ISN_BLOBINDEX:
	    পক্ষ ISN_BLOBSLICE:
		{
		    পূর্ণাঙ্ক		is_slice = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    পূর্ণাঙ্ক		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    পূর্ণাঙ্ক		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_slice ? স্তূপ_TV_BOT(-3) : স্তূপ_TV_BOT(-2);

		    tv = স্তূপ_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_সংখ্যা;
		    clear_tv(tv);

		    যদি (is_slice)
		    {
			tv = স্তূপ_TV_BOT(-2);
			n1 = tv->vval.v_সংখ্যা;
			clear_tv(tv);
		    }

		    ectx->ec_স্তূপ.ga_len -= is_slice ? 2 : 1;
		    tv = স্তূপ_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (is_blob)
		    {
			যদি (blob_slice_or_index(val_tv->vval.v_blob, is_slice,
						    n1, n2, FALSE, tv) == FAIL)
			    গচ্ছ on_error;
		    }
		    অন্যথা যদি (is_tuple)
		    {
			যদি (tuple_slice_or_index(val_tv->vval.v_tuple,
				is_slice, n1, n2, FALSE, tv, TRUE) == FAIL)
			    গচ্ছ on_error;
		    }
		    অন্যথা
		    {
			যদি (list_slice_or_index(val_tv->vval.v_list, is_slice,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    গচ্ছ on_error;
		    }
		}
		ভঙ্গ;

	    পক্ষ ISN_ANYINDEX:
	    পক্ষ ISN_ANYSLICE:
		{
		    পূর্ণাঙ্ক		is_slice = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    পূর্ণাঙ্ক		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_slice ? স্তূপ_TV_BOT(-3) : স্তূপ_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (check_can_index(tv, TRUE, TRUE) == FAIL)
			গচ্ছ on_error;
		    var1 = is_slice ? স্তূপ_TV_BOT(-2) : স্তূপ_TV_BOT(-1);
		    var2 = is_slice ? স্তূপ_TV_BOT(-1) : রিক্ত;
		    res = eval_index_inner(tv, is_slice, var1, var2,
							FALSE, রিক্ত, -1, TRUE);
		    clear_tv(var1);
		    যদি (is_slice)
			clear_tv(var2);
		    ectx->ec_স্তূপ.ga_len -= is_slice ? 2 : 1;
		    যদি (res == FAIL)
			গচ্ছ on_error;
		}
		ভঙ্গ;

	    পক্ষ ISN_SLICE:
		{
		    পূর্ণাঙ্ক		গুনতি = iptr->isn_arg.সংখ্যা;

		    // type will have been checked to be a list
		    tv = স্তূপ_TV_BOT(-1);
		    যদি (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			যদি (list != রিক্ত && list->lv_len >= গুনতি)
			{
			    list_T	*newlist = list_slice(list,
				    গুনতি, list->lv_len - 1);

			    যদি (newlist != রিক্ত)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    অন্যথা
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			যদি (tuple != রিক্ত && TUPLE_LEN(tuple) >= গুনতি)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_slice(tuple, গুনতি,
							TUPLE_LEN(tuple) - 1);
			    যদি (newtuple != রিক্ত)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		ভঙ্গ;

	    পক্ষ ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*item_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = স্তূপ_TV_BOT(-1 - gi->gi_with_op);
		    যদি (tv->v_type == VAR_LIST)
		    {
			li = list_find(tv->vval.v_list, gi->gi_index);
			যদি (li == রিক্ত)
			    গচ্ছ on_error;
			item_tv = &li->li_tv;
		    }
		    অন্যথা
			item_tv = TUPLE_ITEM(tv->vval.v_tuple, gi->gi_index);

		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    ++ectx->ec_স্তূপ.ga_len;
		    copy_tv(item_tv, স্তূপ_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_index = gi->gi_index + 1;
		    ectx->ec_where.wt_kind = WT_VARIABLE;
		}
		ভঙ্গ;

	    // dict member with string key (dict['member'])
	    পক্ষ ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*চাবি;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = স্তূপ_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = স্তূপ_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    চাবি = tv->vval.v_string;
		    যদি (চাবি == রিক্ত)
			চাবি = (char_u *)"";

		    যদি ((di = dict_find(dict, চাবি, -1)) == রিক্ত)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_চাবি_not_present_in_dictionary_str), চাবি);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_স্তূপ.ga_len;
			tv = স্তূপ_TV_BOT(-1);
			(শূন্য) dict_স্তূপ_save(tv);
			tv->v_type = VAR_সংখ্যা;
			tv->vval.v_সংখ্যা = 0;
			গচ্ছ on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_স্তূপ.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = স্তূপ_TV_BOT(-1);
		    যদি (dict_স্তূপ_save(tv) == FAIL)
			গচ্ছ on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		ভঙ্গ;

	    // dict member with string key (dict.member)
	    // or can be an object
	    পক্ষ ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = স্তূপ_TV_BOT(-1);

		    যদি (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			যদি (dict_স্তূপ_save(tv) == FAIL)
			    গচ্ছ on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			যদি (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    গচ্ছ on_error;
		    }
		    অন্যথা
		    {
			যদি (tv->v_type != VAR_DICT || tv->vval.v_dict == রিক্ত)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    গচ্ছ on_error;
			}
			dict = tv->vval.v_dict;

			যদি ((di = dict_find(dict, iptr->isn_arg.string, -1))
								   == রিক্ত)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_চাবি_not_present_in_dictionary_str),
						     iptr->isn_arg.string);
			    গচ্ছ on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			যদি (dict_স্তূপ_save(tv) == FAIL)
			    গচ্ছ on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		ভঙ্গ;

	    পক্ষ ISN_GET_OBJ_MEMBER:
	    পক্ষ ISN_GET_ITF_MEMBER:
		{
		    tv = স্তূপ_TV_BOT(-1);
		    যদি (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			গচ্ছ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    যদি (obj == রিক্ত)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			গচ্ছ on_error;
		    }

		    পূর্ণাঙ্ক idx;
		    যদি (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    অন্যথা
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_index_from_itf_index(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    যদি (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			গচ্ছ on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		ভঙ্গ;

	    পক্ষ ISN_STORE_THIS:
		{
		    পূর্ণাঙ্ক idx = iptr->isn_arg.সংখ্যা;
		    object_T *obj = স্তূপ_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *স্তূপ_TV_BOT(-1);
		    --ectx->ec_স্তূপ.ga_len;
		}
		ভঙ্গ;

	    পক্ষ ISN_CLEARDICT:
		dict_স্তূপ_drop();
		ভঙ্গ;

	    পক্ষ ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_স্তূপ_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = স্তূপ_TV_BOT(-1);
		    যদি (dict_tv != রিক্ত
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != রিক্ত
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == রিক্ত))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_স্তূপ_drop();
		}
		ভঙ্গ;

	    পক্ষ ISN_NEGATENR:
		tv = স্তূপ_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		যদি (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		অন্যথা
		    tv->vval.v_সংখ্যা = -tv->vval.v_সংখ্যা;
		ভঙ্গ;

	    পক্ষ ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    পূর্ণাঙ্ক		r;
		    where_T	where = WHERE_INIT;

		    tv = স্তূপ_TV_BOT((পূর্ণাঙ্ক)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    যদি (ct->ct_arg_idx > 0)
		    {
			where.wt_index = ct->ct_arg_idx;
			where.wt_kind = ct->ct_is_var ? WT_VARIABLE : WT_ARGUMENT;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    যদি (r == FAIL)
			গচ্ছ on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    যদি (tv->v_type == VAR_সংখ্যা
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_সংখ্যা == 0
						|| tv->vval.v_সংখ্যা == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_সংখ্যা = tv->vval.v_সংখ্যা
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		ভঙ্গ;

	    পক্ষ ISN_CHECKLEN:
		{
		    পূর্ণাঙ্ক	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = রিক্ত;
		    tuple_T *tuple = রিক্ত;
		    পূর্ণাঙ্ক	    len = 0;

		    tv = স্তূপ_TV_BOT(-1);

		    পূর্ণাঙ্ক		len_check_failed = FALSE;
		    যদি (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			যদি (list == রিক্ত || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			যদি (list != রিক্ত)
			    len = list->lv_len;
		    }
		    অন্যথা যদি (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			যদি (tuple == রিক্ত || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			যদি (tuple != রিক্ত)
			    len = TUPLE_LEN(tuple);
		    }
		    অন্যথা
			len_check_failed = TRUE;

		    যদি (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_বস্তুসমূহ_but_got_nr),
				     min_len, len);
			গচ্ছ on_error;
		    }
		}
		ভঙ্গ;

	    পক্ষ ISN_SETTYPE:
		set_tv_type(স্তূপ_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		ভঙ্গ;

	    পক্ষ ISN_2BOOL:
	    পক্ষ ISN_COND2BOOL:
		{
		    পূর্ণাঙ্ক n;
		    পূর্ণাঙ্ক error = FALSE;

		    যদি (iptr->isn_type == ISN_2BOOL)
		    {
			tv = স্তূপ_TV_BOT(iptr->isn_arg.tobool.offset);
			n = tv2bool(tv);
			যদি (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    অন্যথা
		    {
			tv = স্তূপ_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			যদি (error)
			    গচ্ছ on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_সংখ্যা = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ভঙ্গ;

	    পক্ষ ISN_2STRING:
	    পক্ষ ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		যদি (do_2string(স্তূপ_TV_BOT(iptr->isn_arg.tostring.offset),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    গচ্ছ on_error;
		ভঙ্গ;

	    পক্ষ ISN_RANGE:
		{
		    exarg_T	ea;
		    বর্ণ	*errormsg;

		    ea.line2 = 0;
		    ea.addr_গুনতি = 0;
		    ea.addr_type = ADDR_সারিগুলি;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    যদি (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
			গচ্ছ on_error;

		    যদি (GA_GROW_FAILS(&ectx->ec_স্তূপ, 1))
			গচ্ছ theend;
		    ++ectx->ec_স্তূপ.ga_len;
		    tv = স্তূপ_TV_BOT(-1);
		    tv->v_type = VAR_সংখ্যা;
		    tv->v_lock = 0;
		    tv->vval.v_সংখ্যা = ea.line2;
		}
		ভঙ্গ;

	    পক্ষ ISN_PUT:
		tv = রিক্ত; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		ভঙ্গ;
	    পক্ষ ISN_IPUT:
		tv = রিক্ত;
		isn_put_do(ectx, iptr, tv, TRUE);
		ভঙ্গ;

	    পক্ষ ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_স্তূপ.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		ভঙ্গ;

	    পক্ষ ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = রিক্ত;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		ভঙ্গ;

	    পক্ষ ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = স্তূপ_TV_BOT(-1);
		যদি (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_argument_must_be_sequence_of_lists_or_tuples));
		    গচ্ছ on_error;
		}

		পূর্ণাঙ্ক rc;
		যদি (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		অন্যথা
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		যদি (rc != EXEC_OK)
		{
		    যদি (rc == EXEC_FAIL)
			গচ্ছ on_error;
		    গচ্ছ theend;
		}
		ভঙ্গ;

	    পক্ষ ISN_PROF_START:
	    পক্ষ ISN_PROF_END:
		{
#ifdef FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    যদি (iptr->isn_type == ISN_PROF_START)
		    {
			func_সারি_start(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_সারি_exec(&cookie);
		    }
		    অন্যথা
			func_সারি_end(&cookie);
#endif
		}
		ভঙ্গ;

	    পক্ষ ISN_DEBUG:
		handle_debug(iptr, ectx);
		ভঙ্গ;

	    পক্ষ ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    পূর্ণাঙ্ক		item = iptr->isn_arg.shuffle.shfl_item;
		    পূর্ণাঙ্ক		up = iptr->isn_arg.shuffle.shfl_up;

		    tmp_tv = *স্তূপ_TV_BOT(-item);
		    প্রতি ( ; up > 0 && item > 1; --up)
		    {
			*স্তূপ_TV_BOT(-item) = *স্তূপ_TV_BOT(-item + 1);
			--item;
		    }
		    *স্তূপ_TV_BOT(-item) = tmp_tv;
		}
		ভঙ্গ;

	    পক্ষ ISN_DROP:
		--ectx->ec_স্তূপ.ga_len;
		clear_tv(স্তূপ_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		ভঙ্গ;

	    পক্ষ ISN_SCRIPTCTX_SET:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		ভঙ্গ;
	}
	অনুবর্ত;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	যদি (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
	    গচ্ছ done;

	যদি (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    গচ্ছ theend;
	অনুবর্ত;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	যদি (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    যদি (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		যাবত্ (ectx->ec_স্তূপ.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_স্তূপ.ga_len;
		    clear_tv(স্তূপ_TV_BOT(0));
		}
		যাবত্ (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    অনুবর্ত;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	যদি (trylevel <= ectx->ec_trylevel_at_start)
	    গচ্ছ theend;
    }

done:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_স্তূপ_clear(dict_স্তূপ_len_at_start);
    ectx->ec_trylevel_at_start = save_trylevel_at_start;
    প্রত্যাগম ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    পূর্ণাঙ্ক
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    পূর্ণাঙ্ক		save_iidx = ectx->ec_iidx;
    পূর্ণাঙ্ক		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    যদি (res == OK)
    {
	*rettv = *স্তূপ_TV_BOT(-1);
	--ectx->ec_স্তূপ.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    প্রত্যাগম res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(শূন্য)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    পূর্ণাঙ্ক		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    যদি (exec_instructions(ectx) == OK)
    {
	typval_T *tv = স্তূপ_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_স্তূপ.ga_len;
	clear_tv(tv);
    }
    অন্যথা
    {
	substitute_instr->subs_status = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    প্রত্যাগম res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    পূর্ণাঙ্ক
call_def_function(
    ufunc_T	*ufunc,
    পূর্ণাঙ্ক		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    পূর্ণাঙ্ক		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    পূর্ণাঙ্ক		argc = argc_arg;
    পূর্ণাঙ্ক		partial_argc = partial == রিক্ত
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    পূর্ণাঙ্ক		total_argc = argc + partial_argc;
    typval_T	*tv;
    পূর্ণাঙ্ক		idx;
    পূর্ণাঙ্ক		ret = FAIL;
    পূর্ণাঙ্ক		defcount = ufunc->uf_args.ga_len - total_argc;
    sctx_T	save_current_sctx = current_sctx;
    পূর্ণাঙ্ক		did_emsg_before = did_emsg_cumul + did_emsg;
    পূর্ণাঙ্ক		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_বার্তা_list = রিক্ত;
    msglist_T	*private_বার্তা_list = রিক্ত;
    পূর্ণাঙ্ক		save_emsg_silent_def = emsg_silent_def;
    পূর্ণাঙ্ক		save_did_emsg_def = did_emsg_def;
    পূর্ণাঙ্ক		orig_funcdepth;
    পূর্ণাঙ্ক		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#undef স্তূপ_TV
#define স্তূপ_TV(idx) (((typval_T *)ectx.ec_স্তূপ.ga_data) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#undef স্তূপ_TV_BOT
#define স্তূপ_TV_BOT(idx) (((typval_T *)ectx.ec_স্তূপ.ga_data) + ectx.ec_স্তূপ.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#undef স্তূপ_TV_VAR
#define স্তূপ_TV_VAR(idx) (((typval_T *)ectx.ec_স্তূপ.ga_data) + ectx.ec_frame_idx + স্তূপ_FRAME_SIZE + idx)

    যদি (ufunc->uf_def_status == UF_NOT_COMPILED
	    || ufunc->uf_def_status == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), রিক্ত) == FAIL))
    {
	যদি (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	প্রত্যাগম FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	যদি (dfunc->df_ufunc == রিক্ত)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    প্রত্যাগম FAIL;
	}
	যদি (INSTRUCTIONS(dfunc) == রিক্ত)
	{
	    iemsg("using call_def_function() on not compiled function");
	    প্রত্যাগম FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    যদি (funcdepth_increment() == FAIL)
	প্রত্যাগম FAIL;

    CLEAR_FIELD(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_স্তূপ, পরিমাণ(typval_T), 500);
    যদি (GA_GROW_FAILS(&ectx.ec_স্তূপ, 20))
    {
	funcdepth_decrement();
	প্রত্যাগম FAIL;
    }
    ga_init2(&ectx.ec_trystack, পরিমাণ(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, পরিমাণ(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = total_argc - ufunc->uf_args.ga_len;
    যদি (idx > 0 && ufunc->uf_va_name == রিক্ত)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
								    idx), idx);
	গচ্ছ failed_early;
    }
    idx = total_argc - ufunc->uf_args.ga_len + ufunc->uf_def_args.ga_len;
    যদি (idx < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	গচ্ছ failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    প্রতি (idx = 0; idx < total_argc
	    && (ufunc->uf_va_name != রিক্ত || idx < ufunc->uf_args.ga_len);
									 ++idx)
    {
	পূর্ণাঙ্ক argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	যদি (idx >= ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_সংখ্যা == VVAL_NONE)
	{
	    // Use the default value.
	    স্তূপ_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক done = FALSE;
	    যদি (ufunc->uf_arg_types != রিক্ত && idx < ufunc->uf_args.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		যদি (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_সংখ্যা)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    স্তূপ_TV_BOT(0)->v_type = VAR_FLOAT;
		    স্তূপ_TV_BOT(0)->v_lock = 0;
		    স্তূপ_TV_BOT(0)->vval.v_float = tv->vval.v_সংখ্যা;
		    done = TRUE;
		}
		অন্যথা যদি (check_typval_arg_type(expected, tv,
						   রিক্ত, argv_idx + 1) == FAIL)
		    গচ্ছ failed_early;
	    }
	    যদি (!done)
		copy_tv(tv, স্তূপ_TV_BOT(0));
	}
	++ectx.ec_স্তূপ.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    যদি (ufunc->uf_va_name != রিক্ত)
    {
	পূর্ণাঙ্ক vararg_গুনতি = argc - ufunc->uf_args.ga_len;

	যদি (vararg_গুনতি < 0)
	    vararg_গুনতি = 0;
	অন্যথা
	    argc -= vararg_গুনতি;
	যদি (exe_newlist(vararg_গুনতি, &ectx) == FAIL)
	    গচ্ছ failed_early;

	// Check the type of the list items.
	tv = স্তূপ_TV_BOT(-1);
	যদি (ufunc->uf_va_type != রিক্ত
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != রিক্ত)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_first;

	    প্রতি (idx = 0; idx < vararg_গুনতি; ++idx)
	    {
		যদি (check_typval_arg_type(expected, &li->li_tv,
						 রিক্ত, argc + idx + 1) == FAIL)
		    গচ্ছ failed_early;
		li = li->li_next;
	    }
	}

	যদি (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *স্তূপ_TV_BOT(defcount - 1) = *স্তূপ_TV_BOT(-1);
	--ectx.ec_স্তূপ.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    যদি (defcount > 0)
	প্রতি (idx = 0; idx < defcount; ++idx)
	{
	    স্তূপ_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_স্তূপ.ga_len;
	}
    যদি (ufunc->uf_va_name != রিক্ত)
	++ectx.ec_স্তূপ.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_frame_idx = ectx.ec_স্তূপ.ga_len;
    ectx.ec_initial_frame_idx = ectx.ec_frame_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *base_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	যদি (partial != রিক্ত || base_ufunc->uf_partial != রিক্ত)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    যদি (ectx.ec_outer_ref == রিক্ত)
		গচ্ছ failed_early;
	    যদি (partial != রিক্ত)
	    {
		outer_T *outer = get_pt_outer(partial);

		যদি (outer->out_স্তূপ == রিক্ত && outer->out_loop_size == 0)
		{
		    // no stack was set
		    যদি (current_ectx != রিক্ত)
		    {
			যদি (current_ectx->ec_outer_ref != রিক্ত
			       && current_ectx->ec_outer_ref->or_outer != রিক্ত)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		অন্যথা
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    অন্যথা
	    {
		ectx.ec_outer_ref->or_outer = &base_ufunc->uf_partial->pt_outer;
		++base_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = base_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    প্রতি (idx = 0; idx < স্তূপ_FRAME_SIZE; ++idx)
    {
	স্তূপ_TV(ectx.ec_স্তূপ.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_স্তূপ.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	প্রতি (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    স্তূপ_TV_VAR(idx)->v_type = VAR_সংখ্যা;
	    স্তূপ_TV_VAR(idx)->vval.v_সংখ্যা = 0;
	}
	ectx.ec_স্তূপ.ga_len += dfunc->df_varcount;

	যদি (object != রিক্ত)
	{
	    // the object is always the variable at index zero
	    tv = স্তূপ_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	যদি (dfunc->df_has_closure)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    স্তূপ_TV_VAR(idx)->v_type = VAR_সংখ্যা;
	    স্তূপ_TV_VAR(idx)->vval.v_সংখ্যা = 0;
	    ++ectx.ec_স্তূপ.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    যদি (funccal != রিক্ত)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_ঠেলো_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_বার্তা_list = বার্তা_list;
    বার্তা_list = &private_বার্তা_list;

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
    যদি (ret == OK)
    {
	// function finished, get result from the stack.
	যদি (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	অন্যথা
	{
	    tv = স্তূপ_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    যদি (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_closure_in_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_বের_করো();
    current_sctx = save_current_sctx;

    যদি (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    যদি (*বার্তা_list != রিক্ত && saved_বার্তা_list != রিক্ত)
    {
	msglist_T **plist = saved_বার্তা_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	যাবত্ (*plist != রিক্ত)
	    plist = &(*plist)->next;

	*plist = *বার্তা_list;
    }
    বার্তা_list = saved_বার্তা_list;

    যদি (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = রিক্ত;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    প্রতি (idx = 0; idx < ectx.ec_স্তূপ.ga_len; ++idx)
    {
	tv = স্তূপ_TV(idx);
	যদি (tv->v_type != VAR_সংখ্যা && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_স্তূপ.ga_data);
    vim_free(ectx.ec_trystack.ga_data);
    যদি (ectx.ec_outer_ref != রিক্ত)
    {
	যদি (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    যদি (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    প্রত্যাগম ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    শূন্য
unwind_def_callstack(ectx_T *ectx)
{
    যাবত্ (ectx->ec_frame_idx != ectx->ec_initial_frame_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    শূন্য
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    যদি (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    স্থির char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    পূর্ণাঙ্ক		depth;

    ga_init2(&ga, 1, 100);
    প্রতি (depth = 0; depth < lvi->lvi_depth; ++depth)
    {
	যদি (ga_grow(&ga, 50) == FAIL)
	    ভঙ্গ;
	যদি (lvi->lvi_loop[depth].var_idx == 0)
	    STRCPY((বর্ণ *)ga.ga_data + ga.ga_len, " -");
	অন্যথা
	    vim_snprintf((বর্ণ *)ga.ga_data + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[depth].var_idx,
			    lvi->lvi_loop[depth].var_idx
					 + lvi->lvi_loop[depth].var_গুনতি - 1);
	ga.ga_len = (পূর্ণাঙ্ক)STRLEN(ga.ga_data);
    }
    প্রত্যাগম ga.ga_data;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    স্থির শূন্য
list_instructions(বর্ণ *pfx, isn_T *instr, পূর্ণাঙ্ক instr_গুনতি, ufunc_T *ufunc)
{
    পূর্ণাঙ্ক		সারি_idx = 0;
    পূর্ণাঙ্ক		prev_current = 0;
    পূর্ণাঙ্ক		current;
    পূর্ণাঙ্ক		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    প্রতি (current = 0; current < instr_গুনতি; ++current)
    {
	isn_T	    *iptr = &instr[current];
	বর্ণ	    *সারি;

	যদি (ufunc != রিক্ত)
	{
	    যাবত্ (সারি_idx < iptr->isn_lnum
					  && সারি_idx < ufunc->uf_সারিগুলি.ga_len)
	    {
		যদি (current > prev_current)
		{
		    বার্তা_puts("\n\n");
		    prev_current = current;
		}
		সারি = ((বর্ণ **)ufunc->uf_সারিগুলি.ga_data)[সারি_idx++];
		যদি (সারি != রিক্ত)
		    বার্তা(সারি);
	    }
	    যদি (iptr->isn_type == ISN_JUMP_IF_ARG_SET)
	    {
		পূর্ণাঙ্ক	first_def_arg = ufunc->uf_args.ga_len
						   - ufunc->uf_def_args.ga_len;

		যদি (def_arg_idx > 0)
		    বার্তা_puts("\n\n");
		বার্তা_start();
		বার্তা_puts("  ");
		বার্তা_puts(((বর্ণ **)(ufunc->uf_args.ga_data))[
						 first_def_arg + def_arg_idx]);
		বার্তা_puts(" = ");
		বার্তা_puts(((বর্ণ **)(ufunc->uf_def_args.ga_data))[def_arg_idx++]);
		বার্তা_clr_eos();
		বার্তা_end();
	    }
	}

	বিকল্প (iptr->isn_type)
	{
	    পক্ষ ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (পূর্ণাঙ্ক)iptr->isn_arg.construct.construct_size);
		ভঙ্গ;
	    পক্ষ ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_EXEC_SPLIT:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.সংখ্যা ? " append" : "");
		ভঙ্গ;
	    পক্ষ ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.সংখ্যা));
#endif
		ভঙ্গ;
	    পক্ষ ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#endif
		ভঙ্গ;
	    পক্ষ ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, INT_MAX, রিক্ত);
		বার্তা("     -------------");
		ভঙ্গ;
	    পক্ষ ISN_SOURCE:
		{
		    scriptitem_T    *si = SCRIPT_ITEM(iptr->isn_arg.সংখ্যা);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		ভঙ্গ;
	    পক্ষ ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, INT_MAX, রিক্ত);
		    বার্তা("     -------------");
		}
		ভঙ্গ;
	    পক্ষ ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.সংখ্যা);
		ভঙ্গ;
	    পক্ষ ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_গুনতি);
		}
		ভঙ্গ;
	    পক্ষ ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_ECHOWINDOW:
		যদি (iptr->isn_arg.echowin.ewin_time > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_গুনতি,
				      iptr->isn_arg.echowin.ewin_time);
		অন্যথা
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_গুনতি);
		ভঙ্গ;
	    পক্ষ ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_LOAD:
		{
		    যদি (iptr->isn_arg.সংখ্যা < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.সংখ্যা
							  + স্তূপ_FRAME_SIZE));
		    অন্যথা
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.সংখ্যা));
		}
		ভঙ্গ;
	    পক্ষ ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    যদি (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_depth,
					outer->outer_idx + স্তূপ_FRAME_SIZE);
		    অন্যথা যদি (outer->outer_depth < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_depth);
		    অন্যথা
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_depth,
					      outer->outer_idx);
		}
		ভঙ্গ;
	    পক্ষ ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    যদি (sv == রিক্ত)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    অন্যথা
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		ভঙ্গ;
	    পক্ষ ISN_LOADS:
	    পক্ষ ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ভঙ্গ;
	    পক্ষ ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (পূর্ণাঙ্ক)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;

	    পক্ষ ISN_STORE:
		যদি (iptr->isn_arg.সংখ্যা < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.সংখ্যা + স্তূপ_FRAME_SIZE);
		অন্যথা
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.সংখ্যা);
		ভঙ্গ;
	    পক্ষ ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    যদি (outer->outer_depth < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_depth);
		    অন্যথা
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_depth, outer->outer_idx);
		}
		ভঙ্গ;
	    পক্ষ ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_STORES:
	    পক্ষ ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ভঙ্গ;
	    পক্ষ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    যদি (sv == রিক্ত)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    অন্যথা
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		ভঙ্গ;
	    পক্ষ ISN_STOREOPT:
	    পক্ষ ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		ভঙ্গ;
	    পক্ষ ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (পূর্ণাঙ্ক)iptr->isn_arg.সংখ্যা);
		ভঙ্গ;
	    পক্ষ ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		ভঙ্গ;

	    পক্ষ ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		ভঙ্গ;

	    পক্ষ ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		ভঙ্গ;

	    পক্ষ ISN_LOAD_CLASSMEMBER:
	    পক্ষ ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    পূর্ণাঙ্ক	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_LOAD_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		ভঙ্গ;

	    // constants
	    পক্ষ ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_PUSHBOOL:
	    পক্ষ ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		ভঙ্গ;
	    পক্ষ ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		ভঙ্গ;
	    পক্ষ ISN_PUSHFUNC:
		{
		    বর্ণ *name = (বর্ণ *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == রিক্ত ? "[none]" : name);
		}
		ভঙ্গ;
	    পক্ষ ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#endif
		ভঙ্গ;
	    পক্ষ ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#endif
		ভঙ্গ;
	    পক্ষ ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == রিক্ত ? "null"
				 : (বর্ণ *)iptr->isn_arg.classarg->class_name.string);
		ভঙ্গ;
	    পক্ষ ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ভঙ্গ;
	    পক্ষ ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ভঙ্গ;
	    পক্ষ ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		ভঙ্গ;
	    পক্ষ ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		ভঙ্গ;

	    // function call
	    পক্ষ ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    internal_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		ভঙ্গ;
	    পক্ষ ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		ভঙ্গ;
	    পক্ষ ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != রিক্ত
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		ভঙ্গ;
	    পক্ষ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		ভঙ্গ;
	    পক্ষ ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_শীর্ষ ? " top" : "", cpfunc->cpf_argcount);
		}
		ভঙ্গ;
	    পক্ষ ISN_PCALL_END:
		smsg("%s%4d PCALL end", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (পূর্ণাঙ্ক)iptr->isn_arg.defer.defer_argcount);
		ভঙ্গ;
	    পক্ষ ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    যদি (extra == রিক্ত || extra->fre_func_name == রিক্ত)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    অন্যথা
			name = extra->fre_func_name;
		    যদি (extra != রিক্ত && extra->fre_class != রিক্ত)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    অন্যথা যদি (extra == রিক্ত
				     || extra->fre_loopvar_info.lvi_depth == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    অন্যথা
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		ভঙ্গ;

	    পক্ষ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    যদি (arg->nfa_loopvar_info.lvi_depth == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    অন্যথা
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		ভঙ্গ;

	    পক্ষ ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == রিক্ত ? (char_u *)"" : name);
		}
		ভঙ্গ;

	    পক্ষ ISN_JUMP:
		{
		    বর্ণ *when = "?";

		    বিকল্প (iptr->isn_arg.jump.jump_when)
		    {
			পক্ষ JUMP_ALWAYS:
			    when = "JUMP";
			    ভঙ্গ;
			পক্ষ JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    ভঙ্গ;
			পক্ষ JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    ভঙ্গ;
			পক্ষ JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    ভঙ্গ;
			পক্ষ JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  // unused
			    ভঙ্গ;
			পক্ষ JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    ভঙ্গ;
			পক্ষ JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    ভঙ্গ;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		ভঙ্গ;

	    পক্ষ ISN_JUMP_IF_ARG_SET:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + স্তূপ_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		ভঙ্গ;

	    পক্ষ ISN_JUMP_IF_ARG_NOT_SET:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + স্তূপ_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		ভঙ্গ;

	    পক্ষ ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_end);
		}
		ভঙ্গ;

	    পক্ষ ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->end_funcref_idx,
			    endloop->end_var_idx,
			    endloop->end_var_idx + endloop->end_var_গুনতি - 1,
			    endloop->end_depth);
		}
		ভঙ্গ;

	    পক্ষ ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_end);
		}
		ভঙ্গ;

	    পক্ষ ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    যদি (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    অন্যথা
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		ভঙ্গ;
	    পক্ষ ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_levels,
				      trycont->tct_levels == 1 ? "" : "s",
				      trycont->tct_where);
		}
		ভঙ্গ;
	    পক্ষ ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		ভঙ্গ;
	    পক্ষ ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		ভঙ্গ;

	    // expression operations on number
	    পক্ষ ISN_OPNR:
	    পক্ষ ISN_OPFLOAT:
	    পক্ষ ISN_OPANY:
		{
		    বর্ণ *what;
		    বর্ণ *ins;

		    বিকল্প (iptr->isn_arg.op.op_type)
		    {
			পক্ষ রাশিমালা_MULT: what = "*"; ভঙ্গ;
			পক্ষ রাশিমালা_DIV: what = "/"; ভঙ্গ;
			পক্ষ রাশিমালা_REM: what = "%"; ভঙ্গ;
			পক্ষ রাশিমালা_SUB: what = "-"; ভঙ্গ;
			পক্ষ রাশিমালা_ADD: what = "+"; ভঙ্গ;
			পক্ষ রাশিমালা_LSHIFT: what = "<<"; ভঙ্গ;
			পক্ষ রাশিমালা_RSHIFT: what = ">>"; ভঙ্গ;
			স্বভাব:       what = "???"; ভঙ্গ;
		    }
		    বিকল্প (iptr->isn_type)
		    {
			পক্ষ ISN_OPNR: ins = "OPNR"; ভঙ্গ;
			পক্ষ ISN_OPFLOAT: ins = "OPFLOAT"; ভঙ্গ;
			পক্ষ ISN_OPANY: ins = "OPANY"; ভঙ্গ;
			স্বভাব: ins = "???"; ভঙ্গ;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		ভঙ্গ;

	    পক্ষ ISN_COMPAREBOOL:
	    পক্ষ ISN_COMPARESPECIAL:
	    পক্ষ ISN_COMPARENULL:
	    পক্ষ ISN_COMPARENR:
	    পক্ষ ISN_COMPAREFLOAT:
	    পক্ষ ISN_COMPARESTRING:
	    পক্ষ ISN_COMPAREBLOB:
	    পক্ষ ISN_COMPARELIST:
	    পক্ষ ISN_COMPARETUPLE:
	    পক্ষ ISN_COMPAREDICT:
	    পক্ষ ISN_COMPAREFUNC:
	    পক্ষ ISN_COMPAREOBJECT:
	    পক্ষ ISN_COMPAREANY:
		   {
		       বর্ণ *p;
		       বর্ণ buf[10];
		       বর্ণ *type;

		       বিকল্প (iptr->isn_arg.op.op_type)
		       {
			   পক্ষ রাশিমালা_EQUAL:	 p = "=="; ভঙ্গ;
			   পক্ষ রাশিমালা_NEQUAL:    p = "!="; ভঙ্গ;
			   পক্ষ রাশিমালা_GREATER:   p = ">"; ভঙ্গ;
			   পক্ষ রাশিমালা_GEQUAL:    p = ">="; ভঙ্গ;
			   পক্ষ রাশিমালা_SMALLER:   p = "<"; ভঙ্গ;
			   পক্ষ রাশিমালা_SEQUAL:    p = "<="; ভঙ্গ;
			   পক্ষ রাশিমালা_MATCH:	 p = "=~"; ভঙ্গ;
			   পক্ষ রাশিমালা_IS:	 p = "is"; ভঙ্গ;
			   পক্ষ রাশিমালা_ISNOT:	 p = "isnot"; ভঙ্গ;
			   পক্ষ রাশিমালা_NOMATCH:	 p = "!~"; ভঙ্গ;
			   স্বভাব:  p = "???"; ভঙ্গ;
		       }
		       STRCPY(buf, p);
		       যদি (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       বিকল্প(iptr->isn_type)
		       {
			   পক্ষ ISN_COMPAREBOOL: type = "COMPAREBOOL"; ভঙ্গ;
			   পক্ষ ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; ভঙ্গ;
			   পক্ষ ISN_COMPARENULL: type = "COMPARENULL"; ভঙ্গ;
			   পক্ষ ISN_COMPARENR: type = "COMPARENR"; ভঙ্গ;
			   পক্ষ ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; ভঙ্গ;
			   পক্ষ ISN_COMPARESTRING:
						  type = "COMPARESTRING"; ভঙ্গ;
			   পক্ষ ISN_COMPAREBLOB: type = "COMPAREBLOB"; ভঙ্গ;
			   পক্ষ ISN_COMPARELIST: type = "COMPARELIST"; ভঙ্গ;
			   পক্ষ ISN_COMPARETUPLE: type = "COMPARETUPLE"; ভঙ্গ;
			   পক্ষ ISN_COMPAREDICT: type = "COMPAREDICT"; ভঙ্গ;
			   পক্ষ ISN_COMPAREFUNC: type = "COMPAREFUNC"; ভঙ্গ;
			   পক্ষ ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; ভঙ্গ;
			   পক্ষ ISN_COMPAREANY: type = "COMPAREANY"; ভঙ্গ;
			   স্বভাব: type = "???"; ভঙ্গ;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   ভঙ্গ;

	    পক্ষ ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); ভঙ্গ;
	    পক্ষ ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); ভঙ্গ;
	    পক্ষ ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); ভঙ্গ;

	    // expression operations
	    পক্ষ ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.সংখ্যা));
		ভঙ্গ;
	    পক্ষ ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); ভঙ্গ;
	    পক্ষ ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); ভঙ্গ;
	    পক্ষ ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); ভঙ্গ;
	    পক্ষ ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); ভঙ্গ;
	    পক্ষ ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); ভঙ্গ;
	    পক্ষ ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); ভঙ্গ;
	    পক্ষ ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); ভঙ্গ;
	    পক্ষ ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); ভঙ্গ;
	    পক্ষ ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); ভঙ্গ;
	    পক্ষ ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); ভঙ্গ;
	    পক্ষ ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); ভঙ্গ;
	    পক্ষ ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); ভঙ্গ;
	    পক্ষ ISN_SLICE: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.সংখ্যা); ভঙ্গ;
	    পক্ষ ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_index,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); ভঙ্গ;
	    পক্ষ ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); ভঙ্গ;
	    পক্ষ ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); ভঙ্গ;
	    পক্ষ ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (পূর্ণাঙ্ক)iptr->isn_arg.classmember.cm_idx);
				     ভঙ্গ;
	    পক্ষ ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (পূর্ণাঙ্ক)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     ভঙ্গ;
	    পক্ষ ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (পূর্ণাঙ্ক)iptr->isn_arg.সংখ্যা); ভঙ্গ;
	    পক্ষ ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); ভঙ্গ;
	    পক্ষ ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); ভঙ্গ;

	    পক্ষ ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); ভঙ্গ;

	    পক্ষ ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      বর্ণ	    *tofree = রিক্ত;
		      বর্ণ	    *typename;

		      যদি (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_সংখ্যা_OK))
			  typename = "float|number";
		      অন্যথা যদি (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      অন্যথা
			  typename = type_name(ct->ct_type, &tofree);

		      যদি (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (পূর্ণাঙ্ক)ct->ct_off);
		      অন্যথা
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (পূর্ণাঙ্ক)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (পূর্ণাঙ্ক)ct->ct_arg_idx);
		      vim_free(tofree);
		      ভঙ্গ;
		  }
	    পক্ষ ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_more_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       ভঙ্গ;
	    পক্ষ ISN_SETTYPE:
		  {
		      বর্ণ *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      ভঙ্গ;
		  }
	    পক্ষ ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); ভঙ্গ;
	    পক্ষ ISN_2BOOL: যদি (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    অন্যথা
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    ভঙ্গ;
	    পক্ষ ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      ভঙ্গ;
	    পক্ষ ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      ভঙ্গ;
	    পক্ষ ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    ভঙ্গ;
	    পক্ষ ISN_PUT:
		যদি (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		অন্যথা যদি (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		অন্যথা
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		ভঙ্গ;
	    পক্ষ ISN_IPUT:
		যদি (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		অন্যথা যদি (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		অন্যথা
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		ভঙ্গ;
	    পক্ষ ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  রিক্ত, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    যদি (likely(buf != রিক্ত))
		    {
			(শূন্য)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    ভঙ্গ;
		}
	    পক্ষ ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); ভঙ্গ;

	    পক্ষ ISN_PROF_START:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 ভঙ্গ;

	    পক্ষ ISN_PROF_END:
		smsg("%s%4d PROFILE END", pfx, current);
		ভঙ্গ;

	    পক্ষ ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_names_len);
		ভঙ্গ;

	    পক্ষ ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_গুনতি,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      ভঙ্গ;
	    পক্ষ ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_item,
					 iptr->isn_arg.shuffle.shfl_up);
			      ভঙ্গ;
	    পক্ষ ISN_DROP: smsg("%s%4d DROP", pfx, current); ভঙ্গ;

	    পক্ষ ISN_SCRIPTCTX_SET:
		{
		    পূর্ণাঙ্ক	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ITEM(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		ভঙ্গ;

	    পক্ষ ISN_FINISH: // End of list of instructions for ISN_SUBSTITUTE.
			   প্রত্যাগম;
	}

	out_flush();	    // output one line at a time
	ui_breakcheck();
	যদি (got_int)
	    ভঙ্গ;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    শূন্য
set_context_in_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_pattern = arg;

    // first argument already typed: only user function names
    যদি (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
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
get_disassemble_argument(expand_T *xp, পূর্ণাঙ্ক idx)
{
    যদি (idx == 0)
	প্রত্যাগম (char_u *)"debug";
    যদি (idx == 1)
	প্রত্যাগম (char_u *)"profile";
    প্রত্যাগম get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    শূন্য
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = রিক্ত;  // init to shut up gcc warning
    পূর্ণাঙ্ক		instr_গুনতি = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = find_func_by_name(arg, &compile_type);
    যদি (ufunc == রিক্ত)
	প্রত্যাগম;
    যদি (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, রিক্ত) == FAIL)
	প্রত্যাগম;
    যদি (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	প্রত্যাগম;
    }
    বার্তা((বর্ণ *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    বিকল্প (compile_type)
    {
	পক্ষ CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_গুনতি = dfunc->df_instr_prof_গুনতি;
	    ভঙ্গ;
#endif
	    // FALLTHROUGH
	পক্ষ CT_NONE:
	    instr = dfunc->df_instr;
	    instr_গুনতি = dfunc->df_instr_গুনতি;
	    ভঙ্গ;
	পক্ষ CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_গুনতি = dfunc->df_instr_debug_গুনতি;
	    ভঙ্গ;
    }

    list_instructions("", instr, instr_গুনতি, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    পূর্ণাঙ্ক
tv2bool(typval_T *tv)
{
    বিকল্প (tv->v_type)
    {
	পক্ষ VAR_সংখ্যা:
	    প্রত্যাগম tv->vval.v_সংখ্যা != 0;
	পক্ষ VAR_FLOAT:
	    প্রত্যাগম tv->vval.v_float != 0.0;
	পক্ষ VAR_PARTIAL:
	    প্রত্যাগম tv->vval.v_partial != রিক্ত;
	পক্ষ VAR_FUNC:
	পক্ষ VAR_STRING:
	    প্রত্যাগম tv->vval.v_string != রিক্ত && *tv->vval.v_string != NUL;
	পক্ষ VAR_LIST:
	    প্রত্যাগম tv->vval.v_list != রিক্ত && tv->vval.v_list->lv_len > 0;
	পক্ষ VAR_TUPLE:
	    প্রত্যাগম tuple_len(tv->vval.v_tuple) > 0;
	পক্ষ VAR_DICT:
	    প্রত্যাগম tv->vval.v_dict != রিক্ত
				    && tv->vval.v_dict->dv_hashtab.ht_used > 0;
	পক্ষ VAR_BOOL:
	পক্ষ VAR_SPECIAL:
	    প্রত্যাগম tv->vval.v_সংখ্যা == VVAL_TRUE ? TRUE : FALSE;
	পক্ষ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    প্রত্যাগম tv->vval.v_job != রিক্ত;
#অন্যথা
	    ভঙ্গ;
#endif
	পক্ষ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    প্রত্যাগম tv->vval.v_channel != রিক্ত;
#অন্যথা
	    ভঙ্গ;
#endif
	পক্ষ VAR_BLOB:
	    প্রত্যাগম tv->vval.v_blob != রিক্ত && tv->vval.v_blob->bv_ga.ga_len > 0;

	পক্ষ VAR_OBJECT:
	    প্রত্যাগম tv->vval.v_object != রিক্ত;

	পক্ষ VAR_CLASS:
	পক্ষ VAR_TYPEALIAS:
	    check_typval_is_মান(tv);
	    ভঙ্গ;

	পক্ষ VAR_UNKNOWN:
	পক্ষ VAR_ANY:
	পক্ষ VAR_VOID:
	পক্ষ VAR_INSTR:
	    ভঙ্গ;
    }
    প্রত্যাগম FALSE;
}

    শূন্য
emsg_using_string_as(typval_T *tv, পূর্ণাঙ্ক as_সংখ্যা)
{
    semsg(_(as_সংখ্যা ? e_using_string_as_সংখ্যা_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == রিক্ত
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    পূর্ণাঙ্ক
check_not_string(typval_T *tv)
{
    যদি (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

#endif // FEAT_EVAL
