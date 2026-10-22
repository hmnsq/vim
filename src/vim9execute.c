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

#ವ್ಯಾಖ್ಯಾನಿಸು USING_FLOAT_STUFF
#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ	    tcd_ಚೌಕಟ್ಟು_idx;	// ec_frame_idx at ISN_TRY
    ಪೂರ್ಣಾಂಕ	    tcd_ರಾಶಿ_len;	// size of ectx.ec_stack at ISN_TRY
    ಪೂರ್ಣಾಂಕ	    tcd_ಒಳ_catch;	// in catch or finally block
    ಪೂರ್ಣಾಂಕ	    tcd_did_throw;	// set did_throw in :endtry
    ಪೂರ್ಣಾಂಕ	    tcd_catch_idx;	// instruction of the first :catch or :finally
    ಪೂರ್ಣಾಂಕ	    tcd_finally_idx;	// instruction of the :finally block or zero
    ಪೂರ್ಣಾಂಕ	    tcd_endtry_idx;	// instruction of the :endtry
    ಪೂರ್ಣಾಂಕ	    tcd_caught;		// catch block entered
    ಪೂರ್ಣಾಂಕ	    tcd_cont;		// :continue encountered, jump here (minus one)
    ಪೂರ್ಣಾಂಕ	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    ಪೂರ್ಣಾಂಕ		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    ಪೂರ್ಣಾಂಕ		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    ಪೂರ್ಣಾಂಕ		or_outer_allocated;  // free "or_outer" later
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
ರಚನೆ ectx_S {
    garray_T	ec_ರಾಶಿ;	// stack of typval_T values
    ಪೂರ್ಣಾಂಕ		ec_ಚೌಕಟ್ಟು_idx;	// index in ec_stack: context of ec_dfunc_idx
    ಪೂರ್ಣಾಂಕ		ec_initial_ಚೌಕಟ್ಟು_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    ಪೂರ್ಣಾಂಕ		ec_dfunc_idx;	// current function index
    ಪೂರ್ಣಾಂಕ		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    ಪೂರ್ಣಾಂಕ		ec_did_emsg_before;
    ಪೂರ್ಣಾಂಕ		ec_trylevel_at_ಆರಂಭ;
    where_T	ec_where;
};

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
// stack of profinfo_T used when profiling.
ಸ್ಥಿರ garray_T profile_info_ga = {0, 0, ಗಾತ್ರ(profinfo_T), 20, ಖಾಲಿ};
#ಆದರೆ_ಅಂತ್ಯ

