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

#تعریف_کن USING_FLOAT_STUFF
#ضمن "vim.h"

#إذا defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
تعريفنوع بنية {
    صحيح	    tcd_قاب_idx;	// ec_frame_idx at ISN_TRY
    صحيح	    tcd_پشته_len;	// size of ectx.ec_stack at ISN_TRY
    صحيح	    tcd_درون_catch;	// in catch or finally block
    صحيح	    tcd_did_throw;	// set did_throw in :endtry
    صحيح	    tcd_catch_idx;	// instruction of the first :catch or :finally
    صحيح	    tcd_finally_idx;	// instruction of the :finally block or zero
    صحيح	    tcd_endtry_idx;	// instruction of the :endtry
    صحيح	    tcd_caught;		// catch block entered
    صحيح	    tcd_cont;		// :continue encountered, jump here (minus one)
    صحيح	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
تعريفنوع بنية {
    صحيح		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    صحيح		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
تعريفنوع بنية {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    صحيح		or_outer_allocated;  // free "or_outer" later
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
بنية ectx_S {
    garray_T	ec_پشته;	// stack of typval_T values
    صحيح		ec_قاب_idx;	// index in ec_stack: context of ec_dfunc_idx
    صحيح		ec_initial_قاب_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    صحيح		ec_dfunc_idx;	// current function index
    صحيح		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    صحيح		ec_did_emsg_before;
    صحيح		ec_trylevel_at_شروع;
    where_T	ec_where;
};

#اگر_تعریف_شده FEAT_PROFILE
// stack of profinfo_T used when profiling.
ساكن garray_T profile_info_ga = {0, 0, حجم(profinfo_T), 20, عدم};
#پایان_اگر

// Get pointer to item in the stack.
#تعریف_کن پشته_TV(idx) (((typval_T *)ectx->ec_پشته.ga_داده) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#تعریف_کن پشته_TV_BOT(idx) (((typval_T *)ectx->ec_پشته.ga_داده) + ectx->ec_پشته.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#تعریف_کن پشته_TV_VAR(idx) (((typval_T *)ectx->ec_پشته.ga_داده) + ectx->ec_قاب_idx + پشته_قاب_اندازه + idx)

// Return value for functions used to execute instructions
#تعریف_کن EXEC_FAIL	0
#تعریف_کن EXEC_OK		1
#تعریف_کن EXEC_انجامشده	2

    فراغ
مقصد_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_مقصد_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    ساكن صحيح
ufunc_argcount(ufunc_T *ufunc)
{
    أرجع ufunc->uf_آرگومانها.ga_len + (ufunc->uf_va_name != عدم ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    ساكن صحيح
exe_concat(صحيح شمار, ectx_T *ectx)
{
    صحيح		idx;
    size_t	len = 0;
    garray_T	ga;
    تعريفنوع بنية
    {
	typval_T    *tv;
	size_t	    درازی;
    } string_segment_T;
    تعداد
    {
	STRING_SEGMENT_CACHE_اندازه = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_اندازه];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    إذا (شمار > (صحيح)ARRAY_درازی(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, شمار);
	إذا (string_segment_tab == عدم)
	    أرجع FAIL;
    }

    ga_init2(&ga, حجم(حرف), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    لكل (idx = 0; idx < شمار; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = پشته_TV_BOT(idx - شمار);
	إذا (segment->tv->vval.v_string != عدم)
	{
	    segment->درازی = STRLEN(segment->tv->vval.v_string);
	    len += segment->درازی;
	}
	وإلا
	    segment->درازی = 0;    // Ensure clean state for the second pass
    }

    إذا (ga_grow(&ga, (صحيح)len + 1) == FAIL)
    {
	إذا (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	أرجع FAIL;
    }

    لكل (idx = 0; idx < شمار; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->درازی);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_پشته.ga_len -= شمار - 1;
    پشته_TV_BOT(-1)->vval.v_string = ga.ga_داده;

    إذا (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    أرجع OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    ساكن صحيح
exe_newlist(صحيح شمار, ectx_T *ectx)
{
    list_T	*list = عدم;
    صحيح		idx;
    typval_T	*tv;

    إذا (شمار >= 0)
    {
	list = list_alloc_with_اقلام(شمار);
	إذا (list == عدم)
	    أرجع FAIL;
	لكل (idx = 0; idx < شمار; ++idx)
	    list_مجموعه_قلم(list, idx, پشته_TV_BOT(idx - شمار));
    }

    إذا (شمار > 0)
	ectx->ec_پشته.ga_len -= شمار - 1;
    وإلا إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
    {
	list_unref(list);
	أرجع FAIL;
    }
    وإلا
	++ectx->ec_پشته.ga_len;
    tv = پشته_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_قفل = 0;
    إذا (list != عدم)
	++list->lv_refcount;
    أرجع OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    ساكن صحيح
exe_newtuple(صحيح شمار, ectx_T *ectx)
{
    tuple_T	*tuple = عدم;
    صحيح		idx;
    typval_T	*tv;

    إذا (شمار >= 0)
    {
	tuple = tuple_alloc_with_اقلام(شمار);
	إذا (tuple == عدم)
	    أرجع FAIL;
	لكل (idx = 0; idx < شمار; ++idx)
	    tuple_مجموعه_قلم(tuple, idx, پشته_TV_BOT(idx - شمار));
    }

    إذا (شمار > 0)
	ectx->ec_پشته.ga_len -= شمار - 1;
    وإلا إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
    {
	tuple_unref(tuple);
	أرجع FAIL;
    }
    وإلا
	++ectx->ec_پشته.ga_len;
    tv = پشته_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_قفل = 0;
    إذا (tuple != عدم)
	++tuple->tv_refcount;

    أرجع OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    ساكن صحيح
exe_newdict(صحيح شمار, ectx_T *ectx)
{
    dict_T	*dict = عدم;
    dictitem_T	*قلم;
    char_u	*کلید;
    صحيح		idx;
    typval_T	*tv;

    إذا (شمار >= 0)
    {
	dict = dict_alloc();
	إذا (unlikely(dict == عدم))
	    أرجع FAIL;
	لكل (idx = 0; idx < شمار; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = پشته_TV_BOT(2 * (idx - شمار));
	    // check key is unique
	    کلید = tv->vval.v_string == عدم ? (char_u *)"" : tv->vval.v_string;
	    قلم = dict_پیدا_کن(dict, کلید, -1);
	    إذا (قلم != عدم)
	    {
		semsg(_(e_duplicate_کلید_درون_dictionary_str), کلید);
		dict_unref(dict);
		أرجع MAYBE;
	    }
	    قلم = dictitem_alloc(کلید);
	    clear_tv(tv);
	    إذا (unlikely(قلم == عدم))
	    {
		dict_unref(dict);
		أرجع FAIL;
	    }
	    tv = پشته_TV_BOT(2 * (idx - شمار) + 1);
	    قلم->di_tv = *tv;
	    قلم->di_tv.v_قفل = 0;
	    tv->v_type = VAR_UNKNOWN;
	    إذا (dict_جمع_کن(dict, قلم) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(قلم);
		dict_unref(dict);
		أرجع FAIL;
	    }
	}
    }

    إذا (شمار > 0)
	ectx->ec_پشته.ga_len -= 2 * شمار - 1;
    وإلا إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
    {
	dict_unref(dict);
	أرجع FAIL;
    }
    وإلا
	++ectx->ec_پشته.ga_len;
    tv = پشته_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_قفل = 0;
    tv->vval.v_dict = dict;
    إذا (dict != عدم)
	++dict->dv_refcount;
    أرجع OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    فراغ
update_has_breakpoint(ufunc_T *ufunc)
{
    إذا (ufunc->uf_debug_tick == debug_tick)
	أرجع;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_پیدا_کن_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

ساكن garray_T dict_پشته = GA_خالی;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    ساكن صحيح
dict_پشته_save(typval_T *tv)
{
    إذا (dict_پشته.ga_growsize == 0)
	ga_init2(&dict_پشته, حجم(typval_T), 10);
    إذا (ga_grow(&dict_پشته, 1) == FAIL)
	أرجع FAIL;
    ((typval_T *)dict_پشته.ga_داده)[dict_پشته.ga_len] = *tv;
    ++dict_پشته.ga_len;
    أرجع OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    ساكن typval_T *
dict_پشته_get_tv(فراغ)
{
    إذا (dict_پشته.ga_len == 0)
	أرجع عدم;
    أرجع ((typval_T *)dict_پشته.ga_داده) + dict_پشته.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    ساكن dict_T *
dict_پشته_get_dict(فراغ)
{
    typval_T *tv;

    إذا (dict_پشته.ga_len == 0)
	أرجع عدم;
    tv = ((typval_T *)dict_پشته.ga_داده) + dict_پشته.ga_len - 1;
    إذا (tv->v_type == VAR_DICT)
	أرجع tv->vval.v_dict;
    أرجع عدم;
}

/*
 * Drop an item from the dict stack.
 */
    ساكن فراغ
dict_پشته_drop(فراغ)
{
    إذا (dict_پشته.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	أرجع;
    }
    --dict_پشته.ga_len;
    clear_tv(((typval_T *)dict_پشته.ga_داده) + dict_پشته.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    ساكن فراغ
dict_پشته_clear(صحيح len)
{
    طالما (dict_پشته.ga_len > len)
	dict_پشته_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    ساكن outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    إذا (ptref == عدم)
	أرجع &pt->pt_outer;

    // partial using partial (recursively)
    طالما (ptref->pt_outer_partial != عدم)
	ptref = ptref->pt_outer_partial;
    أرجع &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    ساكن صحيح
check_ufunc_arg_types(ufunc_T *ufunc, صحيح argcount, صحيح off, ectx_T *ectx)
{
    إذا (ufunc->uf_arg_types == عدم && ufunc->uf_va_type == عدم)
	أرجع OK;

    typval_T	*argv = پشته_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    لكل (صحيح i = 0; i < argcount; ++i)
    {
	type_T *type = عدم;

	// assume a v:none argument, using the default value, is always OK
	إذا (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_عدد == VVAL_NONE)
	    استمر;
	// only pass values to user functions, never types
	إذا (check_typval_is_مقدار(&argv[i]) == FAIL)
	    أرجع FAIL;

	إذا (i < ufunc->uf_آرگومانها.ga_len && ufunc->uf_arg_types != عدم)
	    type = ufunc->uf_arg_types[i];
	وإلا إذا (ufunc->uf_va_type != عدم)
	    type = ufunc->uf_va_type->tt_member;
	إذا (type != عدم && check_typval_arg_type(type,
		    &argv[i], عدم, i + 1) == FAIL)
	    أرجع FAIL;
    }
    أرجع OK;
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
    ساكن صحيح
call_dfunc(
	صحيح		cdf_idx,
	partial_T	*pt_arg,
	صحيح		argcount_arg,
	ectx_T		*ectx)
{
    صحيح		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_داده) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    صحيح		did_emsg_before = did_emsg_cumul + did_emsg;
    صحيح		arg_مقصد_جمع_کن;
    صحيح		vararg_شمار = 0;
    صحيح		varcount;
    صحيح		idx;
    estack_T	*مدخل;
    funclocal_T	*floc = عدم;
    صحيح		res = OK;
    compiletype_T compile_type;

    إذا (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == عدم ? (char_u *)"unknown" : dfunc->df_name);
	أرجع FAIL;
    }

#اگر_تعریف_شده FEAT_PROFILE
    إذا (do_profiling == PROF_YES)
    {
	إذا (GA_GROW_OK(&profile_info_ga, 1))
	{
	    profinfo_T *info = ((profinfo_T *)profile_info_ga.ga_داده)
						      + profile_info_ga.ga_len;
	    ++profile_info_ga.ga_len;
	    CLEAR_اشاره_گر(info);
	    profile_may_شروع_func(info, ufunc,
			(((dfunc_T *)def_functions.ga_داده)
					      + ectx->ec_dfunc_idx)->df_ufunc);
	}
    }
#پایان_اگر

    // When debugging and using "cont" switches to the not-debugged
    // instructions, may need to still compile them.
    compile_type = get_compile_type(ufunc);
    إذا (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, عدم);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_داده) + cdf_idx;
    }
    إذا (res == FAIL || INSTRUCTIONS(dfunc) == عدم)
    {
	إذا (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	أرجع FAIL;
    }

    إذا (ufunc->uf_va_name != عدم)
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
	vararg_شمار = argcount - ufunc->uf_آرگومانها.ga_len;
	إذا (vararg_شمار < 0)
	    vararg_شمار = 0;
	وإلا
	    argcount -= vararg_شمار;
	إذا (exe_newlist(vararg_شمار, ectx) == FAIL)
	    أرجع FAIL;

	vararg_شمار = 1;
    }

    arg_مقصد_جمع_کن = ufunc->uf_آرگومانها.ga_len - argcount;
    إذا (arg_مقصد_جمع_کن < 0)
    {
	semsg(NGETTEXT(e_one_آرگومان_too_many, e_nr_arguments_too_many,
			-arg_مقصد_جمع_کن), -arg_مقصد_جمع_کن);
	أرجع FAIL;
    }
    وإلا إذا (arg_مقصد_جمع_کن > ufunc->uf_def_آرگومانها.ga_len)
    {
	صحيح missing = arg_مقصد_جمع_کن - ufunc->uf_def_آرگومانها.ga_len;

	semsg(NGETTEXT(e_one_آرگومان_too_few, e_nr_arguments_too_few,
			missing), missing);
	أرجع FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = پشته_TV_BOT(0) - argcount - vararg_شمار - 1;

    إذا (IS_OBJECT_روش(ufunc) && !IS_CONSTRUCTOR_روش(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == عدم)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	أرجع FAIL;
    }

    // Check the argument types.
    إذا (check_ufunc_arg_types(ufunc, argcount, vararg_شمار, ectx) == FAIL)
	أرجع FAIL;

    // While check_ufunc_arg_types call, def function compilation process may
    // run.  If so many def functions are compiled, def_functions array may be
    // reallocated and dfunc may no longer have valid pointer.  Get the object
    // pointer from def_functions again here.
    dfunc = ((dfunc_T *)def_functions.ga_داده) + cdf_idx;

    // Reserve space for:
    // - missing arguments
    // - stack frame
    // - local variables
    // - if needed: a counter for number of closures created in
    //   ectx->ec_funcrefs.
    varcount = dfunc->df_varcount + dfunc->df_has_بستگی;
    إذا (GA_GROW_FAILS(&ectx->ec_پشته,
				     arg_مقصد_جمع_کن + پشته_قاب_اندازه + varcount))
	أرجع FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = پشته_TV_BOT(0) - argcount - vararg_شمار - 1;

    // If depth of calling is getting too high, don't execute the function.
    إذا (funcdepth_increment() == FAIL)
	أرجع FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    إذا (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	إذا (floc == عدم)
	    أرجع FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    إذا (vararg_شمار > 0 && arg_مقصد_جمع_کن > 0)
	*پشته_TV_BOT(arg_مقصد_جمع_کن - 1) = *پشته_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    لكل (idx = 0; idx < arg_مقصد_جمع_کن; ++idx)
	پشته_TV_BOT(idx - vararg_شمار)->v_type = VAR_UNKNOWN;
    ectx->ec_پشته.ga_len += arg_مقصد_جمع_کن;

    // Store current execution state in stack frame for ISN_RETURN.
    پشته_TV_BOT(پشته_قاب_FUNC_OFF)->vval.v_عدد = ectx->ec_dfunc_idx;
    پشته_TV_BOT(پشته_قاب_IIDX_OFF)->vval.v_عدد = ectx->ec_iidx;
    پشته_TV_BOT(پشته_قاب_INSTR_OFF)->vval.v_string = (فراغ *)ectx->ec_instr;
    پشته_TV_BOT(پشته_قاب_OUTER_OFF)->vval.v_string =
						    (فراغ *)ectx->ec_outer_ref;
    پشته_TV_BOT(پشته_قاب_FUNCLOCAL_OFF)->vval.v_string = (فراغ *)floc;
    پشته_TV_BOT(پشته_قاب_IDX_OFF)->vval.v_عدد = ectx->ec_قاب_idx;
    ectx->ec_قاب_idx = ectx->ec_پشته.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    صحيح initcount = dfunc->df_varcount + (dfunc->df_has_بستگی ? 1 : 0);
    لكل (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = پشته_TV_BOT(پشته_قاب_اندازه + idx);

	tv->v_type = VAR_عدد;
	tv->vval.v_عدد = 0;
    }
    ectx->ec_پشته.ga_len += پشته_قاب_اندازه + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    إذا (IS_OBJECT_روش(ufunc))
    {
	إذا (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_داخلی_error_str), "type in stack is not an object");
	    أرجع FAIL;
	}

	*پشته_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != عدم ? pt_arg : ufunc->uf_partial;
    إذا (pt != عدم || (ufunc->uf_flags & FC_بستگی))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	إذا (ref == عدم)
	    أرجع FAIL;
	إذا (pt != عدم)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	وإلا
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    إذا (unlikely(ref->or_outer == عدم))
	    {
		vim_free(ref);
		أرجع FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->بیرون_پشته = &ectx->ec_پشته;
	    ref->or_outer->بیرون_قاب_idx = ectx->ec_قاب_idx;
	    إذا (ectx->ec_outer_ref != عدم)
		ref->or_outer->بیرون_به_بالا = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    وإلا
	ectx->ec_outer_ref = عدم;

    إذا (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    مدخل = estack_افزودن_ufunc(ufunc, 1);
    إذا (مدخل != عدم)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	مدخل->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    أرجع OK;
}

// Double linked list of funcstack_T in use.
ساكن funcstack_T *اولی_funcstack = عدم;

    ساكن فراغ
جمع_کن_funcstack_مقصد_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    إذا (اولی_funcstack != عدم)
	اولی_funcstack->fs_prev = funcstack;
    funcstack->fs_بعدی = اولی_funcstack;
    funcstack->fs_prev = عدم;
    اولی_funcstack = funcstack;
}

    ساكن فراغ
remove_funcstack_from_list(funcstack_T *funcstack)
{
    إذا (funcstack->fs_prev == عدم)
	اولی_funcstack = funcstack->fs_بعدی;
    وإلا
	funcstack->fs_prev->fs_بعدی = funcstack->fs_بعدی;
    إذا (funcstack->fs_بعدی != عدم)
	funcstack->fs_بعدی->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    ساكن صحيح
handle_بستگی_درون_use(ectx_T *ectx, صحيح free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_داده)
							  + ectx->ec_dfunc_idx;
    صحيح		argcount;
    صحيح		بالاترین;
    صحيح		idx;
    typval_T	*tv;
    صحيح		بستگی_درون_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	بستگی_شمار;

    إذا (dfunc->df_ufunc == عدم)
	أرجع OK;  // function was freed
    إذا (dfunc->df_has_بستگی == 0)
	أرجع OK;  // no closures
    tv = پشته_TV(ectx->ec_قاب_idx + پشته_قاب_اندازه + dfunc->df_varcount);
    بستگی_شمار = tv->vval.v_عدد;
    إذا (بستگی_شمار == 0)
	أرجع OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    بالاترین = ectx->ec_قاب_idx - argcount;

    // Check if any created closure is still in use.
    لكل (idx = 0; idx < بستگی_شمار; ++idx)
    {
	partial_T   *pt;
	صحيح	    off = gap->ga_len - بستگی_شمار + idx;

	إذا (off < 0)
	    استمر;  // count is off or already done
	pt = ((partial_T **)gap->ga_داده)[off];
	إذا (pt->pt_refcount > 1)
	{
	    صحيح refcount = pt->pt_refcount;
	    صحيح i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    لكل (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = پشته_TV(ectx->ec_قاب_idx
						       + پشته_قاب_اندازه + i);
		إذا (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    إذا (refcount > 1)
	    {
		بستگی_درون_use = TRUE;
		اكسر;
	    }
	}
    }

    إذا (بستگی_درون_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *پشته;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	إذا (funcstack == عدم)
	    أرجع FAIL;

	funcstack->fs_var_جابه_جایی = argcount + پشته_قاب_اندازه;
	funcstack->fs_ga.ga_len = funcstack->fs_var_جابه_جایی
							  + dfunc->df_varcount;
	پشته = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_داده = پشته;
	إذا (پشته == عدم)
	{
	    vim_free(funcstack);
	    أرجع FAIL;
	}
	جمع_کن_funcstack_مقصد_list(funcstack);

	// Move or copy the arguments.
	لكل (idx = 0; idx < argcount; ++idx)
	{
	    tv = پشته_TV(بالاترین + idx);
	    إذا (free_arguments)
	    {
		*(پشته + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    وإلا إذا (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, پشته + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	لكل (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = پشته_TV(ectx->ec_قاب_idx + پشته_قاب_اندازه + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    إذا (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != عدم)
	    {
		صحيح	    i;

		لكل (i = 0; i < بستگی_شمار; ++i)
		    إذا (tv->vval.v_partial == ((partial_T **)gap->ga_داده)[
					      gap->ga_len - بستگی_شمار + i])
			++funcstack->fs_min_refcount;
	    }

	    *(پشته + funcstack->fs_var_جابه_جایی + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	لكل (idx = 0; idx < بستگی_شمار; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_داده)[gap->ga_len
							- بستگی_شمار + idx];
	    إذا (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.بیرون_پشته = &funcstack->fs_ga;
		pt->pt_outer.بیرون_قاب_idx = ectx->ec_قاب_idx - بالاترین;
	    }
	}
    }

    لكل (idx = 0; idx < بستگی_شمار; ++idx)
	partial_unref(((partial_T **)gap->ga_داده)[gap->ga_len
						       - بستگی_شمار + idx]);
    gap->ga_len -= بستگی_شمار;
    إذا (gap->ga_len == 0)
	ga_clear(gap);

    أرجع OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    صحيح
funcstack_check_refcount(funcstack_T *funcstack)
{
    صحيح		i;
    garray_T	*gap = &funcstack->fs_ga;
    صحيح		انجامشده = 0;
    typval_T	*پشته;

    إذا (funcstack->fs_refcount > funcstack->fs_min_refcount)
	أرجع FALSE;
    لكل (i = funcstack->fs_var_جابه_جایی; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_داده) + i;

	إذا (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != عدم
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++انجامشده;
    }
    إذا (انجامشده != funcstack->fs_min_refcount)
	أرجع FALSE;

    پشته = gap->ga_داده;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    لكل (i = 0; i < gap->ga_len; ++i)
	clear_tv(پشته + i);
    vim_free(پشته);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    أرجع TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    صحيح
مجموعه_ref_درون_funcstacks(صحيح copyID)
{
    funcstack_T *funcstack;

    لكل (funcstack = اولی_funcstack; funcstack != عدم;
						funcstack = funcstack->fs_بعدی)
    {
	typval_T    *پشته = funcstack->fs_ga.ga_داده;
	صحيح	    i;

	لكل (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    إذا (مجموعه_ref_درون_قلم(پشته + i, copyID, عدم, عدم, عدم))
		أرجع TRUE;  // abort
    }
    أرجع FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
ساكن ectx_T *current_ectx = عدم;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    صحيح
درون_def_function(فراغ)
{
    أرجع current_ectx != عدم;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    صحيح
fill_exec_lval_ریشه(lval_ریشه_T *ریشه)
{
    ectx_T *ectx = current_ectx;
    إذا (ectx != عدم)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_داده)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	إذا (ufunc->uf_class != عدم)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    إذا (tv != عدم)
	    {
		CLEAR_اشاره_گر(ریشه);
		ریشه->lr_tv = tv;
		copy_tv(پشته_TV_VAR(0), ریشه->lr_tv);
		ریشه->lr_cl_exec = ufunc->uf_class;
		++ریشه->lr_cl_exec->class_refcount;
		أرجع OK;
	    }
	}
    }

    أرجع FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(فراغ)
{
    ectx_T *r = current_ectx;

    current_ectx = عدم;
    أرجع r;
}

    فراغ
restore_current_ectx(ectx_T *ectx)
{
    إذا (current_ectx != عدم)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    ساكن list_T *
جمع_کن_defer_قلم(صحيح var_idx, صحيح argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = پشته_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    إذا (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	إذا (rettv_list_alloc(defer_tv) == FAIL)
	    أرجع عدم;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_اقلام(argcount + 1);
    إذا (l == عدم)
	أرجع عدم;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_قفل = 0;
    إذا (list_درج_tv(defer_l, &listval, defer_l->lv_اولی) == FAIL)
    {
	list_free(l);
	أرجع عدم;
    }

    أرجع l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    ساكن صحيح
defer_command(صحيح var_idx, صحيح argcount, ectx_T *ectx)
{
    list_T	*l = جمع_کن_defer_قلم(var_idx, argcount, ectx);
    صحيح		i;
    typval_T	*func_tv;

    إذا (l == عدم)
	أرجع FAIL;

    func_tv = پشته_TV_BOT(-argcount - 1);
    إذا (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	أرجع FAIL;
    }
    list_مجموعه_قلم(l, 0, func_tv);

    لكل (i = 0; i < argcount; ++i)
	list_مجموعه_قلم(l, i + 1, پشته_TV_BOT(-argcount + i));
    ectx->ec_پشته.ga_len -= argcount + 1;
    أرجع OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    صحيح
جمع_کن_defer_function(char_u *name, صحيح argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_داده)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    صحيح		i;

    إذا (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	أرجع FAIL;
    }

    l = جمع_کن_defer_قلم(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    إذا (l == عدم)
    {
	vim_free(name);
	أرجع FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_قفل = 0;
    func_tv.vval.v_string = name;
    list_مجموعه_قلم(l, 0, &func_tv);

    لكل (i = 0; i < argcount; ++i)
	list_مجموعه_قلم(l, i + 1, argvars + i);
    أرجع OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    ساكن فراغ
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_داده)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = پشته_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    إذا (defer_tv->v_type != VAR_LIST)
	أرجع;	 // no function added
    FOR_ALL_LIST_اقلام(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_آرگومانها];
	صحيح	    i;
	listitem_T  *arg_li = l->lv_اولی;
	typval_T    *functv = &l->lv_اولی->li_tv;
	صحيح	    argcount = l->lv_len - 1;

	إذا (functv->vval.v_string == عدم)
	    // already being called, can happen if function does ":qa"
	    استمر;

	لكل (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_بعدی;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = عدم;
	partial_T	*pt = عدم;

	CLEAR_فیلد(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	إذا (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = عدم;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    إذا (funcexe.fe_object != عدم)
		++funcexe.fe_object->obj_refcount;
	}
	وإلا
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = عدم;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(فراغ)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	إذا (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	وإلا
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    ساكن صحيح
func_return(ectx_T *ectx)
{
    صحيح		idx;
    صحيح		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_داده)
							  + ectx->ec_dfunc_idx;
    صحيح		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*مدخل;
    صحيح		prev_dfunc_idx = پشته_TV(ectx->ec_قاب_idx
					+ پشته_قاب_FUNC_OFF)->vval.v_عدد;
    funclocal_T	*floc;
#اگر_تعریف_شده FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_داده)
							      + prev_dfunc_idx;

    إذا (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	إذا (dfunc->df_ufunc->uf_profiling
				   || (caller != عدم && caller->uf_profiling))
	{
	    profile_may_انتها_func(((profinfo_T *)profile_info_ga.ga_داده)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#پایان_اگر

    إذا (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    إذا (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    مدخل = estack_برداشت();
    إذا (مدخل != عدم)
	current_sctx = مدخل->es_save_sctx;

    إذا (handle_بستگی_درون_use(ectx, TRUE) == FAIL)
	أرجع FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    صحيح بالاترین = ectx->ec_قاب_idx - argcount;
    إذا (IS_OBJECT_روش(dfunc->df_ufunc))
	--بالاترین;
    لكل (idx = بالاترین; idx < ectx->ec_قاب_idx; ++idx)
	clear_tv(پشته_TV(idx));

    // Clear local variables and temp values, but not the return value.
    لكل (idx = ectx->ec_قاب_idx + پشته_قاب_اندازه;
					idx < ectx->ec_پشته.ga_len - 1; ++idx)
	clear_tv(پشته_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_پشته.ga_len - 1;
    إذا (ret_idx == ectx->ec_قاب_idx + پشته_قاب_IDX_OFF)
	ret_idx = 0;

    إذا (ectx->ec_outer_ref != عدم)
    {
	إذا (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = پشته_TV(ectx->ec_قاب_idx
					+ پشته_قاب_IIDX_OFF)->vval.v_عدد;
    ectx->ec_instr = (فراغ *)پشته_TV(ectx->ec_قاب_idx
				       + پشته_قاب_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (فراغ *)پشته_TV(ectx->ec_قاب_idx
				       + پشته_قاب_OUTER_OFF)->vval.v_string;
    floc = (فراغ *)پشته_TV(ectx->ec_قاب_idx
				   + پشته_قاب_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_قاب_idx = پشته_TV(ectx->ec_قاب_idx
				       + پشته_قاب_IDX_OFF)->vval.v_عدد;

    إذا (floc == عدم)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    وإلا
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    إذا (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_پشته.ga_len = بالاترین + 1;
	*پشته_TV_BOT(-1) = *پشته_TV(ret_idx);
    }
    وإلا
	// Reset the stack to the position before the call.
	ectx->ec_پشته.ga_len = بالاترین;

    funcdepth_decrement();
    --ex_nesting_level;
    أرجع OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    ساكن صحيح
call_prepare(صحيح argcount, typval_T *argvars, ectx_T *ectx)
{
    صحيح		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    لكل (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *پشته_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    إذا (argcount > 0)
	ectx->ec_پشته.ga_len -= argcount - 1;
    وإلا إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
	أرجع FAIL;
    وإلا
	++ectx->ec_پشته.ga_len;

    // Default return value is zero.
    tv = پشته_TV_BOT(-1);
    tv->v_type = VAR_عدد;
    tv->vval.v_عدد = 0;
    tv->v_قفل = 0;

    أرجع OK;
}

/*
 * Call a builtin function by index.
 */
    ساكن صحيح
call_bfunc(صحيح func_idx, صحيح argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_آرگومانها];
    صحيح		idx;
    صحيح		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    حرف	*save_func_name = ectx->ec_where.wt_func_name;

    إذا (call_prepare(argcount, argvars, ectx) == FAIL)
	أرجع FAIL;

    // Check for void value being passed as an argument.
    لكل (idx = 0; idx < argcount; ++idx)
	إذا (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_مقدار));
	    لكل (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    أرجع FAIL;
	}

    ectx->ec_where.wt_func_name = داخلی_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_داخلی_func_by_idx(func_idx, argvars, پشته_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    لكل (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    إذا (did_emsg > did_emsg_before)
	أرجع FAIL;
    أرجع OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ساكن صحيح
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	صحيح	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_آرگومانها];
    funcexe_T   funcexe;
    funcerror_T	error;
    صحيح		idx;
    صحيح		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    إذا (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, عدم)
								       == FAIL)
	أرجع FAIL;
    إذا (ufunc->uf_def_وضعیت == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	إذا (error != FCERR_UNKNOWN)
	{
	    إذا (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    وإلا
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    أرجع FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	إذا (iptr != عدم)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	أرجع call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    إذا (call_prepare(argcount, argvars, ectx) == FAIL)
	أرجع FAIL;
    CLEAR_فیلد(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != عدم ? selfdict : dict_پشته_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      پشته_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    لكل (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    إذا (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_یافت_var);
	أرجع FAIL;
    }
    إذا (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	أرجع FAIL;
    أرجع OK;
}

/*
 * If command modifiers were applied restore them.
 */
    ساكن فراغ
may_restore_cmdmod(funclocal_T *funclocal)
{
    إذا (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = عدم;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    ساكن صحيح
vim9_aborting(صحيح prev_uncaught_emsg)
{
    أرجع uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    ساكن صحيح
call_by_name(
	char_u	    *name,
	صحيح	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    إذا (builtin_function(name, -1))
    {
	صحيح func_idx = پیدا_کن_داخلی_func(name);

	إذا (func_idx < 0)  // Impossible?
	    أرجع FAIL;
	إذا (check_داخلی_func(func_idx, argcount) < 0)
	    أرجع FAIL;
	أرجع call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*شروع_bracket = generic_func_پیدا_کن_open_bracket(name);
    إذا (شروع_bracket != عدم)
    {
	cc = *شروع_bracket;
	*شروع_bracket = NUL;
    }

    ufunc = پیدا_کن_func(name, FALSE);

    إذا (ufunc == عدم)
    {
	صحيح prev_uncaught_emsg = uncaught_emsg;

	إذا (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = پیدا_کن_func(name, FALSE);

	إذا (vim9_aborting(prev_uncaught_emsg))
	{
	    إذا (شروع_bracket != عدم)
		*شروع_bracket = cc;
	    أرجع FAIL;  // bail out if loading the script caused an error
	}
    }

    إذا (شروع_bracket != عدم)
	*شروع_bracket = cc;

    إذا (ufunc != عدم)
    {
	إذا (IS_GENERIC_FUNC(ufunc))
	{
	    إذا (شروع_bracket != عدم)
		ufunc = پیدا_کن_generic_func(ufunc, name, &شروع_bracket);
	    وإلا
	    {
		emsg_funcname(e_generic_func_missing_type_آرگومانها_str, name);
		ufunc = عدم;
	    }
	    إذا (ufunc == عدم)
		أرجع FAIL;
	}
	وإلا إذا (شروع_bracket != عدم)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    أرجع FAIL;
	}

	إذا (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    أرجع FAIL;

	أرجع call_ufunc(ufunc, عدم, argcount, ectx, iptr, selfdict);
    }

    أرجع FAIL;
}

    ساكن صحيح
call_partial(
	typval_T    *tv,
	صحيح	    argcount_arg,
	ectx_T	    *ectx)
{
    صحيح		argcount = argcount_arg;
    char_u	*name = عدم;
    صحيح		called_emsg_before = called_emsg;
    صحيح		res = FAIL;
    dict_T	*selfdict = عدم;

    إذا (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	صحيح	    i;

	إذا (pt->pt_obj != عدم)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		أرجع FAIL;
	    لكل (i = 1; i <= argcount; ++i)
		*پشته_TV_BOT(-i + 1) = *پشته_TV_BOT(-i);

	    typval_T *obj_tv = پشته_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_قفل = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_پشته.ga_len;
	}

	إذا (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    إذا (GA_GROW_FAILS(&ectx->ec_پشته, pt->pt_argc))
		أرجع FAIL;
	    لكل (i = 1; i <= argcount; ++i)
		*پشته_TV_BOT(-i + pt->pt_argc) = *پشته_TV_BOT(-i);
	    ectx->ec_پشته.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    لكل (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], پشته_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	إذا (pt->pt_func != عدم)
	    أرجع call_ufunc(pt->pt_func, pt, argcount, ectx, عدم, selfdict);

	name = pt->pt_name;
    }
    وإلا إذا (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    إذا (name != عدم)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = عدم;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	إذا (error != FCERR_NONE)
	    res = FAIL;
	وإلا
	    res = call_by_name(fname, argcount, ectx, عدم, selfdict);
	vim_free(tofree);
    }

    إذا (res == FAIL)
    {
	إذا (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == عدم ? (char_u *)"[unknown]" : name);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    ساكن صحيح
error_if_locked(صحيح قفل, حرف *error)
{
    إذا (قفل & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    ساكن صحيح
check_for_عدد(typval_T *tv)
{
    إذا (tv->v_type != VAR_عدد)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_عدد), vartype_name(tv->v_type));
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    ساكن فراغ
store_var(char_u *name, typval_T *tv)
{
    funccal_مدخل_T مدخل;
    صحيح		    flags = تخصیص_بده_DECL;

    إذا (tv->v_قفل)
	flags |= تخصیص_بده_CONST;
    save_funccal(&مدخل);
    مجموعه_var_const(name, 0, عدم, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    ساكن صحيح
do_2string(typval_T *tv, صحيح is_2string_any, صحيح tostring_flags)
{
    إذا (tv->v_type == VAR_STRING)
	أرجع OK;

    char_u *str;

    إذا (is_2string_any)
    {
	بدل (tv->v_type)
	{
	    حالة VAR_SPECIAL:
	    حالة VAR_BOOL:
	    حالة VAR_عدد:
	    حالة VAR_FLOAT:
	    حالة VAR_DICT:
	    حالة VAR_BLOB:	اكسر;

	    حالة VAR_LIST:
				إذا (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, حجم(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    إذا (str == عدم)
					أرجع FAIL;
				    s = str;
				    طالما ((e = vim_strchr(s, '\n')) != عدم)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					إذا (p != عدم)
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
				    tv->vval.v_string = ga.ga_داده;
				    أرجع OK;
				}
				إذا (tostring_flags & TOSTRING_INTERPOLATE)
				    اكسر;
				// FALLTHROUGH
	    افتراضي:	مقصد_string_error(tv->v_type);
			أرجع FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    أرجع OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    ساكن فراغ
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    بدل (tv->v_type)
    {
	حالة VAR_LIST:
	    إذا (tv->vval.v_list == عدم && sv->sv_type != &t_list_خالی)
		(فراغ)rettv_list_alloc(tv);
	    اكسر;
	حالة VAR_TUPLE:
	    إذا (tv->vval.v_tuple == عدم && sv->sv_type != &t_tuple_خالی)
		(فراغ)rettv_tuple_alloc(tv);
	    اكسر;
	حالة VAR_DICT:
	    إذا (tv->vval.v_dict == عدم && sv->sv_type != &t_dict_خالی)
		(فراغ)rettv_dict_alloc(tv);
	    اكسر;
	حالة VAR_BLOB:
	    إذا (tv->vval.v_blob == عدم && sv->sv_type != &t_blob_null)
		(فراغ)rettv_blob_alloc(tv);
	    اكسر;
	افتراضي:
	    اكسر;
    }
}

/*
 * Return the character "str[index]" where "index" is the character index,
 * including composing characters.
 * If "index" is out of range NULL is returned.
 */
    char_u *
char_from_string(char_u *str, varnumber_T شاخص)
{
    size_t	    nbyte = 0;
    varnumber_T	    nchar = شاخص;
    size_t	    slen;

    إذا (str == عدم)
	أرجع عدم;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    إذا (شاخص < 0)
    {
	صحيح	clen = 0;

	لكل (nbyte = 0; nbyte < slen; ++clen)
	{
	    إذا (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    وإلا إذا (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    وإلا
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + شاخص;
	إذا (nchar < 0)
	    // unlike list: index out of range results in empty string
	    أرجع عدم;
    }

    لكل (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	إذا (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	وإلا إذا (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	وإلا
	    nbyte += mb_ptr2len(str + nbyte);
    }
    إذا (nbyte >= slen)
	أرجع عدم;
    أرجع vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    ساكن دراز
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    إذا (nchar >= 0)
    {
	طالما (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    وإلا
    {
	nbyte = str_len;
	طالما (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_سر_off(str, str + nbyte);
	    ++nchar;
	}
	إذا (nchar < 0)
	    أرجع -1;
    }
    أرجع (دراز)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_برش(char_u *str, varnumber_T اولی, varnumber_T آخرین, صحيح exclusive)
{
    دراز	شروع_byte, انتها_byte;
    size_t	slen;

    إذا (str == عدم)
	أرجع عدم;
    slen = STRLEN(str);
    شروع_byte = char_idx2byte(str, slen, اولی);
    إذا (شروع_byte < 0)
	شروع_byte = 0; // first index very negative: use zero
    إذا ((آخرین == -1 && !exclusive) || آخرین == VARNUM_MAX)
	انتها_byte = (دراز)slen;
    وإلا
    {
	انتها_byte = char_idx2byte(str, slen, آخرین);
	إذا (!exclusive && انتها_byte >= 0 && انتها_byte < (دراز)slen)
	    // end index is inclusive
	    انتها_byte += mb_ptr2len(str + انتها_byte);
    }

    إذا (شروع_byte >= (دراز)slen || انتها_byte <= شروع_byte)
	أرجع عدم;
    أرجع vim_strnsave(str + شروع_byte, انتها_byte - شروع_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    ساكن svar_T *
get_script_svar(scriptref_T *sref, صحيح dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_قلم(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? عدم
			      : ((dfunc_T *)def_functions.ga_داده) + dfunc_idx;
    svar_T	    *sv;

    إذا (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	إذا (dfunc != عدم)
	    semsg(_(e_script_variable_invalid_after_reload_درون_function_str),
					 printable_func_name(dfunc->df_ufunc));
	أرجع عدم;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_داده) + sref->sref_idx;
    إذا (sv->sv_name == عدم)
    {
	إذا (dfunc != عدم)
	    emsg(_(e_script_variable_was_deleted));
	أرجع عدم;
    }
    إذا (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	إذا (dfunc != عدم)
	    emsg(_(e_script_variable_type_تغییریافته));
	أرجع عدم;
    }

    إذا ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	إذا (dfunc != عدم)
	    semsg(_(e_قلم_not_exported_درون_script_str), sv->sv_name);
	أرجع عدم;
    }
    أرجع sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    ساكن char_u *
get_تقسیم_sourceline(
	صحيح c استفادهنشده,
	فراغ *cookie,
	صحيح indent استفادهنشده,
	getline_opt_T options استفادهنشده)
{
    منبع_cookie_T	*sp = (منبع_cookie_T *)cookie;
    char_u		*p;
    char_u		*سطر;

    p = vim_strchr(sp->nextline, '\n');
    إذا (p == عدم)
    {
	سطر = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    وإلا
    {
	سطر = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    أرجع سطر;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ساكن صحيح
call_eval_func(
	char_u	    *name,
	صحيح	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    صحيح	    called_emsg_before = called_emsg;
    صحيح	    res;

    res = call_by_name(name, argcount, ectx, iptr, عدم);
    إذا (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = پیدا_کن_var(name, عدم, FALSE);
	إذا (v == عدم || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    أرجع FAIL;
	}
	أرجع call_partial(&v->di_tv, argcount, ectx);
    }
    أرجع res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    صحيح
fill_partial_and_بستگی(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    إذا (ufunc->uf_flags & FC_بستگی)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_داده)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.بیرون_پشته = &ectx->ec_پشته;
	pt->pt_outer.بیرون_قاب_idx = ectx->ec_قاب_idx;
	إذا (ectx->ec_outer_ref != عدم)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.بیرون_به_بالا = ectx->ec_outer_ref->or_outer;
	    إذا (ectx->ec_outer_ref->or_partial != عدم)
	    {
		pt->pt_outer.بیرون_به_بالا_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.بیرون_به_بالا_partial->pt_refcount;
	    }
	}

	إذا (lvi != عدم)
	{
	    صحيح	عمق;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    لكل (عمق = 0; عمق < lvi->lvi_عمق; ++عمق)
	    {
		pt->pt_outer.بیرون_loop[عمق].پشته = &ectx->ec_پشته;
		pt->pt_outer.بیرون_loop[عمق].var_idx = ectx->ec_قاب_idx
			 + پشته_قاب_اندازه + lvi->lvi_loop[عمق].var_idx;
		pt->pt_outer.بیرون_loop[عمق].var_شمار =
					    lvi->lvi_loop[عمق].var_شمار;
	    }
	    pt->pt_outer.بیرون_loop_اندازه = lvi->lvi_عمق;
	}
	وإلا
	    pt->pt_outer.بیرون_loop_اندازه = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	إذا (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    أرجع FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_پشته.ga_داده) + ectx->ec_قاب_idx
		       + پشته_قاب_اندازه + dfunc->df_varcount)->vval.v_عدد;

	((partial_T **)ectx->ec_funcrefs.ga_داده)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    أرجع OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    ساكن صحيح
exec_command(isn_T *iptr, char_u *cmd_string)
{
    منبع_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_فیلد(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    إذا (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	أرجع FAIL;
    أرجع OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    صحيح
may_بارگیری_script(صحيح sid, صحيح *loaded)
{
    scriptitem_T *si = SCRIPT_قلم(sid);

    إذا (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	إذا (do_منبع(si->sn_name, FALSE, DOSO_NONE, عدم) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    أرجع FAIL;
	}
    }
    أرجع OK;
}

// used for v_instr of typval of VAR_INSTR
بنية instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
تعريفنوع بنية subs_عبارت_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    صحيح		subs_وضعیت;
} subs_عبارت_T;

// Set when calling do_debug().
ساكن ectx_T	*debug_context = عدم;
ساكن صحيح	debug_var_شمار;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    صحيح		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    صحيح		    varargs_off;

    إذا (ectx == عدم)
	أرجع عدم;
    dfunc = ((dfunc_T *)def_functions.ga_داده) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    لكل (idx = debug_var_شمار - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_نامها.ga_داده)[idx];

	// the variable name may be NULL when not available in this block
	إذا (varname != عدم && STRCMP(varname, name) == 0)
	    أرجع پشته_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == عدم ? 0 : 1;
    لكل (idx = 0; idx < ufunc->uf_آرگومانها.ga_len; ++idx)
	إذا (STRCMP(((char_u **)(ufunc->uf_آرگومانها.ga_داده))[idx], name) == 0)
	    أرجع پشته_TV(ectx->ec_قاب_idx - ufunc->uf_آرگومانها.ga_len
							  - varargs_off + idx);
    إذا (ufunc->uf_va_name != عدم && STRCMP(ufunc->uf_va_name, name) == 0)
	أرجع پشته_TV(ectx->ec_قاب_idx - 1);

    أرجع عدم;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    صحيح
may_break_درون_function(ufunc_T *ufunc)
{
    أرجع ufunc->uf_has_breakpoint || debug_has_عبارت_breakpoint();
}

    ساكن فراغ
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*سطر;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_داده)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    صحيح		انتها_lnum = iptr->isn_lnum;
    garray_T	ga;
    صحيح		lnum;

    إذا (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	إذا (!may_break_درون_function(ufunc))
	    أرجع;

	// check for the next breakpoint if needed
	breakpoint = dbg_پیدا_کن_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	إذا (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    أرجع;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_شمار = iptr->isn_arg.debug.dbg_var_نامها_len;

    لكل (ni = iptr + 1; ni->isn_type != ISN_پایان; ++ni)
	إذا (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    انتها_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    اكسر;
	}

    إذا (انتها_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, حجم(char_u *), 10);
	لكل (lnum = iptr->isn_lnum; lnum < انتها_lnum
				     && lnum <= ufunc->uf_سطرها.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_سطرها.ga_داده)[lnum - 1];

	    إذا (p == عدم)
		استمر;  // left over from continuation line
	    p = skipwhite(p);
	    إذا (*p == '#')
		اكسر;
	    إذا (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_داده))[ga.ga_len++] = p;
	    إذا (STRNCMP(p, "def ", 4) == 0)
		اكسر;
	}
	سطر = ga_concat_رشتهها(&ga, "  ");
	vim_free(ga.ga_داده);
    }
    وإلا
	سطر = ((char_u **)ufunc->uf_سطرها.ga_داده)[iptr->isn_lnum - 1];

    do_debug(سطر == عدم ? (char_u *)"[empty]" : سطر);
    debug_context = عدم;

    إذا (انتها_lnum > iptr->isn_lnum)
	vim_free(سطر);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    ساكن صحيح
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    إذا (lhs_list->lv_type == عدم || lhs_list->lv_type->tt_member == عدم)
	أرجع OK;

    أرجع check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    ساكن صحيح
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = پشته_TV_BOT(-2);
    دراز	lidx = 0;
    typval_T	*tv_dest = پشته_TV_BOT(-1);
    صحيح		وضعیت = OK;
    صحيح		check_rhs_type = FALSE;

    إذا (tv_idx->v_type == VAR_عدد)
	lidx = (دراز)tv_idx->vval.v_عدد;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = پشته_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    إذا (dest_type == VAR_OBJECT && tv_dest->vval.v_object == عدم)
    {
	emsg(_(e_using_null_object));
	وضعیت = FAIL;
    }
    وإلا إذا (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	إذا (dest_type == VAR_DICT)
	    وضعیت = do_2string(tv_idx, TRUE, FALSE);
	وإلا إذا (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    إذا (obj == عدم)
	    {
		emsg(_(e_using_null_object));
		وضعیت = FAIL;
	    }
	    وإلا
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		صحيح		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		إذا (m != عدم)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_داده)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    إذا ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == عدم ||
			     !class_instance_از(ufunc->uf_class, cl)))
		    {
			حرف *پیام = (m->ocm_access == VIM_ACCESS_خصوصی)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(پیام, m->ocm_name.string, 0, cl);
			وضعیت = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    وإلا إذا (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			وضعیت = FAIL;
		    وإلا
			lidx = m_idx;
		}
		وإلا
		{
		    member_not_یافت_پیام(cl, VAR_OBJECT, member, 0);
		    وضعیت = FAIL;
		}
	    }
	}
	وإلا إذا (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    وضعیت = FAIL;
	}
	وإلا إذا (dest_type == VAR_LIST && tv_idx->v_type != VAR_عدد)
	{
	    emsg(_(e_عدد_expected));
	    وضعیت = FAIL;
	}
    }

    إذا (وضعیت == OK)
    {
	إذا (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    إذا (list == عدم)
	    {
		emsg(_(e_list_not_مجموعه));
		أرجع FAIL;
	    }
	    إذا (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    إذا (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_شاخص_بیرون_از_range_nr), lidx);
		أرجع FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    إذا (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		أرجع FAIL;

	    إذا (lidx < list->lv_len)
	    {
		listitem_T *li = list_پیدا_کن(list, lidx);
		إذا (li == عدم)
		    أرجع FAIL;
		إذا (error_if_locked(li->li_tv.v_قفل,
					     e_cannot_change_locked_list_قلم))
		    أرجع FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    وإلا
	    {
		إذا (error_if_locked(list->lv_قفل, e_cannot_change_locked_list))
		    أرجع FAIL;
		// append to list, only fails when out of memory
		إذا (list_append_tv(list, tv) == FAIL)
		    أرجع NOTDONE;
		clear_tv(tv);
	    }
	}
	وإلا إذا (dest_type == VAR_DICT)
	{
	    char_u		*کلید = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    إذا (dict == عدم)
	    {
		emsg(_(e_dictionary_not_مجموعه));
		أرجع FAIL;
	    }
	    إذا (کلید == عدم)
		کلید = (char_u *)"";
	    di = dict_پیدا_کن(dict, کلید, -1);
	    إذا (di != عدم)
	    {
		إذا (error_if_locked(di->di_tv.v_قفل,
						    e_cannot_change_dict_قلم))
		    أرجع FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    وإلا
	    {
		إذا (error_if_locked(dict->dv_قفل, e_cannot_change_dict))
		    أرجع FAIL;
		// add to dict, only fails when out of memory
		إذا (dict_جمع_کن_tv(dict, (حرف *)کلید, tv) == FAIL)
		    أرجع NOTDONE;
		clear_tv(tv);
	    }
	}
	وإلا إذا (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    صحيح		    error = FALSE;
	    صحيح		    len;

	    إذا (blob == عدم)
	    {
		emsg(_(e_blob_not_مجموعه));
		أرجع FAIL;
	    }
	    len = blob_len(blob);
	    إذا (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    إذا (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_شاخص_بیرون_از_range_nr), lidx);
		أرجع FAIL;
	    }
	    إذا (مقدار_check_قفل(blob->bv_قفل, (char_u *)"blob", FALSE))
		أرجع FAIL;
	    nr = tv_get_عدد_chk(tv, &error);
	    إذا (error)
		أرجع FAIL;
	    إذا (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_مقدار_for_blob_nr), nr);
		أرجع FAIL;
	    }
	    blob_مجموعه_append(blob, lidx, nr);
	}
	وإلا إذا (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    إذا (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		إذا (itf != عدم)
		    // convert interface member index to class member index
		    lidx = object_شاخص_from_itf_شاخص(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    وإلا
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	وإلا إذا (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    وضعیت = FAIL;
	}
	وإلا
	{
	    وضعیت = FAIL;
	    semsg(_(e_cannot_شاخص_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_پشته.ga_len -= 3;
    إذا (وضعیت == FAIL)
    {
	clear_tv(tv);
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Store a value in a list or blob range.
 */
    ساكن صحيح
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = پشته_TV_BOT(-3);
    typval_T	*tv_idx2 = پشته_TV_BOT(-2);
    typval_T	*tv_dest = پشته_TV_BOT(-1);
    صحيح		وضعیت = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = پشته_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    إذا (tv_dest->v_type == VAR_LIST)
    {
	دراز	    n1;
	دراز	    n2;
	listitem_T  *li1;

	n1 = (دراز)tv_get_عدد_chk(tv_idx1, عدم);
	إذا (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_عدد == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	وإلا
	    n2 = (دراز)tv_get_عدد_chk(tv_idx2, عدم);

	li1 = check_range_شاخص_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	إذا (li1 == عدم)
	    وضعیت = FAIL;
	وإلا
	{
	    وضعیت = check_range_شاخص_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    إذا (وضعیت != FAIL)
		وضعیت = list_تخصیص_بده_range(
			tv_dest->vval.v_list,
			tv->vval.v_list,
			n1,
			n2,
			tv_idx2->v_type == VAR_SPECIAL,
			(char_u *)"=",
			(char_u *)"[unknown]");
	}
    }
    وإلا إذا (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	دراز	    bloblen;

	n1 = tv_get_عدد_chk(tv_idx1, عدم);
	إذا (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_عدد == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	وإلا
	    n2 = tv_get_عدد_chk(tv_idx2, عدم);
	bloblen = blob_len(tv_dest->vval.v_blob);

	إذا (check_blob_شاخص(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    وضعیت = FAIL;
	وإلا
	    وضعیت = blob_مجموعه_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    وإلا
    {
	وضعیت = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_پشته.ga_len -= 4;
    clear_tv(tv);

    أرجع وضعیت;
}

/*
 * Unlet item in list or dict variable.
 */
    ساكن صحيح
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = پشته_TV_BOT(-2);
    typval_T	*tv_dest = پشته_TV_BOT(-1);
    صحيح		وضعیت = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    إذا (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	إذا (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_عدد)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    وضعیت = FAIL;
	}
	وإلا
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*کلید;
	    dictitem_T  *di = عدم;

	    إذا (d != عدم && مقدار_check_قفل(
				      d->dv_قفل, عدم, FALSE))
		وضعیت = FAIL;
	    وإلا
	    {
		إذا (tv_idx->v_type == VAR_STRING)
		{
		    کلید = tv_idx->vval.v_string;
		    إذا (کلید == عدم)
			کلید = (char_u *)"";
		}
		وإلا
		{
		    کلید = tv_get_string(tv_idx);
		}
		إذا (d != عدم)
		    di = dict_پیدا_کن(d, کلید, (صحيح)STRLEN(کلید));
		إذا (di == عدم)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_کلید_not_present_درون_dictionary_str), کلید);
		    وضعیت = FAIL;
		}
		وإلا إذا (var_check_fixed(di->di_flags,
						   عدم, FALSE)
			|| var_check_ro(di->di_flags,
						  عدم, FALSE))
		    وضعیت = FAIL;
		وإلا
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    وإلا إذا (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	إذا (check_for_عدد(tv_idx) == FAIL)
	{
	    وضعیت = FAIL;
	}
	وإلا
	{
	    list_T	*l = tv_dest->vval.v_list;
	    دراز	n = (دراز)tv_idx->vval.v_عدد;

	    إذا (l != عدم && مقدار_check_قفل(
				      l->lv_قفل, عدم, FALSE))
		وضعیت = FAIL;
	    وإلا
	    {
		listitem_T	*li = list_پیدا_کن(l, n);

		إذا (li == عدم)
		{
		    semsg(_(e_list_شاخص_بیرون_از_range_nr), n);
		    وضعیت = FAIL;
		}
		وإلا
		    listitem_remove(l, li);
	    }
	}
    }
    وإلا
    {
	وضعیت = FAIL;
	semsg(_(e_cannot_شاخص_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_پشته.ga_len -= 2;

    أرجع وضعیت;
}

/*
 * Unlet a range of items in a list variable.
 */
    ساكن صحيح
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = پشته_TV_BOT(-3);
    typval_T	*tv_idx2 = پشته_TV_BOT(-2);
    typval_T	*tv_dest = پشته_TV_BOT(-1);
    صحيح		وضعیت = OK;

    إذا (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	إذا (check_for_عدد(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_عدد(tv_idx2) == FAIL))
	{
	    وضعیت = FAIL;
	}
	وإلا
	{
	    list_T	*l = tv_dest->vval.v_list;
	    دراز	n1 = (دراز)tv_idx1->vval.v_عدد;
	    دراز	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (دراز)tv_idx2->vval.v_عدد;
	    listitem_T	*li;

	    li = list_پیدا_کن_شاخص(l, &n1);
	    إذا (li == عدم)
	    {
		semsg(_(e_list_شاخص_بیرون_از_range_nr),
						 (دراز)tv_idx1->vval.v_عدد);
		وضعیت = FAIL;
	    }
	    وإلا
	    {
		إذا (n1 < 0)
		    n1 = list_idx_از_قلم(l, li);
		إذا (n2 < 0)
		{
		    listitem_T *li2 = list_پیدا_کن(l, n2);

		    إذا (li2 == عدم)
		    {
			semsg(_(e_list_شاخص_بیرون_از_range_nr), n2);
			وضعیت = FAIL;
		    }
		    وإلا
			n2 = list_idx_از_قلم(l, li2);
		}
		إذا (وضعیت != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_شاخص_بیرون_از_range_nr), n2);
		    وضعیت = FAIL;
		}
		إذا (وضعیت != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    وإلا
    {
	وضعیت = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_شاخص_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_پشته.ga_len -= 3;

    أرجع وضعیت;
}

/*
 * Top of a for loop.
 */
    ساكن صحيح
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    صحيح		jump = FALSE;
    typval_T	*ltv = پشته_TV_BOT(-1);
    typval_T	*idxtv =
		   پشته_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
	أرجع FAIL;
    إذا (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_عدد;
	إذا (list == عدم
		       || idxtv->vval.v_عدد >= list->lv_len)
	{
	    jump = TRUE;
	}
	وإلا إذا (list->lv_اولی == &range_list_قلم)
	{
	    // non-materialized range() list
	    tv = پشته_TV_BOT(0);
	    tv->v_type = VAR_عدد;
	    tv->v_قفل = 0;
	    tv->vval.v_عدد = list_پیدا_کن_nr(
			     list, idxtv->vval.v_عدد, عدم);
	    ++ectx->ec_پشته.ga_len;
	}
	وإلا
	{
	    listitem_T *li = list_پیدا_کن(list,
					 idxtv->vval.v_عدد);
	    إذا (li == عدم)
		أرجع FAIL;
	    copy_tv(&li->li_tv, پشته_TV_BOT(0));
	    ++ectx->ec_پشته.ga_len;
	}
    }
    وإلا إذا (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_عدد;
	إذا (tuple == عدم || idxtv->vval.v_عدد >= TUPLE_LEN(tuple))
	    jump = TRUE;
	وإلا
	{
	    copy_tv(TUPLE_قلم(tuple, idxtv->vval.v_عدد), پشته_TV_BOT(0));
	    ++ectx->ec_پشته.ga_len;
	}
    }
    وإلا إذا (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_عدد;
	إذا (str == عدم || str[idxtv->vval.v_عدد] == NUL)
	{
	    jump = TRUE;
	}
	وإلا
	{
	    صحيح	clen = mb_ptr2len(str + idxtv->vval.v_عدد);

	    // Push the next character from the string.
	    tv = پشته_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_عدد, clen);
	    ++ectx->ec_پشته.ga_len;
	    idxtv->vval.v_عدد += clen - 1;
	}
    }
    وإلا إذا (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	إذا (idxtv->vval.v_عدد == -1 && blob != عدم)
	{
	    ltv->v_قفل = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_عدد;
	إذا (blob == عدم || idxtv->vval.v_عدد >= blob_len(blob))
	{
	    jump = TRUE;
	}
	وإلا
	{
	    // Push the next byte from the blob.
	    tv = پشته_TV_BOT(0);
	    tv->v_type = VAR_عدد;
	    tv->vval.v_عدد = blob_get(blob,
					 idxtv->vval.v_عدد);
	    ++ectx->ec_پشته.ga_len;
	}
    }
    وإلا
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	أرجع FAIL;
    }

    إذا (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_انتها;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    وإلا
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = پشته_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_عدد = ectx->ec_funcrefs.ga_len;
    }

    أرجع OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
ساكن loopvars_T *اولی_loopvars = عدم;

    ساكن فراغ
جمع_کن_loopvars_مقصد_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    إذا (اولی_loopvars != عدم)
	اولی_loopvars->lvs_prev = loopvars;
    loopvars->lvs_بعدی = اولی_loopvars;
    loopvars->lvs_prev = عدم;
    اولی_loopvars = loopvars;
}

    ساكن فراغ
remove_loopvars_from_list(loopvars_T *loopvars)
{
    إذا (loopvars->lvs_prev == عدم)
	اولی_loopvars = loopvars->lvs_بعدی;
    وإلا
	loopvars->lvs_prev->lvs_بعدی = loopvars->lvs_بعدی;
    إذا (loopvars->lvs_بعدی != عدم)
	loopvars->lvs_بعدی->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    ساكن صحيح
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = پشته_TV_VAR(endloop->انتها_funcref_idx);
    صحيح		prev_بستگی_شمار = tv_refcount->vval.v_عدد;
    صحيح		عمق = endloop->انتها_عمق;
    garray_T	*gap = &ectx->ec_funcrefs;
    صحيح		بستگی_درون_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *پشته;
    صحيح		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    لكل (idx = prev_بستگی_شمار; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_داده)[idx];

	إذا (pt->pt_refcount > 1 && pt->pt_loopvars[عمق] == عدم)
	{
	    صحيح refcount = pt->pt_refcount;
	    صحيح i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    لكل (i = 0; i < endloop->انتها_var_شمار; ++i)
	    {
		typval_T *stv = پشته_TV_VAR(endloop->انتها_var_idx + i);

		إذا (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    إذا (refcount > 1)
	    {
		بستگی_درون_use = TRUE;
		اكسر;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    إذا (!بستگی_درون_use)
	أرجع OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    إذا (loopvars == عدم)
	أرجع FAIL;

    loopvars->lvs_ga.ga_len = endloop->انتها_var_شمار;
    پشته = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_داده = پشته;
    إذا (پشته == عدم)
    {
	vim_free(loopvars);
	أرجع FAIL;
    }
    جمع_کن_loopvars_مقصد_list(loopvars);

    // Move the variable values.
    لكل (idx = 0; idx < endloop->انتها_var_شمار; ++idx)
    {
	typval_T *tv = پشته_TV_VAR(endloop->انتها_var_idx + idx);

	*(پشته + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    لكل (idx = prev_بستگی_شمار; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_داده)[idx];

	إذا (pt->pt_refcount > 1 && pt->pt_loopvars[عمق] == عدم)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[عمق] = loopvars;

	    pt->pt_outer.بیرون_loop[عمق].پشته = &loopvars->lvs_ga;
	    pt->pt_outer.بیرون_loop[عمق].var_idx -=
		  ectx->ec_قاب_idx + پشته_قاب_اندازه + endloop->انتها_var_idx;
	}
    }

    أرجع OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    صحيح
loopvars_check_refcount(loopvars_T *loopvars)
{
    صحيح		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    صحيح		    انجامشده = 0;
	typval_T	*پشته = gap->ga_داده;

    إذا (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	أرجع FALSE;
    لكل (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_داده) + i;

	إذا (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != عدم
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    صحيح	    عمق;

	    لكل (عمق = 0; عمق < MAX_LOOP_عمق; ++عمق)
		إذا (tv->vval.v_partial->pt_loopvars[عمق] == loopvars)
		    ++انجامشده;
	}
    }
    إذا (انجامشده != loopvars->lvs_min_refcount)
	أرجع FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    پشته = gap->ga_داده;
    لكل (i = 0; i < gap->ga_len; ++i)
	clear_tv(پشته + i);
    vim_free(پشته);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    أرجع TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    صحيح
مجموعه_ref_درون_loopvars(صحيح copyID)
{
    loopvars_T *loopvars;

    لكل (loopvars = اولی_loopvars; loopvars != عدم;
						 loopvars = loopvars->lvs_بعدی)
    {
	typval_T    *پشته = loopvars->lvs_ga.ga_داده;
	صحيح	    i;

	لكل (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    إذا (مجموعه_ref_درون_قلم(پشته + i, copyID, عدم, عدم, عدم))
		أرجع TRUE;  // abort
    }
    أرجع FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    ساكن صحيح
بارگیری_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = عدم;
    hashtab_T	*ht = عدم;
    حرف	namespace;

    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
	أرجع NOTDONE;
    بدل (isn_type)
    {
	حالة ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    اكسر;
	حالة ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    اكسر;
	حالة ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    اكسر;
	حالة ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    اكسر;
	افتراضي:  // Cannot reach here
	    أرجع NOTDONE;
    }
    di = پیدا_کن_var_درون_ht(ht, 0, iptr->isn_arg.string, TRUE);

    إذا (di == عدم)
    {
	إذا (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = پیدا_کن_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    إذا (ufunc != عدم)
	    {
		typval_T    *tv = پشته_TV_BOT(0);

		++ectx->ec_پشته.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		إذا (tv->vval.v_string == عدم)
		    أرجع FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		أرجع OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	إذا (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != عدم)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_قلم_not_یافت_درون_script_str), iptr->isn_arg.string);
	وإلا
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = پشته_TV_BOT(0);
	tv->v_type = VAR_عدد;
	tv->v_قفل = 0;
	tv->vval.v_عدد = 0;
	++ectx->ec_پشته.ga_len;
	أرجع FAIL;
    }
    وإلا
    {
	copy_tv(&di->di_tv, پشته_TV_BOT(0));
	++ectx->ec_پشته.ga_len;
    }
    أرجع OK;
}


    ساكن فراغ
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, حجم(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    حرف *tofree = عدم;
    حرف *typename = type_name(type, &tofree);
    semsg(_(e_object_required_یافت_str), typename);
    vim_free(tofree);
    clear_type_list(&type_list);
}

/*
 * Accessing the variable or method of an object or a class stored in a
 * variable of type "any".
 * Returns OK if the member variable is present.
 * Returns FAIL if the variable is not found.
 */
    ساكن صحيح
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    صحيح		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = عدم;
    typval_T	mtv;

    إذا (is_object)
    {
	obj = tv->vval.v_object;
	إذا (obj == عدم)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    أرجع FAIL;
	}
	tv_cl = obj->obj_class;
    }
    وإلا
    {
	tv_cl = tv->vval.v_class;
	إذا (tv_cl == عدم)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    أرجع FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    صحيح		namelen = (صحيح)STRLEN(iptr->isn_arg.string);
    صحيح		save_did_emsg = did_emsg;

    إذا (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	أرجع OK;
    }

    إذا (did_emsg != save_did_emsg)
	أرجع FAIL;

    // could be a class or instance method
    ufunc_T	*oc_روش;
    صحيح		oc_روش_idx;

    oc_روش = روش_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_روش_idx);
    إذا (oc_روش == عدم)
    {
	حرف	*پیام;

	SOURCING_LNUM = iptr->isn_lnum;
	إذا (is_object)
	    پیام = e_variable_not_یافت_on_object_str_str;
	وإلا
	    پیام = e_class_variable_str_not_یافت_درون_class_str;
	semsg(_(پیام), iptr->isn_arg.string, tv_cl->class_name.string);
	أرجع FAIL;
    }

    // Protected methods are not accessible outside the class
    إذا (*oc_روش->uf_name == '_'
			&& !class_instance_از(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_روش_str), oc_روش->uf_name);
	أرجع FAIL;
    }

    // Create a partial for the instance or class method
    إذا (obj_روش_مقصد_partial_tv(is_object ? obj : عدم, oc_روش, tv)
								== FAIL)
	أرجع FAIL;

    أرجع OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    ساكن فراغ
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, صحيح fixindent)
{
    صحيح		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*عبارت = عدم;
    صحيح		dir = FORWARD;

    إذا (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = پشته_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_عدد;
	إذا (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_پشته.ga_len;
    }
    وإلا إذا (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    وإلا إذا (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	إذا (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    إذا (regname == '=')
    {
	tv = پشته_TV_BOT(-1);
	إذا (tv->v_type == VAR_STRING)
	    عبارت = tv->vval.v_string;
	وإلا
	{
	    عبارت = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_پشته.ga_len;
    }
    check_cursor();

    إذا (fixindent)
	do_put(regname, عبارت, dir, 1L, PUT_سطر|PUT_CURSLINE|PUT_FIXINDENT);
    وإلا
	do_put(regname, عبارت, dir, 1L, PUT_سطر|PUT_CURSLINE);
    vim_free(عبارت);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    ساكن صحيح
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    صحيح		شمار = iptr->isn_arg.unpack.unp_شمار;
    صحيح		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    صحيح		i;

    l = tv->vval.v_list;
    إذا (l == عدم
	    || l->lv_len < (semicolon ? شمار - 1 : شمار))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_مقدار_does_not_have_enough_اقلام));
	أرجع EXEC_FAIL;
    }
    وإلا إذا (!semicolon && l->lv_len > شمار)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_مقدار_has_بیشتر_اقلام_than_targets));
	أرجع EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    إذا (GA_GROW_FAILS(&ectx->ec_پشته, شمار - 1))
	أرجع EXEC_انجامشده;
    ectx->ec_پشته.ga_len += شمار - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    إذا (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_اقلام(l->lv_len - شمار + 1);

	إذا (rem_list == عدم)
	    أرجع EXEC_انجامشده;
	tv = پشته_TV_BOT(-شمار);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_قفل = 0;
	li = l->lv_اولی;
	لكل (i = 0; i < شمار - 1; ++i)
	    li = li->li_بعدی;
	لكل (i = 0; li != عدم; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_مجموعه_قلم(rem_list, i, &tvcopy);
	    li = li->li_بعدی;
	}
	--شمار;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_اولی;
    لكل (i = 0; i < شمار; ++i)
    {
	tv = پشته_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_بعدی;
    }

    list_unref(l);

    أرجع EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    ساكن صحيح
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    صحيح		شمار = iptr->isn_arg.unpack.unp_شمار;
    صحيح		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    صحيح		i;

    tuple = tv->vval.v_tuple;
    إذا (tuple == عدم
	    || TUPLE_LEN(tuple) < (semicolon ? شمار - 1 : شمار))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_بیشتر_targets_than_tuple_اقلام));
	أرجع EXEC_FAIL;
    }
    وإلا إذا (!semicolon && TUPLE_LEN(tuple) > شمار)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_اقلام));
	أرجع EXEC_FAIL;
    }

    إذا (GA_GROW_FAILS(&ectx->ec_پشته, شمار - 1))
	أرجع EXEC_انجامشده;
    ectx->ec_پشته.ga_len += شمار - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    إذا (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_اقلام(TUPLE_LEN(tuple) - شمار + 1);

	إذا (rem_list == عدم)
	    أرجع EXEC_انجامشده;
	tv = پشته_TV_BOT(-شمار);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_قفل = 0;
	لكل (i = شمار - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_قلم(tuple, i), &tvcopy);
	    list_مجموعه_قلم(rem_list, i - (شمار - 1), &tvcopy);
	}
	--شمار;
    }

    // Produce the values in reverse order, first item last.
    لكل (i = 0; i < شمار; ++i)
    {
	tv = پشته_TV_BOT(-i - 1);
	copy_tv(TUPLE_قلم(tuple, i), tv);
    }

    tuple_unref(tuple);

    أرجع EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    ساكن صحيح
exec_instructions(ectx_T *ectx)
{
    صحيح		ret = FAIL;
    صحيح		save_trylevel_at_شروع = ectx->ec_trylevel_at_شروع;
    صحيح		dict_پشته_len_at_شروع = dict_پشته.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_شروع = trylevel;

    لكل (;;)
    {
	ساكن صحيح  breakcheck_شمار = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	إذا (unlikely(++breakcheck_شمار >= 100))
	{
	    سطر_breakcheck();
	    breakcheck_شمار = 0;
	}
	إذا (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    إذا (throw_exception("Vim:Interrupt", ET_وقفه, عدم) == FAIL)
		اذهب theend;
	    did_throw = TRUE;
	}

	إذا (unlikely(did_emsg && پیام_list != عدم && *پیام_list != عدم))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    إذا (throw_exception(*پیام_list, ET_ERROR, عدم) == FAIL)
		اذهب theend;
	    did_throw = TRUE;
	    *پیام_list = عدم;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    إذا (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_داده)
							+ trystack->ga_len - 1;
		إذا (trycmd->tcd_قاب_idx == ectx->ec_قاب_idx)
		{
		    إذا (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			پایان_exception(caught_پشته);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	إذا (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = عدم;
	    صحيح		شاخص = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    طالما (شاخص > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_داده) + شاخص - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		إذا (!trycmd->tcd_درون_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_قاب_idx == ectx->ec_قاب_idx)
		    اكسر;
		// In the catch and finally block of this try we have to go up
		// one level.
		--شاخص;
		trycmd = عدم;
	    }
	    إذا (trycmd != عدم && trycmd->tcd_قاب_idx == ectx->ec_قاب_idx)
	    {
		إذا (trycmd->tcd_درون_catch)
		{
		    إذا (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    وإلا
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		وإلا
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_درون_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    وإلا
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    اذهب theend;
		tv = پشته_TV_BOT(0);
		tv->v_type = VAR_عدد;
		tv->vval.v_عدد = 0;
		++ectx->ec_پشته.ga_len;
		إذا (ectx->ec_قاب_idx == ectx->ec_initial_قاب_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    إذا (handle_بستگی_درون_use(ectx, FALSE) == FAIL)
			اذهب theend;
		    اذهب انجامشده;
		}

		إذا (func_return(ectx) == FAIL)
		    اذهب theend;
	    }
	    استمر;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	بدل (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    حالة ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = پشته_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		اكسر;

	    // execute Ex command line
	    حالة ISN_EXEC:
		إذا (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    اذهب on_error;
		اكسر;

	    // execute Ex command line split at NL characters.
	    حالة ISN_EXEC_تقسیم:
		{
		    منبع_cookie_T cookie;
		    char_u	    *سطر;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_فیلد(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    سطر = get_تقسیم_sourceline(0, &cookie, 0, 0);
		    إذا (do_cmdline(سطر,
				get_تقسیم_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(سطر);
			اذهب on_error;
		    }
		    vim_free(سطر);
		}
		اكسر;

	    // execute Ex command line that is only a range
	    حالة ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    حرف	*error = عدم;

		    CLEAR_فیلد(ea);
		    ea.cmdidx = CMD_اندازه;
		    ea.addr_type = ADDR_سطرها;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_آدرس(&ea, &error, FALSE);
		    إذا (ea.cmd == عدم)
			اذهب on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    إذا (error != عدم)
		    {
			emsg(error);
			اذهب on_error;
		    }
		}
		اكسر;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    حالة ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    صحيح	    res;
		    صحيح	    save_flags = cmdmod.cmod_flags;

		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    tv = پشته_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, عدم, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    إذا (res == FAIL)
			اذهب on_error;
		    ++ectx->ec_پشته.ga_len;
		}
		اكسر;

	    // push typeval VAR_INSTR with instructions to be executed
	    حالة ISN_INSTR:
		{
		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    tv = پشته_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    إذا (tv->vval.v_instr == عدم)
			اذهب on_error;
		    ++ectx->ec_پشته.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		اكسر;

	    حالة ISN_منبع:
		{
		    صحيح notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (may_بارگیری_script((صحيح)iptr->isn_arg.عدد, &notused)
								       == FAIL)
			اذهب on_error;
		}
		اكسر;

	    // execute :substitute with an expression
	    حالة ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    منبع_cookie_T	cookie;
		    بنية subs_عبارت_S	*save_instr = substitute_instr;
		    بنية subs_عبارت_S	subs_instr;
		    صحيح			res;

		    subs_instr.subs_ectx = ectx;
		    subs_instr.subs_instr = subs->subs_instr;
		    subs_instr.subs_وضعیت = OK;
		    substitute_instr = &subs_instr;

		    SOURCING_LNUM = iptr->isn_lnum;
		    // This is very much like ISN_EXEC
		    CLEAR_فیلد(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    res = do_cmdline(subs->subs_cmd,
				getsourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
		    substitute_instr = save_instr;

		    إذا (res == FAIL || did_emsg
					     || subs_instr.subs_وضعیت == FAIL)
			اذهب on_error;
		}
		اكسر;

	    حالة ISN_پایان:
		اذهب انجامشده;

	    حالة ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		إذا (alloc_redir_lval() == FAIL)
		    اذهب on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		اكسر;

	    حالة ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    {
			vim_free(res);
			اذهب theend;
		    }
		    tv = پشته_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_پشته.ga_len;
		}
		اكسر;

	    حالة ISN_CEXPR_AUCMD:
#اگر_تعریف_شده FEAT_QUICKFIX
		force_abort = TRUE;
		إذا (trigger_cexpr_autocmd(iptr->isn_arg.عدد) == FAIL)
		    اذهب on_error;
		force_abort = FALSE;
#پایان_اگر
		اكسر;

	    حالة ISN_CEXPR_CORE:
#اگر_تعریف_شده FEAT_QUICKFIX
		{
		    exarg_T ea;
		    صحيح	    res;

		    CLEAR_فیلد(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_پشته.ga_len;
		    tv = پشته_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    إذا (res == FAIL)
			اذهب on_error;
		}
#پایان_اگر
		اكسر;

	    // execute Ex command from pieces on the stack
	    حالة ISN_EXECCONCAT:
		{
		    صحيح	    شمار = iptr->isn_arg.عدد;
		    size_t  len = 0;
		    صحيح	    pass;
		    صحيح	    i;
		    char_u  *cmd = عدم;
		    char_u  *str;

		    لكل (pass = 1; pass <= 2; ++pass)
		    {
			لكل (i = 0; i < شمار; ++i)
			{
			    tv = پشته_TV_BOT(i - شمار);
			    str = tv->vval.v_string;
			    إذا (str != عدم && *str != NUL)
			    {
				إذا (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    إذا (pass == 2)
				clear_tv(tv);
			}
			إذا (pass == 1)
			{
			    cmd = alloc(len + 1);
			    إذا (unlikely(cmd == عدم))
				اذهب theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		اكسر;

	    // execute :echo {string} ...
	    حالة ISN_ECHO:
		{
		    صحيح شمار = iptr->isn_arg.echo.echo_شمار;
		    صحيح	atstart = TRUE;
		    صحيح needclr = TRUE;
		    صحيح	idx;

		    لكل (idx = 0; idx < شمار; ++idx)
		    {
			tv = پشته_TV_BOT(idx - شمار);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    إذا (needclr)
			پیام_clr_eos();
		    ectx->ec_پشته.ga_len -= شمار;
		}
		اكسر;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    حالة ISN_EXECUTE:
	    حالة ISN_ECHOMSG:
	    حالة ISN_ECHOWINDOW:
	    حالة ISN_ECHOCONSOLE:
	    حالة ISN_ECHOERR:
		{
		    صحيح		شمار;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    صحيح		len;
		    صحيح		failed = FALSE;
		    صحيح		idx;

		    إذا (iptr->isn_type == ISN_ECHOWINDOW)
			شمار = iptr->isn_arg.echowin.ewin_شمار;
		    وإلا
			شمار = iptr->isn_arg.عدد;
		    ga_init2(&ga, 1, 80);
		    لكل (idx = 0; idx < شمار; ++idx)
		    {
			tv = پشته_TV_BOT(idx - شمار);
			إذا (iptr->isn_type == ISN_EXECUTE)
			{
			    إذا (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_کار)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_مقدار_as_string_str),
						    vartype_name(tv->v_type));
				اكسر;
			    }
			    وإلا
				p = tv_get_string_buf(tv, buf);
			}
			وإلا
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (صحيح)STRLEN(p);
			إذا (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			وإلا
			{
			    إذا (ga.ga_len > 0)
				((char_u *)(ga.ga_داده))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_داده) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_پشته.ga_len -= شمار;
		    إذا (failed)
		    {
			ga_clear(&ga);
			اذهب on_error;
		    }

		    إذا (ga.ga_داده != عدم)
		    {
			إذا (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_داده);
			    إذا (did_emsg)
			    {
				ga_clear(&ga);
				اذهب on_error;
			    }
			}
			وإلا
			{
			    پیام_sb_eol();
			    إذا (iptr->isn_type == ISN_ECHOMSG)
			    {
				پیام_attr(ga.ga_داده, echo_attr);
				بیرون_flush();
			    }
#اگر_تعریف_شده HAS_پیام_پنجره
			    وإلا إذا (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				شروع_echowindow(
					      iptr->isn_arg.echowin.ewin_زمان);
				پیام_attr(ga.ga_داده, echo_attr);
				انتها_echowindow();
			    }
#پایان_اگر
			    وإلا إذا (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_داده, (صحيح)STRLEN(ga.ga_داده),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    وإلا
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_داده);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		اكسر;

	    // load local variable or argument
	    حالة ISN_بارگیری:
		إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    اذهب theend;
		tv = پشته_TV_VAR(iptr->isn_arg.عدد);
		إذا (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    پشته_TV_BOT(0)->v_type = VAR_SPECIAL;
		    پشته_TV_BOT(0)->vval.v_عدد = VVAL_NONE;
		}
		وإلا
		    copy_tv(tv, پشته_TV_BOT(0));
		++ectx->ec_پشته.ga_len;
		اكسر;

	    // load v: variable
	    حالة ISN_LOADV:
		إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    اذهب theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.عدد), پشته_TV_BOT(0));
		++ectx->ec_پشته.ga_len;
		اكسر;

	    // load s: variable in Vim9 script
	    حالة ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    إذا (sv == عدم)
			اذهب theend;
		    allocate_if_null(sv);
		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    copy_tv(sv->sv_tv, پشته_TV_BOT(0));
		    ++ectx->ec_پشته.ga_len;
		}
		اكسر;

	    // load s: variable in old script or autoload import
	    حالة ISN_LOADS:
	    حالة ISN_LOADEXPORT:
		{
		    صحيح		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = پیدا_کن_var_درون_ht(ht, 0, name, TRUE);

		    إذا (di == عدم)
		    {
			ufunc_T	*ufunc = عدم;

			إذا (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (فراغ)پیدا_کن_exported(sid, name, &ufunc, &type,
							 عدم, عدم, FALSE);
			}
			إذا (ufunc == عدم)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    اذهب on_error;
			}
			إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			    اذهب theend;
			tv = پشته_TV_BOT(0);
			tv->v_قفل = 0;
			++ectx->ec_پشته.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    وإلا
		    {
			إذا (iptr->isn_type == ISN_LOADEXPORT)
			{
			    صحيح idx = get_script_قلم_idx(sid, name, 0,
								   عدم, عدم);
			    svar_T	*sv;

			    إذا (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_قلم(sid)
						  ->sn_var_vals.ga_داده) + idx;
				إذا ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_قلم_not_exported_درون_script_str),
									 name);
				    اذهب on_error;
				}
			    }
			}
			إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			    اذهب theend;
			copy_tv(&di->di_tv, پشته_TV_BOT(0));
			++ectx->ec_پشته.ga_len;
		    }
		}
		اكسر;

	    // load g:/b:/w:/t: variable
	    حالة ISN_LOADG:
	    حالة ISN_LOADB:
	    حالة ISN_LOADW:
	    حالة ISN_LOADT:
		{
		    صحيح res = بارگیری_namespace_var(ectx, iptr->isn_type, iptr);

		    إذا (res == NOTDONE)
			اذهب theend;
		    إذا (res == FAIL)
			اذهب on_error;
		}

		اكسر;

	    // load autoload variable
	    حالة ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (eval_variable(name, (صحيح)STRLEN(name), 0,
			      پشته_TV_BOT(0), عدم, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = پشته_TV_BOT(0);
			tv->v_type = VAR_عدد;
			tv->v_قفل = 0;
			tv->vval.v_عدد = 0;
			++ectx->ec_پشته.ga_len;
			اذهب on_error;
		    }
		    ++ectx->ec_پشته.ga_len;
		}
		اكسر;

	    // load g:/b:/w:/t: namespace
	    حالة ISN_LOADGDICT:
	    حالة ISN_LOADBDICT:
	    حالة ISN_LOADWDICT:
	    حالة ISN_LOADTDICT:
		{
		    dict_T *d = عدم;

		    بدل (iptr->isn_type)
		    {
			حالة ISN_LOADGDICT: d = get_globvar_dict(); اكسر;
			حالة ISN_LOADBDICT: d = curbuf->b_vars; اكسر;
			حالة ISN_LOADWDICT: d = curwin->w_vars; اكسر;
			حالة ISN_LOADTDICT: d = curtab->tp_vars; اكسر;
			افتراضي:  // Cannot reach here
			    اذهب theend;
		    }
		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    tv = پشته_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_قفل = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_پشته.ga_len;
		}
		اكسر;

	    // load &option
	    حالة ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    إذا (eval_option(&name, &optval, TRUE) == FAIL)
			اذهب theend;
		    *پشته_TV_BOT(0) = optval;
		    ++ectx->ec_پشته.ga_len;
		}
		اكسر;

	    // load $ENV
	    حالة ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    // name is always valid, checked when compiling
		    (فراغ)eval_env_var(&name, &optval, TRUE);
		    *پشته_TV_BOT(0) = optval;
		    ++ectx->ec_پشته.ga_len;
		}
		اكسر;

	    // load @register
	    حالة ISN_LOADREG:
		إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    اذهب theend;
		tv = پشته_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_قفل = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.عدد, GREG_عبارت_SRC);
		++ectx->ec_پشته.ga_len;
		اكسر;

	    // store local variable
	    حالة ISN_STORE:
		--ectx->ec_پشته.ga_len;
		tv = پشته_TV_VAR(iptr->isn_arg.عدد);
		إذا (check_typval_is_مقدار(پشته_TV_BOT(0)) == FAIL
			|| پشته_TV_BOT(0)->v_type == VAR_VOID)
		{
		    إذا (پشته_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_مقدار));
		    clear_tv(پشته_TV_BOT(0));
		    اذهب on_error;
		}
		clear_tv(tv);
		*tv = *پشته_TV_BOT(0);
		اكسر;

	    // store s: variable in old script or autoload import
	    حالة ISN_STORES:
	    حالة ISN_STOREEXPORT:
		{
		    صحيح		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = عدم;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    إذا (iptr->isn_type == ISN_STOREEXPORT)
			di = پیدا_کن_var_autoload_پیشوند(name, sid, عدم, عدم);
		    // Then look for a variable in the script's variables.
		    إذا (di == عدم)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = پیدا_کن_var_درون_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_پشته.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (di == عدم)
		    {
			إذا (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(پشته_TV_BOT(0));
			    اذهب on_error;
			}
			store_var(name, پشته_TV_BOT(0));
		    }
		    وإلا
		    {
			إذا (iptr->isn_type == ISN_STOREEXPORT)
			{
			    صحيح idx = get_script_قلم_idx(sid, name, 0,
								   عدم, عدم);

			    // can this ever fail?
			    إذا (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_قلم(sid)
						  ->sn_var_vals.ga_داده) + idx;

				إذا ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_قلم_not_exported_درون_script_str),
									 name);
				    clear_tv(پشته_TV_BOT(0));
				    اذهب on_error;
				}
			    }
			}
			إذا (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(پشته_TV_BOT(0));
			    اذهب on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *پشته_TV_BOT(0);
		    }
		}
		اكسر;

	    // store script-local variable in Vim9 script
	    حالة ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    إذا (sv == عدم)
			اذهب theend;
		    --ectx->ec_پشته.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (مقدار_check_قفل(sv->sv_tv->v_قفل, sv->sv_name, FALSE))
		    {
			clear_tv(پشته_TV_BOT(0));
			اذهب on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *پشته_TV_BOT(0);
		}
		اكسر;

	    // store option
	    حالة ISN_STOREOPT:
	    حالة ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    صحيح		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    دراز	n = 0;
		    char_u	*s = عدم;
		    حرف	*پیام;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = عدم;

		    --ectx->ec_پشته.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = پشته_TV_BOT(0);
		    إذا (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			إذا (s == عدم)
			    s = (char_u *)"";
		    }
		    وإلا إذا (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			إذا (s == عدم || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    اذهب on_error;
			}
		    }
		    وإلا
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_عدد;
		    پیام = مجموعه_option_مقدار(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    إذا (پیام != عدم)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(پیام));
			اذهب on_error;
		    }
		}
		اكسر;

	    // store $ENV
	    حالة ISN_STOREENV:
		إذا (check_secure() || check_restricted())
		    اذهب theend;
		--ectx->ec_پشته.ga_len;
		tv = پشته_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		اكسر;

	    // store @r
	    حالة ISN_STOREREG:
		{
		    صحيح	reg = iptr->isn_arg.عدد;

		    --ectx->ec_پشته.ga_len;
		    tv = پشته_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		اكسر;

	    // store v: variable
	    حالة ISN_STOREV:
		--ectx->ec_پشته.ga_len;
		إذا (مجموعه_vim_var_tv(iptr->isn_arg.عدد, پشته_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    اذهب on_error;
		اكسر;

	    // store g:/b:/w:/t: variable
	    حالة ISN_STOREG:
	    حالة ISN_STOREB:
	    حالة ISN_STOREW:
	    حالة ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    بدل (iptr->isn_type)
		    {
			حالة ISN_STOREG:
			    ht = get_globvar_ht();
			    اكسر;
			حالة ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    اكسر;
			حالة ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    اكسر;
			حالة ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    اكسر;
			افتراضي:  // Cannot reach here
			    اذهب theend;
		    }

		    --ectx->ec_پشته.ga_len;
		    di = پیدا_کن_var_درون_ht(ht, 0, name, TRUE);
		    إذا (di == عدم)
			store_var(iptr->isn_arg.string, پشته_TV_BOT(0));
		    وإلا
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			إذا (var_check_permission(di, name) == FAIL)
			    اذهب on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *پشته_TV_BOT(0);
		    }
		}
		اكسر;

	    // store an autoload variable
	    حالة ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		مجموعه_var(iptr->isn_arg.string, پشته_TV_BOT(-1), TRUE);
		clear_tv(پشته_TV_BOT(-1));
		--ectx->ec_پشته.ga_len;
		اكسر;

	    // store number in local variable
	    حالة ISN_STORENR:
		tv = پشته_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_عدد;
		tv->vval.v_عدد = iptr->isn_arg.storenr.stnr_val;
		اكسر;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    حالة ISN_STOREINDEX:
		{
		    صحيح res = execute_storeindex(iptr, ectx);

		    إذا (res == FAIL)
			اذهب on_error;
		    إذا (res == NOTDONE)
			اذهب theend;
		}
		اكسر;

	    // store value in list or blob range
	    حالة ISN_STORERANGE:
		إذا (execute_storerange(iptr, ectx) == FAIL)
		    اذهب on_error;
		اكسر;

	    حالة ISN_بارگیری_CLASSMEMBER:
		{
		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      پشته_TV_BOT(0));
		    ++ectx->ec_پشته.ga_len;
		}
		اكسر;

	    حالة ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *پشته_TV_BOT(-1);
		    --ectx->ec_پشته.ga_len;
		}
		اكسر;

	    // Load or store variable or argument from outer scope.
	    حالة ISN_LOADOUTER:
	    حالة ISN_STOREOUTER:
		{
		    صحيح		عمق = iptr->isn_arg.outer.outer_عمق;
		    outer_T	*outer = ectx->ec_outer_ref == عدم ? عدم
						: ectx->ec_outer_ref->or_outer;

		    طالما (عمق > 1 && outer != عدم)
		    {
			outer = outer->بیرون_به_بالا;
			--عمق;
		    }
		    إذا (outer == عدم)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			إذا (ectx->ec_قاب_idx == ectx->ec_initial_قاب_idx
						 || ectx->ec_outer_ref == عدم)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_بستگی_called_from_invalid_context));
			وإلا
			    iemsg("LOADOUTER depth more than scope levels");
			اذهب theend;
		    }
		    إذا (عمق < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->بیرون_loop[-عمق - 1]
							       .پشته->ga_داده)
					  + outer->بیرون_loop[-عمق - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    وإلا
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->بیرون_پشته->ga_داده)
				      + outer->بیرون_قاب_idx + پشته_قاب_اندازه
				      + iptr->isn_arg.outer.outer_idx;
		    إذا (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			    اذهب theend;
			// careful: ga_grow_inner may re-alloc the stack
			إذا (عمق < 0)
			    copy = ((typval_T *)outer->بیرون_loop[-عمق - 1]
								   .پشته->ga_داده)
					      + outer->بیرون_loop[-عمق - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			وإلا
			    copy = ((typval_T *)outer->بیرون_پشته->ga_داده)
					  + outer->بیرون_قاب_idx + پشته_قاب_اندازه
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			إذا (copy != tv)
			    tv = copy;
			copy_tv(tv, پشته_TV_BOT(0));
			++ectx->ec_پشته.ga_len;
		    }
		    وإلا
		    {
			--ectx->ec_پشته.ga_len;
			clear_tv(tv);
			*tv = *پشته_TV_BOT(0);
		    }
		}
		اكسر;

	    // unlet item in list or dict variable
	    حالة ISN_UNLETINDEX:
		إذا (execute_unletindex(iptr, ectx) == FAIL)
		    اذهب on_error;
		اكسر;

	    // unlet range of items in list variable
	    حالة ISN_UNLETRANGE:
		إذا (execute_unletrange(iptr, ectx) == FAIL)
		    اذهب on_error;
		اكسر;

	    // push constant
	    حالة ISN_PUSHNR:
	    حالة ISN_PUSHBOOL:
	    حالة ISN_PUSHSPEC:
	    حالة ISN_PUSHF:
	    حالة ISN_PUSHS:
	    حالة ISN_PUSHBLOB:
	    حالة ISN_PUSHFUNC:
	    حالة ISN_PUSHCHANNEL:
	    حالة ISN_PUSHJOB:
	    حالة ISN_PUSHOBJ:
	    حالة ISN_PUSHCLASS:
		إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    اذهب theend;
		tv = پشته_TV_BOT(0);
		tv->v_قفل = 0;
		++ectx->ec_پشته.ga_len;
		بدل (iptr->isn_type)
		{
		    حالة ISN_PUSHNR:
			tv->v_type = VAR_عدد;
			tv->vval.v_عدد = iptr->isn_arg.عدد;
			اكسر;
		    حالة ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_عدد = iptr->isn_arg.عدد;
			اكسر;
		    حالة ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_عدد = iptr->isn_arg.عدد;
			اكسر;
		    حالة ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			اكسر;
		    حالة ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			اكسر;
		    حالة ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			إذا (iptr->isn_arg.string == عدم)
			    tv->vval.v_string = عدم;
			وإلا
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			اكسر;
		    حالة ISN_PUSHCHANNEL:
#اگر_تعریف_شده FEAT_کار_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = عدم;
#پایان_اگر
			اكسر;
		    حالة ISN_PUSHJOB:
#اگر_تعریف_شده FEAT_کار_CHANNEL
			tv->v_type = VAR_کار;
			tv->vval.v_کار = عدم;
#پایان_اگر
			اكسر;
		    حالة ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = عدم;
			اكسر;
		    حالة ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			اكسر;
		    افتراضي:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == عدم
				    ? عدم : vim_strsave(iptr->isn_arg.string);
		}
		اكسر;

	    حالة ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (فراغ)script_autoload(name, FALSE);
		    إذا (پیدا_کن_func(name, TRUE))
		    {
			إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			    اذهب theend;
			tv = پشته_TV_BOT(0);
			tv->v_قفل = 0;
			++ectx->ec_پشته.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    وإلا
		    {
			صحيح res = بارگیری_namespace_var(ectx, ISN_LOADG, iptr);

			إذا (res == NOTDONE)
			    اذهب theend;
			إذا (res == FAIL)
			    اذهب on_error;
		    }
		}
		اكسر;

	    حالة ISN_UNLET:
		إذا (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    اذهب on_error;
		اكسر;
	    حالة ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		اكسر;

	    حالة ISN_LOCKUNLOCK:
		{
#اگر_تعریف_شده LOG_LOCKVAR
		    ch_log(عدم, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#پایان_اگر
		    lval_ریشه_T	*lval_ریشه_save = lval_ریشه;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_پشته.ga_len;
		    lval_ریشه_T ریشه;
		    ریشه.lr_tv      = پشته_TV_BOT(0);
		    ریشه.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    ریشه.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_ریشه = &ریشه;
		    صحيح res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(ریشه.lr_tv);
		    lval_ریشه = lval_ریشه_save;
		    إذا (res == FAIL)
			اذهب on_error;
		}
		اكسر;

	    حالة ISN_LOCKCONST:
		قلم_قفل(پشته_TV_BOT(-1), 100, TRUE, TRUE);
		اكسر;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    حالة ISN_NEWLIST:
		إذا (exe_newlist(iptr->isn_arg.عدد, ectx) == FAIL)
		    اذهب theend;
		اكسر;

	    // create a tuple from items on the stack
	    حالة ISN_NEWTUPLE:
		إذا (exe_newtuple(iptr->isn_arg.عدد, ectx) == FAIL)
		    اذهب theend;
		اكسر;

	    // create a dict from items on the stack
	    حالة ISN_NEWDICT:
		{
		    صحيح res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.عدد, ectx);
		    إذا (res == FAIL)
			اذهب theend;
		    إذا (res == MAYBE)
			اذهب on_error;
		}
		اكسر;

	    حالة ISN_CONCAT:
		إذا (exe_concat(iptr->isn_arg.عدد, ectx) == FAIL)
		    اذهب theend;
		اكسر;

	    // create a partial with NULL value
	    حالة ISN_NEWPARTIAL:
		إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    اذهب theend;
		++ectx->ec_پشته.ga_len;
		tv = پشته_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_قفل = 0;
		tv->vval.v_partial = عدم;
		اكسر;

	    // call a :def function
	    حالة ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		إذا (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				عدم,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    اذهب on_error;
		اكسر;

	    // call a method on an interface
	    حالة ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = پشته_TV_BOT(-1 - mfunc->cmf_argcount);
		    إذا (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			اذهب on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    إذا (obj == عدم)
		    {
			emsg(_(e_using_null_object));
			اذهب on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    إذا (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    وإلا
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			صحيح idx = object_شاخص_from_itf_شاخص(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    إذا (mfunc->cmf_ufunc != عدم && IS_GENERIC_FUNC(ufunc))
		    {
			إذا (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			وإلا
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    إذا (ufunc == عدم)
				اذهب on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    إذا (call_ufunc(ufunc, عدم, mfunc->cmf_argcount, ectx,
							   عدم, عدم) == FAIL)
			اذهب on_error;
		}
		اكسر;

	    // call a builtin function
	    حالة ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		إذا (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    اذهب on_error;
		اكسر;

	    // call a funcref or partial
	    حالة ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    صحيح		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (pfunc->cpf_بالاترین)
		    {
			// funcref is above the arguments
			tv = پشته_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    وإلا
		    {
			// Get the funcref from the stack.
			--ectx->ec_پشته.ga_len;
			partial_tv = *پشته_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    إذا (tv == &partial_tv)
			clear_tv(&partial_tv);
		    إذا (r == FAIL)
			اذهب on_error;
		}
		اكسر;

	    حالة ISN_PCALL_انتها:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_پشته.ga_len;
		clear_tv(پشته_TV_BOT(-1));
		*پشته_TV_BOT(-1) = *پشته_TV_BOT(0);
		اكسر;

	    // call a user defined function or funcref/partial
	    حالة ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			اذهب on_error;
		}
		اكسر;

	    // :defer func(arg)
	    حالة ISN_DEFER:
		إذا (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    اذهب on_error;
		اكسر;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    حالة ISN_RETURN_VOID:
	    حالة ISN_RETURN_OBJECT:
		إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    اذهب theend;
		tv = پشته_TV_BOT(0);
		++ectx->ec_پشته.ga_len;
		إذا (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_عدد = 0;
		    tv->v_قفل = 0;
		}
		وإلا
		{
		    *tv = *پشته_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_قفل_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    حالة ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = عدم;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    إذا (check_typval_is_مقدار(پشته_TV_BOT(-1)) == FAIL)
			اذهب theend;

		    إذا (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_داده)
							+ trystack->ga_len - 1;
		    إذا (trycmd != عدم
				 && trycmd->tcd_قاب_idx == ectx->ec_قاب_idx)
		    {
			// jump to ":finally" or ":endtry"
			إذا (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			وإلا
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    وإلا
			اذهب func_return;
		}
		اكسر;

	    // push a partial, a reference to a compiled function
	    حالة ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    إذا (pt == عدم)
			اذهب theend;
		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    {
			vim_free(pt);
			اذهب theend;
		    }
		    إذا (extra != عدم && extra->fre_class != عدم)
		    {
			class_T	*cl;
			إذا (extra->fre_object_روش)
			{
			    tv = پشته_TV_BOT(-1);
			    إذا (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				اذهب on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    إذا (obj == عدم)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				اذهب on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_پشته.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			وإلا
			    cl = extra->fre_class;

			إذا (extra->fre_object_روش)
			{
			    // object method
			    // convert the interface index to the object index
			    صحيح idx =
				object_شاخص_from_itf_شاخص(extra->fre_class,
					TRUE, extra->fre_روش_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			وإلا
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_روش_idx];
			}
		    }
		    وإلا إذا (extra == عدم || extra->fre_func_name == عدم)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_داده)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    وإلا
		    {
			ufunc = پیدا_کن_func(extra->fre_func_name, FALSE);
		    }
		    إذا (ufunc == عدم)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			اذهب theend;
		    }
		    إذا (extra != عدم && extra->fre_outer_عمق > 0
					   && (ufunc->uf_flags & FC_بستگی))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == عدم
				    ? عدم : ectx->ec_outer_ref->or_partial;

			لكل (صحيح عمق = 1; outer_pt != عدم
				    && عمق < extra->fre_outer_عمق; ++عمق)
			    outer_pt = get_pt_outer(outer_pt)->بیرون_به_بالا_partial;
			إذا (outer_pt == عدم)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    اذهب theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    وإلا إذا (fill_partial_and_بستگی(pt, ufunc,
			       extra == عدم ? عدم : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			اذهب theend;
		    }
		    tv = پشته_TV_BOT(0);
		    ++ectx->ec_پشته.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_قفل = 0;
		}
		اكسر;

	    // Create a global function from a lambda.
	    حالة ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    إذا (copy_lambda_مقصد_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			اذهب theend;
		}
		اكسر;

	    // List functions
	    حالة ISN_DEF:
		إذا (iptr->isn_arg.string == عدم)
		    list_functions(عدم);
		وإلا
		{
		    exarg_T	ea;
		    garray_T	سطرها_مقصد_free;

		    CLEAR_فیلد(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&سطرها_مقصد_free, حجم(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, عدم, &سطرها_مقصد_free, 0, عدم, 0, عدم);
		    ga_clear_رشتهها(&سطرها_مقصد_free);
		}
		اكسر;

	    // jump if a condition is met
	    حالة ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    صحيح		error = FALSE;
		    صحيح		jump = TRUE;

		    إذا (when != JUMP_ALWAYS)
		    {
			tv = پشته_TV_BOT(-1);
			إذا (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    إذا (error)
				اذهب on_error;
			}
			وإلا
			    jump = tv2bool(tv);
			إذا (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			إذا (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_پشته.ga_len;
			}
		    }
		    إذا (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		اكسر;

	    // "while": jump to end if a condition is false
	    حالة ISN_WHILE:
		{
		    صحيح		error = FALSE;
		    صحيح		jump = TRUE;

		    tv = پشته_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    إذا (error)
			اذهب on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_پشته.ga_len;
		    إذا (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_انتها;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = پشته_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_عدد = ectx->ec_funcrefs.ga_len;
		}
		اكسر;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    حالة ISN_JUMP_IF_ARG_مجموعه:
	    حالة ISN_JUMP_IF_ARG_NOT_مجموعه:
		tv = پشته_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		صحيح arg_مجموعه = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_عدد == VVAL_NONE);

		إذا (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_مجموعه && !arg_مجموعه)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_داده)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_آرگومانها.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ پشته_قاب_اندازه;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_اشاره_گر(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    إذا (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		إذا (iptr->isn_type == ISN_JUMP_IF_ARG_مجموعه ? arg_مجموعه : !arg_مجموعه)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		اكسر;

	    // top of a for loop
	    حالة ISN_FOR:
		إذا (execute_for(iptr, ectx) == FAIL)
		    اذهب theend;
		اكسر;

	    // end of a for or while loop
	    حالة ISN_ENDLOOP:
		إذا (execute_endloop(iptr, ectx) == FAIL)
		    اذهب theend;
		اكسر;

	    // start of ":try" block
	    حالة ISN_TRY:
		{
		    trycmd_T    *trycmd = عدم;

		    إذا (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			اذهب theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_داده)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_اشاره_گر(trycmd);
		    trycmd->tcd_قاب_idx = ectx->ec_قاب_idx;
		    trycmd->tcd_پشته_len = ectx->ec_پشته.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		اكسر;

	    حالة ISN_PUSHEXC:
		إذا (current_exception == عدم)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    اذهب theend;
		}
		إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
		    اذهب theend;
		tv = پشته_TV_BOT(0);
		++ectx->ec_پشته.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_قفل = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->مقدار);
		اكسر;

	    حالة ISN_CATCH:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    may_restore_cmdmod(&ectx->ec_funclocal);
		    trycmd = ((trycmd_T *)trystack->ga_داده)
							+ trystack->ga_len - 1;
		    trycmd->tcd_caught = TRUE;
		    trycmd->tcd_did_throw = FALSE;

		    did_emsg = got_int = did_throw = FALSE;
		    force_abort = need_rethrow = FALSE;
		    catch_exception(current_exception);
		}
		اكسر;

	    حالة ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    صحيح		i;
		    trycmd_T    *trycmd;
		    صحيح		iidx = trycont->tct_where;

		    إذا (trystack->ga_len < trycont->tct_سطوح)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_سطوح, trystack->ga_len);
			اذهب theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    لكل (i = trycont->tct_سطوح; i > 0; --i)
		    {
			trycmd = ((trycmd_T *)trystack->ga_داده)
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
		اكسر;

	    حالة ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_داده)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    إذا (trycmd->tcd_caught)
		    {
			// discard the exception
			پایان_exception(caught_پشته);
			trycmd->tcd_caught = FALSE;
		    }
		    اكسر;
		}

	    // end of ":try" block
	    حالة ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_داده) + trystack->ga_len;
		    إذا (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    إذا (trycmd->tcd_caught)
		    {
			// discard the exception
			پایان_exception(caught_پشته);
		    }

		    إذا (trycmd->tcd_return)
			اذهب func_return;

		    طالما (ectx->ec_پشته.ga_len > trycmd->tcd_پشته_len)
		    {
			--ectx->ec_پشته.ga_len;
			clear_tv(پشته_TV_BOT(0));
		    }
		    إذا (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		اكسر;

	    حالة ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    إذا (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = پشته_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_عدد;
			tv->vval.v_عدد = 0;
			اذهب انجامشده;
		    }
		    --ectx->ec_پشته.ga_len;
		    tv = پشته_TV_BOT(0);
		    إذا (tv->vval.v_string == عدم
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_خالی_string));
			اذهب theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    إذا (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_داده)
							+ trystack->ga_len - 1;
			إذا (trycmd->tcd_caught)
			{
			    // discard the exception
			    پایان_exception(caught_پشته);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (throw_exception(tv->vval.v_string, ET_USER, عدم)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			اذهب theend;
		    }
		    did_throw = TRUE;
		}
		اكسر;

	    // compare with special values
	    حالة ISN_COMPAREBOOL:
	    حالة ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_عدد;
		    varnumber_T arg2 = tv2->vval.v_عدد;
		    صحيح		res;

		    إذا (iptr->isn_arg.op.op_type == عبارت_EQUAL)
			res = arg1 == arg2;
		    وإلا
			res = arg1 != arg2;

		    --ectx->ec_پشته.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_عدد = res ? VVAL_TRUE : VVAL_FALSE;
		}
		اكسر;

	    حالة ISN_COMPARENULL:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    صحيح		res;

		    res = typval_compare_null(tv1, tv2);
		    إذا (res == MAYBE)
			اذهب on_error;
		    إذا (iptr->isn_arg.op.op_type == عبارت_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_پشته.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_عدد = res ? VVAL_TRUE : VVAL_FALSE;
		}
		اكسر;

	    // Operation with two number arguments
	    حالة ISN_OPNR:
	    حالة ISN_COMPARENR:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_عدد;
		    varnumber_T arg2 = tv2->vval.v_عدد;
		    varnumber_T res = 0;
		    صحيح		div_zero = FALSE;

		    إذا (iptr->isn_arg.op.op_type == عبارت_LSHIFT
			    || iptr->isn_arg.op.op_type == عبارت_RSHIFT)
		    {
			إذا (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_مثبت));
			    اذهب on_error;
			}
		    }

		    بدل (iptr->isn_arg.op.op_type)
		    {
			حالة عبارت_MULT: res = arg1 * arg2; اكسر;
			حالة عبارت_DIV:  إذا (arg2 == 0)
					    div_zero = TRUE;
					وإلا
					    res = arg1 / arg2;
					اكسر;
			حالة عبارت_REM:  إذا (arg2 == 0)
					    div_zero = TRUE;
					وإلا
					    res = arg1 % arg2;
					اكسر;
			حالة عبارت_SUB: res = arg1 - arg2; اكسر;
			حالة عبارت_جمع_کن: res = arg1 + arg2; اكسر;

			حالة عبارت_EQUAL: res = arg1 == arg2; اكسر;
			حالة عبارت_NEQUAL: res = arg1 != arg2; اكسر;
			حالة عبارت_GREATER: res = arg1 > arg2; اكسر;
			حالة عبارت_GEQUAL: res = arg1 >= arg2; اكسر;
			حالة عبارت_SMALLER: res = arg1 < arg2; اكسر;
			حالة عبارت_SEQUAL: res = arg1 <= arg2; اكسر;
			حالة عبارت_LSHIFT: إذا (arg2 > MAX_LSHIFT_بیتها)
					      res = 0;
					  وإلا
					      res = (uvarnumber_T)arg1 << arg2;
					  اكسر;
			حالة عبارت_RSHIFT: إذا (arg2 > MAX_LSHIFT_بیتها)
					      res = 0;
					  وإلا
					      res = (uvarnumber_T)arg1 >> arg2;
					  اكسر;
			افتراضي: اكسر;
		    }

		    --ectx->ec_پشته.ga_len;
		    إذا (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_عدد = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    وإلا
			tv1->vval.v_عدد = res;
		    إذا (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			اذهب on_error;
		    }
		}
		اكسر;

	    // Computation with two float arguments
	    حالة ISN_OPFLOAT:
	    حالة ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    صحيح		cmp = FALSE;

		    بدل (iptr->isn_arg.op.op_type)
		    {
			حالة عبارت_MULT: res = arg1 * arg2; اكسر;
			حالة عبارت_DIV: res = arg1 / arg2; اكسر;
			حالة عبارت_SUB: res = arg1 - arg2; اكسر;
			حالة عبارت_جمع_کن: res = arg1 + arg2; اكسر;

			حالة عبارت_EQUAL: cmp = arg1 == arg2; اكسر;
			حالة عبارت_NEQUAL: cmp = arg1 != arg2; اكسر;
			حالة عبارت_GREATER: cmp = arg1 > arg2; اكسر;
			حالة عبارت_GEQUAL: cmp = arg1 >= arg2; اكسر;
			حالة عبارت_SMALLER: cmp = arg1 < arg2; اكسر;
			حالة عبارت_SEQUAL: cmp = arg1 <= arg2; اكسر;
			افتراضي: cmp = 0; اكسر;
		    }
		    --ectx->ec_پشته.ga_len;
		    إذا (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_عدد = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    وإلا
			tv1->vval.v_float = res;
		}
		اكسر;

	    حالة ISN_COMPARELIST:
	    حالة ISN_COMPARETUPLE:
	    حالة ISN_COMPAREDICT:
	    حالة ISN_COMPAREFUNC:
	    حالة ISN_COMPARESTRING:
	    حالة ISN_COMPAREBLOB:
	    حالة ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    صحيح		ic = iptr->isn_arg.op.op_ic;
		    صحيح		res = FALSE;
		    صحيح		وضعیت = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (iptr->isn_type == ISN_COMPARELIST)
		    {
			وضعیت = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وإلا إذا (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			وضعیت = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وإلا إذا (iptr->isn_type == ISN_COMPAREDICT)
		    {
			وضعیت = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وإلا إذا (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			وضعیت = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وإلا إذا (iptr->isn_type == ISN_COMPARESTRING)
		    {
			وضعیت = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    وإلا إذا (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			وضعیت = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    وإلا // ISN_COMPAREOBJECT
		    {
			وضعیت = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_پشته.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_عدد = res ? VVAL_TRUE : VVAL_FALSE;
		    إذا (وضعیت == FAIL)
			اذهب theend;
		}
		اكسر;

	    حالة ISN_COMPAREANY:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    صحيح		ic = iptr->isn_arg.op.op_ic;
		    صحيح		وضعیت;

		    SOURCING_LNUM = iptr->isn_lnum;
		    وضعیت = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_پشته.ga_len;
		    إذا (وضعیت == FAIL)
			اذهب theend;
		}
		اكسر;

	    حالة ISN_ADDLIST:
	    حالة ISN_ADDTUPLE:
	    حالة ISN_ADDBLOB:
		{
		    typval_T *tv1 = پشته_TV_BOT(-2);
		    typval_T *tv2 = پشته_TV_BOT(-1);

		    // add two lists or blobs
		    إذا (iptr->isn_type == ISN_ADDLIST)
		    {
			إذا (iptr->isn_arg.op.op_type == عبارت_APPEND
						   && tv1->vval.v_list != عدم)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 عدم);
			وإلا
			    eval_addlist(tv1, tv2);
		    }
		    وإلا إذا (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    وإلا
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_پشته.ga_len;
		}
		اكسر;

	    حالة ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (l == عدم)
		    {
			emsg(_(e_cannot_جمع_کن_مقصد_null_list));
			اذهب on_error;
		    }
		    إذا (مقدار_check_قفل(l->lv_قفل, عدم, FALSE))
			اذهب on_error;
		    إذا (list_append_tv(l, tv2) == FAIL)
			اذهب theend;
		    clear_tv(tv2);
		    --ectx->ec_پشته.ga_len;
		}
		اكسر;

	    حالة ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    صحيح		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    إذا (b == عدم)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_جمع_کن_مقصد_null_blob));
			اذهب on_error;
		    }
		    n = tv_get_عدد_chk(tv2, &error);
		    إذا (error)
			اذهب on_error;
		    ga_append(&b->bv_ga, (صحيح)n);
		    --ectx->ec_پشته.ga_len;
		}
		اكسر;

	    // Computation with two arguments of unknown type
	    حالة ISN_OPANY:
		{
		    typval_T	*tv1 = پشته_TV_BOT(-2);
		    typval_T	*tv2 = پشته_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    صحيح		error = FALSE;

		    إذا (iptr->isn_arg.op.op_type == عبارت_جمع_کن)
		    {
			إذا (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_پشته.ga_len;
			    اكسر;
			}
			وإلا إذا (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_پشته.ga_len;
			    اكسر;
			}
			وإلا إذا (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_پشته.ga_len;
			    اكسر;
			}
		    }
		    إذا (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    وإلا
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_عدد_chk(tv1, &error);
			إذا (error)
			    اذهب on_error;
			إذا (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    إذا (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    وإلا
		    {
			n2 = tv_get_عدد_chk(tv2, &error);
			إذا (error)
			    اذهب on_error;
			إذا (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    إذا (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			بدل (iptr->isn_arg.op.op_type)
			{
			    حالة عبارت_MULT: f1 = f1 * f2; اكسر;
			    حالة عبارت_DIV:  f1 = f1 / f2; اكسر;
			    حالة عبارت_SUB:  f1 = f1 - f2; اكسر;
			    حالة عبارت_جمع_کن:  f1 = f1 + f2; اكسر;
			    افتراضي: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     اذهب on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_پشته.ga_len;
		    }
		    وإلا
		    {
			صحيح failed = FALSE;

			بدل (iptr->isn_arg.op.op_type)
			{
			    حالة عبارت_MULT: n1 = n1 * n2; اكسر;
			    حالة عبارت_DIV:  n1 = رقم_divide(n1, n2, &failed);
					    إذا (failed)
						اذهب on_error;
					    اكسر;
			    حالة عبارت_SUB:  n1 = n1 - n2; اكسر;
			    حالة عبارت_جمع_کن:  n1 = n1 + n2; اكسر;
			    افتراضي:	    n1 = رقم_modulus(n1, n2, &failed);
					    إذا (failed)
						اذهب on_error;
					    اكسر;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_عدد;
			tv1->vval.v_عدد = n1;
			--ectx->ec_پشته.ga_len;
		    }
		}
		اكسر;

	    حالة ISN_STRINDEX:
	    حالة ISN_STRSLICE:
		{
		    صحيح		is_برش = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    إذا (is_برش)
		    {
			tv = پشته_TV_BOT(-2);
			n1 = tv->vval.v_عدد;
		    }

		    tv = پشته_TV_BOT(-1);
		    n2 = tv->vval.v_عدد;

		    ectx->ec_پشته.ga_len -= is_برش ? 2 : 1;
		    tv = پشته_TV_BOT(-1);
		    إذا (is_برش)
			// Slice: Select the characters from the string
			res = string_برش(tv->vval.v_string, n1, n2, FALSE);
		    وإلا
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		اكسر;

	    حالة ISN_LISTINDEX:
	    حالة ISN_LISTSLICE:
	    حالة ISN_TUPLEINDEX:
	    حالة ISN_TUPLESLICE:
	    حالة ISN_BLOBINDEX:
	    حالة ISN_BLOBSLICE:
		{
		    صحيح		is_برش = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    صحيح		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    صحيح		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_برش ? پشته_TV_BOT(-3) : پشته_TV_BOT(-2);

		    tv = پشته_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_عدد;
		    clear_tv(tv);

		    إذا (is_برش)
		    {
			tv = پشته_TV_BOT(-2);
			n1 = tv->vval.v_عدد;
			clear_tv(tv);
		    }

		    ectx->ec_پشته.ga_len -= is_برش ? 2 : 1;
		    tv = پشته_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (is_blob)
		    {
			إذا (blob_برش_or_شاخص(val_tv->vval.v_blob, is_برش,
						    n1, n2, FALSE, tv) == FAIL)
			    اذهب on_error;
		    }
		    وإلا إذا (is_tuple)
		    {
			إذا (tuple_برش_or_شاخص(val_tv->vval.v_tuple,
				is_برش, n1, n2, FALSE, tv, TRUE) == FAIL)
			    اذهب on_error;
		    }
		    وإلا
		    {
			إذا (list_برش_or_شاخص(val_tv->vval.v_list, is_برش,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    اذهب on_error;
		    }
		}
		اكسر;

	    حالة ISN_ANYINDEX:
	    حالة ISN_ANYSLICE:
		{
		    صحيح		is_برش = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    صحيح		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_برش ? پشته_TV_BOT(-3) : پشته_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (check_can_شاخص(tv, TRUE, TRUE) == FAIL)
			اذهب on_error;
		    var1 = is_برش ? پشته_TV_BOT(-2) : پشته_TV_BOT(-1);
		    var2 = is_برش ? پشته_TV_BOT(-1) : عدم;
		    res = eval_شاخص_inner(tv, is_برش, var1, var2,
							FALSE, عدم, -1, TRUE);
		    clear_tv(var1);
		    إذا (is_برش)
			clear_tv(var2);
		    ectx->ec_پشته.ga_len -= is_برش ? 2 : 1;
		    إذا (res == FAIL)
			اذهب on_error;
		}
		اكسر;

	    حالة ISN_برش:
		{
		    صحيح		شمار = iptr->isn_arg.عدد;

		    // type will have been checked to be a list
		    tv = پشته_TV_BOT(-1);
		    إذا (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			إذا (list != عدم && list->lv_len >= شمار)
			{
			    list_T	*newlist = list_برش(list,
				    شمار, list->lv_len - 1);

			    إذا (newlist != عدم)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    وإلا
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			إذا (tuple != عدم && TUPLE_LEN(tuple) >= شمار)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_برش(tuple, شمار,
							TUPLE_LEN(tuple) - 1);
			    إذا (newtuple != عدم)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		اكسر;

	    حالة ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*قلم_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = پشته_TV_BOT(-1 - gi->gi_with_op);
		    إذا (tv->v_type == VAR_LIST)
		    {
			li = list_پیدا_کن(tv->vval.v_list, gi->gi_شاخص);
			إذا (li == عدم)
			    اذهب on_error;
			قلم_tv = &li->li_tv;
		    }
		    وإلا
			قلم_tv = TUPLE_قلم(tv->vval.v_tuple, gi->gi_شاخص);

		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    ++ectx->ec_پشته.ga_len;
		    copy_tv(قلم_tv, پشته_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_شاخص = gi->gi_شاخص + 1;
		    ectx->ec_where.wt_نوع = WT_VARIABLE;
		}
		اكسر;

	    // dict member with string key (dict['member'])
	    حالة ISN_MEMBER:
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
		    إذا (کلید == عدم)
			کلید = (char_u *)"";

		    إذا ((di = dict_پیدا_کن(dict, کلید, -1)) == عدم)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_کلید_not_present_درون_dictionary_str), کلید);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_پشته.ga_len;
			tv = پشته_TV_BOT(-1);
			(فراغ) dict_پشته_save(tv);
			tv->v_type = VAR_عدد;
			tv->vval.v_عدد = 0;
			اذهب on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_پشته.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = پشته_TV_BOT(-1);
		    إذا (dict_پشته_save(tv) == FAIL)
			اذهب on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		اكسر;

	    // dict member with string key (dict.member)
	    // or can be an object
	    حالة ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = پشته_TV_BOT(-1);

		    إذا (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			إذا (dict_پشته_save(tv) == FAIL)
			    اذهب on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_داده)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			إذا (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    اذهب on_error;
		    }
		    وإلا
		    {
			إذا (tv->v_type != VAR_DICT || tv->vval.v_dict == عدم)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    اذهب on_error;
			}
			dict = tv->vval.v_dict;

			إذا ((di = dict_پیدا_کن(dict, iptr->isn_arg.string, -1))
								   == عدم)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_کلید_not_present_درون_dictionary_str),
						     iptr->isn_arg.string);
			    اذهب on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			إذا (dict_پشته_save(tv) == FAIL)
			    اذهب on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		اكسر;

	    حالة ISN_GET_OBJ_MEMBER:
	    حالة ISN_GET_ITF_MEMBER:
		{
		    tv = پشته_TV_BOT(-1);
		    إذا (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			اذهب on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    إذا (obj == عدم)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			اذهب on_error;
		    }

		    صحيح idx;
		    إذا (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    وإلا
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_شاخص_from_itf_شاخص(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    إذا (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			اذهب on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		اكسر;

	    حالة ISN_STORE_THIS:
		{
		    صحيح idx = iptr->isn_arg.عدد;
		    object_T *obj = پشته_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *پشته_TV_BOT(-1);
		    --ectx->ec_پشته.ga_len;
		}
		اكسر;

	    حالة ISN_CLEARDICT:
		dict_پشته_drop();
		اكسر;

	    حالة ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_پشته_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = پشته_TV_BOT(-1);
		    إذا (dict_tv != عدم
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != عدم
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == عدم))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_پشته_drop();
		}
		اكسر;

	    حالة ISN_NEGATENR:
		tv = پشته_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		إذا (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		وإلا
		    tv->vval.v_عدد = -tv->vval.v_عدد;
		اكسر;

	    حالة ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    صحيح		r;
		    where_T	where = WHERE_INIT;

		    tv = پشته_TV_BOT((صحيح)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    إذا (ct->ct_arg_idx > 0)
		    {
			where.wt_شاخص = ct->ct_arg_idx;
			where.wt_نوع = ct->ct_is_var ? WT_VARIABLE : WT_آرگومان;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    إذا (r == FAIL)
			اذهب on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    إذا (tv->v_type == VAR_عدد
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_عدد == 0
						|| tv->vval.v_عدد == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_عدد = tv->vval.v_عدد
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		اكسر;

	    حالة ISN_CHECKLEN:
		{
		    صحيح	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = عدم;
		    tuple_T *tuple = عدم;
		    صحيح	    len = 0;

		    tv = پشته_TV_BOT(-1);

		    صحيح		len_check_failed = FALSE;
		    إذا (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			إذا (list == عدم || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_بیشتر_OK))
			    len_check_failed = TRUE;
			إذا (list != عدم)
			    len = list->lv_len;
		    }
		    وإلا إذا (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			إذا (tuple == عدم || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_بیشتر_OK))
			    len_check_failed = TRUE;
			إذا (tuple != عدم)
			    len = TUPLE_LEN(tuple);
		    }
		    وإلا
			len_check_failed = TRUE;

		    إذا (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_اقلام_but_got_nr),
				     min_len, len);
			اذهب on_error;
		    }
		}
		اكسر;

	    حالة ISN_SETTYPE:
		مجموعه_tv_type(پشته_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		اكسر;

	    حالة ISN_2BOOL:
	    حالة ISN_COND2BOOL:
		{
		    صحيح n;
		    صحيح error = FALSE;

		    إذا (iptr->isn_type == ISN_2BOOL)
		    {
			tv = پشته_TV_BOT(iptr->isn_arg.tobool.جابه_جایی);
			n = tv2bool(tv);
			إذا (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    وإلا
		    {
			tv = پشته_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			إذا (error)
			    اذهب on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_عدد = n ? VVAL_TRUE : VVAL_FALSE;
		}
		اكسر;

	    حالة ISN_2STRING:
	    حالة ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		إذا (do_2string(پشته_TV_BOT(iptr->isn_arg.tostring.جابه_جایی),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    اذهب on_error;
		اكسر;

	    حالة ISN_RANGE:
		{
		    exarg_T	ea;
		    حرف	*errormsg;

		    ea.line2 = 0;
		    ea.addr_شمار = 0;
		    ea.addr_type = ADDR_سطرها;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    إذا (parse_cmd_آدرس(&ea, &errormsg, FALSE) == FAIL)
			اذهب on_error;

		    إذا (GA_GROW_FAILS(&ectx->ec_پشته, 1))
			اذهب theend;
		    ++ectx->ec_پشته.ga_len;
		    tv = پشته_TV_BOT(-1);
		    tv->v_type = VAR_عدد;
		    tv->v_قفل = 0;
		    tv->vval.v_عدد = ea.line2;
		}
		اكسر;

	    حالة ISN_PUT:
		tv = عدم; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		اكسر;
	    حالة ISN_IPUT:
		tv = عدم;
		isn_put_do(ectx, iptr, tv, TRUE);
		اكسر;

	    حالة ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_پشته.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		اكسر;

	    حالة ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = عدم;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		اكسر;

	    حالة ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = پشته_TV_BOT(-1);
		إذا (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_آرگومان_must_be_sequence_از_lists_or_tuples));
		    اذهب on_error;
		}

		صحيح rc;
		إذا (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		وإلا
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		إذا (rc != EXEC_OK)
		{
		    إذا (rc == EXEC_FAIL)
			اذهب on_error;
		    اذهب theend;
		}
		اكسر;

	    حالة ISN_PROF_شروع:
	    حالة ISN_PROF_انتها:
		{
#اگر_تعریف_شده FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_داده)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    إذا (iptr->isn_type == ISN_PROF_شروع)
		    {
			func_سطر_شروع(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_سطر_exec(&cookie);
		    }
		    وإلا
			func_سطر_انتها(&cookie);
#پایان_اگر
		}
		اكسر;

	    حالة ISN_DEBUG:
		handle_debug(iptr, ectx);
		اكسر;

	    حالة ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    صحيح		قلم = iptr->isn_arg.shuffle.shfl_قلم;
		    صحيح		به_بالا = iptr->isn_arg.shuffle.shfl_به_بالا;

		    tmp_tv = *پشته_TV_BOT(-قلم);
		    لكل ( ; به_بالا > 0 && قلم > 1; --به_بالا)
		    {
			*پشته_TV_BOT(-قلم) = *پشته_TV_BOT(-قلم + 1);
			--قلم;
		    }
		    *پشته_TV_BOT(-قلم) = tmp_tv;
		}
		اكسر;

	    حالة ISN_DROP:
		--ectx->ec_پشته.ga_len;
		clear_tv(پشته_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		اكسر;

	    حالة ISN_SCRIPTCTX_مجموعه:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		اكسر;
	}
	استمر;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	إذا (ectx->ec_قاب_idx == ectx->ec_initial_قاب_idx)
	    اذهب انجامشده;

	إذا (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    اذهب theend;
	استمر;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	إذا (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    إذا (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		طالما (ectx->ec_پشته.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_پشته.ga_len;
		    clear_tv(پشته_TV_BOT(0));
		}
		طالما (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    استمر;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	إذا (trylevel <= ectx->ec_trylevel_at_شروع)
	    اذهب theend;
    }

انجامشده:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_پشته_clear(dict_پشته_len_at_شروع);
    ectx->ec_trylevel_at_شروع = save_trylevel_at_شروع;
    أرجع ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    صحيح
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    صحيح		save_iidx = ectx->ec_iidx;
    صحيح		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    إذا (res == OK)
    {
	*rettv = *پشته_TV_BOT(-1);
	--ectx->ec_پشته.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    أرجع res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(فراغ)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    صحيح		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    إذا (exec_instructions(ectx) == OK)
    {
	typval_T *tv = پشته_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_پشته.ga_len;
	clear_tv(tv);
    }
    وإلا
    {
	substitute_instr->subs_وضعیت = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    أرجع res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    صحيح
call_def_function(
    ufunc_T	*ufunc,
    صحيح		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    صحيح		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    صحيح		argc = argc_arg;
    صحيح		partial_argc = partial == عدم
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    صحيح		مجموع_argc = argc + partial_argc;
    typval_T	*tv;
    صحيح		idx;
    صحيح		ret = FAIL;
    صحيح		defcount = ufunc->uf_آرگومانها.ga_len - مجموع_argc;
    sctx_T	save_current_sctx = current_sctx;
    صحيح		did_emsg_before = did_emsg_cumul + did_emsg;
    صحيح		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_پیام_list = عدم;
    msglist_T	*خصوصی_پیام_list = عدم;
    صحيح		save_emsg_silent_def = emsg_silent_def;
    صحيح		save_did_emsg_def = did_emsg_def;
    صحيح		orig_funcdepth;
    صحيح		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#لغو_تعریف پشته_TV
#تعریف_کن پشته_TV(idx) (((typval_T *)ectx.ec_پشته.ga_داده) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#لغو_تعریف پشته_TV_BOT
#تعریف_کن پشته_TV_BOT(idx) (((typval_T *)ectx.ec_پشته.ga_داده) + ectx.ec_پشته.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#لغو_تعریف پشته_TV_VAR
#تعریف_کن پشته_TV_VAR(idx) (((typval_T *)ectx.ec_پشته.ga_داده) + ectx.ec_قاب_idx + پشته_قاب_اندازه + idx)

    إذا (ufunc->uf_def_وضعیت == UF_NOT_COMPILED
	    || ufunc->uf_def_وضعیت == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), عدم) == FAIL))
    {
	إذا (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	أرجع FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_داده)
							 + ufunc->uf_dfunc_idx;
	إذا (dfunc->df_ufunc == عدم)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    أرجع FAIL;
	}
	إذا (INSTRUCTIONS(dfunc) == عدم)
	{
	    iemsg("using call_def_function() on not compiled function");
	    أرجع FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    إذا (funcdepth_increment() == FAIL)
	أرجع FAIL;

    CLEAR_فیلد(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_پشته, حجم(typval_T), 500);
    إذا (GA_GROW_FAILS(&ectx.ec_پشته, 20))
    {
	funcdepth_decrement();
	أرجع FAIL;
    }
    ga_init2(&ectx.ec_trystack, حجم(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, حجم(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = مجموع_argc - ufunc->uf_آرگومانها.ga_len;
    إذا (idx > 0 && ufunc->uf_va_name == عدم)
    {
	semsg(NGETTEXT(e_one_آرگومان_too_many, e_nr_arguments_too_many,
								    idx), idx);
	اذهب failed_early;
    }
    idx = مجموع_argc - ufunc->uf_آرگومانها.ga_len + ufunc->uf_def_آرگومانها.ga_len;
    إذا (idx < 0)
    {
	semsg(NGETTEXT(e_one_آرگومان_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	اذهب failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    لكل (idx = 0; idx < مجموع_argc
	    && (ufunc->uf_va_name != عدم || idx < ufunc->uf_آرگومانها.ga_len);
									 ++idx)
    {
	صحيح argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	إذا (idx >= ufunc->uf_آرگومانها.ga_len - ufunc->uf_def_آرگومانها.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_عدد == VVAL_NONE)
	{
	    // Use the default value.
	    پشته_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	وإلا
	{
	    صحيح انجامشده = FALSE;
	    إذا (ufunc->uf_arg_types != عدم && idx < ufunc->uf_آرگومانها.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		إذا (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_عدد)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    پشته_TV_BOT(0)->v_type = VAR_FLOAT;
		    پشته_TV_BOT(0)->v_قفل = 0;
		    پشته_TV_BOT(0)->vval.v_float = tv->vval.v_عدد;
		    انجامشده = TRUE;
		}
		وإلا إذا (check_typval_arg_type(expected, tv,
						   عدم, argv_idx + 1) == FAIL)
		    اذهب failed_early;
	    }
	    إذا (!انجامشده)
		copy_tv(tv, پشته_TV_BOT(0));
	}
	++ectx.ec_پشته.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    إذا (ufunc->uf_va_name != عدم)
    {
	صحيح vararg_شمار = argc - ufunc->uf_آرگومانها.ga_len;

	إذا (vararg_شمار < 0)
	    vararg_شمار = 0;
	وإلا
	    argc -= vararg_شمار;
	إذا (exe_newlist(vararg_شمار, &ectx) == FAIL)
	    اذهب failed_early;

	// Check the type of the list items.
	tv = پشته_TV_BOT(-1);
	إذا (ufunc->uf_va_type != عدم
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != عدم)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_اولی;

	    لكل (idx = 0; idx < vararg_شمار; ++idx)
	    {
		إذا (check_typval_arg_type(expected, &li->li_tv,
						 عدم, argc + idx + 1) == FAIL)
		    اذهب failed_early;
		li = li->li_بعدی;
	    }
	}

	إذا (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *پشته_TV_BOT(defcount - 1) = *پشته_TV_BOT(-1);
	--ectx.ec_پشته.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    إذا (defcount > 0)
	لكل (idx = 0; idx < defcount; ++idx)
	{
	    پشته_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_پشته.ga_len;
	}
    إذا (ufunc->uf_va_name != عدم)
	++ectx.ec_پشته.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_قاب_idx = ectx.ec_پشته.ga_len;
    ectx.ec_initial_قاب_idx = ectx.ec_قاب_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_داده)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *پایه_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	إذا (partial != عدم || پایه_ufunc->uf_partial != عدم)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    إذا (ectx.ec_outer_ref == عدم)
		اذهب failed_early;
	    إذا (partial != عدم)
	    {
		outer_T *outer = get_pt_outer(partial);

		إذا (outer->بیرون_پشته == عدم && outer->بیرون_loop_اندازه == 0)
		{
		    // no stack was set
		    إذا (current_ectx != عدم)
		    {
			إذا (current_ectx->ec_outer_ref != عدم
			       && current_ectx->ec_outer_ref->or_outer != عدم)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		وإلا
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    وإلا
	    {
		ectx.ec_outer_ref->or_outer = &پایه_ufunc->uf_partial->pt_outer;
		++پایه_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = پایه_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    لكل (idx = 0; idx < پشته_قاب_اندازه; ++idx)
    {
	پشته_TV(ectx.ec_پشته.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_پشته.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_داده)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	لكل (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    پشته_TV_VAR(idx)->v_type = VAR_عدد;
	    پشته_TV_VAR(idx)->vval.v_عدد = 0;
	}
	ectx.ec_پشته.ga_len += dfunc->df_varcount;

	إذا (object != عدم)
	{
	    // the object is always the variable at index zero
	    tv = پشته_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	إذا (dfunc->df_has_بستگی)
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
    إذا (funccal != عدم)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_افزودن_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_نسخه = SCRIPT_نسخه_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_پیام_list = پیام_list;
    پیام_list = &خصوصی_پیام_list;

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
    إذا (ret == OK)
    {
	// function finished, get result from the stack.
	إذا (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	وإلا
	{
	    tv = پشته_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    إذا (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_بستگی_درون_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_برداشت();
    current_sctx = save_current_sctx;

    إذا (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    إذا (*پیام_list != عدم && saved_پیام_list != عدم)
    {
	msglist_T **plist = saved_پیام_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	طالما (*plist != عدم)
	    plist = &(*plist)->بعدی;

	*plist = *پیام_list;
    }
    پیام_list = saved_پیام_list;

    إذا (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = عدم;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    لكل (idx = 0; idx < ectx.ec_پشته.ga_len; ++idx)
    {
	tv = پشته_TV(idx);
	إذا (tv->v_type != VAR_عدد && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_پشته.ga_داده);
    vim_free(ectx.ec_trystack.ga_داده);
    إذا (ectx.ec_outer_ref != عدم)
    {
	إذا (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    إذا (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    أرجع ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    فراغ
unwind_def_callstack(ectx_T *ectx)
{
    طالما (ectx->ec_قاب_idx != ectx->ec_initial_قاب_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    فراغ
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_داده) + ectx->ec_dfunc_idx;

    إذا (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    ساكن char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    صحيح		عمق;

    ga_init2(&ga, 1, 100);
    لكل (عمق = 0; عمق < lvi->lvi_عمق; ++عمق)
    {
	إذا (ga_grow(&ga, 50) == FAIL)
	    اكسر;
	إذا (lvi->lvi_loop[عمق].var_idx == 0)
	    STRCPY((حرف *)ga.ga_داده + ga.ga_len, " -");
	وإلا
	    vim_snprintf((حرف *)ga.ga_داده + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[عمق].var_idx,
			    lvi->lvi_loop[عمق].var_idx
					 + lvi->lvi_loop[عمق].var_شمار - 1);
	ga.ga_len = (صحيح)STRLEN(ga.ga_داده);
    }
    أرجع ga.ga_داده;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    ساكن فراغ
list_instructions(حرف *pfx, isn_T *instr, صحيح instr_شمار, ufunc_T *ufunc)
{
    صحيح		سطر_idx = 0;
    صحيح		prev_current = 0;
    صحيح		current;
    صحيح		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    لكل (current = 0; current < instr_شمار; ++current)
    {
	isn_T	    *iptr = &instr[current];
	حرف	    *سطر;

	إذا (ufunc != عدم)
	{
	    طالما (سطر_idx < iptr->isn_lnum
					  && سطر_idx < ufunc->uf_سطرها.ga_len)
	    {
		إذا (current > prev_current)
		{
		    پیام_puts("\n\n");
		    prev_current = current;
		}
		سطر = ((حرف **)ufunc->uf_سطرها.ga_داده)[سطر_idx++];
		إذا (سطر != عدم)
		    پیام(سطر);
	    }
	    إذا (iptr->isn_type == ISN_JUMP_IF_ARG_مجموعه)
	    {
		صحيح	اولی_def_arg = ufunc->uf_آرگومانها.ga_len
						   - ufunc->uf_def_آرگومانها.ga_len;

		إذا (def_arg_idx > 0)
		    پیام_puts("\n\n");
		پیام_شروع();
		پیام_puts("  ");
		پیام_puts(((حرف **)(ufunc->uf_آرگومانها.ga_داده))[
						 اولی_def_arg + def_arg_idx]);
		پیام_puts(" = ");
		پیام_puts(((حرف **)(ufunc->uf_def_آرگومانها.ga_داده))[def_arg_idx++]);
		پیام_clr_eos();
		پیام_انتها();
	    }
	}

	بدل (iptr->isn_type)
	{
	    حالة ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (صحيح)iptr->isn_arg.construct.construct_اندازه);
		اكسر;
	    حالة ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_EXEC_تقسیم:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		اكسر;
	    حالة ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		اكسر;
	    حالة ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.عدد ? " append" : "");
		اكسر;
	    حالة ISN_CEXPR_AUCMD:
#اگر_تعریف_شده FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.عدد));
#پایان_اگر
		اكسر;
	    حالة ISN_CEXPR_CORE:
#اگر_تعریف_شده FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#پایان_اگر
		اكسر;
	    حالة ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, بیشترین_صحیح, عدم);
		پیام("     -------------");
		اكسر;
	    حالة ISN_منبع:
		{
		    scriptitem_T    *si = SCRIPT_قلم(iptr->isn_arg.عدد);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		اكسر;
	    حالة ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, بیشترین_صحیح, عدم);
		    پیام("     -------------");
		}
		اكسر;
	    حالة ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.عدد);
		اكسر;
	    حالة ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_شمار);
		}
		اكسر;
	    حالة ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_ECHOWINDOW:
		إذا (iptr->isn_arg.echowin.ewin_زمان > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_شمار,
				      iptr->isn_arg.echowin.ewin_زمان);
		وإلا
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_شمار);
		اكسر;
	    حالة ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_بارگیری:
		{
		    إذا (iptr->isn_arg.عدد < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.عدد
							  + پشته_قاب_اندازه));
		    وإلا
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		}
		اكسر;
	    حالة ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    إذا (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_عمق,
					outer->outer_idx + پشته_قاب_اندازه);
		    وإلا إذا (outer->outer_عمق < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_عمق);
		    وإلا
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_عمق,
					      outer->outer_idx);
		}
		اكسر;
	    حالة ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_قلم(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    إذا (sv == عدم)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    وإلا
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		اكسر;
	    حالة ISN_LOADS:
	    حالة ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_قلم(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		اكسر;
	    حالة ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		اكسر;
	    حالة ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		اكسر;
	    حالة ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		اكسر;
	    حالة ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		اكسر;
	    حالة ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (صحيح)(iptr->isn_arg.عدد));
		اكسر;

	    حالة ISN_STORE:
		إذا (iptr->isn_arg.عدد < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.عدد + پشته_قاب_اندازه);
		وإلا
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.عدد);
		اكسر;
	    حالة ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    إذا (outer->outer_عمق < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_عمق);
		    وإلا
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_عمق, outer->outer_idx);
		}
		اكسر;
	    حالة ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_STORES:
	    حالة ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_قلم(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		اكسر;
	    حالة ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_قلم(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    إذا (sv == عدم)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    وإلا
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		اكسر;
	    حالة ISN_STOREOPT:
	    حالة ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		اكسر;
	    حالة ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (صحيح)iptr->isn_arg.عدد);
		اكسر;
	    حالة ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		اكسر;

	    حالة ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		اكسر;

	    حالة ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		اكسر;

	    حالة ISN_بارگیری_CLASSMEMBER:
	    حالة ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    صحيح	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_بارگیری_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		اكسر;

	    // constants
	    حالة ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_PUSHBOOL:
	    حالة ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		اكسر;
	    حالة ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		اكسر;
	    حالة ISN_PUSHFUNC:
		{
		    حرف *name = (حرف *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == عدم ? "[none]" : name);
		}
		اكسر;
	    حالة ISN_PUSHCHANNEL:
#اگر_تعریف_شده FEAT_کار_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#پایان_اگر
		اكسر;
	    حالة ISN_PUSHJOB:
#اگر_تعریف_شده FEAT_کار_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#پایان_اگر
		اكسر;
	    حالة ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		اكسر;
	    حالة ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == عدم ? "null"
				 : (حرف *)iptr->isn_arg.classarg->class_name.string);
		اكسر;
	    حالة ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		اكسر;
	    حالة ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		اكسر;
	    حالة ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		اكسر;
	    حالة ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		اكسر;
	    حالة ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		اكسر;
	    حالة ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		اكسر;
	    حالة ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		اكسر;
	    حالة ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		اكسر;

	    // function call
	    حالة ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    داخلی_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		اكسر;
	    حالة ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_داده)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		اكسر;
	    حالة ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != عدم
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		اكسر;
	    حالة ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		اكسر;
	    حالة ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_بالاترین ? " top" : "", cpfunc->cpf_argcount);
		}
		اكسر;
	    حالة ISN_PCALL_انتها:
		smsg("%s%4d PCALL end", pfx, current);
		اكسر;
	    حالة ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (صحيح)iptr->isn_arg.defer.defer_argcount);
		اكسر;
	    حالة ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		اكسر;
	    حالة ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		اكسر;
	    حالة ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		اكسر;
	    حالة ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    إذا (extra == عدم || extra->fre_func_name == عدم)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_داده)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    وإلا
			name = extra->fre_func_name;
		    إذا (extra != عدم && extra->fre_class != عدم)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    وإلا إذا (extra == عدم
				     || extra->fre_loopvar_info.lvi_عمق == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    وإلا
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		اكسر;

	    حالة ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    إذا (arg->nfa_loopvar_info.lvi_عمق == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    وإلا
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		اكسر;

	    حالة ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == عدم ? (char_u *)"" : name);
		}
		اكسر;

	    حالة ISN_JUMP:
		{
		    حرف *when = "?";

		    بدل (iptr->isn_arg.jump.jump_when)
		    {
			حالة JUMP_ALWAYS:
			    when = "JUMP";
			    اكسر;
			حالة JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    اكسر;
			حالة JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    اكسر;
			حالة JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    اكسر;
			حالة JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  //استفادهنشده
			    اكسر;
			حالة JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    اكسر;
			حالة JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    اكسر;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		اكسر;

	    حالة ISN_JUMP_IF_ARG_مجموعه:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + پشته_قاب_اندازه,
						iptr->isn_arg.jump.jump_where);
		اكسر;

	    حالة ISN_JUMP_IF_ARG_NOT_مجموعه:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + پشته_قاب_اندازه,
						iptr->isn_arg.jump.jump_where);
		اكسر;

	    حالة ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_انتها);
		}
		اكسر;

	    حالة ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->انتها_funcref_idx,
			    endloop->انتها_var_idx,
			    endloop->انتها_var_idx + endloop->انتها_var_شمار - 1,
			    endloop->انتها_عمق);
		}
		اكسر;

	    حالة ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_انتها);
		}
		اكسر;

	    حالة ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    إذا (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    وإلا
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		اكسر;
	    حالة ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		اكسر;
	    حالة ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_سطوح,
				      trycont->tct_سطوح == 1 ? "" : "s",
				      trycont->tct_where);
		}
		اكسر;
	    حالة ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		اكسر;
	    حالة ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		اكسر;
	    حالة ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		اكسر;

	    // expression operations on number
	    حالة ISN_OPNR:
	    حالة ISN_OPFLOAT:
	    حالة ISN_OPANY:
		{
		    حرف *what;
		    حرف *ins;

		    بدل (iptr->isn_arg.op.op_type)
		    {
			حالة عبارت_MULT: what = "*"; اكسر;
			حالة عبارت_DIV: what = "/"; اكسر;
			حالة عبارت_REM: what = "%"; اكسر;
			حالة عبارت_SUB: what = "-"; اكسر;
			حالة عبارت_جمع_کن: what = "+"; اكسر;
			حالة عبارت_LSHIFT: what = "<<"; اكسر;
			حالة عبارت_RSHIFT: what = ">>"; اكسر;
			افتراضي:       what = "???"; اكسر;
		    }
		    بدل (iptr->isn_type)
		    {
			حالة ISN_OPNR: ins = "OPNR"; اكسر;
			حالة ISN_OPFLOAT: ins = "OPFLOAT"; اكسر;
			حالة ISN_OPANY: ins = "OPANY"; اكسر;
			افتراضي: ins = "???"; اكسر;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		اكسر;

	    حالة ISN_COMPAREBOOL:
	    حالة ISN_COMPARESPECIAL:
	    حالة ISN_COMPARENULL:
	    حالة ISN_COMPARENR:
	    حالة ISN_COMPAREFLOAT:
	    حالة ISN_COMPARESTRING:
	    حالة ISN_COMPAREBLOB:
	    حالة ISN_COMPARELIST:
	    حالة ISN_COMPARETUPLE:
	    حالة ISN_COMPAREDICT:
	    حالة ISN_COMPAREFUNC:
	    حالة ISN_COMPAREOBJECT:
	    حالة ISN_COMPAREANY:
		   {
		       حرف *p;
		       حرف buf[10];
		       حرف *type;

		       بدل (iptr->isn_arg.op.op_type)
		       {
			   حالة عبارت_EQUAL:	 p = "=="; اكسر;
			   حالة عبارت_NEQUAL:    p = "!="; اكسر;
			   حالة عبارت_GREATER:   p = ">"; اكسر;
			   حالة عبارت_GEQUAL:    p = ">="; اكسر;
			   حالة عبارت_SMALLER:   p = "<"; اكسر;
			   حالة عبارت_SEQUAL:    p = "<="; اكسر;
			   حالة عبارت_تطابق:	 p = "=~"; اكسر;
			   حالة عبارت_IS:	 p = "is"; اكسر;
			   حالة عبارت_ISNOT:	 p = "isnot"; اكسر;
			   حالة عبارت_NOMATCH:	 p = "!~"; اكسر;
			   افتراضي:  p = "???"; اكسر;
		       }
		       STRCPY(buf, p);
		       إذا (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       بدل(iptr->isn_type)
		       {
			   حالة ISN_COMPAREBOOL: type = "COMPAREBOOL"; اكسر;
			   حالة ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; اكسر;
			   حالة ISN_COMPARENULL: type = "COMPARENULL"; اكسر;
			   حالة ISN_COMPARENR: type = "COMPARENR"; اكسر;
			   حالة ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; اكسر;
			   حالة ISN_COMPARESTRING:
						  type = "COMPARESTRING"; اكسر;
			   حالة ISN_COMPAREBLOB: type = "COMPAREBLOB"; اكسر;
			   حالة ISN_COMPARELIST: type = "COMPARELIST"; اكسر;
			   حالة ISN_COMPARETUPLE: type = "COMPARETUPLE"; اكسر;
			   حالة ISN_COMPAREDICT: type = "COMPAREDICT"; اكسر;
			   حالة ISN_COMPAREFUNC: type = "COMPAREFUNC"; اكسر;
			   حالة ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; اكسر;
			   حالة ISN_COMPAREANY: type = "COMPAREANY"; اكسر;
			   افتراضي: type = "???"; اكسر;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   اكسر;

	    حالة ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); اكسر;
	    حالة ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); اكسر;
	    حالة ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); اكسر;

	    // expression operations
	    حالة ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.عدد));
		اكسر;
	    حالة ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); اكسر;
	    حالة ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); اكسر;
	    حالة ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); اكسر;
	    حالة ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); اكسر;
	    حالة ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); اكسر;
	    حالة ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); اكسر;
	    حالة ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); اكسر;
	    حالة ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); اكسر;
	    حالة ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); اكسر;
	    حالة ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); اكسر;
	    حالة ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); اكسر;
	    حالة ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); اكسر;
	    حالة ISN_برش: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.عدد); اكسر;
	    حالة ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_شاخص,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); اكسر;
	    حالة ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); اكسر;
	    حالة ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); اكسر;
	    حالة ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (صحيح)iptr->isn_arg.classmember.cm_idx);
				     اكسر;
	    حالة ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (صحيح)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     اكسر;
	    حالة ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (صحيح)iptr->isn_arg.عدد); اكسر;
	    حالة ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); اكسر;
	    حالة ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); اكسر;

	    حالة ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); اكسر;

	    حالة ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      حرف	    *tofree = عدم;
		      حرف	    *typename;

		      إذا (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_عدد_OK))
			  typename = "float|number";
		      وإلا إذا (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      وإلا
			  typename = type_name(ct->ct_type, &tofree);

		      إذا (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (صحيح)ct->ct_off);
		      وإلا
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (صحيح)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (صحيح)ct->ct_arg_idx);
		      vim_free(tofree);
		      اكسر;
		  }
	    حالة ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_بیشتر_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       اكسر;
	    حالة ISN_SETTYPE:
		  {
		      حرف *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      اكسر;
		  }
	    حالة ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); اكسر;
	    حالة ISN_2BOOL: إذا (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.جابه_جایی);
			    وإلا
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.جابه_جایی);
			    اكسر;
	    حالة ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.جابه_جایی));
			      اكسر;
	    حالة ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.جابه_جایی));
			      اكسر;
	    حالة ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    اكسر;
	    حالة ISN_PUT:
		إذا (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		وإلا إذا (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		وإلا
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (دراز)iptr->isn_arg.put.put_lnum);
		اكسر;
	    حالة ISN_IPUT:
		إذا (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		وإلا إذا (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		وإلا
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (دراز)iptr->isn_arg.put.put_lnum);
		اكسر;
	    حالة ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  عدم, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    إذا (likely(buf != عدم))
		    {
			(فراغ)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    اكسر;
		}
	    حالة ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); اكسر;

	    حالة ISN_PROF_شروع:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 اكسر;

	    حالة ISN_PROF_انتها:
		smsg("%s%4d PROFILE END", pfx, current);
		اكسر;

	    حالة ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_نامها_len);
		اكسر;

	    حالة ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_شمار,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      اكسر;
	    حالة ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_قلم,
					 iptr->isn_arg.shuffle.shfl_به_بالا);
			      اكسر;
	    حالة ISN_DROP: smsg("%s%4d DROP", pfx, current); اكسر;

	    حالة ISN_SCRIPTCTX_مجموعه:
		{
		    صحيح	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_قلم(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		اكسر;

	    حالة ISN_پایان: // End of list of instructions for ISN_SUBSTITUTE.
			   أرجع;
	}

	بیرون_flush();	    // output one line at a time
	ui_breakcheck();
	إذا (got_int)
	    اكسر;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    فراغ
مجموعه_context_درون_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_الگو = arg;

    // first argument already typed: only user function names
    إذا (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_الگو = skipwhite(p);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the list of :disassemble
 * arguments.
 */
    char_u *
get_disassemble_آرگومان(expand_T *xp, صحيح idx)
{
    إذا (idx == 0)
	أرجع (char_u *)"debug";
    إذا (idx == 1)
	أرجع (char_u *)"profile";
    أرجع get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    فراغ
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = عدم;  // init to shut up gcc warning
    صحيح		instr_شمار = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = پیدا_کن_func_by_name(arg, &compile_type);
    إذا (ufunc == عدم)
	أرجع;
    إذا (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, عدم) == FAIL)
	أرجع;
    إذا (ufunc->uf_def_وضعیت != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	أرجع;
    }
    پیام((حرف *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_داده) + ufunc->uf_dfunc_idx;
    بدل (compile_type)
    {
	حالة CT_PROFILE:
#اگر_تعریف_شده FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_شمار = dfunc->df_instr_prof_شمار;
	    اكسر;
#پایان_اگر
	    // FALLTHROUGH
	حالة CT_NONE:
	    instr = dfunc->df_instr;
	    instr_شمار = dfunc->df_instr_شمار;
	    اكسر;
	حالة CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_شمار = dfunc->df_instr_debug_شمار;
	    اكسر;
    }

    list_instructions("", instr, instr_شمار, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    صحيح
tv2bool(typval_T *tv)
{
    بدل (tv->v_type)
    {
	حالة VAR_عدد:
	    أرجع tv->vval.v_عدد != 0;
	حالة VAR_FLOAT:
	    أرجع tv->vval.v_float != 0.0;
	حالة VAR_PARTIAL:
	    أرجع tv->vval.v_partial != عدم;
	حالة VAR_FUNC:
	حالة VAR_STRING:
	    أرجع tv->vval.v_string != عدم && *tv->vval.v_string != NUL;
	حالة VAR_LIST:
	    أرجع tv->vval.v_list != عدم && tv->vval.v_list->lv_len > 0;
	حالة VAR_TUPLE:
	    أرجع tuple_len(tv->vval.v_tuple) > 0;
	حالة VAR_DICT:
	    أرجع tv->vval.v_dict != عدم
				    && tv->vval.v_dict->dv_hashtab.ht_استفاده_شده > 0;
	حالة VAR_BOOL:
	حالة VAR_SPECIAL:
	    أرجع tv->vval.v_عدد == VVAL_TRUE ? TRUE : FALSE;
	حالة VAR_کار:
#اگر_تعریف_شده FEAT_کار_CHANNEL
	    أرجع tv->vval.v_کار != عدم;
#وإلا
	    اكسر;
#پایان_اگر
	حالة VAR_CHANNEL:
#اگر_تعریف_شده FEAT_کار_CHANNEL
	    أرجع tv->vval.v_channel != عدم;
#وإلا
	    اكسر;
#پایان_اگر
	حالة VAR_BLOB:
	    أرجع tv->vval.v_blob != عدم && tv->vval.v_blob->bv_ga.ga_len > 0;

	حالة VAR_OBJECT:
	    أرجع tv->vval.v_object != عدم;

	حالة VAR_CLASS:
	حالة VAR_TYPEALIAS:
	    check_typval_is_مقدار(tv);
	    اكسر;

	حالة VAR_UNKNOWN:
	حالة VAR_ANY:
	حالة VAR_VOID:
	حالة VAR_INSTR:
	    اكسر;
    }
    أرجع FALSE;
}

    فراغ
emsg_using_string_as(typval_T *tv, صحيح as_عدد)
{
    semsg(_(as_عدد ? e_using_string_as_عدد_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == عدم
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    صحيح
check_not_string(typval_T *tv)
{
    إذا (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	أرجع FAIL;
    }
    أرجع OK;
}

#پایان_اگر // FEAT_EVAL
