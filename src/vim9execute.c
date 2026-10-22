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
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ	    tcd_frame_idx;	// ec_frame_idx at ISN_TRY
    ಪೂರ್ಣಾಙ್ಕ	    tcd_ರಾಶಿ_len;	// size of ectx.ec_stack at ISN_TRY
    ಪೂರ್ಣಾಙ್ಕ	    tcd_in_catch;	// in catch or finally block
    ಪೂರ್ಣಾಙ್ಕ	    tcd_did_throw;	// set did_throw in :endtry
    ಪೂರ್ಣಾಙ್ಕ	    tcd_catch_idx;	// instruction of the first :catch or :finally
    ಪೂರ್ಣಾಙ್ಕ	    tcd_finally_idx;	// instruction of the :finally block or zero
    ಪೂರ್ಣಾಙ್ಕ	    tcd_endtry_idx;	// instruction of the :endtry
    ಪೂರ್ಣಾಙ್ಕ	    tcd_caught;		// catch block entered
    ಪೂರ್ಣಾಙ್ಕ	    tcd_cont;		// :continue encountered, jump here (minus one)
    ಪೂರ್ಣಾಙ್ಕ	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    ಪೂರ್ಣಾಙ್ಕ		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    ಪೂರ್ಣಾಙ್ಕ		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    ಪೂರ್ಣಾಙ್ಕ		or_outer_allocated;  // free "or_outer" later
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
ಸಂರಚನಾ ectx_S {
    garray_T	ec_ರಾಶಿ;	// stack of typval_T values
    ಪೂರ್ಣಾಙ್ಕ		ec_frame_idx;	// index in ec_stack: context of ec_dfunc_idx
    ಪೂರ್ಣಾಙ್ಕ		ec_initial_frame_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    ಪೂರ್ಣಾಙ್ಕ		ec_dfunc_idx;	// current function index
    ಪೂರ್ಣಾಙ್ಕ		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    ಪೂರ್ಣಾಙ್ಕ		ec_did_emsg_before;
    ಪೂರ್ಣಾಙ್ಕ		ec_trylevel_at_start;
    where_T	ec_where;
};

#ifdef FEAT_PROFILE
// stack of profinfo_T used when profiling.
ಸ್ಥಿರ garray_T profile_info_ga = {0, 0, ಪರಿಮಾಣ(profinfo_T), 20, ರಿಕ್ತ};
#endif

// Get pointer to item in the stack.
#define ರಾಶಿ_TV(idx) (((typval_T *)ectx->ec_ರಾಶಿ.ga_data) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#define ರಾಶಿ_TV_BOT(idx) (((typval_T *)ectx->ec_ರಾಶಿ.ga_data) + ectx->ec_ರಾಶಿ.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#define ರಾಶಿ_TV_VAR(idx) (((typval_T *)ectx->ec_ರಾಶಿ.ga_data) + ectx->ec_frame_idx + ರಾಶಿ_FRAME_SIZE + idx)

// Return value for functions used to execute instructions
#define EXEC_FAIL	0
#define EXEC_OK		1
#define EXEC_DONE	2

    ಶೂನ್ಯ