// Get pointer to item in the stack.
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_TV(idx) (((typval_T *)ectx->ec_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_TV_BOT(idx) (((typval_T *)ectx->ec_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + ectx->ec_ರಾಶಿ.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_TV_VAR(idx) (((typval_T *)ectx->ec_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + ectx->ec_ಚೌಕಟ್ಟು_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + idx)

// Return value for functions used to execute instructions
#ವ್ಯಾಖ್ಯಾನಿಸು EXEC_FAIL	0
#ವ್ಯಾಖ್ಯಾನಿಸು EXEC_OK		1
#ವ್ಯಾಖ್ಯಾನಿಸು EXEC_ಮುಗಿದ	2

    ಶೂನ್ಯ
ಗಮ್ಯ_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_ಗಮ್ಯ_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ufunc_argcount(ufunc_T *ufunc)
{
    ಹಿಂತಿರುಗಿ ufunc->uf_ವಾದಗಳು.ga_len + (ufunc->uf_va_name != ಖಾಲಿ ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exe_concat(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ectx_T *ectx)
{
    ಪೂರ್ಣಾಂಕ		idx;
    size_t	len = 0;
    garray_T	ga;
    ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
    {
	typval_T    *tv;
	size_t	    ಉದ್ದಳತೆ;
    } string_segment_T;
    ಎಣಿಕೆ
    {
	STRING_SEGMENT_CACHE_ಗಾತ್ರ = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_ಗಾತ್ರ];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    ಆದರೆ (ಎಣಿಕೆ_2 > (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, ಎಣಿಕೆ_2);
	ಆದರೆ (string_segment_tab == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }

    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ_2);
	ಆದರೆ (segment->tv->vval.v_string != ಖಾಲಿ)
	{
	    segment->ಉದ್ದಳತೆ = STRLEN(segment->tv->vval.v_string);
	    len += segment->ಉದ್ದಳತೆ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    segment->ಉದ್ದಳತೆ = 0;    // Ensure clean state for the second pass
    }

    ಆದರೆ (ga_grow(&ga, (ಪೂರ್ಣಾಂಕ)len + 1) == FAIL)
    {
	ಆದರೆ (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->ಉದ್ದಳತೆ);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ_2 - 1;
    ರಾಶಿ_TV_BOT(-1)->vval.v_string = ga.ga_ದತ್ತಾಂಶ;

    ಆದರೆ (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exe_newlist(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ectx_T *ectx)
{
    list_T	*list = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		idx;
    typval_T	*tv;

    ಆದರೆ (ಎಣಿಕೆ_2 >= 0)
    {
	list = list_alloc_with_ವಸ್ತುಗಳು(ಎಣಿಕೆ_2);
	ಆದರೆ (list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; ++idx)
	    list_ಗಣ_ಅಂಶ(list, idx, ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ_2));
    }

    ಆದರೆ (ಎಣಿಕೆ_2 > 0)
	ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ_2 - 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
    {
	list_unref(list);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	++ectx->ec_ರಾಶಿ.ga_len;
    tv = ರಾಶಿ_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_ಬೀಗ = 0;
    ಆದರೆ (list != ಖಾಲಿ)
	++list->lv_refcount;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exe_newtuple(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ectx_T *ectx)
{
    tuple_T	*tuple = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		idx;
    typval_T	*tv;

    ಆದರೆ (ಎಣಿಕೆ_2 >= 0)
    {
	tuple = tuple_alloc_with_ವಸ್ತುಗಳು(ಎಣಿಕೆ_2);
	ಆದರೆ (tuple == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; ++idx)
	    tuple_ಗಣ_ಅಂಶ(tuple, idx, ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ_2));
    }

    ಆದರೆ (ಎಣಿಕೆ_2 > 0)
	ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ_2 - 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
    {
	tuple_unref(tuple);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	++ectx->ec_ರಾಶಿ.ga_len;
    tv = ರಾಶಿ_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_ಬೀಗ = 0;
    ಆದರೆ (tuple != ಖಾಲಿ)
	++tuple->tv_refcount;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exe_newdict(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2, ectx_T *ectx)
{
    dict_T	*dict = ಖಾಲಿ;
    dictitem_T	*ಅಂಶ;
    char_u	*ಕೀಲಿ;
    ಪೂರ್ಣಾಂಕ		idx;
    typval_T	*tv;

    ಆದರೆ (ಎಣಿಕೆ_2 >= 0)
    {
	dict = dict_alloc();
	ಆದರೆ (unlikely(dict == ಖಾಲಿ))
	    ಹಿಂತಿರುಗಿ FAIL;
	ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = ರಾಶಿ_TV_BOT(2 * (idx - ಎಣಿಕೆ_2));
	    // check key is unique
	    ಕೀಲಿ = tv->vval.v_string == ಖಾಲಿ ? (char_u *)"" : tv->vval.v_string;
	    ಅಂಶ = dict_ಕಂಡುಹಿಡಿ(dict, ಕೀಲಿ, -1);
	    ಆದರೆ (ಅಂಶ != ಖಾಲಿ)
	    {
		semsg(_(e_duplicate_ಕೀಲಿ_ಒಳ_dictionary_str), ಕೀಲಿ);
		dict_unref(dict);
		ಹಿಂತಿರುಗಿ MAYBE;
	    }
	    ಅಂಶ = dictitem_alloc(ಕೀಲಿ);
	    clear_tv(tv);
	    ಆದರೆ (unlikely(ಅಂಶ == ಖಾಲಿ))
	    {
		dict_unref(dict);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    tv = ರಾಶಿ_TV_BOT(2 * (idx - ಎಣಿಕೆ_2) + 1);
	    ಅಂಶ->di_tv = *tv;
	    ಅಂಶ->di_tv.v_ಬೀಗ = 0;
	    tv->v_type = VAR_UNKNOWN;
	    ಆದರೆ (dict_ಕೂಡಿಸು(dict, ಅಂಶ) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(ಅಂಶ);
		dict_unref(dict);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	}
    }

    ಆದರೆ (ಎಣಿಕೆ_2 > 0)
	ectx->ec_ರಾಶಿ.ga_len -= 2 * ಎಣಿಕೆ_2 - 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
    {
	dict_unref(dict);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	++ectx->ec_ರಾಶಿ.ga_len;
    tv = ರಾಶಿ_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_ಬೀಗ = 0;
    tv->vval.v_dict = dict;
    ಆದರೆ (dict != ಖಾಲಿ)
	++dict->dv_refcount;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    ಶೂನ್ಯ
update_has_breakpoint(ufunc_T *ufunc)
{
    ಆದರೆ (ufunc->uf_debug_tick == debug_tick)
	ಹಿಂತಿರುಗಿ;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

ಸ್ಥಿರ garray_T dict_ರಾಶಿ = GA_ಖಾಲಿಗಳು;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
dict_ರಾಶಿ_save(typval_T *tv)
{
    ಆದರೆ (dict_ರಾಶಿ.ga_growsize == 0)
	ga_init2(&dict_ರಾಶಿ, ಗಾತ್ರ(typval_T), 10);
    ಆದರೆ (ga_grow(&dict_ರಾಶಿ, 1) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ((typval_T *)dict_ರಾಶಿ.ga_ದತ್ತಾಂಶ)[dict_ರಾಶಿ.ga_len] = *tv;
    ++dict_ರಾಶಿ.ga_len;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    ಸ್ಥಿರ typval_T *
dict_ರಾಶಿ_get_tv(ಶೂನ್ಯ)
{
    ಆದರೆ (dict_ರಾಶಿ.ga_len == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ ((typval_T *)dict_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + dict_ರಾಶಿ.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    ಸ್ಥಿರ dict_T *
dict_ರಾಶಿ_get_dict(ಶೂನ್ಯ)
{
    typval_T *tv;

    ಆದರೆ (dict_ರಾಶಿ.ga_len == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    tv = ((typval_T *)dict_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + dict_ರಾಶಿ.ga_len - 1;
    ಆದರೆ (tv->v_type == VAR_DICT)
	ಹಿಂತಿರುಗಿ tv->vval.v_dict;
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Drop an item from the dict stack.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dict_ರಾಶಿ_drop(ಶೂನ್ಯ)
{
    ಆದರೆ (dict_ರಾಶಿ.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	ಹಿಂತಿರುಗಿ;
    }
    --dict_ರಾಶಿ.ga_len;
    clear_tv(((typval_T *)dict_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + dict_ರಾಶಿ.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
dict_ರಾಶಿ_clear(ಪೂರ್ಣಾಂಕ len)
{
    ಆಗ (dict_ರಾಶಿ.ga_len > len)
	dict_ರಾಶಿ_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    ಸ್ಥಿರ outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    ಆದರೆ (ptref == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ &pt->pt_outer;

    // partial using partial (recursively)
    ಆಗ (ptref->pt_outer_partial != ಖಾಲಿ)
	ptref = ptref->pt_outer_partial;
    ಹಿಂತಿರುಗಿ &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_ufunc_arg_types(ufunc_T *ufunc, ಪೂರ್ಣಾಂಕ argcount, ಪೂರ್ಣಾಂಕ off, ectx_T *ectx)
{
    ಆದರೆ (ufunc->uf_arg_types == ಖಾಲಿ && ufunc->uf_va_type == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    typval_T	*argv = ರಾಶಿ_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < argcount; ++i)
    {
	type_T *type = ಖಾಲಿ;

	// assume a v:none argument, using the default value, is always OK
	ಆದರೆ (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
	    ಮುಂದುವರಿ;
	// only pass values to user functions, never types
	ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(&argv[i]) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (i < ufunc->uf_ವಾದಗಳು.ga_len && ufunc->uf_arg_types != ಖಾಲಿ)
	    type = ufunc->uf_arg_types[i];
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ufunc->uf_va_type != ಖಾಲಿ)
	    type = ufunc->uf_va_type->tt_member;
	ಆದರೆ (type != ಖಾಲಿ && check_typval_arg_type(type,
		    &argv[i], ಖಾಲಿ, i + 1) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_dfunc(
	ಪೂರ್ಣಾಂಕ		cdf_idx,
	partial_T	*pt_arg,
	ಪೂರ್ಣಾಂಕ		argcount_arg,
	ectx_T		*ectx)
{
    ಪೂರ್ಣಾಂಕ		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg_cumul + did_emsg;
    ಪೂರ್ಣಾಂಕ		arg_ಗಮ್ಯ_ಕೂಡಿಸು;
    ಪೂರ್ಣಾಂಕ		vararg_ಎಣಿಕೆ = 0;
    ಪೂರ್ಣಾಂಕ		varcount;
    ಪೂರ್ಣಾಂಕ		idx;
    estack_T	*ನಮೂದು;
    funclocal_T	*floc = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		res = OK;
    compiletype_T compile_type;

    ಆದರೆ (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == ಖಾಲಿ ? (char_u *)"unknown" : dfunc->df_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    ಆದರೆ (do_profiling == PROF_YES)
    {
	ಆದರೆ (GA_GROW_OK(&profile_info_ga, 1))
	{
	    profinfo_T *info = ((profinfo_T *)profile_info_ga.ga_ದತ್ತಾಂಶ)
						      + profile_info_ga.ga_len;
	    ++profile_info_ga.ga_len;
	    CLEAR_ಸೂಚಕ(info);
	    profile_may_ಆರಂಭ_func(info, ufunc,
			(((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
					      + ectx->ec_dfunc_idx)->df_ufunc);
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    // When debugging and using "cont" switches to the not-debugged
    // instructions, may need to still compile them.
    compile_type = get_compile_type(ufunc);
    ಆದರೆ (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, ಖಾಲಿ);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + cdf_idx;
    }
    ಆದರೆ (res == FAIL || INSTRUCTIONS(dfunc) == ಖಾಲಿ)
    {
	ಆದರೆ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (ufunc->uf_va_name != ಖಾಲಿ)
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
	vararg_ಎಣಿಕೆ = argcount - ufunc->uf_ವಾದಗಳು.ga_len;
	ಆದರೆ (vararg_ಎಣಿಕೆ < 0)
	    vararg_ಎಣಿಕೆ = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    argcount -= vararg_ಎಣಿಕೆ;
	ಆದರೆ (exe_newlist(vararg_ಎಣಿಕೆ, ectx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	vararg_ಎಣಿಕೆ = 1;
    }

    arg_ಗಮ್ಯ_ಕೂಡಿಸು = ufunc->uf_ವಾದಗಳು.ga_len - argcount;
    ಆದರೆ (arg_ಗಮ್ಯ_ಕೂಡಿಸು < 0)
    {
	semsg(NGETTEXT(e_one_ವಾದ_too_many, e_nr_arguments_too_many,
			-arg_ಗಮ್ಯ_ಕೂಡಿಸು), -arg_ಗಮ್ಯ_ಕೂಡಿಸು);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (arg_ಗಮ್ಯ_ಕೂಡಿಸು > ufunc->uf_def_ವಾದಗಳು.ga_len)
    {
	ಪೂರ್ಣಾಂಕ missing = arg_ಗಮ್ಯ_ಕೂಡಿಸು - ufunc->uf_def_ವಾದಗಳು.ga_len;

	semsg(NGETTEXT(e_one_ವಾದ_too_few, e_nr_arguments_too_few,
			missing), missing);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = ರಾಶಿ_TV_BOT(0) - argcount - vararg_ಎಣಿಕೆ - 1;

    ಆದರೆ (IS_OBJECT_ವಿಧಾನ(ufunc) && !IS_CONSTRUCTOR_ವಿಧಾನ(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == ಖಾಲಿ)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Check the argument types.
    ಆದರೆ (check_ufunc_arg_types(ufunc, argcount, vararg_ಎಣಿಕೆ, ectx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // While check_ufunc_arg_types call, def function compilation process may
    // run.  If so many def functions are compiled, def_functions array may be
    // reallocated and dfunc may no longer have valid pointer.  Get the object
    // pointer from def_functions again here.
    dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + cdf_idx;

    // Reserve space for:
    // - missing arguments
    // - stack frame
    // - local variables
    // - if needed: a counter for number of closures created in
    //   ectx->ec_funcrefs.
    varcount = dfunc->df_varcount + dfunc->df_has_ಆವರಣ;
    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ,
				     arg_ಗಮ್ಯ_ಕೂಡಿಸು + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + varcount))
	ಹಿಂತಿರುಗಿ FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = ರಾಶಿ_TV_BOT(0) - argcount - vararg_ಎಣಿಕೆ - 1;

    // If depth of calling is getting too high, don't execute the function.
    ಆದರೆ (funcdepth_increment() == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    ಆದರೆ (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	ಆದರೆ (floc == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    ಆದರೆ (vararg_ಎಣಿಕೆ > 0 && arg_ಗಮ್ಯ_ಕೂಡಿಸು > 0)
	*ರಾಶಿ_TV_BOT(arg_ಗಮ್ಯ_ಕೂಡಿಸು - 1) = *ರಾಶಿ_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    ಗಾಗಿ (idx = 0; idx < arg_ಗಮ್ಯ_ಕೂಡಿಸು; ++idx)
	ರಾಶಿ_TV_BOT(idx - vararg_ಎಣಿಕೆ)->v_type = VAR_UNKNOWN;
    ectx->ec_ರಾಶಿ.ga_len += arg_ಗಮ್ಯ_ಕೂಡಿಸು;

    // Store current execution state in stack frame for ISN_RETURN.
    ರಾಶಿ_TV_BOT(ರಾಶಿ_ಚೌಕಟ್ಟು_FUNC_OFF)->vval.v_ಸಂಖ್ಯೆ = ectx->ec_dfunc_idx;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_ಚೌಕಟ್ಟು_IIDX_OFF)->vval.v_ಸಂಖ್ಯೆ = ectx->ec_iidx;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_ಚೌಕಟ್ಟು_INSTR_OFF)->vval.v_string = (ಶೂನ್ಯ *)ectx->ec_instr;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_ಚೌಕಟ್ಟು_OUTER_OFF)->vval.v_string =
						    (ಶೂನ್ಯ *)ectx->ec_outer_ref;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_ಚೌಕಟ್ಟು_FUNCLOCAL_OFF)->vval.v_string = (ಶೂನ್ಯ *)floc;
    ರಾಶಿ_TV_BOT(ರಾಶಿ_ಚೌಕಟ್ಟು_IDX_OFF)->vval.v_ಸಂಖ್ಯೆ = ectx->ec_ಚೌಕಟ್ಟು_idx;
    ectx->ec_ಚೌಕಟ್ಟು_idx = ectx->ec_ರಾಶಿ.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    ಪೂರ್ಣಾಂಕ initcount = dfunc->df_varcount + (dfunc->df_has_ಆವರಣ ? 1 : 0);
    ಗಾಗಿ (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = ರಾಶಿ_TV_BOT(ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + idx);

	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->vval.v_ಸಂಖ್ಯೆ = 0;
    }
    ectx->ec_ರಾಶಿ.ga_len += ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    ಆದರೆ (IS_OBJECT_ವಿಧಾನ(ufunc))
    {
	ಆದರೆ (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_ಆಂತರಿಕ_error_str), "type in stack is not an object");
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	*ರಾಶಿ_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != ಖಾಲಿ ? pt_arg : ufunc->uf_partial;
    ಆದರೆ (pt != ಖಾಲಿ || (ufunc->uf_flags & FC_ಆವರಣ))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	ಆದರೆ (ref == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (pt != ಖಾಲಿ)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    ಆದರೆ (unlikely(ref->or_outer == ಖಾಲಿ))
	    {
		vim_free(ref);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->ಹೊರಹೋಗು_ರಾಶಿ = &ectx->ec_ರಾಶಿ;
	    ref->or_outer->ಹೊರಹೋಗು_ಚೌಕಟ್ಟು_idx = ectx->ec_ಚೌಕಟ್ಟು_idx;
	    ಆದರೆ (ectx->ec_outer_ref != ಖಾಲಿ)
		ref->or_outer->ಹೊರಹೋಗು_ಮೇಲೆ = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ectx->ec_outer_ref = ಖಾಲಿ;

    ಆದರೆ (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    ನಮೂದು = estack_ತಳ್ಳು_ufunc(ufunc, 1);
    ಆದರೆ (ನಮೂದು != ಖಾಲಿ)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	ನಮೂದು->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    ಹಿಂತಿರುಗಿ OK;
}

// Double linked list of funcstack_T in use.
ಸ್ಥಿರ funcstack_T *ಮೊದಲ_funcstack = ಖಾಲಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_funcstack_ಗಮ್ಯ_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    ಆದರೆ (ಮೊದಲ_funcstack != ಖಾಲಿ)
	ಮೊದಲ_funcstack->fs_prev = funcstack;
    funcstack->fs_ಮುಂದಿನ = ಮೊದಲ_funcstack;
    funcstack->fs_prev = ಖಾಲಿ;
    ಮೊದಲ_funcstack = funcstack;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_funcstack_from_list(funcstack_T *funcstack)
{
    ಆದರೆ (funcstack->fs_prev == ಖಾಲಿ)
	ಮೊದಲ_funcstack = funcstack->fs_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	funcstack->fs_prev->fs_ಮುಂದಿನ = funcstack->fs_ಮುಂದಿನ;
    ಆದರೆ (funcstack->fs_ಮುಂದಿನ != ಖಾಲಿ)
	funcstack->fs_ಮುಂದಿನ->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
handle_ಆವರಣ_ಒಳ_use(ectx_T *ectx, ಪೂರ್ಣಾಂಕ free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							  + ectx->ec_dfunc_idx;
    ಪೂರ್ಣಾಂಕ		argcount;
    ಪೂರ್ಣಾಂಕ		ತುದಿ;
    ಪೂರ್ಣಾಂಕ		idx;
    typval_T	*tv;
    ಪೂರ್ಣಾಂಕ		ಆವರಣ_ಒಳ_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	ಆವರಣ_ಎಣಿಕೆ;

    ಆದರೆ (dfunc->df_ufunc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;  // function was freed
    ಆದರೆ (dfunc->df_has_ಆವರಣ == 0)
	ಹಿಂತಿರುಗಿ OK;  // no closures
    tv = ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + dfunc->df_varcount);
    ಆವರಣ_ಎಣಿಕೆ = tv->vval.v_ಸಂಖ್ಯೆ;
    ಆದರೆ (ಆವರಣ_ಎಣಿಕೆ == 0)
	ಹಿಂತಿರುಗಿ OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    ತುದಿ = ectx->ec_ಚೌಕಟ್ಟು_idx - argcount;

    // Check if any created closure is still in use.
    ಗಾಗಿ (idx = 0; idx < ಆವರಣ_ಎಣಿಕೆ; ++idx)
    {
	partial_T   *pt;
	ಪೂರ್ಣಾಂಕ	    off = gap->ga_len - ಆವರಣ_ಎಣಿಕೆ + idx;

	ಆದರೆ (off < 0)
	    ಮುಂದುವರಿ;  // count is off or already done
	pt = ((partial_T **)gap->ga_ದತ್ತಾಂಶ)[off];
	ಆದರೆ (pt->pt_refcount > 1)
	{
	    ಪೂರ್ಣಾಂಕ refcount = pt->pt_refcount;
	    ಪೂರ್ಣಾಂಕ i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    ಗಾಗಿ (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx
						       + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + i);
		ಆದರೆ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ಆದರೆ (refcount > 1)
	    {
		ಆವರಣ_ಒಳ_use = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    ಆದರೆ (ಆವರಣ_ಒಳ_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *ರಾಶಿ;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	ಆದರೆ (funcstack == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	funcstack->fs_var_ಸ್ಥಾನಾಂತರ = argcount + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ;
	funcstack->fs_ga.ga_len = funcstack->fs_var_ಸ್ಥಾನಾಂತರ
							  + dfunc->df_varcount;
	ರಾಶಿ = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_ದತ್ತಾಂಶ = ರಾಶಿ;
	ಆದರೆ (ರಾಶಿ == ಖಾಲಿ)
	{
	    vim_free(funcstack);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಕೂಡಿಸು_funcstack_ಗಮ್ಯ_list(funcstack);

	// Move or copy the arguments.
	ಗಾಗಿ (idx = 0; idx < argcount; ++idx)
	{
	    tv = ರಾಶಿ_TV(ತುದಿ + idx);
	    ಆದರೆ (free_arguments)
	    {
		*(ರಾಶಿ + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, ರಾಶಿ + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	ಗಾಗಿ (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    ಆದರೆ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ	    i;

		ಗಾಗಿ (i = 0; i < ಆವರಣ_ಎಣಿಕೆ; ++i)
		    ಆದರೆ (tv->vval.v_partial == ((partial_T **)gap->ga_ದತ್ತಾಂಶ)[
					      gap->ga_len - ಆವರಣ_ಎಣಿಕೆ + i])
			++funcstack->fs_min_refcount;
	    }

	    *(ರಾಶಿ + funcstack->fs_var_ಸ್ಥಾನಾಂತರ + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	ಗಾಗಿ (idx = 0; idx < ಆವರಣ_ಎಣಿಕೆ; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len
							- ಆವರಣ_ಎಣಿಕೆ + idx];
	    ಆದರೆ (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.ಹೊರಹೋಗು_ರಾಶಿ = &funcstack->fs_ga;
		pt->pt_outer.ಹೊರಹೋಗು_ಚೌಕಟ್ಟು_idx = ectx->ec_ಚೌಕಟ್ಟು_idx - ತುದಿ;
	    }
	}
    }

    ಗಾಗಿ (idx = 0; idx < ಆವರಣ_ಎಣಿಕೆ; ++idx)
	partial_unref(((partial_T **)gap->ga_ದತ್ತಾಂಶ)[gap->ga_len
						       - ಆವರಣ_ಎಣಿಕೆ + idx]);
    gap->ga_len -= ಆವರಣ_ಎಣಿಕೆ;
    ಆದರೆ (gap->ga_len == 0)
	ga_clear(gap);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    ಪೂರ್ಣಾಂಕ
funcstack_check_refcount(funcstack_T *funcstack)
{
    ಪೂರ್ಣಾಂಕ		i;
    garray_T	*gap = &funcstack->fs_ga;
    ಪೂರ್ಣಾಂಕ		ಮುಗಿದ = 0;
    typval_T	*ರಾಶಿ;

    ಆದರೆ (funcstack->fs_refcount > funcstack->fs_min_refcount)
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (i = funcstack->fs_var_ಸ್ಥಾನಾಂತರ; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_ದತ್ತಾಂಶ) + i;

	ಆದರೆ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ಖಾಲಿ
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++ಮುಗಿದ;
    }
    ಆದರೆ (ಮುಗಿದ != funcstack->fs_min_refcount)
	ಹಿಂತಿರುಗಿ FALSE;

    ರಾಶಿ = gap->ga_ದತ್ತಾಂಶ;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	clear_tv(ರಾಶಿ + i);
    vim_free(ರಾಶಿ);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_funcstacks(ಪೂರ್ಣಾಂಕ copyID)
{
    funcstack_T *funcstack;

    ಗಾಗಿ (funcstack = ಮೊದಲ_funcstack; funcstack != ಖಾಲಿ;
						funcstack = funcstack->fs_ಮುಂದಿನ)
    {
	typval_T    *ರಾಶಿ = funcstack->fs_ga.ga_ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ	    i;

	ಗಾಗಿ (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    ಆದರೆ (ಗಣ_ref_ಒಳ_ಅಂಶ(ರಾಶಿ + i, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ))
		ಹಿಂತಿರುಗಿ TRUE;  // abort
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
ಸ್ಥಿರ ectx_T *current_ectx = ಖಾಲಿ;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    ಪೂರ್ಣಾಂಕ
ಒಳ_def_function(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ current_ectx != ಖಾಲಿ;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    ಪೂರ್ಣಾಂಕ
fill_exec_lval_ಬೇರು(lval_ಬೇರು_T *ಬೇರು)
{
    ectx_T *ectx = current_ectx;
    ಆದರೆ (ectx != ಖಾಲಿ)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	ಆದರೆ (ufunc->uf_class != ಖಾಲಿ)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    ಆದರೆ (tv != ಖಾಲಿ)
	    {
		CLEAR_ಸೂಚಕ(ಬೇರು);
		ಬೇರು->lr_tv = tv;
		copy_tv(ರಾಶಿ_TV_VAR(0), ಬೇರು->lr_tv);
		ಬೇರು->lr_cl_exec = ufunc->uf_class;
		++ಬೇರು->lr_cl_exec->class_refcount;
		ಹಿಂತಿರುಗಿ OK;
	    }
	}
    }

    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(ಶೂನ್ಯ)
{
    ectx_T *r = current_ectx;

    current_ectx = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ r;
}

    ಶೂನ್ಯ
restore_current_ectx(ectx_T *ectx)
{
    ಆದರೆ (current_ectx != ಖಾಲಿ)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    ಸ್ಥಿರ list_T *
ಕೂಡಿಸು_defer_ಅಂಶ(ಪೂರ್ಣಾಂಕ var_idx, ಪೂರ್ಣಾಂಕ argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = ರಾಶಿ_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    ಆದರೆ (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	ಆದರೆ (rettv_list_alloc(defer_tv) == FAIL)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_ವಸ್ತುಗಳು(argcount + 1);
    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_ಬೀಗ = 0;
    ಆದರೆ (list_ಸೇರಿಸು_tv(defer_l, &listval, defer_l->lv_ಮೊದಲ) == FAIL)
    {
	list_free(l);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
defer_command(ಪೂರ್ಣಾಂಕ var_idx, ಪೂರ್ಣಾಂಕ argcount, ectx_T *ectx)
{
    list_T	*l = ಕೂಡಿಸು_defer_ಅಂಶ(var_idx, argcount, ectx);
    ಪೂರ್ಣಾಂಕ		i;
    typval_T	*func_tv;

    ಆದರೆ (l == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    func_tv = ರಾಶಿ_TV_BOT(-argcount - 1);
    ಆದರೆ (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    list_ಗಣ_ಅಂಶ(l, 0, func_tv);

    ಗಾಗಿ (i = 0; i < argcount; ++i)
	list_ಗಣ_ಅಂಶ(l, i + 1, ರಾಶಿ_TV_BOT(-argcount + i));
    ectx->ec_ರಾಶಿ.ga_len -= argcount + 1;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    ಪೂರ್ಣಾಂಕ
ಕೂಡಿಸು_defer_function(char_u *name, ಪೂರ್ಣಾಂಕ argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    l = ಕೂಡಿಸು_defer_ಅಂಶ(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    ಆದರೆ (l == ಖಾಲಿ)
    {
	vim_free(name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_ಬೀಗ = 0;
    func_tv.vval.v_string = name;
    list_ಗಣ_ಅಂಶ(l, 0, &func_tv);

    ಗಾಗಿ (i = 0; i < argcount; ++i)
	list_ಗಣ_ಅಂಶ(l, i + 1, argvars + i);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = ರಾಶಿ_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    ಆದರೆ (defer_tv->v_type != VAR_LIST)
	ಹಿಂತಿರುಗಿ;	 // no function added
    FOR_ALL_LIST_ವಸ್ತುಗಳು(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ವಾದಗಳು];
	ಪೂರ್ಣಾಂಕ	    i;
	listitem_T  *arg_li = l->lv_ಮೊದಲ;
	typval_T    *functv = &l->lv_ಮೊದಲ->li_tv;
	ಪೂರ್ಣಾಂಕ	    argcount = l->lv_len - 1;

	ಆದರೆ (functv->vval.v_string == ಖಾಲಿ)
	    // already being called, can happen if function does ":qa"
	    ಮುಂದುವರಿ;

	ಗಾಗಿ (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_ಮುಂದಿನ;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = ಖಾಲಿ;
	partial_T	*pt = ಖಾಲಿ;

	CLEAR_ಕ್ಷೇತ್ರ(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	ಆದರೆ (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = ಖಾಲಿ;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    ಆದರೆ (funcexe.fe_object != ಖಾಲಿ)
		++funcexe.fe_object->obj_refcount;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = ಖಾಲಿ;
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
	ಆದರೆ (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
func_return(ectx_T *ectx)
{
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							  + ectx->ec_dfunc_idx;
    ಪೂರ್ಣಾಂಕ		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*ನಮೂದು;
    ಪೂರ್ಣಾಂಕ		prev_dfunc_idx = ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx
					+ ರಾಶಿ_ಚೌಕಟ್ಟು_FUNC_OFF)->vval.v_ಸಂಖ್ಯೆ;
    funclocal_T	*floc;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							      + prev_dfunc_idx;

    ಆದರೆ (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	ಆದರೆ (dfunc->df_ufunc->uf_profiling
				   || (caller != ಖಾಲಿ && caller->uf_profiling))
	{
	    profile_may_ಕೊನೆ_func(((profinfo_T *)profile_info_ga.ga_ದತ್ತಾಂಶ)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    ಆದರೆ (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    ನಮೂದು = estack_ತೆಗೆ();
    ಆದರೆ (ನಮೂದು != ಖಾಲಿ)
	current_sctx = ನಮೂದು->es_save_sctx;

    ಆದರೆ (handle_ಆವರಣ_ಒಳ_use(ectx, TRUE) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    ಪೂರ್ಣಾಂಕ ತುದಿ = ectx->ec_ಚೌಕಟ್ಟು_idx - argcount;
    ಆದರೆ (IS_OBJECT_ವಿಧಾನ(dfunc->df_ufunc))
	--ತುದಿ;
    ಗಾಗಿ (idx = ತುದಿ; idx < ectx->ec_ಚೌಕಟ್ಟು_idx; ++idx)
	clear_tv(ರಾಶಿ_TV(idx));

    // Clear local variables and temp values, but not the return value.
    ಗಾಗಿ (idx = ectx->ec_ಚೌಕಟ್ಟು_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ;
					idx < ectx->ec_ರಾಶಿ.ga_len - 1; ++idx)
	clear_tv(ರಾಶಿ_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_ರಾಶಿ.ga_len - 1;
    ಆದರೆ (ret_idx == ectx->ec_ಚೌಕಟ್ಟು_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_IDX_OFF)
	ret_idx = 0;

    ಆದರೆ (ectx->ec_outer_ref != ಖಾಲಿ)
    {
	ಆದರೆ (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx
					+ ರಾಶಿ_ಚೌಕಟ್ಟು_IIDX_OFF)->vval.v_ಸಂಖ್ಯೆ;
    ectx->ec_instr = (ಶೂನ್ಯ *)ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx
				       + ರಾಶಿ_ಚೌಕಟ್ಟು_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (ಶೂನ್ಯ *)ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx
				       + ರಾಶಿ_ಚೌಕಟ್ಟು_OUTER_OFF)->vval.v_string;
    floc = (ಶೂನ್ಯ *)ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx
				   + ರಾಶಿ_ಚೌಕಟ್ಟು_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_ಚೌಕಟ್ಟು_idx = ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx
				       + ರಾಶಿ_ಚೌಕಟ್ಟು_IDX_OFF)->vval.v_ಸಂಖ್ಯೆ;

    ಆದರೆ (floc == ಖಾಲಿ)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    ಆದರೆ (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_ರಾಶಿ.ga_len = ತುದಿ + 1;
	*ರಾಶಿ_TV_BOT(-1) = *ರಾಶಿ_TV(ret_idx);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// Reset the stack to the position before the call.
	ectx->ec_ರಾಶಿ.ga_len = ತುದಿ;

    funcdepth_decrement();
    --ex_nesting_level;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_prepare(ಪೂರ್ಣಾಂಕ argcount, typval_T *argvars, ectx_T *ectx)
{
    ಪೂರ್ಣಾಂಕ		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    ಗಾಗಿ (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *ರಾಶಿ_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    ಆದರೆ (argcount > 0)
	ectx->ec_ರಾಶಿ.ga_len -= argcount - 1;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
	ಹಿಂತಿರುಗಿ FAIL;
    ಇಲ್ಲದಿದ್ದರೆ
	++ectx->ec_ರಾಶಿ.ga_len;

    // Default return value is zero.
    tv = ರಾಶಿ_TV_BOT(-1);
    tv->v_type = VAR_ಸಂಖ್ಯೆ;
    tv->vval.v_ಸಂಖ್ಯೆ = 0;
    tv->v_ಬೀಗ = 0;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Call a builtin function by index.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_bfunc(ಪೂರ್ಣಾಂಕ func_idx, ಪೂರ್ಣಾಂಕ argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ವಾದಗಳು];
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    ಅಕ್ಷರ	*save_func_name = ectx->ec_where.wt_func_name;

    ಆದರೆ (call_prepare(argcount, argvars, ectx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    // Check for void value being passed as an argument.
    ಗಾಗಿ (idx = 0; idx < argcount; ++idx)
	ಆದರೆ (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
	    ಗಾಗಿ (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

    ectx->ec_where.wt_func_name = ಆಂತರಿಕ_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_ಆಂತರಿಕ_func_by_idx(func_idx, argvars, ರಾಶಿ_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    ಗಾಗಿ (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ಆದರೆ (did_emsg > did_emsg_before)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	ಪೂರ್ಣಾಂಕ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ವಾದಗಳು];
    funcexe_T   funcexe;
    funcerror_T	error;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    ಆದರೆ (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, ಖಾಲಿ)
								       == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	ಆದರೆ (error != FCERR_UNKNOWN)
	{
	    ಆದರೆ (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    ಇಲ್ಲದಿದ್ದರೆ
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	ಆದರೆ (iptr != ಖಾಲಿ)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	ಹಿಂತಿರುಗಿ call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    ಆದರೆ (call_prepare(argcount, argvars, ectx) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    CLEAR_ಕ್ಷೇತ್ರ(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != ಖಾಲಿ ? selfdict : dict_ರಾಶಿ_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      ರಾಶಿ_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    ಗಾಗಿ (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ಆದರೆ (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_ಸಿಕ್ಕಿತು_var);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * If command modifiers were applied restore them.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_restore_cmdmod(funclocal_T *funclocal)
{
    ಆದರೆ (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ಖಾಲಿ;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
vim9_aborting(ಪೂರ್ಣಾಂಕ prev_uncaught_emsg)
{
    ಹಿಂತಿರುಗಿ uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_by_name(
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    ಆದರೆ (builtin_function(name, -1))
    {
	ಪೂರ್ಣಾಂಕ func_idx = ಕಂಡುಹಿಡಿ_ಆಂತರಿಕ_func(name);

	ಆದರೆ (func_idx < 0)  // Impossible?
	    ಹಿಂತಿರುಗಿ FAIL;
	ಆದರೆ (check_ಆಂತರಿಕ_func(func_idx, argcount) < 0)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಹಿಂತಿರುಗಿ call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*ಆರಂಭ_bracket = generic_func_ಕಂಡುಹಿಡಿ_open_bracket(name);
    ಆದರೆ (ಆರಂಭ_bracket != ಖಾಲಿ)
    {
	cc = *ಆರಂಭ_bracket;
	*ಆರಂಭ_bracket = NUL;
    }

    ufunc = ಕಂಡುಹಿಡಿ_func(name, FALSE);

    ಆದರೆ (ufunc == ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ prev_uncaught_emsg = uncaught_emsg;

	ಆದರೆ (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = ಕಂಡುಹಿಡಿ_func(name, FALSE);

	ಆದರೆ (vim9_aborting(prev_uncaught_emsg))
	{
	    ಆದರೆ (ಆರಂಭ_bracket != ಖಾಲಿ)
		*ಆರಂಭ_bracket = cc;
	    ಹಿಂತಿರುಗಿ FAIL;  // bail out if loading the script caused an error
	}
    }

    ಆದರೆ (ಆರಂಭ_bracket != ಖಾಲಿ)
	*ಆರಂಭ_bracket = cc;

    ಆದರೆ (ufunc != ಖಾಲಿ)
    {
	ಆದರೆ (IS_GENERIC_FUNC(ufunc))
	{
	    ಆದರೆ (ಆರಂಭ_bracket != ಖಾಲಿ)
		ufunc = ಕಂಡುಹಿಡಿ_generic_func(ufunc, name, &ಆರಂಭ_bracket);
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		emsg_funcname(e_generic_func_missing_type_ವಾದಗಳು_str, name);
		ufunc = ಖಾಲಿ;
	    }
	    ಆದರೆ (ufunc == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಆರಂಭ_bracket != ಖಾಲಿ)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}

	ಆದರೆ (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಹಿಂತಿರುಗಿ call_ufunc(ufunc, ಖಾಲಿ, argcount, ectx, iptr, selfdict);
    }

    ಹಿಂತಿರುಗಿ FAIL;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_partial(
	typval_T    *tv,
	ಪೂರ್ಣಾಂಕ	    argcount_arg,
	ectx_T	    *ectx)
{
    ಪೂರ್ಣಾಂಕ		argcount = argcount_arg;
    char_u	*name = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಂಕ		res = FAIL;
    dict_T	*selfdict = ಖಾಲಿ;

    ಆದರೆ (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	ಪೂರ್ಣಾಂಕ	    i;

	ಆದರೆ (pt->pt_obj != ಖಾಲಿ)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		ಹಿಂತಿರುಗಿ FAIL;
	    ಗಾಗಿ (i = 1; i <= argcount; ++i)
		*ರಾಶಿ_TV_BOT(-i + 1) = *ರಾಶಿ_TV_BOT(-i);

	    typval_T *obj_tv = ರಾಶಿ_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_ಬೀಗ = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}

	ಆದರೆ (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, pt->pt_argc))
		ಹಿಂತಿರುಗಿ FAIL;
	    ಗಾಗಿ (i = 1; i <= argcount; ++i)
		*ರಾಶಿ_TV_BOT(-i + pt->pt_argc) = *ರಾಶಿ_TV_BOT(-i);
	    ectx->ec_ರಾಶಿ.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    ಗಾಗಿ (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], ರಾಶಿ_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	ಆದರೆ (pt->pt_func != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ call_ufunc(pt->pt_func, pt, argcount, ectx, ಖಾಲಿ, selfdict);

	name = pt->pt_name;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    ಆದರೆ (name != ಖಾಲಿ)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = ಖಾಲಿ;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ಆದರೆ (error != FCERR_NONE)
	    res = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	    res = call_by_name(fname, argcount, ectx, ಖಾಲಿ, selfdict);
	vim_free(tofree);
    }

    ಆದರೆ (res == FAIL)
    {
	ಆದರೆ (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == ಖಾಲಿ ? (char_u *)"[unknown]" : name);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
error_if_locked(ಪೂರ್ಣಾಂಕ ಬೀಗ, ಅಕ್ಷರ *error)
{
    ಆದರೆ (ಬೀಗ & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
check_for_ಸಂಖ್ಯೆ(typval_T *tv)
{
    ಆದರೆ (tv->v_type != VAR_ಸಂಖ್ಯೆ)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_ಸಂಖ್ಯೆ), vartype_name(tv->v_type));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
store_var(char_u *name, typval_T *tv)
{
    funccal_ನಮೂದು_T ನಮೂದು;
    ಪೂರ್ಣಾಂಕ		    flags = ನಿಯೋಜಿಸು_DECL;

    ಆದರೆ (tv->v_ಬೀಗ)
	flags |= ನಿಯೋಜಿಸು_CONST;
    save_funccal(&ನಮೂದು);
    ಗಣ_var_const(name, 0, ಖಾಲಿ, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_2string(typval_T *tv, ಪೂರ್ಣಾಂಕ is_2string_any, ಪೂರ್ಣಾಂಕ tostring_flags)
{
    ಆದರೆ (tv->v_type == VAR_STRING)
	ಹಿಂತಿರುಗಿ OK;

    char_u *str;

    ಆದರೆ (is_2string_any)
    {
	ಆಯ್ಕೆ (tv->v_type)
	{
	    ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಸ್ಥಿತಿ VAR_BOOL:
	    ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    ಸ್ಥಿತಿ VAR_FLOAT:
	    ಸ್ಥಿತಿ VAR_DICT:
	    ಸ್ಥಿತಿ VAR_BLOB:	ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ VAR_LIST:
				ಆದರೆ (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, ಗಾತ್ರ(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    ಆದರೆ (str == ಖಾಲಿ)
					ಹಿಂತಿರುಗಿ FAIL;
				    s = str;
				    ಆಗ ((e = vim_strchr(s, '\n')) != ಖಾಲಿ)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					ಆದರೆ (p != ಖಾಲಿ)
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
				    tv->vval.v_string = ga.ga_ದತ್ತಾಂಶ;
				    ಹಿಂತಿರುಗಿ OK;
				}
				ಆದರೆ (tostring_flags & TOSTRING_INTERPOLATE)
				    ನಿಲ್ಲಿಸು;
				// FALLTHROUGH
	    ಪೂರ್ವನಿಯೋಜಿತ:	ಗಮ್ಯ_string_error(tv->v_type);
			ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_LIST:
	    ಆದರೆ (tv->vval.v_list == ಖಾಲಿ && sv->sv_type != &t_list_ಖಾಲಿಗಳು)
		(ಶೂನ್ಯ)rettv_list_alloc(tv);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಆದರೆ (tv->vval.v_tuple == ಖಾಲಿ && sv->sv_type != &t_tuple_ಖಾಲಿಗಳು)
		(ಶೂನ್ಯ)rettv_tuple_alloc(tv);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_DICT:
	    ಆದರೆ (tv->vval.v_dict == ಖಾಲಿ && sv->sv_type != &t_dict_ಖಾಲಿಗಳು)
		(ಶೂನ್ಯ)rettv_dict_alloc(tv);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ VAR_BLOB:
	    ಆದರೆ (tv->vval.v_blob == ಖಾಲಿ && sv->sv_type != &t_blob_null)
		(ಶೂನ್ಯ)rettv_blob_alloc(tv);
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * Return the character "str[index]" where "index" is the character index,
 * including composing characters.
 * If "index" is out of range NULL is returned.
 */
    char_u *
char_from_string(char_u *str, varnumber_T ಸೂಚ್ಯಂಕ)
{
    size_t	    nbyte = 0;
    varnumber_T	    nchar = ಸೂಚ್ಯಂಕ;
    size_t	    slen;

    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    ಆದರೆ (ಸೂಚ್ಯಂಕ < 0)
    {
	ಪೂರ್ಣಾಂಕ	clen = 0;

	ಗಾಗಿ (nbyte = 0; nbyte < slen; ++clen)
	{
	    ಆದರೆ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    ಇಲ್ಲದಿದ್ದರೆ
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + ಸೂಚ್ಯಂಕ;
	ಆದರೆ (nchar < 0)
	    // unlike list: index out of range results in empty string
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಗಾಗಿ (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	ಆದರೆ (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	ಇಲ್ಲದಿದ್ದರೆ
	    nbyte += mb_ptr2len(str + nbyte);
    }
    ಆದರೆ (nbyte >= slen)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    ಸ್ಥಿರ ದೀರ್ಘ
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    ಆದರೆ (nchar >= 0)
    {
	ಆಗ (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	nbyte = str_len;
	ಆಗ (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_ತಲೆ_off(str, str + nbyte);
	    ++nchar;
	}
	ಆದರೆ (nchar < 0)
	    ಹಿಂತಿರುಗಿ -1;
    }
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_ತುಂಡು(char_u *str, varnumber_T ಮೊದಲ, varnumber_T ಕೊನೆಯ, ಪೂರ್ಣಾಂಕ exclusive)
{
    ದೀರ್ಘ	ಆರಂಭ_byte, ಕೊನೆ_byte;
    size_t	slen;

    ಆದರೆ (str == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    slen = STRLEN(str);
    ಆರಂಭ_byte = char_idx2byte(str, slen, ಮೊದಲ);
    ಆದರೆ (ಆರಂಭ_byte < 0)
	ಆರಂಭ_byte = 0; // first index very negative: use zero
    ಆದರೆ ((ಕೊನೆಯ == -1 && !exclusive) || ಕೊನೆಯ == VARNUM_MAX)
	ಕೊನೆ_byte = (ದೀರ್ಘ)slen;
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಕೊನೆ_byte = char_idx2byte(str, slen, ಕೊನೆಯ);
	ಆದರೆ (!exclusive && ಕೊನೆ_byte >= 0 && ಕೊನೆ_byte < (ದೀರ್ಘ)slen)
	    // end index is inclusive
	    ಕೊನೆ_byte += mb_ptr2len(str + ಕೊನೆ_byte);
    }

    ಆದರೆ (ಆರಂಭ_byte >= (ದೀರ್ಘ)slen || ಕೊನೆ_byte <= ಆರಂಭ_byte)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ vim_strnsave(str + ಆರಂಭ_byte, ಕೊನೆ_byte - ಆರಂಭ_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    ಸ್ಥಿರ svar_T *
get_script_svar(scriptref_T *sref, ಪೂರ್ಣಾಂಕ dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ಅಂಶ(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? ಖಾಲಿ
			      : ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + dfunc_idx;
    svar_T	    *sv;

    ಆದರೆ (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	ಆದರೆ (dfunc != ಖಾಲಿ)
	    semsg(_(e_script_variable_invalid_after_reload_ಒಳ_function_str),
					 printable_func_name(dfunc->df_ufunc));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_ದತ್ತಾಂಶ) + sref->sref_idx;
    ಆದರೆ (sv->sv_name == ಖಾಲಿ)
    {
	ಆದರೆ (dfunc != ಖಾಲಿ)
	    emsg(_(e_script_variable_was_deleted));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	ಆದರೆ (dfunc != ಖಾಲಿ)
	    emsg(_(e_script_variable_type_ಬದಲಾಯಿತು));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	ಆದರೆ (dfunc != ಖಾಲಿ)
	    semsg(_(e_ಅಂಶ_not_exported_ಒಳ_script_str), sv->sv_name);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಹಿಂತಿರುಗಿ sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    ಸ್ಥಿರ char_u *
get_ವಿಭಜಿಸು_sourceline(
	ಪೂರ್ಣಾಂಕ c ಬಳಸದ,
	ಶೂನ್ಯ *cookie,
	ಪೂರ್ಣಾಂಕ indent ಬಳಸದ,
	getline_opt_T options ಬಳಸದ)
{
    ಮೂಲ_cookie_T	*sp = (ಮೂಲ_cookie_T *)cookie;
    char_u		*p;
    char_u		*ಸಾಲು;

    p = vim_strchr(sp->nextline, '\n');
    ಆದರೆ (p == ಖಾಲಿ)
    {
	ಸಾಲು = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸಾಲು = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    ಹಿಂತಿರುಗಿ ಸಾಲು;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
call_eval_func(
	char_u	    *name,
	ಪೂರ್ಣಾಂಕ	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    ಪೂರ್ಣಾಂಕ	    called_emsg_before = called_emsg;
    ಪೂರ್ಣಾಂಕ	    res;

    res = call_by_name(name, argcount, ectx, iptr, ಖಾಲಿ);
    ಆದರೆ (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = ಕಂಡುಹಿಡಿ_var(name, ಖಾಲಿ, FALSE);
	ಆದರೆ (v == ಖಾಲಿ || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಹಿಂತಿರುಗಿ call_partial(&v->di_tv, argcount, ectx);
    }
    ಹಿಂತಿರುಗಿ res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    ಪೂರ್ಣಾಂಕ
fill_partial_and_ಆವರಣ(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ಆದರೆ (ufunc->uf_flags & FC_ಆವರಣ)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.ಹೊರಹೋಗು_ರಾಶಿ = &ectx->ec_ರಾಶಿ;
	pt->pt_outer.ಹೊರಹೋಗು_ಚೌಕಟ್ಟು_idx = ectx->ec_ಚೌಕಟ್ಟು_idx;
	ಆದರೆ (ectx->ec_outer_ref != ಖಾಲಿ)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.ಹೊರಹೋಗು_ಮೇಲೆ = ectx->ec_outer_ref->or_outer;
	    ಆದರೆ (ectx->ec_outer_ref->or_partial != ಖಾಲಿ)
	    {
		pt->pt_outer.ಹೊರಹೋಗು_ಮೇಲೆ_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.ಹೊರಹೋಗು_ಮೇಲೆ_partial->pt_refcount;
	    }
	}

	ಆದರೆ (lvi != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ	ಆಳ;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    ಗಾಗಿ (ಆಳ = 0; ಆಳ < lvi->lvi_ಆಳ; ++ಆಳ)
	    {
		pt->pt_outer.ಹೊರಹೋಗು_loop[ಆಳ].ರಾಶಿ = &ectx->ec_ರಾಶಿ;
		pt->pt_outer.ಹೊರಹೋಗು_loop[ಆಳ].var_idx = ectx->ec_ಚೌಕಟ್ಟು_idx
			 + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + lvi->lvi_loop[ಆಳ].var_idx;
		pt->pt_outer.ಹೊರಹೋಗು_loop[ಆಳ].var_ಎಣಿಕೆ =
					    lvi->lvi_loop[ಆಳ].var_ಎಣಿಕೆ;
	    }
	    pt->pt_outer.ಹೊರಹೋಗು_loop_ಗಾತ್ರ = lvi->lvi_ಆಳ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    pt->pt_outer.ಹೊರಹೋಗು_loop_ಗಾತ್ರ = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	ಆದರೆ (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    ಹಿಂತಿರುಗಿ FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + ectx->ec_ಚೌಕಟ್ಟು_idx
		       + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + dfunc->df_varcount)->vval.v_ಸಂಖ್ಯೆ;

	((partial_T **)ectx->ec_funcrefs.ga_ದತ್ತಾಂಶ)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exec_command(isn_T *iptr, char_u *cmd_string)
{
    ಮೂಲ_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_ಕ್ಷೇತ್ರ(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    ಆದರೆ (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    ಪೂರ್ಣಾಂಕ
may_ತುಂಬು_script(ಪೂರ್ಣಾಂಕ sid, ಪೂರ್ಣಾಂಕ *loaded)
{
    scriptitem_T *si = SCRIPT_ಅಂಶ(sid);

    ಆದರೆ (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	ಆದರೆ (do_ಮೂಲ(si->sn_name, FALSE, DOSO_NONE, ಖಾಲಿ) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }
    ಹಿಂತಿರುಗಿ OK;
}

// used for v_instr of typval of VAR_INSTR
ರಚನೆ instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ subs_ಅಭಿವ್ಯಕ್ತಿ_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    ಪೂರ್ಣಾಂಕ		subs_ಸ್ಥಿತಿ;
} subs_ಅಭಿವ್ಯಕ್ತಿ_T;

// Set when calling do_debug().
ಸ್ಥಿರ ectx_T	*debug_context = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	debug_var_ಎಣಿಕೆ;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    ಪೂರ್ಣಾಂಕ		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    ಪೂರ್ಣಾಂಕ		    varargs_off;

    ಆದರೆ (ectx == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    ಗಾಗಿ (idx = debug_var_ಎಣಿಕೆ - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_ಹೆಸರುಗಳು.ga_ದತ್ತಾಂಶ)[idx];

	// the variable name may be NULL when not available in this block
	ಆದರೆ (varname != ಖಾಲಿ && STRCMP(varname, name) == 0)
	    ಹಿಂತಿರುಗಿ ರಾಶಿ_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == ಖಾಲಿ ? 0 : 1;
    ಗಾಗಿ (idx = 0; idx < ufunc->uf_ವಾದಗಳು.ga_len; ++idx)
	ಆದರೆ (STRCMP(((char_u **)(ufunc->uf_ವಾದಗಳು.ga_ದತ್ತಾಂಶ))[idx], name) == 0)
	    ಹಿಂತಿರುಗಿ ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx - ufunc->uf_ವಾದಗಳು.ga_len
							  - varargs_off + idx);
    ಆದರೆ (ufunc->uf_va_name != ಖಾಲಿ && STRCMP(ufunc->uf_va_name, name) == 0)
	ಹಿಂತಿರುಗಿ ರಾಶಿ_TV(ectx->ec_ಚೌಕಟ್ಟು_idx - 1);

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    ಪೂರ್ಣಾಂಕ
may_break_ಒಳ_function(ufunc_T *ufunc)
{
    ಹಿಂತಿರುಗಿ ufunc->uf_has_breakpoint || debug_has_ಅಭಿವ್ಯಕ್ತಿ_breakpoint();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*ಸಾಲು;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    ಪೂರ್ಣಾಂಕ		ಕೊನೆ_lnum = iptr->isn_lnum;
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		lnum;

    ಆದರೆ (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	ಆದರೆ (!may_break_ಒಳ_function(ufunc))
	    ಹಿಂತಿರುಗಿ;

	// check for the next breakpoint if needed
	breakpoint = dbg_ಕಂಡುಹಿಡಿ_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	ಆದರೆ (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    ಹಿಂತಿರುಗಿ;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_ಎಣಿಕೆ = iptr->isn_arg.debug.dbg_var_ಹೆಸರುಗಳು_len;

    ಗಾಗಿ (ni = iptr + 1; ni->isn_type != ISN_ಮುಕ್ತಾಯ; ++ni)
	ಆದರೆ (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    ಕೊನೆ_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    ನಿಲ್ಲಿಸು;
	}

    ಆದರೆ (ಕೊನೆ_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, ಗಾತ್ರ(char_u *), 10);
	ಗಾಗಿ (lnum = iptr->isn_lnum; lnum < ಕೊನೆ_lnum
				     && lnum <= ufunc->uf_ಸಾಲುಗಳು.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_ಸಾಲುಗಳು.ga_ದತ್ತಾಂಶ)[lnum - 1];

	    ಆದರೆ (p == ಖಾಲಿ)
		ಮುಂದುವರಿ;  // left over from continuation line
	    p = skipwhite(p);
	    ಆದರೆ (*p == '#')
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_ದತ್ತಾಂಶ))[ga.ga_len++] = p;
	    ಆದರೆ (STRNCMP(p, "def ", 4) == 0)
		ನಿಲ್ಲಿಸು;
	}
	ಸಾಲು = ga_concat_ಅಕ್ಷರಸರಣಿಗಳು(&ga, "  ");
	vim_free(ga.ga_ದತ್ತಾಂಶ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಸಾಲು = ((char_u **)ufunc->uf_ಸಾಲುಗಳು.ga_ದತ್ತಾಂಶ)[iptr->isn_lnum - 1];

    do_debug(ಸಾಲು == ಖಾಲಿ ? (char_u *)"[empty]" : ಸಾಲು);
    debug_context = ಖಾಲಿ;

    ಆದರೆ (ಕೊನೆ_lnum > iptr->isn_lnum)
	vim_free(ಸಾಲು);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    ಆದರೆ (lhs_list->lv_type == ಖಾಲಿ || lhs_list->lv_type->tt_member == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    ಹಿಂತಿರುಗಿ check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = ರಾಶಿ_TV_BOT(-2);
    ದೀರ್ಘ	lidx = 0;
    typval_T	*tv_dest = ರಾಶಿ_TV_BOT(-1);
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;
    ಪೂರ್ಣಾಂಕ		check_rhs_type = FALSE;

    ಆದರೆ (tv_idx->v_type == VAR_ಸಂಖ್ಯೆ)
	lidx = (ದೀರ್ಘ)tv_idx->vval.v_ಸಂಖ್ಯೆ;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = ರಾಶಿ_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    ಆದರೆ (dest_type == VAR_OBJECT && tv_dest->vval.v_object == ಖಾಲಿ)
    {
	emsg(_(e_using_null_object));
	ಸ್ಥಿತಿ_2 = FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	ಆದರೆ (dest_type == VAR_DICT)
	    ಸ್ಥಿತಿ_2 = do_2string(tv_idx, TRUE, FALSE);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    ಆದರೆ (obj == ಖಾಲಿ)
	    {
		emsg(_(e_using_null_object));
		ಸ್ಥಿತಿ_2 = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		ಪೂರ್ಣಾಂಕ		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		ಆದರೆ (m != ಖಾಲಿ)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    ಆದರೆ ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == ಖಾಲಿ ||
			     !class_instance_ದ(ufunc->uf_class, cl)))
		    {
			ಅಕ್ಷರ *ಸಂದೇಶ = (m->ocm_access == VIM_ACCESS_ಖಾಸಗಿ)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(ಸಂದೇಶ, m->ocm_name.string, 0, cl);
			ಸ್ಥಿತಿ_2 = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			ಸ್ಥಿತಿ_2 = FAIL;
		    ಇಲ್ಲದಿದ್ದರೆ
			lidx = m_idx;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    member_not_ಸಿಕ್ಕಿತು_ಸಂದೇಶ(cl, VAR_OBJECT, member, 0);
		    ಸ್ಥಿತಿ_2 = FAIL;
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    ಸ್ಥಿತಿ_2 = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest_type == VAR_LIST && tv_idx->v_type != VAR_ಸಂಖ್ಯೆ)
	{
	    emsg(_(e_ಸಂಖ್ಯೆ_expected));
	    ಸ್ಥಿತಿ_2 = FAIL;
	}
    }

    ಆದರೆ (ಸ್ಥಿತಿ_2 == OK)
    {
	ಆದರೆ (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    ಆದರೆ (list == ಖಾಲಿ)
	    {
		emsg(_(e_list_not_ಗಣ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    ಆದರೆ (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), lidx);
		ಹಿಂತಿರುಗಿ FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    ಆದರೆ (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		ಹಿಂತಿರುಗಿ FAIL;

	    ಆದರೆ (lidx < list->lv_len)
	    {
		listitem_T *li = list_ಕಂಡುಹಿಡಿ(list, lidx);
		ಆದರೆ (li == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
		ಆದರೆ (error_if_locked(li->li_tv.v_ಬೀಗ,
					     e_cannot_change_locked_list_ಅಂಶ))
		    ಹಿಂತಿರುಗಿ FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (error_if_locked(list->lv_ಬೀಗ, e_cannot_change_locked_list))
		    ಹಿಂತಿರುಗಿ FAIL;
		// append to list, only fails when out of memory
		ಆದರೆ (list_append_tv(list, tv) == FAIL)
		    ಹಿಂತಿರುಗಿ NOTDONE;
		clear_tv(tv);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest_type == VAR_DICT)
	{
	    char_u		*ಕೀಲಿ = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    ಆದರೆ (dict == ಖಾಲಿ)
	    {
		emsg(_(e_dictionary_not_ಗಣ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
		ಕೀಲಿ = (char_u *)"";
	    di = dict_ಕಂಡುಹಿಡಿ(dict, ಕೀಲಿ, -1);
	    ಆದರೆ (di != ಖಾಲಿ)
	    {
		ಆದರೆ (error_if_locked(di->di_tv.v_ಬೀಗ,
						    e_cannot_change_dict_ಅಂಶ))
		    ಹಿಂತಿರುಗಿ FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (error_if_locked(dict->dv_ಬೀಗ, e_cannot_change_dict))
		    ಹಿಂತಿರುಗಿ FAIL;
		// add to dict, only fails when out of memory
		ಆದರೆ (dict_ಕೂಡಿಸು_tv(dict, (ಅಕ್ಷರ *)ಕೀಲಿ, tv) == FAIL)
		    ಹಿಂತಿರುಗಿ NOTDONE;
		clear_tv(tv);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    ಪೂರ್ಣಾಂಕ		    error = FALSE;
	    ಪೂರ್ಣಾಂಕ		    len;

	    ಆದರೆ (blob == ಖಾಲಿ)
	    {
		emsg(_(e_blob_not_ಗಣ));
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    len = blob_len(blob);
	    ಆದರೆ (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    ಆದರೆ (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), lidx);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(blob->bv_ಬೀಗ, (char_u *)"blob", FALSE))
		ಹಿಂತಿರುಗಿ FAIL;
	    nr = tv_get_ಸಂಖ್ಯೆ_chk(tv, &error);
	    ಆದರೆ (error)
		ಹಿಂತಿರುಗಿ FAIL;
	    ಆದರೆ (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_ಮೌಲ್ಯ_for_blob_nr), nr);
		ಹಿಂತಿರುಗಿ FAIL;
	    }
	    blob_ಗಣ_append(blob, lidx, nr);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    ಆದರೆ (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		ಆದರೆ (itf != ಖಾಲಿ)
		    // convert interface member index to class member index
		    lidx = object_ಸೂಚ್ಯಂಕ_from_itf_ಸೂಚ್ಯಂಕ(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    ಸ್ಥಿತಿ_2 = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಸ್ಥಿತಿ_2 = FAIL;
	    semsg(_(e_cannot_ಸೂಚ್ಯಂಕ_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ರಾಶಿ.ga_len -= 3;
    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
    {
	clear_tv(tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Store a value in a list or blob range.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = ರಾಶಿ_TV_BOT(-3);
    typval_T	*tv_idx2 = ರಾಶಿ_TV_BOT(-2);
    typval_T	*tv_dest = ರಾಶಿ_TV_BOT(-1);
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = ರಾಶಿ_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    ಆದರೆ (tv_dest->v_type == VAR_LIST)
    {
	ದೀರ್ಘ	    n1;
	ದೀರ್ಘ	    n2;
	listitem_T  *li1;

	n1 = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(tv_idx1, ಖಾಲಿ);
	ಆದರೆ (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    n2 = (ದೀರ್ಘ)tv_get_ಸಂಖ್ಯೆ_chk(tv_idx2, ಖಾಲಿ);

	li1 = check_range_ಸೂಚ್ಯಂಕ_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	ಆದರೆ (li1 == ಖಾಲಿ)
	    ಸ್ಥಿತಿ_2 = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಸ್ಥಿತಿ_2 = check_range_ಸೂಚ್ಯಂಕ_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    ಆದರೆ (ಸ್ಥಿತಿ_2 != FAIL)
		ಸ್ಥಿತಿ_2 = list_ನಿಯೋಜಿಸು_range(
			tv_dest->vval.v_list,
			tv->vval.v_list,
			n1,
			n2,
			tv_idx2->v_type == VAR_SPECIAL,
			(char_u *)"=",
			(char_u *)"[unknown]");
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	ದೀರ್ಘ	    bloblen;

	n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv_idx1, ಖಾಲಿ);
	ಆದರೆ (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	ಇಲ್ಲದಿದ್ದರೆ
	    n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv_idx2, ಖಾಲಿ);
	bloblen = blob_len(tv_dest->vval.v_blob);

	ಆದರೆ (check_blob_ಸೂಚ್ಯಂಕ(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    ಸ್ಥಿತಿ_2 = FAIL;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ಥಿತಿ_2 = blob_ಗಣ_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸ್ಥಿತಿ_2 = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ರಾಶಿ.ga_len -= 4;
    clear_tv(tv);

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Unlet item in list or dict variable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = ರಾಶಿ_TV_BOT(-2);
    typval_T	*tv_dest = ರಾಶಿ_TV_BOT(-1);
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    ಆದರೆ (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	ಆದರೆ (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_ಸಂಖ್ಯೆ)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    ಸ್ಥಿತಿ_2 = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*ಕೀಲಿ;
	    dictitem_T  *di = ಖಾಲಿ;

	    ಆದರೆ (d != ಖಾಲಿ && ಮೌಲ್ಯ_check_ಬೀಗ(
				      d->dv_ಬೀಗ, ಖಾಲಿ, FALSE))
		ಸ್ಥಿತಿ_2 = FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (tv_idx->v_type == VAR_STRING)
		{
		    ಕೀಲಿ = tv_idx->vval.v_string;
		    ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
			ಕೀಲಿ = (char_u *)"";
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಕೀಲಿ = tv_get_string(tv_idx);
		}
		ಆದರೆ (d != ಖಾಲಿ)
		    di = dict_ಕಂಡುಹಿಡಿ(d, ಕೀಲಿ, (ಪೂರ್ಣಾಂಕ)STRLEN(ಕೀಲಿ));
		ಆದರೆ (di == ಖಾಲಿ)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str), ಕೀಲಿ);
		    ಸ್ಥಿತಿ_2 = FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (var_check_fixed(di->di_flags,
						   ಖಾಲಿ, FALSE)
			|| var_check_ro(di->di_flags,
						  ಖಾಲಿ, FALSE))
		    ಸ್ಥಿತಿ_2 = FAIL;
		ಇಲ್ಲದಿದ್ದರೆ
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	ಆದರೆ (check_for_ಸಂಖ್ಯೆ(tv_idx) == FAIL)
	{
	    ಸ್ಥಿತಿ_2 = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    ದೀರ್ಘ	n = (ದೀರ್ಘ)tv_idx->vval.v_ಸಂಖ್ಯೆ;

	    ಆದರೆ (l != ಖಾಲಿ && ಮೌಲ್ಯ_check_ಬೀಗ(
				      l->lv_ಬೀಗ, ಖಾಲಿ, FALSE))
		ಸ್ಥಿತಿ_2 = FAIL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		listitem_T	*li = list_ಕಂಡುಹಿಡಿ(l, n);

		ಆದರೆ (li == ಖಾಲಿ)
		{
		    semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), n);
		    ಸ್ಥಿತಿ_2 = FAIL;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    listitem_remove(l, li);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸ್ಥಿತಿ_2 = FAIL;
	semsg(_(e_cannot_ಸೂಚ್ಯಂಕ_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ರಾಶಿ.ga_len -= 2;

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Unlet a range of items in a list variable.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = ರಾಶಿ_TV_BOT(-3);
    typval_T	*tv_idx2 = ರಾಶಿ_TV_BOT(-2);
    typval_T	*tv_dest = ರಾಶಿ_TV_BOT(-1);
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;

    ಆದರೆ (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	ಆದರೆ (check_for_ಸಂಖ್ಯೆ(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_ಸಂಖ್ಯೆ(tv_idx2) == FAIL))
	{
	    ಸ್ಥಿತಿ_2 = FAIL;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    list_T	*l = tv_dest->vval.v_list;
	    ದೀರ್ಘ	n1 = (ದೀರ್ಘ)tv_idx1->vval.v_ಸಂಖ್ಯೆ;
	    ದೀರ್ಘ	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (ದೀರ್ಘ)tv_idx2->vval.v_ಸಂಖ್ಯೆ;
	    listitem_T	*li;

	    li = list_ಕಂಡುಹಿಡಿ_ಸೂಚ್ಯಂಕ(l, &n1);
	    ಆದರೆ (li == ಖಾಲಿ)
	    {
		semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr),
						 (ದೀರ್ಘ)tv_idx1->vval.v_ಸಂಖ್ಯೆ);
		ಸ್ಥಿತಿ_2 = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಆದರೆ (n1 < 0)
		    n1 = list_idx_ದ_ಅಂಶ(l, li);
		ಆದರೆ (n2 < 0)
		{
		    listitem_T *li2 = list_ಕಂಡುಹಿಡಿ(l, n2);

		    ಆದರೆ (li2 == ಖಾಲಿ)
		    {
			semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), n2);
			ಸ್ಥಿತಿ_2 = FAIL;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			n2 = list_idx_ದ_ಅಂಶ(l, li2);
		}
		ಆದರೆ (ಸ್ಥಿತಿ_2 != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), n2);
		    ಸ್ಥಿತಿ_2 = FAIL;
		}
		ಆದರೆ (ಸ್ಥಿತಿ_2 != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಸ್ಥಿತಿ_2 = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_ಸೂಚ್ಯಂಕ_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ರಾಶಿ.ga_len -= 3;

    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}

/*
 * Top of a for loop.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    ಪೂರ್ಣಾಂಕ		jump = FALSE;
    typval_T	*ltv = ರಾಶಿ_TV_BOT(-1);
    typval_T	*idxtv =
		   ರಾಶಿ_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
	ಹಿಂತಿರುಗಿ FAIL;
    ಆದರೆ (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_ಸಂಖ್ಯೆ;
	ಆದರೆ (list == ಖಾಲಿ
		       || idxtv->vval.v_ಸಂಖ್ಯೆ >= list->lv_len)
	{
	    jump = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (list->lv_ಮೊದಲ == &range_list_ಅಂಶ)
	{
	    // non-materialized range() list
	    tv = ರಾಶಿ_TV_BOT(0);
	    tv->v_type = VAR_ಸಂಖ್ಯೆ;
	    tv->v_ಬೀಗ = 0;
	    tv->vval.v_ಸಂಖ್ಯೆ = list_ಕಂಡುಹಿಡಿ_nr(
			     list, idxtv->vval.v_ಸಂಖ್ಯೆ, ಖಾಲಿ);
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    listitem_T *li = list_ಕಂಡುಹಿಡಿ(list,
					 idxtv->vval.v_ಸಂಖ್ಯೆ);
	    ಆದರೆ (li == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ FAIL;
	    copy_tv(&li->li_tv, ರಾಶಿ_TV_BOT(0));
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_ಸಂಖ್ಯೆ;
	ಆದರೆ (tuple == ಖಾಲಿ || idxtv->vval.v_ಸಂಖ್ಯೆ >= TUPLE_LEN(tuple))
	    jump = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    copy_tv(TUPLE_ಅಂಶ(tuple, idxtv->vval.v_ಸಂಖ್ಯೆ), ರಾಶಿ_TV_BOT(0));
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_ಸಂಖ್ಯೆ;
	ಆದರೆ (str == ಖಾಲಿ || str[idxtv->vval.v_ಸಂಖ್ಯೆ] == NUL)
	{
	    jump = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ	clen = mb_ptr2len(str + idxtv->vval.v_ಸಂಖ್ಯೆ);

	    // Push the next character from the string.
	    tv = ರಾಶಿ_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_ಸಂಖ್ಯೆ, clen);
	    ++ectx->ec_ರಾಶಿ.ga_len;
	    idxtv->vval.v_ಸಂಖ್ಯೆ += clen - 1;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	ಆದರೆ (idxtv->vval.v_ಸಂಖ್ಯೆ == -1 && blob != ಖಾಲಿ)
	{
	    ltv->v_ಬೀಗ = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_ಸಂಖ್ಯೆ;
	ಆದರೆ (blob == ಖಾಲಿ || idxtv->vval.v_ಸಂಖ್ಯೆ >= blob_len(blob))
	{
	    jump = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Push the next byte from the blob.
	    tv = ರಾಶಿ_TV_BOT(0);
	    tv->v_type = VAR_ಸಂಖ್ಯೆ;
	    tv->vval.v_ಸಂಖ್ಯೆ = blob_get(blob,
					 idxtv->vval.v_ಸಂಖ್ಯೆ);
	    ++ectx->ec_ರಾಶಿ.ga_len;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_ಕೊನೆ;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_ಸಂಖ್ಯೆ = ectx->ec_funcrefs.ga_len;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
ಸ್ಥಿರ loopvars_T *ಮೊದಲ_loopvars = ಖಾಲಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_loopvars_ಗಮ್ಯ_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    ಆದರೆ (ಮೊದಲ_loopvars != ಖಾಲಿ)
	ಮೊದಲ_loopvars->lvs_prev = loopvars;
    loopvars->lvs_ಮುಂದಿನ = ಮೊದಲ_loopvars;
    loopvars->lvs_prev = ಖಾಲಿ;
    ಮೊದಲ_loopvars = loopvars;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
remove_loopvars_from_list(loopvars_T *loopvars)
{
    ಆದರೆ (loopvars->lvs_prev == ಖಾಲಿ)
	ಮೊದಲ_loopvars = loopvars->lvs_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	loopvars->lvs_prev->lvs_ಮುಂದಿನ = loopvars->lvs_ಮುಂದಿನ;
    ಆದರೆ (loopvars->lvs_ಮುಂದಿನ != ಖಾಲಿ)
	loopvars->lvs_ಮುಂದಿನ->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = ರಾಶಿ_TV_VAR(endloop->ಕೊನೆ_funcref_idx);
    ಪೂರ್ಣಾಂಕ		prev_ಆವರಣ_ಎಣಿಕೆ = tv_refcount->vval.v_ಸಂಖ್ಯೆ;
    ಪೂರ್ಣಾಂಕ		ಆಳ = endloop->ಕೊನೆ_ಆಳ;
    garray_T	*gap = &ectx->ec_funcrefs;
    ಪೂರ್ಣಾಂಕ		ಆವರಣ_ಒಳ_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *ರಾಶಿ;
    ಪೂರ್ಣಾಂಕ		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    ಗಾಗಿ (idx = prev_ಆವರಣ_ಎಣಿಕೆ; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_ದತ್ತಾಂಶ)[idx];

	ಆದರೆ (pt->pt_refcount > 1 && pt->pt_loopvars[ಆಳ] == ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ refcount = pt->pt_refcount;
	    ಪೂರ್ಣಾಂಕ i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    ಗಾಗಿ (i = 0; i < endloop->ಕೊನೆ_var_ಎಣಿಕೆ; ++i)
	    {
		typval_T *stv = ರಾಶಿ_TV_VAR(endloop->ಕೊನೆ_var_idx + i);

		ಆದರೆ (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ಆದರೆ (refcount > 1)
	    {
		ಆವರಣ_ಒಳ_use = TRUE;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    ಆದರೆ (!ಆವರಣ_ಒಳ_use)
	ಹಿಂತಿರುಗಿ OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    ಆದರೆ (loopvars == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    loopvars->lvs_ga.ga_len = endloop->ಕೊನೆ_var_ಎಣಿಕೆ;
    ರಾಶಿ = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_ದತ್ತಾಂಶ = ರಾಶಿ;
    ಆದರೆ (ರಾಶಿ == ಖಾಲಿ)
    {
	vim_free(loopvars);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಕೂಡಿಸು_loopvars_ಗಮ್ಯ_list(loopvars);

    // Move the variable values.
    ಗಾಗಿ (idx = 0; idx < endloop->ಕೊನೆ_var_ಎಣಿಕೆ; ++idx)
    {
	typval_T *tv = ರಾಶಿ_TV_VAR(endloop->ಕೊನೆ_var_idx + idx);

	*(ರಾಶಿ + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    ಗಾಗಿ (idx = prev_ಆವರಣ_ಎಣಿಕೆ; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_ದತ್ತಾಂಶ)[idx];

	ಆದರೆ (pt->pt_refcount > 1 && pt->pt_loopvars[ಆಳ] == ಖಾಲಿ)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[ಆಳ] = loopvars;

	    pt->pt_outer.ಹೊರಹೋಗು_loop[ಆಳ].ರಾಶಿ = &loopvars->lvs_ga;
	    pt->pt_outer.ಹೊರಹೋಗು_loop[ಆಳ].var_idx -=
		  ectx->ec_ಚೌಕಟ್ಟು_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + endloop->ಕೊನೆ_var_idx;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    ಪೂರ್ಣಾಂಕ
loopvars_check_refcount(loopvars_T *loopvars)
{
    ಪೂರ್ಣಾಂಕ		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    ಪೂರ್ಣಾಂಕ		    ಮುಗಿದ = 0;
	typval_T	*ರಾಶಿ = gap->ga_ದತ್ತಾಂಶ;

    ಆದರೆ (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	ಹಿಂತಿರುಗಿ FALSE;
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_ದತ್ತಾಂಶ) + i;

	ಆದರೆ (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ಖಾಲಿ
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    ಪೂರ್ಣಾಂಕ	    ಆಳ;

	    ಗಾಗಿ (ಆಳ = 0; ಆಳ < MAX_LOOP_ಆಳ; ++ಆಳ)
		ಆದರೆ (tv->vval.v_partial->pt_loopvars[ಆಳ] == loopvars)
		    ++ಮುಗಿದ;
	}
    }
    ಆದರೆ (ಮುಗಿದ != loopvars->lvs_min_refcount)
	ಹಿಂತಿರುಗಿ FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    ರಾಶಿ = gap->ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < gap->ga_len; ++i)
	clear_tv(ರಾಶಿ + i);
    vim_free(ರಾಶಿ);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    ಪೂರ್ಣಾಂಕ
ಗಣ_ref_ಒಳ_loopvars(ಪೂರ್ಣಾಂಕ copyID)
{
    loopvars_T *loopvars;

    ಗಾಗಿ (loopvars = ಮೊದಲ_loopvars; loopvars != ಖಾಲಿ;
						 loopvars = loopvars->lvs_ಮುಂದಿನ)
    {
	typval_T    *ರಾಶಿ = loopvars->lvs_ga.ga_ದತ್ತಾಂಶ;
	ಪೂರ್ಣಾಂಕ	    i;

	ಗಾಗಿ (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    ಆದರೆ (ಗಣ_ref_ಒಳ_ಅಂಶ(ರಾಶಿ + i, copyID, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ))
		ಹಿಂತಿರುಗಿ TRUE;  // abort
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ತುಂಬು_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = ಖಾಲಿ;
    hashtab_T	*ht = ಖಾಲಿ;
    ಅಕ್ಷರ	namespace;

    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
	ಹಿಂತಿರುಗಿ NOTDONE;
    ಆಯ್ಕೆ (isn_type)
    {
	ಸ್ಥಿತಿ ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:  // Cannot reach here
	    ಹಿಂತಿರುಗಿ NOTDONE;
    }
    di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, iptr->isn_arg.string, TRUE);

    ಆದರೆ (di == ಖಾಲಿ)
    {
	ಆದರೆ (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = ಕಂಡುಹಿಡಿ_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    ಆದರೆ (ufunc != ಖಾಲಿ)
	    {
		typval_T    *tv = ರಾಶಿ_TV_BOT(0);

		++ectx->ec_ರಾಶಿ.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		ಆದರೆ (tv->vval.v_string == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		ಹಿಂತಿರುಗಿ OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	ಆದರೆ (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != ಖಾಲಿ)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_ಅಂಶ_not_ಸಿಕ್ಕಿತು_ಒಳ_script_str), iptr->isn_arg.string);
	ಇಲ್ಲದಿದ್ದರೆ
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = ರಾಶಿ_TV_BOT(0);
	tv->v_type = VAR_ಸಂಖ್ಯೆ;
	tv->v_ಬೀಗ = 0;
	tv->vval.v_ಸಂಖ್ಯೆ = 0;
	++ectx->ec_ರಾಶಿ.ga_len;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	copy_tv(&di->di_tv, ರಾಶಿ_TV_BOT(0));
	++ectx->ec_ರಾಶಿ.ga_len;
    }
    ಹಿಂತಿರುಗಿ OK;
}


    ಸ್ಥಿರ ಶೂನ್ಯ
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, ಗಾತ್ರ(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    ಅಕ್ಷರ *tofree = ಖಾಲಿ;
    ಅಕ್ಷರ *typename = type_name(type, &tofree);
    semsg(_(e_object_required_ಸಿಕ್ಕಿತು_str), typename);
    vim_free(tofree);
    clear_type_list(&type_list);
}

/*
 * Accessing the variable or method of an object or a class stored in a
 * variable of type "any".
 * Returns OK if the member variable is present.
 * Returns FAIL if the variable is not found.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    ಪೂರ್ಣಾಂಕ		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = ಖಾಲಿ;
    typval_T	mtv;

    ಆದರೆ (is_object)
    {
	obj = tv->vval.v_object;
	ಆದರೆ (obj == ಖಾಲಿ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	tv_cl = obj->obj_class;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tv_cl = tv->vval.v_class;
	ಆದರೆ (tv_cl == ಖಾಲಿ)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    ಪೂರ್ಣಾಂಕ		namelen = (ಪೂರ್ಣಾಂಕ)STRLEN(iptr->isn_arg.string);
    ಪೂರ್ಣಾಂಕ		save_did_emsg = did_emsg;

    ಆದರೆ (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ (did_emsg != save_did_emsg)
	ಹಿಂತಿರುಗಿ FAIL;

    // could be a class or instance method
    ufunc_T	*oc_ವಿಧಾನ;
    ಪೂರ್ಣಾಂಕ		oc_ವಿಧಾನ_idx;

    oc_ವಿಧಾನ = ವಿಧಾನ_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_ವಿಧಾನ_idx);
    ಆದರೆ (oc_ವಿಧಾನ == ಖಾಲಿ)
    {
	ಅಕ್ಷರ	*ಸಂದೇಶ;

	SOURCING_LNUM = iptr->isn_lnum;
	ಆದರೆ (is_object)
	    ಸಂದೇಶ = e_variable_not_ಸಿಕ್ಕಿತು_on_object_str_str;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ = e_class_variable_str_not_ಸಿಕ್ಕಿತು_ಒಳ_class_str;
	semsg(_(ಸಂದೇಶ), iptr->isn_arg.string, tv_cl->class_name.string);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Protected methods are not accessible outside the class
    ಆದರೆ (*oc_ವಿಧಾನ->uf_name == '_'
			&& !class_instance_ದ(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_ವಿಧಾನ_str), oc_ವಿಧಾನ->uf_name);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    // Create a partial for the instance or class method
    ಆದರೆ (obj_ವಿಧಾನ_ಗಮ್ಯ_partial_tv(is_object ? obj : ಖಾಲಿ, oc_ವಿಧಾನ, tv)
								== FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, ಪೂರ್ಣಾಂಕ fixindent)
{
    ಪೂರ್ಣಾಂಕ		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*ಅಭಿವ್ಯಕ್ತಿ = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		dir = FORWARD;

    ಆದರೆ (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = ರಾಶಿ_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_ಸಂಖ್ಯೆ;
	ಆದರೆ (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_ರಾಶಿ.ga_len;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	ಆದರೆ (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    ಆದರೆ (regname == '=')
    {
	tv = ರಾಶಿ_TV_BOT(-1);
	ಆದರೆ (tv->v_type == VAR_STRING)
	    ಅಭಿವ್ಯಕ್ತಿ = tv->vval.v_string;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಭಿವ್ಯಕ್ತಿ = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_ರಾಶಿ.ga_len;
    }
    check_cursor();

    ಆದರೆ (fixindent)
	do_put(regname, ಅಭಿವ್ಯಕ್ತಿ, dir, 1L, PUT_ಸಾಲು|PUT_CURSLINE|PUT_FIXINDENT);
    ಇಲ್ಲದಿದ್ದರೆ
	do_put(regname, ಅಭಿವ್ಯಕ್ತಿ, dir, 1L, PUT_ಸಾಲು|PUT_CURSLINE);
    vim_free(ಅಭಿವ್ಯಕ್ತಿ);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = iptr->isn_arg.unpack.unp_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    ಪೂರ್ಣಾಂಕ		i;

    l = tv->vval.v_list;
    ಆದರೆ (l == ಖಾಲಿ
	    || l->lv_len < (semicolon ? ಎಣಿಕೆ_2 - 1 : ಎಣಿಕೆ_2))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_ಮೌಲ್ಯ_does_not_have_enough_ವಸ್ತುಗಳು));
	ಹಿಂತಿರುಗಿ EXEC_FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!semicolon && l->lv_len > ಎಣಿಕೆ_2)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_ಮೌಲ್ಯ_has_ಇನ್ನಷ್ಟು_ವಸ್ತುಗಳು_than_targets));
	ಹಿಂತಿರುಗಿ EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, ಎಣಿಕೆ_2 - 1))
	ಹಿಂತಿರುಗಿ EXEC_ಮುಗಿದ;
    ectx->ec_ರಾಶಿ.ga_len += ಎಣಿಕೆ_2 - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ಆದರೆ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ವಸ್ತುಗಳು(l->lv_len - ಎಣಿಕೆ_2 + 1);

	ಆದರೆ (rem_list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ EXEC_ಮುಗಿದ;
	tv = ರಾಶಿ_TV_BOT(-ಎಣಿಕೆ_2);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_ಬೀಗ = 0;
	li = l->lv_ಮೊದಲ;
	ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2 - 1; ++i)
	    li = li->li_ಮುಂದಿನ;
	ಗಾಗಿ (i = 0; li != ಖಾಲಿ; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_ಗಣ_ಅಂಶ(rem_list, i, &tvcopy);
	    li = li->li_ಮುಂದಿನ;
	}
	--ಎಣಿಕೆ_2;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_ಮೊದಲ;
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	tv = ರಾಶಿ_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_ಮುಂದಿನ;
    }

    list_unref(l);

    ಹಿಂತಿರುಗಿ EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = iptr->isn_arg.unpack.unp_ಎಣಿಕೆ;
    ಪೂರ್ಣಾಂಕ		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    ಪೂರ್ಣಾಂಕ		i;

    tuple = tv->vval.v_tuple;
    ಆದರೆ (tuple == ಖಾಲಿ
	    || TUPLE_LEN(tuple) < (semicolon ? ಎಣಿಕೆ_2 - 1 : ಎಣಿಕೆ_2))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_ಇನ್ನಷ್ಟು_targets_than_tuple_ವಸ್ತುಗಳು));
	ಹಿಂತಿರುಗಿ EXEC_FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!semicolon && TUPLE_LEN(tuple) > ಎಣಿಕೆ_2)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_ವಸ್ತುಗಳು));
	ಹಿಂತಿರುಗಿ EXEC_FAIL;
    }

    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, ಎಣಿಕೆ_2 - 1))
	ಹಿಂತಿರುಗಿ EXEC_ಮುಗಿದ;
    ectx->ec_ರಾಶಿ.ga_len += ಎಣಿಕೆ_2 - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ಆದರೆ (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ವಸ್ತುಗಳು(TUPLE_LEN(tuple) - ಎಣಿಕೆ_2 + 1);

	ಆದರೆ (rem_list == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ EXEC_ಮುಗಿದ;
	tv = ರಾಶಿ_TV_BOT(-ಎಣಿಕೆ_2);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_ಬೀಗ = 0;
	ಗಾಗಿ (i = ಎಣಿಕೆ_2 - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ಅಂಶ(tuple, i), &tvcopy);
	    list_ಗಣ_ಅಂಶ(rem_list, i - (ಎಣಿಕೆ_2 - 1), &tvcopy);
	}
	--ಎಣಿಕೆ_2;
    }

    // Produce the values in reverse order, first item last.
    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
    {
	tv = ರಾಶಿ_TV_BOT(-i - 1);
	copy_tv(TUPLE_ಅಂಶ(tuple, i), tv);
    }

    tuple_unref(tuple);

    ಹಿಂತಿರುಗಿ EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
exec_instructions(ectx_T *ectx)
{
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    ಪೂರ್ಣಾಂಕ		save_trylevel_at_ಆರಂಭ = ectx->ec_trylevel_at_ಆರಂಭ;
    ಪೂರ್ಣಾಂಕ		dict_ರಾಶಿ_len_at_ಆರಂಭ = dict_ರಾಶಿ.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_ಆರಂಭ = trylevel;

    ಗಾಗಿ (;;)
    {
	ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  breakcheck_ಎಣಿಕೆ = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	ಆದರೆ (unlikely(++breakcheck_ಎಣಿಕೆ >= 100))
	{
	    ಸಾಲು_breakcheck();
	    breakcheck_ಎಣಿಕೆ = 0;
	}
	ಆದರೆ (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    ಆದರೆ (throw_exception("Vim:Interrupt", ET_ಅಡಚಣೆ, ಖಾಲಿ) == FAIL)
		ಹೋಗು theend;
	    did_throw = TRUE;
	}

	ಆದರೆ (unlikely(did_emsg && ಸಂದೇಶ_list != ಖಾಲಿ && *ಸಂದೇಶ_list != ಖಾಲಿ))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    ಆದರೆ (throw_exception(*ಸಂದೇಶ_list, ET_ERROR, ಖಾಲಿ) == FAIL)
		ಹೋಗು theend;
	    did_throw = TRUE;
	    *ಸಂದೇಶ_list = ಖಾಲಿ;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    ಆದರೆ (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_ದತ್ತಾಂಶ)
							+ trystack->ga_len - 1;
		ಆದರೆ (trycmd->tcd_ಚೌಕಟ್ಟು_idx == ectx->ec_ಚೌಕಟ್ಟು_idx)
		{
		    ಆದರೆ (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			ಮುಕ್ತಾಯ_exception(caught_ರಾಶಿ);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	ಆದರೆ (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = ಖಾಲಿ;
	    ಪೂರ್ಣಾಂಕ		ಸೂಚ್ಯಂಕ = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    ಆಗ (ಸೂಚ್ಯಂಕ > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_ದತ್ತಾಂಶ) + ಸೂಚ್ಯಂಕ - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		ಆದರೆ (!trycmd->tcd_ಒಳ_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_ಚೌಕಟ್ಟು_idx == ectx->ec_ಚೌಕಟ್ಟು_idx)
		    ನಿಲ್ಲಿಸು;
		// In the catch and finally block of this try we have to go up
		// one level.
		--ಸೂಚ್ಯಂಕ;
		trycmd = ಖಾಲಿ;
	    }
	    ಆದರೆ (trycmd != ಖಾಲಿ && trycmd->tcd_ಚೌಕಟ್ಟು_idx == ectx->ec_ಚೌಕಟ್ಟು_idx)
	    {
		ಆದರೆ (trycmd->tcd_ಒಳ_catch)
		{
		    ಆದರೆ (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_ಒಳ_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಹೋಗು theend;
		tv = ರಾಶಿ_TV_BOT(0);
		tv->v_type = VAR_ಸಂಖ್ಯೆ;
		tv->vval.v_ಸಂಖ್ಯೆ = 0;
		++ectx->ec_ರಾಶಿ.ga_len;
		ಆದರೆ (ectx->ec_ಚೌಕಟ್ಟು_idx == ectx->ec_initial_ಚೌಕಟ್ಟು_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    ಆದರೆ (handle_ಆವರಣ_ಒಳ_use(ectx, FALSE) == FAIL)
			ಹೋಗು theend;
		    ಹೋಗು ಮುಗಿದ;
		}

		ಆದರೆ (func_return(ectx) == FAIL)
		    ಹೋಗು theend;
	    }
	    ಮುಂದುವರಿ;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	ಆಯ್ಕೆ (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    ಸ್ಥಿತಿ ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = ರಾಶಿ_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		ನಿಲ್ಲಿಸು;

	    // execute Ex command line
	    ಸ್ಥಿತಿ ISN_EXEC:
		ಆದರೆ (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;

	    // execute Ex command line split at NL characters.
	    ಸ್ಥಿತಿ ISN_EXEC_ವಿಭಜಿಸು:
		{
		    ಮೂಲ_cookie_T cookie;
		    char_u	    *ಸಾಲು;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_ಕ್ಷೇತ್ರ(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    ಸಾಲು = get_ವಿಭಜಿಸು_sourceline(0, &cookie, 0, 0);
		    ಆದರೆ (do_cmdline(ಸಾಲು,
				get_ವಿಭಜಿಸು_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(ಸಾಲು);
			ಹೋಗು on_error;
		    }
		    vim_free(ಸಾಲು);
		}
		ನಿಲ್ಲಿಸು;

	    // execute Ex command line that is only a range
	    ಸ್ಥಿತಿ ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    ಅಕ್ಷರ	*error = ಖಾಲಿ;

		    CLEAR_ಕ್ಷೇತ್ರ(ea);
		    ea.cmdidx = CMD_ಗಾತ್ರ;
		    ea.addr_type = ADDR_ಸಾಲುಗಳು;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_ವಿಳಾಸ(&ea, &error, FALSE);
		    ಆದರೆ (ea.cmd == ಖಾಲಿ)
			ಹೋಗು on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    ಆದರೆ (error != ಖಾಲಿ)
		    {
			emsg(error);
			ಹೋಗು on_error;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    ಸ್ಥಿತಿ ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    ಪೂರ್ಣಾಂಕ	    res;
		    ಪೂರ್ಣಾಂಕ	    save_flags = cmdmod.cmod_flags;

		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    tv = ರಾಶಿ_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, ಖಾಲಿ, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    ಆದರೆ (res == FAIL)
			ಹೋಗು on_error;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    // push typeval VAR_INSTR with instructions to be executed
	    ಸ್ಥಿತಿ ISN_INSTR:
		{
		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    tv = ರಾಶಿ_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    ಆದರೆ (tv->vval.v_instr == ಖಾಲಿ)
			ಹೋಗು on_error;
		    ++ectx->ec_ರಾಶಿ.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ಮೂಲ:
		{
		    ಪೂರ್ಣಾಂಕ notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (may_ತುಂಬು_script((ಪೂರ್ಣಾಂಕ)iptr->isn_arg.ಸಂಖ್ಯೆ, &notused)
								       == FAIL)
			ಹೋಗು on_error;
		}
		ನಿಲ್ಲಿಸು;

	    // execute :substitute with an expression
	    ಸ್ಥಿತಿ ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    ಮೂಲ_cookie_T	cookie;
		    ರಚನೆ subs_ಅಭಿವ್ಯಕ್ತಿ_S	*save_instr = substitute_instr;
		    ರಚನೆ subs_ಅಭಿವ್ಯಕ್ತಿ_S	subs_instr;
		    ಪೂರ್ಣಾಂಕ			res;

		    subs_instr.subs_ectx = ectx;
		    subs_instr.subs_instr = subs->subs_instr;
		    subs_instr.subs_ಸ್ಥಿತಿ = OK;
		    substitute_instr = &subs_instr;

		    SOURCING_LNUM = iptr->isn_lnum;
		    // This is very much like ISN_EXEC
		    CLEAR_ಕ್ಷೇತ್ರ(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    res = do_cmdline(subs->subs_cmd,
				getsourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED);
		    substitute_instr = save_instr;

		    ಆದರೆ (res == FAIL || did_emsg
					     || subs_instr.subs_ಸ್ಥಿತಿ == FAIL)
			ಹೋಗು on_error;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ಮುಕ್ತಾಯ:
		ಹೋಗು ಮುಗಿದ;

	    ಸ್ಥಿತಿ ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		ಆದರೆ (alloc_redir_lval() == FAIL)
		    ಹೋಗು on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    {
			vim_free(res);
			ಹೋಗು theend;
		    }
		    tv = ರಾಶಿ_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CEXPR_AUCMD:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		force_abort = TRUE;
		ಆದರೆ (trigger_cexpr_autocmd(iptr->isn_arg.ಸಂಖ್ಯೆ) == FAIL)
		    ಹೋಗು on_error;
		force_abort = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CEXPR_CORE:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		{
		    exarg_T ea;
		    ಪೂರ್ಣಾಂಕ	    res;

		    CLEAR_ಕ್ಷೇತ್ರ(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv = ರಾಶಿ_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    ಆದರೆ (res == FAIL)
			ಹೋಗು on_error;
		}
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;

	    // execute Ex command from pieces on the stack
	    ಸ್ಥಿತಿ ISN_EXECCONCAT:
		{
		    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2 = iptr->isn_arg.ಸಂಖ್ಯೆ;
		    size_t  len = 0;
		    ಪೂರ್ಣಾಂಕ	    pass;
		    ಪೂರ್ಣಾಂಕ	    i;
		    char_u  *cmd = ಖಾಲಿ;
		    char_u  *str;

		    ಗಾಗಿ (pass = 1; pass <= 2; ++pass)
		    {
			ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
			{
			    tv = ರಾಶಿ_TV_BOT(i - ಎಣಿಕೆ_2);
			    str = tv->vval.v_string;
			    ಆದರೆ (str != ಖಾಲಿ && *str != NUL)
			    {
				ಆದರೆ (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    ಆದರೆ (pass == 2)
				clear_tv(tv);
			}
			ಆದರೆ (pass == 1)
			{
			    cmd = alloc(len + 1);
			    ಆದರೆ (unlikely(cmd == ಖಾಲಿ))
				ಹೋಗು theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		ನಿಲ್ಲಿಸು;

	    // execute :echo {string} ...
	    ಸ್ಥಿತಿ ISN_ECHO:
		{
		    ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = iptr->isn_arg.echo.echo_ಎಣಿಕೆ;
		    ಪೂರ್ಣಾಂಕ	atstart = TRUE;
		    ಪೂರ್ಣಾಂಕ needclr = TRUE;
		    ಪೂರ್ಣಾಂಕ	idx;

		    ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; ++idx)
		    {
			tv = ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ_2);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    ಆದರೆ (needclr)
			ಸಂದೇಶ_clr_eos();
		    ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ_2;
		}
		ನಿಲ್ಲಿಸು;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    ಸ್ಥಿತಿ ISN_EXECUTE:
	    ಸ್ಥಿತಿ ISN_ECHOMSG:
	    ಸ್ಥಿತಿ ISN_ECHOWINDOW:
	    ಸ್ಥಿತಿ ISN_ECHOCONSOLE:
	    ಸ್ಥಿತಿ ISN_ECHOERR:
		{
		    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    ಪೂರ್ಣಾಂಕ		len;
		    ಪೂರ್ಣಾಂಕ		failed = FALSE;
		    ಪೂರ್ಣಾಂಕ		idx;

		    ಆದರೆ (iptr->isn_type == ISN_ECHOWINDOW)
			ಎಣಿಕೆ_2 = iptr->isn_arg.echowin.ewin_ಎಣಿಕೆ;
		    ಇಲ್ಲದಿದ್ದರೆ
			ಎಣಿಕೆ_2 = iptr->isn_arg.ಸಂಖ್ಯೆ;
		    ga_init2(&ga, 1, 80);
		    ಗಾಗಿ (idx = 0; idx < ಎಣಿಕೆ_2; ++idx)
		    {
			tv = ರಾಶಿ_TV_BOT(idx - ಎಣಿಕೆ_2);
			ಆದರೆ (iptr->isn_type == ISN_EXECUTE)
			{
			    ಆದರೆ (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_ಕೆಲಸ)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_ಮೌಲ್ಯ_as_string_str),
						    vartype_name(tv->v_type));
				ನಿಲ್ಲಿಸು;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
				p = tv_get_string_buf(tv, buf);
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (ಪೂರ್ಣಾಂಕ)STRLEN(p);
			ಆದರೆ (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಆದರೆ (ga.ga_len > 0)
				((char_u *)(ga.ga_ದತ್ತಾಂಶ))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_ದತ್ತಾಂಶ) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_ರಾಶಿ.ga_len -= ಎಣಿಕೆ_2;
		    ಆದರೆ (failed)
		    {
			ga_clear(&ga);
			ಹೋಗು on_error;
		    }

		    ಆದರೆ (ga.ga_ದತ್ತಾಂಶ != ಖಾಲಿ)
		    {
			ಆದರೆ (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_ದತ್ತಾಂಶ);
			    ಆದರೆ (did_emsg)
			    {
				ga_clear(&ga);
				ಹೋಗು on_error;
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಸಂದೇಶ_sb_eol();
			    ಆದರೆ (iptr->isn_type == ISN_ECHOMSG)
			    {
				ಸಂದೇಶ_attr(ga.ga_ದತ್ತಾಂಶ, echo_attr);
				ಹೊರಹೋಗು_flush();
			    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAS_ಸಂದೇಶ_ಕಿಟಕಿ
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				ಆರಂಭ_echowindow(
					      iptr->isn_arg.echowin.ewin_ಸಮಯ);
				ಸಂದೇಶ_attr(ga.ga_ದತ್ತಾಂಶ, echo_attr);
				ಕೊನೆ_echowindow();
			    }
#ಆದರೆ_ಅಂತ್ಯ
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_ದತ್ತಾಂಶ, (ಪೂರ್ಣಾಂಕ)STRLEN(ga.ga_ದತ್ತಾಂಶ),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_ದತ್ತಾಂಶ);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		ನಿಲ್ಲಿಸು;

	    // load local variable or argument
	    ಸ್ಥಿತಿ ISN_ತುಂಬು:
		ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಹೋಗು theend;
		tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.ಸಂಖ್ಯೆ);
		ಆದರೆ (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    ರಾಶಿ_TV_BOT(0)->v_type = VAR_SPECIAL;
		    ರಾಶಿ_TV_BOT(0)->vval.v_ಸಂಖ್ಯೆ = VVAL_NONE;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		    copy_tv(tv, ರಾಶಿ_TV_BOT(0));
		++ectx->ec_ರಾಶಿ.ga_len;
		ನಿಲ್ಲಿಸು;

	    // load v: variable
	    ಸ್ಥಿತಿ ISN_LOADV:
		ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಹೋಗು theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.ಸಂಖ್ಯೆ), ರಾಶಿ_TV_BOT(0));
		++ectx->ec_ರಾಶಿ.ga_len;
		ನಿಲ್ಲಿಸು;

	    // load s: variable in Vim9 script
	    ಸ್ಥಿತಿ ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ಆದರೆ (sv == ಖಾಲಿ)
			ಹೋಗು theend;
		    allocate_if_null(sv);
		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    copy_tv(sv->sv_tv, ರಾಶಿ_TV_BOT(0));
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    // load s: variable in old script or autoload import
	    ಸ್ಥಿತಿ ISN_LOADS:
	    ಸ್ಥಿತಿ ISN_LOADEXPORT:
		{
		    ಪೂರ್ಣಾಂಕ		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, name, TRUE);

		    ಆದರೆ (di == ಖಾಲಿ)
		    {
			ufunc_T	*ufunc = ಖಾಲಿ;

			ಆದರೆ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (ಶೂನ್ಯ)ಕಂಡುಹಿಡಿ_exported(sid, name, &ufunc, &type,
							 ಖಾಲಿ, ಖಾಲಿ, FALSE);
			}
			ಆದರೆ (ufunc == ಖಾಲಿ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    ಹೋಗು on_error;
			}
			ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			    ಹೋಗು theend;
			tv = ರಾಶಿ_TV_BOT(0);
			tv->v_ಬೀಗ = 0;
			++ectx->ec_ರಾಶಿ.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (iptr->isn_type == ISN_LOADEXPORT)
			{
			    ಪೂರ್ಣಾಂಕ idx = get_script_ಅಂಶ_idx(sid, name, 0,
								   ಖಾಲಿ, ಖಾಲಿ);
			    svar_T	*sv;

			    ಆದರೆ (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ಅಂಶ(sid)
						  ->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;
				ಆದರೆ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_ಅಂಶ_not_exported_ಒಳ_script_str),
									 name);
				    ಹೋಗು on_error;
				}
			    }
			}
			ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			    ಹೋಗು theend;
			copy_tv(&di->di_tv, ರಾಶಿ_TV_BOT(0));
			++ectx->ec_ರಾಶಿ.ga_len;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    // load g:/b:/w:/t: variable
	    ಸ್ಥಿತಿ ISN_LOADG:
	    ಸ್ಥಿತಿ ISN_LOADB:
	    ಸ್ಥಿತಿ ISN_LOADW:
	    ಸ್ಥಿತಿ ISN_LOADT:
		{
		    ಪೂರ್ಣಾಂಕ res = ತುಂಬು_namespace_var(ectx, iptr->isn_type, iptr);

		    ಆದರೆ (res == NOTDONE)
			ಹೋಗು theend;
		    ಆದರೆ (res == FAIL)
			ಹೋಗು on_error;
		}

		ನಿಲ್ಲಿಸು;

	    // load autoload variable
	    ಸ್ಥಿತಿ ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (eval_variable(name, (ಪೂರ್ಣಾಂಕ)STRLEN(name), 0,
			      ರಾಶಿ_TV_BOT(0), ಖಾಲಿ, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = ರಾಶಿ_TV_BOT(0);
			tv->v_type = VAR_ಸಂಖ್ಯೆ;
			tv->v_ಬೀಗ = 0;
			tv->vval.v_ಸಂಖ್ಯೆ = 0;
			++ectx->ec_ರಾಶಿ.ga_len;
			ಹೋಗು on_error;
		    }
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    // load g:/b:/w:/t: namespace
	    ಸ್ಥಿತಿ ISN_LOADGDICT:
	    ಸ್ಥಿತಿ ISN_LOADBDICT:
	    ಸ್ಥಿತಿ ISN_LOADWDICT:
	    ಸ್ಥಿತಿ ISN_LOADTDICT:
		{
		    dict_T *d = ಖಾಲಿ;

		    ಆಯ್ಕೆ (iptr->isn_type)
		    {
			ಸ್ಥಿತಿ ISN_LOADGDICT: d = get_globvar_dict(); ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ISN_LOADBDICT: d = curbuf->b_vars; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ISN_LOADWDICT: d = curwin->w_vars; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ISN_LOADTDICT: d = curtab->tp_vars; ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:  // Cannot reach here
			    ಹೋಗು theend;
		    }
		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    tv = ರಾಶಿ_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_ಬೀಗ = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    // load &option
	    ಸ್ಥಿತಿ ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    ಆದರೆ (eval_option(&name, &optval, TRUE) == FAIL)
			ಹೋಗು theend;
		    *ರಾಶಿ_TV_BOT(0) = optval;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    // load $ENV
	    ಸ್ಥಿತಿ ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    // name is always valid, checked when compiling
		    (ಶೂನ್ಯ)eval_env_var(&name, &optval, TRUE);
		    *ರಾಶಿ_TV_BOT(0) = optval;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    // load @register
	    ಸ್ಥಿತಿ ISN_LOADREG:
		ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಹೋಗು theend;
		tv = ರಾಶಿ_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_ಬೀಗ = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.ಸಂಖ್ಯೆ, GREG_ಅಭಿವ್ಯಕ್ತಿ_SRC);
		++ectx->ec_ರಾಶಿ.ga_len;
		ನಿಲ್ಲಿಸು;

	    // store local variable
	    ಸ್ಥಿತಿ ISN_STORE:
		--ectx->ec_ರಾಶಿ.ga_len;
		tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.ಸಂಖ್ಯೆ);
		ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(ರಾಶಿ_TV_BOT(0)) == FAIL
			|| ರಾಶಿ_TV_BOT(0)->v_type == VAR_VOID)
		{
		    ಆದರೆ (ರಾಶಿ_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_ಮೌಲ್ಯ));
		    clear_tv(ರಾಶಿ_TV_BOT(0));
		    ಹೋಗು on_error;
		}
		clear_tv(tv);
		*tv = *ರಾಶಿ_TV_BOT(0);
		ನಿಲ್ಲಿಸು;

	    // store s: variable in old script or autoload import
	    ಸ್ಥಿತಿ ISN_STORES:
	    ಸ್ಥಿತಿ ISN_STOREEXPORT:
		{
		    ಪೂರ್ಣಾಂಕ		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = ಖಾಲಿ;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    ಆದರೆ (iptr->isn_type == ISN_STOREEXPORT)
			di = ಕಂಡುಹಿಡಿ_var_autoload_ಪೂರ್ವಪ್ರತ್ಯಯ(name, sid, ಖಾಲಿ, ಖಾಲಿ);
		    // Then look for a variable in the script's variables.
		    ಆದರೆ (di == ಖಾಲಿ)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_ರಾಶಿ.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (di == ಖಾಲಿ)
		    {
			ಆದರೆ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(ರಾಶಿ_TV_BOT(0));
			    ಹೋಗು on_error;
			}
			store_var(name, ರಾಶಿ_TV_BOT(0));
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (iptr->isn_type == ISN_STOREEXPORT)
			{
			    ಪೂರ್ಣಾಂಕ idx = get_script_ಅಂಶ_idx(sid, name, 0,
								   ಖಾಲಿ, ಖಾಲಿ);

			    // can this ever fail?
			    ಆದರೆ (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ಅಂಶ(sid)
						  ->sn_var_vals.ga_ದತ್ತಾಂಶ) + idx;

				ಆದರೆ ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_ಅಂಶ_not_exported_ಒಳ_script_str),
									 name);
				    clear_tv(ರಾಶಿ_TV_BOT(0));
				    ಹೋಗು on_error;
				}
			    }
			}
			ಆದರೆ (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(ರಾಶಿ_TV_BOT(0));
			    ಹೋಗು on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *ರಾಶಿ_TV_BOT(0);
		    }
		}
		ನಿಲ್ಲಿಸು;

	    // store script-local variable in Vim9 script
	    ಸ್ಥಿತಿ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ಆದರೆ (sv == ಖಾಲಿ)
			ಹೋಗು theend;
		    --ectx->ec_ರಾಶಿ.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(sv->sv_tv->v_ಬೀಗ, sv->sv_name, FALSE))
		    {
			clear_tv(ರಾಶಿ_TV_BOT(0));
			ಹೋಗು on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *ರಾಶಿ_TV_BOT(0);
		}
		ನಿಲ್ಲಿಸು;

	    // store option
	    ಸ್ಥಿತಿ ISN_STOREOPT:
	    ಸ್ಥಿತಿ ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    ಪೂರ್ಣಾಂಕ		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    ದೀರ್ಘ	n = 0;
		    char_u	*s = ಖಾಲಿ;
		    ಅಕ್ಷರ	*ಸಂದೇಶ;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = ಖಾಲಿ;

		    --ectx->ec_ರಾಶಿ.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ರಾಶಿ_TV_BOT(0);
		    ಆದರೆ (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			ಆದರೆ (s == ಖಾಲಿ)
			    s = (char_u *)"";
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			ಆದರೆ (s == ಖಾಲಿ || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    ಹೋಗು on_error;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_ಸಂಖ್ಯೆ;
		    ಸಂದೇಶ = ಗಣ_option_ಮೌಲ್ಯ(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    ಆದರೆ (ಸಂದೇಶ != ಖಾಲಿ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(ಸಂದೇಶ));
			ಹೋಗು on_error;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    // store $ENV
	    ಸ್ಥಿತಿ ISN_STOREENV:
		ಆದರೆ (check_secure() || check_restricted())
		    ಹೋಗು theend;
		--ectx->ec_ರಾಶಿ.ga_len;
		tv = ರಾಶಿ_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		ನಿಲ್ಲಿಸು;

	    // store @r
	    ಸ್ಥಿತಿ ISN_STOREREG:
		{
		    ಪೂರ್ಣಾಂಕ	reg = iptr->isn_arg.ಸಂಖ್ಯೆ;

		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv = ರಾಶಿ_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		ನಿಲ್ಲಿಸು;

	    // store v: variable
	    ಸ್ಥಿತಿ ISN_STOREV:
		--ectx->ec_ರಾಶಿ.ga_len;
		ಆದರೆ (ಗಣ_vim_var_tv(iptr->isn_arg.ಸಂಖ್ಯೆ, ರಾಶಿ_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;

	    // store g:/b:/w:/t: variable
	    ಸ್ಥಿತಿ ISN_STOREG:
	    ಸ್ಥಿತಿ ISN_STOREB:
	    ಸ್ಥಿತಿ ISN_STOREW:
	    ಸ್ಥಿತಿ ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    ಆಯ್ಕೆ (iptr->isn_type)
		    {
			ಸ್ಥಿತಿ ISN_STOREG:
			    ht = get_globvar_ht();
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:  // Cannot reach here
			    ಹೋಗು theend;
		    }

		    --ectx->ec_ರಾಶಿ.ga_len;
		    di = ಕಂಡುಹಿಡಿ_var_ಒಳ_ht(ht, 0, name, TRUE);
		    ಆದರೆ (di == ಖಾಲಿ)
			store_var(iptr->isn_arg.string, ರಾಶಿ_TV_BOT(0));
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ಆದರೆ (var_check_permission(di, name) == FAIL)
			    ಹೋಗು on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *ರಾಶಿ_TV_BOT(0);
		    }
		}
		ನಿಲ್ಲಿಸು;

	    // store an autoload variable
	    ಸ್ಥಿತಿ ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		ಗಣ_var(iptr->isn_arg.string, ರಾಶಿ_TV_BOT(-1), TRUE);
		clear_tv(ರಾಶಿ_TV_BOT(-1));
		--ectx->ec_ರಾಶಿ.ga_len;
		ನಿಲ್ಲಿಸು;

	    // store number in local variable
	    ಸ್ಥಿತಿ ISN_STORENR:
		tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_ಸಂಖ್ಯೆ;
		tv->vval.v_ಸಂಖ್ಯೆ = iptr->isn_arg.storenr.stnr_val;
		ನಿಲ್ಲಿಸು;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    ಸ್ಥಿತಿ ISN_STOREINDEX:
		{
		    ಪೂರ್ಣಾಂಕ res = execute_storeindex(iptr, ectx);

		    ಆದರೆ (res == FAIL)
			ಹೋಗು on_error;
		    ಆದರೆ (res == NOTDONE)
			ಹೋಗು theend;
		}
		ನಿಲ್ಲಿಸು;

	    // store value in list or blob range
	    ಸ್ಥಿತಿ ISN_STORERANGE:
		ಆದರೆ (execute_storerange(iptr, ectx) == FAIL)
		    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ತುಂಬು_CLASSMEMBER:
		{
		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      ರಾಶಿ_TV_BOT(0));
		    ++ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *ರಾಶಿ_TV_BOT(-1);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    // Load or store variable or argument from outer scope.
	    ಸ್ಥಿತಿ ISN_LOADOUTER:
	    ಸ್ಥಿತಿ ISN_STOREOUTER:
		{
		    ಪೂರ್ಣಾಂಕ		ಆಳ = iptr->isn_arg.outer.outer_ಆಳ;
		    outer_T	*outer = ectx->ec_outer_ref == ಖಾಲಿ ? ಖಾಲಿ
						: ectx->ec_outer_ref->or_outer;

		    ಆಗ (ಆಳ > 1 && outer != ಖಾಲಿ)
		    {
			outer = outer->ಹೊರಹೋಗು_ಮೇಲೆ;
			--ಆಳ;
		    }
		    ಆದರೆ (outer == ಖಾಲಿ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ಆದರೆ (ectx->ec_ಚೌಕಟ್ಟು_idx == ectx->ec_initial_ಚೌಕಟ್ಟು_idx
						 || ectx->ec_outer_ref == ಖಾಲಿ)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_ಆವರಣ_called_from_invalid_context));
			ಇಲ್ಲದಿದ್ದರೆ
			    iemsg("LOADOUTER depth more than scope levels");
			ಹೋಗು theend;
		    }
		    ಆದರೆ (ಆಳ < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->ಹೊರಹೋಗು_loop[-ಆಳ - 1]
							       .ರಾಶಿ->ga_ದತ್ತಾಂಶ)
					  + outer->ಹೊರಹೋಗು_loop[-ಆಳ - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    ಇಲ್ಲದಿದ್ದರೆ
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->ಹೊರಹೋಗು_ರಾಶಿ->ga_ದತ್ತಾಂಶ)
				      + outer->ಹೊರಹೋಗು_ಚೌಕಟ್ಟು_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ
				      + iptr->isn_arg.outer.outer_idx;
		    ಆದರೆ (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			    ಹೋಗು theend;
			// careful: ga_grow_inner may re-alloc the stack
			ಆದರೆ (ಆಳ < 0)
			    copy = ((typval_T *)outer->ಹೊರಹೋಗು_loop[-ಆಳ - 1]
								   .ರಾಶಿ->ga_ದತ್ತಾಂಶ)
					      + outer->ಹೊರಹೋಗು_loop[-ಆಳ - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			ಇಲ್ಲದಿದ್ದರೆ
			    copy = ((typval_T *)outer->ಹೊರಹೋಗು_ರಾಶಿ->ga_ದತ್ತಾಂಶ)
					  + outer->ಹೊರಹೋಗು_ಚೌಕಟ್ಟು_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			ಆದರೆ (copy != tv)
			    tv = copy;
			copy_tv(tv, ರಾಶಿ_TV_BOT(0));
			++ectx->ec_ರಾಶಿ.ga_len;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			--ectx->ec_ರಾಶಿ.ga_len;
			clear_tv(tv);
			*tv = *ರಾಶಿ_TV_BOT(0);
		    }
		}
		ನಿಲ್ಲಿಸು;

	    // unlet item in list or dict variable
	    ಸ್ಥಿತಿ ISN_UNLETINDEX:
		ಆದರೆ (execute_unletindex(iptr, ectx) == FAIL)
		    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;

	    // unlet range of items in list variable
	    ಸ್ಥಿತಿ ISN_UNLETRANGE:
		ಆದರೆ (execute_unletrange(iptr, ectx) == FAIL)
		    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;

	    // push constant
	    ಸ್ಥಿತಿ ISN_PUSHNR:
	    ಸ್ಥಿತಿ ISN_PUSHBOOL:
	    ಸ್ಥಿತಿ ISN_PUSHSPEC:
	    ಸ್ಥಿತಿ ISN_PUSHF:
	    ಸ್ಥಿತಿ ISN_PUSHS:
	    ಸ್ಥಿತಿ ISN_PUSHBLOB:
	    ಸ್ಥಿತಿ ISN_PUSHFUNC:
	    ಸ್ಥಿತಿ ISN_PUSHCHANNEL:
	    ಸ್ಥಿತಿ ISN_PUSHJOB:
	    ಸ್ಥಿತಿ ISN_PUSHOBJ:
	    ಸ್ಥಿತಿ ISN_PUSHCLASS:
		ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಹೋಗು theend;
		tv = ರಾಶಿ_TV_BOT(0);
		tv->v_ಬೀಗ = 0;
		++ectx->ec_ರಾಶಿ.ga_len;
		ಆಯ್ಕೆ (iptr->isn_type)
		{
		    ಸ್ಥಿತಿ ISN_PUSHNR:
			tv->v_type = VAR_ಸಂಖ್ಯೆ;
			tv->vval.v_ಸಂಖ್ಯೆ = iptr->isn_arg.ಸಂಖ್ಯೆ;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_ಸಂಖ್ಯೆ = iptr->isn_arg.ಸಂಖ್ಯೆ;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_ಸಂಖ್ಯೆ = iptr->isn_arg.ಸಂಖ್ಯೆ;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			ಆದರೆ (iptr->isn_arg.string == ಖಾಲಿ)
			    tv->vval.v_string = ಖಾಲಿ;
			ಇಲ್ಲದಿದ್ದರೆ
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ISN_PUSHCHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ISN_PUSHJOB:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
			tv->v_type = VAR_ಕೆಲಸ;
			tv->vval.v_ಕೆಲಸ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = ಖಾಲಿ;
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			ನಿಲ್ಲಿಸು;
		    ಪೂರ್ವನಿಯೋಜಿತ:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == ಖಾಲಿ
				    ? ಖಾಲಿ : vim_strsave(iptr->isn_arg.string);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (ಶೂನ್ಯ)script_autoload(name, FALSE);
		    ಆದರೆ (ಕಂಡುಹಿಡಿ_func(name, TRUE))
		    {
			ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			    ಹೋಗು theend;
			tv = ರಾಶಿ_TV_BOT(0);
			tv->v_ಬೀಗ = 0;
			++ectx->ec_ರಾಶಿ.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಪೂರ್ಣಾಂಕ res = ತುಂಬು_namespace_var(ectx, ISN_LOADG, iptr);

			ಆದರೆ (res == NOTDONE)
			    ಹೋಗು theend;
			ಆದರೆ (res == FAIL)
			    ಹೋಗು on_error;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_UNLET:
		ಆದರೆ (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_LOCKUNLOCK:
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LOG_LOCKVAR
		    ch_log(ಖಾಲಿ, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#ಆದರೆ_ಅಂತ್ಯ
		    lval_ಬೇರು_T	*lval_ಬೇರು_save = lval_ಬೇರು;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_ರಾಶಿ.ga_len;
		    lval_ಬೇರು_T ಬೇರು;
		    ಬೇರು.lr_tv      = ರಾಶಿ_TV_BOT(0);
		    ಬೇರು.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    ಬೇರು.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_ಬೇರು = &ಬೇರು;
		    ಪೂರ್ಣಾಂಕ res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(ಬೇರು.lr_tv);
		    lval_ಬೇರು = lval_ಬೇರು_save;
		    ಆದರೆ (res == FAIL)
			ಹೋಗು on_error;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_LOCKCONST:
		ಅಂಶ_ಬೀಗ(ರಾಶಿ_TV_BOT(-1), 100, TRUE, TRUE);
		ನಿಲ್ಲಿಸು;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    ಸ್ಥಿತಿ ISN_NEWLIST:
		ಆದರೆ (exe_newlist(iptr->isn_arg.ಸಂಖ್ಯೆ, ectx) == FAIL)
		    ಹೋಗು theend;
		ನಿಲ್ಲಿಸು;

	    // create a tuple from items on the stack
	    ಸ್ಥಿತಿ ISN_NEWTUPLE:
		ಆದರೆ (exe_newtuple(iptr->isn_arg.ಸಂಖ್ಯೆ, ectx) == FAIL)
		    ಹೋಗು theend;
		ನಿಲ್ಲಿಸು;

	    // create a dict from items on the stack
	    ಸ್ಥಿತಿ ISN_NEWDICT:
		{
		    ಪೂರ್ಣಾಂಕ res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.ಸಂಖ್ಯೆ, ectx);
		    ಆದರೆ (res == FAIL)
			ಹೋಗು theend;
		    ಆದರೆ (res == MAYBE)
			ಹೋಗು on_error;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CONCAT:
		ಆದರೆ (exe_concat(iptr->isn_arg.ಸಂಖ್ಯೆ, ectx) == FAIL)
		    ಹೋಗು theend;
		ನಿಲ್ಲಿಸು;

	    // create a partial with NULL value
	    ಸ್ಥಿತಿ ISN_NEWPARTIAL:
		ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಹೋಗು theend;
		++ectx->ec_ರಾಶಿ.ga_len;
		tv = ರಾಶಿ_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_ಬೀಗ = 0;
		tv->vval.v_partial = ಖಾಲಿ;
		ನಿಲ್ಲಿಸು;

	    // call a :def function
	    ಸ್ಥಿತಿ ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ಆದರೆ (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				ಖಾಲಿ,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;

	    // call a method on an interface
	    ಸ್ಥಿತಿ ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ರಾಶಿ_TV_BOT(-1 - mfunc->cmf_argcount);
		    ಆದರೆ (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			ಹೋಗು on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ಆದರೆ (obj == ಖಾಲಿ)
		    {
			emsg(_(e_using_null_object));
			ಹೋಗು on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    ಆದರೆ (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			ಪೂರ್ಣಾಂಕ idx = object_ಸೂಚ್ಯಂಕ_from_itf_ಸೂಚ್ಯಂಕ(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    ಆದರೆ (mfunc->cmf_ufunc != ಖಾಲಿ && IS_GENERIC_FUNC(ufunc))
		    {
			ಆದರೆ (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    ಆದರೆ (ufunc == ಖಾಲಿ)
				ಹೋಗು on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    ಆದರೆ (call_ufunc(ufunc, ಖಾಲಿ, mfunc->cmf_argcount, ectx,
							   ಖಾಲಿ, ಖಾಲಿ) == FAIL)
			ಹೋಗು on_error;
		}
		ನಿಲ್ಲಿಸು;

	    // call a builtin function
	    ಸ್ಥಿತಿ ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ಆದರೆ (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;

	    // call a funcref or partial
	    ಸ್ಥಿತಿ ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    ಪೂರ್ಣಾಂಕ		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (pfunc->cpf_ತುದಿ)
		    {
			// funcref is above the arguments
			tv = ರಾಶಿ_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// Get the funcref from the stack.
			--ectx->ec_ರಾಶಿ.ga_len;
			partial_tv = *ರಾಶಿ_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    ಆದರೆ (tv == &partial_tv)
			clear_tv(&partial_tv);
		    ಆದರೆ (r == FAIL)
			ಹೋಗು on_error;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_PCALL_ಕೊನೆ:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_ರಾಶಿ.ga_len;
		clear_tv(ರಾಶಿ_TV_BOT(-1));
		*ರಾಶಿ_TV_BOT(-1) = *ರಾಶಿ_TV_BOT(0);
		ನಿಲ್ಲಿಸು;

	    // call a user defined function or funcref/partial
	    ಸ್ಥಿತಿ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			ಹೋಗು on_error;
		}
		ನಿಲ್ಲಿಸು;

	    // :defer func(arg)
	    ಸ್ಥಿತಿ ISN_DEFER:
		ಆದರೆ (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    ಸ್ಥಿತಿ ISN_RETURN_VOID:
	    ಸ್ಥಿತಿ ISN_RETURN_OBJECT:
		ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಹೋಗು theend;
		tv = ರಾಶಿ_TV_BOT(0);
		++ectx->ec_ರಾಶಿ.ga_len;
		ಆದರೆ (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_ಸಂಖ್ಯೆ = 0;
		    tv->v_ಬೀಗ = 0;
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    *tv = *ರಾಶಿ_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_ಬೀಗ_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    ಸ್ಥಿತಿ ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ಖಾಲಿ;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(ರಾಶಿ_TV_BOT(-1)) == FAIL)
			ಹೋಗು theend;

		    ಆದರೆ (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_ದತ್ತಾಂಶ)
							+ trystack->ga_len - 1;
		    ಆದರೆ (trycmd != ಖಾಲಿ
				 && trycmd->tcd_ಚೌಕಟ್ಟು_idx == ectx->ec_ಚೌಕಟ್ಟು_idx)
		    {
			// jump to ":finally" or ":endtry"
			ಆದರೆ (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			ಇಲ್ಲದಿದ್ದರೆ
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ಹೋಗು func_return;
		}
		ನಿಲ್ಲಿಸು;

	    // push a partial, a reference to a compiled function
	    ಸ್ಥಿತಿ ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    ಆದರೆ (pt == ಖಾಲಿ)
			ಹೋಗು theend;
		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    {
			vim_free(pt);
			ಹೋಗು theend;
		    }
		    ಆದರೆ (extra != ಖಾಲಿ && extra->fre_class != ಖಾಲಿ)
		    {
			class_T	*cl;
			ಆದರೆ (extra->fre_object_ವಿಧಾನ)
			{
			    tv = ರಾಶಿ_TV_BOT(-1);
			    ಆದರೆ (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				ಹೋಗು on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    ಆದರೆ (obj == ಖಾಲಿ)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				ಹೋಗು on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ರಾಶಿ.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    cl = extra->fre_class;

			ಆದರೆ (extra->fre_object_ವಿಧಾನ)
			{
			    // object method
			    // convert the interface index to the object index
			    ಪೂರ್ಣಾಂಕ idx =
				object_ಸೂಚ್ಯಂಕ_from_itf_ಸೂಚ್ಯಂಕ(extra->fre_class,
					TRUE, extra->fre_ವಿಧಾನ_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_ವಿಧಾನ_idx];
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra == ಖಾಲಿ || extra->fre_func_name == ಖಾಲಿ)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ufunc = ಕಂಡುಹಿಡಿ_func(extra->fre_func_name, FALSE);
		    }
		    ಆದರೆ (ufunc == ಖಾಲಿ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			ಹೋಗು theend;
		    }
		    ಆದರೆ (extra != ಖಾಲಿ && extra->fre_outer_ಆಳ > 0
					   && (ufunc->uf_flags & FC_ಆವರಣ))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == ಖಾಲಿ
				    ? ಖಾಲಿ : ectx->ec_outer_ref->or_partial;

			ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಆಳ = 1; outer_pt != ಖಾಲಿ
				    && ಆಳ < extra->fre_outer_ಆಳ; ++ಆಳ)
			    outer_pt = get_pt_outer(outer_pt)->ಹೊರಹೋಗು_ಮೇಲೆ_partial;
			ಆದರೆ (outer_pt == ಖಾಲಿ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    ಹೋಗು theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fill_partial_and_ಆವರಣ(pt, ufunc,
			       extra == ಖಾಲಿ ? ಖಾಲಿ : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			ಹೋಗು theend;
		    }
		    tv = ರಾಶಿ_TV_BOT(0);
		    ++ectx->ec_ರಾಶಿ.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_ಬೀಗ = 0;
		}
		ನಿಲ್ಲಿಸು;

	    // Create a global function from a lambda.
	    ಸ್ಥಿತಿ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ಆದರೆ (copy_lambda_ಗಮ್ಯ_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			ಹೋಗು theend;
		}
		ನಿಲ್ಲಿಸು;

	    // List functions
	    ಸ್ಥಿತಿ ISN_DEF:
		ಆದರೆ (iptr->isn_arg.string == ಖಾಲಿ)
		    list_functions(ಖಾಲಿ);
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    exarg_T	ea;
		    garray_T	ಸಾಲುಗಳು_ಗಮ್ಯ_free;

		    CLEAR_ಕ್ಷೇತ್ರ(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&ಸಾಲುಗಳು_ಗಮ್ಯ_free, ಗಾತ್ರ(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, ಖಾಲಿ, &ಸಾಲುಗಳು_ಗಮ್ಯ_free, 0, ಖಾಲಿ, 0, ಖಾಲಿ);
		    ga_clear_ಅಕ್ಷರಸರಣಿಗಳು(&ಸಾಲುಗಳು_ಗಮ್ಯ_free);
		}
		ನಿಲ್ಲಿಸು;

	    // jump if a condition is met
	    ಸ್ಥಿತಿ ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    ಪೂರ್ಣಾಂಕ		error = FALSE;
		    ಪೂರ್ಣಾಂಕ		jump = TRUE;

		    ಆದರೆ (when != JUMP_ALWAYS)
		    {
			tv = ರಾಶಿ_TV_BOT(-1);
			ಆದರೆ (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    ಆದರೆ (error)
				ಹೋಗು on_error;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			    jump = tv2bool(tv);
			ಆದರೆ (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			ಆದರೆ (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ರಾಶಿ.ga_len;
			}
		    }
		    ಆದರೆ (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		ನಿಲ್ಲಿಸು;

	    // "while": jump to end if a condition is false
	    ಸ್ಥಿತಿ ISN_WHILE:
		{
		    ಪೂರ್ಣಾಂಕ		error = FALSE;
		    ಪೂರ್ಣಾಂಕ		jump = TRUE;

		    tv = ರಾಶಿ_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    ಆದರೆ (error)
			ಹೋಗು on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_ರಾಶಿ.ga_len;
		    ಆದರೆ (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_ಕೊನೆ;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = ರಾಶಿ_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_ಸಂಖ್ಯೆ = ectx->ec_funcrefs.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    ಸ್ಥಿತಿ ISN_JUMP_IF_ARG_ಗಣ:
	    ಸ್ಥಿತಿ ISN_JUMP_IF_ARG_NOT_ಗಣ:
		tv = ರಾಶಿ_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		ಪೂರ್ಣಾಂಕ arg_ಗಣ = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE);

		ಆದರೆ (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_ಗಣ && !arg_ಗಣ)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_ವಾದಗಳು.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_ಸೂಚಕ(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    ಆದರೆ (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		ಆದರೆ (iptr->isn_type == ISN_JUMP_IF_ARG_ಗಣ ? arg_ಗಣ : !arg_ಗಣ)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		ನಿಲ್ಲಿಸು;

	    // top of a for loop
	    ಸ್ಥಿತಿ ISN_FOR:
		ಆದರೆ (execute_for(iptr, ectx) == FAIL)
		    ಹೋಗು theend;
		ನಿಲ್ಲಿಸು;

	    // end of a for or while loop
	    ಸ್ಥಿತಿ ISN_ENDLOOP:
		ಆದರೆ (execute_endloop(iptr, ectx) == FAIL)
		    ಹೋಗು theend;
		ನಿಲ್ಲಿಸು;

	    // start of ":try" block
	    ಸ್ಥಿತಿ ISN_TRY:
		{
		    trycmd_T    *trycmd = ಖಾಲಿ;

		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			ಹೋಗು theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_ದತ್ತಾಂಶ)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_ಸೂಚಕ(trycmd);
		    trycmd->tcd_ಚೌಕಟ್ಟು_idx = ectx->ec_ಚೌಕಟ್ಟು_idx;
		    trycmd->tcd_ರಾಶಿ_len = ectx->ec_ರಾಶಿ.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_PUSHEXC:
		ಆದರೆ (current_exception == ಖಾಲಿ)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    ಹೋಗು theend;
		}
		ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
		    ಹೋಗು theend;
		tv = ರಾಶಿ_TV_BOT(0);
		++ectx->ec_ರಾಶಿ.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_ಬೀಗ = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->ಮೌಲ್ಯ);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CATCH:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    may_restore_cmdmod(&ectx->ec_funclocal);
		    trycmd = ((trycmd_T *)trystack->ga_ದತ್ತಾಂಶ)
							+ trystack->ga_len - 1;
		    trycmd->tcd_caught = TRUE;
		    trycmd->tcd_did_throw = FALSE;

		    did_emsg = got_int = did_throw = FALSE;
		    force_abort = need_rethrow = FALSE;
		    catch_exception(current_exception);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    ಪೂರ್ಣಾಂಕ		i;
		    trycmd_T    *trycmd;
		    ಪೂರ್ಣಾಂಕ		iidx = trycont->tct_where;

		    ಆದರೆ (trystack->ga_len < trycont->tct_ಮಟ್ಟಗಳು)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_ಮಟ್ಟಗಳು, trystack->ga_len);
			ಹೋಗು theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    ಗಾಗಿ (i = trycont->tct_ಮಟ್ಟಗಳು; i > 0; --i)
		    {
			trycmd = ((trycmd_T *)trystack->ga_ದತ್ತಾಂಶ)
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
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_ದತ್ತಾಂಶ)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    ಆದರೆ (trycmd->tcd_caught)
		    {
			// discard the exception
			ಮುಕ್ತಾಯ_exception(caught_ರಾಶಿ);
			trycmd->tcd_caught = FALSE;
		    }
		    ನಿಲ್ಲಿಸು;
		}

	    // end of ":try" block
	    ಸ್ಥಿತಿ ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_ದತ್ತಾಂಶ) + trystack->ga_len;
		    ಆದರೆ (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    ಆದರೆ (trycmd->tcd_caught)
		    {
			// discard the exception
			ಮುಕ್ತಾಯ_exception(caught_ರಾಶಿ);
		    }

		    ಆದರೆ (trycmd->tcd_return)
			ಹೋಗು func_return;

		    ಆಗ (ectx->ec_ರಾಶಿ.ga_len > trycmd->tcd_ರಾಶಿ_len)
		    {
			--ectx->ec_ರಾಶಿ.ga_len;
			clear_tv(ರಾಶಿ_TV_BOT(0));
		    }
		    ಆದರೆ (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    ಆದರೆ (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = ರಾಶಿ_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_ಸಂಖ್ಯೆ;
			tv->vval.v_ಸಂಖ್ಯೆ = 0;
			ಹೋಗು ಮುಗಿದ;
		    }
		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv = ರಾಶಿ_TV_BOT(0);
		    ಆದರೆ (tv->vval.v_string == ಖಾಲಿ
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_ಖಾಲಿಗಳು_string));
			ಹೋಗು theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    ಆದರೆ (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_ದತ್ತಾಂಶ)
							+ trystack->ga_len - 1;
			ಆದರೆ (trycmd->tcd_caught)
			{
			    // discard the exception
			    ಮುಕ್ತಾಯ_exception(caught_ರಾಶಿ);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (throw_exception(tv->vval.v_string, ET_USER, ಖಾಲಿ)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			ಹೋಗು theend;
		    }
		    did_throw = TRUE;
		}
		ನಿಲ್ಲಿಸು;

	    // compare with special values
	    ಸ್ಥಿತಿ ISN_COMPAREBOOL:
	    ಸ್ಥಿತಿ ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_ಸಂಖ್ಯೆ;
		    varnumber_T arg2 = tv2->vval.v_ಸಂಖ್ಯೆ;
		    ಪೂರ್ಣಾಂಕ		res;

		    ಆದರೆ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_EQUAL)
			res = arg1 == arg2;
		    ಇಲ್ಲದಿದ್ದರೆ
			res = arg1 != arg2;

		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_COMPARENULL:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    ಪೂರ್ಣಾಂಕ		res;

		    res = typval_compare_null(tv1, tv2);
		    ಆದರೆ (res == MAYBE)
			ಹೋಗು on_error;
		    ಆದರೆ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_ರಾಶಿ.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ನಿಲ್ಲಿಸು;

	    // Operation with two number arguments
	    ಸ್ಥಿತಿ ISN_OPNR:
	    ಸ್ಥಿತಿ ISN_COMPARENR:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_ಸಂಖ್ಯೆ;
		    varnumber_T arg2 = tv2->vval.v_ಸಂಖ್ಯೆ;
		    varnumber_T res = 0;
		    ಪೂರ್ಣಾಂಕ		div_zero = FALSE;

		    ಆದರೆ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_LSHIFT
			    || iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_RSHIFT)
		    {
			ಆದರೆ (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_ಧನಾತ್ಮಕ));
			    ಹೋಗು on_error;
			}
		    }

		    ಆಯ್ಕೆ (iptr->isn_arg.op.op_type)
		    {
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_MULT: res = arg1 * arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_DIV:  ಆದರೆ (arg2 == 0)
					    div_zero = TRUE;
					ಇಲ್ಲದಿದ್ದರೆ
					    res = arg1 / arg2;
					ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_REM:  ಆದರೆ (arg2 == 0)
					    div_zero = TRUE;
					ಇಲ್ಲದಿದ್ದರೆ
					    res = arg1 % arg2;
					ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SUB: res = arg1 - arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ಕೂಡಿಸು: res = arg1 + arg2; ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_EQUAL: res = arg1 == arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL: res = arg1 != arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GREATER: res = arg1 > arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL: res = arg1 >= arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SMALLER: res = arg1 < arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL: res = arg1 <= arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_LSHIFT: ಆದರೆ (arg2 > MAX_LSHIFT_ಬಿಟ್ಟುಗಳು)
					      res = 0;
					  ಇಲ್ಲದಿದ್ದರೆ
					      res = (uvarnumber_T)arg1 << arg2;
					  ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_RSHIFT: ಆದರೆ (arg2 > MAX_LSHIFT_ಬಿಟ್ಟುಗಳು)
					      res = 0;
					  ಇಲ್ಲದಿದ್ದರೆ
					      res = (uvarnumber_T)arg1 >> arg2;
					  ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ: ನಿಲ್ಲಿಸು;
		    }

		    --ectx->ec_ರಾಶಿ.ga_len;
		    ಆದರೆ (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			tv1->vval.v_ಸಂಖ್ಯೆ = res;
		    ಆದರೆ (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			ಹೋಗು on_error;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    // Computation with two float arguments
	    ಸ್ಥಿತಿ ISN_OPFLOAT:
	    ಸ್ಥಿತಿ ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    ಪೂರ್ಣಾಂಕ		cmp = FALSE;

		    ಆಯ್ಕೆ (iptr->isn_arg.op.op_type)
		    {
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_MULT: res = arg1 * arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_DIV: res = arg1 / arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SUB: res = arg1 - arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ಕೂಡಿಸು: res = arg1 + arg2; ನಿಲ್ಲಿಸು;

			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_EQUAL: cmp = arg1 == arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL: cmp = arg1 != arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GREATER: cmp = arg1 > arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL: cmp = arg1 >= arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SMALLER: cmp = arg1 < arg2; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL: cmp = arg1 <= arg2; ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ: cmp = 0; ನಿಲ್ಲಿಸು;
		    }
		    --ectx->ec_ರಾಶಿ.ga_len;
		    ಆದರೆ (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_ಸಂಖ್ಯೆ = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			tv1->vval.v_float = res;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_COMPARELIST:
	    ಸ್ಥಿತಿ ISN_COMPARETUPLE:
	    ಸ್ಥಿತಿ ISN_COMPAREDICT:
	    ಸ್ಥಿತಿ ISN_COMPAREFUNC:
	    ಸ್ಥಿತಿ ISN_COMPARESTRING:
	    ಸ್ಥಿತಿ ISN_COMPAREBLOB:
	    ಸ್ಥಿತಿ ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    ಪೂರ್ಣಾಂಕ		ic = iptr->isn_arg.op.op_ic;
		    ಪೂರ್ಣಾಂಕ		res = FALSE;
		    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (iptr->isn_type == ISN_COMPARELIST)
		    {
			ಸ್ಥಿತಿ_2 = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			ಸ್ಥಿತಿ_2 = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_type == ISN_COMPAREDICT)
		    {
			ಸ್ಥಿತಿ_2 = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			ಸ್ಥಿತಿ_2 = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_type == ISN_COMPARESTRING)
		    {
			ಸ್ಥಿತಿ_2 = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			ಸ್ಥಿತಿ_2 = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ // ISN_COMPAREOBJECT
		    {
			ಸ್ಥಿತಿ_2 = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_ರಾಶಿ.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ಸಂಖ್ಯೆ = res ? VVAL_TRUE : VVAL_FALSE;
		    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
			ಹೋಗು theend;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_COMPAREANY:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    ಪೂರ್ಣಾಂಕ		ic = iptr->isn_arg.op.op_ic;
		    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ಸ್ಥಿತಿ_2 = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_ರಾಶಿ.ga_len;
		    ಆದರೆ (ಸ್ಥಿತಿ_2 == FAIL)
			ಹೋಗು theend;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ADDLIST:
	    ಸ್ಥಿತಿ ISN_ADDTUPLE:
	    ಸ್ಥಿತಿ ISN_ADDBLOB:
		{
		    typval_T *tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T *tv2 = ರಾಶಿ_TV_BOT(-1);

		    // add two lists or blobs
		    ಆದರೆ (iptr->isn_type == ISN_ADDLIST)
		    {
			ಆದರೆ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_APPEND
						   && tv1->vval.v_list != ಖಾಲಿ)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 ಖಾಲಿ);
			ಇಲ್ಲದಿದ್ದರೆ
			    eval_addlist(tv1, tv2);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    ಇಲ್ಲದಿದ್ದರೆ
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (l == ಖಾಲಿ)
		    {
			emsg(_(e_cannot_ಕೂಡಿಸು_ಗಮ್ಯ_null_list));
			ಹೋಗು on_error;
		    }
		    ಆದರೆ (ಮೌಲ್ಯ_check_ಬೀಗ(l->lv_ಬೀಗ, ಖಾಲಿ, FALSE))
			ಹೋಗು on_error;
		    ಆದರೆ (list_append_tv(l, tv2) == FAIL)
			ಹೋಗು theend;
		    clear_tv(tv2);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    ಪೂರ್ಣಾಂಕ		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    ಆದರೆ (b == ಖಾಲಿ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_ಕೂಡಿಸು_ಗಮ್ಯ_null_blob));
			ಹೋಗು on_error;
		    }
		    n = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
		    ಆದರೆ (error)
			ಹೋಗು on_error;
		    ga_append(&b->bv_ga, (ಪೂರ್ಣಾಂಕ)n);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    // Computation with two arguments of unknown type
	    ಸ್ಥಿತಿ ISN_OPANY:
		{
		    typval_T	*tv1 = ರಾಶಿ_TV_BOT(-2);
		    typval_T	*tv2 = ರಾಶಿ_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    ಪೂರ್ಣಾಂಕ		error = FALSE;

		    ಆದರೆ (iptr->isn_arg.op.op_type == ಅಭಿವ್ಯಕ್ತಿ_ಕೂಡಿಸು)
		    {
			ಆದರೆ (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ರಾಶಿ.ga_len;
			    ನಿಲ್ಲಿಸು;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ರಾಶಿ.ga_len;
			    ನಿಲ್ಲಿಸು;
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ರಾಶಿ.ga_len;
			    ನಿಲ್ಲಿಸು;
			}
		    }
		    ಆದರೆ (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_ಸಂಖ್ಯೆ_chk(tv1, &error);
			ಆದರೆ (error)
			    ಹೋಗು on_error;
			ಆದರೆ (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    ಆದರೆ (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			n2 = tv_get_ಸಂಖ್ಯೆ_chk(tv2, &error);
			ಆದರೆ (error)
			    ಹೋಗು on_error;
			ಆದರೆ (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    ಆದರೆ (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			ಆಯ್ಕೆ (iptr->isn_arg.op.op_type)
			{
			    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_MULT: f1 = f1 * f2; ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_DIV:  f1 = f1 / f2; ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SUB:  f1 = f1 - f2; ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ಕೂಡಿಸು:  f1 = f1 + f2; ನಿಲ್ಲಿಸು;
			    ಪೂರ್ವನಿಯೋಜಿತ: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     ಹೋಗು on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_ರಾಶಿ.ga_len;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಪೂರ್ಣಾಂಕ failed = FALSE;

			ಆಯ್ಕೆ (iptr->isn_arg.op.op_type)
			{
			    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_MULT: n1 = n1 * n2; ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_DIV:  n1 = ಅಂಕೆ_divide(n1, n2, &failed);
					    ಆದರೆ (failed)
						ಹೋಗು on_error;
					    ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SUB:  n1 = n1 - n2; ನಿಲ್ಲಿಸು;
			    ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ಕೂಡಿಸು:  n1 = n1 + n2; ನಿಲ್ಲಿಸು;
			    ಪೂರ್ವನಿಯೋಜಿತ:	    n1 = ಅಂಕೆ_modulus(n1, n2, &failed);
					    ಆದರೆ (failed)
						ಹೋಗು on_error;
					    ನಿಲ್ಲಿಸು;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_ಸಂಖ್ಯೆ;
			tv1->vval.v_ಸಂಖ್ಯೆ = n1;
			--ectx->ec_ರಾಶಿ.ga_len;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_STRINDEX:
	    ಸ್ಥಿತಿ ISN_STRSLICE:
		{
		    ಪೂರ್ಣಾಂಕ		is_ತುಂಡು = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    ಆದರೆ (is_ತುಂಡು)
		    {
			tv = ರಾಶಿ_TV_BOT(-2);
			n1 = tv->vval.v_ಸಂಖ್ಯೆ;
		    }

		    tv = ರಾಶಿ_TV_BOT(-1);
		    n2 = tv->vval.v_ಸಂಖ್ಯೆ;

		    ectx->ec_ರಾಶಿ.ga_len -= is_ತುಂಡು ? 2 : 1;
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಆದರೆ (is_ತುಂಡು)
			// Slice: Select the characters from the string
			res = string_ತುಂಡು(tv->vval.v_string, n1, n2, FALSE);
		    ಇಲ್ಲದಿದ್ದರೆ
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_LISTINDEX:
	    ಸ್ಥಿತಿ ISN_LISTSLICE:
	    ಸ್ಥಿತಿ ISN_TUPLEINDEX:
	    ಸ್ಥಿತಿ ISN_TUPLESLICE:
	    ಸ್ಥಿತಿ ISN_BLOBINDEX:
	    ಸ್ಥಿತಿ ISN_BLOBSLICE:
		{
		    ಪೂರ್ಣಾಂಕ		is_ತುಂಡು = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    ಪೂರ್ಣಾಂಕ		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    ಪೂರ್ಣಾಂಕ		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_ತುಂಡು ? ರಾಶಿ_TV_BOT(-3) : ರಾಶಿ_TV_BOT(-2);

		    tv = ರಾಶಿ_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_ಸಂಖ್ಯೆ;
		    clear_tv(tv);

		    ಆದರೆ (is_ತುಂಡು)
		    {
			tv = ರಾಶಿ_TV_BOT(-2);
			n1 = tv->vval.v_ಸಂಖ್ಯೆ;
			clear_tv(tv);
		    }

		    ectx->ec_ರಾಶಿ.ga_len -= is_ತುಂಡು ? 2 : 1;
		    tv = ರಾಶಿ_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (is_blob)
		    {
			ಆದರೆ (blob_ತುಂಡು_or_ಸೂಚ್ಯಂಕ(val_tv->vval.v_blob, is_ತುಂಡು,
						    n1, n2, FALSE, tv) == FAIL)
			    ಹೋಗು on_error;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (is_tuple)
		    {
			ಆದರೆ (tuple_ತುಂಡು_or_ಸೂಚ್ಯಂಕ(val_tv->vval.v_tuple,
				is_ತುಂಡು, n1, n2, FALSE, tv, TRUE) == FAIL)
			    ಹೋಗು on_error;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (list_ತುಂಡು_or_ಸೂಚ್ಯಂಕ(val_tv->vval.v_list, is_ತುಂಡು,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    ಹೋಗು on_error;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ANYINDEX:
	    ಸ್ಥಿತಿ ISN_ANYSLICE:
		{
		    ಪೂರ್ಣಾಂಕ		is_ತುಂಡು = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    ಪೂರ್ಣಾಂಕ		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_ತುಂಡು ? ರಾಶಿ_TV_BOT(-3) : ರಾಶಿ_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (check_can_ಸೂಚ್ಯಂಕ(tv, TRUE, TRUE) == FAIL)
			ಹೋಗು on_error;
		    var1 = is_ತುಂಡು ? ರಾಶಿ_TV_BOT(-2) : ರಾಶಿ_TV_BOT(-1);
		    var2 = is_ತುಂಡು ? ರಾಶಿ_TV_BOT(-1) : ಖಾಲಿ;
		    res = eval_ಸೂಚ್ಯಂಕ_inner(tv, is_ತುಂಡು, var1, var2,
							FALSE, ಖಾಲಿ, -1, TRUE);
		    clear_tv(var1);
		    ಆದರೆ (is_ತುಂಡು)
			clear_tv(var2);
		    ectx->ec_ರಾಶಿ.ga_len -= is_ತುಂಡು ? 2 : 1;
		    ಆದರೆ (res == FAIL)
			ಹೋಗು on_error;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ತುಂಡು:
		{
		    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = iptr->isn_arg.ಸಂಖ್ಯೆ;

		    // type will have been checked to be a list
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಆದರೆ (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			ಆದರೆ (list != ಖಾಲಿ && list->lv_len >= ಎಣಿಕೆ_2)
			{
			    list_T	*newlist = list_ತುಂಡು(list,
				    ಎಣಿಕೆ_2, list->lv_len - 1);

			    ಆದರೆ (newlist != ಖಾಲಿ)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			ಆದರೆ (tuple != ಖಾಲಿ && TUPLE_LEN(tuple) >= ಎಣಿಕೆ_2)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_ತುಂಡು(tuple, ಎಣಿಕೆ_2,
							TUPLE_LEN(tuple) - 1);
			    ಆದರೆ (newtuple != ಖಾಲಿ)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*ಅಂಶ_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = ರಾಶಿ_TV_BOT(-1 - gi->gi_with_op);
		    ಆದರೆ (tv->v_type == VAR_LIST)
		    {
			li = list_ಕಂಡುಹಿಡಿ(tv->vval.v_list, gi->gi_ಸೂಚ್ಯಂಕ);
			ಆದರೆ (li == ಖಾಲಿ)
			    ಹೋಗು on_error;
			ಅಂಶ_tv = &li->li_tv;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			ಅಂಶ_tv = TUPLE_ಅಂಶ(tv->vval.v_tuple, gi->gi_ಸೂಚ್ಯಂಕ);

		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		    copy_tv(ಅಂಶ_tv, ರಾಶಿ_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_ಸೂಚ್ಯಂಕ = gi->gi_ಸೂಚ್ಯಂಕ + 1;
		    ectx->ec_where.wt_ಬಗೆ = WT_VARIABLE;
		}
		ನಿಲ್ಲಿಸು;

	    // dict member with string key (dict['member'])
	    ಸ್ಥಿತಿ ISN_MEMBER:
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
		    ಆದರೆ (ಕೀಲಿ == ಖಾಲಿ)
			ಕೀಲಿ = (char_u *)"";

		    ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(dict, ಕೀಲಿ, -1)) == ಖಾಲಿ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str), ಕೀಲಿ);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_ರಾಶಿ.ga_len;
			tv = ರಾಶಿ_TV_BOT(-1);
			(ಶೂನ್ಯ) dict_ರಾಶಿ_save(tv);
			tv->v_type = VAR_ಸಂಖ್ಯೆ;
			tv->vval.v_ಸಂಖ್ಯೆ = 0;
			ಹೋಗು on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_ರಾಶಿ.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಆದರೆ (dict_ರಾಶಿ_save(tv) == FAIL)
			ಹೋಗು on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		ನಿಲ್ಲಿಸು;

	    // dict member with string key (dict.member)
	    // or can be an object
	    ಸ್ಥಿತಿ ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = ರಾಶಿ_TV_BOT(-1);

		    ಆದರೆ (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			ಆದರೆ (dict_ರಾಶಿ_save(tv) == FAIL)
			    ಹೋಗು on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			ಆದರೆ (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    ಹೋಗು on_error;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			ಆದರೆ (tv->v_type != VAR_DICT || tv->vval.v_dict == ಖಾಲಿ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    ಹೋಗು on_error;
			}
			dict = tv->vval.v_dict;

			ಆದರೆ ((di = dict_ಕಂಡುಹಿಡಿ(dict, iptr->isn_arg.string, -1))
								   == ಖಾಲಿ)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_ಕೀಲಿ_not_present_ಒಳ_dictionary_str),
						     iptr->isn_arg.string);
			    ಹೋಗು on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			ಆದರೆ (dict_ರಾಶಿ_save(tv) == FAIL)
			    ಹೋಗು on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_GET_OBJ_MEMBER:
	    ಸ್ಥಿತಿ ISN_GET_ITF_MEMBER:
		{
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಆದರೆ (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			ಹೋಗು on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ಆದರೆ (obj == ಖಾಲಿ)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			ಹೋಗು on_error;
		    }

		    ಪೂರ್ಣಾಂಕ idx;
		    ಆದರೆ (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_ಸೂಚ್ಯಂಕ_from_itf_ಸೂಚ್ಯಂಕ(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    ಆದರೆ (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			ಹೋಗು on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_STORE_THIS:
		{
		    ಪೂರ್ಣಾಂಕ idx = iptr->isn_arg.ಸಂಖ್ಯೆ;
		    object_T *obj = ರಾಶಿ_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *ರಾಶಿ_TV_BOT(-1);
		    --ectx->ec_ರಾಶಿ.ga_len;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CLEARDICT:
		dict_ರಾಶಿ_drop();
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_ರಾಶಿ_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = ರಾಶಿ_TV_BOT(-1);
		    ಆದರೆ (dict_tv != ಖಾಲಿ
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != ಖಾಲಿ
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == ಖಾಲಿ))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_ರಾಶಿ_drop();
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_NEGATENR:
		tv = ರಾಶಿ_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		ಆದರೆ (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		ಇಲ್ಲದಿದ್ದರೆ
		    tv->vval.v_ಸಂಖ್ಯೆ = -tv->vval.v_ಸಂಖ್ಯೆ;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    ಪೂರ್ಣಾಂಕ		r;
		    where_T	where = WHERE_INIT;

		    tv = ರಾಶಿ_TV_BOT((ಪೂರ್ಣಾಂಕ)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ಆದರೆ (ct->ct_arg_idx > 0)
		    {
			where.wt_ಸೂಚ್ಯಂಕ = ct->ct_arg_idx;
			where.wt_ಬಗೆ = ct->ct_is_var ? WT_VARIABLE : WT_ವಾದ;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    ಆದರೆ (r == FAIL)
			ಹೋಗು on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    ಆದರೆ (tv->v_type == VAR_ಸಂಖ್ಯೆ
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_ಸಂಖ್ಯೆ == 0
						|| tv->vval.v_ಸಂಖ್ಯೆ == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_ಸಂಖ್ಯೆ = tv->vval.v_ಸಂಖ್ಯೆ
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CHECKLEN:
		{
		    ಪೂರ್ಣಾಂಕ	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = ಖಾಲಿ;
		    tuple_T *tuple = ಖಾಲಿ;
		    ಪೂರ್ಣಾಂಕ	    len = 0;

		    tv = ರಾಶಿ_TV_BOT(-1);

		    ಪೂರ್ಣಾಂಕ		len_check_failed = FALSE;
		    ಆದರೆ (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			ಆದರೆ (list == ಖಾಲಿ || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_ಇನ್ನಷ್ಟು_OK))
			    len_check_failed = TRUE;
			ಆದರೆ (list != ಖಾಲಿ)
			    len = list->lv_len;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			ಆದರೆ (tuple == ಖಾಲಿ || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_ಇನ್ನಷ್ಟು_OK))
			    len_check_failed = TRUE;
			ಆದರೆ (tuple != ಖಾಲಿ)
			    len = TUPLE_LEN(tuple);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			len_check_failed = TRUE;

		    ಆದರೆ (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_ವಸ್ತುಗಳು_but_got_nr),
				     min_len, len);
			ಹೋಗು on_error;
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_SETTYPE:
		ಗಣ_tv_type(ರಾಶಿ_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_2BOOL:
	    ಸ್ಥಿತಿ ISN_COND2BOOL:
		{
		    ಪೂರ್ಣಾಂಕ n;
		    ಪೂರ್ಣಾಂಕ error = FALSE;

		    ಆದರೆ (iptr->isn_type == ISN_2BOOL)
		    {
			tv = ರಾಶಿ_TV_BOT(iptr->isn_arg.tobool.ಸ್ಥಾನಾಂತರ);
			n = tv2bool(tv);
			ಆದರೆ (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			tv = ರಾಶಿ_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			ಆದರೆ (error)
			    ಹೋಗು on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_ಸಂಖ್ಯೆ = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_2STRING:
	    ಸ್ಥಿತಿ ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		ಆದರೆ (do_2string(ರಾಶಿ_TV_BOT(iptr->isn_arg.tostring.ಸ್ಥಾನಾಂತರ),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    ಹೋಗು on_error;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_RANGE:
		{
		    exarg_T	ea;
		    ಅಕ್ಷರ	*errormsg;

		    ea.line2 = 0;
		    ea.addr_ಎಣಿಕೆ = 0;
		    ea.addr_type = ADDR_ಸಾಲುಗಳು;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    ಆದರೆ (parse_cmd_ವಿಳಾಸ(&ea, &errormsg, FALSE) == FAIL)
			ಹೋಗು on_error;

		    ಆದರೆ (GA_GROW_FAILS(&ectx->ec_ರಾಶಿ, 1))
			ಹೋಗು theend;
		    ++ectx->ec_ರಾಶಿ.ga_len;
		    tv = ರಾಶಿ_TV_BOT(-1);
		    tv->v_type = VAR_ಸಂಖ್ಯೆ;
		    tv->v_ಬೀಗ = 0;
		    tv->vval.v_ಸಂಖ್ಯೆ = ea.line2;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_PUT:
		tv = ಖಾಲಿ; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_IPUT:
		tv = ಖಾಲಿ;
		isn_put_do(ectx, iptr, tv, TRUE);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_ರಾಶಿ.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = ಖಾಲಿ;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = ರಾಶಿ_TV_BOT(-1);
		ಆದರೆ (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_ವಾದ_must_be_sequence_ದ_lists_or_tuples));
		    ಹೋಗು on_error;
		}

		ಪೂರ್ಣಾಂಕ rc;
		ಆದರೆ (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		ಇಲ್ಲದಿದ್ದರೆ
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		ಆದರೆ (rc != EXEC_OK)
		{
		    ಆದರೆ (rc == EXEC_FAIL)
			ಹೋಗು on_error;
		    ಹೋಗು theend;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_PROF_ಆರಂಭ:
	    ಸ್ಥಿತಿ ISN_PROF_ಕೊನೆ:
		{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    ಆದರೆ (iptr->isn_type == ISN_PROF_ಆರಂಭ)
		    {
			func_ಸಾಲು_ಆರಂಭ(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_ಸಾಲು_exec(&cookie);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			func_ಸಾಲು_ಕೊನೆ(&cookie);
#ಆದರೆ_ಅಂತ್ಯ
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_DEBUG:
		handle_debug(iptr, ectx);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    ಪೂರ್ಣಾಂಕ		ಅಂಶ = iptr->isn_arg.shuffle.shfl_ಅಂಶ;
		    ಪೂರ್ಣಾಂಕ		ಮೇಲೆ = iptr->isn_arg.shuffle.shfl_ಮೇಲೆ;

		    tmp_tv = *ರಾಶಿ_TV_BOT(-ಅಂಶ);
		    ಗಾಗಿ ( ; ಮೇಲೆ > 0 && ಅಂಶ > 1; --ಮೇಲೆ)
		    {
			*ರಾಶಿ_TV_BOT(-ಅಂಶ) = *ರಾಶಿ_TV_BOT(-ಅಂಶ + 1);
			--ಅಂಶ;
		    }
		    *ರಾಶಿ_TV_BOT(-ಅಂಶ) = tmp_tv;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_DROP:
		--ectx->ec_ರಾಶಿ.ga_len;
		clear_tv(ರಾಶಿ_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_SCRIPTCTX_ಗಣ:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		ನಿಲ್ಲಿಸು;
	}
	ಮುಂದುವರಿ;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	ಆದರೆ (ectx->ec_ಚೌಕಟ್ಟು_idx == ectx->ec_initial_ಚೌಕಟ್ಟು_idx)
	    ಹೋಗು ಮುಗಿದ;

	ಆದರೆ (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    ಹೋಗು theend;
	ಮುಂದುವರಿ;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	ಆದರೆ (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    ಆದರೆ (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		ಆಗ (ectx->ec_ರಾಶಿ.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_ರಾಶಿ.ga_len;
		    clear_tv(ರಾಶಿ_TV_BOT(0));
		}
		ಆಗ (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    ಮುಂದುವರಿ;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	ಆದರೆ (trylevel <= ectx->ec_trylevel_at_ಆರಂಭ)
	    ಹೋಗು theend;
    }

ಮುಗಿದ:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_ರಾಶಿ_clear(dict_ರಾಶಿ_len_at_ಆರಂಭ);
    ectx->ec_trylevel_at_ಆರಂಭ = save_trylevel_at_ಆರಂಭ;
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    ಪೂರ್ಣಾಂಕ		save_iidx = ectx->ec_iidx;
    ಪೂರ್ಣಾಂಕ		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    ಆದರೆ (res == OK)
    {
	*rettv = *ರಾಶಿ_TV_BOT(-1);
	--ectx->ec_ರಾಶಿ.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    ಹಿಂತಿರುಗಿ res;
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
    ಪೂರ್ಣಾಂಕ		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    ಆದರೆ (exec_instructions(ectx) == OK)
    {
	typval_T *tv = ರಾಶಿ_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_ರಾಶಿ.ga_len;
	clear_tv(tv);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	substitute_instr->subs_ಸ್ಥಿತಿ = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    ಹಿಂತಿರುಗಿ res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
call_def_function(
    ufunc_T	*ufunc,
    ಪೂರ್ಣಾಂಕ		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    ಪೂರ್ಣಾಂಕ		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    ಪೂರ್ಣಾಂಕ		argc = argc_arg;
    ಪೂರ್ಣಾಂಕ		partial_argc = partial == ಖಾಲಿ
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    ಪೂರ್ಣಾಂಕ		ಒಟ್ಟು_argc = argc + partial_argc;
    typval_T	*tv;
    ಪೂರ್ಣಾಂಕ		idx;
    ಪೂರ್ಣಾಂಕ		ret = FAIL;
    ಪೂರ್ಣಾಂಕ		defcount = ufunc->uf_ವಾದಗಳು.ga_len - ಒಟ್ಟು_argc;
    sctx_T	save_current_sctx = current_sctx;
    ಪೂರ್ಣಾಂಕ		did_emsg_before = did_emsg_cumul + did_emsg;
    ಪೂರ್ಣಾಂಕ		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_ಸಂದೇಶ_list = ಖಾಲಿ;
    msglist_T	*ಖಾಸಗಿ_ಸಂದೇಶ_list = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		save_emsg_silent_def = emsg_silent_def;
    ಪೂರ್ಣಾಂಕ		save_did_emsg_def = did_emsg_def;
    ಪೂರ್ಣಾಂಕ		orig_funcdepth;
    ಪೂರ್ಣಾಂಕ		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ರಾಶಿ_TV
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_TV(idx) (((typval_T *)ectx.ec_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ರಾಶಿ_TV_BOT
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_TV_BOT(idx) (((typval_T *)ectx.ec_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + ectx.ec_ರಾಶಿ.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ರಾಶಿ_TV_VAR
#ವ್ಯಾಖ್ಯಾನಿಸು ರಾಶಿ_TV_VAR(idx) (((typval_T *)ectx.ec_ರಾಶಿ.ga_ದತ್ತಾಂಶ) + ectx.ec_ಚೌಕಟ್ಟು_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ + idx)

    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ == UF_NOT_COMPILED
	    || ufunc->uf_def_ಸ್ಥಿತಿ == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), ಖಾಲಿ) == FAIL))
    {
	ಆದರೆ (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							 + ufunc->uf_dfunc_idx;
	ಆದರೆ (dfunc->df_ufunc == ಖಾಲಿ)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (INSTRUCTIONS(dfunc) == ಖಾಲಿ)
	{
	    iemsg("using call_def_function() on not compiled function");
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    ಆದರೆ (funcdepth_increment() == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    CLEAR_ಕ್ಷೇತ್ರ(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_ರಾಶಿ, ಗಾತ್ರ(typval_T), 500);
    ಆದರೆ (GA_GROW_FAILS(&ectx.ec_ರಾಶಿ, 20))
    {
	funcdepth_decrement();
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ga_init2(&ectx.ec_trystack, ಗಾತ್ರ(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, ಗಾತ್ರ(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = ಒಟ್ಟು_argc - ufunc->uf_ವಾದಗಳು.ga_len;
    ಆದರೆ (idx > 0 && ufunc->uf_va_name == ಖಾಲಿ)
    {
	semsg(NGETTEXT(e_one_ವಾದ_too_many, e_nr_arguments_too_many,
								    idx), idx);
	ಹೋಗು failed_early;
    }
    idx = ಒಟ್ಟು_argc - ufunc->uf_ವಾದಗಳು.ga_len + ufunc->uf_def_ವಾದಗಳು.ga_len;
    ಆದರೆ (idx < 0)
    {
	semsg(NGETTEXT(e_one_ವಾದ_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	ಹೋಗು failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    ಗಾಗಿ (idx = 0; idx < ಒಟ್ಟು_argc
	    && (ufunc->uf_va_name != ಖಾಲಿ || idx < ufunc->uf_ವಾದಗಳು.ga_len);
									 ++idx)
    {
	ಪೂರ್ಣಾಂಕ argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	ಆದರೆ (idx >= ufunc->uf_ವಾದಗಳು.ga_len - ufunc->uf_def_ವಾದಗಳು.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_ಸಂಖ್ಯೆ == VVAL_NONE)
	{
	    // Use the default value.
	    ರಾಶಿ_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ ಮುಗಿದ = FALSE;
	    ಆದರೆ (ufunc->uf_arg_types != ಖಾಲಿ && idx < ufunc->uf_ವಾದಗಳು.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		ಆದರೆ (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_ಸಂಖ್ಯೆ)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    ರಾಶಿ_TV_BOT(0)->v_type = VAR_FLOAT;
		    ರಾಶಿ_TV_BOT(0)->v_ಬೀಗ = 0;
		    ರಾಶಿ_TV_BOT(0)->vval.v_float = tv->vval.v_ಸಂಖ್ಯೆ;
		    ಮುಗಿದ = TRUE;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (check_typval_arg_type(expected, tv,
						   ಖಾಲಿ, argv_idx + 1) == FAIL)
		    ಹೋಗು failed_early;
	    }
	    ಆದರೆ (!ಮುಗಿದ)
		copy_tv(tv, ರಾಶಿ_TV_BOT(0));
	}
	++ectx.ec_ರಾಶಿ.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    ಆದರೆ (ufunc->uf_va_name != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ vararg_ಎಣಿಕೆ = argc - ufunc->uf_ವಾದಗಳು.ga_len;

	ಆದರೆ (vararg_ಎಣಿಕೆ < 0)
	    vararg_ಎಣಿಕೆ = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	    argc -= vararg_ಎಣಿಕೆ;
	ಆದರೆ (exe_newlist(vararg_ಎಣಿಕೆ, &ectx) == FAIL)
	    ಹೋಗು failed_early;

	// Check the type of the list items.
	tv = ರಾಶಿ_TV_BOT(-1);
	ಆದರೆ (ufunc->uf_va_type != ಖಾಲಿ
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != ಖಾಲಿ)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_ಮೊದಲ;

	    ಗಾಗಿ (idx = 0; idx < vararg_ಎಣಿಕೆ; ++idx)
	    {
		ಆದರೆ (check_typval_arg_type(expected, &li->li_tv,
						 ಖಾಲಿ, argc + idx + 1) == FAIL)
		    ಹೋಗು failed_early;
		li = li->li_ಮುಂದಿನ;
	    }
	}

	ಆದರೆ (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *ರಾಶಿ_TV_BOT(defcount - 1) = *ರಾಶಿ_TV_BOT(-1);
	--ectx.ec_ರಾಶಿ.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    ಆದರೆ (defcount > 0)
	ಗಾಗಿ (idx = 0; idx < defcount; ++idx)
	{
	    ರಾಶಿ_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_ರಾಶಿ.ga_len;
	}
    ಆದರೆ (ufunc->uf_va_name != ಖಾಲಿ)
	++ectx.ec_ರಾಶಿ.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_ಚೌಕಟ್ಟು_idx = ectx.ec_ರಾಶಿ.ga_len;
    ectx.ec_initial_ಚೌಕಟ್ಟು_idx = ectx.ec_ಚೌಕಟ್ಟು_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *ತಳ_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	ಆದರೆ (partial != ಖಾಲಿ || ತಳ_ufunc->uf_partial != ಖಾಲಿ)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    ಆದರೆ (ectx.ec_outer_ref == ಖಾಲಿ)
		ಹೋಗು failed_early;
	    ಆದರೆ (partial != ಖಾಲಿ)
	    {
		outer_T *outer = get_pt_outer(partial);

		ಆದರೆ (outer->ಹೊರಹೋಗು_ರಾಶಿ == ಖಾಲಿ && outer->ಹೊರಹೋಗು_loop_ಗಾತ್ರ == 0)
		{
		    // no stack was set
		    ಆದರೆ (current_ectx != ಖಾಲಿ)
		    {
			ಆದರೆ (current_ectx->ec_outer_ref != ಖಾಲಿ
			       && current_ectx->ec_outer_ref->or_outer != ಖಾಲಿ)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ectx.ec_outer_ref->or_outer = &ತಳ_ufunc->uf_partial->pt_outer;
		++ತಳ_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = ತಳ_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    ಗಾಗಿ (idx = 0; idx < ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ; ++idx)
    {
	ರಾಶಿ_TV(ectx.ec_ರಾಶಿ.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_ರಾಶಿ.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	ಗಾಗಿ (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    ರಾಶಿ_TV_VAR(idx)->v_type = VAR_ಸಂಖ್ಯೆ;
	    ರಾಶಿ_TV_VAR(idx)->vval.v_ಸಂಖ್ಯೆ = 0;
	}
	ectx.ec_ರಾಶಿ.ga_len += dfunc->df_varcount;

	ಆದರೆ (object != ಖಾಲಿ)
	{
	    // the object is always the variable at index zero
	    tv = ರಾಶಿ_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	ಆದರೆ (dfunc->df_has_ಆವರಣ)
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
    ಆದರೆ (funccal != ಖಾಲಿ)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_ತಳ್ಳು_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_ಆವೃತ್ತಿ = SCRIPT_ಆವೃತ್ತಿ_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_ಸಂದೇಶ_list = ಸಂದೇಶ_list;
    ಸಂದೇಶ_list = &ಖಾಸಗಿ_ಸಂದೇಶ_list;

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
    ಆದರೆ (ret == OK)
    {
	// function finished, get result from the stack.
	ಆದರೆ (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    tv = ರಾಶಿ_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    ಆದರೆ (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_ಆವರಣ_ಒಳ_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_ತೆಗೆ();
    current_sctx = save_current_sctx;

    ಆದರೆ (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    ಆದರೆ (*ಸಂದೇಶ_list != ಖಾಲಿ && saved_ಸಂದೇಶ_list != ಖಾಲಿ)
    {
	msglist_T **plist = saved_ಸಂದೇಶ_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	ಆಗ (*plist != ಖಾಲಿ)
	    plist = &(*plist)->ಮುಂದಿನ;

	*plist = *ಸಂದೇಶ_list;
    }
    ಸಂದೇಶ_list = saved_ಸಂದೇಶ_list;

    ಆದರೆ (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ಖಾಲಿ;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    ಗಾಗಿ (idx = 0; idx < ectx.ec_ರಾಶಿ.ga_len; ++idx)
    {
	tv = ರಾಶಿ_TV(idx);
	ಆದರೆ (tv->v_type != VAR_ಸಂಖ್ಯೆ && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_ರಾಶಿ.ga_ದತ್ತಾಂಶ);
    vim_free(ectx.ec_trystack.ga_ದತ್ತಾಂಶ);
    ಆದರೆ (ectx.ec_outer_ref != ಖಾಲಿ)
    {
	ಆದರೆ (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    ಆದರೆ (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    ಶೂನ್ಯ
unwind_def_callstack(ectx_T *ectx)
{
    ಆಗ (ectx->ec_ಚೌಕಟ್ಟು_idx != ectx->ec_initial_ಚೌಕಟ್ಟು_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    ಶೂನ್ಯ
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + ectx->ec_dfunc_idx;

    ಆದರೆ (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    ಸ್ಥಿರ char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    ಪೂರ್ಣಾಂಕ		ಆಳ;

    ga_init2(&ga, 1, 100);
    ಗಾಗಿ (ಆಳ = 0; ಆಳ < lvi->lvi_ಆಳ; ++ಆಳ)
    {
	ಆದರೆ (ga_grow(&ga, 50) == FAIL)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (lvi->lvi_loop[ಆಳ].var_idx == 0)
	    STRCPY((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, " -");
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_snprintf((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[ಆಳ].var_idx,
			    lvi->lvi_loop[ಆಳ].var_idx
					 + lvi->lvi_loop[ಆಳ].var_ಎಣಿಕೆ - 1);
	ga.ga_len = (ಪೂರ್ಣಾಂಕ)STRLEN(ga.ga_ದತ್ತಾಂಶ);
    }
    ಹಿಂತಿರುಗಿ ga.ga_ದತ್ತಾಂಶ;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
list_instructions(ಅಕ್ಷರ *pfx, isn_T *instr, ಪೂರ್ಣಾಂಕ instr_ಎಣಿಕೆ, ufunc_T *ufunc)
{
    ಪೂರ್ಣಾಂಕ		ಸಾಲು_idx = 0;
    ಪೂರ್ಣಾಂಕ		prev_current = 0;
    ಪೂರ್ಣಾಂಕ		current;
    ಪೂರ್ಣಾಂಕ		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    ಗಾಗಿ (current = 0; current < instr_ಎಣಿಕೆ; ++current)
    {
	isn_T	    *iptr = &instr[current];
	ಅಕ್ಷರ	    *ಸಾಲು;

	ಆದರೆ (ufunc != ಖಾಲಿ)
	{
	    ಆಗ (ಸಾಲು_idx < iptr->isn_lnum
					  && ಸಾಲು_idx < ufunc->uf_ಸಾಲುಗಳು.ga_len)
	    {
		ಆದರೆ (current > prev_current)
		{
		    ಸಂದೇಶ_puts("\n\n");
		    prev_current = current;
		}
		ಸಾಲು = ((ಅಕ್ಷರ **)ufunc->uf_ಸಾಲುಗಳು.ga_ದತ್ತಾಂಶ)[ಸಾಲು_idx++];
		ಆದರೆ (ಸಾಲು != ಖಾಲಿ)
		    ಸಂದೇಶ(ಸಾಲು);
	    }
	    ಆದರೆ (iptr->isn_type == ISN_JUMP_IF_ARG_ಗಣ)
	    {
		ಪೂರ್ಣಾಂಕ	ಮೊದಲ_def_arg = ufunc->uf_ವಾದಗಳು.ga_len
						   - ufunc->uf_def_ವಾದಗಳು.ga_len;

		ಆದರೆ (def_arg_idx > 0)
		    ಸಂದೇಶ_puts("\n\n");
		ಸಂದೇಶ_ಆರಂಭ();
		ಸಂದೇಶ_puts("  ");
		ಸಂದೇಶ_puts(((ಅಕ್ಷರ **)(ufunc->uf_ವಾದಗಳು.ga_ದತ್ತಾಂಶ))[
						 ಮೊದಲ_def_arg + def_arg_idx]);
		ಸಂದೇಶ_puts(" = ");
		ಸಂದೇಶ_puts(((ಅಕ್ಷರ **)(ufunc->uf_def_ವಾದಗಳು.ga_ದತ್ತಾಂಶ))[def_arg_idx++]);
		ಸಂದೇಶ_clr_eos();
		ಸಂದೇಶ_ಕೊನೆ();
	    }
	}

	ಆಯ್ಕೆ (iptr->isn_type)
	{
	    ಸ್ಥಿತಿ ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (ಪೂರ್ಣಾಂಕ)iptr->isn_arg.construct.construct_ಗಾತ್ರ);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_EXEC_ವಿಭಜಿಸು:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.ಸಂಖ್ಯೆ ? " append" : "");
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_CEXPR_AUCMD:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.ಸಂಖ್ಯೆ));
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_CEXPR_CORE:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ, ಖಾಲಿ);
		ಸಂದೇಶ("     -------------");
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ಮೂಲ:
		{
		    scriptitem_T    *si = SCRIPT_ಅಂಶ(iptr->isn_arg.ಸಂಖ್ಯೆ);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ, ಖಾಲಿ);
		    ಸಂದೇಶ("     -------------");
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.ಸಂಖ್ಯೆ);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_ಎಣಿಕೆ);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ECHOWINDOW:
		ಆದರೆ (iptr->isn_arg.echowin.ewin_ಸಮಯ > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_ಎಣಿಕೆ,
				      iptr->isn_arg.echowin.ewin_ಸಮಯ);
		ಇಲ್ಲದಿದ್ದರೆ
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_ಎಣಿಕೆ);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ತುಂಬು:
		{
		    ಆದರೆ (iptr->isn_arg.ಸಂಖ್ಯೆ < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ
							  + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ));
		    ಇಲ್ಲದಿದ್ದರೆ
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ಆದರೆ (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_ಆಳ,
					outer->outer_idx + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ);
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (outer->outer_ಆಳ < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_ಆಳ);
		    ಇಲ್ಲದಿದ್ದರೆ
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_ಆಳ,
					      outer->outer_idx);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ಅಂಶ(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ಆದರೆ (sv == ಖಾಲಿ)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    ಇಲ್ಲದಿದ್ದರೆ
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADS:
	    ಸ್ಥಿತಿ ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ಅಂಶ(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (ಪೂರ್ಣಾಂಕ)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_STORE:
		ಆದರೆ (iptr->isn_arg.ಸಂಖ್ಯೆ < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.ಸಂಖ್ಯೆ + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ);
		ಇಲ್ಲದಿದ್ದರೆ
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.ಸಂಖ್ಯೆ);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ಆದರೆ (outer->outer_ಆಳ < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_ಆಳ);
		    ಇಲ್ಲದಿದ್ದರೆ
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_ಆಳ, outer->outer_idx);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STORES:
	    ಸ್ಥಿತಿ ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ಅಂಶ(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ಅಂಶ(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ಆದರೆ (sv == ಖಾಲಿ)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    ಇಲ್ಲದಿದ್ದರೆ
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STOREOPT:
	    ಸ್ಥಿತಿ ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (ಪೂರ್ಣಾಂಕ)iptr->isn_arg.ಸಂಖ್ಯೆ);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ತುಂಬು_CLASSMEMBER:
	    ಸ್ಥಿತಿ ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    ಪೂರ್ಣಾಂಕ	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_ತುಂಬು_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		ನಿಲ್ಲಿಸು;

	    // constants
	    ಸ್ಥಿತಿ ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHBOOL:
	    ಸ್ಥಿತಿ ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHFUNC:
		{
		    ಅಕ್ಷರ *name = (ಅಕ್ಷರ *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == ಖಾಲಿ ? "[none]" : name);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHCHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHJOB:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#ಆದರೆ_ಅಂತ್ಯ
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == ಖಾಲಿ ? "null"
				 : (ಅಕ್ಷರ *)iptr->isn_arg.classarg->class_name.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		ನಿಲ್ಲಿಸು;

	    // function call
	    ಸ್ಥಿತಿ ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    ಆಂತರಿಕ_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != ಖಾಲಿ
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_ತುದಿ ? " top" : "", cpfunc->cpf_argcount);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PCALL_ಕೊನೆ:
		smsg("%s%4d PCALL end", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (ಪೂರ್ಣಾಂಕ)iptr->isn_arg.defer.defer_argcount);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    ಆದರೆ (extra == ಖಾಲಿ || extra->fre_func_name == ಖಾಲಿ)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			name = extra->fre_func_name;
		    ಆದರೆ (extra != ಖಾಲಿ && extra->fre_class != ಖಾಲಿ)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (extra == ಖಾಲಿ
				     || extra->fre_loopvar_info.lvi_ಆಳ == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ಆದರೆ (arg->nfa_loopvar_info.lvi_ಆಳ == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == ಖಾಲಿ ? (char_u *)"" : name);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_JUMP:
		{
		    ಅಕ್ಷರ *when = "?";

		    ಆಯ್ಕೆ (iptr->isn_arg.jump.jump_when)
		    {
			ಸ್ಥಿತಿ JUMP_ALWAYS:
			    when = "JUMP";
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  //ಬಳಸದ
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    ನಿಲ್ಲಿಸು;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_JUMP_IF_ARG_ಗಣ:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ,
						iptr->isn_arg.jump.jump_where);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_JUMP_IF_ARG_NOT_ಗಣ:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ರಾಶಿ_ಚೌಕಟ್ಟು_ಗಾತ್ರ,
						iptr->isn_arg.jump.jump_where);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_ಕೊನೆ);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->ಕೊನೆ_funcref_idx,
			    endloop->ಕೊನೆ_var_idx,
			    endloop->ಕೊನೆ_var_idx + endloop->ಕೊನೆ_var_ಎಣಿಕೆ - 1,
			    endloop->ಕೊನೆ_ಆಳ);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_ಕೊನೆ);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    ಆದರೆ (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    ಇಲ್ಲದಿದ್ದರೆ
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_ಮಟ್ಟಗಳು,
				      trycont->tct_ಮಟ್ಟಗಳು == 1 ? "" : "s",
				      trycont->tct_where);
		}
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		ನಿಲ್ಲಿಸು;

	    // expression operations on number
	    ಸ್ಥಿತಿ ISN_OPNR:
	    ಸ್ಥಿತಿ ISN_OPFLOAT:
	    ಸ್ಥಿತಿ ISN_OPANY:
		{
		    ಅಕ್ಷರ *what;
		    ಅಕ್ಷರ *ins;

		    ಆಯ್ಕೆ (iptr->isn_arg.op.op_type)
		    {
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_MULT: what = "*"; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_DIV: what = "/"; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_REM: what = "%"; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SUB: what = "-"; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ಕೂಡಿಸು: what = "+"; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_LSHIFT: what = "<<"; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_RSHIFT: what = ">>"; ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ:       what = "???"; ನಿಲ್ಲಿಸು;
		    }
		    ಆಯ್ಕೆ (iptr->isn_type)
		    {
			ಸ್ಥಿತಿ ISN_OPNR: ins = "OPNR"; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ISN_OPFLOAT: ins = "OPFLOAT"; ನಿಲ್ಲಿಸು;
			ಸ್ಥಿತಿ ISN_OPANY: ins = "OPANY"; ನಿಲ್ಲಿಸು;
			ಪೂರ್ವನಿಯೋಜಿತ: ins = "???"; ನಿಲ್ಲಿಸು;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_COMPAREBOOL:
	    ಸ್ಥಿತಿ ISN_COMPARESPECIAL:
	    ಸ್ಥಿತಿ ISN_COMPARENULL:
	    ಸ್ಥಿತಿ ISN_COMPARENR:
	    ಸ್ಥಿತಿ ISN_COMPAREFLOAT:
	    ಸ್ಥಿತಿ ISN_COMPARESTRING:
	    ಸ್ಥಿತಿ ISN_COMPAREBLOB:
	    ಸ್ಥಿತಿ ISN_COMPARELIST:
	    ಸ್ಥಿತಿ ISN_COMPARETUPLE:
	    ಸ್ಥಿತಿ ISN_COMPAREDICT:
	    ಸ್ಥಿತಿ ISN_COMPAREFUNC:
	    ಸ್ಥಿತಿ ISN_COMPAREOBJECT:
	    ಸ್ಥಿತಿ ISN_COMPAREANY:
		   {
		       ಅಕ್ಷರ *p;
		       ಅಕ್ಷರ buf[10];
		       ಅಕ್ಷರ *type;

		       ಆಯ್ಕೆ (iptr->isn_arg.op.op_type)
		       {
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_EQUAL:	 p = "=="; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_NEQUAL:    p = "!="; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GREATER:   p = ">"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_GEQUAL:    p = ">="; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SMALLER:   p = "<"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_SEQUAL:    p = "<="; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ಹೊಂದಿಕೆ:	 p = "=~"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_IS:	 p = "is"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_ISNOT:	 p = "isnot"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ಅಭಿವ್ಯಕ್ತಿ_NOMATCH:	 p = "!~"; ನಿಲ್ಲಿಸು;
			   ಪೂರ್ವನಿಯೋಜಿತ:  p = "???"; ನಿಲ್ಲಿಸು;
		       }
		       STRCPY(buf, p);
		       ಆದರೆ (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       ಆಯ್ಕೆ(iptr->isn_type)
		       {
			   ಸ್ಥಿತಿ ISN_COMPAREBOOL: type = "COMPAREBOOL"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPARENULL: type = "COMPARENULL"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPARENR: type = "COMPARENR"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPARESTRING:
						  type = "COMPARESTRING"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPAREBLOB: type = "COMPAREBLOB"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPARELIST: type = "COMPARELIST"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPARETUPLE: type = "COMPARETUPLE"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPAREDICT: type = "COMPAREDICT"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPAREFUNC: type = "COMPAREFUNC"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; ನಿಲ್ಲಿಸು;
			   ಸ್ಥಿತಿ ISN_COMPAREANY: type = "COMPAREANY"; ನಿಲ್ಲಿಸು;
			   ಪೂರ್ವನಿಯೋಜಿತ: type = "???"; ನಿಲ್ಲಿಸು;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); ನಿಲ್ಲಿಸು;

	    // expression operations
	    ಸ್ಥಿತಿ ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.ಸಂಖ್ಯೆ));
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_ತುಂಡು: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.ಸಂಖ್ಯೆ); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_ಸೂಚ್ಯಂಕ,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (ಪೂರ್ಣಾಂಕ)iptr->isn_arg.classmember.cm_idx);
				     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (ಪೂರ್ಣಾಂಕ)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (ಪೂರ್ಣಾಂಕ)iptr->isn_arg.ಸಂಖ್ಯೆ); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      ಅಕ್ಷರ	    *tofree = ಖಾಲಿ;
		      ಅಕ್ಷರ	    *typename;

		      ಆದರೆ (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_ಸಂಖ್ಯೆ_OK))
			  typename = "float|number";
		      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      ಇಲ್ಲದಿದ್ದರೆ
			  typename = type_name(ct->ct_type, &tofree);

		      ಆದರೆ (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (ಪೂರ್ಣಾಂಕ)ct->ct_off);
		      ಇಲ್ಲದಿದ್ದರೆ
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (ಪೂರ್ಣಾಂಕ)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (ಪೂರ್ಣಾಂಕ)ct->ct_arg_idx);
		      vim_free(tofree);
		      ನಿಲ್ಲಿಸು;
		  }
	    ಸ್ಥಿತಿ ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_ಇನ್ನಷ್ಟು_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_SETTYPE:
		  {
		      ಅಕ್ಷರ *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      ನಿಲ್ಲಿಸು;
		  }
	    ಸ್ಥಿತಿ ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_2BOOL: ಆದರೆ (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.ಸ್ಥಾನಾಂತರ);
			    ಇಲ್ಲದಿದ್ದರೆ
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.ಸ್ಥಾನಾಂತರ);
			    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.ಸ್ಥಾನಾಂತರ));
			      ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.ಸ್ಥಾನಾಂತರ));
			      ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_PUT:
		ಆದರೆ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ಇಲ್ಲದಿದ್ದರೆ
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (ದೀರ್ಘ)iptr->isn_arg.put.put_lnum);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_IPUT:
		ಆದರೆ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ಇಲ್ಲದಿದ್ದರೆ
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (ದೀರ್ಘ)iptr->isn_arg.put.put_lnum);
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  ಖಾಲಿ, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    ಆದರೆ (likely(buf != ಖಾಲಿ))
		    {
			(ಶೂನ್ಯ)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    ನಿಲ್ಲಿಸು;
		}
	    ಸ್ಥಿತಿ ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_PROF_ಆರಂಭ:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_PROF_ಕೊನೆ:
		smsg("%s%4d PROFILE END", pfx, current);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_ಹೆಸರುಗಳು_len);
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_ಎಣಿಕೆ,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_ಅಂಶ,
					 iptr->isn_arg.shuffle.shfl_ಮೇಲೆ);
			      ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ ISN_DROP: smsg("%s%4d DROP", pfx, current); ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_SCRIPTCTX_ಗಣ:
		{
		    ಪೂರ್ಣಾಂಕ	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ಅಂಶ(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ ISN_ಮುಕ್ತಾಯ: // End of list of instructions for ISN_SUBSTITUTE.
			   ಹಿಂತಿರುಗಿ;
	}

	ಹೊರಹೋಗು_flush();	    // output one line at a time
	ui_breakcheck();
	ಆದರೆ (got_int)
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    ಶೂನ್ಯ
ಗಣ_context_ಒಳ_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_ನಮೂನೆ = arg;

    // first argument already typed: only user function names
    ಆದರೆ (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
    {
	xp->xp_context = EXPAND_USER_FUNC;
	xp->xp_ನಮೂನೆ = skipwhite(p);
    }
}

/*
 * Function given to ExpandGeneric() to obtain the list of :disassemble
 * arguments.
 */
    char_u *
get_disassemble_ವಾದ(expand_T *xp, ಪೂರ್ಣಾಂಕ idx)
{
    ಆದರೆ (idx == 0)
	ಹಿಂತಿರುಗಿ (char_u *)"debug";
    ಆದರೆ (idx == 1)
	ಹಿಂತಿರುಗಿ (char_u *)"profile";
    ಹಿಂತಿರುಗಿ get_user_func_name(xp, idx - 2);
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
    isn_T	*instr = ಖಾಲಿ;  // init to shut up gcc warning
    ಪೂರ್ಣಾಂಕ		instr_ಎಣಿಕೆ = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = ಕಂಡುಹಿಡಿ_func_by_name(arg, &compile_type);
    ಆದರೆ (ufunc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, ಖಾಲಿ) == FAIL)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (ufunc->uf_def_ಸ್ಥಿತಿ != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	ಹಿಂತಿರುಗಿ;
    }
    ಸಂದೇಶ((ಅಕ್ಷರ *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_ದತ್ತಾಂಶ) + ufunc->uf_dfunc_idx;
    ಆಯ್ಕೆ (compile_type)
    {
	ಸ್ಥಿತಿ CT_PROFILE:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_ಎಣಿಕೆ = dfunc->df_instr_prof_ಎಣಿಕೆ;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    // FALLTHROUGH
	ಸ್ಥಿತಿ CT_NONE:
	    instr = dfunc->df_instr;
	    instr_ಎಣಿಕೆ = dfunc->df_instr_ಎಣಿಕೆ;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_ಎಣಿಕೆ = dfunc->df_instr_debug_ಎಣಿಕೆ;
	    ನಿಲ್ಲಿಸು;
    }

    list_instructions("", instr, instr_ಎಣಿಕೆ, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    ಪೂರ್ಣಾಂಕ
tv2bool(typval_T *tv)
{
    ಆಯ್ಕೆ (tv->v_type)
    {
	ಸ್ಥಿತಿ VAR_ಸಂಖ್ಯೆ:
	    ಹಿಂತಿರುಗಿ tv->vval.v_ಸಂಖ್ಯೆ != 0;
	ಸ್ಥಿತಿ VAR_FLOAT:
	    ಹಿಂತಿರುಗಿ tv->vval.v_float != 0.0;
	ಸ್ಥಿತಿ VAR_PARTIAL:
	    ಹಿಂತಿರುಗಿ tv->vval.v_partial != ಖಾಲಿ;
	ಸ್ಥಿತಿ VAR_FUNC:
	ಸ್ಥಿತಿ VAR_STRING:
	    ಹಿಂತಿರುಗಿ tv->vval.v_string != ಖಾಲಿ && *tv->vval.v_string != NUL;
	ಸ್ಥಿತಿ VAR_LIST:
	    ಹಿಂತಿರುಗಿ tv->vval.v_list != ಖಾಲಿ && tv->vval.v_list->lv_len > 0;
	ಸ್ಥಿತಿ VAR_TUPLE:
	    ಹಿಂತಿರುಗಿ tuple_len(tv->vval.v_tuple) > 0;
	ಸ್ಥಿತಿ VAR_DICT:
	    ಹಿಂತಿರುಗಿ tv->vval.v_dict != ಖಾಲಿ
				    && tv->vval.v_dict->dv_hashtab.ht_ಬಳಸಿದ > 0;
	ಸ್ಥಿತಿ VAR_BOOL:
	ಸ್ಥಿತಿ VAR_SPECIAL:
	    ಹಿಂತಿರುಗಿ tv->vval.v_ಸಂಖ್ಯೆ == VVAL_TRUE ? TRUE : FALSE;
	ಸ್ಥಿತಿ VAR_ಕೆಲಸ:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಹಿಂತಿರುಗಿ tv->vval.v_ಕೆಲಸ != ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_CHANNEL:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
	    ಹಿಂತಿರುಗಿ tv->vval.v_channel != ಖಾಲಿ;
#ಇಲ್ಲದಿದ್ದರೆ
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಸ್ಥಿತಿ VAR_BLOB:
	    ಹಿಂತಿರುಗಿ tv->vval.v_blob != ಖಾಲಿ && tv->vval.v_blob->bv_ga.ga_len > 0;

	ಸ್ಥಿತಿ VAR_OBJECT:
	    ಹಿಂತಿರುಗಿ tv->vval.v_object != ಖಾಲಿ;

	ಸ್ಥಿತಿ VAR_CLASS:
	ಸ್ಥಿತಿ VAR_TYPEALIAS:
	    check_typval_is_ಮೌಲ್ಯ(tv);
	    ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ VAR_UNKNOWN:
	ಸ್ಥಿತಿ VAR_ANY:
	ಸ್ಥಿತಿ VAR_VOID:
	ಸ್ಥಿತಿ VAR_INSTR:
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಶೂನ್ಯ
emsg_using_string_as(typval_T *tv, ಪೂರ್ಣಾಂಕ as_ಸಂಖ್ಯೆ)
{
    semsg(_(as_ಸಂಖ್ಯೆ ? e_using_string_as_ಸಂಖ್ಯೆ_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == ಖಾಲಿ
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    ಪೂರ್ಣಾಂಕ
check_not_string(typval_T *tv)
{
    ಆದರೆ (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_EVAL
