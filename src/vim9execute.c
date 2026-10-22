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

#تعریف_کرو USING_FLOAT_STUFF
#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
قسمتعریف ساخت {
    صحیح	    tcd_چوکھٹا_idx;	// ec_frame_idx at ISN_TRY
    صحیح	    tcd_ڈھیر_len;	// size of ectx.ec_stack at ISN_TRY
    صحیح	    tcd_اندر_catch;	// in catch or finally block
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
قسمتعریف ساخت {
    صحیح		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    صحیح		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
قسمتعریف ساخت {
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
ساخت ectx_S {
    garray_T	ec_ڈھیر;	// stack of typval_T values
    صحیح		ec_چوکھٹا_idx;	// index in ec_stack: context of ec_dfunc_idx
    صحیح		ec_initial_چوکھٹا_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    صحیح		ec_dfunc_idx;	// current function index
    صحیح		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    صحیح		ec_did_emsg_before;
    صحیح		ec_trylevel_at_آغاز;
    where_T	ec_where;
};

#اگر_تعریف_شدہ FEAT_PROFILE
// stack of profinfo_T used when profiling.
ساکن garray_T profile_info_ga = {0, 0, حجم(profinfo_T), 20, تہی};
#اگر_ختم

// Get pointer to item in the stack.
#تعریف_کرو ڈھیر_TV(idx) (((typval_T *)ectx->ec_ڈھیر.ga_ڈیٹا) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#تعریف_کرو ڈھیر_TV_BOT(idx) (((typval_T *)ectx->ec_ڈھیر.ga_ڈیٹا) + ectx->ec_ڈھیر.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#تعریف_کرو ڈھیر_TV_VAR(idx) (((typval_T *)ectx->ec_ڈھیر.ga_ڈیٹا) + ectx->ec_چوکھٹا_idx + ڈھیر_چوکھٹا_سائز + idx)

// Return value for functions used to execute instructions
#تعریف_کرو EXEC_FAIL	0
#تعریف_کرو EXEC_OK		1
#تعریف_کرو EXEC_مکمل	2

    خالی
مقصد_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_مقصد_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    ساکن صحیح
ufunc_argcount(ufunc_T *ufunc)
{
    واپس ufunc->uf_دلائل.ga_len + (ufunc->uf_va_name != تہی ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    ساکن صحیح
exe_concat(صحیح گنتی, ectx_T *ectx)
{
    صحیح		idx;
    size_t	len = 0;
    garray_T	ga;
    قسمتعریف ساخت
    {
	typval_T    *tv;
	size_t	    لمبائی;
    } string_segment_T;
    شمار
    {
	STRING_SEGMENT_CACHE_سائز = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_سائز];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    اگر (گنتی > (صحیح)ARRAY_لمبائی(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, گنتی);
	اگر (string_segment_tab == تہی)
	    واپس FAIL;
    }

    ga_init2(&ga, حجم(حرف), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    برائے (idx = 0; idx < گنتی; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = ڈھیر_TV_BOT(idx - گنتی);
	اگر (segment->tv->vval.v_string != تہی)
	{
	    segment->لمبائی = STRLEN(segment->tv->vval.v_string);
	    len += segment->لمبائی;
	}
	ورنہ
	    segment->لمبائی = 0;    // Ensure clean state for the second pass
    }

    اگر (ga_grow(&ga, (صحیح)len + 1) == FAIL)
    {
	اگر (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	واپس FAIL;
    }

    برائے (idx = 0; idx < گنتی; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->لمبائی);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_ڈھیر.ga_len -= گنتی - 1;
    ڈھیر_TV_BOT(-1)->vval.v_string = ga.ga_ڈیٹا;

    اگر (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    واپس OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    ساکن صحیح
exe_newlist(صحیح گنتی, ectx_T *ectx)
{
    list_T	*list = تہی;
    صحیح		idx;
    typval_T	*tv;

    اگر (گنتی >= 0)
    {
	list = list_alloc_with_اشیاء(گنتی);
	اگر (list == تہی)
	    واپس FAIL;
	برائے (idx = 0; idx < گنتی; ++idx)
	    list_سیٹ_شے(list, idx, ڈھیر_TV_BOT(idx - گنتی));
    }

    اگر (گنتی > 0)
	ectx->ec_ڈھیر.ga_len -= گنتی - 1;
    ورنہ اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
    {
	list_unref(list);
	واپس FAIL;
    }
    ورنہ
	++ectx->ec_ڈھیر.ga_len;
    tv = ڈھیر_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_تالا = 0;
    اگر (list != تہی)
	++list->lv_refcount;
    واپس OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    ساکن صحیح
exe_newtuple(صحیح گنتی, ectx_T *ectx)
{
    tuple_T	*tuple = تہی;
    صحیح		idx;
    typval_T	*tv;

    اگر (گنتی >= 0)
    {
	tuple = tuple_alloc_with_اشیاء(گنتی);
	اگر (tuple == تہی)
	    واپس FAIL;
	برائے (idx = 0; idx < گنتی; ++idx)
	    tuple_سیٹ_شے(tuple, idx, ڈھیر_TV_BOT(idx - گنتی));
    }

    اگر (گنتی > 0)
	ectx->ec_ڈھیر.ga_len -= گنتی - 1;
    ورنہ اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
    {
	tuple_unref(tuple);
	واپس FAIL;
    }
    ورنہ
	++ectx->ec_ڈھیر.ga_len;
    tv = ڈھیر_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_تالا = 0;
    اگر (tuple != تہی)
	++tuple->tv_refcount;

    واپس OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    ساکن صحیح
exe_newdict(صحیح گنتی, ectx_T *ectx)
{
    dict_T	*dict = تہی;
    dictitem_T	*شے;
    char_u	*کلید;
    صحیح		idx;
    typval_T	*tv;

    اگر (گنتی >= 0)
    {
	dict = dict_alloc();
	اگر (unlikely(dict == تہی))
	    واپس FAIL;
	برائے (idx = 0; idx < گنتی; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = ڈھیر_TV_BOT(2 * (idx - گنتی));
	    // check key is unique
	    کلید = tv->vval.v_string == تہی ? (char_u *)"" : tv->vval.v_string;
	    شے = dict_ڈھونڈو(dict, کلید, -1);
	    اگر (شے != تہی)
	    {
		semsg(_(e_duplicate_کلید_اندر_dictionary_str), کلید);
		dict_unref(dict);
		واپس MAYBE;
	    }
	    شے = dictitem_alloc(کلید);
	    clear_tv(tv);
	    اگر (unlikely(شے == تہی))
	    {
		dict_unref(dict);
		واپس FAIL;
	    }
	    tv = ڈھیر_TV_BOT(2 * (idx - گنتی) + 1);
	    شے->di_tv = *tv;
	    شے->di_tv.v_تالا = 0;
	    tv->v_type = VAR_UNKNOWN;
	    اگر (dict_جوڑو(dict, شے) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(شے);
		dict_unref(dict);
		واپس FAIL;
	    }
	}
    }

    اگر (گنتی > 0)
	ectx->ec_ڈھیر.ga_len -= 2 * گنتی - 1;
    ورنہ اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
    {
	dict_unref(dict);
	واپس FAIL;
    }
    ورنہ
	++ectx->ec_ڈھیر.ga_len;
    tv = ڈھیر_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_تالا = 0;
    tv->vval.v_dict = dict;
    اگر (dict != تہی)
	++dict->dv_refcount;
    واپس OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    خالی
update_has_breakpoint(ufunc_T *ufunc)
{
    اگر (ufunc->uf_debug_tick == debug_tick)
	واپس;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_ڈھونڈو_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

ساکن garray_T dict_ڈھیر = GA_خالی_خانے;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    ساکن صحیح
dict_ڈھیر_save(typval_T *tv)
{
    اگر (dict_ڈھیر.ga_growsize == 0)
	ga_init2(&dict_ڈھیر, حجم(typval_T), 10);
    اگر (ga_grow(&dict_ڈھیر, 1) == FAIL)
	واپس FAIL;
    ((typval_T *)dict_ڈھیر.ga_ڈیٹا)[dict_ڈھیر.ga_len] = *tv;
    ++dict_ڈھیر.ga_len;
    واپس OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    ساکن typval_T *
dict_ڈھیر_get_tv(خالی)
{
    اگر (dict_ڈھیر.ga_len == 0)
	واپس تہی;
    واپس ((typval_T *)dict_ڈھیر.ga_ڈیٹا) + dict_ڈھیر.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    ساکن dict_T *
dict_ڈھیر_get_dict(خالی)
{
    typval_T *tv;

    اگر (dict_ڈھیر.ga_len == 0)
	واپس تہی;
    tv = ((typval_T *)dict_ڈھیر.ga_ڈیٹا) + dict_ڈھیر.ga_len - 1;
    اگر (tv->v_type == VAR_DICT)
	واپس tv->vval.v_dict;
    واپس تہی;
}

/*
 * Drop an item from the dict stack.
 */
    ساکن خالی
dict_ڈھیر_drop(خالی)
{
    اگر (dict_ڈھیر.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	واپس;
    }
    --dict_ڈھیر.ga_len;
    clear_tv(((typval_T *)dict_ڈھیر.ga_ڈیٹا) + dict_ڈھیر.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    ساکن خالی
dict_ڈھیر_clear(صحیح len)
{
    جب (dict_ڈھیر.ga_len > len)
	dict_ڈھیر_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    ساکن outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    اگر (ptref == تہی)
	واپس &pt->pt_outer;

    // partial using partial (recursively)
    جب (ptref->pt_outer_partial != تہی)
	ptref = ptref->pt_outer_partial;
    واپس &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    ساکن صحیح
check_ufunc_arg_types(ufunc_T *ufunc, صحیح argcount, صحیح off, ectx_T *ectx)
{
    اگر (ufunc->uf_arg_types == تہی && ufunc->uf_va_type == تہی)
	واپس OK;

    typval_T	*argv = ڈھیر_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    برائے (صحیح i = 0; i < argcount; ++i)
    {
	type_T *type = تہی;

	// assume a v:none argument, using the default value, is always OK
	اگر (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_نمبر == VVAL_NONE)
	    جاری;
	// only pass values to user functions, never types
	اگر (check_typval_is_قدر(&argv[i]) == FAIL)
	    واپس FAIL;

	اگر (i < ufunc->uf_دلائل.ga_len && ufunc->uf_arg_types != تہی)
	    type = ufunc->uf_arg_types[i];
	ورنہ اگر (ufunc->uf_va_type != تہی)
	    type = ufunc->uf_va_type->tt_member;
	اگر (type != تہی && check_typval_arg_type(type,
		    &argv[i], تہی, i + 1) == FAIL)
	    واپس FAIL;
    }
    واپس OK;
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
    ساکن صحیح
call_dfunc(
	صحیح		cdf_idx,
	partial_T	*pt_arg,
	صحیح		argcount_arg,
	ectx_T		*ectx)
{
    صحیح		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    صحیح		did_emsg_before = did_emsg_cumul + did_emsg;
    صحیح		arg_مقصد_جوڑو;
    صحیح		vararg_گنتی = 0;
    صحیح		varcount;
    صحیح		idx;
    estack_T	*اندراج_2;
    funclocal_T	*floc = تہی;
    صحیح		res = OK;
    compiletype_T compile_type;

    اگر (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == تہی ? (char_u *)"unknown" : dfunc->df_name);
	واپس FAIL;
    }

#اگر_تعریف_شدہ FEAT_PROFILE
    اگر (do_profiling == PROF_YES)
    {
	اگر (GA_GROW_OK(&profile_info_ga, 1))
	{
	    profinfo_T *info = ((profinfo_T *)profile_info_ga.ga_ڈیٹا)
						      + profile_info_ga.ga_len;
	    ++profile_info_ga.ga_len;
	    CLEAR_اشارہ_گر(info);
	    profile_may_آغاز_func(info, ufunc,
			(((dfunc_T *)def_functions.ga_ڈیٹا)
					      + ectx->ec_dfunc_idx)->df_ufunc);
	}
    }
#اگر_ختم

    // When debugging and using "cont" switches to the not-debugged
    // instructions, may need to still compile them.
    compile_type = get_compile_type(ufunc);
    اگر (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, تہی);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + cdf_idx;
    }
    اگر (res == FAIL || INSTRUCTIONS(dfunc) == تہی)
    {
	اگر (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	واپس FAIL;
    }

    اگر (ufunc->uf_va_name != تہی)
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
	vararg_گنتی = argcount - ufunc->uf_دلائل.ga_len;
	اگر (vararg_گنتی < 0)
	    vararg_گنتی = 0;
	ورنہ
	    argcount -= vararg_گنتی;
	اگر (exe_newlist(vararg_گنتی, ectx) == FAIL)
	    واپس FAIL;

	vararg_گنتی = 1;
    }

    arg_مقصد_جوڑو = ufunc->uf_دلائل.ga_len - argcount;
    اگر (arg_مقصد_جوڑو < 0)
    {
	semsg(NGETTEXT(e_one_دلیل_too_many, e_nr_arguments_too_many,
			-arg_مقصد_جوڑو), -arg_مقصد_جوڑو);
	واپس FAIL;
    }
    ورنہ اگر (arg_مقصد_جوڑو > ufunc->uf_def_دلائل.ga_len)
    {
	صحیح missing = arg_مقصد_جوڑو - ufunc->uf_def_دلائل.ga_len;

	semsg(NGETTEXT(e_one_دلیل_too_few, e_nr_arguments_too_few,
			missing), missing);
	واپس FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = ڈھیر_TV_BOT(0) - argcount - vararg_گنتی - 1;

    اگر (IS_OBJECT_طریقہ(ufunc) && !IS_CONSTRUCTOR_طریقہ(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == تہی)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	واپس FAIL;
    }

    // Check the argument types.
    اگر (check_ufunc_arg_types(ufunc, argcount, vararg_گنتی, ectx) == FAIL)
	واپس FAIL;

    // While check_ufunc_arg_types call, def function compilation process may
    // run.  If so many def functions are compiled, def_functions array may be
    // reallocated and dfunc may no longer have valid pointer.  Get the object
    // pointer from def_functions again here.
    dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + cdf_idx;

    // Reserve space for:
    // - missing arguments
    // - stack frame
    // - local variables
    // - if needed: a counter for number of closures created in
    //   ectx->ec_funcrefs.
    varcount = dfunc->df_varcount + dfunc->df_has_احاطہ;
    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر,
				     arg_مقصد_جوڑو + ڈھیر_چوکھٹا_سائز + varcount))
	واپس FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = ڈھیر_TV_BOT(0) - argcount - vararg_گنتی - 1;

    // If depth of calling is getting too high, don't execute the function.
    اگر (funcdepth_increment() == FAIL)
	واپس FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    اگر (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	اگر (floc == تہی)
	    واپس FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    اگر (vararg_گنتی > 0 && arg_مقصد_جوڑو > 0)
	*ڈھیر_TV_BOT(arg_مقصد_جوڑو - 1) = *ڈھیر_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    برائے (idx = 0; idx < arg_مقصد_جوڑو; ++idx)
	ڈھیر_TV_BOT(idx - vararg_گنتی)->v_type = VAR_UNKNOWN;
    ectx->ec_ڈھیر.ga_len += arg_مقصد_جوڑو;

    // Store current execution state in stack frame for ISN_RETURN.
    ڈھیر_TV_BOT(ڈھیر_چوکھٹا_FUNC_OFF)->vval.v_نمبر = ectx->ec_dfunc_idx;
    ڈھیر_TV_BOT(ڈھیر_چوکھٹا_IIDX_OFF)->vval.v_نمبر = ectx->ec_iidx;
    ڈھیر_TV_BOT(ڈھیر_چوکھٹا_INSTR_OFF)->vval.v_string = (خالی *)ectx->ec_instr;
    ڈھیر_TV_BOT(ڈھیر_چوکھٹا_OUTER_OFF)->vval.v_string =
						    (خالی *)ectx->ec_outer_ref;
    ڈھیر_TV_BOT(ڈھیر_چوکھٹا_FUNCLOCAL_OFF)->vval.v_string = (خالی *)floc;
    ڈھیر_TV_BOT(ڈھیر_چوکھٹا_IDX_OFF)->vval.v_نمبر = ectx->ec_چوکھٹا_idx;
    ectx->ec_چوکھٹا_idx = ectx->ec_ڈھیر.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    صحیح initcount = dfunc->df_varcount + (dfunc->df_has_احاطہ ? 1 : 0);
    برائے (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = ڈھیر_TV_BOT(ڈھیر_چوکھٹا_سائز + idx);

	tv->v_type = VAR_نمبر;
	tv->vval.v_نمبر = 0;
    }
    ectx->ec_ڈھیر.ga_len += ڈھیر_چوکھٹا_سائز + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    اگر (IS_OBJECT_طریقہ(ufunc))
    {
	اگر (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_اندرونی_error_str), "type in stack is not an object");
	    واپس FAIL;
	}

	*ڈھیر_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != تہی ? pt_arg : ufunc->uf_partial;
    اگر (pt != تہی || (ufunc->uf_flags & FC_احاطہ))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	اگر (ref == تہی)
	    واپس FAIL;
	اگر (pt != تہی)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	ورنہ
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    اگر (unlikely(ref->or_outer == تہی))
	    {
		vim_free(ref);
		واپس FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->برآمد_ڈھیر = &ectx->ec_ڈھیر;
	    ref->or_outer->برآمد_چوکھٹا_idx = ectx->ec_چوکھٹا_idx;
	    اگر (ectx->ec_outer_ref != تہی)
		ref->or_outer->برآمد_اوپر = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    ورنہ
	ectx->ec_outer_ref = تہی;

    اگر (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    اندراج_2 = estack_ڈالو_ufunc(ufunc, 1);
    اگر (اندراج_2 != تہی)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	اندراج_2->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    واپس OK;
}

// Double linked list of funcstack_T in use.
ساکن funcstack_T *پہلا_funcstack = تہی;

    ساکن خالی
جوڑو_funcstack_مقصد_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    اگر (پہلا_funcstack != تہی)
	پہلا_funcstack->fs_پچھلا = funcstack;
    funcstack->fs_اگلا = پہلا_funcstack;
    funcstack->fs_پچھلا = تہی;
    پہلا_funcstack = funcstack;
}

    ساکن خالی
remove_funcstack_from_list(funcstack_T *funcstack)
{
    اگر (funcstack->fs_پچھلا == تہی)
	پہلا_funcstack = funcstack->fs_اگلا;
    ورنہ
	funcstack->fs_پچھلا->fs_اگلا = funcstack->fs_اگلا;
    اگر (funcstack->fs_اگلا != تہی)
	funcstack->fs_اگلا->fs_پچھلا = funcstack->fs_پچھلا;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    ساکن صحیح
handle_احاطہ_اندر_use(ectx_T *ectx, صحیح free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
							  + ectx->ec_dfunc_idx;
    صحیح		argcount;
    صحیح		چوٹی;
    صحیح		idx;
    typval_T	*tv;
    صحیح		احاطہ_اندر_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	احاطہ_گنتی;

    اگر (dfunc->df_ufunc == تہی)
	واپس OK;  // function was freed
    اگر (dfunc->df_has_احاطہ == 0)
	واپس OK;  // no closures
    tv = ڈھیر_TV(ectx->ec_چوکھٹا_idx + ڈھیر_چوکھٹا_سائز + dfunc->df_varcount);
    احاطہ_گنتی = tv->vval.v_نمبر;
    اگر (احاطہ_گنتی == 0)
	واپس OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    چوٹی = ectx->ec_چوکھٹا_idx - argcount;

    // Check if any created closure is still in use.
    برائے (idx = 0; idx < احاطہ_گنتی; ++idx)
    {
	partial_T   *pt;
	صحیح	    off = gap->ga_len - احاطہ_گنتی + idx;

	اگر (off < 0)
	    جاری;  // count is off or already done
	pt = ((partial_T **)gap->ga_ڈیٹا)[off];
	اگر (pt->pt_refcount > 1)
	{
	    صحیح refcount = pt->pt_refcount;
	    صحیح i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    برائے (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = ڈھیر_TV(ectx->ec_چوکھٹا_idx
						       + ڈھیر_چوکھٹا_سائز + i);
		اگر (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    اگر (refcount > 1)
	    {
		احاطہ_اندر_use = TRUE;
		توڑو;
	    }
	}
    }

    اگر (احاطہ_اندر_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *ڈھیر;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	اگر (funcstack == تہی)
	    واپس FAIL;

	funcstack->fs_var_انحراف = argcount + ڈھیر_چوکھٹا_سائز;
	funcstack->fs_ga.ga_len = funcstack->fs_var_انحراف
							  + dfunc->df_varcount;
	ڈھیر = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_ڈیٹا = ڈھیر;
	اگر (ڈھیر == تہی)
	{
	    vim_free(funcstack);
	    واپس FAIL;
	}
	جوڑو_funcstack_مقصد_list(funcstack);

	// Move or copy the arguments.
	برائے (idx = 0; idx < argcount; ++idx)
	{
	    tv = ڈھیر_TV(چوٹی + idx);
	    اگر (free_arguments)
	    {
		*(ڈھیر + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    ورنہ اگر (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, ڈھیر + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	برائے (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = ڈھیر_TV(ectx->ec_چوکھٹا_idx + ڈھیر_چوکھٹا_سائز + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != تہی)
	    {
		صحیح	    i;

		برائے (i = 0; i < احاطہ_گنتی; ++i)
		    اگر (tv->vval.v_partial == ((partial_T **)gap->ga_ڈیٹا)[
					      gap->ga_len - احاطہ_گنتی + i])
			++funcstack->fs_min_refcount;
	    }

	    *(ڈھیر + funcstack->fs_var_انحراف + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	برائے (idx = 0; idx < احاطہ_گنتی; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_ڈیٹا)[gap->ga_len
							- احاطہ_گنتی + idx];
	    اگر (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.برآمد_ڈھیر = &funcstack->fs_ga;
		pt->pt_outer.برآمد_چوکھٹا_idx = ectx->ec_چوکھٹا_idx - چوٹی;
	    }
	}
    }

    برائے (idx = 0; idx < احاطہ_گنتی; ++idx)
	partial_unref(((partial_T **)gap->ga_ڈیٹا)[gap->ga_len
						       - احاطہ_گنتی + idx]);
    gap->ga_len -= احاطہ_گنتی;
    اگر (gap->ga_len == 0)
	ga_clear(gap);

    واپس OK;
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
    صحیح		مکمل = 0;
    typval_T	*ڈھیر;

    اگر (funcstack->fs_refcount > funcstack->fs_min_refcount)
	واپس FALSE;
    برائے (i = funcstack->fs_var_انحراف; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_ڈیٹا) + i;

	اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != تہی
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++مکمل;
    }
    اگر (مکمل != funcstack->fs_min_refcount)
	واپس FALSE;

    ڈھیر = gap->ga_ڈیٹا;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    برائے (i = 0; i < gap->ga_len; ++i)
	clear_tv(ڈھیر + i);
    vim_free(ڈھیر);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    واپس TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    صحیح
سیٹ_ref_اندر_funcstacks(صحیح copyID)
{
    funcstack_T *funcstack;

    برائے (funcstack = پہلا_funcstack; funcstack != تہی;
						funcstack = funcstack->fs_اگلا)
    {
	typval_T    *ڈھیر = funcstack->fs_ga.ga_ڈیٹا;
	صحیح	    i;

	برائے (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    اگر (سیٹ_ref_اندر_شے(ڈھیر + i, copyID, تہی, تہی, تہی))
		واپس TRUE;  // abort
    }
    واپس FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
ساکن ectx_T *current_ectx = تہی;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    صحیح
اندر_def_function(خالی)
{
    واپس current_ectx != تہی;
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
fill_exec_lval_جڑ(lval_جڑ_T *جڑ)
{
    ectx_T *ectx = current_ectx;
    اگر (ectx != تہی)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	اگر (ufunc->uf_class != تہی)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    اگر (tv != تہی)
	    {
		CLEAR_اشارہ_گر(جڑ);
		جڑ->lr_tv = tv;
		copy_tv(ڈھیر_TV_VAR(0), جڑ->lr_tv);
		جڑ->lr_cl_exec = ufunc->uf_class;
		++جڑ->lr_cl_exec->class_refcount;
		واپس OK;
	    }
	}
    }

    واپس FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(خالی)
{
    ectx_T *r = current_ectx;

    current_ectx = تہی;
    واپس r;
}

    خالی
restore_current_ectx(ectx_T *ectx)
{
    اگر (current_ectx != تہی)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    ساکن list_T *
جوڑو_defer_شے(صحیح var_idx, صحیح argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = ڈھیر_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    اگر (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	اگر (rettv_list_alloc(defer_tv) == FAIL)
	    واپس تہی;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_اشیاء(argcount + 1);
    اگر (l == تہی)
	واپس تہی;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_تالا = 0;
    اگر (list_داخل_کرو_tv(defer_l, &listval, defer_l->lv_پہلا) == FAIL)
    {
	list_free(l);
	واپس تہی;
    }

    واپس l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    ساکن صحیح
defer_command(صحیح var_idx, صحیح argcount, ectx_T *ectx)
{
    list_T	*l = جوڑو_defer_شے(var_idx, argcount, ectx);
    صحیح		i;
    typval_T	*func_tv;

    اگر (l == تہی)
	واپس FAIL;

    func_tv = ڈھیر_TV_BOT(-argcount - 1);
    اگر (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	واپس FAIL;
    }
    list_سیٹ_شے(l, 0, func_tv);

    برائے (i = 0; i < argcount; ++i)
	list_سیٹ_شے(l, i + 1, ڈھیر_TV_BOT(-argcount + i));
    ectx->ec_ڈھیر.ga_len -= argcount + 1;
    واپس OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    صحیح
جوڑو_defer_function(char_u *name, صحیح argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    صحیح		i;

    اگر (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	واپس FAIL;
    }

    l = جوڑو_defer_شے(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    اگر (l == تہی)
    {
	vim_free(name);
	واپس FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_تالا = 0;
    func_tv.vval.v_string = name;
    list_سیٹ_شے(l, 0, &func_tv);

    برائے (i = 0; i < argcount; ++i)
	list_سیٹ_شے(l, i + 1, argvars + i);
    واپس OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    ساکن خالی
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = ڈھیر_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    اگر (defer_tv->v_type != VAR_LIST)
	واپس;	 // no function added
    FOR_ALL_LIST_اشیاء(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_دلائل];
	صحیح	    i;
	listitem_T  *arg_li = l->lv_پہلا;
	typval_T    *functv = &l->lv_پہلا->li_tv;
	صحیح	    argcount = l->lv_len - 1;

	اگر (functv->vval.v_string == تہی)
	    // already being called, can happen if function does ":qa"
	    جاری;

	برائے (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_اگلا;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = تہی;
	partial_T	*pt = تہی;

	CLEAR_میدان(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	اگر (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = تہی;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    اگر (funcexe.fe_object != تہی)
		++funcexe.fe_object->obj_refcount;
	}
	ورنہ
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = تہی;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(خالی)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	اگر (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	ورنہ
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    ساکن صحیح
func_return(ectx_T *ectx)
{
    صحیح		idx;
    صحیح		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
							  + ectx->ec_dfunc_idx;
    صحیح		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*اندراج_2;
    صحیح		پچھلا_dfunc_idx = ڈھیر_TV(ectx->ec_چوکھٹا_idx
					+ ڈھیر_چوکھٹا_FUNC_OFF)->vval.v_نمبر;
    funclocal_T	*floc;
#اگر_تعریف_شدہ FEAT_PROFILE
    dfunc_T	*پچھلا_dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
							      + پچھلا_dfunc_idx;

    اگر (do_profiling == PROF_YES)
    {
	ufunc_T *caller = پچھلا_dfunc->df_ufunc;

	اگر (dfunc->df_ufunc->uf_profiling
				   || (caller != تہی && caller->uf_profiling))
	{
	    profile_may_انتہا_func(((profinfo_T *)profile_info_ga.ga_ڈیٹا)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#اگر_ختم

    اگر (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    اگر (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    اندراج_2 = estack_نکالو();
    اگر (اندراج_2 != تہی)
	current_sctx = اندراج_2->es_save_sctx;

    اگر (handle_احاطہ_اندر_use(ectx, TRUE) == FAIL)
	واپس FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    صحیح چوٹی = ectx->ec_چوکھٹا_idx - argcount;
    اگر (IS_OBJECT_طریقہ(dfunc->df_ufunc))
	--چوٹی;
    برائے (idx = چوٹی; idx < ectx->ec_چوکھٹا_idx; ++idx)
	clear_tv(ڈھیر_TV(idx));

    // Clear local variables and temp values, but not the return value.
    برائے (idx = ectx->ec_چوکھٹا_idx + ڈھیر_چوکھٹا_سائز;
					idx < ectx->ec_ڈھیر.ga_len - 1; ++idx)
	clear_tv(ڈھیر_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_ڈھیر.ga_len - 1;
    اگر (ret_idx == ectx->ec_چوکھٹا_idx + ڈھیر_چوکھٹا_IDX_OFF)
	ret_idx = 0;

    اگر (ectx->ec_outer_ref != تہی)
    {
	اگر (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = پچھلا_dfunc_idx;
    ectx->ec_iidx = ڈھیر_TV(ectx->ec_چوکھٹا_idx
					+ ڈھیر_چوکھٹا_IIDX_OFF)->vval.v_نمبر;
    ectx->ec_instr = (خالی *)ڈھیر_TV(ectx->ec_چوکھٹا_idx
				       + ڈھیر_چوکھٹا_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (خالی *)ڈھیر_TV(ectx->ec_چوکھٹا_idx
				       + ڈھیر_چوکھٹا_OUTER_OFF)->vval.v_string;
    floc = (خالی *)ڈھیر_TV(ectx->ec_چوکھٹا_idx
				   + ڈھیر_چوکھٹا_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_چوکھٹا_idx = ڈھیر_TV(ectx->ec_چوکھٹا_idx
				       + ڈھیر_چوکھٹا_IDX_OFF)->vval.v_نمبر;

    اگر (floc == تہی)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    ورنہ
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    اگر (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_ڈھیر.ga_len = چوٹی + 1;
	*ڈھیر_TV_BOT(-1) = *ڈھیر_TV(ret_idx);
    }
    ورنہ
	// Reset the stack to the position before the call.
	ectx->ec_ڈھیر.ga_len = چوٹی;

    funcdepth_decrement();
    --ex_nesting_level;
    واپس OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    ساکن صحیح
call_prepare(صحیح argcount, typval_T *argvars, ectx_T *ectx)
{
    صحیح		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    برائے (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *ڈھیر_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    اگر (argcount > 0)
	ectx->ec_ڈھیر.ga_len -= argcount - 1;
    ورنہ اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
	واپس FAIL;
    ورنہ
	++ectx->ec_ڈھیر.ga_len;

    // Default return value is zero.
    tv = ڈھیر_TV_BOT(-1);
    tv->v_type = VAR_نمبر;
    tv->vval.v_نمبر = 0;
    tv->v_تالا = 0;

    واپس OK;
}

/*
 * Call a builtin function by index.
 */
    ساکن صحیح
call_bfunc(صحیح func_idx, صحیح argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_دلائل];
    صحیح		idx;
    صحیح		did_emsg_before = did_emsg;
    ectx_T	*پچھلا_ectx = current_ectx;
    حرف	*save_func_name = ectx->ec_where.wt_func_name;

    اگر (call_prepare(argcount, argvars, ectx) == FAIL)
	واپس FAIL;

    // Check for void value being passed as an argument.
    برائے (idx = 0; idx < argcount; ++idx)
	اگر (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_قدر));
	    برائے (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    واپس FAIL;
	}

    ectx->ec_where.wt_func_name = اندرونی_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_اندرونی_func_by_idx(func_idx, argvars, ڈھیر_TV_BOT(-1));
    current_ectx = پچھلا_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    برائے (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    اگر (did_emsg > did_emsg_before)
	واپس FAIL;
    واپس OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ساکن صحیح
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	صحیح	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_دلائل];
    funcexe_T   funcexe;
    funcerror_T	error;
    صحیح		idx;
    صحیح		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    اگر (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, تہی)
								       == FAIL)
	واپس FAIL;
    اگر (ufunc->uf_def_حالت == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	اگر (error != FCERR_UNKNOWN)
	{
	    اگر (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    ورنہ
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    واپس FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	اگر (iptr != تہی)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	واپس call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    اگر (call_prepare(argcount, argvars, ectx) == FAIL)
	واپس FAIL;
    CLEAR_میدان(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != تہی ? selfdict : dict_ڈھیر_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      ڈھیر_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    برائے (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    اگر (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_ملا_var);
	واپس FAIL;
    }
    اگر (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	واپس FAIL;
    واپس OK;
}

/*
 * If command modifiers were applied restore them.
 */
    ساکن خالی
may_restore_cmdmod(funclocal_T *funclocal)
{
    اگر (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = تہی;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    ساکن صحیح
vim9_aborting(صحیح پچھلا_uncaught_emsg)
{
    واپس uncaught_emsg > پچھلا_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    ساکن صحیح
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
	صحیح func_idx = ڈھونڈو_اندرونی_func(name);

	اگر (func_idx < 0)  // Impossible?
	    واپس FAIL;
	اگر (check_اندرونی_func(func_idx, argcount) < 0)
	    واپس FAIL;
	واپس call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*آغاز_bracket = generic_func_ڈھونڈو_open_bracket(name);
    اگر (آغاز_bracket != تہی)
    {
	cc = *آغاز_bracket;
	*آغاز_bracket = NUL;
    }

    ufunc = ڈھونڈو_func(name, FALSE);

    اگر (ufunc == تہی)
    {
	صحیح پچھلا_uncaught_emsg = uncaught_emsg;

	اگر (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = ڈھونڈو_func(name, FALSE);

	اگر (vim9_aborting(پچھلا_uncaught_emsg))
	{
	    اگر (آغاز_bracket != تہی)
		*آغاز_bracket = cc;
	    واپس FAIL;  // bail out if loading the script caused an error
	}
    }

    اگر (آغاز_bracket != تہی)
	*آغاز_bracket = cc;

    اگر (ufunc != تہی)
    {
	اگر (IS_GENERIC_FUNC(ufunc))
	{
	    اگر (آغاز_bracket != تہی)
		ufunc = ڈھونڈو_generic_func(ufunc, name, &آغاز_bracket);
	    ورنہ
	    {
		emsg_funcname(e_generic_func_missing_type_دلائل_str, name);
		ufunc = تہی;
	    }
	    اگر (ufunc == تہی)
		واپس FAIL;
	}
	ورنہ اگر (آغاز_bracket != تہی)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    واپس FAIL;
	}

	اگر (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    واپس FAIL;

	واپس call_ufunc(ufunc, تہی, argcount, ectx, iptr, selfdict);
    }

    واپس FAIL;
}

    ساکن صحیح
call_partial(
	typval_T    *tv,
	صحیح	    argcount_arg,
	ectx_T	    *ectx)
{
    صحیح		argcount = argcount_arg;
    char_u	*name = تہی;
    صحیح		called_emsg_before = called_emsg;
    صحیح		res = FAIL;
    dict_T	*selfdict = تہی;

    اگر (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	صحیح	    i;

	اگر (pt->pt_obj != تہی)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		واپس FAIL;
	    برائے (i = 1; i <= argcount; ++i)
		*ڈھیر_TV_BOT(-i + 1) = *ڈھیر_TV_BOT(-i);

	    typval_T *obj_tv = ڈھیر_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_تالا = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_ڈھیر.ga_len;
	}

	اگر (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, pt->pt_argc))
		واپس FAIL;
	    برائے (i = 1; i <= argcount; ++i)
		*ڈھیر_TV_BOT(-i + pt->pt_argc) = *ڈھیر_TV_BOT(-i);
	    ectx->ec_ڈھیر.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    برائے (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], ڈھیر_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	اگر (pt->pt_func != تہی)
	    واپس call_ufunc(pt->pt_func, pt, argcount, ectx, تہی, selfdict);

	name = pt->pt_name;
    }
    ورنہ اگر (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    اگر (name != تہی)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = تہی;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	اگر (error != FCERR_NONE)
	    res = FAIL;
	ورنہ
	    res = call_by_name(fname, argcount, ectx, تہی, selfdict);
	vim_free(tofree);
    }

    اگر (res == FAIL)
    {
	اگر (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == تہی ? (char_u *)"[unknown]" : name);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    ساکن صحیح
error_if_locked(صحیح تالا, حرف *error)
{
    اگر (تالا & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    ساکن صحیح
check_for_نمبر(typval_T *tv)
{
    اگر (tv->v_type != VAR_نمبر)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_نمبر), vartype_name(tv->v_type));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    ساکن خالی
store_var(char_u *name, typval_T *tv)
{
    funccal_اندراج_T اندراج_2;
    صحیح		    flags = تفویض_DECL;

    اگر (tv->v_تالا)
	flags |= تفویض_CONST;
    save_funccal(&اندراج_2);
    سیٹ_var_const(name, 0, تہی, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    ساکن صحیح
do_2string(typval_T *tv, صحیح is_2string_any, صحیح tostring_flags)
{
    اگر (tv->v_type == VAR_STRING)
	واپس OK;

    char_u *str;

    اگر (is_2string_any)
    {
	انتخاب (tv->v_type)
	{
	    حالت VAR_SPECIAL:
	    حالت VAR_BOOL:
	    حالت VAR_نمبر:
	    حالت VAR_FLOAT:
	    حالت VAR_DICT:
	    حالت VAR_BLOB:	توڑو;

	    حالت VAR_LIST:
				اگر (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, حجم(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    اگر (str == تہی)
					واپس FAIL;
				    s = str;
				    جب ((e = vim_strchr(s, '\n')) != تہی)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					اگر (p != تہی)
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
				    tv->vval.v_string = ga.ga_ڈیٹا;
				    واپس OK;
				}
				اگر (tostring_flags & TOSTRING_INTERPOLATE)
				    توڑو;
				// FALLTHROUGH
	    طےشدہ:	مقصد_string_error(tv->v_type);
			واپس FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    واپس OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    ساکن خالی
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    انتخاب (tv->v_type)
    {
	حالت VAR_LIST:
	    اگر (tv->vval.v_list == تہی && sv->sv_type != &t_list_خالی_خانے)
		(خالی)rettv_list_alloc(tv);
	    توڑو;
	حالت VAR_TUPLE:
	    اگر (tv->vval.v_tuple == تہی && sv->sv_type != &t_tuple_خالی_خانے)
		(خالی)rettv_tuple_alloc(tv);
	    توڑو;
	حالت VAR_DICT:
	    اگر (tv->vval.v_dict == تہی && sv->sv_type != &t_dict_خالی_خانے)
		(خالی)rettv_dict_alloc(tv);
	    توڑو;
	حالت VAR_BLOB:
	    اگر (tv->vval.v_blob == تہی && sv->sv_type != &t_blob_null)
		(خالی)rettv_blob_alloc(tv);
	    توڑو;
	طےشدہ:
	    توڑو;
    }
}

/*
 * Return the character "str[index]" where "index" is the character index,
 * including composing characters.
 * If "index" is out of range NULL is returned.
 */
    char_u *
char_from_string(char_u *str, varnumber_T اشاریہ)
{
    size_t	    nbyte = 0;
    varnumber_T	    nchar = اشاریہ;
    size_t	    slen;

    اگر (str == تہی)
	واپس تہی;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    اگر (اشاریہ < 0)
    {
	صحیح	clen = 0;

	برائے (nbyte = 0; nbyte < slen; ++clen)
	{
	    اگر (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    ورنہ اگر (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    ورنہ
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + اشاریہ;
	اگر (nchar < 0)
	    // unlike list: index out of range results in empty string
	    واپس تہی;
    }

    برائے (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	اگر (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	ورنہ اگر (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	ورنہ
	    nbyte += mb_ptr2len(str + nbyte);
    }
    اگر (nbyte >= slen)
	واپس تہی;
    واپس vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    ساکن طویل
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    اگر (nchar >= 0)
    {
	جب (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    ورنہ
    {
	nbyte = str_len;
	جب (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_سر_off(str, str + nbyte);
	    ++nchar;
	}
	اگر (nchar < 0)
	    واپس -1;
    }
    واپس (طویل)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_قاش(char_u *str, varnumber_T پہلا, varnumber_T آخری, صحیح exclusive)
{
    طویل	آغاز_byte, انتہا_byte;
    size_t	slen;

    اگر (str == تہی)
	واپس تہی;
    slen = STRLEN(str);
    آغاز_byte = char_idx2byte(str, slen, پہلا);
    اگر (آغاز_byte < 0)
	آغاز_byte = 0; // first index very negative: use zero
    اگر ((آخری == -1 && !exclusive) || آخری == VARNUM_MAX)
	انتہا_byte = (طویل)slen;
    ورنہ
    {
	انتہا_byte = char_idx2byte(str, slen, آخری);
	اگر (!exclusive && انتہا_byte >= 0 && انتہا_byte < (طویل)slen)
	    // end index is inclusive
	    انتہا_byte += mb_ptr2len(str + انتہا_byte);
    }

    اگر (آغاز_byte >= (طویل)slen || انتہا_byte <= آغاز_byte)
	واپس تہی;
    واپس vim_strnsave(str + آغاز_byte, انتہا_byte - آغاز_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    ساکن svar_T *
get_script_svar(scriptref_T *sref, صحیح dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_شے(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? تہی
			      : ((dfunc_T *)def_functions.ga_ڈیٹا) + dfunc_idx;
    svar_T	    *sv;

    اگر (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	اگر (dfunc != تہی)
	    semsg(_(e_script_variable_invalid_after_reload_اندر_function_str),
					 printable_func_name(dfunc->df_ufunc));
	واپس تہی;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_ڈیٹا) + sref->sref_idx;
    اگر (sv->sv_name == تہی)
    {
	اگر (dfunc != تہی)
	    emsg(_(e_script_variable_was_deleted));
	واپس تہی;
    }
    اگر (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	اگر (dfunc != تہی)
	    emsg(_(e_script_variable_type_بدلا));
	واپس تہی;
    }

    اگر ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	اگر (dfunc != تہی)
	    semsg(_(e_شے_not_exported_اندر_script_str), sv->sv_name);
	واپس تہی;
    }
    واپس sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    ساکن char_u *
get_بانٹو_sourceline(
	صحیح c غیر_مستعمل,
	خالی *cookie,
	صحیح indent غیر_مستعمل,
	getline_opt_T options غیر_مستعمل)
{
    ماخذ_cookie_T	*sp = (ماخذ_cookie_T *)cookie;
    char_u		*p;
    char_u		*سطر;

    p = vim_strchr(sp->nextline, '\n');
    اگر (p == تہی)
    {
	سطر = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    ورنہ
    {
	سطر = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    واپس سطر;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ساکن صحیح
call_eval_func(
	char_u	    *name,
	صحیح	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    صحیح	    called_emsg_before = called_emsg;
    صحیح	    res;

    res = call_by_name(name, argcount, ectx, iptr, تہی);
    اگر (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = ڈھونڈو_var(name, تہی, FALSE);
	اگر (v == تہی || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    واپس FAIL;
	}
	واپس call_partial(&v->di_tv, argcount, ectx);
    }
    واپس res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    صحیح
fill_partial_and_احاطہ(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    اگر (ufunc->uf_flags & FC_احاطہ)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.برآمد_ڈھیر = &ectx->ec_ڈھیر;
	pt->pt_outer.برآمد_چوکھٹا_idx = ectx->ec_چوکھٹا_idx;
	اگر (ectx->ec_outer_ref != تہی)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.برآمد_اوپر = ectx->ec_outer_ref->or_outer;
	    اگر (ectx->ec_outer_ref->or_partial != تہی)
	    {
		pt->pt_outer.برآمد_اوپر_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.برآمد_اوپر_partial->pt_refcount;
	    }
	}

	اگر (lvi != تہی)
	{
	    صحیح	گہرائی;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    برائے (گہرائی = 0; گہرائی < lvi->lvi_گہرائی; ++گہرائی)
	    {
		pt->pt_outer.برآمد_loop[گہرائی].ڈھیر = &ectx->ec_ڈھیر;
		pt->pt_outer.برآمد_loop[گہرائی].var_idx = ectx->ec_چوکھٹا_idx
			 + ڈھیر_چوکھٹا_سائز + lvi->lvi_loop[گہرائی].var_idx;
		pt->pt_outer.برآمد_loop[گہرائی].var_گنتی =
					    lvi->lvi_loop[گہرائی].var_گنتی;
	    }
	    pt->pt_outer.برآمد_loop_سائز = lvi->lvi_گہرائی;
	}
	ورنہ
	    pt->pt_outer.برآمد_loop_سائز = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	اگر (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    واپس FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_ڈھیر.ga_ڈیٹا) + ectx->ec_چوکھٹا_idx
		       + ڈھیر_چوکھٹا_سائز + dfunc->df_varcount)->vval.v_نمبر;

	((partial_T **)ectx->ec_funcrefs.ga_ڈیٹا)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    واپس OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    ساکن صحیح
exec_command(isn_T *iptr, char_u *cmd_string)
{
    ماخذ_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_میدان(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    اگر (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	واپس FAIL;
    واپس OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    صحیح
may_بھرو_script(صحیح sid, صحیح *loaded)
{
    scriptitem_T *si = SCRIPT_شے(sid);

    اگر (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	اگر (do_ماخذ(si->sn_name, FALSE, DOSO_NONE, تہی) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    واپس FAIL;
	}
    }
    واپس OK;
}

// used for v_instr of typval of VAR_INSTR
ساخت instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
قسمتعریف ساخت subs_اظہاریہ_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    صحیح		subs_حالت;
} subs_اظہاریہ_T;

// Set when calling do_debug().
ساکن ectx_T	*debug_context = تہی;
ساکن صحیح	debug_var_گنتی;

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

    اگر (ectx == تہی)
	واپس تہی;
    dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    برائے (idx = debug_var_گنتی - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_نام.ga_ڈیٹا)[idx];

	// the variable name may be NULL when not available in this block
	اگر (varname != تہی && STRCMP(varname, name) == 0)
	    واپس ڈھیر_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == تہی ? 0 : 1;
    برائے (idx = 0; idx < ufunc->uf_دلائل.ga_len; ++idx)
	اگر (STRCMP(((char_u **)(ufunc->uf_دلائل.ga_ڈیٹا))[idx], name) == 0)
	    واپس ڈھیر_TV(ectx->ec_چوکھٹا_idx - ufunc->uf_دلائل.ga_len
							  - varargs_off + idx);
    اگر (ufunc->uf_va_name != تہی && STRCMP(ufunc->uf_va_name, name) == 0)
	واپس ڈھیر_TV(ectx->ec_چوکھٹا_idx - 1);

    واپس تہی;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    صحیح
may_break_اندر_function(ufunc_T *ufunc)
{
    واپس ufunc->uf_has_breakpoint || debug_has_اظہاریہ_breakpoint();
}

    ساکن خالی
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*سطر;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_ڈیٹا)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    صحیح		انتہا_lnum = iptr->isn_lnum;
    garray_T	ga;
    صحیح		lnum;

    اگر (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	اگر (!may_break_اندر_function(ufunc))
	    واپس;

	// check for the next breakpoint if needed
	breakpoint = dbg_ڈھونڈو_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	اگر (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    واپس;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_گنتی = iptr->isn_arg.debug.dbg_var_نام_len;

    برائے (ni = iptr + 1; ni->isn_type != ISN_اختتام; ++ni)
	اگر (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    انتہا_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    توڑو;
	}

    اگر (انتہا_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, حجم(char_u *), 10);
	برائے (lnum = iptr->isn_lnum; lnum < انتہا_lnum
				     && lnum <= ufunc->uf_سطریں.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_سطریں.ga_ڈیٹا)[lnum - 1];

	    اگر (p == تہی)
		جاری;  // left over from continuation line
	    p = skipwhite(p);
	    اگر (*p == '#')
		توڑو;
	    اگر (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_ڈیٹا))[ga.ga_len++] = p;
	    اگر (STRNCMP(p, "def ", 4) == 0)
		توڑو;
	}
	سطر = ga_concat_لڑیاں(&ga, "  ");
	vim_free(ga.ga_ڈیٹا);
    }
    ورنہ
	سطر = ((char_u **)ufunc->uf_سطریں.ga_ڈیٹا)[iptr->isn_lnum - 1];

    do_debug(سطر == تہی ? (char_u *)"[empty]" : سطر);
    debug_context = تہی;

    اگر (انتہا_lnum > iptr->isn_lnum)
	vim_free(سطر);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    ساکن صحیح
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    اگر (lhs_list->lv_type == تہی || lhs_list->lv_type->tt_member == تہی)
	واپس OK;

    واپس check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    ساکن صحیح
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = ڈھیر_TV_BOT(-2);
    طویل	lidx = 0;
    typval_T	*tv_dest = ڈھیر_TV_BOT(-1);
    صحیح		حالت_2 = OK;
    صحیح		check_rhs_type = FALSE;

    اگر (tv_idx->v_type == VAR_نمبر)
	lidx = (طویل)tv_idx->vval.v_نمبر;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = ڈھیر_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    اگر (dest_type == VAR_OBJECT && tv_dest->vval.v_object == تہی)
    {
	emsg(_(e_using_null_object));
	حالت_2 = FAIL;
    }
    ورنہ اگر (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	اگر (dest_type == VAR_DICT)
	    حالت_2 = do_2string(tv_idx, TRUE, FALSE);
	ورنہ اگر (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    اگر (obj == تہی)
	    {
		emsg(_(e_using_null_object));
		حالت_2 = FAIL;
	    }
	    ورنہ
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		صحیح		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		اگر (m != تہی)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_ڈیٹا)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    اگر ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == تہی ||
			     !class_instance_کا(ufunc->uf_class, cl)))
		    {
			حرف *پیغام = (m->ocm_access == VIM_ACCESS_نجی)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(پیغام, m->ocm_name.string, 0, cl);
			حالت_2 = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    ورنہ اگر (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			حالت_2 = FAIL;
		    ورنہ
			lidx = m_idx;
		}
		ورنہ
		{
		    member_not_ملا_پیغام(cl, VAR_OBJECT, member, 0);
		    حالت_2 = FAIL;
		}
	    }
	}
	ورنہ اگر (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    حالت_2 = FAIL;
	}
	ورنہ اگر (dest_type == VAR_LIST && tv_idx->v_type != VAR_نمبر)
	{
	    emsg(_(e_نمبر_expected));
	    حالت_2 = FAIL;
	}
    }

    اگر (حالت_2 == OK)
    {
	اگر (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    اگر (list == تہی)
	    {
		emsg(_(e_list_not_سیٹ));
		واپس FAIL;
	    }
	    اگر (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    اگر (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), lidx);
		واپس FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    اگر (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		واپس FAIL;

	    اگر (lidx < list->lv_len)
	    {
		listitem_T *li = list_ڈھونڈو(list, lidx);
		اگر (li == تہی)
		    واپس FAIL;
		اگر (error_if_locked(li->li_tv.v_تالا,
					     e_cannot_change_locked_list_شے))
		    واپس FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    ورنہ
	    {
		اگر (error_if_locked(list->lv_تالا, e_cannot_change_locked_list))
		    واپس FAIL;
		// append to list, only fails when out of memory
		اگر (list_append_tv(list, tv) == FAIL)
		    واپس NOTDONE;
		clear_tv(tv);
	    }
	}
	ورنہ اگر (dest_type == VAR_DICT)
	{
	    char_u		*کلید = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    اگر (dict == تہی)
	    {
		emsg(_(e_dictionary_not_سیٹ));
		واپس FAIL;
	    }
	    اگر (کلید == تہی)
		کلید = (char_u *)"";
	    di = dict_ڈھونڈو(dict, کلید, -1);
	    اگر (di != تہی)
	    {
		اگر (error_if_locked(di->di_tv.v_تالا,
						    e_cannot_change_dict_شے))
		    واپس FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    ورنہ
	    {
		اگر (error_if_locked(dict->dv_تالا, e_cannot_change_dict))
		    واپس FAIL;
		// add to dict, only fails when out of memory
		اگر (dict_جوڑو_tv(dict, (حرف *)کلید, tv) == FAIL)
		    واپس NOTDONE;
		clear_tv(tv);
	    }
	}
	ورنہ اگر (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    صحیح		    error = FALSE;
	    صحیح		    len;

	    اگر (blob == تہی)
	    {
		emsg(_(e_blob_not_سیٹ));
		واپس FAIL;
	    }
	    len = blob_len(blob);
	    اگر (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    اگر (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_اشاریہ_برآمد_کا_range_nr), lidx);
		واپس FAIL;
	    }
	    اگر (قدر_check_تالا(blob->bv_تالا, (char_u *)"blob", FALSE))
		واپس FAIL;
	    nr = tv_get_نمبر_chk(tv, &error);
	    اگر (error)
		واپس FAIL;
	    اگر (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_قدر_for_blob_nr), nr);
		واپس FAIL;
	    }
	    blob_سیٹ_append(blob, lidx, nr);
	}
	ورنہ اگر (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    اگر (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		اگر (itf != تہی)
		    // convert interface member index to class member index
		    lidx = object_اشاریہ_from_itf_اشاریہ(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    ورنہ
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	ورنہ اگر (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    حالت_2 = FAIL;
	}
	ورنہ
	{
	    حالت_2 = FAIL;
	    semsg(_(e_cannot_اشاریہ_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ڈھیر.ga_len -= 3;
    اگر (حالت_2 == FAIL)
    {
	clear_tv(tv);
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Store a value in a list or blob range.
 */
    ساکن صحیح
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = ڈھیر_TV_BOT(-3);
    typval_T	*tv_idx2 = ڈھیر_TV_BOT(-2);
    typval_T	*tv_dest = ڈھیر_TV_BOT(-1);
    صحیح		حالت_2 = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = ڈھیر_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    اگر (tv_dest->v_type == VAR_LIST)
    {
	طویل	    n1;
	طویل	    n2;
	listitem_T  *li1;

	n1 = (طویل)tv_get_نمبر_chk(tv_idx1, تہی);
	اگر (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_نمبر == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	ورنہ
	    n2 = (طویل)tv_get_نمبر_chk(tv_idx2, تہی);

	li1 = check_range_اشاریہ_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	اگر (li1 == تہی)
	    حالت_2 = FAIL;
	ورنہ
	{
	    حالت_2 = check_range_اشاریہ_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    اگر (حالت_2 != FAIL)
		حالت_2 = list_تفویض_range(
			tv_dest->vval.v_list,
			tv->vval.v_list,
			n1,
			n2,
			tv_idx2->v_type == VAR_SPECIAL,
			(char_u *)"=",
			(char_u *)"[unknown]");
	}
    }
    ورنہ اگر (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	طویل	    bloblen;

	n1 = tv_get_نمبر_chk(tv_idx1, تہی);
	اگر (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_نمبر == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	ورنہ
	    n2 = tv_get_نمبر_chk(tv_idx2, تہی);
	bloblen = blob_len(tv_dest->vval.v_blob);

	اگر (check_blob_اشاریہ(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    حالت_2 = FAIL;
	ورنہ
	    حالت_2 = blob_سیٹ_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    ورنہ
    {
	حالت_2 = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ڈھیر.ga_len -= 4;
    clear_tv(tv);

    واپس حالت_2;
}

/*
 * Unlet item in list or dict variable.
 */
    ساکن صحیح
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = ڈھیر_TV_BOT(-2);
    typval_T	*tv_dest = ڈھیر_TV_BOT(-1);
    صحیح		حالت_2 = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    اگر (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	اگر (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_نمبر)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    حالت_2 = FAIL;
	}
	ورنہ
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*کلید;
	    dictitem_T  *di = تہی;

	    اگر (d != تہی && قدر_check_تالا(
				      d->dv_تالا, تہی, FALSE))
		حالت_2 = FAIL;
	    ورنہ
	    {
		اگر (tv_idx->v_type == VAR_STRING)
		{
		    کلید = tv_idx->vval.v_string;
		    اگر (کلید == تہی)
			کلید = (char_u *)"";
		}
		ورنہ
		{
		    کلید = tv_get_string(tv_idx);
		}
		اگر (d != تہی)
		    di = dict_ڈھونڈو(d, کلید, (صحیح)STRLEN(کلید));
		اگر (di == تہی)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_کلید_not_present_اندر_dictionary_str), کلید);
		    حالت_2 = FAIL;
		}
		ورنہ اگر (var_check_fixed(di->di_flags,
						   تہی, FALSE)
			|| var_check_ro(di->di_flags,
						  تہی, FALSE))
		    حالت_2 = FAIL;
		ورنہ
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    ورنہ اگر (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	اگر (check_for_نمبر(tv_idx) == FAIL)
	{
	    حالت_2 = FAIL;
	}
	ورنہ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    طویل	n = (طویل)tv_idx->vval.v_نمبر;

	    اگر (l != تہی && قدر_check_تالا(
				      l->lv_تالا, تہی, FALSE))
		حالت_2 = FAIL;
	    ورنہ
	    {
		listitem_T	*li = list_ڈھونڈو(l, n);

		اگر (li == تہی)
		{
		    semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), n);
		    حالت_2 = FAIL;
		}
		ورنہ
		    listitem_remove(l, li);
	    }
	}
    }
    ورنہ
    {
	حالت_2 = FAIL;
	semsg(_(e_cannot_اشاریہ_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ڈھیر.ga_len -= 2;

    واپس حالت_2;
}

/*
 * Unlet a range of items in a list variable.
 */
    ساکن صحیح
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = ڈھیر_TV_BOT(-3);
    typval_T	*tv_idx2 = ڈھیر_TV_BOT(-2);
    typval_T	*tv_dest = ڈھیر_TV_BOT(-1);
    صحیح		حالت_2 = OK;

    اگر (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	اگر (check_for_نمبر(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_نمبر(tv_idx2) == FAIL))
	{
	    حالت_2 = FAIL;
	}
	ورنہ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    طویل	n1 = (طویل)tv_idx1->vval.v_نمبر;
	    طویل	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (طویل)tv_idx2->vval.v_نمبر;
	    listitem_T	*li;

	    li = list_ڈھونڈو_اشاریہ(l, &n1);
	    اگر (li == تہی)
	    {
		semsg(_(e_list_اشاریہ_برآمد_کا_range_nr),
						 (طویل)tv_idx1->vval.v_نمبر);
		حالت_2 = FAIL;
	    }
	    ورنہ
	    {
		اگر (n1 < 0)
		    n1 = list_idx_کا_شے(l, li);
		اگر (n2 < 0)
		{
		    listitem_T *li2 = list_ڈھونڈو(l, n2);

		    اگر (li2 == تہی)
		    {
			semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), n2);
			حالت_2 = FAIL;
		    }
		    ورنہ
			n2 = list_idx_کا_شے(l, li2);
		}
		اگر (حالت_2 != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_اشاریہ_برآمد_کا_range_nr), n2);
		    حالت_2 = FAIL;
		}
		اگر (حالت_2 != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    ورنہ
    {
	حالت_2 = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_اشاریہ_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ڈھیر.ga_len -= 3;

    واپس حالت_2;
}

/*
 * Top of a for loop.
 */
    ساکن صحیح
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    صحیح		jump = FALSE;
    typval_T	*ltv = ڈھیر_TV_BOT(-1);
    typval_T	*idxtv =
		   ڈھیر_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
	واپس FAIL;
    اگر (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_نمبر;
	اگر (list == تہی
		       || idxtv->vval.v_نمبر >= list->lv_len)
	{
	    jump = TRUE;
	}
	ورنہ اگر (list->lv_پہلا == &range_list_شے)
	{
	    // non-materialized range() list
	    tv = ڈھیر_TV_BOT(0);
	    tv->v_type = VAR_نمبر;
	    tv->v_تالا = 0;
	    tv->vval.v_نمبر = list_ڈھونڈو_nr(
			     list, idxtv->vval.v_نمبر, تہی);
	    ++ectx->ec_ڈھیر.ga_len;
	}
	ورنہ
	{
	    listitem_T *li = list_ڈھونڈو(list,
					 idxtv->vval.v_نمبر);
	    اگر (li == تہی)
		واپس FAIL;
	    copy_tv(&li->li_tv, ڈھیر_TV_BOT(0));
	    ++ectx->ec_ڈھیر.ga_len;
	}
    }
    ورنہ اگر (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_نمبر;
	اگر (tuple == تہی || idxtv->vval.v_نمبر >= TUPLE_LEN(tuple))
	    jump = TRUE;
	ورنہ
	{
	    copy_tv(TUPLE_شے(tuple, idxtv->vval.v_نمبر), ڈھیر_TV_BOT(0));
	    ++ectx->ec_ڈھیر.ga_len;
	}
    }
    ورنہ اگر (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_نمبر;
	اگر (str == تہی || str[idxtv->vval.v_نمبر] == NUL)
	{
	    jump = TRUE;
	}
	ورنہ
	{
	    صحیح	clen = mb_ptr2len(str + idxtv->vval.v_نمبر);

	    // Push the next character from the string.
	    tv = ڈھیر_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_نمبر, clen);
	    ++ectx->ec_ڈھیر.ga_len;
	    idxtv->vval.v_نمبر += clen - 1;
	}
    }
    ورنہ اگر (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	اگر (idxtv->vval.v_نمبر == -1 && blob != تہی)
	{
	    ltv->v_تالا = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_نمبر;
	اگر (blob == تہی || idxtv->vval.v_نمبر >= blob_len(blob))
	{
	    jump = TRUE;
	}
	ورنہ
	{
	    // Push the next byte from the blob.
	    tv = ڈھیر_TV_BOT(0);
	    tv->v_type = VAR_نمبر;
	    tv->vval.v_نمبر = blob_get(blob,
					 idxtv->vval.v_نمبر);
	    ++ectx->ec_ڈھیر.ga_len;
	}
    }
    ورنہ
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	واپس FAIL;
    }

    اگر (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_انتہا;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    ورنہ
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = ڈھیر_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_نمبر = ectx->ec_funcrefs.ga_len;
    }

    واپس OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
ساکن loopvars_T *پہلا_loopvars = تہی;

    ساکن خالی
جوڑو_loopvars_مقصد_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    اگر (پہلا_loopvars != تہی)
	پہلا_loopvars->lvs_پچھلا = loopvars;
    loopvars->lvs_اگلا = پہلا_loopvars;
    loopvars->lvs_پچھلا = تہی;
    پہلا_loopvars = loopvars;
}

    ساکن خالی
remove_loopvars_from_list(loopvars_T *loopvars)
{
    اگر (loopvars->lvs_پچھلا == تہی)
	پہلا_loopvars = loopvars->lvs_اگلا;
    ورنہ
	loopvars->lvs_پچھلا->lvs_اگلا = loopvars->lvs_اگلا;
    اگر (loopvars->lvs_اگلا != تہی)
	loopvars->lvs_اگلا->lvs_پچھلا = loopvars->lvs_پچھلا;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    ساکن صحیح
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = ڈھیر_TV_VAR(endloop->انتہا_funcref_idx);
    صحیح		پچھلا_احاطہ_گنتی = tv_refcount->vval.v_نمبر;
    صحیح		گہرائی = endloop->انتہا_گہرائی;
    garray_T	*gap = &ectx->ec_funcrefs;
    صحیح		احاطہ_اندر_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *ڈھیر;
    صحیح		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    برائے (idx = پچھلا_احاطہ_گنتی; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_ڈیٹا)[idx];

	اگر (pt->pt_refcount > 1 && pt->pt_loopvars[گہرائی] == تہی)
	{
	    صحیح refcount = pt->pt_refcount;
	    صحیح i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    برائے (i = 0; i < endloop->انتہا_var_گنتی; ++i)
	    {
		typval_T *stv = ڈھیر_TV_VAR(endloop->انتہا_var_idx + i);

		اگر (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    اگر (refcount > 1)
	    {
		احاطہ_اندر_use = TRUE;
		توڑو;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    اگر (!احاطہ_اندر_use)
	واپس OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    اگر (loopvars == تہی)
	واپس FAIL;

    loopvars->lvs_ga.ga_len = endloop->انتہا_var_گنتی;
    ڈھیر = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_ڈیٹا = ڈھیر;
    اگر (ڈھیر == تہی)
    {
	vim_free(loopvars);
	واپس FAIL;
    }
    جوڑو_loopvars_مقصد_list(loopvars);

    // Move the variable values.
    برائے (idx = 0; idx < endloop->انتہا_var_گنتی; ++idx)
    {
	typval_T *tv = ڈھیر_TV_VAR(endloop->انتہا_var_idx + idx);

	*(ڈھیر + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    برائے (idx = پچھلا_احاطہ_گنتی; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_ڈیٹا)[idx];

	اگر (pt->pt_refcount > 1 && pt->pt_loopvars[گہرائی] == تہی)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[گہرائی] = loopvars;

	    pt->pt_outer.برآمد_loop[گہرائی].ڈھیر = &loopvars->lvs_ga;
	    pt->pt_outer.برآمد_loop[گہرائی].var_idx -=
		  ectx->ec_چوکھٹا_idx + ڈھیر_چوکھٹا_سائز + endloop->انتہا_var_idx;
	}
    }

    واپس OK;
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
    صحیح		    مکمل = 0;
	typval_T	*ڈھیر = gap->ga_ڈیٹا;

    اگر (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	واپس FALSE;
    برائے (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_ڈیٹا) + i;

	اگر (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != تہی
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    صحیح	    گہرائی;

	    برائے (گہرائی = 0; گہرائی < MAX_LOOP_گہرائی; ++گہرائی)
		اگر (tv->vval.v_partial->pt_loopvars[گہرائی] == loopvars)
		    ++مکمل;
	}
    }
    اگر (مکمل != loopvars->lvs_min_refcount)
	واپس FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    ڈھیر = gap->ga_ڈیٹا;
    برائے (i = 0; i < gap->ga_len; ++i)
	clear_tv(ڈھیر + i);
    vim_free(ڈھیر);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    واپس TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    صحیح
سیٹ_ref_اندر_loopvars(صحیح copyID)
{
    loopvars_T *loopvars;

    برائے (loopvars = پہلا_loopvars; loopvars != تہی;
						 loopvars = loopvars->lvs_اگلا)
    {
	typval_T    *ڈھیر = loopvars->lvs_ga.ga_ڈیٹا;
	صحیح	    i;

	برائے (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    اگر (سیٹ_ref_اندر_شے(ڈھیر + i, copyID, تہی, تہی, تہی))
		واپس TRUE;  // abort
    }
    واپس FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    ساکن صحیح
بھرو_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = تہی;
    hashtab_T	*ht = تہی;
    حرف	namespace;

    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
	واپس NOTDONE;
    انتخاب (isn_type)
    {
	حالت ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    توڑو;
	حالت ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    توڑو;
	حالت ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    توڑو;
	حالت ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    توڑو;
	طےشدہ:  // Cannot reach here
	    واپس NOTDONE;
    }
    di = ڈھونڈو_var_اندر_ht(ht, 0, iptr->isn_arg.string, TRUE);

    اگر (di == تہی)
    {
	اگر (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = ڈھونڈو_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    اگر (ufunc != تہی)
	    {
		typval_T    *tv = ڈھیر_TV_BOT(0);

		++ectx->ec_ڈھیر.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		اگر (tv->vval.v_string == تہی)
		    واپس FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		واپس OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	اگر (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != تہی)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_شے_not_ملا_اندر_script_str), iptr->isn_arg.string);
	ورنہ
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = ڈھیر_TV_BOT(0);
	tv->v_type = VAR_نمبر;
	tv->v_تالا = 0;
	tv->vval.v_نمبر = 0;
	++ectx->ec_ڈھیر.ga_len;
	واپس FAIL;
    }
    ورنہ
    {
	copy_tv(&di->di_tv, ڈھیر_TV_BOT(0));
	++ectx->ec_ڈھیر.ga_len;
    }
    واپس OK;
}


    ساکن خالی
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, حجم(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    حرف *tofree = تہی;
    حرف *typename = type_name(type, &tofree);
    semsg(_(e_object_required_ملا_str), typename);
    vim_free(tofree);
    clear_type_list(&type_list);
}

/*
 * Accessing the variable or method of an object or a class stored in a
 * variable of type "any".
 * Returns OK if the member variable is present.
 * Returns FAIL if the variable is not found.
 */
    ساکن صحیح
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    صحیح		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = تہی;
    typval_T	mtv;

    اگر (is_object)
    {
	obj = tv->vval.v_object;
	اگر (obj == تہی)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    واپس FAIL;
	}
	tv_cl = obj->obj_class;
    }
    ورنہ
    {
	tv_cl = tv->vval.v_class;
	اگر (tv_cl == تہی)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    واپس FAIL;
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
	واپس OK;
    }

    اگر (did_emsg != save_did_emsg)
	واپس FAIL;

    // could be a class or instance method
    ufunc_T	*oc_طریقہ;
    صحیح		oc_طریقہ_idx;

    oc_طریقہ = طریقہ_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_طریقہ_idx);
    اگر (oc_طریقہ == تہی)
    {
	حرف	*پیغام;

	SOURCING_LNUM = iptr->isn_lnum;
	اگر (is_object)
	    پیغام = e_variable_not_ملا_on_object_str_str;
	ورنہ
	    پیغام = e_class_variable_str_not_ملا_اندر_class_str;
	semsg(_(پیغام), iptr->isn_arg.string, tv_cl->class_name.string);
	واپس FAIL;
    }

    // Protected methods are not accessible outside the class
    اگر (*oc_طریقہ->uf_name == '_'
			&& !class_instance_کا(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_طریقہ_str), oc_طریقہ->uf_name);
	واپس FAIL;
    }

    // Create a partial for the instance or class method
    اگر (obj_طریقہ_مقصد_partial_tv(is_object ? obj : تہی, oc_طریقہ, tv)
								== FAIL)
	واپس FAIL;

    واپس OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    ساکن خالی
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, صحیح fixindent)
{
    صحیح		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*اظہاریہ = تہی;
    صحیح		dir = FORWARD;

    اگر (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = ڈھیر_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_نمبر;
	اگر (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_ڈھیر.ga_len;
    }
    ورنہ اگر (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    ورنہ اگر (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	اگر (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    اگر (regname == '=')
    {
	tv = ڈھیر_TV_BOT(-1);
	اگر (tv->v_type == VAR_STRING)
	    اظہاریہ = tv->vval.v_string;
	ورنہ
	{
	    اظہاریہ = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_ڈھیر.ga_len;
    }
    check_cursor();

    اگر (fixindent)
	do_put(regname, اظہاریہ, dir, 1L, PUT_سطر|PUT_CURSLINE|PUT_FIXINDENT);
    ورنہ
	do_put(regname, اظہاریہ, dir, 1L, PUT_سطر|PUT_CURSLINE);
    vim_free(اظہاریہ);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    ساکن صحیح
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    صحیح		گنتی = iptr->isn_arg.unpack.unp_گنتی;
    صحیح		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    صحیح		i;

    l = tv->vval.v_list;
    اگر (l == تہی
	    || l->lv_len < (semicolon ? گنتی - 1 : گنتی))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_قدر_does_not_have_enough_اشیاء));
	واپس EXEC_FAIL;
    }
    ورنہ اگر (!semicolon && l->lv_len > گنتی)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_قدر_has_مزید_اشیاء_than_targets));
	واپس EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, گنتی - 1))
	واپس EXEC_مکمل;
    ectx->ec_ڈھیر.ga_len += گنتی - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    اگر (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_اشیاء(l->lv_len - گنتی + 1);

	اگر (rem_list == تہی)
	    واپس EXEC_مکمل;
	tv = ڈھیر_TV_BOT(-گنتی);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_تالا = 0;
	li = l->lv_پہلا;
	برائے (i = 0; i < گنتی - 1; ++i)
	    li = li->li_اگلا;
	برائے (i = 0; li != تہی; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_سیٹ_شے(rem_list, i, &tvcopy);
	    li = li->li_اگلا;
	}
	--گنتی;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_پہلا;
    برائے (i = 0; i < گنتی; ++i)
    {
	tv = ڈھیر_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_اگلا;
    }

    list_unref(l);

    واپس EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    ساکن صحیح
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    صحیح		گنتی = iptr->isn_arg.unpack.unp_گنتی;
    صحیح		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    صحیح		i;

    tuple = tv->vval.v_tuple;
    اگر (tuple == تہی
	    || TUPLE_LEN(tuple) < (semicolon ? گنتی - 1 : گنتی))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_مزید_targets_than_tuple_اشیاء));
	واپس EXEC_FAIL;
    }
    ورنہ اگر (!semicolon && TUPLE_LEN(tuple) > گنتی)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_اشیاء));
	واپس EXEC_FAIL;
    }

    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, گنتی - 1))
	واپس EXEC_مکمل;
    ectx->ec_ڈھیر.ga_len += گنتی - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    اگر (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_اشیاء(TUPLE_LEN(tuple) - گنتی + 1);

	اگر (rem_list == تہی)
	    واپس EXEC_مکمل;
	tv = ڈھیر_TV_BOT(-گنتی);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_تالا = 0;
	برائے (i = گنتی - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_شے(tuple, i), &tvcopy);
	    list_سیٹ_شے(rem_list, i - (گنتی - 1), &tvcopy);
	}
	--گنتی;
    }

    // Produce the values in reverse order, first item last.
    برائے (i = 0; i < گنتی; ++i)
    {
	tv = ڈھیر_TV_BOT(-i - 1);
	copy_tv(TUPLE_شے(tuple, i), tv);
    }

    tuple_unref(tuple);

    واپس EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    ساکن صحیح
exec_instructions(ectx_T *ectx)
{
    صحیح		ret = FAIL;
    صحیح		save_trylevel_at_آغاز = ectx->ec_trylevel_at_آغاز;
    صحیح		dict_ڈھیر_len_at_آغاز = dict_ڈھیر.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_آغاز = trylevel;

    برائے (;;)
    {
	ساکن صحیح  breakcheck_گنتی = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	اگر (unlikely(++breakcheck_گنتی >= 100))
	{
	    سطر_breakcheck();
	    breakcheck_گنتی = 0;
	}
	اگر (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    اگر (throw_exception("Vim:Interrupt", ET_مداخلت, تہی) == FAIL)
		جاؤ theend;
	    did_throw = TRUE;
	}

	اگر (unlikely(did_emsg && پیغام_list != تہی && *پیغام_list != تہی))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    اگر (throw_exception(*پیغام_list, ET_ERROR, تہی) == FAIL)
		جاؤ theend;
	    did_throw = TRUE;
	    *پیغام_list = تہی;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    اگر (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_ڈیٹا)
							+ trystack->ga_len - 1;
		اگر (trycmd->tcd_چوکھٹا_idx == ectx->ec_چوکھٹا_idx)
		{
		    اگر (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			اختتام_exception(caught_ڈھیر);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	اگر (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = تہی;
	    صحیح		اشاریہ = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    جب (اشاریہ > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_ڈیٹا) + اشاریہ - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		اگر (!trycmd->tcd_اندر_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_چوکھٹا_idx == ectx->ec_چوکھٹا_idx)
		    توڑو;
		// In the catch and finally block of this try we have to go up
		// one level.
		--اشاریہ;
		trycmd = تہی;
	    }
	    اگر (trycmd != تہی && trycmd->tcd_چوکھٹا_idx == ectx->ec_چوکھٹا_idx)
	    {
		اگر (trycmd->tcd_اندر_catch)
		{
		    اگر (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    ورنہ
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		ورنہ
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_اندر_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    ورنہ
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    جاؤ theend;
		tv = ڈھیر_TV_BOT(0);
		tv->v_type = VAR_نمبر;
		tv->vval.v_نمبر = 0;
		++ectx->ec_ڈھیر.ga_len;
		اگر (ectx->ec_چوکھٹا_idx == ectx->ec_initial_چوکھٹا_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    اگر (handle_احاطہ_اندر_use(ectx, FALSE) == FAIL)
			جاؤ theend;
		    جاؤ مکمل;
		}

		اگر (func_return(ectx) == FAIL)
		    جاؤ theend;
	    }
	    جاری;
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
		tv = ڈھیر_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		توڑو;

	    // execute Ex command line
	    حالت ISN_EXEC:
		اگر (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    جاؤ on_error;
		توڑو;

	    // execute Ex command line split at NL characters.
	    حالت ISN_EXEC_بانٹو:
		{
		    ماخذ_cookie_T cookie;
		    char_u	    *سطر;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_میدان(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    سطر = get_بانٹو_sourceline(0, &cookie, 0, 0);
		    اگر (do_cmdline(سطر,
				get_بانٹو_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(سطر);
			جاؤ on_error;
		    }
		    vim_free(سطر);
		}
		توڑو;

	    // execute Ex command line that is only a range
	    حالت ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    حرف	*error = تہی;

		    CLEAR_میدان(ea);
		    ea.cmdidx = CMD_سائز;
		    ea.addr_type = ADDR_سطریں;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_پتہ(&ea, &error, FALSE);
		    اگر (ea.cmd == تہی)
			جاؤ on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    اگر (error != تہی)
		    {
			emsg(error);
			جاؤ on_error;
		    }
		}
		توڑو;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    حالت ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    صحیح	    res;
		    صحیح	    save_flags = cmdmod.cmod_flags;

		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    tv = ڈھیر_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, تہی, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    اگر (res == FAIL)
			جاؤ on_error;
		    ++ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    // push typeval VAR_INSTR with instructions to be executed
	    حالت ISN_INSTR:
		{
		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    tv = ڈھیر_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    اگر (tv->vval.v_instr == تہی)
			جاؤ on_error;
		    ++ectx->ec_ڈھیر.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		توڑو;

	    حالت ISN_ماخذ:
		{
		    صحیح notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (may_بھرو_script((صحیح)iptr->isn_arg.نمبر, &notused)
								       == FAIL)
			جاؤ on_error;
		}
		توڑو;

	    // execute :substitute with an expression
	    حالت ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    ماخذ_cookie_T	cookie;
		    ساخت subs_اظہاریہ_S	*save_instr = substitute_instr;
		    ساخت subs_اظہاریہ_S	subs_instr;
		    صحیح			res;

		    subs_instr.subs_ectx = ectx;
		    subs_instr.subs_instr = subs->subs_instr;
		    subs_instr.subs_حالت = OK;
		    substitute_instr = &subs_instr;

		    SOURCING_LNUM = iptr->isn_lnum;
		    // This is very much like ISN_EXEC
		    CLEAR_میدان(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    res = do_cmdline(subs->subs_cmd,
				getsourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
		    substitute_instr = save_instr;

		    اگر (res == FAIL || did_emsg
					     || subs_instr.subs_حالت == FAIL)
			جاؤ on_error;
		}
		توڑو;

	    حالت ISN_اختتام:
		جاؤ مکمل;

	    حالت ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		اگر (alloc_redir_lval() == FAIL)
		    جاؤ on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		توڑو;

	    حالت ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    {
			vim_free(res);
			جاؤ theend;
		    }
		    tv = ڈھیر_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    حالت ISN_CEXPR_AUCMD:
#اگر_تعریف_شدہ FEAT_QUICKFIX
		force_abort = TRUE;
		اگر (trigger_cexpr_autocmd(iptr->isn_arg.نمبر) == FAIL)
		    جاؤ on_error;
		force_abort = FALSE;
#اگر_ختم
		توڑو;

	    حالت ISN_CEXPR_CORE:
#اگر_تعریف_شدہ FEAT_QUICKFIX
		{
		    exarg_T ea;
		    صحیح	    res;

		    CLEAR_میدان(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_ڈھیر.ga_len;
		    tv = ڈھیر_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    اگر (res == FAIL)
			جاؤ on_error;
		}
#اگر_ختم
		توڑو;

	    // execute Ex command from pieces on the stack
	    حالت ISN_EXECCONCAT:
		{
		    صحیح	    گنتی = iptr->isn_arg.نمبر;
		    size_t  len = 0;
		    صحیح	    pass;
		    صحیح	    i;
		    char_u  *cmd = تہی;
		    char_u  *str;

		    برائے (pass = 1; pass <= 2; ++pass)
		    {
			برائے (i = 0; i < گنتی; ++i)
			{
			    tv = ڈھیر_TV_BOT(i - گنتی);
			    str = tv->vval.v_string;
			    اگر (str != تہی && *str != NUL)
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
			    اگر (unlikely(cmd == تہی))
				جاؤ theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		توڑو;

	    // execute :echo {string} ...
	    حالت ISN_ECHO:
		{
		    صحیح گنتی = iptr->isn_arg.echo.echo_گنتی;
		    صحیح	atstart = TRUE;
		    صحیح needclr = TRUE;
		    صحیح	idx;

		    برائے (idx = 0; idx < گنتی; ++idx)
		    {
			tv = ڈھیر_TV_BOT(idx - گنتی);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    اگر (needclr)
			پیغام_clr_eos();
		    ectx->ec_ڈھیر.ga_len -= گنتی;
		}
		توڑو;

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
		    صحیح		گنتی;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    صحیح		len;
		    صحیح		failed = FALSE;
		    صحیح		idx;

		    اگر (iptr->isn_type == ISN_ECHOWINDOW)
			گنتی = iptr->isn_arg.echowin.ewin_گنتی;
		    ورنہ
			گنتی = iptr->isn_arg.نمبر;
		    ga_init2(&ga, 1, 80);
		    برائے (idx = 0; idx < گنتی; ++idx)
		    {
			tv = ڈھیر_TV_BOT(idx - گنتی);
			اگر (iptr->isn_type == ISN_EXECUTE)
			{
			    اگر (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_کام)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_قدر_as_string_str),
						    vartype_name(tv->v_type));
				توڑو;
			    }
			    ورنہ
				p = tv_get_string_buf(tv, buf);
			}
			ورنہ
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (صحیح)STRLEN(p);
			اگر (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			ورنہ
			{
			    اگر (ga.ga_len > 0)
				((char_u *)(ga.ga_ڈیٹا))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_ڈیٹا) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_ڈھیر.ga_len -= گنتی;
		    اگر (failed)
		    {
			ga_clear(&ga);
			جاؤ on_error;
		    }

		    اگر (ga.ga_ڈیٹا != تہی)
		    {
			اگر (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_ڈیٹا);
			    اگر (did_emsg)
			    {
				ga_clear(&ga);
				جاؤ on_error;
			    }
			}
			ورنہ
			{
			    پیغام_sb_eol();
			    اگر (iptr->isn_type == ISN_ECHOMSG)
			    {
				پیغام_attr(ga.ga_ڈیٹا, echo_attr);
				برآمد_flush();
			    }
#اگر_تعریف_شدہ HAS_پیغام_کھڑکی
			    ورنہ اگر (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				آغاز_echowindow(
					      iptr->isn_arg.echowin.ewin_وقت);
				پیغام_attr(ga.ga_ڈیٹا, echo_attr);
				انتہا_echowindow();
			    }
#اگر_ختم
			    ورنہ اگر (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_ڈیٹا, (صحیح)STRLEN(ga.ga_ڈیٹا),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    ورنہ
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_ڈیٹا);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		توڑو;

	    // load local variable or argument
	    حالت ISN_بھرو:
		اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    جاؤ theend;
		tv = ڈھیر_TV_VAR(iptr->isn_arg.نمبر);
		اگر (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    ڈھیر_TV_BOT(0)->v_type = VAR_SPECIAL;
		    ڈھیر_TV_BOT(0)->vval.v_نمبر = VVAL_NONE;
		}
		ورنہ
		    copy_tv(tv, ڈھیر_TV_BOT(0));
		++ectx->ec_ڈھیر.ga_len;
		توڑو;

	    // load v: variable
	    حالت ISN_LOADV:
		اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    جاؤ theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.نمبر), ڈھیر_TV_BOT(0));
		++ectx->ec_ڈھیر.ga_len;
		توڑو;

	    // load s: variable in Vim9 script
	    حالت ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    اگر (sv == تہی)
			جاؤ theend;
		    allocate_if_null(sv);
		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    copy_tv(sv->sv_tv, ڈھیر_TV_BOT(0));
		    ++ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    // load s: variable in old script or autoload import
	    حالت ISN_LOADS:
	    حالت ISN_LOADEXPORT:
		{
		    صحیح		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = ڈھونڈو_var_اندر_ht(ht, 0, name, TRUE);

		    اگر (di == تہی)
		    {
			ufunc_T	*ufunc = تہی;

			اگر (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (خالی)ڈھونڈو_exported(sid, name, &ufunc, &type,
							 تہی, تہی, FALSE);
			}
			اگر (ufunc == تہی)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    جاؤ on_error;
			}
			اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			    جاؤ theend;
			tv = ڈھیر_TV_BOT(0);
			tv->v_تالا = 0;
			++ectx->ec_ڈھیر.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    ورنہ
		    {
			اگر (iptr->isn_type == ISN_LOADEXPORT)
			{
			    صحیح idx = get_script_شے_idx(sid, name, 0,
								   تہی, تہی);
			    svar_T	*sv;

			    اگر (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_شے(sid)
						  ->sn_var_vals.ga_ڈیٹا) + idx;
				اگر ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_شے_not_exported_اندر_script_str),
									 name);
				    جاؤ on_error;
				}
			    }
			}
			اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			    جاؤ theend;
			copy_tv(&di->di_tv, ڈھیر_TV_BOT(0));
			++ectx->ec_ڈھیر.ga_len;
		    }
		}
		توڑو;

	    // load g:/b:/w:/t: variable
	    حالت ISN_LOADG:
	    حالت ISN_LOADB:
	    حالت ISN_LOADW:
	    حالت ISN_LOADT:
		{
		    صحیح res = بھرو_namespace_var(ectx, iptr->isn_type, iptr);

		    اگر (res == NOTDONE)
			جاؤ theend;
		    اگر (res == FAIL)
			جاؤ on_error;
		}

		توڑو;

	    // load autoload variable
	    حالت ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (eval_variable(name, (صحیح)STRLEN(name), 0,
			      ڈھیر_TV_BOT(0), تہی, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = ڈھیر_TV_BOT(0);
			tv->v_type = VAR_نمبر;
			tv->v_تالا = 0;
			tv->vval.v_نمبر = 0;
			++ectx->ec_ڈھیر.ga_len;
			جاؤ on_error;
		    }
		    ++ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    // load g:/b:/w:/t: namespace
	    حالت ISN_LOADGDICT:
	    حالت ISN_LOADBDICT:
	    حالت ISN_LOADWDICT:
	    حالت ISN_LOADTDICT:
		{
		    dict_T *d = تہی;

		    انتخاب (iptr->isn_type)
		    {
			حالت ISN_LOADGDICT: d = get_globvar_dict(); توڑو;
			حالت ISN_LOADBDICT: d = curbuf->b_vars; توڑو;
			حالت ISN_LOADWDICT: d = curwin->w_vars; توڑو;
			حالت ISN_LOADTDICT: d = curtab->tp_vars; توڑو;
			طےشدہ:  // Cannot reach here
			    جاؤ theend;
		    }
		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    tv = ڈھیر_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_تالا = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    // load &option
	    حالت ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    اگر (eval_option(&name, &optval, TRUE) == FAIL)
			جاؤ theend;
		    *ڈھیر_TV_BOT(0) = optval;
		    ++ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    // load $ENV
	    حالت ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    // name is always valid, checked when compiling
		    (خالی)eval_env_var(&name, &optval, TRUE);
		    *ڈھیر_TV_BOT(0) = optval;
		    ++ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    // load @register
	    حالت ISN_LOADREG:
		اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    جاؤ theend;
		tv = ڈھیر_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_تالا = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.نمبر, GREG_اظہاریہ_SRC);
		++ectx->ec_ڈھیر.ga_len;
		توڑو;

	    // store local variable
	    حالت ISN_STORE:
		--ectx->ec_ڈھیر.ga_len;
		tv = ڈھیر_TV_VAR(iptr->isn_arg.نمبر);
		اگر (check_typval_is_قدر(ڈھیر_TV_BOT(0)) == FAIL
			|| ڈھیر_TV_BOT(0)->v_type == VAR_VOID)
		{
		    اگر (ڈھیر_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_قدر));
		    clear_tv(ڈھیر_TV_BOT(0));
		    جاؤ on_error;
		}
		clear_tv(tv);
		*tv = *ڈھیر_TV_BOT(0);
		توڑو;

	    // store s: variable in old script or autoload import
	    حالت ISN_STORES:
	    حالت ISN_STOREEXPORT:
		{
		    صحیح		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = تہی;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    اگر (iptr->isn_type == ISN_STOREEXPORT)
			di = ڈھونڈو_var_autoload_سابقہ(name, sid, تہی, تہی);
		    // Then look for a variable in the script's variables.
		    اگر (di == تہی)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = ڈھونڈو_var_اندر_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_ڈھیر.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (di == تہی)
		    {
			اگر (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(ڈھیر_TV_BOT(0));
			    جاؤ on_error;
			}
			store_var(name, ڈھیر_TV_BOT(0));
		    }
		    ورنہ
		    {
			اگر (iptr->isn_type == ISN_STOREEXPORT)
			{
			    صحیح idx = get_script_شے_idx(sid, name, 0,
								   تہی, تہی);

			    // can this ever fail?
			    اگر (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_شے(sid)
						  ->sn_var_vals.ga_ڈیٹا) + idx;

				اگر ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_شے_not_exported_اندر_script_str),
									 name);
				    clear_tv(ڈھیر_TV_BOT(0));
				    جاؤ on_error;
				}
			    }
			}
			اگر (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(ڈھیر_TV_BOT(0));
			    جاؤ on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *ڈھیر_TV_BOT(0);
		    }
		}
		توڑو;

	    // store script-local variable in Vim9 script
	    حالت ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    اگر (sv == تہی)
			جاؤ theend;
		    --ectx->ec_ڈھیر.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (قدر_check_تالا(sv->sv_tv->v_تالا, sv->sv_name, FALSE))
		    {
			clear_tv(ڈھیر_TV_BOT(0));
			جاؤ on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *ڈھیر_TV_BOT(0);
		}
		توڑو;

	    // store option
	    حالت ISN_STOREOPT:
	    حالت ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    صحیح		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    طویل	n = 0;
		    char_u	*s = تہی;
		    حرف	*پیغام;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = تہی;

		    --ectx->ec_ڈھیر.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ڈھیر_TV_BOT(0);
		    اگر (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			اگر (s == تہی)
			    s = (char_u *)"";
		    }
		    ورنہ اگر (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			اگر (s == تہی || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    جاؤ on_error;
			}
		    }
		    ورنہ
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_نمبر;
		    پیغام = سیٹ_option_قدر(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    اگر (پیغام != تہی)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(پیغام));
			جاؤ on_error;
		    }
		}
		توڑو;

	    // store $ENV
	    حالت ISN_STOREENV:
		اگر (check_secure() || check_restricted())
		    جاؤ theend;
		--ectx->ec_ڈھیر.ga_len;
		tv = ڈھیر_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		توڑو;

	    // store @r
	    حالت ISN_STOREREG:
		{
		    صحیح	reg = iptr->isn_arg.نمبر;

		    --ectx->ec_ڈھیر.ga_len;
		    tv = ڈھیر_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		توڑو;

	    // store v: variable
	    حالت ISN_STOREV:
		--ectx->ec_ڈھیر.ga_len;
		اگر (سیٹ_vim_var_tv(iptr->isn_arg.نمبر, ڈھیر_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    جاؤ on_error;
		توڑو;

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
			    توڑو;
			حالت ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    توڑو;
			حالت ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    توڑو;
			حالت ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    توڑو;
			طےشدہ:  // Cannot reach here
			    جاؤ theend;
		    }

		    --ectx->ec_ڈھیر.ga_len;
		    di = ڈھونڈو_var_اندر_ht(ht, 0, name, TRUE);
		    اگر (di == تہی)
			store_var(iptr->isn_arg.string, ڈھیر_TV_BOT(0));
		    ورنہ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			اگر (var_check_permission(di, name) == FAIL)
			    جاؤ on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *ڈھیر_TV_BOT(0);
		    }
		}
		توڑو;

	    // store an autoload variable
	    حالت ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		سیٹ_var(iptr->isn_arg.string, ڈھیر_TV_BOT(-1), TRUE);
		clear_tv(ڈھیر_TV_BOT(-1));
		--ectx->ec_ڈھیر.ga_len;
		توڑو;

	    // store number in local variable
	    حالت ISN_STORENR:
		tv = ڈھیر_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_نمبر;
		tv->vval.v_نمبر = iptr->isn_arg.storenr.stnr_val;
		توڑو;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    حالت ISN_STOREINDEX:
		{
		    صحیح res = execute_storeindex(iptr, ectx);

		    اگر (res == FAIL)
			جاؤ on_error;
		    اگر (res == NOTDONE)
			جاؤ theend;
		}
		توڑو;

	    // store value in list or blob range
	    حالت ISN_STORERANGE:
		اگر (execute_storerange(iptr, ectx) == FAIL)
		    جاؤ on_error;
		توڑو;

	    حالت ISN_بھرو_CLASSMEMBER:
		{
		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      ڈھیر_TV_BOT(0));
		    ++ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    حالت ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *ڈھیر_TV_BOT(-1);
		    --ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    // Load or store variable or argument from outer scope.
	    حالت ISN_LOADOUTER:
	    حالت ISN_STOREOUTER:
		{
		    صحیح		گہرائی = iptr->isn_arg.outer.outer_گہرائی;
		    outer_T	*outer = ectx->ec_outer_ref == تہی ? تہی
						: ectx->ec_outer_ref->or_outer;

		    جب (گہرائی > 1 && outer != تہی)
		    {
			outer = outer->برآمد_اوپر;
			--گہرائی;
		    }
		    اگر (outer == تہی)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			اگر (ectx->ec_چوکھٹا_idx == ectx->ec_initial_چوکھٹا_idx
						 || ectx->ec_outer_ref == تہی)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_احاطہ_called_from_invalid_context));
			ورنہ
			    iemsg("LOADOUTER depth more than scope levels");
			جاؤ theend;
		    }
		    اگر (گہرائی < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->برآمد_loop[-گہرائی - 1]
							       .ڈھیر->ga_ڈیٹا)
					  + outer->برآمد_loop[-گہرائی - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    ورنہ
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->برآمد_ڈھیر->ga_ڈیٹا)
				      + outer->برآمد_چوکھٹا_idx + ڈھیر_چوکھٹا_سائز
				      + iptr->isn_arg.outer.outer_idx;
		    اگر (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			    جاؤ theend;
			// careful: ga_grow_inner may re-alloc the stack
			اگر (گہرائی < 0)
			    copy = ((typval_T *)outer->برآمد_loop[-گہرائی - 1]
								   .ڈھیر->ga_ڈیٹا)
					      + outer->برآمد_loop[-گہرائی - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			ورنہ
			    copy = ((typval_T *)outer->برآمد_ڈھیر->ga_ڈیٹا)
					  + outer->برآمد_چوکھٹا_idx + ڈھیر_چوکھٹا_سائز
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			اگر (copy != tv)
			    tv = copy;
			copy_tv(tv, ڈھیر_TV_BOT(0));
			++ectx->ec_ڈھیر.ga_len;
		    }
		    ورنہ
		    {
			--ectx->ec_ڈھیر.ga_len;
			clear_tv(tv);
			*tv = *ڈھیر_TV_BOT(0);
		    }
		}
		توڑو;

	    // unlet item in list or dict variable
	    حالت ISN_UNLETINDEX:
		اگر (execute_unletindex(iptr, ectx) == FAIL)
		    جاؤ on_error;
		توڑو;

	    // unlet range of items in list variable
	    حالت ISN_UNLETRANGE:
		اگر (execute_unletrange(iptr, ectx) == FAIL)
		    جاؤ on_error;
		توڑو;

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
		اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    جاؤ theend;
		tv = ڈھیر_TV_BOT(0);
		tv->v_تالا = 0;
		++ectx->ec_ڈھیر.ga_len;
		انتخاب (iptr->isn_type)
		{
		    حالت ISN_PUSHNR:
			tv->v_type = VAR_نمبر;
			tv->vval.v_نمبر = iptr->isn_arg.نمبر;
			توڑو;
		    حالت ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_نمبر = iptr->isn_arg.نمبر;
			توڑو;
		    حالت ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_نمبر = iptr->isn_arg.نمبر;
			توڑو;
		    حالت ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			توڑو;
		    حالت ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			توڑو;
		    حالت ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			اگر (iptr->isn_arg.string == تہی)
			    tv->vval.v_string = تہی;
			ورنہ
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			توڑو;
		    حالت ISN_PUSHCHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = تہی;
#اگر_ختم
			توڑو;
		    حالت ISN_PUSHJOB:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
			tv->v_type = VAR_کام;
			tv->vval.v_کام = تہی;
#اگر_ختم
			توڑو;
		    حالت ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = تہی;
			توڑو;
		    حالت ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			توڑو;
		    طےشدہ:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == تہی
				    ? تہی : vim_strsave(iptr->isn_arg.string);
		}
		توڑو;

	    حالت ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (خالی)script_autoload(name, FALSE);
		    اگر (ڈھونڈو_func(name, TRUE))
		    {
			اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			    جاؤ theend;
			tv = ڈھیر_TV_BOT(0);
			tv->v_تالا = 0;
			++ectx->ec_ڈھیر.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    ورنہ
		    {
			صحیح res = بھرو_namespace_var(ectx, ISN_LOADG, iptr);

			اگر (res == NOTDONE)
			    جاؤ theend;
			اگر (res == FAIL)
			    جاؤ on_error;
		    }
		}
		توڑو;

	    حالت ISN_UNLET:
		اگر (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    جاؤ on_error;
		توڑو;
	    حالت ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		توڑو;

	    حالت ISN_LOCKUNLOCK:
		{
#اگر_تعریف_شدہ LOG_LOCKVAR
		    ch_log(تہی, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#اگر_ختم
		    lval_جڑ_T	*lval_جڑ_save = lval_جڑ;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_ڈھیر.ga_len;
		    lval_جڑ_T جڑ;
		    جڑ.lr_tv      = ڈھیر_TV_BOT(0);
		    جڑ.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    جڑ.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_جڑ = &جڑ;
		    صحیح res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(جڑ.lr_tv);
		    lval_جڑ = lval_جڑ_save;
		    اگر (res == FAIL)
			جاؤ on_error;
		}
		توڑو;

	    حالت ISN_LOCKCONST:
		شے_تالا(ڈھیر_TV_BOT(-1), 100, TRUE, TRUE);
		توڑو;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    حالت ISN_NEWLIST:
		اگر (exe_newlist(iptr->isn_arg.نمبر, ectx) == FAIL)
		    جاؤ theend;
		توڑو;

	    // create a tuple from items on the stack
	    حالت ISN_NEWTUPLE:
		اگر (exe_newtuple(iptr->isn_arg.نمبر, ectx) == FAIL)
		    جاؤ theend;
		توڑو;

	    // create a dict from items on the stack
	    حالت ISN_NEWDICT:
		{
		    صحیح res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.نمبر, ectx);
		    اگر (res == FAIL)
			جاؤ theend;
		    اگر (res == MAYBE)
			جاؤ on_error;
		}
		توڑو;

	    حالت ISN_CONCAT:
		اگر (exe_concat(iptr->isn_arg.نمبر, ectx) == FAIL)
		    جاؤ theend;
		توڑو;

	    // create a partial with NULL value
	    حالت ISN_NEWPARTIAL:
		اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    جاؤ theend;
		++ectx->ec_ڈھیر.ga_len;
		tv = ڈھیر_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_تالا = 0;
		tv->vval.v_partial = تہی;
		توڑو;

	    // call a :def function
	    حالت ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		اگر (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				تہی,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    جاؤ on_error;
		توڑو;

	    // call a method on an interface
	    حالت ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ڈھیر_TV_BOT(-1 - mfunc->cmf_argcount);
		    اگر (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			جاؤ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    اگر (obj == تہی)
		    {
			emsg(_(e_using_null_object));
			جاؤ on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    اگر (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    ورنہ
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			صحیح idx = object_اشاریہ_from_itf_اشاریہ(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    اگر (mfunc->cmf_ufunc != تہی && IS_GENERIC_FUNC(ufunc))
		    {
			اگر (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			ورنہ
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    اگر (ufunc == تہی)
				جاؤ on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    اگر (call_ufunc(ufunc, تہی, mfunc->cmf_argcount, ectx,
							   تہی, تہی) == FAIL)
			جاؤ on_error;
		}
		توڑو;

	    // call a builtin function
	    حالت ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		اگر (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    جاؤ on_error;
		توڑو;

	    // call a funcref or partial
	    حالت ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    صحیح		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (pfunc->cpf_چوٹی)
		    {
			// funcref is above the arguments
			tv = ڈھیر_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    ورنہ
		    {
			// Get the funcref from the stack.
			--ectx->ec_ڈھیر.ga_len;
			partial_tv = *ڈھیر_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    اگر (tv == &partial_tv)
			clear_tv(&partial_tv);
		    اگر (r == FAIL)
			جاؤ on_error;
		}
		توڑو;

	    حالت ISN_PCALL_انتہا:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_ڈھیر.ga_len;
		clear_tv(ڈھیر_TV_BOT(-1));
		*ڈھیر_TV_BOT(-1) = *ڈھیر_TV_BOT(0);
		توڑو;

	    // call a user defined function or funcref/partial
	    حالت ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			جاؤ on_error;
		}
		توڑو;

	    // :defer func(arg)
	    حالت ISN_DEFER:
		اگر (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    جاؤ on_error;
		توڑو;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    حالت ISN_RETURN_VOID:
	    حالت ISN_RETURN_OBJECT:
		اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    جاؤ theend;
		tv = ڈھیر_TV_BOT(0);
		++ectx->ec_ڈھیر.ga_len;
		اگر (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_نمبر = 0;
		    tv->v_تالا = 0;
		}
		ورنہ
		{
		    *tv = *ڈھیر_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_تالا_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    حالت ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = تہی;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    اگر (check_typval_is_قدر(ڈھیر_TV_BOT(-1)) == FAIL)
			جاؤ theend;

		    اگر (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_ڈیٹا)
							+ trystack->ga_len - 1;
		    اگر (trycmd != تہی
				 && trycmd->tcd_چوکھٹا_idx == ectx->ec_چوکھٹا_idx)
		    {
			// jump to ":finally" or ":endtry"
			اگر (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			ورنہ
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    ورنہ
			جاؤ func_return;
		}
		توڑو;

	    // push a partial, a reference to a compiled function
	    حالت ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    اگر (pt == تہی)
			جاؤ theend;
		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    {
			vim_free(pt);
			جاؤ theend;
		    }
		    اگر (extra != تہی && extra->fre_class != تہی)
		    {
			class_T	*cl;
			اگر (extra->fre_object_طریقہ)
			{
			    tv = ڈھیر_TV_BOT(-1);
			    اگر (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				جاؤ on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    اگر (obj == تہی)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				جاؤ on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ڈھیر.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			ورنہ
			    cl = extra->fre_class;

			اگر (extra->fre_object_طریقہ)
			{
			    // object method
			    // convert the interface index to the object index
			    صحیح idx =
				object_اشاریہ_from_itf_اشاریہ(extra->fre_class,
					TRUE, extra->fre_طریقہ_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			ورنہ
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_طریقہ_idx];
			}
		    }
		    ورنہ اگر (extra == تہی || extra->fre_func_name == تہی)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    ورنہ
		    {
			ufunc = ڈھونڈو_func(extra->fre_func_name, FALSE);
		    }
		    اگر (ufunc == تہی)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			جاؤ theend;
		    }
		    اگر (extra != تہی && extra->fre_outer_گہرائی > 0
					   && (ufunc->uf_flags & FC_احاطہ))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == تہی
				    ? تہی : ectx->ec_outer_ref->or_partial;

			برائے (صحیح گہرائی = 1; outer_pt != تہی
				    && گہرائی < extra->fre_outer_گہرائی; ++گہرائی)
			    outer_pt = get_pt_outer(outer_pt)->برآمد_اوپر_partial;
			اگر (outer_pt == تہی)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    جاؤ theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    ورنہ اگر (fill_partial_and_احاطہ(pt, ufunc,
			       extra == تہی ? تہی : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			جاؤ theend;
		    }
		    tv = ڈھیر_TV_BOT(0);
		    ++ectx->ec_ڈھیر.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_تالا = 0;
		}
		توڑو;

	    // Create a global function from a lambda.
	    حالت ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    اگر (copy_lambda_مقصد_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			جاؤ theend;
		}
		توڑو;

	    // List functions
	    حالت ISN_DEF:
		اگر (iptr->isn_arg.string == تہی)
		    list_functions(تہی);
		ورنہ
		{
		    exarg_T	ea;
		    garray_T	سطریں_مقصد_free;

		    CLEAR_میدان(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&سطریں_مقصد_free, حجم(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, تہی, &سطریں_مقصد_free, 0, تہی, 0, تہی);
		    ga_clear_لڑیاں(&سطریں_مقصد_free);
		}
		توڑو;

	    // jump if a condition is met
	    حالت ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    صحیح		error = FALSE;
		    صحیح		jump = TRUE;

		    اگر (when != JUMP_ALWAYS)
		    {
			tv = ڈھیر_TV_BOT(-1);
			اگر (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    اگر (error)
				جاؤ on_error;
			}
			ورنہ
			    jump = tv2bool(tv);
			اگر (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			اگر (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ڈھیر.ga_len;
			}
		    }
		    اگر (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		توڑو;

	    // "while": jump to end if a condition is false
	    حالت ISN_WHILE:
		{
		    صحیح		error = FALSE;
		    صحیح		jump = TRUE;

		    tv = ڈھیر_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    اگر (error)
			جاؤ on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_ڈھیر.ga_len;
		    اگر (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_انتہا;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = ڈھیر_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_نمبر = ectx->ec_funcrefs.ga_len;
		}
		توڑو;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    حالت ISN_JUMP_IF_ARG_سیٹ:
	    حالت ISN_JUMP_IF_ARG_NOT_سیٹ:
		tv = ڈھیر_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		صحیح arg_سیٹ = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_نمبر == VVAL_NONE);

		اگر (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_سیٹ && !arg_سیٹ)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_ڈیٹا)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_دلائل.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ ڈھیر_چوکھٹا_سائز;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_اشارہ_گر(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    اگر (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		اگر (iptr->isn_type == ISN_JUMP_IF_ARG_سیٹ ? arg_سیٹ : !arg_سیٹ)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		توڑو;

	    // top of a for loop
	    حالت ISN_FOR:
		اگر (execute_for(iptr, ectx) == FAIL)
		    جاؤ theend;
		توڑو;

	    // end of a for or while loop
	    حالت ISN_ENDLOOP:
		اگر (execute_endloop(iptr, ectx) == FAIL)
		    جاؤ theend;
		توڑو;

	    // start of ":try" block
	    حالت ISN_TRY:
		{
		    trycmd_T    *trycmd = تہی;

		    اگر (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			جاؤ theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_ڈیٹا)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_اشارہ_گر(trycmd);
		    trycmd->tcd_چوکھٹا_idx = ectx->ec_چوکھٹا_idx;
		    trycmd->tcd_ڈھیر_len = ectx->ec_ڈھیر.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		توڑو;

	    حالت ISN_PUSHEXC:
		اگر (current_exception == تہی)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    جاؤ theend;
		}
		اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
		    جاؤ theend;
		tv = ڈھیر_TV_BOT(0);
		++ectx->ec_ڈھیر.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_تالا = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->قدر);
		توڑو;

	    حالت ISN_CATCH:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    may_restore_cmdmod(&ectx->ec_funclocal);
		    trycmd = ((trycmd_T *)trystack->ga_ڈیٹا)
							+ trystack->ga_len - 1;
		    trycmd->tcd_caught = TRUE;
		    trycmd->tcd_did_throw = FALSE;

		    did_emsg = got_int = did_throw = FALSE;
		    force_abort = need_rethrow = FALSE;
		    catch_exception(current_exception);
		}
		توڑو;

	    حالت ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    صحیح		i;
		    trycmd_T    *trycmd;
		    صحیح		iidx = trycont->tct_where;

		    اگر (trystack->ga_len < trycont->tct_سطحیں)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_سطحیں, trystack->ga_len);
			جاؤ theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    برائے (i = trycont->tct_سطحیں; i > 0; --i)
		    {
			trycmd = ((trycmd_T *)trystack->ga_ڈیٹا)
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
		توڑو;

	    حالت ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_ڈیٹا)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    اگر (trycmd->tcd_caught)
		    {
			// discard the exception
			اختتام_exception(caught_ڈھیر);
			trycmd->tcd_caught = FALSE;
		    }
		    توڑو;
		}

	    // end of ":try" block
	    حالت ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_ڈیٹا) + trystack->ga_len;
		    اگر (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    اگر (trycmd->tcd_caught)
		    {
			// discard the exception
			اختتام_exception(caught_ڈھیر);
		    }

		    اگر (trycmd->tcd_return)
			جاؤ func_return;

		    جب (ectx->ec_ڈھیر.ga_len > trycmd->tcd_ڈھیر_len)
		    {
			--ectx->ec_ڈھیر.ga_len;
			clear_tv(ڈھیر_TV_BOT(0));
		    }
		    اگر (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		توڑو;

	    حالت ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    اگر (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = ڈھیر_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_نمبر;
			tv->vval.v_نمبر = 0;
			جاؤ مکمل;
		    }
		    --ectx->ec_ڈھیر.ga_len;
		    tv = ڈھیر_TV_BOT(0);
		    اگر (tv->vval.v_string == تہی
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_خالی_خانے_string));
			جاؤ theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    اگر (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_ڈیٹا)
							+ trystack->ga_len - 1;
			اگر (trycmd->tcd_caught)
			{
			    // discard the exception
			    اختتام_exception(caught_ڈھیر);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (throw_exception(tv->vval.v_string, ET_USER, تہی)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			جاؤ theend;
		    }
		    did_throw = TRUE;
		}
		توڑو;

	    // compare with special values
	    حالت ISN_COMPAREBOOL:
	    حالت ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = ڈھیر_TV_BOT(-2);
		    typval_T	*tv2 = ڈھیر_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_نمبر;
		    varnumber_T arg2 = tv2->vval.v_نمبر;
		    صحیح		res;

		    اگر (iptr->isn_arg.op.op_type == اظہاریہ_EQUAL)
			res = arg1 == arg2;
		    ورنہ
			res = arg1 != arg2;

		    --ectx->ec_ڈھیر.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_نمبر = res ? VVAL_TRUE : VVAL_FALSE;
		}
		توڑو;

	    حالت ISN_COMPARENULL:
		{
		    typval_T	*tv1 = ڈھیر_TV_BOT(-2);
		    typval_T	*tv2 = ڈھیر_TV_BOT(-1);
		    صحیح		res;

		    res = typval_compare_null(tv1, tv2);
		    اگر (res == MAYBE)
			جاؤ on_error;
		    اگر (iptr->isn_arg.op.op_type == اظہاریہ_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_ڈھیر.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_نمبر = res ? VVAL_TRUE : VVAL_FALSE;
		}
		توڑو;

	    // Operation with two number arguments
	    حالت ISN_OPNR:
	    حالت ISN_COMPARENR:
		{
		    typval_T	*tv1 = ڈھیر_TV_BOT(-2);
		    typval_T	*tv2 = ڈھیر_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_نمبر;
		    varnumber_T arg2 = tv2->vval.v_نمبر;
		    varnumber_T res = 0;
		    صحیح		div_zero = FALSE;

		    اگر (iptr->isn_arg.op.op_type == اظہاریہ_LSHIFT
			    || iptr->isn_arg.op.op_type == اظہاریہ_RSHIFT)
		    {
			اگر (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_مثبت));
			    جاؤ on_error;
			}
		    }

		    انتخاب (iptr->isn_arg.op.op_type)
		    {
			حالت اظہاریہ_MULT: res = arg1 * arg2; توڑو;
			حالت اظہاریہ_DIV:  اگر (arg2 == 0)
					    div_zero = TRUE;
					ورنہ
					    res = arg1 / arg2;
					توڑو;
			حالت اظہاریہ_REM:  اگر (arg2 == 0)
					    div_zero = TRUE;
					ورنہ
					    res = arg1 % arg2;
					توڑو;
			حالت اظہاریہ_SUB: res = arg1 - arg2; توڑو;
			حالت اظہاریہ_جوڑو: res = arg1 + arg2; توڑو;

			حالت اظہاریہ_EQUAL: res = arg1 == arg2; توڑو;
			حالت اظہاریہ_NEQUAL: res = arg1 != arg2; توڑو;
			حالت اظہاریہ_GREATER: res = arg1 > arg2; توڑو;
			حالت اظہاریہ_GEQUAL: res = arg1 >= arg2; توڑو;
			حالت اظہاریہ_SMALLER: res = arg1 < arg2; توڑو;
			حالت اظہاریہ_SEQUAL: res = arg1 <= arg2; توڑو;
			حالت اظہاریہ_LSHIFT: اگر (arg2 > MAX_LSHIFT_بٹ)
					      res = 0;
					  ورنہ
					      res = (uvarnumber_T)arg1 << arg2;
					  توڑو;
			حالت اظہاریہ_RSHIFT: اگر (arg2 > MAX_LSHIFT_بٹ)
					      res = 0;
					  ورنہ
					      res = (uvarnumber_T)arg1 >> arg2;
					  توڑو;
			طےشدہ: توڑو;
		    }

		    --ectx->ec_ڈھیر.ga_len;
		    اگر (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_نمبر = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ورنہ
			tv1->vval.v_نمبر = res;
		    اگر (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			جاؤ on_error;
		    }
		}
		توڑو;

	    // Computation with two float arguments
	    حالت ISN_OPFLOAT:
	    حالت ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = ڈھیر_TV_BOT(-2);
		    typval_T	*tv2 = ڈھیر_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    صحیح		cmp = FALSE;

		    انتخاب (iptr->isn_arg.op.op_type)
		    {
			حالت اظہاریہ_MULT: res = arg1 * arg2; توڑو;
			حالت اظہاریہ_DIV: res = arg1 / arg2; توڑو;
			حالت اظہاریہ_SUB: res = arg1 - arg2; توڑو;
			حالت اظہاریہ_جوڑو: res = arg1 + arg2; توڑو;

			حالت اظہاریہ_EQUAL: cmp = arg1 == arg2; توڑو;
			حالت اظہاریہ_NEQUAL: cmp = arg1 != arg2; توڑو;
			حالت اظہاریہ_GREATER: cmp = arg1 > arg2; توڑو;
			حالت اظہاریہ_GEQUAL: cmp = arg1 >= arg2; توڑو;
			حالت اظہاریہ_SMALLER: cmp = arg1 < arg2; توڑو;
			حالت اظہاریہ_SEQUAL: cmp = arg1 <= arg2; توڑو;
			طےشدہ: cmp = 0; توڑو;
		    }
		    --ectx->ec_ڈھیر.ga_len;
		    اگر (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_نمبر = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ورنہ
			tv1->vval.v_float = res;
		}
		توڑو;

	    حالت ISN_COMPARELIST:
	    حالت ISN_COMPARETUPLE:
	    حالت ISN_COMPAREDICT:
	    حالت ISN_COMPAREFUNC:
	    حالت ISN_COMPARESTRING:
	    حالت ISN_COMPAREBLOB:
	    حالت ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = ڈھیر_TV_BOT(-2);
		    typval_T	*tv2 = ڈھیر_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    صحیح		ic = iptr->isn_arg.op.op_ic;
		    صحیح		res = FALSE;
		    صحیح		حالت_2 = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (iptr->isn_type == ISN_COMPARELIST)
		    {
			حالت_2 = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ورنہ اگر (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			حالت_2 = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ورنہ اگر (iptr->isn_type == ISN_COMPAREDICT)
		    {
			حالت_2 = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ورنہ اگر (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			حالت_2 = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ورنہ اگر (iptr->isn_type == ISN_COMPARESTRING)
		    {
			حالت_2 = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ورنہ اگر (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			حالت_2 = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    ورنہ // ISN_COMPAREOBJECT
		    {
			حالت_2 = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_ڈھیر.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_نمبر = res ? VVAL_TRUE : VVAL_FALSE;
		    اگر (حالت_2 == FAIL)
			جاؤ theend;
		}
		توڑو;

	    حالت ISN_COMPAREANY:
		{
		    typval_T	*tv1 = ڈھیر_TV_BOT(-2);
		    typval_T	*tv2 = ڈھیر_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    صحیح		ic = iptr->isn_arg.op.op_ic;
		    صحیح		حالت_2;

		    SOURCING_LNUM = iptr->isn_lnum;
		    حالت_2 = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_ڈھیر.ga_len;
		    اگر (حالت_2 == FAIL)
			جاؤ theend;
		}
		توڑو;

	    حالت ISN_ADDLIST:
	    حالت ISN_ADDTUPLE:
	    حالت ISN_ADDBLOB:
		{
		    typval_T *tv1 = ڈھیر_TV_BOT(-2);
		    typval_T *tv2 = ڈھیر_TV_BOT(-1);

		    // add two lists or blobs
		    اگر (iptr->isn_type == ISN_ADDLIST)
		    {
			اگر (iptr->isn_arg.op.op_type == اظہاریہ_APPEND
						   && tv1->vval.v_list != تہی)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 تہی);
			ورنہ
			    eval_addlist(tv1, tv2);
		    }
		    ورنہ اگر (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    ورنہ
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    حالت ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = ڈھیر_TV_BOT(-2);
		    typval_T	*tv2 = ڈھیر_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (l == تہی)
		    {
			emsg(_(e_cannot_جوڑو_مقصد_null_list));
			جاؤ on_error;
		    }
		    اگر (قدر_check_تالا(l->lv_تالا, تہی, FALSE))
			جاؤ on_error;
		    اگر (list_append_tv(l, tv2) == FAIL)
			جاؤ theend;
		    clear_tv(tv2);
		    --ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    حالت ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = ڈھیر_TV_BOT(-2);
		    typval_T	*tv2 = ڈھیر_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    صحیح		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    اگر (b == تہی)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_جوڑو_مقصد_null_blob));
			جاؤ on_error;
		    }
		    n = tv_get_نمبر_chk(tv2, &error);
		    اگر (error)
			جاؤ on_error;
		    ga_append(&b->bv_ga, (صحیح)n);
		    --ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    // Computation with two arguments of unknown type
	    حالت ISN_OPANY:
		{
		    typval_T	*tv1 = ڈھیر_TV_BOT(-2);
		    typval_T	*tv2 = ڈھیر_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    صحیح		error = FALSE;

		    اگر (iptr->isn_arg.op.op_type == اظہاریہ_جوڑو)
		    {
			اگر (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ڈھیر.ga_len;
			    توڑو;
			}
			ورنہ اگر (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ڈھیر.ga_len;
			    توڑو;
			}
			ورنہ اگر (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ڈھیر.ga_len;
			    توڑو;
			}
		    }
		    اگر (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    ورنہ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_نمبر_chk(tv1, &error);
			اگر (error)
			    جاؤ on_error;
			اگر (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    اگر (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    ورنہ
		    {
			n2 = tv_get_نمبر_chk(tv2, &error);
			اگر (error)
			    جاؤ on_error;
			اگر (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    اگر (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			انتخاب (iptr->isn_arg.op.op_type)
			{
			    حالت اظہاریہ_MULT: f1 = f1 * f2; توڑو;
			    حالت اظہاریہ_DIV:  f1 = f1 / f2; توڑو;
			    حالت اظہاریہ_SUB:  f1 = f1 - f2; توڑو;
			    حالت اظہاریہ_جوڑو:  f1 = f1 + f2; توڑو;
			    طےشدہ: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     جاؤ on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_ڈھیر.ga_len;
		    }
		    ورنہ
		    {
			صحیح failed = FALSE;

			انتخاب (iptr->isn_arg.op.op_type)
			{
			    حالت اظہاریہ_MULT: n1 = n1 * n2; توڑو;
			    حالت اظہاریہ_DIV:  n1 = عدد_divide(n1, n2, &failed);
					    اگر (failed)
						جاؤ on_error;
					    توڑو;
			    حالت اظہاریہ_SUB:  n1 = n1 - n2; توڑو;
			    حالت اظہاریہ_جوڑو:  n1 = n1 + n2; توڑو;
			    طےشدہ:	    n1 = عدد_modulus(n1, n2, &failed);
					    اگر (failed)
						جاؤ on_error;
					    توڑو;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_نمبر;
			tv1->vval.v_نمبر = n1;
			--ectx->ec_ڈھیر.ga_len;
		    }
		}
		توڑو;

	    حالت ISN_STRINDEX:
	    حالت ISN_STRSLICE:
		{
		    صحیح		is_قاش = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    اگر (is_قاش)
		    {
			tv = ڈھیر_TV_BOT(-2);
			n1 = tv->vval.v_نمبر;
		    }

		    tv = ڈھیر_TV_BOT(-1);
		    n2 = tv->vval.v_نمبر;

		    ectx->ec_ڈھیر.ga_len -= is_قاش ? 2 : 1;
		    tv = ڈھیر_TV_BOT(-1);
		    اگر (is_قاش)
			// Slice: Select the characters from the string
			res = string_قاش(tv->vval.v_string, n1, n2, FALSE);
		    ورنہ
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		توڑو;

	    حالت ISN_LISTINDEX:
	    حالت ISN_LISTSLICE:
	    حالت ISN_TUPLEINDEX:
	    حالت ISN_TUPLESLICE:
	    حالت ISN_BLOBINDEX:
	    حالت ISN_BLOBSLICE:
		{
		    صحیح		is_قاش = iptr->isn_type == ISN_LISTSLICE
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
		    val_tv = is_قاش ? ڈھیر_TV_BOT(-3) : ڈھیر_TV_BOT(-2);

		    tv = ڈھیر_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_نمبر;
		    clear_tv(tv);

		    اگر (is_قاش)
		    {
			tv = ڈھیر_TV_BOT(-2);
			n1 = tv->vval.v_نمبر;
			clear_tv(tv);
		    }

		    ectx->ec_ڈھیر.ga_len -= is_قاش ? 2 : 1;
		    tv = ڈھیر_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (is_blob)
		    {
			اگر (blob_قاش_or_اشاریہ(val_tv->vval.v_blob, is_قاش,
						    n1, n2, FALSE, tv) == FAIL)
			    جاؤ on_error;
		    }
		    ورنہ اگر (is_tuple)
		    {
			اگر (tuple_قاش_or_اشاریہ(val_tv->vval.v_tuple,
				is_قاش, n1, n2, FALSE, tv, TRUE) == FAIL)
			    جاؤ on_error;
		    }
		    ورنہ
		    {
			اگر (list_قاش_or_اشاریہ(val_tv->vval.v_list, is_قاش,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    جاؤ on_error;
		    }
		}
		توڑو;

	    حالت ISN_ANYINDEX:
	    حالت ISN_ANYSLICE:
		{
		    صحیح		is_قاش = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    صحیح		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_قاش ? ڈھیر_TV_BOT(-3) : ڈھیر_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (check_can_اشاریہ(tv, TRUE, TRUE) == FAIL)
			جاؤ on_error;
		    var1 = is_قاش ? ڈھیر_TV_BOT(-2) : ڈھیر_TV_BOT(-1);
		    var2 = is_قاش ? ڈھیر_TV_BOT(-1) : تہی;
		    res = eval_اشاریہ_inner(tv, is_قاش, var1, var2,
							FALSE, تہی, -1, TRUE);
		    clear_tv(var1);
		    اگر (is_قاش)
			clear_tv(var2);
		    ectx->ec_ڈھیر.ga_len -= is_قاش ? 2 : 1;
		    اگر (res == FAIL)
			جاؤ on_error;
		}
		توڑو;

	    حالت ISN_قاش:
		{
		    صحیح		گنتی = iptr->isn_arg.نمبر;

		    // type will have been checked to be a list
		    tv = ڈھیر_TV_BOT(-1);
		    اگر (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			اگر (list != تہی && list->lv_len >= گنتی)
			{
			    list_T	*newlist = list_قاش(list,
				    گنتی, list->lv_len - 1);

			    اگر (newlist != تہی)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    ورنہ
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			اگر (tuple != تہی && TUPLE_LEN(tuple) >= گنتی)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_قاش(tuple, گنتی,
							TUPLE_LEN(tuple) - 1);
			    اگر (newtuple != تہی)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		توڑو;

	    حالت ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*شے_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = ڈھیر_TV_BOT(-1 - gi->gi_with_op);
		    اگر (tv->v_type == VAR_LIST)
		    {
			li = list_ڈھونڈو(tv->vval.v_list, gi->gi_اشاریہ);
			اگر (li == تہی)
			    جاؤ on_error;
			شے_tv = &li->li_tv;
		    }
		    ورنہ
			شے_tv = TUPLE_شے(tv->vval.v_tuple, gi->gi_اشاریہ);

		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    ++ectx->ec_ڈھیر.ga_len;
		    copy_tv(شے_tv, ڈھیر_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_اشاریہ = gi->gi_اشاریہ + 1;
		    ectx->ec_where.wt_قسم = WT_VARIABLE;
		}
		توڑو;

	    // dict member with string key (dict['member'])
	    حالت ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*کلید;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = ڈھیر_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = ڈھیر_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    کلید = tv->vval.v_string;
		    اگر (کلید == تہی)
			کلید = (char_u *)"";

		    اگر ((di = dict_ڈھونڈو(dict, کلید, -1)) == تہی)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_کلید_not_present_اندر_dictionary_str), کلید);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_ڈھیر.ga_len;
			tv = ڈھیر_TV_BOT(-1);
			(خالی) dict_ڈھیر_save(tv);
			tv->v_type = VAR_نمبر;
			tv->vval.v_نمبر = 0;
			جاؤ on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_ڈھیر.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = ڈھیر_TV_BOT(-1);
		    اگر (dict_ڈھیر_save(tv) == FAIL)
			جاؤ on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		توڑو;

	    // dict member with string key (dict.member)
	    // or can be an object
	    حالت ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = ڈھیر_TV_BOT(-1);

		    اگر (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			اگر (dict_ڈھیر_save(tv) == FAIL)
			    جاؤ on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_ڈیٹا)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			اگر (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    جاؤ on_error;
		    }
		    ورنہ
		    {
			اگر (tv->v_type != VAR_DICT || tv->vval.v_dict == تہی)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    جاؤ on_error;
			}
			dict = tv->vval.v_dict;

			اگر ((di = dict_ڈھونڈو(dict, iptr->isn_arg.string, -1))
								   == تہی)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_کلید_not_present_اندر_dictionary_str),
						     iptr->isn_arg.string);
			    جاؤ on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			اگر (dict_ڈھیر_save(tv) == FAIL)
			    جاؤ on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		توڑو;

	    حالت ISN_GET_OBJ_MEMBER:
	    حالت ISN_GET_ITF_MEMBER:
		{
		    tv = ڈھیر_TV_BOT(-1);
		    اگر (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			جاؤ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    اگر (obj == تہی)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			جاؤ on_error;
		    }

		    صحیح idx;
		    اگر (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    ورنہ
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_اشاریہ_from_itf_اشاریہ(
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
			جاؤ on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		توڑو;

	    حالت ISN_STORE_THIS:
		{
		    صحیح idx = iptr->isn_arg.نمبر;
		    object_T *obj = ڈھیر_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *ڈھیر_TV_BOT(-1);
		    --ectx->ec_ڈھیر.ga_len;
		}
		توڑو;

	    حالت ISN_CLEARDICT:
		dict_ڈھیر_drop();
		توڑو;

	    حالت ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_ڈھیر_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = ڈھیر_TV_BOT(-1);
		    اگر (dict_tv != تہی
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != تہی
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == تہی))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_ڈھیر_drop();
		}
		توڑو;

	    حالت ISN_NEGATENR:
		tv = ڈھیر_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		اگر (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		ورنہ
		    tv->vval.v_نمبر = -tv->vval.v_نمبر;
		توڑو;

	    حالت ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    صحیح		r;
		    where_T	where = WHERE_INIT;

		    tv = ڈھیر_TV_BOT((صحیح)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    اگر (ct->ct_arg_idx > 0)
		    {
			where.wt_اشاریہ = ct->ct_arg_idx;
			where.wt_قسم = ct->ct_is_var ? WT_VARIABLE : WT_دلیل;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    اگر (r == FAIL)
			جاؤ on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    اگر (tv->v_type == VAR_نمبر
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_نمبر == 0
						|| tv->vval.v_نمبر == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_نمبر = tv->vval.v_نمبر
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		توڑو;

	    حالت ISN_CHECKLEN:
		{
		    صحیح	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = تہی;
		    tuple_T *tuple = تہی;
		    صحیح	    len = 0;

		    tv = ڈھیر_TV_BOT(-1);

		    صحیح		len_check_failed = FALSE;
		    اگر (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			اگر (list == تہی || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_مزید_OK))
			    len_check_failed = TRUE;
			اگر (list != تہی)
			    len = list->lv_len;
		    }
		    ورنہ اگر (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			اگر (tuple == تہی || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_مزید_OK))
			    len_check_failed = TRUE;
			اگر (tuple != تہی)
			    len = TUPLE_LEN(tuple);
		    }
		    ورنہ
			len_check_failed = TRUE;

		    اگر (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_اشیاء_but_got_nr),
				     min_len, len);
			جاؤ on_error;
		    }
		}
		توڑو;

	    حالت ISN_SETTYPE:
		سیٹ_tv_type(ڈھیر_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		توڑو;

	    حالت ISN_2BOOL:
	    حالت ISN_COND2BOOL:
		{
		    صحیح n;
		    صحیح error = FALSE;

		    اگر (iptr->isn_type == ISN_2BOOL)
		    {
			tv = ڈھیر_TV_BOT(iptr->isn_arg.tobool.انحراف);
			n = tv2bool(tv);
			اگر (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    ورنہ
		    {
			tv = ڈھیر_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			اگر (error)
			    جاؤ on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_نمبر = n ? VVAL_TRUE : VVAL_FALSE;
		}
		توڑو;

	    حالت ISN_2STRING:
	    حالت ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		اگر (do_2string(ڈھیر_TV_BOT(iptr->isn_arg.tostring.انحراف),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    جاؤ on_error;
		توڑو;

	    حالت ISN_RANGE:
		{
		    exarg_T	ea;
		    حرف	*errormsg;

		    ea.line2 = 0;
		    ea.addr_گنتی = 0;
		    ea.addr_type = ADDR_سطریں;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    اگر (parse_cmd_پتہ(&ea, &errormsg, FALSE) == FAIL)
			جاؤ on_error;

		    اگر (GA_GROW_FAILS(&ectx->ec_ڈھیر, 1))
			جاؤ theend;
		    ++ectx->ec_ڈھیر.ga_len;
		    tv = ڈھیر_TV_BOT(-1);
		    tv->v_type = VAR_نمبر;
		    tv->v_تالا = 0;
		    tv->vval.v_نمبر = ea.line2;
		}
		توڑو;

	    حالت ISN_PUT:
		tv = تہی; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		توڑو;
	    حالت ISN_IPUT:
		tv = تہی;
		isn_put_do(ectx, iptr, tv, TRUE);
		توڑو;

	    حالت ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_ڈھیر.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		توڑو;

	    حالت ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = تہی;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		توڑو;

	    حالت ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = ڈھیر_TV_BOT(-1);
		اگر (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_دلیل_must_be_sequence_کا_lists_or_tuples));
		    جاؤ on_error;
		}

		صحیح rc;
		اگر (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		ورنہ
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		اگر (rc != EXEC_OK)
		{
		    اگر (rc == EXEC_FAIL)
			جاؤ on_error;
		    جاؤ theend;
		}
		توڑو;

	    حالت ISN_PROF_آغاز:
	    حالت ISN_PROF_انتہا:
		{
#اگر_تعریف_شدہ FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_ڈیٹا)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    اگر (iptr->isn_type == ISN_PROF_آغاز)
		    {
			func_سطر_آغاز(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_سطر_exec(&cookie);
		    }
		    ورنہ
			func_سطر_انتہا(&cookie);
#اگر_ختم
		}
		توڑو;

	    حالت ISN_DEBUG:
		handle_debug(iptr, ectx);
		توڑو;

	    حالت ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    صحیح		شے = iptr->isn_arg.shuffle.shfl_شے;
		    صحیح		اوپر = iptr->isn_arg.shuffle.shfl_اوپر;

		    tmp_tv = *ڈھیر_TV_BOT(-شے);
		    برائے ( ; اوپر > 0 && شے > 1; --اوپر)
		    {
			*ڈھیر_TV_BOT(-شے) = *ڈھیر_TV_BOT(-شے + 1);
			--شے;
		    }
		    *ڈھیر_TV_BOT(-شے) = tmp_tv;
		}
		توڑو;

	    حالت ISN_DROP:
		--ectx->ec_ڈھیر.ga_len;
		clear_tv(ڈھیر_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		توڑو;

	    حالت ISN_SCRIPTCTX_سیٹ:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		توڑو;
	}
	جاری;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	اگر (ectx->ec_چوکھٹا_idx == ectx->ec_initial_چوکھٹا_idx)
	    جاؤ مکمل;

	اگر (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    جاؤ theend;
	جاری;

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
		جب (ectx->ec_ڈھیر.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_ڈھیر.ga_len;
		    clear_tv(ڈھیر_TV_BOT(0));
		}
		جب (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    جاری;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	اگر (trylevel <= ectx->ec_trylevel_at_آغاز)
	    جاؤ theend;
    }

مکمل:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_ڈھیر_clear(dict_ڈھیر_len_at_آغاز);
    ectx->ec_trylevel_at_آغاز = save_trylevel_at_آغاز;
    واپس ret;
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
	*rettv = *ڈھیر_TV_BOT(-1);
	--ectx->ec_ڈھیر.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    واپس res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(خالی)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    صحیح		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    اگر (exec_instructions(ectx) == OK)
    {
	typval_T *tv = ڈھیر_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_ڈھیر.ga_len;
	clear_tv(tv);
    }
    ورنہ
    {
	substitute_instr->subs_حالت = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    واپس res;
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
    صحیح		partial_argc = partial == تہی
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    صحیح		کل_argc = argc + partial_argc;
    typval_T	*tv;
    صحیح		idx;
    صحیح		ret = FAIL;
    صحیح		defcount = ufunc->uf_دلائل.ga_len - کل_argc;
    sctx_T	save_current_sctx = current_sctx;
    صحیح		did_emsg_before = did_emsg_cumul + did_emsg;
    صحیح		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_پیغام_list = تہی;
    msglist_T	*نجی_پیغام_list = تہی;
    صحیح		save_emsg_silent_def = emsg_silent_def;
    صحیح		save_did_emsg_def = did_emsg_def;
    صحیح		orig_funcdepth;
    صحیح		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#تعریف_منسوخ ڈھیر_TV
#تعریف_کرو ڈھیر_TV(idx) (((typval_T *)ectx.ec_ڈھیر.ga_ڈیٹا) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#تعریف_منسوخ ڈھیر_TV_BOT
#تعریف_کرو ڈھیر_TV_BOT(idx) (((typval_T *)ectx.ec_ڈھیر.ga_ڈیٹا) + ectx.ec_ڈھیر.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#تعریف_منسوخ ڈھیر_TV_VAR
#تعریف_کرو ڈھیر_TV_VAR(idx) (((typval_T *)ectx.ec_ڈھیر.ga_ڈیٹا) + ectx.ec_چوکھٹا_idx + ڈھیر_چوکھٹا_سائز + idx)

    اگر (ufunc->uf_def_حالت == UF_NOT_COMPILED
	    || ufunc->uf_def_حالت == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), تہی) == FAIL))
    {
	اگر (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	واپس FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
							 + ufunc->uf_dfunc_idx;
	اگر (dfunc->df_ufunc == تہی)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    واپس FAIL;
	}
	اگر (INSTRUCTIONS(dfunc) == تہی)
	{
	    iemsg("using call_def_function() on not compiled function");
	    واپس FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    اگر (funcdepth_increment() == FAIL)
	واپس FAIL;

    CLEAR_میدان(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_ڈھیر, حجم(typval_T), 500);
    اگر (GA_GROW_FAILS(&ectx.ec_ڈھیر, 20))
    {
	funcdepth_decrement();
	واپس FAIL;
    }
    ga_init2(&ectx.ec_trystack, حجم(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, حجم(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = کل_argc - ufunc->uf_دلائل.ga_len;
    اگر (idx > 0 && ufunc->uf_va_name == تہی)
    {
	semsg(NGETTEXT(e_one_دلیل_too_many, e_nr_arguments_too_many,
								    idx), idx);
	جاؤ failed_early;
    }
    idx = کل_argc - ufunc->uf_دلائل.ga_len + ufunc->uf_def_دلائل.ga_len;
    اگر (idx < 0)
    {
	semsg(NGETTEXT(e_one_دلیل_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	جاؤ failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    برائے (idx = 0; idx < کل_argc
	    && (ufunc->uf_va_name != تہی || idx < ufunc->uf_دلائل.ga_len);
									 ++idx)
    {
	صحیح argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	اگر (idx >= ufunc->uf_دلائل.ga_len - ufunc->uf_def_دلائل.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_نمبر == VVAL_NONE)
	{
	    // Use the default value.
	    ڈھیر_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	ورنہ
	{
	    صحیح مکمل = FALSE;
	    اگر (ufunc->uf_arg_types != تہی && idx < ufunc->uf_دلائل.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		اگر (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_نمبر)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    ڈھیر_TV_BOT(0)->v_type = VAR_FLOAT;
		    ڈھیر_TV_BOT(0)->v_تالا = 0;
		    ڈھیر_TV_BOT(0)->vval.v_float = tv->vval.v_نمبر;
		    مکمل = TRUE;
		}
		ورنہ اگر (check_typval_arg_type(expected, tv,
						   تہی, argv_idx + 1) == FAIL)
		    جاؤ failed_early;
	    }
	    اگر (!مکمل)
		copy_tv(tv, ڈھیر_TV_BOT(0));
	}
	++ectx.ec_ڈھیر.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    اگر (ufunc->uf_va_name != تہی)
    {
	صحیح vararg_گنتی = argc - ufunc->uf_دلائل.ga_len;

	اگر (vararg_گنتی < 0)
	    vararg_گنتی = 0;
	ورنہ
	    argc -= vararg_گنتی;
	اگر (exe_newlist(vararg_گنتی, &ectx) == FAIL)
	    جاؤ failed_early;

	// Check the type of the list items.
	tv = ڈھیر_TV_BOT(-1);
	اگر (ufunc->uf_va_type != تہی
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != تہی)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_پہلا;

	    برائے (idx = 0; idx < vararg_گنتی; ++idx)
	    {
		اگر (check_typval_arg_type(expected, &li->li_tv,
						 تہی, argc + idx + 1) == FAIL)
		    جاؤ failed_early;
		li = li->li_اگلا;
	    }
	}

	اگر (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *ڈھیر_TV_BOT(defcount - 1) = *ڈھیر_TV_BOT(-1);
	--ectx.ec_ڈھیر.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    اگر (defcount > 0)
	برائے (idx = 0; idx < defcount; ++idx)
	{
	    ڈھیر_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_ڈھیر.ga_len;
	}
    اگر (ufunc->uf_va_name != تہی)
	++ectx.ec_ڈھیر.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_چوکھٹا_idx = ectx.ec_ڈھیر.ga_len;
    ectx.ec_initial_چوکھٹا_idx = ectx.ec_چوکھٹا_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *بنیاد_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	اگر (partial != تہی || بنیاد_ufunc->uf_partial != تہی)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    اگر (ectx.ec_outer_ref == تہی)
		جاؤ failed_early;
	    اگر (partial != تہی)
	    {
		outer_T *outer = get_pt_outer(partial);

		اگر (outer->برآمد_ڈھیر == تہی && outer->برآمد_loop_سائز == 0)
		{
		    // no stack was set
		    اگر (current_ectx != تہی)
		    {
			اگر (current_ectx->ec_outer_ref != تہی
			       && current_ectx->ec_outer_ref->or_outer != تہی)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		ورنہ
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    ورنہ
	    {
		ectx.ec_outer_ref->or_outer = &بنیاد_ufunc->uf_partial->pt_outer;
		++بنیاد_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = بنیاد_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    برائے (idx = 0; idx < ڈھیر_چوکھٹا_سائز; ++idx)
    {
	ڈھیر_TV(ectx.ec_ڈھیر.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_ڈھیر.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	برائے (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    ڈھیر_TV_VAR(idx)->v_type = VAR_نمبر;
	    ڈھیر_TV_VAR(idx)->vval.v_نمبر = 0;
	}
	ectx.ec_ڈھیر.ga_len += dfunc->df_varcount;

	اگر (object != تہی)
	{
	    // the object is always the variable at index zero
	    tv = ڈھیر_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	اگر (dfunc->df_has_احاطہ)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    ڈھیر_TV_VAR(idx)->v_type = VAR_نمبر;
	    ڈھیر_TV_VAR(idx)->vval.v_نمبر = 0;
	    ++ectx.ec_ڈھیر.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    اگر (funccal != تہی)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_ڈالو_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_نسخہ = SCRIPT_نسخہ_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_پیغام_list = پیغام_list;
    پیغام_list = &نجی_پیغام_list;

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
	ورنہ
	{
	    tv = ڈھیر_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    اگر (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_احاطہ_اندر_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_نکالو();
    current_sctx = save_current_sctx;

    اگر (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    اگر (*پیغام_list != تہی && saved_پیغام_list != تہی)
    {
	msglist_T **plist = saved_پیغام_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	جب (*plist != تہی)
	    plist = &(*plist)->اگلا;

	*plist = *پیغام_list;
    }
    پیغام_list = saved_پیغام_list;

    اگر (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = تہی;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    برائے (idx = 0; idx < ectx.ec_ڈھیر.ga_len; ++idx)
    {
	tv = ڈھیر_TV(idx);
	اگر (tv->v_type != VAR_نمبر && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_ڈھیر.ga_ڈیٹا);
    vim_free(ectx.ec_trystack.ga_ڈیٹا);
    اگر (ectx.ec_outer_ref != تہی)
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
    واپس ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    خالی
unwind_def_callstack(ectx_T *ectx)
{
    جب (ectx->ec_چوکھٹا_idx != ectx->ec_initial_چوکھٹا_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    خالی
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + ectx->ec_dfunc_idx;

    اگر (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    ساکن char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    صحیح		گہرائی;

    ga_init2(&ga, 1, 100);
    برائے (گہرائی = 0; گہرائی < lvi->lvi_گہرائی; ++گہرائی)
    {
	اگر (ga_grow(&ga, 50) == FAIL)
	    توڑو;
	اگر (lvi->lvi_loop[گہرائی].var_idx == 0)
	    STRCPY((حرف *)ga.ga_ڈیٹا + ga.ga_len, " -");
	ورنہ
	    vim_snprintf((حرف *)ga.ga_ڈیٹا + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[گہرائی].var_idx,
			    lvi->lvi_loop[گہرائی].var_idx
					 + lvi->lvi_loop[گہرائی].var_گنتی - 1);
	ga.ga_len = (صحیح)STRLEN(ga.ga_ڈیٹا);
    }
    واپس ga.ga_ڈیٹا;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    ساکن خالی
list_instructions(حرف *pfx, isn_T *instr, صحیح instr_گنتی, ufunc_T *ufunc)
{
    صحیح		سطر_idx = 0;
    صحیح		پچھلا_current = 0;
    صحیح		current;
    صحیح		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    برائے (current = 0; current < instr_گنتی; ++current)
    {
	isn_T	    *iptr = &instr[current];
	حرف	    *سطر;

	اگر (ufunc != تہی)
	{
	    جب (سطر_idx < iptr->isn_lnum
					  && سطر_idx < ufunc->uf_سطریں.ga_len)
	    {
		اگر (current > پچھلا_current)
		{
		    پیغام_puts("\n\n");
		    پچھلا_current = current;
		}
		سطر = ((حرف **)ufunc->uf_سطریں.ga_ڈیٹا)[سطر_idx++];
		اگر (سطر != تہی)
		    پیغام(سطر);
	    }
	    اگر (iptr->isn_type == ISN_JUMP_IF_ARG_سیٹ)
	    {
		صحیح	پہلا_def_arg = ufunc->uf_دلائل.ga_len
						   - ufunc->uf_def_دلائل.ga_len;

		اگر (def_arg_idx > 0)
		    پیغام_puts("\n\n");
		پیغام_آغاز();
		پیغام_puts("  ");
		پیغام_puts(((حرف **)(ufunc->uf_دلائل.ga_ڈیٹا))[
						 پہلا_def_arg + def_arg_idx]);
		پیغام_puts(" = ");
		پیغام_puts(((حرف **)(ufunc->uf_def_دلائل.ga_ڈیٹا))[def_arg_idx++]);
		پیغام_clr_eos();
		پیغام_انتہا();
	    }
	}

	انتخاب (iptr->isn_type)
	{
	    حالت ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (صحیح)iptr->isn_arg.construct.construct_سائز);
		توڑو;
	    حالت ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_EXEC_بانٹو:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		توڑو;
	    حالت ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		توڑو;
	    حالت ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.نمبر ? " append" : "");
		توڑو;
	    حالت ISN_CEXPR_AUCMD:
#اگر_تعریف_شدہ FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.نمبر));
#اگر_ختم
		توڑو;
	    حالت ISN_CEXPR_CORE:
#اگر_تعریف_شدہ FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#اگر_ختم
		توڑو;
	    حالت ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, زیادہ_سے_زیادہ_عدد, تہی);
		پیغام("     -------------");
		توڑو;
	    حالت ISN_ماخذ:
		{
		    scriptitem_T    *si = SCRIPT_شے(iptr->isn_arg.نمبر);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		توڑو;
	    حالت ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, زیادہ_سے_زیادہ_عدد, تہی);
		    پیغام("     -------------");
		}
		توڑو;
	    حالت ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.نمبر);
		توڑو;
	    حالت ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_گنتی);
		}
		توڑو;
	    حالت ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_ECHOWINDOW:
		اگر (iptr->isn_arg.echowin.ewin_وقت > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_گنتی,
				      iptr->isn_arg.echowin.ewin_وقت);
		ورنہ
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_گنتی);
		توڑو;
	    حالت ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_بھرو:
		{
		    اگر (iptr->isn_arg.نمبر < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.نمبر
							  + ڈھیر_چوکھٹا_سائز));
		    ورنہ
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.نمبر));
		}
		توڑو;
	    حالت ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    اگر (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_گہرائی,
					outer->outer_idx + ڈھیر_چوکھٹا_سائز);
		    ورنہ اگر (outer->outer_گہرائی < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_گہرائی);
		    ورنہ
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_گہرائی,
					      outer->outer_idx);
		}
		توڑو;
	    حالت ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_شے(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    اگر (sv == تہی)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    ورنہ
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		توڑو;
	    حالت ISN_LOADS:
	    حالت ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_شے(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		توڑو;
	    حالت ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		توڑو;
	    حالت ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		توڑو;
	    حالت ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		توڑو;
	    حالت ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		توڑو;
	    حالت ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (صحیح)(iptr->isn_arg.نمبر));
		توڑو;

	    حالت ISN_STORE:
		اگر (iptr->isn_arg.نمبر < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.نمبر + ڈھیر_چوکھٹا_سائز);
		ورنہ
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.نمبر);
		توڑو;
	    حالت ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    اگر (outer->outer_گہرائی < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_گہرائی);
		    ورنہ
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_گہرائی, outer->outer_idx);
		}
		توڑو;
	    حالت ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_STORES:
	    حالت ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_شے(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		توڑو;
	    حالت ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_شے(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    اگر (sv == تہی)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    ورنہ
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		توڑو;
	    حالت ISN_STOREOPT:
	    حالت ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		توڑو;
	    حالت ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (صحیح)iptr->isn_arg.نمبر);
		توڑو;
	    حالت ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		توڑو;

	    حالت ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		توڑو;

	    حالت ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		توڑو;

	    حالت ISN_بھرو_CLASSMEMBER:
	    حالت ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    صحیح	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_بھرو_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		توڑو;

	    // constants
	    حالت ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_PUSHBOOL:
	    حالت ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		توڑو;
	    حالت ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		توڑو;
	    حالت ISN_PUSHFUNC:
		{
		    حرف *name = (حرف *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == تہی ? "[none]" : name);
		}
		توڑو;
	    حالت ISN_PUSHCHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#اگر_ختم
		توڑو;
	    حالت ISN_PUSHJOB:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#اگر_ختم
		توڑو;
	    حالت ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		توڑو;
	    حالت ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == تہی ? "null"
				 : (حرف *)iptr->isn_arg.classarg->class_name.string);
		توڑو;
	    حالت ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		توڑو;
	    حالت ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		توڑو;
	    حالت ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		توڑو;
	    حالت ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		توڑو;
	    حالت ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		توڑو;
	    حالت ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		توڑو;
	    حالت ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		توڑو;
	    حالت ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		توڑو;

	    // function call
	    حالت ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    اندرونی_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		توڑو;
	    حالت ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_ڈیٹا)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		توڑو;
	    حالت ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != تہی
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		توڑو;
	    حالت ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		توڑو;
	    حالت ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_چوٹی ? " top" : "", cpfunc->cpf_argcount);
		}
		توڑو;
	    حالت ISN_PCALL_انتہا:
		smsg("%s%4d PCALL end", pfx, current);
		توڑو;
	    حالت ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (صحیح)iptr->isn_arg.defer.defer_argcount);
		توڑو;
	    حالت ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		توڑو;
	    حالت ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		توڑو;
	    حالت ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		توڑو;
	    حالت ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    اگر (extra == تہی || extra->fre_func_name == تہی)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_ڈیٹا)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    ورنہ
			name = extra->fre_func_name;
		    اگر (extra != تہی && extra->fre_class != تہی)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    ورنہ اگر (extra == تہی
				     || extra->fre_loopvar_info.lvi_گہرائی == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    ورنہ
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		توڑو;

	    حالت ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    اگر (arg->nfa_loopvar_info.lvi_گہرائی == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    ورنہ
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		توڑو;

	    حالت ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == تہی ? (char_u *)"" : name);
		}
		توڑو;

	    حالت ISN_JUMP:
		{
		    حرف *when = "?";

		    انتخاب (iptr->isn_arg.jump.jump_when)
		    {
			حالت JUMP_ALWAYS:
			    when = "JUMP";
			    توڑو;
			حالت JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    توڑو;
			حالت JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    توڑو;
			حالت JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    توڑو;
			حالت JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  //غیر_مستعمل
			    توڑو;
			حالت JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    توڑو;
			حالت JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    توڑو;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		توڑو;

	    حالت ISN_JUMP_IF_ARG_سیٹ:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ڈھیر_چوکھٹا_سائز,
						iptr->isn_arg.jump.jump_where);
		توڑو;

	    حالت ISN_JUMP_IF_ARG_NOT_سیٹ:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ڈھیر_چوکھٹا_سائز,
						iptr->isn_arg.jump.jump_where);
		توڑو;

	    حالت ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_انتہا);
		}
		توڑو;

	    حالت ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->انتہا_funcref_idx,
			    endloop->انتہا_var_idx,
			    endloop->انتہا_var_idx + endloop->انتہا_var_گنتی - 1,
			    endloop->انتہا_گہرائی);
		}
		توڑو;

	    حالت ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_انتہا);
		}
		توڑو;

	    حالت ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    اگر (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    ورنہ
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		توڑو;
	    حالت ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		توڑو;
	    حالت ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_سطحیں,
				      trycont->tct_سطحیں == 1 ? "" : "s",
				      trycont->tct_where);
		}
		توڑو;
	    حالت ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		توڑو;
	    حالت ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		توڑو;
	    حالت ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		توڑو;

	    // expression operations on number
	    حالت ISN_OPNR:
	    حالت ISN_OPFLOAT:
	    حالت ISN_OPANY:
		{
		    حرف *what;
		    حرف *ins;

		    انتخاب (iptr->isn_arg.op.op_type)
		    {
			حالت اظہاریہ_MULT: what = "*"; توڑو;
			حالت اظہاریہ_DIV: what = "/"; توڑو;
			حالت اظہاریہ_REM: what = "%"; توڑو;
			حالت اظہاریہ_SUB: what = "-"; توڑو;
			حالت اظہاریہ_جوڑو: what = "+"; توڑو;
			حالت اظہاریہ_LSHIFT: what = "<<"; توڑو;
			حالت اظہاریہ_RSHIFT: what = ">>"; توڑو;
			طےشدہ:       what = "???"; توڑو;
		    }
		    انتخاب (iptr->isn_type)
		    {
			حالت ISN_OPNR: ins = "OPNR"; توڑو;
			حالت ISN_OPFLOAT: ins = "OPFLOAT"; توڑو;
			حالت ISN_OPANY: ins = "OPANY"; توڑو;
			طےشدہ: ins = "???"; توڑو;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		توڑو;

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
		       حرف *p;
		       حرف buf[10];
		       حرف *type;

		       انتخاب (iptr->isn_arg.op.op_type)
		       {
			   حالت اظہاریہ_EQUAL:	 p = "=="; توڑو;
			   حالت اظہاریہ_NEQUAL:    p = "!="; توڑو;
			   حالت اظہاریہ_GREATER:   p = ">"; توڑو;
			   حالت اظہاریہ_GEQUAL:    p = ">="; توڑو;
			   حالت اظہاریہ_SMALLER:   p = "<"; توڑو;
			   حالت اظہاریہ_SEQUAL:    p = "<="; توڑو;
			   حالت اظہاریہ_مطابقت:	 p = "=~"; توڑو;
			   حالت اظہاریہ_IS:	 p = "is"; توڑو;
			   حالت اظہاریہ_ISNOT:	 p = "isnot"; توڑو;
			   حالت اظہاریہ_NOMATCH:	 p = "!~"; توڑو;
			   طےشدہ:  p = "???"; توڑو;
		       }
		       STRCPY(buf, p);
		       اگر (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       انتخاب(iptr->isn_type)
		       {
			   حالت ISN_COMPAREBOOL: type = "COMPAREBOOL"; توڑو;
			   حالت ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; توڑو;
			   حالت ISN_COMPARENULL: type = "COMPARENULL"; توڑو;
			   حالت ISN_COMPARENR: type = "COMPARENR"; توڑو;
			   حالت ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; توڑو;
			   حالت ISN_COMPARESTRING:
						  type = "COMPARESTRING"; توڑو;
			   حالت ISN_COMPAREBLOB: type = "COMPAREBLOB"; توڑو;
			   حالت ISN_COMPARELIST: type = "COMPARELIST"; توڑو;
			   حالت ISN_COMPARETUPLE: type = "COMPARETUPLE"; توڑو;
			   حالت ISN_COMPAREDICT: type = "COMPAREDICT"; توڑو;
			   حالت ISN_COMPAREFUNC: type = "COMPAREFUNC"; توڑو;
			   حالت ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; توڑو;
			   حالت ISN_COMPAREANY: type = "COMPAREANY"; توڑو;
			   طےشدہ: type = "???"; توڑو;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   توڑو;

	    حالت ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); توڑو;
	    حالت ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); توڑو;
	    حالت ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); توڑو;

	    // expression operations
	    حالت ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.نمبر));
		توڑو;
	    حالت ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); توڑو;
	    حالت ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); توڑو;
	    حالت ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); توڑو;
	    حالت ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); توڑو;
	    حالت ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); توڑو;
	    حالت ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); توڑو;
	    حالت ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); توڑو;
	    حالت ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); توڑو;
	    حالت ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); توڑو;
	    حالت ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); توڑو;
	    حالت ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); توڑو;
	    حالت ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); توڑو;
	    حالت ISN_قاش: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.نمبر); توڑو;
	    حالت ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_اشاریہ,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); توڑو;
	    حالت ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); توڑو;
	    حالت ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); توڑو;
	    حالت ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (صحیح)iptr->isn_arg.classmember.cm_idx);
				     توڑو;
	    حالت ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (صحیح)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     توڑو;
	    حالت ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (صحیح)iptr->isn_arg.نمبر); توڑو;
	    حالت ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); توڑو;
	    حالت ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); توڑو;

	    حالت ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); توڑو;

	    حالت ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      حرف	    *tofree = تہی;
		      حرف	    *typename;

		      اگر (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_نمبر_OK))
			  typename = "float|number";
		      ورنہ اگر (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      ورنہ
			  typename = type_name(ct->ct_type, &tofree);

		      اگر (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (صحیح)ct->ct_off);
		      ورنہ
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (صحیح)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (صحیح)ct->ct_arg_idx);
		      vim_free(tofree);
		      توڑو;
		  }
	    حالت ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_مزید_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       توڑو;
	    حالت ISN_SETTYPE:
		  {
		      حرف *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      توڑو;
		  }
	    حالت ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); توڑو;
	    حالت ISN_2BOOL: اگر (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.انحراف);
			    ورنہ
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.انحراف);
			    توڑو;
	    حالت ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.انحراف));
			      توڑو;
	    حالت ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.انحراف));
			      توڑو;
	    حالت ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    توڑو;
	    حالت ISN_PUT:
		اگر (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ورنہ اگر (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ورنہ
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (طویل)iptr->isn_arg.put.put_lnum);
		توڑو;
	    حالت ISN_IPUT:
		اگر (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ورنہ اگر (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ورنہ
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (طویل)iptr->isn_arg.put.put_lnum);
		توڑو;
	    حالت ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  تہی, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    اگر (likely(buf != تہی))
		    {
			(خالی)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    توڑو;
		}
	    حالت ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); توڑو;

	    حالت ISN_PROF_آغاز:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 توڑو;

	    حالت ISN_PROF_انتہا:
		smsg("%s%4d PROFILE END", pfx, current);
		توڑو;

	    حالت ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_نام_len);
		توڑو;

	    حالت ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_گنتی,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      توڑو;
	    حالت ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_شے,
					 iptr->isn_arg.shuffle.shfl_اوپر);
			      توڑو;
	    حالت ISN_DROP: smsg("%s%4d DROP", pfx, current); توڑو;

	    حالت ISN_SCRIPTCTX_سیٹ:
		{
		    صحیح	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_شے(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		توڑو;

	    حالت ISN_اختتام: // End of list of instructions for ISN_SUBSTITUTE.
			   واپس;
	}

	برآمد_flush();	    // output one line at a time
	ui_breakcheck();
	اگر (got_int)
	    توڑو;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    خالی
سیٹ_context_اندر_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_نمونہ = arg;

    // first argument already typed: only user function names
    اگر (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_نمونہ = skipwhite(p);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the list of :disassemble
 * arguments.
 */
    char_u *
get_disassemble_دلیل(expand_T *xp, صحیح idx)
{
    اگر (idx == 0)
	واپس (char_u *)"debug";
    اگر (idx == 1)
	واپس (char_u *)"profile";
    واپس get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    خالی
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = تہی;  // init to shut up gcc warning
    صحیح		instr_گنتی = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = ڈھونڈو_func_by_name(arg, &compile_type);
    اگر (ufunc == تہی)
	واپس;
    اگر (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, تہی) == FAIL)
	واپس;
    اگر (ufunc->uf_def_حالت != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	واپس;
    }
    پیغام((حرف *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_ڈیٹا) + ufunc->uf_dfunc_idx;
    انتخاب (compile_type)
    {
	حالت CT_PROFILE:
#اگر_تعریف_شدہ FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_گنتی = dfunc->df_instr_prof_گنتی;
	    توڑو;
#اگر_ختم
	    // FALLTHROUGH
	حالت CT_NONE:
	    instr = dfunc->df_instr;
	    instr_گنتی = dfunc->df_instr_گنتی;
	    توڑو;
	حالت CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_گنتی = dfunc->df_instr_debug_گنتی;
	    توڑو;
    }

    list_instructions("", instr, instr_گنتی, ufunc);
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
	حالت VAR_نمبر:
	    واپس tv->vval.v_نمبر != 0;
	حالت VAR_FLOAT:
	    واپس tv->vval.v_float != 0.0;
	حالت VAR_PARTIAL:
	    واپس tv->vval.v_partial != تہی;
	حالت VAR_FUNC:
	حالت VAR_STRING:
	    واپس tv->vval.v_string != تہی && *tv->vval.v_string != NUL;
	حالت VAR_LIST:
	    واپس tv->vval.v_list != تہی && tv->vval.v_list->lv_len > 0;
	حالت VAR_TUPLE:
	    واپس tuple_len(tv->vval.v_tuple) > 0;
	حالت VAR_DICT:
	    واپس tv->vval.v_dict != تہی
				    && tv->vval.v_dict->dv_hashtab.ht_مستعمل > 0;
	حالت VAR_BOOL:
	حالت VAR_SPECIAL:
	    واپس tv->vval.v_نمبر == VVAL_TRUE ? TRUE : FALSE;
	حالت VAR_کام:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    واپس tv->vval.v_کام != تہی;
#ورنہ
	    توڑو;
#اگر_ختم
	حالت VAR_CHANNEL:
#اگر_تعریف_شدہ FEAT_کام_CHANNEL
	    واپس tv->vval.v_channel != تہی;
#ورنہ
	    توڑو;
#اگر_ختم
	حالت VAR_BLOB:
	    واپس tv->vval.v_blob != تہی && tv->vval.v_blob->bv_ga.ga_len > 0;

	حالت VAR_OBJECT:
	    واپس tv->vval.v_object != تہی;

	حالت VAR_CLASS:
	حالت VAR_TYPEALIAS:
	    check_typval_is_قدر(tv);
	    توڑو;

	حالت VAR_UNKNOWN:
	حالت VAR_ANY:
	حالت VAR_VOID:
	حالت VAR_INSTR:
	    توڑو;
    }
    واپس FALSE;
}

    خالی
emsg_using_string_as(typval_T *tv, صحیح as_نمبر)
{
    semsg(_(as_نمبر ? e_using_string_as_نمبر_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == تہی
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
	واپس FAIL;
    }
    واپس OK;
}

#اگر_ختم // FEAT_EVAL
