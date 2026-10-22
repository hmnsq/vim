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
#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ	    tcd_frame_idx;	// ec_frame_idx at ISN_TRY
    പൂർണ്ണസംഖ്യ	    tcd_അട്ടി_len;	// size of ectx.ec_stack at ISN_TRY
    പൂർണ്ണസംഖ്യ	    tcd_in_catch;	// in catch or finally block
    പൂർണ്ണസംഖ്യ	    tcd_did_throw;	// set did_throw in :endtry
    പൂർണ്ണസംഖ്യ	    tcd_catch_idx;	// instruction of the first :catch or :finally
    പൂർണ്ണസംഖ്യ	    tcd_finally_idx;	// instruction of the :finally block or zero
    പൂർണ്ണസംഖ്യ	    tcd_endtry_idx;	// instruction of the :endtry
    പൂർണ്ണസംഖ്യ	    tcd_caught;		// catch block entered
    പൂർണ്ണസംഖ്യ	    tcd_cont;		// :continue encountered, jump here (minus one)
    പൂർണ്ണസംഖ്യ	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
തരനിർവചനം ഘടന {
    പൂർണ്ണസംഖ്യ		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    പൂർണ്ണസംഖ്യ		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
തരനിർവചനം ഘടന {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    പൂർണ്ണസംഖ്യ		or_outer_allocated;  // free "or_outer" later
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
ഘടന ectx_S {
    garray_T	ec_അട്ടി;	// stack of typval_T values
    പൂർണ്ണസംഖ്യ		ec_frame_idx;	// index in ec_stack: context of ec_dfunc_idx
    പൂർണ്ണസംഖ്യ		ec_initial_frame_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    പൂർണ്ണസംഖ്യ		ec_dfunc_idx;	// current function index
    പൂർണ്ണസംഖ്യ		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    പൂർണ്ണസംഖ്യ		ec_did_emsg_before;
    പൂർണ്ണസംഖ്യ		ec_trylevel_at_start;
    where_T	ec_where;
};

#ifdef FEAT_PROFILE
// stack of profinfo_T used when profiling.
സ്ഥിതം garray_T profile_info_ga = {0, 0, വലുപ്പം(profinfo_T), 20, ശൂന്യ};
#endif

// Get pointer to item in the stack.
#define അട്ടി_TV(idx) (((typval_T *)ectx->ec_അട്ടി.ga_data) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#define അട്ടി_TV_BOT(idx) (((typval_T *)ectx->ec_അട്ടി.ga_data) + ectx->ec_അട്ടി.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#define അട്ടി_TV_VAR(idx) (((typval_T *)ectx->ec_അട്ടി.ga_data) + ectx->ec_frame_idx + അട്ടി_FRAME_SIZE + idx)

// Return value for functions used to execute instructions
#define EXEC_FAIL	0
#define EXEC_OK		1
#define EXEC_DONE	2

    ശൂന്യം
to_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_to_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
ufunc_argcount(ufunc_T *ufunc)
{
    തിരികെ ufunc->uf_args.ga_len + (ufunc->uf_va_name != ശൂന്യ ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exe_concat(പൂർണ്ണസംഖ്യ എണ്ണം, ectx_T *ectx)
{
    പൂർണ്ണസംഖ്യ		idx;
    size_t	len = 0;
    garray_T	ga;
    തരനിർവചനം ഘടന
    {
	typval_T    *tv;
	size_t	    length;
    } string_segment_T;
    എണ്ണൽ
    {
	STRING_SEGMENT_CACHE_SIZE = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_SIZE];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    എങ്കിൽ (എണ്ണം > (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, എണ്ണം);
	എങ്കിൽ (string_segment_tab == ശൂന്യ)
	    തിരികെ FAIL;
    }

    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    വേണ്ടി (idx = 0; idx < എണ്ണം; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = അട്ടി_TV_BOT(idx - എണ്ണം);
	എങ്കിൽ (segment->tv->vval.v_string != ശൂന്യ)
	{
	    segment->length = STRLEN(segment->tv->vval.v_string);
	    len += segment->length;
	}
	അല്ലെങ്കിൽ
	    segment->length = 0;    // Ensure clean state for the second pass
    }

    എങ്കിൽ (ga_grow(&ga, (പൂർണ്ണസംഖ്യ)len + 1) == FAIL)
    {
	എങ്കിൽ (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	തിരികെ FAIL;
    }

    വേണ്ടി (idx = 0; idx < എണ്ണം; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->length);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_അട്ടി.ga_len -= എണ്ണം - 1;
    അട്ടി_TV_BOT(-1)->vval.v_string = ga.ga_data;

    എങ്കിൽ (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    തിരികെ OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exe_newlist(പൂർണ്ണസംഖ്യ എണ്ണം, ectx_T *ectx)
{
    list_T	*list = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		idx;
    typval_T	*tv;

    എങ്കിൽ (എണ്ണം >= 0)
    {
	list = list_alloc_with_ഇനങ്ങൾ(എണ്ണം);
	എങ്കിൽ (list == ശൂന്യ)
	    തിരികെ FAIL;
	വേണ്ടി (idx = 0; idx < എണ്ണം; ++idx)
	    list_set_item(list, idx, അട്ടി_TV_BOT(idx - എണ്ണം));
    }

    എങ്കിൽ (എണ്ണം > 0)
	ectx->ec_അട്ടി.ga_len -= എണ്ണം - 1;
    അല്ലെങ്കിൽ എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
    {
	list_unref(list);
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
	++ectx->ec_അട്ടി.ga_len;
    tv = അട്ടി_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_lock = 0;
    എങ്കിൽ (list != ശൂന്യ)
	++list->lv_refcount;
    തിരികെ OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exe_newtuple(പൂർണ്ണസംഖ്യ എണ്ണം, ectx_T *ectx)
{
    tuple_T	*tuple = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		idx;
    typval_T	*tv;

    എങ്കിൽ (എണ്ണം >= 0)
    {
	tuple = tuple_alloc_with_ഇനങ്ങൾ(എണ്ണം);
	എങ്കിൽ (tuple == ശൂന്യ)
	    തിരികെ FAIL;
	വേണ്ടി (idx = 0; idx < എണ്ണം; ++idx)
	    tuple_set_item(tuple, idx, അട്ടി_TV_BOT(idx - എണ്ണം));
    }

    എങ്കിൽ (എണ്ണം > 0)
	ectx->ec_അട്ടി.ga_len -= എണ്ണം - 1;
    അല്ലെങ്കിൽ എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
    {
	tuple_unref(tuple);
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
	++ectx->ec_അട്ടി.ga_len;
    tv = അട്ടി_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_lock = 0;
    എങ്കിൽ (tuple != ശൂന്യ)
	++tuple->tv_refcount;

    തിരികെ OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exe_newdict(പൂർണ്ണസംഖ്യ എണ്ണം, ectx_T *ectx)
{
    dict_T	*dict = ശൂന്യ;
    dictitem_T	*item;
    char_u	*താക്കോൽ;
    പൂർണ്ണസംഖ്യ		idx;
    typval_T	*tv;

    എങ്കിൽ (എണ്ണം >= 0)
    {
	dict = dict_alloc();
	എങ്കിൽ (unlikely(dict == ശൂന്യ))
	    തിരികെ FAIL;
	വേണ്ടി (idx = 0; idx < എണ്ണം; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = അട്ടി_TV_BOT(2 * (idx - എണ്ണം));
	    // check key is unique
	    താക്കോൽ = tv->vval.v_string == ശൂന്യ ? (char_u *)"" : tv->vval.v_string;
	    item = dict_find(dict, താക്കോൽ, -1);
	    എങ്കിൽ (item != ശൂന്യ)
	    {
		semsg(_(e_duplicate_താക്കോൽ_in_dictionary_str), താക്കോൽ);
		dict_unref(dict);
		തിരികെ MAYBE;
	    }
	    item = dictitem_alloc(താക്കോൽ);
	    clear_tv(tv);
	    എങ്കിൽ (unlikely(item == ശൂന്യ))
	    {
		dict_unref(dict);
		തിരികെ FAIL;
	    }
	    tv = അട്ടി_TV_BOT(2 * (idx - എണ്ണം) + 1);
	    item->di_tv = *tv;
	    item->di_tv.v_lock = 0;
	    tv->v_type = VAR_UNKNOWN;
	    എങ്കിൽ (dict_add(dict, item) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(item);
		dict_unref(dict);
		തിരികെ FAIL;
	    }
	}
    }

    എങ്കിൽ (എണ്ണം > 0)
	ectx->ec_അട്ടി.ga_len -= 2 * എണ്ണം - 1;
    അല്ലെങ്കിൽ എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
    {
	dict_unref(dict);
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
	++ectx->ec_അട്ടി.ga_len;
    tv = അട്ടി_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_lock = 0;
    tv->vval.v_dict = dict;
    എങ്കിൽ (dict != ശൂന്യ)
	++dict->dv_refcount;
    തിരികെ OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    ശൂന്യം
update_has_breakpoint(ufunc_T *ufunc)
{
    എങ്കിൽ (ufunc->uf_debug_tick == debug_tick)
	തിരികെ;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

സ്ഥിതം garray_T dict_അട്ടി = GA_EMPTY;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
dict_അട്ടി_save(typval_T *tv)
{
    എങ്കിൽ (dict_അട്ടി.ga_growsize == 0)
	ga_init2(&dict_അട്ടി, വലുപ്പം(typval_T), 10);
    എങ്കിൽ (ga_grow(&dict_അട്ടി, 1) == FAIL)
	തിരികെ FAIL;
    ((typval_T *)dict_അട്ടി.ga_data)[dict_അട്ടി.ga_len] = *tv;
    ++dict_അട്ടി.ga_len;
    തിരികെ OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    സ്ഥിതം typval_T *
dict_അട്ടി_get_tv(ശൂന്യം)
{
    എങ്കിൽ (dict_അട്ടി.ga_len == 0)
	തിരികെ ശൂന്യ;
    തിരികെ ((typval_T *)dict_അട്ടി.ga_data) + dict_അട്ടി.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    സ്ഥിതം dict_T *
dict_അട്ടി_get_dict(ശൂന്യം)
{
    typval_T *tv;

    എങ്കിൽ (dict_അട്ടി.ga_len == 0)
	തിരികെ ശൂന്യ;
    tv = ((typval_T *)dict_അട്ടി.ga_data) + dict_അട്ടി.ga_len - 1;
    എങ്കിൽ (tv->v_type == VAR_DICT)
	തിരികെ tv->vval.v_dict;
    തിരികെ ശൂന്യ;
}

/*
 * Drop an item from the dict stack.
 */
    സ്ഥിതം ശൂന്യം
dict_അട്ടി_drop(ശൂന്യം)
{
    എങ്കിൽ (dict_അട്ടി.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	തിരികെ;
    }
    --dict_അട്ടി.ga_len;
    clear_tv(((typval_T *)dict_അട്ടി.ga_data) + dict_അട്ടി.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    സ്ഥിതം ശൂന്യം
dict_അട്ടി_clear(പൂർണ്ണസംഖ്യ len)
{
    ആകുമ്പോൾ (dict_അട്ടി.ga_len > len)
	dict_അട്ടി_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    സ്ഥിതം outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    എങ്കിൽ (ptref == ശൂന്യ)
	തിരികെ &pt->pt_outer;

    // partial using partial (recursively)
    ആകുമ്പോൾ (ptref->pt_outer_partial != ശൂന്യ)
	ptref = ptref->pt_outer_partial;
    തിരികെ &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_ufunc_arg_types(ufunc_T *ufunc, പൂർണ്ണസംഖ്യ argcount, പൂർണ്ണസംഖ്യ off, ectx_T *ectx)
{
    എങ്കിൽ (ufunc->uf_arg_types == ശൂന്യ && ufunc->uf_va_type == ശൂന്യ)
	തിരികെ OK;

    typval_T	*argv = അട്ടി_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < argcount; ++i)
    {
	type_T *type = ശൂന്യ;

	// assume a v:none argument, using the default value, is always OK
	എങ്കിൽ (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_സംഖ്യ == VVAL_NONE)
	    തുടരുക;
	// only pass values to user functions, never types
	എങ്കിൽ (check_typval_is_മൂല്യം(&argv[i]) == FAIL)
	    തിരികെ FAIL;

	എങ്കിൽ (i < ufunc->uf_args.ga_len && ufunc->uf_arg_types != ശൂന്യ)
	    type = ufunc->uf_arg_types[i];
	അല്ലെങ്കിൽ എങ്കിൽ (ufunc->uf_va_type != ശൂന്യ)
	    type = ufunc->uf_va_type->tt_member;
	എങ്കിൽ (type != ശൂന്യ && check_typval_arg_type(type,
		    &argv[i], ശൂന്യ, i + 1) == FAIL)
	    തിരികെ FAIL;
    }
    തിരികെ OK;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
call_dfunc(
	പൂർണ്ണസംഖ്യ		cdf_idx,
	partial_T	*pt_arg,
	പൂർണ്ണസംഖ്യ		argcount_arg,
	ectx_T		*ectx)
{
    പൂർണ്ണസംഖ്യ		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg_cumul + did_emsg;
    പൂർണ്ണസംഖ്യ		arg_to_add;
    പൂർണ്ണസംഖ്യ		vararg_എണ്ണം = 0;
    പൂർണ്ണസംഖ്യ		varcount;
    പൂർണ്ണസംഖ്യ		idx;
    estack_T	*entry;
    funclocal_T	*floc = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		res = OK;
    compiletype_T compile_type;

    എങ്കിൽ (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == ശൂന്യ ? (char_u *)"unknown" : dfunc->df_name);
	തിരികെ FAIL;
    }

#ifdef FEAT_PROFILE
    എങ്കിൽ (do_profiling == PROF_YES)
    {
	എങ്കിൽ (GA_GROW_OK(&profile_info_ga, 1))
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
    എങ്കിൽ (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, ശൂന്യ);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    }
    എങ്കിൽ (res == FAIL || INSTRUCTIONS(dfunc) == ശൂന്യ)
    {
	എങ്കിൽ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	തിരികെ FAIL;
    }

    എങ്കിൽ (ufunc->uf_va_name != ശൂന്യ)
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
	vararg_എണ്ണം = argcount - ufunc->uf_args.ga_len;
	എങ്കിൽ (vararg_എണ്ണം < 0)
	    vararg_എണ്ണം = 0;
	അല്ലെങ്കിൽ
	    argcount -= vararg_എണ്ണം;
	എങ്കിൽ (exe_newlist(vararg_എണ്ണം, ectx) == FAIL)
	    തിരികെ FAIL;

	vararg_എണ്ണം = 1;
    }

    arg_to_add = ufunc->uf_args.ga_len - argcount;
    എങ്കിൽ (arg_to_add < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
			-arg_to_add), -arg_to_add);
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (arg_to_add > ufunc->uf_def_args.ga_len)
    {
	പൂർണ്ണസംഖ്യ missing = arg_to_add - ufunc->uf_def_args.ga_len;

	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
			missing), missing);
	തിരികെ FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = അട്ടി_TV_BOT(0) - argcount - vararg_എണ്ണം - 1;

    എങ്കിൽ (IS_OBJECT_METHOD(ufunc) && !IS_CONSTRUCTOR_METHOD(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == ശൂന്യ)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	തിരികെ FAIL;
    }

    // Check the argument types.
    എങ്കിൽ (check_ufunc_arg_types(ufunc, argcount, vararg_എണ്ണം, ectx) == FAIL)
	തിരികെ FAIL;

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
    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി,
				     arg_to_add + അട്ടി_FRAME_SIZE + varcount))
	തിരികെ FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = അട്ടി_TV_BOT(0) - argcount - vararg_എണ്ണം - 1;

    // If depth of calling is getting too high, don't execute the function.
    എങ്കിൽ (funcdepth_increment() == FAIL)
	തിരികെ FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    എങ്കിൽ (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	എങ്കിൽ (floc == ശൂന്യ)
	    തിരികെ FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    എങ്കിൽ (vararg_എണ്ണം > 0 && arg_to_add > 0)
	*അട്ടി_TV_BOT(arg_to_add - 1) = *അട്ടി_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    വേണ്ടി (idx = 0; idx < arg_to_add; ++idx)
	അട്ടി_TV_BOT(idx - vararg_എണ്ണം)->v_type = VAR_UNKNOWN;
    ectx->ec_അട്ടി.ga_len += arg_to_add;

    // Store current execution state in stack frame for ISN_RETURN.
    അട്ടി_TV_BOT(അട്ടി_FRAME_FUNC_OFF)->vval.v_സംഖ്യ = ectx->ec_dfunc_idx;
    അട്ടി_TV_BOT(അട്ടി_FRAME_IIDX_OFF)->vval.v_സംഖ്യ = ectx->ec_iidx;
    അട്ടി_TV_BOT(അട്ടി_FRAME_INSTR_OFF)->vval.v_string = (ശൂന്യം *)ectx->ec_instr;
    അട്ടി_TV_BOT(അട്ടി_FRAME_OUTER_OFF)->vval.v_string =
						    (ശൂന്യം *)ectx->ec_outer_ref;
    അട്ടി_TV_BOT(അട്ടി_FRAME_FUNCLOCAL_OFF)->vval.v_string = (ശൂന്യം *)floc;
    അട്ടി_TV_BOT(അട്ടി_FRAME_IDX_OFF)->vval.v_സംഖ്യ = ectx->ec_frame_idx;
    ectx->ec_frame_idx = ectx->ec_അട്ടി.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    പൂർണ്ണസംഖ്യ initcount = dfunc->df_varcount + (dfunc->df_has_closure ? 1 : 0);
    വേണ്ടി (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = അട്ടി_TV_BOT(അട്ടി_FRAME_SIZE + idx);

	tv->v_type = VAR_സംഖ്യ;
	tv->vval.v_സംഖ്യ = 0;
    }
    ectx->ec_അട്ടി.ga_len += അട്ടി_FRAME_SIZE + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    എങ്കിൽ (IS_OBJECT_METHOD(ufunc))
    {
	എങ്കിൽ (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_internal_error_str), "type in stack is not an object");
	    തിരികെ FAIL;
	}

	*അട്ടി_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != ശൂന്യ ? pt_arg : ufunc->uf_partial;
    എങ്കിൽ (pt != ശൂന്യ || (ufunc->uf_flags & FC_CLOSURE))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	എങ്കിൽ (ref == ശൂന്യ)
	    തിരികെ FAIL;
	എങ്കിൽ (pt != ശൂന്യ)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	അല്ലെങ്കിൽ
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    എങ്കിൽ (unlikely(ref->or_outer == ശൂന്യ))
	    {
		vim_free(ref);
		തിരികെ FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->out_അട്ടി = &ectx->ec_അട്ടി;
	    ref->or_outer->out_frame_idx = ectx->ec_frame_idx;
	    എങ്കിൽ (ectx->ec_outer_ref != ശൂന്യ)
		ref->or_outer->out_up = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    അല്ലെങ്കിൽ
	ectx->ec_outer_ref = ശൂന്യ;

    എങ്കിൽ (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    entry = estack_തള്ളുക_ufunc(ufunc, 1);
    എങ്കിൽ (entry != ശൂന്യ)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	entry->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    തിരികെ OK;
}

// Double linked list of funcstack_T in use.
സ്ഥിതം funcstack_T *first_funcstack = ശൂന്യ;

    സ്ഥിതം ശൂന്യം
add_funcstack_to_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    എങ്കിൽ (first_funcstack != ശൂന്യ)
	first_funcstack->fs_prev = funcstack;
    funcstack->fs_next = first_funcstack;
    funcstack->fs_prev = ശൂന്യ;
    first_funcstack = funcstack;
}

    സ്ഥിതം ശൂന്യം
remove_funcstack_from_list(funcstack_T *funcstack)
{
    എങ്കിൽ (funcstack->fs_prev == ശൂന്യ)
	first_funcstack = funcstack->fs_next;
    അല്ലെങ്കിൽ
	funcstack->fs_prev->fs_next = funcstack->fs_next;
    എങ്കിൽ (funcstack->fs_next != ശൂന്യ)
	funcstack->fs_next->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
handle_closure_in_use(ectx_T *ectx, പൂർണ്ണസംഖ്യ free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    പൂർണ്ണസംഖ്യ		argcount;
    പൂർണ്ണസംഖ്യ		മുകൾ;
    പൂർണ്ണസംഖ്യ		idx;
    typval_T	*tv;
    പൂർണ്ണസംഖ്യ		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_എണ്ണം;

    എങ്കിൽ (dfunc->df_ufunc == ശൂന്യ)
	തിരികെ OK;  // function was freed
    എങ്കിൽ (dfunc->df_has_closure == 0)
	തിരികെ OK;  // no closures
    tv = അട്ടി_TV(ectx->ec_frame_idx + അട്ടി_FRAME_SIZE + dfunc->df_varcount);
    closure_എണ്ണം = tv->vval.v_സംഖ്യ;
    എങ്കിൽ (closure_എണ്ണം == 0)
	തിരികെ OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    മുകൾ = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    വേണ്ടി (idx = 0; idx < closure_എണ്ണം; ++idx)
    {
	partial_T   *pt;
	പൂർണ്ണസംഖ്യ	    off = gap->ga_len - closure_എണ്ണം + idx;

	എങ്കിൽ (off < 0)
	    തുടരുക;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	എങ്കിൽ (pt->pt_refcount > 1)
	{
	    പൂർണ്ണസംഖ്യ refcount = pt->pt_refcount;
	    പൂർണ്ണസംഖ്യ i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    വേണ്ടി (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = അട്ടി_TV(ectx->ec_frame_idx
						       + അട്ടി_FRAME_SIZE + i);
		എങ്കിൽ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    എങ്കിൽ (refcount > 1)
	    {
		closure_in_use = TRUE;
		നിർത്തുക;
	    }
	}
    }

    എങ്കിൽ (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *അട്ടി;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	എങ്കിൽ (funcstack == ശൂന്യ)
	    തിരികെ FAIL;

	funcstack->fs_var_offset = argcount + അട്ടി_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset
							  + dfunc->df_varcount;
	അട്ടി = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = അട്ടി;
	എങ്കിൽ (അട്ടി == ശൂന്യ)
	{
	    vim_free(funcstack);
	    തിരികെ FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	വേണ്ടി (idx = 0; idx < argcount; ++idx)
	{
	    tv = അട്ടി_TV(മുകൾ + idx);
	    എങ്കിൽ (free_arguments)
	    {
		*(അട്ടി + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, അട്ടി + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	വേണ്ടി (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = അട്ടി_TV(ectx->ec_frame_idx + അട്ടി_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    എങ്കിൽ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ	    i;

		വേണ്ടി (i = 0; i < closure_എണ്ണം; ++i)
		    എങ്കിൽ (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_എണ്ണം + i])
			++funcstack->fs_min_refcount;
	    }

	    *(അട്ടി + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	വേണ്ടി (idx = 0; idx < closure_എണ്ണം; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_എണ്ണം + idx];
	    എങ്കിൽ (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_അട്ടി = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - മുകൾ;
	    }
	}
    }

    വേണ്ടി (idx = 0; idx < closure_എണ്ണം; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_എണ്ണം + idx]);
    gap->ga_len -= closure_എണ്ണം;
    എങ്കിൽ (gap->ga_len == 0)
	ga_clear(gap);

    തിരികെ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    പൂർണ്ണസംഖ്യ
funcstack_check_refcount(funcstack_T *funcstack)
{
    പൂർണ്ണസംഖ്യ		i;
    garray_T	*gap = &funcstack->fs_ga;
    പൂർണ്ണസംഖ്യ		done = 0;
    typval_T	*അട്ടി;

    എങ്കിൽ (funcstack->fs_refcount > funcstack->fs_min_refcount)
	തിരികെ FALSE;
    വേണ്ടി (i = funcstack->fs_var_offset; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_data) + i;

	എങ്കിൽ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ശൂന്യ
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++done;
    }
    എങ്കിൽ (done != funcstack->fs_min_refcount)
	തിരികെ FALSE;

    അട്ടി = gap->ga_data;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	clear_tv(അട്ടി + i);
    vim_free(അട്ടി);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    തിരികെ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_funcstacks(പൂർണ്ണസംഖ്യ copyID)
{
    funcstack_T *funcstack;

    വേണ്ടി (funcstack = first_funcstack; funcstack != ശൂന്യ;
						funcstack = funcstack->fs_next)
    {
	typval_T    *അട്ടി = funcstack->fs_ga.ga_data;
	പൂർണ്ണസംഖ്യ	    i;

	വേണ്ടി (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    എങ്കിൽ (set_ref_in_item(അട്ടി + i, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ))
		തിരികെ TRUE;  // abort
    }
    തിരികെ FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
സ്ഥിതം ectx_T *current_ectx = ശൂന്യ;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    പൂർണ്ണസംഖ്യ
in_def_function(ശൂന്യം)
{
    തിരികെ current_ectx != ശൂന്യ;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    പൂർണ്ണസംഖ്യ
fill_exec_lval_വേര്(lval_വേര്_T *വേര്)
{
    ectx_T *ectx = current_ectx;
    എങ്കിൽ (ectx != ശൂന്യ)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	എങ്കിൽ (ufunc->uf_class != ശൂന്യ)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    എങ്കിൽ (tv != ശൂന്യ)
	    {
		CLEAR_POINTER(വേര്);
		വേര്->lr_tv = tv;
		copy_tv(അട്ടി_TV_VAR(0), വേര്->lr_tv);
		വേര്->lr_cl_exec = ufunc->uf_class;
		++വേര്->lr_cl_exec->class_refcount;
		തിരികെ OK;
	    }
	}
    }

    തിരികെ FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(ശൂന്യം)
{
    ectx_T *r = current_ectx;

    current_ectx = ശൂന്യ;
    തിരികെ r;
}

    ശൂന്യം
restore_current_ectx(ectx_T *ectx)
{
    എങ്കിൽ (current_ectx != ശൂന്യ)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    സ്ഥിതം list_T *
add_defer_item(പൂർണ്ണസംഖ്യ var_idx, പൂർണ്ണസംഖ്യ argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = അട്ടി_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    എങ്കിൽ (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	എങ്കിൽ (rettv_list_alloc(defer_tv) == FAIL)
	    തിരികെ ശൂന്യ;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_ഇനങ്ങൾ(argcount + 1);
    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ ശൂന്യ;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_lock = 0;
    എങ്കിൽ (list_തിരുകുക_tv(defer_l, &listval, defer_l->lv_first) == FAIL)
    {
	list_free(l);
	തിരികെ ശൂന്യ;
    }

    തിരികെ l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
defer_command(പൂർണ്ണസംഖ്യ var_idx, പൂർണ്ണസംഖ്യ argcount, ectx_T *ectx)
{
    list_T	*l = add_defer_item(var_idx, argcount, ectx);
    പൂർണ്ണസംഖ്യ		i;
    typval_T	*func_tv;

    എങ്കിൽ (l == ശൂന്യ)
	തിരികെ FAIL;

    func_tv = അട്ടി_TV_BOT(-argcount - 1);
    എങ്കിൽ (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	തിരികെ FAIL;
    }
    list_set_item(l, 0, func_tv);

    വേണ്ടി (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, അട്ടി_TV_BOT(-argcount + i));
    ectx->ec_അട്ടി.ga_len -= argcount + 1;
    തിരികെ OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    പൂർണ്ണസംഖ്യ
add_defer_function(char_u *name, പൂർണ്ണസംഖ്യ argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	തിരികെ FAIL;
    }

    l = add_defer_item(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    എങ്കിൽ (l == ശൂന്യ)
    {
	vim_free(name);
	തിരികെ FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_lock = 0;
    func_tv.vval.v_string = name;
    list_set_item(l, 0, &func_tv);

    വേണ്ടി (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, argvars + i);
    തിരികെ OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    സ്ഥിതം ശൂന്യം
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = അട്ടി_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    എങ്കിൽ (defer_tv->v_type != VAR_LIST)
	തിരികെ;	 // no function added
    FOR_ALL_LIST_ഇനങ്ങൾ(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ARGS];
	പൂർണ്ണസംഖ്യ	    i;
	listitem_T  *arg_li = l->lv_first;
	typval_T    *functv = &l->lv_first->li_tv;
	പൂർണ്ണസംഖ്യ	    argcount = l->lv_len - 1;

	എങ്കിൽ (functv->vval.v_string == ശൂന്യ)
	    // already being called, can happen if function does ":qa"
	    തുടരുക;

	വേണ്ടി (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_next;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = ശൂന്യ;
	partial_T	*pt = ശൂന്യ;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	എങ്കിൽ (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = ശൂന്യ;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    എങ്കിൽ (funcexe.fe_object != ശൂന്യ)
		++funcexe.fe_object->obj_refcount;
	}
	അല്ലെങ്കിൽ
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = ശൂന്യ;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(ശൂന്യം)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	എങ്കിൽ (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	അല്ലെങ്കിൽ
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
func_return(ectx_T *ectx)
{
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    പൂർണ്ണസംഖ്യ		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*entry;
    പൂർണ്ണസംഖ്യ		prev_dfunc_idx = അട്ടി_TV(ectx->ec_frame_idx
					+ അട്ടി_FRAME_FUNC_OFF)->vval.v_സംഖ്യ;
    funclocal_T	*floc;
#ifdef FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_data)
							      + prev_dfunc_idx;

    എങ്കിൽ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	എങ്കിൽ (dfunc->df_ufunc->uf_profiling
				   || (caller != ശൂന്യ && caller->uf_profiling))
	{
	    profile_may_end_func(((profinfo_T *)profile_info_ga.ga_data)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#endif

    എങ്കിൽ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    എങ്കിൽ (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    entry = estack_എടുക്കുക();
    എങ്കിൽ (entry != ശൂന്യ)
	current_sctx = entry->es_save_sctx;

    എങ്കിൽ (handle_closure_in_use(ectx, TRUE) == FAIL)
	തിരികെ FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    പൂർണ്ണസംഖ്യ മുകൾ = ectx->ec_frame_idx - argcount;
    എങ്കിൽ (IS_OBJECT_METHOD(dfunc->df_ufunc))
	--മുകൾ;
    വേണ്ടി (idx = മുകൾ; idx < ectx->ec_frame_idx; ++idx)
	clear_tv(അട്ടി_TV(idx));

    // Clear local variables and temp values, but not the return value.
    വേണ്ടി (idx = ectx->ec_frame_idx + അട്ടി_FRAME_SIZE;
					idx < ectx->ec_അട്ടി.ga_len - 1; ++idx)
	clear_tv(അട്ടി_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_അട്ടി.ga_len - 1;
    എങ്കിൽ (ret_idx == ectx->ec_frame_idx + അട്ടി_FRAME_IDX_OFF)
	ret_idx = 0;

    എങ്കിൽ (ectx->ec_outer_ref != ശൂന്യ)
    {
	എങ്കിൽ (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = അട്ടി_TV(ectx->ec_frame_idx
					+ അട്ടി_FRAME_IIDX_OFF)->vval.v_സംഖ്യ;
    ectx->ec_instr = (ശൂന്യം *)അട്ടി_TV(ectx->ec_frame_idx
				       + അട്ടി_FRAME_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (ശൂന്യം *)അട്ടി_TV(ectx->ec_frame_idx
				       + അട്ടി_FRAME_OUTER_OFF)->vval.v_string;
    floc = (ശൂന്യം *)അട്ടി_TV(ectx->ec_frame_idx
				   + അട്ടി_FRAME_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_frame_idx = അട്ടി_TV(ectx->ec_frame_idx
				       + അട്ടി_FRAME_IDX_OFF)->vval.v_സംഖ്യ;

    എങ്കിൽ (floc == ശൂന്യ)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    അല്ലെങ്കിൽ
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    എങ്കിൽ (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_അട്ടി.ga_len = മുകൾ + 1;
	*അട്ടി_TV_BOT(-1) = *അട്ടി_TV(ret_idx);
    }
    അല്ലെങ്കിൽ
	// Reset the stack to the position before the call.
	ectx->ec_അട്ടി.ga_len = മുകൾ;

    funcdepth_decrement();
    --ex_nesting_level;
    തിരികെ OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
call_prepare(പൂർണ്ണസംഖ്യ argcount, typval_T *argvars, ectx_T *ectx)
{
    പൂർണ്ണസംഖ്യ		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    വേണ്ടി (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *അട്ടി_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    എങ്കിൽ (argcount > 0)
	ectx->ec_അട്ടി.ga_len -= argcount - 1;
    അല്ലെങ്കിൽ എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
	തിരികെ FAIL;
    അല്ലെങ്കിൽ
	++ectx->ec_അട്ടി.ga_len;

    // Default return value is zero.
    tv = അട്ടി_TV_BOT(-1);
    tv->v_type = VAR_സംഖ്യ;
    tv->vval.v_സംഖ്യ = 0;
    tv->v_lock = 0;

    തിരികെ OK;
}

/*
 * Call a builtin function by index.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
call_bfunc(പൂർണ്ണസംഖ്യ func_idx, പൂർണ്ണസംഖ്യ argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    അക്ഷരം	*save_func_name = ectx->ec_where.wt_func_name;

    എങ്കിൽ (call_prepare(argcount, argvars, ectx) == FAIL)
	തിരികെ FAIL;

    // Check for void value being passed as an argument.
    വേണ്ടി (idx = 0; idx < argcount; ++idx)
	എങ്കിൽ (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_മൂല്യം));
	    വേണ്ടി (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    തിരികെ FAIL;
	}

    ectx->ec_where.wt_func_name = internal_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_internal_func_by_idx(func_idx, argvars, അട്ടി_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    വേണ്ടി (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    എങ്കിൽ (did_emsg > did_emsg_before)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	പൂർണ്ണസംഖ്യ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    funcexe_T   funcexe;
    funcerror_T	error;
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    എങ്കിൽ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, ശൂന്യ)
								       == FAIL)
	തിരികെ FAIL;
    എങ്കിൽ (ufunc->uf_def_status == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	എങ്കിൽ (error != FCERR_UNKNOWN)
	{
	    എങ്കിൽ (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    അല്ലെങ്കിൽ
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    തിരികെ FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	എങ്കിൽ (iptr != ശൂന്യ)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	തിരികെ call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    എങ്കിൽ (call_prepare(argcount, argvars, ectx) == FAIL)
	തിരികെ FAIL;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != ശൂന്യ ? selfdict : dict_അട്ടി_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      അട്ടി_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    വേണ്ടി (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    എങ്കിൽ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_found_var);
	തിരികെ FAIL;
    }
    എങ്കിൽ (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * If command modifiers were applied restore them.
 */
    സ്ഥിതം ശൂന്യം
may_restore_cmdmod(funclocal_T *funclocal)
{
    എങ്കിൽ (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ശൂന്യ;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
vim9_aborting(പൂർണ്ണസംഖ്യ prev_uncaught_emsg)
{
    തിരികെ uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
call_by_name(
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    എങ്കിൽ (builtin_function(name, -1))
    {
	പൂർണ്ണസംഖ്യ func_idx = find_internal_func(name);

	എങ്കിൽ (func_idx < 0)  // Impossible?
	    തിരികെ FAIL;
	എങ്കിൽ (check_internal_func(func_idx, argcount) < 0)
	    തിരികെ FAIL;
	തിരികെ call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*start_bracket = generic_func_find_open_bracket(name);
    എങ്കിൽ (start_bracket != ശൂന്യ)
    {
	cc = *start_bracket;
	*start_bracket = NUL;
    }

    ufunc = find_func(name, FALSE);

    എങ്കിൽ (ufunc == ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ prev_uncaught_emsg = uncaught_emsg;

	എങ്കിൽ (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = find_func(name, FALSE);

	എങ്കിൽ (vim9_aborting(prev_uncaught_emsg))
	{
	    എങ്കിൽ (start_bracket != ശൂന്യ)
		*start_bracket = cc;
	    തിരികെ FAIL;  // bail out if loading the script caused an error
	}
    }

    എങ്കിൽ (start_bracket != ശൂന്യ)
	*start_bracket = cc;

    എങ്കിൽ (ufunc != ശൂന്യ)
    {
	എങ്കിൽ (IS_GENERIC_FUNC(ufunc))
	{
	    എങ്കിൽ (start_bracket != ശൂന്യ)
		ufunc = find_generic_func(ufunc, name, &start_bracket);
	    അല്ലെങ്കിൽ
	    {
		emsg_funcname(e_generic_func_missing_type_args_str, name);
		ufunc = ശൂന്യ;
	    }
	    എങ്കിൽ (ufunc == ശൂന്യ)
		തിരികെ FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (start_bracket != ശൂന്യ)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    തിരികെ FAIL;
	}

	എങ്കിൽ (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    തിരികെ FAIL;

	തിരികെ call_ufunc(ufunc, ശൂന്യ, argcount, ectx, iptr, selfdict);
    }

    തിരികെ FAIL;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
call_partial(
	typval_T    *tv,
	പൂർണ്ണസംഖ്യ	    argcount_arg,
	ectx_T	    *ectx)
{
    പൂർണ്ണസംഖ്യ		argcount = argcount_arg;
    char_u	*name = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		called_emsg_before = called_emsg;
    പൂർണ്ണസംഖ്യ		res = FAIL;
    dict_T	*selfdict = ശൂന്യ;

    എങ്കിൽ (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	പൂർണ്ണസംഖ്യ	    i;

	എങ്കിൽ (pt->pt_obj != ശൂന്യ)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		തിരികെ FAIL;
	    വേണ്ടി (i = 1; i <= argcount; ++i)
		*അട്ടി_TV_BOT(-i + 1) = *അട്ടി_TV_BOT(-i);

	    typval_T *obj_tv = അട്ടി_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_lock = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_അട്ടി.ga_len;
	}

	എങ്കിൽ (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, pt->pt_argc))
		തിരികെ FAIL;
	    വേണ്ടി (i = 1; i <= argcount; ++i)
		*അട്ടി_TV_BOT(-i + pt->pt_argc) = *അട്ടി_TV_BOT(-i);
	    ectx->ec_അട്ടി.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    വേണ്ടി (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], അട്ടി_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	എങ്കിൽ (pt->pt_func != ശൂന്യ)
	    തിരികെ call_ufunc(pt->pt_func, pt, argcount, ectx, ശൂന്യ, selfdict);

	name = pt->pt_name;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    എങ്കിൽ (name != ശൂന്യ)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = ശൂന്യ;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	എങ്കിൽ (error != FCERR_NONE)
	    res = FAIL;
	അല്ലെങ്കിൽ
	    res = call_by_name(fname, argcount, ectx, ശൂന്യ, selfdict);
	vim_free(tofree);
    }

    എങ്കിൽ (res == FAIL)
    {
	എങ്കിൽ (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == ശൂന്യ ? (char_u *)"[unknown]" : name);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
error_if_locked(പൂർണ്ണസംഖ്യ lock, അക്ഷരം *error)
{
    എങ്കിൽ (lock & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
check_for_സംഖ്യ(typval_T *tv)
{
    എങ്കിൽ (tv->v_type != VAR_സംഖ്യ)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_സംഖ്യ), vartype_name(tv->v_type));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    സ്ഥിതം ശൂന്യം
store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    പൂർണ്ണസംഖ്യ		    flags = ASSIGN_DECL;

    എങ്കിൽ (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, ശൂന്യ, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_2string(typval_T *tv, പൂർണ്ണസംഖ്യ is_2string_any, പൂർണ്ണസംഖ്യ tostring_flags)
{
    എങ്കിൽ (tv->v_type == VAR_STRING)
	തിരികെ OK;

    char_u *str;

    എങ്കിൽ (is_2string_any)
    {
	തിരഞ്ഞെടുപ്പ് (tv->v_type)
	{
	    അവസ്ഥ VAR_SPECIAL:
	    അവസ്ഥ VAR_BOOL:
	    അവസ്ഥ VAR_സംഖ്യ:
	    അവസ്ഥ VAR_FLOAT:
	    അവസ്ഥ VAR_DICT:
	    അവസ്ഥ VAR_BLOB:	നിർത്തുക;

	    അവസ്ഥ VAR_LIST:
				എങ്കിൽ (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, വലുപ്പം(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    എങ്കിൽ (str == ശൂന്യ)
					തിരികെ FAIL;
				    s = str;
				    ആകുമ്പോൾ ((e = vim_strchr(s, '\n')) != ശൂന്യ)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					എങ്കിൽ (p != ശൂന്യ)
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
				    തിരികെ OK;
				}
				എങ്കിൽ (tostring_flags & TOSTRING_INTERPOLATE)
				    നിർത്തുക;
				// FALLTHROUGH
	    സ്ഥിരസ്ഥിതി:	to_string_error(tv->v_type);
			തിരികെ FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    തിരികെ OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    സ്ഥിതം ശൂന്യം
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_LIST:
	    എങ്കിൽ (tv->vval.v_list == ശൂന്യ && sv->sv_type != &t_list_empty)
		(ശൂന്യം)rettv_list_alloc(tv);
	    നിർത്തുക;
	അവസ്ഥ VAR_TUPLE:
	    എങ്കിൽ (tv->vval.v_tuple == ശൂന്യ && sv->sv_type != &t_tuple_empty)
		(ശൂന്യം)rettv_tuple_alloc(tv);
	    നിർത്തുക;
	അവസ്ഥ VAR_DICT:
	    എങ്കിൽ (tv->vval.v_dict == ശൂന്യ && sv->sv_type != &t_dict_empty)
		(ശൂന്യം)rettv_dict_alloc(tv);
	    നിർത്തുക;
	അവസ്ഥ VAR_BLOB:
	    എങ്കിൽ (tv->vval.v_blob == ശൂന്യ && sv->sv_type != &t_blob_null)
		(ശൂന്യം)rettv_blob_alloc(tv);
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    നിർത്തുക;
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

    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ ശൂന്യ;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    എങ്കിൽ (index < 0)
    {
	പൂർണ്ണസംഖ്യ	clen = 0;

	വേണ്ടി (nbyte = 0; nbyte < slen; ++clen)
	{
	    എങ്കിൽ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    അല്ലെങ്കിൽ
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + index;
	എങ്കിൽ (nchar < 0)
	    // unlike list: index out of range results in empty string
	    തിരികെ ശൂന്യ;
    }

    വേണ്ടി (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	എങ്കിൽ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	അല്ലെങ്കിൽ എങ്കിൽ (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	അല്ലെങ്കിൽ
	    nbyte += mb_ptr2len(str + nbyte);
    }
    എങ്കിൽ (nbyte >= slen)
	തിരികെ ശൂന്യ;
    തിരികെ vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    സ്ഥിതം long
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    എങ്കിൽ (nchar >= 0)
    {
	ആകുമ്പോൾ (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    അല്ലെങ്കിൽ
    {
	nbyte = str_len;
	ആകുമ്പോൾ (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_head_off(str, str + nbyte);
	    ++nchar;
	}
	എങ്കിൽ (nchar < 0)
	    തിരികെ -1;
    }
    തിരികെ (long)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_slice(char_u *str, varnumber_T first, varnumber_T last, പൂർണ്ണസംഖ്യ exclusive)
{
    long	start_byte, end_byte;
    size_t	slen;

    എങ്കിൽ (str == ശൂന്യ)
	തിരികെ ശൂന്യ;
    slen = STRLEN(str);
    start_byte = char_idx2byte(str, slen, first);
    എങ്കിൽ (start_byte < 0)
	start_byte = 0; // first index very negative: use zero
    എങ്കിൽ ((last == -1 && !exclusive) || last == VARNUM_MAX)
	end_byte = (long)slen;
    അല്ലെങ്കിൽ
    {
	end_byte = char_idx2byte(str, slen, last);
	എങ്കിൽ (!exclusive && end_byte >= 0 && end_byte < (long)slen)
	    // end index is inclusive
	    end_byte += mb_ptr2len(str + end_byte);
    }

    എങ്കിൽ (start_byte >= (long)slen || end_byte <= start_byte)
	തിരികെ ശൂന്യ;
    തിരികെ vim_strnsave(str + start_byte, end_byte - start_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    സ്ഥിതം svar_T *
get_script_svar(scriptref_T *sref, പൂർണ്ണസംഖ്യ dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? ശൂന്യ
			      : ((dfunc_T *)def_functions.ga_data) + dfunc_idx;
    svar_T	    *sv;

    എങ്കിൽ (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	എങ്കിൽ (dfunc != ശൂന്യ)
	    semsg(_(e_script_variable_invalid_after_reload_in_function_str),
					 printable_func_name(dfunc->df_ufunc));
	തിരികെ ശൂന്യ;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_data) + sref->sref_idx;
    എങ്കിൽ (sv->sv_name == ശൂന്യ)
    {
	എങ്കിൽ (dfunc != ശൂന്യ)
	    emsg(_(e_script_variable_was_deleted));
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	എങ്കിൽ (dfunc != ശൂന്യ)
	    emsg(_(e_script_variable_type_changed));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	എങ്കിൽ (dfunc != ശൂന്യ)
	    semsg(_(e_item_not_exported_in_script_str), sv->sv_name);
	തിരികെ ശൂന്യ;
    }
    തിരികെ sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    സ്ഥിതം char_u *
get_split_sourceline(
	പൂർണ്ണസംഖ്യ c UNUSED,
	ശൂന്യം *cookie,
	പൂർണ്ണസംഖ്യ indent UNUSED,
	getline_opt_T options UNUSED)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*p;
    char_u		*വരി;

    p = vim_strchr(sp->nextline, '\n');
    എങ്കിൽ (p == ശൂന്യ)
    {
	വരി = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    അല്ലെങ്കിൽ
    {
	വരി = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    തിരികെ വരി;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
call_eval_func(
	char_u	    *name,
	പൂർണ്ണസംഖ്യ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    പൂർണ്ണസംഖ്യ	    called_emsg_before = called_emsg;
    പൂർണ്ണസംഖ്യ	    res;

    res = call_by_name(name, argcount, ectx, iptr, ശൂന്യ);
    എങ്കിൽ (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = find_var(name, ശൂന്യ, FALSE);
	എങ്കിൽ (v == ശൂന്യ || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    തിരികെ FAIL;
	}
	തിരികെ call_partial(&v->di_tv, argcount, ectx);
    }
    തിരികെ res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    പൂർണ്ണസംഖ്യ
fill_partial_and_closure(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    എങ്കിൽ (ufunc->uf_flags & FC_CLOSURE)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.out_അട്ടി = &ectx->ec_അട്ടി;
	pt->pt_outer.out_frame_idx = ectx->ec_frame_idx;
	എങ്കിൽ (ectx->ec_outer_ref != ശൂന്യ)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.out_up = ectx->ec_outer_ref->or_outer;
	    എങ്കിൽ (ectx->ec_outer_ref->or_partial != ശൂന്യ)
	    {
		pt->pt_outer.out_up_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.out_up_partial->pt_refcount;
	    }
	}

	എങ്കിൽ (lvi != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ	depth;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    വേണ്ടി (depth = 0; depth < lvi->lvi_depth; ++depth)
	    {
		pt->pt_outer.out_loop[depth].അട്ടി = &ectx->ec_അട്ടി;
		pt->pt_outer.out_loop[depth].var_idx = ectx->ec_frame_idx
			 + അട്ടി_FRAME_SIZE + lvi->lvi_loop[depth].var_idx;
		pt->pt_outer.out_loop[depth].var_എണ്ണം =
					    lvi->lvi_loop[depth].var_എണ്ണം;
	    }
	    pt->pt_outer.out_loop_size = lvi->lvi_depth;
	}
	അല്ലെങ്കിൽ
	    pt->pt_outer.out_loop_size = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    തിരികെ FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_അട്ടി.ga_data) + ectx->ec_frame_idx
		       + അട്ടി_FRAME_SIZE + dfunc->df_varcount)->vval.v_സംഖ്യ;

	((partial_T **)ectx->ec_funcrefs.ga_data)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    തിരികെ OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exec_command(isn_T *iptr, char_u *cmd_string)
{
    source_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_FIELD(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    എങ്കിൽ (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	തിരികെ FAIL;
    തിരികെ OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    പൂർണ്ണസംഖ്യ
may_load_script(പൂർണ്ണസംഖ്യ sid, പൂർണ്ണസംഖ്യ *loaded)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);

    എങ്കിൽ (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	എങ്കിൽ (do_source(si->sn_name, FALSE, DOSO_NONE, ശൂന്യ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    തിരികെ FAIL;
	}
    }
    തിരികെ OK;
}

// used for v_instr of typval of VAR_INSTR
ഘടന instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
തരനിർവചനം ഘടന subs_വ്യഞ്ജകം_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    പൂർണ്ണസംഖ്യ		subs_status;
} subs_വ്യഞ്ജകം_T;

// Set when calling do_debug().
സ്ഥിതം ectx_T	*debug_context = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	debug_var_എണ്ണം;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    പൂർണ്ണസംഖ്യ		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    പൂർണ്ണസംഖ്യ		    varargs_off;

    എങ്കിൽ (ectx == ശൂന്യ)
	തിരികെ ശൂന്യ;
    dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    വേണ്ടി (idx = debug_var_എണ്ണം - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_names.ga_data)[idx];

	// the variable name may be NULL when not available in this block
	എങ്കിൽ (varname != ശൂന്യ && STRCMP(varname, name) == 0)
	    തിരികെ അട്ടി_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == ശൂന്യ ? 0 : 1;
    വേണ്ടി (idx = 0; idx < ufunc->uf_args.ga_len; ++idx)
	എങ്കിൽ (STRCMP(((char_u **)(ufunc->uf_args.ga_data))[idx], name) == 0)
	    തിരികെ അട്ടി_TV(ectx->ec_frame_idx - ufunc->uf_args.ga_len
							  - varargs_off + idx);
    എങ്കിൽ (ufunc->uf_va_name != ശൂന്യ && STRCMP(ufunc->uf_va_name, name) == 0)
	തിരികെ അട്ടി_TV(ectx->ec_frame_idx - 1);

    തിരികെ ശൂന്യ;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    പൂർണ്ണസംഖ്യ
may_break_in_function(ufunc_T *ufunc)
{
    തിരികെ ufunc->uf_has_breakpoint || debug_has_വ്യഞ്ജകം_breakpoint();
}

    സ്ഥിതം ശൂന്യം
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*വരി;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    പൂർണ്ണസംഖ്യ		end_lnum = iptr->isn_lnum;
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		lnum;

    എങ്കിൽ (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	എങ്കിൽ (!may_break_in_function(ufunc))
	    തിരികെ;

	// check for the next breakpoint if needed
	breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	എങ്കിൽ (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    തിരികെ;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_എണ്ണം = iptr->isn_arg.debug.dbg_var_names_len;

    വേണ്ടി (ni = iptr + 1; ni->isn_type != ISN_FINISH; ++ni)
	എങ്കിൽ (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    end_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    നിർത്തുക;
	}

    എങ്കിൽ (end_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, വലുപ്പം(char_u *), 10);
	വേണ്ടി (lnum = iptr->isn_lnum; lnum < end_lnum
				     && lnum <= ufunc->uf_വരികൾ.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_വരികൾ.ga_data)[lnum - 1];

	    എങ്കിൽ (p == ശൂന്യ)
		തുടരുക;  // left over from continuation line
	    p = skipwhite(p);
	    എങ്കിൽ (*p == '#')
		നിർത്തുക;
	    എങ്കിൽ (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_data))[ga.ga_len++] = p;
	    എങ്കിൽ (STRNCMP(p, "def ", 4) == 0)
		നിർത്തുക;
	}
	വരി = ga_concat_strings(&ga, "  ");
	vim_free(ga.ga_data);
    }
    അല്ലെങ്കിൽ
	വരി = ((char_u **)ufunc->uf_വരികൾ.ga_data)[iptr->isn_lnum - 1];

    do_debug(വരി == ശൂന്യ ? (char_u *)"[empty]" : വരി);
    debug_context = ശൂന്യ;

    എങ്കിൽ (end_lnum > iptr->isn_lnum)
	vim_free(വരി);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    എങ്കിൽ (lhs_list->lv_type == ശൂന്യ || lhs_list->lv_type->tt_member == ശൂന്യ)
	തിരികെ OK;

    തിരികെ check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = അട്ടി_TV_BOT(-2);
    long	lidx = 0;
    typval_T	*tv_dest = അട്ടി_TV_BOT(-1);
    പൂർണ്ണസംഖ്യ		status = OK;
    പൂർണ്ണസംഖ്യ		check_rhs_type = FALSE;

    എങ്കിൽ (tv_idx->v_type == VAR_സംഖ്യ)
	lidx = (long)tv_idx->vval.v_സംഖ്യ;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = അട്ടി_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    എങ്കിൽ (dest_type == VAR_OBJECT && tv_dest->vval.v_object == ശൂന്യ)
    {
	emsg(_(e_using_null_object));
	status = FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	എങ്കിൽ (dest_type == VAR_DICT)
	    status = do_2string(tv_idx, TRUE, FALSE);
	അല്ലെങ്കിൽ എങ്കിൽ (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    എങ്കിൽ (obj == ശൂന്യ)
	    {
		emsg(_(e_using_null_object));
		status = FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		പൂർണ്ണസംഖ്യ		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		എങ്കിൽ (m != ശൂന്യ)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    എങ്കിൽ ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == ശൂന്യ ||
			     !class_instance_of(ufunc->uf_class, cl)))
		    {
			അക്ഷരം *സന്ദേശം = (m->ocm_access == VIM_ACCESS_PRIVATE)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(സന്ദേശം, m->ocm_name.string, 0, cl);
			status = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    അല്ലെങ്കിൽ എങ്കിൽ (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			status = FAIL;
		    അല്ലെങ്കിൽ
			lidx = m_idx;
		}
		അല്ലെങ്കിൽ
		{
		    member_not_found_സന്ദേശം(cl, VAR_OBJECT, member, 0);
		    status = FAIL;
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    status = FAIL;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dest_type == VAR_LIST && tv_idx->v_type != VAR_സംഖ്യ)
	{
	    emsg(_(e_സംഖ്യ_expected));
	    status = FAIL;
	}
    }

    എങ്കിൽ (status == OK)
    {
	എങ്കിൽ (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    എങ്കിൽ (list == ശൂന്യ)
	    {
		emsg(_(e_list_not_set));
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    എങ്കിൽ (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_index_out_of_range_nr), lidx);
		തിരികെ FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    എങ്കിൽ (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		തിരികെ FAIL;

	    എങ്കിൽ (lidx < list->lv_len)
	    {
		listitem_T *li = list_find(list, lidx);
		എങ്കിൽ (li == ശൂന്യ)
		    തിരികെ FAIL;
		എങ്കിൽ (error_if_locked(li->li_tv.v_lock,
					     e_cannot_change_locked_list_item))
		    തിരികെ FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (error_if_locked(list->lv_lock, e_cannot_change_locked_list))
		    തിരികെ FAIL;
		// append to list, only fails when out of memory
		എങ്കിൽ (list_append_tv(list, tv) == FAIL)
		    തിരികെ NOTDONE;
		clear_tv(tv);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dest_type == VAR_DICT)
	{
	    char_u		*താക്കോൽ = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    എങ്കിൽ (dict == ശൂന്യ)
	    {
		emsg(_(e_dictionary_not_set));
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (താക്കോൽ == ശൂന്യ)
		താക്കോൽ = (char_u *)"";
	    di = dict_find(dict, താക്കോൽ, -1);
	    എങ്കിൽ (di != ശൂന്യ)
	    {
		എങ്കിൽ (error_if_locked(di->di_tv.v_lock,
						    e_cannot_change_dict_item))
		    തിരികെ FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (error_if_locked(dict->dv_lock, e_cannot_change_dict))
		    തിരികെ FAIL;
		// add to dict, only fails when out of memory
		എങ്കിൽ (dict_add_tv(dict, (അക്ഷരം *)താക്കോൽ, tv) == FAIL)
		    തിരികെ NOTDONE;
		clear_tv(tv);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    പൂർണ്ണസംഖ്യ		    error = FALSE;
	    പൂർണ്ണസംഖ്യ		    len;

	    എങ്കിൽ (blob == ശൂന്യ)
	    {
		emsg(_(e_blob_not_set));
		തിരികെ FAIL;
	    }
	    len = blob_len(blob);
	    എങ്കിൽ (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    എങ്കിൽ (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), lidx);
		തിരികെ FAIL;
	    }
	    എങ്കിൽ (മൂല്യം_check_lock(blob->bv_lock, (char_u *)"blob", FALSE))
		തിരികെ FAIL;
	    nr = tv_get_സംഖ്യ_chk(tv, &error);
	    എങ്കിൽ (error)
		തിരികെ FAIL;
	    എങ്കിൽ (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_മൂല്യം_for_blob_nr), nr);
		തിരികെ FAIL;
	    }
	    blob_set_append(blob, lidx, nr);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    എങ്കിൽ (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		എങ്കിൽ (itf != ശൂന്യ)
		    // convert interface member index to class member index
		    lidx = object_index_from_itf_index(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    അല്ലെങ്കിൽ
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    status = FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    status = FAIL;
	    semsg(_(e_cannot_index_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_അട്ടി.ga_len -= 3;
    എങ്കിൽ (status == FAIL)
    {
	clear_tv(tv);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Store a value in a list or blob range.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = അട്ടി_TV_BOT(-3);
    typval_T	*tv_idx2 = അട്ടി_TV_BOT(-2);
    typval_T	*tv_dest = അട്ടി_TV_BOT(-1);
    പൂർണ്ണസംഖ്യ		status = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = അട്ടി_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    എങ്കിൽ (tv_dest->v_type == VAR_LIST)
    {
	long	    n1;
	long	    n2;
	listitem_T  *li1;

	n1 = (long)tv_get_സംഖ്യ_chk(tv_idx1, ശൂന്യ);
	എങ്കിൽ (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_സംഖ്യ == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	അല്ലെങ്കിൽ
	    n2 = (long)tv_get_സംഖ്യ_chk(tv_idx2, ശൂന്യ);

	li1 = check_range_index_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	എങ്കിൽ (li1 == ശൂന്യ)
	    status = FAIL;
	അല്ലെങ്കിൽ
	{
	    status = check_range_index_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    എങ്കിൽ (status != FAIL)
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
    അല്ലെങ്കിൽ എങ്കിൽ (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	long	    bloblen;

	n1 = tv_get_സംഖ്യ_chk(tv_idx1, ശൂന്യ);
	എങ്കിൽ (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_സംഖ്യ == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	അല്ലെങ്കിൽ
	    n2 = tv_get_സംഖ്യ_chk(tv_idx2, ശൂന്യ);
	bloblen = blob_len(tv_dest->vval.v_blob);

	എങ്കിൽ (check_blob_index(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    status = FAIL;
	അല്ലെങ്കിൽ
	    status = blob_set_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    അല്ലെങ്കിൽ
    {
	status = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_അട്ടി.ga_len -= 4;
    clear_tv(tv);

    തിരികെ status;
}

/*
 * Unlet item in list or dict variable.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = അട്ടി_TV_BOT(-2);
    typval_T	*tv_dest = അട്ടി_TV_BOT(-1);
    പൂർണ്ണസംഖ്യ		status = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    എങ്കിൽ (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	എങ്കിൽ (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_സംഖ്യ)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    status = FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*താക്കോൽ;
	    dictitem_T  *di = ശൂന്യ;

	    എങ്കിൽ (d != ശൂന്യ && മൂല്യം_check_lock(
				      d->dv_lock, ശൂന്യ, FALSE))
		status = FAIL;
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (tv_idx->v_type == VAR_STRING)
		{
		    താക്കോൽ = tv_idx->vval.v_string;
		    എങ്കിൽ (താക്കോൽ == ശൂന്യ)
			താക്കോൽ = (char_u *)"";
		}
		അല്ലെങ്കിൽ
		{
		    താക്കോൽ = tv_get_string(tv_idx);
		}
		എങ്കിൽ (d != ശൂന്യ)
		    di = dict_find(d, താക്കോൽ, (പൂർണ്ണസംഖ്യ)STRLEN(താക്കോൽ));
		എങ്കിൽ (di == ശൂന്യ)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_താക്കോൽ_not_present_in_dictionary_str), താക്കോൽ);
		    status = FAIL;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (var_check_fixed(di->di_flags,
						   ശൂന്യ, FALSE)
			|| var_check_ro(di->di_flags,
						  ശൂന്യ, FALSE))
		    status = FAIL;
		അല്ലെങ്കിൽ
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	എങ്കിൽ (check_for_സംഖ്യ(tv_idx) == FAIL)
	{
	    status = FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n = (long)tv_idx->vval.v_സംഖ്യ;

	    എങ്കിൽ (l != ശൂന്യ && മൂല്യം_check_lock(
				      l->lv_lock, ശൂന്യ, FALSE))
		status = FAIL;
	    അല്ലെങ്കിൽ
	    {
		listitem_T	*li = list_find(l, n);

		എങ്കിൽ (li == ശൂന്യ)
		{
		    semsg(_(e_list_index_out_of_range_nr), n);
		    status = FAIL;
		}
		അല്ലെങ്കിൽ
		    listitem_remove(l, li);
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	status = FAIL;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_അട്ടി.ga_len -= 2;

    തിരികെ status;
}

/*
 * Unlet a range of items in a list variable.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = അട്ടി_TV_BOT(-3);
    typval_T	*tv_idx2 = അട്ടി_TV_BOT(-2);
    typval_T	*tv_dest = അട്ടി_TV_BOT(-1);
    പൂർണ്ണസംഖ്യ		status = OK;

    എങ്കിൽ (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	എങ്കിൽ (check_for_സംഖ്യ(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_സംഖ്യ(tv_idx2) == FAIL))
	{
	    status = FAIL;
	}
	അല്ലെങ്കിൽ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n1 = (long)tv_idx1->vval.v_സംഖ്യ;
	    long	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (long)tv_idx2->vval.v_സംഖ്യ;
	    listitem_T	*li;

	    li = list_find_index(l, &n1);
	    എങ്കിൽ (li == ശൂന്യ)
	    {
		semsg(_(e_list_index_out_of_range_nr),
						 (long)tv_idx1->vval.v_സംഖ്യ);
		status = FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		എങ്കിൽ (n1 < 0)
		    n1 = list_idx_of_item(l, li);
		എങ്കിൽ (n2 < 0)
		{
		    listitem_T *li2 = list_find(l, n2);

		    എങ്കിൽ (li2 == ശൂന്യ)
		    {
			semsg(_(e_list_index_out_of_range_nr), n2);
			status = FAIL;
		    }
		    അല്ലെങ്കിൽ
			n2 = list_idx_of_item(l, li2);
		}
		എങ്കിൽ (status != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_index_out_of_range_nr), n2);
		    status = FAIL;
		}
		എങ്കിൽ (status != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	status = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_അട്ടി.ga_len -= 3;

    തിരികെ status;
}

/*
 * Top of a for loop.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    പൂർണ്ണസംഖ്യ		jump = FALSE;
    typval_T	*ltv = അട്ടി_TV_BOT(-1);
    typval_T	*idxtv =
		   അട്ടി_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
	തിരികെ FAIL;
    എങ്കിൽ (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_സംഖ്യ;
	എങ്കിൽ (list == ശൂന്യ
		       || idxtv->vval.v_സംഖ്യ >= list->lv_len)
	{
	    jump = TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (list->lv_first == &range_list_item)
	{
	    // non-materialized range() list
	    tv = അട്ടി_TV_BOT(0);
	    tv->v_type = VAR_സംഖ്യ;
	    tv->v_lock = 0;
	    tv->vval.v_സംഖ്യ = list_find_nr(
			     list, idxtv->vval.v_സംഖ്യ, ശൂന്യ);
	    ++ectx->ec_അട്ടി.ga_len;
	}
	അല്ലെങ്കിൽ
	{
	    listitem_T *li = list_find(list,
					 idxtv->vval.v_സംഖ്യ);
	    എങ്കിൽ (li == ശൂന്യ)
		തിരികെ FAIL;
	    copy_tv(&li->li_tv, അട്ടി_TV_BOT(0));
	    ++ectx->ec_അട്ടി.ga_len;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_സംഖ്യ;
	എങ്കിൽ (tuple == ശൂന്യ || idxtv->vval.v_സംഖ്യ >= TUPLE_LEN(tuple))
	    jump = TRUE;
	അല്ലെങ്കിൽ
	{
	    copy_tv(TUPLE_ITEM(tuple, idxtv->vval.v_സംഖ്യ), അട്ടി_TV_BOT(0));
	    ++ectx->ec_അട്ടി.ga_len;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_സംഖ്യ;
	എങ്കിൽ (str == ശൂന്യ || str[idxtv->vval.v_സംഖ്യ] == NUL)
	{
	    jump = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ	clen = mb_ptr2len(str + idxtv->vval.v_സംഖ്യ);

	    // Push the next character from the string.
	    tv = അട്ടി_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_സംഖ്യ, clen);
	    ++ectx->ec_അട്ടി.ga_len;
	    idxtv->vval.v_സംഖ്യ += clen - 1;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	എങ്കിൽ (idxtv->vval.v_സംഖ്യ == -1 && blob != ശൂന്യ)
	{
	    ltv->v_lock = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_സംഖ്യ;
	എങ്കിൽ (blob == ശൂന്യ || idxtv->vval.v_സംഖ്യ >= blob_len(blob))
	{
	    jump = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    // Push the next byte from the blob.
	    tv = അട്ടി_TV_BOT(0);
	    tv->v_type = VAR_സംഖ്യ;
	    tv->vval.v_സംഖ്യ = blob_get(blob,
					 idxtv->vval.v_സംഖ്യ);
	    ++ectx->ec_അട്ടി.ga_len;
	}
    }
    അല്ലെങ്കിൽ
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	തിരികെ FAIL;
    }

    എങ്കിൽ (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_end;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    അല്ലെങ്കിൽ
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = അട്ടി_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_സംഖ്യ = ectx->ec_funcrefs.ga_len;
    }

    തിരികെ OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
സ്ഥിതം loopvars_T *first_loopvars = ശൂന്യ;

    സ്ഥിതം ശൂന്യം
add_loopvars_to_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    എങ്കിൽ (first_loopvars != ശൂന്യ)
	first_loopvars->lvs_prev = loopvars;
    loopvars->lvs_next = first_loopvars;
    loopvars->lvs_prev = ശൂന്യ;
    first_loopvars = loopvars;
}

    സ്ഥിതം ശൂന്യം
remove_loopvars_from_list(loopvars_T *loopvars)
{
    എങ്കിൽ (loopvars->lvs_prev == ശൂന്യ)
	first_loopvars = loopvars->lvs_next;
    അല്ലെങ്കിൽ
	loopvars->lvs_prev->lvs_next = loopvars->lvs_next;
    എങ്കിൽ (loopvars->lvs_next != ശൂന്യ)
	loopvars->lvs_next->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = അട്ടി_TV_VAR(endloop->end_funcref_idx);
    പൂർണ്ണസംഖ്യ		prev_closure_എണ്ണം = tv_refcount->vval.v_സംഖ്യ;
    പൂർണ്ണസംഖ്യ		depth = endloop->end_depth;
    garray_T	*gap = &ectx->ec_funcrefs;
    പൂർണ്ണസംഖ്യ		closure_in_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *അട്ടി;
    പൂർണ്ണസംഖ്യ		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    വേണ്ടി (idx = prev_closure_എണ്ണം; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	എങ്കിൽ (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ refcount = pt->pt_refcount;
	    പൂർണ്ണസംഖ്യ i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    വേണ്ടി (i = 0; i < endloop->end_var_എണ്ണം; ++i)
	    {
		typval_T *stv = അട്ടി_TV_VAR(endloop->end_var_idx + i);

		എങ്കിൽ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    എങ്കിൽ (refcount > 1)
	    {
		closure_in_use = TRUE;
		നിർത്തുക;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    എങ്കിൽ (!closure_in_use)
	തിരികെ OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    എങ്കിൽ (loopvars == ശൂന്യ)
	തിരികെ FAIL;

    loopvars->lvs_ga.ga_len = endloop->end_var_എണ്ണം;
    അട്ടി = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_data = അട്ടി;
    എങ്കിൽ (അട്ടി == ശൂന്യ)
    {
	vim_free(loopvars);
	തിരികെ FAIL;
    }
    add_loopvars_to_list(loopvars);

    // Move the variable values.
    വേണ്ടി (idx = 0; idx < endloop->end_var_എണ്ണം; ++idx)
    {
	typval_T *tv = അട്ടി_TV_VAR(endloop->end_var_idx + idx);

	*(അട്ടി + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    വേണ്ടി (idx = prev_closure_എണ്ണം; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	എങ്കിൽ (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == ശൂന്യ)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[depth] = loopvars;

	    pt->pt_outer.out_loop[depth].അട്ടി = &loopvars->lvs_ga;
	    pt->pt_outer.out_loop[depth].var_idx -=
		  ectx->ec_frame_idx + അട്ടി_FRAME_SIZE + endloop->end_var_idx;
	}
    }

    തിരികെ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    പൂർണ്ണസംഖ്യ
loopvars_check_refcount(loopvars_T *loopvars)
{
    പൂർണ്ണസംഖ്യ		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    പൂർണ്ണസംഖ്യ		    done = 0;
	typval_T	*അട്ടി = gap->ga_data;

    എങ്കിൽ (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	തിരികെ FALSE;
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_data) + i;

	എങ്കിൽ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ശൂന്യ
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    പൂർണ്ണസംഖ്യ	    depth;

	    വേണ്ടി (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		എങ്കിൽ (tv->vval.v_partial->pt_loopvars[depth] == loopvars)
		    ++done;
	}
    }
    എങ്കിൽ (done != loopvars->lvs_min_refcount)
	തിരികെ FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    അട്ടി = gap->ga_data;
    വേണ്ടി (i = 0; i < gap->ga_len; ++i)
	clear_tv(അട്ടി + i);
    vim_free(അട്ടി);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    തിരികെ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    പൂർണ്ണസംഖ്യ
set_ref_in_loopvars(പൂർണ്ണസംഖ്യ copyID)
{
    loopvars_T *loopvars;

    വേണ്ടി (loopvars = first_loopvars; loopvars != ശൂന്യ;
						 loopvars = loopvars->lvs_next)
    {
	typval_T    *അട്ടി = loopvars->lvs_ga.ga_data;
	പൂർണ്ണസംഖ്യ	    i;

	വേണ്ടി (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    എങ്കിൽ (set_ref_in_item(അട്ടി + i, copyID, ശൂന്യ, ശൂന്യ, ശൂന്യ))
		തിരികെ TRUE;  // abort
    }
    തിരികെ FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
load_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = ശൂന്യ;
    hashtab_T	*ht = ശൂന്യ;
    അക്ഷരം	namespace;

    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
	തിരികെ NOTDONE;
    തിരഞ്ഞെടുപ്പ് (isn_type)
    {
	അവസ്ഥ ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    നിർത്തുക;
	അവസ്ഥ ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    നിർത്തുക;
	അവസ്ഥ ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    നിർത്തുക;
	അവസ്ഥ ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:  // Cannot reach here
	    തിരികെ NOTDONE;
    }
    di = find_var_in_ht(ht, 0, iptr->isn_arg.string, TRUE);

    എങ്കിൽ (di == ശൂന്യ)
    {
	എങ്കിൽ (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = find_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    എങ്കിൽ (ufunc != ശൂന്യ)
	    {
		typval_T    *tv = അട്ടി_TV_BOT(0);

		++ectx->ec_അട്ടി.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		എങ്കിൽ (tv->vval.v_string == ശൂന്യ)
		    തിരികെ FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		തിരികെ OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	എങ്കിൽ (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != ശൂന്യ)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_item_not_found_in_script_str), iptr->isn_arg.string);
	അല്ലെങ്കിൽ
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = അട്ടി_TV_BOT(0);
	tv->v_type = VAR_സംഖ്യ;
	tv->v_lock = 0;
	tv->vval.v_സംഖ്യ = 0;
	++ectx->ec_അട്ടി.ga_len;
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
    {
	copy_tv(&di->di_tv, അട്ടി_TV_BOT(0));
	++ectx->ec_അട്ടി.ga_len;
    }
    തിരികെ OK;
}


    സ്ഥിതം ശൂന്യം
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, വലുപ്പം(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    അക്ഷരം *tofree = ശൂന്യ;
    അക്ഷരം *typename = type_name(type, &tofree);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    പൂർണ്ണസംഖ്യ		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = ശൂന്യ;
    typval_T	mtv;

    എങ്കിൽ (is_object)
    {
	obj = tv->vval.v_object;
	എങ്കിൽ (obj == ശൂന്യ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    തിരികെ FAIL;
	}
	tv_cl = obj->obj_class;
    }
    അല്ലെങ്കിൽ
    {
	tv_cl = tv->vval.v_class;
	എങ്കിൽ (tv_cl == ശൂന്യ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    തിരികെ FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    പൂർണ്ണസംഖ്യ		namelen = (പൂർണ്ണസംഖ്യ)STRLEN(iptr->isn_arg.string);
    പൂർണ്ണസംഖ്യ		save_did_emsg = did_emsg;

    എങ്കിൽ (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	തിരികെ OK;
    }

    എങ്കിൽ (did_emsg != save_did_emsg)
	തിരികെ FAIL;

    // could be a class or instance method
    ufunc_T	*oc_method;
    പൂർണ്ണസംഖ്യ		oc_method_idx;

    oc_method = method_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_method_idx);
    എങ്കിൽ (oc_method == ശൂന്യ)
    {
	അക്ഷരം	*സന്ദേശം;

	SOURCING_LNUM = iptr->isn_lnum;
	എങ്കിൽ (is_object)
	    സന്ദേശം = e_variable_not_found_on_object_str_str;
	അല്ലെങ്കിൽ
	    സന്ദേശം = e_class_variable_str_not_found_in_class_str;
	semsg(_(സന്ദേശം), iptr->isn_arg.string, tv_cl->class_name.string);
	തിരികെ FAIL;
    }

    // Protected methods are not accessible outside the class
    എങ്കിൽ (*oc_method->uf_name == '_'
			&& !class_instance_of(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_method_str), oc_method->uf_name);
	തിരികെ FAIL;
    }

    // Create a partial for the instance or class method
    എങ്കിൽ (obj_method_to_partial_tv(is_object ? obj : ശൂന്യ, oc_method, tv)
								== FAIL)
	തിരികെ FAIL;

    തിരികെ OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    സ്ഥിതം ശൂന്യം
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, പൂർണ്ണസംഖ്യ fixindent)
{
    പൂർണ്ണസംഖ്യ		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*വ്യഞ്ജകം = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		dir = FORWARD;

    എങ്കിൽ (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = അട്ടി_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_സംഖ്യ;
	എങ്കിൽ (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_അട്ടി.ga_len;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    അല്ലെങ്കിൽ എങ്കിൽ (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	എങ്കിൽ (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    എങ്കിൽ (regname == '=')
    {
	tv = അട്ടി_TV_BOT(-1);
	എങ്കിൽ (tv->v_type == VAR_STRING)
	    വ്യഞ്ജകം = tv->vval.v_string;
	അല്ലെങ്കിൽ
	{
	    വ്യഞ്ജകം = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_അട്ടി.ga_len;
    }
    check_cursor();

    എങ്കിൽ (fixindent)
	do_put(regname, വ്യഞ്ജകം, dir, 1L, PUT_വരി|PUT_CURSLINE|PUT_FIXINDENT);
    അല്ലെങ്കിൽ
	do_put(regname, വ്യഞ്ജകം, dir, 1L, PUT_വരി|PUT_CURSLINE);
    vim_free(വ്യഞ്ജകം);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    പൂർണ്ണസംഖ്യ		എണ്ണം = iptr->isn_arg.unpack.unp_എണ്ണം;
    പൂർണ്ണസംഖ്യ		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    പൂർണ്ണസംഖ്യ		i;

    l = tv->vval.v_list;
    എങ്കിൽ (l == ശൂന്യ
	    || l->lv_len < (semicolon ? എണ്ണം - 1 : എണ്ണം))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_മൂല്യം_does_not_have_enough_ഇനങ്ങൾ));
	തിരികെ EXEC_FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!semicolon && l->lv_len > എണ്ണം)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_മൂല്യം_has_more_ഇനങ്ങൾ_than_targets));
	തിരികെ EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, എണ്ണം - 1))
	തിരികെ EXEC_DONE;
    ectx->ec_അട്ടി.ga_len += എണ്ണം - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    എങ്കിൽ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ഇനങ്ങൾ(l->lv_len - എണ്ണം + 1);

	എങ്കിൽ (rem_list == ശൂന്യ)
	    തിരികെ EXEC_DONE;
	tv = അട്ടി_TV_BOT(-എണ്ണം);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	li = l->lv_first;
	വേണ്ടി (i = 0; i < എണ്ണം - 1; ++i)
	    li = li->li_next;
	വേണ്ടി (i = 0; li != ശൂന്യ; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_set_item(rem_list, i, &tvcopy);
	    li = li->li_next;
	}
	--എണ്ണം;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_first;
    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
    {
	tv = അട്ടി_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_next;
    }

    list_unref(l);

    തിരികെ EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    പൂർണ്ണസംഖ്യ		എണ്ണം = iptr->isn_arg.unpack.unp_എണ്ണം;
    പൂർണ്ണസംഖ്യ		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    പൂർണ്ണസംഖ്യ		i;

    tuple = tv->vval.v_tuple;
    എങ്കിൽ (tuple == ശൂന്യ
	    || TUPLE_LEN(tuple) < (semicolon ? എണ്ണം - 1 : എണ്ണം))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_more_targets_than_tuple_ഇനങ്ങൾ));
	തിരികെ EXEC_FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!semicolon && TUPLE_LEN(tuple) > എണ്ണം)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_ഇനങ്ങൾ));
	തിരികെ EXEC_FAIL;
    }

    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, എണ്ണം - 1))
	തിരികെ EXEC_DONE;
    ectx->ec_അട്ടി.ga_len += എണ്ണം - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    എങ്കിൽ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ഇനങ്ങൾ(TUPLE_LEN(tuple) - എണ്ണം + 1);

	എങ്കിൽ (rem_list == ശൂന്യ)
	    തിരികെ EXEC_DONE;
	tv = അട്ടി_TV_BOT(-എണ്ണം);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	വേണ്ടി (i = എണ്ണം - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ITEM(tuple, i), &tvcopy);
	    list_set_item(rem_list, i - (എണ്ണം - 1), &tvcopy);
	}
	--എണ്ണം;
    }

    // Produce the values in reverse order, first item last.
    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
    {
	tv = അട്ടി_TV_BOT(-i - 1);
	copy_tv(TUPLE_ITEM(tuple, i), tv);
    }

    tuple_unref(tuple);

    തിരികെ EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
exec_instructions(ectx_T *ectx)
{
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    പൂർണ്ണസംഖ്യ		save_trylevel_at_start = ectx->ec_trylevel_at_start;
    പൂർണ്ണസംഖ്യ		dict_അട്ടി_len_at_start = dict_അട്ടി.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_start = trylevel;

    വേണ്ടി (;;)
    {
	സ്ഥിതം പൂർണ്ണസംഖ്യ  breakcheck_എണ്ണം = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	എങ്കിൽ (unlikely(++breakcheck_എണ്ണം >= 100))
	{
	    വരി_breakcheck();
	    breakcheck_എണ്ണം = 0;
	}
	എങ്കിൽ (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    എങ്കിൽ (throw_exception("Vim:Interrupt", ET_INTERRUPT, ശൂന്യ) == FAIL)
		പോകുക theend;
	    did_throw = TRUE;
	}

	എങ്കിൽ (unlikely(did_emsg && സന്ദേശം_list != ശൂന്യ && *സന്ദേശം_list != ശൂന്യ))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    എങ്കിൽ (throw_exception(*സന്ദേശം_list, ET_ERROR, ശൂന്യ) == FAIL)
		പോകുക theend;
	    did_throw = TRUE;
	    *സന്ദേശം_list = ശൂന്യ;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    എങ്കിൽ (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		എങ്കിൽ (trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		{
		    എങ്കിൽ (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			finish_exception(caught_അട്ടി);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	എങ്കിൽ (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = ശൂന്യ;
	    പൂർണ്ണസംഖ്യ		index = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    ആകുമ്പോൾ (index > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_data) + index - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		എങ്കിൽ (!trycmd->tcd_in_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    നിർത്തുക;
		// In the catch and finally block of this try we have to go up
		// one level.
		--index;
		trycmd = ശൂന്യ;
	    }
	    എങ്കിൽ (trycmd != ശൂന്യ && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
	    {
		എങ്കിൽ (trycmd->tcd_in_catch)
		{
		    എങ്കിൽ (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    അല്ലെങ്കിൽ
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		അല്ലെങ്കിൽ
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_in_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    അല്ലെങ്കിൽ
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    പോകുക theend;
		tv = അട്ടി_TV_BOT(0);
		tv->v_type = VAR_സംഖ്യ;
		tv->vval.v_സംഖ്യ = 0;
		++ectx->ec_അട്ടി.ga_len;
		എങ്കിൽ (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    എങ്കിൽ (handle_closure_in_use(ectx, FALSE) == FAIL)
			പോകുക theend;
		    പോകുക done;
		}

		എങ്കിൽ (func_return(ectx) == FAIL)
		    പോകുക theend;
	    }
	    തുടരുക;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	തിരഞ്ഞെടുപ്പ് (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    അവസ്ഥ ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = അട്ടി_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		നിർത്തുക;

	    // execute Ex command line
	    അവസ്ഥ ISN_EXEC:
		എങ്കിൽ (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    പോകുക on_error;
		നിർത്തുക;

	    // execute Ex command line split at NL characters.
	    അവസ്ഥ ISN_EXEC_SPLIT:
		{
		    source_cookie_T cookie;
		    char_u	    *വരി;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    വരി = get_split_sourceline(0, &cookie, 0, 0);
		    എങ്കിൽ (do_cmdline(വരി,
				get_split_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(വരി);
			പോകുക on_error;
		    }
		    vim_free(വരി);
		}
		നിർത്തുക;

	    // execute Ex command line that is only a range
	    അവസ്ഥ ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    അക്ഷരം	*error = ശൂന്യ;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = CMD_SIZE;
		    ea.addr_type = ADDR_വരികൾ;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_address(&ea, &error, FALSE);
		    എങ്കിൽ (ea.cmd == ശൂന്യ)
			പോകുക on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    എങ്കിൽ (error != ശൂന്യ)
		    {
			emsg(error);
			പോകുക on_error;
		    }
		}
		നിർത്തുക;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    അവസ്ഥ ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    പൂർണ്ണസംഖ്യ	    res;
		    പൂർണ്ണസംഖ്യ	    save_flags = cmdmod.cmod_flags;

		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    tv = അട്ടി_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, ശൂന്യ, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    എങ്കിൽ (res == FAIL)
			പോകുക on_error;
		    ++ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    // push typeval VAR_INSTR with instructions to be executed
	    അവസ്ഥ ISN_INSTR:
		{
		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    tv = അട്ടി_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    എങ്കിൽ (tv->vval.v_instr == ശൂന്യ)
			പോകുക on_error;
		    ++ectx->ec_അട്ടി.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_SOURCE:
		{
		    പൂർണ്ണസംഖ്യ notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (may_load_script((പൂർണ്ണസംഖ്യ)iptr->isn_arg.സംഖ്യ, &notused)
								       == FAIL)
			പോകുക on_error;
		}
		നിർത്തുക;

	    // execute :substitute with an expression
	    അവസ്ഥ ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    source_cookie_T	cookie;
		    ഘടന subs_വ്യഞ്ജകം_S	*save_instr = substitute_instr;
		    ഘടന subs_വ്യഞ്ജകം_S	subs_instr;
		    പൂർണ്ണസംഖ്യ			res;

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

		    എങ്കിൽ (res == FAIL || did_emsg
					     || subs_instr.subs_status == FAIL)
			പോകുക on_error;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_FINISH:
		പോകുക done;

	    അവസ്ഥ ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		എങ്കിൽ (alloc_redir_lval() == FAIL)
		    പോകുക on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		നിർത്തുക;

	    അവസ്ഥ ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    {
			vim_free(res);
			പോകുക theend;
		    }
		    tv = അട്ടി_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		force_abort = TRUE;
		എങ്കിൽ (trigger_cexpr_autocmd(iptr->isn_arg.സംഖ്യ) == FAIL)
		    പോകുക on_error;
		force_abort = FALSE;
#endif
		നിർത്തുക;

	    അവസ്ഥ ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    exarg_T ea;
		    പൂർണ്ണസംഖ്യ	    res;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_അട്ടി.ga_len;
		    tv = അട്ടി_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    എങ്കിൽ (res == FAIL)
			പോകുക on_error;
		}
#endif
		നിർത്തുക;

	    // execute Ex command from pieces on the stack
	    അവസ്ഥ ISN_EXECCONCAT:
		{
		    പൂർണ്ണസംഖ്യ	    എണ്ണം = iptr->isn_arg.സംഖ്യ;
		    size_t  len = 0;
		    പൂർണ്ണസംഖ്യ	    pass;
		    പൂർണ്ണസംഖ്യ	    i;
		    char_u  *cmd = ശൂന്യ;
		    char_u  *str;

		    വേണ്ടി (pass = 1; pass <= 2; ++pass)
		    {
			വേണ്ടി (i = 0; i < എണ്ണം; ++i)
			{
			    tv = അട്ടി_TV_BOT(i - എണ്ണം);
			    str = tv->vval.v_string;
			    എങ്കിൽ (str != ശൂന്യ && *str != NUL)
			    {
				എങ്കിൽ (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    എങ്കിൽ (pass == 2)
				clear_tv(tv);
			}
			എങ്കിൽ (pass == 1)
			{
			    cmd = alloc(len + 1);
			    എങ്കിൽ (unlikely(cmd == ശൂന്യ))
				പോകുക theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		നിർത്തുക;

	    // execute :echo {string} ...
	    അവസ്ഥ ISN_ECHO:
		{
		    പൂർണ്ണസംഖ്യ എണ്ണം = iptr->isn_arg.echo.echo_എണ്ണം;
		    പൂർണ്ണസംഖ്യ	atstart = TRUE;
		    പൂർണ്ണസംഖ്യ needclr = TRUE;
		    പൂർണ്ണസംഖ്യ	idx;

		    വേണ്ടി (idx = 0; idx < എണ്ണം; ++idx)
		    {
			tv = അട്ടി_TV_BOT(idx - എണ്ണം);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    എങ്കിൽ (needclr)
			സന്ദേശം_clr_eos();
		    ectx->ec_അട്ടി.ga_len -= എണ്ണം;
		}
		നിർത്തുക;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    അവസ്ഥ ISN_EXECUTE:
	    അവസ്ഥ ISN_ECHOMSG:
	    അവസ്ഥ ISN_ECHOWINDOW:
	    അവസ്ഥ ISN_ECHOCONSOLE:
	    അവസ്ഥ ISN_ECHOERR:
		{
		    പൂർണ്ണസംഖ്യ		എണ്ണം;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    പൂർണ്ണസംഖ്യ		len;
		    പൂർണ്ണസംഖ്യ		failed = FALSE;
		    പൂർണ്ണസംഖ്യ		idx;

		    എങ്കിൽ (iptr->isn_type == ISN_ECHOWINDOW)
			എണ്ണം = iptr->isn_arg.echowin.ewin_എണ്ണം;
		    അല്ലെങ്കിൽ
			എണ്ണം = iptr->isn_arg.സംഖ്യ;
		    ga_init2(&ga, 1, 80);
		    വേണ്ടി (idx = 0; idx < എണ്ണം; ++idx)
		    {
			tv = അട്ടി_TV_BOT(idx - എണ്ണം);
			എങ്കിൽ (iptr->isn_type == ISN_EXECUTE)
			{
			    എങ്കിൽ (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_JOB)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_മൂല്യം_as_string_str),
						    vartype_name(tv->v_type));
				നിർത്തുക;
			    }
			    അല്ലെങ്കിൽ
				p = tv_get_string_buf(tv, buf);
			}
			അല്ലെങ്കിൽ
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (പൂർണ്ണസംഖ്യ)STRLEN(p);
			എങ്കിൽ (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			അല്ലെങ്കിൽ
			{
			    എങ്കിൽ (ga.ga_len > 0)
				((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_അട്ടി.ga_len -= എണ്ണം;
		    എങ്കിൽ (failed)
		    {
			ga_clear(&ga);
			പോകുക on_error;
		    }

		    എങ്കിൽ (ga.ga_data != ശൂന്യ)
		    {
			എങ്കിൽ (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_data);
			    എങ്കിൽ (did_emsg)
			    {
				ga_clear(&ga);
				പോകുക on_error;
			    }
			}
			അല്ലെങ്കിൽ
			{
			    സന്ദേശം_sb_eol();
			    എങ്കിൽ (iptr->isn_type == ISN_ECHOMSG)
			    {
				സന്ദേശം_attr(ga.ga_data, echo_attr);
				out_flush();
			    }
#ifdef HAS_MESSAGE_WINDOW
			    അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				start_echowindow(
					      iptr->isn_arg.echowin.ewin_time);
				സന്ദേശം_attr(ga.ga_data, echo_attr);
				end_echowindow();
			    }
#endif
			    അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_data, (പൂർണ്ണസംഖ്യ)STRLEN(ga.ga_data),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    അല്ലെങ്കിൽ
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_data);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		നിർത്തുക;

	    // load local variable or argument
	    അവസ്ഥ ISN_LOAD:
		എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    പോകുക theend;
		tv = അട്ടി_TV_VAR(iptr->isn_arg.സംഖ്യ);
		എങ്കിൽ (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    അട്ടി_TV_BOT(0)->v_type = VAR_SPECIAL;
		    അട്ടി_TV_BOT(0)->vval.v_സംഖ്യ = VVAL_NONE;
		}
		അല്ലെങ്കിൽ
		    copy_tv(tv, അട്ടി_TV_BOT(0));
		++ectx->ec_അട്ടി.ga_len;
		നിർത്തുക;

	    // load v: variable
	    അവസ്ഥ ISN_LOADV:
		എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    പോകുക theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.സംഖ്യ), അട്ടി_TV_BOT(0));
		++ectx->ec_അട്ടി.ga_len;
		നിർത്തുക;

	    // load s: variable in Vim9 script
	    അവസ്ഥ ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    എങ്കിൽ (sv == ശൂന്യ)
			പോകുക theend;
		    allocate_if_null(sv);
		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    copy_tv(sv->sv_tv, അട്ടി_TV_BOT(0));
		    ++ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    // load s: variable in old script or autoload import
	    അവസ്ഥ ISN_LOADS:
	    അവസ്ഥ ISN_LOADEXPORT:
		{
		    പൂർണ്ണസംഖ്യ		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = find_var_in_ht(ht, 0, name, TRUE);

		    എങ്കിൽ (di == ശൂന്യ)
		    {
			ufunc_T	*ufunc = ശൂന്യ;

			എങ്കിൽ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (ശൂന്യം)find_exported(sid, name, &ufunc, &type,
							 ശൂന്യ, ശൂന്യ, FALSE);
			}
			എങ്കിൽ (ufunc == ശൂന്യ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    പോകുക on_error;
			}
			എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			    പോകുക theend;
			tv = അട്ടി_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_അട്ടി.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    പൂർണ്ണസംഖ്യ idx = get_script_item_idx(sid, name, 0,
								   ശൂന്യ, ശൂന്യ);
			    svar_T	*sv;

			    എങ്കിൽ (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;
				എങ്കിൽ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    പോകുക on_error;
				}
			    }
			}
			എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			    പോകുക theend;
			copy_tv(&di->di_tv, അട്ടി_TV_BOT(0));
			++ectx->ec_അട്ടി.ga_len;
		    }
		}
		നിർത്തുക;

	    // load g:/b:/w:/t: variable
	    അവസ്ഥ ISN_LOADG:
	    അവസ്ഥ ISN_LOADB:
	    അവസ്ഥ ISN_LOADW:
	    അവസ്ഥ ISN_LOADT:
		{
		    പൂർണ്ണസംഖ്യ res = load_namespace_var(ectx, iptr->isn_type, iptr);

		    എങ്കിൽ (res == NOTDONE)
			പോകുക theend;
		    എങ്കിൽ (res == FAIL)
			പോകുക on_error;
		}

		നിർത്തുക;

	    // load autoload variable
	    അവസ്ഥ ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (eval_variable(name, (പൂർണ്ണസംഖ്യ)STRLEN(name), 0,
			      അട്ടി_TV_BOT(0), ശൂന്യ, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = അട്ടി_TV_BOT(0);
			tv->v_type = VAR_സംഖ്യ;
			tv->v_lock = 0;
			tv->vval.v_സംഖ്യ = 0;
			++ectx->ec_അട്ടി.ga_len;
			പോകുക on_error;
		    }
		    ++ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    // load g:/b:/w:/t: namespace
	    അവസ്ഥ ISN_LOADGDICT:
	    അവസ്ഥ ISN_LOADBDICT:
	    അവസ്ഥ ISN_LOADWDICT:
	    അവസ്ഥ ISN_LOADTDICT:
		{
		    dict_T *d = ശൂന്യ;

		    തിരഞ്ഞെടുപ്പ് (iptr->isn_type)
		    {
			അവസ്ഥ ISN_LOADGDICT: d = get_globvar_dict(); നിർത്തുക;
			അവസ്ഥ ISN_LOADBDICT: d = curbuf->b_vars; നിർത്തുക;
			അവസ്ഥ ISN_LOADWDICT: d = curwin->w_vars; നിർത്തുക;
			അവസ്ഥ ISN_LOADTDICT: d = curtab->tp_vars; നിർത്തുക;
			സ്ഥിരസ്ഥിതി:  // Cannot reach here
			    പോകുക theend;
		    }
		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    tv = അട്ടി_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_lock = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    // load &option
	    അവസ്ഥ ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    എങ്കിൽ (eval_option(&name, &optval, TRUE) == FAIL)
			പോകുക theend;
		    *അട്ടി_TV_BOT(0) = optval;
		    ++ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    // load $ENV
	    അവസ്ഥ ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    // name is always valid, checked when compiling
		    (ശൂന്യം)eval_env_var(&name, &optval, TRUE);
		    *അട്ടി_TV_BOT(0) = optval;
		    ++ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    // load @register
	    അവസ്ഥ ISN_LOADREG:
		എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    പോകുക theend;
		tv = അട്ടി_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.സംഖ്യ, GREG_വ്യഞ്ജകം_SRC);
		++ectx->ec_അട്ടി.ga_len;
		നിർത്തുക;

	    // store local variable
	    അവസ്ഥ ISN_STORE:
		--ectx->ec_അട്ടി.ga_len;
		tv = അട്ടി_TV_VAR(iptr->isn_arg.സംഖ്യ);
		എങ്കിൽ (check_typval_is_മൂല്യം(അട്ടി_TV_BOT(0)) == FAIL
			|| അട്ടി_TV_BOT(0)->v_type == VAR_VOID)
		{
		    എങ്കിൽ (അട്ടി_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_മൂല്യം));
		    clear_tv(അട്ടി_TV_BOT(0));
		    പോകുക on_error;
		}
		clear_tv(tv);
		*tv = *അട്ടി_TV_BOT(0);
		നിർത്തുക;

	    // store s: variable in old script or autoload import
	    അവസ്ഥ ISN_STORES:
	    അവസ്ഥ ISN_STOREEXPORT:
		{
		    പൂർണ്ണസംഖ്യ		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = ശൂന്യ;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    എങ്കിൽ (iptr->isn_type == ISN_STOREEXPORT)
			di = find_var_autoload_prefix(name, sid, ശൂന്യ, ശൂന്യ);
		    // Then look for a variable in the script's variables.
		    എങ്കിൽ (di == ശൂന്യ)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = find_var_in_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_അട്ടി.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (di == ശൂന്യ)
		    {
			എങ്കിൽ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(അട്ടി_TV_BOT(0));
			    പോകുക on_error;
			}
			store_var(name, അട്ടി_TV_BOT(0));
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    പൂർണ്ണസംഖ്യ idx = get_script_item_idx(sid, name, 0,
								   ശൂന്യ, ശൂന്യ);

			    // can this ever fail?
			    എങ്കിൽ (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;

				എങ്കിൽ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    clear_tv(അട്ടി_TV_BOT(0));
				    പോകുക on_error;
				}
			    }
			}
			എങ്കിൽ (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(അട്ടി_TV_BOT(0));
			    പോകുക on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *അട്ടി_TV_BOT(0);
		    }
		}
		നിർത്തുക;

	    // store script-local variable in Vim9 script
	    അവസ്ഥ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    എങ്കിൽ (sv == ശൂന്യ)
			പോകുക theend;
		    --ectx->ec_അട്ടി.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (മൂല്യം_check_lock(sv->sv_tv->v_lock, sv->sv_name, FALSE))
		    {
			clear_tv(അട്ടി_TV_BOT(0));
			പോകുക on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *അട്ടി_TV_BOT(0);
		}
		നിർത്തുക;

	    // store option
	    അവസ്ഥ ISN_STOREOPT:
	    അവസ്ഥ ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    പൂർണ്ണസംഖ്യ		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    long	n = 0;
		    char_u	*s = ശൂന്യ;
		    അക്ഷരം	*സന്ദേശം;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = ശൂന്യ;

		    --ectx->ec_അട്ടി.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = അട്ടി_TV_BOT(0);
		    എങ്കിൽ (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			എങ്കിൽ (s == ശൂന്യ)
			    s = (char_u *)"";
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			എങ്കിൽ (s == ശൂന്യ || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    പോകുക on_error;
			}
		    }
		    അല്ലെങ്കിൽ
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_സംഖ്യ;
		    സന്ദേശം = set_option_മൂല്യം(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    എങ്കിൽ (സന്ദേശം != ശൂന്യ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(സന്ദേശം));
			പോകുക on_error;
		    }
		}
		നിർത്തുക;

	    // store $ENV
	    അവസ്ഥ ISN_STOREENV:
		എങ്കിൽ (check_secure() || check_restricted())
		    പോകുക theend;
		--ectx->ec_അട്ടി.ga_len;
		tv = അട്ടി_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		നിർത്തുക;

	    // store @r
	    അവസ്ഥ ISN_STOREREG:
		{
		    പൂർണ്ണസംഖ്യ	reg = iptr->isn_arg.സംഖ്യ;

		    --ectx->ec_അട്ടി.ga_len;
		    tv = അട്ടി_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		നിർത്തുക;

	    // store v: variable
	    അവസ്ഥ ISN_STOREV:
		--ectx->ec_അട്ടി.ga_len;
		എങ്കിൽ (set_vim_var_tv(iptr->isn_arg.സംഖ്യ, അട്ടി_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    പോകുക on_error;
		നിർത്തുക;

	    // store g:/b:/w:/t: variable
	    അവസ്ഥ ISN_STOREG:
	    അവസ്ഥ ISN_STOREB:
	    അവസ്ഥ ISN_STOREW:
	    അവസ്ഥ ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    തിരഞ്ഞെടുപ്പ് (iptr->isn_type)
		    {
			അവസ്ഥ ISN_STOREG:
			    ht = get_globvar_ht();
			    നിർത്തുക;
			അവസ്ഥ ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    നിർത്തുക;
			അവസ്ഥ ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    നിർത്തുക;
			അവസ്ഥ ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    നിർത്തുക;
			സ്ഥിരസ്ഥിതി:  // Cannot reach here
			    പോകുക theend;
		    }

		    --ectx->ec_അട്ടി.ga_len;
		    di = find_var_in_ht(ht, 0, name, TRUE);
		    എങ്കിൽ (di == ശൂന്യ)
			store_var(iptr->isn_arg.string, അട്ടി_TV_BOT(0));
		    അല്ലെങ്കിൽ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			എങ്കിൽ (var_check_permission(di, name) == FAIL)
			    പോകുക on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *അട്ടി_TV_BOT(0);
		    }
		}
		നിർത്തുക;

	    // store an autoload variable
	    അവസ്ഥ ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		set_var(iptr->isn_arg.string, അട്ടി_TV_BOT(-1), TRUE);
		clear_tv(അട്ടി_TV_BOT(-1));
		--ectx->ec_അട്ടി.ga_len;
		നിർത്തുക;

	    // store number in local variable
	    അവസ്ഥ ISN_STORENR:
		tv = അട്ടി_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_സംഖ്യ;
		tv->vval.v_സംഖ്യ = iptr->isn_arg.storenr.stnr_val;
		നിർത്തുക;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    അവസ്ഥ ISN_STOREINDEX:
		{
		    പൂർണ്ണസംഖ്യ res = execute_storeindex(iptr, ectx);

		    എങ്കിൽ (res == FAIL)
			പോകുക on_error;
		    എങ്കിൽ (res == NOTDONE)
			പോകുക theend;
		}
		നിർത്തുക;

	    // store value in list or blob range
	    അവസ്ഥ ISN_STORERANGE:
		എങ്കിൽ (execute_storerange(iptr, ectx) == FAIL)
		    പോകുക on_error;
		നിർത്തുക;

	    അവസ്ഥ ISN_LOAD_CLASSMEMBER:
		{
		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      അട്ടി_TV_BOT(0));
		    ++ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *അട്ടി_TV_BOT(-1);
		    --ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    // Load or store variable or argument from outer scope.
	    അവസ്ഥ ISN_LOADOUTER:
	    അവസ്ഥ ISN_STOREOUTER:
		{
		    പൂർണ്ണസംഖ്യ		depth = iptr->isn_arg.outer.outer_depth;
		    outer_T	*outer = ectx->ec_outer_ref == ശൂന്യ ? ശൂന്യ
						: ectx->ec_outer_ref->or_outer;

		    ആകുമ്പോൾ (depth > 1 && outer != ശൂന്യ)
		    {
			outer = outer->out_up;
			--depth;
		    }
		    എങ്കിൽ (outer == ശൂന്യ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			എങ്കിൽ (ectx->ec_frame_idx == ectx->ec_initial_frame_idx
						 || ectx->ec_outer_ref == ശൂന്യ)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_closure_called_from_invalid_context));
			അല്ലെങ്കിൽ
			    iemsg("LOADOUTER depth more than scope levels");
			പോകുക theend;
		    }
		    എങ്കിൽ (depth < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->out_loop[-depth - 1]
							       .അട്ടി->ga_data)
					  + outer->out_loop[-depth - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    അല്ലെങ്കിൽ
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->out_അട്ടി->ga_data)
				      + outer->out_frame_idx + അട്ടി_FRAME_SIZE
				      + iptr->isn_arg.outer.outer_idx;
		    എങ്കിൽ (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			    പോകുക theend;
			// careful: ga_grow_inner may re-alloc the stack
			എങ്കിൽ (depth < 0)
			    copy = ((typval_T *)outer->out_loop[-depth - 1]
								   .അട്ടി->ga_data)
					      + outer->out_loop[-depth - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			അല്ലെങ്കിൽ
			    copy = ((typval_T *)outer->out_അട്ടി->ga_data)
					  + outer->out_frame_idx + അട്ടി_FRAME_SIZE
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			എങ്കിൽ (copy != tv)
			    tv = copy;
			copy_tv(tv, അട്ടി_TV_BOT(0));
			++ectx->ec_അട്ടി.ga_len;
		    }
		    അല്ലെങ്കിൽ
		    {
			--ectx->ec_അട്ടി.ga_len;
			clear_tv(tv);
			*tv = *അട്ടി_TV_BOT(0);
		    }
		}
		നിർത്തുക;

	    // unlet item in list or dict variable
	    അവസ്ഥ ISN_UNLETINDEX:
		എങ്കിൽ (execute_unletindex(iptr, ectx) == FAIL)
		    പോകുക on_error;
		നിർത്തുക;

	    // unlet range of items in list variable
	    അവസ്ഥ ISN_UNLETRANGE:
		എങ്കിൽ (execute_unletrange(iptr, ectx) == FAIL)
		    പോകുക on_error;
		നിർത്തുക;

	    // push constant
	    അവസ്ഥ ISN_PUSHNR:
	    അവസ്ഥ ISN_PUSHBOOL:
	    അവസ്ഥ ISN_PUSHSPEC:
	    അവസ്ഥ ISN_PUSHF:
	    അവസ്ഥ ISN_PUSHS:
	    അവസ്ഥ ISN_PUSHBLOB:
	    അവസ്ഥ ISN_PUSHFUNC:
	    അവസ്ഥ ISN_PUSHCHANNEL:
	    അവസ്ഥ ISN_PUSHJOB:
	    അവസ്ഥ ISN_PUSHOBJ:
	    അവസ്ഥ ISN_PUSHCLASS:
		എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    പോകുക theend;
		tv = അട്ടി_TV_BOT(0);
		tv->v_lock = 0;
		++ectx->ec_അട്ടി.ga_len;
		തിരഞ്ഞെടുപ്പ് (iptr->isn_type)
		{
		    അവസ്ഥ ISN_PUSHNR:
			tv->v_type = VAR_സംഖ്യ;
			tv->vval.v_സംഖ്യ = iptr->isn_arg.സംഖ്യ;
			നിർത്തുക;
		    അവസ്ഥ ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_സംഖ്യ = iptr->isn_arg.സംഖ്യ;
			നിർത്തുക;
		    അവസ്ഥ ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_സംഖ്യ = iptr->isn_arg.സംഖ്യ;
			നിർത്തുക;
		    അവസ്ഥ ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			നിർത്തുക;
		    അവസ്ഥ ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			നിർത്തുക;
		    അവസ്ഥ ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			എങ്കിൽ (iptr->isn_arg.string == ശൂന്യ)
			    tv->vval.v_string = ശൂന്യ;
			അല്ലെങ്കിൽ
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			നിർത്തുക;
		    അവസ്ഥ ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = ശൂന്യ;
#endif
			നിർത്തുക;
		    അവസ്ഥ ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_JOB;
			tv->vval.v_job = ശൂന്യ;
#endif
			നിർത്തുക;
		    അവസ്ഥ ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = ശൂന്യ;
			നിർത്തുക;
		    അവസ്ഥ ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			നിർത്തുക;
		    സ്ഥിരസ്ഥിതി:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == ശൂന്യ
				    ? ശൂന്യ : vim_strsave(iptr->isn_arg.string);
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (ശൂന്യം)script_autoload(name, FALSE);
		    എങ്കിൽ (find_func(name, TRUE))
		    {
			എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			    പോകുക theend;
			tv = അട്ടി_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_അട്ടി.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    അല്ലെങ്കിൽ
		    {
			പൂർണ്ണസംഖ്യ res = load_namespace_var(ectx, ISN_LOADG, iptr);

			എങ്കിൽ (res == NOTDONE)
			    പോകുക theend;
			എങ്കിൽ (res == FAIL)
			    പോകുക on_error;
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_UNLET:
		എങ്കിൽ (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    പോകുക on_error;
		നിർത്തുക;
	    അവസ്ഥ ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		നിർത്തുക;

	    അവസ്ഥ ISN_LOCKUNLOCK:
		{
#ifdef LOG_LOCKVAR
		    ch_log(ശൂന്യ, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#endif
		    lval_വേര്_T	*lval_വേര്_save = lval_വേര്;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_അട്ടി.ga_len;
		    lval_വേര്_T വേര്;
		    വേര്.lr_tv      = അട്ടി_TV_BOT(0);
		    വേര്.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    വേര്.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_വേര് = &വേര്;
		    പൂർണ്ണസംഖ്യ res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(വേര്.lr_tv);
		    lval_വേര് = lval_വേര്_save;
		    എങ്കിൽ (res == FAIL)
			പോകുക on_error;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_LOCKCONST:
		item_lock(അട്ടി_TV_BOT(-1), 100, TRUE, TRUE);
		നിർത്തുക;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    അവസ്ഥ ISN_NEWLIST:
		എങ്കിൽ (exe_newlist(iptr->isn_arg.സംഖ്യ, ectx) == FAIL)
		    പോകുക theend;
		നിർത്തുക;

	    // create a tuple from items on the stack
	    അവസ്ഥ ISN_NEWTUPLE:
		എങ്കിൽ (exe_newtuple(iptr->isn_arg.സംഖ്യ, ectx) == FAIL)
		    പോകുക theend;
		നിർത്തുക;

	    // create a dict from items on the stack
	    അവസ്ഥ ISN_NEWDICT:
		{
		    പൂർണ്ണസംഖ്യ res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.സംഖ്യ, ectx);
		    എങ്കിൽ (res == FAIL)
			പോകുക theend;
		    എങ്കിൽ (res == MAYBE)
			പോകുക on_error;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_CONCAT:
		എങ്കിൽ (exe_concat(iptr->isn_arg.സംഖ്യ, ectx) == FAIL)
		    പോകുക theend;
		നിർത്തുക;

	    // create a partial with NULL value
	    അവസ്ഥ ISN_NEWPARTIAL:
		എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    പോകുക theend;
		++ectx->ec_അട്ടി.ga_len;
		tv = അട്ടി_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_lock = 0;
		tv->vval.v_partial = ശൂന്യ;
		നിർത്തുക;

	    // call a :def function
	    അവസ്ഥ ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		എങ്കിൽ (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				ശൂന്യ,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    പോകുക on_error;
		നിർത്തുക;

	    // call a method on an interface
	    അവസ്ഥ ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = അട്ടി_TV_BOT(-1 - mfunc->cmf_argcount);
		    എങ്കിൽ (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			പോകുക on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    എങ്കിൽ (obj == ശൂന്യ)
		    {
			emsg(_(e_using_null_object));
			പോകുക on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    എങ്കിൽ (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    അല്ലെങ്കിൽ
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			പൂർണ്ണസംഖ്യ idx = object_index_from_itf_index(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    എങ്കിൽ (mfunc->cmf_ufunc != ശൂന്യ && IS_GENERIC_FUNC(ufunc))
		    {
			എങ്കിൽ (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			അല്ലെങ്കിൽ
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    എങ്കിൽ (ufunc == ശൂന്യ)
				പോകുക on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    എങ്കിൽ (call_ufunc(ufunc, ശൂന്യ, mfunc->cmf_argcount, ectx,
							   ശൂന്യ, ശൂന്യ) == FAIL)
			പോകുക on_error;
		}
		നിർത്തുക;

	    // call a builtin function
	    അവസ്ഥ ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		എങ്കിൽ (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    പോകുക on_error;
		നിർത്തുക;

	    // call a funcref or partial
	    അവസ്ഥ ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    പൂർണ്ണസംഖ്യ		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (pfunc->cpf_മുകൾ)
		    {
			// funcref is above the arguments
			tv = അട്ടി_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    അല്ലെങ്കിൽ
		    {
			// Get the funcref from the stack.
			--ectx->ec_അട്ടി.ga_len;
			partial_tv = *അട്ടി_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    എങ്കിൽ (tv == &partial_tv)
			clear_tv(&partial_tv);
		    എങ്കിൽ (r == FAIL)
			പോകുക on_error;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_PCALL_END:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_അട്ടി.ga_len;
		clear_tv(അട്ടി_TV_BOT(-1));
		*അട്ടി_TV_BOT(-1) = *അട്ടി_TV_BOT(0);
		നിർത്തുക;

	    // call a user defined function or funcref/partial
	    അവസ്ഥ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			പോകുക on_error;
		}
		നിർത്തുക;

	    // :defer func(arg)
	    അവസ്ഥ ISN_DEFER:
		എങ്കിൽ (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    പോകുക on_error;
		നിർത്തുക;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    അവസ്ഥ ISN_RETURN_VOID:
	    അവസ്ഥ ISN_RETURN_OBJECT:
		എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    പോകുക theend;
		tv = അട്ടി_TV_BOT(0);
		++ectx->ec_അട്ടി.ga_len;
		എങ്കിൽ (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_സംഖ്യ = 0;
		    tv->v_lock = 0;
		}
		അല്ലെങ്കിൽ
		{
		    *tv = *അട്ടി_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_lock_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    അവസ്ഥ ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ശൂന്യ;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    എങ്കിൽ (check_typval_is_മൂല്യം(അട്ടി_TV_BOT(-1)) == FAIL)
			പോകുക theend;

		    എങ്കിൽ (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    എങ്കിൽ (trycmd != ശൂന്യ
				 && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    {
			// jump to ":finally" or ":endtry"
			എങ്കിൽ (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			അല്ലെങ്കിൽ
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    അല്ലെങ്കിൽ
			പോകുക func_return;
		}
		നിർത്തുക;

	    // push a partial, a reference to a compiled function
	    അവസ്ഥ ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    എങ്കിൽ (pt == ശൂന്യ)
			പോകുക theend;
		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    {
			vim_free(pt);
			പോകുക theend;
		    }
		    എങ്കിൽ (extra != ശൂന്യ && extra->fre_class != ശൂന്യ)
		    {
			class_T	*cl;
			എങ്കിൽ (extra->fre_object_method)
			{
			    tv = അട്ടി_TV_BOT(-1);
			    എങ്കിൽ (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				പോകുക on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    എങ്കിൽ (obj == ശൂന്യ)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				പോകുക on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_അട്ടി.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			അല്ലെങ്കിൽ
			    cl = extra->fre_class;

			എങ്കിൽ (extra->fre_object_method)
			{
			    // object method
			    // convert the interface index to the object index
			    പൂർണ്ണസംഖ്യ idx =
				object_index_from_itf_index(extra->fre_class,
					TRUE, extra->fre_method_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			അല്ലെങ്കിൽ
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_method_idx];
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (extra == ശൂന്യ || extra->fre_func_name == ശൂന്യ)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    അല്ലെങ്കിൽ
		    {
			ufunc = find_func(extra->fre_func_name, FALSE);
		    }
		    എങ്കിൽ (ufunc == ശൂന്യ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			പോകുക theend;
		    }
		    എങ്കിൽ (extra != ശൂന്യ && extra->fre_outer_depth > 0
					   && (ufunc->uf_flags & FC_CLOSURE))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == ശൂന്യ
				    ? ശൂന്യ : ectx->ec_outer_ref->or_partial;

			വേണ്ടി (പൂർണ്ണസംഖ്യ depth = 1; outer_pt != ശൂന്യ
				    && depth < extra->fre_outer_depth; ++depth)
			    outer_pt = get_pt_outer(outer_pt)->out_up_partial;
			എങ്കിൽ (outer_pt == ശൂന്യ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    പോകുക theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (fill_partial_and_closure(pt, ufunc,
			       extra == ശൂന്യ ? ശൂന്യ : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			പോകുക theend;
		    }
		    tv = അട്ടി_TV_BOT(0);
		    ++ectx->ec_അട്ടി.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_lock = 0;
		}
		നിർത്തുക;

	    // Create a global function from a lambda.
	    അവസ്ഥ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    എങ്കിൽ (copy_lambda_to_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			പോകുക theend;
		}
		നിർത്തുക;

	    // List functions
	    അവസ്ഥ ISN_DEF:
		എങ്കിൽ (iptr->isn_arg.string == ശൂന്യ)
		    list_functions(ശൂന്യ);
		അല്ലെങ്കിൽ
		{
		    exarg_T	ea;
		    garray_T	വരികൾ_to_free;

		    CLEAR_FIELD(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&വരികൾ_to_free, വലുപ്പം(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, ശൂന്യ, &വരികൾ_to_free, 0, ശൂന്യ, 0, ശൂന്യ);
		    ga_clear_strings(&വരികൾ_to_free);
		}
		നിർത്തുക;

	    // jump if a condition is met
	    അവസ്ഥ ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    പൂർണ്ണസംഖ്യ		error = FALSE;
		    പൂർണ്ണസംഖ്യ		jump = TRUE;

		    എങ്കിൽ (when != JUMP_ALWAYS)
		    {
			tv = അട്ടി_TV_BOT(-1);
			എങ്കിൽ (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    എങ്കിൽ (error)
				പോകുക on_error;
			}
			അല്ലെങ്കിൽ
			    jump = tv2bool(tv);
			എങ്കിൽ (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			എങ്കിൽ (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_അട്ടി.ga_len;
			}
		    }
		    എങ്കിൽ (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		നിർത്തുക;

	    // "while": jump to end if a condition is false
	    അവസ്ഥ ISN_WHILE:
		{
		    പൂർണ്ണസംഖ്യ		error = FALSE;
		    പൂർണ്ണസംഖ്യ		jump = TRUE;

		    tv = അട്ടി_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    എങ്കിൽ (error)
			പോകുക on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_അട്ടി.ga_len;
		    എങ്കിൽ (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_end;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = അട്ടി_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_സംഖ്യ = ectx->ec_funcrefs.ga_len;
		}
		നിർത്തുക;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    അവസ്ഥ ISN_JUMP_IF_ARG_SET:
	    അവസ്ഥ ISN_JUMP_IF_ARG_NOT_SET:
		tv = അട്ടി_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		പൂർണ്ണസംഖ്യ arg_set = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_സംഖ്യ == VVAL_NONE);

		എങ്കിൽ (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_SET && !arg_set)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_data)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_args.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ അട്ടി_FRAME_SIZE;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_POINTER(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    എങ്കിൽ (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		എങ്കിൽ (iptr->isn_type == ISN_JUMP_IF_ARG_SET ? arg_set : !arg_set)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		നിർത്തുക;

	    // top of a for loop
	    അവസ്ഥ ISN_FOR:
		എങ്കിൽ (execute_for(iptr, ectx) == FAIL)
		    പോകുക theend;
		നിർത്തുക;

	    // end of a for or while loop
	    അവസ്ഥ ISN_ENDLOOP:
		എങ്കിൽ (execute_endloop(iptr, ectx) == FAIL)
		    പോകുക theend;
		നിർത്തുക;

	    // start of ":try" block
	    അവസ്ഥ ISN_TRY:
		{
		    trycmd_T    *trycmd = ശൂന്യ;

		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			പോകുക theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_data)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_POINTER(trycmd);
		    trycmd->tcd_frame_idx = ectx->ec_frame_idx;
		    trycmd->tcd_അട്ടി_len = ectx->ec_അട്ടി.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_PUSHEXC:
		എങ്കിൽ (current_exception == ശൂന്യ)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    പോകുക theend;
		}
		എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
		    പോകുക theend;
		tv = അട്ടി_TV_BOT(0);
		++ectx->ec_അട്ടി.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->മൂല്യം);
		നിർത്തുക;

	    അവസ്ഥ ISN_CATCH:
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
		നിർത്തുക;

	    അവസ്ഥ ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    പൂർണ്ണസംഖ്യ		i;
		    trycmd_T    *trycmd;
		    പൂർണ്ണസംഖ്യ		iidx = trycont->tct_where;

		    എങ്കിൽ (trystack->ga_len < trycont->tct_levels)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_levels, trystack->ga_len);
			പോകുക theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    വേണ്ടി (i = trycont->tct_levels; i > 0; --i)
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
		നിർത്തുക;

	    അവസ്ഥ ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    എങ്കിൽ (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_അട്ടി);
			trycmd->tcd_caught = FALSE;
		    }
		    നിർത്തുക;
		}

	    // end of ":try" block
	    അവസ്ഥ ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_data) + trystack->ga_len;
		    എങ്കിൽ (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    എങ്കിൽ (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_അട്ടി);
		    }

		    എങ്കിൽ (trycmd->tcd_return)
			പോകുക func_return;

		    ആകുമ്പോൾ (ectx->ec_അട്ടി.ga_len > trycmd->tcd_അട്ടി_len)
		    {
			--ectx->ec_അട്ടി.ga_len;
			clear_tv(അട്ടി_TV_BOT(0));
		    }
		    എങ്കിൽ (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    എങ്കിൽ (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = അട്ടി_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_സംഖ്യ;
			tv->vval.v_സംഖ്യ = 0;
			പോകുക done;
		    }
		    --ectx->ec_അട്ടി.ga_len;
		    tv = അട്ടി_TV_BOT(0);
		    എങ്കിൽ (tv->vval.v_string == ശൂന്യ
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_empty_string));
			പോകുക theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    എങ്കിൽ (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
			എങ്കിൽ (trycmd->tcd_caught)
			{
			    // discard the exception
			    finish_exception(caught_അട്ടി);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (throw_exception(tv->vval.v_string, ET_USER, ശൂന്യ)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			പോകുക theend;
		    }
		    did_throw = TRUE;
		}
		നിർത്തുക;

	    // compare with special values
	    അവസ്ഥ ISN_COMPAREBOOL:
	    അവസ്ഥ ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = അട്ടി_TV_BOT(-2);
		    typval_T	*tv2 = അട്ടി_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_സംഖ്യ;
		    varnumber_T arg2 = tv2->vval.v_സംഖ്യ;
		    പൂർണ്ണസംഖ്യ		res;

		    എങ്കിൽ (iptr->isn_arg.op.op_type == വ്യഞ്ജകം_EQUAL)
			res = arg1 == arg2;
		    അല്ലെങ്കിൽ
			res = arg1 != arg2;

		    --ectx->ec_അട്ടി.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_സംഖ്യ = res ? VVAL_TRUE : VVAL_FALSE;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_COMPARENULL:
		{
		    typval_T	*tv1 = അട്ടി_TV_BOT(-2);
		    typval_T	*tv2 = അട്ടി_TV_BOT(-1);
		    പൂർണ്ണസംഖ്യ		res;

		    res = typval_compare_null(tv1, tv2);
		    എങ്കിൽ (res == MAYBE)
			പോകുക on_error;
		    എങ്കിൽ (iptr->isn_arg.op.op_type == വ്യഞ്ജകം_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_അട്ടി.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_സംഖ്യ = res ? VVAL_TRUE : VVAL_FALSE;
		}
		നിർത്തുക;

	    // Operation with two number arguments
	    അവസ്ഥ ISN_OPNR:
	    അവസ്ഥ ISN_COMPARENR:
		{
		    typval_T	*tv1 = അട്ടി_TV_BOT(-2);
		    typval_T	*tv2 = അട്ടി_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_സംഖ്യ;
		    varnumber_T arg2 = tv2->vval.v_സംഖ്യ;
		    varnumber_T res = 0;
		    പൂർണ്ണസംഖ്യ		div_zero = FALSE;

		    എങ്കിൽ (iptr->isn_arg.op.op_type == വ്യഞ്ജകം_LSHIFT
			    || iptr->isn_arg.op.op_type == വ്യഞ്ജകം_RSHIFT)
		    {
			എങ്കിൽ (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_positive));
			    പോകുക on_error;
			}
		    }

		    തിരഞ്ഞെടുപ്പ് (iptr->isn_arg.op.op_type)
		    {
			അവസ്ഥ വ്യഞ്ജകം_MULT: res = arg1 * arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_DIV:  എങ്കിൽ (arg2 == 0)
					    div_zero = TRUE;
					അല്ലെങ്കിൽ
					    res = arg1 / arg2;
					നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_REM:  എങ്കിൽ (arg2 == 0)
					    div_zero = TRUE;
					അല്ലെങ്കിൽ
					    res = arg1 % arg2;
					നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_SUB: res = arg1 - arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_ADD: res = arg1 + arg2; നിർത്തുക;

			അവസ്ഥ വ്യഞ്ജകം_EQUAL: res = arg1 == arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_NEQUAL: res = arg1 != arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_GREATER: res = arg1 > arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_GEQUAL: res = arg1 >= arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_SMALLER: res = arg1 < arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_SEQUAL: res = arg1 <= arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_LSHIFT: എങ്കിൽ (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  അല്ലെങ്കിൽ
					      res = (uvarnumber_T)arg1 << arg2;
					  നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_RSHIFT: എങ്കിൽ (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  അല്ലെങ്കിൽ
					      res = (uvarnumber_T)arg1 >> arg2;
					  നിർത്തുക;
			സ്ഥിരസ്ഥിതി: നിർത്തുക;
		    }

		    --ectx->ec_അട്ടി.ga_len;
		    എങ്കിൽ (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_സംഖ്യ = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    അല്ലെങ്കിൽ
			tv1->vval.v_സംഖ്യ = res;
		    എങ്കിൽ (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			പോകുക on_error;
		    }
		}
		നിർത്തുക;

	    // Computation with two float arguments
	    അവസ്ഥ ISN_OPFLOAT:
	    അവസ്ഥ ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = അട്ടി_TV_BOT(-2);
		    typval_T	*tv2 = അട്ടി_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    പൂർണ്ണസംഖ്യ		cmp = FALSE;

		    തിരഞ്ഞെടുപ്പ് (iptr->isn_arg.op.op_type)
		    {
			അവസ്ഥ വ്യഞ്ജകം_MULT: res = arg1 * arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_DIV: res = arg1 / arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_SUB: res = arg1 - arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_ADD: res = arg1 + arg2; നിർത്തുക;

			അവസ്ഥ വ്യഞ്ജകം_EQUAL: cmp = arg1 == arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_NEQUAL: cmp = arg1 != arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_GREATER: cmp = arg1 > arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_GEQUAL: cmp = arg1 >= arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_SMALLER: cmp = arg1 < arg2; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_SEQUAL: cmp = arg1 <= arg2; നിർത്തുക;
			സ്ഥിരസ്ഥിതി: cmp = 0; നിർത്തുക;
		    }
		    --ectx->ec_അട്ടി.ga_len;
		    എങ്കിൽ (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_സംഖ്യ = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    അല്ലെങ്കിൽ
			tv1->vval.v_float = res;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_COMPARELIST:
	    അവസ്ഥ ISN_COMPARETUPLE:
	    അവസ്ഥ ISN_COMPAREDICT:
	    അവസ്ഥ ISN_COMPAREFUNC:
	    അവസ്ഥ ISN_COMPARESTRING:
	    അവസ്ഥ ISN_COMPAREBLOB:
	    അവസ്ഥ ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = അട്ടി_TV_BOT(-2);
		    typval_T	*tv2 = അട്ടി_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    പൂർണ്ണസംഖ്യ		ic = iptr->isn_arg.op.op_ic;
		    പൂർണ്ണസംഖ്യ		res = FALSE;
		    പൂർണ്ണസംഖ്യ		status = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (iptr->isn_type == ISN_COMPARELIST)
		    {
			status = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			status = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_type == ISN_COMPAREDICT)
		    {
			status = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			status = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_type == ISN_COMPARESTRING)
		    {
			status = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			status = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    അല്ലെങ്കിൽ // ISN_COMPAREOBJECT
		    {
			status = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_അട്ടി.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_സംഖ്യ = res ? VVAL_TRUE : VVAL_FALSE;
		    എങ്കിൽ (status == FAIL)
			പോകുക theend;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_COMPAREANY:
		{
		    typval_T	*tv1 = അട്ടി_TV_BOT(-2);
		    typval_T	*tv2 = അട്ടി_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    പൂർണ്ണസംഖ്യ		ic = iptr->isn_arg.op.op_ic;
		    പൂർണ്ണസംഖ്യ		status;

		    SOURCING_LNUM = iptr->isn_lnum;
		    status = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_അട്ടി.ga_len;
		    എങ്കിൽ (status == FAIL)
			പോകുക theend;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_ADDLIST:
	    അവസ്ഥ ISN_ADDTUPLE:
	    അവസ്ഥ ISN_ADDBLOB:
		{
		    typval_T *tv1 = അട്ടി_TV_BOT(-2);
		    typval_T *tv2 = അട്ടി_TV_BOT(-1);

		    // add two lists or blobs
		    എങ്കിൽ (iptr->isn_type == ISN_ADDLIST)
		    {
			എങ്കിൽ (iptr->isn_arg.op.op_type == വ്യഞ്ജകം_APPEND
						   && tv1->vval.v_list != ശൂന്യ)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 ശൂന്യ);
			അല്ലെങ്കിൽ
			    eval_addlist(tv1, tv2);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    അല്ലെങ്കിൽ
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = അട്ടി_TV_BOT(-2);
		    typval_T	*tv2 = അട്ടി_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (l == ശൂന്യ)
		    {
			emsg(_(e_cannot_add_to_null_list));
			പോകുക on_error;
		    }
		    എങ്കിൽ (മൂല്യം_check_lock(l->lv_lock, ശൂന്യ, FALSE))
			പോകുക on_error;
		    എങ്കിൽ (list_append_tv(l, tv2) == FAIL)
			പോകുക theend;
		    clear_tv(tv2);
		    --ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = അട്ടി_TV_BOT(-2);
		    typval_T	*tv2 = അട്ടി_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    പൂർണ്ണസംഖ്യ		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    എങ്കിൽ (b == ശൂന്യ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_add_to_null_blob));
			പോകുക on_error;
		    }
		    n = tv_get_സംഖ്യ_chk(tv2, &error);
		    എങ്കിൽ (error)
			പോകുക on_error;
		    ga_append(&b->bv_ga, (പൂർണ്ണസംഖ്യ)n);
		    --ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    // Computation with two arguments of unknown type
	    അവസ്ഥ ISN_OPANY:
		{
		    typval_T	*tv1 = അട്ടി_TV_BOT(-2);
		    typval_T	*tv2 = അട്ടി_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    പൂർണ്ണസംഖ്യ		error = FALSE;

		    എങ്കിൽ (iptr->isn_arg.op.op_type == വ്യഞ്ജകം_ADD)
		    {
			എങ്കിൽ (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_അട്ടി.ga_len;
			    നിർത്തുക;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_അട്ടി.ga_len;
			    നിർത്തുക;
			}
			അല്ലെങ്കിൽ എങ്കിൽ (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_അട്ടി.ga_len;
			    നിർത്തുക;
			}
		    }
		    എങ്കിൽ (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    അല്ലെങ്കിൽ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_സംഖ്യ_chk(tv1, &error);
			എങ്കിൽ (error)
			    പോകുക on_error;
			എങ്കിൽ (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    എങ്കിൽ (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    അല്ലെങ്കിൽ
		    {
			n2 = tv_get_സംഖ്യ_chk(tv2, &error);
			എങ്കിൽ (error)
			    പോകുക on_error;
			എങ്കിൽ (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    എങ്കിൽ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			തിരഞ്ഞെടുപ്പ് (iptr->isn_arg.op.op_type)
			{
			    അവസ്ഥ വ്യഞ്ജകം_MULT: f1 = f1 * f2; നിർത്തുക;
			    അവസ്ഥ വ്യഞ്ജകം_DIV:  f1 = f1 / f2; നിർത്തുക;
			    അവസ്ഥ വ്യഞ്ജകം_SUB:  f1 = f1 - f2; നിർത്തുക;
			    അവസ്ഥ വ്യഞ്ജകം_ADD:  f1 = f1 + f2; നിർത്തുക;
			    സ്ഥിരസ്ഥിതി: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     പോകുക on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_അട്ടി.ga_len;
		    }
		    അല്ലെങ്കിൽ
		    {
			പൂർണ്ണസംഖ്യ failed = FALSE;

			തിരഞ്ഞെടുപ്പ് (iptr->isn_arg.op.op_type)
			{
			    അവസ്ഥ വ്യഞ്ജകം_MULT: n1 = n1 * n2; നിർത്തുക;
			    അവസ്ഥ വ്യഞ്ജകം_DIV:  n1 = അക്കം_divide(n1, n2, &failed);
					    എങ്കിൽ (failed)
						പോകുക on_error;
					    നിർത്തുക;
			    അവസ്ഥ വ്യഞ്ജകം_SUB:  n1 = n1 - n2; നിർത്തുക;
			    അവസ്ഥ വ്യഞ്ജകം_ADD:  n1 = n1 + n2; നിർത്തുക;
			    സ്ഥിരസ്ഥിതി:	    n1 = അക്കം_modulus(n1, n2, &failed);
					    എങ്കിൽ (failed)
						പോകുക on_error;
					    നിർത്തുക;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_സംഖ്യ;
			tv1->vval.v_സംഖ്യ = n1;
			--ectx->ec_അട്ടി.ga_len;
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_STRINDEX:
	    അവസ്ഥ ISN_STRSLICE:
		{
		    പൂർണ്ണസംഖ്യ		is_slice = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    എങ്കിൽ (is_slice)
		    {
			tv = അട്ടി_TV_BOT(-2);
			n1 = tv->vval.v_സംഖ്യ;
		    }

		    tv = അട്ടി_TV_BOT(-1);
		    n2 = tv->vval.v_സംഖ്യ;

		    ectx->ec_അട്ടി.ga_len -= is_slice ? 2 : 1;
		    tv = അട്ടി_TV_BOT(-1);
		    എങ്കിൽ (is_slice)
			// Slice: Select the characters from the string
			res = string_slice(tv->vval.v_string, n1, n2, FALSE);
		    അല്ലെങ്കിൽ
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_LISTINDEX:
	    അവസ്ഥ ISN_LISTSLICE:
	    അവസ്ഥ ISN_TUPLEINDEX:
	    അവസ്ഥ ISN_TUPLESLICE:
	    അവസ്ഥ ISN_BLOBINDEX:
	    അവസ്ഥ ISN_BLOBSLICE:
		{
		    പൂർണ്ണസംഖ്യ		is_slice = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    പൂർണ്ണസംഖ്യ		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    പൂർണ്ണസംഖ്യ		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_slice ? അട്ടി_TV_BOT(-3) : അട്ടി_TV_BOT(-2);

		    tv = അട്ടി_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_സംഖ്യ;
		    clear_tv(tv);

		    എങ്കിൽ (is_slice)
		    {
			tv = അട്ടി_TV_BOT(-2);
			n1 = tv->vval.v_സംഖ്യ;
			clear_tv(tv);
		    }

		    ectx->ec_അട്ടി.ga_len -= is_slice ? 2 : 1;
		    tv = അട്ടി_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (is_blob)
		    {
			എങ്കിൽ (blob_slice_or_index(val_tv->vval.v_blob, is_slice,
						    n1, n2, FALSE, tv) == FAIL)
			    പോകുക on_error;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (is_tuple)
		    {
			എങ്കിൽ (tuple_slice_or_index(val_tv->vval.v_tuple,
				is_slice, n1, n2, FALSE, tv, TRUE) == FAIL)
			    പോകുക on_error;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (list_slice_or_index(val_tv->vval.v_list, is_slice,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    പോകുക on_error;
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_ANYINDEX:
	    അവസ്ഥ ISN_ANYSLICE:
		{
		    പൂർണ്ണസംഖ്യ		is_slice = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    പൂർണ്ണസംഖ്യ		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_slice ? അട്ടി_TV_BOT(-3) : അട്ടി_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (check_can_index(tv, TRUE, TRUE) == FAIL)
			പോകുക on_error;
		    var1 = is_slice ? അട്ടി_TV_BOT(-2) : അട്ടി_TV_BOT(-1);
		    var2 = is_slice ? അട്ടി_TV_BOT(-1) : ശൂന്യ;
		    res = eval_index_inner(tv, is_slice, var1, var2,
							FALSE, ശൂന്യ, -1, TRUE);
		    clear_tv(var1);
		    എങ്കിൽ (is_slice)
			clear_tv(var2);
		    ectx->ec_അട്ടി.ga_len -= is_slice ? 2 : 1;
		    എങ്കിൽ (res == FAIL)
			പോകുക on_error;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_SLICE:
		{
		    പൂർണ്ണസംഖ്യ		എണ്ണം = iptr->isn_arg.സംഖ്യ;

		    // type will have been checked to be a list
		    tv = അട്ടി_TV_BOT(-1);
		    എങ്കിൽ (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			എങ്കിൽ (list != ശൂന്യ && list->lv_len >= എണ്ണം)
			{
			    list_T	*newlist = list_slice(list,
				    എണ്ണം, list->lv_len - 1);

			    എങ്കിൽ (newlist != ശൂന്യ)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    അല്ലെങ്കിൽ
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			എങ്കിൽ (tuple != ശൂന്യ && TUPLE_LEN(tuple) >= എണ്ണം)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_slice(tuple, എണ്ണം,
							TUPLE_LEN(tuple) - 1);
			    എങ്കിൽ (newtuple != ശൂന്യ)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*item_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = അട്ടി_TV_BOT(-1 - gi->gi_with_op);
		    എങ്കിൽ (tv->v_type == VAR_LIST)
		    {
			li = list_find(tv->vval.v_list, gi->gi_index);
			എങ്കിൽ (li == ശൂന്യ)
			    പോകുക on_error;
			item_tv = &li->li_tv;
		    }
		    അല്ലെങ്കിൽ
			item_tv = TUPLE_ITEM(tv->vval.v_tuple, gi->gi_index);

		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    ++ectx->ec_അട്ടി.ga_len;
		    copy_tv(item_tv, അട്ടി_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_index = gi->gi_index + 1;
		    ectx->ec_where.wt_kind = WT_VARIABLE;
		}
		നിർത്തുക;

	    // dict member with string key (dict['member'])
	    അവസ്ഥ ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*താക്കോൽ;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = അട്ടി_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = അട്ടി_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    താക്കോൽ = tv->vval.v_string;
		    എങ്കിൽ (താക്കോൽ == ശൂന്യ)
			താക്കോൽ = (char_u *)"";

		    എങ്കിൽ ((di = dict_find(dict, താക്കോൽ, -1)) == ശൂന്യ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_താക്കോൽ_not_present_in_dictionary_str), താക്കോൽ);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_അട്ടി.ga_len;
			tv = അട്ടി_TV_BOT(-1);
			(ശൂന്യം) dict_അട്ടി_save(tv);
			tv->v_type = VAR_സംഖ്യ;
			tv->vval.v_സംഖ്യ = 0;
			പോകുക on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_അട്ടി.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = അട്ടി_TV_BOT(-1);
		    എങ്കിൽ (dict_അട്ടി_save(tv) == FAIL)
			പോകുക on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		നിർത്തുക;

	    // dict member with string key (dict.member)
	    // or can be an object
	    അവസ്ഥ ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = അട്ടി_TV_BOT(-1);

		    എങ്കിൽ (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			എങ്കിൽ (dict_അട്ടി_save(tv) == FAIL)
			    പോകുക on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			എങ്കിൽ (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    പോകുക on_error;
		    }
		    അല്ലെങ്കിൽ
		    {
			എങ്കിൽ (tv->v_type != VAR_DICT || tv->vval.v_dict == ശൂന്യ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    പോകുക on_error;
			}
			dict = tv->vval.v_dict;

			എങ്കിൽ ((di = dict_find(dict, iptr->isn_arg.string, -1))
								   == ശൂന്യ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_താക്കോൽ_not_present_in_dictionary_str),
						     iptr->isn_arg.string);
			    പോകുക on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			എങ്കിൽ (dict_അട്ടി_save(tv) == FAIL)
			    പോകുക on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_GET_OBJ_MEMBER:
	    അവസ്ഥ ISN_GET_ITF_MEMBER:
		{
		    tv = അട്ടി_TV_BOT(-1);
		    എങ്കിൽ (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			പോകുക on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    എങ്കിൽ (obj == ശൂന്യ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			പോകുക on_error;
		    }

		    പൂർണ്ണസംഖ്യ idx;
		    എങ്കിൽ (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    അല്ലെങ്കിൽ
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_index_from_itf_index(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    എങ്കിൽ (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			പോകുക on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_STORE_THIS:
		{
		    പൂർണ്ണസംഖ്യ idx = iptr->isn_arg.സംഖ്യ;
		    object_T *obj = അട്ടി_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *അട്ടി_TV_BOT(-1);
		    --ectx->ec_അട്ടി.ga_len;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_CLEARDICT:
		dict_അട്ടി_drop();
		നിർത്തുക;

	    അവസ്ഥ ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_അട്ടി_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = അട്ടി_TV_BOT(-1);
		    എങ്കിൽ (dict_tv != ശൂന്യ
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != ശൂന്യ
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == ശൂന്യ))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_അട്ടി_drop();
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_NEGATENR:
		tv = അട്ടി_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		എങ്കിൽ (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		അല്ലെങ്കിൽ
		    tv->vval.v_സംഖ്യ = -tv->vval.v_സംഖ്യ;
		നിർത്തുക;

	    അവസ്ഥ ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    പൂർണ്ണസംഖ്യ		r;
		    where_T	where = WHERE_INIT;

		    tv = അട്ടി_TV_BOT((പൂർണ്ണസംഖ്യ)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    എങ്കിൽ (ct->ct_arg_idx > 0)
		    {
			where.wt_index = ct->ct_arg_idx;
			where.wt_kind = ct->ct_is_var ? WT_VARIABLE : WT_ARGUMENT;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    എങ്കിൽ (r == FAIL)
			പോകുക on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    എങ്കിൽ (tv->v_type == VAR_സംഖ്യ
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_സംഖ്യ == 0
						|| tv->vval.v_സംഖ്യ == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_സംഖ്യ = tv->vval.v_സംഖ്യ
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_CHECKLEN:
		{
		    പൂർണ്ണസംഖ്യ	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = ശൂന്യ;
		    tuple_T *tuple = ശൂന്യ;
		    പൂർണ്ണസംഖ്യ	    len = 0;

		    tv = അട്ടി_TV_BOT(-1);

		    പൂർണ്ണസംഖ്യ		len_check_failed = FALSE;
		    എങ്കിൽ (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			എങ്കിൽ (list == ശൂന്യ || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			എങ്കിൽ (list != ശൂന്യ)
			    len = list->lv_len;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			എങ്കിൽ (tuple == ശൂന്യ || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			എങ്കിൽ (tuple != ശൂന്യ)
			    len = TUPLE_LEN(tuple);
		    }
		    അല്ലെങ്കിൽ
			len_check_failed = TRUE;

		    എങ്കിൽ (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_ഇനങ്ങൾ_but_got_nr),
				     min_len, len);
			പോകുക on_error;
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_SETTYPE:
		set_tv_type(അട്ടി_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		നിർത്തുക;

	    അവസ്ഥ ISN_2BOOL:
	    അവസ്ഥ ISN_COND2BOOL:
		{
		    പൂർണ്ണസംഖ്യ n;
		    പൂർണ്ണസംഖ്യ error = FALSE;

		    എങ്കിൽ (iptr->isn_type == ISN_2BOOL)
		    {
			tv = അട്ടി_TV_BOT(iptr->isn_arg.tobool.offset);
			n = tv2bool(tv);
			എങ്കിൽ (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    അല്ലെങ്കിൽ
		    {
			tv = അട്ടി_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			എങ്കിൽ (error)
			    പോകുക on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_സംഖ്യ = n ? VVAL_TRUE : VVAL_FALSE;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_2STRING:
	    അവസ്ഥ ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		എങ്കിൽ (do_2string(അട്ടി_TV_BOT(iptr->isn_arg.tostring.offset),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    പോകുക on_error;
		നിർത്തുക;

	    അവസ്ഥ ISN_RANGE:
		{
		    exarg_T	ea;
		    അക്ഷരം	*errormsg;

		    ea.line2 = 0;
		    ea.addr_എണ്ണം = 0;
		    ea.addr_type = ADDR_വരികൾ;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    എങ്കിൽ (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
			പോകുക on_error;

		    എങ്കിൽ (GA_GROW_FAILS(&ectx->ec_അട്ടി, 1))
			പോകുക theend;
		    ++ectx->ec_അട്ടി.ga_len;
		    tv = അട്ടി_TV_BOT(-1);
		    tv->v_type = VAR_സംഖ്യ;
		    tv->v_lock = 0;
		    tv->vval.v_സംഖ്യ = ea.line2;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_PUT:
		tv = ശൂന്യ; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		നിർത്തുക;
	    അവസ്ഥ ISN_IPUT:
		tv = ശൂന്യ;
		isn_put_do(ectx, iptr, tv, TRUE);
		നിർത്തുക;

	    അവസ്ഥ ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_അട്ടി.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		നിർത്തുക;

	    അവസ്ഥ ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = ശൂന്യ;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		നിർത്തുക;

	    അവസ്ഥ ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = അട്ടി_TV_BOT(-1);
		എങ്കിൽ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_argument_must_be_sequence_of_lists_or_tuples));
		    പോകുക on_error;
		}

		പൂർണ്ണസംഖ്യ rc;
		എങ്കിൽ (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		അല്ലെങ്കിൽ
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		എങ്കിൽ (rc != EXEC_OK)
		{
		    എങ്കിൽ (rc == EXEC_FAIL)
			പോകുക on_error;
		    പോകുക theend;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_PROF_START:
	    അവസ്ഥ ISN_PROF_END:
		{
#ifdef FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    എങ്കിൽ (iptr->isn_type == ISN_PROF_START)
		    {
			func_വരി_start(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_വരി_exec(&cookie);
		    }
		    അല്ലെങ്കിൽ
			func_വരി_end(&cookie);
#endif
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_DEBUG:
		handle_debug(iptr, ectx);
		നിർത്തുക;

	    അവസ്ഥ ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    പൂർണ്ണസംഖ്യ		item = iptr->isn_arg.shuffle.shfl_item;
		    പൂർണ്ണസംഖ്യ		up = iptr->isn_arg.shuffle.shfl_up;

		    tmp_tv = *അട്ടി_TV_BOT(-item);
		    വേണ്ടി ( ; up > 0 && item > 1; --up)
		    {
			*അട്ടി_TV_BOT(-item) = *അട്ടി_TV_BOT(-item + 1);
			--item;
		    }
		    *അട്ടി_TV_BOT(-item) = tmp_tv;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_DROP:
		--ectx->ec_അട്ടി.ga_len;
		clear_tv(അട്ടി_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		നിർത്തുക;

	    അവസ്ഥ ISN_SCRIPTCTX_SET:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		നിർത്തുക;
	}
	തുടരുക;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	എങ്കിൽ (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
	    പോകുക done;

	എങ്കിൽ (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    പോകുക theend;
	തുടരുക;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	എങ്കിൽ (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    എങ്കിൽ (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		ആകുമ്പോൾ (ectx->ec_അട്ടി.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_അട്ടി.ga_len;
		    clear_tv(അട്ടി_TV_BOT(0));
		}
		ആകുമ്പോൾ (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    തുടരുക;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	എങ്കിൽ (trylevel <= ectx->ec_trylevel_at_start)
	    പോകുക theend;
    }

done:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_അട്ടി_clear(dict_അട്ടി_len_at_start);
    ectx->ec_trylevel_at_start = save_trylevel_at_start;
    തിരികെ ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    പൂർണ്ണസംഖ്യ		save_iidx = ectx->ec_iidx;
    പൂർണ്ണസംഖ്യ		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    എങ്കിൽ (res == OK)
    {
	*rettv = *അട്ടി_TV_BOT(-1);
	--ectx->ec_അട്ടി.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    തിരികെ res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(ശൂന്യം)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    പൂർണ്ണസംഖ്യ		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    എങ്കിൽ (exec_instructions(ectx) == OK)
    {
	typval_T *tv = അട്ടി_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_അട്ടി.ga_len;
	clear_tv(tv);
    }
    അല്ലെങ്കിൽ
    {
	substitute_instr->subs_status = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    തിരികെ res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
call_def_function(
    ufunc_T	*ufunc,
    പൂർണ്ണസംഖ്യ		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    പൂർണ്ണസംഖ്യ		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    പൂർണ്ണസംഖ്യ		argc = argc_arg;
    പൂർണ്ണസംഖ്യ		partial_argc = partial == ശൂന്യ
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    പൂർണ്ണസംഖ്യ		total_argc = argc + partial_argc;
    typval_T	*tv;
    പൂർണ്ണസംഖ്യ		idx;
    പൂർണ്ണസംഖ്യ		ret = FAIL;
    പൂർണ്ണസംഖ്യ		defcount = ufunc->uf_args.ga_len - total_argc;
    sctx_T	save_current_sctx = current_sctx;
    പൂർണ്ണസംഖ്യ		did_emsg_before = did_emsg_cumul + did_emsg;
    പൂർണ്ണസംഖ്യ		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_സന്ദേശം_list = ശൂന്യ;
    msglist_T	*private_സന്ദേശം_list = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		save_emsg_silent_def = emsg_silent_def;
    പൂർണ്ണസംഖ്യ		save_did_emsg_def = did_emsg_def;
    പൂർണ്ണസംഖ്യ		orig_funcdepth;
    പൂർണ്ണസംഖ്യ		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#undef അട്ടി_TV
#define അട്ടി_TV(idx) (((typval_T *)ectx.ec_അട്ടി.ga_data) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#undef അട്ടി_TV_BOT
#define അട്ടി_TV_BOT(idx) (((typval_T *)ectx.ec_അട്ടി.ga_data) + ectx.ec_അട്ടി.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#undef അട്ടി_TV_VAR
#define അട്ടി_TV_VAR(idx) (((typval_T *)ectx.ec_അട്ടി.ga_data) + ectx.ec_frame_idx + അട്ടി_FRAME_SIZE + idx)

    എങ്കിൽ (ufunc->uf_def_status == UF_NOT_COMPILED
	    || ufunc->uf_def_status == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), ശൂന്യ) == FAIL))
    {
	എങ്കിൽ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	തിരികെ FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	എങ്കിൽ (dfunc->df_ufunc == ശൂന്യ)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    തിരികെ FAIL;
	}
	എങ്കിൽ (INSTRUCTIONS(dfunc) == ശൂന്യ)
	{
	    iemsg("using call_def_function() on not compiled function");
	    തിരികെ FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    എങ്കിൽ (funcdepth_increment() == FAIL)
	തിരികെ FAIL;

    CLEAR_FIELD(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_അട്ടി, വലുപ്പം(typval_T), 500);
    എങ്കിൽ (GA_GROW_FAILS(&ectx.ec_അട്ടി, 20))
    {
	funcdepth_decrement();
	തിരികെ FAIL;
    }
    ga_init2(&ectx.ec_trystack, വലുപ്പം(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, വലുപ്പം(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = total_argc - ufunc->uf_args.ga_len;
    എങ്കിൽ (idx > 0 && ufunc->uf_va_name == ശൂന്യ)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
								    idx), idx);
	പോകുക failed_early;
    }
    idx = total_argc - ufunc->uf_args.ga_len + ufunc->uf_def_args.ga_len;
    എങ്കിൽ (idx < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	പോകുക failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    വേണ്ടി (idx = 0; idx < total_argc
	    && (ufunc->uf_va_name != ശൂന്യ || idx < ufunc->uf_args.ga_len);
									 ++idx)
    {
	പൂർണ്ണസംഖ്യ argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	എങ്കിൽ (idx >= ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_സംഖ്യ == VVAL_NONE)
	{
	    // Use the default value.
	    അട്ടി_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ done = FALSE;
	    എങ്കിൽ (ufunc->uf_arg_types != ശൂന്യ && idx < ufunc->uf_args.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		എങ്കിൽ (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_സംഖ്യ)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    അട്ടി_TV_BOT(0)->v_type = VAR_FLOAT;
		    അട്ടി_TV_BOT(0)->v_lock = 0;
		    അട്ടി_TV_BOT(0)->vval.v_float = tv->vval.v_സംഖ്യ;
		    done = TRUE;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (check_typval_arg_type(expected, tv,
						   ശൂന്യ, argv_idx + 1) == FAIL)
		    പോകുക failed_early;
	    }
	    എങ്കിൽ (!done)
		copy_tv(tv, അട്ടി_TV_BOT(0));
	}
	++ectx.ec_അട്ടി.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    എങ്കിൽ (ufunc->uf_va_name != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ vararg_എണ്ണം = argc - ufunc->uf_args.ga_len;

	എങ്കിൽ (vararg_എണ്ണം < 0)
	    vararg_എണ്ണം = 0;
	അല്ലെങ്കിൽ
	    argc -= vararg_എണ്ണം;
	എങ്കിൽ (exe_newlist(vararg_എണ്ണം, &ectx) == FAIL)
	    പോകുക failed_early;

	// Check the type of the list items.
	tv = അട്ടി_TV_BOT(-1);
	എങ്കിൽ (ufunc->uf_va_type != ശൂന്യ
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != ശൂന്യ)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_first;

	    വേണ്ടി (idx = 0; idx < vararg_എണ്ണം; ++idx)
	    {
		എങ്കിൽ (check_typval_arg_type(expected, &li->li_tv,
						 ശൂന്യ, argc + idx + 1) == FAIL)
		    പോകുക failed_early;
		li = li->li_next;
	    }
	}

	എങ്കിൽ (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *അട്ടി_TV_BOT(defcount - 1) = *അട്ടി_TV_BOT(-1);
	--ectx.ec_അട്ടി.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    എങ്കിൽ (defcount > 0)
	വേണ്ടി (idx = 0; idx < defcount; ++idx)
	{
	    അട്ടി_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_അട്ടി.ga_len;
	}
    എങ്കിൽ (ufunc->uf_va_name != ശൂന്യ)
	++ectx.ec_അട്ടി.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_frame_idx = ectx.ec_അട്ടി.ga_len;
    ectx.ec_initial_frame_idx = ectx.ec_frame_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *base_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	എങ്കിൽ (partial != ശൂന്യ || base_ufunc->uf_partial != ശൂന്യ)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    എങ്കിൽ (ectx.ec_outer_ref == ശൂന്യ)
		പോകുക failed_early;
	    എങ്കിൽ (partial != ശൂന്യ)
	    {
		outer_T *outer = get_pt_outer(partial);

		എങ്കിൽ (outer->out_അട്ടി == ശൂന്യ && outer->out_loop_size == 0)
		{
		    // no stack was set
		    എങ്കിൽ (current_ectx != ശൂന്യ)
		    {
			എങ്കിൽ (current_ectx->ec_outer_ref != ശൂന്യ
			       && current_ectx->ec_outer_ref->or_outer != ശൂന്യ)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		അല്ലെങ്കിൽ
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		ectx.ec_outer_ref->or_outer = &base_ufunc->uf_partial->pt_outer;
		++base_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = base_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    വേണ്ടി (idx = 0; idx < അട്ടി_FRAME_SIZE; ++idx)
    {
	അട്ടി_TV(ectx.ec_അട്ടി.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_അട്ടി.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	വേണ്ടി (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    അട്ടി_TV_VAR(idx)->v_type = VAR_സംഖ്യ;
	    അട്ടി_TV_VAR(idx)->vval.v_സംഖ്യ = 0;
	}
	ectx.ec_അട്ടി.ga_len += dfunc->df_varcount;

	എങ്കിൽ (object != ശൂന്യ)
	{
	    // the object is always the variable at index zero
	    tv = അട്ടി_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	എങ്കിൽ (dfunc->df_has_closure)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    അട്ടി_TV_VAR(idx)->v_type = VAR_സംഖ്യ;
	    അട്ടി_TV_VAR(idx)->vval.v_സംഖ്യ = 0;
	    ++ectx.ec_അട്ടി.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    എങ്കിൽ (funccal != ശൂന്യ)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_തള്ളുക_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_സന്ദേശം_list = സന്ദേശം_list;
    സന്ദേശം_list = &private_സന്ദേശം_list;

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
    എങ്കിൽ (ret == OK)
    {
	// function finished, get result from the stack.
	എങ്കിൽ (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	അല്ലെങ്കിൽ
	{
	    tv = അട്ടി_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    എങ്കിൽ (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_closure_in_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_എടുക്കുക();
    current_sctx = save_current_sctx;

    എങ്കിൽ (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    എങ്കിൽ (*സന്ദേശം_list != ശൂന്യ && saved_സന്ദേശം_list != ശൂന്യ)
    {
	msglist_T **plist = saved_സന്ദേശം_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	ആകുമ്പോൾ (*plist != ശൂന്യ)
	    plist = &(*plist)->next;

	*plist = *സന്ദേശം_list;
    }
    സന്ദേശം_list = saved_സന്ദേശം_list;

    എങ്കിൽ (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ശൂന്യ;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    വേണ്ടി (idx = 0; idx < ectx.ec_അട്ടി.ga_len; ++idx)
    {
	tv = അട്ടി_TV(idx);
	എങ്കിൽ (tv->v_type != VAR_സംഖ്യ && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_അട്ടി.ga_data);
    vim_free(ectx.ec_trystack.ga_data);
    എങ്കിൽ (ectx.ec_outer_ref != ശൂന്യ)
    {
	എങ്കിൽ (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    എങ്കിൽ (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    തിരികെ ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    ശൂന്യം
unwind_def_callstack(ectx_T *ectx)
{
    ആകുമ്പോൾ (ectx->ec_frame_idx != ectx->ec_initial_frame_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    ശൂന്യം
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    എങ്കിൽ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    സ്ഥിതം char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    പൂർണ്ണസംഖ്യ		depth;

    ga_init2(&ga, 1, 100);
    വേണ്ടി (depth = 0; depth < lvi->lvi_depth; ++depth)
    {
	എങ്കിൽ (ga_grow(&ga, 50) == FAIL)
	    നിർത്തുക;
	എങ്കിൽ (lvi->lvi_loop[depth].var_idx == 0)
	    STRCPY((അക്ഷരം *)ga.ga_data + ga.ga_len, " -");
	അല്ലെങ്കിൽ
	    vim_snprintf((അക്ഷരം *)ga.ga_data + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[depth].var_idx,
			    lvi->lvi_loop[depth].var_idx
					 + lvi->lvi_loop[depth].var_എണ്ണം - 1);
	ga.ga_len = (പൂർണ്ണസംഖ്യ)STRLEN(ga.ga_data);
    }
    തിരികെ ga.ga_data;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    സ്ഥിതം ശൂന്യം
list_instructions(അക്ഷരം *pfx, isn_T *instr, പൂർണ്ണസംഖ്യ instr_എണ്ണം, ufunc_T *ufunc)
{
    പൂർണ്ണസംഖ്യ		വരി_idx = 0;
    പൂർണ്ണസംഖ്യ		prev_current = 0;
    പൂർണ്ണസംഖ്യ		current;
    പൂർണ്ണസംഖ്യ		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    വേണ്ടി (current = 0; current < instr_എണ്ണം; ++current)
    {
	isn_T	    *iptr = &instr[current];
	അക്ഷരം	    *വരി;

	എങ്കിൽ (ufunc != ശൂന്യ)
	{
	    ആകുമ്പോൾ (വരി_idx < iptr->isn_lnum
					  && വരി_idx < ufunc->uf_വരികൾ.ga_len)
	    {
		എങ്കിൽ (current > prev_current)
		{
		    സന്ദേശം_puts("\n\n");
		    prev_current = current;
		}
		വരി = ((അക്ഷരം **)ufunc->uf_വരികൾ.ga_data)[വരി_idx++];
		എങ്കിൽ (വരി != ശൂന്യ)
		    സന്ദേശം(വരി);
	    }
	    എങ്കിൽ (iptr->isn_type == ISN_JUMP_IF_ARG_SET)
	    {
		പൂർണ്ണസംഖ്യ	first_def_arg = ufunc->uf_args.ga_len
						   - ufunc->uf_def_args.ga_len;

		എങ്കിൽ (def_arg_idx > 0)
		    സന്ദേശം_puts("\n\n");
		സന്ദേശം_start();
		സന്ദേശം_puts("  ");
		സന്ദേശം_puts(((അക്ഷരം **)(ufunc->uf_args.ga_data))[
						 first_def_arg + def_arg_idx]);
		സന്ദേശം_puts(" = ");
		സന്ദേശം_puts(((അക്ഷരം **)(ufunc->uf_def_args.ga_data))[def_arg_idx++]);
		സന്ദേശം_clr_eos();
		സന്ദേശം_end();
	    }
	}

	തിരഞ്ഞെടുപ്പ് (iptr->isn_type)
	{
	    അവസ്ഥ ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (പൂർണ്ണസംഖ്യ)iptr->isn_arg.construct.construct_size);
		നിർത്തുക;
	    അവസ്ഥ ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_EXEC_SPLIT:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.സംഖ്യ ? " append" : "");
		നിർത്തുക;
	    അവസ്ഥ ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.സംഖ്യ));
#endif
		നിർത്തുക;
	    അവസ്ഥ ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#endif
		നിർത്തുക;
	    അവസ്ഥ ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, INT_MAX, ശൂന്യ);
		സന്ദേശം("     -------------");
		നിർത്തുക;
	    അവസ്ഥ ISN_SOURCE:
		{
		    scriptitem_T    *si = SCRIPT_ITEM(iptr->isn_arg.സംഖ്യ);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, INT_MAX, ശൂന്യ);
		    സന്ദേശം("     -------------");
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.സംഖ്യ);
		നിർത്തുക;
	    അവസ്ഥ ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_എണ്ണം);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_ECHOWINDOW:
		എങ്കിൽ (iptr->isn_arg.echowin.ewin_time > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_എണ്ണം,
				      iptr->isn_arg.echowin.ewin_time);
		അല്ലെങ്കിൽ
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_എണ്ണം);
		നിർത്തുക;
	    അവസ്ഥ ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_LOAD:
		{
		    എങ്കിൽ (iptr->isn_arg.സംഖ്യ < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.സംഖ്യ
							  + അട്ടി_FRAME_SIZE));
		    അല്ലെങ്കിൽ
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    എങ്കിൽ (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_depth,
					outer->outer_idx + അട്ടി_FRAME_SIZE);
		    അല്ലെങ്കിൽ എങ്കിൽ (outer->outer_depth < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_depth);
		    അല്ലെങ്കിൽ
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_depth,
					      outer->outer_idx);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    എങ്കിൽ (sv == ശൂന്യ)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    അല്ലെങ്കിൽ
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADS:
	    അവസ്ഥ ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (പൂർണ്ണസംഖ്യ)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;

	    അവസ്ഥ ISN_STORE:
		എങ്കിൽ (iptr->isn_arg.സംഖ്യ < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.സംഖ്യ + അട്ടി_FRAME_SIZE);
		അല്ലെങ്കിൽ
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.സംഖ്യ);
		നിർത്തുക;
	    അവസ്ഥ ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    എങ്കിൽ (outer->outer_depth < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_depth);
		    അല്ലെങ്കിൽ
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_depth, outer->outer_idx);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_STORES:
	    അവസ്ഥ ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    എങ്കിൽ (sv == ശൂന്യ)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    അല്ലെങ്കിൽ
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_STOREOPT:
	    അവസ്ഥ ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		നിർത്തുക;
	    അവസ്ഥ ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (പൂർണ്ണസംഖ്യ)iptr->isn_arg.സംഖ്യ);
		നിർത്തുക;
	    അവസ്ഥ ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		നിർത്തുക;

	    അവസ്ഥ ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		നിർത്തുക;

	    അവസ്ഥ ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		നിർത്തുക;

	    അവസ്ഥ ISN_LOAD_CLASSMEMBER:
	    അവസ്ഥ ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    പൂർണ്ണസംഖ്യ	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_LOAD_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		നിർത്തുക;

	    // constants
	    അവസ്ഥ ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHBOOL:
	    അവസ്ഥ ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHFUNC:
		{
		    അക്ഷരം *name = (അക്ഷരം *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == ശൂന്യ ? "[none]" : name);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#endif
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#endif
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == ശൂന്യ ? "null"
				 : (അക്ഷരം *)iptr->isn_arg.classarg->class_name.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		നിർത്തുക;
	    അവസ്ഥ ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		നിർത്തുക;
	    അവസ്ഥ ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		നിർത്തുക;
	    അവസ്ഥ ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		നിർത്തുക;

	    // function call
	    അവസ്ഥ ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    internal_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != ശൂന്യ
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_മുകൾ ? " top" : "", cpfunc->cpf_argcount);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_PCALL_END:
		smsg("%s%4d PCALL end", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (പൂർണ്ണസംഖ്യ)iptr->isn_arg.defer.defer_argcount);
		നിർത്തുക;
	    അവസ്ഥ ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    എങ്കിൽ (extra == ശൂന്യ || extra->fre_func_name == ശൂന്യ)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    അല്ലെങ്കിൽ
			name = extra->fre_func_name;
		    എങ്കിൽ (extra != ശൂന്യ && extra->fre_class != ശൂന്യ)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (extra == ശൂന്യ
				     || extra->fre_loopvar_info.lvi_depth == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    അല്ലെങ്കിൽ
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    എങ്കിൽ (arg->nfa_loopvar_info.lvi_depth == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    അല്ലെങ്കിൽ
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == ശൂന്യ ? (char_u *)"" : name);
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_JUMP:
		{
		    അക്ഷരം *when = "?";

		    തിരഞ്ഞെടുപ്പ് (iptr->isn_arg.jump.jump_when)
		    {
			അവസ്ഥ JUMP_ALWAYS:
			    when = "JUMP";
			    നിർത്തുക;
			അവസ്ഥ JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    നിർത്തുക;
			അവസ്ഥ JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    നിർത്തുക;
			അവസ്ഥ JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    നിർത്തുക;
			അവസ്ഥ JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  // unused
			    നിർത്തുക;
			അവസ്ഥ JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    നിർത്തുക;
			അവസ്ഥ JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    നിർത്തുക;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_JUMP_IF_ARG_SET:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + അട്ടി_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		നിർത്തുക;

	    അവസ്ഥ ISN_JUMP_IF_ARG_NOT_SET:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + അട്ടി_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		നിർത്തുക;

	    അവസ്ഥ ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_end);
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->end_funcref_idx,
			    endloop->end_var_idx,
			    endloop->end_var_idx + endloop->end_var_എണ്ണം - 1,
			    endloop->end_depth);
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_end);
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    എങ്കിൽ (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    അല്ലെങ്കിൽ
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_levels,
				      trycont->tct_levels == 1 ? "" : "s",
				      trycont->tct_where);
		}
		നിർത്തുക;
	    അവസ്ഥ ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		നിർത്തുക;
	    അവസ്ഥ ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		നിർത്തുക;

	    // expression operations on number
	    അവസ്ഥ ISN_OPNR:
	    അവസ്ഥ ISN_OPFLOAT:
	    അവസ്ഥ ISN_OPANY:
		{
		    അക്ഷരം *what;
		    അക്ഷരം *ins;

		    തിരഞ്ഞെടുപ്പ് (iptr->isn_arg.op.op_type)
		    {
			അവസ്ഥ വ്യഞ്ജകം_MULT: what = "*"; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_DIV: what = "/"; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_REM: what = "%"; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_SUB: what = "-"; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_ADD: what = "+"; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_LSHIFT: what = "<<"; നിർത്തുക;
			അവസ്ഥ വ്യഞ്ജകം_RSHIFT: what = ">>"; നിർത്തുക;
			സ്ഥിരസ്ഥിതി:       what = "???"; നിർത്തുക;
		    }
		    തിരഞ്ഞെടുപ്പ് (iptr->isn_type)
		    {
			അവസ്ഥ ISN_OPNR: ins = "OPNR"; നിർത്തുക;
			അവസ്ഥ ISN_OPFLOAT: ins = "OPFLOAT"; നിർത്തുക;
			അവസ്ഥ ISN_OPANY: ins = "OPANY"; നിർത്തുക;
			സ്ഥിരസ്ഥിതി: ins = "???"; നിർത്തുക;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_COMPAREBOOL:
	    അവസ്ഥ ISN_COMPARESPECIAL:
	    അവസ്ഥ ISN_COMPARENULL:
	    അവസ്ഥ ISN_COMPARENR:
	    അവസ്ഥ ISN_COMPAREFLOAT:
	    അവസ്ഥ ISN_COMPARESTRING:
	    അവസ്ഥ ISN_COMPAREBLOB:
	    അവസ്ഥ ISN_COMPARELIST:
	    അവസ്ഥ ISN_COMPARETUPLE:
	    അവസ്ഥ ISN_COMPAREDICT:
	    അവസ്ഥ ISN_COMPAREFUNC:
	    അവസ്ഥ ISN_COMPAREOBJECT:
	    അവസ്ഥ ISN_COMPAREANY:
		   {
		       അക്ഷരം *p;
		       അക്ഷരം buf[10];
		       അക്ഷരം *type;

		       തിരഞ്ഞെടുപ്പ് (iptr->isn_arg.op.op_type)
		       {
			   അവസ്ഥ വ്യഞ്ജകം_EQUAL:	 p = "=="; നിർത്തുക;
			   അവസ്ഥ വ്യഞ്ജകം_NEQUAL:    p = "!="; നിർത്തുക;
			   അവസ്ഥ വ്യഞ്ജകം_GREATER:   p = ">"; നിർത്തുക;
			   അവസ്ഥ വ്യഞ്ജകം_GEQUAL:    p = ">="; നിർത്തുക;
			   അവസ്ഥ വ്യഞ്ജകം_SMALLER:   p = "<"; നിർത്തുക;
			   അവസ്ഥ വ്യഞ്ജകം_SEQUAL:    p = "<="; നിർത്തുക;
			   അവസ്ഥ വ്യഞ്ജകം_MATCH:	 p = "=~"; നിർത്തുക;
			   അവസ്ഥ വ്യഞ്ജകം_IS:	 p = "is"; നിർത്തുക;
			   അവസ്ഥ വ്യഞ്ജകം_ISNOT:	 p = "isnot"; നിർത്തുക;
			   അവസ്ഥ വ്യഞ്ജകം_NOMATCH:	 p = "!~"; നിർത്തുക;
			   സ്ഥിരസ്ഥിതി:  p = "???"; നിർത്തുക;
		       }
		       STRCPY(buf, p);
		       എങ്കിൽ (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       തിരഞ്ഞെടുപ്പ്(iptr->isn_type)
		       {
			   അവസ്ഥ ISN_COMPAREBOOL: type = "COMPAREBOOL"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPARENULL: type = "COMPARENULL"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPARENR: type = "COMPARENR"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPARESTRING:
						  type = "COMPARESTRING"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPAREBLOB: type = "COMPAREBLOB"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPARELIST: type = "COMPARELIST"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPARETUPLE: type = "COMPARETUPLE"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPAREDICT: type = "COMPAREDICT"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPAREFUNC: type = "COMPAREFUNC"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; നിർത്തുക;
			   അവസ്ഥ ISN_COMPAREANY: type = "COMPAREANY"; നിർത്തുക;
			   സ്ഥിരസ്ഥിതി: type = "???"; നിർത്തുക;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   നിർത്തുക;

	    അവസ്ഥ ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); നിർത്തുക;

	    // expression operations
	    അവസ്ഥ ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.സംഖ്യ));
		നിർത്തുക;
	    അവസ്ഥ ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_SLICE: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.സംഖ്യ); നിർത്തുക;
	    അവസ്ഥ ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_index,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); നിർത്തുക;
	    അവസ്ഥ ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); നിർത്തുക;
	    അവസ്ഥ ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (പൂർണ്ണസംഖ്യ)iptr->isn_arg.classmember.cm_idx);
				     നിർത്തുക;
	    അവസ്ഥ ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (പൂർണ്ണസംഖ്യ)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     നിർത്തുക;
	    അവസ്ഥ ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (പൂർണ്ണസംഖ്യ)iptr->isn_arg.സംഖ്യ); നിർത്തുക;
	    അവസ്ഥ ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); നിർത്തുക;

	    അവസ്ഥ ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); നിർത്തുക;

	    അവസ്ഥ ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      അക്ഷരം	    *tofree = ശൂന്യ;
		      അക്ഷരം	    *typename;

		      എങ്കിൽ (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_സംഖ്യ_OK))
			  typename = "float|number";
		      അല്ലെങ്കിൽ എങ്കിൽ (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      അല്ലെങ്കിൽ
			  typename = type_name(ct->ct_type, &tofree);

		      എങ്കിൽ (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (പൂർണ്ണസംഖ്യ)ct->ct_off);
		      അല്ലെങ്കിൽ
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (പൂർണ്ണസംഖ്യ)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (പൂർണ്ണസംഖ്യ)ct->ct_arg_idx);
		      vim_free(tofree);
		      നിർത്തുക;
		  }
	    അവസ്ഥ ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_more_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       നിർത്തുക;
	    അവസ്ഥ ISN_SETTYPE:
		  {
		      അക്ഷരം *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      നിർത്തുക;
		  }
	    അവസ്ഥ ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); നിർത്തുക;
	    അവസ്ഥ ISN_2BOOL: എങ്കിൽ (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    അല്ലെങ്കിൽ
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    നിർത്തുക;
	    അവസ്ഥ ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      നിർത്തുക;
	    അവസ്ഥ ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      നിർത്തുക;
	    അവസ്ഥ ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    നിർത്തുക;
	    അവസ്ഥ ISN_PUT:
		എങ്കിൽ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		അല്ലെങ്കിൽ
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		നിർത്തുക;
	    അവസ്ഥ ISN_IPUT:
		എങ്കിൽ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		അല്ലെങ്കിൽ എങ്കിൽ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		അല്ലെങ്കിൽ
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		നിർത്തുക;
	    അവസ്ഥ ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  ശൂന്യ, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    എങ്കിൽ (likely(buf != ശൂന്യ))
		    {
			(ശൂന്യം)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    നിർത്തുക;
		}
	    അവസ്ഥ ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); നിർത്തുക;

	    അവസ്ഥ ISN_PROF_START:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 നിർത്തുക;

	    അവസ്ഥ ISN_PROF_END:
		smsg("%s%4d PROFILE END", pfx, current);
		നിർത്തുക;

	    അവസ്ഥ ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_names_len);
		നിർത്തുക;

	    അവസ്ഥ ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_എണ്ണം,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      നിർത്തുക;
	    അവസ്ഥ ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_item,
					 iptr->isn_arg.shuffle.shfl_up);
			      നിർത്തുക;
	    അവസ്ഥ ISN_DROP: smsg("%s%4d DROP", pfx, current); നിർത്തുക;

	    അവസ്ഥ ISN_SCRIPTCTX_SET:
		{
		    പൂർണ്ണസംഖ്യ	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ITEM(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		നിർത്തുക;

	    അവസ്ഥ ISN_FINISH: // End of list of instructions for ISN_SUBSTITUTE.
			   തിരികെ;
	}

	out_flush();	    // output one line at a time
	ui_breakcheck();
	എങ്കിൽ (got_int)
	    നിർത്തുക;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    ശൂന്യം
set_context_in_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_pattern = arg;

    // first argument already typed: only user function names
    എങ്കിൽ (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
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
get_disassemble_argument(expand_T *xp, പൂർണ്ണസംഖ്യ idx)
{
    എങ്കിൽ (idx == 0)
	തിരികെ (char_u *)"debug";
    എങ്കിൽ (idx == 1)
	തിരികെ (char_u *)"profile";
    തിരികെ get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    ശൂന്യം
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = ശൂന്യ;  // init to shut up gcc warning
    പൂർണ്ണസംഖ്യ		instr_എണ്ണം = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = find_func_by_name(arg, &compile_type);
    എങ്കിൽ (ufunc == ശൂന്യ)
	തിരികെ;
    എങ്കിൽ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, ശൂന്യ) == FAIL)
	തിരികെ;
    എങ്കിൽ (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	തിരികെ;
    }
    സന്ദേശം((അക്ഷരം *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    തിരഞ്ഞെടുപ്പ് (compile_type)
    {
	അവസ്ഥ CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_എണ്ണം = dfunc->df_instr_prof_എണ്ണം;
	    നിർത്തുക;
#endif
	    // FALLTHROUGH
	അവസ്ഥ CT_NONE:
	    instr = dfunc->df_instr;
	    instr_എണ്ണം = dfunc->df_instr_എണ്ണം;
	    നിർത്തുക;
	അവസ്ഥ CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_എണ്ണം = dfunc->df_instr_debug_എണ്ണം;
	    നിർത്തുക;
    }

    list_instructions("", instr, instr_എണ്ണം, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    പൂർണ്ണസംഖ്യ
tv2bool(typval_T *tv)
{
    തിരഞ്ഞെടുപ്പ് (tv->v_type)
    {
	അവസ്ഥ VAR_സംഖ്യ:
	    തിരികെ tv->vval.v_സംഖ്യ != 0;
	അവസ്ഥ VAR_FLOAT:
	    തിരികെ tv->vval.v_float != 0.0;
	അവസ്ഥ VAR_PARTIAL:
	    തിരികെ tv->vval.v_partial != ശൂന്യ;
	അവസ്ഥ VAR_FUNC:
	അവസ്ഥ VAR_STRING:
	    തിരികെ tv->vval.v_string != ശൂന്യ && *tv->vval.v_string != NUL;
	അവസ്ഥ VAR_LIST:
	    തിരികെ tv->vval.v_list != ശൂന്യ && tv->vval.v_list->lv_len > 0;
	അവസ്ഥ VAR_TUPLE:
	    തിരികെ tuple_len(tv->vval.v_tuple) > 0;
	അവസ്ഥ VAR_DICT:
	    തിരികെ tv->vval.v_dict != ശൂന്യ
				    && tv->vval.v_dict->dv_hashtab.ht_used > 0;
	അവസ്ഥ VAR_BOOL:
	അവസ്ഥ VAR_SPECIAL:
	    തിരികെ tv->vval.v_സംഖ്യ == VVAL_TRUE ? TRUE : FALSE;
	അവസ്ഥ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    തിരികെ tv->vval.v_job != ശൂന്യ;
#അല്ലെങ്കിൽ
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    തിരികെ tv->vval.v_channel != ശൂന്യ;
#അല്ലെങ്കിൽ
	    നിർത്തുക;
#endif
	അവസ്ഥ VAR_BLOB:
	    തിരികെ tv->vval.v_blob != ശൂന്യ && tv->vval.v_blob->bv_ga.ga_len > 0;

	അവസ്ഥ VAR_OBJECT:
	    തിരികെ tv->vval.v_object != ശൂന്യ;

	അവസ്ഥ VAR_CLASS:
	അവസ്ഥ VAR_TYPEALIAS:
	    check_typval_is_മൂല്യം(tv);
	    നിർത്തുക;

	അവസ്ഥ VAR_UNKNOWN:
	അവസ്ഥ VAR_ANY:
	അവസ്ഥ VAR_VOID:
	അവസ്ഥ VAR_INSTR:
	    നിർത്തുക;
    }
    തിരികെ FALSE;
}

    ശൂന്യം
emsg_using_string_as(typval_T *tv, പൂർണ്ണസംഖ്യ as_സംഖ്യ)
{
    semsg(_(as_സംഖ്യ ? e_using_string_as_സംഖ്യ_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == ശൂന്യ
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    പൂർണ്ണസംഖ്യ
check_not_string(typval_T *tv)
{
    എങ്കിൽ (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

#endif // FEAT_EVAL
