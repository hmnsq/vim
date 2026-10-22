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
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
تعریفنوع ساختار {
    صحیح	    tcd_frame_idx;	// ec_frame_idx at ISN_TRY
    صحیح	    tcd_پشته_len;	// size of ectx.ec_stack at ISN_TRY
    صحیح	    tcd_in_catch;	// in catch or finally block
    صحیح	    tcd_did_throw;	// set did_throw in :endtry
    صحیح	    tcd_catch_idx;	// instruction of the first :catch or :finally
    صحیح	    tcd_finally_idx;	// instruction of the :finally block or zero
    صحیح	    tcd_endtry_idx;	// instruction of the :endtry
    صحیح	    tcd_caught;		// catch block entered
    صحیح	    tcd_cont;		// :continue encountered, jump here (minus one)
    صحیح	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
تعریفنوع ساختار {
    صحیح		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    صحیح		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
تعریفنوع ساختار {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    صحیح		or_outer_allocated;  // free "or_outer" later
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
ساختار ectx_S {
    garray_T	ec_پشته;	// stack of typval_T values
    صحیح		ec_frame_idx;	// index in ec_stack: context of ec_dfunc_idx
    صحیح		ec_initial_frame_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    صحیح		ec_dfunc_idx;	// current function index
    صحیح		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    صحیح		ec_did_emsg_before;
    صحیح		ec_trylevel_at_start;
    where_T	ec_where;
};

#ifdef FEAT_PROFILE
// stack of profinfo_T used when profiling.
ایستا garray_T profile_info_ga = {0, 0, اندازه(profinfo_T), 20, پوچ};
#endif

// Get pointer to item in the stack.
#define پشته_TV(idx) (((typval_T *)ectx->ec_پشته.ga_data) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#define پشته_TV_BOT(idx) (((typval_T *)ectx->ec_پشته.ga_data) + ectx->ec_پشته.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#define پشته_TV_VAR(idx) (((typval_T *)ectx->ec_پشته.ga_data) + ectx->ec_frame_idx + پشته_FRAME_SIZE + idx)

// Return value for functions used to execute instructions
#define EXEC_FAIL	0
#define EXEC_OK		1
#define EXEC_DONE	2

    تهی
to_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_to_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    ایستا صحیح
ufunc_argcount(ufunc_T *ufunc)
{
    بازگشت ufunc->uf_args.ga_len + (ufunc->uf_va_name != پوچ ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    ایستا صحیح
exe_concat(صحیح شمار, ectx_T *ectx)
{
    صحیح		idx;
    size_t	len = 0;
    garray_T	ga;
    تعریفنوع ساختار
    {
	typval_T    *tv;
	size_t	    length;
    } string_segment_T;
    شمارش
    {
	STRING_SEGMENT_CACHE_SIZE = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_SIZE];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    اگر (شمار > (صحیح)ARRAY_LENGTH(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, شمار);
	اگر (string_segment_tab == پوچ)
	    بازگشت FAIL;
    }

    ga_init2(&ga, اندازه(نویسه), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    برای (idx = 0; idx < شمار; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = پشته_TV_BOT(idx - شمار);
	اگر (segment->tv->vval.v_string != پوچ)
	{
	    segment->length = STRLEN(segment->tv->vval.v_string);
	    len += segment->length;
	}
	وگرنه
	    segment->length = 0;    // Ensure clean state for the second pass
    }

    اگر (ga_grow(&ga, (صحیح)len + 1) == FAIL)
    {
	اگر (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	بازگشت FAIL;
    }

    برای (idx = 0; idx < شمار; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->length);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_پشته.ga_len -= شمار - 1;
    پشته_TV_BOT(-1)->vval.v_string = ga.ga_data;

    اگر (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    بازگشت OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    ایستا صحیح
exe_newlist(صحیح شمار, ectx_T *ectx)
{
    list_T	*list = پوچ;
    صحیح		idx;
    typval_T	*tv;

    اگر (شمار >= 0)
    {
	list = list_alloc_with_اقلام(شمار);
	اگر (list == پوچ)
	    بازگشت FAIL;
	برای (idx = 0; idx < شمار; ++idx)
	    list_set_item(list, idx, پشته_TV_BOT(idx - شمار));
    }

    اگر (شمار > 0)
	ectx->ec_پشته.ga_len -= شمار - 1;
    وگرنه اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
    {
	list_unref(list);
	بازگشت FAIL;
    }
    وگرنه
	++ectx->ec_پشته.ga_len;
    tv = پشته_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_lock = 0;
    اگر (list != پوچ)
	++list->lv_refcount;
    بازگشت OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    ایستا صحیح
exe_newtuple(صحیح شمار, ectx_T *ectx)
{
    tuple_T	*tuple = پوچ;
    صحیح		idx;
    typval_T	*tv;

    اگر (شمار >= 0)
    {
	tuple = tuple_alloc_with_اقلام(شمار);
	اگر (tuple == پوچ)
	    بازگشت FAIL;
	برای (idx = 0; idx < شمار; ++idx)
	    tuple_set_item(tuple, idx, پشته_TV_BOT(idx - شمار));
    }

    اگر (شمار > 0)
	ectx->ec_پشته.ga_len -= شمار - 1;
    وگرنه اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
    {
	tuple_unref(tuple);
	بازگشت FAIL;
    }
    وگرنه
	++ectx->ec_پشته.ga_len;
    tv = پشته_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_lock = 0;
    اگر (tuple != پوچ)
	++tuple->tv_refcount;

    بازگشت OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    ایستا صحیح
exe_newdict(صحیح شمار, ectx_T *ectx)
{
    dict_T	*dict = پوچ;
    dictitem_T	*item;
    char_u	*کلید;
    صحیح		idx;
    typval_T	*tv;

    اگر (شمار >= 0)
    {
	dict = dict_alloc();
	اگر (unlikely(dict == پوچ))
	    بازگشت FAIL;
	برای (idx = 0; idx < شمار; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = پشته_TV_BOT(2 * (idx - شمار));
	    // check key is unique
	    کلید = tv->vval.v_string == پوچ ? (char_u *)"" : tv->vval.v_string;
	    item = dict_find(dict, کلید, -1);
	    اگر (item != پوچ)
	    {
		semsg(_(e_duplicate_کلید_in_dictionary_str), کلید);
		dict_unref(dict);
		بازگشت MAYBE;
	    }
	    item = dictitem_alloc(کلید);
	    clear_tv(tv);
	    اگر (unlikely(item == پوچ))
	    {
		dict_unref(dict);
		بازگشت FAIL;
	    }
	    tv = پشته_TV_BOT(2 * (idx - شمار) + 1);
	    item->di_tv = *tv;
	    item->di_tv.v_lock = 0;
	    tv->v_type = VAR_UNKNOWN;
	    اگر (dict_add(dict, item) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(item);
		dict_unref(dict);
		بازگشت FAIL;
	    }
	}
    }

    اگر (شمار > 0)
	ectx->ec_پشته.ga_len -= 2 * شمار - 1;
    وگرنه اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
    {
	dict_unref(dict);
	بازگشت FAIL;
    }
    وگرنه
	++ectx->ec_پشته.ga_len;
    tv = پشته_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_lock = 0;
    tv->vval.v_dict = dict;
    اگر (dict != پوچ)
	++dict->dv_refcount;
    بازگشت OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    تهی
update_has_breakpoint(ufunc_T *ufunc)
{
    اگر (ufunc->uf_debug_tick == debug_tick)
	بازگشت;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

ایستا garray_T dict_پشته = GA_EMPTY;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    ایستا صحیح
dict_پشته_save(typval_T *tv)
{
    اگر (dict_پشته.ga_growsize == 0)
	ga_init2(&dict_پشته, اندازه(typval_T), 10);
    اگر (ga_grow(&dict_پشته, 1) == FAIL)
	بازگشت FAIL;
    ((typval_T *)dict_پشته.ga_data)[dict_پشته.ga_len] = *tv;
    ++dict_پشته.ga_len;
    بازگشت OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    ایستا typval_T *
dict_پشته_get_tv(تهی)
{
    اگر (dict_پشته.ga_len == 0)
	بازگشت پوچ;
    بازگشت ((typval_T *)dict_پشته.ga_data) + dict_پشته.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    ایستا dict_T *
dict_پشته_get_dict(تهی)
{
    typval_T *tv;

    اگر (dict_پشته.ga_len == 0)
	بازگشت پوچ;
    tv = ((typval_T *)dict_پشته.ga_data) + dict_پشته.ga_len - 1;
    اگر (tv->v_type == VAR_DICT)
	بازگشت tv->vval.v_dict;
    بازگشت پوچ;
}

/*
 * Drop an item from the dict stack.
 */
    ایستا تهی
dict_پشته_drop(تهی)
{
    اگر (dict_پشته.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	بازگشت;
    }
    --dict_پشته.ga_len;
    clear_tv(((typval_T *)dict_پشته.ga_data) + dict_پشته.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    ایستا تهی
dict_پشته_clear(صحیح len)
{
    وقتی (dict_پشته.ga_len > len)
	dict_پشته_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    ایستا outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    اگر (ptref == پوچ)
	بازگشت &pt->pt_outer;

    // partial using partial (recursively)
    وقتی (ptref->pt_outer_partial != پوچ)
	ptref = ptref->pt_outer_partial;
    بازگشت &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    ایستا صحیح
check_ufunc_arg_types(ufunc_T *ufunc, صحیح argcount, صحیح off, ectx_T *ectx)
{
    اگر (ufunc->uf_arg_types == پوچ && ufunc->uf_va_type == پوچ)
	بازگشت OK;

    typval_T	*argv = پشته_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    برای (صحیح i = 0; i < argcount; ++i)
    {
	type_T *type = پوچ;

	// assume a v:none argument, using the default value, is always OK
	اگر (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_عدد == VVAL_NONE)
	    ادامه;
	// only pass values to user functions, never types
	اگر (check_typval_is_مقدار(&argv[i]) == FAIL)
	    بازگشت FAIL;

	اگر (i < ufunc->uf_args.ga_len && ufunc->uf_arg_types != پوچ)
	    type = ufunc->uf_arg_types[i];
	وگرنه اگر (ufunc->uf_va_type != پوچ)
	    type = ufunc->uf_va_type->tt_member;
	اگر (type != پوچ && check_typval_arg_type(type,
		    &argv[i], پوچ, i + 1) == FAIL)
	    بازگشت FAIL;
    }
    بازگشت OK;
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
    ایستا صحیح
call_dfunc(
	صحیح		cdf_idx,
	partial_T	*pt_arg,
	صحیح		argcount_arg,
	ectx_T		*ectx)
{
    صحیح		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    صحیح		did_emsg_before = did_emsg_cumul + did_emsg;
    صحیح		arg_to_add;
    صحیح		vararg_شمار = 0;
    صحیح		varcount;
    صحیح		idx;
    estack_T	*entry;
    funclocal_T	*floc = پوچ;
    صحیح		res = OK;
    compiletype_T compile_type;

    اگر (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == پوچ ? (char_u *)"unknown" : dfunc->df_name);
	بازگشت FAIL;
    }

#ifdef FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
    {
	اگر (GA_GROW_OK(&profile_info_ga, 1))
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
    اگر (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, پوچ);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    }
    اگر (res == FAIL || INSTRUCTIONS(dfunc) == پوچ)
    {
	اگر (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	بازگشت FAIL;
    }

    اگر (ufunc->uf_va_name != پوچ)
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
	vararg_شمار = argcount - ufunc->uf_args.ga_len;
	اگر (vararg_شمار < 0)
	    vararg_شمار = 0;
	وگرنه
	    argcount -= vararg_شمار;
	اگر (exe_newlist(vararg_شمار, ectx) == FAIL)
	    بازگشت FAIL;

	vararg_شمار = 1;
    }

    arg_to_add = ufunc->uf_args.ga_len - argcount;
    اگر (arg_to_add < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
			-arg_to_add), -arg_to_add);
	بازگشت FAIL;
    }
    وگرنه اگر (arg_to_add > ufunc->uf_def_args.ga_len)
    {
	صحیح missing = arg_to_add - ufunc->uf_def_args.ga_len;

	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
			missing), missing);
	بازگشت FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = پشته_TV_BOT(0) - argcount - vararg_شمار - 1;

    اگر (IS_OBJECT_METHOD(ufunc) && !IS_CONSTRUCTOR_METHOD(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == پوچ)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	بازگشت FAIL;
    }

    // Check the argument types.
    اگر (check_ufunc_arg_types(ufunc, argcount, vararg_شمار, ectx) == FAIL)
	بازگشت FAIL;

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
    اگر (GA_GROW_FAILS(&ectx->ec_پشته,
				     arg_to_add + پشته_FRAME_SIZE + varcount))
	بازگشت FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = پشته_TV_BOT(0) - argcount - vararg_شمار - 1;

    // If depth of calling is getting too high, don't execute the function.
    اگر (funcdepth_increment() == FAIL)
	بازگشت FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    اگر (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	اگر (floc == پوچ)
	    بازگشت FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    اگر (vararg_شمار > 0 && arg_to_add > 0)
	*پشته_TV_BOT(arg_to_add - 1) = *پشته_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    برای (idx = 0; idx < arg_to_add; ++idx)
	پشته_TV_BOT(idx - vararg_شمار)->v_type = VAR_UNKNOWN;
    ectx->ec_پشته.ga_len += arg_to_add;

    // Store current execution state in stack frame for ISN_RETURN.
    پشته_TV_BOT(پشته_FRAME_FUNC_OFF)->vval.v_عدد = ectx->ec_dfunc_idx;
    پشته_TV_BOT(پشته_FRAME_IIDX_OFF)->vval.v_عدد = ectx->ec_iidx;
    پشته_TV_BOT(پشته_FRAME_INSTR_OFF)->vval.v_string = (تهی *)ectx->ec_instr;
    پشته_TV_BOT(پشته_FRAME_OUTER_OFF)->vval.v_string =
						    (تهی *)ectx->ec_outer_ref;
    پشته_TV_BOT(پشته_FRAME_FUNCLOCAL_OFF)->vval.v_string = (تهی *)floc;
    پشته_TV_BOT(پشته_FRAME_IDX_OFF)->vval.v_عدد = ectx->ec_frame_idx;
    ectx->ec_frame_idx = ectx->ec_پشته.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    صحیح initcount = dfunc->df_varcount + (dfunc->df_has_closure ? 1 : 0);
    برای (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = پشته_TV_BOT(پشته_FRAME_SIZE + idx);

	tv->v_type = VAR_عدد;
	tv->vval.v_عدد = 0;
    }
    ectx->ec_پشته.ga_len += پشته_FRAME_SIZE + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    اگر (IS_OBJECT_METHOD(ufunc))
    {
	اگر (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_internal_error_str), "type in stack is not an object");
	    بازگشت FAIL;
	}

	*پشته_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != پوچ ? pt_arg : ufunc->uf_partial;
    اگر (pt != پوچ || (ufunc->uf_flags & FC_CLOSURE))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	اگر (ref == پوچ)
	    بازگشت FAIL;
	اگر (pt != پوچ)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	وگرنه
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    اگر (unlikely(ref->or_outer == پوچ))
	    {
		vim_free(ref);
		بازگشت FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->out_پشته = &ectx->ec_پشته;
	    ref->or_outer->out_frame_idx = ectx->ec_frame_idx;
	    اگر (ectx->ec_outer_ref != پوچ)
		ref->or_outer->out_up = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    وگرنه
	ectx->ec_outer_ref = پوچ;

    اگر (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    entry = estack_افزودن_ufunc(ufunc, 1);
    اگر (entry != پوچ)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	entry->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    بازگشت OK;
}

// Double linked list of funcstack_T in use.
ایستا funcstack_T *first_funcstack = پوچ;

    ایستا تهی
add_funcstack_to_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    اگر (first_funcstack != پوچ)
	first_funcstack->fs_prev = funcstack;
    funcstack->fs_next = first_funcstack;
    funcstack->fs_prev = پوچ;
    first_funcstack = funcstack;
}

    ایستا تهی
remove_funcstack_from_list(funcstack_T *funcstack)
{
    اگر (funcstack->fs_prev == پوچ)
	first_funcstack = funcstack->fs_next;
    وگرنه
	funcstack->fs_prev->fs_next = funcstack->fs_next;
    اگر (funcstack->fs_next != پوچ)
	funcstack->fs_next->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    ایستا صحیح
handle_closure_in_use(ectx_T *ectx, صحیح free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    صحیح		argcount;
    صحیح		بالاترین;
    صحیح		idx;
    typval_T	*tv;
    صحیح		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_شمار;

    اگر (dfunc->df_ufunc == پوچ)
	بازگشت OK;  // function was freed
    اگر (dfunc->df_has_closure == 0)
	بازگشت OK;  // no closures
    tv = پشته_TV(ectx->ec_frame_idx + پشته_FRAME_SIZE + dfunc->df_varcount);
    closure_شمار = tv->vval.v_عدد;
    اگر (closure_شمار == 0)
	بازگشت OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    بالاترین = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    برای (idx = 0; idx < closure_شمار; ++idx)
    {
	partial_T   *pt;
	صحیح	    off = gap->ga_len - closure_شمار + idx;

	اگر (off < 0)
	    ادامه;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	اگر (pt->pt_refcount > 1)
	{
	    صحیح refcount = pt->pt_refcount;
	    صحیح i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    برای (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = پشته_TV(ectx->ec_frame_idx
						       + پشته_FRAME_SIZE + i);
		اگر (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    اگر (refcount > 1)
	    {
		closure_in_use = TRUE;
		بشکن;
	    }
	}
    }

    اگر (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *پشته;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	اگر (funcstack == پوچ)
	    بازگشت FAIL;

	funcstack->fs_var_offset = argcount + پشته_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset
							  + dfunc->df_varcount;
	پشته = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = پشته;
	اگر (پشته == پوچ)
	{
	    vim_free(funcstack);
	    بازگشت FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	برای (idx = 0; idx < argcount; ++idx)
	{
	    tv = پشته_TV(بالاترین + idx);
	    اگر (free_arguments)
	    {
		*(پشته + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    وگرنه اگر (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, پشته + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	برای (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = پشته_TV(ectx->ec_frame_idx + پشته_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != پوچ)
	    {
		صحیح	    i;

		برای (i = 0; i < closure_شمار; ++i)
		    اگر (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_شمار + i])
			++funcstack->fs_min_refcount;
	    }

	    *(پشته + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	برای (idx = 0; idx < closure_شمار; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_شمار + idx];
	    اگر (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_پشته = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - بالاترین;
	    }
	}
    }

    برای (idx = 0; idx < closure_شمار; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_شمار + idx]);
    gap->ga_len -= closure_شمار;
    اگر (gap->ga_len == 0)
	ga_clear(gap);

    بازگشت OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    صحیح
funcstack_check_refcount(funcstack_T *funcstack)
{
    صحیح		i;
    garray_T	*gap = &funcstack->fs_ga;
    صحیح		done = 0;
    typval_T	*پشته;

    اگر (funcstack->fs_refcount > funcstack->fs_min_refcount)
	بازگشت FALSE;
    برای (i = funcstack->fs_var_offset; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_data) + i;

	اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != پوچ
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++done;
    }
    اگر (done != funcstack->fs_min_refcount)
	بازگشت FALSE;

    پشته = gap->ga_data;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    برای (i = 0; i < gap->ga_len; ++i)
	clear_tv(پشته + i);
    vim_free(پشته);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    بازگشت TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    صحیح
set_ref_in_funcstacks(صحیح copyID)
{
    funcstack_T *funcstack;

    برای (funcstack = first_funcstack; funcstack != پوچ;
						funcstack = funcstack->fs_next)
    {
	typval_T    *پشته = funcstack->fs_ga.ga_data;
	صحیح	    i;

	برای (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    اگر (set_ref_in_item(پشته + i, copyID, پوچ, پوچ, پوچ))
		بازگشت TRUE;  // abort
    }
    بازگشت FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
ایستا ectx_T *current_ectx = پوچ;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    صحیح
in_def_function(تهی)
{
    بازگشت current_ectx != پوچ;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    صحیح
fill_exec_lval_ریشه(lval_ریشه_T *ریشه)
{
    ectx_T *ectx = current_ectx;
    اگر (ectx != پوچ)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	اگر (ufunc->uf_class != پوچ)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    اگر (tv != پوچ)
	    {
		CLEAR_POINTER(ریشه);
		ریشه->lr_tv = tv;
		copy_tv(پشته_TV_VAR(0), ریشه->lr_tv);
		ریشه->lr_cl_exec = ufunc->uf_class;
		++ریشه->lr_cl_exec->class_refcount;
		بازگشت OK;
	    }
	}
    }

    بازگشت FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(تهی)
{
    ectx_T *r = current_ectx;

    current_ectx = پوچ;
    بازگشت r;
}

    تهی
restore_current_ectx(ectx_T *ectx)
{
    اگر (current_ectx != پوچ)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    ایستا list_T *
add_defer_item(صحیح var_idx, صحیح argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = پشته_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    اگر (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	اگر (rettv_list_alloc(defer_tv) == FAIL)
	    بازگشت پوچ;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_اقلام(argcount + 1);
    اگر (l == پوچ)
	بازگشت پوچ;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_lock = 0;
    اگر (list_درج_tv(defer_l, &listval, defer_l->lv_first) == FAIL)
    {
	list_free(l);
	بازگشت پوچ;
    }

    بازگشت l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    ایستا صحیح
defer_command(صحیح var_idx, صحیح argcount, ectx_T *ectx)
{
    list_T	*l = add_defer_item(var_idx, argcount, ectx);
    صحیح		i;
    typval_T	*func_tv;

    اگر (l == پوچ)
	بازگشت FAIL;

    func_tv = پشته_TV_BOT(-argcount - 1);
    اگر (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	بازگشت FAIL;
    }
    list_set_item(l, 0, func_tv);

    برای (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, پشته_TV_BOT(-argcount + i));
    ectx->ec_پشته.ga_len -= argcount + 1;
    بازگشت OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    صحیح
add_defer_function(char_u *name, صحیح argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    صحیح		i;

    اگر (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	بازگشت FAIL;
    }

    l = add_defer_item(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    اگر (l == پوچ)
    {
	vim_free(name);
	بازگشت FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_lock = 0;
    func_tv.vval.v_string = name;
    list_set_item(l, 0, &func_tv);

    برای (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, argvars + i);
    بازگشت OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    ایستا تهی
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = پشته_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    اگر (defer_tv->v_type != VAR_LIST)
	بازگشت;	 // no function added
    FOR_ALL_LIST_اقلام(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ARGS];
	صحیح	    i;
	listitem_T  *arg_li = l->lv_first;
	typval_T    *functv = &l->lv_first->li_tv;
	صحیح	    argcount = l->lv_len - 1;

	اگر (functv->vval.v_string == پوچ)
	    // already being called, can happen if function does ":qa"
	    ادامه;

	برای (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_next;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = پوچ;
	partial_T	*pt = پوچ;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	اگر (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = پوچ;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    اگر (funcexe.fe_object != پوچ)
		++funcexe.fe_object->obj_refcount;
	}
	وگرنه
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = پوچ;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(تهی)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	اگر (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	وگرنه
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    ایستا صحیح
func_return(ectx_T *ectx)
{
    صحیح		idx;
    صحیح		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    صحیح		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*entry;
    صحیح		prev_dfunc_idx = پشته_TV(ectx->ec_frame_idx
					+ پشته_FRAME_FUNC_OFF)->vval.v_عدد;
    funclocal_T	*floc;
#ifdef FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_data)
							      + prev_dfunc_idx;

    اگر (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	اگر (dfunc->df_ufunc->uf_profiling
				   || (caller != پوچ && caller->uf_profiling))
	{
	    profile_may_end_func(((profinfo_T *)profile_info_ga.ga_data)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#endif

    اگر (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    اگر (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    entry = estack_برداشت();
    اگر (entry != پوچ)
	current_sctx = entry->es_save_sctx;

    اگر (handle_closure_in_use(ectx, TRUE) == FAIL)
	بازگشت FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    صحیح بالاترین = ectx->ec_frame_idx - argcount;
    اگر (IS_OBJECT_METHOD(dfunc->df_ufunc))
	--بالاترین;
    برای (idx = بالاترین; idx < ectx->ec_frame_idx; ++idx)
	clear_tv(پشته_TV(idx));

    // Clear local variables and temp values, but not the return value.
    برای (idx = ectx->ec_frame_idx + پشته_FRAME_SIZE;
					idx < ectx->ec_پشته.ga_len - 1; ++idx)
	clear_tv(پشته_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_پشته.ga_len - 1;
    اگر (ret_idx == ectx->ec_frame_idx + پشته_FRAME_IDX_OFF)
	ret_idx = 0;

    اگر (ectx->ec_outer_ref != پوچ)
    {
	اگر (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = پشته_TV(ectx->ec_frame_idx
					+ پشته_FRAME_IIDX_OFF)->vval.v_عدد;
    ectx->ec_instr = (تهی *)پشته_TV(ectx->ec_frame_idx
				       + پشته_FRAME_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (تهی *)پشته_TV(ectx->ec_frame_idx
				       + پشته_FRAME_OUTER_OFF)->vval.v_string;
    floc = (تهی *)پشته_TV(ectx->ec_frame_idx
				   + پشته_FRAME_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_frame_idx = پشته_TV(ectx->ec_frame_idx
				       + پشته_FRAME_IDX_OFF)->vval.v_عدد;

    اگر (floc == پوچ)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    وگرنه
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    اگر (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_پشته.ga_len = بالاترین + 1;
	*پشته_TV_BOT(-1) = *پشته_TV(ret_idx);
    }
    وگرنه
	// Reset the stack to the position before the call.
	ectx->ec_پشته.ga_len = بالاترین;

    funcdepth_decrement();
    --ex_nesting_level;
    بازگشت OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    ایستا صحیح
call_prepare(صحیح argcount, typval_T *argvars, ectx_T *ectx)
{
    صحیح		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    برای (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *پشته_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    اگر (argcount > 0)
	ectx->ec_پشته.ga_len -= argcount - 1;
    وگرنه اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
	بازگشت FAIL;
    وگرنه
	++ectx->ec_پشته.ga_len;

    // Default return value is zero.
    tv = پشته_TV_BOT(-1);
    tv->v_type = VAR_عدد;
    tv->vval.v_عدد = 0;
    tv->v_lock = 0;

    بازگشت OK;
}

/*
 * Call a builtin function by index.
 */
    ایستا صحیح
call_bfunc(صحیح func_idx, صحیح argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    صحیح		idx;
    صحیح		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    نویسه	*save_func_name = ectx->ec_where.wt_func_name;

    اگر (call_prepare(argcount, argvars, ectx) == FAIL)
	بازگشت FAIL;

    // Check for void value being passed as an argument.
    برای (idx = 0; idx < argcount; ++idx)
	اگر (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_مقدار));
	    برای (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    بازگشت FAIL;
	}

    ectx->ec_where.wt_func_name = internal_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_internal_func_by_idx(func_idx, argvars, پشته_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    برای (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    اگر (did_emsg > did_emsg_before)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ایستا صحیح
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	صحیح	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    funcexe_T   funcexe;
    funcerror_T	error;
    صحیح		idx;
    صحیح		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    اگر (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, پوچ)
								       == FAIL)
	بازگشت FAIL;
    اگر (ufunc->uf_def_status == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	اگر (error != FCERR_UNKNOWN)
	{
	    اگر (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    وگرنه
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    بازگشت FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	اگر (iptr != پوچ)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	بازگشت call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    اگر (call_prepare(argcount, argvars, ectx) == FAIL)
	بازگشت FAIL;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != پوچ ? selfdict : dict_پشته_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      پشته_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    برای (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    اگر (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_found_var);
	بازگشت FAIL;
    }
    اگر (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * If command modifiers were applied restore them.
 */
    ایستا تهی
may_restore_cmdmod(funclocal_T *funclocal)
{
    اگر (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = پوچ;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    ایستا صحیح
vim9_aborting(صحیح prev_uncaught_emsg)
{
    بازگشت uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    ایستا صحیح
call_by_name(
	char_u	    *name,
	صحیح	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    اگر (builtin_function(name, -1))
    {
	صحیح func_idx = find_internal_func(name);

	اگر (func_idx < 0)  // Impossible?
	    بازگشت FAIL;
	اگر (check_internal_func(func_idx, argcount) < 0)
	    بازگشت FAIL;
	بازگشت call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*start_bracket = generic_func_find_open_bracket(name);
    اگر (start_bracket != پوچ)
    {
	cc = *start_bracket;
	*start_bracket = NUL;
    }

    ufunc = find_func(name, FALSE);

    اگر (ufunc == پوچ)
    {
	صحیح prev_uncaught_emsg = uncaught_emsg;

	اگر (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = find_func(name, FALSE);

	اگر (vim9_aborting(prev_uncaught_emsg))
	{
	    اگر (start_bracket != پوچ)
		*start_bracket = cc;
	    بازگشت FAIL;  // bail out if loading the script caused an error
	}
    }

    اگر (start_bracket != پوچ)
	*start_bracket = cc;

    اگر (ufunc != پوچ)
    {
	اگر (IS_GENERIC_FUNC(ufunc))
	{
	    اگر (start_bracket != پوچ)
		ufunc = find_generic_func(ufunc, name, &start_bracket);
	    وگرنه
	    {
		emsg_funcname(e_generic_func_missing_type_args_str, name);
		ufunc = پوچ;
	    }
	    اگر (ufunc == پوچ)
		بازگشت FAIL;
	}
	وگرنه اگر (start_bracket != پوچ)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    بازگشت FAIL;
	}

	اگر (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    بازگشت FAIL;

	بازگشت call_ufunc(ufunc, پوچ, argcount, ectx, iptr, selfdict);
    }

    بازگشت FAIL;
}

    ایستا صحیح
call_partial(
	typval_T    *tv,
	صحیح	    argcount_arg,
	ectx_T	    *ectx)
{
    صحیح		argcount = argcount_arg;
    char_u	*name = پوچ;
    صحیح		called_emsg_before = called_emsg;
    صحیح		res = FAIL;
    dict_T	*selfdict = پوچ;

    اگر (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	صحیح	    i;

	اگر (pt->pt_obj != پوچ)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		بازگشت FAIL;
	    برای (i = 1; i <= argcount; ++i)
		*پشته_TV_BOT(-i + 1) = *پشته_TV_BOT(-i);

	    typval_T *obj_tv = پشته_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_lock = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_پشته.ga_len;
	}

	اگر (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    اگر (GA_GROW_FAILS(&ectx->ec_پشته, pt->pt_argc))
		بازگشت FAIL;
	    برای (i = 1; i <= argcount; ++i)
		*پشته_TV_BOT(-i + pt->pt_argc) = *پشته_TV_BOT(-i);
	    ectx->ec_پشته.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    برای (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], پشته_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	اگر (pt->pt_func != پوچ)
	    بازگشت call_ufunc(pt->pt_func, pt, argcount, ectx, پوچ, selfdict);

	name = pt->pt_name;
    }
    وگرنه اگر (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    اگر (name != پوچ)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = پوچ;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	اگر (error != FCERR_NONE)
	    res = FAIL;
	وگرنه
	    res = call_by_name(fname, argcount, ectx, پوچ, selfdict);
	vim_free(tofree);
    }

    اگر (res == FAIL)
    {
	اگر (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == پوچ ? (char_u *)"[unknown]" : name);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    ایستا صحیح
error_if_locked(صحیح lock, نویسه *error)
{
    اگر (lock & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    ایستا صحیح
check_for_عدد(typval_T *tv)
{
    اگر (tv->v_type != VAR_عدد)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_عدد), vartype_name(tv->v_type));
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    ایستا تهی
store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    صحیح		    flags = ASSIGN_DECL;

    اگر (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, پوچ, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    ایستا صحیح
do_2string(typval_T *tv, صحیح is_2string_any, صحیح tostring_flags)
{
    اگر (tv->v_type == VAR_STRING)
	بازگشت OK;

    char_u *str;

    اگر (is_2string_any)
    {
	انتخاب (tv->v_type)
	{
	    حالت VAR_SPECIAL:
	    حالت VAR_BOOL:
	    حالت VAR_عدد:
	    حالت VAR_FLOAT:
	    حالت VAR_DICT:
	    حالت VAR_BLOB:	بشکن;

	    حالت VAR_LIST:
				اگر (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, اندازه(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    اگر (str == پوچ)
					بازگشت FAIL;
				    s = str;
				    وقتی ((e = vim_strchr(s, '\n')) != پوچ)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					اگر (p != پوچ)
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
				    بازگشت OK;
				}
				اگر (tostring_flags & TOSTRING_INTERPOLATE)
				    بشکن;
				// FALLTHROUGH
	    پیشفرض:	to_string_error(tv->v_type);
			بازگشت FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    بازگشت OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    ایستا تهی
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    انتخاب (tv->v_type)
    {
	حالت VAR_LIST:
	    اگر (tv->vval.v_list == پوچ && sv->sv_type != &t_list_empty)
		(تهی)rettv_list_alloc(tv);
	    بشکن;
	حالت VAR_TUPLE:
	    اگر (tv->vval.v_tuple == پوچ && sv->sv_type != &t_tuple_empty)
		(تهی)rettv_tuple_alloc(tv);
	    بشکن;
	حالت VAR_DICT:
	    اگر (tv->vval.v_dict == پوچ && sv->sv_type != &t_dict_empty)
		(تهی)rettv_dict_alloc(tv);
	    بشکن;
	حالت VAR_BLOB:
	    اگر (tv->vval.v_blob == پوچ && sv->sv_type != &t_blob_null)
		(تهی)rettv_blob_alloc(tv);
	    بشکن;
	پیشفرض:
	    بشکن;
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

    اگر (str == پوچ)
	بازگشت پوچ;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    اگر (index < 0)
    {
	صحیح	clen = 0;

	برای (nbyte = 0; nbyte < slen; ++clen)
	{
	    اگر (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    وگرنه اگر (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    وگرنه
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + index;
	اگر (nchar < 0)
	    // unlike list: index out of range results in empty string
	    بازگشت پوچ;
    }

    برای (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	اگر (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	وگرنه اگر (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	وگرنه
	    nbyte += mb_ptr2len(str + nbyte);
    }
    اگر (nbyte >= slen)
	بازگشت پوچ;
    بازگشت vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    ایستا long
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    اگر (nchar >= 0)
    {
	وقتی (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    وگرنه
    {
	nbyte = str_len;
	وقتی (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_head_off(str, str + nbyte);
	    ++nchar;
	}
	اگر (nchar < 0)
	    بازگشت -1;
    }
    بازگشت (long)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_slice(char_u *str, varnumber_T first, varnumber_T last, صحیح exclusive)
{
    long	start_byte, end_byte;
    size_t	slen;

    اگر (str == پوچ)
	بازگشت پوچ;
    slen = STRLEN(str);
    start_byte = char_idx2byte(str, slen, first);
    اگر (start_byte < 0)
	start_byte = 0; // first index very negative: use zero
    اگر ((last == -1 && !exclusive) || last == VARNUM_MAX)
	end_byte = (long)slen;
    وگرنه
    {
	end_byte = char_idx2byte(str, slen, last);
	اگر (!exclusive && end_byte >= 0 && end_byte < (long)slen)
	    // end index is inclusive
	    end_byte += mb_ptr2len(str + end_byte);
    }

    اگر (start_byte >= (long)slen || end_byte <= start_byte)
	بازگشت پوچ;
    بازگشت vim_strnsave(str + start_byte, end_byte - start_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    ایستا svar_T *
get_script_svar(scriptref_T *sref, صحیح dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? پوچ
			      : ((dfunc_T *)def_functions.ga_data) + dfunc_idx;
    svar_T	    *sv;

    اگر (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	اگر (dfunc != پوچ)
	    semsg(_(e_script_variable_invalid_after_reload_in_function_str),
					 printable_func_name(dfunc->df_ufunc));
	بازگشت پوچ;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_data) + sref->sref_idx;
    اگر (sv->sv_name == پوچ)
    {
	اگر (dfunc != پوچ)
	    emsg(_(e_script_variable_was_deleted));
	بازگشت پوچ;
    }
    اگر (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	اگر (dfunc != پوچ)
	    emsg(_(e_script_variable_type_changed));
	بازگشت پوچ;
    }

    اگر ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	اگر (dfunc != پوچ)
	    semsg(_(e_item_not_exported_in_script_str), sv->sv_name);
	بازگشت پوچ;
    }
    بازگشت sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    ایستا char_u *
get_split_sourceline(
	صحیح c UNUSED,
	تهی *cookie,
	صحیح indent UNUSED,
	getline_opt_T options UNUSED)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*p;
    char_u		*سطر;

    p = vim_strchr(sp->nextline, '\n');
    اگر (p == پوچ)
    {
	سطر = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    وگرنه
    {
	سطر = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    بازگشت سطر;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ایستا صحیح
call_eval_func(
	char_u	    *name,
	صحیح	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    صحیح	    called_emsg_before = called_emsg;
    صحیح	    res;

    res = call_by_name(name, argcount, ectx, iptr, پوچ);
    اگر (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = find_var(name, پوچ, FALSE);
	اگر (v == پوچ || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    بازگشت FAIL;
	}
	بازگشت call_partial(&v->di_tv, argcount, ectx);
    }
    بازگشت res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    صحیح
fill_partial_and_closure(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    اگر (ufunc->uf_flags & FC_CLOSURE)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.out_پشته = &ectx->ec_پشته;
	pt->pt_outer.out_frame_idx = ectx->ec_frame_idx;
	اگر (ectx->ec_outer_ref != پوچ)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.out_up = ectx->ec_outer_ref->or_outer;
	    اگر (ectx->ec_outer_ref->or_partial != پوچ)
	    {
		pt->pt_outer.out_up_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.out_up_partial->pt_refcount;
	    }
	}

	اگر (lvi != پوچ)
	{
	    صحیح	depth;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    برای (depth = 0; depth < lvi->lvi_depth; ++depth)
	    {
		pt->pt_outer.out_loop[depth].پشته = &ectx->ec_پشته;
		pt->pt_outer.out_loop[depth].var_idx = ectx->ec_frame_idx
			 + پشته_FRAME_SIZE + lvi->lvi_loop[depth].var_idx;
		pt->pt_outer.out_loop[depth].var_شمار =
					    lvi->lvi_loop[depth].var_شمار;
	    }
	    pt->pt_outer.out_loop_size = lvi->lvi_depth;
	}
	وگرنه
	    pt->pt_outer.out_loop_size = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	اگر (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    بازگشت FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_پشته.ga_data) + ectx->ec_frame_idx
		       + پشته_FRAME_SIZE + dfunc->df_varcount)->vval.v_عدد;

	((partial_T **)ectx->ec_funcrefs.ga_data)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    بازگشت OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    ایستا صحیح
exec_command(isn_T *iptr, char_u *cmd_string)
{
    source_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_FIELD(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    اگر (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	بازگشت FAIL;
    بازگشت OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    صحیح
may_load_script(صحیح sid, صحیح *loaded)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);

    اگر (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	اگر (do_source(si->sn_name, FALSE, DOSO_NONE, پوچ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    بازگشت FAIL;
	}
    }
    بازگشت OK;
}

// used for v_instr of typval of VAR_INSTR
ساختار instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
تعریفنوع ساختار subs_عبارت_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    صحیح		subs_status;
} subs_عبارت_T;

// Set when calling do_debug().
ایستا ectx_T	*debug_context = پوچ;
ایستا صحیح	debug_var_شمار;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    صحیح		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    صحیح		    varargs_off;

    اگر (ectx == پوچ)
	بازگشت پوچ;
    dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    برای (idx = debug_var_شمار - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_names.ga_data)[idx];

	// the variable name may be NULL when not available in this block
	اگر (varname != پوچ && STRCMP(varname, name) == 0)
	    بازگشت پشته_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == پوچ ? 0 : 1;
    برای (idx = 0; idx < ufunc->uf_args.ga_len; ++idx)
	اگر (STRCMP(((char_u **)(ufunc->uf_args.ga_data))[idx], name) == 0)
	    بازگشت پشته_TV(ectx->ec_frame_idx - ufunc->uf_args.ga_len
							  - varargs_off + idx);
    اگر (ufunc->uf_va_name != پوچ && STRCMP(ufunc->uf_va_name, name) == 0)
	بازگشت پشته_TV(ectx->ec_frame_idx - 1);

    بازگشت پوچ;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    صحیح
may_break_in_function(ufunc_T *ufunc)
{
    بازگشت ufunc->uf_has_breakpoint || debug_has_عبارت_breakpoint();
}

    ایستا تهی
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*سطر;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    صحیح		end_lnum = iptr->isn_lnum;
    garray_T	ga;
    صحیح		lnum;

    اگر (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	اگر (!may_break_in_function(ufunc))
	    بازگشت;

	// check for the next breakpoint if needed
	breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	اگر (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    بازگشت;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_شمار = iptr->isn_arg.debug.dbg_var_names_len;

    برای (ni = iptr + 1; ni->isn_type != ISN_FINISH; ++ni)
	اگر (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    end_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    بشکن;
	}

    اگر (end_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, اندازه(char_u *), 10);
	برای (lnum = iptr->isn_lnum; lnum < end_lnum
				     && lnum <= ufunc->uf_سطرها.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_سطرها.ga_data)[lnum - 1];

	    اگر (p == پوچ)
		ادامه;  // left over from continuation line
	    p = skipwhite(p);
	    اگر (*p == '#')
		بشکن;
	    اگر (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_data))[ga.ga_len++] = p;
	    اگر (STRNCMP(p, "def ", 4) == 0)
		بشکن;
	}
	سطر = ga_concat_strings(&ga, "  ");
	vim_free(ga.ga_data);
    }
    وگرنه
	سطر = ((char_u **)ufunc->uf_سطرها.ga_data)[iptr->isn_lnum - 1];

    do_debug(سطر == پوچ ? (char_u *)"[empty]" : سطر);
    debug_context = پوچ;

    اگر (end_lnum > iptr->isn_lnum)
	vim_free(سطر);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    ایستا صحیح
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    اگر (lhs_list->lv_type == پوچ || lhs_list->lv_type->tt_member == پوچ)
	بازگشت OK;

    بازگشت check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    ایستا صحیح
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = پشته_TV_BOT(-2);
    long	lidx = 0;
    typval_T	*tv_dest = پشته_TV_BOT(-1);
    صحیح		status = OK;
    صحیح		check_rhs_type = FALSE;

    اگر (tv_idx->v_type == VAR_عدد)
	lidx = (long)tv_idx->vval.v_عدد;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = پشته_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    اگر (dest_type == VAR_OBJECT && tv_dest->vval.v_object == پوچ)
    {
	emsg(_(e_using_null_object));
	status = FAIL;
    }
    وگرنه اگر (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	اگر (dest_type == VAR_DICT)
	    status = do_2string(tv_idx, TRUE, FALSE);
	وگرنه اگر (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    اگر (obj == پوچ)
	    {
		emsg(_(e_using_null_object));
		status = FAIL;
	    }
	    وگرنه
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		صحیح		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		اگر (m != پوچ)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    اگر ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == پوچ ||
			     !class_instance_of(ufunc->uf_class, cl)))
		    {
			نویسه *پیام = (m->ocm_access == VIM_ACCESS_PRIVATE)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(پیام, m->ocm_name.string, 0, cl);
			status = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    وگرنه اگر (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			status = FAIL;
		    وگرنه
			lidx = m_idx;
		}
		وگرنه
		{
		    member_not_found_پیام(cl, VAR_OBJECT, member, 0);
		    status = FAIL;
		}
	    }
	}
	وگرنه اگر (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    status = FAIL;
	}
	وگرنه اگر (dest_type == VAR_LIST && tv_idx->v_type != VAR_عدد)
	{
	    emsg(_(e_عدد_expected));
	    status = FAIL;
	}
    }

    اگر (status == OK)
    {
	اگر (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    اگر (list == پوچ)
	    {
		emsg(_(e_list_not_set));
		بازگشت FAIL;
	    }
	    اگر (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    اگر (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_index_out_of_range_nr), lidx);
		بازگشت FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    اگر (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		بازگشت FAIL;

	    اگر (lidx < list->lv_len)
	    {
		listitem_T *li = list_find(list, lidx);
		اگر (li == پوچ)
		    بازگشت FAIL;
		اگر (error_if_locked(li->li_tv.v_lock,
					     e_cannot_change_locked_list_item))
		    بازگشت FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    وگرنه
	    {
		اگر (error_if_locked(list->lv_lock, e_cannot_change_locked_list))
		    بازگشت FAIL;
		// append to list, only fails when out of memory
		اگر (list_append_tv(list, tv) == FAIL)
		    بازگشت NOTDONE;
		clear_tv(tv);
	    }
	}
	وگرنه اگر (dest_type == VAR_DICT)
	{
	    char_u		*کلید = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    اگر (dict == پوچ)
	    {
		emsg(_(e_dictionary_not_set));
		بازگشت FAIL;
	    }
	    اگر (کلید == پوچ)
		کلید = (char_u *)"";
	    di = dict_find(dict, کلید, -1);
	    اگر (di != پوچ)
	    {
		اگر (error_if_locked(di->di_tv.v_lock,
						    e_cannot_change_dict_item))
		    بازگشت FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    وگرنه
	    {
		اگر (error_if_locked(dict->dv_lock, e_cannot_change_dict))
		    بازگشت FAIL;
		// add to dict, only fails when out of memory
		اگر (dict_add_tv(dict, (نویسه *)کلید, tv) == FAIL)
		    بازگشت NOTDONE;
		clear_tv(tv);
	    }
	}
	وگرنه اگر (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    صحیح		    error = FALSE;
	    صحیح		    len;

	    اگر (blob == پوچ)
	    {
		emsg(_(e_blob_not_set));
		بازگشت FAIL;
	    }
	    len = blob_len(blob);
	    اگر (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    اگر (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), lidx);
		بازگشت FAIL;
	    }
	    اگر (مقدار_check_lock(blob->bv_lock, (char_u *)"blob", FALSE))
		بازگشت FAIL;
	    nr = tv_get_عدد_chk(tv, &error);
	    اگر (error)
		بازگشت FAIL;
	    اگر (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_مقدار_for_blob_nr), nr);
		بازگشت FAIL;
	    }
	    blob_set_append(blob, lidx, nr);
	}
	وگرنه اگر (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    اگر (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		اگر (itf != پوچ)
		    // convert interface member index to class member index
		    lidx = object_index_from_itf_index(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    وگرنه
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	وگرنه اگر (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    status = FAIL;
	}
	وگرنه
	{
	    status = FAIL;
	    semsg(_(e_cannot_index_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_پشته.ga_len -= 3;
    اگر (status == FAIL)
    {
	clear_tv(tv);
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Store a value in a list or blob range.
 */
    ایستا صحیح
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = پشته_TV_BOT(-3);
    typval_T	*tv_idx2 = پشته_TV_BOT(-2);
    typval_T	*tv_dest = پشته_TV_BOT(-1);
    صحیح		status = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = پشته_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    اگر (tv_dest->v_type == VAR_LIST)
    {
	long	    n1;
	long	    n2;
	listitem_T  *li1;

	n1 = (long)tv_get_عدد_chk(tv_idx1, پوچ);
	اگر (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_عدد == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	وگرنه
	    n2 = (long)tv_get_عدد_chk(tv_idx2, پوچ);

	li1 = check_range_index_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	اگر (li1 == پوچ)
	    status = FAIL;
	وگرنه
	{
	    status = check_range_index_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    اگر (status != FAIL)
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
    وگرنه اگر (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	long	    bloblen;

	n1 = tv_get_عدد_chk(tv_idx1, پوچ);
	اگر (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_عدد == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	وگرنه
	    n2 = tv_get_عدد_chk(tv_idx2, پوچ);
	bloblen = blob_len(tv_dest->vval.v_blob);

	اگر (check_blob_index(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    status = FAIL;
	وگرنه
	    status = blob_set_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    وگرنه
    {
	status = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_پشته.ga_len -= 4;
    clear_tv(tv);

    بازگشت status;
}

/*
 * Unlet item in list or dict variable.
 */
    ایستا صحیح
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = پشته_TV_BOT(-2);
    typval_T	*tv_dest = پشته_TV_BOT(-1);
    صحیح		status = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    اگر (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	اگر (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_عدد)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    status = FAIL;
	}
	وگرنه
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*کلید;
	    dictitem_T  *di = پوچ;

	    اگر (d != پوچ && مقدار_check_lock(
				      d->dv_lock, پوچ, FALSE))
		status = FAIL;
	    وگرنه
	    {
		اگر (tv_idx->v_type == VAR_STRING)
		{
		    کلید = tv_idx->vval.v_string;
		    اگر (کلید == پوچ)
			کلید = (char_u *)"";
		}
		وگرنه
		{
		    کلید = tv_get_string(tv_idx);
		}
		اگر (d != پوچ)
		    di = dict_find(d, کلید, (صحیح)STRLEN(کلید));
		اگر (di == پوچ)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_کلید_not_present_in_dictionary_str), کلید);
		    status = FAIL;
		}
		وگرنه اگر (var_check_fixed(di->di_flags,
						   پوچ, FALSE)
			|| var_check_ro(di->di_flags,
						  پوچ, FALSE))
		    status = FAIL;
		وگرنه
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    وگرنه اگر (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	اگر (check_for_عدد(tv_idx) == FAIL)
	{
	    status = FAIL;
	}
	وگرنه
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n = (long)tv_idx->vval.v_عدد;

	    اگر (l != پوچ && مقدار_check_lock(
				      l->lv_lock, پوچ, FALSE))
		status = FAIL;
	    وگرنه
	    {
		listitem_T	*li = list_find(l, n);

		اگر (li == پوچ)
		{
		    semsg(_(e_list_index_out_of_range_nr), n);
		    status = FAIL;
		}
		وگرنه
		    listitem_remove(l, li);
	    }
	}
    }
    وگرنه
    {
	status = FAIL;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_پشته.ga_len -= 2;

    بازگشت status;
}

/*
 * Unlet a range of items in a list variable.
 */
    ایستا صحیح
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = پشته_TV_BOT(-3);
    typval_T	*tv_idx2 = پشته_TV_BOT(-2);
    typval_T	*tv_dest = پشته_TV_BOT(-1);
    صحیح		status = OK;

    اگر (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	اگر (check_for_عدد(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_عدد(tv_idx2) == FAIL))
	{
	    status = FAIL;
	}
	وگرنه
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n1 = (long)tv_idx1->vval.v_عدد;
	    long	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (long)tv_idx2->vval.v_عدد;
	    listitem_T	*li;

	    li = list_find_index(l, &n1);
	    اگر (li == پوچ)
	    {
		semsg(_(e_list_index_out_of_range_nr),
						 (long)tv_idx1->vval.v_عدد);
		status = FAIL;
	    }
	    وگرنه
	    {
		اگر (n1 < 0)
		    n1 = list_idx_of_item(l, li);
		اگر (n2 < 0)
		{
		    listitem_T *li2 = list_find(l, n2);

		    اگر (li2 == پوچ)
		    {
			semsg(_(e_list_index_out_of_range_nr), n2);
			status = FAIL;
		    }
		    وگرنه
			n2 = list_idx_of_item(l, li2);
		}
		اگر (status != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_index_out_of_range_nr), n2);
		    status = FAIL;
		}
		اگر (status != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    وگرنه
    {
	status = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_پشته.ga_len -= 3;

    بازگشت status;
}

/*
 * Top of a for loop.
 */
    ایستا صحیح
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    صحیح		jump = FALSE;
    typval_T	*ltv = پشته_TV_BOT(-1);
    typval_T	*idxtv =
		   پشته_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
	بازگشت FAIL;
    اگر (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_عدد;
	اگر (list == پوچ
		       || idxtv->vval.v_عدد >= list->lv_len)
	{
	    jump = TRUE;
	}
	وگرنه اگر (list->lv_first == &range_list_item)
	{
	    // non-materialized range() list
	    tv = پشته_TV_BOT(0);
	    tv->v_type = VAR_عدد;
	    tv->v_lock = 0;
	    tv->vval.v_عدد = list_find_nr(
			     list, idxtv->vval.v_عدد, پوچ);
	    ++ectx->ec_پشته.ga_len;
	}
	وگرنه
	{
	    listitem_T *li = list_find(list,
					 idxtv->vval.v_عدد);
	    اگر (li == پوچ)
		بازگشت FAIL;
	    copy_tv(&li->li_tv, پشته_TV_BOT(0));
	    ++ectx->ec_پشته.ga_len;
	}
    }
    وگرنه اگر (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_عدد;
	اگر (tuple == پوچ || idxtv->vval.v_عدد >= TUPLE_LEN(tuple))
	    jump = TRUE;
	وگرنه
	{
	    copy_tv(TUPLE_ITEM(tuple, idxtv->vval.v_عدد), پشته_TV_BOT(0));
	    ++ectx->ec_پشته.ga_len;
	}
    }
    وگرنه اگر (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_عدد;
	اگر (str == پوچ || str[idxtv->vval.v_عدد] == NUL)
	{
	    jump = TRUE;
	}
	وگرنه
	{
	    صحیح	clen = mb_ptr2len(str + idxtv->vval.v_عدد);

	    // Push the next character from the string.
	    tv = پشته_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_عدد, clen);
	    ++ectx->ec_پشته.ga_len;
	    idxtv->vval.v_عدد += clen - 1;
	}
    }
    وگرنه اگر (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	اگر (idxtv->vval.v_عدد == -1 && blob != پوچ)
	{
	    ltv->v_lock = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_عدد;
	اگر (blob == پوچ || idxtv->vval.v_عدد >= blob_len(blob))
	{
	    jump = TRUE;
	}
	وگرنه
	{
	    // Push the next byte from the blob.
	    tv = پشته_TV_BOT(0);
	    tv->v_type = VAR_عدد;
	    tv->vval.v_عدد = blob_get(blob,
					 idxtv->vval.v_عدد);
	    ++ectx->ec_پشته.ga_len;
	}
    }
    وگرنه
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	بازگشت FAIL;
    }

    اگر (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_end;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    وگرنه
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = پشته_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_عدد = ectx->ec_funcrefs.ga_len;
    }

    بازگشت OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
ایستا loopvars_T *first_loopvars = پوچ;

    ایستا تهی
add_loopvars_to_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    اگر (first_loopvars != پوچ)
	first_loopvars->lvs_prev = loopvars;
    loopvars->lvs_next = first_loopvars;
    loopvars->lvs_prev = پوچ;
    first_loopvars = loopvars;
}

    ایستا تهی
remove_loopvars_from_list(loopvars_T *loopvars)
{
    اگر (loopvars->lvs_prev == پوچ)
	first_loopvars = loopvars->lvs_next;
    وگرنه
	loopvars->lvs_prev->lvs_next = loopvars->lvs_next;
    اگر (loopvars->lvs_next != پوچ)
	loopvars->lvs_next->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    ایستا صحیح
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = پشته_TV_VAR(endloop->end_funcref_idx);
    صحیح		prev_closure_شمار = tv_refcount->vval.v_عدد;
    صحیح		depth = endloop->end_depth;
    garray_T	*gap = &ectx->ec_funcrefs;
    صحیح		closure_in_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *پشته;
    صحیح		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    برای (idx = prev_closure_شمار; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	اگر (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == پوچ)
	{
	    صحیح refcount = pt->pt_refcount;
	    صحیح i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    برای (i = 0; i < endloop->end_var_شمار; ++i)
	    {
		typval_T *stv = پشته_TV_VAR(endloop->end_var_idx + i);

		اگر (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    اگر (refcount > 1)
	    {
		closure_in_use = TRUE;
		بشکن;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    اگر (!closure_in_use)
	بازگشت OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    اگر (loopvars == پوچ)
	بازگشت FAIL;

    loopvars->lvs_ga.ga_len = endloop->end_var_شمار;
    پشته = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_data = پشته;
    اگر (پشته == پوچ)
    {
	vim_free(loopvars);
	بازگشت FAIL;
    }
    add_loopvars_to_list(loopvars);

    // Move the variable values.
    برای (idx = 0; idx < endloop->end_var_شمار; ++idx)
    {
	typval_T *tv = پشته_TV_VAR(endloop->end_var_idx + idx);

	*(پشته + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    برای (idx = prev_closure_شمار; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	اگر (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == پوچ)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[depth] = loopvars;

	    pt->pt_outer.out_loop[depth].پشته = &loopvars->lvs_ga;
	    pt->pt_outer.out_loop[depth].var_idx -=
		  ectx->ec_frame_idx + پشته_FRAME_SIZE + endloop->end_var_idx;
	}
    }

    بازگشت OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    صحیح
loopvars_check_refcount(loopvars_T *loopvars)
{
    صحیح		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    صحیح		    done = 0;
	typval_T	*پشته = gap->ga_data;

    اگر (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	بازگشت FALSE;
    برای (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_data) + i;

	اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != پوچ
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    صحیح	    depth;

	    برای (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		اگر (tv->vval.v_partial->pt_loopvars[depth] == loopvars)
		    ++done;
	}
    }
    اگر (done != loopvars->lvs_min_refcount)
	بازگشت FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    پشته = gap->ga_data;
    برای (i = 0; i < gap->ga_len; ++i)
	clear_tv(پشته + i);
    vim_free(پشته);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    بازگشت TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    صحیح
set_ref_in_loopvars(صحیح copyID)
{
    loopvars_T *loopvars;

    برای (loopvars = first_loopvars; loopvars != پوچ;
						 loopvars = loopvars->lvs_next)
    {
	typval_T    *پشته = loopvars->lvs_ga.ga_data;
	صحیح	    i;

	برای (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    اگر (set_ref_in_item(پشته + i, copyID, پوچ, پوچ, پوچ))
		بازگشت TRUE;  // abort
    }
    بازگشت FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    ایستا صحیح
load_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = پوچ;
    hashtab_T	*ht = پوچ;
    نویسه	namespace;

    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
	بازگشت NOTDONE;
    انتخاب (isn_type)
    {
	حالت ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    بشکن;
	حالت ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    بشکن;
	حالت ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    بشکن;
	حالت ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    بشکن;
	پیشفرض:  // Cannot reach here
	    بازگشت NOTDONE;
    }
    di = find_var_in_ht(ht, 0, iptr->isn_arg.string, TRUE);

    اگر (di == پوچ)
    {
	اگر (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = find_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    اگر (ufunc != پوچ)
	    {
		typval_T    *tv = پشته_TV_BOT(0);

		++ectx->ec_پشته.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		اگر (tv->vval.v_string == پوچ)
		    بازگشت FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		بازگشت OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	اگر (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != پوچ)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_item_not_found_in_script_str), iptr->isn_arg.string);
	وگرنه
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = پشته_TV_BOT(0);
	tv->v_type = VAR_عدد;
	tv->v_lock = 0;
	tv->vval.v_عدد = 0;
	++ectx->ec_پشته.ga_len;
	بازگشت FAIL;
    }
    وگرنه
    {
	copy_tv(&di->di_tv, پشته_TV_BOT(0));
	++ectx->ec_پشته.ga_len;
    }
    بازگشت OK;
}


    ایستا تهی
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, اندازه(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    نویسه *tofree = پوچ;
    نویسه *typename = type_name(type, &tofree);
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
    ایستا صحیح
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    صحیح		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = پوچ;
    typval_T	mtv;

    اگر (is_object)
    {
	obj = tv->vval.v_object;
	اگر (obj == پوچ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    بازگشت FAIL;
	}
	tv_cl = obj->obj_class;
    }
    وگرنه
    {
	tv_cl = tv->vval.v_class;
	اگر (tv_cl == پوچ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    بازگشت FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    صحیح		namelen = (صحیح)STRLEN(iptr->isn_arg.string);
    صحیح		save_did_emsg = did_emsg;

    اگر (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	بازگشت OK;
    }

    اگر (did_emsg != save_did_emsg)
	بازگشت FAIL;

    // could be a class or instance method
    ufunc_T	*oc_method;
    صحیح		oc_method_idx;

    oc_method = method_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_method_idx);
    اگر (oc_method == پوچ)
    {
	نویسه	*پیام;

	SOURCING_LNUM = iptr->isn_lnum;
	اگر (is_object)
	    پیام = e_variable_not_found_on_object_str_str;
	وگرنه
	    پیام = e_class_variable_str_not_found_in_class_str;
	semsg(_(پیام), iptr->isn_arg.string, tv_cl->class_name.string);
	بازگشت FAIL;
    }

    // Protected methods are not accessible outside the class
    اگر (*oc_method->uf_name == '_'
			&& !class_instance_of(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_method_str), oc_method->uf_name);
	بازگشت FAIL;
    }

    // Create a partial for the instance or class method
    اگر (obj_method_to_partial_tv(is_object ? obj : پوچ, oc_method, tv)
								== FAIL)
	بازگشت FAIL;

    بازگشت OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    ایستا تهی
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, صحیح fixindent)
{
    صحیح		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*عبارت = پوچ;
    صحیح		dir = FORWARD;

    اگر (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = پشته_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_عدد;
	اگر (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_پشته.ga_len;
    }
    وگرنه اگر (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    وگرنه اگر (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	اگر (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    اگر (regname == '=')
    {
	tv = پشته_TV_BOT(-1);
	اگر (tv->v_type == VAR_STRING)
	    عبارت = tv->vval.v_string;
	وگرنه
	{
	    عبارت = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_پشته.ga_len;
    }
    check_cursor();

    اگر (fixindent)
	do_put(regname, عبارت, dir, 1L, PUT_سطر|PUT_CURSLINE|PUT_FIXINDENT);
    وگرنه
	do_put(regname, عبارت, dir, 1L, PUT_سطر|PUT_CURSLINE);
    vim_free(عبارت);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    ایستا صحیح
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    صحیح		شمار = iptr->isn_arg.unpack.unp_شمار;
    صحیح		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    صحیح		i;

    l = tv->vval.v_list;
    اگر (l == پوچ
	    || l->lv_len < (semicolon ? شمار - 1 : شمار))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_مقدار_does_not_have_enough_اقلام));
	بازگشت EXEC_FAIL;
    }
    وگرنه اگر (!semicolon && l->lv_len > شمار)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_مقدار_has_more_اقلام_than_targets));
	بازگشت EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    اگر (GA_GROW_FAILS(&ectx->ec_پشته, شمار - 1))
	بازگشت EXEC_DONE;
    ectx->ec_پشته.ga_len += شمار - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    اگر (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_اقلام(l->lv_len - شمار + 1);

	اگر (rem_list == پوچ)
	    بازگشت EXEC_DONE;
	tv = پشته_TV_BOT(-شمار);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	li = l->lv_first;
	برای (i = 0; i < شمار - 1; ++i)
	    li = li->li_next;
	برای (i = 0; li != پوچ; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_set_item(rem_list, i, &tvcopy);
	    li = li->li_next;
	}
	--شمار;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_first;
    برای (i = 0; i < شمار; ++i)
    {
	tv = پشته_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_next;
    }

    list_unref(l);

    بازگشت EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    ایستا صحیح
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    صحیح		شمار = iptr->isn_arg.unpack.unp_شمار;
    صحیح		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    صحیح		i;

    tuple = tv->vval.v_tuple;
    اگر (tuple == پوچ
	    || TUPLE_LEN(tuple) < (semicolon ? شمار - 1 : شمار))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_more_targets_than_tuple_اقلام));
	بازگشت EXEC_FAIL;
    }
    وگرنه اگر (!semicolon && TUPLE_LEN(tuple) > شمار)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_اقلام));
	بازگشت EXEC_FAIL;
    }

    اگر (GA_GROW_FAILS(&ectx->ec_پشته, شمار - 1))
	بازگشت EXEC_DONE;
    ectx->ec_پشته.ga_len += شمار - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    اگر (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_اقلام(TUPLE_LEN(tuple) - شمار + 1);

	اگر (rem_list == پوچ)
	    بازگشت EXEC_DONE;
	tv = پشته_TV_BOT(-شمار);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	برای (i = شمار - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ITEM(tuple, i), &tvcopy);
	    list_set_item(rem_list, i - (شمار - 1), &tvcopy);
	}
	--شمار;
    }

    // Produce the values in reverse order, first item last.
    برای (i = 0; i < شمار; ++i)
    {
	tv = پشته_TV_BOT(-i - 1);
	copy_tv(TUPLE_ITEM(tuple, i), tv);
    }

    tuple_unref(tuple);

    بازگشت EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    ایستا صحیح
exec_instructions(ectx_T *ectx)
{
    صحیح		ret = FAIL;
    صحیح		save_trylevel_at_start = ectx->ec_trylevel_at_start;
    صحیح		dict_پشته_len_at_start = dict_پشته.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_start = trylevel;

    برای (;;)
    {
	ایستا صحیح  breakcheck_شمار = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	اگر (unlikely(++breakcheck_شمار >= 100))
	{
	    سطر_breakcheck();
	    breakcheck_شمار = 0;
	}
	اگر (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    اگر (throw_exception("Vim:Interrupt", ET_INTERRUPT, پوچ) == FAIL)
		برو theend;
	    did_throw = TRUE;
	}

	اگر (unlikely(did_emsg && پیام_list != پوچ && *پیام_list != پوچ))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    اگر (throw_exception(*پیام_list, ET_ERROR, پوچ) == FAIL)
		برو theend;
	    did_throw = TRUE;
	    *پیام_list = پوچ;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    اگر (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		اگر (trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		{
		    اگر (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			finish_exception(caught_پشته);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	اگر (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = پوچ;
	    صحیح		index = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    وقتی (index > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_data) + index - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		اگر (!trycmd->tcd_in_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    بشکن;
		// In the catch and finally block of this try we have to go up
		// one level.
		--index;
		trycmd = پوچ;
	    }
	    اگر (trycmd != پوچ && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
	    {
		اگر (trycmd->tcd_in_catch)
		{
		    اگر (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    وگرنه
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		وگرنه
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_in_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    وگرنه
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    برو theend;
		tv = پشته_TV_BOT(0);
		tv->v_type = VAR_عدد;
		tv->vval.v_عدد = 0;
		++ectx->ec_پشته.ga_len;
		اگر (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    اگر (handle_closure_in_use(ectx, FALSE) == FAIL)
			برو theend;
		    برو done;
		}

		اگر (func_return(ectx) == FAIL)
		    برو theend;
	    }
	    ادامه;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	انتخاب (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    حالت ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = پشته_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		بشکن;

	    // execute Ex command line
	    حالت ISN_EXEC:
		اگر (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    برو on_error;
		بشکن;

	    // execute Ex command line split at NL characters.
	    حالت ISN_EXEC_SPLIT:
		{
		    source_cookie_T cookie;
		    char_u	    *سطر;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    سطر = get_split_sourceline(0, &cookie, 0, 0);
		    اگر (do_cmdline(سطر,
				get_split_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(سطر);
			برو on_error;
		    }
		    vim_free(سطر);
		}
		بشکن;

	    // execute Ex command line that is only a range
	    حالت ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    نویسه	*error = پوچ;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = CMD_SIZE;
		    ea.addr_type = ADDR_سطرها;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_address(&ea, &error, FALSE);
		    اگر (ea.cmd == پوچ)
			برو on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    اگر (error != پوچ)
		    {
			emsg(error);
			برو on_error;
		    }
		}
		بشکن;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    حالت ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    صحیح	    res;
		    صحیح	    save_flags = cmdmod.cmod_flags;

		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    tv = پشته_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, پوچ, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    اگر (res == FAIL)
			برو on_error;
		    ++ectx->ec_پشته.ga_len;
		}
		بشکن;

	    // push typeval VAR_INSTR with instructions to be executed
	    حالت ISN_INSTR:
		{
		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    tv = پشته_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    اگر (tv->vval.v_instr == پوچ)
			برو on_error;
		    ++ectx->ec_پشته.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		بشکن;

	    حالت ISN_SOURCE:
		{
		    صحیح notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (may_load_script((صحیح)iptr->isn_arg.عدد, &notused)
								       == FAIL)
			برو on_error;
		}
		بشکن;

	    // execute :substitute with an expression
	    حالت ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    source_cookie_T	cookie;
		    ساختار subs_عبارت_S	*save_instr = substitute_instr;
		    ساختار subs_عبارت_S	subs_instr;
		    صحیح			res;

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

		    اگر (res == FAIL || did_emsg
					     || subs_instr.subs_status == FAIL)
			برو on_error;
		}
		بشکن;

	    حالت ISN_FINISH:
		برو done;

	    حالت ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		اگر (alloc_redir_lval() == FAIL)
		    برو on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		بشکن;

	    حالت ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    {
			vim_free(res);
			برو theend;
		    }
		    tv = پشته_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_پشته.ga_len;
		}
		بشکن;

	    حالت ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		force_abort = TRUE;
		اگر (trigger_cexpr_autocmd(iptr->isn_arg.عدد) == FAIL)
		    برو on_error;
		force_abort = FALSE;
#endif
		بشکن;

	    حالت ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    exarg_T ea;
		    صحیح	    res;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_پشته.ga_len;
		    tv = پشته_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    اگر (res == FAIL)
			برو on_error;
		}
#endif
		بشکن;

	    // execute Ex command from pieces on the stack
	    حالت ISN_EXECCONCAT:
		{
		    صحیح	    شمار = iptr->isn_arg.عدد;
		    size_t  len = 0;
		    صحیح	    pass;
		    صحیح	    i;
		    char_u  *cmd = پوچ;
		    char_u  *str;

		    برای (pass = 1; pass <= 2; ++pass)
		    {
			برای (i = 0; i < شمار; ++i)
			{
			    tv = پشته_TV_BOT(i - شمار);
			    str = tv->vval.v_string;
			    اگر (str != پوچ && *str != NUL)
			    {
				اگر (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    اگر (pass == 2)
				clear_tv(tv);
			}
			اگر (pass == 1)
			{
			    cmd = alloc(len + 1);
			    اگر (unlikely(cmd == پوچ))
				برو theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		بشکن;

	    // execute :echo {string} ...
	    حالت ISN_ECHO:
		{
		    صحیح شمار = iptr->isn_arg.echo.echo_شمار;
		    صحیح	atstart = TRUE;
		    صحیح needclr = TRUE;
		    صحیح	idx;

		    برای (idx = 0; idx < شمار; ++idx)
		    {
			tv = پشته_TV_BOT(idx - شمار);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    اگر (needclr)
			پیام_clr_eos();
		    ectx->ec_پشته.ga_len -= شمار;
		}
		بشکن;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    حالت ISN_EXECUTE:
	    حالت ISN_ECHOMSG:
	    حالت ISN_ECHOWINDOW:
	    حالت ISN_ECHOCONSOLE:
	    حالت ISN_ECHOERR:
		{
		    صحیح		شمار;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    صحیح		len;
		    صحیح		failed = FALSE;
		    صحیح		idx;

		    اگر (iptr->isn_type == ISN_ECHOWINDOW)
			شمار = iptr->isn_arg.echowin.ewin_شمار;
		    وگرنه
			شمار = iptr->isn_arg.عدد;
		    ga_init2(&ga, 1, 80);
		    برای (idx = 0; idx < شمار; ++idx)
		    {
			tv = پشته_TV_BOT(idx - شمار);
			اگر (iptr->isn_type == ISN_EXECUTE)
			{
			    اگر (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_JOB)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_مقدار_as_string_str),
						    vartype_name(tv->v_type));
				بشکن;
			    }
			    وگرنه
				p = tv_get_string_buf(tv, buf);
			}
			وگرنه
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (صحیح)STRLEN(p);
			اگر (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			وگرنه
			{
			    اگر (ga.ga_len > 0)
				((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_پشته.ga_len -= شمار;
		    اگر (failed)
		    {
			ga_clear(&ga);
			برو on_error;
		    }

		    اگر (ga.ga_data != پوچ)
		    {
			اگر (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_data);
			    اگر (did_emsg)
			    {
				ga_clear(&ga);
				برو on_error;
			    }
			}
			وگرنه
			{
			    پیام_sb_eol();
			    اگر (iptr->isn_type == ISN_ECHOMSG)
			    {
				پیام_attr(ga.ga_data, echo_attr);
				out_flush();
			    }
#ifdef HAS_MESSAGE_WINDOW
			    وگرنه اگر (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				start_echowindow(
					      iptr->isn_arg.echowin.ewin_time);
				پیام_attr(ga.ga_data, echo_attr);
				end_echowindow();
			    }
#endif
			    وگرنه اگر (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_data, (صحیح)STRLEN(ga.ga_data),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    وگرنه
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_data);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		بشکن;

	    // load local variable or argument
	    حالت ISN_LOAD:
		اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    برو theend;
		tv = پشته_TV_VAR(iptr->isn_arg.عدد);
		اگر (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    پشته_TV_BOT(0)->v_type = VAR_SPECIAL;
		    پشته_TV_BOT(0)->vval.v_عدد = VVAL_NONE;
		}
		وگرنه
		    copy_tv(tv, پشته_TV_BOT(0));
		++ectx->ec_پشته.ga_len;
		بشکن;

	    // load v: variable
	    حالت ISN_LOADV:
		اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    برو theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.عدد), پشته_TV_BOT(0));
		++ectx->ec_پشته.ga_len;
		بشکن;

	    // load s: variable in Vim9 script
	    حالت ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    اگر (sv == پوچ)
			برو theend;
		    allocate_if_null(sv);
		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    copy_tv(sv->sv_tv, پشته_TV_BOT(0));
		    ++ectx->ec_پشته.ga_len;
		}
		بشکن;

	    // load s: variable in old script or autoload import
	    حالت ISN_LOADS:
	    حالت ISN_LOADEXPORT:
		{
		    صحیح		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = find_var_in_ht(ht, 0, name, TRUE);

		    اگر (di == پوچ)
		    {
			ufunc_T	*ufunc = پوچ;

			اگر (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (تهی)find_exported(sid, name, &ufunc, &type,
							 پوچ, پوچ, FALSE);
			}
			اگر (ufunc == پوچ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    برو on_error;
			}
			اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			    برو theend;
			tv = پشته_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_پشته.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    وگرنه
		    {
			اگر (iptr->isn_type == ISN_LOADEXPORT)
			{
			    صحیح idx = get_script_item_idx(sid, name, 0,
								   پوچ, پوچ);
			    svar_T	*sv;

			    اگر (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;
				اگر ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    برو on_error;
				}
			    }
			}
			اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			    برو theend;
			copy_tv(&di->di_tv, پشته_TV_BOT(0));
			++ectx->ec_پشته.ga_len;
		    }
		}
		بشکن;

	    // load g:/b:/w:/t: variable
	    حالت ISN_LOADG:
	    حالت ISN_LOADB:
	    حالت ISN_LOADW:
	    حالت ISN_LOADT:
		{
		    صحیح res = load_namespace_var(ectx, iptr->isn_type, iptr);

		    اگر (res == NOTDONE)
			برو theend;
		    اگر (res == FAIL)
			برو on_error;
		}

		بشکن;

	    // load autoload variable
	    حالت ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (eval_variable(name, (صحیح)STRLEN(name), 0,
			      پشته_TV_BOT(0), پوچ, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = پشته_TV_BOT(0);
			tv->v_type = VAR_عدد;
			tv->v_lock = 0;
			tv->vval.v_عدد = 0;
			++ectx->ec_پشته.ga_len;
			برو on_error;
		    }
		    ++ectx->ec_پشته.ga_len;
		}
		بشکن;

	    // load g:/b:/w:/t: namespace
	    حالت ISN_LOADGDICT:
	    حالت ISN_LOADBDICT:
	    حالت ISN_LOADWDICT:
	    حالت ISN_LOADTDICT:
		{
		    dict_T *d = پوچ;

		    انتخاب (iptr->isn_type)
		    {
			حالت ISN_LOADGDICT: d = get_globvar_dict(); بشکن;
			حالت ISN_LOADBDICT: d = curbuf->b_vars; بشکن;
			حالت ISN_LOADWDICT: d = curwin->w_vars; بشکن;
			حالت ISN_LOADTDICT: d = curtab->tp_vars; بشکن;
			پیشفرض:  // Cannot reach here
			    برو theend;
		    }
		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    tv = پشته_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_lock = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_پشته.ga_len;
		}
		بشکن;

	    // load &option
	    حالت ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    اگر (eval_option(&name, &optval, TRUE) == FAIL)
			برو theend;
		    *پشته_TV_BOT(0) = optval;
		    ++ectx->ec_پشته.ga_len;
		}
		بشکن;

	    // load $ENV
	    حالت ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    // name is always valid, checked when compiling
		    (تهی)eval_env_var(&name, &optval, TRUE);
		    *پشته_TV_BOT(0) = optval;
		    ++ectx->ec_پشته.ga_len;
		}
		بشکن;

	    // load @register
	    حالت ISN_LOADREG:
		اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    برو theend;
		tv = پشته_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.عدد, GREG_عبارت_SRC);
		++ectx->ec_پشته.ga_len;
		بشکن;

	    // store local variable
	    حالت ISN_STORE:
		--ectx->ec_پشته.ga_len;
		tv = پشته_TV_VAR(iptr->isn_arg.عدد);
		اگر (check_typval_is_مقدار(پشته_TV_BOT(0)) == FAIL
			|| پشته_TV_BOT(0)->v_type == VAR_VOID)
		{
		    اگر (پشته_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_مقدار));
		    clear_tv(پشته_TV_BOT(0));
		    برو on_error;
		}
		clear_tv(tv);
		*tv = *پشته_TV_BOT(0);
		بشکن;

	    // store s: variable in old script or autoload import
	    حالت ISN_STORES:
	    حالت ISN_STOREEXPORT:
		{
		    صحیح		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = پوچ;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    اگر (iptr->isn_type == ISN_STOREEXPORT)
			di = find_var_autoload_prefix(name, sid, پوچ, پوچ);
		    // Then look for a variable in the script's variables.
		    اگر (di == پوچ)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = find_var_in_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_پشته.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (di == پوچ)
		    {
			اگر (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(پشته_TV_BOT(0));
			    برو on_error;
			}
			store_var(name, پشته_TV_BOT(0));
		    }
		    وگرنه
		    {
			اگر (iptr->isn_type == ISN_STOREEXPORT)
			{
			    صحیح idx = get_script_item_idx(sid, name, 0,
								   پوچ, پوچ);

			    // can this ever fail?
			    اگر (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;

				اگر ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    clear_tv(پشته_TV_BOT(0));
				    برو on_error;
				}
			    }
			}
			اگر (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(پشته_TV_BOT(0));
			    برو on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *پشته_TV_BOT(0);
		    }
		}
		بشکن;

	    // store script-local variable in Vim9 script
	    حالت ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    اگر (sv == پوچ)
			برو theend;
		    --ectx->ec_پشته.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (مقدار_check_lock(sv->sv_tv->v_lock, sv->sv_name, FALSE))
		    {
			clear_tv(پشته_TV_BOT(0));
			برو on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *پشته_TV_BOT(0);
		}
		بشکن;

	    // store option
	    حالت ISN_STOREOPT:
	    حالت ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    صحیح		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    long	n = 0;
		    char_u	*s = پوچ;
		    نویسه	*پیام;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = پوچ;

		    --ectx->ec_پشته.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = پشته_TV_BOT(0);
		    اگر (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			اگر (s == پوچ)
			    s = (char_u *)"";
		    }
		    وگرنه اگر (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			اگر (s == پوچ || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    برو on_error;
			}
		    }
		    وگرنه
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_عدد;
		    پیام = set_option_مقدار(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    اگر (پیام != پوچ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(پیام));
			برو on_error;
		    }
		}
		بشکن;

	    // store $ENV
	    حالت ISN_STOREENV:
		اگر (check_secure() || check_restricted())
		    برو theend;
		--ectx->ec_پشته.ga_len;
		tv = پشته_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		بشکن;

	    // store @r
	    حالت ISN_STOREREG:
		{
		    صحیح	reg = iptr->isn_arg.عدد;

		    --ectx->ec_پشته.ga_len;
		    tv = پشته_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		بشکن;

	    // store v: variable
	    حالت ISN_STOREV:
		--ectx->ec_پشته.ga_len;
		اگر (set_vim_var_tv(iptr->isn_arg.عدد, پشته_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    برو on_error;
		بشکن;

	    // store g:/b:/w:/t: variable
	    حالت ISN_STOREG:
	    حالت ISN_STOREB:
	    حالت ISN_STOREW:
	    حالت ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    انتخاب (iptr->isn_type)
		    {
			حالت ISN_STOREG:
			    ht = get_globvar_ht();
			    بشکن;
			حالت ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    بشکن;
			حالت ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    بشکن;
			حالت ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    بشکن;
			پیشفرض:  // Cannot reach here
			    برو theend;
		    }

		    --ectx->ec_پشته.ga_len;
		    di = find_var_in_ht(ht, 0, name, TRUE);
		    اگر (di == پوچ)
			store_var(iptr->isn_arg.string, پشته_TV_BOT(0));
		    وگرنه
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			اگر (var_check_permission(di, name) == FAIL)
			    برو on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *پشته_TV_BOT(0);
		    }
		}
		بشکن;

	    // store an autoload variable
	    حالت ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		set_var(iptr->isn_arg.string, پشته_TV_BOT(-1), TRUE);
		clear_tv(پشته_TV_BOT(-1));
		--ectx->ec_پشته.ga_len;
		بشکن;

	    // store number in local variable
	    حالت ISN_STORENR:
		tv = پشته_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_عدد;
		tv->vval.v_عدد = iptr->isn_arg.storenr.stnr_val;
		بشکن;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    حالت ISN_STOREINDEX:
		{
		    صحیح res = execute_storeindex(iptr, ectx);

		    اگر (res == FAIL)
			برو on_error;
		    اگر (res == NOTDONE)
			برو theend;
		}
		بشکن;

	    // store value in list or blob range
	    حالت ISN_STORERANGE:
		اگر (execute_storerange(iptr, ectx) == FAIL)
		    برو on_error;
		بشکن;

	    حالت ISN_LOAD_CLASSMEMBER:
		{
		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      پشته_TV_BOT(0));
		    ++ectx->ec_پشته.ga_len;
		}
		بشکن;

	    حالت ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *پشته_TV_BOT(-1);
		    --ectx->ec_پشته.ga_len;
		}
		بشکن;

	    // Load or store variable or argument from outer scope.
	    حالت ISN_LOADOUTER:
	    حالت ISN_STOREOUTER:
		{
		    صحیح		depth = iptr->isn_arg.outer.outer_depth;
		    outer_T	*outer = ectx->ec_outer_ref == پوچ ? پوچ
						: ectx->ec_outer_ref->or_outer;

		    وقتی (depth > 1 && outer != پوچ)
		    {
			outer = outer->out_up;
			--depth;
		    }
		    اگر (outer == پوچ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			اگر (ectx->ec_frame_idx == ectx->ec_initial_frame_idx
						 || ectx->ec_outer_ref == پوچ)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_closure_called_from_invalid_context));
			وگرنه
			    iemsg("LOADOUTER depth more than scope levels");
			برو theend;
		    }
		    اگر (depth < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->out_loop[-depth - 1]
							       .پشته->ga_data)
					  + outer->out_loop[-depth - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    وگرنه
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->out_پشته->ga_data)
				      + outer->out_frame_idx + پشته_FRAME_SIZE
				      + iptr->isn_arg.outer.outer_idx;
		    اگر (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			    برو theend;
			// careful: ga_grow_inner may re-alloc the stack
			اگر (depth < 0)
			    copy = ((typval_T *)outer->out_loop[-depth - 1]
								   .پشته->ga_data)
					      + outer->out_loop[-depth - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			وگرنه
			    copy = ((typval_T *)outer->out_پشته->ga_data)
					  + outer->out_frame_idx + پشته_FRAME_SIZE
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			اگر (copy != tv)
			    tv = copy;
			copy_tv(tv, پشته_TV_BOT(0));
			++ectx->ec_پشته.ga_len;
		    }
		    وگرنه
		    {
			--ectx->ec_پشته.ga_len;
			clear_tv(tv);
			*tv = *پشته_TV_BOT(0);
		    }
		}
		بشکن;

	    // unlet item in list or dict variable
	    حالت ISN_UNLETINDEX:
		اگر (execute_unletindex(iptr, ectx) == FAIL)
		    برو on_error;
		بشکن;

	    // unlet range of items in list variable
	    حالت ISN_UNLETRANGE:
		اگر (execute_unletrange(iptr, ectx) == FAIL)
		    برو on_error;
		بشکن;

	    // push constant
	    حالت ISN_PUSHNR:
	    حالت ISN_PUSHBOOL:
	    حالت ISN_PUSHSPEC:
	    حالت ISN_PUSHF:
	    حالت ISN_PUSHS:
	    حالت ISN_PUSHBLOB:
	    حالت ISN_PUSHFUNC:
	    حالت ISN_PUSHCHANNEL:
	    حالت ISN_PUSHJOB:
	    حالت ISN_PUSHOBJ:
	    حالت ISN_PUSHCLASS:
		اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    برو theend;
		tv = پشته_TV_BOT(0);
		tv->v_lock = 0;
		++ectx->ec_پشته.ga_len;
		انتخاب (iptr->isn_type)
		{
		    حالت ISN_PUSHNR:
			tv->v_type = VAR_عدد;
			tv->vval.v_عدد = iptr->isn_arg.عدد;
			بشکن;
		    حالت ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_عدد = iptr->isn_arg.عدد;
			بشکن;
		    حالت ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_عدد = iptr->isn_arg.عدد;
			بشکن;
		    حالت ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			بشکن;
		    حالت ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			بشکن;
		    حالت ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			اگر (iptr->isn_arg.string == پوچ)
			    tv->vval.v_string = پوچ;
			وگرنه
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			بشکن;
		    حالت ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = پوچ;
#endif
			بشکن;
		    حالت ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_JOB;
			tv->vval.v_job = پوچ;
#endif
			بشکن;
		    حالت ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = پوچ;
			بشکن;
		    حالت ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			بشکن;
		    پیشفرض:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == پوچ
				    ? پوچ : vim_strsave(iptr->isn_arg.string);
		}
		بشکن;

	    حالت ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (تهی)script_autoload(name, FALSE);
		    اگر (find_func(name, TRUE))
		    {
			اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			    برو theend;
			tv = پشته_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_پشته.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    وگرنه
		    {
			صحیح res = load_namespace_var(ectx, ISN_LOADG, iptr);

			اگر (res == NOTDONE)
			    برو theend;
			اگر (res == FAIL)
			    برو on_error;
		    }
		}
		بشکن;

	    حالت ISN_UNLET:
		اگر (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    برو on_error;
		بشکن;
	    حالت ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		بشکن;

	    حالت ISN_LOCKUNLOCK:
		{
#ifdef LOG_LOCKVAR
		    ch_log(پوچ, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#endif
		    lval_ریشه_T	*lval_ریشه_save = lval_ریشه;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_پشته.ga_len;
		    lval_ریشه_T ریشه;
		    ریشه.lr_tv      = پشته_TV_BOT(0);
		    ریشه.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    ریشه.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_ریشه = &ریشه;
		    صحیح res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(ریشه.lr_tv);
		    lval_ریشه = lval_ریشه_save;
		    اگر (res == FAIL)
			برو on_error;
		}
		بشکن;

	    حالت ISN_LOCKCONST:
		item_lock(پشته_TV_BOT(-1), 100, TRUE, TRUE);
		بشکن;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    حالت ISN_NEWLIST:
		اگر (exe_newlist(iptr->isn_arg.عدد, ectx) == FAIL)
		    برو theend;
		بشکن;

	    // create a tuple from items on the stack
	    حالت ISN_NEWTUPLE:
		اگر (exe_newtuple(iptr->isn_arg.عدد, ectx) == FAIL)
		    برو theend;
		بشکن;

	    // create a dict from items on the stack
	    حالت ISN_NEWDICT:
		{
		    صحیح res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.عدد, ectx);
		    اگر (res == FAIL)
			برو theend;
		    اگر (res == MAYBE)
			برو on_error;
		}
		بشکن;

	    حالت ISN_CONCAT:
		اگر (exe_concat(iptr->isn_arg.عدد, ectx) == FAIL)
		    برو theend;
		بشکن;

	    // create a partial with NULL value
	    حالت ISN_NEWPARTIAL:
		اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    برو theend;
		++ectx->ec_پشته.ga_len;
		tv = پشته_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_lock = 0;
		tv->vval.v_partial = پوچ;
		بشکن;

	    // call a :def function
	    حالت ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		اگر (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				پوچ,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    برو on_error;
		بشکن;

	    // call a method on an interface
	    حالت ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = پشته_TV_BOT(-1 - mfunc->cmf_argcount);
		    اگر (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			برو on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    اگر (obj == پوچ)
		    {
			emsg(_(e_using_null_object));
			برو on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    اگر (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    وگرنه
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			صحیح idx = object_index_from_itf_index(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    اگر (mfunc->cmf_ufunc != پوچ && IS_GENERIC_FUNC(ufunc))
		    {
			اگر (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			وگرنه
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    اگر (ufunc == پوچ)
				برو on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    اگر (call_ufunc(ufunc, پوچ, mfunc->cmf_argcount, ectx,
							   پوچ, پوچ) == FAIL)
			برو on_error;
		}
		بشکن;

	    // call a builtin function
	    حالت ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		اگر (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    برو on_error;
		بشکن;

	    // call a funcref or partial
	    حالت ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    صحیح		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (pfunc->cpf_بالاترین)
		    {
			// funcref is above the arguments
			tv = پشته_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    وگرنه
		    {
			// Get the funcref from the stack.
			--ectx->ec_پشته.ga_len;
			partial_tv = *پشته_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    اگر (tv == &partial_tv)
			clear_tv(&partial_tv);
		    اگر (r == FAIL)
			برو on_error;
		}
		بشکن;

	    حالت ISN_PCALL_END:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_پشته.ga_len;
		clear_tv(پشته_TV_BOT(-1));
		*پشته_TV_BOT(-1) = *پشته_TV_BOT(0);
		بشکن;

	    // call a user defined function or funcref/partial
	    حالت ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			برو on_error;
		}
		بشکن;

	    // :defer func(arg)
	    حالت ISN_DEFER:
		اگر (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    برو on_error;
		بشکن;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    حالت ISN_RETURN_VOID:
	    حالت ISN_RETURN_OBJECT:
		اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    برو theend;
		tv = پشته_TV_BOT(0);
		++ectx->ec_پشته.ga_len;
		اگر (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_عدد = 0;
		    tv->v_lock = 0;
		}
		وگرنه
		{
		    *tv = *پشته_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_lock_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    حالت ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = پوچ;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    اگر (check_typval_is_مقدار(پشته_TV_BOT(-1)) == FAIL)
			برو theend;

		    اگر (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    اگر (trycmd != پوچ
				 && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    {
			// jump to ":finally" or ":endtry"
			اگر (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			وگرنه
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    وگرنه
			برو func_return;
		}
		بشکن;

	    // push a partial, a reference to a compiled function
	    حالت ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    اگر (pt == پوچ)
			برو theend;
		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    {
			vim_free(pt);
			برو theend;
		    }
		    اگر (extra != پوچ && extra->fre_class != پوچ)
		    {
			class_T	*cl;
			اگر (extra->fre_object_method)
			{
			    tv = پشته_TV_BOT(-1);
			    اگر (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				برو on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    اگر (obj == پوچ)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				برو on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_پشته.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			وگرنه
			    cl = extra->fre_class;

			اگر (extra->fre_object_method)
			{
			    // object method
			    // convert the interface index to the object index
			    صحیح idx =
				object_index_from_itf_index(extra->fre_class,
					TRUE, extra->fre_method_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			وگرنه
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_method_idx];
			}
		    }
		    وگرنه اگر (extra == پوچ || extra->fre_func_name == پوچ)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    وگرنه
		    {
			ufunc = find_func(extra->fre_func_name, FALSE);
		    }
		    اگر (ufunc == پوچ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			برو theend;
		    }
		    اگر (extra != پوچ && extra->fre_outer_depth > 0
					   && (ufunc->uf_flags & FC_CLOSURE))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == پوچ
				    ? پوچ : ectx->ec_outer_ref->or_partial;

			برای (صحیح depth = 1; outer_pt != پوچ
				    && depth < extra->fre_outer_depth; ++depth)
			    outer_pt = get_pt_outer(outer_pt)->out_up_partial;
			اگر (outer_pt == پوچ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    برو theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    وگرنه اگر (fill_partial_and_closure(pt, ufunc,
			       extra == پوچ ? پوچ : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			برو theend;
		    }
		    tv = پشته_TV_BOT(0);
		    ++ectx->ec_پشته.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_lock = 0;
		}
		بشکن;

	    // Create a global function from a lambda.
	    حالت ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    اگر (copy_lambda_to_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			برو theend;
		}
		بشکن;

	    // List functions
	    حالت ISN_DEF:
		اگر (iptr->isn_arg.string == پوچ)
		    list_functions(پوچ);
		وگرنه
		{
		    exarg_T	ea;
		    garray_T	سطرها_to_free;

		    CLEAR_FIELD(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&سطرها_to_free, اندازه(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, پوچ, &سطرها_to_free, 0, پوچ, 0, پوچ);
		    ga_clear_strings(&سطرها_to_free);
		}
		بشکن;

	    // jump if a condition is met
	    حالت ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    صحیح		error = FALSE;
		    صحیح		jump = TRUE;

		    اگر (when != JUMP_ALWAYS)
		    {
			tv = پشته_TV_BOT(-1);
			اگر (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    اگر (error)
				برو on_error;
			}
			وگرنه
			    jump = tv2bool(tv);
			اگر (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			اگر (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_پشته.ga_len;
			}
		    }
		    اگر (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		بشکن;

	    // "while": jump to end if a condition is false
	    حالت ISN_WHILE:
		{
		    صحیح		error = FALSE;
		    صحیح		jump = TRUE;

		    tv = پشته_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    اگر (error)
			برو on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_پشته.ga_len;
		    اگر (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_end;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = پشته_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_عدد = ectx->ec_funcrefs.ga_len;
		}
		بشکن;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    حالت ISN_JUMP_IF_ARG_SET:
	    حالت ISN_JUMP_IF_ARG_NOT_SET:
		tv = پشته_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		صحیح arg_set = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_عدد == VVAL_NONE);

		اگر (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_SET && !arg_set)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_data)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_args.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ پشته_FRAME_SIZE;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_POINTER(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    اگر (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		اگر (iptr->isn_type == ISN_JUMP_IF_ARG_SET ? arg_set : !arg_set)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		بشکن;

	    // top of a for loop
	    حالت ISN_FOR:
		اگر (execute_for(iptr, ectx) == FAIL)
		    برو theend;
		بشکن;

	    // end of a for or while loop
	    حالت ISN_ENDLOOP:
		اگر (execute_endloop(iptr, ectx) == FAIL)
		    برو theend;
		بشکن;

	    // start of ":try" block
	    حالت ISN_TRY:
		{
		    trycmd_T    *trycmd = پوچ;

		    اگر (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			برو theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_data)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_POINTER(trycmd);
		    trycmd->tcd_frame_idx = ectx->ec_frame_idx;
		    trycmd->tcd_پشته_len = ectx->ec_پشته.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		بشکن;

	    حالت ISN_PUSHEXC:
		اگر (current_exception == پوچ)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    برو theend;
		}
		اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    برو theend;
		tv = پشته_TV_BOT(0);
		++ectx->ec_پشته.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->مقدار);
		بشکن;

	    حالت ISN_CATCH:
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
		بشکن;

	    حالت ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    صحیح		i;
		    trycmd_T    *trycmd;
		    صحیح		iidx = trycont->tct_where;

		    اگر (trystack->ga_len < trycont->tct_levels)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_levels, trystack->ga_len);
			برو theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    برای (i = trycont->tct_levels; i > 0; --i)
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
		بشکن;

	    حالت ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    اگر (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_پشته);
			trycmd->tcd_caught = FALSE;
		    }
		    بشکن;
		}

	    // end of ":try" block
	    حالت ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_data) + trystack->ga_len;
		    اگر (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    اگر (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_پشته);
		    }

		    اگر (trycmd->tcd_return)
			برو func_return;

		    وقتی (ectx->ec_پشته.ga_len > trycmd->tcd_پشته_len)
		    {
			--ectx->ec_پشته.ga_len;
			clear_tv(پشته_TV_BOT(0));
		    }
		    اگر (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		بشکن;

	    حالت ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    اگر (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = پشته_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_عدد;
			tv->vval.v_عدد = 0;
			برو done;
		    }
		    --ectx->ec_پشته.ga_len;
		    tv = پشته_TV_BOT(0);
		    اگر (tv->vval.v_string == پوچ
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_empty_string));
			برو theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    اگر (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
			اگر (trycmd->tcd_caught)
			{
			    // discard the exception
			    finish_exception(caught_پشته);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (throw_exception(tv->vval.v_string, ET_USER, پوچ)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			برو theend;
		    }
		    did_throw = TRUE;
		}
		بشکن;

	    // compare with special values
	    حالت ISN_COMPAREBOOL:
	    حالت ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_عدد;
		    varnumber_T arg2 = tv2->vval.v_عدد;
		    صحیح		res;

		    اگر (iptr->isn_arg.op.op_type == عبارت_EQUAL)
			res = arg1 == arg2;
		    وگرنه
			res = arg1 != arg2;

		    --ectx->ec_پشته.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_عدد = res ? VVAL_TRUE : VVAL_FALSE;
		}
		بشکن;

	    حالت ISN_COMPARENULL:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    صحیح		res;

		    res = typval_compare_null(tv1, tv2);
		    اگر (res == MAYBE)
			برو on_error;
		    اگر (iptr->isn_arg.op.op_type == عبارت_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_پشته.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_عدد = res ? VVAL_TRUE : VVAL_FALSE;
		}
		بشکن;

	    // Operation with two number arguments
	    حالت ISN_OPNR:
	    حالت ISN_COMPARENR:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_عدد;
		    varnumber_T arg2 = tv2->vval.v_عدد;
		    varnumber_T res = 0;
		    صحیح		div_zero = FALSE;

		    اگر (iptr->isn_arg.op.op_type == عبارت_LSHIFT
			    || iptr->isn_arg.op.op_type == عبارت_RSHIFT)
		    {
			اگر (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_positive));
			    برو on_error;
			}
		    }

		    انتخاب (iptr->isn_arg.op.op_type)
		    {
			حالت عبارت_MULT: res = arg1 * arg2; بشکن;
			حالت عبارت_DIV:  اگر (arg2 == 0)
					    div_zero = TRUE;
					وگرنه
					    res = arg1 / arg2;
					بشکن;
			حالت عبارت_REM:  اگر (arg2 == 0)
					    div_zero = TRUE;
					وگرنه
					    res = arg1 % arg2;
					بشکن;
			حالت عبارت_SUB: res = arg1 - arg2; بشکن;
			حالت عبارت_ADD: res = arg1 + arg2; بشکن;

			حالت عبارت_EQUAL: res = arg1 == arg2; بشکن;
			حالت عبارت_NEQUAL: res = arg1 != arg2; بشکن;
			حالت عبارت_GREATER: res = arg1 > arg2; بشکن;
			حالت عبارت_GEQUAL: res = arg1 >= arg2; بشکن;
			حالت عبارت_SMALLER: res = arg1 < arg2; بشکن;
			حالت عبارت_SEQUAL: res = arg1 <= arg2; بشکن;
			حالت عبارت_LSHIFT: اگر (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  وگرنه
					      res = (uvarnumber_T)arg1 << arg2;
					  بشکن;
			حالت عبارت_RSHIFT: اگر (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  وگرنه
					      res = (uvarnumber_T)arg1 >> arg2;
					  بشکن;
			پیشفرض: بشکن;
		    }

		    --ectx->ec_پشته.ga_len;
		    اگر (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_عدد = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    وگرنه
			tv1->vval.v_عدد = res;
		    اگر (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			برو on_error;
		    }
		}
		بشکن;

	    // Computation with two float arguments
	    حالت ISN_OPFLOAT:
	    حالت ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    صحیح		cmp = FALSE;

		    انتخاب (iptr->isn_arg.op.op_type)
		    {
			حالت عبارت_MULT: res = arg1 * arg2; بشکن;
			حالت عبارت_DIV: res = arg1 / arg2; بشکن;
			حالت عبارت_SUB: res = arg1 - arg2; بشکن;
			حالت عبارت_ADD: res = arg1 + arg2; بشکن;

			حالت عبارت_EQUAL: cmp = arg1 == arg2; بشکن;
			حالت عبارت_NEQUAL: cmp = arg1 != arg2; بشکن;
			حالت عبارت_GREATER: cmp = arg1 > arg2; بشکن;
			حالت عبارت_GEQUAL: cmp = arg1 >= arg2; بشکن;
			حالت عبارت_SMALLER: cmp = arg1 < arg2; بشکن;
			حالت عبارت_SEQUAL: cmp = arg1 <= arg2; بشکن;
			پیشفرض: cmp = 0; بشکن;
		    }
		    --ectx->ec_پشته.ga_len;
		    اگر (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_عدد = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    وگرنه
			tv1->vval.v_float = res;
		}
		بشکن;

	    حالت ISN_COMPARELIST:
	    حالت ISN_COMPARETUPLE:
	    حالت ISN_COMPAREDICT:
	    حالت ISN_COMPAREFUNC:
	    حالت ISN_COMPARESTRING:
	    حالت ISN_COMPAREBLOB:
	    حالت ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    صحیح		ic = iptr->isn_arg.op.op_ic;
		    صحیح		res = FALSE;
		    صحیح		status = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (iptr->isn_type == ISN_COMPARELIST)
		    {
			status = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وگرنه اگر (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			status = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وگرنه اگر (iptr->isn_type == ISN_COMPAREDICT)
		    {
			status = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وگرنه اگر (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			status = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وگرنه اگر (iptr->isn_type == ISN_COMPARESTRING)
		    {
			status = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وگرنه اگر (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			status = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    وگرنه // ISN_COMPAREOBJECT
		    {
			status = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_پشته.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_عدد = res ? VVAL_TRUE : VVAL_FALSE;
		    اگر (status == FAIL)
			برو theend;
		}
		بشکن;

	    حالت ISN_COMPAREANY:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    صحیح		ic = iptr->isn_arg.op.op_ic;
		    صحیح		status;

		    SOURCING_LNUM = iptr->isn_lnum;
		    status = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_پشته.ga_len;
		    اگر (status == FAIL)
			برو theend;
		}
		بشکن;

	    حالت ISN_ADDLIST:
	    حالت ISN_ADDTUPLE:
	    حالت ISN_ADDBLOB:
		{
		    typval_T *tv1 = پشته_TV_BOT(-2);
		    typval_T *tv2 = پشته_TV_BOT(-1);

		    // add two lists or blobs
		    اگر (iptr->isn_type == ISN_ADDLIST)
		    {
			اگر (iptr->isn_arg.op.op_type == عبارت_APPEND
						   && tv1->vval.v_list != پوچ)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 پوچ);
			وگرنه
			    eval_addlist(tv1, tv2);
		    }
		    وگرنه اگر (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    وگرنه
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_پشته.ga_len;
		}
		بشکن;

	    حالت ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (l == پوچ)
		    {
			emsg(_(e_cannot_add_to_null_list));
			برو on_error;
		    }
		    اگر (مقدار_check_lock(l->lv_lock, پوچ, FALSE))
			برو on_error;
		    اگر (list_append_tv(l, tv2) == FAIL)
			برو theend;
		    clear_tv(tv2);
		    --ectx->ec_پشته.ga_len;
		}
		بشکن;

	    حالت ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    صحیح		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    اگر (b == پوچ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_add_to_null_blob));
			برو on_error;
		    }
		    n = tv_get_عدد_chk(tv2, &error);
		    اگر (error)
			برو on_error;
		    ga_append(&b->bv_ga, (صحیح)n);
		    --ectx->ec_پشته.ga_len;
		}
		بشکن;

	    // Computation with two arguments of unknown type
	    حالت ISN_OPANY:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    صحیح		error = FALSE;

		    اگر (iptr->isn_arg.op.op_type == عبارت_ADD)
		    {
			اگر (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_پشته.ga_len;
			    بشکن;
			}
			وگرنه اگر (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_پشته.ga_len;
			    بشکن;
			}
			وگرنه اگر (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_پشته.ga_len;
			    بشکن;
			}
		    }
		    اگر (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    وگرنه
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_عدد_chk(tv1, &error);
			اگر (error)
			    برو on_error;
			اگر (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    اگر (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    وگرنه
		    {
			n2 = tv_get_عدد_chk(tv2, &error);
			اگر (error)
			    برو on_error;
			اگر (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    اگر (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			انتخاب (iptr->isn_arg.op.op_type)
			{
			    حالت عبارت_MULT: f1 = f1 * f2; بشکن;
			    حالت عبارت_DIV:  f1 = f1 / f2; بشکن;
			    حالت عبارت_SUB:  f1 = f1 - f2; بشکن;
			    حالت عبارت_ADD:  f1 = f1 + f2; بشکن;
			    پیشفرض: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     برو on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_پشته.ga_len;
		    }
		    وگرنه
		    {
			صحیح failed = FALSE;

			انتخاب (iptr->isn_arg.op.op_type)
			{
			    حالت عبارت_MULT: n1 = n1 * n2; بشکن;
			    حالت عبارت_DIV:  n1 = رقم_divide(n1, n2, &failed);
					    اگر (failed)
						برو on_error;
					    بشکن;
			    حالت عبارت_SUB:  n1 = n1 - n2; بشکن;
			    حالت عبارت_ADD:  n1 = n1 + n2; بشکن;
			    پیشفرض:	    n1 = رقم_modulus(n1, n2, &failed);
					    اگر (failed)
						برو on_error;
					    بشکن;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_عدد;
			tv1->vval.v_عدد = n1;
			--ectx->ec_پشته.ga_len;
		    }
		}
		بشکن;

	    حالت ISN_STRINDEX:
	    حالت ISN_STRSLICE:
		{
		    صحیح		is_slice = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    اگر (is_slice)
		    {
			tv = پشته_TV_BOT(-2);
			n1 = tv->vval.v_عدد;
		    }

		    tv = پشته_TV_BOT(-1);
		    n2 = tv->vval.v_عدد;

		    ectx->ec_پشته.ga_len -= is_slice ? 2 : 1;
		    tv = پشته_TV_BOT(-1);
		    اگر (is_slice)
			// Slice: Select the characters from the string
			res = string_slice(tv->vval.v_string, n1, n2, FALSE);
		    وگرنه
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		بشکن;

	    حالت ISN_LISTINDEX:
	    حالت ISN_LISTSLICE:
	    حالت ISN_TUPLEINDEX:
	    حالت ISN_TUPLESLICE:
	    حالت ISN_BLOBINDEX:
	    حالت ISN_BLOBSLICE:
		{
		    صحیح		is_slice = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    صحیح		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    صحیح		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_slice ? پشته_TV_BOT(-3) : پشته_TV_BOT(-2);

		    tv = پشته_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_عدد;
		    clear_tv(tv);

		    اگر (is_slice)
		    {
			tv = پشته_TV_BOT(-2);
			n1 = tv->vval.v_عدد;
			clear_tv(tv);
		    }

		    ectx->ec_پشته.ga_len -= is_slice ? 2 : 1;
		    tv = پشته_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (is_blob)
		    {
			اگر (blob_slice_or_index(val_tv->vval.v_blob, is_slice,
						    n1, n2, FALSE, tv) == FAIL)
			    برو on_error;
		    }
		    وگرنه اگر (is_tuple)
		    {
			اگر (tuple_slice_or_index(val_tv->vval.v_tuple,
				is_slice, n1, n2, FALSE, tv, TRUE) == FAIL)
			    برو on_error;
		    }
		    وگرنه
		    {
			اگر (list_slice_or_index(val_tv->vval.v_list, is_slice,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    برو on_error;
		    }
		}
		بشکن;

	    حالت ISN_ANYINDEX:
	    حالت ISN_ANYSLICE:
		{
		    صحیح		is_slice = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    صحیح		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_slice ? پشته_TV_BOT(-3) : پشته_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (check_can_index(tv, TRUE, TRUE) == FAIL)
			برو on_error;
		    var1 = is_slice ? پشته_TV_BOT(-2) : پشته_TV_BOT(-1);
		    var2 = is_slice ? پشته_TV_BOT(-1) : پوچ;
		    res = eval_index_inner(tv, is_slice, var1, var2,
							FALSE, پوچ, -1, TRUE);
		    clear_tv(var1);
		    اگر (is_slice)
			clear_tv(var2);
		    ectx->ec_پشته.ga_len -= is_slice ? 2 : 1;
		    اگر (res == FAIL)
			برو on_error;
		}
		بشکن;

	    حالت ISN_SLICE:
		{
		    صحیح		شمار = iptr->isn_arg.عدد;

		    // type will have been checked to be a list
		    tv = پشته_TV_BOT(-1);
		    اگر (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			اگر (list != پوچ && list->lv_len >= شمار)
			{
			    list_T	*newlist = list_slice(list,
				    شمار, list->lv_len - 1);

			    اگر (newlist != پوچ)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    وگرنه
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			اگر (tuple != پوچ && TUPLE_LEN(tuple) >= شمار)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_slice(tuple, شمار,
							TUPLE_LEN(tuple) - 1);
			    اگر (newtuple != پوچ)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		بشکن;

	    حالت ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*item_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = پشته_TV_BOT(-1 - gi->gi_with_op);
		    اگر (tv->v_type == VAR_LIST)
		    {
			li = list_find(tv->vval.v_list, gi->gi_index);
			اگر (li == پوچ)
			    برو on_error;
			item_tv = &li->li_tv;
		    }
		    وگرنه
			item_tv = TUPLE_ITEM(tv->vval.v_tuple, gi->gi_index);

		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    ++ectx->ec_پشته.ga_len;
		    copy_tv(item_tv, پشته_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_index = gi->gi_index + 1;
		    ectx->ec_where.wt_kind = WT_VARIABLE;
		}
		بشکن;

	    // dict member with string key (dict['member'])
	    حالت ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*کلید;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = پشته_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = پشته_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    کلید = tv->vval.v_string;
		    اگر (کلید == پوچ)
			کلید = (char_u *)"";

		    اگر ((di = dict_find(dict, کلید, -1)) == پوچ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_کلید_not_present_in_dictionary_str), کلید);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_پشته.ga_len;
			tv = پشته_TV_BOT(-1);
			(تهی) dict_پشته_save(tv);
			tv->v_type = VAR_عدد;
			tv->vval.v_عدد = 0;
			برو on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_پشته.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = پشته_TV_BOT(-1);
		    اگر (dict_پشته_save(tv) == FAIL)
			برو on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		بشکن;

	    // dict member with string key (dict.member)
	    // or can be an object
	    حالت ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = پشته_TV_BOT(-1);

		    اگر (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			اگر (dict_پشته_save(tv) == FAIL)
			    برو on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			اگر (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    برو on_error;
		    }
		    وگرنه
		    {
			اگر (tv->v_type != VAR_DICT || tv->vval.v_dict == پوچ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    برو on_error;
			}
			dict = tv->vval.v_dict;

			اگر ((di = dict_find(dict, iptr->isn_arg.string, -1))
								   == پوچ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_کلید_not_present_in_dictionary_str),
						     iptr->isn_arg.string);
			    برو on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			اگر (dict_پشته_save(tv) == FAIL)
			    برو on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		بشکن;

	    حالت ISN_GET_OBJ_MEMBER:
	    حالت ISN_GET_ITF_MEMBER:
		{
		    tv = پشته_TV_BOT(-1);
		    اگر (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			برو on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    اگر (obj == پوچ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			برو on_error;
		    }

		    صحیح idx;
		    اگر (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    وگرنه
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_index_from_itf_index(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    اگر (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			برو on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		بشکن;

	    حالت ISN_STORE_THIS:
		{
		    صحیح idx = iptr->isn_arg.عدد;
		    object_T *obj = پشته_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *پشته_TV_BOT(-1);
		    --ectx->ec_پشته.ga_len;
		}
		بشکن;

	    حالت ISN_CLEARDICT:
		dict_پشته_drop();
		بشکن;

	    حالت ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_پشته_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = پشته_TV_BOT(-1);
		    اگر (dict_tv != پوچ
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != پوچ
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == پوچ))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_پشته_drop();
		}
		بشکن;

	    حالت ISN_NEGATENR:
		tv = پشته_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		اگر (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		وگرنه
		    tv->vval.v_عدد = -tv->vval.v_عدد;
		بشکن;

	    حالت ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    صحیح		r;
		    where_T	where = WHERE_INIT;

		    tv = پشته_TV_BOT((صحیح)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (ct->ct_arg_idx > 0)
		    {
			where.wt_index = ct->ct_arg_idx;
			where.wt_kind = ct->ct_is_var ? WT_VARIABLE : WT_ARGUMENT;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    اگر (r == FAIL)
			برو on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    اگر (tv->v_type == VAR_عدد
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_عدد == 0
						|| tv->vval.v_عدد == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_عدد = tv->vval.v_عدد
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		بشکن;

	    حالت ISN_CHECKLEN:
		{
		    صحیح	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = پوچ;
		    tuple_T *tuple = پوچ;
		    صحیح	    len = 0;

		    tv = پشته_TV_BOT(-1);

		    صحیح		len_check_failed = FALSE;
		    اگر (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			اگر (list == پوچ || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			اگر (list != پوچ)
			    len = list->lv_len;
		    }
		    وگرنه اگر (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			اگر (tuple == پوچ || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			اگر (tuple != پوچ)
			    len = TUPLE_LEN(tuple);
		    }
		    وگرنه
			len_check_failed = TRUE;

		    اگر (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_اقلام_but_got_nr),
				     min_len, len);
			برو on_error;
		    }
		}
		بشکن;

	    حالت ISN_SETTYPE:
		set_tv_type(پشته_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		بشکن;

	    حالت ISN_2BOOL:
	    حالت ISN_COND2BOOL:
		{
		    صحیح n;
		    صحیح error = FALSE;

		    اگر (iptr->isn_type == ISN_2BOOL)
		    {
			tv = پشته_TV_BOT(iptr->isn_arg.tobool.offset);
			n = tv2bool(tv);
			اگر (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    وگرنه
		    {
			tv = پشته_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			اگر (error)
			    برو on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_عدد = n ? VVAL_TRUE : VVAL_FALSE;
		}
		بشکن;

	    حالت ISN_2STRING:
	    حالت ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		اگر (do_2string(پشته_TV_BOT(iptr->isn_arg.tostring.offset),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    برو on_error;
		بشکن;

	    حالت ISN_RANGE:
		{
		    exarg_T	ea;
		    نویسه	*errormsg;

		    ea.line2 = 0;
		    ea.addr_شمار = 0;
		    ea.addr_type = ADDR_سطرها;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    اگر (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
			برو on_error;

		    اگر (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			برو theend;
		    ++ectx->ec_پشته.ga_len;
		    tv = پشته_TV_BOT(-1);
		    tv->v_type = VAR_عدد;
		    tv->v_lock = 0;
		    tv->vval.v_عدد = ea.line2;
		}
		بشکن;

	    حالت ISN_PUT:
		tv = پوچ; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		بشکن;
	    حالت ISN_IPUT:
		tv = پوچ;
		isn_put_do(ectx, iptr, tv, TRUE);
		بشکن;

	    حالت ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_پشته.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		بشکن;

	    حالت ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = پوچ;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		بشکن;

	    حالت ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = پشته_TV_BOT(-1);
		اگر (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_argument_must_be_sequence_of_lists_or_tuples));
		    برو on_error;
		}

		صحیح rc;
		اگر (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		وگرنه
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		اگر (rc != EXEC_OK)
		{
		    اگر (rc == EXEC_FAIL)
			برو on_error;
		    برو theend;
		}
		بشکن;

	    حالت ISN_PROF_START:
	    حالت ISN_PROF_END:
		{
#ifdef FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    اگر (iptr->isn_type == ISN_PROF_START)
		    {
			func_سطر_start(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_سطر_exec(&cookie);
		    }
		    وگرنه
			func_سطر_end(&cookie);
#endif
		}
		بشکن;

	    حالت ISN_DEBUG:
		handle_debug(iptr, ectx);
		بشکن;

	    حالت ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    صحیح		item = iptr->isn_arg.shuffle.shfl_item;
		    صحیح		up = iptr->isn_arg.shuffle.shfl_up;

		    tmp_tv = *پشته_TV_BOT(-item);
		    برای ( ; up > 0 && item > 1; --up)
		    {
			*پشته_TV_BOT(-item) = *پشته_TV_BOT(-item + 1);
			--item;
		    }
		    *پشته_TV_BOT(-item) = tmp_tv;
		}
		بشکن;

	    حالت ISN_DROP:
		--ectx->ec_پشته.ga_len;
		clear_tv(پشته_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		بشکن;

	    حالت ISN_SCRIPTCTX_SET:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		بشکن;
	}
	ادامه;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	اگر (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
	    برو done;

	اگر (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    برو theend;
	ادامه;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	اگر (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    اگر (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		وقتی (ectx->ec_پشته.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_پشته.ga_len;
		    clear_tv(پشته_TV_BOT(0));
		}
		وقتی (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    ادامه;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	اگر (trylevel <= ectx->ec_trylevel_at_start)
	    برو theend;
    }

done:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_پشته_clear(dict_پشته_len_at_start);
    ectx->ec_trylevel_at_start = save_trylevel_at_start;
    بازگشت ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    صحیح
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    صحیح		save_iidx = ectx->ec_iidx;
    صحیح		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    اگر (res == OK)
    {
	*rettv = *پشته_TV_BOT(-1);
	--ectx->ec_پشته.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    بازگشت res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(تهی)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    صحیح		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    اگر (exec_instructions(ectx) == OK)
    {
	typval_T *tv = پشته_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_پشته.ga_len;
	clear_tv(tv);
    }
    وگرنه
    {
	substitute_instr->subs_status = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    بازگشت res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    صحیح
call_def_function(
    ufunc_T	*ufunc,
    صحیح		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    صحیح		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    صحیح		argc = argc_arg;
    صحیح		partial_argc = partial == پوچ
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    صحیح		total_argc = argc + partial_argc;
    typval_T	*tv;
    صحیح		idx;
    صحیح		ret = FAIL;
    صحیح		defcount = ufunc->uf_args.ga_len - total_argc;
    sctx_T	save_current_sctx = current_sctx;
    صحیح		did_emsg_before = did_emsg_cumul + did_emsg;
    صحیح		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_پیام_list = پوچ;
    msglist_T	*private_پیام_list = پوچ;
    صحیح		save_emsg_silent_def = emsg_silent_def;
    صحیح		save_did_emsg_def = did_emsg_def;
    صحیح		orig_funcdepth;
    صحیح		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#undef پشته_TV
#define پشته_TV(idx) (((typval_T *)ectx.ec_پشته.ga_data) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#undef پشته_TV_BOT
#define پشته_TV_BOT(idx) (((typval_T *)ectx.ec_پشته.ga_data) + ectx.ec_پشته.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#undef پشته_TV_VAR
#define پشته_TV_VAR(idx) (((typval_T *)ectx.ec_پشته.ga_data) + ectx.ec_frame_idx + پشته_FRAME_SIZE + idx)

    اگر (ufunc->uf_def_status == UF_NOT_COMPILED
	    || ufunc->uf_def_status == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), پوچ) == FAIL))
    {
	اگر (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	بازگشت FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	اگر (dfunc->df_ufunc == پوچ)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    بازگشت FAIL;
	}
	اگر (INSTRUCTIONS(dfunc) == پوچ)
	{
	    iemsg("using call_def_function() on not compiled function");
	    بازگشت FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    اگر (funcdepth_increment() == FAIL)
	بازگشت FAIL;

    CLEAR_FIELD(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_پشته, اندازه(typval_T), 500);
    اگر (GA_GROW_FAILS(&ectx.ec_پشته, 20))
    {
	funcdepth_decrement();
	بازگشت FAIL;
    }
    ga_init2(&ectx.ec_trystack, اندازه(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, اندازه(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = total_argc - ufunc->uf_args.ga_len;
    اگر (idx > 0 && ufunc->uf_va_name == پوچ)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
								    idx), idx);
	برو failed_early;
    }
    idx = total_argc - ufunc->uf_args.ga_len + ufunc->uf_def_args.ga_len;
    اگر (idx < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	برو failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    برای (idx = 0; idx < total_argc
	    && (ufunc->uf_va_name != پوچ || idx < ufunc->uf_args.ga_len);
									 ++idx)
    {
	صحیح argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	اگر (idx >= ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_عدد == VVAL_NONE)
	{
	    // Use the default value.
	    پشته_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	وگرنه
	{
	    صحیح done = FALSE;
	    اگر (ufunc->uf_arg_types != پوچ && idx < ufunc->uf_args.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		اگر (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_عدد)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    پشته_TV_BOT(0)->v_type = VAR_FLOAT;
		    پشته_TV_BOT(0)->v_lock = 0;
		    پشته_TV_BOT(0)->vval.v_float = tv->vval.v_عدد;
		    done = TRUE;
		}
		وگرنه اگر (check_typval_arg_type(expected, tv,
						   پوچ, argv_idx + 1) == FAIL)
		    برو failed_early;
	    }
	    اگر (!done)
		copy_tv(tv, پشته_TV_BOT(0));
	}
	++ectx.ec_پشته.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    اگر (ufunc->uf_va_name != پوچ)
    {
	صحیح vararg_شمار = argc - ufunc->uf_args.ga_len;

	اگر (vararg_شمار < 0)
	    vararg_شمار = 0;
	وگرنه
	    argc -= vararg_شمار;
	اگر (exe_newlist(vararg_شمار, &ectx) == FAIL)
	    برو failed_early;

	// Check the type of the list items.
	tv = پشته_TV_BOT(-1);
	اگر (ufunc->uf_va_type != پوچ
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != پوچ)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_first;

	    برای (idx = 0; idx < vararg_شمار; ++idx)
	    {
		اگر (check_typval_arg_type(expected, &li->li_tv,
						 پوچ, argc + idx + 1) == FAIL)
		    برو failed_early;
		li = li->li_next;
	    }
	}

	اگر (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *پشته_TV_BOT(defcount - 1) = *پشته_TV_BOT(-1);
	--ectx.ec_پشته.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    اگر (defcount > 0)
	برای (idx = 0; idx < defcount; ++idx)
	{
	    پشته_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_پشته.ga_len;
	}
    اگر (ufunc->uf_va_name != پوچ)
	++ectx.ec_پشته.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_frame_idx = ectx.ec_پشته.ga_len;
    ectx.ec_initial_frame_idx = ectx.ec_frame_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *base_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	اگر (partial != پوچ || base_ufunc->uf_partial != پوچ)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    اگر (ectx.ec_outer_ref == پوچ)
		برو failed_early;
	    اگر (partial != پوچ)
	    {
		outer_T *outer = get_pt_outer(partial);

		اگر (outer->out_پشته == پوچ && outer->out_loop_size == 0)
		{
		    // no stack was set
		    اگر (current_ectx != پوچ)
		    {
			اگر (current_ectx->ec_outer_ref != پوچ
			       && current_ectx->ec_outer_ref->or_outer != پوچ)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		وگرنه
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    وگرنه
	    {
		ectx.ec_outer_ref->or_outer = &base_ufunc->uf_partial->pt_outer;
		++base_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = base_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    برای (idx = 0; idx < پشته_FRAME_SIZE; ++idx)
    {
	پشته_TV(ectx.ec_پشته.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_پشته.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	برای (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    پشته_TV_VAR(idx)->v_type = VAR_عدد;
	    پشته_TV_VAR(idx)->vval.v_عدد = 0;
	}
	ectx.ec_پشته.ga_len += dfunc->df_varcount;

	اگر (object != پوچ)
	{
	    // the object is always the variable at index zero
	    tv = پشته_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	اگر (dfunc->df_has_closure)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    پشته_TV_VAR(idx)->v_type = VAR_عدد;
	    پشته_TV_VAR(idx)->vval.v_عدد = 0;
	    ++ectx.ec_پشته.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    اگر (funccal != پوچ)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_افزودن_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_پیام_list = پیام_list;
    پیام_list = &private_پیام_list;

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
    اگر (ret == OK)
    {
	// function finished, get result from the stack.
	اگر (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	وگرنه
	{
	    tv = پشته_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    اگر (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_closure_in_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_برداشت();
    current_sctx = save_current_sctx;

    اگر (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    اگر (*پیام_list != پوچ && saved_پیام_list != پوچ)
    {
	msglist_T **plist = saved_پیام_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	وقتی (*plist != پوچ)
	    plist = &(*plist)->next;

	*plist = *پیام_list;
    }
    پیام_list = saved_پیام_list;

    اگر (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = پوچ;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    برای (idx = 0; idx < ectx.ec_پشته.ga_len; ++idx)
    {
	tv = پشته_TV(idx);
	اگر (tv->v_type != VAR_عدد && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_پشته.ga_data);
    vim_free(ectx.ec_trystack.ga_data);
    اگر (ectx.ec_outer_ref != پوچ)
    {
	اگر (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    اگر (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    بازگشت ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    تهی
unwind_def_callstack(ectx_T *ectx)
{
    وقتی (ectx->ec_frame_idx != ectx->ec_initial_frame_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    تهی
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    اگر (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    ایستا char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    صحیح		depth;

    ga_init2(&ga, 1, 100);
    برای (depth = 0; depth < lvi->lvi_depth; ++depth)
    {
	اگر (ga_grow(&ga, 50) == FAIL)
	    بشکن;
	اگر (lvi->lvi_loop[depth].var_idx == 0)
	    STRCPY((نویسه *)ga.ga_data + ga.ga_len, " -");
	وگرنه
	    vim_snprintf((نویسه *)ga.ga_data + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[depth].var_idx,
			    lvi->lvi_loop[depth].var_idx
					 + lvi->lvi_loop[depth].var_شمار - 1);
	ga.ga_len = (صحیح)STRLEN(ga.ga_data);
    }
    بازگشت ga.ga_data;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    ایستا تهی
list_instructions(نویسه *pfx, isn_T *instr, صحیح instr_شمار, ufunc_T *ufunc)
{
    صحیح		سطر_idx = 0;
    صحیح		prev_current = 0;
    صحیح		current;
    صحیح		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    برای (current = 0; current < instr_شمار; ++current)
    {
	isn_T	    *iptr = &instr[current];
	نویسه	    *سطر;

	اگر (ufunc != پوچ)
	{
	    وقتی (سطر_idx < iptr->isn_lnum
					  && سطر_idx < ufunc->uf_سطرها.ga_len)
	    {
		اگر (current > prev_current)
		{
		    پیام_puts("\n\n");
		    prev_current = current;
		}
		سطر = ((نویسه **)ufunc->uf_سطرها.ga_data)[سطر_idx++];
		اگر (سطر != پوچ)
		    پیام(سطر);
	    }
	    اگر (iptr->isn_type == ISN_JUMP_IF_ARG_SET)
	    {
		صحیح	first_def_arg = ufunc->uf_args.ga_len
						   - ufunc->uf_def_args.ga_len;

		اگر (def_arg_idx > 0)
		    پیام_puts("\n\n");
		پیام_start();
		پیام_puts("  ");
		پیام_puts(((نویسه **)(ufunc->uf_args.ga_data))[
						 first_def_arg + def_arg_idx]);
		پیام_puts(" = ");
		پیام_puts(((نویسه **)(ufunc->uf_def_args.ga_data))[def_arg_idx++]);
		پیام_clr_eos();
		پیام_end();
	    }
	}

	انتخاب (iptr->isn_type)
	{
	    حالت ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (صحیح)iptr->isn_arg.construct.construct_size);
		بشکن;
	    حالت ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_EXEC_SPLIT:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		بشکن;
	    حالت ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		بشکن;
	    حالت ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.عدد ? " append" : "");
		بشکن;
	    حالت ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.عدد));
#endif
		بشکن;
	    حالت ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#endif
		بشکن;
	    حالت ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, INT_MAX, پوچ);
		پیام("     -------------");
		بشکن;
	    حالت ISN_SOURCE:
		{
		    scriptitem_T    *si = SCRIPT_ITEM(iptr->isn_arg.عدد);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		بشکن;
	    حالت ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, INT_MAX, پوچ);
		    پیام("     -------------");
		}
		بشکن;
	    حالت ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.عدد);
		بشکن;
	    حالت ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_شمار);
		}
		بشکن;
	    حالت ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_ECHOWINDOW:
		اگر (iptr->isn_arg.echowin.ewin_time > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_شمار,
				      iptr->isn_arg.echowin.ewin_time);
		وگرنه
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_شمار);
		بشکن;
	    حالت ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_LOAD:
		{
		    اگر (iptr->isn_arg.عدد < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.عدد
							  + پشته_FRAME_SIZE));
		    وگرنه
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		}
		بشکن;
	    حالت ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    اگر (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_depth,
					outer->outer_idx + پشته_FRAME_SIZE);
		    وگرنه اگر (outer->outer_depth < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_depth);
		    وگرنه
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_depth,
					      outer->outer_idx);
		}
		بشکن;
	    حالت ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    اگر (sv == پوچ)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    وگرنه
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		بشکن;
	    حالت ISN_LOADS:
	    حالت ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		بشکن;
	    حالت ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		بشکن;
	    حالت ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		بشکن;
	    حالت ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		بشکن;
	    حالت ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		بشکن;
	    حالت ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (صحیح)(iptr->isn_arg.عدد));
		بشکن;

	    حالت ISN_STORE:
		اگر (iptr->isn_arg.عدد < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.عدد + پشته_FRAME_SIZE);
		وگرنه
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.عدد);
		بشکن;
	    حالت ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    اگر (outer->outer_depth < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_depth);
		    وگرنه
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_depth, outer->outer_idx);
		}
		بشکن;
	    حالت ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_STORES:
	    حالت ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		بشکن;
	    حالت ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    اگر (sv == پوچ)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    وگرنه
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		بشکن;
	    حالت ISN_STOREOPT:
	    حالت ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		بشکن;
	    حالت ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (صحیح)iptr->isn_arg.عدد);
		بشکن;
	    حالت ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		بشکن;

	    حالت ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		بشکن;

	    حالت ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		بشکن;

	    حالت ISN_LOAD_CLASSMEMBER:
	    حالت ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    صحیح	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_LOAD_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		بشکن;

	    // constants
	    حالت ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_PUSHBOOL:
	    حالت ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		بشکن;
	    حالت ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		بشکن;
	    حالت ISN_PUSHFUNC:
		{
		    نویسه *name = (نویسه *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == پوچ ? "[none]" : name);
		}
		بشکن;
	    حالت ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#endif
		بشکن;
	    حالت ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#endif
		بشکن;
	    حالت ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		بشکن;
	    حالت ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == پوچ ? "null"
				 : (نویسه *)iptr->isn_arg.classarg->class_name.string);
		بشکن;
	    حالت ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		بشکن;
	    حالت ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		بشکن;
	    حالت ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		بشکن;
	    حالت ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		بشکن;
	    حالت ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		بشکن;
	    حالت ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		بشکن;
	    حالت ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		بشکن;
	    حالت ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		بشکن;

	    // function call
	    حالت ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    internal_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		بشکن;
	    حالت ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		بشکن;
	    حالت ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != پوچ
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		بشکن;
	    حالت ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		بشکن;
	    حالت ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_بالاترین ? " top" : "", cpfunc->cpf_argcount);
		}
		بشکن;
	    حالت ISN_PCALL_END:
		smsg("%s%4d PCALL end", pfx, current);
		بشکن;
	    حالت ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (صحیح)iptr->isn_arg.defer.defer_argcount);
		بشکن;
	    حالت ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		بشکن;
	    حالت ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		بشکن;
	    حالت ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		بشکن;
	    حالت ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    اگر (extra == پوچ || extra->fre_func_name == پوچ)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    وگرنه
			name = extra->fre_func_name;
		    اگر (extra != پوچ && extra->fre_class != پوچ)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    وگرنه اگر (extra == پوچ
				     || extra->fre_loopvar_info.lvi_depth == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    وگرنه
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		بشکن;

	    حالت ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    اگر (arg->nfa_loopvar_info.lvi_depth == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    وگرنه
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		بشکن;

	    حالت ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == پوچ ? (char_u *)"" : name);
		}
		بشکن;

	    حالت ISN_JUMP:
		{
		    نویسه *when = "?";

		    انتخاب (iptr->isn_arg.jump.jump_when)
		    {
			حالت JUMP_ALWAYS:
			    when = "JUMP";
			    بشکن;
			حالت JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    بشکن;
			حالت JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    بشکن;
			حالت JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    بشکن;
			حالت JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  // unused
			    بشکن;
			حالت JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    بشکن;
			حالت JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    بشکن;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		بشکن;

	    حالت ISN_JUMP_IF_ARG_SET:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + پشته_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		بشکن;

	    حالت ISN_JUMP_IF_ARG_NOT_SET:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + پشته_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		بشکن;

	    حالت ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_end);
		}
		بشکن;

	    حالت ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->end_funcref_idx,
			    endloop->end_var_idx,
			    endloop->end_var_idx + endloop->end_var_شمار - 1,
			    endloop->end_depth);
		}
		بشکن;

	    حالت ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_end);
		}
		بشکن;

	    حالت ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    اگر (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    وگرنه
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		بشکن;
	    حالت ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		بشکن;
	    حالت ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_levels,
				      trycont->tct_levels == 1 ? "" : "s",
				      trycont->tct_where);
		}
		بشکن;
	    حالت ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		بشکن;
	    حالت ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		بشکن;
	    حالت ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		بشکن;

	    // expression operations on number
	    حالت ISN_OPNR:
	    حالت ISN_OPFLOAT:
	    حالت ISN_OPANY:
		{
		    نویسه *what;
		    نویسه *ins;

		    انتخاب (iptr->isn_arg.op.op_type)
		    {
			حالت عبارت_MULT: what = "*"; بشکن;
			حالت عبارت_DIV: what = "/"; بشکن;
			حالت عبارت_REM: what = "%"; بشکن;
			حالت عبارت_SUB: what = "-"; بشکن;
			حالت عبارت_ADD: what = "+"; بشکن;
			حالت عبارت_LSHIFT: what = "<<"; بشکن;
			حالت عبارت_RSHIFT: what = ">>"; بشکن;
			پیشفرض:       what = "???"; بشکن;
		    }
		    انتخاب (iptr->isn_type)
		    {
			حالت ISN_OPNR: ins = "OPNR"; بشکن;
			حالت ISN_OPFLOAT: ins = "OPFLOAT"; بشکن;
			حالت ISN_OPANY: ins = "OPANY"; بشکن;
			پیشفرض: ins = "???"; بشکن;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		بشکن;

	    حالت ISN_COMPAREBOOL:
	    حالت ISN_COMPARESPECIAL:
	    حالت ISN_COMPARENULL:
	    حالت ISN_COMPARENR:
	    حالت ISN_COMPAREFLOAT:
	    حالت ISN_COMPARESTRING:
	    حالت ISN_COMPAREBLOB:
	    حالت ISN_COMPARELIST:
	    حالت ISN_COMPARETUPLE:
	    حالت ISN_COMPAREDICT:
	    حالت ISN_COMPAREFUNC:
	    حالت ISN_COMPAREOBJECT:
	    حالت ISN_COMPAREANY:
		   {
		       نویسه *p;
		       نویسه buf[10];
		       نویسه *type;

		       انتخاب (iptr->isn_arg.op.op_type)
		       {
			   حالت عبارت_EQUAL:	 p = "=="; بشکن;
			   حالت عبارت_NEQUAL:    p = "!="; بشکن;
			   حالت عبارت_GREATER:   p = ">"; بشکن;
			   حالت عبارت_GEQUAL:    p = ">="; بشکن;
			   حالت عبارت_SMALLER:   p = "<"; بشکن;
			   حالت عبارت_SEQUAL:    p = "<="; بشکن;
			   حالت عبارت_MATCH:	 p = "=~"; بشکن;
			   حالت عبارت_IS:	 p = "is"; بشکن;
			   حالت عبارت_ISNOT:	 p = "isnot"; بشکن;
			   حالت عبارت_NOMATCH:	 p = "!~"; بشکن;
			   پیشفرض:  p = "???"; بشکن;
		       }
		       STRCPY(buf, p);
		       اگر (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       انتخاب(iptr->isn_type)
		       {
			   حالت ISN_COMPAREBOOL: type = "COMPAREBOOL"; بشکن;
			   حالت ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; بشکن;
			   حالت ISN_COMPARENULL: type = "COMPARENULL"; بشکن;
			   حالت ISN_COMPARENR: type = "COMPARENR"; بشکن;
			   حالت ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; بشکن;
			   حالت ISN_COMPARESTRING:
						  type = "COMPARESTRING"; بشکن;
			   حالت ISN_COMPAREBLOB: type = "COMPAREBLOB"; بشکن;
			   حالت ISN_COMPARELIST: type = "COMPARELIST"; بشکن;
			   حالت ISN_COMPARETUPLE: type = "COMPARETUPLE"; بشکن;
			   حالت ISN_COMPAREDICT: type = "COMPAREDICT"; بشکن;
			   حالت ISN_COMPAREFUNC: type = "COMPAREFUNC"; بشکن;
			   حالت ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; بشکن;
			   حالت ISN_COMPAREANY: type = "COMPAREANY"; بشکن;
			   پیشفرض: type = "???"; بشکن;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   بشکن;

	    حالت ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); بشکن;
	    حالت ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); بشکن;
	    حالت ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); بشکن;

	    // expression operations
	    حالت ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.عدد));
		بشکن;
	    حالت ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); بشکن;
	    حالت ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); بشکن;
	    حالت ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); بشکن;
	    حالت ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); بشکن;
	    حالت ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); بشکن;
	    حالت ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); بشکن;
	    حالت ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); بشکن;
	    حالت ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); بشکن;
	    حالت ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); بشکن;
	    حالت ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); بشکن;
	    حالت ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); بشکن;
	    حالت ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); بشکن;
	    حالت ISN_SLICE: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.عدد); بشکن;
	    حالت ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_index,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); بشکن;
	    حالت ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); بشکن;
	    حالت ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); بشکن;
	    حالت ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (صحیح)iptr->isn_arg.classmember.cm_idx);
				     بشکن;
	    حالت ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (صحیح)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     بشکن;
	    حالت ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (صحیح)iptr->isn_arg.عدد); بشکن;
	    حالت ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); بشکن;
	    حالت ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); بشکن;

	    حالت ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); بشکن;

	    حالت ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      نویسه	    *tofree = پوچ;
		      نویسه	    *typename;

		      اگر (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_عدد_OK))
			  typename = "float|number";
		      وگرنه اگر (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      وگرنه
			  typename = type_name(ct->ct_type, &tofree);

		      اگر (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (صحیح)ct->ct_off);
		      وگرنه
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (صحیح)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (صحیح)ct->ct_arg_idx);
		      vim_free(tofree);
		      بشکن;
		  }
	    حالت ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_more_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       بشکن;
	    حالت ISN_SETTYPE:
		  {
		      نویسه *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      بشکن;
		  }
	    حالت ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); بشکن;
	    حالت ISN_2BOOL: اگر (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    وگرنه
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    بشکن;
	    حالت ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      بشکن;
	    حالت ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      بشکن;
	    حالت ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    بشکن;
	    حالت ISN_PUT:
		اگر (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		وگرنه اگر (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		وگرنه
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		بشکن;
	    حالت ISN_IPUT:
		اگر (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		وگرنه اگر (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		وگرنه
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		بشکن;
	    حالت ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  پوچ, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    اگر (likely(buf != پوچ))
		    {
			(تهی)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    بشکن;
		}
	    حالت ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); بشکن;

	    حالت ISN_PROF_START:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 بشکن;

	    حالت ISN_PROF_END:
		smsg("%s%4d PROFILE END", pfx, current);
		بشکن;

	    حالت ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_names_len);
		بشکن;

	    حالت ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_شمار,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      بشکن;
	    حالت ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_item,
					 iptr->isn_arg.shuffle.shfl_up);
			      بشکن;
	    حالت ISN_DROP: smsg("%s%4d DROP", pfx, current); بشکن;

	    حالت ISN_SCRIPTCTX_SET:
		{
		    صحیح	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ITEM(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		بشکن;

	    حالت ISN_FINISH: // End of list of instructions for ISN_SUBSTITUTE.
			   بازگشت;
	}

	out_flush();	    // output one line at a time
	ui_breakcheck();
	اگر (got_int)
	    بشکن;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    تهی
set_context_in_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_pattern = arg;

    // first argument already typed: only user function names
    اگر (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
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
get_disassemble_argument(expand_T *xp, صحیح idx)
{
    اگر (idx == 0)
	بازگشت (char_u *)"debug";
    اگر (idx == 1)
	بازگشت (char_u *)"profile";
    بازگشت get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    تهی
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = پوچ;  // init to shut up gcc warning
    صحیح		instr_شمار = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = find_func_by_name(arg, &compile_type);
    اگر (ufunc == پوچ)
	بازگشت;
    اگر (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, پوچ) == FAIL)
	بازگشت;
    اگر (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	بازگشت;
    }
    پیام((نویسه *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    انتخاب (compile_type)
    {
	حالت CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_شمار = dfunc->df_instr_prof_شمار;
	    بشکن;
#endif
	    // FALLTHROUGH
	حالت CT_NONE:
	    instr = dfunc->df_instr;
	    instr_شمار = dfunc->df_instr_شمار;
	    بشکن;
	حالت CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_شمار = dfunc->df_instr_debug_شمار;
	    بشکن;
    }

    list_instructions("", instr, instr_شمار, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    صحیح
tv2bool(typval_T *tv)
{
    انتخاب (tv->v_type)
    {
	حالت VAR_عدد:
	    بازگشت tv->vval.v_عدد != 0;
	حالت VAR_FLOAT:
	    بازگشت tv->vval.v_float != 0.0;
	حالت VAR_PARTIAL:
	    بازگشت tv->vval.v_partial != پوچ;
	حالت VAR_FUNC:
	حالت VAR_STRING:
	    بازگشت tv->vval.v_string != پوچ && *tv->vval.v_string != NUL;
	حالت VAR_LIST:
	    بازگشت tv->vval.v_list != پوچ && tv->vval.v_list->lv_len > 0;
	حالت VAR_TUPLE:
	    بازگشت tuple_len(tv->vval.v_tuple) > 0;
	حالت VAR_DICT:
	    بازگشت tv->vval.v_dict != پوچ
				    && tv->vval.v_dict->dv_hashtab.ht_used > 0;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    بازگشت tv->vval.v_عدد == VVAL_TRUE ? TRUE : FALSE;
	حالت VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    بازگشت tv->vval.v_job != پوچ;
#وگرنه
	    بشکن;
#endif
	حالت VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    بازگشت tv->vval.v_channel != پوچ;
#وگرنه
	    بشکن;
#endif
	حالت VAR_BLOB:
	    بازگشت tv->vval.v_blob != پوچ && tv->vval.v_blob->bv_ga.ga_len > 0;

	حالت VAR_OBJECT:
	    بازگشت tv->vval.v_object != پوچ;

	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	    check_typval_is_مقدار(tv);
	    بشکن;

	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_INSTR:
	    بشکن;
    }
    بازگشت FALSE;
}

    تهی
emsg_using_string_as(typval_T *tv, صحیح as_عدد)
{
    semsg(_(as_عدد ? e_using_string_as_عدد_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == پوچ
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    صحیح
check_not_string(typval_T *tv)
{
    اگر (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	بازگشت FAIL;
    }
    بازگشت OK;
}

#endif // FEAT_EVAL