to_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_to_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
ufunc_argcount(ufunc_T *ufunc)
{
    ಪ್ರತ್ಯಾಗಮ ufunc->uf_args.ga_len + (ufunc->uf_va_name != ರಿಕ್ತ ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exe_concat(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ectx_T *ectx)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    size_t	len = 0;
    garray_T	ga;
    ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
    {
	typval_T    *tv;
	size_t	    length;
    } string_segment_T;
    ಗಣನಾ
    {
	STRING_SEGMENT_CACHE_SIZE = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_SIZE];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    ಯದಿ (ಎಣಿಕೆ > (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, ಎಣಿಕೆ);
	ಯದಿ (string_segment_tab == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ);
	ಯದಿ (segment->tv->vval.v_string != ರಿಕ್ತ)
	{
	    segment->length = STRLEN(segment->tv->vval.v_string);
	    len += segment->length;
	}
	ಅನ್ಯಥಾ
	    segment->length = 0;    // Ensure clean state for the second pass
    }

    ಯದಿ (ga_grow(&ga, (ಪೂರ್ಣಾಙ್ಕ)len + 1) == FAIL)
    {
	ಯದಿ (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->length);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ - 1;
    ರಾಶಿ_TV_BOT(-1)->vval.v_string = ga.ga_data;

    ಯದಿ (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exe_newlist(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ectx_T *ectx)
{
    list_T	*list = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    typval_T	*tv;

    ಯದಿ (ಎಣಿಕೆ >= 0)
    {
	list = list_alloc_with_ವಸ್ತುಗಳು(ಎಣಿಕೆ);
	ಯದಿ (list == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; ++idx)
	    list_set_item(list, idx, ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ));
    }

    ಯದಿ (ಎಣಿಕೆ > 0)
	ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ - 1;
    ಅನ್ಯಥಾ ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
    {
	list_unref(list);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
	++ectx->ec_ರಾಶಿ.ga_len;
    tv = ರಾಶಿ_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_lock = 0;
    ಯದಿ (list != ರಿಕ್ತ)
	++list->lv_refcount;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exe_newtuple(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ectx_T *ectx)
{
    tuple_T	*tuple = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    typval_T	*tv;

    ಯದಿ (ಎಣಿಕೆ >= 0)
    {
	tuple = tuple_alloc_with_ವಸ್ತುಗಳು(ಎಣಿಕೆ);
	ಯದಿ (tuple == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; ++idx)
	    tuple_set_item(tuple, idx, ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ));
    }

    ಯದಿ (ಎಣಿಕೆ > 0)
	ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ - 1;
    ಅನ್ಯಥಾ ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
    {
	tuple_unref(tuple);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
	++ectx->ec_ರಾಶಿ.ga_len;
    tv = ರಾಶಿ_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_lock = 0;
    ಯದಿ (tuple != ರಿಕ್ತ)
	++tuple->tv_refcount;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exe_newdict(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ, ectx_T *ectx)
{
    dict_T	*dict = ರಿಕ್ತ;
    dictitem_T	*item;
    char_u	*ಕೀಲಿ;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    typval_T	*tv;

    ಯದಿ (ಎಣಿಕೆ >= 0)
    {
	dict = dict_alloc();
	ಯದಿ (unlikely(dict == ರಿಕ್ತ))
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = ರಾಶಿ_TV_BOT(2 * (idx - ಎಣಿಕೆ));
	    // check key is unique
	    ಕೀಲಿ = tv->vval.v_string == ರಿಕ್ತ ? (char_u *)"" : tv->vval.v_string;
	    item = dict_find(dict, ಕೀಲಿ, -1);
	    ಯದಿ (item != ರಿಕ್ತ)
	    {
		semsg(_(e_duplicate_ಕೀಲಿ_in_dictionary_str), ಕೀಲಿ);
		dict_unref(dict);
		ಪ್ರತ್ಯಾಗಮ MAYBE;
	    }
	    item = dictitem_alloc(ಕೀಲಿ);
	    clear_tv(tv);
	    ಯದಿ (unlikely(item == ರಿಕ್ತ))
	    {
		dict_unref(dict);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    tv = ರಾಶಿ_TV_BOT(2 * (idx - ಎಣಿಕೆ) + 1);
	    item->di_tv = *tv;
	    item->di_tv.v_lock = 0;
	    tv->v_type = VAR_UNKNOWN;
	    ಯದಿ (dict_add(dict, item) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(item);
		dict_unref(dict);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	}
    }

    ಯದಿ (ಎಣಿಕೆ > 0)
	ectx->ec_ರಾಶಿ.ga_len -= 2 * ಎಣಿಕೆ - 1;
    ಅನ್ಯಥಾ ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
    {
	dict_unref(dict);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
	++ectx->ec_ರಾಶಿ.ga_len;
    tv = ರಾಶಿ_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_lock = 0;
    tv->vval.v_dict = dict;
    ಯದಿ (dict != ರಿಕ್ತ)
	++dict->dv_refcount;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    ಶೂನ್ಯ
update_has_breakpoint(ufunc_T *ufunc)
{
    ಯದಿ (ufunc->uf_debug_tick == debug_tick)
	ಪ್ರತ್ಯಾಗಮ;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

ಸ್ಥಿರ garray_T dict_ರಾಶಿ = GA_EMPTY;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
dict_ರಾಶಿ_save(typval_T *tv)
{
    ಯದಿ (dict_ರಾಶಿ.ga_growsize == 0)
	ga_init2(&dict_ರಾಶಿ, ಪರಿಮಾಣ(typval_T), 10);
    ಯದಿ (ga_grow(&dict_ರಾಶಿ, 1) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ((typval_T *)dict_ರಾಶಿ.ga_data)[dict_ರಾಶಿ.ga_len] = *tv;
    ++dict_ರಾಶಿ.ga_len;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    ಸ್ಥಿರ typval_T *
dict_ರಾಶಿ_get_tv(ಶೂನ್ಯ)
{
    ಯದಿ (dict_ರಾಶಿ.ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ ((typval_T *)dict_ರಾಶಿ.ga_data) + dict_ರಾಶಿ.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    ಸ್ಥಿರ dict_T *
dict_ರಾಶಿ_get_dict(ಶೂನ್ಯ)
{
    typval_T *tv;

    ಯದಿ (dict_ರಾಶಿ.ga_len == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    tv = ((typval_T *)dict_ರಾಶಿ.ga_data) + dict_ರಾಶಿ.ga_len - 1;
    ಯದಿ (tv->v_type == VAR_DICT)
	ಪ್ರತ್ಯಾಗಮ tv->vval.v_dict;
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Drop an item from the dict stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dict_ರಾಶಿ_drop(ಶೂನ್ಯ)
{
    ಯದಿ (dict_ರಾಶಿ.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	ಪ್ರತ್ಯಾಗಮ;
    }
    --dict_ರಾಶಿ.ga_len;
    clear_tv(((typval_T *)dict_ರಾಶಿ.ga_data) + dict_ರಾಶಿ.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dict_ರಾಶಿ_clear(ಪೂರ್ಣಾಙ್ಕ len)
{
    ಯಾವತ್ (dict_ರಾಶಿ.ga_len > len)
	dict_ರಾಶಿ_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    ಸ್ಥಿರ outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    ಯದಿ (ptref == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ &pt->pt_outer;

    // partial using partial (recursively)
    ಯಾವತ್ (ptref->pt_outer_partial != ರಿಕ್ತ)
	ptref = ptref->pt_outer_partial;
    ಪ್ರತ್ಯಾಗಮ &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_ufunc_arg_types(ufunc_T *ufunc, ಪೂರ್ಣಾಙ್ಕ argcount, ಪೂರ್ಣಾಙ್ಕ off, ectx_T *ectx)
{
    ಯದಿ (ufunc->uf_arg_types == ರಿಕ್ತ && ufunc->uf_va_type == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    typval_T	*argv = ರಾಶಿ_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < argcount; ++i)
    {
	type_T *type = ರಿಕ್ತ;

	// assume a v:none argument, using the default value, is always OK
	ಯದಿ (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
	    ಅನುವರ್ತ;
	// only pass values to user functions, never types
	ಯದಿ (check_typval_is_ಮೌಲ್ಯ(&argv[i]) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (i < ufunc->uf_args.ga_len && ufunc->uf_arg_types != ರಿಕ್ತ)
	    type = ufunc->uf_arg_types[i];
	ಅನ್ಯಥಾ ಯದಿ (ufunc->uf_va_type != ರಿಕ್ತ)
	    type = ufunc->uf_va_type->tt_member;
	ಯದಿ (type != ರಿಕ್ತ && check_typval_arg_type(type,
		    &argv[i], ರಿಕ್ತ, i + 1) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
call_dfunc(
	ಪೂರ್ಣಾಙ್ಕ		cdf_idx,
	partial_T	*pt_arg,
	ಪೂರ್ಣಾಙ್ಕ		argcount_arg,
	ectx_T		*ectx)
{
    ಪೂರ್ಣಾಙ್ಕ		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg_cumul + did_emsg;
    ಪೂರ್ಣಾಙ್ಕ		arg_to_add;
    ಪೂರ್ಣಾಙ್ಕ		vararg_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಙ್ಕ		varcount;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    estack_T	*entry;
    funclocal_T	*floc = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		res = OK;
    compiletype_T compile_type;

    ಯದಿ (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == ರಿಕ್ತ ? (char_u *)"unknown" : dfunc->df_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

#ifdef FEAT_PROFILE
    ಯದಿ (do_profiling == PROF_YES)
    {
	ಯದಿ (GA_GROW_OK(&profile_info_ga, 1))
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
    ಯದಿ (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, ರಿಕ್ತ);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    }
    ಯದಿ (res == FAIL || INSTRUCTIONS(dfunc) == ರಿಕ್ತ)
    {
	ಯದಿ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (ufunc->uf_va_name != ರಿಕ್ತ)
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
	vararg_ಎಣಿಕೆ = argcount - ufunc->uf_args.ga_len;
	ಯದಿ (vararg_ಎಣಿಕೆ < 0)
	    vararg_ಎಣಿಕೆ = 0;
	ಅನ್ಯಥಾ
	    argcount -= vararg_ಎಣಿಕೆ;
	ಯದಿ (exe_newlist(vararg_ಎಣಿಕೆ, ectx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	vararg_ಎಣಿಕೆ = 1;
    }

    arg_to_add = ufunc->uf_args.ga_len - argcount;
    ಯದಿ (arg_to_add < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
			-arg_to_add), -arg_to_add);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (arg_to_add > ufunc->uf_def_args.ga_len)
    {
	ಪೂರ್ಣಾಙ್ಕ missing = arg_to_add - ufunc->uf_def_args.ga_len;

	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
			missing), missing);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = ರಾಶಿ_TV_BOT(0) - argcount - vararg_ಎಣಿಕೆ - 1;

    ಯದಿ (IS_OBJECT_METHOD(ufunc) && !IS_CONSTRUCTOR_METHOD(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == ರಿಕ್ತ)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Check the argument types.
    ಯದಿ (check_ufunc_arg_types(ufunc, argcount, vararg_ಎಣಿಕೆ, ectx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

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
    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ,
				     arg_to_add + ರಾಶಿ_FRAME_SIZE + varcount))
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = ರಾಶಿ_TV_BOT(0) - argcount - vararg_ಎಣಿಕೆ - 1;

    // If depth of calling is getting too high, don't execute the function.
    ಯದಿ (funcdepth_increment() == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    ಯದಿ (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	ಯದಿ (floc == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    ಯದಿ (vararg_ಎಣಿಕೆ > 0 && arg_to_add > 0)
	*ರಾಶಿ_TV_BOT(arg_to_add - 1) = *ರಾಶಿ_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    ಪ್ರತಿ (idx = 0; idx < arg_to_add; ++idx)
	ರಾಶಿ_TV_BOT(idx - vararg_ಎಣಿಕೆ)->v_type = VAR_UNKNOWN;
    ectx->ec_ರಾಶಿ.ga_len += arg_to_add;

    // Store current execution state in stack frame for ISN_RETURN.
    ರಾಶಿ_TV_BOT(ರಾಶಿ_FRAME_FUNC_OFF)->vval.v_ಸಂಖ್ಯೆ = ectx->ec_dfunc_idx;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_FRAME_IIDX_OFF)->vval.v_ಸಂಖ್ಯೆ = ectx->ec_iidx;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_FRAME_INSTR_OFF)->vval.v_string = (ಶೂನ್ಯ *)ectx->ec_instr;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_FRAME_OUTER_OFF)->vval.v_string =
						    (ಶೂನ್ಯ *)ectx->ec_outer_ref;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_FRAME_FUNCLOCAL_OFF)->vval.v_string = (ಶೂನ್ಯ *)floc;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_FRAME_IDX_OFF)->vval.v_ಸಂಖ್ಯೆ = ectx->ec_frame_idx;
    ectx->ec_frame_idx = ectx->ec_ರಾಶಿ.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    ಪೂರ್ಣಾಙ್ಕ initcount = dfunc->df_varcount + (dfunc->df_has_closure ? 1 : 0);
    ಪ್ರತಿ (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = ರಾಶಿ_TV_BOT(ರಾಶಿ_FRAME_SIZE + idx);

	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ectx->ec_ರಾಶಿ.ga_len += ರಾಶಿ_FRAME_SIZE + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    ಯದಿ (IS_OBJECT_METHOD(ufunc))
    {
	ಯದಿ (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_internal_error_str), "type in stack is not an object");
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	*ರಾಶಿ_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != ರಿಕ್ತ ? pt_arg : ufunc->uf_partial;
    ಯದಿ (pt != ರಿಕ್ತ || (ufunc->uf_flags & FC_CLOSURE))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	ಯದಿ (ref == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (pt != ರಿಕ್ತ)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	ಅನ್ಯಥಾ
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    ಯದಿ (unlikely(ref->or_outer == ರಿಕ್ತ))
	    {
		vim_free(ref);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->out_ರಾಶಿ = &ectx->ec_ರಾಶಿ;
	    ref->or_outer->out_frame_idx = ectx->ec_frame_idx;
	    ಯದಿ (ectx->ec_outer_ref != ರಿಕ್ತ)
		ref->or_outer->out_up = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    ಅನ್ಯಥಾ
	ectx->ec_outer_ref = ರಿಕ್ತ;

    ಯದಿ (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    entry = estack_ತಳ್ಳು_ufunc(ufunc, 1);
    ಯದಿ (entry != ರಿಕ್ತ)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	entry->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    ಪ್ರತ್ಯಾಗಮ OK;
}

// Double linked list of funcstack_T in use.
ಸ್ಥಿರ funcstack_T *first_funcstack = ರಿಕ್ತ;

    ಸ್ಥಿರ ಶೂನ್ಯ
add_funcstack_to_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    ಯದಿ (first_funcstack != ರಿಕ್ತ)
	first_funcstack->fs_prev = funcstack;
    funcstack->fs_next = first_funcstack;
    funcstack->fs_prev = ರಿಕ್ತ;
    first_funcstack = funcstack;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_funcstack_from_list(funcstack_T *funcstack)
{
    ಯದಿ (funcstack->fs_prev == ರಿಕ್ತ)
	first_funcstack = funcstack->fs_next;
    ಅನ್ಯಥಾ
	funcstack->fs_prev->fs_next = funcstack->fs_next;
    ಯದಿ (funcstack->fs_next != ರಿಕ್ತ)
	funcstack->fs_next->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
handle_closure_in_use(ectx_T *ectx, ಪೂರ್ಣಾಙ್ಕ free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    ಪೂರ್ಣಾಙ್ಕ		argcount;
    ಪೂರ್ಣಾಙ್ಕ		ತುದಿ;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    typval_T	*tv;
    ಪೂರ್ಣಾಙ್ಕ		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_ಎಣಿಕೆ;

    ಯದಿ (dfunc->df_ufunc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;  // function was freed
    ಯದಿ (dfunc->df_has_closure == 0)
	ಪ್ರತ್ಯಾಗಮ OK;  // no closures
    tv = ರಾಶಿ_TV(ectx->ec_frame_idx + ರಾಶಿ_FRAME_SIZE + dfunc->df_varcount);
    closure_ಎಣಿಕೆ = tv->vval.v_ಸಂಖ್ಯೆ;
    ಯದಿ (closure_ಎಣಿಕೆ == 0)
	ಪ್ರತ್ಯಾಗಮ OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    ತುದಿ = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    ಪ್ರತಿ (idx = 0; idx < closure_ಎಣಿಕೆ; ++idx)
    {
	partial_T   *pt;
	ಪೂರ್ಣಾಙ್ಕ	    off = gap->ga_len - closure_ಎಣಿಕೆ + idx;

	ಯದಿ (off < 0)
	    ಅನುವರ್ತ;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	ಯದಿ (pt->pt_refcount > 1)
	{
	    ಪೂರ್ಣಾಙ್ಕ refcount = pt->pt_refcount;
	    ಪೂರ್ಣಾಙ್ಕ i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    ಪ್ರತಿ (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = ರಾಶಿ_TV(ectx->ec_frame_idx
						       + ರಾಶಿ_FRAME_SIZE + i);
		ಯದಿ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ಯದಿ (refcount > 1)
	    {
		closure_in_use = TRUE;
		ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *ರಾಶಿ;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	ಯದಿ (funcstack == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	funcstack->fs_var_offset = argcount + ರಾಶಿ_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset
							  + dfunc->df_varcount;
	ರಾಶಿ = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = ರಾಶಿ;
	ಯದಿ (ರಾಶಿ == ರಿಕ್ತ)
	{
	    vim_free(funcstack);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	ಪ್ರತಿ (idx = 0; idx < argcount; ++idx)
	{
	    tv = ರಾಶಿ_TV(ತುದಿ + idx);
	    ಯದಿ (free_arguments)
	    {
		*(ರಾಶಿ + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, ರಾಶಿ + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	ಪ್ರತಿ (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = ರಾಶಿ_TV(ectx->ec_frame_idx + ರಾಶಿ_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    ಯದಿ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ	    i;

		ಪ್ರತಿ (i = 0; i < closure_ಎಣಿಕೆ; ++i)
		    ಯದಿ (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_ಎಣಿಕೆ + i])
			++funcstack->fs_min_refcount;
	    }

	    *(ರಾಶಿ + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	ಪ್ರತಿ (idx = 0; idx < closure_ಎಣಿಕೆ; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_ಎಣಿಕೆ + idx];
	    ಯದಿ (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_ರಾಶಿ = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - ತುದಿ;
	    }
	}
    }

    ಪ್ರತಿ (idx = 0; idx < closure_ಎಣಿಕೆ; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_ಎಣಿಕೆ + idx]);
    gap->ga_len -= closure_ಎಣಿಕೆ;
    ಯದಿ (gap->ga_len == 0)
	ga_clear(gap);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    ಪೂರ್ಣಾಙ್ಕ
funcstack_check_refcount(funcstack_T *funcstack)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    garray_T	*gap = &funcstack->fs_ga;
    ಪೂರ್ಣಾಙ್ಕ		done = 0;
    typval_T	*ರಾಶಿ;

    ಯದಿ (funcstack->fs_refcount > funcstack->fs_min_refcount)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (i = funcstack->fs_var_offset; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_data) + i;

	ಯದಿ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ರಿಕ್ತ
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++done;
    }
    ಯದಿ (done != funcstack->fs_min_refcount)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ರಾಶಿ = gap->ga_data;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	clear_tv(ರಾಶಿ + i);
    vim_free(ರಾಶಿ);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_funcstacks(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    funcstack_T *funcstack;

    ಪ್ರತಿ (funcstack = first_funcstack; funcstack != ರಿಕ್ತ;
						funcstack = funcstack->fs_next)
    {
	typval_T    *ರಾಶಿ = funcstack->fs_ga.ga_data;
	ಪೂರ್ಣಾಙ್ಕ	    i;

	ಪ್ರತಿ (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    ಯದಿ (set_ref_in_item(ರಾಶಿ + i, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ))
		ಪ್ರತ್ಯಾಗಮ TRUE;  // abort
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
ಸ್ಥಿರ ectx_T *current_ectx = ರಿಕ್ತ;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    ಪೂರ್ಣಾಙ್ಕ
in_def_function(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ current_ectx != ರಿಕ್ತ;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
fill_exec_lval_ಬೇರು(lval_ಬೇರು_T *ಬೇರು)
{
    ectx_T *ectx = current_ectx;
    ಯದಿ (ectx != ರಿಕ್ತ)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	ಯದಿ (ufunc->uf_class != ರಿಕ್ತ)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    ಯದಿ (tv != ರಿಕ್ತ)
	    {
		CLEAR_POINTER(ಬೇರು);
		ಬೇರು->lr_tv = tv;
		copy_tv(ರಾಶಿ_TV_VAR(0), ಬೇರು->lr_tv);
		ಬೇರು->lr_cl_exec = ufunc->uf_class;
		++ಬೇರು->lr_cl_exec->class_refcount;
		ಪ್ರತ್ಯಾಗಮ OK;
	    }
	}
    }

    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(ಶೂನ್ಯ)
{
    ectx_T *r = current_ectx;

    current_ectx = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ r;
}

    ಶೂನ್ಯ
restore_current_ectx(ectx_T *ectx)
{
    ಯದಿ (current_ectx != ರಿಕ್ತ)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    ಸ್ಥಿರ list_T *
add_defer_item(ಪೂರ್ಣಾಙ್ಕ var_idx, ಪೂರ್ಣಾಙ್ಕ argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = ರಾಶಿ_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    ಯದಿ (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	ಯದಿ (rettv_list_alloc(defer_tv) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_ವಸ್ತುಗಳು(argcount + 1);
    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_lock = 0;
    ಯದಿ (list_ಸೇರಿಸು_tv(defer_l, &listval, defer_l->lv_first) == FAIL)
    {
	list_free(l);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
defer_command(ಪೂರ್ಣಾಙ್ಕ var_idx, ಪೂರ್ಣಾಙ್ಕ argcount, ectx_T *ectx)
{
    list_T	*l = add_defer_item(var_idx, argcount, ectx);
    ಪೂರ್ಣಾಙ್ಕ		i;
    typval_T	*func_tv;

    ಯದಿ (l == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    func_tv = ರಾಶಿ_TV_BOT(-argcount - 1);
    ಯದಿ (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    list_set_item(l, 0, func_tv);

    ಪ್ರತಿ (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, ರಾಶಿ_TV_BOT(-argcount + i));
    ectx->ec_ರಾಶಿ.ga_len -= argcount + 1;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    ಪೂರ್ಣಾಙ್ಕ
add_defer_function(char_u *name, ಪೂರ್ಣಾಙ್ಕ argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    l = add_defer_item(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    ಯದಿ (l == ರಿಕ್ತ)
    {
	vim_free(name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_lock = 0;
    func_tv.vval.v_string = name;
    list_set_item(l, 0, &func_tv);

    ಪ್ರತಿ (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, argvars + i);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = ರಾಶಿ_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    ಯದಿ (defer_tv->v_type != VAR_LIST)
	ಪ್ರತ್ಯಾಗಮ;	 // no function added
    FOR_ALL_LIST_ವಸ್ತುಗಳು(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ARGS];
	ಪೂರ್ಣಾಙ್ಕ	    i;
	listitem_T  *arg_li = l->lv_first;
	typval_T    *functv = &l->lv_first->li_tv;
	ಪೂರ್ಣಾಙ್ಕ	    argcount = l->lv_len - 1;

	ಯದಿ (functv->vval.v_string == ರಿಕ್ತ)
	    // already being called, can happen if function does ":qa"
	    ಅನುವರ್ತ;

	ಪ್ರತಿ (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_next;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = ರಿಕ್ತ;
	partial_T	*pt = ರಿಕ್ತ;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	ಯದಿ (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = ರಿಕ್ತ;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    ಯದಿ (funcexe.fe_object != ರಿಕ್ತ)
		++funcexe.fe_object->obj_refcount;
	}
	ಅನ್ಯಥಾ
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = ರಿಕ್ತ;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(ಶೂನ್ಯ)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	ಯದಿ (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	ಅನ್ಯಥಾ
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
func_return(ectx_T *ectx)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    ಪೂರ್ಣಾಙ್ಕ		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*entry;
    ಪೂರ್ಣಾಙ್ಕ		prev_dfunc_idx = ರಾಶಿ_TV(ectx->ec_frame_idx
					+ ರಾಶಿ_FRAME_FUNC_OFF)->vval.v_ಸಂಖ್ಯೆ;
    funclocal_T	*floc;
#ifdef FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_data)
							      + prev_dfunc_idx;

    ಯದಿ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	ಯದಿ (dfunc->df_ufunc->uf_profiling
				   || (caller != ರಿಕ್ತ && caller->uf_profiling))
	{
	    profile_may_end_func(((profinfo_T *)profile_info_ga.ga_data)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#endif

    ಯದಿ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    ಯದಿ (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    entry = estack_ತೆಗೆ();
    ಯದಿ (entry != ರಿಕ್ತ)
	current_sctx = entry->es_save_sctx;

    ಯದಿ (handle_closure_in_use(ectx, TRUE) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    ಪೂರ್ಣಾಙ್ಕ ತುದಿ = ectx->ec_frame_idx - argcount;
    ಯದಿ (IS_OBJECT_METHOD(dfunc->df_ufunc))
	--ತುದಿ;
    ಪ್ರತಿ (idx = ತುದಿ; idx < ectx->ec_frame_idx; ++idx)
	clear_tv(ರಾಶಿ_TV(idx));

    // Clear local variables and temp values, but not the return value.
    ಪ್ರತಿ (idx = ectx->ec_frame_idx + ರಾಶಿ_FRAME_SIZE;
					idx < ectx->ec_ರಾಶಿ.ga_len - 1; ++idx)
	clear_tv(ರಾಶಿ_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_ರಾಶಿ.ga_len - 1;
    ಯದಿ (ret_idx == ectx->ec_frame_idx + ರಾಶಿ_FRAME_IDX_OFF)
	ret_idx = 0;

    ಯದಿ (ectx->ec_outer_ref != ರಿಕ್ತ)
    {
	ಯದಿ (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = ರಾಶಿ_TV(ectx->ec_frame_idx
					+ ರಾಶಿ_FRAME_IIDX_OFF)->vval.v_ಸಂಖ್ಯೆ;
    ectx->ec_instr = (ಶೂನ್ಯ *)ರಾಶಿ_TV(ectx->ec_frame_idx
				       + ರಾಶಿ_FRAME_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (ಶೂನ್ಯ *)ರಾಶಿ_TV(ectx->ec_frame_idx
				       + ರಾಶಿ_FRAME_OUTER_OFF)->vval.v_string;
    floc = (ಶೂನ್ಯ *)ರಾಶಿ_TV(ectx->ec_frame_idx
				   + ರಾಶಿ_FRAME_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_frame_idx = ರಾಶಿ_TV(ectx->ec_frame_idx
				       + ರಾಶಿ_FRAME_IDX_OFF)->vval.v_ಸಂಖ್ಯೆ;

    ಯದಿ (floc == ರಿಕ್ತ)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    ಅನ್ಯಥಾ
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    ಯದಿ (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_ರಾಶಿ.ga_len = ತುದಿ + 1;
	*ರಾಶಿ_TV_BOT(-1) = *ರಾಶಿ_TV(ret_idx);
    }
    ಅನ್ಯಥಾ
	// Reset the stack to the position before the call.
	ectx->ec_ರಾಶಿ.ga_len = ತುದಿ;

    funcdepth_decrement();
    --ex_nesting_level;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
call_prepare(ಪೂರ್ಣಾಙ್ಕ argcount, typval_T *argvars, ectx_T *ectx)
{
    ಪೂರ್ಣಾಙ್ಕ		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    ಪ್ರತಿ (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *ರಾಶಿ_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    ಯದಿ (argcount > 0)
	ectx->ec_ರಾಶಿ.ga_len -= argcount - 1;
    ಅನ್ಯಥಾ ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಅನ್ಯಥಾ
	++ectx->ec_ರಾಶಿ.ga_len;

    // Default return value is zero.
    tv = ರಾಶಿ_TV_BOT(-1);
    tv->v_type = VAR_ಸಂಖ್ಯೆ;
    tv->vval.v_ಸಂಖ್ಯೆ = 0;
    tv->v_lock = 0;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Call a builtin function by index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
call_bfunc(ಪೂರ್ಣಾಙ್ಕ func_idx, ಪೂರ್ಣಾಙ್ಕ argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    ವರ್ಣ	*save_func_name = ectx->ec_where.wt_func_name;

    ಯದಿ (call_prepare(argcount, argvars, ectx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Check for void value being passed as an argument.
    ಪ್ರತಿ (idx = 0; idx < argcount; ++idx)
	ಯದಿ (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ಪ್ರತಿ (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

    ectx->ec_where.wt_func_name = internal_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_internal_func_by_idx(func_idx, argvars, ರಾಶಿ_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    ಪ್ರತಿ (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ಯದಿ (did_emsg > did_emsg_before)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    funcexe_T   funcexe;
    funcerror_T	error;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    ಯದಿ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, ರಿಕ್ತ)
								       == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (ufunc->uf_def_status == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	ಯದಿ (error != FCERR_UNKNOWN)
	{
	    ಯದಿ (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    ಅನ್ಯಥಾ
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	ಯದಿ (iptr != ರಿಕ್ತ)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	ಪ್ರತ್ಯಾಗಮ call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    ಯದಿ (call_prepare(argcount, argvars, ectx) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != ರಿಕ್ತ ? selfdict : dict_ರಾಶಿ_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      ರಾಶಿ_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    ಪ್ರತಿ (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ಯದಿ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_found_var);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * If command modifiers were applied restore them.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_restore_cmdmod(funclocal_T *funclocal)
{
    ಯದಿ (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ರಿಕ್ತ;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vim9_aborting(ಪೂರ್ಣಾಙ್ಕ prev_uncaught_emsg)
{
    ಪ್ರತ್ಯಾಗಮ uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
call_by_name(
	char_u	    *name,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    ಯದಿ (builtin_function(name, -1))
    {
	ಪೂರ್ಣಾಙ್ಕ func_idx = find_internal_func(name);

	ಯದಿ (func_idx < 0)  // Impossible?
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಯದಿ (check_internal_func(func_idx, argcount) < 0)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಪ್ರತ್ಯಾಗಮ call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*start_bracket = generic_func_find_open_bracket(name);
    ಯದಿ (start_bracket != ರಿಕ್ತ)
    {
	cc = *start_bracket;
	*start_bracket = NUL;
    }

    ufunc = find_func(name, FALSE);

    ಯದಿ (ufunc == ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ prev_uncaught_emsg = uncaught_emsg;

	ಯದಿ (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = find_func(name, FALSE);

	ಯದಿ (vim9_aborting(prev_uncaught_emsg))
	{
	    ಯದಿ (start_bracket != ರಿಕ್ತ)
		*start_bracket = cc;
	    ಪ್ರತ್ಯಾಗಮ FAIL;  // bail out if loading the script caused an error
	}
    }

    ಯದಿ (start_bracket != ರಿಕ್ತ)
	*start_bracket = cc;

    ಯದಿ (ufunc != ರಿಕ್ತ)
    {
	ಯದಿ (IS_GENERIC_FUNC(ufunc))
	{
	    ಯದಿ (start_bracket != ರಿಕ್ತ)
		ufunc = find_generic_func(ufunc, name, &start_bracket);
	    ಅನ್ಯಥಾ
	    {
		emsg_funcname(e_generic_func_missing_type_args_str, name);
		ufunc = ರಿಕ್ತ;
	    }
	    ಯದಿ (ufunc == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (start_bracket != ರಿಕ್ತ)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}

	ಯದಿ (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಪ್ರತ್ಯಾಗಮ call_ufunc(ufunc, ರಿಕ್ತ, argcount, ectx, iptr, selfdict);
    }

    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
call_partial(
	typval_T    *tv,
	ಪೂರ್ಣಾಙ್ಕ	    argcount_arg,
	ectx_T	    *ectx)
{
    ಪೂರ್ಣಾಙ್ಕ		argcount = argcount_arg;
    char_u	*name = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಙ್ಕ		res = FAIL;
    dict_T	*selfdict = ರಿಕ್ತ;

    ಯದಿ (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	ಪೂರ್ಣಾಙ್ಕ	    i;

	ಯದಿ (pt->pt_obj != ರಿಕ್ತ)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಪ್ರತಿ (i = 1; i <= argcount; ++i)
		*ರಾಶಿ_TV_BOT(-i + 1) = *ರಾಶಿ_TV_BOT(-i);

	    typval_T *obj_tv = ರಾಶಿ_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_lock = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}

	ಯದಿ (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, pt->pt_argc))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಪ್ರತಿ (i = 1; i <= argcount; ++i)
		*ರಾಶಿ_TV_BOT(-i + pt->pt_argc) = *ರಾಶಿ_TV_BOT(-i);
	    ectx->ec_ರಾಶಿ.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    ಪ್ರತಿ (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], ರಾಶಿ_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	ಯದಿ (pt->pt_func != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ call_ufunc(pt->pt_func, pt, argcount, ectx, ರಿಕ್ತ, selfdict);

	name = pt->pt_name;
    }
    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    ಯದಿ (name != ರಿಕ್ತ)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = ರಿಕ್ತ;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ಯದಿ (error != FCERR_NONE)
	    res = FAIL;
	ಅನ್ಯಥಾ
	    res = call_by_name(fname, argcount, ectx, ರಿಕ್ತ, selfdict);
	vim_free(tofree);
    }

    ಯದಿ (res == FAIL)
    {
	ಯದಿ (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == ರಿಕ್ತ ? (char_u *)"[unknown]" : name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
error_if_locked(ಪೂರ್ಣಾಙ್ಕ lock, ವರ್ಣ *error)
{
    ಯದಿ (lock & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
check_for_ಸಂಖ್ಯೆ(typval_T *tv)
{
    ಯದಿ (tv->v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_ಸಂಖ್ಯೆ), vartype_name(tv->v_type));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    ಪೂರ್ಣಾಙ್ಕ		    flags = ASSIGN_DECL;

    ಯದಿ (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, ರಿಕ್ತ, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_2string(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ is_2string_any, ಪೂರ್ಣಾಙ್ಕ tostring_flags)
{
    ಯದಿ (tv->v_type == VAR_STRING)
	ಪ್ರತ್ಯಾಗಮ OK;

    char_u *str;

    ಯದಿ (is_2string_any)
    {
	ವಿಕಲ್ಪ (tv->v_type)
	{
	    ಪಕ್ಷ VAR_SPECIAL:
	    ಪಕ್ಷ VAR_BOOL:
	    ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    ಪಕ್ಷ VAR_FLOAT:
	    ಪಕ್ಷ VAR_DICT:
	    ಪಕ್ಷ VAR_BLOB:	ಭಙ್ಗ;

	    ಪಕ್ಷ VAR_LIST:
				ಯದಿ (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, ಪರಿಮಾಣ(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    ಯದಿ (str == ರಿಕ್ತ)
					ಪ್ರತ್ಯಾಗಮ FAIL;
				    s = str;
				    ಯಾವತ್ ((e = vim_strchr(s, '\n')) != ರಿಕ್ತ)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					ಯದಿ (p != ರಿಕ್ತ)
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
				    ಪ್ರತ್ಯಾಗಮ OK;
				}
				ಯದಿ (tostring_flags & TOSTRING_INTERPOLATE)
				    ಭಙ್ಗ;
				// FALLTHROUGH
	    ಸ್ವಭಾವ:	to_string_error(tv->v_type);
			ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_LIST:
	    ಯದಿ (tv->vval.v_list == ರಿಕ್ತ && sv->sv_type != &t_list_empty)
		(ಶೂನ್ಯ)rettv_list_alloc(tv);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_TUPLE:
	    ಯದಿ (tv->vval.v_tuple == ರಿಕ್ತ && sv->sv_type != &t_tuple_empty)
		(ಶೂನ್ಯ)rettv_tuple_alloc(tv);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_DICT:
	    ಯದಿ (tv->vval.v_dict == ರಿಕ್ತ && sv->sv_type != &t_dict_empty)
		(ಶೂನ್ಯ)rettv_dict_alloc(tv);
	    ಭಙ್ಗ;
	ಪಕ್ಷ VAR_BLOB:
	    ಯದಿ (tv->vval.v_blob == ರಿಕ್ತ && sv->sv_type != &t_blob_null)
		(ಶೂನ್ಯ)rettv_blob_alloc(tv);
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
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

    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    ಯದಿ (index < 0)
    {
	ಪೂರ್ಣಾಙ್ಕ	clen = 0;

	ಪ್ರತಿ (nbyte = 0; nbyte < slen; ++clen)
	{
	    ಯದಿ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    ಅನ್ಯಥಾ ಯದಿ (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    ಅನ್ಯಥಾ
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + index;
	ಯದಿ (nchar < 0)
	    // unlike list: index out of range results in empty string
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತಿ (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	ಯದಿ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	ಅನ್ಯಥಾ ಯದಿ (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	ಅನ್ಯಥಾ
	    nbyte += mb_ptr2len(str + nbyte);
    }
    ಯದಿ (nbyte >= slen)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    ಸ್ಥಿರ long
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    ಯದಿ (nchar >= 0)
    {
	ಯಾವತ್ (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    ಅನ್ಯಥಾ
    {
	nbyte = str_len;
	ಯಾವತ್ (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_head_off(str, str + nbyte);
	    ++nchar;
	}
	ಯದಿ (nchar < 0)
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
    ಪ್ರತ್ಯಾಗಮ (long)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_slice(char_u *str, varnumber_T first, varnumber_T last, ಪೂರ್ಣಾಙ್ಕ exclusive)
{
    long	start_byte, end_byte;
    size_t	slen;

    ಯದಿ (str == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    slen = STRLEN(str);
    start_byte = char_idx2byte(str, slen, first);
    ಯದಿ (start_byte < 0)
	start_byte = 0; // first index very negative: use zero
    ಯದಿ ((last == -1 && !exclusive) || last == VARNUM_MAX)
	end_byte = (long)slen;
    ಅನ್ಯಥಾ
    {
	end_byte = char_idx2byte(str, slen, last);
	ಯದಿ (!exclusive && end_byte >= 0 && end_byte < (long)slen)
	    // end index is inclusive
	    end_byte += mb_ptr2len(str + end_byte);
    }

    ಯದಿ (start_byte >= (long)slen || end_byte <= start_byte)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ vim_strnsave(str + start_byte, end_byte - start_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    ಸ್ಥಿರ svar_T *
get_script_svar(scriptref_T *sref, ಪೂರ್ಣಾಙ್ಕ dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? ರಿಕ್ತ
			      : ((dfunc_T *)def_functions.ga_data) + dfunc_idx;
    svar_T	    *sv;

    ಯದಿ (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	ಯದಿ (dfunc != ರಿಕ್ತ)
	    semsg(_(e_script_variable_invalid_after_reload_in_function_str),
					 printable_func_name(dfunc->df_ufunc));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_data) + sref->sref_idx;
    ಯದಿ (sv->sv_name == ರಿಕ್ತ)
    {
	ಯದಿ (dfunc != ರಿಕ್ತ)
	    emsg(_(e_script_variable_was_deleted));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	ಯದಿ (dfunc != ರಿಕ್ತ)
	    emsg(_(e_script_variable_type_changed));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	ಯದಿ (dfunc != ರಿಕ್ತ)
	    semsg(_(e_item_not_exported_in_script_str), sv->sv_name);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಪ್ರತ್ಯಾಗಮ sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    ಸ್ಥಿರ char_u *
get_split_sourceline(
	ಪೂರ್ಣಾಙ್ಕ c UNUSED,
	ಶೂನ್ಯ *cookie,
	ಪೂರ್ಣಾಙ್ಕ indent UNUSED,
	getline_opt_T options UNUSED)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*p;
    char_u		*ಸಾಲು;

    p = vim_strchr(sp->nextline, '\n');
    ಯದಿ (p == ರಿಕ್ತ)
    {
	ಸಾಲು = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    ಅನ್ಯಥಾ
    {
	ಸಾಲು = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    ಪ್ರತ್ಯಾಗಮ ಸಾಲು;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
call_eval_func(
	char_u	    *name,
	ಪೂರ್ಣಾಙ್ಕ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    ಪೂರ್ಣಾಙ್ಕ	    called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಙ್ಕ	    res;

    res = call_by_name(name, argcount, ectx, iptr, ರಿಕ್ತ);
    ಯದಿ (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = find_var(name, ರಿಕ್ತ, FALSE);
	ಯದಿ (v == ರಿಕ್ತ || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಪ್ರತ್ಯಾಗಮ call_partial(&v->di_tv, argcount, ectx);
    }
    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    ಪೂರ್ಣಾಙ್ಕ
fill_partial_and_closure(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ಯದಿ (ufunc->uf_flags & FC_CLOSURE)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.out_ರಾಶಿ = &ectx->ec_ರಾಶಿ;
	pt->pt_outer.out_frame_idx = ectx->ec_frame_idx;
	ಯದಿ (ectx->ec_outer_ref != ರಿಕ್ತ)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.out_up = ectx->ec_outer_ref->or_outer;
	    ಯದಿ (ectx->ec_outer_ref->or_partial != ರಿಕ್ತ)
	    {
		pt->pt_outer.out_up_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.out_up_partial->pt_refcount;
	    }
	}

	ಯದಿ (lvi != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ	depth;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    ಪ್ರತಿ (depth = 0; depth < lvi->lvi_depth; ++depth)
	    {
		pt->pt_outer.out_loop[depth].ರಾಶಿ = &ectx->ec_ರಾಶಿ;
		pt->pt_outer.out_loop[depth].var_idx = ectx->ec_frame_idx
			 + ರಾಶಿ_FRAME_SIZE + lvi->lvi_loop[depth].var_idx;
		pt->pt_outer.out_loop[depth].var_ಎಣಿಕೆ =
					    lvi->lvi_loop[depth].var_ಎಣಿಕೆ;
	    }
	    pt->pt_outer.out_loop_size = lvi->lvi_depth;
	}
	ಅನ್ಯಥಾ
	    pt->pt_outer.out_loop_size = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	ಯದಿ (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_ರಾಶಿ.ga_data) + ectx->ec_frame_idx
		       + ರಾಶಿ_FRAME_SIZE + dfunc->df_varcount)->vval.v_ಸಂಖ್ಯೆ;

	((partial_T **)ectx->ec_funcrefs.ga_data)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exec_command(isn_T *iptr, char_u *cmd_string)
{
    source_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_FIELD(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    ಯದಿ (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    ಪೂರ್ಣಾಙ್ಕ
may_load_script(ಪೂರ್ಣಾಙ್ಕ sid, ಪೂರ್ಣಾಙ್ಕ *loaded)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);

    ಯದಿ (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	ಯದಿ (do_source(si->sn_name, FALSE, DOSO_NONE, ರಿಕ್ತ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

// used for v_instr of typval of VAR_INSTR
ಸಂರಚನಾ instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ subs_ಅಭಿವ್ಯಕ್ತಿ_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    ಪೂರ್ಣಾಙ್ಕ		subs_status;
} subs_ಅಭಿವ್ಯಕ್ತಿ_T;

// Set when calling do_debug().
ಸ್ಥಿರ ectx_T	*debug_context = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	debug_var_ಎಣಿಕೆ;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    ಪೂರ್ಣಾಙ್ಕ		    varargs_off;

    ಯದಿ (ectx == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    ಪ್ರತಿ (idx = debug_var_ಎಣಿಕೆ - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_names.ga_data)[idx];

	// the variable name may be NULL when not available in this block
	ಯದಿ (varname != ರಿಕ್ತ && STRCMP(varname, name) == 0)
	    ಪ್ರತ್ಯಾಗಮ ರಾಶಿ_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == ರಿಕ್ತ ? 0 : 1;
    ಪ್ರತಿ (idx = 0; idx < ufunc->uf_args.ga_len; ++idx)
	ಯದಿ (STRCMP(((char_u **)(ufunc->uf_args.ga_data))[idx], name) == 0)
	    ಪ್ರತ್ಯಾಗಮ ರಾಶಿ_TV(ectx->ec_frame_idx - ufunc->uf_args.ga_len
							  - varargs_off + idx);
    ಯದಿ (ufunc->uf_va_name != ರಿಕ್ತ && STRCMP(ufunc->uf_va_name, name) == 0)
	ಪ್ರತ್ಯಾಗಮ ರಾಶಿ_TV(ectx->ec_frame_idx - 1);

    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    ಪೂರ್ಣಾಙ್ಕ
may_break_in_function(ufunc_T *ufunc)
{
    ಪ್ರತ್ಯಾಗಮ ufunc->uf_has_breakpoint || debug_has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*ಸಾಲು;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    ಪೂರ್ಣಾಙ್ಕ		end_lnum = iptr->isn_lnum;
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		lnum;

    ಯದಿ (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	ಯದಿ (!may_break_in_function(ufunc))
	    ಪ್ರತ್ಯಾಗಮ;

	// check for the next breakpoint if needed
	breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	ಯದಿ (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_ಎಣಿಕೆ = iptr->isn_arg.debug.dbg_var_names_len;

    ಪ್ರತಿ (ni = iptr + 1; ni->isn_type != ISN_FINISH; ++ni)
	ಯದಿ (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    end_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    ಭಙ್ಗ;
	}

    ಯದಿ (end_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, ಪರಿಮಾಣ(char_u *), 10);
	ಪ್ರತಿ (lnum = iptr->isn_lnum; lnum < end_lnum
				     && lnum <= ufunc->uf_ಸಾಲುಗಳು.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_ಸಾಲುಗಳು.ga_data)[lnum - 1];

	    ಯದಿ (p == ರಿಕ್ತ)
		ಅನುವರ್ತ;  // left over from continuation line
	    p = skipwhite(p);
	    ಯದಿ (*p == '#')
		ಭಙ್ಗ;
	    ಯದಿ (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_data))[ga.ga_len++] = p;
	    ಯದಿ (STRNCMP(p, "def ", 4) == 0)
		ಭಙ್ಗ;
	}
	ಸಾಲು = ga_concat_strings(&ga, "  ");
	vim_free(ga.ga_data);
    }
    ಅನ್ಯಥಾ
	ಸಾಲು = ((char_u **)ufunc->uf_ಸಾಲುಗಳು.ga_data)[iptr->isn_lnum - 1];

    do_debug(ಸಾಲು == ರಿಕ್ತ ? (char_u *)"[empty]" : ಸಾಲು);
    debug_context = ರಿಕ್ತ;

    ಯದಿ (end_lnum > iptr->isn_lnum)
	vim_free(ಸಾಲು);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    ಯದಿ (lhs_list->lv_type == ರಿಕ್ತ || lhs_list->lv_type->tt_member == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಪ್ರತ್ಯಾಗಮ check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = ರಾಶಿ_TV_BOT(-2);
    long	lidx = 0;
    typval_T	*tv_dest = ರಾಶಿ_TV_BOT(-1);
    ಪೂರ್ಣಾಙ್ಕ		status = OK;
    ಪೂರ್ಣಾಙ್ಕ		check_rhs_type = FALSE;

    ಯದಿ (tv_idx->v_type == VAR_ಸಂಖ್ಯೆ)
	lidx = (long)tv_idx->vval.v_ಸಂಖ್ಯೆ;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = ರಾಶಿ_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    ಯದಿ (dest_type == VAR_OBJECT && tv_dest->vval.v_object == ರಿಕ್ತ)
    {
	emsg(_(e_using_null_object));
	status = FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	ಯದಿ (dest_type == VAR_DICT)
	    status = do_2string(tv_idx, TRUE, FALSE);
	ಅನ್ಯಥಾ ಯದಿ (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    ಯದಿ (obj == ರಿಕ್ತ)
	    {
		emsg(_(e_using_null_object));
		status = FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		ಪೂರ್ಣಾಙ್ಕ		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		ಯದಿ (m != ರಿಕ್ತ)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    ಯದಿ ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == ರಿಕ್ತ ||
			     !class_instance_of(ufunc->uf_class, cl)))
		    {
			ವರ್ಣ *ಸಂದೇಶ = (m->ocm_access == VIM_ACCESS_PRIVATE)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(ಸಂದೇಶ, m->ocm_name.string, 0, cl);
			status = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    ಅನ್ಯಥಾ ಯದಿ (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			status = FAIL;
		    ಅನ್ಯಥಾ
			lidx = m_idx;
		}
		ಅನ್ಯಥಾ
		{
		    member_not_found_ಸಂದೇಶ(cl, VAR_OBJECT, member, 0);
		    status = FAIL;
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    status = FAIL;
	}
	ಅನ್ಯಥಾ ಯದಿ (dest_type == VAR_LIST && tv_idx->v_type != VAR_ಸಂಖ್ಯೆ)
	{
	    emsg(_(e_ಸಂಖ್ಯೆ_expected));
	    status = FAIL;
	}
    }

    ಯದಿ (status == OK)
    {
	ಯದಿ (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    ಯದಿ (list == ರಿಕ್ತ)
	    {
		emsg(_(e_list_not_set));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    ಯದಿ (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_index_out_of_range_nr), lidx);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    ಯದಿ (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		ಪ್ರತ್ಯಾಗಮ FAIL;

	    ಯದಿ (lidx < list->lv_len)
	    {
		listitem_T *li = list_find(list, lidx);
		ಯದಿ (li == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		ಯದಿ (error_if_locked(li->li_tv.v_lock,
					     e_cannot_change_locked_list_item))
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (error_if_locked(list->lv_lock, e_cannot_change_locked_list))
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		// append to list, only fails when out of memory
		ಯದಿ (list_append_tv(list, tv) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ NOTDONE;
		clear_tv(tv);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (dest_type == VAR_DICT)
	{
	    char_u		*ಕೀಲಿ = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    ಯದಿ (dict == ರಿಕ್ತ)
	    {
		emsg(_(e_dictionary_not_set));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
		ಕೀಲಿ = (char_u *)"";
	    di = dict_find(dict, ಕೀಲಿ, -1);
	    ಯದಿ (di != ರಿಕ್ತ)
	    {
		ಯದಿ (error_if_locked(di->di_tv.v_lock,
						    e_cannot_change_dict_item))
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (error_if_locked(dict->dv_lock, e_cannot_change_dict))
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		// add to dict, only fails when out of memory
		ಯದಿ (dict_add_tv(dict, (ವರ್ಣ *)ಕೀಲಿ, tv) == FAIL)
		    ಪ್ರತ್ಯಾಗಮ NOTDONE;
		clear_tv(tv);
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    ಪೂರ್ಣಾಙ್ಕ		    error = FALSE;
	    ಪೂರ್ಣಾಙ್ಕ		    len;

	    ಯದಿ (blob == ರಿಕ್ತ)
	    {
		emsg(_(e_blob_not_set));
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    len = blob_len(blob);
	    ಯದಿ (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    ಯದಿ (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), lidx);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    ಯದಿ (ಮೌಲ್ಯ_check_lock(blob->bv_lock, (char_u *)"blob", FALSE))
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    nr = tv_get_ಸಂಖ್ಯೆ_chk(tv, &error);
	    ಯದಿ (error)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    ಯದಿ (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_blob_nr), nr);
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    }
	    blob_set_append(blob, lidx, nr);
	}
	ಅನ್ಯಥಾ ಯದಿ (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    ಯದಿ (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		ಯದಿ (itf != ರಿಕ್ತ)
		    // convert interface member index to class member index
		    lidx = object_index_from_itf_index(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    ಅನ್ಯಥಾ
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	ಅನ್ಯಥಾ ಯದಿ (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    status = FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    status = FAIL;
	    semsg(_(e_cannot_index_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ರಾಶಿ.ga_len -= 3;
    ಯದಿ (status == FAIL)
    {
	clear_tv(tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Store a value in a list or blob range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = ರಾಶಿ_TV_BOT(-3);
    typval_T	*tv_idx2 = ರಾಶಿ_TV_BOT(-2);
    typval_T	*tv_dest = ರಾಶಿ_TV_BOT(-1);
    ಪೂರ್ಣಾಙ್ಕ		status = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = ರಾಶಿ_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    ಯದಿ (tv_dest->v_type == VAR_LIST)
    {
	long	    n1;
	long	    n2;
	listitem_T  *li1;

	n1 = (long)tv_get_ಸಂಖ್ಯೆ_chk(tv_idx1, ರಿಕ್ತ);
	ಯದಿ (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	ಅನ್ಯಥಾ
	    n2 = (long)tv_get_ಸಂಖ್ಯೆ_chk(tv_idx2, ರಿಕ್ತ);

	li1 = check_range_index_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	ಯದಿ (li1 == ರಿಕ್ತ)
	    status = FAIL;
	ಅನ್ಯಥಾ
	{
	    status = check_range_index_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    ಯದಿ (status != FAIL)
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
    ಅನ್ಯಥಾ ಯದಿ (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	long	    bloblen;

	n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv_idx1, ರಿಕ್ತ);
	ಯದಿ (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	ಅನ್ಯಥಾ
	    n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv_idx2, ರಿಕ್ತ);
	bloblen = blob_len(tv_dest->vval.v_blob);

	ಯದಿ (check_blob_index(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    status = FAIL;
	ಅನ್ಯಥಾ
	    status = blob_set_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    ಅನ್ಯಥಾ
    {
	status = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ರಾಶಿ.ga_len -= 4;
    clear_tv(tv);

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Unlet item in list or dict variable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = ರಾಶಿ_TV_BOT(-2);
    typval_T	*tv_dest = ರಾಶಿ_TV_BOT(-1);
    ಪೂರ್ಣಾಙ್ಕ		status = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    ಯದಿ (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	ಯದಿ (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_ಸಂಖ್ಯೆ)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    status = FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*ಕೀಲಿ;
	    dictitem_T  *di = ರಿಕ್ತ;

	    ಯದಿ (d != ರಿಕ್ತ && ಮೌಲ್ಯ_check_lock(
				      d->dv_lock, ರಿಕ್ತ, FALSE))
		status = FAIL;
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (tv_idx->v_type == VAR_STRING)
		{
		    ಕೀಲಿ = tv_idx->vval.v_string;
		    ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
			ಕೀಲಿ = (char_u *)"";
		}
		ಅನ್ಯಥಾ
		{
		    ಕೀಲಿ = tv_get_string(tv_idx);
		}
		ಯದಿ (d != ರಿಕ್ತ)
		    di = dict_find(d, ಕೀಲಿ, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ಕೀಲಿ));
		ಯದಿ (di == ರಿಕ್ತ)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str), ಕೀಲಿ);
		    status = FAIL;
		}
		ಅನ್ಯಥಾ ಯದಿ (var_check_fixed(di->di_flags,
						   ರಿಕ್ತ, FALSE)
			|| var_check_ro(di->di_flags,
						  ರಿಕ್ತ, FALSE))
		    status = FAIL;
		ಅನ್ಯಥಾ
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	ಯದಿ (check_for_ಸಂಖ್ಯೆ(tv_idx) == FAIL)
	{
	    status = FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n = (long)tv_idx->vval.v_ಸಂಖ್ಯೆ;

	    ಯದಿ (l != ರಿಕ್ತ && ಮೌಲ್ಯ_check_lock(
				      l->lv_lock, ರಿಕ್ತ, FALSE))
		status = FAIL;
	    ಅನ್ಯಥಾ
	    {
		listitem_T	*li = list_find(l, n);

		ಯದಿ (li == ರಿಕ್ತ)
		{
		    semsg(_(e_list_index_out_of_range_nr), n);
		    status = FAIL;
		}
		ಅನ್ಯಥಾ
		    listitem_remove(l, li);
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	status = FAIL;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ರಾಶಿ.ga_len -= 2;

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Unlet a range of items in a list variable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = ರಾಶಿ_TV_BOT(-3);
    typval_T	*tv_idx2 = ರಾಶಿ_TV_BOT(-2);
    typval_T	*tv_dest = ರಾಶಿ_TV_BOT(-1);
    ಪೂರ್ಣಾಙ್ಕ		status = OK;

    ಯದಿ (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	ಯದಿ (check_for_ಸಂಖ್ಯೆ(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_ಸಂಖ್ಯೆ(tv_idx2) == FAIL))
	{
	    status = FAIL;
	}
	ಅನ್ಯಥಾ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n1 = (long)tv_idx1->vval.v_ಸಂಖ್ಯೆ;
	    long	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (long)tv_idx2->vval.v_ಸಂಖ್ಯೆ;
	    listitem_T	*li;

	    li = list_find_index(l, &n1);
	    ಯದಿ (li == ರಿಕ್ತ)
	    {
		semsg(_(e_list_index_out_of_range_nr),
						 (long)tv_idx1->vval.v_ಸಂಖ್ಯೆ);
		status = FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		ಯದಿ (n1 < 0)
		    n1 = list_idx_of_item(l, li);
		ಯದಿ (n2 < 0)
		{
		    listitem_T *li2 = list_find(l, n2);

		    ಯದಿ (li2 == ರಿಕ್ತ)
		    {
			semsg(_(e_list_index_out_of_range_nr), n2);
			status = FAIL;
		    }
		    ಅನ್ಯಥಾ
			n2 = list_idx_of_item(l, li2);
		}
		ಯದಿ (status != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_index_out_of_range_nr), n2);
		    status = FAIL;
		}
		ಯದಿ (status != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	status = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ರಾಶಿ.ga_len -= 3;

    ಪ್ರತ್ಯಾಗಮ status;
}

/*
 * Top of a for loop.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    ಪೂರ್ಣಾಙ್ಕ		jump = FALSE;
    typval_T	*ltv = ರಾಶಿ_TV_BOT(-1);
    typval_T	*idxtv =
		   ರಾಶಿ_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಯದಿ (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_ಸಂಖ್ಯೆ;
	ಯದಿ (list == ರಿಕ್ತ
		       || idxtv->vval.v_ಸಂಖ್ಯೆ >= list->lv_len)
	{
	    jump = TRUE;
	}
	ಅನ್ಯಥಾ ಯದಿ (list->lv_first == &range_list_item)
	{
	    // non-materialized range() list
	    tv = ರಾಶಿ_TV_BOT(0);
	    tv->v_type = VAR_ಸಂಖ್ಯೆ;
	    tv->v_lock = 0;
	    tv->vval.v_ಸಂಖ್ಯೆ = list_find_nr(
			     list, idxtv->vval.v_ಸಂಖ್ಯೆ, ರಿಕ್ತ);
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}
	ಅನ್ಯಥಾ
	{
	    listitem_T *li = list_find(list,
					 idxtv->vval.v_ಸಂಖ್ಯೆ);
	    ಯದಿ (li == ರಿಕ್ತ)
		ಪ್ರತ್ಯಾಗಮ FAIL;
	    copy_tv(&li->li_tv, ರಾಶಿ_TV_BOT(0));
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_ಸಂಖ್ಯೆ;
	ಯದಿ (tuple == ರಿಕ್ತ || idxtv->vval.v_ಸಂಖ್ಯೆ >= TUPLE_LEN(tuple))
	    jump = TRUE;
	ಅನ್ಯಥಾ
	{
	    copy_tv(TUPLE_ITEM(tuple, idxtv->vval.v_ಸಂಖ್ಯೆ), ರಾಶಿ_TV_BOT(0));
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_ಸಂಖ್ಯೆ;
	ಯದಿ (str == ರಿಕ್ತ || str[idxtv->vval.v_ಸಂಖ್ಯೆ] == NUL)
	{
	    jump = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ	clen = mb_ptr2len(str + idxtv->vval.v_ಸಂಖ್ಯೆ);

	    // Push the next character from the string.
	    tv = ರಾಶಿ_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_ಸಂಖ್ಯೆ, clen);
	    ++ectx->ec_ರಾಶಿ.ga_len;
	    idxtv->vval.v_ಸಂಖ್ಯೆ += clen - 1;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	ಯದಿ (idxtv->vval.v_ಸಂಖ್ಯೆ == -1 && blob != ರಿಕ್ತ)
	{
	    ltv->v_lock = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_ಸಂಖ್ಯೆ;
	ಯದಿ (blob == ರಿಕ್ತ || idxtv->vval.v_ಸಂಖ್ಯೆ >= blob_len(blob))
	{
	    jump = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    // Push the next byte from the blob.
	    tv = ರಾಶಿ_TV_BOT(0);
	    tv->v_type = VAR_ಸಂಖ್ಯೆ;
	    tv->vval.v_ಸಂಖ್ಯೆ = blob_get(blob,
					 idxtv->vval.v_ಸಂಖ್ಯೆ);
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}
    }
    ಅನ್ಯಥಾ
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_end;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    ಅನ್ಯಥಾ
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_ಸಂಖ್ಯೆ = ectx->ec_funcrefs.ga_len;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
ಸ್ಥಿರ loopvars_T *first_loopvars = ರಿಕ್ತ;

    ಸ್ಥಿರ ಶೂನ್ಯ
add_loopvars_to_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    ಯದಿ (first_loopvars != ರಿಕ್ತ)
	first_loopvars->lvs_prev = loopvars;
    loopvars->lvs_next = first_loopvars;
    loopvars->lvs_prev = ರಿಕ್ತ;
    first_loopvars = loopvars;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_loopvars_from_list(loopvars_T *loopvars)
{
    ಯದಿ (loopvars->lvs_prev == ರಿಕ್ತ)
	first_loopvars = loopvars->lvs_next;
    ಅನ್ಯಥಾ
	loopvars->lvs_prev->lvs_next = loopvars->lvs_next;
    ಯದಿ (loopvars->lvs_next != ರಿಕ್ತ)
	loopvars->lvs_next->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = ರಾಶಿ_TV_VAR(endloop->end_funcref_idx);
    ಪೂರ್ಣಾಙ್ಕ		prev_closure_ಎಣಿಕೆ = tv_refcount->vval.v_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಙ್ಕ		depth = endloop->end_depth;
    garray_T	*gap = &ectx->ec_funcrefs;
    ಪೂರ್ಣಾಙ್ಕ		closure_in_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *ರಾಶಿ;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    ಪ್ರತಿ (idx = prev_closure_ಎಣಿಕೆ; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	ಯದಿ (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ refcount = pt->pt_refcount;
	    ಪೂರ್ಣಾಙ್ಕ i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    ಪ್ರತಿ (i = 0; i < endloop->end_var_ಎಣಿಕೆ; ++i)
	    {
		typval_T *stv = ರಾಶಿ_TV_VAR(endloop->end_var_idx + i);

		ಯದಿ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ಯದಿ (refcount > 1)
	    {
		closure_in_use = TRUE;
		ಭಙ್ಗ;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    ಯದಿ (!closure_in_use)
	ಪ್ರತ್ಯಾಗಮ OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    ಯದಿ (loopvars == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    loopvars->lvs_ga.ga_len = endloop->end_var_ಎಣಿಕೆ;
    ರಾಶಿ = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_data = ರಾಶಿ;
    ಯದಿ (ರಾಶಿ == ರಿಕ್ತ)
    {
	vim_free(loopvars);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    add_loopvars_to_list(loopvars);

    // Move the variable values.
    ಪ್ರತಿ (idx = 0; idx < endloop->end_var_ಎಣಿಕೆ; ++idx)
    {
	typval_T *tv = ರಾಶಿ_TV_VAR(endloop->end_var_idx + idx);

	*(ರಾಶಿ + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    ಪ್ರತಿ (idx = prev_closure_ಎಣಿಕೆ; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	ಯದಿ (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == ರಿಕ್ತ)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[depth] = loopvars;

	    pt->pt_outer.out_loop[depth].ರಾಶಿ = &loopvars->lvs_ga;
	    pt->pt_outer.out_loop[depth].var_idx -=
		  ectx->ec_frame_idx + ರಾಶಿ_FRAME_SIZE + endloop->end_var_idx;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    ಪೂರ್ಣಾಙ್ಕ
loopvars_check_refcount(loopvars_T *loopvars)
{
    ಪೂರ್ಣಾಙ್ಕ		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    ಪೂರ್ಣಾಙ್ಕ		    done = 0;
	typval_T	*ರಾಶಿ = gap->ga_data;

    ಯದಿ (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_data) + i;

	ಯದಿ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ರಿಕ್ತ
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    ಪೂರ್ಣಾಙ್ಕ	    depth;

	    ಪ್ರತಿ (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		ಯದಿ (tv->vval.v_partial->pt_loopvars[depth] == loopvars)
		    ++done;
	}
    }
    ಯದಿ (done != loopvars->lvs_min_refcount)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    ರಾಶಿ = gap->ga_data;
    ಪ್ರತಿ (i = 0; i < gap->ga_len; ++i)
	clear_tv(ರಾಶಿ + i);
    vim_free(ರಾಶಿ);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    ಪೂರ್ಣಾಙ್ಕ
set_ref_in_loopvars(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    loopvars_T *loopvars;

    ಪ್ರತಿ (loopvars = first_loopvars; loopvars != ರಿಕ್ತ;
						 loopvars = loopvars->lvs_next)
    {
	typval_T    *ರಾಶಿ = loopvars->lvs_ga.ga_data;
	ಪೂರ್ಣಾಙ್ಕ	    i;

	ಪ್ರತಿ (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    ಯದಿ (set_ref_in_item(ರಾಶಿ + i, copyID, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ))
		ಪ್ರತ್ಯಾಗಮ TRUE;  // abort
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
load_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = ರಿಕ್ತ;
    hashtab_T	*ht = ರಿಕ್ತ;
    ವರ್ಣ	namespace;

    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
	ಪ್ರತ್ಯಾಗಮ NOTDONE;
    ವಿಕಲ್ಪ (isn_type)
    {
	ಪಕ್ಷ ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    ಭಙ್ಗ;
	ಪಕ್ಷ ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    ಭಙ್ಗ;
	ಪಕ್ಷ ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    ಭಙ್ಗ;
	ಪಕ್ಷ ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:  // Cannot reach here
	    ಪ್ರತ್ಯಾಗಮ NOTDONE;
    }
    di = find_var_in_ht(ht, 0, iptr->isn_arg.string, TRUE);

    ಯದಿ (di == ರಿಕ್ತ)
    {
	ಯದಿ (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = find_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    ಯದಿ (ufunc != ರಿಕ್ತ)
	    {
		typval_T    *tv = ರಾಶಿ_TV_BOT(0);

		++ectx->ec_ರಾಶಿ.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		ಯದಿ (tv->vval.v_string == ರಿಕ್ತ)
		    ಪ್ರತ್ಯಾಗಮ FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		ಪ್ರತ್ಯಾಗಮ OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	ಯದಿ (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != ರಿಕ್ತ)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_item_not_found_in_script_str), iptr->isn_arg.string);
	ಅನ್ಯಥಾ
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = ರಾಶಿ_TV_BOT(0);
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->v_lock = 0;
	tv->vval.v_ಸಂಖ್ಯೆ = 0;
	++ectx->ec_ರಾಶಿ.ga_len;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
    {
	copy_tv(&di->di_tv, ರಾಶಿ_TV_BOT(0));
	++ectx->ec_ರಾಶಿ.ga_len;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}


    ಸ್ಥಿರ ಶೂನ್ಯ
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, ಪರಿಮಾಣ(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    ವರ್ಣ *tofree = ರಿಕ್ತ;
    ವರ್ಣ *typename = type_name(type, &tofree);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    ಪೂರ್ಣಾಙ್ಕ		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = ರಿಕ್ತ;
    typval_T	mtv;

    ಯದಿ (is_object)
    {
	obj = tv->vval.v_object;
	ಯದಿ (obj == ರಿಕ್ತ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	tv_cl = obj->obj_class;
    }
    ಅನ್ಯಥಾ
    {
	tv_cl = tv->vval.v_class;
	ಯದಿ (tv_cl == ರಿಕ್ತ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    ಪೂರ್ಣಾಙ್ಕ		namelen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(iptr->isn_arg.string);
    ಪೂರ್ಣಾಙ್ಕ		save_did_emsg = did_emsg;

    ಯದಿ (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ (did_emsg != save_did_emsg)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // could be a class or instance method
    ufunc_T	*oc_method;
    ಪೂರ್ಣಾಙ್ಕ		oc_method_idx;

    oc_method = method_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_method_idx);
    ಯದಿ (oc_method == ರಿಕ್ತ)
    {
	ವರ್ಣ	*ಸಂದೇಶ;

	SOURCING_LNUM = iptr->isn_lnum;
	ಯದಿ (is_object)
	    ಸಂದೇಶ = e_variable_not_found_on_object_str_str;
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ = e_class_variable_str_not_found_in_class_str;
	semsg(_(ಸಂದೇಶ), iptr->isn_arg.string, tv_cl->class_name.string);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Protected methods are not accessible outside the class
    ಯದಿ (*oc_method->uf_name == '_'
			&& !class_instance_of(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_method_str), oc_method->uf_name);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    // Create a partial for the instance or class method
    ಯದಿ (obj_method_to_partial_tv(is_object ? obj : ರಿಕ್ತ, oc_method, tv)
								== FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, ಪೂರ್ಣಾಙ್ಕ fixindent)
{
    ಪೂರ್ಣಾಙ್ಕ		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		dir = FORWARD;

    ಯದಿ (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = ರಾಶಿ_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_ಸಂಖ್ಯೆ;
	ಯದಿ (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_ರಾಶಿ.ga_len;
    }
    ಅನ್ಯಥಾ ಯದಿ (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    ಅನ್ಯಥಾ ಯದಿ (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	ಯದಿ (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    ಯದಿ (regname == '=')
    {
	tv = ರಾಶಿ_TV_BOT(-1);
	ಯದಿ (tv->v_type == VAR_STRING)
	    ಅಭಿವ್ಯಕ್ತಿ = tv->vval.v_string;
	ಅನ್ಯಥಾ
	{
	    ಅಭಿವ್ಯಕ್ತಿ = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_ರಾಶಿ.ga_len;
    }
    check_cursor();

    ಯದಿ (fixindent)
	do_put(regname, ಅಭಿವ್ಯಕ್ತಿ, dir, 1L, PUT_ಸಾಲು|PUT_CURSLINE|PUT_FIXINDENT);
    ಅನ್ಯಥಾ
	do_put(regname, ಅಭಿವ್ಯಕ್ತಿ, dir, 1L, PUT_ಸಾಲು|PUT_CURSLINE);
    vim_free(ಅಭಿವ್ಯಕ್ತಿ);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = iptr->isn_arg.unpack.unp_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    ಪೂರ್ಣಾಙ್ಕ		i;

    l = tv->vval.v_list;
    ಯದಿ (l == ರಿಕ್ತ
	    || l->lv_len < (semicolon ? ಎಣಿಕೆ - 1 : ಎಣಿಕೆ))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_ಮೌಲ್ಯ_does_not_have_enough_ವಸ್ತುಗಳು));
	ಪ್ರತ್ಯಾಗಮ EXEC_FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (!semicolon && l->lv_len > ಎಣಿಕೆ)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_ಮೌಲ್ಯ_has_more_ವಸ್ತುಗಳು_than_targets));
	ಪ್ರತ್ಯಾಗಮ EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, ಎಣಿಕೆ - 1))
	ಪ್ರತ್ಯಾಗಮ EXEC_DONE;
    ectx->ec_ರಾಶಿ.ga_len += ಎಣಿಕೆ - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ಯದಿ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ವಸ್ತುಗಳು(l->lv_len - ಎಣಿಕೆ + 1);

	ಯದಿ (rem_list == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ EXEC_DONE;
	tv = ರಾಶಿ_TV_BOT(-ಎಣಿಕೆ);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	li = l->lv_first;
	ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ - 1; ++i)
	    li = li->li_next;
	ಪ್ರತಿ (i = 0; li != ರಿಕ್ತ; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_set_item(rem_list, i, &tvcopy);
	    li = li->li_next;
	}
	--ಎಣಿಕೆ;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_first;
    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
    {
	tv = ರಾಶಿ_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_next;
    }

    list_unref(l);

    ಪ್ರತ್ಯಾಗಮ EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = iptr->isn_arg.unpack.unp_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    ಪೂರ್ಣಾಙ್ಕ		i;

    tuple = tv->vval.v_tuple;
    ಯದಿ (tuple == ರಿಕ್ತ
	    || TUPLE_LEN(tuple) < (semicolon ? ಎಣಿಕೆ - 1 : ಎಣಿಕೆ))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_more_targets_than_tuple_ವಸ್ತುಗಳು));
	ಪ್ರತ್ಯಾಗಮ EXEC_FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (!semicolon && TUPLE_LEN(tuple) > ಎಣಿಕೆ)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_ವಸ್ತುಗಳು));
	ಪ್ರತ್ಯಾಗಮ EXEC_FAIL;
    }

    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, ಎಣಿಕೆ - 1))
	ಪ್ರತ್ಯಾಗಮ EXEC_DONE;
    ectx->ec_ರಾಶಿ.ga_len += ಎಣಿಕೆ - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ಯದಿ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ವಸ್ತುಗಳು(TUPLE_LEN(tuple) - ಎಣಿಕೆ + 1);

	ಯದಿ (rem_list == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ EXEC_DONE;
	tv = ರಾಶಿ_TV_BOT(-ಎಣಿಕೆ);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	ಪ್ರತಿ (i = ಎಣಿಕೆ - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ITEM(tuple, i), &tvcopy);
	    list_set_item(rem_list, i - (ಎಣಿಕೆ - 1), &tvcopy);
	}
	--ಎಣಿಕೆ;
    }

    // Produce the values in reverse order, first item last.
    ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
    {
	tv = ರಾಶಿ_TV_BOT(-i - 1);
	copy_tv(TUPLE_ITEM(tuple, i), tv);
    }

    tuple_unref(tuple);

    ಪ್ರತ್ಯಾಗಮ EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
exec_instructions(ectx_T *ectx)
{
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		save_trylevel_at_start = ectx->ec_trylevel_at_start;
    ಪೂರ್ಣಾಙ್ಕ		dict_ರಾಶಿ_len_at_start = dict_ರಾಶಿ.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_start = trylevel;

    ಪ್ರತಿ (;;)
    {
	ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  breakcheck_ಎಣಿಕೆ = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	ಯದಿ (unlikely(++breakcheck_ಎಣಿಕೆ >= 100))
	{
	    ಸಾಲು_breakcheck();
	    breakcheck_ಎಣಿಕೆ = 0;
	}
	ಯದಿ (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    ಯದಿ (throw_exception("Vim:Interrupt", ET_INTERRUPT, ರಿಕ್ತ) == FAIL)
		ಗಚ್ಛ theend;
	    did_throw = TRUE;
	}

	ಯದಿ (unlikely(did_emsg && ಸಂದೇಶ_list != ರಿಕ್ತ && *ಸಂದೇಶ_list != ರಿಕ್ತ))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    ಯದಿ (throw_exception(*ಸಂದೇಶ_list, ET_ERROR, ರಿಕ್ತ) == FAIL)
		ಗಚ್ಛ theend;
	    did_throw = TRUE;
	    *ಸಂದೇಶ_list = ರಿಕ್ತ;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    ಯದಿ (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		ಯದಿ (trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		{
		    ಯದಿ (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			finish_exception(caught_ರಾಶಿ);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	ಯದಿ (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = ರಿಕ್ತ;
	    ಪೂರ್ಣಾಙ್ಕ		index = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    ಯಾವತ್ (index > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_data) + index - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		ಯದಿ (!trycmd->tcd_in_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    ಭಙ್ಗ;
		// In the catch and finally block of this try we have to go up
		// one level.
		--index;
		trycmd = ರಿಕ್ತ;
	    }
	    ಯದಿ (trycmd != ರಿಕ್ತ && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
	    {
		ಯದಿ (trycmd->tcd_in_catch)
		{
		    ಯದಿ (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    ಅನ್ಯಥಾ
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		ಅನ್ಯಥಾ
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_in_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    ಅನ್ಯಥಾ
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಗಚ್ಛ theend;
		tv = ರಾಶಿ_TV_BOT(0);
		tv->v_type = VAR_ಸಂಖ್ಯೆ;
		tv->vval.v_ಸಂಖ್ಯೆ = 0;
		++ectx->ec_ರಾಶಿ.ga_len;
		ಯದಿ (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    ಯದಿ (handle_closure_in_use(ectx, FALSE) == FAIL)
			ಗಚ್ಛ theend;
		    ಗಚ್ಛ done;
		}

		ಯದಿ (func_return(ectx) == FAIL)
		    ಗಚ್ಛ theend;
	    }
	    ಅನುವರ್ತ;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	ವಿಕಲ್ಪ (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    ಪಕ್ಷ ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = ರಾಶಿ_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		ಭಙ್ಗ;

	    // execute Ex command line
	    ಪಕ್ಷ ISN_EXEC:
		ಯದಿ (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;

	    // execute Ex command line split at NL characters.
	    ಪಕ್ಷ ISN_EXEC_SPLIT:
		{
		    source_cookie_T cookie;
		    char_u	    *ಸಾಲು;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    ಸಾಲು = get_split_sourceline(0, &cookie, 0, 0);
		    ಯದಿ (do_cmdline(ಸಾಲು,
				get_split_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(ಸಾಲು);
			ಗಚ್ಛ on_error;
		    }
		    vim_free(ಸಾಲು);
		}
		ಭಙ್ಗ;

	    // execute Ex command line that is only a range
	    ಪಕ್ಷ ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    ವರ್ಣ	*error = ರಿಕ್ತ;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = CMD_SIZE;
		    ea.addr_type = ADDR_ಸಾಲುಗಳು;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_address(&ea, &error, FALSE);
		    ಯದಿ (ea.cmd == ರಿಕ್ತ)
			ಗಚ್ಛ on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    ಯದಿ (error != ರಿಕ್ತ)
		    {
			emsg(error);
			ಗಚ್ಛ on_error;
		    }
		}
		ಭಙ್ಗ;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    ಪಕ್ಷ ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    ಪೂರ್ಣಾಙ್ಕ	    res;
		    ಪೂರ್ಣಾಙ್ಕ	    save_flags = cmdmod.cmod_flags;

		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    tv = ರಾಶಿ_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, ರಿಕ್ತ, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    ಯದಿ (res == FAIL)
			ಗಚ್ಛ on_error;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    // push typeval VAR_INSTR with instructions to be executed
	    ಪಕ್ಷ ISN_INSTR:
		{
		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    tv = ರಾಶಿ_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    ಯದಿ (tv->vval.v_instr == ರಿಕ್ತ)
			ಗಚ್ಛ on_error;
		    ++ectx->ec_ರಾಶಿ.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_SOURCE:
		{
		    ಪೂರ್ಣಾಙ್ಕ notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (may_load_script((ಪೂರ್ಣಾಙ್ಕ)iptr->isn_arg.ಸಂಖ್ಯೆ, &notused)
								       == FAIL)
			ಗಚ್ಛ on_error;
		}
		ಭಙ್ಗ;

	    // execute :substitute with an expression
	    ಪಕ್ಷ ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    source_cookie_T	cookie;
		    ಸಂರಚನಾ subs_ಅಭಿವ್ಯಕ್ತಿ_S	*save_instr = substitute_instr;
		    ಸಂರಚನಾ subs_ಅಭಿವ್ಯಕ್ತಿ_S	subs_instr;
		    ಪೂರ್ಣಾಙ್ಕ			res;

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

		    ಯದಿ (res == FAIL || did_emsg
					     || subs_instr.subs_status == FAIL)
			ಗಚ್ಛ on_error;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_FINISH:
		ಗಚ್ಛ done;

	    ಪಕ್ಷ ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		ಯದಿ (alloc_redir_lval() == FAIL)
		    ಗಚ್ಛ on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    {
			vim_free(res);
			ಗಚ್ಛ theend;
		    }
		    tv = ರಾಶಿ_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		force_abort = TRUE;
		ಯದಿ (trigger_cexpr_autocmd(iptr->isn_arg.ಸಂಖ್ಯೆ) == FAIL)
		    ಗಚ್ಛ on_error;
		force_abort = FALSE;
#endif
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    exarg_T ea;
		    ಪೂರ್ಣಾಙ್ಕ	    res;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv = ರಾಶಿ_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    ಯದಿ (res == FAIL)
			ಗಚ್ಛ on_error;
		}
#endif
		ಭಙ್ಗ;

	    // execute Ex command from pieces on the stack
	    ಪಕ್ಷ ISN_EXECCONCAT:
		{
		    ಪೂರ್ಣಾಙ್ಕ	    ಎಣಿಕೆ = iptr->isn_arg.ಸಂಖ್ಯೆ;
		    size_t  len = 0;
		    ಪೂರ್ಣಾಙ್ಕ	    pass;
		    ಪೂರ್ಣಾಙ್ಕ	    i;
		    char_u  *cmd = ರಿಕ್ತ;
		    char_u  *str;

		    ಪ್ರತಿ (pass = 1; pass <= 2; ++pass)
		    {
			ಪ್ರತಿ (i = 0; i < ಎಣಿಕೆ; ++i)
			{
			    tv = ರಾಶಿ_TV_BOT(i - ಎಣಿಕೆ);
			    str = tv->vval.v_string;
			    ಯದಿ (str != ರಿಕ್ತ && *str != NUL)
			    {
				ಯದಿ (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    ಯದಿ (pass == 2)
				clear_tv(tv);
			}
			ಯದಿ (pass == 1)
			{
			    cmd = alloc(len + 1);
			    ಯದಿ (unlikely(cmd == ರಿಕ್ತ))
				ಗಚ್ಛ theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		ಭಙ್ಗ;

	    // execute :echo {string} ...
	    ಪಕ್ಷ ISN_ECHO:
		{
		    ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = iptr->isn_arg.echo.echo_ಎಣಿಕೆ;
		    ಪೂರ್ಣಾಙ್ಕ	atstart = TRUE;
		    ಪೂರ್ಣಾಙ್ಕ needclr = TRUE;
		    ಪೂರ್ಣಾಙ್ಕ	idx;

		    ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; ++idx)
		    {
			tv = ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    ಯದಿ (needclr)
			ಸಂದೇಶ_clr_eos();
		    ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ;
		}
		ಭಙ್ಗ;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    ಪಕ್ಷ ISN_EXECUTE:
	    ಪಕ್ಷ ISN_ECHOMSG:
	    ಪಕ್ಷ ISN_ECHOWINDOW:
	    ಪಕ್ಷ ISN_ECHOCONSOLE:
	    ಪಕ್ಷ ISN_ECHOERR:
		{
		    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    ಪೂರ್ಣಾಙ್ಕ		len;
		    ಪೂರ್ಣಾಙ್ಕ		failed = FALSE;
		    ಪೂರ್ಣಾಙ್ಕ		idx;

		    ಯದಿ (iptr->isn_type == ISN_ECHOWINDOW)
			ಎಣಿಕೆ = iptr->isn_arg.echowin.ewin_ಎಣಿಕೆ;
		    ಅನ್ಯಥಾ
			ಎಣಿಕೆ = iptr->isn_arg.ಸಂಖ್ಯೆ;
		    ga_init2(&ga, 1, 80);
		    ಪ್ರತಿ (idx = 0; idx < ಎಣಿಕೆ; ++idx)
		    {
			tv = ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ);
			ಯದಿ (iptr->isn_type == ISN_EXECUTE)
			{
			    ಯದಿ (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_JOB)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
						    vartype_name(tv->v_type));
				ಭಙ್ಗ;
			    }
			    ಅನ್ಯಥಾ
				p = tv_get_string_buf(tv, buf);
			}
			ಅನ್ಯಥಾ
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(p);
			ಯದಿ (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			ಅನ್ಯಥಾ
			{
			    ಯದಿ (ga.ga_len > 0)
				((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ;
		    ಯದಿ (failed)
		    {
			ga_clear(&ga);
			ಗಚ್ಛ on_error;
		    }

		    ಯದಿ (ga.ga_data != ರಿಕ್ತ)
		    {
			ಯದಿ (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_data);
			    ಯದಿ (did_emsg)
			    {
				ga_clear(&ga);
				ಗಚ್ಛ on_error;
			    }
			}
			ಅನ್ಯಥಾ
			{
			    ಸಂದೇಶ_sb_eol();
			    ಯದಿ (iptr->isn_type == ISN_ECHOMSG)
			    {
				ಸಂದೇಶ_attr(ga.ga_data, echo_attr);
				out_flush();
			    }
#ifdef HAS_MESSAGE_WINDOW
			    ಅನ್ಯಥಾ ಯದಿ (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				start_echowindow(
					      iptr->isn_arg.echowin.ewin_time);
				ಸಂದೇಶ_attr(ga.ga_data, echo_attr);
				end_echowindow();
			    }
#endif
			    ಅನ್ಯಥಾ ಯದಿ (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_data, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ga.ga_data),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    ಅನ್ಯಥಾ
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_data);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		ಭಙ್ಗ;

	    // load local variable or argument
	    ಪಕ್ಷ ISN_LOAD:
		ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಗಚ್ಛ theend;
		tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.ಸಂಖ್ಯೆ);
		ಯದಿ (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    ರಾಶಿ_TV_BOT(0)->v_type = VAR_SPECIAL;
		    ರಾಶಿ_TV_BOT(0)->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
		}
		ಅನ್ಯಥಾ
		    copy_tv(tv, ರಾಶಿ_TV_BOT(0));
		++ectx->ec_ರಾಶಿ.ga_len;
		ಭಙ್ಗ;

	    // load v: variable
	    ಪಕ್ಷ ISN_LOADV:
		ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಗಚ್ಛ theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.ಸಂಖ್ಯೆ), ರಾಶಿ_TV_BOT(0));
		++ectx->ec_ರಾಶಿ.ga_len;
		ಭಙ್ಗ;

	    // load s: variable in Vim9 script
	    ಪಕ್ಷ ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ಯದಿ (sv == ರಿಕ್ತ)
			ಗಚ್ಛ theend;
		    allocate_if_null(sv);
		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    copy_tv(sv->sv_tv, ರಾಶಿ_TV_BOT(0));
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    // load s: variable in old script or autoload import
	    ಪಕ್ಷ ISN_LOADS:
	    ಪಕ್ಷ ISN_LOADEXPORT:
		{
		    ಪೂರ್ಣಾಙ್ಕ		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = find_var_in_ht(ht, 0, name, TRUE);

		    ಯದಿ (di == ರಿಕ್ತ)
		    {
			ufunc_T	*ufunc = ರಿಕ್ತ;

			ಯದಿ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (ಶೂನ್ಯ)find_exported(sid, name, &ufunc, &type,
							 ರಿಕ್ತ, ರಿಕ್ತ, FALSE);
			}
			ಯದಿ (ufunc == ರಿಕ್ತ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    ಗಚ್ಛ on_error;
			}
			ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			    ಗಚ್ಛ theend;
			tv = ರಾಶಿ_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_ರಾಶಿ.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    ಪೂರ್ಣಾಙ್ಕ idx = get_script_item_idx(sid, name, 0,
								   ರಿಕ್ತ, ರಿಕ್ತ);
			    svar_T	*sv;

			    ಯದಿ (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;
				ಯದಿ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    ಗಚ್ಛ on_error;
				}
			    }
			}
			ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			    ಗಚ್ಛ theend;
			copy_tv(&di->di_tv, ರಾಶಿ_TV_BOT(0));
			++ectx->ec_ರಾಶಿ.ga_len;
		    }
		}
		ಭಙ್ಗ;

	    // load g:/b:/w:/t: variable
	    ಪಕ್ಷ ISN_LOADG:
	    ಪಕ್ಷ ISN_LOADB:
	    ಪಕ್ಷ ISN_LOADW:
	    ಪಕ್ಷ ISN_LOADT:
		{
		    ಪೂರ್ಣಾಙ್ಕ res = load_namespace_var(ectx, iptr->isn_type, iptr);

		    ಯದಿ (res == NOTDONE)
			ಗಚ್ಛ theend;
		    ಯದಿ (res == FAIL)
			ಗಚ್ಛ on_error;
		}

		ಭಙ್ಗ;

	    // load autoload variable
	    ಪಕ್ಷ ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (eval_variable(name, (ಪೂರ್ಣಾಙ್ಕ)STRLEN(name), 0,
			      ರಾಶಿ_TV_BOT(0), ರಿಕ್ತ, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = ರಾಶಿ_TV_BOT(0);
			tv->v_type = VAR_ಸಂಖ್ಯೆ;
			tv->v_lock = 0;
			tv->vval.v_ಸಂಖ್ಯೆ = 0;
			++ectx->ec_ರಾಶಿ.ga_len;
			ಗಚ್ಛ on_error;
		    }
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    // load g:/b:/w:/t: namespace
	    ಪಕ್ಷ ISN_LOADGDICT:
	    ಪಕ್ಷ ISN_LOADBDICT:
	    ಪಕ್ಷ ISN_LOADWDICT:
	    ಪಕ್ಷ ISN_LOADTDICT:
		{
		    dict_T *d = ರಿಕ್ತ;

		    ವಿಕಲ್ಪ (iptr->isn_type)
		    {
			ಪಕ್ಷ ISN_LOADGDICT: d = get_globvar_dict(); ಭಙ್ಗ;
			ಪಕ್ಷ ISN_LOADBDICT: d = curbuf->b_vars; ಭಙ್ಗ;
			ಪಕ್ಷ ISN_LOADWDICT: d = curwin->w_vars; ಭಙ್ಗ;
			ಪಕ್ಷ ISN_LOADTDICT: d = curtab->tp_vars; ಭಙ್ಗ;
			ಸ್ವಭಾವ:  // Cannot reach here
			    ಗಚ್ಛ theend;
		    }
		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    tv = ರಾಶಿ_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_lock = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    // load &option
	    ಪಕ್ಷ ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    ಯದಿ (eval_option(&name, &optval, TRUE) == FAIL)
			ಗಚ್ಛ theend;
		    *ರಾಶಿ_TV_BOT(0) = optval;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    // load $ENV
	    ಪಕ್ಷ ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    // name is always valid, checked when compiling
		    (ಶೂನ್ಯ)eval_env_var(&name, &optval, TRUE);
		    *ರಾಶಿ_TV_BOT(0) = optval;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    // load @register
	    ಪಕ್ಷ ISN_LOADREG:
		ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಗಚ್ಛ theend;
		tv = ರಾಶಿ_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.ಸಂಖ್ಯೆ, GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC);
		++ectx->ec_ರಾಶಿ.ga_len;
		ಭಙ್ಗ;

	    // store local variable
	    ಪಕ್ಷ ISN_STORE:
		--ectx->ec_ರಾಶಿ.ga_len;
		tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.ಸಂಖ್ಯೆ);
		ಯದಿ (check_typval_is_ಮೌಲ್ಯ(ರಾಶಿ_TV_BOT(0)) == FAIL
			|| ರಾಶಿ_TV_BOT(0)->v_type == VAR_VOID)
		{
		    ಯದಿ (ರಾಶಿ_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
		    clear_tv(ರಾಶಿ_TV_BOT(0));
		    ಗಚ್ಛ on_error;
		}
		clear_tv(tv);
		*tv = *ರಾಶಿ_TV_BOT(0);
		ಭಙ್ಗ;

	    // store s: variable in old script or autoload import
	    ಪಕ್ಷ ISN_STORES:
	    ಪಕ್ಷ ISN_STOREEXPORT:
		{
		    ಪೂರ್ಣಾಙ್ಕ		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = ರಿಕ್ತ;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    ಯದಿ (iptr->isn_type == ISN_STOREEXPORT)
			di = find_var_autoload_prefix(name, sid, ರಿಕ್ತ, ರಿಕ್ತ);
		    // Then look for a variable in the script's variables.
		    ಯದಿ (di == ರಿಕ್ತ)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = find_var_in_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_ರಾಶಿ.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (di == ರಿಕ್ತ)
		    {
			ಯದಿ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(ರಾಶಿ_TV_BOT(0));
			    ಗಚ್ಛ on_error;
			}
			store_var(name, ರಾಶಿ_TV_BOT(0));
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    ಪೂರ್ಣಾಙ್ಕ idx = get_script_item_idx(sid, name, 0,
								   ರಿಕ್ತ, ರಿಕ್ತ);

			    // can this ever fail?
			    ಯದಿ (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;

				ಯದಿ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    clear_tv(ರಾಶಿ_TV_BOT(0));
				    ಗಚ್ಛ on_error;
				}
			    }
			}
			ಯದಿ (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(ರಾಶಿ_TV_BOT(0));
			    ಗಚ್ಛ on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *ರಾಶಿ_TV_BOT(0);
		    }
		}
		ಭಙ್ಗ;

	    // store script-local variable in Vim9 script
	    ಪಕ್ಷ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ಯದಿ (sv == ರಿಕ್ತ)
			ಗಚ್ಛ theend;
		    --ectx->ec_ರಾಶಿ.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (ಮೌಲ್ಯ_check_lock(sv->sv_tv->v_lock, sv->sv_name, FALSE))
		    {
			clear_tv(ರಾಶಿ_TV_BOT(0));
			ಗಚ್ಛ on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *ರಾಶಿ_TV_BOT(0);
		}
		ಭಙ್ಗ;

	    // store option
	    ಪಕ್ಷ ISN_STOREOPT:
	    ಪಕ್ಷ ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    ಪೂರ್ಣಾಙ್ಕ		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    long	n = 0;
		    char_u	*s = ರಿಕ್ತ;
		    ವರ್ಣ	*ಸಂದೇಶ;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = ರಿಕ್ತ;

		    --ectx->ec_ರಾಶಿ.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ರಾಶಿ_TV_BOT(0);
		    ಯದಿ (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			ಯದಿ (s == ರಿಕ್ತ)
			    s = (char_u *)"";
		    }
		    ಅನ್ಯಥಾ ಯದಿ (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			ಯದಿ (s == ರಿಕ್ತ || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    ಗಚ್ಛ on_error;
			}
		    }
		    ಅನ್ಯಥಾ
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_ಸಂಖ್ಯೆ;
		    ಸಂದೇಶ = set_option_ಮೌಲ್ಯ(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    ಯದಿ (ಸಂದೇಶ != ರಿಕ್ತ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(ಸಂದೇಶ));
			ಗಚ್ಛ on_error;
		    }
		}
		ಭಙ್ಗ;

	    // store $ENV
	    ಪಕ್ಷ ISN_STOREENV:
		ಯದಿ (check_secure() || check_restricted())
		    ಗಚ್ಛ theend;
		--ectx->ec_ರಾಶಿ.ga_len;
		tv = ರಾಶಿ_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		ಭಙ್ಗ;

	    // store @r
	    ಪಕ್ಷ ISN_STOREREG:
		{
		    ಪೂರ್ಣಾಙ್ಕ	reg = iptr->isn_arg.ಸಂಖ್ಯೆ;

		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv = ರಾಶಿ_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		ಭಙ್ಗ;

	    // store v: variable
	    ಪಕ್ಷ ISN_STOREV:
		--ectx->ec_ರಾಶಿ.ga_len;
		ಯದಿ (set_vim_var_tv(iptr->isn_arg.ಸಂಖ್ಯೆ, ರಾಶಿ_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;

	    // store g:/b:/w:/t: variable
	    ಪಕ್ಷ ISN_STOREG:
	    ಪಕ್ಷ ISN_STOREB:
	    ಪಕ್ಷ ISN_STOREW:
	    ಪಕ್ಷ ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    ವಿಕಲ್ಪ (iptr->isn_type)
		    {
			ಪಕ್ಷ ISN_STOREG:
			    ht = get_globvar_ht();
			    ಭಙ್ಗ;
			ಪಕ್ಷ ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    ಭಙ್ಗ;
			ಪಕ್ಷ ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    ಭಙ್ಗ;
			ಪಕ್ಷ ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    ಭಙ್ಗ;
			ಸ್ವಭಾವ:  // Cannot reach here
			    ಗಚ್ಛ theend;
		    }

		    --ectx->ec_ರಾಶಿ.ga_len;
		    di = find_var_in_ht(ht, 0, name, TRUE);
		    ಯದಿ (di == ರಿಕ್ತ)
			store_var(iptr->isn_arg.string, ರಾಶಿ_TV_BOT(0));
		    ಅನ್ಯಥಾ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ಯದಿ (var_check_permission(di, name) == FAIL)
			    ಗಚ್ಛ on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *ರಾಶಿ_TV_BOT(0);
		    }
		}
		ಭಙ್ಗ;

	    // store an autoload variable
	    ಪಕ್ಷ ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		set_var(iptr->isn_arg.string, ರಾಶಿ_TV_BOT(-1), TRUE);
		clear_tv(ರಾಶಿ_TV_BOT(-1));
		--ectx->ec_ರಾಶಿ.ga_len;
		ಭಙ್ಗ;

	    // store number in local variable
	    ಪಕ್ಷ ISN_STORENR:
		tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_ಸಂಖ್ಯೆ;
		tv->vval.v_ಸಂಖ್ಯೆ = iptr->isn_arg.storenr.stnr_val;
		ಭಙ್ಗ;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    ಪಕ್ಷ ISN_STOREINDEX:
		{
		    ಪೂರ್ಣಾಙ್ಕ res = execute_storeindex(iptr, ectx);

		    ಯದಿ (res == FAIL)
			ಗಚ್ಛ on_error;
		    ಯದಿ (res == NOTDONE)
			ಗಚ್ಛ theend;
		}
		ಭಙ್ಗ;

	    // store value in list or blob range
	    ಪಕ್ಷ ISN_STORERANGE:
		ಯದಿ (execute_storerange(iptr, ectx) == FAIL)
		    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_LOAD_CLASSMEMBER:
		{
		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      ರಾಶಿ_TV_BOT(0));
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *ರಾಶಿ_TV_BOT(-1);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    // Load or store variable or argument from outer scope.
	    ಪಕ್ಷ ISN_LOADOUTER:
	    ಪಕ್ಷ ISN_STOREOUTER:
		{
		    ಪೂರ್ಣಾಙ್ಕ		depth = iptr->isn_arg.outer.outer_depth;
		    outer_T	*outer = ectx->ec_outer_ref == ರಿಕ್ತ ? ರಿಕ್ತ
						: ectx->ec_outer_ref->or_outer;

		    ಯಾವತ್ (depth > 1 && outer != ರಿಕ್ತ)
		    {
			outer = outer->out_up;
			--depth;
		    }
		    ಯದಿ (outer == ರಿಕ್ತ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ಯದಿ (ectx->ec_frame_idx == ectx->ec_initial_frame_idx
						 || ectx->ec_outer_ref == ರಿಕ್ತ)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_closure_called_from_invalid_context));
			ಅನ್ಯಥಾ
			    iemsg("LOADOUTER depth more than scope levels");
			ಗಚ್ಛ theend;
		    }
		    ಯದಿ (depth < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->out_loop[-depth - 1]
							       .ರಾಶಿ->ga_data)
					  + outer->out_loop[-depth - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    ಅನ್ಯಥಾ
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->out_ರಾಶಿ->ga_data)
				      + outer->out_frame_idx + ರಾಶಿ_FRAME_SIZE
				      + iptr->isn_arg.outer.outer_idx;
		    ಯದಿ (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			    ಗಚ್ಛ theend;
			// careful: ga_grow_inner may re-alloc the stack
			ಯದಿ (depth < 0)
			    copy = ((typval_T *)outer->out_loop[-depth - 1]
								   .ರಾಶಿ->ga_data)
					      + outer->out_loop[-depth - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			ಅನ್ಯಥಾ
			    copy = ((typval_T *)outer->out_ರಾಶಿ->ga_data)
					  + outer->out_frame_idx + ರಾಶಿ_FRAME_SIZE
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			ಯದಿ (copy != tv)
			    tv = copy;
			copy_tv(tv, ರಾಶಿ_TV_BOT(0));
			++ectx->ec_ರಾಶಿ.ga_len;
		    }
		    ಅನ್ಯಥಾ
		    {
			--ectx->ec_ರಾಶಿ.ga_len;
			clear_tv(tv);
			*tv = *ರಾಶಿ_TV_BOT(0);
		    }
		}
		ಭಙ್ಗ;

	    // unlet item in list or dict variable
	    ಪಕ್ಷ ISN_UNLETINDEX:
		ಯದಿ (execute_unletindex(iptr, ectx) == FAIL)
		    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;

	    // unlet range of items in list variable
	    ಪಕ್ಷ ISN_UNLETRANGE:
		ಯದಿ (execute_unletrange(iptr, ectx) == FAIL)
		    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;

	    // push constant
	    ಪಕ್ಷ ISN_PUSHNR:
	    ಪಕ್ಷ ISN_PUSHBOOL:
	    ಪಕ್ಷ ISN_PUSHSPEC:
	    ಪಕ್ಷ ISN_PUSHF:
	    ಪಕ್ಷ ISN_PUSHS:
	    ಪಕ್ಷ ISN_PUSHBLOB:
	    ಪಕ್ಷ ISN_PUSHFUNC:
	    ಪಕ್ಷ ISN_PUSHCHANNEL:
	    ಪಕ್ಷ ISN_PUSHJOB:
	    ಪಕ್ಷ ISN_PUSHOBJ:
	    ಪಕ್ಷ ISN_PUSHCLASS:
		ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಗಚ್ಛ theend;
		tv = ರಾಶಿ_TV_BOT(0);
		tv->v_lock = 0;
		++ectx->ec_ರಾಶಿ.ga_len;
		ವಿಕಲ್ಪ (iptr->isn_type)
		{
		    ಪಕ್ಷ ISN_PUSHNR:
			tv->v_type = VAR_ಸಂಖ್ಯೆ;
			tv->vval.v_ಸಂಖ್ಯೆ = iptr->isn_arg.ಸಂಖ್ಯೆ;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_ಸಂಖ್ಯೆ = iptr->isn_arg.ಸಂಖ್ಯೆ;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_ಸಂಖ್ಯೆ = iptr->isn_arg.ಸಂಖ್ಯೆ;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			ಭಙ್ಗ;
		    ಪಕ್ಷ ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			ಯದಿ (iptr->isn_arg.string == ರಿಕ್ತ)
			    tv->vval.v_string = ರಿಕ್ತ;
			ಅನ್ಯಥಾ
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			ಭಙ್ಗ;
		    ಪಕ್ಷ ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = ರಿಕ್ತ;
#endif
			ಭಙ್ಗ;
		    ಪಕ್ಷ ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_JOB;
			tv->vval.v_job = ರಿಕ್ತ;
#endif
			ಭಙ್ಗ;
		    ಪಕ್ಷ ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = ರಿಕ್ತ;
			ಭಙ್ಗ;
		    ಪಕ್ಷ ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			ಭಙ್ಗ;
		    ಸ್ವಭಾವ:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == ರಿಕ್ತ
				    ? ರಿಕ್ತ : vim_strsave(iptr->isn_arg.string);
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (ಶೂನ್ಯ)script_autoload(name, FALSE);
		    ಯದಿ (find_func(name, TRUE))
		    {
			ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			    ಗಚ್ಛ theend;
			tv = ರಾಶಿ_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_ರಾಶಿ.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    ಅನ್ಯಥಾ
		    {
			ಪೂರ್ಣಾಙ್ಕ res = load_namespace_var(ectx, ISN_LOADG, iptr);

			ಯದಿ (res == NOTDONE)
			    ಗಚ್ಛ theend;
			ಯದಿ (res == FAIL)
			    ಗಚ್ಛ on_error;
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_UNLET:
		ಯದಿ (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_LOCKUNLOCK:
		{
#ifdef LOG_LOCKVAR
		    ch_log(ರಿಕ್ತ, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#endif
		    lval_ಬೇರು_T	*lval_ಬೇರು_save = lval_ಬೇರು;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_ರಾಶಿ.ga_len;
		    lval_ಬೇರು_T ಬೇರು;
		    ಬೇರು.lr_tv      = ರಾಶಿ_TV_BOT(0);
		    ಬೇರು.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    ಬೇರು.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_ಬೇರು = &ಬೇರು;
		    ಪೂರ್ಣಾಙ್ಕ res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(ಬೇರು.lr_tv);
		    lval_ಬೇರು = lval_ಬೇರು_save;
		    ಯದಿ (res == FAIL)
			ಗಚ್ಛ on_error;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_LOCKCONST:
		item_lock(ರಾಶಿ_TV_BOT(-1), 100, TRUE, TRUE);
		ಭಙ್ಗ;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    ಪಕ್ಷ ISN_NEWLIST:
		ಯದಿ (exe_newlist(iptr->isn_arg.ಸಂಖ್ಯೆ, ectx) == FAIL)
		    ಗಚ್ಛ theend;
		ಭಙ್ಗ;

	    // create a tuple from items on the stack
	    ಪಕ್ಷ ISN_NEWTUPLE:
		ಯದಿ (exe_newtuple(iptr->isn_arg.ಸಂಖ್ಯೆ, ectx) == FAIL)
		    ಗಚ್ಛ theend;
		ಭಙ್ಗ;

	    // create a dict from items on the stack
	    ಪಕ್ಷ ISN_NEWDICT:
		{
		    ಪೂರ್ಣಾಙ್ಕ res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.ಸಂಖ್ಯೆ, ectx);
		    ಯದಿ (res == FAIL)
			ಗಚ್ಛ theend;
		    ಯದಿ (res == MAYBE)
			ಗಚ್ಛ on_error;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CONCAT:
		ಯದಿ (exe_concat(iptr->isn_arg.ಸಂಖ್ಯೆ, ectx) == FAIL)
		    ಗಚ್ಛ theend;
		ಭಙ್ಗ;

	    // create a partial with NULL value
	    ಪಕ್ಷ ISN_NEWPARTIAL:
		ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಗಚ್ಛ theend;
		++ectx->ec_ರಾಶಿ.ga_len;
		tv = ರಾಶಿ_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_lock = 0;
		tv->vval.v_partial = ರಿಕ್ತ;
		ಭಙ್ಗ;

	    // call a :def function
	    ಪಕ್ಷ ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ಯದಿ (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				ರಿಕ್ತ,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;

	    // call a method on an interface
	    ಪಕ್ಷ ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ರಾಶಿ_TV_BOT(-1 - mfunc->cmf_argcount);
		    ಯದಿ (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			ಗಚ್ಛ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ಯದಿ (obj == ರಿಕ್ತ)
		    {
			emsg(_(e_using_null_object));
			ಗಚ್ಛ on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    ಯದಿ (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    ಅನ್ಯಥಾ
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			ಪೂರ್ಣಾಙ್ಕ idx = object_index_from_itf_index(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    ಯದಿ (mfunc->cmf_ufunc != ರಿಕ್ತ && IS_GENERIC_FUNC(ufunc))
		    {
			ಯದಿ (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			ಅನ್ಯಥಾ
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    ಯದಿ (ufunc == ರಿಕ್ತ)
				ಗಚ್ಛ on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    ಯದಿ (call_ufunc(ufunc, ರಿಕ್ತ, mfunc->cmf_argcount, ectx,
							   ರಿಕ್ತ, ರಿಕ್ತ) == FAIL)
			ಗಚ್ಛ on_error;
		}
		ಭಙ್ಗ;

	    // call a builtin function
	    ಪಕ್ಷ ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ಯದಿ (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;

	    // call a funcref or partial
	    ಪಕ್ಷ ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    ಪೂರ್ಣಾಙ್ಕ		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (pfunc->cpf_ತುದಿ)
		    {
			// funcref is above the arguments
			tv = ರಾಶಿ_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    ಅನ್ಯಥಾ
		    {
			// Get the funcref from the stack.
			--ectx->ec_ರಾಶಿ.ga_len;
			partial_tv = *ರಾಶಿ_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    ಯದಿ (tv == &partial_tv)
			clear_tv(&partial_tv);
		    ಯದಿ (r == FAIL)
			ಗಚ್ಛ on_error;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_PCALL_END:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_ರಾಶಿ.ga_len;
		clear_tv(ರಾಶಿ_TV_BOT(-1));
		*ರಾಶಿ_TV_BOT(-1) = *ರಾಶಿ_TV_BOT(0);
		ಭಙ್ಗ;

	    // call a user defined function or funcref/partial
	    ಪಕ್ಷ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			ಗಚ್ಛ on_error;
		}
		ಭಙ್ಗ;

	    // :defer func(arg)
	    ಪಕ್ಷ ISN_DEFER:
		ಯದಿ (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    ಪಕ್ಷ ISN_RETURN_VOID:
	    ಪಕ್ಷ ISN_RETURN_OBJECT:
		ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಗಚ್ಛ theend;
		tv = ರಾಶಿ_TV_BOT(0);
		++ectx->ec_ರಾಶಿ.ga_len;
		ಯದಿ (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_ಸಂಖ್ಯೆ = 0;
		    tv->v_lock = 0;
		}
		ಅನ್ಯಥಾ
		{
		    *tv = *ರಾಶಿ_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_lock_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    ಪಕ್ಷ ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ರಿಕ್ತ;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    ಯದಿ (check_typval_is_ಮೌಲ್ಯ(ರಾಶಿ_TV_BOT(-1)) == FAIL)
			ಗಚ್ಛ theend;

		    ಯದಿ (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    ಯದಿ (trycmd != ರಿಕ್ತ
				 && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    {
			// jump to ":finally" or ":endtry"
			ಯದಿ (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			ಅನ್ಯಥಾ
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    ಅನ್ಯಥಾ
			ಗಚ್ಛ func_return;
		}
		ಭಙ್ಗ;

	    // push a partial, a reference to a compiled function
	    ಪಕ್ಷ ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    ಯದಿ (pt == ರಿಕ್ತ)
			ಗಚ್ಛ theend;
		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    {
			vim_free(pt);
			ಗಚ್ಛ theend;
		    }
		    ಯದಿ (extra != ರಿಕ್ತ && extra->fre_class != ರಿಕ್ತ)
		    {
			class_T	*cl;
			ಯದಿ (extra->fre_object_method)
			{
			    tv = ರಾಶಿ_TV_BOT(-1);
			    ಯದಿ (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				ಗಚ್ಛ on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    ಯದಿ (obj == ರಿಕ್ತ)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				ಗಚ್ಛ on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ರಾಶಿ.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			ಅನ್ಯಥಾ
			    cl = extra->fre_class;

			ಯದಿ (extra->fre_object_method)
			{
			    // object method
			    // convert the interface index to the object index
			    ಪೂರ್ಣಾಙ್ಕ idx =
				object_index_from_itf_index(extra->fre_class,
					TRUE, extra->fre_method_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			ಅನ್ಯಥಾ
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_method_idx];
			}
		    }
		    ಅನ್ಯಥಾ ಯದಿ (extra == ರಿಕ್ತ || extra->fre_func_name == ರಿಕ್ತ)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    ಅನ್ಯಥಾ
		    {
			ufunc = find_func(extra->fre_func_name, FALSE);
		    }
		    ಯದಿ (ufunc == ರಿಕ್ತ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			ಗಚ್ಛ theend;
		    }
		    ಯದಿ (extra != ರಿಕ್ತ && extra->fre_outer_depth > 0
					   && (ufunc->uf_flags & FC_CLOSURE))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == ರಿಕ್ತ
				    ? ರಿಕ್ತ : ectx->ec_outer_ref->or_partial;

			ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ depth = 1; outer_pt != ರಿಕ್ತ
				    && depth < extra->fre_outer_depth; ++depth)
			    outer_pt = get_pt_outer(outer_pt)->out_up_partial;
			ಯದಿ (outer_pt == ರಿಕ್ತ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    ಗಚ್ಛ theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (fill_partial_and_closure(pt, ufunc,
			       extra == ರಿಕ್ತ ? ರಿಕ್ತ : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			ಗಚ್ಛ theend;
		    }
		    tv = ರಾಶಿ_TV_BOT(0);
		    ++ectx->ec_ರಾಶಿ.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_lock = 0;
		}
		ಭಙ್ಗ;

	    // Create a global function from a lambda.
	    ಪಕ್ಷ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ಯದಿ (copy_lambda_to_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			ಗಚ್ಛ theend;
		}
		ಭಙ್ಗ;

	    // List functions
	    ಪಕ್ಷ ISN_DEF:
		ಯದಿ (iptr->isn_arg.string == ರಿಕ್ತ)
		    list_functions(ರಿಕ್ತ);
		ಅನ್ಯಥಾ
		{
		    exarg_T	ea;
		    garray_T	ಸಾಲುಗಳು_to_free;

		    CLEAR_FIELD(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&ಸಾಲುಗಳು_to_free, ಪರಿಮಾಣ(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, ರಿಕ್ತ, &ಸಾಲುಗಳು_to_free, 0, ರಿಕ್ತ, 0, ರಿಕ್ತ);
		    ga_clear_strings(&ಸಾಲುಗಳು_to_free);
		}
		ಭಙ್ಗ;

	    // jump if a condition is met
	    ಪಕ್ಷ ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
		    ಪೂರ್ಣಾಙ್ಕ		jump = TRUE;

		    ಯದಿ (when != JUMP_ALWAYS)
		    {
			tv = ರಾಶಿ_TV_BOT(-1);
			ಯದಿ (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    ಯದಿ (error)
				ಗಚ್ಛ on_error;
			}
			ಅನ್ಯಥಾ
			    jump = tv2bool(tv);
			ಯದಿ (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			ಯದಿ (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ರಾಶಿ.ga_len;
			}
		    }
		    ಯದಿ (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		ಭಙ್ಗ;

	    // "while": jump to end if a condition is false
	    ಪಕ್ಷ ISN_WHILE:
		{
		    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
		    ಪೂರ್ಣಾಙ್ಕ		jump = TRUE;

		    tv = ರಾಶಿ_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    ಯದಿ (error)
			ಗಚ್ಛ on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_ರಾಶಿ.ga_len;
		    ಯದಿ (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_end;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = ರಾಶಿ_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_ಸಂಖ್ಯೆ = ectx->ec_funcrefs.ga_len;
		}
		ಭಙ್ಗ;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    ಪಕ್ಷ ISN_JUMP_IF_ARG_SET:
	    ಪಕ್ಷ ISN_JUMP_IF_ARG_NOT_SET:
		tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		ಪೂರ್ಣಾಙ್ಕ arg_set = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE);

		ಯದಿ (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_SET && !arg_set)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_data)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_args.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ ರಾಶಿ_FRAME_SIZE;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_POINTER(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    ಯದಿ (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		ಯದಿ (iptr->isn_type == ISN_JUMP_IF_ARG_SET ? arg_set : !arg_set)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		ಭಙ್ಗ;

	    // top of a for loop
	    ಪಕ್ಷ ISN_FOR:
		ಯದಿ (execute_for(iptr, ectx) == FAIL)
		    ಗಚ್ಛ theend;
		ಭಙ್ಗ;

	    // end of a for or while loop
	    ಪಕ್ಷ ISN_ENDLOOP:
		ಯದಿ (execute_endloop(iptr, ectx) == FAIL)
		    ಗಚ್ಛ theend;
		ಭಙ್ಗ;

	    // start of ":try" block
	    ಪಕ್ಷ ISN_TRY:
		{
		    trycmd_T    *trycmd = ರಿಕ್ತ;

		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			ಗಚ್ಛ theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_data)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_POINTER(trycmd);
		    trycmd->tcd_frame_idx = ectx->ec_frame_idx;
		    trycmd->tcd_ರಾಶಿ_len = ectx->ec_ರಾಶಿ.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_PUSHEXC:
		ಯದಿ (current_exception == ರಿಕ್ತ)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    ಗಚ್ಛ theend;
		}
		ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಗಚ್ಛ theend;
		tv = ರಾಶಿ_TV_BOT(0);
		++ectx->ec_ರಾಶಿ.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->ಮೌಲ್ಯ);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CATCH:
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
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    ಪೂರ್ಣಾಙ್ಕ		i;
		    trycmd_T    *trycmd;
		    ಪೂರ್ಣಾಙ್ಕ		iidx = trycont->tct_where;

		    ಯದಿ (trystack->ga_len < trycont->tct_levels)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_levels, trystack->ga_len);
			ಗಚ್ಛ theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    ಪ್ರತಿ (i = trycont->tct_levels; i > 0; --i)
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
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    ಯದಿ (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_ರಾಶಿ);
			trycmd->tcd_caught = FALSE;
		    }
		    ಭಙ್ಗ;
		}

	    // end of ":try" block
	    ಪಕ್ಷ ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_data) + trystack->ga_len;
		    ಯದಿ (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    ಯದಿ (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_ರಾಶಿ);
		    }

		    ಯದಿ (trycmd->tcd_return)
			ಗಚ್ಛ func_return;

		    ಯಾವತ್ (ectx->ec_ರಾಶಿ.ga_len > trycmd->tcd_ರಾಶಿ_len)
		    {
			--ectx->ec_ರಾಶಿ.ga_len;
			clear_tv(ರಾಶಿ_TV_BOT(0));
		    }
		    ಯದಿ (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    ಯದಿ (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = ರಾಶಿ_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_ಸಂಖ್ಯೆ;
			tv->vval.v_ಸಂಖ್ಯೆ = 0;
			ಗಚ್ಛ done;
		    }
		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv = ರಾಶಿ_TV_BOT(0);
		    ಯದಿ (tv->vval.v_string == ರಿಕ್ತ
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_empty_string));
			ಗಚ್ಛ theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    ಯದಿ (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
			ಯದಿ (trycmd->tcd_caught)
			{
			    // discard the exception
			    finish_exception(caught_ರಾಶಿ);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (throw_exception(tv->vval.v_string, ET_USER, ರಿಕ್ತ)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			ಗಚ್ಛ theend;
		    }
		    did_throw = TRUE;
		}
		ಭಙ್ಗ;

	    // compare with special values
	    ಪಕ್ಷ ISN_COMPAREBOOL:
	    ಪಕ್ಷ ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_ಸಂಖ್ಯೆ;
		    varnumber_T arg2 = tv2->vval.v_ಸಂಖ್ಯೆ;
		    ಪೂರ್ಣಾಙ್ಕ		res;

		    ಯದಿ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_EQUAL)
			res = arg1 == arg2;
		    ಅನ್ಯಥಾ
			res = arg1 != arg2;

		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_COMPARENULL:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    ಪೂರ್ಣಾಙ್ಕ		res;

		    res = typval_compare_null(tv1, tv2);
		    ಯದಿ (res == MAYBE)
			ಗಚ್ಛ on_error;
		    ಯದಿ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ಭಙ್ಗ;

	    // Operation with two number arguments
	    ಪಕ್ಷ ISN_OPNR:
	    ಪಕ್ಷ ISN_COMPARENR:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_ಸಂಖ್ಯೆ;
		    varnumber_T arg2 = tv2->vval.v_ಸಂಖ್ಯೆ;
		    varnumber_T res = 0;
		    ಪೂರ್ಣಾಙ್ಕ		div_zero = FALSE;

		    ಯದಿ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_LSHIFT
			    || iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_RSHIFT)
		    {
			ಯದಿ (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_positive));
			    ಗಚ್ಛ on_error;
			}
		    }

		    ವಿಕಲ್ಪ (iptr->isn_arg.op.op_type)
		    {
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_MULT: res = arg1 * arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_DIV:  ಯದಿ (arg2 == 0)
					    div_zero = TRUE;
					ಅನ್ಯಥಾ
					    res = arg1 / arg2;
					ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_REM:  ಯದಿ (arg2 == 0)
					    div_zero = TRUE;
					ಅನ್ಯಥಾ
					    res = arg1 % arg2;
					ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SUB: res = arg1 - arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ADD: res = arg1 + arg2; ಭಙ್ಗ;

			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_EQUAL: res = arg1 == arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL: res = arg1 != arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GREATER: res = arg1 > arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL: res = arg1 >= arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SMALLER: res = arg1 < arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL: res = arg1 <= arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_LSHIFT: ಯದಿ (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  ಅನ್ಯಥಾ
					      res = (uvarnumber_T)arg1 << arg2;
					  ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_RSHIFT: ಯದಿ (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  ಅನ್ಯಥಾ
					      res = (uvarnumber_T)arg1 >> arg2;
					  ಭಙ್ಗ;
			ಸ್ವಭಾವ: ಭಙ್ಗ;
		    }

		    --ectx->ec_ರಾಶಿ.ga_len;
		    ಯದಿ (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ಅನ್ಯಥಾ
			tv1->vval.v_ಸಂಖ್ಯೆ = res;
		    ಯದಿ (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			ಗಚ್ಛ on_error;
		    }
		}
		ಭಙ್ಗ;

	    // Computation with two float arguments
	    ಪಕ್ಷ ISN_OPFLOAT:
	    ಪಕ್ಷ ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    ಪೂರ್ಣಾಙ್ಕ		cmp = FALSE;

		    ವಿಕಲ್ಪ (iptr->isn_arg.op.op_type)
		    {
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_MULT: res = arg1 * arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_DIV: res = arg1 / arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SUB: res = arg1 - arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ADD: res = arg1 + arg2; ಭಙ್ಗ;

			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_EQUAL: cmp = arg1 == arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL: cmp = arg1 != arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GREATER: cmp = arg1 > arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL: cmp = arg1 >= arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SMALLER: cmp = arg1 < arg2; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL: cmp = arg1 <= arg2; ಭಙ್ಗ;
			ಸ್ವಭಾವ: cmp = 0; ಭಙ್ಗ;
		    }
		    --ectx->ec_ರಾಶಿ.ga_len;
		    ಯದಿ (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_ಸಂಖ್ಯೆ = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ಅನ್ಯಥಾ
			tv1->vval.v_float = res;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_COMPARELIST:
	    ಪಕ್ಷ ISN_COMPARETUPLE:
	    ಪಕ್ಷ ISN_COMPAREDICT:
	    ಪಕ್ಷ ISN_COMPAREFUNC:
	    ಪಕ್ಷ ISN_COMPARESTRING:
	    ಪಕ್ಷ ISN_COMPAREBLOB:
	    ಪಕ್ಷ ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    ಪೂರ್ಣಾಙ್ಕ		ic = iptr->isn_arg.op.op_ic;
		    ಪೂರ್ಣಾಙ್ಕ		res = FALSE;
		    ಪೂರ್ಣಾಙ್ಕ		status = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (iptr->isn_type == ISN_COMPARELIST)
		    {
			status = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			status = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (iptr->isn_type == ISN_COMPAREDICT)
		    {
			status = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			status = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (iptr->isn_type == ISN_COMPARESTRING)
		    {
			status = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			status = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    ಅನ್ಯಥಾ // ISN_COMPAREOBJECT
		    {
			status = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_ರಾಶಿ.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
		    ಯದಿ (status == FAIL)
			ಗಚ್ಛ theend;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_COMPAREANY:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    ಪೂರ್ಣಾಙ್ಕ		ic = iptr->isn_arg.op.op_ic;
		    ಪೂರ್ಣಾಙ್ಕ		status;

		    SOURCING_LNUM = iptr->isn_lnum;
		    status = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_ರಾಶಿ.ga_len;
		    ಯದಿ (status == FAIL)
			ಗಚ್ಛ theend;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_ADDLIST:
	    ಪಕ್ಷ ISN_ADDTUPLE:
	    ಪಕ್ಷ ISN_ADDBLOB:
		{
		    typval_T *tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T *tv2 = ರಾಶಿ_TV_BOT(-1);

		    // add two lists or blobs
		    ಯದಿ (iptr->isn_type == ISN_ADDLIST)
		    {
			ಯದಿ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_APPEND
						   && tv1->vval.v_list != ರಿಕ್ತ)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 ರಿಕ್ತ);
			ಅನ್ಯಥಾ
			    eval_addlist(tv1, tv2);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    ಅನ್ಯಥಾ
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (l == ರಿಕ್ತ)
		    {
			emsg(_(e_cannot_add_to_null_list));
			ಗಚ್ಛ on_error;
		    }
		    ಯದಿ (ಮೌಲ್ಯ_check_lock(l->lv_lock, ರಿಕ್ತ, FALSE))
			ಗಚ್ಛ on_error;
		    ಯದಿ (list_append_tv(l, tv2) == FAIL)
			ಗಚ್ಛ theend;
		    clear_tv(tv2);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    ಯದಿ (b == ರಿಕ್ತ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_add_to_null_blob));
			ಗಚ್ಛ on_error;
		    }
		    n = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
		    ಯದಿ (error)
			ಗಚ್ಛ on_error;
		    ga_append(&b->bv_ga, (ಪೂರ್ಣಾಙ್ಕ)n);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    // Computation with two arguments of unknown type
	    ಪಕ್ಷ ISN_OPANY:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    ಪೂರ್ಣಾಙ್ಕ		error = FALSE;

		    ಯದಿ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_ADD)
		    {
			ಯದಿ (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ರಾಶಿ.ga_len;
			    ಭಙ್ಗ;
			}
			ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ರಾಶಿ.ga_len;
			    ಭಙ್ಗ;
			}
			ಅನ್ಯಥಾ ಯದಿ (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ರಾಶಿ.ga_len;
			    ಭಙ್ಗ;
			}
		    }
		    ಯದಿ (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    ಅನ್ಯಥಾ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv1, &error);
			ಯದಿ (error)
			    ಗಚ್ಛ on_error;
			ಯದಿ (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    ಯದಿ (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    ಅನ್ಯಥಾ
		    {
			n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
			ಯದಿ (error)
			    ಗಚ್ಛ on_error;
			ಯದಿ (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    ಯದಿ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			ವಿಕಲ್ಪ (iptr->isn_arg.op.op_type)
			{
			    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_MULT: f1 = f1 * f2; ಭಙ್ಗ;
			    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_DIV:  f1 = f1 / f2; ಭಙ್ಗ;
			    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SUB:  f1 = f1 - f2; ಭಙ್ಗ;
			    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ADD:  f1 = f1 + f2; ಭಙ್ಗ;
			    ಸ್ವಭಾವ: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     ಗಚ್ಛ on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_ರಾಶಿ.ga_len;
		    }
		    ಅನ್ಯಥಾ
		    {
			ಪೂರ್ಣಾಙ್ಕ failed = FALSE;

			ವಿಕಲ್ಪ (iptr->isn_arg.op.op_type)
			{
			    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_MULT: n1 = n1 * n2; ಭಙ್ಗ;
			    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_DIV:  n1 = ಅಂಕೆ_divide(n1, n2, &failed);
					    ಯದಿ (failed)
						ಗಚ್ಛ on_error;
					    ಭಙ್ಗ;
			    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SUB:  n1 = n1 - n2; ಭಙ್ಗ;
			    ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ADD:  n1 = n1 + n2; ಭಙ್ಗ;
			    ಸ್ವಭಾವ:	    n1 = ಅಂಕೆ_modulus(n1, n2, &failed);
					    ಯದಿ (failed)
						ಗಚ್ಛ on_error;
					    ಭಙ್ಗ;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_ಸಂಖ್ಯೆ;
			tv1->vval.v_ಸಂಖ್ಯೆ = n1;
			--ectx->ec_ರಾಶಿ.ga_len;
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_STRINDEX:
	    ಪಕ್ಷ ISN_STRSLICE:
		{
		    ಪೂರ್ಣಾಙ್ಕ		is_slice = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    ಯದಿ (is_slice)
		    {
			tv = ರಾಶಿ_TV_BOT(-2);
			n1 = tv->vval.v_ಸಂಖ್ಯೆ;
		    }

		    tv = ರಾಶಿ_TV_BOT(-1);
		    n2 = tv->vval.v_ಸಂಖ್ಯೆ;

		    ectx->ec_ರಾಶಿ.ga_len -= is_slice ? 2 : 1;
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಯದಿ (is_slice)
			// Slice: Select the characters from the string
			res = string_slice(tv->vval.v_string, n1, n2, FALSE);
		    ಅನ್ಯಥಾ
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_LISTINDEX:
	    ಪಕ್ಷ ISN_LISTSLICE:
	    ಪಕ್ಷ ISN_TUPLEINDEX:
	    ಪಕ್ಷ ISN_TUPLESLICE:
	    ಪಕ್ಷ ISN_BLOBINDEX:
	    ಪಕ್ಷ ISN_BLOBSLICE:
		{
		    ಪೂರ್ಣಾಙ್ಕ		is_slice = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    ಪೂರ್ಣಾಙ್ಕ		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    ಪೂರ್ಣಾಙ್ಕ		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_slice ? ರಾಶಿ_TV_BOT(-3) : ರಾಶಿ_TV_BOT(-2);

		    tv = ರಾಶಿ_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_ಸಂಖ್ಯೆ;
		    clear_tv(tv);

		    ಯದಿ (is_slice)
		    {
			tv = ರಾಶಿ_TV_BOT(-2);
			n1 = tv->vval.v_ಸಂಖ್ಯೆ;
			clear_tv(tv);
		    }

		    ectx->ec_ರಾಶಿ.ga_len -= is_slice ? 2 : 1;
		    tv = ರಾಶಿ_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (is_blob)
		    {
			ಯದಿ (blob_slice_or_index(val_tv->vval.v_blob, is_slice,
						    n1, n2, FALSE, tv) == FAIL)
			    ಗಚ್ಛ on_error;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (is_tuple)
		    {
			ಯದಿ (tuple_slice_or_index(val_tv->vval.v_tuple,
				is_slice, n1, n2, FALSE, tv, TRUE) == FAIL)
			    ಗಚ್ಛ on_error;
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (list_slice_or_index(val_tv->vval.v_list, is_slice,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    ಗಚ್ಛ on_error;
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_ANYINDEX:
	    ಪಕ್ಷ ISN_ANYSLICE:
		{
		    ಪೂರ್ಣಾಙ್ಕ		is_slice = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    ಪೂರ್ಣಾಙ್ಕ		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_slice ? ರಾಶಿ_TV_BOT(-3) : ರಾಶಿ_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (check_can_index(tv, TRUE, TRUE) == FAIL)
			ಗಚ್ಛ on_error;
		    var1 = is_slice ? ರಾಶಿ_TV_BOT(-2) : ರಾಶಿ_TV_BOT(-1);
		    var2 = is_slice ? ರಾಶಿ_TV_BOT(-1) : ರಿಕ್ತ;
		    res = eval_index_inner(tv, is_slice, var1, var2,
							FALSE, ರಿಕ್ತ, -1, TRUE);
		    clear_tv(var1);
		    ಯದಿ (is_slice)
			clear_tv(var2);
		    ectx->ec_ರಾಶಿ.ga_len -= is_slice ? 2 : 1;
		    ಯದಿ (res == FAIL)
			ಗಚ್ಛ on_error;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_SLICE:
		{
		    ಪೂರ್ಣಾಙ್ಕ		ಎಣಿಕೆ = iptr->isn_arg.ಸಂಖ್ಯೆ;

		    // type will have been checked to be a list
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಯದಿ (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			ಯದಿ (list != ರಿಕ್ತ && list->lv_len >= ಎಣಿಕೆ)
			{
			    list_T	*newlist = list_slice(list,
				    ಎಣಿಕೆ, list->lv_len - 1);

			    ಯದಿ (newlist != ರಿಕ್ತ)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    ಅನ್ಯಥಾ
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			ಯದಿ (tuple != ರಿಕ್ತ && TUPLE_LEN(tuple) >= ಎಣಿಕೆ)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_slice(tuple, ಎಣಿಕೆ,
							TUPLE_LEN(tuple) - 1);
			    ಯದಿ (newtuple != ರಿಕ್ತ)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*item_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = ರಾಶಿ_TV_BOT(-1 - gi->gi_with_op);
		    ಯದಿ (tv->v_type == VAR_LIST)
		    {
			li = list_find(tv->vval.v_list, gi->gi_index);
			ಯದಿ (li == ರಿಕ್ತ)
			    ಗಚ್ಛ on_error;
			item_tv = &li->li_tv;
		    }
		    ಅನ್ಯಥಾ
			item_tv = TUPLE_ITEM(tv->vval.v_tuple, gi->gi_index);

		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		    copy_tv(item_tv, ರಾಶಿ_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_index = gi->gi_index + 1;
		    ectx->ec_where.wt_kind = WT_VARIABLE;
		}
		ಭಙ್ಗ;

	    // dict member with string key (dict['member'])
	    ಪಕ್ಷ ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*ಕೀಲಿ;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = ರಾಶಿ_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = ರಾಶಿ_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    ಕೀಲಿ = tv->vval.v_string;
		    ಯದಿ (ಕೀಲಿ == ರಿಕ್ತ)
			ಕೀಲಿ = (char_u *)"";

		    ಯದಿ ((di = dict_find(dict, ಕೀಲಿ, -1)) == ರಿಕ್ತ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str), ಕೀಲಿ);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_ರಾಶಿ.ga_len;
			tv = ರಾಶಿ_TV_BOT(-1);
			(ಶೂನ್ಯ) dict_ರಾಶಿ_save(tv);
			tv->v_type = VAR_ಸಂಖ್ಯೆ;
			tv->vval.v_ಸಂಖ್ಯೆ = 0;
			ಗಚ್ಛ on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_ರಾಶಿ.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಯದಿ (dict_ರಾಶಿ_save(tv) == FAIL)
			ಗಚ್ಛ on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		ಭಙ್ಗ;

	    // dict member with string key (dict.member)
	    // or can be an object
	    ಪಕ್ಷ ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = ರಾಶಿ_TV_BOT(-1);

		    ಯದಿ (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			ಯದಿ (dict_ರಾಶಿ_save(tv) == FAIL)
			    ಗಚ್ಛ on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			ಯದಿ (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    ಗಚ್ಛ on_error;
		    }
		    ಅನ್ಯಥಾ
		    {
			ಯದಿ (tv->v_type != VAR_DICT || tv->vval.v_dict == ರಿಕ್ತ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    ಗಚ್ಛ on_error;
			}
			dict = tv->vval.v_dict;

			ಯದಿ ((di = dict_find(dict, iptr->isn_arg.string, -1))
								   == ರಿಕ್ತ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_ಕೀಲಿ_not_present_in_dictionary_str),
						     iptr->isn_arg.string);
			    ಗಚ್ಛ on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			ಯದಿ (dict_ರಾಶಿ_save(tv) == FAIL)
			    ಗಚ್ಛ on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_GET_OBJ_MEMBER:
	    ಪಕ್ಷ ISN_GET_ITF_MEMBER:
		{
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಯದಿ (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			ಗಚ್ಛ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ಯದಿ (obj == ರಿಕ್ತ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			ಗಚ್ಛ on_error;
		    }

		    ಪೂರ್ಣಾಙ್ಕ idx;
		    ಯದಿ (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    ಅನ್ಯಥಾ
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_index_from_itf_index(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    ಯದಿ (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			ಗಚ್ಛ on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_STORE_THIS:
		{
		    ಪೂರ್ಣಾಙ್ಕ idx = iptr->isn_arg.ಸಂಖ್ಯೆ;
		    object_T *obj = ರಾಶಿ_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *ರಾಶಿ_TV_BOT(-1);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CLEARDICT:
		dict_ರಾಶಿ_drop();
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_ರಾಶಿ_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಯದಿ (dict_tv != ರಿಕ್ತ
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != ರಿಕ್ತ
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == ರಿಕ್ತ))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_ರಾಶಿ_drop();
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_NEGATENR:
		tv = ರಾಶಿ_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		ಯದಿ (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		ಅನ್ಯಥಾ
		    tv->vval.v_ಸಂಖ್ಯೆ = -tv->vval.v_ಸಂಖ್ಯೆ;
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    ಪೂರ್ಣಾಙ್ಕ		r;
		    where_T	where = WHERE_INIT;

		    tv = ರಾಶಿ_TV_BOT((ಪೂರ್ಣಾಙ್ಕ)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಯದಿ (ct->ct_arg_idx > 0)
		    {
			where.wt_index = ct->ct_arg_idx;
			where.wt_kind = ct->ct_is_var ? WT_VARIABLE : WT_ARGUMENT;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    ಯದಿ (r == FAIL)
			ಗಚ್ಛ on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    ಯದಿ (tv->v_type == VAR_ಸಂಖ್ಯೆ
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_ಸಂಖ್ಯೆ == 0
						|| tv->vval.v_ಸಂಖ್ಯೆ == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_ಸಂಖ್ಯೆ = tv->vval.v_ಸಂಖ್ಯೆ
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CHECKLEN:
		{
		    ಪೂರ್ಣಾಙ್ಕ	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = ರಿಕ್ತ;
		    tuple_T *tuple = ರಿಕ್ತ;
		    ಪೂರ್ಣಾಙ್ಕ	    len = 0;

		    tv = ರಾಶಿ_TV_BOT(-1);

		    ಪೂರ್ಣಾಙ್ಕ		len_check_failed = FALSE;
		    ಯದಿ (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			ಯದಿ (list == ರಿಕ್ತ || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			ಯದಿ (list != ರಿಕ್ತ)
			    len = list->lv_len;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			ಯದಿ (tuple == ರಿಕ್ತ || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			ಯದಿ (tuple != ರಿಕ್ತ)
			    len = TUPLE_LEN(tuple);
		    }
		    ಅನ್ಯಥಾ
			len_check_failed = TRUE;

		    ಯದಿ (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_ವಸ್ತುಗಳು_but_got_nr),
				     min_len, len);
			ಗಚ್ಛ on_error;
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_SETTYPE:
		set_tv_type(ರಾಶಿ_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_2BOOL:
	    ಪಕ್ಷ ISN_COND2BOOL:
		{
		    ಪೂರ್ಣಾಙ್ಕ n;
		    ಪೂರ್ಣಾಙ್ಕ error = FALSE;

		    ಯದಿ (iptr->isn_type == ISN_2BOOL)
		    {
			tv = ರಾಶಿ_TV_BOT(iptr->isn_arg.tobool.offset);
			n = tv2bool(tv);
			ಯದಿ (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    ಅನ್ಯಥಾ
		    {
			tv = ರಾಶಿ_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			ಯದಿ (error)
			    ಗಚ್ಛ on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_ಸಂಖ್ಯೆ = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_2STRING:
	    ಪಕ್ಷ ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		ಯದಿ (do_2string(ರಾಶಿ_TV_BOT(iptr->isn_arg.tostring.offset),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    ಗಚ್ಛ on_error;
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_RANGE:
		{
		    exarg_T	ea;
		    ವರ್ಣ	*errormsg;

		    ea.line2 = 0;
		    ea.addr_ಎಣಿಕೆ = 0;
		    ea.addr_type = ADDR_ಸಾಲುಗಳು;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    ಯದಿ (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
			ಗಚ್ಛ on_error;

		    ಯದಿ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಗಚ್ಛ theend;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		    tv = ರಾಶಿ_TV_BOT(-1);
		    tv->v_type = VAR_ಸಂಖ್ಯೆ;
		    tv->v_lock = 0;
		    tv->vval.v_ಸಂಖ್ಯೆ = ea.line2;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_PUT:
		tv = ರಿಕ್ತ; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_IPUT:
		tv = ರಿಕ್ತ;
		isn_put_do(ectx, iptr, tv, TRUE);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_ರಾಶಿ.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = ರಿಕ್ತ;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = ರಾಶಿ_TV_BOT(-1);
		ಯದಿ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_argument_must_be_sequence_of_lists_or_tuples));
		    ಗಚ್ಛ on_error;
		}

		ಪೂರ್ಣಾಙ್ಕ rc;
		ಯದಿ (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		ಅನ್ಯಥಾ
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		ಯದಿ (rc != EXEC_OK)
		{
		    ಯದಿ (rc == EXEC_FAIL)
			ಗಚ್ಛ on_error;
		    ಗಚ್ಛ theend;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_PROF_START:
	    ಪಕ್ಷ ISN_PROF_END:
		{
#ifdef FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    ಯದಿ (iptr->isn_type == ISN_PROF_START)
		    {
			func_ಸಾಲು_start(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_ಸಾಲು_exec(&cookie);
		    }
		    ಅನ್ಯಥಾ
			func_ಸಾಲು_end(&cookie);
#endif
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_DEBUG:
		handle_debug(iptr, ectx);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    ಪೂರ್ಣಾಙ್ಕ		item = iptr->isn_arg.shuffle.shfl_item;
		    ಪೂರ್ಣಾಙ್ಕ		up = iptr->isn_arg.shuffle.shfl_up;

		    tmp_tv = *ರಾಶಿ_TV_BOT(-item);
		    ಪ್ರತಿ ( ; up > 0 && item > 1; --up)
		    {
			*ರಾಶಿ_TV_BOT(-item) = *ರಾಶಿ_TV_BOT(-item + 1);
			--item;
		    }
		    *ರಾಶಿ_TV_BOT(-item) = tmp_tv;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_DROP:
		--ectx->ec_ರಾಶಿ.ga_len;
		clear_tv(ರಾಶಿ_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_SCRIPTCTX_SET:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		ಭಙ್ಗ;
	}
	ಅನುವರ್ತ;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	ಯದಿ (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
	    ಗಚ್ಛ done;

	ಯದಿ (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    ಗಚ್ಛ theend;
	ಅನುವರ್ತ;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	ಯದಿ (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    ಯದಿ (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		ಯಾವತ್ (ectx->ec_ರಾಶಿ.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_ರಾಶಿ.ga_len;
		    clear_tv(ರಾಶಿ_TV_BOT(0));
		}
		ಯಾವತ್ (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    ಅನುವರ್ತ;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	ಯದಿ (trylevel <= ectx->ec_trylevel_at_start)
	    ಗಚ್ಛ theend;
    }

done:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_ರಾಶಿ_clear(dict_ರಾಶಿ_len_at_start);
    ectx->ec_trylevel_at_start = save_trylevel_at_start;
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    ಪೂರ್ಣಾಙ್ಕ		save_iidx = ectx->ec_iidx;
    ಪೂರ್ಣಾಙ್ಕ		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    ಯದಿ (res == OK)
    {
	*rettv = *ರಾಶಿ_TV_BOT(-1);
	--ectx->ec_ರಾಶಿ.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(ಶೂನ್ಯ)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    ಪೂರ್ಣಾಙ್ಕ		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    ಯದಿ (exec_instructions(ectx) == OK)
    {
	typval_T *tv = ರಾಶಿ_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_ರಾಶಿ.ga_len;
	clear_tv(tv);
    }
    ಅನ್ಯಥಾ
    {
	substitute_instr->subs_status = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    ಪ್ರತ್ಯಾಗಮ res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
call_def_function(
    ufunc_T	*ufunc,
    ಪೂರ್ಣಾಙ್ಕ		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    ಪೂರ್ಣಾಙ್ಕ		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    ಪೂರ್ಣಾಙ್ಕ		argc = argc_arg;
    ಪೂರ್ಣಾಙ್ಕ		partial_argc = partial == ರಿಕ್ತ
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    ಪೂರ್ಣಾಙ್ಕ		total_argc = argc + partial_argc;
    typval_T	*tv;
    ಪೂರ್ಣಾಙ್ಕ		idx;
    ಪೂರ್ಣಾಙ್ಕ		ret = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		defcount = ufunc->uf_args.ga_len - total_argc;
    sctx_T	save_current_sctx = current_sctx;
    ಪೂರ್ಣಾಙ್ಕ		did_emsg_before = did_emsg_cumul + did_emsg;
    ಪೂರ್ಣಾಙ್ಕ		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_ಸಂದೇಶ_list = ರಿಕ್ತ;
    msglist_T	*private_ಸಂದೇಶ_list = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		save_emsg_silent_def = emsg_silent_def;
    ಪೂರ್ಣಾಙ್ಕ		save_did_emsg_def = did_emsg_def;
    ಪೂರ್ಣಾಙ್ಕ		orig_funcdepth;
    ಪೂರ್ಣಾಙ್ಕ		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#undef ರಾಶಿ_TV
#define ರಾಶಿ_TV(idx) (((typval_T *)ectx.ec_ರಾಶಿ.ga_data) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#undef ರಾಶಿ_TV_BOT
#define ರಾಶಿ_TV_BOT(idx) (((typval_T *)ectx.ec_ರಾಶಿ.ga_data) + ectx.ec_ರಾಶಿ.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#undef ರಾಶಿ_TV_VAR
#define ರಾಶಿ_TV_VAR(idx) (((typval_T *)ectx.ec_ರಾಶಿ.ga_data) + ectx.ec_frame_idx + ರಾಶಿ_FRAME_SIZE + idx)

    ಯದಿ (ufunc->uf_def_status == UF_NOT_COMPILED
	    || ufunc->uf_def_status == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), ರಿಕ್ತ) == FAIL))
    {
	ಯದಿ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ಯದಿ (dfunc->df_ufunc == ರಿಕ್ತ)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (INSTRUCTIONS(dfunc) == ರಿಕ್ತ)
	{
	    iemsg("using call_def_function() on not compiled function");
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    ಯದಿ (funcdepth_increment() == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    CLEAR_FIELD(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_ರಾಶಿ, ಪರಿಮಾಣ(typval_T), 500);
    ಯದಿ (GA_GROW_FAILS(&ectx.ec_ರಾಶಿ, 20))
    {
	funcdepth_decrement();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ga_init2(&ectx.ec_trystack, ಪರಿಮಾಣ(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, ಪರಿಮಾಣ(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = total_argc - ufunc->uf_args.ga_len;
    ಯದಿ (idx > 0 && ufunc->uf_va_name == ರಿಕ್ತ)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
								    idx), idx);
	ಗಚ್ಛ failed_early;
    }
    idx = total_argc - ufunc->uf_args.ga_len + ufunc->uf_def_args.ga_len;
    ಯದಿ (idx < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	ಗಚ್ಛ failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    ಪ್ರತಿ (idx = 0; idx < total_argc
	    && (ufunc->uf_va_name != ರಿಕ್ತ || idx < ufunc->uf_args.ga_len);
									 ++idx)
    {
	ಪೂರ್ಣಾಙ್ಕ argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	ಯದಿ (idx >= ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
	{
	    // Use the default value.
	    ರಾಶಿ_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ done = FALSE;
	    ಯದಿ (ufunc->uf_arg_types != ರಿಕ್ತ && idx < ufunc->uf_args.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		ಯದಿ (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_ಸಂಖ್ಯೆ)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    ರಾಶಿ_TV_BOT(0)->v_type = VAR_FLOAT;
		    ರಾಶಿ_TV_BOT(0)->v_lock = 0;
		    ರಾಶಿ_TV_BOT(0)->vval.v_float = tv->vval.v_ಸಂಖ್ಯೆ;
		    done = TRUE;
		}
		ಅನ್ಯಥಾ ಯದಿ (check_typval_arg_type(expected, tv,
						   ರಿಕ್ತ, argv_idx + 1) == FAIL)
		    ಗಚ್ಛ failed_early;
	    }
	    ಯದಿ (!done)
		copy_tv(tv, ರಾಶಿ_TV_BOT(0));
	}
	++ectx.ec_ರಾಶಿ.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    ಯದಿ (ufunc->uf_va_name != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ vararg_ಎಣಿಕೆ = argc - ufunc->uf_args.ga_len;

	ಯದಿ (vararg_ಎಣಿಕೆ < 0)
	    vararg_ಎಣಿಕೆ = 0;
	ಅನ್ಯಥಾ
	    argc -= vararg_ಎಣಿಕೆ;
	ಯದಿ (exe_newlist(vararg_ಎಣಿಕೆ, &ectx) == FAIL)
	    ಗಚ್ಛ failed_early;

	// Check the type of the list items.
	tv = ರಾಶಿ_TV_BOT(-1);
	ಯದಿ (ufunc->uf_va_type != ರಿಕ್ತ
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != ರಿಕ್ತ)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_first;

	    ಪ್ರತಿ (idx = 0; idx < vararg_ಎಣಿಕೆ; ++idx)
	    {
		ಯದಿ (check_typval_arg_type(expected, &li->li_tv,
						 ರಿಕ್ತ, argc + idx + 1) == FAIL)
		    ಗಚ್ಛ failed_early;
		li = li->li_next;
	    }
	}

	ಯದಿ (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *ರಾಶಿ_TV_BOT(defcount - 1) = *ರಾಶಿ_TV_BOT(-1);
	--ectx.ec_ರಾಶಿ.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    ಯದಿ (defcount > 0)
	ಪ್ರತಿ (idx = 0; idx < defcount; ++idx)
	{
	    ರಾಶಿ_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_ರಾಶಿ.ga_len;
	}
    ಯದಿ (ufunc->uf_va_name != ರಿಕ್ತ)
	++ectx.ec_ರಾಶಿ.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_frame_idx = ectx.ec_ರಾಶಿ.ga_len;
    ectx.ec_initial_frame_idx = ectx.ec_frame_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *base_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	ಯದಿ (partial != ರಿಕ್ತ || base_ufunc->uf_partial != ರಿಕ್ತ)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    ಯದಿ (ectx.ec_outer_ref == ರಿಕ್ತ)
		ಗಚ್ಛ failed_early;
	    ಯದಿ (partial != ರಿಕ್ತ)
	    {
		outer_T *outer = get_pt_outer(partial);

		ಯದಿ (outer->out_ರಾಶಿ == ರಿಕ್ತ && outer->out_loop_size == 0)
		{
		    // no stack was set
		    ಯದಿ (current_ectx != ರಿಕ್ತ)
		    {
			ಯದಿ (current_ectx->ec_outer_ref != ರಿಕ್ತ
			       && current_ectx->ec_outer_ref->or_outer != ರಿಕ್ತ)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		ಅನ್ಯಥಾ
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    ಅನ್ಯಥಾ
	    {
		ectx.ec_outer_ref->or_outer = &base_ufunc->uf_partial->pt_outer;
		++base_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = base_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    ಪ್ರತಿ (idx = 0; idx < ರಾಶಿ_FRAME_SIZE; ++idx)
    {
	ರಾಶಿ_TV(ectx.ec_ರಾಶಿ.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_ರಾಶಿ.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	ಪ್ರತಿ (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    ರಾಶಿ_TV_VAR(idx)->v_type = VAR_ಸಂಖ್ಯೆ;
	    ರಾಶಿ_TV_VAR(idx)->vval.v_ಸಂಖ್ಯೆ = 0;
	}
	ectx.ec_ರಾಶಿ.ga_len += dfunc->df_varcount;

	ಯದಿ (object != ರಿಕ್ತ)
	{
	    // the object is always the variable at index zero
	    tv = ರಾಶಿ_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	ಯದಿ (dfunc->df_has_closure)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    ರಾಶಿ_TV_VAR(idx)->v_type = VAR_ಸಂಖ್ಯೆ;
	    ರಾಶಿ_TV_VAR(idx)->vval.v_ಸಂಖ್ಯೆ = 0;
	    ++ectx.ec_ರಾಶಿ.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    ಯದಿ (funccal != ರಿಕ್ತ)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_ತಳ್ಳು_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_ಸಂದೇಶ_list = ಸಂದೇಶ_list;
    ಸಂದೇಶ_list = &private_ಸಂದೇಶ_list;

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
    ಯದಿ (ret == OK)
    {
	// function finished, get result from the stack.
	ಯದಿ (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	ಅನ್ಯಥಾ
	{
	    tv = ರಾಶಿ_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    ಯದಿ (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_closure_in_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_ತೆಗೆ();
    current_sctx = save_current_sctx;

    ಯದಿ (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    ಯದಿ (*ಸಂದೇಶ_list != ರಿಕ್ತ && saved_ಸಂದೇಶ_list != ರಿಕ್ತ)
    {
	msglist_T **plist = saved_ಸಂದೇಶ_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	ಯಾವತ್ (*plist != ರಿಕ್ತ)
	    plist = &(*plist)->next;

	*plist = *ಸಂದೇಶ_list;
    }
    ಸಂದೇಶ_list = saved_ಸಂದೇಶ_list;

    ಯದಿ (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ರಿಕ್ತ;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    ಪ್ರತಿ (idx = 0; idx < ectx.ec_ರಾಶಿ.ga_len; ++idx)
    {
	tv = ರಾಶಿ_TV(idx);
	ಯದಿ (tv->v_type != VAR_ಸಂಖ್ಯೆ && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_ರಾಶಿ.ga_data);
    vim_free(ectx.ec_trystack.ga_data);
    ಯದಿ (ectx.ec_outer_ref != ರಿಕ್ತ)
    {
	ಯದಿ (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    ಯದಿ (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    ಶೂನ್ಯ
unwind_def_callstack(ectx_T *ectx)
{
    ಯಾವತ್ (ectx->ec_frame_idx != ectx->ec_initial_frame_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    ಶೂನ್ಯ
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    ಯದಿ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    ಸ್ಥಿರ char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    ಪೂರ್ಣಾಙ್ಕ		depth;

    ga_init2(&ga, 1, 100);
    ಪ್ರತಿ (depth = 0; depth < lvi->lvi_depth; ++depth)
    {
	ಯದಿ (ga_grow(&ga, 50) == FAIL)
	    ಭಙ್ಗ;
	ಯದಿ (lvi->lvi_loop[depth].var_idx == 0)
	    STRCPY((ವರ್ಣ *)ga.ga_data + ga.ga_len, " -");
	ಅನ್ಯಥಾ
	    vim_snprintf((ವರ್ಣ *)ga.ga_data + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[depth].var_idx,
			    lvi->lvi_loop[depth].var_idx
					 + lvi->lvi_loop[depth].var_ಎಣಿಕೆ - 1);
	ga.ga_len = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(ga.ga_data);
    }
    ಪ್ರತ್ಯಾಗಮ ga.ga_data;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_instructions(ವರ್ಣ *pfx, isn_T *instr, ಪೂರ್ಣಾಙ್ಕ instr_ಎಣಿಕೆ, ufunc_T *ufunc)
{
    ಪೂರ್ಣಾಙ್ಕ		ಸಾಲು_idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		prev_current = 0;
    ಪೂರ್ಣಾಙ್ಕ		current;
    ಪೂರ್ಣಾಙ್ಕ		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    ಪ್ರತಿ (current = 0; current < instr_ಎಣಿಕೆ; ++current)
    {
	isn_T	    *iptr = &instr[current];
	ವರ್ಣ	    *ಸಾಲು;

	ಯದಿ (ufunc != ರಿಕ್ತ)
	{
	    ಯಾವತ್ (ಸಾಲು_idx < iptr->isn_lnum
					  && ಸಾಲು_idx < ufunc->uf_ಸಾಲುಗಳು.ga_len)
	    {
		ಯದಿ (current > prev_current)
		{
		    ಸಂದೇಶ_puts("\n\n");
		    prev_current = current;
		}
		ಸಾಲು = ((ವರ್ಣ **)ufunc->uf_ಸಾಲುಗಳು.ga_data)[ಸಾಲು_idx++];
		ಯದಿ (ಸಾಲು != ರಿಕ್ತ)
		    ಸಂದೇಶ(ಸಾಲು);
	    }
	    ಯದಿ (iptr->isn_type == ISN_JUMP_IF_ARG_SET)
	    {
		ಪೂರ್ಣಾಙ್ಕ	first_def_arg = ufunc->uf_args.ga_len
						   - ufunc->uf_def_args.ga_len;

		ಯದಿ (def_arg_idx > 0)
		    ಸಂದೇಶ_puts("\n\n");
		ಸಂದೇಶ_start();
		ಸಂದೇಶ_puts("  ");
		ಸಂದೇಶ_puts(((ವರ್ಣ **)(ufunc->uf_args.ga_data))[
						 first_def_arg + def_arg_idx]);
		ಸಂದೇಶ_puts(" = ");
		ಸಂದೇಶ_puts(((ವರ್ಣ **)(ufunc->uf_def_args.ga_data))[def_arg_idx++]);
		ಸಂದೇಶ_clr_eos();
		ಸಂದೇಶ_end();
	    }
	}

	ವಿಕಲ್ಪ (iptr->isn_type)
	{
	    ಪಕ್ಷ ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (ಪೂರ್ಣಾಙ್ಕ)iptr->isn_arg.construct.construct_size);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_EXEC_SPLIT:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.ಸಂಖ್ಯೆ ? " append" : "");
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.ಸಂಖ್ಯೆ));
#endif
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#endif
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, INT_MAX, ರಿಕ್ತ);
		ಸಂದೇಶ("     -------------");
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_SOURCE:
		{
		    scriptitem_T    *si = SCRIPT_ITEM(iptr->isn_arg.ಸಂಖ್ಯೆ);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, INT_MAX, ರಿಕ್ತ);
		    ಸಂದೇಶ("     -------------");
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.ಸಂಖ್ಯೆ);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_ಎಣಿಕೆ);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ECHOWINDOW:
		ಯದಿ (iptr->isn_arg.echowin.ewin_time > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_ಎಣಿಕೆ,
				      iptr->isn_arg.echowin.ewin_time);
		ಅನ್ಯಥಾ
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_ಎಣಿಕೆ);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOAD:
		{
		    ಯದಿ (iptr->isn_arg.ಸಂಖ್ಯೆ < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ
							  + ರಾಶಿ_FRAME_SIZE));
		    ಅನ್ಯಥಾ
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ಯದಿ (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_depth,
					outer->outer_idx + ರಾಶಿ_FRAME_SIZE);
		    ಅನ್ಯಥಾ ಯದಿ (outer->outer_depth < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_depth);
		    ಅನ್ಯಥಾ
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_depth,
					      outer->outer_idx);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ಯದಿ (sv == ರಿಕ್ತ)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    ಅನ್ಯಥಾ
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADS:
	    ಪಕ್ಷ ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (ಪೂರ್ಣಾಙ್ಕ)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_STORE:
		ಯದಿ (iptr->isn_arg.ಸಂಖ್ಯೆ < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.ಸಂಖ್ಯೆ + ರಾಶಿ_FRAME_SIZE);
		ಅನ್ಯಥಾ
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.ಸಂಖ್ಯೆ);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ಯದಿ (outer->outer_depth < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_depth);
		    ಅನ್ಯಥಾ
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_depth, outer->outer_idx);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STORES:
	    ಪಕ್ಷ ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ಯದಿ (sv == ರಿಕ್ತ)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    ಅನ್ಯಥಾ
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STOREOPT:
	    ಪಕ್ಷ ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (ಪೂರ್ಣಾಙ್ಕ)iptr->isn_arg.ಸಂಖ್ಯೆ);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_LOAD_CLASSMEMBER:
	    ಪಕ್ಷ ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    ಪೂರ್ಣಾಙ್ಕ	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_LOAD_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		ಭಙ್ಗ;

	    // constants
	    ಪಕ್ಷ ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHBOOL:
	    ಪಕ್ಷ ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHFUNC:
		{
		    ವರ್ಣ *name = (ವರ್ಣ *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == ರಿಕ್ತ ? "[none]" : name);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#endif
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#endif
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == ರಿಕ್ತ ? "null"
				 : (ವರ್ಣ *)iptr->isn_arg.classarg->class_name.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		ಭಙ್ಗ;

	    // function call
	    ಪಕ್ಷ ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    internal_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != ರಿಕ್ತ
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_ತುದಿ ? " top" : "", cpfunc->cpf_argcount);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PCALL_END:
		smsg("%s%4d PCALL end", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (ಪೂರ್ಣಾಙ್ಕ)iptr->isn_arg.defer.defer_argcount);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    ಯದಿ (extra == ರಿಕ್ತ || extra->fre_func_name == ರಿಕ್ತ)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    ಅನ್ಯಥಾ
			name = extra->fre_func_name;
		    ಯದಿ (extra != ರಿಕ್ತ && extra->fre_class != ರಿಕ್ತ)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    ಅನ್ಯಥಾ ಯದಿ (extra == ರಿಕ್ತ
				     || extra->fre_loopvar_info.lvi_depth == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    ಅನ್ಯಥಾ
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ಯದಿ (arg->nfa_loopvar_info.lvi_depth == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    ಅನ್ಯಥಾ
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == ರಿಕ್ತ ? (char_u *)"" : name);
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_JUMP:
		{
		    ವರ್ಣ *when = "?";

		    ವಿಕಲ್ಪ (iptr->isn_arg.jump.jump_when)
		    {
			ಪಕ್ಷ JUMP_ALWAYS:
			    when = "JUMP";
			    ಭಙ್ಗ;
			ಪಕ್ಷ JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    ಭಙ್ಗ;
			ಪಕ್ಷ JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    ಭಙ್ಗ;
			ಪಕ್ಷ JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    ಭಙ್ಗ;
			ಪಕ್ಷ JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  // unused
			    ಭಙ್ಗ;
			ಪಕ್ಷ JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    ಭಙ್ಗ;
			ಪಕ್ಷ JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    ಭಙ್ಗ;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_JUMP_IF_ARG_SET:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ರಾಶಿ_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_JUMP_IF_ARG_NOT_SET:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ರಾಶಿ_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_end);
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->end_funcref_idx,
			    endloop->end_var_idx,
			    endloop->end_var_idx + endloop->end_var_ಎಣಿಕೆ - 1,
			    endloop->end_depth);
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_end);
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    ಯದಿ (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    ಅನ್ಯಥಾ
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_levels,
				      trycont->tct_levels == 1 ? "" : "s",
				      trycont->tct_where);
		}
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		ಭಙ್ಗ;

	    // expression operations on number
	    ಪಕ್ಷ ISN_OPNR:
	    ಪಕ್ಷ ISN_OPFLOAT:
	    ಪಕ್ಷ ISN_OPANY:
		{
		    ವರ್ಣ *what;
		    ವರ್ಣ *ins;

		    ವಿಕಲ್ಪ (iptr->isn_arg.op.op_type)
		    {
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_MULT: what = "*"; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_DIV: what = "/"; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_REM: what = "%"; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SUB: what = "-"; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ADD: what = "+"; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_LSHIFT: what = "<<"; ಭಙ್ಗ;
			ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_RSHIFT: what = ">>"; ಭಙ್ಗ;
			ಸ್ವಭಾವ:       what = "???"; ಭಙ್ಗ;
		    }
		    ವಿಕಲ್ಪ (iptr->isn_type)
		    {
			ಪಕ್ಷ ISN_OPNR: ins = "OPNR"; ಭಙ್ಗ;
			ಪಕ್ಷ ISN_OPFLOAT: ins = "OPFLOAT"; ಭಙ್ಗ;
			ಪಕ್ಷ ISN_OPANY: ins = "OPANY"; ಭಙ್ಗ;
			ಸ್ವಭಾವ: ins = "???"; ಭಙ್ಗ;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_COMPAREBOOL:
	    ಪಕ್ಷ ISN_COMPARESPECIAL:
	    ಪಕ್ಷ ISN_COMPARENULL:
	    ಪಕ್ಷ ISN_COMPARENR:
	    ಪಕ್ಷ ISN_COMPAREFLOAT:
	    ಪಕ್ಷ ISN_COMPARESTRING:
	    ಪಕ್ಷ ISN_COMPAREBLOB:
	    ಪಕ್ಷ ISN_COMPARELIST:
	    ಪಕ್ಷ ISN_COMPARETUPLE:
	    ಪಕ್ಷ ISN_COMPAREDICT:
	    ಪಕ್ಷ ISN_COMPAREFUNC:
	    ಪಕ್ಷ ISN_COMPAREOBJECT:
	    ಪಕ್ಷ ISN_COMPAREANY:
		   {
		       ವರ್ಣ *p;
		       ವರ್ಣ buf[10];
		       ವರ್ಣ *type;

		       ವಿಕಲ್ಪ (iptr->isn_arg.op.op_type)
		       {
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:	 p = "=="; ಭಙ್ಗ;
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:    p = "!="; ಭಙ್ಗ;
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GREATER:   p = ">"; ಭಙ್ಗ;
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL:    p = ">="; ಭಙ್ಗ;
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SMALLER:   p = "<"; ಭಙ್ಗ;
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL:    p = "<="; ಭಙ್ಗ;
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_MATCH:	 p = "=~"; ಭಙ್ಗ;
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_IS:	 p = "is"; ಭಙ್ಗ;
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:	 p = "isnot"; ಭಙ್ಗ;
			   ಪಕ್ಷ ಅಭಿವ್ಯಕ್ತಿ_NOMATCH:	 p = "!~"; ಭಙ್ಗ;
			   ಸ್ವಭಾವ:  p = "???"; ಭಙ್ಗ;
		       }
		       STRCPY(buf, p);
		       ಯದಿ (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       ವಿಕಲ್ಪ(iptr->isn_type)
		       {
			   ಪಕ್ಷ ISN_COMPAREBOOL: type = "COMPAREBOOL"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPARENULL: type = "COMPARENULL"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPARENR: type = "COMPARENR"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPARESTRING:
						  type = "COMPARESTRING"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPAREBLOB: type = "COMPAREBLOB"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPARELIST: type = "COMPARELIST"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPARETUPLE: type = "COMPARETUPLE"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPAREDICT: type = "COMPAREDICT"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPAREFUNC: type = "COMPAREFUNC"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; ಭಙ್ಗ;
			   ಪಕ್ಷ ISN_COMPAREANY: type = "COMPAREANY"; ಭಙ್ಗ;
			   ಸ್ವಭಾವ: type = "???"; ಭಙ್ಗ;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); ಭಙ್ಗ;

	    // expression operations
	    ಪಕ್ಷ ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_SLICE: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.ಸಂಖ್ಯೆ); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_index,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (ಪೂರ್ಣಾಙ್ಕ)iptr->isn_arg.classmember.cm_idx);
				     ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (ಪೂರ್ಣಾಙ್ಕ)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (ಪೂರ್ಣಾಙ್ಕ)iptr->isn_arg.ಸಂಖ್ಯೆ); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      ವರ್ಣ	    *tofree = ರಿಕ್ತ;
		      ವರ್ಣ	    *typename;

		      ಯದಿ (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_ಸಂಖ್ಯೆ_OK))
			  typename = "float|number";
		      ಅನ್ಯಥಾ ಯದಿ (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      ಅನ್ಯಥಾ
			  typename = type_name(ct->ct_type, &tofree);

		      ಯದಿ (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (ಪೂರ್ಣಾಙ್ಕ)ct->ct_off);
		      ಅನ್ಯಥಾ
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (ಪೂರ್ಣಾಙ್ಕ)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (ಪೂರ್ಣಾಙ್ಕ)ct->ct_arg_idx);
		      vim_free(tofree);
		      ಭಙ್ಗ;
		  }
	    ಪಕ್ಷ ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_more_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_SETTYPE:
		  {
		      ವರ್ಣ *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      ಭಙ್ಗ;
		  }
	    ಪಕ್ಷ ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_2BOOL: ಯದಿ (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    ಅನ್ಯಥಾ
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_PUT:
		ಯದಿ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ಅನ್ಯಥಾ ಯದಿ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ಅನ್ಯಥಾ
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_IPUT:
		ಯದಿ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ಅನ್ಯಥಾ ಯದಿ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ಅನ್ಯಥಾ
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  ರಿಕ್ತ, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    ಯದಿ (likely(buf != ರಿಕ್ತ))
		    {
			(ಶೂನ್ಯ)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    ಭಙ್ಗ;
		}
	    ಪಕ್ಷ ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_PROF_START:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_PROF_END:
		smsg("%s%4d PROFILE END", pfx, current);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_names_len);
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_ಎಣಿಕೆ,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_item,
					 iptr->isn_arg.shuffle.shfl_up);
			      ಭಙ್ಗ;
	    ಪಕ್ಷ ISN_DROP: smsg("%s%4d DROP", pfx, current); ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_SCRIPTCTX_SET:
		{
		    ಪೂರ್ಣಾಙ್ಕ	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ITEM(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		ಭಙ್ಗ;

	    ಪಕ್ಷ ISN_FINISH: // End of list of instructions for ISN_SUBSTITUTE.
			   ಪ್ರತ್ಯಾಗಮ;
	}

	out_flush();	    // output one line at a time
	ui_breakcheck();
	ಯದಿ (got_int)
	    ಭಙ್ಗ;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    ಶೂನ್ಯ
set_context_in_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_pattern = arg;

    // first argument already typed: only user function names
    ಯದಿ (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
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
get_disassemble_argument(expand_T *xp, ಪೂರ್ಣಾಙ್ಕ idx)
{
    ಯದಿ (idx == 0)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"debug";
    ಯದಿ (idx == 1)
	ಪ್ರತ್ಯಾಗಮ (char_u *)"profile";
    ಪ್ರತ್ಯಾಗಮ get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    ಶೂನ್ಯ
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = ರಿಕ್ತ;  // init to shut up gcc warning
    ಪೂರ್ಣಾಙ್ಕ		instr_ಎಣಿಕೆ = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = find_func_by_name(arg, &compile_type);
    ಯದಿ (ufunc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, ರಿಕ್ತ) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಸಂದೇಶ((ವರ್ಣ *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    ವಿಕಲ್ಪ (compile_type)
    {
	ಪಕ್ಷ CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_ಎಣಿಕೆ = dfunc->df_instr_prof_ಎಣಿಕೆ;
	    ಭಙ್ಗ;
#endif
	    // FALLTHROUGH
	ಪಕ್ಷ CT_NONE:
	    instr = dfunc->df_instr;
	    instr_ಎಣಿಕೆ = dfunc->df_instr_ಎಣಿಕೆ;
	    ಭಙ್ಗ;
	ಪಕ್ಷ CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_ಎಣಿಕೆ = dfunc->df_instr_debug_ಎಣಿಕೆ;
	    ಭಙ್ಗ;
    }

    list_instructions("", instr, instr_ಎಣಿಕೆ, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
tv2bool(typval_T *tv)
{
    ವಿಕಲ್ಪ (tv->v_type)
    {
	ಪಕ್ಷ VAR_ಸಂಖ್ಯೆ:
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_ಸಂಖ್ಯೆ != 0;
	ಪಕ್ಷ VAR_FLOAT:
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_float != 0.0;
	ಪಕ್ಷ VAR_PARTIAL:
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_partial != ರಿಕ್ತ;
	ಪಕ್ಷ VAR_FUNC:
	ಪಕ್ಷ VAR_STRING:
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_string != ರಿಕ್ತ && *tv->vval.v_string != NUL;
	ಪಕ್ಷ VAR_LIST:
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_list != ರಿಕ್ತ && tv->vval.v_list->lv_len > 0;
	ಪಕ್ಷ VAR_TUPLE:
	    ಪ್ರತ್ಯಾಗಮ tuple_len(tv->vval.v_tuple) > 0;
	ಪಕ್ಷ VAR_DICT:
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_dict != ರಿಕ್ತ
				    && tv->vval.v_dict->dv_hashtab.ht_used > 0;
	ಪಕ್ಷ VAR_BOOL:
	ಪಕ್ಷ VAR_SPECIAL:
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE ? TRUE : FALSE;
	ಪಕ್ಷ VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_job != ರಿಕ್ತ;
#ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_channel != ರಿಕ್ತ;
#ಅನ್ಯಥಾ
	    ಭಙ್ಗ;
#endif
	ಪಕ್ಷ VAR_BLOB:
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_blob != ರಿಕ್ತ && tv->vval.v_blob->bv_ga.ga_len > 0;

	ಪಕ್ಷ VAR_OBJECT:
	    ಪ್ರತ್ಯಾಗಮ tv->vval.v_object != ರಿಕ್ತ;

	ಪಕ್ಷ VAR_CLASS:
	ಪಕ್ಷ VAR_TYPEALIAS:
	    check_typval_is_ಮೌಲ್ಯ(tv);
	    ಭಙ್ಗ;

	ಪಕ್ಷ VAR_UNKNOWN:
	ಪಕ್ಷ VAR_ANY:
	ಪಕ್ಷ VAR_VOID:
	ಪಕ್ಷ VAR_INSTR:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಶೂನ್ಯ
emsg_using_string_as(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ as_ಸಂಖ್ಯೆ)
{
    semsg(_(as_ಸಂಖ್ಯೆ ? e_using_string_as_ಸಂಖ್ಯೆ_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == ರಿಕ್ತ
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_not_string(typval_T *tv)
{
    ಯದಿ (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

#endif // FEAT_EVAL
