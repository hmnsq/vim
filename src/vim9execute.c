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

#నిర్వచించు USING_FLOAT_STUFF
#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం	    tcd_చట్రం_idx;	// ec_frame_idx at ISN_TRY
    పూర్ణాంకం	    tcd_దొంతర_len;	// size of ectx.ec_stack at ISN_TRY
    పూర్ణాంకం	    tcd_లోపల_catch;	// in catch or finally block
    పూర్ణాంకం	    tcd_did_throw;	// set did_throw in :endtry
    పూర్ణాంకం	    tcd_catch_idx;	// instruction of the first :catch or :finally
    పూర్ణాంకం	    tcd_finally_idx;	// instruction of the :finally block or zero
    పూర్ణాంకం	    tcd_endtry_idx;	// instruction of the :endtry
    పూర్ణాంకం	    tcd_caught;		// catch block entered
    పూర్ణాంకం	    tcd_cont;		// :continue encountered, jump here (minus one)
    పూర్ణాంకం	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
రకనిర్వచనం నిర్మాణం {
    పూర్ణాంకం		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    పూర్ణాంకం		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
రకనిర్వచనం నిర్మాణం {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    పూర్ణాంకం		or_outer_allocated;  // free "or_outer" later
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
నిర్మాణం ectx_S {
    garray_T	ec_దొంతర;	// stack of typval_T values
    పూర్ణాంకం		ec_చట్రం_idx;	// index in ec_stack: context of ec_dfunc_idx
    పూర్ణాంకం		ec_initial_చట్రం_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    పూర్ణాంకం		ec_dfunc_idx;	// current function index
    పూర్ణాంకం		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    పూర్ణాంకం		ec_did_emsg_before;
    పూర్ణాంకం		ec_trylevel_at_ప్రారంభం;
    where_T	ec_where;
};

#నిర్వచితమైతే FEAT_PROFILE
// stack of profinfo_T used when profiling.
స్థిర garray_T profile_info_ga = {0, 0, పరిమాణం(profinfo_T), 20, ఖాళీ};
#అయితే_ముగింపు

// Get pointer to item in the stack.
#నిర్వచించు దొంతర_TV(idx) (((typval_T *)ectx->ec_దొంతర.ga_దత్తాంశం) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#నిర్వచించు దొంతర_TV_BOT(idx) (((typval_T *)ectx->ec_దొంతర.ga_దత్తాంశం) + ectx->ec_దొంతర.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#నిర్వచించు దొంతర_TV_VAR(idx) (((typval_T *)ectx->ec_దొంతర.ga_దత్తాంశం) + ectx->ec_చట్రం_idx + దొంతర_చట్రం_కొలత + idx)

// Return value for functions used to execute instructions
#నిర్వచించు EXEC_FAIL	0
#నిర్వచించు EXEC_OK		1
#నిర్వచించు EXEC_పూర్తి	2

    శూన్య
గమ్యం_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_గమ్యం_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    స్థిర పూర్ణాంకం
ufunc_argcount(ufunc_T *ufunc)
{
    తిరిగి ufunc->uf_వాదనలు.ga_len + (ufunc->uf_va_name != ఖాళీ ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    స్థిర పూర్ణాంకం
exe_concat(పూర్ణాంకం లెక్క, ectx_T *ectx)
{
    పూర్ణాంకం		idx;
    size_t	len = 0;
    garray_T	ga;
    రకనిర్వచనం నిర్మాణం
    {
	typval_T    *tv;
	size_t	    నిడివి;
    } string_segment_T;
    గణన
    {
	STRING_SEGMENT_CACHE_కొలత = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_కొలత];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    ఒకవేళ (లెక్క > (పూర్ణాంకం)ARRAY_నిడివి(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, లెక్క);
	ఒకవేళ (string_segment_tab == ఖాళీ)
	    తిరిగి FAIL;
    }

    ga_init2(&ga, పరిమాణం(అక్షరం), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    కొరకు (idx = 0; idx < లెక్క; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = దొంతర_TV_BOT(idx - లెక్క);
	ఒకవేళ (segment->tv->vval.v_string != ఖాళీ)
	{
	    segment->నిడివి = STRLEN(segment->tv->vval.v_string);
	    len += segment->నిడివి;
	}
	లేకపోతే
	    segment->నిడివి = 0;    // Ensure clean state for the second pass
    }

    ఒకవేళ (ga_grow(&ga, (పూర్ణాంకం)len + 1) == FAIL)
    {
	ఒకవేళ (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	తిరిగి FAIL;
    }

    కొరకు (idx = 0; idx < లెక్క; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->నిడివి);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_దొంతర.ga_len -= లెక్క - 1;
    దొంతర_TV_BOT(-1)->vval.v_string = ga.ga_దత్తాంశం;

    ఒకవేళ (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    తిరిగి OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    స్థిర పూర్ణాంకం
exe_newlist(పూర్ణాంకం లెక్క, ectx_T *ectx)
{
    list_T	*list = ఖాళీ;
    పూర్ణాంకం		idx;
    typval_T	*tv;

    ఒకవేళ (లెక్క >= 0)
    {
	list = list_alloc_with_వస్తువులు(లెక్క);
	ఒకవేళ (list == ఖాళీ)
	    తిరిగి FAIL;
	కొరకు (idx = 0; idx < లెక్క; ++idx)
	    list_సమితి_అంశం(list, idx, దొంతర_TV_BOT(idx - లెక్క));
    }

    ఒకవేళ (లెక్క > 0)
	ectx->ec_దొంతర.ga_len -= లెక్క - 1;
    లేకపోతే ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
    {
	list_unref(list);
	తిరిగి FAIL;
    }
    లేకపోతే
	++ectx->ec_దొంతర.ga_len;
    tv = దొంతర_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_తాళం = 0;
    ఒకవేళ (list != ఖాళీ)
	++list->lv_refcount;
    తిరిగి OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    స్థిర పూర్ణాంకం
exe_newtuple(పూర్ణాంకం లెక్క, ectx_T *ectx)
{
    tuple_T	*tuple = ఖాళీ;
    పూర్ణాంకం		idx;
    typval_T	*tv;

    ఒకవేళ (లెక్క >= 0)
    {
	tuple = tuple_alloc_with_వస్తువులు(లెక్క);
	ఒకవేళ (tuple == ఖాళీ)
	    తిరిగి FAIL;
	కొరకు (idx = 0; idx < లెక్క; ++idx)
	    tuple_సమితి_అంశం(tuple, idx, దొంతర_TV_BOT(idx - లెక్క));
    }

    ఒకవేళ (లెక్క > 0)
	ectx->ec_దొంతర.ga_len -= లెక్క - 1;
    లేకపోతే ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
    {
	tuple_unref(tuple);
	తిరిగి FAIL;
    }
    లేకపోతే
	++ectx->ec_దొంతర.ga_len;
    tv = దొంతర_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_తాళం = 0;
    ఒకవేళ (tuple != ఖాళీ)
	++tuple->tv_refcount;

    తిరిగి OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    స్థిర పూర్ణాంకం
exe_newdict(పూర్ణాంకం లెక్క, ectx_T *ectx)
{
    dict_T	*dict = ఖాళీ;
    dictitem_T	*అంశం;
    char_u	*కీలకం;
    పూర్ణాంకం		idx;
    typval_T	*tv;

    ఒకవేళ (లెక్క >= 0)
    {
	dict = dict_alloc();
	ఒకవేళ (unlikely(dict == ఖాళీ))
	    తిరిగి FAIL;
	కొరకు (idx = 0; idx < లెక్క; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = దొంతర_TV_BOT(2 * (idx - లెక్క));
	    // check key is unique
	    కీలకం = tv->vval.v_string == ఖాళీ ? (char_u *)"" : tv->vval.v_string;
	    అంశం = dict_కనుగొను(dict, కీలకం, -1);
	    ఒకవేళ (అంశం != ఖాళీ)
	    {
		semsg(_(e_duplicate_కీలకం_లోపల_dictionary_str), కీలకం);
		dict_unref(dict);
		తిరిగి MAYBE;
	    }
	    అంశం = dictitem_alloc(కీలకం);
	    clear_tv(tv);
	    ఒకవేళ (unlikely(అంశం == ఖాళీ))
	    {
		dict_unref(dict);
		తిరిగి FAIL;
	    }
	    tv = దొంతర_TV_BOT(2 * (idx - లెక్క) + 1);
	    అంశం->di_tv = *tv;
	    అంశం->di_tv.v_తాళం = 0;
	    tv->v_type = VAR_UNKNOWN;
	    ఒకవేళ (dict_కూడు(dict, అంశం) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(అంశం);
		dict_unref(dict);
		తిరిగి FAIL;
	    }
	}
    }

    ఒకవేళ (లెక్క > 0)
	ectx->ec_దొంతర.ga_len -= 2 * లెక్క - 1;
    లేకపోతే ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
    {
	dict_unref(dict);
	తిరిగి FAIL;
    }
    లేకపోతే
	++ectx->ec_దొంతర.ga_len;
    tv = దొంతర_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_తాళం = 0;
    tv->vval.v_dict = dict;
    ఒకవేళ (dict != ఖాళీ)
	++dict->dv_refcount;
    తిరిగి OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    శూన్య
update_has_breakpoint(ufunc_T *ufunc)
{
    ఒకవేళ (ufunc->uf_debug_tick == debug_tick)
	తిరిగి;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_కనుగొను_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

స్థిర garray_T dict_దొంతర = GA_ఖాళీలు;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    స్థిర పూర్ణాంకం
dict_దొంతర_save(typval_T *tv)
{
    ఒకవేళ (dict_దొంతర.ga_growsize == 0)
	ga_init2(&dict_దొంతర, పరిమాణం(typval_T), 10);
    ఒకవేళ (ga_grow(&dict_దొంతర, 1) == FAIL)
	తిరిగి FAIL;
    ((typval_T *)dict_దొంతర.ga_దత్తాంశం)[dict_దొంతర.ga_len] = *tv;
    ++dict_దొంతర.ga_len;
    తిరిగి OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    స్థిర typval_T *
dict_దొంతర_get_tv(శూన్య)
{
    ఒకవేళ (dict_దొంతర.ga_len == 0)
	తిరిగి ఖాళీ;
    తిరిగి ((typval_T *)dict_దొంతర.ga_దత్తాంశం) + dict_దొంతర.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    స్థిర dict_T *
dict_దొంతర_get_dict(శూన్య)
{
    typval_T *tv;

    ఒకవేళ (dict_దొంతర.ga_len == 0)
	తిరిగి ఖాళీ;
    tv = ((typval_T *)dict_దొంతర.ga_దత్తాంశం) + dict_దొంతర.ga_len - 1;
    ఒకవేళ (tv->v_type == VAR_DICT)
	తిరిగి tv->vval.v_dict;
    తిరిగి ఖాళీ;
}

/*
 * Drop an item from the dict stack.
 */
    స్థిర శూన్య
dict_దొంతర_drop(శూన్య)
{
    ఒకవేళ (dict_దొంతర.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	తిరిగి;
    }
    --dict_దొంతర.ga_len;
    clear_tv(((typval_T *)dict_దొంతర.ga_దత్తాంశం) + dict_దొంతర.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    స్థిర శూన్య
dict_దొంతర_clear(పూర్ణాంకం len)
{
    అయితే (dict_దొంతర.ga_len > len)
	dict_దొంతర_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    స్థిర outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    ఒకవేళ (ptref == ఖాళీ)
	తిరిగి &pt->pt_outer;

    // partial using partial (recursively)
    అయితే (ptref->pt_outer_partial != ఖాళీ)
	ptref = ptref->pt_outer_partial;
    తిరిగి &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    స్థిర పూర్ణాంకం
check_ufunc_arg_types(ufunc_T *ufunc, పూర్ణాంకం argcount, పూర్ణాంకం off, ectx_T *ectx)
{
    ఒకవేళ (ufunc->uf_arg_types == ఖాళీ && ufunc->uf_va_type == ఖాళీ)
	తిరిగి OK;

    typval_T	*argv = దొంతర_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    కొరకు (పూర్ణాంకం i = 0; i < argcount; ++i)
    {
	type_T *type = ఖాళీ;

	// assume a v:none argument, using the default value, is always OK
	ఒకవేళ (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_సంఖ్య == VVAL_NONE)
	    కొనసాగు;
	// only pass values to user functions, never types
	ఒకవేళ (check_typval_is_విలువ(&argv[i]) == FAIL)
	    తిరిగి FAIL;

	ఒకవేళ (i < ufunc->uf_వాదనలు.ga_len && ufunc->uf_arg_types != ఖాళీ)
	    type = ufunc->uf_arg_types[i];
	లేకపోతే ఒకవేళ (ufunc->uf_va_type != ఖాళీ)
	    type = ufunc->uf_va_type->tt_member;
	ఒకవేళ (type != ఖాళీ && check_typval_arg_type(type,
		    &argv[i], ఖాళీ, i + 1) == FAIL)
	    తిరిగి FAIL;
    }
    తిరిగి OK;
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
    స్థిర పూర్ణాంకం
call_dfunc(
	పూర్ణాంకం		cdf_idx,
	partial_T	*pt_arg,
	పూర్ణాంకం		argcount_arg,
	ectx_T		*ectx)
{
    పూర్ణాంకం		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    పూర్ణాంకం		did_emsg_before = did_emsg_cumul + did_emsg;
    పూర్ణాంకం		arg_గమ్యం_కూడు;
    పూర్ణాంకం		vararg_లెక్క = 0;
    పూర్ణాంకం		varcount;
    పూర్ణాంకం		idx;
    estack_T	*నమోదు_2;
    funclocal_T	*floc = ఖాళీ;
    పూర్ణాంకం		res = OK;
    compiletype_T compile_type;

    ఒకవేళ (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == ఖాళీ ? (char_u *)"unknown" : dfunc->df_name);
	తిరిగి FAIL;
    }

#నిర్వచితమైతే FEAT_PROFILE
    ఒకవేళ (do_profiling == PROF_YES)
    {
	ఒకవేళ (GA_GROW_OK(&profile_info_ga, 1))
	{
	    profinfo_T *info = ((profinfo_T *)profile_info_ga.ga_దత్తాంశం)
						      + profile_info_ga.ga_len;
	    ++profile_info_ga.ga_len;
	    CLEAR_సూచిక(info);
	    profile_may_ప్రారంభం_func(info, ufunc,
			(((dfunc_T *)def_functions.ga_దత్తాంశం)
					      + ectx->ec_dfunc_idx)->df_ufunc);
	}
    }
#అయితే_ముగింపు

    // When debugging and using "cont" switches to the not-debugged
    // instructions, may need to still compile them.
    compile_type = get_compile_type(ufunc);
    ఒకవేళ (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, ఖాళీ);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + cdf_idx;
    }
    ఒకవేళ (res == FAIL || INSTRUCTIONS(dfunc) == ఖాళీ)
    {
	ఒకవేళ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	తిరిగి FAIL;
    }

    ఒకవేళ (ufunc->uf_va_name != ఖాళీ)
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
	vararg_లెక్క = argcount - ufunc->uf_వాదనలు.ga_len;
	ఒకవేళ (vararg_లెక్క < 0)
	    vararg_లెక్క = 0;
	లేకపోతే
	    argcount -= vararg_లెక్క;
	ఒకవేళ (exe_newlist(vararg_లెక్క, ectx) == FAIL)
	    తిరిగి FAIL;

	vararg_లెక్క = 1;
    }

    arg_గమ్యం_కూడు = ufunc->uf_వాదనలు.ga_len - argcount;
    ఒకవేళ (arg_గమ్యం_కూడు < 0)
    {
	semsg(NGETTEXT(e_one_వాదం_too_many, e_nr_arguments_too_many,
			-arg_గమ్యం_కూడు), -arg_గమ్యం_కూడు);
	తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (arg_గమ్యం_కూడు > ufunc->uf_def_వాదనలు.ga_len)
    {
	పూర్ణాంకం missing = arg_గమ్యం_కూడు - ufunc->uf_def_వాదనలు.ga_len;

	semsg(NGETTEXT(e_one_వాదం_too_few, e_nr_arguments_too_few,
			missing), missing);
	తిరిగి FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = దొంతర_TV_BOT(0) - argcount - vararg_లెక్క - 1;

    ఒకవేళ (IS_OBJECT_పద్ధతి(ufunc) && !IS_CONSTRUCTOR_పద్ధతి(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == ఖాళీ)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	తిరిగి FAIL;
    }

    // Check the argument types.
    ఒకవేళ (check_ufunc_arg_types(ufunc, argcount, vararg_లెక్క, ectx) == FAIL)
	తిరిగి FAIL;

    // While check_ufunc_arg_types call, def function compilation process may
    // run.  If so many def functions are compiled, def_functions array may be
    // reallocated and dfunc may no longer have valid pointer.  Get the object
    // pointer from def_functions again here.
    dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + cdf_idx;

    // Reserve space for:
    // - missing arguments
    // - stack frame
    // - local variables
    // - if needed: a counter for number of closures created in
    //   ectx->ec_funcrefs.
    varcount = dfunc->df_varcount + dfunc->df_has_సంవృతం;
    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర,
				     arg_గమ్యం_కూడు + దొంతర_చట్రం_కొలత + varcount))
	తిరిగి FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = దొంతర_TV_BOT(0) - argcount - vararg_లెక్క - 1;

    // If depth of calling is getting too high, don't execute the function.
    ఒకవేళ (funcdepth_increment() == FAIL)
	తిరిగి FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    ఒకవేళ (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	ఒకవేళ (floc == ఖాళీ)
	    తిరిగి FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    ఒకవేళ (vararg_లెక్క > 0 && arg_గమ్యం_కూడు > 0)
	*దొంతర_TV_BOT(arg_గమ్యం_కూడు - 1) = *దొంతర_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    కొరకు (idx = 0; idx < arg_గమ్యం_కూడు; ++idx)
	దొంతర_TV_BOT(idx - vararg_లెక్క)->v_type = VAR_UNKNOWN;
    ectx->ec_దొంతర.ga_len += arg_గమ్యం_కూడు;

    // Store current execution state in stack frame for ISN_RETURN.
    దొంతర_TV_BOT(దొంతర_చట్రం_FUNC_OFF)->vval.v_సంఖ్య = ectx->ec_dfunc_idx;
    దొంతర_TV_BOT(దొంతర_చట్రం_IIDX_OFF)->vval.v_సంఖ్య = ectx->ec_iidx;
    దొంతర_TV_BOT(దొంతర_చట్రం_INSTR_OFF)->vval.v_string = (శూన్య *)ectx->ec_instr;
    దొంతర_TV_BOT(దొంతర_చట్రం_OUTER_OFF)->vval.v_string =
						    (శూన్య *)ectx->ec_outer_ref;
    దొంతర_TV_BOT(దొంతర_చట్రం_FUNCLOCAL_OFF)->vval.v_string = (శూన్య *)floc;
    దొంతర_TV_BOT(దొంతర_చట్రం_IDX_OFF)->vval.v_సంఖ్య = ectx->ec_చట్రం_idx;
    ectx->ec_చట్రం_idx = ectx->ec_దొంతర.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    పూర్ణాంకం initcount = dfunc->df_varcount + (dfunc->df_has_సంవృతం ? 1 : 0);
    కొరకు (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = దొంతర_TV_BOT(దొంతర_చట్రం_కొలత + idx);

	tv->v_type = VAR_సంఖ్య;
	tv->vval.v_సంఖ్య = 0;
    }
    ectx->ec_దొంతర.ga_len += దొంతర_చట్రం_కొలత + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    ఒకవేళ (IS_OBJECT_పద్ధతి(ufunc))
    {
	ఒకవేళ (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_అంతర్గత_error_str), "type in stack is not an object");
	    తిరిగి FAIL;
	}

	*దొంతర_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != ఖాళీ ? pt_arg : ufunc->uf_partial;
    ఒకవేళ (pt != ఖాళీ || (ufunc->uf_flags & FC_సంవృతం))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	ఒకవేళ (ref == ఖాళీ)
	    తిరిగి FAIL;
	ఒకవేళ (pt != ఖాళీ)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	లేకపోతే
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    ఒకవేళ (unlikely(ref->or_outer == ఖాళీ))
	    {
		vim_free(ref);
		తిరిగి FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->బహిర్గతం_దొంతర = &ectx->ec_దొంతర;
	    ref->or_outer->బహిర్గతం_చట్రం_idx = ectx->ec_చట్రం_idx;
	    ఒకవేళ (ectx->ec_outer_ref != ఖాళీ)
		ref->or_outer->బహిర్గతం_పైకి = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    లేకపోతే
	ectx->ec_outer_ref = ఖాళీ;

    ఒకవేళ (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    నమోదు_2 = estack_నెట్టు_ufunc(ufunc, 1);
    ఒకవేళ (నమోదు_2 != ఖాళీ)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	నమోదు_2->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    తిరిగి OK;
}

// Double linked list of funcstack_T in use.
స్థిర funcstack_T *మొదటి_funcstack = ఖాళీ;

    స్థిర శూన్య
కూడు_funcstack_గమ్యం_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    ఒకవేళ (మొదటి_funcstack != ఖాళీ)
	మొదటి_funcstack->fs_prev = funcstack;
    funcstack->fs_తదుపరి = మొదటి_funcstack;
    funcstack->fs_prev = ఖాళీ;
    మొదటి_funcstack = funcstack;
}

    స్థిర శూన్య
remove_funcstack_from_list(funcstack_T *funcstack)
{
    ఒకవేళ (funcstack->fs_prev == ఖాళీ)
	మొదటి_funcstack = funcstack->fs_తదుపరి;
    లేకపోతే
	funcstack->fs_prev->fs_తదుపరి = funcstack->fs_తదుపరి;
    ఒకవేళ (funcstack->fs_తదుపరి != ఖాళీ)
	funcstack->fs_తదుపరి->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    స్థిర పూర్ణాంకం
handle_సంవృతం_లోపల_use(ectx_T *ectx, పూర్ణాంకం free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							  + ectx->ec_dfunc_idx;
    పూర్ణాంకం		argcount;
    పూర్ణాంకం		శిఖరం;
    పూర్ణాంకం		idx;
    typval_T	*tv;
    పూర్ణాంకం		సంవృతం_లోపల_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	సంవృతం_లెక్క;

    ఒకవేళ (dfunc->df_ufunc == ఖాళీ)
	తిరిగి OK;  // function was freed
    ఒకవేళ (dfunc->df_has_సంవృతం == 0)
	తిరిగి OK;  // no closures
    tv = దొంతర_TV(ectx->ec_చట్రం_idx + దొంతర_చట్రం_కొలత + dfunc->df_varcount);
    సంవృతం_లెక్క = tv->vval.v_సంఖ్య;
    ఒకవేళ (సంవృతం_లెక్క == 0)
	తిరిగి OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    శిఖరం = ectx->ec_చట్రం_idx - argcount;

    // Check if any created closure is still in use.
    కొరకు (idx = 0; idx < సంవృతం_లెక్క; ++idx)
    {
	partial_T   *pt;
	పూర్ణాంకం	    off = gap->ga_len - సంవృతం_లెక్క + idx;

	ఒకవేళ (off < 0)
	    కొనసాగు;  // count is off or already done
	pt = ((partial_T **)gap->ga_దత్తాంశం)[off];
	ఒకవేళ (pt->pt_refcount > 1)
	{
	    పూర్ణాంకం refcount = pt->pt_refcount;
	    పూర్ణాంకం i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    కొరకు (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = దొంతర_TV(ectx->ec_చట్రం_idx
						       + దొంతర_చట్రం_కొలత + i);
		ఒకవేళ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ఒకవేళ (refcount > 1)
	    {
		సంవృతం_లోపల_use = TRUE;
		ఆపు;
	    }
	}
    }

    ఒకవేళ (సంవృతం_లోపల_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *దొంతర;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	ఒకవేళ (funcstack == ఖాళీ)
	    తిరిగి FAIL;

	funcstack->fs_var_స్థానభ్రంశం = argcount + దొంతర_చట్రం_కొలత;
	funcstack->fs_ga.ga_len = funcstack->fs_var_స్థానభ్రంశం
							  + dfunc->df_varcount;
	దొంతర = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_దత్తాంశం = దొంతర;
	ఒకవేళ (దొంతర == ఖాళీ)
	{
	    vim_free(funcstack);
	    తిరిగి FAIL;
	}
	కూడు_funcstack_గమ్యం_list(funcstack);

	// Move or copy the arguments.
	కొరకు (idx = 0; idx < argcount; ++idx)
	{
	    tv = దొంతర_TV(శిఖరం + idx);
	    ఒకవేళ (free_arguments)
	    {
		*(దొంతర + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    లేకపోతే ఒకవేళ (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, దొంతర + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	కొరకు (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = దొంతర_TV(ectx->ec_చట్రం_idx + దొంతర_చట్రం_కొలత + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    ఒకవేళ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ఖాళీ)
	    {
		పూర్ణాంకం	    i;

		కొరకు (i = 0; i < సంవృతం_లెక్క; ++i)
		    ఒకవేళ (tv->vval.v_partial == ((partial_T **)gap->ga_దత్తాంశం)[
					      gap->ga_len - సంవృతం_లెక్క + i])
			++funcstack->fs_min_refcount;
	    }

	    *(దొంతర + funcstack->fs_var_స్థానభ్రంశం + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	కొరకు (idx = 0; idx < సంవృతం_లెక్క; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_దత్తాంశం)[gap->ga_len
							- సంవృతం_లెక్క + idx];
	    ఒకవేళ (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.బహిర్గతం_దొంతర = &funcstack->fs_ga;
		pt->pt_outer.బహిర్గతం_చట్రం_idx = ectx->ec_చట్రం_idx - శిఖరం;
	    }
	}
    }

    కొరకు (idx = 0; idx < సంవృతం_లెక్క; ++idx)
	partial_unref(((partial_T **)gap->ga_దత్తాంశం)[gap->ga_len
						       - సంవృతం_లెక్క + idx]);
    gap->ga_len -= సంవృతం_లెక్క;
    ఒకవేళ (gap->ga_len == 0)
	ga_clear(gap);

    తిరిగి OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    పూర్ణాంకం
funcstack_check_refcount(funcstack_T *funcstack)
{
    పూర్ణాంకం		i;
    garray_T	*gap = &funcstack->fs_ga;
    పూర్ణాంకం		పూర్తి = 0;
    typval_T	*దొంతర;

    ఒకవేళ (funcstack->fs_refcount > funcstack->fs_min_refcount)
	తిరిగి FALSE;
    కొరకు (i = funcstack->fs_var_స్థానభ్రంశం; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_దత్తాంశం) + i;

	ఒకవేళ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ఖాళీ
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++పూర్తి;
    }
    ఒకవేళ (పూర్తి != funcstack->fs_min_refcount)
	తిరిగి FALSE;

    దొంతర = gap->ga_దత్తాంశం;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    కొరకు (i = 0; i < gap->ga_len; ++i)
	clear_tv(దొంతర + i);
    vim_free(దొంతర);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    తిరిగి TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_funcstacks(పూర్ణాంకం copyID)
{
    funcstack_T *funcstack;

    కొరకు (funcstack = మొదటి_funcstack; funcstack != ఖాళీ;
						funcstack = funcstack->fs_తదుపరి)
    {
	typval_T    *దొంతర = funcstack->fs_ga.ga_దత్తాంశం;
	పూర్ణాంకం	    i;

	కొరకు (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    ఒకవేళ (సమితి_ref_లోపల_అంశం(దొంతర + i, copyID, ఖాళీ, ఖాళీ, ఖాళీ))
		తిరిగి TRUE;  // abort
    }
    తిరిగి FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
స్థిర ectx_T *current_ectx = ఖాళీ;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    పూర్ణాంకం
లోపల_def_function(శూన్య)
{
    తిరిగి current_ectx != ఖాళీ;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    పూర్ణాంకం
fill_exec_lval_మూలం(lval_మూలం_T *మూలం)
{
    ectx_T *ectx = current_ectx;
    ఒకవేళ (ectx != ఖాళీ)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	ఒకవేళ (ufunc->uf_class != ఖాళీ)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    ఒకవేళ (tv != ఖాళీ)
	    {
		CLEAR_సూచిక(మూలం);
		మూలం->lr_tv = tv;
		copy_tv(దొంతర_TV_VAR(0), మూలం->lr_tv);
		మూలం->lr_cl_exec = ufunc->uf_class;
		++మూలం->lr_cl_exec->class_refcount;
		తిరిగి OK;
	    }
	}
    }

    తిరిగి FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(శూన్య)
{
    ectx_T *r = current_ectx;

    current_ectx = ఖాళీ;
    తిరిగి r;
}

    శూన్య
restore_current_ectx(ectx_T *ectx)
{
    ఒకవేళ (current_ectx != ఖాళీ)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    స్థిర list_T *
కూడు_defer_అంశం(పూర్ణాంకం var_idx, పూర్ణాంకం argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = దొంతర_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    ఒకవేళ (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	ఒకవేళ (rettv_list_alloc(defer_tv) == FAIL)
	    తిరిగి ఖాళీ;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_వస్తువులు(argcount + 1);
    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_తాళం = 0;
    ఒకవేళ (list_చొప్పించు_tv(defer_l, &listval, defer_l->lv_మొదటి) == FAIL)
    {
	list_free(l);
	తిరిగి ఖాళీ;
    }

    తిరిగి l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    స్థిర పూర్ణాంకం
defer_command(పూర్ణాంకం var_idx, పూర్ణాంకం argcount, ectx_T *ectx)
{
    list_T	*l = కూడు_defer_అంశం(var_idx, argcount, ectx);
    పూర్ణాంకం		i;
    typval_T	*func_tv;

    ఒకవేళ (l == ఖాళీ)
	తిరిగి FAIL;

    func_tv = దొంతర_TV_BOT(-argcount - 1);
    ఒకవేళ (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	తిరిగి FAIL;
    }
    list_సమితి_అంశం(l, 0, func_tv);

    కొరకు (i = 0; i < argcount; ++i)
	list_సమితి_అంశం(l, i + 1, దొంతర_TV_BOT(-argcount + i));
    ectx->ec_దొంతర.ga_len -= argcount + 1;
    తిరిగి OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    పూర్ణాంకం
కూడు_defer_function(char_u *name, పూర్ణాంకం argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    పూర్ణాంకం		i;

    ఒకవేళ (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	తిరిగి FAIL;
    }

    l = కూడు_defer_అంశం(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    ఒకవేళ (l == ఖాళీ)
    {
	vim_free(name);
	తిరిగి FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_తాళం = 0;
    func_tv.vval.v_string = name;
    list_సమితి_అంశం(l, 0, &func_tv);

    కొరకు (i = 0; i < argcount; ++i)
	list_సమితి_అంశం(l, i + 1, argvars + i);
    తిరిగి OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    స్థిర శూన్య
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = దొంతర_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    ఒకవేళ (defer_tv->v_type != VAR_LIST)
	తిరిగి;	 // no function added
    FOR_ALL_LIST_వస్తువులు(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_వాదనలు];
	పూర్ణాంకం	    i;
	listitem_T  *arg_li = l->lv_మొదటి;
	typval_T    *functv = &l->lv_మొదటి->li_tv;
	పూర్ణాంకం	    argcount = l->lv_len - 1;

	ఒకవేళ (functv->vval.v_string == ఖాళీ)
	    // already being called, can happen if function does ":qa"
	    కొనసాగు;

	కొరకు (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_తదుపరి;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = ఖాళీ;
	partial_T	*pt = ఖాళీ;

	CLEAR_క్షేత్రం(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	ఒకవేళ (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = ఖాళీ;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    ఒకవేళ (funcexe.fe_object != ఖాళీ)
		++funcexe.fe_object->obj_refcount;
	}
	లేకపోతే
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = ఖాళీ;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(శూన్య)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	ఒకవేళ (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	లేకపోతే
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    స్థిర పూర్ణాంకం
func_return(ectx_T *ectx)
{
    పూర్ణాంకం		idx;
    పూర్ణాంకం		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							  + ectx->ec_dfunc_idx;
    పూర్ణాంకం		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*నమోదు_2;
    పూర్ణాంకం		prev_dfunc_idx = దొంతర_TV(ectx->ec_చట్రం_idx
					+ దొంతర_చట్రం_FUNC_OFF)->vval.v_సంఖ్య;
    funclocal_T	*floc;
#నిర్వచితమైతే FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							      + prev_dfunc_idx;

    ఒకవేళ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	ఒకవేళ (dfunc->df_ufunc->uf_profiling
				   || (caller != ఖాళీ && caller->uf_profiling))
	{
	    profile_may_చివర_func(((profinfo_T *)profile_info_ga.ga_దత్తాంశం)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#అయితే_ముగింపు

    ఒకవేళ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    ఒకవేళ (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    నమోదు_2 = estack_తీయి();
    ఒకవేళ (నమోదు_2 != ఖాళీ)
	current_sctx = నమోదు_2->es_save_sctx;

    ఒకవేళ (handle_సంవృతం_లోపల_use(ectx, TRUE) == FAIL)
	తిరిగి FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    పూర్ణాంకం శిఖరం = ectx->ec_చట్రం_idx - argcount;
    ఒకవేళ (IS_OBJECT_పద్ధతి(dfunc->df_ufunc))
	--శిఖరం;
    కొరకు (idx = శిఖరం; idx < ectx->ec_చట్రం_idx; ++idx)
	clear_tv(దొంతర_TV(idx));

    // Clear local variables and temp values, but not the return value.
    కొరకు (idx = ectx->ec_చట్రం_idx + దొంతర_చట్రం_కొలత;
					idx < ectx->ec_దొంతర.ga_len - 1; ++idx)
	clear_tv(దొంతర_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_దొంతర.ga_len - 1;
    ఒకవేళ (ret_idx == ectx->ec_చట్రం_idx + దొంతర_చట్రం_IDX_OFF)
	ret_idx = 0;

    ఒకవేళ (ectx->ec_outer_ref != ఖాళీ)
    {
	ఒకవేళ (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = దొంతర_TV(ectx->ec_చట్రం_idx
					+ దొంతర_చట్రం_IIDX_OFF)->vval.v_సంఖ్య;
    ectx->ec_instr = (శూన్య *)దొంతర_TV(ectx->ec_చట్రం_idx
				       + దొంతర_చట్రం_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (శూన్య *)దొంతర_TV(ectx->ec_చట్రం_idx
				       + దొంతర_చట్రం_OUTER_OFF)->vval.v_string;
    floc = (శూన్య *)దొంతర_TV(ectx->ec_చట్రం_idx
				   + దొంతర_చట్రం_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_చట్రం_idx = దొంతర_TV(ectx->ec_చట్రం_idx
				       + దొంతర_చట్రం_IDX_OFF)->vval.v_సంఖ్య;

    ఒకవేళ (floc == ఖాళీ)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    లేకపోతే
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    ఒకవేళ (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_దొంతర.ga_len = శిఖరం + 1;
	*దొంతర_TV_BOT(-1) = *దొంతర_TV(ret_idx);
    }
    లేకపోతే
	// Reset the stack to the position before the call.
	ectx->ec_దొంతర.ga_len = శిఖరం;

    funcdepth_decrement();
    --ex_nesting_level;
    తిరిగి OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    స్థిర పూర్ణాంకం
call_prepare(పూర్ణాంకం argcount, typval_T *argvars, ectx_T *ectx)
{
    పూర్ణాంకం		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    కొరకు (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *దొంతర_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    ఒకవేళ (argcount > 0)
	ectx->ec_దొంతర.ga_len -= argcount - 1;
    లేకపోతే ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
	తిరిగి FAIL;
    లేకపోతే
	++ectx->ec_దొంతర.ga_len;

    // Default return value is zero.
    tv = దొంతర_TV_BOT(-1);
    tv->v_type = VAR_సంఖ్య;
    tv->vval.v_సంఖ్య = 0;
    tv->v_తాళం = 0;

    తిరిగి OK;
}

/*
 * Call a builtin function by index.
 */
    స్థిర పూర్ణాంకం
call_bfunc(పూర్ణాంకం func_idx, పూర్ణాంకం argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_వాదనలు];
    పూర్ణాంకం		idx;
    పూర్ణాంకం		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    అక్షరం	*save_func_name = ectx->ec_where.wt_func_name;

    ఒకవేళ (call_prepare(argcount, argvars, ectx) == FAIL)
	తిరిగి FAIL;

    // Check for void value being passed as an argument.
    కొరకు (idx = 0; idx < argcount; ++idx)
	ఒకవేళ (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_విలువ));
	    కొరకు (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    తిరిగి FAIL;
	}

    ectx->ec_where.wt_func_name = అంతర్గత_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_అంతర్గత_func_by_idx(func_idx, argvars, దొంతర_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    కొరకు (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ఒకవేళ (did_emsg > did_emsg_before)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    స్థిర పూర్ణాంకం
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	పూర్ణాంకం	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_వాదనలు];
    funcexe_T   funcexe;
    funcerror_T	error;
    పూర్ణాంకం		idx;
    పూర్ణాంకం		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    ఒకవేళ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, ఖాళీ)
								       == FAIL)
	తిరిగి FAIL;
    ఒకవేళ (ufunc->uf_def_స్థితి == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	ఒకవేళ (error != FCERR_UNKNOWN)
	{
	    ఒకవేళ (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    లేకపోతే
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    తిరిగి FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	ఒకవేళ (iptr != ఖాళీ)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	తిరిగి call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    ఒకవేళ (call_prepare(argcount, argvars, ectx) == FAIL)
	తిరిగి FAIL;
    CLEAR_క్షేత్రం(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != ఖాళీ ? selfdict : dict_దొంతర_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      దొంతర_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    కొరకు (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ఒకవేళ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_దొరికింది_var);
	తిరిగి FAIL;
    }
    ఒకవేళ (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * If command modifiers were applied restore them.
 */
    స్థిర శూన్య
may_restore_cmdmod(funclocal_T *funclocal)
{
    ఒకవేళ (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ఖాళీ;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    స్థిర పూర్ణాంకం
vim9_aborting(పూర్ణాంకం prev_uncaught_emsg)
{
    తిరిగి uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    స్థిర పూర్ణాంకం
call_by_name(
	char_u	    *name,
	పూర్ణాంకం	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    ఒకవేళ (builtin_function(name, -1))
    {
	పూర్ణాంకం func_idx = కనుగొను_అంతర్గత_func(name);

	ఒకవేళ (func_idx < 0)  // Impossible?
	    తిరిగి FAIL;
	ఒకవేళ (check_అంతర్గత_func(func_idx, argcount) < 0)
	    తిరిగి FAIL;
	తిరిగి call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*ప్రారంభం_bracket = generic_func_కనుగొను_open_bracket(name);
    ఒకవేళ (ప్రారంభం_bracket != ఖాళీ)
    {
	cc = *ప్రారంభం_bracket;
	*ప్రారంభం_bracket = NUL;
    }

    ufunc = కనుగొను_func(name, FALSE);

    ఒకవేళ (ufunc == ఖాళీ)
    {
	పూర్ణాంకం prev_uncaught_emsg = uncaught_emsg;

	ఒకవేళ (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = కనుగొను_func(name, FALSE);

	ఒకవేళ (vim9_aborting(prev_uncaught_emsg))
	{
	    ఒకవేళ (ప్రారంభం_bracket != ఖాళీ)
		*ప్రారంభం_bracket = cc;
	    తిరిగి FAIL;  // bail out if loading the script caused an error
	}
    }

    ఒకవేళ (ప్రారంభం_bracket != ఖాళీ)
	*ప్రారంభం_bracket = cc;

    ఒకవేళ (ufunc != ఖాళీ)
    {
	ఒకవేళ (IS_GENERIC_FUNC(ufunc))
	{
	    ఒకవేళ (ప్రారంభం_bracket != ఖాళీ)
		ufunc = కనుగొను_generic_func(ufunc, name, &ప్రారంభం_bracket);
	    లేకపోతే
	    {
		emsg_funcname(e_generic_func_missing_type_వాదనలు_str, name);
		ufunc = ఖాళీ;
	    }
	    ఒకవేళ (ufunc == ఖాళీ)
		తిరిగి FAIL;
	}
	లేకపోతే ఒకవేళ (ప్రారంభం_bracket != ఖాళీ)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    తిరిగి FAIL;
	}

	ఒకవేళ (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    తిరిగి FAIL;

	తిరిగి call_ufunc(ufunc, ఖాళీ, argcount, ectx, iptr, selfdict);
    }

    తిరిగి FAIL;
}

    స్థిర పూర్ణాంకం
call_partial(
	typval_T    *tv,
	పూర్ణాంకం	    argcount_arg,
	ectx_T	    *ectx)
{
    పూర్ణాంకం		argcount = argcount_arg;
    char_u	*name = ఖాళీ;
    పూర్ణాంకం		called_emsg_before = called_emsg;
    పూర్ణాంకం		res = FAIL;
    dict_T	*selfdict = ఖాళీ;

    ఒకవేళ (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	పూర్ణాంకం	    i;

	ఒకవేళ (pt->pt_obj != ఖాళీ)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		తిరిగి FAIL;
	    కొరకు (i = 1; i <= argcount; ++i)
		*దొంతర_TV_BOT(-i + 1) = *దొంతర_TV_BOT(-i);

	    typval_T *obj_tv = దొంతర_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_తాళం = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_దొంతర.ga_len;
	}

	ఒకవేళ (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, pt->pt_argc))
		తిరిగి FAIL;
	    కొరకు (i = 1; i <= argcount; ++i)
		*దొంతర_TV_BOT(-i + pt->pt_argc) = *దొంతర_TV_BOT(-i);
	    ectx->ec_దొంతర.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    కొరకు (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], దొంతర_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	ఒకవేళ (pt->pt_func != ఖాళీ)
	    తిరిగి call_ufunc(pt->pt_func, pt, argcount, ectx, ఖాళీ, selfdict);

	name = pt->pt_name;
    }
    లేకపోతే ఒకవేళ (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    ఒకవేళ (name != ఖాళీ)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = ఖాళీ;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ఒకవేళ (error != FCERR_NONE)
	    res = FAIL;
	లేకపోతే
	    res = call_by_name(fname, argcount, ectx, ఖాళీ, selfdict);
	vim_free(tofree);
    }

    ఒకవేళ (res == FAIL)
    {
	ఒకవేళ (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == ఖాళీ ? (char_u *)"[unknown]" : name);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    స్థిర పూర్ణాంకం
error_if_locked(పూర్ణాంకం తాళం, అక్షరం *error)
{
    ఒకవేళ (తాళం & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    స్థిర పూర్ణాంకం
check_for_సంఖ్య(typval_T *tv)
{
    ఒకవేళ (tv->v_type != VAR_సంఖ్య)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_సంఖ్య), vartype_name(tv->v_type));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    స్థిర శూన్య
store_var(char_u *name, typval_T *tv)
{
    funccal_నమోదు_T నమోదు_2;
    పూర్ణాంకం		    flags = కేటాయించు_DECL;

    ఒకవేళ (tv->v_తాళం)
	flags |= కేటాయించు_CONST;
    save_funccal(&నమోదు_2);
    సమితి_var_const(name, 0, ఖాళీ, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    స్థిర పూర్ణాంకం
do_2string(typval_T *tv, పూర్ణాంకం is_2string_any, పూర్ణాంకం tostring_flags)
{
    ఒకవేళ (tv->v_type == VAR_STRING)
	తిరిగి OK;

    char_u *str;

    ఒకవేళ (is_2string_any)
    {
	ఎంపిక (tv->v_type)
	{
	    స్థితి VAR_SPECIAL:
	    స్థితి VAR_BOOL:
	    స్థితి VAR_సంఖ్య:
	    స్థితి VAR_FLOAT:
	    స్థితి VAR_DICT:
	    స్థితి VAR_BLOB:	ఆపు;

	    స్థితి VAR_LIST:
				ఒకవేళ (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, పరిమాణం(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    ఒకవేళ (str == ఖాళీ)
					తిరిగి FAIL;
				    s = str;
				    అయితే ((e = vim_strchr(s, '\n')) != ఖాళీ)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					ఒకవేళ (p != ఖాళీ)
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
				    tv->vval.v_string = ga.ga_దత్తాంశం;
				    తిరిగి OK;
				}
				ఒకవేళ (tostring_flags & TOSTRING_INTERPOLATE)
				    ఆపు;
				// FALLTHROUGH
	    అప్రమేయ:	గమ్యం_string_error(tv->v_type);
			తిరిగి FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    తిరిగి OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    స్థిర శూన్య
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_LIST:
	    ఒకవేళ (tv->vval.v_list == ఖాళీ && sv->sv_type != &t_list_ఖాళీలు)
		(శూన్య)rettv_list_alloc(tv);
	    ఆపు;
	స్థితి VAR_TUPLE:
	    ఒకవేళ (tv->vval.v_tuple == ఖాళీ && sv->sv_type != &t_tuple_ఖాళీలు)
		(శూన్య)rettv_tuple_alloc(tv);
	    ఆపు;
	స్థితి VAR_DICT:
	    ఒకవేళ (tv->vval.v_dict == ఖాళీ && sv->sv_type != &t_dict_ఖాళీలు)
		(శూన్య)rettv_dict_alloc(tv);
	    ఆపు;
	స్థితి VAR_BLOB:
	    ఒకవేళ (tv->vval.v_blob == ఖాళీ && sv->sv_type != &t_blob_null)
		(శూన్య)rettv_blob_alloc(tv);
	    ఆపు;
	అప్రమేయ:
	    ఆపు;
    }
}

/*
 * Return the character "str[index]" where "index" is the character index,
 * including composing characters.
 * If "index" is out of range NULL is returned.
 */
    char_u *
char_from_string(char_u *str, varnumber_T సూచీ)
{
    size_t	    nbyte = 0;
    varnumber_T	    nchar = సూచీ;
    size_t	    slen;

    ఒకవేళ (str == ఖాళీ)
	తిరిగి ఖాళీ;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    ఒకవేళ (సూచీ < 0)
    {
	పూర్ణాంకం	clen = 0;

	కొరకు (nbyte = 0; nbyte < slen; ++clen)
	{
	    ఒకవేళ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    లేకపోతే ఒకవేళ (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    లేకపోతే
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + సూచీ;
	ఒకవేళ (nchar < 0)
	    // unlike list: index out of range results in empty string
	    తిరిగి ఖాళీ;
    }

    కొరకు (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	ఒకవేళ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	లేకపోతే ఒకవేళ (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	లేకపోతే
	    nbyte += mb_ptr2len(str + nbyte);
    }
    ఒకవేళ (nbyte >= slen)
	తిరిగి ఖాళీ;
    తిరిగి vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    స్థిర దీర్ఘ
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    ఒకవేళ (nchar >= 0)
    {
	అయితే (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    లేకపోతే
    {
	nbyte = str_len;
	అయితే (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_తల_off(str, str + nbyte);
	    ++nchar;
	}
	ఒకవేళ (nchar < 0)
	    తిరిగి -1;
    }
    తిరిగి (దీర్ఘ)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_ముక్క(char_u *str, varnumber_T మొదటి, varnumber_T చివరి, పూర్ణాంకం exclusive)
{
    దీర్ఘ	ప్రారంభం_byte, చివర_byte;
    size_t	slen;

    ఒకవేళ (str == ఖాళీ)
	తిరిగి ఖాళీ;
    slen = STRLEN(str);
    ప్రారంభం_byte = char_idx2byte(str, slen, మొదటి);
    ఒకవేళ (ప్రారంభం_byte < 0)
	ప్రారంభం_byte = 0; // first index very negative: use zero
    ఒకవేళ ((చివరి == -1 && !exclusive) || చివరి == VARNUM_MAX)
	చివర_byte = (దీర్ఘ)slen;
    లేకపోతే
    {
	చివర_byte = char_idx2byte(str, slen, చివరి);
	ఒకవేళ (!exclusive && చివర_byte >= 0 && చివర_byte < (దీర్ఘ)slen)
	    // end index is inclusive
	    చివర_byte += mb_ptr2len(str + చివర_byte);
    }

    ఒకవేళ (ప్రారంభం_byte >= (దీర్ఘ)slen || చివర_byte <= ప్రారంభం_byte)
	తిరిగి ఖాళీ;
    తిరిగి vim_strnsave(str + ప్రారంభం_byte, చివర_byte - ప్రారంభం_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    స్థిర svar_T *
get_script_svar(scriptref_T *sref, పూర్ణాంకం dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_అంశం(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? ఖాళీ
			      : ((dfunc_T *)def_functions.ga_దత్తాంశం) + dfunc_idx;
    svar_T	    *sv;

    ఒకవేళ (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	ఒకవేళ (dfunc != ఖాళీ)
	    semsg(_(e_script_variable_invalid_after_reload_లోపల_function_str),
					 printable_func_name(dfunc->df_ufunc));
	తిరిగి ఖాళీ;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_దత్తాంశం) + sref->sref_idx;
    ఒకవేళ (sv->sv_name == ఖాళీ)
    {
	ఒకవేళ (dfunc != ఖాళీ)
	    emsg(_(e_script_variable_was_deleted));
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	ఒకవేళ (dfunc != ఖాళీ)
	    emsg(_(e_script_variable_type_మారింది));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	ఒకవేళ (dfunc != ఖాళీ)
	    semsg(_(e_అంశం_not_exported_లోపల_script_str), sv->sv_name);
	తిరిగి ఖాళీ;
    }
    తిరిగి sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    స్థిర char_u *
get_విభజించు_sourceline(
	పూర్ణాంకం c వాడని,
	శూన్య *cookie,
	పూర్ణాంకం indent వాడని,
	getline_opt_T options వాడని)
{
    ఆధారం_cookie_T	*sp = (ఆధారం_cookie_T *)cookie;
    char_u		*p;
    char_u		*పంక్తి;

    p = vim_strchr(sp->nextline, '\n');
    ఒకవేళ (p == ఖాళీ)
    {
	పంక్తి = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    లేకపోతే
    {
	పంక్తి = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    తిరిగి పంక్తి;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    స్థిర పూర్ణాంకం
call_eval_func(
	char_u	    *name,
	పూర్ణాంకం	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    పూర్ణాంకం	    called_emsg_before = called_emsg;
    పూర్ణాంకం	    res;

    res = call_by_name(name, argcount, ectx, iptr, ఖాళీ);
    ఒకవేళ (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = కనుగొను_var(name, ఖాళీ, FALSE);
	ఒకవేళ (v == ఖాళీ || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    తిరిగి FAIL;
	}
	తిరిగి call_partial(&v->di_tv, argcount, ectx);
    }
    తిరిగి res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    పూర్ణాంకం
fill_partial_and_సంవృతం(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ఒకవేళ (ufunc->uf_flags & FC_సంవృతం)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.బహిర్గతం_దొంతర = &ectx->ec_దొంతర;
	pt->pt_outer.బహిర్గతం_చట్రం_idx = ectx->ec_చట్రం_idx;
	ఒకవేళ (ectx->ec_outer_ref != ఖాళీ)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.బహిర్గతం_పైకి = ectx->ec_outer_ref->or_outer;
	    ఒకవేళ (ectx->ec_outer_ref->or_partial != ఖాళీ)
	    {
		pt->pt_outer.బహిర్గతం_పైకి_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.బహిర్గతం_పైకి_partial->pt_refcount;
	    }
	}

	ఒకవేళ (lvi != ఖాళీ)
	{
	    పూర్ణాంకం	లోతు;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    కొరకు (లోతు = 0; లోతు < lvi->lvi_లోతు; ++లోతు)
	    {
		pt->pt_outer.బహిర్గతం_loop[లోతు].దొంతర = &ectx->ec_దొంతర;
		pt->pt_outer.బహిర్గతం_loop[లోతు].var_idx = ectx->ec_చట్రం_idx
			 + దొంతర_చట్రం_కొలత + lvi->lvi_loop[లోతు].var_idx;
		pt->pt_outer.బహిర్గతం_loop[లోతు].var_లెక్క =
					    lvi->lvi_loop[లోతు].var_లెక్క;
	    }
	    pt->pt_outer.బహిర్గతం_loop_కొలత = lvi->lvi_లోతు;
	}
	లేకపోతే
	    pt->pt_outer.బహిర్గతం_loop_కొలత = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	ఒకవేళ (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    తిరిగి FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_దొంతర.ga_దత్తాంశం) + ectx->ec_చట్రం_idx
		       + దొంతర_చట్రం_కొలత + dfunc->df_varcount)->vval.v_సంఖ్య;

	((partial_T **)ectx->ec_funcrefs.ga_దత్తాంశం)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    తిరిగి OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    స్థిర పూర్ణాంకం
exec_command(isn_T *iptr, char_u *cmd_string)
{
    ఆధారం_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_క్షేత్రం(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    ఒకవేళ (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    పూర్ణాంకం
may_ఎక్కించు_script(పూర్ణాంకం sid, పూర్ణాంకం *loaded)
{
    scriptitem_T *si = SCRIPT_అంశం(sid);

    ఒకవేళ (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	ఒకవేళ (do_ఆధారం(si->sn_name, FALSE, DOSO_NONE, ఖాళీ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    తిరిగి FAIL;
	}
    }
    తిరిగి OK;
}

// used for v_instr of typval of VAR_INSTR
నిర్మాణం instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
రకనిర్వచనం నిర్మాణం subs_సమాసం_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    పూర్ణాంకం		subs_స్థితి;
} subs_సమాసం_T;

// Set when calling do_debug().
స్థిర ectx_T	*debug_context = ఖాళీ;
స్థిర పూర్ణాంకం	debug_var_లెక్క;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    పూర్ణాంకం		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    పూర్ణాంకం		    varargs_off;

    ఒకవేళ (ectx == ఖాళీ)
	తిరిగి ఖాళీ;
    dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    కొరకు (idx = debug_var_లెక్క - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_పేర్లు.ga_దత్తాంశం)[idx];

	// the variable name may be NULL when not available in this block
	ఒకవేళ (varname != ఖాళీ && STRCMP(varname, name) == 0)
	    తిరిగి దొంతర_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == ఖాళీ ? 0 : 1;
    కొరకు (idx = 0; idx < ufunc->uf_వాదనలు.ga_len; ++idx)
	ఒకవేళ (STRCMP(((char_u **)(ufunc->uf_వాదనలు.ga_దత్తాంశం))[idx], name) == 0)
	    తిరిగి దొంతర_TV(ectx->ec_చట్రం_idx - ufunc->uf_వాదనలు.ga_len
							  - varargs_off + idx);
    ఒకవేళ (ufunc->uf_va_name != ఖాళీ && STRCMP(ufunc->uf_va_name, name) == 0)
	తిరిగి దొంతర_TV(ectx->ec_చట్రం_idx - 1);

    తిరిగి ఖాళీ;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    పూర్ణాంకం
may_break_లోపల_function(ufunc_T *ufunc)
{
    తిరిగి ufunc->uf_has_breakpoint || debug_has_సమాసం_breakpoint();
}

    స్థిర శూన్య
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*పంక్తి;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_దత్తాంశం)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    పూర్ణాంకం		చివర_lnum = iptr->isn_lnum;
    garray_T	ga;
    పూర్ణాంకం		lnum;

    ఒకవేళ (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	ఒకవేళ (!may_break_లోపల_function(ufunc))
	    తిరిగి;

	// check for the next breakpoint if needed
	breakpoint = dbg_కనుగొను_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	ఒకవేళ (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    తిరిగి;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_లెక్క = iptr->isn_arg.debug.dbg_var_పేర్లు_len;

    కొరకు (ni = iptr + 1; ni->isn_type != ISN_ముగింపు; ++ni)
	ఒకవేళ (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    చివర_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    ఆపు;
	}

    ఒకవేళ (చివర_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, పరిమాణం(char_u *), 10);
	కొరకు (lnum = iptr->isn_lnum; lnum < చివర_lnum
				     && lnum <= ufunc->uf_పంక్తులు.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_పంక్తులు.ga_దత్తాంశం)[lnum - 1];

	    ఒకవేళ (p == ఖాళీ)
		కొనసాగు;  // left over from continuation line
	    p = skipwhite(p);
	    ఒకవేళ (*p == '#')
		ఆపు;
	    ఒకవేళ (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_దత్తాంశం))[ga.ga_len++] = p;
	    ఒకవేళ (STRNCMP(p, "def ", 4) == 0)
		ఆపు;
	}
	పంక్తి = ga_concat_అక్షరశ్రేణులు(&ga, "  ");
	vim_free(ga.ga_దత్తాంశం);
    }
    లేకపోతే
	పంక్తి = ((char_u **)ufunc->uf_పంక్తులు.ga_దత్తాంశం)[iptr->isn_lnum - 1];

    do_debug(పంక్తి == ఖాళీ ? (char_u *)"[empty]" : పంక్తి);
    debug_context = ఖాళీ;

    ఒకవేళ (చివర_lnum > iptr->isn_lnum)
	vim_free(పంక్తి);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    స్థిర పూర్ణాంకం
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    ఒకవేళ (lhs_list->lv_type == ఖాళీ || lhs_list->lv_type->tt_member == ఖాళీ)
	తిరిగి OK;

    తిరిగి check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    స్థిర పూర్ణాంకం
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = దొంతర_TV_BOT(-2);
    దీర్ఘ	lidx = 0;
    typval_T	*tv_dest = దొంతర_TV_BOT(-1);
    పూర్ణాంకం		స్థితి_2 = OK;
    పూర్ణాంకం		check_rhs_type = FALSE;

    ఒకవేళ (tv_idx->v_type == VAR_సంఖ్య)
	lidx = (దీర్ఘ)tv_idx->vval.v_సంఖ్య;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = దొంతర_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    ఒకవేళ (dest_type == VAR_OBJECT && tv_dest->vval.v_object == ఖాళీ)
    {
	emsg(_(e_using_null_object));
	స్థితి_2 = FAIL;
    }
    లేకపోతే ఒకవేళ (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	ఒకవేళ (dest_type == VAR_DICT)
	    స్థితి_2 = do_2string(tv_idx, TRUE, FALSE);
	లేకపోతే ఒకవేళ (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    ఒకవేళ (obj == ఖాళీ)
	    {
		emsg(_(e_using_null_object));
		స్థితి_2 = FAIL;
	    }
	    లేకపోతే
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		పూర్ణాంకం		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		ఒకవేళ (m != ఖాళీ)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_దత్తాంశం)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    ఒకవేళ ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == ఖాళీ ||
			     !class_instance_యొక్క(ufunc->uf_class, cl)))
		    {
			అక్షరం *సందేశం = (m->ocm_access == VIM_ACCESS_వ్యక్తిగత)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(సందేశం, m->ocm_name.string, 0, cl);
			స్థితి_2 = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    లేకపోతే ఒకవేళ (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			స్థితి_2 = FAIL;
		    లేకపోతే
			lidx = m_idx;
		}
		లేకపోతే
		{
		    member_not_దొరికింది_సందేశం(cl, VAR_OBJECT, member, 0);
		    స్థితి_2 = FAIL;
		}
	    }
	}
	లేకపోతే ఒకవేళ (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    స్థితి_2 = FAIL;
	}
	లేకపోతే ఒకవేళ (dest_type == VAR_LIST && tv_idx->v_type != VAR_సంఖ్య)
	{
	    emsg(_(e_సంఖ్య_expected));
	    స్థితి_2 = FAIL;
	}
    }

    ఒకవేళ (స్థితి_2 == OK)
    {
	ఒకవేళ (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    ఒకవేళ (list == ఖాళీ)
	    {
		emsg(_(e_list_not_సమితి));
		తిరిగి FAIL;
	    }
	    ఒకవేళ (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    ఒకవేళ (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), lidx);
		తిరిగి FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    ఒకవేళ (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		తిరిగి FAIL;

	    ఒకవేళ (lidx < list->lv_len)
	    {
		listitem_T *li = list_కనుగొను(list, lidx);
		ఒకవేళ (li == ఖాళీ)
		    తిరిగి FAIL;
		ఒకవేళ (error_if_locked(li->li_tv.v_తాళం,
					     e_cannot_change_locked_list_అంశం))
		    తిరిగి FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (error_if_locked(list->lv_తాళం, e_cannot_change_locked_list))
		    తిరిగి FAIL;
		// append to list, only fails when out of memory
		ఒకవేళ (list_append_tv(list, tv) == FAIL)
		    తిరిగి NOTDONE;
		clear_tv(tv);
	    }
	}
	లేకపోతే ఒకవేళ (dest_type == VAR_DICT)
	{
	    char_u		*కీలకం = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    ఒకవేళ (dict == ఖాళీ)
	    {
		emsg(_(e_dictionary_not_సమితి));
		తిరిగి FAIL;
	    }
	    ఒకవేళ (కీలకం == ఖాళీ)
		కీలకం = (char_u *)"";
	    di = dict_కనుగొను(dict, కీలకం, -1);
	    ఒకవేళ (di != ఖాళీ)
	    {
		ఒకవేళ (error_if_locked(di->di_tv.v_తాళం,
						    e_cannot_change_dict_అంశం))
		    తిరిగి FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (error_if_locked(dict->dv_తాళం, e_cannot_change_dict))
		    తిరిగి FAIL;
		// add to dict, only fails when out of memory
		ఒకవేళ (dict_కూడు_tv(dict, (అక్షరం *)కీలకం, tv) == FAIL)
		    తిరిగి NOTDONE;
		clear_tv(tv);
	    }
	}
	లేకపోతే ఒకవేళ (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    పూర్ణాంకం		    error = FALSE;
	    పూర్ణాంకం		    len;

	    ఒకవేళ (blob == ఖాళీ)
	    {
		emsg(_(e_blob_not_సమితి));
		తిరిగి FAIL;
	    }
	    len = blob_len(blob);
	    ఒకవేళ (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    ఒకవేళ (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_సూచీ_బహిర్గతం_యొక్క_range_nr), lidx);
		తిరిగి FAIL;
	    }
	    ఒకవేళ (విలువ_check_తాళం(blob->bv_తాళం, (char_u *)"blob", FALSE))
		తిరిగి FAIL;
	    nr = tv_get_సంఖ్య_chk(tv, &error);
	    ఒకవేళ (error)
		తిరిగి FAIL;
	    ఒకవేళ (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_విలువ_for_blob_nr), nr);
		తిరిగి FAIL;
	    }
	    blob_సమితి_append(blob, lidx, nr);
	}
	లేకపోతే ఒకవేళ (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    ఒకవేళ (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		ఒకవేళ (itf != ఖాళీ)
		    // convert interface member index to class member index
		    lidx = object_సూచీ_from_itf_సూచీ(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    లేకపోతే
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	లేకపోతే ఒకవేళ (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    స్థితి_2 = FAIL;
	}
	లేకపోతే
	{
	    స్థితి_2 = FAIL;
	    semsg(_(e_cannot_సూచీ_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_దొంతర.ga_len -= 3;
    ఒకవేళ (స్థితి_2 == FAIL)
    {
	clear_tv(tv);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Store a value in a list or blob range.
 */
    స్థిర పూర్ణాంకం
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = దొంతర_TV_BOT(-3);
    typval_T	*tv_idx2 = దొంతర_TV_BOT(-2);
    typval_T	*tv_dest = దొంతర_TV_BOT(-1);
    పూర్ణాంకం		స్థితి_2 = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = దొంతర_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    ఒకవేళ (tv_dest->v_type == VAR_LIST)
    {
	దీర్ఘ	    n1;
	దీర్ఘ	    n2;
	listitem_T  *li1;

	n1 = (దీర్ఘ)tv_get_సంఖ్య_chk(tv_idx1, ఖాళీ);
	ఒకవేళ (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_సంఖ్య == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	లేకపోతే
	    n2 = (దీర్ఘ)tv_get_సంఖ్య_chk(tv_idx2, ఖాళీ);

	li1 = check_range_సూచీ_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	ఒకవేళ (li1 == ఖాళీ)
	    స్థితి_2 = FAIL;
	లేకపోతే
	{
	    స్థితి_2 = check_range_సూచీ_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    ఒకవేళ (స్థితి_2 != FAIL)
		స్థితి_2 = list_కేటాయించు_range(
			tv_dest->vval.v_list,
			tv->vval.v_list,
			n1,
			n2,
			tv_idx2->v_type == VAR_SPECIAL,
			(char_u *)"=",
			(char_u *)"[unknown]");
	}
    }
    లేకపోతే ఒకవేళ (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	దీర్ఘ	    bloblen;

	n1 = tv_get_సంఖ్య_chk(tv_idx1, ఖాళీ);
	ఒకవేళ (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_సంఖ్య == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	లేకపోతే
	    n2 = tv_get_సంఖ్య_chk(tv_idx2, ఖాళీ);
	bloblen = blob_len(tv_dest->vval.v_blob);

	ఒకవేళ (check_blob_సూచీ(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    స్థితి_2 = FAIL;
	లేకపోతే
	    స్థితి_2 = blob_సమితి_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    లేకపోతే
    {
	స్థితి_2 = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_దొంతర.ga_len -= 4;
    clear_tv(tv);

    తిరిగి స్థితి_2;
}

/*
 * Unlet item in list or dict variable.
 */
    స్థిర పూర్ణాంకం
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = దొంతర_TV_BOT(-2);
    typval_T	*tv_dest = దొంతర_TV_BOT(-1);
    పూర్ణాంకం		స్థితి_2 = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    ఒకవేళ (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	ఒకవేళ (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_సంఖ్య)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    స్థితి_2 = FAIL;
	}
	లేకపోతే
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*కీలకం;
	    dictitem_T  *di = ఖాళీ;

	    ఒకవేళ (d != ఖాళీ && విలువ_check_తాళం(
				      d->dv_తాళం, ఖాళీ, FALSE))
		స్థితి_2 = FAIL;
	    లేకపోతే
	    {
		ఒకవేళ (tv_idx->v_type == VAR_STRING)
		{
		    కీలకం = tv_idx->vval.v_string;
		    ఒకవేళ (కీలకం == ఖాళీ)
			కీలకం = (char_u *)"";
		}
		లేకపోతే
		{
		    కీలకం = tv_get_string(tv_idx);
		}
		ఒకవేళ (d != ఖాళీ)
		    di = dict_కనుగొను(d, కీలకం, (పూర్ణాంకం)STRLEN(కీలకం));
		ఒకవేళ (di == ఖాళీ)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_కీలకం_not_present_లోపల_dictionary_str), కీలకం);
		    స్థితి_2 = FAIL;
		}
		లేకపోతే ఒకవేళ (var_check_fixed(di->di_flags,
						   ఖాళీ, FALSE)
			|| var_check_ro(di->di_flags,
						  ఖాళీ, FALSE))
		    స్థితి_2 = FAIL;
		లేకపోతే
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    లేకపోతే ఒకవేళ (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	ఒకవేళ (check_for_సంఖ్య(tv_idx) == FAIL)
	{
	    స్థితి_2 = FAIL;
	}
	లేకపోతే
	{
	    list_T	*l = tv_dest->vval.v_list;
	    దీర్ఘ	n = (దీర్ఘ)tv_idx->vval.v_సంఖ్య;

	    ఒకవేళ (l != ఖాళీ && విలువ_check_తాళం(
				      l->lv_తాళం, ఖాళీ, FALSE))
		స్థితి_2 = FAIL;
	    లేకపోతే
	    {
		listitem_T	*li = list_కనుగొను(l, n);

		ఒకవేళ (li == ఖాళీ)
		{
		    semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), n);
		    స్థితి_2 = FAIL;
		}
		లేకపోతే
		    listitem_remove(l, li);
	    }
	}
    }
    లేకపోతే
    {
	స్థితి_2 = FAIL;
	semsg(_(e_cannot_సూచీ_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_దొంతర.ga_len -= 2;

    తిరిగి స్థితి_2;
}

/*
 * Unlet a range of items in a list variable.
 */
    స్థిర పూర్ణాంకం
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = దొంతర_TV_BOT(-3);
    typval_T	*tv_idx2 = దొంతర_TV_BOT(-2);
    typval_T	*tv_dest = దొంతర_TV_BOT(-1);
    పూర్ణాంకం		స్థితి_2 = OK;

    ఒకవేళ (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	ఒకవేళ (check_for_సంఖ్య(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_సంఖ్య(tv_idx2) == FAIL))
	{
	    స్థితి_2 = FAIL;
	}
	లేకపోతే
	{
	    list_T	*l = tv_dest->vval.v_list;
	    దీర్ఘ	n1 = (దీర్ఘ)tv_idx1->vval.v_సంఖ్య;
	    దీర్ఘ	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (దీర్ఘ)tv_idx2->vval.v_సంఖ్య;
	    listitem_T	*li;

	    li = list_కనుగొను_సూచీ(l, &n1);
	    ఒకవేళ (li == ఖాళీ)
	    {
		semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr),
						 (దీర్ఘ)tv_idx1->vval.v_సంఖ్య);
		స్థితి_2 = FAIL;
	    }
	    లేకపోతే
	    {
		ఒకవేళ (n1 < 0)
		    n1 = list_idx_యొక్క_అంశం(l, li);
		ఒకవేళ (n2 < 0)
		{
		    listitem_T *li2 = list_కనుగొను(l, n2);

		    ఒకవేళ (li2 == ఖాళీ)
		    {
			semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), n2);
			స్థితి_2 = FAIL;
		    }
		    లేకపోతే
			n2 = list_idx_యొక్క_అంశం(l, li2);
		}
		ఒకవేళ (స్థితి_2 != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), n2);
		    స్థితి_2 = FAIL;
		}
		ఒకవేళ (స్థితి_2 != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    లేకపోతే
    {
	స్థితి_2 = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_సూచీ_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_దొంతర.ga_len -= 3;

    తిరిగి స్థితి_2;
}

/*
 * Top of a for loop.
 */
    స్థిర పూర్ణాంకం
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    పూర్ణాంకం		jump = FALSE;
    typval_T	*ltv = దొంతర_TV_BOT(-1);
    typval_T	*idxtv =
		   దొంతర_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
	తిరిగి FAIL;
    ఒకవేళ (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_సంఖ్య;
	ఒకవేళ (list == ఖాళీ
		       || idxtv->vval.v_సంఖ్య >= list->lv_len)
	{
	    jump = TRUE;
	}
	లేకపోతే ఒకవేళ (list->lv_మొదటి == &range_list_అంశం)
	{
	    // non-materialized range() list
	    tv = దొంతర_TV_BOT(0);
	    tv->v_type = VAR_సంఖ్య;
	    tv->v_తాళం = 0;
	    tv->vval.v_సంఖ్య = list_కనుగొను_nr(
			     list, idxtv->vval.v_సంఖ్య, ఖాళీ);
	    ++ectx->ec_దొంతర.ga_len;
	}
	లేకపోతే
	{
	    listitem_T *li = list_కనుగొను(list,
					 idxtv->vval.v_సంఖ్య);
	    ఒకవేళ (li == ఖాళీ)
		తిరిగి FAIL;
	    copy_tv(&li->li_tv, దొంతర_TV_BOT(0));
	    ++ectx->ec_దొంతర.ga_len;
	}
    }
    లేకపోతే ఒకవేళ (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_సంఖ్య;
	ఒకవేళ (tuple == ఖాళీ || idxtv->vval.v_సంఖ్య >= TUPLE_LEN(tuple))
	    jump = TRUE;
	లేకపోతే
	{
	    copy_tv(TUPLE_అంశం(tuple, idxtv->vval.v_సంఖ్య), దొంతర_TV_BOT(0));
	    ++ectx->ec_దొంతర.ga_len;
	}
    }
    లేకపోతే ఒకవేళ (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_సంఖ్య;
	ఒకవేళ (str == ఖాళీ || str[idxtv->vval.v_సంఖ్య] == NUL)
	{
	    jump = TRUE;
	}
	లేకపోతే
	{
	    పూర్ణాంకం	clen = mb_ptr2len(str + idxtv->vval.v_సంఖ్య);

	    // Push the next character from the string.
	    tv = దొంతర_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_సంఖ్య, clen);
	    ++ectx->ec_దొంతర.ga_len;
	    idxtv->vval.v_సంఖ్య += clen - 1;
	}
    }
    లేకపోతే ఒకవేళ (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	ఒకవేళ (idxtv->vval.v_సంఖ్య == -1 && blob != ఖాళీ)
	{
	    ltv->v_తాళం = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_సంఖ్య;
	ఒకవేళ (blob == ఖాళీ || idxtv->vval.v_సంఖ్య >= blob_len(blob))
	{
	    jump = TRUE;
	}
	లేకపోతే
	{
	    // Push the next byte from the blob.
	    tv = దొంతర_TV_BOT(0);
	    tv->v_type = VAR_సంఖ్య;
	    tv->vval.v_సంఖ్య = blob_get(blob,
					 idxtv->vval.v_సంఖ్య);
	    ++ectx->ec_దొంతర.ga_len;
	}
    }
    లేకపోతే
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	తిరిగి FAIL;
    }

    ఒకవేళ (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_చివర;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    లేకపోతే
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = దొంతర_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_సంఖ్య = ectx->ec_funcrefs.ga_len;
    }

    తిరిగి OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
స్థిర loopvars_T *మొదటి_loopvars = ఖాళీ;

    స్థిర శూన్య
కూడు_loopvars_గమ్యం_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    ఒకవేళ (మొదటి_loopvars != ఖాళీ)
	మొదటి_loopvars->lvs_prev = loopvars;
    loopvars->lvs_తదుపరి = మొదటి_loopvars;
    loopvars->lvs_prev = ఖాళీ;
    మొదటి_loopvars = loopvars;
}

    స్థిర శూన్య
remove_loopvars_from_list(loopvars_T *loopvars)
{
    ఒకవేళ (loopvars->lvs_prev == ఖాళీ)
	మొదటి_loopvars = loopvars->lvs_తదుపరి;
    లేకపోతే
	loopvars->lvs_prev->lvs_తదుపరి = loopvars->lvs_తదుపరి;
    ఒకవేళ (loopvars->lvs_తదుపరి != ఖాళీ)
	loopvars->lvs_తదుపరి->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    స్థిర పూర్ణాంకం
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = దొంతర_TV_VAR(endloop->చివర_funcref_idx);
    పూర్ణాంకం		prev_సంవృతం_లెక్క = tv_refcount->vval.v_సంఖ్య;
    పూర్ణాంకం		లోతు = endloop->చివర_లోతు;
    garray_T	*gap = &ectx->ec_funcrefs;
    పూర్ణాంకం		సంవృతం_లోపల_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *దొంతర;
    పూర్ణాంకం		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    కొరకు (idx = prev_సంవృతం_లెక్క; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_దత్తాంశం)[idx];

	ఒకవేళ (pt->pt_refcount > 1 && pt->pt_loopvars[లోతు] == ఖాళీ)
	{
	    పూర్ణాంకం refcount = pt->pt_refcount;
	    పూర్ణాంకం i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    కొరకు (i = 0; i < endloop->చివర_var_లెక్క; ++i)
	    {
		typval_T *stv = దొంతర_TV_VAR(endloop->చివర_var_idx + i);

		ఒకవేళ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ఒకవేళ (refcount > 1)
	    {
		సంవృతం_లోపల_use = TRUE;
		ఆపు;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    ఒకవేళ (!సంవృతం_లోపల_use)
	తిరిగి OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    ఒకవేళ (loopvars == ఖాళీ)
	తిరిగి FAIL;

    loopvars->lvs_ga.ga_len = endloop->చివర_var_లెక్క;
    దొంతర = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_దత్తాంశం = దొంతర;
    ఒకవేళ (దొంతర == ఖాళీ)
    {
	vim_free(loopvars);
	తిరిగి FAIL;
    }
    కూడు_loopvars_గమ్యం_list(loopvars);

    // Move the variable values.
    కొరకు (idx = 0; idx < endloop->చివర_var_లెక్క; ++idx)
    {
	typval_T *tv = దొంతర_TV_VAR(endloop->చివర_var_idx + idx);

	*(దొంతర + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    కొరకు (idx = prev_సంవృతం_లెక్క; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_దత్తాంశం)[idx];

	ఒకవేళ (pt->pt_refcount > 1 && pt->pt_loopvars[లోతు] == ఖాళీ)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[లోతు] = loopvars;

	    pt->pt_outer.బహిర్గతం_loop[లోతు].దొంతర = &loopvars->lvs_ga;
	    pt->pt_outer.బహిర్గతం_loop[లోతు].var_idx -=
		  ectx->ec_చట్రం_idx + దొంతర_చట్రం_కొలత + endloop->చివర_var_idx;
	}
    }

    తిరిగి OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    పూర్ణాంకం
loopvars_check_refcount(loopvars_T *loopvars)
{
    పూర్ణాంకం		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    పూర్ణాంకం		    పూర్తి = 0;
	typval_T	*దొంతర = gap->ga_దత్తాంశం;

    ఒకవేళ (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	తిరిగి FALSE;
    కొరకు (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_దత్తాంశం) + i;

	ఒకవేళ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ఖాళీ
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    పూర్ణాంకం	    లోతు;

	    కొరకు (లోతు = 0; లోతు < MAX_LOOP_లోతు; ++లోతు)
		ఒకవేళ (tv->vval.v_partial->pt_loopvars[లోతు] == loopvars)
		    ++పూర్తి;
	}
    }
    ఒకవేళ (పూర్తి != loopvars->lvs_min_refcount)
	తిరిగి FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    దొంతర = gap->ga_దత్తాంశం;
    కొరకు (i = 0; i < gap->ga_len; ++i)
	clear_tv(దొంతర + i);
    vim_free(దొంతర);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    తిరిగి TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    పూర్ణాంకం
సమితి_ref_లోపల_loopvars(పూర్ణాంకం copyID)
{
    loopvars_T *loopvars;

    కొరకు (loopvars = మొదటి_loopvars; loopvars != ఖాళీ;
						 loopvars = loopvars->lvs_తదుపరి)
    {
	typval_T    *దొంతర = loopvars->lvs_ga.ga_దత్తాంశం;
	పూర్ణాంకం	    i;

	కొరకు (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    ఒకవేళ (సమితి_ref_లోపల_అంశం(దొంతర + i, copyID, ఖాళీ, ఖాళీ, ఖాళీ))
		తిరిగి TRUE;  // abort
    }
    తిరిగి FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    స్థిర పూర్ణాంకం
ఎక్కించు_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = ఖాళీ;
    hashtab_T	*ht = ఖాళీ;
    అక్షరం	namespace;

    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
	తిరిగి NOTDONE;
    ఎంపిక (isn_type)
    {
	స్థితి ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    ఆపు;
	స్థితి ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    ఆపు;
	స్థితి ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    ఆపు;
	స్థితి ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    ఆపు;
	అప్రమేయ:  // Cannot reach here
	    తిరిగి NOTDONE;
    }
    di = కనుగొను_var_లోపల_ht(ht, 0, iptr->isn_arg.string, TRUE);

    ఒకవేళ (di == ఖాళీ)
    {
	ఒకవేళ (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = కనుగొను_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    ఒకవేళ (ufunc != ఖాళీ)
	    {
		typval_T    *tv = దొంతర_TV_BOT(0);

		++ectx->ec_దొంతర.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		ఒకవేళ (tv->vval.v_string == ఖాళీ)
		    తిరిగి FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		తిరిగి OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	ఒకవేళ (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != ఖాళీ)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_అంశం_not_దొరికింది_లోపల_script_str), iptr->isn_arg.string);
	లేకపోతే
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = దొంతర_TV_BOT(0);
	tv->v_type = VAR_సంఖ్య;
	tv->v_తాళం = 0;
	tv->vval.v_సంఖ్య = 0;
	++ectx->ec_దొంతర.ga_len;
	తిరిగి FAIL;
    }
    లేకపోతే
    {
	copy_tv(&di->di_tv, దొంతర_TV_BOT(0));
	++ectx->ec_దొంతర.ga_len;
    }
    తిరిగి OK;
}


    స్థిర శూన్య
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, పరిమాణం(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    అక్షరం *tofree = ఖాళీ;
    అక్షరం *typename = type_name(type, &tofree);
    semsg(_(e_object_required_దొరికింది_str), typename);
    vim_free(tofree);
    clear_type_list(&type_list);
}

/*
 * Accessing the variable or method of an object or a class stored in a
 * variable of type "any".
 * Returns OK if the member variable is present.
 * Returns FAIL if the variable is not found.
 */
    స్థిర పూర్ణాంకం
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    పూర్ణాంకం		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = ఖాళీ;
    typval_T	mtv;

    ఒకవేళ (is_object)
    {
	obj = tv->vval.v_object;
	ఒకవేళ (obj == ఖాళీ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    తిరిగి FAIL;
	}
	tv_cl = obj->obj_class;
    }
    లేకపోతే
    {
	tv_cl = tv->vval.v_class;
	ఒకవేళ (tv_cl == ఖాళీ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    తిరిగి FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    పూర్ణాంకం		namelen = (పూర్ణాంకం)STRLEN(iptr->isn_arg.string);
    పూర్ణాంకం		save_did_emsg = did_emsg;

    ఒకవేళ (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	తిరిగి OK;
    }

    ఒకవేళ (did_emsg != save_did_emsg)
	తిరిగి FAIL;

    // could be a class or instance method
    ufunc_T	*oc_పద్ధతి;
    పూర్ణాంకం		oc_పద్ధతి_idx;

    oc_పద్ధతి = పద్ధతి_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_పద్ధతి_idx);
    ఒకవేళ (oc_పద్ధతి == ఖాళీ)
    {
	అక్షరం	*సందేశం;

	SOURCING_LNUM = iptr->isn_lnum;
	ఒకవేళ (is_object)
	    సందేశం = e_variable_not_దొరికింది_on_object_str_str;
	లేకపోతే
	    సందేశం = e_class_variable_str_not_దొరికింది_లోపల_class_str;
	semsg(_(సందేశం), iptr->isn_arg.string, tv_cl->class_name.string);
	తిరిగి FAIL;
    }

    // Protected methods are not accessible outside the class
    ఒకవేళ (*oc_పద్ధతి->uf_name == '_'
			&& !class_instance_యొక్క(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_పద్ధతి_str), oc_పద్ధతి->uf_name);
	తిరిగి FAIL;
    }

    // Create a partial for the instance or class method
    ఒకవేళ (obj_పద్ధతి_గమ్యం_partial_tv(is_object ? obj : ఖాళీ, oc_పద్ధతి, tv)
								== FAIL)
	తిరిగి FAIL;

    తిరిగి OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    స్థిర శూన్య
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, పూర్ణాంకం fixindent)
{
    పూర్ణాంకం		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*సమాసం = ఖాళీ;
    పూర్ణాంకం		dir = FORWARD;

    ఒకవేళ (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = దొంతర_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_సంఖ్య;
	ఒకవేళ (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_దొంతర.ga_len;
    }
    లేకపోతే ఒకవేళ (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    లేకపోతే ఒకవేళ (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	ఒకవేళ (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    ఒకవేళ (regname == '=')
    {
	tv = దొంతర_TV_BOT(-1);
	ఒకవేళ (tv->v_type == VAR_STRING)
	    సమాసం = tv->vval.v_string;
	లేకపోతే
	{
	    సమాసం = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_దొంతర.ga_len;
    }
    check_cursor();

    ఒకవేళ (fixindent)
	do_put(regname, సమాసం, dir, 1L, PUT_పంక్తి|PUT_CURSLINE|PUT_FIXINDENT);
    లేకపోతే
	do_put(regname, సమాసం, dir, 1L, PUT_పంక్తి|PUT_CURSLINE);
    vim_free(సమాసం);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    స్థిర పూర్ణాంకం
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    పూర్ణాంకం		లెక్క = iptr->isn_arg.unpack.unp_లెక్క;
    పూర్ణాంకం		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    పూర్ణాంకం		i;

    l = tv->vval.v_list;
    ఒకవేళ (l == ఖాళీ
	    || l->lv_len < (semicolon ? లెక్క - 1 : లెక్క))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_విలువ_does_not_have_enough_వస్తువులు));
	తిరిగి EXEC_FAIL;
    }
    లేకపోతే ఒకవేళ (!semicolon && l->lv_len > లెక్క)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_విలువ_has_మరిన్ని_వస్తువులు_than_targets));
	తిరిగి EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, లెక్క - 1))
	తిరిగి EXEC_పూర్తి;
    ectx->ec_దొంతర.ga_len += లెక్క - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ఒకవేళ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_వస్తువులు(l->lv_len - లెక్క + 1);

	ఒకవేళ (rem_list == ఖాళీ)
	    తిరిగి EXEC_పూర్తి;
	tv = దొంతర_TV_BOT(-లెక్క);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_తాళం = 0;
	li = l->lv_మొదటి;
	కొరకు (i = 0; i < లెక్క - 1; ++i)
	    li = li->li_తదుపరి;
	కొరకు (i = 0; li != ఖాళీ; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_సమితి_అంశం(rem_list, i, &tvcopy);
	    li = li->li_తదుపరి;
	}
	--లెక్క;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_మొదటి;
    కొరకు (i = 0; i < లెక్క; ++i)
    {
	tv = దొంతర_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_తదుపరి;
    }

    list_unref(l);

    తిరిగి EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    స్థిర పూర్ణాంకం
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    పూర్ణాంకం		లెక్క = iptr->isn_arg.unpack.unp_లెక్క;
    పూర్ణాంకం		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    పూర్ణాంకం		i;

    tuple = tv->vval.v_tuple;
    ఒకవేళ (tuple == ఖాళీ
	    || TUPLE_LEN(tuple) < (semicolon ? లెక్క - 1 : లెక్క))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_మరిన్ని_targets_than_tuple_వస్తువులు));
	తిరిగి EXEC_FAIL;
    }
    లేకపోతే ఒకవేళ (!semicolon && TUPLE_LEN(tuple) > లెక్క)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_వస్తువులు));
	తిరిగి EXEC_FAIL;
    }

    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, లెక్క - 1))
	తిరిగి EXEC_పూర్తి;
    ectx->ec_దొంతర.ga_len += లెక్క - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ఒకవేళ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_వస్తువులు(TUPLE_LEN(tuple) - లెక్క + 1);

	ఒకవేళ (rem_list == ఖాళీ)
	    తిరిగి EXEC_పూర్తి;
	tv = దొంతర_TV_BOT(-లెక్క);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_తాళం = 0;
	కొరకు (i = లెక్క - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_అంశం(tuple, i), &tvcopy);
	    list_సమితి_అంశం(rem_list, i - (లెక్క - 1), &tvcopy);
	}
	--లెక్క;
    }

    // Produce the values in reverse order, first item last.
    కొరకు (i = 0; i < లెక్క; ++i)
    {
	tv = దొంతర_TV_BOT(-i - 1);
	copy_tv(TUPLE_అంశం(tuple, i), tv);
    }

    tuple_unref(tuple);

    తిరిగి EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    స్థిర పూర్ణాంకం
exec_instructions(ectx_T *ectx)
{
    పూర్ణాంకం		ret = FAIL;
    పూర్ణాంకం		save_trylevel_at_ప్రారంభం = ectx->ec_trylevel_at_ప్రారంభం;
    పూర్ణాంకం		dict_దొంతర_len_at_ప్రారంభం = dict_దొంతర.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_ప్రారంభం = trylevel;

    కొరకు (;;)
    {
	స్థిర పూర్ణాంకం  breakcheck_లెక్క = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	ఒకవేళ (unlikely(++breakcheck_లెక్క >= 100))
	{
	    పంక్తి_breakcheck();
	    breakcheck_లెక్క = 0;
	}
	ఒకవేళ (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    ఒకవేళ (throw_exception("Vim:Interrupt", ET_అంతరాయం, ఖాళీ) == FAIL)
		వెళ్ళు theend;
	    did_throw = TRUE;
	}

	ఒకవేళ (unlikely(did_emsg && సందేశం_list != ఖాళీ && *సందేశం_list != ఖాళీ))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    ఒకవేళ (throw_exception(*సందేశం_list, ET_ERROR, ఖాళీ) == FAIL)
		వెళ్ళు theend;
	    did_throw = TRUE;
	    *సందేశం_list = ఖాళీ;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    ఒకవేళ (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_దత్తాంశం)
							+ trystack->ga_len - 1;
		ఒకవేళ (trycmd->tcd_చట్రం_idx == ectx->ec_చట్రం_idx)
		{
		    ఒకవేళ (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			ముగింపు_exception(caught_దొంతర);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	ఒకవేళ (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = ఖాళీ;
	    పూర్ణాంకం		సూచీ = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    అయితే (సూచీ > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_దత్తాంశం) + సూచీ - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		ఒకవేళ (!trycmd->tcd_లోపల_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_చట్రం_idx == ectx->ec_చట్రం_idx)
		    ఆపు;
		// In the catch and finally block of this try we have to go up
		// one level.
		--సూచీ;
		trycmd = ఖాళీ;
	    }
	    ఒకవేళ (trycmd != ఖాళీ && trycmd->tcd_చట్రం_idx == ectx->ec_చట్రం_idx)
	    {
		ఒకవేళ (trycmd->tcd_లోపల_catch)
		{
		    ఒకవేళ (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    లేకపోతే
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		లేకపోతే
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_లోపల_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    లేకపోతే
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    వెళ్ళు theend;
		tv = దొంతర_TV_BOT(0);
		tv->v_type = VAR_సంఖ్య;
		tv->vval.v_సంఖ్య = 0;
		++ectx->ec_దొంతర.ga_len;
		ఒకవేళ (ectx->ec_చట్రం_idx == ectx->ec_initial_చట్రం_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    ఒకవేళ (handle_సంవృతం_లోపల_use(ectx, FALSE) == FAIL)
			వెళ్ళు theend;
		    వెళ్ళు పూర్తి;
		}

		ఒకవేళ (func_return(ectx) == FAIL)
		    వెళ్ళు theend;
	    }
	    కొనసాగు;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	ఎంపిక (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    స్థితి ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = దొంతర_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		ఆపు;

	    // execute Ex command line
	    స్థితి ISN_EXEC:
		ఒకవేళ (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    వెళ్ళు on_error;
		ఆపు;

	    // execute Ex command line split at NL characters.
	    స్థితి ISN_EXEC_విభజించు:
		{
		    ఆధారం_cookie_T cookie;
		    char_u	    *పంక్తి;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_క్షేత్రం(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    పంక్తి = get_విభజించు_sourceline(0, &cookie, 0, 0);
		    ఒకవేళ (do_cmdline(పంక్తి,
				get_విభజించు_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(పంక్తి);
			వెళ్ళు on_error;
		    }
		    vim_free(పంక్తి);
		}
		ఆపు;

	    // execute Ex command line that is only a range
	    స్థితి ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    అక్షరం	*error = ఖాళీ;

		    CLEAR_క్షేత్రం(ea);
		    ea.cmdidx = CMD_కొలత;
		    ea.addr_type = ADDR_పంక్తులు;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_చిరునామా(&ea, &error, FALSE);
		    ఒకవేళ (ea.cmd == ఖాళీ)
			వెళ్ళు on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    ఒకవేళ (error != ఖాళీ)
		    {
			emsg(error);
			వెళ్ళు on_error;
		    }
		}
		ఆపు;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    స్థితి ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    పూర్ణాంకం	    res;
		    పూర్ణాంకం	    save_flags = cmdmod.cmod_flags;

		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    tv = దొంతర_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, ఖాళీ, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    ఒకవేళ (res == FAIL)
			వెళ్ళు on_error;
		    ++ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    // push typeval VAR_INSTR with instructions to be executed
	    స్థితి ISN_INSTR:
		{
		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    tv = దొంతర_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    ఒకవేళ (tv->vval.v_instr == ఖాళీ)
			వెళ్ళు on_error;
		    ++ectx->ec_దొంతర.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		ఆపు;

	    స్థితి ISN_ఆధారం:
		{
		    పూర్ణాంకం notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (may_ఎక్కించు_script((పూర్ణాంకం)iptr->isn_arg.సంఖ్య, &notused)
								       == FAIL)
			వెళ్ళు on_error;
		}
		ఆపు;

	    // execute :substitute with an expression
	    స్థితి ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    ఆధారం_cookie_T	cookie;
		    నిర్మాణం subs_సమాసం_S	*save_instr = substitute_instr;
		    నిర్మాణం subs_సమాసం_S	subs_instr;
		    పూర్ణాంకం			res;

		    subs_instr.subs_ectx = ectx;
		    subs_instr.subs_instr = subs->subs_instr;
		    subs_instr.subs_స్థితి = OK;
		    substitute_instr = &subs_instr;

		    SOURCING_LNUM = iptr->isn_lnum;
		    // This is very much like ISN_EXEC
		    CLEAR_క్షేత్రం(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    res = do_cmdline(subs->subs_cmd,
				getsourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
		    substitute_instr = save_instr;

		    ఒకవేళ (res == FAIL || did_emsg
					     || subs_instr.subs_స్థితి == FAIL)
			వెళ్ళు on_error;
		}
		ఆపు;

	    స్థితి ISN_ముగింపు:
		వెళ్ళు పూర్తి;

	    స్థితి ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		ఒకవేళ (alloc_redir_lval() == FAIL)
		    వెళ్ళు on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		ఆపు;

	    స్థితి ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    {
			vim_free(res);
			వెళ్ళు theend;
		    }
		    tv = దొంతర_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    స్థితి ISN_CEXPR_AUCMD:
#నిర్వచితమైతే FEAT_QUICKFIX
		force_abort = TRUE;
		ఒకవేళ (trigger_cexpr_autocmd(iptr->isn_arg.సంఖ్య) == FAIL)
		    వెళ్ళు on_error;
		force_abort = FALSE;
#అయితే_ముగింపు
		ఆపు;

	    స్థితి ISN_CEXPR_CORE:
#నిర్వచితమైతే FEAT_QUICKFIX
		{
		    exarg_T ea;
		    పూర్ణాంకం	    res;

		    CLEAR_క్షేత్రం(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_దొంతర.ga_len;
		    tv = దొంతర_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    ఒకవేళ (res == FAIL)
			వెళ్ళు on_error;
		}
#అయితే_ముగింపు
		ఆపు;

	    // execute Ex command from pieces on the stack
	    స్థితి ISN_EXECCONCAT:
		{
		    పూర్ణాంకం	    లెక్క = iptr->isn_arg.సంఖ్య;
		    size_t  len = 0;
		    పూర్ణాంకం	    pass;
		    పూర్ణాంకం	    i;
		    char_u  *cmd = ఖాళీ;
		    char_u  *str;

		    కొరకు (pass = 1; pass <= 2; ++pass)
		    {
			కొరకు (i = 0; i < లెక్క; ++i)
			{
			    tv = దొంతర_TV_BOT(i - లెక్క);
			    str = tv->vval.v_string;
			    ఒకవేళ (str != ఖాళీ && *str != NUL)
			    {
				ఒకవేళ (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    ఒకవేళ (pass == 2)
				clear_tv(tv);
			}
			ఒకవేళ (pass == 1)
			{
			    cmd = alloc(len + 1);
			    ఒకవేళ (unlikely(cmd == ఖాళీ))
				వెళ్ళు theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		ఆపు;

	    // execute :echo {string} ...
	    స్థితి ISN_ECHO:
		{
		    పూర్ణాంకం లెక్క = iptr->isn_arg.echo.echo_లెక్క;
		    పూర్ణాంకం	atstart = TRUE;
		    పూర్ణాంకం needclr = TRUE;
		    పూర్ణాంకం	idx;

		    కొరకు (idx = 0; idx < లెక్క; ++idx)
		    {
			tv = దొంతర_TV_BOT(idx - లెక్క);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    ఒకవేళ (needclr)
			సందేశం_clr_eos();
		    ectx->ec_దొంతర.ga_len -= లెక్క;
		}
		ఆపు;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    స్థితి ISN_EXECUTE:
	    స్థితి ISN_ECHOMSG:
	    స్థితి ISN_ECHOWINDOW:
	    స్థితి ISN_ECHOCONSOLE:
	    స్థితి ISN_ECHOERR:
		{
		    పూర్ణాంకం		లెక్క;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    పూర్ణాంకం		len;
		    పూర్ణాంకం		failed = FALSE;
		    పూర్ణాంకం		idx;

		    ఒకవేళ (iptr->isn_type == ISN_ECHOWINDOW)
			లెక్క = iptr->isn_arg.echowin.ewin_లెక్క;
		    లేకపోతే
			లెక్క = iptr->isn_arg.సంఖ్య;
		    ga_init2(&ga, 1, 80);
		    కొరకు (idx = 0; idx < లెక్క; ++idx)
		    {
			tv = దొంతర_TV_BOT(idx - లెక్క);
			ఒకవేళ (iptr->isn_type == ISN_EXECUTE)
			{
			    ఒకవేళ (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_పని)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_విలువ_as_string_str),
						    vartype_name(tv->v_type));
				ఆపు;
			    }
			    లేకపోతే
				p = tv_get_string_buf(tv, buf);
			}
			లేకపోతే
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (పూర్ణాంకం)STRLEN(p);
			ఒకవేళ (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			లేకపోతే
			{
			    ఒకవేళ (ga.ga_len > 0)
				((char_u *)(ga.ga_దత్తాంశం))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_దత్తాంశం) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_దొంతర.ga_len -= లెక్క;
		    ఒకవేళ (failed)
		    {
			ga_clear(&ga);
			వెళ్ళు on_error;
		    }

		    ఒకవేళ (ga.ga_దత్తాంశం != ఖాళీ)
		    {
			ఒకవేళ (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_దత్తాంశం);
			    ఒకవేళ (did_emsg)
			    {
				ga_clear(&ga);
				వెళ్ళు on_error;
			    }
			}
			లేకపోతే
			{
			    సందేశం_sb_eol();
			    ఒకవేళ (iptr->isn_type == ISN_ECHOMSG)
			    {
				సందేశం_attr(ga.ga_దత్తాంశం, echo_attr);
				బహిర్గతం_flush();
			    }
#నిర్వచితమైతే HAS_సందేశం_కిటికీ
			    లేకపోతే ఒకవేళ (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				ప్రారంభం_echowindow(
					      iptr->isn_arg.echowin.ewin_సమయం);
				సందేశం_attr(ga.ga_దత్తాంశం, echo_attr);
				చివర_echowindow();
			    }
#అయితే_ముగింపు
			    లేకపోతే ఒకవేళ (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_దత్తాంశం, (పూర్ణాంకం)STRLEN(ga.ga_దత్తాంశం),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    లేకపోతే
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_దత్తాంశం);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		ఆపు;

	    // load local variable or argument
	    స్థితి ISN_ఎక్కించు:
		ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    వెళ్ళు theend;
		tv = దొంతర_TV_VAR(iptr->isn_arg.సంఖ్య);
		ఒకవేళ (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    దొంతర_TV_BOT(0)->v_type = VAR_SPECIAL;
		    దొంతర_TV_BOT(0)->vval.v_సంఖ్య = VVAL_NONE;
		}
		లేకపోతే
		    copy_tv(tv, దొంతర_TV_BOT(0));
		++ectx->ec_దొంతర.ga_len;
		ఆపు;

	    // load v: variable
	    స్థితి ISN_LOADV:
		ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    వెళ్ళు theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.సంఖ్య), దొంతర_TV_BOT(0));
		++ectx->ec_దొంతర.ga_len;
		ఆపు;

	    // load s: variable in Vim9 script
	    స్థితి ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ఒకవేళ (sv == ఖాళీ)
			వెళ్ళు theend;
		    allocate_if_null(sv);
		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    copy_tv(sv->sv_tv, దొంతర_TV_BOT(0));
		    ++ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    // load s: variable in old script or autoload import
	    స్థితి ISN_LOADS:
	    స్థితి ISN_LOADEXPORT:
		{
		    పూర్ణాంకం		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = కనుగొను_var_లోపల_ht(ht, 0, name, TRUE);

		    ఒకవేళ (di == ఖాళీ)
		    {
			ufunc_T	*ufunc = ఖాళీ;

			ఒకవేళ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (శూన్య)కనుగొను_exported(sid, name, &ufunc, &type,
							 ఖాళీ, ఖాళీ, FALSE);
			}
			ఒకవేళ (ufunc == ఖాళీ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    వెళ్ళు on_error;
			}
			ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			    వెళ్ళు theend;
			tv = దొంతర_TV_BOT(0);
			tv->v_తాళం = 0;
			++ectx->ec_దొంతర.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    లేకపోతే
		    {
			ఒకవేళ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    పూర్ణాంకం idx = get_script_అంశం_idx(sid, name, 0,
								   ఖాళీ, ఖాళీ);
			    svar_T	*sv;

			    ఒకవేళ (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_అంశం(sid)
						  ->sn_var_vals.ga_దత్తాంశం) + idx;
				ఒకవేళ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_అంశం_not_exported_లోపల_script_str),
									 name);
				    వెళ్ళు on_error;
				}
			    }
			}
			ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			    వెళ్ళు theend;
			copy_tv(&di->di_tv, దొంతర_TV_BOT(0));
			++ectx->ec_దొంతర.ga_len;
		    }
		}
		ఆపు;

	    // load g:/b:/w:/t: variable
	    స్థితి ISN_LOADG:
	    స్థితి ISN_LOADB:
	    స్థితి ISN_LOADW:
	    స్థితి ISN_LOADT:
		{
		    పూర్ణాంకం res = ఎక్కించు_namespace_var(ectx, iptr->isn_type, iptr);

		    ఒకవేళ (res == NOTDONE)
			వెళ్ళు theend;
		    ఒకవేళ (res == FAIL)
			వెళ్ళు on_error;
		}

		ఆపు;

	    // load autoload variable
	    స్థితి ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (eval_variable(name, (పూర్ణాంకం)STRLEN(name), 0,
			      దొంతర_TV_BOT(0), ఖాళీ, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = దొంతర_TV_BOT(0);
			tv->v_type = VAR_సంఖ్య;
			tv->v_తాళం = 0;
			tv->vval.v_సంఖ్య = 0;
			++ectx->ec_దొంతర.ga_len;
			వెళ్ళు on_error;
		    }
		    ++ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    // load g:/b:/w:/t: namespace
	    స్థితి ISN_LOADGDICT:
	    స్థితి ISN_LOADBDICT:
	    స్థితి ISN_LOADWDICT:
	    స్థితి ISN_LOADTDICT:
		{
		    dict_T *d = ఖాళీ;

		    ఎంపిక (iptr->isn_type)
		    {
			స్థితి ISN_LOADGDICT: d = get_globvar_dict(); ఆపు;
			స్థితి ISN_LOADBDICT: d = curbuf->b_vars; ఆపు;
			స్థితి ISN_LOADWDICT: d = curwin->w_vars; ఆపు;
			స్థితి ISN_LOADTDICT: d = curtab->tp_vars; ఆపు;
			అప్రమేయ:  // Cannot reach here
			    వెళ్ళు theend;
		    }
		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    tv = దొంతర_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_తాళం = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    // load &option
	    స్థితి ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    ఒకవేళ (eval_option(&name, &optval, TRUE) == FAIL)
			వెళ్ళు theend;
		    *దొంతర_TV_BOT(0) = optval;
		    ++ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    // load $ENV
	    స్థితి ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    // name is always valid, checked when compiling
		    (శూన్య)eval_env_var(&name, &optval, TRUE);
		    *దొంతర_TV_BOT(0) = optval;
		    ++ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    // load @register
	    స్థితి ISN_LOADREG:
		ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    వెళ్ళు theend;
		tv = దొంతర_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_తాళం = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.సంఖ్య, GREG_సమాసం_SRC);
		++ectx->ec_దొంతర.ga_len;
		ఆపు;

	    // store local variable
	    స్థితి ISN_STORE:
		--ectx->ec_దొంతర.ga_len;
		tv = దొంతర_TV_VAR(iptr->isn_arg.సంఖ్య);
		ఒకవేళ (check_typval_is_విలువ(దొంతర_TV_BOT(0)) == FAIL
			|| దొంతర_TV_BOT(0)->v_type == VAR_VOID)
		{
		    ఒకవేళ (దొంతర_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_విలువ));
		    clear_tv(దొంతర_TV_BOT(0));
		    వెళ్ళు on_error;
		}
		clear_tv(tv);
		*tv = *దొంతర_TV_BOT(0);
		ఆపు;

	    // store s: variable in old script or autoload import
	    స్థితి ISN_STORES:
	    స్థితి ISN_STOREEXPORT:
		{
		    పూర్ణాంకం		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = ఖాళీ;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    ఒకవేళ (iptr->isn_type == ISN_STOREEXPORT)
			di = కనుగొను_var_autoload_ఉపసర్గ(name, sid, ఖాళీ, ఖాళీ);
		    // Then look for a variable in the script's variables.
		    ఒకవేళ (di == ఖాళీ)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = కనుగొను_var_లోపల_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_దొంతర.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (di == ఖాళీ)
		    {
			ఒకవేళ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(దొంతర_TV_BOT(0));
			    వెళ్ళు on_error;
			}
			store_var(name, దొంతర_TV_BOT(0));
		    }
		    లేకపోతే
		    {
			ఒకవేళ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    పూర్ణాంకం idx = get_script_అంశం_idx(sid, name, 0,
								   ఖాళీ, ఖాళీ);

			    // can this ever fail?
			    ఒకవేళ (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_అంశం(sid)
						  ->sn_var_vals.ga_దత్తాంశం) + idx;

				ఒకవేళ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_అంశం_not_exported_లోపల_script_str),
									 name);
				    clear_tv(దొంతర_TV_BOT(0));
				    వెళ్ళు on_error;
				}
			    }
			}
			ఒకవేళ (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(దొంతర_TV_BOT(0));
			    వెళ్ళు on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *దొంతర_TV_BOT(0);
		    }
		}
		ఆపు;

	    // store script-local variable in Vim9 script
	    స్థితి ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ఒకవేళ (sv == ఖాళీ)
			వెళ్ళు theend;
		    --ectx->ec_దొంతర.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (విలువ_check_తాళం(sv->sv_tv->v_తాళం, sv->sv_name, FALSE))
		    {
			clear_tv(దొంతర_TV_BOT(0));
			వెళ్ళు on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *దొంతర_TV_BOT(0);
		}
		ఆపు;

	    // store option
	    స్థితి ISN_STOREOPT:
	    స్థితి ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    పూర్ణాంకం		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    దీర్ఘ	n = 0;
		    char_u	*s = ఖాళీ;
		    అక్షరం	*సందేశం;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = ఖాళీ;

		    --ectx->ec_దొంతర.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = దొంతర_TV_BOT(0);
		    ఒకవేళ (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			ఒకవేళ (s == ఖాళీ)
			    s = (char_u *)"";
		    }
		    లేకపోతే ఒకవేళ (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			ఒకవేళ (s == ఖాళీ || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    వెళ్ళు on_error;
			}
		    }
		    లేకపోతే
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_సంఖ్య;
		    సందేశం = సమితి_option_విలువ(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    ఒకవేళ (సందేశం != ఖాళీ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(సందేశం));
			వెళ్ళు on_error;
		    }
		}
		ఆపు;

	    // store $ENV
	    స్థితి ISN_STOREENV:
		ఒకవేళ (check_secure() || check_restricted())
		    వెళ్ళు theend;
		--ectx->ec_దొంతర.ga_len;
		tv = దొంతర_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		ఆపు;

	    // store @r
	    స్థితి ISN_STOREREG:
		{
		    పూర్ణాంకం	reg = iptr->isn_arg.సంఖ్య;

		    --ectx->ec_దొంతర.ga_len;
		    tv = దొంతర_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		ఆపు;

	    // store v: variable
	    స్థితి ISN_STOREV:
		--ectx->ec_దొంతర.ga_len;
		ఒకవేళ (సమితి_vim_var_tv(iptr->isn_arg.సంఖ్య, దొంతర_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    వెళ్ళు on_error;
		ఆపు;

	    // store g:/b:/w:/t: variable
	    స్థితి ISN_STOREG:
	    స్థితి ISN_STOREB:
	    స్థితి ISN_STOREW:
	    స్థితి ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    ఎంపిక (iptr->isn_type)
		    {
			స్థితి ISN_STOREG:
			    ht = get_globvar_ht();
			    ఆపు;
			స్థితి ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    ఆపు;
			స్థితి ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    ఆపు;
			స్థితి ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    ఆపు;
			అప్రమేయ:  // Cannot reach here
			    వెళ్ళు theend;
		    }

		    --ectx->ec_దొంతర.ga_len;
		    di = కనుగొను_var_లోపల_ht(ht, 0, name, TRUE);
		    ఒకవేళ (di == ఖాళీ)
			store_var(iptr->isn_arg.string, దొంతర_TV_BOT(0));
		    లేకపోతే
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ఒకవేళ (var_check_permission(di, name) == FAIL)
			    వెళ్ళు on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *దొంతర_TV_BOT(0);
		    }
		}
		ఆపు;

	    // store an autoload variable
	    స్థితి ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		సమితి_var(iptr->isn_arg.string, దొంతర_TV_BOT(-1), TRUE);
		clear_tv(దొంతర_TV_BOT(-1));
		--ectx->ec_దొంతర.ga_len;
		ఆపు;

	    // store number in local variable
	    స్థితి ISN_STORENR:
		tv = దొంతర_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_సంఖ్య;
		tv->vval.v_సంఖ్య = iptr->isn_arg.storenr.stnr_val;
		ఆపు;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    స్థితి ISN_STOREINDEX:
		{
		    పూర్ణాంకం res = execute_storeindex(iptr, ectx);

		    ఒకవేళ (res == FAIL)
			వెళ్ళు on_error;
		    ఒకవేళ (res == NOTDONE)
			వెళ్ళు theend;
		}
		ఆపు;

	    // store value in list or blob range
	    స్థితి ISN_STORERANGE:
		ఒకవేళ (execute_storerange(iptr, ectx) == FAIL)
		    వెళ్ళు on_error;
		ఆపు;

	    స్థితి ISN_ఎక్కించు_CLASSMEMBER:
		{
		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      దొంతర_TV_BOT(0));
		    ++ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    స్థితి ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *దొంతర_TV_BOT(-1);
		    --ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    // Load or store variable or argument from outer scope.
	    స్థితి ISN_LOADOUTER:
	    స్థితి ISN_STOREOUTER:
		{
		    పూర్ణాంకం		లోతు = iptr->isn_arg.outer.outer_లోతు;
		    outer_T	*outer = ectx->ec_outer_ref == ఖాళీ ? ఖాళీ
						: ectx->ec_outer_ref->or_outer;

		    అయితే (లోతు > 1 && outer != ఖాళీ)
		    {
			outer = outer->బహిర్గతం_పైకి;
			--లోతు;
		    }
		    ఒకవేళ (outer == ఖాళీ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ఒకవేళ (ectx->ec_చట్రం_idx == ectx->ec_initial_చట్రం_idx
						 || ectx->ec_outer_ref == ఖాళీ)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_సంవృతం_called_from_invalid_context));
			లేకపోతే
			    iemsg("LOADOUTER depth more than scope levels");
			వెళ్ళు theend;
		    }
		    ఒకవేళ (లోతు < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->బహిర్గతం_loop[-లోతు - 1]
							       .దొంతర->ga_దత్తాంశం)
					  + outer->బహిర్గతం_loop[-లోతు - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    లేకపోతే
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->బహిర్గతం_దొంతర->ga_దత్తాంశం)
				      + outer->బహిర్గతం_చట్రం_idx + దొంతర_చట్రం_కొలత
				      + iptr->isn_arg.outer.outer_idx;
		    ఒకవేళ (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			    వెళ్ళు theend;
			// careful: ga_grow_inner may re-alloc the stack
			ఒకవేళ (లోతు < 0)
			    copy = ((typval_T *)outer->బహిర్గతం_loop[-లోతు - 1]
								   .దొంతర->ga_దత్తాంశం)
					      + outer->బహిర్గతం_loop[-లోతు - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			లేకపోతే
			    copy = ((typval_T *)outer->బహిర్గతం_దొంతర->ga_దత్తాంశం)
					  + outer->బహిర్గతం_చట్రం_idx + దొంతర_చట్రం_కొలత
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			ఒకవేళ (copy != tv)
			    tv = copy;
			copy_tv(tv, దొంతర_TV_BOT(0));
			++ectx->ec_దొంతర.ga_len;
		    }
		    లేకపోతే
		    {
			--ectx->ec_దొంతర.ga_len;
			clear_tv(tv);
			*tv = *దొంతర_TV_BOT(0);
		    }
		}
		ఆపు;

	    // unlet item in list or dict variable
	    స్థితి ISN_UNLETINDEX:
		ఒకవేళ (execute_unletindex(iptr, ectx) == FAIL)
		    వెళ్ళు on_error;
		ఆపు;

	    // unlet range of items in list variable
	    స్థితి ISN_UNLETRANGE:
		ఒకవేళ (execute_unletrange(iptr, ectx) == FAIL)
		    వెళ్ళు on_error;
		ఆపు;

	    // push constant
	    స్థితి ISN_PUSHNR:
	    స్థితి ISN_PUSHBOOL:
	    స్థితి ISN_PUSHSPEC:
	    స్థితి ISN_PUSHF:
	    స్థితి ISN_PUSHS:
	    స్థితి ISN_PUSHBLOB:
	    స్థితి ISN_PUSHFUNC:
	    స్థితి ISN_PUSHCHANNEL:
	    స్థితి ISN_PUSHJOB:
	    స్థితి ISN_PUSHOBJ:
	    స్థితి ISN_PUSHCLASS:
		ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    వెళ్ళు theend;
		tv = దొంతర_TV_BOT(0);
		tv->v_తాళం = 0;
		++ectx->ec_దొంతర.ga_len;
		ఎంపిక (iptr->isn_type)
		{
		    స్థితి ISN_PUSHNR:
			tv->v_type = VAR_సంఖ్య;
			tv->vval.v_సంఖ్య = iptr->isn_arg.సంఖ్య;
			ఆపు;
		    స్థితి ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_సంఖ్య = iptr->isn_arg.సంఖ్య;
			ఆపు;
		    స్థితి ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_సంఖ్య = iptr->isn_arg.సంఖ్య;
			ఆపు;
		    స్థితి ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			ఆపు;
		    స్థితి ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			ఆపు;
		    స్థితి ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			ఒకవేళ (iptr->isn_arg.string == ఖాళీ)
			    tv->vval.v_string = ఖాళీ;
			లేకపోతే
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			ఆపు;
		    స్థితి ISN_PUSHCHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = ఖాళీ;
#అయితే_ముగింపు
			ఆపు;
		    స్థితి ISN_PUSHJOB:
#నిర్వచితమైతే FEAT_పని_CHANNEL
			tv->v_type = VAR_పని;
			tv->vval.v_పని = ఖాళీ;
#అయితే_ముగింపు
			ఆపు;
		    స్థితి ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = ఖాళీ;
			ఆపు;
		    స్థితి ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			ఆపు;
		    అప్రమేయ:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == ఖాళీ
				    ? ఖాళీ : vim_strsave(iptr->isn_arg.string);
		}
		ఆపు;

	    స్థితి ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (శూన్య)script_autoload(name, FALSE);
		    ఒకవేళ (కనుగొను_func(name, TRUE))
		    {
			ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			    వెళ్ళు theend;
			tv = దొంతర_TV_BOT(0);
			tv->v_తాళం = 0;
			++ectx->ec_దొంతర.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    లేకపోతే
		    {
			పూర్ణాంకం res = ఎక్కించు_namespace_var(ectx, ISN_LOADG, iptr);

			ఒకవేళ (res == NOTDONE)
			    వెళ్ళు theend;
			ఒకవేళ (res == FAIL)
			    వెళ్ళు on_error;
		    }
		}
		ఆపు;

	    స్థితి ISN_UNLET:
		ఒకవేళ (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    వెళ్ళు on_error;
		ఆపు;
	    స్థితి ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		ఆపు;

	    స్థితి ISN_LOCKUNLOCK:
		{
#నిర్వచితమైతే LOG_LOCKVAR
		    ch_log(ఖాళీ, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#అయితే_ముగింపు
		    lval_మూలం_T	*lval_మూలం_save = lval_మూలం;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_దొంతర.ga_len;
		    lval_మూలం_T మూలం;
		    మూలం.lr_tv      = దొంతర_TV_BOT(0);
		    మూలం.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    మూలం.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_మూలం = &మూలం;
		    పూర్ణాంకం res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(మూలం.lr_tv);
		    lval_మూలం = lval_మూలం_save;
		    ఒకవేళ (res == FAIL)
			వెళ్ళు on_error;
		}
		ఆపు;

	    స్థితి ISN_LOCKCONST:
		అంశం_తాళం(దొంతర_TV_BOT(-1), 100, TRUE, TRUE);
		ఆపు;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    స్థితి ISN_NEWLIST:
		ఒకవేళ (exe_newlist(iptr->isn_arg.సంఖ్య, ectx) == FAIL)
		    వెళ్ళు theend;
		ఆపు;

	    // create a tuple from items on the stack
	    స్థితి ISN_NEWTUPLE:
		ఒకవేళ (exe_newtuple(iptr->isn_arg.సంఖ్య, ectx) == FAIL)
		    వెళ్ళు theend;
		ఆపు;

	    // create a dict from items on the stack
	    స్థితి ISN_NEWDICT:
		{
		    పూర్ణాంకం res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.సంఖ్య, ectx);
		    ఒకవేళ (res == FAIL)
			వెళ్ళు theend;
		    ఒకవేళ (res == MAYBE)
			వెళ్ళు on_error;
		}
		ఆపు;

	    స్థితి ISN_CONCAT:
		ఒకవేళ (exe_concat(iptr->isn_arg.సంఖ్య, ectx) == FAIL)
		    వెళ్ళు theend;
		ఆపు;

	    // create a partial with NULL value
	    స్థితి ISN_NEWPARTIAL:
		ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    వెళ్ళు theend;
		++ectx->ec_దొంతర.ga_len;
		tv = దొంతర_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_తాళం = 0;
		tv->vval.v_partial = ఖాళీ;
		ఆపు;

	    // call a :def function
	    స్థితి ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ఒకవేళ (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				ఖాళీ,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    వెళ్ళు on_error;
		ఆపు;

	    // call a method on an interface
	    స్థితి ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = దొంతర_TV_BOT(-1 - mfunc->cmf_argcount);
		    ఒకవేళ (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			వెళ్ళు on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ఒకవేళ (obj == ఖాళీ)
		    {
			emsg(_(e_using_null_object));
			వెళ్ళు on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    ఒకవేళ (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    లేకపోతే
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			పూర్ణాంకం idx = object_సూచీ_from_itf_సూచీ(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    ఒకవేళ (mfunc->cmf_ufunc != ఖాళీ && IS_GENERIC_FUNC(ufunc))
		    {
			ఒకవేళ (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			లేకపోతే
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    ఒకవేళ (ufunc == ఖాళీ)
				వెళ్ళు on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    ఒకవేళ (call_ufunc(ufunc, ఖాళీ, mfunc->cmf_argcount, ectx,
							   ఖాళీ, ఖాళీ) == FAIL)
			వెళ్ళు on_error;
		}
		ఆపు;

	    // call a builtin function
	    స్థితి ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ఒకవేళ (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    వెళ్ళు on_error;
		ఆపు;

	    // call a funcref or partial
	    స్థితి ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    పూర్ణాంకం		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (pfunc->cpf_శిఖరం)
		    {
			// funcref is above the arguments
			tv = దొంతర_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    లేకపోతే
		    {
			// Get the funcref from the stack.
			--ectx->ec_దొంతర.ga_len;
			partial_tv = *దొంతర_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    ఒకవేళ (tv == &partial_tv)
			clear_tv(&partial_tv);
		    ఒకవేళ (r == FAIL)
			వెళ్ళు on_error;
		}
		ఆపు;

	    స్థితి ISN_PCALL_చివర:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_దొంతర.ga_len;
		clear_tv(దొంతర_TV_BOT(-1));
		*దొంతర_TV_BOT(-1) = *దొంతర_TV_BOT(0);
		ఆపు;

	    // call a user defined function or funcref/partial
	    స్థితి ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			వెళ్ళు on_error;
		}
		ఆపు;

	    // :defer func(arg)
	    స్థితి ISN_DEFER:
		ఒకవేళ (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    వెళ్ళు on_error;
		ఆపు;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    స్థితి ISN_RETURN_VOID:
	    స్థితి ISN_RETURN_OBJECT:
		ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    వెళ్ళు theend;
		tv = దొంతర_TV_BOT(0);
		++ectx->ec_దొంతర.ga_len;
		ఒకవేళ (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_సంఖ్య = 0;
		    tv->v_తాళం = 0;
		}
		లేకపోతే
		{
		    *tv = *దొంతర_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_తాళం_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    స్థితి ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ఖాళీ;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    ఒకవేళ (check_typval_is_విలువ(దొంతర_TV_BOT(-1)) == FAIL)
			వెళ్ళు theend;

		    ఒకవేళ (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_దత్తాంశం)
							+ trystack->ga_len - 1;
		    ఒకవేళ (trycmd != ఖాళీ
				 && trycmd->tcd_చట్రం_idx == ectx->ec_చట్రం_idx)
		    {
			// jump to ":finally" or ":endtry"
			ఒకవేళ (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			లేకపోతే
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    లేకపోతే
			వెళ్ళు func_return;
		}
		ఆపు;

	    // push a partial, a reference to a compiled function
	    స్థితి ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    ఒకవేళ (pt == ఖాళీ)
			వెళ్ళు theend;
		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    {
			vim_free(pt);
			వెళ్ళు theend;
		    }
		    ఒకవేళ (extra != ఖాళీ && extra->fre_class != ఖాళీ)
		    {
			class_T	*cl;
			ఒకవేళ (extra->fre_object_పద్ధతి)
			{
			    tv = దొంతర_TV_BOT(-1);
			    ఒకవేళ (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				వెళ్ళు on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    ఒకవేళ (obj == ఖాళీ)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				వెళ్ళు on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_దొంతర.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			లేకపోతే
			    cl = extra->fre_class;

			ఒకవేళ (extra->fre_object_పద్ధతి)
			{
			    // object method
			    // convert the interface index to the object index
			    పూర్ణాంకం idx =
				object_సూచీ_from_itf_సూచీ(extra->fre_class,
					TRUE, extra->fre_పద్ధతి_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			లేకపోతే
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_పద్ధతి_idx];
			}
		    }
		    లేకపోతే ఒకవేళ (extra == ఖాళీ || extra->fre_func_name == ఖాళీ)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    లేకపోతే
		    {
			ufunc = కనుగొను_func(extra->fre_func_name, FALSE);
		    }
		    ఒకవేళ (ufunc == ఖాళీ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			వెళ్ళు theend;
		    }
		    ఒకవేళ (extra != ఖాళీ && extra->fre_outer_లోతు > 0
					   && (ufunc->uf_flags & FC_సంవృతం))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == ఖాళీ
				    ? ఖాళీ : ectx->ec_outer_ref->or_partial;

			కొరకు (పూర్ణాంకం లోతు = 1; outer_pt != ఖాళీ
				    && లోతు < extra->fre_outer_లోతు; ++లోతు)
			    outer_pt = get_pt_outer(outer_pt)->బహిర్గతం_పైకి_partial;
			ఒకవేళ (outer_pt == ఖాళీ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    వెళ్ళు theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    లేకపోతే ఒకవేళ (fill_partial_and_సంవృతం(pt, ufunc,
			       extra == ఖాళీ ? ఖాళీ : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			వెళ్ళు theend;
		    }
		    tv = దొంతర_TV_BOT(0);
		    ++ectx->ec_దొంతర.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_తాళం = 0;
		}
		ఆపు;

	    // Create a global function from a lambda.
	    స్థితి ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ఒకవేళ (copy_lambda_గమ్యం_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			వెళ్ళు theend;
		}
		ఆపు;

	    // List functions
	    స్థితి ISN_DEF:
		ఒకవేళ (iptr->isn_arg.string == ఖాళీ)
		    list_functions(ఖాళీ);
		లేకపోతే
		{
		    exarg_T	ea;
		    garray_T	పంక్తులు_గమ్యం_free;

		    CLEAR_క్షేత్రం(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&పంక్తులు_గమ్యం_free, పరిమాణం(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, ఖాళీ, &పంక్తులు_గమ్యం_free, 0, ఖాళీ, 0, ఖాళీ);
		    ga_clear_అక్షరశ్రేణులు(&పంక్తులు_గమ్యం_free);
		}
		ఆపు;

	    // jump if a condition is met
	    స్థితి ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    పూర్ణాంకం		error = FALSE;
		    పూర్ణాంకం		jump = TRUE;

		    ఒకవేళ (when != JUMP_ALWAYS)
		    {
			tv = దొంతర_TV_BOT(-1);
			ఒకవేళ (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    ఒకవేళ (error)
				వెళ్ళు on_error;
			}
			లేకపోతే
			    jump = tv2bool(tv);
			ఒకవేళ (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			ఒకవేళ (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_దొంతర.ga_len;
			}
		    }
		    ఒకవేళ (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		ఆపు;

	    // "while": jump to end if a condition is false
	    స్థితి ISN_WHILE:
		{
		    పూర్ణాంకం		error = FALSE;
		    పూర్ణాంకం		jump = TRUE;

		    tv = దొంతర_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    ఒకవేళ (error)
			వెళ్ళు on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_దొంతర.ga_len;
		    ఒకవేళ (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_చివర;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = దొంతర_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_సంఖ్య = ectx->ec_funcrefs.ga_len;
		}
		ఆపు;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    స్థితి ISN_JUMP_IF_ARG_సమితి:
	    స్థితి ISN_JUMP_IF_ARG_NOT_సమితి:
		tv = దొంతర_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		పూర్ణాంకం arg_సమితి = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_సంఖ్య == VVAL_NONE);

		ఒకవేళ (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_సమితి && !arg_సమితి)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_వాదనలు.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ దొంతర_చట్రం_కొలత;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_సూచిక(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    ఒకవేళ (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		ఒకవేళ (iptr->isn_type == ISN_JUMP_IF_ARG_సమితి ? arg_సమితి : !arg_సమితి)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		ఆపు;

	    // top of a for loop
	    స్థితి ISN_FOR:
		ఒకవేళ (execute_for(iptr, ectx) == FAIL)
		    వెళ్ళు theend;
		ఆపు;

	    // end of a for or while loop
	    స్థితి ISN_ENDLOOP:
		ఒకవేళ (execute_endloop(iptr, ectx) == FAIL)
		    వెళ్ళు theend;
		ఆపు;

	    // start of ":try" block
	    స్థితి ISN_TRY:
		{
		    trycmd_T    *trycmd = ఖాళీ;

		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			వెళ్ళు theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_దత్తాంశం)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_సూచిక(trycmd);
		    trycmd->tcd_చట్రం_idx = ectx->ec_చట్రం_idx;
		    trycmd->tcd_దొంతర_len = ectx->ec_దొంతర.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		ఆపు;

	    స్థితి ISN_PUSHEXC:
		ఒకవేళ (current_exception == ఖాళీ)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    వెళ్ళు theend;
		}
		ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
		    వెళ్ళు theend;
		tv = దొంతర_TV_BOT(0);
		++ectx->ec_దొంతర.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_తాళం = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->విలువ);
		ఆపు;

	    స్థితి ISN_CATCH:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    may_restore_cmdmod(&ectx->ec_funclocal);
		    trycmd = ((trycmd_T *)trystack->ga_దత్తాంశం)
							+ trystack->ga_len - 1;
		    trycmd->tcd_caught = TRUE;
		    trycmd->tcd_did_throw = FALSE;

		    did_emsg = got_int = did_throw = FALSE;
		    force_abort = need_rethrow = FALSE;
		    catch_exception(current_exception);
		}
		ఆపు;

	    స్థితి ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    పూర్ణాంకం		i;
		    trycmd_T    *trycmd;
		    పూర్ణాంకం		iidx = trycont->tct_where;

		    ఒకవేళ (trystack->ga_len < trycont->tct_స్థాయిలు)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_స్థాయిలు, trystack->ga_len);
			వెళ్ళు theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    కొరకు (i = trycont->tct_స్థాయిలు; i > 0; --i)
		    {
			trycmd = ((trycmd_T *)trystack->ga_దత్తాంశం)
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
		ఆపు;

	    స్థితి ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_దత్తాంశం)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    ఒకవేళ (trycmd->tcd_caught)
		    {
			// discard the exception
			ముగింపు_exception(caught_దొంతర);
			trycmd->tcd_caught = FALSE;
		    }
		    ఆపు;
		}

	    // end of ":try" block
	    స్థితి ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_దత్తాంశం) + trystack->ga_len;
		    ఒకవేళ (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    ఒకవేళ (trycmd->tcd_caught)
		    {
			// discard the exception
			ముగింపు_exception(caught_దొంతర);
		    }

		    ఒకవేళ (trycmd->tcd_return)
			వెళ్ళు func_return;

		    అయితే (ectx->ec_దొంతర.ga_len > trycmd->tcd_దొంతర_len)
		    {
			--ectx->ec_దొంతర.ga_len;
			clear_tv(దొంతర_TV_BOT(0));
		    }
		    ఒకవేళ (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		ఆపు;

	    స్థితి ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    ఒకవేళ (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = దొంతర_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_సంఖ్య;
			tv->vval.v_సంఖ్య = 0;
			వెళ్ళు పూర్తి;
		    }
		    --ectx->ec_దొంతర.ga_len;
		    tv = దొంతర_TV_BOT(0);
		    ఒకవేళ (tv->vval.v_string == ఖాళీ
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_ఖాళీలు_string));
			వెళ్ళు theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    ఒకవేళ (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_దత్తాంశం)
							+ trystack->ga_len - 1;
			ఒకవేళ (trycmd->tcd_caught)
			{
			    // discard the exception
			    ముగింపు_exception(caught_దొంతర);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (throw_exception(tv->vval.v_string, ET_USER, ఖాళీ)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			వెళ్ళు theend;
		    }
		    did_throw = TRUE;
		}
		ఆపు;

	    // compare with special values
	    స్థితి ISN_COMPAREBOOL:
	    స్థితి ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = దొంతర_TV_BOT(-2);
		    typval_T	*tv2 = దొంతర_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_సంఖ్య;
		    varnumber_T arg2 = tv2->vval.v_సంఖ్య;
		    పూర్ణాంకం		res;

		    ఒకవేళ (iptr->isn_arg.op.op_type == సమాసం_EQUAL)
			res = arg1 == arg2;
		    లేకపోతే
			res = arg1 != arg2;

		    --ectx->ec_దొంతర.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_సంఖ్య = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ఆపు;

	    స్థితి ISN_COMPARENULL:
		{
		    typval_T	*tv1 = దొంతర_TV_BOT(-2);
		    typval_T	*tv2 = దొంతర_TV_BOT(-1);
		    పూర్ణాంకం		res;

		    res = typval_compare_null(tv1, tv2);
		    ఒకవేళ (res == MAYBE)
			వెళ్ళు on_error;
		    ఒకవేళ (iptr->isn_arg.op.op_type == సమాసం_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_దొంతర.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_సంఖ్య = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ఆపు;

	    // Operation with two number arguments
	    స్థితి ISN_OPNR:
	    స్థితి ISN_COMPARENR:
		{
		    typval_T	*tv1 = దొంతర_TV_BOT(-2);
		    typval_T	*tv2 = దొంతర_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_సంఖ్య;
		    varnumber_T arg2 = tv2->vval.v_సంఖ్య;
		    varnumber_T res = 0;
		    పూర్ణాంకం		div_zero = FALSE;

		    ఒకవేళ (iptr->isn_arg.op.op_type == సమాసం_LSHIFT
			    || iptr->isn_arg.op.op_type == సమాసం_RSHIFT)
		    {
			ఒకవేళ (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_ధనాత్మక));
			    వెళ్ళు on_error;
			}
		    }

		    ఎంపిక (iptr->isn_arg.op.op_type)
		    {
			స్థితి సమాసం_MULT: res = arg1 * arg2; ఆపు;
			స్థితి సమాసం_DIV:  ఒకవేళ (arg2 == 0)
					    div_zero = TRUE;
					లేకపోతే
					    res = arg1 / arg2;
					ఆపు;
			స్థితి సమాసం_REM:  ఒకవేళ (arg2 == 0)
					    div_zero = TRUE;
					లేకపోతే
					    res = arg1 % arg2;
					ఆపు;
			స్థితి సమాసం_SUB: res = arg1 - arg2; ఆపు;
			స్థితి సమాసం_కూడు: res = arg1 + arg2; ఆపు;

			స్థితి సమాసం_EQUAL: res = arg1 == arg2; ఆపు;
			స్థితి సమాసం_NEQUAL: res = arg1 != arg2; ఆపు;
			స్థితి సమాసం_GREATER: res = arg1 > arg2; ఆపు;
			స్థితి సమాసం_GEQUAL: res = arg1 >= arg2; ఆపు;
			స్థితి సమాసం_SMALLER: res = arg1 < arg2; ఆపు;
			స్థితి సమాసం_SEQUAL: res = arg1 <= arg2; ఆపు;
			స్థితి సమాసం_LSHIFT: ఒకవేళ (arg2 > MAX_LSHIFT_బిట్లు)
					      res = 0;
					  లేకపోతే
					      res = (uvarnumber_T)arg1 << arg2;
					  ఆపు;
			స్థితి సమాసం_RSHIFT: ఒకవేళ (arg2 > MAX_LSHIFT_బిట్లు)
					      res = 0;
					  లేకపోతే
					      res = (uvarnumber_T)arg1 >> arg2;
					  ఆపు;
			అప్రమేయ: ఆపు;
		    }

		    --ectx->ec_దొంతర.ga_len;
		    ఒకవేళ (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_సంఖ్య = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    లేకపోతే
			tv1->vval.v_సంఖ్య = res;
		    ఒకవేళ (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			వెళ్ళు on_error;
		    }
		}
		ఆపు;

	    // Computation with two float arguments
	    స్థితి ISN_OPFLOAT:
	    స్థితి ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = దొంతర_TV_BOT(-2);
		    typval_T	*tv2 = దొంతర_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    పూర్ణాంకం		cmp = FALSE;

		    ఎంపిక (iptr->isn_arg.op.op_type)
		    {
			స్థితి సమాసం_MULT: res = arg1 * arg2; ఆపు;
			స్థితి సమాసం_DIV: res = arg1 / arg2; ఆపు;
			స్థితి సమాసం_SUB: res = arg1 - arg2; ఆపు;
			స్థితి సమాసం_కూడు: res = arg1 + arg2; ఆపు;

			స్థితి సమాసం_EQUAL: cmp = arg1 == arg2; ఆపు;
			స్థితి సమాసం_NEQUAL: cmp = arg1 != arg2; ఆపు;
			స్థితి సమాసం_GREATER: cmp = arg1 > arg2; ఆపు;
			స్థితి సమాసం_GEQUAL: cmp = arg1 >= arg2; ఆపు;
			స్థితి సమాసం_SMALLER: cmp = arg1 < arg2; ఆపు;
			స్థితి సమాసం_SEQUAL: cmp = arg1 <= arg2; ఆపు;
			అప్రమేయ: cmp = 0; ఆపు;
		    }
		    --ectx->ec_దొంతర.ga_len;
		    ఒకవేళ (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_సంఖ్య = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    లేకపోతే
			tv1->vval.v_float = res;
		}
		ఆపు;

	    స్థితి ISN_COMPARELIST:
	    స్థితి ISN_COMPARETUPLE:
	    స్థితి ISN_COMPAREDICT:
	    స్థితి ISN_COMPAREFUNC:
	    స్థితి ISN_COMPARESTRING:
	    స్థితి ISN_COMPAREBLOB:
	    స్థితి ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = దొంతర_TV_BOT(-2);
		    typval_T	*tv2 = దొంతర_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    పూర్ణాంకం		ic = iptr->isn_arg.op.op_ic;
		    పూర్ణాంకం		res = FALSE;
		    పూర్ణాంకం		స్థితి_2 = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (iptr->isn_type == ISN_COMPARELIST)
		    {
			స్థితి_2 = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    లేకపోతే ఒకవేళ (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			స్థితి_2 = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    లేకపోతే ఒకవేళ (iptr->isn_type == ISN_COMPAREDICT)
		    {
			స్థితి_2 = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    లేకపోతే ఒకవేళ (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			స్థితి_2 = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    లేకపోతే ఒకవేళ (iptr->isn_type == ISN_COMPARESTRING)
		    {
			స్థితి_2 = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    లేకపోతే ఒకవేళ (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			స్థితి_2 = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    లేకపోతే // ISN_COMPAREOBJECT
		    {
			స్థితి_2 = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_దొంతర.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_సంఖ్య = res ? VVAL_TRUE : VVAL_FALSE;
		    ఒకవేళ (స్థితి_2 == FAIL)
			వెళ్ళు theend;
		}
		ఆపు;

	    స్థితి ISN_COMPAREANY:
		{
		    typval_T	*tv1 = దొంతర_TV_BOT(-2);
		    typval_T	*tv2 = దొంతర_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    పూర్ణాంకం		ic = iptr->isn_arg.op.op_ic;
		    పూర్ణాంకం		స్థితి_2;

		    SOURCING_LNUM = iptr->isn_lnum;
		    స్థితి_2 = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_దొంతర.ga_len;
		    ఒకవేళ (స్థితి_2 == FAIL)
			వెళ్ళు theend;
		}
		ఆపు;

	    స్థితి ISN_ADDLIST:
	    స్థితి ISN_ADDTUPLE:
	    స్థితి ISN_ADDBLOB:
		{
		    typval_T *tv1 = దొంతర_TV_BOT(-2);
		    typval_T *tv2 = దొంతర_TV_BOT(-1);

		    // add two lists or blobs
		    ఒకవేళ (iptr->isn_type == ISN_ADDLIST)
		    {
			ఒకవేళ (iptr->isn_arg.op.op_type == సమాసం_APPEND
						   && tv1->vval.v_list != ఖాళీ)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 ఖాళీ);
			లేకపోతే
			    eval_addlist(tv1, tv2);
		    }
		    లేకపోతే ఒకవేళ (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    లేకపోతే
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    స్థితి ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = దొంతర_TV_BOT(-2);
		    typval_T	*tv2 = దొంతర_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (l == ఖాళీ)
		    {
			emsg(_(e_cannot_కూడు_గమ్యం_null_list));
			వెళ్ళు on_error;
		    }
		    ఒకవేళ (విలువ_check_తాళం(l->lv_తాళం, ఖాళీ, FALSE))
			వెళ్ళు on_error;
		    ఒకవేళ (list_append_tv(l, tv2) == FAIL)
			వెళ్ళు theend;
		    clear_tv(tv2);
		    --ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    స్థితి ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = దొంతర_TV_BOT(-2);
		    typval_T	*tv2 = దొంతర_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    పూర్ణాంకం		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    ఒకవేళ (b == ఖాళీ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_కూడు_గమ్యం_null_blob));
			వెళ్ళు on_error;
		    }
		    n = tv_get_సంఖ్య_chk(tv2, &error);
		    ఒకవేళ (error)
			వెళ్ళు on_error;
		    ga_append(&b->bv_ga, (పూర్ణాంకం)n);
		    --ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    // Computation with two arguments of unknown type
	    స్థితి ISN_OPANY:
		{
		    typval_T	*tv1 = దొంతర_TV_BOT(-2);
		    typval_T	*tv2 = దొంతర_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    పూర్ణాంకం		error = FALSE;

		    ఒకవేళ (iptr->isn_arg.op.op_type == సమాసం_కూడు)
		    {
			ఒకవేళ (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_దొంతర.ga_len;
			    ఆపు;
			}
			లేకపోతే ఒకవేళ (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_దొంతర.ga_len;
			    ఆపు;
			}
			లేకపోతే ఒకవేళ (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_దొంతర.ga_len;
			    ఆపు;
			}
		    }
		    ఒకవేళ (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    లేకపోతే
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_సంఖ్య_chk(tv1, &error);
			ఒకవేళ (error)
			    వెళ్ళు on_error;
			ఒకవేళ (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    ఒకవేళ (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    లేకపోతే
		    {
			n2 = tv_get_సంఖ్య_chk(tv2, &error);
			ఒకవేళ (error)
			    వెళ్ళు on_error;
			ఒకవేళ (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    ఒకవేళ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			ఎంపిక (iptr->isn_arg.op.op_type)
			{
			    స్థితి సమాసం_MULT: f1 = f1 * f2; ఆపు;
			    స్థితి సమాసం_DIV:  f1 = f1 / f2; ఆపు;
			    స్థితి సమాసం_SUB:  f1 = f1 - f2; ఆపు;
			    స్థితి సమాసం_కూడు:  f1 = f1 + f2; ఆపు;
			    అప్రమేయ: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     వెళ్ళు on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_దొంతర.ga_len;
		    }
		    లేకపోతే
		    {
			పూర్ణాంకం failed = FALSE;

			ఎంపిక (iptr->isn_arg.op.op_type)
			{
			    స్థితి సమాసం_MULT: n1 = n1 * n2; ఆపు;
			    స్థితి సమాసం_DIV:  n1 = అంకె_divide(n1, n2, &failed);
					    ఒకవేళ (failed)
						వెళ్ళు on_error;
					    ఆపు;
			    స్థితి సమాసం_SUB:  n1 = n1 - n2; ఆపు;
			    స్థితి సమాసం_కూడు:  n1 = n1 + n2; ఆపు;
			    అప్రమేయ:	    n1 = అంకె_modulus(n1, n2, &failed);
					    ఒకవేళ (failed)
						వెళ్ళు on_error;
					    ఆపు;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_సంఖ్య;
			tv1->vval.v_సంఖ్య = n1;
			--ectx->ec_దొంతర.ga_len;
		    }
		}
		ఆపు;

	    స్థితి ISN_STRINDEX:
	    స్థితి ISN_STRSLICE:
		{
		    పూర్ణాంకం		is_ముక్క = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    ఒకవేళ (is_ముక్క)
		    {
			tv = దొంతర_TV_BOT(-2);
			n1 = tv->vval.v_సంఖ్య;
		    }

		    tv = దొంతర_TV_BOT(-1);
		    n2 = tv->vval.v_సంఖ్య;

		    ectx->ec_దొంతర.ga_len -= is_ముక్క ? 2 : 1;
		    tv = దొంతర_TV_BOT(-1);
		    ఒకవేళ (is_ముక్క)
			// Slice: Select the characters from the string
			res = string_ముక్క(tv->vval.v_string, n1, n2, FALSE);
		    లేకపోతే
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		ఆపు;

	    స్థితి ISN_LISTINDEX:
	    స్థితి ISN_LISTSLICE:
	    స్థితి ISN_TUPLEINDEX:
	    స్థితి ISN_TUPLESLICE:
	    స్థితి ISN_BLOBINDEX:
	    స్థితి ISN_BLOBSLICE:
		{
		    పూర్ణాంకం		is_ముక్క = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    పూర్ణాంకం		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    పూర్ణాంకం		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_ముక్క ? దొంతర_TV_BOT(-3) : దొంతర_TV_BOT(-2);

		    tv = దొంతర_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_సంఖ్య;
		    clear_tv(tv);

		    ఒకవేళ (is_ముక్క)
		    {
			tv = దొంతర_TV_BOT(-2);
			n1 = tv->vval.v_సంఖ్య;
			clear_tv(tv);
		    }

		    ectx->ec_దొంతర.ga_len -= is_ముక్క ? 2 : 1;
		    tv = దొంతర_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (is_blob)
		    {
			ఒకవేళ (blob_ముక్క_or_సూచీ(val_tv->vval.v_blob, is_ముక్క,
						    n1, n2, FALSE, tv) == FAIL)
			    వెళ్ళు on_error;
		    }
		    లేకపోతే ఒకవేళ (is_tuple)
		    {
			ఒకవేళ (tuple_ముక్క_or_సూచీ(val_tv->vval.v_tuple,
				is_ముక్క, n1, n2, FALSE, tv, TRUE) == FAIL)
			    వెళ్ళు on_error;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (list_ముక్క_or_సూచీ(val_tv->vval.v_list, is_ముక్క,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    వెళ్ళు on_error;
		    }
		}
		ఆపు;

	    స్థితి ISN_ANYINDEX:
	    స్థితి ISN_ANYSLICE:
		{
		    పూర్ణాంకం		is_ముక్క = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    పూర్ణాంకం		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_ముక్క ? దొంతర_TV_BOT(-3) : దొంతర_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (check_can_సూచీ(tv, TRUE, TRUE) == FAIL)
			వెళ్ళు on_error;
		    var1 = is_ముక్క ? దొంతర_TV_BOT(-2) : దొంతర_TV_BOT(-1);
		    var2 = is_ముక్క ? దొంతర_TV_BOT(-1) : ఖాళీ;
		    res = eval_సూచీ_inner(tv, is_ముక్క, var1, var2,
							FALSE, ఖాళీ, -1, TRUE);
		    clear_tv(var1);
		    ఒకవేళ (is_ముక్క)
			clear_tv(var2);
		    ectx->ec_దొంతర.ga_len -= is_ముక్క ? 2 : 1;
		    ఒకవేళ (res == FAIL)
			వెళ్ళు on_error;
		}
		ఆపు;

	    స్థితి ISN_ముక్క:
		{
		    పూర్ణాంకం		లెక్క = iptr->isn_arg.సంఖ్య;

		    // type will have been checked to be a list
		    tv = దొంతర_TV_BOT(-1);
		    ఒకవేళ (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			ఒకవేళ (list != ఖాళీ && list->lv_len >= లెక్క)
			{
			    list_T	*newlist = list_ముక్క(list,
				    లెక్క, list->lv_len - 1);

			    ఒకవేళ (newlist != ఖాళీ)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    లేకపోతే
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			ఒకవేళ (tuple != ఖాళీ && TUPLE_LEN(tuple) >= లెక్క)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_ముక్క(tuple, లెక్క,
							TUPLE_LEN(tuple) - 1);
			    ఒకవేళ (newtuple != ఖాళీ)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		ఆపు;

	    స్థితి ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*అంశం_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = దొంతర_TV_BOT(-1 - gi->gi_with_op);
		    ఒకవేళ (tv->v_type == VAR_LIST)
		    {
			li = list_కనుగొను(tv->vval.v_list, gi->gi_సూచీ);
			ఒకవేళ (li == ఖాళీ)
			    వెళ్ళు on_error;
			అంశం_tv = &li->li_tv;
		    }
		    లేకపోతే
			అంశం_tv = TUPLE_అంశం(tv->vval.v_tuple, gi->gi_సూచీ);

		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    ++ectx->ec_దొంతర.ga_len;
		    copy_tv(అంశం_tv, దొంతర_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_సూచీ = gi->gi_సూచీ + 1;
		    ectx->ec_where.wt_రకం = WT_VARIABLE;
		}
		ఆపు;

	    // dict member with string key (dict['member'])
	    స్థితి ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*కీలకం;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = దొంతర_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = దొంతర_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    కీలకం = tv->vval.v_string;
		    ఒకవేళ (కీలకం == ఖాళీ)
			కీలకం = (char_u *)"";

		    ఒకవేళ ((di = dict_కనుగొను(dict, కీలకం, -1)) == ఖాళీ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_కీలకం_not_present_లోపల_dictionary_str), కీలకం);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_దొంతర.ga_len;
			tv = దొంతర_TV_BOT(-1);
			(శూన్య) dict_దొంతర_save(tv);
			tv->v_type = VAR_సంఖ్య;
			tv->vval.v_సంఖ్య = 0;
			వెళ్ళు on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_దొంతర.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = దొంతర_TV_BOT(-1);
		    ఒకవేళ (dict_దొంతర_save(tv) == FAIL)
			వెళ్ళు on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		ఆపు;

	    // dict member with string key (dict.member)
	    // or can be an object
	    స్థితి ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = దొంతర_TV_BOT(-1);

		    ఒకవేళ (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			ఒకవేళ (dict_దొంతర_save(tv) == FAIL)
			    వెళ్ళు on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_దత్తాంశం)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			ఒకవేళ (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    వెళ్ళు on_error;
		    }
		    లేకపోతే
		    {
			ఒకవేళ (tv->v_type != VAR_DICT || tv->vval.v_dict == ఖాళీ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    వెళ్ళు on_error;
			}
			dict = tv->vval.v_dict;

			ఒకవేళ ((di = dict_కనుగొను(dict, iptr->isn_arg.string, -1))
								   == ఖాళీ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_కీలకం_not_present_లోపల_dictionary_str),
						     iptr->isn_arg.string);
			    వెళ్ళు on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			ఒకవేళ (dict_దొంతర_save(tv) == FAIL)
			    వెళ్ళు on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		ఆపు;

	    స్థితి ISN_GET_OBJ_MEMBER:
	    స్థితి ISN_GET_ITF_MEMBER:
		{
		    tv = దొంతర_TV_BOT(-1);
		    ఒకవేళ (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			వెళ్ళు on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ఒకవేళ (obj == ఖాళీ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			వెళ్ళు on_error;
		    }

		    పూర్ణాంకం idx;
		    ఒకవేళ (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    లేకపోతే
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_సూచీ_from_itf_సూచీ(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    ఒకవేళ (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			వెళ్ళు on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		ఆపు;

	    స్థితి ISN_STORE_THIS:
		{
		    పూర్ణాంకం idx = iptr->isn_arg.సంఖ్య;
		    object_T *obj = దొంతర_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *దొంతర_TV_BOT(-1);
		    --ectx->ec_దొంతర.ga_len;
		}
		ఆపు;

	    స్థితి ISN_CLEARDICT:
		dict_దొంతర_drop();
		ఆపు;

	    స్థితి ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_దొంతర_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = దొంతర_TV_BOT(-1);
		    ఒకవేళ (dict_tv != ఖాళీ
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != ఖాళీ
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == ఖాళీ))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_దొంతర_drop();
		}
		ఆపు;

	    స్థితి ISN_NEGATENR:
		tv = దొంతర_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		ఒకవేళ (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		లేకపోతే
		    tv->vval.v_సంఖ్య = -tv->vval.v_సంఖ్య;
		ఆపు;

	    స్థితి ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    పూర్ణాంకం		r;
		    where_T	where = WHERE_INIT;

		    tv = దొంతర_TV_BOT((పూర్ణాంకం)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ఒకవేళ (ct->ct_arg_idx > 0)
		    {
			where.wt_సూచీ = ct->ct_arg_idx;
			where.wt_రకం = ct->ct_is_var ? WT_VARIABLE : WT_వాదం;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    ఒకవేళ (r == FAIL)
			వెళ్ళు on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    ఒకవేళ (tv->v_type == VAR_సంఖ్య
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_సంఖ్య == 0
						|| tv->vval.v_సంఖ్య == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_సంఖ్య = tv->vval.v_సంఖ్య
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		ఆపు;

	    స్థితి ISN_CHECKLEN:
		{
		    పూర్ణాంకం	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = ఖాళీ;
		    tuple_T *tuple = ఖాళీ;
		    పూర్ణాంకం	    len = 0;

		    tv = దొంతర_TV_BOT(-1);

		    పూర్ణాంకం		len_check_failed = FALSE;
		    ఒకవేళ (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			ఒకవేళ (list == ఖాళీ || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_మరిన్ని_OK))
			    len_check_failed = TRUE;
			ఒకవేళ (list != ఖాళీ)
			    len = list->lv_len;
		    }
		    లేకపోతే ఒకవేళ (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			ఒకవేళ (tuple == ఖాళీ || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_మరిన్ని_OK))
			    len_check_failed = TRUE;
			ఒకవేళ (tuple != ఖాళీ)
			    len = TUPLE_LEN(tuple);
		    }
		    లేకపోతే
			len_check_failed = TRUE;

		    ఒకవేళ (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_వస్తువులు_but_got_nr),
				     min_len, len);
			వెళ్ళు on_error;
		    }
		}
		ఆపు;

	    స్థితి ISN_SETTYPE:
		సమితి_tv_type(దొంతర_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		ఆపు;

	    స్థితి ISN_2BOOL:
	    స్థితి ISN_COND2BOOL:
		{
		    పూర్ణాంకం n;
		    పూర్ణాంకం error = FALSE;

		    ఒకవేళ (iptr->isn_type == ISN_2BOOL)
		    {
			tv = దొంతర_TV_BOT(iptr->isn_arg.tobool.స్థానభ్రంశం);
			n = tv2bool(tv);
			ఒకవేళ (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    లేకపోతే
		    {
			tv = దొంతర_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			ఒకవేళ (error)
			    వెళ్ళు on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_సంఖ్య = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ఆపు;

	    స్థితి ISN_2STRING:
	    స్థితి ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		ఒకవేళ (do_2string(దొంతర_TV_BOT(iptr->isn_arg.tostring.స్థానభ్రంశం),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    వెళ్ళు on_error;
		ఆపు;

	    స్థితి ISN_RANGE:
		{
		    exarg_T	ea;
		    అక్షరం	*errormsg;

		    ea.line2 = 0;
		    ea.addr_లెక్క = 0;
		    ea.addr_type = ADDR_పంక్తులు;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    ఒకవేళ (parse_cmd_చిరునామా(&ea, &errormsg, FALSE) == FAIL)
			వెళ్ళు on_error;

		    ఒకవేళ (GA_GROW_FAILS(&ectx->ec_దొంతర, 1))
			వెళ్ళు theend;
		    ++ectx->ec_దొంతర.ga_len;
		    tv = దొంతర_TV_BOT(-1);
		    tv->v_type = VAR_సంఖ్య;
		    tv->v_తాళం = 0;
		    tv->vval.v_సంఖ్య = ea.line2;
		}
		ఆపు;

	    స్థితి ISN_PUT:
		tv = ఖాళీ; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		ఆపు;
	    స్థితి ISN_IPUT:
		tv = ఖాళీ;
		isn_put_do(ectx, iptr, tv, TRUE);
		ఆపు;

	    స్థితి ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_దొంతర.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		ఆపు;

	    స్థితి ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = ఖాళీ;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		ఆపు;

	    స్థితి ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = దొంతర_TV_BOT(-1);
		ఒకవేళ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_వాదం_must_be_sequence_యొక్క_lists_or_tuples));
		    వెళ్ళు on_error;
		}

		పూర్ణాంకం rc;
		ఒకవేళ (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		లేకపోతే
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		ఒకవేళ (rc != EXEC_OK)
		{
		    ఒకవేళ (rc == EXEC_FAIL)
			వెళ్ళు on_error;
		    వెళ్ళు theend;
		}
		ఆపు;

	    స్థితి ISN_PROF_ప్రారంభం:
	    స్థితి ISN_PROF_చివర:
		{
#నిర్వచితమైతే FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_దత్తాంశం)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    ఒకవేళ (iptr->isn_type == ISN_PROF_ప్రారంభం)
		    {
			func_పంక్తి_ప్రారంభం(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_పంక్తి_exec(&cookie);
		    }
		    లేకపోతే
			func_పంక్తి_చివర(&cookie);
#అయితే_ముగింపు
		}
		ఆపు;

	    స్థితి ISN_DEBUG:
		handle_debug(iptr, ectx);
		ఆపు;

	    స్థితి ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    పూర్ణాంకం		అంశం = iptr->isn_arg.shuffle.shfl_అంశం;
		    పూర్ణాంకం		పైకి = iptr->isn_arg.shuffle.shfl_పైకి;

		    tmp_tv = *దొంతర_TV_BOT(-అంశం);
		    కొరకు ( ; పైకి > 0 && అంశం > 1; --పైకి)
		    {
			*దొంతర_TV_BOT(-అంశం) = *దొంతర_TV_BOT(-అంశం + 1);
			--అంశం;
		    }
		    *దొంతర_TV_BOT(-అంశం) = tmp_tv;
		}
		ఆపు;

	    స్థితి ISN_DROP:
		--ectx->ec_దొంతర.ga_len;
		clear_tv(దొంతర_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		ఆపు;

	    స్థితి ISN_SCRIPTCTX_సమితి:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		ఆపు;
	}
	కొనసాగు;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	ఒకవేళ (ectx->ec_చట్రం_idx == ectx->ec_initial_చట్రం_idx)
	    వెళ్ళు పూర్తి;

	ఒకవేళ (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    వెళ్ళు theend;
	కొనసాగు;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	ఒకవేళ (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    ఒకవేళ (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		అయితే (ectx->ec_దొంతర.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_దొంతర.ga_len;
		    clear_tv(దొంతర_TV_BOT(0));
		}
		అయితే (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    కొనసాగు;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	ఒకవేళ (trylevel <= ectx->ec_trylevel_at_ప్రారంభం)
	    వెళ్ళు theend;
    }

పూర్తి:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_దొంతర_clear(dict_దొంతర_len_at_ప్రారంభం);
    ectx->ec_trylevel_at_ప్రారంభం = save_trylevel_at_ప్రారంభం;
    తిరిగి ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    పూర్ణాంకం
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    పూర్ణాంకం		save_iidx = ectx->ec_iidx;
    పూర్ణాంకం		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    ఒకవేళ (res == OK)
    {
	*rettv = *దొంతర_TV_BOT(-1);
	--ectx->ec_దొంతర.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    తిరిగి res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(శూన్య)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    పూర్ణాంకం		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    ఒకవేళ (exec_instructions(ectx) == OK)
    {
	typval_T *tv = దొంతర_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_దొంతర.ga_len;
	clear_tv(tv);
    }
    లేకపోతే
    {
	substitute_instr->subs_స్థితి = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    తిరిగి res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    పూర్ణాంకం
call_def_function(
    ufunc_T	*ufunc,
    పూర్ణాంకం		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    పూర్ణాంకం		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    పూర్ణాంకం		argc = argc_arg;
    పూర్ణాంకం		partial_argc = partial == ఖాళీ
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    పూర్ణాంకం		మొత్తం_argc = argc + partial_argc;
    typval_T	*tv;
    పూర్ణాంకం		idx;
    పూర్ణాంకం		ret = FAIL;
    పూర్ణాంకం		defcount = ufunc->uf_వాదనలు.ga_len - మొత్తం_argc;
    sctx_T	save_current_sctx = current_sctx;
    పూర్ణాంకం		did_emsg_before = did_emsg_cumul + did_emsg;
    పూర్ణాంకం		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_సందేశం_list = ఖాళీ;
    msglist_T	*వ్యక్తిగత_సందేశం_list = ఖాళీ;
    పూర్ణాంకం		save_emsg_silent_def = emsg_silent_def;
    పూర్ణాంకం		save_did_emsg_def = did_emsg_def;
    పూర్ణాంకం		orig_funcdepth;
    పూర్ణాంకం		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#నిర్వచనం_రద్దు దొంతర_TV
#నిర్వచించు దొంతర_TV(idx) (((typval_T *)ectx.ec_దొంతర.ga_దత్తాంశం) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#నిర్వచనం_రద్దు దొంతర_TV_BOT
#నిర్వచించు దొంతర_TV_BOT(idx) (((typval_T *)ectx.ec_దొంతర.ga_దత్తాంశం) + ectx.ec_దొంతర.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#నిర్వచనం_రద్దు దొంతర_TV_VAR
#నిర్వచించు దొంతర_TV_VAR(idx) (((typval_T *)ectx.ec_దొంతర.ga_దత్తాంశం) + ectx.ec_చట్రం_idx + దొంతర_చట్రం_కొలత + idx)

    ఒకవేళ (ufunc->uf_def_స్థితి == UF_NOT_COMPILED
	    || ufunc->uf_def_స్థితి == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), ఖాళీ) == FAIL))
    {
	ఒకవేళ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	తిరిగి FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							 + ufunc->uf_dfunc_idx;
	ఒకవేళ (dfunc->df_ufunc == ఖాళీ)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    తిరిగి FAIL;
	}
	ఒకవేళ (INSTRUCTIONS(dfunc) == ఖాళీ)
	{
	    iemsg("using call_def_function() on not compiled function");
	    తిరిగి FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    ఒకవేళ (funcdepth_increment() == FAIL)
	తిరిగి FAIL;

    CLEAR_క్షేత్రం(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_దొంతర, పరిమాణం(typval_T), 500);
    ఒకవేళ (GA_GROW_FAILS(&ectx.ec_దొంతర, 20))
    {
	funcdepth_decrement();
	తిరిగి FAIL;
    }
    ga_init2(&ectx.ec_trystack, పరిమాణం(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, పరిమాణం(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = మొత్తం_argc - ufunc->uf_వాదనలు.ga_len;
    ఒకవేళ (idx > 0 && ufunc->uf_va_name == ఖాళీ)
    {
	semsg(NGETTEXT(e_one_వాదం_too_many, e_nr_arguments_too_many,
								    idx), idx);
	వెళ్ళు failed_early;
    }
    idx = మొత్తం_argc - ufunc->uf_వాదనలు.ga_len + ufunc->uf_def_వాదనలు.ga_len;
    ఒకవేళ (idx < 0)
    {
	semsg(NGETTEXT(e_one_వాదం_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	వెళ్ళు failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    కొరకు (idx = 0; idx < మొత్తం_argc
	    && (ufunc->uf_va_name != ఖాళీ || idx < ufunc->uf_వాదనలు.ga_len);
									 ++idx)
    {
	పూర్ణాంకం argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	ఒకవేళ (idx >= ufunc->uf_వాదనలు.ga_len - ufunc->uf_def_వాదనలు.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_సంఖ్య == VVAL_NONE)
	{
	    // Use the default value.
	    దొంతర_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	లేకపోతే
	{
	    పూర్ణాంకం పూర్తి = FALSE;
	    ఒకవేళ (ufunc->uf_arg_types != ఖాళీ && idx < ufunc->uf_వాదనలు.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		ఒకవేళ (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_సంఖ్య)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    దొంతర_TV_BOT(0)->v_type = VAR_FLOAT;
		    దొంతర_TV_BOT(0)->v_తాళం = 0;
		    దొంతర_TV_BOT(0)->vval.v_float = tv->vval.v_సంఖ్య;
		    పూర్తి = TRUE;
		}
		లేకపోతే ఒకవేళ (check_typval_arg_type(expected, tv,
						   ఖాళీ, argv_idx + 1) == FAIL)
		    వెళ్ళు failed_early;
	    }
	    ఒకవేళ (!పూర్తి)
		copy_tv(tv, దొంతర_TV_BOT(0));
	}
	++ectx.ec_దొంతర.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    ఒకవేళ (ufunc->uf_va_name != ఖాళీ)
    {
	పూర్ణాంకం vararg_లెక్క = argc - ufunc->uf_వాదనలు.ga_len;

	ఒకవేళ (vararg_లెక్క < 0)
	    vararg_లెక్క = 0;
	లేకపోతే
	    argc -= vararg_లెక్క;
	ఒకవేళ (exe_newlist(vararg_లెక్క, &ectx) == FAIL)
	    వెళ్ళు failed_early;

	// Check the type of the list items.
	tv = దొంతర_TV_BOT(-1);
	ఒకవేళ (ufunc->uf_va_type != ఖాళీ
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != ఖాళీ)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_మొదటి;

	    కొరకు (idx = 0; idx < vararg_లెక్క; ++idx)
	    {
		ఒకవేళ (check_typval_arg_type(expected, &li->li_tv,
						 ఖాళీ, argc + idx + 1) == FAIL)
		    వెళ్ళు failed_early;
		li = li->li_తదుపరి;
	    }
	}

	ఒకవేళ (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *దొంతర_TV_BOT(defcount - 1) = *దొంతర_TV_BOT(-1);
	--ectx.ec_దొంతర.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    ఒకవేళ (defcount > 0)
	కొరకు (idx = 0; idx < defcount; ++idx)
	{
	    దొంతర_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_దొంతర.ga_len;
	}
    ఒకవేళ (ufunc->uf_va_name != ఖాళీ)
	++ectx.ec_దొంతర.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_చట్రం_idx = ectx.ec_దొంతర.ga_len;
    ectx.ec_initial_చట్రం_idx = ectx.ec_చట్రం_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *పునాది_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	ఒకవేళ (partial != ఖాళీ || పునాది_ufunc->uf_partial != ఖాళీ)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    ఒకవేళ (ectx.ec_outer_ref == ఖాళీ)
		వెళ్ళు failed_early;
	    ఒకవేళ (partial != ఖాళీ)
	    {
		outer_T *outer = get_pt_outer(partial);

		ఒకవేళ (outer->బహిర్గతం_దొంతర == ఖాళీ && outer->బహిర్గతం_loop_కొలత == 0)
		{
		    // no stack was set
		    ఒకవేళ (current_ectx != ఖాళీ)
		    {
			ఒకవేళ (current_ectx->ec_outer_ref != ఖాళీ
			       && current_ectx->ec_outer_ref->or_outer != ఖాళీ)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		లేకపోతే
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    లేకపోతే
	    {
		ectx.ec_outer_ref->or_outer = &పునాది_ufunc->uf_partial->pt_outer;
		++పునాది_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = పునాది_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    కొరకు (idx = 0; idx < దొంతర_చట్రం_కొలత; ++idx)
    {
	దొంతర_TV(ectx.ec_దొంతర.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_దొంతర.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	కొరకు (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    దొంతర_TV_VAR(idx)->v_type = VAR_సంఖ్య;
	    దొంతర_TV_VAR(idx)->vval.v_సంఖ్య = 0;
	}
	ectx.ec_దొంతర.ga_len += dfunc->df_varcount;

	ఒకవేళ (object != ఖాళీ)
	{
	    // the object is always the variable at index zero
	    tv = దొంతర_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	ఒకవేళ (dfunc->df_has_సంవృతం)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    దొంతర_TV_VAR(idx)->v_type = VAR_సంఖ్య;
	    దొంతర_TV_VAR(idx)->vval.v_సంఖ్య = 0;
	    ++ectx.ec_దొంతర.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    ఒకవేళ (funccal != ఖాళీ)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_నెట్టు_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_సంస్కరణ = SCRIPT_సంస్కరణ_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_సందేశం_list = సందేశం_list;
    సందేశం_list = &వ్యక్తిగత_సందేశం_list;

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
    ఒకవేళ (ret == OK)
    {
	// function finished, get result from the stack.
	ఒకవేళ (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	లేకపోతే
	{
	    tv = దొంతర_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    ఒకవేళ (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_సంవృతం_లోపల_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_తీయి();
    current_sctx = save_current_sctx;

    ఒకవేళ (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    ఒకవేళ (*సందేశం_list != ఖాళీ && saved_సందేశం_list != ఖాళీ)
    {
	msglist_T **plist = saved_సందేశం_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	అయితే (*plist != ఖాళీ)
	    plist = &(*plist)->తదుపరి;

	*plist = *సందేశం_list;
    }
    సందేశం_list = saved_సందేశం_list;

    ఒకవేళ (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ఖాళీ;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    కొరకు (idx = 0; idx < ectx.ec_దొంతర.ga_len; ++idx)
    {
	tv = దొంతర_TV(idx);
	ఒకవేళ (tv->v_type != VAR_సంఖ్య && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_దొంతర.ga_దత్తాంశం);
    vim_free(ectx.ec_trystack.ga_దత్తాంశం);
    ఒకవేళ (ectx.ec_outer_ref != ఖాళీ)
    {
	ఒకవేళ (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    ఒకవేళ (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    తిరిగి ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    శూన్య
unwind_def_callstack(ectx_T *ectx)
{
    అయితే (ectx->ec_చట్రం_idx != ectx->ec_initial_చట్రం_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    శూన్య
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + ectx->ec_dfunc_idx;

    ఒకవేళ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    స్థిర char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    పూర్ణాంకం		లోతు;

    ga_init2(&ga, 1, 100);
    కొరకు (లోతు = 0; లోతు < lvi->lvi_లోతు; ++లోతు)
    {
	ఒకవేళ (ga_grow(&ga, 50) == FAIL)
	    ఆపు;
	ఒకవేళ (lvi->lvi_loop[లోతు].var_idx == 0)
	    STRCPY((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, " -");
	లేకపోతే
	    vim_snprintf((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[లోతు].var_idx,
			    lvi->lvi_loop[లోతు].var_idx
					 + lvi->lvi_loop[లోతు].var_లెక్క - 1);
	ga.ga_len = (పూర్ణాంకం)STRLEN(ga.ga_దత్తాంశం);
    }
    తిరిగి ga.ga_దత్తాంశం;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    స్థిర శూన్య
list_instructions(అక్షరం *pfx, isn_T *instr, పూర్ణాంకం instr_లెక్క, ufunc_T *ufunc)
{
    పూర్ణాంకం		పంక్తి_idx = 0;
    పూర్ణాంకం		prev_current = 0;
    పూర్ణాంకం		current;
    పూర్ణాంకం		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    కొరకు (current = 0; current < instr_లెక్క; ++current)
    {
	isn_T	    *iptr = &instr[current];
	అక్షరం	    *పంక్తి;

	ఒకవేళ (ufunc != ఖాళీ)
	{
	    అయితే (పంక్తి_idx < iptr->isn_lnum
					  && పంక్తి_idx < ufunc->uf_పంక్తులు.ga_len)
	    {
		ఒకవేళ (current > prev_current)
		{
		    సందేశం_puts("\n\n");
		    prev_current = current;
		}
		పంక్తి = ((అక్షరం **)ufunc->uf_పంక్తులు.ga_దత్తాంశం)[పంక్తి_idx++];
		ఒకవేళ (పంక్తి != ఖాళీ)
		    సందేశం(పంక్తి);
	    }
	    ఒకవేళ (iptr->isn_type == ISN_JUMP_IF_ARG_సమితి)
	    {
		పూర్ణాంకం	మొదటి_def_arg = ufunc->uf_వాదనలు.ga_len
						   - ufunc->uf_def_వాదనలు.ga_len;

		ఒకవేళ (def_arg_idx > 0)
		    సందేశం_puts("\n\n");
		సందేశం_ప్రారంభం();
		సందేశం_puts("  ");
		సందేశం_puts(((అక్షరం **)(ufunc->uf_వాదనలు.ga_దత్తాంశం))[
						 మొదటి_def_arg + def_arg_idx]);
		సందేశం_puts(" = ");
		సందేశం_puts(((అక్షరం **)(ufunc->uf_def_వాదనలు.ga_దత్తాంశం))[def_arg_idx++]);
		సందేశం_clr_eos();
		సందేశం_చివర();
	    }
	}

	ఎంపిక (iptr->isn_type)
	{
	    స్థితి ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (పూర్ణాంకం)iptr->isn_arg.construct.construct_కొలత);
		ఆపు;
	    స్థితి ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_EXEC_విభజించు:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		ఆపు;
	    స్థితి ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.సంఖ్య ? " append" : "");
		ఆపు;
	    స్థితి ISN_CEXPR_AUCMD:
#నిర్వచితమైతే FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.సంఖ్య));
#అయితే_ముగింపు
		ఆపు;
	    స్థితి ISN_CEXPR_CORE:
#నిర్వచితమైతే FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#అయితే_ముగింపు
		ఆపు;
	    స్థితి ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, గరిష్ఠ_పూర్ణాంకం, ఖాళీ);
		సందేశం("     -------------");
		ఆపు;
	    స్థితి ISN_ఆధారం:
		{
		    scriptitem_T    *si = SCRIPT_అంశం(iptr->isn_arg.సంఖ్య);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		ఆపు;
	    స్థితి ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, గరిష్ఠ_పూర్ణాంకం, ఖాళీ);
		    సందేశం("     -------------");
		}
		ఆపు;
	    స్థితి ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.సంఖ్య);
		ఆపు;
	    స్థితి ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_లెక్క);
		}
		ఆపు;
	    స్థితి ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_ECHOWINDOW:
		ఒకవేళ (iptr->isn_arg.echowin.ewin_సమయం > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_లెక్క,
				      iptr->isn_arg.echowin.ewin_సమయం);
		లేకపోతే
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_లెక్క);
		ఆపు;
	    స్థితి ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_ఎక్కించు:
		{
		    ఒకవేళ (iptr->isn_arg.సంఖ్య < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.సంఖ్య
							  + దొంతర_చట్రం_కొలత));
		    లేకపోతే
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.సంఖ్య));
		}
		ఆపు;
	    స్థితి ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ఒకవేళ (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_లోతు,
					outer->outer_idx + దొంతర_చట్రం_కొలత);
		    లేకపోతే ఒకవేళ (outer->outer_లోతు < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_లోతు);
		    లేకపోతే
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_లోతు,
					      outer->outer_idx);
		}
		ఆపు;
	    స్థితి ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_అంశం(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ఒకవేళ (sv == ఖాళీ)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    లేకపోతే
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		ఆపు;
	    స్థితి ISN_LOADS:
	    స్థితి ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_అంశం(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ఆపు;
	    స్థితి ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		ఆపు;
	    స్థితి ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		ఆపు;
	    స్థితి ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		ఆపు;
	    స్థితి ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		ఆపు;
	    స్థితి ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (పూర్ణాంకం)(iptr->isn_arg.సంఖ్య));
		ఆపు;

	    స్థితి ISN_STORE:
		ఒకవేళ (iptr->isn_arg.సంఖ్య < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.సంఖ్య + దొంతర_చట్రం_కొలత);
		లేకపోతే
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.సంఖ్య);
		ఆపు;
	    స్థితి ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ఒకవేళ (outer->outer_లోతు < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_లోతు);
		    లేకపోతే
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_లోతు, outer->outer_idx);
		}
		ఆపు;
	    స్థితి ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_STORES:
	    స్థితి ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_అంశం(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ఆపు;
	    స్థితి ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_అంశం(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ఒకవేళ (sv == ఖాళీ)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    లేకపోతే
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		ఆపు;
	    స్థితి ISN_STOREOPT:
	    స్థితి ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		ఆపు;
	    స్థితి ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (పూర్ణాంకం)iptr->isn_arg.సంఖ్య);
		ఆపు;
	    స్థితి ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		ఆపు;

	    స్థితి ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		ఆపు;

	    స్థితి ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		ఆపు;

	    స్థితి ISN_ఎక్కించు_CLASSMEMBER:
	    స్థితి ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    పూర్ణాంకం	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_ఎక్కించు_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		ఆపు;

	    // constants
	    స్థితి ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_PUSHBOOL:
	    స్థితి ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		ఆపు;
	    స్థితి ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		ఆపు;
	    స్థితి ISN_PUSHFUNC:
		{
		    అక్షరం *name = (అక్షరం *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == ఖాళీ ? "[none]" : name);
		}
		ఆపు;
	    స్థితి ISN_PUSHCHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#అయితే_ముగింపు
		ఆపు;
	    స్థితి ISN_PUSHJOB:
#నిర్వచితమైతే FEAT_పని_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#అయితే_ముగింపు
		ఆపు;
	    స్థితి ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		ఆపు;
	    స్థితి ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == ఖాళీ ? "null"
				 : (అక్షరం *)iptr->isn_arg.classarg->class_name.string);
		ఆపు;
	    స్థితి ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		ఆపు;
	    స్థితి ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ఆపు;
	    స్థితి ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ఆపు;
	    స్థితి ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		ఆపు;
	    స్థితి ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		ఆపు;
	    స్థితి ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		ఆపు;
	    స్థితి ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		ఆపు;
	    స్థితి ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		ఆపు;

	    // function call
	    స్థితి ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    అంతర్గత_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		ఆపు;
	    స్థితి ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_దత్తాంశం)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		ఆపు;
	    స్థితి ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != ఖాళీ
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		ఆపు;
	    స్థితి ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		ఆపు;
	    స్థితి ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_శిఖరం ? " top" : "", cpfunc->cpf_argcount);
		}
		ఆపు;
	    స్థితి ISN_PCALL_చివర:
		smsg("%s%4d PCALL end", pfx, current);
		ఆపు;
	    స్థితి ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (పూర్ణాంకం)iptr->isn_arg.defer.defer_argcount);
		ఆపు;
	    స్థితి ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		ఆపు;
	    స్థితి ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		ఆపు;
	    స్థితి ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		ఆపు;
	    స్థితి ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    ఒకవేళ (extra == ఖాళీ || extra->fre_func_name == ఖాళీ)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_దత్తాంశం)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    లేకపోతే
			name = extra->fre_func_name;
		    ఒకవేళ (extra != ఖాళీ && extra->fre_class != ఖాళీ)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    లేకపోతే ఒకవేళ (extra == ఖాళీ
				     || extra->fre_loopvar_info.lvi_లోతు == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    లేకపోతే
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		ఆపు;

	    స్థితి ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ఒకవేళ (arg->nfa_loopvar_info.lvi_లోతు == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    లేకపోతే
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		ఆపు;

	    స్థితి ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == ఖాళీ ? (char_u *)"" : name);
		}
		ఆపు;

	    స్థితి ISN_JUMP:
		{
		    అక్షరం *when = "?";

		    ఎంపిక (iptr->isn_arg.jump.jump_when)
		    {
			స్థితి JUMP_ALWAYS:
			    when = "JUMP";
			    ఆపు;
			స్థితి JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    ఆపు;
			స్థితి JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    ఆపు;
			స్థితి JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    ఆపు;
			స్థితి JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  //వాడని
			    ఆపు;
			స్థితి JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    ఆపు;
			స్థితి JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    ఆపు;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		ఆపు;

	    స్థితి ISN_JUMP_IF_ARG_సమితి:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + దొంతర_చట్రం_కొలత,
						iptr->isn_arg.jump.jump_where);
		ఆపు;

	    స్థితి ISN_JUMP_IF_ARG_NOT_సమితి:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + దొంతర_చట్రం_కొలత,
						iptr->isn_arg.jump.jump_where);
		ఆపు;

	    స్థితి ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_చివర);
		}
		ఆపు;

	    స్థితి ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->చివర_funcref_idx,
			    endloop->చివర_var_idx,
			    endloop->చివర_var_idx + endloop->చివర_var_లెక్క - 1,
			    endloop->చివర_లోతు);
		}
		ఆపు;

	    స్థితి ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_చివర);
		}
		ఆపు;

	    స్థితి ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    ఒకవేళ (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    లేకపోతే
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		ఆపు;
	    స్థితి ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		ఆపు;
	    స్థితి ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_స్థాయిలు,
				      trycont->tct_స్థాయిలు == 1 ? "" : "s",
				      trycont->tct_where);
		}
		ఆపు;
	    స్థితి ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		ఆపు;
	    స్థితి ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		ఆపు;
	    స్థితి ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		ఆపు;

	    // expression operations on number
	    స్థితి ISN_OPNR:
	    స్థితి ISN_OPFLOAT:
	    స్థితి ISN_OPANY:
		{
		    అక్షరం *what;
		    అక్షరం *ins;

		    ఎంపిక (iptr->isn_arg.op.op_type)
		    {
			స్థితి సమాసం_MULT: what = "*"; ఆపు;
			స్థితి సమాసం_DIV: what = "/"; ఆపు;
			స్థితి సమాసం_REM: what = "%"; ఆపు;
			స్థితి సమాసం_SUB: what = "-"; ఆపు;
			స్థితి సమాసం_కూడు: what = "+"; ఆపు;
			స్థితి సమాసం_LSHIFT: what = "<<"; ఆపు;
			స్థితి సమాసం_RSHIFT: what = ">>"; ఆపు;
			అప్రమేయ:       what = "???"; ఆపు;
		    }
		    ఎంపిక (iptr->isn_type)
		    {
			స్థితి ISN_OPNR: ins = "OPNR"; ఆపు;
			స్థితి ISN_OPFLOAT: ins = "OPFLOAT"; ఆపు;
			స్థితి ISN_OPANY: ins = "OPANY"; ఆపు;
			అప్రమేయ: ins = "???"; ఆపు;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		ఆపు;

	    స్థితి ISN_COMPAREBOOL:
	    స్థితి ISN_COMPARESPECIAL:
	    స్థితి ISN_COMPARENULL:
	    స్థితి ISN_COMPARENR:
	    స్థితి ISN_COMPAREFLOAT:
	    స్థితి ISN_COMPARESTRING:
	    స్థితి ISN_COMPAREBLOB:
	    స్థితి ISN_COMPARELIST:
	    స్థితి ISN_COMPARETUPLE:
	    స్థితి ISN_COMPAREDICT:
	    స్థితి ISN_COMPAREFUNC:
	    స్థితి ISN_COMPAREOBJECT:
	    స్థితి ISN_COMPAREANY:
		   {
		       అక్షరం *p;
		       అక్షరం buf[10];
		       అక్షరం *type;

		       ఎంపిక (iptr->isn_arg.op.op_type)
		       {
			   స్థితి సమాసం_EQUAL:	 p = "=="; ఆపు;
			   స్థితి సమాసం_NEQUAL:    p = "!="; ఆపు;
			   స్థితి సమాసం_GREATER:   p = ">"; ఆపు;
			   స్థితి సమాసం_GEQUAL:    p = ">="; ఆపు;
			   స్థితి సమాసం_SMALLER:   p = "<"; ఆపు;
			   స్థితి సమాసం_SEQUAL:    p = "<="; ఆపు;
			   స్థితి సమాసం_సరిపోలిక:	 p = "=~"; ఆపు;
			   స్థితి సమాసం_IS:	 p = "is"; ఆపు;
			   స్థితి సమాసం_ISNOT:	 p = "isnot"; ఆపు;
			   స్థితి సమాసం_NOMATCH:	 p = "!~"; ఆపు;
			   అప్రమేయ:  p = "???"; ఆపు;
		       }
		       STRCPY(buf, p);
		       ఒకవేళ (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       ఎంపిక(iptr->isn_type)
		       {
			   స్థితి ISN_COMPAREBOOL: type = "COMPAREBOOL"; ఆపు;
			   స్థితి ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; ఆపు;
			   స్థితి ISN_COMPARENULL: type = "COMPARENULL"; ఆపు;
			   స్థితి ISN_COMPARENR: type = "COMPARENR"; ఆపు;
			   స్థితి ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; ఆపు;
			   స్థితి ISN_COMPARESTRING:
						  type = "COMPARESTRING"; ఆపు;
			   స్థితి ISN_COMPAREBLOB: type = "COMPAREBLOB"; ఆపు;
			   స్థితి ISN_COMPARELIST: type = "COMPARELIST"; ఆపు;
			   స్థితి ISN_COMPARETUPLE: type = "COMPARETUPLE"; ఆపు;
			   స్థితి ISN_COMPAREDICT: type = "COMPAREDICT"; ఆపు;
			   స్థితి ISN_COMPAREFUNC: type = "COMPAREFUNC"; ఆపు;
			   స్థితి ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; ఆపు;
			   స్థితి ISN_COMPAREANY: type = "COMPAREANY"; ఆపు;
			   అప్రమేయ: type = "???"; ఆపు;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   ఆపు;

	    స్థితి ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); ఆపు;
	    స్థితి ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); ఆపు;
	    స్థితి ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); ఆపు;

	    // expression operations
	    స్థితి ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.సంఖ్య));
		ఆపు;
	    స్థితి ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); ఆపు;
	    స్థితి ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); ఆపు;
	    స్థితి ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); ఆపు;
	    స్థితి ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); ఆపు;
	    స్థితి ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); ఆపు;
	    స్థితి ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); ఆపు;
	    స్థితి ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); ఆపు;
	    స్థితి ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); ఆపు;
	    స్థితి ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); ఆపు;
	    స్థితి ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); ఆపు;
	    స్థితి ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); ఆపు;
	    స్థితి ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); ఆపు;
	    స్థితి ISN_ముక్క: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.సంఖ్య); ఆపు;
	    స్థితి ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_సూచీ,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); ఆపు;
	    స్థితి ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); ఆపు;
	    స్థితి ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); ఆపు;
	    స్థితి ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (పూర్ణాంకం)iptr->isn_arg.classmember.cm_idx);
				     ఆపు;
	    స్థితి ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (పూర్ణాంకం)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     ఆపు;
	    స్థితి ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (పూర్ణాంకం)iptr->isn_arg.సంఖ్య); ఆపు;
	    స్థితి ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); ఆపు;
	    స్థితి ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); ఆపు;

	    స్థితి ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); ఆపు;

	    స్థితి ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      అక్షరం	    *tofree = ఖాళీ;
		      అక్షరం	    *typename;

		      ఒకవేళ (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_సంఖ్య_OK))
			  typename = "float|number";
		      లేకపోతే ఒకవేళ (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      లేకపోతే
			  typename = type_name(ct->ct_type, &tofree);

		      ఒకవేళ (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (పూర్ణాంకం)ct->ct_off);
		      లేకపోతే
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (పూర్ణాంకం)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (పూర్ణాంకం)ct->ct_arg_idx);
		      vim_free(tofree);
		      ఆపు;
		  }
	    స్థితి ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_మరిన్ని_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       ఆపు;
	    స్థితి ISN_SETTYPE:
		  {
		      అక్షరం *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      ఆపు;
		  }
	    స్థితి ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); ఆపు;
	    స్థితి ISN_2BOOL: ఒకవేళ (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.స్థానభ్రంశం);
			    లేకపోతే
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.స్థానభ్రంశం);
			    ఆపు;
	    స్థితి ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.స్థానభ్రంశం));
			      ఆపు;
	    స్థితి ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.స్థానభ్రంశం));
			      ఆపు;
	    స్థితి ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    ఆపు;
	    స్థితి ISN_PUT:
		ఒకవేళ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		లేకపోతే ఒకవేళ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		లేకపోతే
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (దీర్ఘ)iptr->isn_arg.put.put_lnum);
		ఆపు;
	    స్థితి ISN_IPUT:
		ఒకవేళ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		లేకపోతే ఒకవేళ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		లేకపోతే
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (దీర్ఘ)iptr->isn_arg.put.put_lnum);
		ఆపు;
	    స్థితి ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  ఖాళీ, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    ఒకవేళ (likely(buf != ఖాళీ))
		    {
			(శూన్య)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    ఆపు;
		}
	    స్థితి ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); ఆపు;

	    స్థితి ISN_PROF_ప్రారంభం:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 ఆపు;

	    స్థితి ISN_PROF_చివర:
		smsg("%s%4d PROFILE END", pfx, current);
		ఆపు;

	    స్థితి ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_పేర్లు_len);
		ఆపు;

	    స్థితి ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_లెక్క,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      ఆపు;
	    స్థితి ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_అంశం,
					 iptr->isn_arg.shuffle.shfl_పైకి);
			      ఆపు;
	    స్థితి ISN_DROP: smsg("%s%4d DROP", pfx, current); ఆపు;

	    స్థితి ISN_SCRIPTCTX_సమితి:
		{
		    పూర్ణాంకం	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_అంశం(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		ఆపు;

	    స్థితి ISN_ముగింపు: // End of list of instructions for ISN_SUBSTITUTE.
			   తిరిగి;
	}

	బహిర్గతం_flush();	    // output one line at a time
	ui_breakcheck();
	ఒకవేళ (got_int)
	    ఆపు;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    శూన్య
సమితి_context_లోపల_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_నమూనా = arg;

    // first argument already typed: only user function names
    ఒకవేళ (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_నమూనా = skipwhite(p);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the list of :disassemble
 * arguments.
 */
    char_u *
get_disassemble_వాదం(expand_T *xp, పూర్ణాంకం idx)
{
    ఒకవేళ (idx == 0)
	తిరిగి (char_u *)"debug";
    ఒకవేళ (idx == 1)
	తిరిగి (char_u *)"profile";
    తిరిగి get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    శూన్య
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = ఖాళీ;  // init to shut up gcc warning
    పూర్ణాంకం		instr_లెక్క = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = కనుగొను_func_by_name(arg, &compile_type);
    ఒకవేళ (ufunc == ఖాళీ)
	తిరిగి;
    ఒకవేళ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, ఖాళీ) == FAIL)
	తిరిగి;
    ఒకవేళ (ufunc->uf_def_స్థితి != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	తిరిగి;
    }
    సందేశం((అక్షరం *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_దత్తాంశం) + ufunc->uf_dfunc_idx;
    ఎంపిక (compile_type)
    {
	స్థితి CT_PROFILE:
#నిర్వచితమైతే FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_లెక్క = dfunc->df_instr_prof_లెక్క;
	    ఆపు;
#అయితే_ముగింపు
	    // FALLTHROUGH
	స్థితి CT_NONE:
	    instr = dfunc->df_instr;
	    instr_లెక్క = dfunc->df_instr_లెక్క;
	    ఆపు;
	స్థితి CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_లెక్క = dfunc->df_instr_debug_లెక్క;
	    ఆపు;
    }

    list_instructions("", instr, instr_లెక్క, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    పూర్ణాంకం
tv2bool(typval_T *tv)
{
    ఎంపిక (tv->v_type)
    {
	స్థితి VAR_సంఖ్య:
	    తిరిగి tv->vval.v_సంఖ్య != 0;
	స్థితి VAR_FLOAT:
	    తిరిగి tv->vval.v_float != 0.0;
	స్థితి VAR_PARTIAL:
	    తిరిగి tv->vval.v_partial != ఖాళీ;
	స్థితి VAR_FUNC:
	స్థితి VAR_STRING:
	    తిరిగి tv->vval.v_string != ఖాళీ && *tv->vval.v_string != NUL;
	స్థితి VAR_LIST:
	    తిరిగి tv->vval.v_list != ఖాళీ && tv->vval.v_list->lv_len > 0;
	స్థితి VAR_TUPLE:
	    తిరిగి tuple_len(tv->vval.v_tuple) > 0;
	స్థితి VAR_DICT:
	    తిరిగి tv->vval.v_dict != ఖాళీ
				    && tv->vval.v_dict->dv_hashtab.ht_వాడిన > 0;
	స్థితి VAR_BOOL:
	స్థితి VAR_SPECIAL:
	    తిరిగి tv->vval.v_సంఖ్య == VVAL_TRUE ? TRUE : FALSE;
	స్థితి VAR_పని:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    తిరిగి tv->vval.v_పని != ఖాళీ;
#లేకపోతే
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_CHANNEL:
#నిర్వచితమైతే FEAT_పని_CHANNEL
	    తిరిగి tv->vval.v_channel != ఖాళీ;
#లేకపోతే
	    ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_BLOB:
	    తిరిగి tv->vval.v_blob != ఖాళీ && tv->vval.v_blob->bv_ga.ga_len > 0;

	స్థితి VAR_OBJECT:
	    తిరిగి tv->vval.v_object != ఖాళీ;

	స్థితి VAR_CLASS:
	స్థితి VAR_TYPEALIAS:
	    check_typval_is_విలువ(tv);
	    ఆపు;

	స్థితి VAR_UNKNOWN:
	స్థితి VAR_ANY:
	స్థితి VAR_VOID:
	స్థితి VAR_INSTR:
	    ఆపు;
    }
    తిరిగి FALSE;
}

    శూన్య
emsg_using_string_as(typval_T *tv, పూర్ణాంకం as_సంఖ్య)
{
    semsg(_(as_సంఖ్య ? e_using_string_as_సంఖ్య_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == ఖాళీ
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    పూర్ణాంకం
check_not_string(typval_T *tv)
{
    ఒకవేళ (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

#అయితే_ముగింపు // FEAT_EVAL
