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
#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
வகைவரை கட்டமைப்பு {
    முழு	    tcd_frame_idx;	// ec_frame_idx at ISN_TRY
    முழு	    tcd_அடுக்கு_len;	// size of ectx.ec_stack at ISN_TRY
    முழு	    tcd_in_catch;	// in catch or finally block
    முழு	    tcd_did_throw;	// set did_throw in :endtry
    முழு	    tcd_catch_idx;	// instruction of the first :catch or :finally
    முழு	    tcd_finally_idx;	// instruction of the :finally block or zero
    முழு	    tcd_endtry_idx;	// instruction of the :endtry
    முழு	    tcd_caught;		// catch block entered
    முழு	    tcd_cont;		// :continue encountered, jump here (minus one)
    முழு	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
வகைவரை கட்டமைப்பு {
    முழு		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    முழு		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
வகைவரை கட்டமைப்பு {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    முழு		or_outer_allocated;  // free "or_outer" later
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
கட்டமைப்பு ectx_S {
    garray_T	ec_அடுக்கு;	// stack of typval_T values
    முழு		ec_frame_idx;	// index in ec_stack: context of ec_dfunc_idx
    முழு		ec_initial_frame_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    முழு		ec_dfunc_idx;	// current function index
    முழு		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    முழு		ec_did_emsg_before;
    முழு		ec_trylevel_at_start;
    where_T	ec_where;
};

#ifdef FEAT_PROFILE
// stack of profinfo_T used when profiling.
நிலையான garray_T profile_info_ga = {0, 0, அளவு(profinfo_T), 20, வெற்று};
#endif

// Get pointer to item in the stack.
#define அடுக்கு_TV(idx) (((typval_T *)ectx->ec_அடுக்கு.ga_data) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#define அடுக்கு_TV_BOT(idx) (((typval_T *)ectx->ec_அடுக்கு.ga_data) + ectx->ec_அடுக்கு.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#define அடுக்கு_TV_VAR(idx) (((typval_T *)ectx->ec_அடுக்கு.ga_data) + ectx->ec_frame_idx + அடுக்கு_FRAME_SIZE + idx)

// Return value for functions used to execute instructions
#define EXEC_FAIL	0
#define EXEC_OK		1
#define EXEC_DONE	2

    வெறுமை
to_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_to_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    நிலையான முழு
ufunc_argcount(ufunc_T *ufunc)
{
    திரும்பு ufunc->uf_args.ga_len + (ufunc->uf_va_name != வெற்று ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    நிலையான முழு
exe_concat(முழு மொத்தம், ectx_T *ectx)
{
    முழு		idx;
    size_t	len = 0;
    garray_T	ga;
    வகைவரை கட்டமைப்பு
    {
	typval_T    *tv;
	size_t	    length;
    } string_segment_T;
    எண்ணிக்கை
    {
	STRING_SEGMENT_CACHE_SIZE = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_SIZE];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    எனில் (மொத்தம் > (முழு)ARRAY_LENGTH(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, மொத்தம்);
	எனில் (string_segment_tab == வெற்று)
	    திரும்பு FAIL;
    }

    ga_init2(&ga, அளவு(எழுத்து), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    க்கு (idx = 0; idx < மொத்தம்; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = அடுக்கு_TV_BOT(idx - மொத்தம்);
	எனில் (segment->tv->vval.v_string != வெற்று)
	{
	    segment->length = STRLEN(segment->tv->vval.v_string);
	    len += segment->length;
	}
	இல்லையெனில்
	    segment->length = 0;    // Ensure clean state for the second pass
    }

    எனில் (ga_grow(&ga, (முழு)len + 1) == FAIL)
    {
	எனில் (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	திரும்பு FAIL;
    }

    க்கு (idx = 0; idx < மொத்தம்; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->length);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_அடுக்கு.ga_len -= மொத்தம் - 1;
    அடுக்கு_TV_BOT(-1)->vval.v_string = ga.ga_data;

    எனில் (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    திரும்பு OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    நிலையான முழு
exe_newlist(முழு மொத்தம், ectx_T *ectx)
{
    list_T	*list = வெற்று;
    முழு		idx;
    typval_T	*tv;

    எனில் (மொத்தம் >= 0)
    {
	list = list_alloc_with_பொருட்கள்(மொத்தம்);
	எனில் (list == வெற்று)
	    திரும்பு FAIL;
	க்கு (idx = 0; idx < மொத்தம்; ++idx)
	    list_set_item(list, idx, அடுக்கு_TV_BOT(idx - மொத்தம்));
    }

    எனில் (மொத்தம் > 0)
	ectx->ec_அடுக்கு.ga_len -= மொத்தம் - 1;
    இல்லையெனில் எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
    {
	list_unref(list);
	திரும்பு FAIL;
    }
    இல்லையெனில்
	++ectx->ec_அடுக்கு.ga_len;
    tv = அடுக்கு_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_lock = 0;
    எனில் (list != வெற்று)
	++list->lv_refcount;
    திரும்பு OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    நிலையான முழு
exe_newtuple(முழு மொத்தம், ectx_T *ectx)
{
    tuple_T	*tuple = வெற்று;
    முழு		idx;
    typval_T	*tv;

    எனில் (மொத்தம் >= 0)
    {
	tuple = tuple_alloc_with_பொருட்கள்(மொத்தம்);
	எனில் (tuple == வெற்று)
	    திரும்பு FAIL;
	க்கு (idx = 0; idx < மொத்தம்; ++idx)
	    tuple_set_item(tuple, idx, அடுக்கு_TV_BOT(idx - மொத்தம்));
    }

    எனில் (மொத்தம் > 0)
	ectx->ec_அடுக்கு.ga_len -= மொத்தம் - 1;
    இல்லையெனில் எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
    {
	tuple_unref(tuple);
	திரும்பு FAIL;
    }
    இல்லையெனில்
	++ectx->ec_அடுக்கு.ga_len;
    tv = அடுக்கு_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_lock = 0;
    எனில் (tuple != வெற்று)
	++tuple->tv_refcount;

    திரும்பு OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    நிலையான முழு
exe_newdict(முழு மொத்தம், ectx_T *ectx)
{
    dict_T	*dict = வெற்று;
    dictitem_T	*item;
    char_u	*சாவி;
    முழு		idx;
    typval_T	*tv;

    எனில் (மொத்தம் >= 0)
    {
	dict = dict_alloc();
	எனில் (unlikely(dict == வெற்று))
	    திரும்பு FAIL;
	க்கு (idx = 0; idx < மொத்தம்; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = அடுக்கு_TV_BOT(2 * (idx - மொத்தம்));
	    // check key is unique
	    சாவி = tv->vval.v_string == வெற்று ? (char_u *)"" : tv->vval.v_string;
	    item = dict_find(dict, சாவி, -1);
	    எனில் (item != வெற்று)
	    {
		semsg(_(e_duplicate_சாவி_in_dictionary_str), சாவி);
		dict_unref(dict);
		திரும்பு MAYBE;
	    }
	    item = dictitem_alloc(சாவி);
	    clear_tv(tv);
	    எனில் (unlikely(item == வெற்று))
	    {
		dict_unref(dict);
		திரும்பு FAIL;
	    }
	    tv = அடுக்கு_TV_BOT(2 * (idx - மொத்தம்) + 1);
	    item->di_tv = *tv;
	    item->di_tv.v_lock = 0;
	    tv->v_type = VAR_UNKNOWN;
	    எனில் (dict_add(dict, item) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(item);
		dict_unref(dict);
		திரும்பு FAIL;
	    }
	}
    }

    எனில் (மொத்தம் > 0)
	ectx->ec_அடுக்கு.ga_len -= 2 * மொத்தம் - 1;
    இல்லையெனில் எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
    {
	dict_unref(dict);
	திரும்பு FAIL;
    }
    இல்லையெனில்
	++ectx->ec_அடுக்கு.ga_len;
    tv = அடுக்கு_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_lock = 0;
    tv->vval.v_dict = dict;
    எனில் (dict != வெற்று)
	++dict->dv_refcount;
    திரும்பு OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    வெறுமை
update_has_breakpoint(ufunc_T *ufunc)
{
    எனில் (ufunc->uf_debug_tick == debug_tick)
	திரும்பு;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

நிலையான garray_T dict_அடுக்கு = GA_EMPTY;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    நிலையான முழு
dict_அடுக்கு_save(typval_T *tv)
{
    எனில் (dict_அடுக்கு.ga_growsize == 0)
	ga_init2(&dict_அடுக்கு, அளவு(typval_T), 10);
    எனில் (ga_grow(&dict_அடுக்கு, 1) == FAIL)
	திரும்பு FAIL;
    ((typval_T *)dict_அடுக்கு.ga_data)[dict_அடுக்கு.ga_len] = *tv;
    ++dict_அடுக்கு.ga_len;
    திரும்பு OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    நிலையான typval_T *
dict_அடுக்கு_get_tv(வெறுமை)
{
    எனில் (dict_அடுக்கு.ga_len == 0)
	திரும்பு வெற்று;
    திரும்பு ((typval_T *)dict_அடுக்கு.ga_data) + dict_அடுக்கு.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    நிலையான dict_T *
dict_அடுக்கு_get_dict(வெறுமை)
{
    typval_T *tv;

    எனில் (dict_அடுக்கு.ga_len == 0)
	திரும்பு வெற்று;
    tv = ((typval_T *)dict_அடுக்கு.ga_data) + dict_அடுக்கு.ga_len - 1;
    எனில் (tv->v_type == VAR_DICT)
	திரும்பு tv->vval.v_dict;
    திரும்பு வெற்று;
}

/*
 * Drop an item from the dict stack.
 */
    நிலையான வெறுமை
dict_அடுக்கு_drop(வெறுமை)
{
    எனில் (dict_அடுக்கு.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	திரும்பு;
    }
    --dict_அடுக்கு.ga_len;
    clear_tv(((typval_T *)dict_அடுக்கு.ga_data) + dict_அடுக்கு.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    நிலையான வெறுமை
dict_அடுக்கு_clear(முழு len)
{
    போது (dict_அடுக்கு.ga_len > len)
	dict_அடுக்கு_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    நிலையான outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    எனில் (ptref == வெற்று)
	திரும்பு &pt->pt_outer;

    // partial using partial (recursively)
    போது (ptref->pt_outer_partial != வெற்று)
	ptref = ptref->pt_outer_partial;
    திரும்பு &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    நிலையான முழு
check_ufunc_arg_types(ufunc_T *ufunc, முழு argcount, முழு off, ectx_T *ectx)
{
    எனில் (ufunc->uf_arg_types == வெற்று && ufunc->uf_va_type == வெற்று)
	திரும்பு OK;

    typval_T	*argv = அடுக்கு_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    க்கு (முழு i = 0; i < argcount; ++i)
    {
	type_T *type = வெற்று;

	// assume a v:none argument, using the default value, is always OK
	எனில் (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_எண் == VVAL_NONE)
	    தொடர்;
	// only pass values to user functions, never types
	எனில் (check_typval_is_மதிப்பு(&argv[i]) == FAIL)
	    திரும்பு FAIL;

	எனில் (i < ufunc->uf_args.ga_len && ufunc->uf_arg_types != வெற்று)
	    type = ufunc->uf_arg_types[i];
	இல்லையெனில் எனில் (ufunc->uf_va_type != வெற்று)
	    type = ufunc->uf_va_type->tt_member;
	எனில் (type != வெற்று && check_typval_arg_type(type,
		    &argv[i], வெற்று, i + 1) == FAIL)
	    திரும்பு FAIL;
    }
    திரும்பு OK;
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
    நிலையான முழு
call_dfunc(
	முழு		cdf_idx,
	partial_T	*pt_arg,
	முழு		argcount_arg,
	ectx_T		*ectx)
{
    முழு		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    முழு		did_emsg_before = did_emsg_cumul + did_emsg;
    முழு		arg_to_add;
    முழு		vararg_மொத்தம் = 0;
    முழு		varcount;
    முழு		idx;
    estack_T	*entry;
    funclocal_T	*floc = வெற்று;
    முழு		res = OK;
    compiletype_T compile_type;

    எனில் (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == வெற்று ? (char_u *)"unknown" : dfunc->df_name);
	திரும்பு FAIL;
    }

#ifdef FEAT_PROFILE
    எனில் (do_profiling == PROF_YES)
    {
	எனில் (GA_GROW_OK(&profile_info_ga, 1))
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
    எனில் (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, வெற்று);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    }
    எனில் (res == FAIL || INSTRUCTIONS(dfunc) == வெற்று)
    {
	எனில் (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	திரும்பு FAIL;
    }

    எனில் (ufunc->uf_va_name != வெற்று)
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
	vararg_மொத்தம் = argcount - ufunc->uf_args.ga_len;
	எனில் (vararg_மொத்தம் < 0)
	    vararg_மொத்தம் = 0;
	இல்லையெனில்
	    argcount -= vararg_மொத்தம்;
	எனில் (exe_newlist(vararg_மொத்தம், ectx) == FAIL)
	    திரும்பு FAIL;

	vararg_மொத்தம் = 1;
    }

    arg_to_add = ufunc->uf_args.ga_len - argcount;
    எனில் (arg_to_add < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
			-arg_to_add), -arg_to_add);
	திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (arg_to_add > ufunc->uf_def_args.ga_len)
    {
	முழு missing = arg_to_add - ufunc->uf_def_args.ga_len;

	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
			missing), missing);
	திரும்பு FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = அடுக்கு_TV_BOT(0) - argcount - vararg_மொத்தம் - 1;

    எனில் (IS_OBJECT_METHOD(ufunc) && !IS_CONSTRUCTOR_METHOD(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == வெற்று)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	திரும்பு FAIL;
    }

    // Check the argument types.
    எனில் (check_ufunc_arg_types(ufunc, argcount, vararg_மொத்தம், ectx) == FAIL)
	திரும்பு FAIL;

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
    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு,
				     arg_to_add + அடுக்கு_FRAME_SIZE + varcount))
	திரும்பு FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = அடுக்கு_TV_BOT(0) - argcount - vararg_மொத்தம் - 1;

    // If depth of calling is getting too high, don't execute the function.
    எனில் (funcdepth_increment() == FAIL)
	திரும்பு FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    எனில் (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	எனில் (floc == வெற்று)
	    திரும்பு FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    எனில் (vararg_மொத்தம் > 0 && arg_to_add > 0)
	*அடுக்கு_TV_BOT(arg_to_add - 1) = *அடுக்கு_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    க்கு (idx = 0; idx < arg_to_add; ++idx)
	அடுக்கு_TV_BOT(idx - vararg_மொத்தம்)->v_type = VAR_UNKNOWN;
    ectx->ec_அடுக்கு.ga_len += arg_to_add;

    // Store current execution state in stack frame for ISN_RETURN.
    அடுக்கு_TV_BOT(அடுக்கு_FRAME_FUNC_OFF)->vval.v_எண் = ectx->ec_dfunc_idx;
    அடுக்கு_TV_BOT(அடுக்கு_FRAME_IIDX_OFF)->vval.v_எண் = ectx->ec_iidx;
    அடுக்கு_TV_BOT(அடுக்கு_FRAME_INSTR_OFF)->vval.v_string = (வெறுமை *)ectx->ec_instr;
    அடுக்கு_TV_BOT(அடுக்கு_FRAME_OUTER_OFF)->vval.v_string =
						    (வெறுமை *)ectx->ec_outer_ref;
    அடுக்கு_TV_BOT(அடுக்கு_FRAME_FUNCLOCAL_OFF)->vval.v_string = (வெறுமை *)floc;
    அடுக்கு_TV_BOT(அடுக்கு_FRAME_IDX_OFF)->vval.v_எண் = ectx->ec_frame_idx;
    ectx->ec_frame_idx = ectx->ec_அடுக்கு.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    முழு initcount = dfunc->df_varcount + (dfunc->df_has_closure ? 1 : 0);
    க்கு (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = அடுக்கு_TV_BOT(அடுக்கு_FRAME_SIZE + idx);

	tv->v_type = VAR_எண்;
	tv->vval.v_எண் = 0;
    }
    ectx->ec_அடுக்கு.ga_len += அடுக்கு_FRAME_SIZE + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    எனில் (IS_OBJECT_METHOD(ufunc))
    {
	எனில் (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_internal_error_str), "type in stack is not an object");
	    திரும்பு FAIL;
	}

	*அடுக்கு_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != வெற்று ? pt_arg : ufunc->uf_partial;
    எனில் (pt != வெற்று || (ufunc->uf_flags & FC_CLOSURE))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	எனில் (ref == வெற்று)
	    திரும்பு FAIL;
	எனில் (pt != வெற்று)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	இல்லையெனில்
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    எனில் (unlikely(ref->or_outer == வெற்று))
	    {
		vim_free(ref);
		திரும்பு FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->out_அடுக்கு = &ectx->ec_அடுக்கு;
	    ref->or_outer->out_frame_idx = ectx->ec_frame_idx;
	    எனில் (ectx->ec_outer_ref != வெற்று)
		ref->or_outer->out_up = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    இல்லையெனில்
	ectx->ec_outer_ref = வெற்று;

    எனில் (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    entry = estack_தள்ளு_ufunc(ufunc, 1);
    எனில் (entry != வெற்று)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	entry->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    திரும்பு OK;
}

// Double linked list of funcstack_T in use.
நிலையான funcstack_T *first_funcstack = வெற்று;

    நிலையான வெறுமை
add_funcstack_to_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    எனில் (first_funcstack != வெற்று)
	first_funcstack->fs_prev = funcstack;
    funcstack->fs_next = first_funcstack;
    funcstack->fs_prev = வெற்று;
    first_funcstack = funcstack;
}

    நிலையான வெறுமை
remove_funcstack_from_list(funcstack_T *funcstack)
{
    எனில் (funcstack->fs_prev == வெற்று)
	first_funcstack = funcstack->fs_next;
    இல்லையெனில்
	funcstack->fs_prev->fs_next = funcstack->fs_next;
    எனில் (funcstack->fs_next != வெற்று)
	funcstack->fs_next->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    நிலையான முழு
handle_closure_in_use(ectx_T *ectx, முழு free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    முழு		argcount;
    முழு		உச்சி;
    முழு		idx;
    typval_T	*tv;
    முழு		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_மொத்தம்;

    எனில் (dfunc->df_ufunc == வெற்று)
	திரும்பு OK;  // function was freed
    எனில் (dfunc->df_has_closure == 0)
	திரும்பு OK;  // no closures
    tv = அடுக்கு_TV(ectx->ec_frame_idx + அடுக்கு_FRAME_SIZE + dfunc->df_varcount);
    closure_மொத்தம் = tv->vval.v_எண்;
    எனில் (closure_மொத்தம் == 0)
	திரும்பு OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    உச்சி = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    க்கு (idx = 0; idx < closure_மொத்தம்; ++idx)
    {
	partial_T   *pt;
	முழு	    off = gap->ga_len - closure_மொத்தம் + idx;

	எனில் (off < 0)
	    தொடர்;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	எனில் (pt->pt_refcount > 1)
	{
	    முழு refcount = pt->pt_refcount;
	    முழு i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    க்கு (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = அடுக்கு_TV(ectx->ec_frame_idx
						       + அடுக்கு_FRAME_SIZE + i);
		எனில் (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    எனில் (refcount > 1)
	    {
		closure_in_use = TRUE;
		நிறுத்து;
	    }
	}
    }

    எனில் (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *அடுக்கு;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	எனில் (funcstack == வெற்று)
	    திரும்பு FAIL;

	funcstack->fs_var_offset = argcount + அடுக்கு_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset
							  + dfunc->df_varcount;
	அடுக்கு = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = அடுக்கு;
	எனில் (அடுக்கு == வெற்று)
	{
	    vim_free(funcstack);
	    திரும்பு FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	க்கு (idx = 0; idx < argcount; ++idx)
	{
	    tv = அடுக்கு_TV(உச்சி + idx);
	    எனில் (free_arguments)
	    {
		*(அடுக்கு + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    இல்லையெனில் எனில் (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, அடுக்கு + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	க்கு (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = அடுக்கு_TV(ectx->ec_frame_idx + அடுக்கு_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    எனில் (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != வெற்று)
	    {
		முழு	    i;

		க்கு (i = 0; i < closure_மொத்தம்; ++i)
		    எனில் (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_மொத்தம் + i])
			++funcstack->fs_min_refcount;
	    }

	    *(அடுக்கு + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	க்கு (idx = 0; idx < closure_மொத்தம்; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_மொத்தம் + idx];
	    எனில் (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_அடுக்கு = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - உச்சி;
	    }
	}
    }

    க்கு (idx = 0; idx < closure_மொத்தம்; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_மொத்தம் + idx]);
    gap->ga_len -= closure_மொத்தம்;
    எனில் (gap->ga_len == 0)
	ga_clear(gap);

    திரும்பு OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    முழு
funcstack_check_refcount(funcstack_T *funcstack)
{
    முழு		i;
    garray_T	*gap = &funcstack->fs_ga;
    முழு		done = 0;
    typval_T	*அடுக்கு;

    எனில் (funcstack->fs_refcount > funcstack->fs_min_refcount)
	திரும்பு FALSE;
    க்கு (i = funcstack->fs_var_offset; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_data) + i;

	எனில் (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != வெற்று
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++done;
    }
    எனில் (done != funcstack->fs_min_refcount)
	திரும்பு FALSE;

    அடுக்கு = gap->ga_data;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    க்கு (i = 0; i < gap->ga_len; ++i)
	clear_tv(அடுக்கு + i);
    vim_free(அடுக்கு);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    திரும்பு TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    முழு
set_ref_in_funcstacks(முழு copyID)
{
    funcstack_T *funcstack;

    க்கு (funcstack = first_funcstack; funcstack != வெற்று;
						funcstack = funcstack->fs_next)
    {
	typval_T    *அடுக்கு = funcstack->fs_ga.ga_data;
	முழு	    i;

	க்கு (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    எனில் (set_ref_in_item(அடுக்கு + i, copyID, வெற்று, வெற்று, வெற்று))
		திரும்பு TRUE;  // abort
    }
    திரும்பு FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
நிலையான ectx_T *current_ectx = வெற்று;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    முழு
in_def_function(வெறுமை)
{
    திரும்பு current_ectx != வெற்று;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    முழு
fill_exec_lval_வேர்(lval_வேர்_T *வேர்)
{
    ectx_T *ectx = current_ectx;
    எனில் (ectx != வெற்று)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	எனில் (ufunc->uf_class != வெற்று)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    எனில் (tv != வெற்று)
	    {
		CLEAR_POINTER(வேர்);
		வேர்->lr_tv = tv;
		copy_tv(அடுக்கு_TV_VAR(0), வேர்->lr_tv);
		வேர்->lr_cl_exec = ufunc->uf_class;
		++வேர்->lr_cl_exec->class_refcount;
		திரும்பு OK;
	    }
	}
    }

    திரும்பு FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(வெறுமை)
{
    ectx_T *r = current_ectx;

    current_ectx = வெற்று;
    திரும்பு r;
}

    வெறுமை
restore_current_ectx(ectx_T *ectx)
{
    எனில் (current_ectx != வெற்று)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    நிலையான list_T *
add_defer_item(முழு var_idx, முழு argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = அடுக்கு_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    எனில் (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	எனில் (rettv_list_alloc(defer_tv) == FAIL)
	    திரும்பு வெற்று;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_பொருட்கள்(argcount + 1);
    எனில் (l == வெற்று)
	திரும்பு வெற்று;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_lock = 0;
    எனில் (list_செருகு_tv(defer_l, &listval, defer_l->lv_first) == FAIL)
    {
	list_free(l);
	திரும்பு வெற்று;
    }

    திரும்பு l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    நிலையான முழு
defer_command(முழு var_idx, முழு argcount, ectx_T *ectx)
{
    list_T	*l = add_defer_item(var_idx, argcount, ectx);
    முழு		i;
    typval_T	*func_tv;

    எனில் (l == வெற்று)
	திரும்பு FAIL;

    func_tv = அடுக்கு_TV_BOT(-argcount - 1);
    எனில் (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	திரும்பு FAIL;
    }
    list_set_item(l, 0, func_tv);

    க்கு (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, அடுக்கு_TV_BOT(-argcount + i));
    ectx->ec_அடுக்கு.ga_len -= argcount + 1;
    திரும்பு OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    முழு
add_defer_function(char_u *name, முழு argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    முழு		i;

    எனில் (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	திரும்பு FAIL;
    }

    l = add_defer_item(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    எனில் (l == வெற்று)
    {
	vim_free(name);
	திரும்பு FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_lock = 0;
    func_tv.vval.v_string = name;
    list_set_item(l, 0, &func_tv);

    க்கு (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, argvars + i);
    திரும்பு OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    நிலையான வெறுமை
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = அடுக்கு_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    எனில் (defer_tv->v_type != VAR_LIST)
	திரும்பு;	 // no function added
    FOR_ALL_LIST_பொருட்கள்(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ARGS];
	முழு	    i;
	listitem_T  *arg_li = l->lv_first;
	typval_T    *functv = &l->lv_first->li_tv;
	முழு	    argcount = l->lv_len - 1;

	எனில் (functv->vval.v_string == வெற்று)
	    // already being called, can happen if function does ":qa"
	    தொடர்;

	க்கு (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_next;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = வெற்று;
	partial_T	*pt = வெற்று;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	எனில் (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = வெற்று;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    எனில் (funcexe.fe_object != வெற்று)
		++funcexe.fe_object->obj_refcount;
	}
	இல்லையெனில்
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = வெற்று;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(வெறுமை)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	எனில் (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	இல்லையெனில்
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    நிலையான முழு
func_return(ectx_T *ectx)
{
    முழு		idx;
    முழு		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    முழு		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*entry;
    முழு		prev_dfunc_idx = அடுக்கு_TV(ectx->ec_frame_idx
					+ அடுக்கு_FRAME_FUNC_OFF)->vval.v_எண்;
    funclocal_T	*floc;
#ifdef FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_data)
							      + prev_dfunc_idx;

    எனில் (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	எனில் (dfunc->df_ufunc->uf_profiling
				   || (caller != வெற்று && caller->uf_profiling))
	{
	    profile_may_end_func(((profinfo_T *)profile_info_ga.ga_data)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#endif

    எனில் (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    எனில் (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    entry = estack_எடு();
    எனில் (entry != வெற்று)
	current_sctx = entry->es_save_sctx;

    எனில் (handle_closure_in_use(ectx, TRUE) == FAIL)
	திரும்பு FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    முழு உச்சி = ectx->ec_frame_idx - argcount;
    எனில் (IS_OBJECT_METHOD(dfunc->df_ufunc))
	--உச்சி;
    க்கு (idx = உச்சி; idx < ectx->ec_frame_idx; ++idx)
	clear_tv(அடுக்கு_TV(idx));

    // Clear local variables and temp values, but not the return value.
    க்கு (idx = ectx->ec_frame_idx + அடுக்கு_FRAME_SIZE;
					idx < ectx->ec_அடுக்கு.ga_len - 1; ++idx)
	clear_tv(அடுக்கு_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_அடுக்கு.ga_len - 1;
    எனில் (ret_idx == ectx->ec_frame_idx + அடுக்கு_FRAME_IDX_OFF)
	ret_idx = 0;

    எனில் (ectx->ec_outer_ref != வெற்று)
    {
	எனில் (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = அடுக்கு_TV(ectx->ec_frame_idx
					+ அடுக்கு_FRAME_IIDX_OFF)->vval.v_எண்;
    ectx->ec_instr = (வெறுமை *)அடுக்கு_TV(ectx->ec_frame_idx
				       + அடுக்கு_FRAME_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (வெறுமை *)அடுக்கு_TV(ectx->ec_frame_idx
				       + அடுக்கு_FRAME_OUTER_OFF)->vval.v_string;
    floc = (வெறுமை *)அடுக்கு_TV(ectx->ec_frame_idx
				   + அடுக்கு_FRAME_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_frame_idx = அடுக்கு_TV(ectx->ec_frame_idx
				       + அடுக்கு_FRAME_IDX_OFF)->vval.v_எண்;

    எனில் (floc == வெற்று)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    இல்லையெனில்
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    எனில் (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_அடுக்கு.ga_len = உச்சி + 1;
	*அடுக்கு_TV_BOT(-1) = *அடுக்கு_TV(ret_idx);
    }
    இல்லையெனில்
	// Reset the stack to the position before the call.
	ectx->ec_அடுக்கு.ga_len = உச்சி;

    funcdepth_decrement();
    --ex_nesting_level;
    திரும்பு OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    நிலையான முழு
call_prepare(முழு argcount, typval_T *argvars, ectx_T *ectx)
{
    முழு		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    க்கு (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *அடுக்கு_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    எனில் (argcount > 0)
	ectx->ec_அடுக்கு.ga_len -= argcount - 1;
    இல்லையெனில் எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
	திரும்பு FAIL;
    இல்லையெனில்
	++ectx->ec_அடுக்கு.ga_len;

    // Default return value is zero.
    tv = அடுக்கு_TV_BOT(-1);
    tv->v_type = VAR_எண்;
    tv->vval.v_எண் = 0;
    tv->v_lock = 0;

    திரும்பு OK;
}

/*
 * Call a builtin function by index.
 */
    நிலையான முழு
call_bfunc(முழு func_idx, முழு argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    முழு		idx;
    முழு		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    எழுத்து	*save_func_name = ectx->ec_where.wt_func_name;

    எனில் (call_prepare(argcount, argvars, ectx) == FAIL)
	திரும்பு FAIL;

    // Check for void value being passed as an argument.
    க்கு (idx = 0; idx < argcount; ++idx)
	எனில் (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_மதிப்பு));
	    க்கு (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    திரும்பு FAIL;
	}

    ectx->ec_where.wt_func_name = internal_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_internal_func_by_idx(func_idx, argvars, அடுக்கு_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    க்கு (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    எனில் (did_emsg > did_emsg_before)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    நிலையான முழு
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	முழு	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    funcexe_T   funcexe;
    funcerror_T	error;
    முழு		idx;
    முழு		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    எனில் (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, வெற்று)
								       == FAIL)
	திரும்பு FAIL;
    எனில் (ufunc->uf_def_status == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	எனில் (error != FCERR_UNKNOWN)
	{
	    எனில் (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    இல்லையெனில்
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    திரும்பு FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	எனில் (iptr != வெற்று)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	திரும்பு call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    எனில் (call_prepare(argcount, argvars, ectx) == FAIL)
	திரும்பு FAIL;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != வெற்று ? selfdict : dict_அடுக்கு_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      அடுக்கு_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    க்கு (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    எனில் (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_found_var);
	திரும்பு FAIL;
    }
    எனில் (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * If command modifiers were applied restore them.
 */
    நிலையான வெறுமை
may_restore_cmdmod(funclocal_T *funclocal)
{
    எனில் (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = வெற்று;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    நிலையான முழு
vim9_aborting(முழு prev_uncaught_emsg)
{
    திரும்பு uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    நிலையான முழு
call_by_name(
	char_u	    *name,
	முழு	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    எனில் (builtin_function(name, -1))
    {
	முழு func_idx = find_internal_func(name);

	எனில் (func_idx < 0)  // Impossible?
	    திரும்பு FAIL;
	எனில் (check_internal_func(func_idx, argcount) < 0)
	    திரும்பு FAIL;
	திரும்பு call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*start_bracket = generic_func_find_open_bracket(name);
    எனில் (start_bracket != வெற்று)
    {
	cc = *start_bracket;
	*start_bracket = NUL;
    }

    ufunc = find_func(name, FALSE);

    எனில் (ufunc == வெற்று)
    {
	முழு prev_uncaught_emsg = uncaught_emsg;

	எனில் (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = find_func(name, FALSE);

	எனில் (vim9_aborting(prev_uncaught_emsg))
	{
	    எனில் (start_bracket != வெற்று)
		*start_bracket = cc;
	    திரும்பு FAIL;  // bail out if loading the script caused an error
	}
    }

    எனில் (start_bracket != வெற்று)
	*start_bracket = cc;

    எனில் (ufunc != வெற்று)
    {
	எனில் (IS_GENERIC_FUNC(ufunc))
	{
	    எனில் (start_bracket != வெற்று)
		ufunc = find_generic_func(ufunc, name, &start_bracket);
	    இல்லையெனில்
	    {
		emsg_funcname(e_generic_func_missing_type_args_str, name);
		ufunc = வெற்று;
	    }
	    எனில் (ufunc == வெற்று)
		திரும்பு FAIL;
	}
	இல்லையெனில் எனில் (start_bracket != வெற்று)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    திரும்பு FAIL;
	}

	எனில் (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    திரும்பு FAIL;

	திரும்பு call_ufunc(ufunc, வெற்று, argcount, ectx, iptr, selfdict);
    }

    திரும்பு FAIL;
}

    நிலையான முழு
call_partial(
	typval_T    *tv,
	முழு	    argcount_arg,
	ectx_T	    *ectx)
{
    முழு		argcount = argcount_arg;
    char_u	*name = வெற்று;
    முழு		called_emsg_before = called_emsg;
    முழு		res = FAIL;
    dict_T	*selfdict = வெற்று;

    எனில் (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	முழு	    i;

	எனில் (pt->pt_obj != வெற்று)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		திரும்பு FAIL;
	    க்கு (i = 1; i <= argcount; ++i)
		*அடுக்கு_TV_BOT(-i + 1) = *அடுக்கு_TV_BOT(-i);

	    typval_T *obj_tv = அடுக்கு_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_lock = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_அடுக்கு.ga_len;
	}

	எனில் (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, pt->pt_argc))
		திரும்பு FAIL;
	    க்கு (i = 1; i <= argcount; ++i)
		*அடுக்கு_TV_BOT(-i + pt->pt_argc) = *அடுக்கு_TV_BOT(-i);
	    ectx->ec_அடுக்கு.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    க்கு (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], அடுக்கு_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	எனில் (pt->pt_func != வெற்று)
	    திரும்பு call_ufunc(pt->pt_func, pt, argcount, ectx, வெற்று, selfdict);

	name = pt->pt_name;
    }
    இல்லையெனில் எனில் (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    எனில் (name != வெற்று)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = வெற்று;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	எனில் (error != FCERR_NONE)
	    res = FAIL;
	இல்லையெனில்
	    res = call_by_name(fname, argcount, ectx, வெற்று, selfdict);
	vim_free(tofree);
    }

    எனில் (res == FAIL)
    {
	எனில் (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == வெற்று ? (char_u *)"[unknown]" : name);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    நிலையான முழு
error_if_locked(முழு lock, எழுத்து *error)
{
    எனில் (lock & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    நிலையான முழு
check_for_எண்(typval_T *tv)
{
    எனில் (tv->v_type != VAR_எண்)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_எண்), vartype_name(tv->v_type));
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    நிலையான வெறுமை
store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    முழு		    flags = ASSIGN_DECL;

    எனில் (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, வெற்று, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    நிலையான முழு
do_2string(typval_T *tv, முழு is_2string_any, முழு tostring_flags)
{
    எனில் (tv->v_type == VAR_STRING)
	திரும்பு OK;

    char_u *str;

    எனில் (is_2string_any)
    {
	தேர்வு (tv->v_type)
	{
	    நிலை VAR_SPECIAL:
	    நிலை VAR_BOOL:
	    நிலை VAR_எண்:
	    நிலை VAR_FLOAT:
	    நிலை VAR_DICT:
	    நிலை VAR_BLOB:	நிறுத்து;

	    நிலை VAR_LIST:
				எனில் (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, அளவு(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    எனில் (str == வெற்று)
					திரும்பு FAIL;
				    s = str;
				    போது ((e = vim_strchr(s, '\n')) != வெற்று)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					எனில் (p != வெற்று)
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
				    திரும்பு OK;
				}
				எனில் (tostring_flags & TOSTRING_INTERPOLATE)
				    நிறுத்து;
				// FALLTHROUGH
	    இயல்பு:	to_string_error(tv->v_type);
			திரும்பு FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    திரும்பு OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    நிலையான வெறுமை
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    தேர்வு (tv->v_type)
    {
	நிலை VAR_LIST:
	    எனில் (tv->vval.v_list == வெற்று && sv->sv_type != &t_list_empty)
		(வெறுமை)rettv_list_alloc(tv);
	    நிறுத்து;
	நிலை VAR_TUPLE:
	    எனில் (tv->vval.v_tuple == வெற்று && sv->sv_type != &t_tuple_empty)
		(வெறுமை)rettv_tuple_alloc(tv);
	    நிறுத்து;
	நிலை VAR_DICT:
	    எனில் (tv->vval.v_dict == வெற்று && sv->sv_type != &t_dict_empty)
		(வெறுமை)rettv_dict_alloc(tv);
	    நிறுத்து;
	நிலை VAR_BLOB:
	    எனில் (tv->vval.v_blob == வெற்று && sv->sv_type != &t_blob_null)
		(வெறுமை)rettv_blob_alloc(tv);
	    நிறுத்து;
	இயல்பு:
	    நிறுத்து;
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

    எனில் (str == வெற்று)
	திரும்பு வெற்று;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    எனில் (index < 0)
    {
	முழு	clen = 0;

	க்கு (nbyte = 0; nbyte < slen; ++clen)
	{
	    எனில் (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    இல்லையெனில் எனில் (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    இல்லையெனில்
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + index;
	எனில் (nchar < 0)
	    // unlike list: index out of range results in empty string
	    திரும்பு வெற்று;
    }

    க்கு (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	எனில் (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	இல்லையெனில் எனில் (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	இல்லையெனில்
	    nbyte += mb_ptr2len(str + nbyte);
    }
    எனில் (nbyte >= slen)
	திரும்பு வெற்று;
    திரும்பு vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    நிலையான long
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    எனில் (nchar >= 0)
    {
	போது (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    இல்லையெனில்
    {
	nbyte = str_len;
	போது (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_head_off(str, str + nbyte);
	    ++nchar;
	}
	எனில் (nchar < 0)
	    திரும்பு -1;
    }
    திரும்பு (long)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_slice(char_u *str, varnumber_T first, varnumber_T last, முழு exclusive)
{
    long	start_byte, end_byte;
    size_t	slen;

    எனில் (str == வெற்று)
	திரும்பு வெற்று;
    slen = STRLEN(str);
    start_byte = char_idx2byte(str, slen, first);
    எனில் (start_byte < 0)
	start_byte = 0; // first index very negative: use zero
    எனில் ((last == -1 && !exclusive) || last == VARNUM_MAX)
	end_byte = (long)slen;
    இல்லையெனில்
    {
	end_byte = char_idx2byte(str, slen, last);
	எனில் (!exclusive && end_byte >= 0 && end_byte < (long)slen)
	    // end index is inclusive
	    end_byte += mb_ptr2len(str + end_byte);
    }

    எனில் (start_byte >= (long)slen || end_byte <= start_byte)
	திரும்பு வெற்று;
    திரும்பு vim_strnsave(str + start_byte, end_byte - start_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    நிலையான svar_T *
get_script_svar(scriptref_T *sref, முழு dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? வெற்று
			      : ((dfunc_T *)def_functions.ga_data) + dfunc_idx;
    svar_T	    *sv;

    எனில் (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	எனில் (dfunc != வெற்று)
	    semsg(_(e_script_variable_invalid_after_reload_in_function_str),
					 printable_func_name(dfunc->df_ufunc));
	திரும்பு வெற்று;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_data) + sref->sref_idx;
    எனில் (sv->sv_name == வெற்று)
    {
	எனில் (dfunc != வெற்று)
	    emsg(_(e_script_variable_was_deleted));
	திரும்பு வெற்று;
    }
    எனில் (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	எனில் (dfunc != வெற்று)
	    emsg(_(e_script_variable_type_changed));
	திரும்பு வெற்று;
    }

    எனில் ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	எனில் (dfunc != வெற்று)
	    semsg(_(e_item_not_exported_in_script_str), sv->sv_name);
	திரும்பு வெற்று;
    }
    திரும்பு sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    நிலையான char_u *
get_split_sourceline(
	முழு c UNUSED,
	வெறுமை *cookie,
	முழு indent UNUSED,
	getline_opt_T options UNUSED)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*p;
    char_u		*வரி;

    p = vim_strchr(sp->nextline, '\n');
    எனில் (p == வெற்று)
    {
	வரி = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    இல்லையெனில்
    {
	வரி = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    திரும்பு வரி;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    நிலையான முழு
call_eval_func(
	char_u	    *name,
	முழு	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    முழு	    called_emsg_before = called_emsg;
    முழு	    res;

    res = call_by_name(name, argcount, ectx, iptr, வெற்று);
    எனில் (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = find_var(name, வெற்று, FALSE);
	எனில் (v == வெற்று || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    திரும்பு FAIL;
	}
	திரும்பு call_partial(&v->di_tv, argcount, ectx);
    }
    திரும்பு res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    முழு
fill_partial_and_closure(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    எனில் (ufunc->uf_flags & FC_CLOSURE)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.out_அடுக்கு = &ectx->ec_அடுக்கு;
	pt->pt_outer.out_frame_idx = ectx->ec_frame_idx;
	எனில் (ectx->ec_outer_ref != வெற்று)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.out_up = ectx->ec_outer_ref->or_outer;
	    எனில் (ectx->ec_outer_ref->or_partial != வெற்று)
	    {
		pt->pt_outer.out_up_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.out_up_partial->pt_refcount;
	    }
	}

	எனில் (lvi != வெற்று)
	{
	    முழு	depth;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    க்கு (depth = 0; depth < lvi->lvi_depth; ++depth)
	    {
		pt->pt_outer.out_loop[depth].அடுக்கு = &ectx->ec_அடுக்கு;
		pt->pt_outer.out_loop[depth].var_idx = ectx->ec_frame_idx
			 + அடுக்கு_FRAME_SIZE + lvi->lvi_loop[depth].var_idx;
		pt->pt_outer.out_loop[depth].var_மொத்தம் =
					    lvi->lvi_loop[depth].var_மொத்தம்;
	    }
	    pt->pt_outer.out_loop_size = lvi->lvi_depth;
	}
	இல்லையெனில்
	    pt->pt_outer.out_loop_size = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	எனில் (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    திரும்பு FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_அடுக்கு.ga_data) + ectx->ec_frame_idx
		       + அடுக்கு_FRAME_SIZE + dfunc->df_varcount)->vval.v_எண்;

	((partial_T **)ectx->ec_funcrefs.ga_data)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    திரும்பு OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    நிலையான முழு
exec_command(isn_T *iptr, char_u *cmd_string)
{
    source_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_FIELD(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    எனில் (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	திரும்பு FAIL;
    திரும்பு OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    முழு
may_load_script(முழு sid, முழு *loaded)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);

    எனில் (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	எனில் (do_source(si->sn_name, FALSE, DOSO_NONE, வெற்று) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    திரும்பு FAIL;
	}
    }
    திரும்பு OK;
}

// used for v_instr of typval of VAR_INSTR
கட்டமைப்பு instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
வகைவரை கட்டமைப்பு subs_கோவை_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    முழு		subs_status;
} subs_கோவை_T;

// Set when calling do_debug().
நிலையான ectx_T	*debug_context = வெற்று;
நிலையான முழு	debug_var_மொத்தம்;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    முழு		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    முழு		    varargs_off;

    எனில் (ectx == வெற்று)
	திரும்பு வெற்று;
    dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    க்கு (idx = debug_var_மொத்தம் - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_names.ga_data)[idx];

	// the variable name may be NULL when not available in this block
	எனில் (varname != வெற்று && STRCMP(varname, name) == 0)
	    திரும்பு அடுக்கு_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == வெற்று ? 0 : 1;
    க்கு (idx = 0; idx < ufunc->uf_args.ga_len; ++idx)
	எனில் (STRCMP(((char_u **)(ufunc->uf_args.ga_data))[idx], name) == 0)
	    திரும்பு அடுக்கு_TV(ectx->ec_frame_idx - ufunc->uf_args.ga_len
							  - varargs_off + idx);
    எனில் (ufunc->uf_va_name != வெற்று && STRCMP(ufunc->uf_va_name, name) == 0)
	திரும்பு அடுக்கு_TV(ectx->ec_frame_idx - 1);

    திரும்பு வெற்று;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    முழு
may_break_in_function(ufunc_T *ufunc)
{
    திரும்பு ufunc->uf_has_breakpoint || debug_has_கோவை_breakpoint();
}

    நிலையான வெறுமை
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*வரி;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    முழு		end_lnum = iptr->isn_lnum;
    garray_T	ga;
    முழு		lnum;

    எனில் (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	எனில் (!may_break_in_function(ufunc))
	    திரும்பு;

	// check for the next breakpoint if needed
	breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	எனில் (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    திரும்பு;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_மொத்தம் = iptr->isn_arg.debug.dbg_var_names_len;

    க்கு (ni = iptr + 1; ni->isn_type != ISN_FINISH; ++ni)
	எனில் (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    end_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    நிறுத்து;
	}

    எனில் (end_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, அளவு(char_u *), 10);
	க்கு (lnum = iptr->isn_lnum; lnum < end_lnum
				     && lnum <= ufunc->uf_வரிகள்.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_வரிகள்.ga_data)[lnum - 1];

	    எனில் (p == வெற்று)
		தொடர்;  // left over from continuation line
	    p = skipwhite(p);
	    எனில் (*p == '#')
		நிறுத்து;
	    எனில் (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_data))[ga.ga_len++] = p;
	    எனில் (STRNCMP(p, "def ", 4) == 0)
		நிறுத்து;
	}
	வரி = ga_concat_strings(&ga, "  ");
	vim_free(ga.ga_data);
    }
    இல்லையெனில்
	வரி = ((char_u **)ufunc->uf_வரிகள்.ga_data)[iptr->isn_lnum - 1];

    do_debug(வரி == வெற்று ? (char_u *)"[empty]" : வரி);
    debug_context = வெற்று;

    எனில் (end_lnum > iptr->isn_lnum)
	vim_free(வரி);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    நிலையான முழு
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    எனில் (lhs_list->lv_type == வெற்று || lhs_list->lv_type->tt_member == வெற்று)
	திரும்பு OK;

    திரும்பு check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    நிலையான முழு
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = அடுக்கு_TV_BOT(-2);
    long	lidx = 0;
    typval_T	*tv_dest = அடுக்கு_TV_BOT(-1);
    முழு		status = OK;
    முழு		check_rhs_type = FALSE;

    எனில் (tv_idx->v_type == VAR_எண்)
	lidx = (long)tv_idx->vval.v_எண்;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = அடுக்கு_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    எனில் (dest_type == VAR_OBJECT && tv_dest->vval.v_object == வெற்று)
    {
	emsg(_(e_using_null_object));
	status = FAIL;
    }
    இல்லையெனில் எனில் (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	எனில் (dest_type == VAR_DICT)
	    status = do_2string(tv_idx, TRUE, FALSE);
	இல்லையெனில் எனில் (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    எனில் (obj == வெற்று)
	    {
		emsg(_(e_using_null_object));
		status = FAIL;
	    }
	    இல்லையெனில்
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		முழு		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		எனில் (m != வெற்று)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    எனில் ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == வெற்று ||
			     !class_instance_of(ufunc->uf_class, cl)))
		    {
			எழுத்து *செய்தி = (m->ocm_access == VIM_ACCESS_PRIVATE)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(செய்தி, m->ocm_name.string, 0, cl);
			status = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    இல்லையெனில் எனில் (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			status = FAIL;
		    இல்லையெனில்
			lidx = m_idx;
		}
		இல்லையெனில்
		{
		    member_not_found_செய்தி(cl, VAR_OBJECT, member, 0);
		    status = FAIL;
		}
	    }
	}
	இல்லையெனில் எனில் (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    status = FAIL;
	}
	இல்லையெனில் எனில் (dest_type == VAR_LIST && tv_idx->v_type != VAR_எண்)
	{
	    emsg(_(e_எண்_expected));
	    status = FAIL;
	}
    }

    எனில் (status == OK)
    {
	எனில் (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    எனில் (list == வெற்று)
	    {
		emsg(_(e_list_not_set));
		திரும்பு FAIL;
	    }
	    எனில் (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    எனில் (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_index_out_of_range_nr), lidx);
		திரும்பு FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    எனில் (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		திரும்பு FAIL;

	    எனில் (lidx < list->lv_len)
	    {
		listitem_T *li = list_find(list, lidx);
		எனில் (li == வெற்று)
		    திரும்பு FAIL;
		எனில் (error_if_locked(li->li_tv.v_lock,
					     e_cannot_change_locked_list_item))
		    திரும்பு FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    இல்லையெனில்
	    {
		எனில் (error_if_locked(list->lv_lock, e_cannot_change_locked_list))
		    திரும்பு FAIL;
		// append to list, only fails when out of memory
		எனில் (list_append_tv(list, tv) == FAIL)
		    திரும்பு NOTDONE;
		clear_tv(tv);
	    }
	}
	இல்லையெனில் எனில் (dest_type == VAR_DICT)
	{
	    char_u		*சாவி = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    எனில் (dict == வெற்று)
	    {
		emsg(_(e_dictionary_not_set));
		திரும்பு FAIL;
	    }
	    எனில் (சாவி == வெற்று)
		சாவி = (char_u *)"";
	    di = dict_find(dict, சாவி, -1);
	    எனில் (di != வெற்று)
	    {
		எனில் (error_if_locked(di->di_tv.v_lock,
						    e_cannot_change_dict_item))
		    திரும்பு FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    இல்லையெனில்
	    {
		எனில் (error_if_locked(dict->dv_lock, e_cannot_change_dict))
		    திரும்பு FAIL;
		// add to dict, only fails when out of memory
		எனில் (dict_add_tv(dict, (எழுத்து *)சாவி, tv) == FAIL)
		    திரும்பு NOTDONE;
		clear_tv(tv);
	    }
	}
	இல்லையெனில் எனில் (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    முழு		    error = FALSE;
	    முழு		    len;

	    எனில் (blob == வெற்று)
	    {
		emsg(_(e_blob_not_set));
		திரும்பு FAIL;
	    }
	    len = blob_len(blob);
	    எனில் (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    எனில் (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), lidx);
		திரும்பு FAIL;
	    }
	    எனில் (மதிப்பு_check_lock(blob->bv_lock, (char_u *)"blob", FALSE))
		திரும்பு FAIL;
	    nr = tv_get_எண்_chk(tv, &error);
	    எனில் (error)
		திரும்பு FAIL;
	    எனில் (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_மதிப்பு_for_blob_nr), nr);
		திரும்பு FAIL;
	    }
	    blob_set_append(blob, lidx, nr);
	}
	இல்லையெனில் எனில் (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    எனில் (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		எனில் (itf != வெற்று)
		    // convert interface member index to class member index
		    lidx = object_index_from_itf_index(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    இல்லையெனில்
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	இல்லையெனில் எனில் (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    status = FAIL;
	}
	இல்லையெனில்
	{
	    status = FAIL;
	    semsg(_(e_cannot_index_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_அடுக்கு.ga_len -= 3;
    எனில் (status == FAIL)
    {
	clear_tv(tv);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

/*
 * Store a value in a list or blob range.
 */
    நிலையான முழு
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = அடுக்கு_TV_BOT(-3);
    typval_T	*tv_idx2 = அடுக்கு_TV_BOT(-2);
    typval_T	*tv_dest = அடுக்கு_TV_BOT(-1);
    முழு		status = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = அடுக்கு_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    எனில் (tv_dest->v_type == VAR_LIST)
    {
	long	    n1;
	long	    n2;
	listitem_T  *li1;

	n1 = (long)tv_get_எண்_chk(tv_idx1, வெற்று);
	எனில் (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_எண் == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	இல்லையெனில்
	    n2 = (long)tv_get_எண்_chk(tv_idx2, வெற்று);

	li1 = check_range_index_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	எனில் (li1 == வெற்று)
	    status = FAIL;
	இல்லையெனில்
	{
	    status = check_range_index_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    எனில் (status != FAIL)
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
    இல்லையெனில் எனில் (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	long	    bloblen;

	n1 = tv_get_எண்_chk(tv_idx1, வெற்று);
	எனில் (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_எண் == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	இல்லையெனில்
	    n2 = tv_get_எண்_chk(tv_idx2, வெற்று);
	bloblen = blob_len(tv_dest->vval.v_blob);

	எனில் (check_blob_index(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    status = FAIL;
	இல்லையெனில்
	    status = blob_set_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    இல்லையெனில்
    {
	status = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_அடுக்கு.ga_len -= 4;
    clear_tv(tv);

    திரும்பு status;
}

/*
 * Unlet item in list or dict variable.
 */
    நிலையான முழு
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = அடுக்கு_TV_BOT(-2);
    typval_T	*tv_dest = அடுக்கு_TV_BOT(-1);
    முழு		status = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    எனில் (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	எனில் (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_எண்)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    status = FAIL;
	}
	இல்லையெனில்
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*சாவி;
	    dictitem_T  *di = வெற்று;

	    எனில் (d != வெற்று && மதிப்பு_check_lock(
				      d->dv_lock, வெற்று, FALSE))
		status = FAIL;
	    இல்லையெனில்
	    {
		எனில் (tv_idx->v_type == VAR_STRING)
		{
		    சாவி = tv_idx->vval.v_string;
		    எனில் (சாவி == வெற்று)
			சாவி = (char_u *)"";
		}
		இல்லையெனில்
		{
		    சாவி = tv_get_string(tv_idx);
		}
		எனில் (d != வெற்று)
		    di = dict_find(d, சாவி, (முழு)STRLEN(சாவி));
		எனில் (di == வெற்று)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_சாவி_not_present_in_dictionary_str), சாவி);
		    status = FAIL;
		}
		இல்லையெனில் எனில் (var_check_fixed(di->di_flags,
						   வெற்று, FALSE)
			|| var_check_ro(di->di_flags,
						  வெற்று, FALSE))
		    status = FAIL;
		இல்லையெனில்
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    இல்லையெனில் எனில் (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	எனில் (check_for_எண்(tv_idx) == FAIL)
	{
	    status = FAIL;
	}
	இல்லையெனில்
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n = (long)tv_idx->vval.v_எண்;

	    எனில் (l != வெற்று && மதிப்பு_check_lock(
				      l->lv_lock, வெற்று, FALSE))
		status = FAIL;
	    இல்லையெனில்
	    {
		listitem_T	*li = list_find(l, n);

		எனில் (li == வெற்று)
		{
		    semsg(_(e_list_index_out_of_range_nr), n);
		    status = FAIL;
		}
		இல்லையெனில்
		    listitem_remove(l, li);
	    }
	}
    }
    இல்லையெனில்
    {
	status = FAIL;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_அடுக்கு.ga_len -= 2;

    திரும்பு status;
}

/*
 * Unlet a range of items in a list variable.
 */
    நிலையான முழு
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = அடுக்கு_TV_BOT(-3);
    typval_T	*tv_idx2 = அடுக்கு_TV_BOT(-2);
    typval_T	*tv_dest = அடுக்கு_TV_BOT(-1);
    முழு		status = OK;

    எனில் (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	எனில் (check_for_எண்(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_எண்(tv_idx2) == FAIL))
	{
	    status = FAIL;
	}
	இல்லையெனில்
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n1 = (long)tv_idx1->vval.v_எண்;
	    long	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (long)tv_idx2->vval.v_எண்;
	    listitem_T	*li;

	    li = list_find_index(l, &n1);
	    எனில் (li == வெற்று)
	    {
		semsg(_(e_list_index_out_of_range_nr),
						 (long)tv_idx1->vval.v_எண்);
		status = FAIL;
	    }
	    இல்லையெனில்
	    {
		எனில் (n1 < 0)
		    n1 = list_idx_of_item(l, li);
		எனில் (n2 < 0)
		{
		    listitem_T *li2 = list_find(l, n2);

		    எனில் (li2 == வெற்று)
		    {
			semsg(_(e_list_index_out_of_range_nr), n2);
			status = FAIL;
		    }
		    இல்லையெனில்
			n2 = list_idx_of_item(l, li2);
		}
		எனில் (status != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_index_out_of_range_nr), n2);
		    status = FAIL;
		}
		எனில் (status != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    இல்லையெனில்
    {
	status = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_அடுக்கு.ga_len -= 3;

    திரும்பு status;
}

/*
 * Top of a for loop.
 */
    நிலையான முழு
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    முழு		jump = FALSE;
    typval_T	*ltv = அடுக்கு_TV_BOT(-1);
    typval_T	*idxtv =
		   அடுக்கு_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
	திரும்பு FAIL;
    எனில் (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_எண்;
	எனில் (list == வெற்று
		       || idxtv->vval.v_எண் >= list->lv_len)
	{
	    jump = TRUE;
	}
	இல்லையெனில் எனில் (list->lv_first == &range_list_item)
	{
	    // non-materialized range() list
	    tv = அடுக்கு_TV_BOT(0);
	    tv->v_type = VAR_எண்;
	    tv->v_lock = 0;
	    tv->vval.v_எண் = list_find_nr(
			     list, idxtv->vval.v_எண், வெற்று);
	    ++ectx->ec_அடுக்கு.ga_len;
	}
	இல்லையெனில்
	{
	    listitem_T *li = list_find(list,
					 idxtv->vval.v_எண்);
	    எனில் (li == வெற்று)
		திரும்பு FAIL;
	    copy_tv(&li->li_tv, அடுக்கு_TV_BOT(0));
	    ++ectx->ec_அடுக்கு.ga_len;
	}
    }
    இல்லையெனில் எனில் (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_எண்;
	எனில் (tuple == வெற்று || idxtv->vval.v_எண் >= TUPLE_LEN(tuple))
	    jump = TRUE;
	இல்லையெனில்
	{
	    copy_tv(TUPLE_ITEM(tuple, idxtv->vval.v_எண்), அடுக்கு_TV_BOT(0));
	    ++ectx->ec_அடுக்கு.ga_len;
	}
    }
    இல்லையெனில் எனில் (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_எண்;
	எனில் (str == வெற்று || str[idxtv->vval.v_எண்] == NUL)
	{
	    jump = TRUE;
	}
	இல்லையெனில்
	{
	    முழு	clen = mb_ptr2len(str + idxtv->vval.v_எண்);

	    // Push the next character from the string.
	    tv = அடுக்கு_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_எண், clen);
	    ++ectx->ec_அடுக்கு.ga_len;
	    idxtv->vval.v_எண் += clen - 1;
	}
    }
    இல்லையெனில் எனில் (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	எனில் (idxtv->vval.v_எண் == -1 && blob != வெற்று)
	{
	    ltv->v_lock = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_எண்;
	எனில் (blob == வெற்று || idxtv->vval.v_எண் >= blob_len(blob))
	{
	    jump = TRUE;
	}
	இல்லையெனில்
	{
	    // Push the next byte from the blob.
	    tv = அடுக்கு_TV_BOT(0);
	    tv->v_type = VAR_எண்;
	    tv->vval.v_எண் = blob_get(blob,
					 idxtv->vval.v_எண்);
	    ++ectx->ec_அடுக்கு.ga_len;
	}
    }
    இல்லையெனில்
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	திரும்பு FAIL;
    }

    எனில் (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_end;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    இல்லையெனில்
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = அடுக்கு_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_எண் = ectx->ec_funcrefs.ga_len;
    }

    திரும்பு OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
நிலையான loopvars_T *first_loopvars = வெற்று;

    நிலையான வெறுமை
add_loopvars_to_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    எனில் (first_loopvars != வெற்று)
	first_loopvars->lvs_prev = loopvars;
    loopvars->lvs_next = first_loopvars;
    loopvars->lvs_prev = வெற்று;
    first_loopvars = loopvars;
}

    நிலையான வெறுமை
remove_loopvars_from_list(loopvars_T *loopvars)
{
    எனில் (loopvars->lvs_prev == வெற்று)
	first_loopvars = loopvars->lvs_next;
    இல்லையெனில்
	loopvars->lvs_prev->lvs_next = loopvars->lvs_next;
    எனில் (loopvars->lvs_next != வெற்று)
	loopvars->lvs_next->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    நிலையான முழு
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = அடுக்கு_TV_VAR(endloop->end_funcref_idx);
    முழு		prev_closure_மொத்தம் = tv_refcount->vval.v_எண்;
    முழு		depth = endloop->end_depth;
    garray_T	*gap = &ectx->ec_funcrefs;
    முழு		closure_in_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *அடுக்கு;
    முழு		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    க்கு (idx = prev_closure_மொத்தம்; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	எனில் (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == வெற்று)
	{
	    முழு refcount = pt->pt_refcount;
	    முழு i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    க்கு (i = 0; i < endloop->end_var_மொத்தம்; ++i)
	    {
		typval_T *stv = அடுக்கு_TV_VAR(endloop->end_var_idx + i);

		எனில் (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    எனில் (refcount > 1)
	    {
		closure_in_use = TRUE;
		நிறுத்து;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    எனில் (!closure_in_use)
	திரும்பு OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    எனில் (loopvars == வெற்று)
	திரும்பு FAIL;

    loopvars->lvs_ga.ga_len = endloop->end_var_மொத்தம்;
    அடுக்கு = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_data = அடுக்கு;
    எனில் (அடுக்கு == வெற்று)
    {
	vim_free(loopvars);
	திரும்பு FAIL;
    }
    add_loopvars_to_list(loopvars);

    // Move the variable values.
    க்கு (idx = 0; idx < endloop->end_var_மொத்தம்; ++idx)
    {
	typval_T *tv = அடுக்கு_TV_VAR(endloop->end_var_idx + idx);

	*(அடுக்கு + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    க்கு (idx = prev_closure_மொத்தம்; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	எனில் (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == வெற்று)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[depth] = loopvars;

	    pt->pt_outer.out_loop[depth].அடுக்கு = &loopvars->lvs_ga;
	    pt->pt_outer.out_loop[depth].var_idx -=
		  ectx->ec_frame_idx + அடுக்கு_FRAME_SIZE + endloop->end_var_idx;
	}
    }

    திரும்பு OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    முழு
loopvars_check_refcount(loopvars_T *loopvars)
{
    முழு		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    முழு		    done = 0;
	typval_T	*அடுக்கு = gap->ga_data;

    எனில் (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	திரும்பு FALSE;
    க்கு (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_data) + i;

	எனில் (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != வெற்று
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    முழு	    depth;

	    க்கு (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		எனில் (tv->vval.v_partial->pt_loopvars[depth] == loopvars)
		    ++done;
	}
    }
    எனில் (done != loopvars->lvs_min_refcount)
	திரும்பு FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    அடுக்கு = gap->ga_data;
    க்கு (i = 0; i < gap->ga_len; ++i)
	clear_tv(அடுக்கு + i);
    vim_free(அடுக்கு);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    திரும்பு TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    முழு
set_ref_in_loopvars(முழு copyID)
{
    loopvars_T *loopvars;

    க்கு (loopvars = first_loopvars; loopvars != வெற்று;
						 loopvars = loopvars->lvs_next)
    {
	typval_T    *அடுக்கு = loopvars->lvs_ga.ga_data;
	முழு	    i;

	க்கு (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    எனில் (set_ref_in_item(அடுக்கு + i, copyID, வெற்று, வெற்று, வெற்று))
		திரும்பு TRUE;  // abort
    }
    திரும்பு FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    நிலையான முழு
load_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = வெற்று;
    hashtab_T	*ht = வெற்று;
    எழுத்து	namespace;

    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
	திரும்பு NOTDONE;
    தேர்வு (isn_type)
    {
	நிலை ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    நிறுத்து;
	நிலை ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    நிறுத்து;
	நிலை ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    நிறுத்து;
	நிலை ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    நிறுத்து;
	இயல்பு:  // Cannot reach here
	    திரும்பு NOTDONE;
    }
    di = find_var_in_ht(ht, 0, iptr->isn_arg.string, TRUE);

    எனில் (di == வெற்று)
    {
	எனில் (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = find_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    எனில் (ufunc != வெற்று)
	    {
		typval_T    *tv = அடுக்கு_TV_BOT(0);

		++ectx->ec_அடுக்கு.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		எனில் (tv->vval.v_string == வெற்று)
		    திரும்பு FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		திரும்பு OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	எனில் (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != வெற்று)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_item_not_found_in_script_str), iptr->isn_arg.string);
	இல்லையெனில்
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = அடுக்கு_TV_BOT(0);
	tv->v_type = VAR_எண்;
	tv->v_lock = 0;
	tv->vval.v_எண் = 0;
	++ectx->ec_அடுக்கு.ga_len;
	திரும்பு FAIL;
    }
    இல்லையெனில்
    {
	copy_tv(&di->di_tv, அடுக்கு_TV_BOT(0));
	++ectx->ec_அடுக்கு.ga_len;
    }
    திரும்பு OK;
}


    நிலையான வெறுமை
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, அளவு(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    எழுத்து *tofree = வெற்று;
    எழுத்து *typename = type_name(type, &tofree);
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
    நிலையான முழு
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    முழு		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = வெற்று;
    typval_T	mtv;

    எனில் (is_object)
    {
	obj = tv->vval.v_object;
	எனில் (obj == வெற்று)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    திரும்பு FAIL;
	}
	tv_cl = obj->obj_class;
    }
    இல்லையெனில்
    {
	tv_cl = tv->vval.v_class;
	எனில் (tv_cl == வெற்று)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    திரும்பு FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    முழு		namelen = (முழு)STRLEN(iptr->isn_arg.string);
    முழு		save_did_emsg = did_emsg;

    எனில் (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	திரும்பு OK;
    }

    எனில் (did_emsg != save_did_emsg)
	திரும்பு FAIL;

    // could be a class or instance method
    ufunc_T	*oc_method;
    முழு		oc_method_idx;

    oc_method = method_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_method_idx);
    எனில் (oc_method == வெற்று)
    {
	எழுத்து	*செய்தி;

	SOURCING_LNUM = iptr->isn_lnum;
	எனில் (is_object)
	    செய்தி = e_variable_not_found_on_object_str_str;
	இல்லையெனில்
	    செய்தி = e_class_variable_str_not_found_in_class_str;
	semsg(_(செய்தி), iptr->isn_arg.string, tv_cl->class_name.string);
	திரும்பு FAIL;
    }

    // Protected methods are not accessible outside the class
    எனில் (*oc_method->uf_name == '_'
			&& !class_instance_of(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_method_str), oc_method->uf_name);
	திரும்பு FAIL;
    }

    // Create a partial for the instance or class method
    எனில் (obj_method_to_partial_tv(is_object ? obj : வெற்று, oc_method, tv)
								== FAIL)
	திரும்பு FAIL;

    திரும்பு OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    நிலையான வெறுமை
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, முழு fixindent)
{
    முழு		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*கோவை = வெற்று;
    முழு		dir = FORWARD;

    எனில் (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = அடுக்கு_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_எண்;
	எனில் (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_அடுக்கு.ga_len;
    }
    இல்லையெனில் எனில் (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    இல்லையெனில் எனில் (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	எனில் (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    எனில் (regname == '=')
    {
	tv = அடுக்கு_TV_BOT(-1);
	எனில் (tv->v_type == VAR_STRING)
	    கோவை = tv->vval.v_string;
	இல்லையெனில்
	{
	    கோவை = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_அடுக்கு.ga_len;
    }
    check_cursor();

    எனில் (fixindent)
	do_put(regname, கோவை, dir, 1L, PUT_வரி|PUT_CURSLINE|PUT_FIXINDENT);
    இல்லையெனில்
	do_put(regname, கோவை, dir, 1L, PUT_வரி|PUT_CURSLINE);
    vim_free(கோவை);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    நிலையான முழு
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    முழு		மொத்தம் = iptr->isn_arg.unpack.unp_மொத்தம்;
    முழு		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    முழு		i;

    l = tv->vval.v_list;
    எனில் (l == வெற்று
	    || l->lv_len < (semicolon ? மொத்தம் - 1 : மொத்தம்))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_மதிப்பு_does_not_have_enough_பொருட்கள்));
	திரும்பு EXEC_FAIL;
    }
    இல்லையெனில் எனில் (!semicolon && l->lv_len > மொத்தம்)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_மதிப்பு_has_more_பொருட்கள்_than_targets));
	திரும்பு EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, மொத்தம் - 1))
	திரும்பு EXEC_DONE;
    ectx->ec_அடுக்கு.ga_len += மொத்தம் - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    எனில் (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_பொருட்கள்(l->lv_len - மொத்தம் + 1);

	எனில் (rem_list == வெற்று)
	    திரும்பு EXEC_DONE;
	tv = அடுக்கு_TV_BOT(-மொத்தம்);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	li = l->lv_first;
	க்கு (i = 0; i < மொத்தம் - 1; ++i)
	    li = li->li_next;
	க்கு (i = 0; li != வெற்று; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_set_item(rem_list, i, &tvcopy);
	    li = li->li_next;
	}
	--மொத்தம்;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_first;
    க்கு (i = 0; i < மொத்தம்; ++i)
    {
	tv = அடுக்கு_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_next;
    }

    list_unref(l);

    திரும்பு EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    நிலையான முழு
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    முழு		மொத்தம் = iptr->isn_arg.unpack.unp_மொத்தம்;
    முழு		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    முழு		i;

    tuple = tv->vval.v_tuple;
    எனில் (tuple == வெற்று
	    || TUPLE_LEN(tuple) < (semicolon ? மொத்தம் - 1 : மொத்தம்))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_more_targets_than_tuple_பொருட்கள்));
	திரும்பு EXEC_FAIL;
    }
    இல்லையெனில் எனில் (!semicolon && TUPLE_LEN(tuple) > மொத்தம்)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_பொருட்கள்));
	திரும்பு EXEC_FAIL;
    }

    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, மொத்தம் - 1))
	திரும்பு EXEC_DONE;
    ectx->ec_அடுக்கு.ga_len += மொத்தம் - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    எனில் (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_பொருட்கள்(TUPLE_LEN(tuple) - மொத்தம் + 1);

	எனில் (rem_list == வெற்று)
	    திரும்பு EXEC_DONE;
	tv = அடுக்கு_TV_BOT(-மொத்தம்);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	க்கு (i = மொத்தம் - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ITEM(tuple, i), &tvcopy);
	    list_set_item(rem_list, i - (மொத்தம் - 1), &tvcopy);
	}
	--மொத்தம்;
    }

    // Produce the values in reverse order, first item last.
    க்கு (i = 0; i < மொத்தம்; ++i)
    {
	tv = அடுக்கு_TV_BOT(-i - 1);
	copy_tv(TUPLE_ITEM(tuple, i), tv);
    }

    tuple_unref(tuple);

    திரும்பு EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    நிலையான முழு
exec_instructions(ectx_T *ectx)
{
    முழு		ret = FAIL;
    முழு		save_trylevel_at_start = ectx->ec_trylevel_at_start;
    முழு		dict_அடுக்கு_len_at_start = dict_அடுக்கு.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_start = trylevel;

    க்கு (;;)
    {
	நிலையான முழு  breakcheck_மொத்தம் = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	எனில் (unlikely(++breakcheck_மொத்தம் >= 100))
	{
	    வரி_breakcheck();
	    breakcheck_மொத்தம் = 0;
	}
	எனில் (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    எனில் (throw_exception("Vim:Interrupt", ET_INTERRUPT, வெற்று) == FAIL)
		செல் theend;
	    did_throw = TRUE;
	}

	எனில் (unlikely(did_emsg && செய்தி_list != வெற்று && *செய்தி_list != வெற்று))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    எனில் (throw_exception(*செய்தி_list, ET_ERROR, வெற்று) == FAIL)
		செல் theend;
	    did_throw = TRUE;
	    *செய்தி_list = வெற்று;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    எனில் (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		எனில் (trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		{
		    எனில் (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			finish_exception(caught_அடுக்கு);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	எனில் (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = வெற்று;
	    முழு		index = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    போது (index > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_data) + index - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		எனில் (!trycmd->tcd_in_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    நிறுத்து;
		// In the catch and finally block of this try we have to go up
		// one level.
		--index;
		trycmd = வெற்று;
	    }
	    எனில் (trycmd != வெற்று && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
	    {
		எனில் (trycmd->tcd_in_catch)
		{
		    எனில் (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    இல்லையெனில்
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		இல்லையெனில்
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_in_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    இல்லையெனில்
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    செல் theend;
		tv = அடுக்கு_TV_BOT(0);
		tv->v_type = VAR_எண்;
		tv->vval.v_எண் = 0;
		++ectx->ec_அடுக்கு.ga_len;
		எனில் (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    எனில் (handle_closure_in_use(ectx, FALSE) == FAIL)
			செல் theend;
		    செல் done;
		}

		எனில் (func_return(ectx) == FAIL)
		    செல் theend;
	    }
	    தொடர்;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	தேர்வு (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    நிலை ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = அடுக்கு_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		நிறுத்து;

	    // execute Ex command line
	    நிலை ISN_EXEC:
		எனில் (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    செல் on_error;
		நிறுத்து;

	    // execute Ex command line split at NL characters.
	    நிலை ISN_EXEC_SPLIT:
		{
		    source_cookie_T cookie;
		    char_u	    *வரி;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    வரி = get_split_sourceline(0, &cookie, 0, 0);
		    எனில் (do_cmdline(வரி,
				get_split_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(வரி);
			செல் on_error;
		    }
		    vim_free(வரி);
		}
		நிறுத்து;

	    // execute Ex command line that is only a range
	    நிலை ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    எழுத்து	*error = வெற்று;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = CMD_SIZE;
		    ea.addr_type = ADDR_வரிகள்;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_address(&ea, &error, FALSE);
		    எனில் (ea.cmd == வெற்று)
			செல் on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    எனில் (error != வெற்று)
		    {
			emsg(error);
			செல் on_error;
		    }
		}
		நிறுத்து;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    நிலை ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    முழு	    res;
		    முழு	    save_flags = cmdmod.cmod_flags;

		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    tv = அடுக்கு_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, வெற்று, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    எனில் (res == FAIL)
			செல் on_error;
		    ++ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    // push typeval VAR_INSTR with instructions to be executed
	    நிலை ISN_INSTR:
		{
		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    tv = அடுக்கு_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    எனில் (tv->vval.v_instr == வெற்று)
			செல் on_error;
		    ++ectx->ec_அடுக்கு.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		நிறுத்து;

	    நிலை ISN_SOURCE:
		{
		    முழு notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (may_load_script((முழு)iptr->isn_arg.எண், &notused)
								       == FAIL)
			செல் on_error;
		}
		நிறுத்து;

	    // execute :substitute with an expression
	    நிலை ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    source_cookie_T	cookie;
		    கட்டமைப்பு subs_கோவை_S	*save_instr = substitute_instr;
		    கட்டமைப்பு subs_கோவை_S	subs_instr;
		    முழு			res;

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

		    எனில் (res == FAIL || did_emsg
					     || subs_instr.subs_status == FAIL)
			செல் on_error;
		}
		நிறுத்து;

	    நிலை ISN_FINISH:
		செல் done;

	    நிலை ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		எனில் (alloc_redir_lval() == FAIL)
		    செல் on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		நிறுத்து;

	    நிலை ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    {
			vim_free(res);
			செல் theend;
		    }
		    tv = அடுக்கு_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    நிலை ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		force_abort = TRUE;
		எனில் (trigger_cexpr_autocmd(iptr->isn_arg.எண்) == FAIL)
		    செல் on_error;
		force_abort = FALSE;
#endif
		நிறுத்து;

	    நிலை ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    exarg_T ea;
		    முழு	    res;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_அடுக்கு.ga_len;
		    tv = அடுக்கு_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    எனில் (res == FAIL)
			செல் on_error;
		}
#endif
		நிறுத்து;

	    // execute Ex command from pieces on the stack
	    நிலை ISN_EXECCONCAT:
		{
		    முழு	    மொத்தம் = iptr->isn_arg.எண்;
		    size_t  len = 0;
		    முழு	    pass;
		    முழு	    i;
		    char_u  *cmd = வெற்று;
		    char_u  *str;

		    க்கு (pass = 1; pass <= 2; ++pass)
		    {
			க்கு (i = 0; i < மொத்தம்; ++i)
			{
			    tv = அடுக்கு_TV_BOT(i - மொத்தம்);
			    str = tv->vval.v_string;
			    எனில் (str != வெற்று && *str != NUL)
			    {
				எனில் (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    எனில் (pass == 2)
				clear_tv(tv);
			}
			எனில் (pass == 1)
			{
			    cmd = alloc(len + 1);
			    எனில் (unlikely(cmd == வெற்று))
				செல் theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		நிறுத்து;

	    // execute :echo {string} ...
	    நிலை ISN_ECHO:
		{
		    முழு மொத்தம் = iptr->isn_arg.echo.echo_மொத்தம்;
		    முழு	atstart = TRUE;
		    முழு needclr = TRUE;
		    முழு	idx;

		    க்கு (idx = 0; idx < மொத்தம்; ++idx)
		    {
			tv = அடுக்கு_TV_BOT(idx - மொத்தம்);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    எனில் (needclr)
			செய்தி_clr_eos();
		    ectx->ec_அடுக்கு.ga_len -= மொத்தம்;
		}
		நிறுத்து;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    நிலை ISN_EXECUTE:
	    நிலை ISN_ECHOMSG:
	    நிலை ISN_ECHOWINDOW:
	    நிலை ISN_ECHOCONSOLE:
	    நிலை ISN_ECHOERR:
		{
		    முழு		மொத்தம்;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    முழு		len;
		    முழு		failed = FALSE;
		    முழு		idx;

		    எனில் (iptr->isn_type == ISN_ECHOWINDOW)
			மொத்தம் = iptr->isn_arg.echowin.ewin_மொத்தம்;
		    இல்லையெனில்
			மொத்தம் = iptr->isn_arg.எண்;
		    ga_init2(&ga, 1, 80);
		    க்கு (idx = 0; idx < மொத்தம்; ++idx)
		    {
			tv = அடுக்கு_TV_BOT(idx - மொத்தம்);
			எனில் (iptr->isn_type == ISN_EXECUTE)
			{
			    எனில் (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_JOB)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_மதிப்பு_as_string_str),
						    vartype_name(tv->v_type));
				நிறுத்து;
			    }
			    இல்லையெனில்
				p = tv_get_string_buf(tv, buf);
			}
			இல்லையெனில்
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (முழு)STRLEN(p);
			எனில் (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			இல்லையெனில்
			{
			    எனில் (ga.ga_len > 0)
				((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_அடுக்கு.ga_len -= மொத்தம்;
		    எனில் (failed)
		    {
			ga_clear(&ga);
			செல் on_error;
		    }

		    எனில் (ga.ga_data != வெற்று)
		    {
			எனில் (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_data);
			    எனில் (did_emsg)
			    {
				ga_clear(&ga);
				செல் on_error;
			    }
			}
			இல்லையெனில்
			{
			    செய்தி_sb_eol();
			    எனில் (iptr->isn_type == ISN_ECHOMSG)
			    {
				செய்தி_attr(ga.ga_data, echo_attr);
				out_flush();
			    }
#ifdef HAS_MESSAGE_WINDOW
			    இல்லையெனில் எனில் (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				start_echowindow(
					      iptr->isn_arg.echowin.ewin_time);
				செய்தி_attr(ga.ga_data, echo_attr);
				end_echowindow();
			    }
#endif
			    இல்லையெனில் எனில் (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_data, (முழு)STRLEN(ga.ga_data),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    இல்லையெனில்
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_data);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		நிறுத்து;

	    // load local variable or argument
	    நிலை ISN_LOAD:
		எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    செல் theend;
		tv = அடுக்கு_TV_VAR(iptr->isn_arg.எண்);
		எனில் (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    அடுக்கு_TV_BOT(0)->v_type = VAR_SPECIAL;
		    அடுக்கு_TV_BOT(0)->vval.v_எண் = VVAL_NONE;
		}
		இல்லையெனில்
		    copy_tv(tv, அடுக்கு_TV_BOT(0));
		++ectx->ec_அடுக்கு.ga_len;
		நிறுத்து;

	    // load v: variable
	    நிலை ISN_LOADV:
		எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    செல் theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.எண்), அடுக்கு_TV_BOT(0));
		++ectx->ec_அடுக்கு.ga_len;
		நிறுத்து;

	    // load s: variable in Vim9 script
	    நிலை ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    எனில் (sv == வெற்று)
			செல் theend;
		    allocate_if_null(sv);
		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    copy_tv(sv->sv_tv, அடுக்கு_TV_BOT(0));
		    ++ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    // load s: variable in old script or autoload import
	    நிலை ISN_LOADS:
	    நிலை ISN_LOADEXPORT:
		{
		    முழு		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = find_var_in_ht(ht, 0, name, TRUE);

		    எனில் (di == வெற்று)
		    {
			ufunc_T	*ufunc = வெற்று;

			எனில் (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (வெறுமை)find_exported(sid, name, &ufunc, &type,
							 வெற்று, வெற்று, FALSE);
			}
			எனில் (ufunc == வெற்று)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    செல் on_error;
			}
			எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			    செல் theend;
			tv = அடுக்கு_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_அடுக்கு.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    இல்லையெனில்
		    {
			எனில் (iptr->isn_type == ISN_LOADEXPORT)
			{
			    முழு idx = get_script_item_idx(sid, name, 0,
								   வெற்று, வெற்று);
			    svar_T	*sv;

			    எனில் (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;
				எனில் ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    செல் on_error;
				}
			    }
			}
			எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			    செல் theend;
			copy_tv(&di->di_tv, அடுக்கு_TV_BOT(0));
			++ectx->ec_அடுக்கு.ga_len;
		    }
		}
		நிறுத்து;

	    // load g:/b:/w:/t: variable
	    நிலை ISN_LOADG:
	    நிலை ISN_LOADB:
	    நிலை ISN_LOADW:
	    நிலை ISN_LOADT:
		{
		    முழு res = load_namespace_var(ectx, iptr->isn_type, iptr);

		    எனில் (res == NOTDONE)
			செல் theend;
		    எனில் (res == FAIL)
			செல் on_error;
		}

		நிறுத்து;

	    // load autoload variable
	    நிலை ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (eval_variable(name, (முழு)STRLEN(name), 0,
			      அடுக்கு_TV_BOT(0), வெற்று, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = அடுக்கு_TV_BOT(0);
			tv->v_type = VAR_எண்;
			tv->v_lock = 0;
			tv->vval.v_எண் = 0;
			++ectx->ec_அடுக்கு.ga_len;
			செல் on_error;
		    }
		    ++ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    // load g:/b:/w:/t: namespace
	    நிலை ISN_LOADGDICT:
	    நிலை ISN_LOADBDICT:
	    நிலை ISN_LOADWDICT:
	    நிலை ISN_LOADTDICT:
		{
		    dict_T *d = வெற்று;

		    தேர்வு (iptr->isn_type)
		    {
			நிலை ISN_LOADGDICT: d = get_globvar_dict(); நிறுத்து;
			நிலை ISN_LOADBDICT: d = curbuf->b_vars; நிறுத்து;
			நிலை ISN_LOADWDICT: d = curwin->w_vars; நிறுத்து;
			நிலை ISN_LOADTDICT: d = curtab->tp_vars; நிறுத்து;
			இயல்பு:  // Cannot reach here
			    செல் theend;
		    }
		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    tv = அடுக்கு_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_lock = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    // load &option
	    நிலை ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    எனில் (eval_option(&name, &optval, TRUE) == FAIL)
			செல் theend;
		    *அடுக்கு_TV_BOT(0) = optval;
		    ++ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    // load $ENV
	    நிலை ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    // name is always valid, checked when compiling
		    (வெறுமை)eval_env_var(&name, &optval, TRUE);
		    *அடுக்கு_TV_BOT(0) = optval;
		    ++ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    // load @register
	    நிலை ISN_LOADREG:
		எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    செல் theend;
		tv = அடுக்கு_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.எண், GREG_கோவை_SRC);
		++ectx->ec_அடுக்கு.ga_len;
		நிறுத்து;

	    // store local variable
	    நிலை ISN_STORE:
		--ectx->ec_அடுக்கு.ga_len;
		tv = அடுக்கு_TV_VAR(iptr->isn_arg.எண்);
		எனில் (check_typval_is_மதிப்பு(அடுக்கு_TV_BOT(0)) == FAIL
			|| அடுக்கு_TV_BOT(0)->v_type == VAR_VOID)
		{
		    எனில் (அடுக்கு_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_மதிப்பு));
		    clear_tv(அடுக்கு_TV_BOT(0));
		    செல் on_error;
		}
		clear_tv(tv);
		*tv = *அடுக்கு_TV_BOT(0);
		நிறுத்து;

	    // store s: variable in old script or autoload import
	    நிலை ISN_STORES:
	    நிலை ISN_STOREEXPORT:
		{
		    முழு		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = வெற்று;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    எனில் (iptr->isn_type == ISN_STOREEXPORT)
			di = find_var_autoload_prefix(name, sid, வெற்று, வெற்று);
		    // Then look for a variable in the script's variables.
		    எனில் (di == வெற்று)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = find_var_in_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_அடுக்கு.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (di == வெற்று)
		    {
			எனில் (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(அடுக்கு_TV_BOT(0));
			    செல் on_error;
			}
			store_var(name, அடுக்கு_TV_BOT(0));
		    }
		    இல்லையெனில்
		    {
			எனில் (iptr->isn_type == ISN_STOREEXPORT)
			{
			    முழு idx = get_script_item_idx(sid, name, 0,
								   வெற்று, வெற்று);

			    // can this ever fail?
			    எனில் (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;

				எனில் ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    clear_tv(அடுக்கு_TV_BOT(0));
				    செல் on_error;
				}
			    }
			}
			எனில் (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(அடுக்கு_TV_BOT(0));
			    செல் on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *அடுக்கு_TV_BOT(0);
		    }
		}
		நிறுத்து;

	    // store script-local variable in Vim9 script
	    நிலை ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    எனில் (sv == வெற்று)
			செல் theend;
		    --ectx->ec_அடுக்கு.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (மதிப்பு_check_lock(sv->sv_tv->v_lock, sv->sv_name, FALSE))
		    {
			clear_tv(அடுக்கு_TV_BOT(0));
			செல் on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *அடுக்கு_TV_BOT(0);
		}
		நிறுத்து;

	    // store option
	    நிலை ISN_STOREOPT:
	    நிலை ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    முழு		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    long	n = 0;
		    char_u	*s = வெற்று;
		    எழுத்து	*செய்தி;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = வெற்று;

		    --ectx->ec_அடுக்கு.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = அடுக்கு_TV_BOT(0);
		    எனில் (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			எனில் (s == வெற்று)
			    s = (char_u *)"";
		    }
		    இல்லையெனில் எனில் (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			எனில் (s == வெற்று || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    செல் on_error;
			}
		    }
		    இல்லையெனில்
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_எண்;
		    செய்தி = set_option_மதிப்பு(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    எனில் (செய்தி != வெற்று)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(செய்தி));
			செல் on_error;
		    }
		}
		நிறுத்து;

	    // store $ENV
	    நிலை ISN_STOREENV:
		எனில் (check_secure() || check_restricted())
		    செல் theend;
		--ectx->ec_அடுக்கு.ga_len;
		tv = அடுக்கு_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		நிறுத்து;

	    // store @r
	    நிலை ISN_STOREREG:
		{
		    முழு	reg = iptr->isn_arg.எண்;

		    --ectx->ec_அடுக்கு.ga_len;
		    tv = அடுக்கு_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		நிறுத்து;

	    // store v: variable
	    நிலை ISN_STOREV:
		--ectx->ec_அடுக்கு.ga_len;
		எனில் (set_vim_var_tv(iptr->isn_arg.எண், அடுக்கு_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    செல் on_error;
		நிறுத்து;

	    // store g:/b:/w:/t: variable
	    நிலை ISN_STOREG:
	    நிலை ISN_STOREB:
	    நிலை ISN_STOREW:
	    நிலை ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    தேர்வு (iptr->isn_type)
		    {
			நிலை ISN_STOREG:
			    ht = get_globvar_ht();
			    நிறுத்து;
			நிலை ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    நிறுத்து;
			நிலை ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    நிறுத்து;
			நிலை ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    நிறுத்து;
			இயல்பு:  // Cannot reach here
			    செல் theend;
		    }

		    --ectx->ec_அடுக்கு.ga_len;
		    di = find_var_in_ht(ht, 0, name, TRUE);
		    எனில் (di == வெற்று)
			store_var(iptr->isn_arg.string, அடுக்கு_TV_BOT(0));
		    இல்லையெனில்
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			எனில் (var_check_permission(di, name) == FAIL)
			    செல் on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *அடுக்கு_TV_BOT(0);
		    }
		}
		நிறுத்து;

	    // store an autoload variable
	    நிலை ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		set_var(iptr->isn_arg.string, அடுக்கு_TV_BOT(-1), TRUE);
		clear_tv(அடுக்கு_TV_BOT(-1));
		--ectx->ec_அடுக்கு.ga_len;
		நிறுத்து;

	    // store number in local variable
	    நிலை ISN_STORENR:
		tv = அடுக்கு_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_எண்;
		tv->vval.v_எண் = iptr->isn_arg.storenr.stnr_val;
		நிறுத்து;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    நிலை ISN_STOREINDEX:
		{
		    முழு res = execute_storeindex(iptr, ectx);

		    எனில் (res == FAIL)
			செல் on_error;
		    எனில் (res == NOTDONE)
			செல் theend;
		}
		நிறுத்து;

	    // store value in list or blob range
	    நிலை ISN_STORERANGE:
		எனில் (execute_storerange(iptr, ectx) == FAIL)
		    செல் on_error;
		நிறுத்து;

	    நிலை ISN_LOAD_CLASSMEMBER:
		{
		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      அடுக்கு_TV_BOT(0));
		    ++ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    நிலை ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *அடுக்கு_TV_BOT(-1);
		    --ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    // Load or store variable or argument from outer scope.
	    நிலை ISN_LOADOUTER:
	    நிலை ISN_STOREOUTER:
		{
		    முழு		depth = iptr->isn_arg.outer.outer_depth;
		    outer_T	*outer = ectx->ec_outer_ref == வெற்று ? வெற்று
						: ectx->ec_outer_ref->or_outer;

		    போது (depth > 1 && outer != வெற்று)
		    {
			outer = outer->out_up;
			--depth;
		    }
		    எனில் (outer == வெற்று)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			எனில் (ectx->ec_frame_idx == ectx->ec_initial_frame_idx
						 || ectx->ec_outer_ref == வெற்று)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_closure_called_from_invalid_context));
			இல்லையெனில்
			    iemsg("LOADOUTER depth more than scope levels");
			செல் theend;
		    }
		    எனில் (depth < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->out_loop[-depth - 1]
							       .அடுக்கு->ga_data)
					  + outer->out_loop[-depth - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    இல்லையெனில்
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->out_அடுக்கு->ga_data)
				      + outer->out_frame_idx + அடுக்கு_FRAME_SIZE
				      + iptr->isn_arg.outer.outer_idx;
		    எனில் (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			    செல் theend;
			// careful: ga_grow_inner may re-alloc the stack
			எனில் (depth < 0)
			    copy = ((typval_T *)outer->out_loop[-depth - 1]
								   .அடுக்கு->ga_data)
					      + outer->out_loop[-depth - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			இல்லையெனில்
			    copy = ((typval_T *)outer->out_அடுக்கு->ga_data)
					  + outer->out_frame_idx + அடுக்கு_FRAME_SIZE
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			எனில் (copy != tv)
			    tv = copy;
			copy_tv(tv, அடுக்கு_TV_BOT(0));
			++ectx->ec_அடுக்கு.ga_len;
		    }
		    இல்லையெனில்
		    {
			--ectx->ec_அடுக்கு.ga_len;
			clear_tv(tv);
			*tv = *அடுக்கு_TV_BOT(0);
		    }
		}
		நிறுத்து;

	    // unlet item in list or dict variable
	    நிலை ISN_UNLETINDEX:
		எனில் (execute_unletindex(iptr, ectx) == FAIL)
		    செல் on_error;
		நிறுத்து;

	    // unlet range of items in list variable
	    நிலை ISN_UNLETRANGE:
		எனில் (execute_unletrange(iptr, ectx) == FAIL)
		    செல் on_error;
		நிறுத்து;

	    // push constant
	    நிலை ISN_PUSHNR:
	    நிலை ISN_PUSHBOOL:
	    நிலை ISN_PUSHSPEC:
	    நிலை ISN_PUSHF:
	    நிலை ISN_PUSHS:
	    நிலை ISN_PUSHBLOB:
	    நிலை ISN_PUSHFUNC:
	    நிலை ISN_PUSHCHANNEL:
	    நிலை ISN_PUSHJOB:
	    நிலை ISN_PUSHOBJ:
	    நிலை ISN_PUSHCLASS:
		எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    செல் theend;
		tv = அடுக்கு_TV_BOT(0);
		tv->v_lock = 0;
		++ectx->ec_அடுக்கு.ga_len;
		தேர்வு (iptr->isn_type)
		{
		    நிலை ISN_PUSHNR:
			tv->v_type = VAR_எண்;
			tv->vval.v_எண் = iptr->isn_arg.எண்;
			நிறுத்து;
		    நிலை ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_எண் = iptr->isn_arg.எண்;
			நிறுத்து;
		    நிலை ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_எண் = iptr->isn_arg.எண்;
			நிறுத்து;
		    நிலை ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			நிறுத்து;
		    நிலை ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			நிறுத்து;
		    நிலை ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			எனில் (iptr->isn_arg.string == வெற்று)
			    tv->vval.v_string = வெற்று;
			இல்லையெனில்
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			நிறுத்து;
		    நிலை ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = வெற்று;
#endif
			நிறுத்து;
		    நிலை ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_JOB;
			tv->vval.v_job = வெற்று;
#endif
			நிறுத்து;
		    நிலை ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = வெற்று;
			நிறுத்து;
		    நிலை ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			நிறுத்து;
		    இயல்பு:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == வெற்று
				    ? வெற்று : vim_strsave(iptr->isn_arg.string);
		}
		நிறுத்து;

	    நிலை ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (வெறுமை)script_autoload(name, FALSE);
		    எனில் (find_func(name, TRUE))
		    {
			எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			    செல் theend;
			tv = அடுக்கு_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_அடுக்கு.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    இல்லையெனில்
		    {
			முழு res = load_namespace_var(ectx, ISN_LOADG, iptr);

			எனில் (res == NOTDONE)
			    செல் theend;
			எனில் (res == FAIL)
			    செல் on_error;
		    }
		}
		நிறுத்து;

	    நிலை ISN_UNLET:
		எனில் (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    செல் on_error;
		நிறுத்து;
	    நிலை ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		நிறுத்து;

	    நிலை ISN_LOCKUNLOCK:
		{
#ifdef LOG_LOCKVAR
		    ch_log(வெற்று, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#endif
		    lval_வேர்_T	*lval_வேர்_save = lval_வேர்;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_அடுக்கு.ga_len;
		    lval_வேர்_T வேர்;
		    வேர்.lr_tv      = அடுக்கு_TV_BOT(0);
		    வேர்.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    வேர்.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_வேர் = &வேர்;
		    முழு res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(வேர்.lr_tv);
		    lval_வேர் = lval_வேர்_save;
		    எனில் (res == FAIL)
			செல் on_error;
		}
		நிறுத்து;

	    நிலை ISN_LOCKCONST:
		item_lock(அடுக்கு_TV_BOT(-1), 100, TRUE, TRUE);
		நிறுத்து;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    நிலை ISN_NEWLIST:
		எனில் (exe_newlist(iptr->isn_arg.எண், ectx) == FAIL)
		    செல் theend;
		நிறுத்து;

	    // create a tuple from items on the stack
	    நிலை ISN_NEWTUPLE:
		எனில் (exe_newtuple(iptr->isn_arg.எண், ectx) == FAIL)
		    செல் theend;
		நிறுத்து;

	    // create a dict from items on the stack
	    நிலை ISN_NEWDICT:
		{
		    முழு res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.எண், ectx);
		    எனில் (res == FAIL)
			செல் theend;
		    எனில் (res == MAYBE)
			செல் on_error;
		}
		நிறுத்து;

	    நிலை ISN_CONCAT:
		எனில் (exe_concat(iptr->isn_arg.எண், ectx) == FAIL)
		    செல் theend;
		நிறுத்து;

	    // create a partial with NULL value
	    நிலை ISN_NEWPARTIAL:
		எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    செல் theend;
		++ectx->ec_அடுக்கு.ga_len;
		tv = அடுக்கு_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_lock = 0;
		tv->vval.v_partial = வெற்று;
		நிறுத்து;

	    // call a :def function
	    நிலை ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		எனில் (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				வெற்று,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    செல் on_error;
		நிறுத்து;

	    // call a method on an interface
	    நிலை ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = அடுக்கு_TV_BOT(-1 - mfunc->cmf_argcount);
		    எனில் (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			செல் on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    எனில் (obj == வெற்று)
		    {
			emsg(_(e_using_null_object));
			செல் on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    எனில் (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    இல்லையெனில்
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			முழு idx = object_index_from_itf_index(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    எனில் (mfunc->cmf_ufunc != வெற்று && IS_GENERIC_FUNC(ufunc))
		    {
			எனில் (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			இல்லையெனில்
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    எனில் (ufunc == வெற்று)
				செல் on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    எனில் (call_ufunc(ufunc, வெற்று, mfunc->cmf_argcount, ectx,
							   வெற்று, வெற்று) == FAIL)
			செல் on_error;
		}
		நிறுத்து;

	    // call a builtin function
	    நிலை ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		எனில் (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    செல் on_error;
		நிறுத்து;

	    // call a funcref or partial
	    நிலை ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    முழு		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (pfunc->cpf_உச்சி)
		    {
			// funcref is above the arguments
			tv = அடுக்கு_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    இல்லையெனில்
		    {
			// Get the funcref from the stack.
			--ectx->ec_அடுக்கு.ga_len;
			partial_tv = *அடுக்கு_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    எனில் (tv == &partial_tv)
			clear_tv(&partial_tv);
		    எனில் (r == FAIL)
			செல் on_error;
		}
		நிறுத்து;

	    நிலை ISN_PCALL_END:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_அடுக்கு.ga_len;
		clear_tv(அடுக்கு_TV_BOT(-1));
		*அடுக்கு_TV_BOT(-1) = *அடுக்கு_TV_BOT(0);
		நிறுத்து;

	    // call a user defined function or funcref/partial
	    நிலை ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			செல் on_error;
		}
		நிறுத்து;

	    // :defer func(arg)
	    நிலை ISN_DEFER:
		எனில் (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    செல் on_error;
		நிறுத்து;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    நிலை ISN_RETURN_VOID:
	    நிலை ISN_RETURN_OBJECT:
		எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    செல் theend;
		tv = அடுக்கு_TV_BOT(0);
		++ectx->ec_அடுக்கு.ga_len;
		எனில் (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_எண் = 0;
		    tv->v_lock = 0;
		}
		இல்லையெனில்
		{
		    *tv = *அடுக்கு_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_lock_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    நிலை ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = வெற்று;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    எனில் (check_typval_is_மதிப்பு(அடுக்கு_TV_BOT(-1)) == FAIL)
			செல் theend;

		    எனில் (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    எனில் (trycmd != வெற்று
				 && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    {
			// jump to ":finally" or ":endtry"
			எனில் (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			இல்லையெனில்
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    இல்லையெனில்
			செல் func_return;
		}
		நிறுத்து;

	    // push a partial, a reference to a compiled function
	    நிலை ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    எனில் (pt == வெற்று)
			செல் theend;
		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    {
			vim_free(pt);
			செல் theend;
		    }
		    எனில் (extra != வெற்று && extra->fre_class != வெற்று)
		    {
			class_T	*cl;
			எனில் (extra->fre_object_method)
			{
			    tv = அடுக்கு_TV_BOT(-1);
			    எனில் (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				செல் on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    எனில் (obj == வெற்று)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				செல் on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_அடுக்கு.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			இல்லையெனில்
			    cl = extra->fre_class;

			எனில் (extra->fre_object_method)
			{
			    // object method
			    // convert the interface index to the object index
			    முழு idx =
				object_index_from_itf_index(extra->fre_class,
					TRUE, extra->fre_method_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			இல்லையெனில்
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_method_idx];
			}
		    }
		    இல்லையெனில் எனில் (extra == வெற்று || extra->fre_func_name == வெற்று)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    இல்லையெனில்
		    {
			ufunc = find_func(extra->fre_func_name, FALSE);
		    }
		    எனில் (ufunc == வெற்று)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			செல் theend;
		    }
		    எனில் (extra != வெற்று && extra->fre_outer_depth > 0
					   && (ufunc->uf_flags & FC_CLOSURE))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == வெற்று
				    ? வெற்று : ectx->ec_outer_ref->or_partial;

			க்கு (முழு depth = 1; outer_pt != வெற்று
				    && depth < extra->fre_outer_depth; ++depth)
			    outer_pt = get_pt_outer(outer_pt)->out_up_partial;
			எனில் (outer_pt == வெற்று)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    செல் theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    இல்லையெனில் எனில் (fill_partial_and_closure(pt, ufunc,
			       extra == வெற்று ? வெற்று : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			செல் theend;
		    }
		    tv = அடுக்கு_TV_BOT(0);
		    ++ectx->ec_அடுக்கு.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_lock = 0;
		}
		நிறுத்து;

	    // Create a global function from a lambda.
	    நிலை ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    எனில் (copy_lambda_to_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			செல் theend;
		}
		நிறுத்து;

	    // List functions
	    நிலை ISN_DEF:
		எனில் (iptr->isn_arg.string == வெற்று)
		    list_functions(வெற்று);
		இல்லையெனில்
		{
		    exarg_T	ea;
		    garray_T	வரிகள்_to_free;

		    CLEAR_FIELD(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&வரிகள்_to_free, அளவு(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, வெற்று, &வரிகள்_to_free, 0, வெற்று, 0, வெற்று);
		    ga_clear_strings(&வரிகள்_to_free);
		}
		நிறுத்து;

	    // jump if a condition is met
	    நிலை ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    முழு		error = FALSE;
		    முழு		jump = TRUE;

		    எனில் (when != JUMP_ALWAYS)
		    {
			tv = அடுக்கு_TV_BOT(-1);
			எனில் (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    எனில் (error)
				செல் on_error;
			}
			இல்லையெனில்
			    jump = tv2bool(tv);
			எனில் (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			எனில் (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_அடுக்கு.ga_len;
			}
		    }
		    எனில் (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		நிறுத்து;

	    // "while": jump to end if a condition is false
	    நிலை ISN_WHILE:
		{
		    முழு		error = FALSE;
		    முழு		jump = TRUE;

		    tv = அடுக்கு_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    எனில் (error)
			செல் on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_அடுக்கு.ga_len;
		    எனில் (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_end;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = அடுக்கு_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_எண் = ectx->ec_funcrefs.ga_len;
		}
		நிறுத்து;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    நிலை ISN_JUMP_IF_ARG_SET:
	    நிலை ISN_JUMP_IF_ARG_NOT_SET:
		tv = அடுக்கு_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		முழு arg_set = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_எண் == VVAL_NONE);

		எனில் (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_SET && !arg_set)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_data)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_args.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ அடுக்கு_FRAME_SIZE;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_POINTER(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    எனில் (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		எனில் (iptr->isn_type == ISN_JUMP_IF_ARG_SET ? arg_set : !arg_set)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		நிறுத்து;

	    // top of a for loop
	    நிலை ISN_FOR:
		எனில் (execute_for(iptr, ectx) == FAIL)
		    செல் theend;
		நிறுத்து;

	    // end of a for or while loop
	    நிலை ISN_ENDLOOP:
		எனில் (execute_endloop(iptr, ectx) == FAIL)
		    செல் theend;
		நிறுத்து;

	    // start of ":try" block
	    நிலை ISN_TRY:
		{
		    trycmd_T    *trycmd = வெற்று;

		    எனில் (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			செல் theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_data)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_POINTER(trycmd);
		    trycmd->tcd_frame_idx = ectx->ec_frame_idx;
		    trycmd->tcd_அடுக்கு_len = ectx->ec_அடுக்கு.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		நிறுத்து;

	    நிலை ISN_PUSHEXC:
		எனில் (current_exception == வெற்று)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    செல் theend;
		}
		எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
		    செல் theend;
		tv = அடுக்கு_TV_BOT(0);
		++ectx->ec_அடுக்கு.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->மதிப்பு);
		நிறுத்து;

	    நிலை ISN_CATCH:
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
		நிறுத்து;

	    நிலை ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    முழு		i;
		    trycmd_T    *trycmd;
		    முழு		iidx = trycont->tct_where;

		    எனில் (trystack->ga_len < trycont->tct_levels)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_levels, trystack->ga_len);
			செல் theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    க்கு (i = trycont->tct_levels; i > 0; --i)
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
		நிறுத்து;

	    நிலை ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    எனில் (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_அடுக்கு);
			trycmd->tcd_caught = FALSE;
		    }
		    நிறுத்து;
		}

	    // end of ":try" block
	    நிலை ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_data) + trystack->ga_len;
		    எனில் (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    எனில் (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_அடுக்கு);
		    }

		    எனில் (trycmd->tcd_return)
			செல் func_return;

		    போது (ectx->ec_அடுக்கு.ga_len > trycmd->tcd_அடுக்கு_len)
		    {
			--ectx->ec_அடுக்கு.ga_len;
			clear_tv(அடுக்கு_TV_BOT(0));
		    }
		    எனில் (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		நிறுத்து;

	    நிலை ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    எனில் (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = அடுக்கு_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_எண்;
			tv->vval.v_எண் = 0;
			செல் done;
		    }
		    --ectx->ec_அடுக்கு.ga_len;
		    tv = அடுக்கு_TV_BOT(0);
		    எனில் (tv->vval.v_string == வெற்று
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_empty_string));
			செல் theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    எனில் (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
			எனில் (trycmd->tcd_caught)
			{
			    // discard the exception
			    finish_exception(caught_அடுக்கு);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (throw_exception(tv->vval.v_string, ET_USER, வெற்று)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			செல் theend;
		    }
		    did_throw = TRUE;
		}
		நிறுத்து;

	    // compare with special values
	    நிலை ISN_COMPAREBOOL:
	    நிலை ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T	*tv2 = அடுக்கு_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_எண்;
		    varnumber_T arg2 = tv2->vval.v_எண்;
		    முழு		res;

		    எனில் (iptr->isn_arg.op.op_type == கோவை_EQUAL)
			res = arg1 == arg2;
		    இல்லையெனில்
			res = arg1 != arg2;

		    --ectx->ec_அடுக்கு.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_எண் = res ? VVAL_TRUE : VVAL_FALSE;
		}
		நிறுத்து;

	    நிலை ISN_COMPARENULL:
		{
		    typval_T	*tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T	*tv2 = அடுக்கு_TV_BOT(-1);
		    முழு		res;

		    res = typval_compare_null(tv1, tv2);
		    எனில் (res == MAYBE)
			செல் on_error;
		    எனில் (iptr->isn_arg.op.op_type == கோவை_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_அடுக்கு.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_எண் = res ? VVAL_TRUE : VVAL_FALSE;
		}
		நிறுத்து;

	    // Operation with two number arguments
	    நிலை ISN_OPNR:
	    நிலை ISN_COMPARENR:
		{
		    typval_T	*tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T	*tv2 = அடுக்கு_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_எண்;
		    varnumber_T arg2 = tv2->vval.v_எண்;
		    varnumber_T res = 0;
		    முழு		div_zero = FALSE;

		    எனில் (iptr->isn_arg.op.op_type == கோவை_LSHIFT
			    || iptr->isn_arg.op.op_type == கோவை_RSHIFT)
		    {
			எனில் (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_positive));
			    செல் on_error;
			}
		    }

		    தேர்வு (iptr->isn_arg.op.op_type)
		    {
			நிலை கோவை_MULT: res = arg1 * arg2; நிறுத்து;
			நிலை கோவை_DIV:  எனில் (arg2 == 0)
					    div_zero = TRUE;
					இல்லையெனில்
					    res = arg1 / arg2;
					நிறுத்து;
			நிலை கோவை_REM:  எனில் (arg2 == 0)
					    div_zero = TRUE;
					இல்லையெனில்
					    res = arg1 % arg2;
					நிறுத்து;
			நிலை கோவை_SUB: res = arg1 - arg2; நிறுத்து;
			நிலை கோவை_ADD: res = arg1 + arg2; நிறுத்து;

			நிலை கோவை_EQUAL: res = arg1 == arg2; நிறுத்து;
			நிலை கோவை_NEQUAL: res = arg1 != arg2; நிறுத்து;
			நிலை கோவை_GREATER: res = arg1 > arg2; நிறுத்து;
			நிலை கோவை_GEQUAL: res = arg1 >= arg2; நிறுத்து;
			நிலை கோவை_SMALLER: res = arg1 < arg2; நிறுத்து;
			நிலை கோவை_SEQUAL: res = arg1 <= arg2; நிறுத்து;
			நிலை கோவை_LSHIFT: எனில் (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  இல்லையெனில்
					      res = (uvarnumber_T)arg1 << arg2;
					  நிறுத்து;
			நிலை கோவை_RSHIFT: எனில் (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  இல்லையெனில்
					      res = (uvarnumber_T)arg1 >> arg2;
					  நிறுத்து;
			இயல்பு: நிறுத்து;
		    }

		    --ectx->ec_அடுக்கு.ga_len;
		    எனில் (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_எண் = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    இல்லையெனில்
			tv1->vval.v_எண் = res;
		    எனில் (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			செல் on_error;
		    }
		}
		நிறுத்து;

	    // Computation with two float arguments
	    நிலை ISN_OPFLOAT:
	    நிலை ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T	*tv2 = அடுக்கு_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    முழு		cmp = FALSE;

		    தேர்வு (iptr->isn_arg.op.op_type)
		    {
			நிலை கோவை_MULT: res = arg1 * arg2; நிறுத்து;
			நிலை கோவை_DIV: res = arg1 / arg2; நிறுத்து;
			நிலை கோவை_SUB: res = arg1 - arg2; நிறுத்து;
			நிலை கோவை_ADD: res = arg1 + arg2; நிறுத்து;

			நிலை கோவை_EQUAL: cmp = arg1 == arg2; நிறுத்து;
			நிலை கோவை_NEQUAL: cmp = arg1 != arg2; நிறுத்து;
			நிலை கோவை_GREATER: cmp = arg1 > arg2; நிறுத்து;
			நிலை கோவை_GEQUAL: cmp = arg1 >= arg2; நிறுத்து;
			நிலை கோவை_SMALLER: cmp = arg1 < arg2; நிறுத்து;
			நிலை கோவை_SEQUAL: cmp = arg1 <= arg2; நிறுத்து;
			இயல்பு: cmp = 0; நிறுத்து;
		    }
		    --ectx->ec_அடுக்கு.ga_len;
		    எனில் (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_எண் = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    இல்லையெனில்
			tv1->vval.v_float = res;
		}
		நிறுத்து;

	    நிலை ISN_COMPARELIST:
	    நிலை ISN_COMPARETUPLE:
	    நிலை ISN_COMPAREDICT:
	    நிலை ISN_COMPAREFUNC:
	    நிலை ISN_COMPARESTRING:
	    நிலை ISN_COMPAREBLOB:
	    நிலை ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T	*tv2 = அடுக்கு_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    முழு		ic = iptr->isn_arg.op.op_ic;
		    முழு		res = FALSE;
		    முழு		status = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (iptr->isn_type == ISN_COMPARELIST)
		    {
			status = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    இல்லையெனில் எனில் (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			status = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    இல்லையெனில் எனில் (iptr->isn_type == ISN_COMPAREDICT)
		    {
			status = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    இல்லையெனில் எனில் (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			status = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    இல்லையெனில் எனில் (iptr->isn_type == ISN_COMPARESTRING)
		    {
			status = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    இல்லையெனில் எனில் (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			status = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    இல்லையெனில் // ISN_COMPAREOBJECT
		    {
			status = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_அடுக்கு.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_எண் = res ? VVAL_TRUE : VVAL_FALSE;
		    எனில் (status == FAIL)
			செல் theend;
		}
		நிறுத்து;

	    நிலை ISN_COMPAREANY:
		{
		    typval_T	*tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T	*tv2 = அடுக்கு_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    முழு		ic = iptr->isn_arg.op.op_ic;
		    முழு		status;

		    SOURCING_LNUM = iptr->isn_lnum;
		    status = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_அடுக்கு.ga_len;
		    எனில் (status == FAIL)
			செல் theend;
		}
		நிறுத்து;

	    நிலை ISN_ADDLIST:
	    நிலை ISN_ADDTUPLE:
	    நிலை ISN_ADDBLOB:
		{
		    typval_T *tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T *tv2 = அடுக்கு_TV_BOT(-1);

		    // add two lists or blobs
		    எனில் (iptr->isn_type == ISN_ADDLIST)
		    {
			எனில் (iptr->isn_arg.op.op_type == கோவை_APPEND
						   && tv1->vval.v_list != வெற்று)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 வெற்று);
			இல்லையெனில்
			    eval_addlist(tv1, tv2);
		    }
		    இல்லையெனில் எனில் (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    இல்லையெனில்
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    நிலை ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T	*tv2 = அடுக்கு_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (l == வெற்று)
		    {
			emsg(_(e_cannot_add_to_null_list));
			செல் on_error;
		    }
		    எனில் (மதிப்பு_check_lock(l->lv_lock, வெற்று, FALSE))
			செல் on_error;
		    எனில் (list_append_tv(l, tv2) == FAIL)
			செல் theend;
		    clear_tv(tv2);
		    --ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    நிலை ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T	*tv2 = அடுக்கு_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    முழு		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    எனில் (b == வெற்று)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_add_to_null_blob));
			செல் on_error;
		    }
		    n = tv_get_எண்_chk(tv2, &error);
		    எனில் (error)
			செல் on_error;
		    ga_append(&b->bv_ga, (முழு)n);
		    --ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    // Computation with two arguments of unknown type
	    நிலை ISN_OPANY:
		{
		    typval_T	*tv1 = அடுக்கு_TV_BOT(-2);
		    typval_T	*tv2 = அடுக்கு_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    முழு		error = FALSE;

		    எனில் (iptr->isn_arg.op.op_type == கோவை_ADD)
		    {
			எனில் (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_அடுக்கு.ga_len;
			    நிறுத்து;
			}
			இல்லையெனில் எனில் (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_அடுக்கு.ga_len;
			    நிறுத்து;
			}
			இல்லையெனில் எனில் (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_அடுக்கு.ga_len;
			    நிறுத்து;
			}
		    }
		    எனில் (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    இல்லையெனில்
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_எண்_chk(tv1, &error);
			எனில் (error)
			    செல் on_error;
			எனில் (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    எனில் (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    இல்லையெனில்
		    {
			n2 = tv_get_எண்_chk(tv2, &error);
			எனில் (error)
			    செல் on_error;
			எனில் (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    எனில் (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			தேர்வு (iptr->isn_arg.op.op_type)
			{
			    நிலை கோவை_MULT: f1 = f1 * f2; நிறுத்து;
			    நிலை கோவை_DIV:  f1 = f1 / f2; நிறுத்து;
			    நிலை கோவை_SUB:  f1 = f1 - f2; நிறுத்து;
			    நிலை கோவை_ADD:  f1 = f1 + f2; நிறுத்து;
			    இயல்பு: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     செல் on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_அடுக்கு.ga_len;
		    }
		    இல்லையெனில்
		    {
			முழு failed = FALSE;

			தேர்வு (iptr->isn_arg.op.op_type)
			{
			    நிலை கோவை_MULT: n1 = n1 * n2; நிறுத்து;
			    நிலை கோவை_DIV:  n1 = இலக்கம்_divide(n1, n2, &failed);
					    எனில் (failed)
						செல் on_error;
					    நிறுத்து;
			    நிலை கோவை_SUB:  n1 = n1 - n2; நிறுத்து;
			    நிலை கோவை_ADD:  n1 = n1 + n2; நிறுத்து;
			    இயல்பு:	    n1 = இலக்கம்_modulus(n1, n2, &failed);
					    எனில் (failed)
						செல் on_error;
					    நிறுத்து;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_எண்;
			tv1->vval.v_எண் = n1;
			--ectx->ec_அடுக்கு.ga_len;
		    }
		}
		நிறுத்து;

	    நிலை ISN_STRINDEX:
	    நிலை ISN_STRSLICE:
		{
		    முழு		is_slice = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    எனில் (is_slice)
		    {
			tv = அடுக்கு_TV_BOT(-2);
			n1 = tv->vval.v_எண்;
		    }

		    tv = அடுக்கு_TV_BOT(-1);
		    n2 = tv->vval.v_எண்;

		    ectx->ec_அடுக்கு.ga_len -= is_slice ? 2 : 1;
		    tv = அடுக்கு_TV_BOT(-1);
		    எனில் (is_slice)
			// Slice: Select the characters from the string
			res = string_slice(tv->vval.v_string, n1, n2, FALSE);
		    இல்லையெனில்
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		நிறுத்து;

	    நிலை ISN_LISTINDEX:
	    நிலை ISN_LISTSLICE:
	    நிலை ISN_TUPLEINDEX:
	    நிலை ISN_TUPLESLICE:
	    நிலை ISN_BLOBINDEX:
	    நிலை ISN_BLOBSLICE:
		{
		    முழு		is_slice = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    முழு		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    முழு		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_slice ? அடுக்கு_TV_BOT(-3) : அடுக்கு_TV_BOT(-2);

		    tv = அடுக்கு_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_எண்;
		    clear_tv(tv);

		    எனில் (is_slice)
		    {
			tv = அடுக்கு_TV_BOT(-2);
			n1 = tv->vval.v_எண்;
			clear_tv(tv);
		    }

		    ectx->ec_அடுக்கு.ga_len -= is_slice ? 2 : 1;
		    tv = அடுக்கு_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (is_blob)
		    {
			எனில் (blob_slice_or_index(val_tv->vval.v_blob, is_slice,
						    n1, n2, FALSE, tv) == FAIL)
			    செல் on_error;
		    }
		    இல்லையெனில் எனில் (is_tuple)
		    {
			எனில் (tuple_slice_or_index(val_tv->vval.v_tuple,
				is_slice, n1, n2, FALSE, tv, TRUE) == FAIL)
			    செல் on_error;
		    }
		    இல்லையெனில்
		    {
			எனில் (list_slice_or_index(val_tv->vval.v_list, is_slice,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    செல் on_error;
		    }
		}
		நிறுத்து;

	    நிலை ISN_ANYINDEX:
	    நிலை ISN_ANYSLICE:
		{
		    முழு		is_slice = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    முழு		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_slice ? அடுக்கு_TV_BOT(-3) : அடுக்கு_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (check_can_index(tv, TRUE, TRUE) == FAIL)
			செல் on_error;
		    var1 = is_slice ? அடுக்கு_TV_BOT(-2) : அடுக்கு_TV_BOT(-1);
		    var2 = is_slice ? அடுக்கு_TV_BOT(-1) : வெற்று;
		    res = eval_index_inner(tv, is_slice, var1, var2,
							FALSE, வெற்று, -1, TRUE);
		    clear_tv(var1);
		    எனில் (is_slice)
			clear_tv(var2);
		    ectx->ec_அடுக்கு.ga_len -= is_slice ? 2 : 1;
		    எனில் (res == FAIL)
			செல் on_error;
		}
		நிறுத்து;

	    நிலை ISN_SLICE:
		{
		    முழு		மொத்தம் = iptr->isn_arg.எண்;

		    // type will have been checked to be a list
		    tv = அடுக்கு_TV_BOT(-1);
		    எனில் (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			எனில் (list != வெற்று && list->lv_len >= மொத்தம்)
			{
			    list_T	*newlist = list_slice(list,
				    மொத்தம், list->lv_len - 1);

			    எனில் (newlist != வெற்று)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    இல்லையெனில்
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			எனில் (tuple != வெற்று && TUPLE_LEN(tuple) >= மொத்தம்)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_slice(tuple, மொத்தம்,
							TUPLE_LEN(tuple) - 1);
			    எனில் (newtuple != வெற்று)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		நிறுத்து;

	    நிலை ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*item_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = அடுக்கு_TV_BOT(-1 - gi->gi_with_op);
		    எனில் (tv->v_type == VAR_LIST)
		    {
			li = list_find(tv->vval.v_list, gi->gi_index);
			எனில் (li == வெற்று)
			    செல் on_error;
			item_tv = &li->li_tv;
		    }
		    இல்லையெனில்
			item_tv = TUPLE_ITEM(tv->vval.v_tuple, gi->gi_index);

		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    ++ectx->ec_அடுக்கு.ga_len;
		    copy_tv(item_tv, அடுக்கு_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_index = gi->gi_index + 1;
		    ectx->ec_where.wt_kind = WT_VARIABLE;
		}
		நிறுத்து;

	    // dict member with string key (dict['member'])
	    நிலை ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*சாவி;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = அடுக்கு_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = அடுக்கு_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    சாவி = tv->vval.v_string;
		    எனில் (சாவி == வெற்று)
			சாவி = (char_u *)"";

		    எனில் ((di = dict_find(dict, சாவி, -1)) == வெற்று)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_சாவி_not_present_in_dictionary_str), சாவி);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_அடுக்கு.ga_len;
			tv = அடுக்கு_TV_BOT(-1);
			(வெறுமை) dict_அடுக்கு_save(tv);
			tv->v_type = VAR_எண்;
			tv->vval.v_எண் = 0;
			செல் on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_அடுக்கு.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = அடுக்கு_TV_BOT(-1);
		    எனில் (dict_அடுக்கு_save(tv) == FAIL)
			செல் on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		நிறுத்து;

	    // dict member with string key (dict.member)
	    // or can be an object
	    நிலை ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = அடுக்கு_TV_BOT(-1);

		    எனில் (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			எனில் (dict_அடுக்கு_save(tv) == FAIL)
			    செல் on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			எனில் (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    செல் on_error;
		    }
		    இல்லையெனில்
		    {
			எனில் (tv->v_type != VAR_DICT || tv->vval.v_dict == வெற்று)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    செல் on_error;
			}
			dict = tv->vval.v_dict;

			எனில் ((di = dict_find(dict, iptr->isn_arg.string, -1))
								   == வெற்று)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_சாவி_not_present_in_dictionary_str),
						     iptr->isn_arg.string);
			    செல் on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			எனில் (dict_அடுக்கு_save(tv) == FAIL)
			    செல் on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		நிறுத்து;

	    நிலை ISN_GET_OBJ_MEMBER:
	    நிலை ISN_GET_ITF_MEMBER:
		{
		    tv = அடுக்கு_TV_BOT(-1);
		    எனில் (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			செல் on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    எனில் (obj == வெற்று)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			செல் on_error;
		    }

		    முழு idx;
		    எனில் (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    இல்லையெனில்
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_index_from_itf_index(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    எனில் (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			செல் on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		நிறுத்து;

	    நிலை ISN_STORE_THIS:
		{
		    முழு idx = iptr->isn_arg.எண்;
		    object_T *obj = அடுக்கு_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *அடுக்கு_TV_BOT(-1);
		    --ectx->ec_அடுக்கு.ga_len;
		}
		நிறுத்து;

	    நிலை ISN_CLEARDICT:
		dict_அடுக்கு_drop();
		நிறுத்து;

	    நிலை ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_அடுக்கு_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = அடுக்கு_TV_BOT(-1);
		    எனில் (dict_tv != வெற்று
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != வெற்று
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == வெற்று))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_அடுக்கு_drop();
		}
		நிறுத்து;

	    நிலை ISN_NEGATENR:
		tv = அடுக்கு_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		எனில் (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		இல்லையெனில்
		    tv->vval.v_எண் = -tv->vval.v_எண்;
		நிறுத்து;

	    நிலை ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    முழு		r;
		    where_T	where = WHERE_INIT;

		    tv = அடுக்கு_TV_BOT((முழு)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    எனில் (ct->ct_arg_idx > 0)
		    {
			where.wt_index = ct->ct_arg_idx;
			where.wt_kind = ct->ct_is_var ? WT_VARIABLE : WT_ARGUMENT;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    எனில் (r == FAIL)
			செல் on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    எனில் (tv->v_type == VAR_எண்
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_எண் == 0
						|| tv->vval.v_எண் == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_எண் = tv->vval.v_எண்
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		நிறுத்து;

	    நிலை ISN_CHECKLEN:
		{
		    முழு	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = வெற்று;
		    tuple_T *tuple = வெற்று;
		    முழு	    len = 0;

		    tv = அடுக்கு_TV_BOT(-1);

		    முழு		len_check_failed = FALSE;
		    எனில் (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			எனில் (list == வெற்று || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			எனில் (list != வெற்று)
			    len = list->lv_len;
		    }
		    இல்லையெனில் எனில் (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			எனில் (tuple == வெற்று || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			எனில் (tuple != வெற்று)
			    len = TUPLE_LEN(tuple);
		    }
		    இல்லையெனில்
			len_check_failed = TRUE;

		    எனில் (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_பொருட்கள்_but_got_nr),
				     min_len, len);
			செல் on_error;
		    }
		}
		நிறுத்து;

	    நிலை ISN_SETTYPE:
		set_tv_type(அடுக்கு_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		நிறுத்து;

	    நிலை ISN_2BOOL:
	    நிலை ISN_COND2BOOL:
		{
		    முழு n;
		    முழு error = FALSE;

		    எனில் (iptr->isn_type == ISN_2BOOL)
		    {
			tv = அடுக்கு_TV_BOT(iptr->isn_arg.tobool.offset);
			n = tv2bool(tv);
			எனில் (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    இல்லையெனில்
		    {
			tv = அடுக்கு_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			எனில் (error)
			    செல் on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_எண் = n ? VVAL_TRUE : VVAL_FALSE;
		}
		நிறுத்து;

	    நிலை ISN_2STRING:
	    நிலை ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		எனில் (do_2string(அடுக்கு_TV_BOT(iptr->isn_arg.tostring.offset),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    செல் on_error;
		நிறுத்து;

	    நிலை ISN_RANGE:
		{
		    exarg_T	ea;
		    எழுத்து	*errormsg;

		    ea.line2 = 0;
		    ea.addr_மொத்தம் = 0;
		    ea.addr_type = ADDR_வரிகள்;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    எனில் (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
			செல் on_error;

		    எனில் (GA_GROW_FAILS(&ectx->ec_அடுக்கு, 1))
			செல் theend;
		    ++ectx->ec_அடுக்கு.ga_len;
		    tv = அடுக்கு_TV_BOT(-1);
		    tv->v_type = VAR_எண்;
		    tv->v_lock = 0;
		    tv->vval.v_எண் = ea.line2;
		}
		நிறுத்து;

	    நிலை ISN_PUT:
		tv = வெற்று; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		நிறுத்து;
	    நிலை ISN_IPUT:
		tv = வெற்று;
		isn_put_do(ectx, iptr, tv, TRUE);
		நிறுத்து;

	    நிலை ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_அடுக்கு.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		நிறுத்து;

	    நிலை ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = வெற்று;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		நிறுத்து;

	    நிலை ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = அடுக்கு_TV_BOT(-1);
		எனில் (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_argument_must_be_sequence_of_lists_or_tuples));
		    செல் on_error;
		}

		முழு rc;
		எனில் (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		இல்லையெனில்
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		எனில் (rc != EXEC_OK)
		{
		    எனில் (rc == EXEC_FAIL)
			செல் on_error;
		    செல் theend;
		}
		நிறுத்து;

	    நிலை ISN_PROF_START:
	    நிலை ISN_PROF_END:
		{
#ifdef FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    எனில் (iptr->isn_type == ISN_PROF_START)
		    {
			func_வரி_start(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_வரி_exec(&cookie);
		    }
		    இல்லையெனில்
			func_வரி_end(&cookie);
#endif
		}
		நிறுத்து;

	    நிலை ISN_DEBUG:
		handle_debug(iptr, ectx);
		நிறுத்து;

	    நிலை ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    முழு		item = iptr->isn_arg.shuffle.shfl_item;
		    முழு		up = iptr->isn_arg.shuffle.shfl_up;

		    tmp_tv = *அடுக்கு_TV_BOT(-item);
		    க்கு ( ; up > 0 && item > 1; --up)
		    {
			*அடுக்கு_TV_BOT(-item) = *அடுக்கு_TV_BOT(-item + 1);
			--item;
		    }
		    *அடுக்கு_TV_BOT(-item) = tmp_tv;
		}
		நிறுத்து;

	    நிலை ISN_DROP:
		--ectx->ec_அடுக்கு.ga_len;
		clear_tv(அடுக்கு_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		நிறுத்து;

	    நிலை ISN_SCRIPTCTX_SET:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		நிறுத்து;
	}
	தொடர்;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	எனில் (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
	    செல் done;

	எனில் (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    செல் theend;
	தொடர்;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	எனில் (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    எனில் (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		போது (ectx->ec_அடுக்கு.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_அடுக்கு.ga_len;
		    clear_tv(அடுக்கு_TV_BOT(0));
		}
		போது (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    தொடர்;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	எனில் (trylevel <= ectx->ec_trylevel_at_start)
	    செல் theend;
    }

done:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_அடுக்கு_clear(dict_அடுக்கு_len_at_start);
    ectx->ec_trylevel_at_start = save_trylevel_at_start;
    திரும்பு ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    முழு
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    முழு		save_iidx = ectx->ec_iidx;
    முழு		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    எனில் (res == OK)
    {
	*rettv = *அடுக்கு_TV_BOT(-1);
	--ectx->ec_அடுக்கு.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    திரும்பு res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(வெறுமை)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    முழு		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    எனில் (exec_instructions(ectx) == OK)
    {
	typval_T *tv = அடுக்கு_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_அடுக்கு.ga_len;
	clear_tv(tv);
    }
    இல்லையெனில்
    {
	substitute_instr->subs_status = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    திரும்பு res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    முழு
call_def_function(
    ufunc_T	*ufunc,
    முழு		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    முழு		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    முழு		argc = argc_arg;
    முழு		partial_argc = partial == வெற்று
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    முழு		total_argc = argc + partial_argc;
    typval_T	*tv;
    முழு		idx;
    முழு		ret = FAIL;
    முழு		defcount = ufunc->uf_args.ga_len - total_argc;
    sctx_T	save_current_sctx = current_sctx;
    முழு		did_emsg_before = did_emsg_cumul + did_emsg;
    முழு		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_செய்தி_list = வெற்று;
    msglist_T	*private_செய்தி_list = வெற்று;
    முழு		save_emsg_silent_def = emsg_silent_def;
    முழு		save_did_emsg_def = did_emsg_def;
    முழு		orig_funcdepth;
    முழு		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#undef அடுக்கு_TV
#define அடுக்கு_TV(idx) (((typval_T *)ectx.ec_அடுக்கு.ga_data) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#undef அடுக்கு_TV_BOT
#define அடுக்கு_TV_BOT(idx) (((typval_T *)ectx.ec_அடுக்கு.ga_data) + ectx.ec_அடுக்கு.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#undef அடுக்கு_TV_VAR
#define அடுக்கு_TV_VAR(idx) (((typval_T *)ectx.ec_அடுக்கு.ga_data) + ectx.ec_frame_idx + அடுக்கு_FRAME_SIZE + idx)

    எனில் (ufunc->uf_def_status == UF_NOT_COMPILED
	    || ufunc->uf_def_status == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), வெற்று) == FAIL))
    {
	எனில் (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	திரும்பு FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	எனில் (dfunc->df_ufunc == வெற்று)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    திரும்பு FAIL;
	}
	எனில் (INSTRUCTIONS(dfunc) == வெற்று)
	{
	    iemsg("using call_def_function() on not compiled function");
	    திரும்பு FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    எனில் (funcdepth_increment() == FAIL)
	திரும்பு FAIL;

    CLEAR_FIELD(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_அடுக்கு, அளவு(typval_T), 500);
    எனில் (GA_GROW_FAILS(&ectx.ec_அடுக்கு, 20))
    {
	funcdepth_decrement();
	திரும்பு FAIL;
    }
    ga_init2(&ectx.ec_trystack, அளவு(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, அளவு(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = total_argc - ufunc->uf_args.ga_len;
    எனில் (idx > 0 && ufunc->uf_va_name == வெற்று)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
								    idx), idx);
	செல் failed_early;
    }
    idx = total_argc - ufunc->uf_args.ga_len + ufunc->uf_def_args.ga_len;
    எனில் (idx < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	செல் failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    க்கு (idx = 0; idx < total_argc
	    && (ufunc->uf_va_name != வெற்று || idx < ufunc->uf_args.ga_len);
									 ++idx)
    {
	முழு argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	எனில் (idx >= ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_எண் == VVAL_NONE)
	{
	    // Use the default value.
	    அடுக்கு_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	இல்லையெனில்
	{
	    முழு done = FALSE;
	    எனில் (ufunc->uf_arg_types != வெற்று && idx < ufunc->uf_args.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		எனில் (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_எண்)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    அடுக்கு_TV_BOT(0)->v_type = VAR_FLOAT;
		    அடுக்கு_TV_BOT(0)->v_lock = 0;
		    அடுக்கு_TV_BOT(0)->vval.v_float = tv->vval.v_எண்;
		    done = TRUE;
		}
		இல்லையெனில் எனில் (check_typval_arg_type(expected, tv,
						   வெற்று, argv_idx + 1) == FAIL)
		    செல் failed_early;
	    }
	    எனில் (!done)
		copy_tv(tv, அடுக்கு_TV_BOT(0));
	}
	++ectx.ec_அடுக்கு.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    எனில் (ufunc->uf_va_name != வெற்று)
    {
	முழு vararg_மொத்தம் = argc - ufunc->uf_args.ga_len;

	எனில் (vararg_மொத்தம் < 0)
	    vararg_மொத்தம் = 0;
	இல்லையெனில்
	    argc -= vararg_மொத்தம்;
	எனில் (exe_newlist(vararg_மொத்தம், &ectx) == FAIL)
	    செல் failed_early;

	// Check the type of the list items.
	tv = அடுக்கு_TV_BOT(-1);
	எனில் (ufunc->uf_va_type != வெற்று
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != வெற்று)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_first;

	    க்கு (idx = 0; idx < vararg_மொத்தம்; ++idx)
	    {
		எனில் (check_typval_arg_type(expected, &li->li_tv,
						 வெற்று, argc + idx + 1) == FAIL)
		    செல் failed_early;
		li = li->li_next;
	    }
	}

	எனில் (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *அடுக்கு_TV_BOT(defcount - 1) = *அடுக்கு_TV_BOT(-1);
	--ectx.ec_அடுக்கு.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    எனில் (defcount > 0)
	க்கு (idx = 0; idx < defcount; ++idx)
	{
	    அடுக்கு_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_அடுக்கு.ga_len;
	}
    எனில் (ufunc->uf_va_name != வெற்று)
	++ectx.ec_அடுக்கு.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_frame_idx = ectx.ec_அடுக்கு.ga_len;
    ectx.ec_initial_frame_idx = ectx.ec_frame_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *base_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	எனில் (partial != வெற்று || base_ufunc->uf_partial != வெற்று)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    எனில் (ectx.ec_outer_ref == வெற்று)
		செல் failed_early;
	    எனில் (partial != வெற்று)
	    {
		outer_T *outer = get_pt_outer(partial);

		எனில் (outer->out_அடுக்கு == வெற்று && outer->out_loop_size == 0)
		{
		    // no stack was set
		    எனில் (current_ectx != வெற்று)
		    {
			எனில் (current_ectx->ec_outer_ref != வெற்று
			       && current_ectx->ec_outer_ref->or_outer != வெற்று)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		இல்லையெனில்
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    இல்லையெனில்
	    {
		ectx.ec_outer_ref->or_outer = &base_ufunc->uf_partial->pt_outer;
		++base_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = base_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    க்கு (idx = 0; idx < அடுக்கு_FRAME_SIZE; ++idx)
    {
	அடுக்கு_TV(ectx.ec_அடுக்கு.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_அடுக்கு.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	க்கு (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    அடுக்கு_TV_VAR(idx)->v_type = VAR_எண்;
	    அடுக்கு_TV_VAR(idx)->vval.v_எண் = 0;
	}
	ectx.ec_அடுக்கு.ga_len += dfunc->df_varcount;

	எனில் (object != வெற்று)
	{
	    // the object is always the variable at index zero
	    tv = அடுக்கு_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	எனில் (dfunc->df_has_closure)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    அடுக்கு_TV_VAR(idx)->v_type = VAR_எண்;
	    அடுக்கு_TV_VAR(idx)->vval.v_எண் = 0;
	    ++ectx.ec_அடுக்கு.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    எனில் (funccal != வெற்று)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_தள்ளு_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_செய்தி_list = செய்தி_list;
    செய்தி_list = &private_செய்தி_list;

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
    எனில் (ret == OK)
    {
	// function finished, get result from the stack.
	எனில் (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	இல்லையெனில்
	{
	    tv = அடுக்கு_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    எனில் (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_closure_in_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_எடு();
    current_sctx = save_current_sctx;

    எனில் (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    எனில் (*செய்தி_list != வெற்று && saved_செய்தி_list != வெற்று)
    {
	msglist_T **plist = saved_செய்தி_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	போது (*plist != வெற்று)
	    plist = &(*plist)->next;

	*plist = *செய்தி_list;
    }
    செய்தி_list = saved_செய்தி_list;

    எனில் (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = வெற்று;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    க்கு (idx = 0; idx < ectx.ec_அடுக்கு.ga_len; ++idx)
    {
	tv = அடுக்கு_TV(idx);
	எனில் (tv->v_type != VAR_எண் && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_அடுக்கு.ga_data);
    vim_free(ectx.ec_trystack.ga_data);
    எனில் (ectx.ec_outer_ref != வெற்று)
    {
	எனில் (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    எனில் (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    திரும்பு ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    வெறுமை
unwind_def_callstack(ectx_T *ectx)
{
    போது (ectx->ec_frame_idx != ectx->ec_initial_frame_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    வெறுமை
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    எனில் (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    நிலையான char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    முழு		depth;

    ga_init2(&ga, 1, 100);
    க்கு (depth = 0; depth < lvi->lvi_depth; ++depth)
    {
	எனில் (ga_grow(&ga, 50) == FAIL)
	    நிறுத்து;
	எனில் (lvi->lvi_loop[depth].var_idx == 0)
	    STRCPY((எழுத்து *)ga.ga_data + ga.ga_len, " -");
	இல்லையெனில்
	    vim_snprintf((எழுத்து *)ga.ga_data + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[depth].var_idx,
			    lvi->lvi_loop[depth].var_idx
					 + lvi->lvi_loop[depth].var_மொத்தம் - 1);
	ga.ga_len = (முழு)STRLEN(ga.ga_data);
    }
    திரும்பு ga.ga_data;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    நிலையான வெறுமை
list_instructions(எழுத்து *pfx, isn_T *instr, முழு instr_மொத்தம், ufunc_T *ufunc)
{
    முழு		வரி_idx = 0;
    முழு		prev_current = 0;
    முழு		current;
    முழு		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    க்கு (current = 0; current < instr_மொத்தம்; ++current)
    {
	isn_T	    *iptr = &instr[current];
	எழுத்து	    *வரி;

	எனில் (ufunc != வெற்று)
	{
	    போது (வரி_idx < iptr->isn_lnum
					  && வரி_idx < ufunc->uf_வரிகள்.ga_len)
	    {
		எனில் (current > prev_current)
		{
		    செய்தி_puts("\n\n");
		    prev_current = current;
		}
		வரி = ((எழுத்து **)ufunc->uf_வரிகள்.ga_data)[வரி_idx++];
		எனில் (வரி != வெற்று)
		    செய்தி(வரி);
	    }
	    எனில் (iptr->isn_type == ISN_JUMP_IF_ARG_SET)
	    {
		முழு	first_def_arg = ufunc->uf_args.ga_len
						   - ufunc->uf_def_args.ga_len;

		எனில் (def_arg_idx > 0)
		    செய்தி_puts("\n\n");
		செய்தி_start();
		செய்தி_puts("  ");
		செய்தி_puts(((எழுத்து **)(ufunc->uf_args.ga_data))[
						 first_def_arg + def_arg_idx]);
		செய்தி_puts(" = ");
		செய்தி_puts(((எழுத்து **)(ufunc->uf_def_args.ga_data))[def_arg_idx++]);
		செய்தி_clr_eos();
		செய்தி_end();
	    }
	}

	தேர்வு (iptr->isn_type)
	{
	    நிலை ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (முழு)iptr->isn_arg.construct.construct_size);
		நிறுத்து;
	    நிலை ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_EXEC_SPLIT:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		நிறுத்து;
	    நிலை ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.எண் ? " append" : "");
		நிறுத்து;
	    நிலை ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.எண்));
#endif
		நிறுத்து;
	    நிலை ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#endif
		நிறுத்து;
	    நிலை ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, INT_MAX, வெற்று);
		செய்தி("     -------------");
		நிறுத்து;
	    நிலை ISN_SOURCE:
		{
		    scriptitem_T    *si = SCRIPT_ITEM(iptr->isn_arg.எண்);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		நிறுத்து;
	    நிலை ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, INT_MAX, வெற்று);
		    செய்தி("     -------------");
		}
		நிறுத்து;
	    நிலை ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.எண்);
		நிறுத்து;
	    நிலை ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_மொத்தம்);
		}
		நிறுத்து;
	    நிலை ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_ECHOWINDOW:
		எனில் (iptr->isn_arg.echowin.ewin_time > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_மொத்தம்,
				      iptr->isn_arg.echowin.ewin_time);
		இல்லையெனில்
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_மொத்தம்);
		நிறுத்து;
	    நிலை ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_LOAD:
		{
		    எனில் (iptr->isn_arg.எண் < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.எண்
							  + அடுக்கு_FRAME_SIZE));
		    இல்லையெனில்
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.எண்));
		}
		நிறுத்து;
	    நிலை ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    எனில் (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_depth,
					outer->outer_idx + அடுக்கு_FRAME_SIZE);
		    இல்லையெனில் எனில் (outer->outer_depth < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_depth);
		    இல்லையெனில்
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_depth,
					      outer->outer_idx);
		}
		நிறுத்து;
	    நிலை ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    எனில் (sv == வெற்று)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    இல்லையெனில்
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		நிறுத்து;
	    நிலை ISN_LOADS:
	    நிலை ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		நிறுத்து;
	    நிலை ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		நிறுத்து;
	    நிலை ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		நிறுத்து;
	    நிலை ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		நிறுத்து;
	    நிலை ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		நிறுத்து;
	    நிலை ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (முழு)(iptr->isn_arg.எண்));
		நிறுத்து;

	    நிலை ISN_STORE:
		எனில் (iptr->isn_arg.எண் < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.எண் + அடுக்கு_FRAME_SIZE);
		இல்லையெனில்
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.எண்);
		நிறுத்து;
	    நிலை ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    எனில் (outer->outer_depth < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_depth);
		    இல்லையெனில்
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_depth, outer->outer_idx);
		}
		நிறுத்து;
	    நிலை ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_STORES:
	    நிலை ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		நிறுத்து;
	    நிலை ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    எனில் (sv == வெற்று)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    இல்லையெனில்
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		நிறுத்து;
	    நிலை ISN_STOREOPT:
	    நிலை ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		நிறுத்து;
	    நிலை ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (முழு)iptr->isn_arg.எண்);
		நிறுத்து;
	    நிலை ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		நிறுத்து;

	    நிலை ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		நிறுத்து;

	    நிலை ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		நிறுத்து;

	    நிலை ISN_LOAD_CLASSMEMBER:
	    நிலை ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    முழு	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_LOAD_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		நிறுத்து;

	    // constants
	    நிலை ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_PUSHBOOL:
	    நிலை ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		நிறுத்து;
	    நிலை ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		நிறுத்து;
	    நிலை ISN_PUSHFUNC:
		{
		    எழுத்து *name = (எழுத்து *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == வெற்று ? "[none]" : name);
		}
		நிறுத்து;
	    நிலை ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#endif
		நிறுத்து;
	    நிலை ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#endif
		நிறுத்து;
	    நிலை ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		நிறுத்து;
	    நிலை ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == வெற்று ? "null"
				 : (எழுத்து *)iptr->isn_arg.classarg->class_name.string);
		நிறுத்து;
	    நிலை ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		நிறுத்து;
	    நிலை ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		நிறுத்து;
	    நிலை ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		நிறுத்து;
	    நிலை ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		நிறுத்து;
	    நிலை ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		நிறுத்து;
	    நிலை ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		நிறுத்து;
	    நிலை ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		நிறுத்து;
	    நிலை ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		நிறுத்து;

	    // function call
	    நிலை ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    internal_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		நிறுத்து;
	    நிலை ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		நிறுத்து;
	    நிலை ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != வெற்று
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		நிறுத்து;
	    நிலை ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		நிறுத்து;
	    நிலை ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_உச்சி ? " top" : "", cpfunc->cpf_argcount);
		}
		நிறுத்து;
	    நிலை ISN_PCALL_END:
		smsg("%s%4d PCALL end", pfx, current);
		நிறுத்து;
	    நிலை ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (முழு)iptr->isn_arg.defer.defer_argcount);
		நிறுத்து;
	    நிலை ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		நிறுத்து;
	    நிலை ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		நிறுத்து;
	    நிலை ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		நிறுத்து;
	    நிலை ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    எனில் (extra == வெற்று || extra->fre_func_name == வெற்று)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    இல்லையெனில்
			name = extra->fre_func_name;
		    எனில் (extra != வெற்று && extra->fre_class != வெற்று)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    இல்லையெனில் எனில் (extra == வெற்று
				     || extra->fre_loopvar_info.lvi_depth == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    இல்லையெனில்
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		நிறுத்து;

	    நிலை ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    எனில் (arg->nfa_loopvar_info.lvi_depth == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    இல்லையெனில்
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		நிறுத்து;

	    நிலை ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == வெற்று ? (char_u *)"" : name);
		}
		நிறுத்து;

	    நிலை ISN_JUMP:
		{
		    எழுத்து *when = "?";

		    தேர்வு (iptr->isn_arg.jump.jump_when)
		    {
			நிலை JUMP_ALWAYS:
			    when = "JUMP";
			    நிறுத்து;
			நிலை JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    நிறுத்து;
			நிலை JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    நிறுத்து;
			நிலை JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    நிறுத்து;
			நிலை JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  // unused
			    நிறுத்து;
			நிலை JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    நிறுத்து;
			நிலை JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    நிறுத்து;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		நிறுத்து;

	    நிலை ISN_JUMP_IF_ARG_SET:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + அடுக்கு_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		நிறுத்து;

	    நிலை ISN_JUMP_IF_ARG_NOT_SET:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + அடுக்கு_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		நிறுத்து;

	    நிலை ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_end);
		}
		நிறுத்து;

	    நிலை ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->end_funcref_idx,
			    endloop->end_var_idx,
			    endloop->end_var_idx + endloop->end_var_மொத்தம் - 1,
			    endloop->end_depth);
		}
		நிறுத்து;

	    நிலை ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_end);
		}
		நிறுத்து;

	    நிலை ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    எனில் (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    இல்லையெனில்
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		நிறுத்து;
	    நிலை ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		நிறுத்து;
	    நிலை ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_levels,
				      trycont->tct_levels == 1 ? "" : "s",
				      trycont->tct_where);
		}
		நிறுத்து;
	    நிலை ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		நிறுத்து;
	    நிலை ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		நிறுத்து;
	    நிலை ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		நிறுத்து;

	    // expression operations on number
	    நிலை ISN_OPNR:
	    நிலை ISN_OPFLOAT:
	    நிலை ISN_OPANY:
		{
		    எழுத்து *what;
		    எழுத்து *ins;

		    தேர்வு (iptr->isn_arg.op.op_type)
		    {
			நிலை கோவை_MULT: what = "*"; நிறுத்து;
			நிலை கோவை_DIV: what = "/"; நிறுத்து;
			நிலை கோவை_REM: what = "%"; நிறுத்து;
			நிலை கோவை_SUB: what = "-"; நிறுத்து;
			நிலை கோவை_ADD: what = "+"; நிறுத்து;
			நிலை கோவை_LSHIFT: what = "<<"; நிறுத்து;
			நிலை கோவை_RSHIFT: what = ">>"; நிறுத்து;
			இயல்பு:       what = "???"; நிறுத்து;
		    }
		    தேர்வு (iptr->isn_type)
		    {
			நிலை ISN_OPNR: ins = "OPNR"; நிறுத்து;
			நிலை ISN_OPFLOAT: ins = "OPFLOAT"; நிறுத்து;
			நிலை ISN_OPANY: ins = "OPANY"; நிறுத்து;
			இயல்பு: ins = "???"; நிறுத்து;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		நிறுத்து;

	    நிலை ISN_COMPAREBOOL:
	    நிலை ISN_COMPARESPECIAL:
	    நிலை ISN_COMPARENULL:
	    நிலை ISN_COMPARENR:
	    நிலை ISN_COMPAREFLOAT:
	    நிலை ISN_COMPARESTRING:
	    நிலை ISN_COMPAREBLOB:
	    நிலை ISN_COMPARELIST:
	    நிலை ISN_COMPARETUPLE:
	    நிலை ISN_COMPAREDICT:
	    நிலை ISN_COMPAREFUNC:
	    நிலை ISN_COMPAREOBJECT:
	    நிலை ISN_COMPAREANY:
		   {
		       எழுத்து *p;
		       எழுத்து buf[10];
		       எழுத்து *type;

		       தேர்வு (iptr->isn_arg.op.op_type)
		       {
			   நிலை கோவை_EQUAL:	 p = "=="; நிறுத்து;
			   நிலை கோவை_NEQUAL:    p = "!="; நிறுத்து;
			   நிலை கோவை_GREATER:   p = ">"; நிறுத்து;
			   நிலை கோவை_GEQUAL:    p = ">="; நிறுத்து;
			   நிலை கோவை_SMALLER:   p = "<"; நிறுத்து;
			   நிலை கோவை_SEQUAL:    p = "<="; நிறுத்து;
			   நிலை கோவை_MATCH:	 p = "=~"; நிறுத்து;
			   நிலை கோவை_IS:	 p = "is"; நிறுத்து;
			   நிலை கோவை_ISNOT:	 p = "isnot"; நிறுத்து;
			   நிலை கோவை_NOMATCH:	 p = "!~"; நிறுத்து;
			   இயல்பு:  p = "???"; நிறுத்து;
		       }
		       STRCPY(buf, p);
		       எனில் (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       தேர்வு(iptr->isn_type)
		       {
			   நிலை ISN_COMPAREBOOL: type = "COMPAREBOOL"; நிறுத்து;
			   நிலை ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; நிறுத்து;
			   நிலை ISN_COMPARENULL: type = "COMPARENULL"; நிறுத்து;
			   நிலை ISN_COMPARENR: type = "COMPARENR"; நிறுத்து;
			   நிலை ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; நிறுத்து;
			   நிலை ISN_COMPARESTRING:
						  type = "COMPARESTRING"; நிறுத்து;
			   நிலை ISN_COMPAREBLOB: type = "COMPAREBLOB"; நிறுத்து;
			   நிலை ISN_COMPARELIST: type = "COMPARELIST"; நிறுத்து;
			   நிலை ISN_COMPARETUPLE: type = "COMPARETUPLE"; நிறுத்து;
			   நிலை ISN_COMPAREDICT: type = "COMPAREDICT"; நிறுத்து;
			   நிலை ISN_COMPAREFUNC: type = "COMPAREFUNC"; நிறுத்து;
			   நிலை ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; நிறுத்து;
			   நிலை ISN_COMPAREANY: type = "COMPAREANY"; நிறுத்து;
			   இயல்பு: type = "???"; நிறுத்து;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   நிறுத்து;

	    நிலை ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); நிறுத்து;
	    நிலை ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); நிறுத்து;
	    நிலை ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); நிறுத்து;

	    // expression operations
	    நிலை ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.எண்));
		நிறுத்து;
	    நிலை ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); நிறுத்து;
	    நிலை ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); நிறுத்து;
	    நிலை ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); நிறுத்து;
	    நிலை ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); நிறுத்து;
	    நிலை ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); நிறுத்து;
	    நிலை ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); நிறுத்து;
	    நிலை ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); நிறுத்து;
	    நிலை ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); நிறுத்து;
	    நிலை ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); நிறுத்து;
	    நிலை ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); நிறுத்து;
	    நிலை ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); நிறுத்து;
	    நிலை ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); நிறுத்து;
	    நிலை ISN_SLICE: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.எண்); நிறுத்து;
	    நிலை ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_index,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); நிறுத்து;
	    நிலை ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); நிறுத்து;
	    நிலை ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); நிறுத்து;
	    நிலை ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (முழு)iptr->isn_arg.classmember.cm_idx);
				     நிறுத்து;
	    நிலை ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (முழு)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     நிறுத்து;
	    நிலை ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (முழு)iptr->isn_arg.எண்); நிறுத்து;
	    நிலை ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); நிறுத்து;
	    நிலை ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); நிறுத்து;

	    நிலை ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); நிறுத்து;

	    நிலை ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      எழுத்து	    *tofree = வெற்று;
		      எழுத்து	    *typename;

		      எனில் (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_எண்_OK))
			  typename = "float|number";
		      இல்லையெனில் எனில் (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      இல்லையெனில்
			  typename = type_name(ct->ct_type, &tofree);

		      எனில் (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (முழு)ct->ct_off);
		      இல்லையெனில்
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (முழு)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (முழு)ct->ct_arg_idx);
		      vim_free(tofree);
		      நிறுத்து;
		  }
	    நிலை ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_more_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       நிறுத்து;
	    நிலை ISN_SETTYPE:
		  {
		      எழுத்து *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      நிறுத்து;
		  }
	    நிலை ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); நிறுத்து;
	    நிலை ISN_2BOOL: எனில் (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    இல்லையெனில்
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    நிறுத்து;
	    நிலை ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      நிறுத்து;
	    நிலை ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      நிறுத்து;
	    நிலை ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    நிறுத்து;
	    நிலை ISN_PUT:
		எனில் (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		இல்லையெனில் எனில் (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		இல்லையெனில்
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		நிறுத்து;
	    நிலை ISN_IPUT:
		எனில் (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		இல்லையெனில் எனில் (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		இல்லையெனில்
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		நிறுத்து;
	    நிலை ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  வெற்று, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    எனில் (likely(buf != வெற்று))
		    {
			(வெறுமை)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    நிறுத்து;
		}
	    நிலை ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); நிறுத்து;

	    நிலை ISN_PROF_START:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 நிறுத்து;

	    நிலை ISN_PROF_END:
		smsg("%s%4d PROFILE END", pfx, current);
		நிறுத்து;

	    நிலை ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_names_len);
		நிறுத்து;

	    நிலை ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_மொத்தம்,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      நிறுத்து;
	    நிலை ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_item,
					 iptr->isn_arg.shuffle.shfl_up);
			      நிறுத்து;
	    நிலை ISN_DROP: smsg("%s%4d DROP", pfx, current); நிறுத்து;

	    நிலை ISN_SCRIPTCTX_SET:
		{
		    முழு	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ITEM(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		நிறுத்து;

	    நிலை ISN_FINISH: // End of list of instructions for ISN_SUBSTITUTE.
			   திரும்பு;
	}

	out_flush();	    // output one line at a time
	ui_breakcheck();
	எனில் (got_int)
	    நிறுத்து;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    வெறுமை
set_context_in_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_pattern = arg;

    // first argument already typed: only user function names
    எனில் (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
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
get_disassemble_argument(expand_T *xp, முழு idx)
{
    எனில் (idx == 0)
	திரும்பு (char_u *)"debug";
    எனில் (idx == 1)
	திரும்பு (char_u *)"profile";
    திரும்பு get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    வெறுமை
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = வெற்று;  // init to shut up gcc warning
    முழு		instr_மொத்தம் = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = find_func_by_name(arg, &compile_type);
    எனில் (ufunc == வெற்று)
	திரும்பு;
    எனில் (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, வெற்று) == FAIL)
	திரும்பு;
    எனில் (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	திரும்பு;
    }
    செய்தி((எழுத்து *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    தேர்வு (compile_type)
    {
	நிலை CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_மொத்தம் = dfunc->df_instr_prof_மொத்தம்;
	    நிறுத்து;
#endif
	    // FALLTHROUGH
	நிலை CT_NONE:
	    instr = dfunc->df_instr;
	    instr_மொத்தம் = dfunc->df_instr_மொத்தம்;
	    நிறுத்து;
	நிலை CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_மொத்தம் = dfunc->df_instr_debug_மொத்தம்;
	    நிறுத்து;
    }

    list_instructions("", instr, instr_மொத்தம், ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    முழு
tv2bool(typval_T *tv)
{
    தேர்வு (tv->v_type)
    {
	நிலை VAR_எண்:
	    திரும்பு tv->vval.v_எண் != 0;
	நிலை VAR_FLOAT:
	    திரும்பு tv->vval.v_float != 0.0;
	நிலை VAR_PARTIAL:
	    திரும்பு tv->vval.v_partial != வெற்று;
	நிலை VAR_FUNC:
	நிலை VAR_STRING:
	    திரும்பு tv->vval.v_string != வெற்று && *tv->vval.v_string != NUL;
	நிலை VAR_LIST:
	    திரும்பு tv->vval.v_list != வெற்று && tv->vval.v_list->lv_len > 0;
	நிலை VAR_TUPLE:
	    திரும்பு tuple_len(tv->vval.v_tuple) > 0;
	நிலை VAR_DICT:
	    திரும்பு tv->vval.v_dict != வெற்று
				    && tv->vval.v_dict->dv_hashtab.ht_used > 0;
	நிலை VAR_BOOL:
	நிலை VAR_SPECIAL:
	    திரும்பு tv->vval.v_எண் == VVAL_TRUE ? TRUE : FALSE;
	நிலை VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    திரும்பு tv->vval.v_job != வெற்று;
#இல்லையெனில்
	    நிறுத்து;
#endif
	நிலை VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    திரும்பு tv->vval.v_channel != வெற்று;
#இல்லையெனில்
	    நிறுத்து;
#endif
	நிலை VAR_BLOB:
	    திரும்பு tv->vval.v_blob != வெற்று && tv->vval.v_blob->bv_ga.ga_len > 0;

	நிலை VAR_OBJECT:
	    திரும்பு tv->vval.v_object != வெற்று;

	நிலை VAR_CLASS:
	நிலை VAR_TYPEALIAS:
	    check_typval_is_மதிப்பு(tv);
	    நிறுத்து;

	நிலை VAR_UNKNOWN:
	நிலை VAR_ANY:
	நிலை VAR_VOID:
	நிலை VAR_INSTR:
	    நிறுத்து;
    }
    திரும்பு FALSE;
}

    வெறுமை
emsg_using_string_as(typval_T *tv, முழு as_எண்)
{
    semsg(_(as_எண் ? e_using_string_as_எண்_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == வெற்று
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    முழு
check_not_string(typval_T *tv)
{
    எனில் (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

#endif // FEAT_EVAL
