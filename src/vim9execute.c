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
#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_frame_idx;	// ec_frame_idx at ISN_TRY
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_ଗଦା_len;	// size of ectx.ec_stack at ISN_TRY
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_in_catch;	// in catch or finally block
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_did_throw;	// set did_throw in :endtry
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_catch_idx;	// instruction of the first :catch or :finally
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_finally_idx;	// instruction of the :finally block or zero
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_endtry_idx;	// instruction of the :endtry
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_caught;		// catch block entered
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_cont;		// :continue encountered, jump here (minus one)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		or_outer_allocated;  // free "or_outer" later
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
ସଂରଚନା ectx_S {
    garray_T	ec_ଗଦା;	// stack of typval_T values
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ec_frame_idx;	// index in ec_stack: context of ec_dfunc_idx
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ec_initial_frame_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ec_dfunc_idx;	// current function index
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ec_did_emsg_before;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ec_trylevel_at_start;
    where_T	ec_where;
};

#ifdef FEAT_PROFILE
// stack of profinfo_T used when profiling.
ସ୍ଥିର garray_T profile_info_ga = {0, 0, ଆକାର(profinfo_T), 20, ଖାଲି};
#endif

// Get pointer to item in the stack.
#define ଗଦା_TV(idx) (((typval_T *)ectx->ec_ଗଦା.ga_data) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#define ଗଦା_TV_BOT(idx) (((typval_T *)ectx->ec_ଗଦା.ga_data) + ectx->ec_ଗଦା.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#define ଗଦା_TV_VAR(idx) (((typval_T *)ectx->ec_ଗଦା.ga_data) + ectx->ec_frame_idx + ଗଦା_FRAME_SIZE + idx)

// Return value for functions used to execute instructions
#define EXEC_FAIL	0
#define EXEC_OK		1
#define EXEC_DONE	2

    ଶୂନ୍ୟ
to_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_to_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
ufunc_argcount(ufunc_T *ufunc)
{
    ଫେରନ୍ତୁ ufunc->uf_args.ga_len + (ufunc->uf_va_name != ଖାଲି ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exe_concat(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ectx_T *ectx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    size_t	len = 0;
    garray_T	ga;
    ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
    {
	typval_T    *tv;
	size_t	    length;
    } string_segment_T;
    ଗଣନା
    {
	STRING_SEGMENT_CACHE_SIZE = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_SIZE];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    ଯଦି (ଗଣନା_2 > (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, ଗଣନା_2);
	ଯଦି (string_segment_tab == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
    }

    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    ପାଇଁ (idx = 0; idx < ଗଣନା_2; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = ଗଦା_TV_BOT(idx - ଗଣନା_2);
	ଯଦି (segment->tv->vval.v_string != ଖାଲି)
	{
	    segment->length = STRLEN(segment->tv->vval.v_string);
	    len += segment->length;
	}
	ଅନ୍ୟଥା
	    segment->length = 0;    // Ensure clean state for the second pass
    }

    ଯଦି (ga_grow(&ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len + 1) == FAIL)
    {
	ଯଦି (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	ଫେରନ୍ତୁ FAIL;
    }

    ପାଇଁ (idx = 0; idx < ଗଣନା_2; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->length);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_ଗଦା.ga_len -= ଗଣନା_2 - 1;
    ଗଦା_TV_BOT(-1)->vval.v_string = ga.ga_data;

    ଯଦି (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    ଫେରନ୍ତୁ OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exe_newlist(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ectx_T *ectx)
{
    list_T	*list = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    typval_T	*tv;

    ଯଦି (ଗଣନା_2 >= 0)
    {
	list = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(ଗଣନା_2);
	ଯଦି (list == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ପାଇଁ (idx = 0; idx < ଗଣନା_2; ++idx)
	    list_set_item(list, idx, ଗଦା_TV_BOT(idx - ଗଣନା_2));
    }

    ଯଦି (ଗଣନା_2 > 0)
	ectx->ec_ଗଦା.ga_len -= ଗଣନା_2 - 1;
    ଅନ୍ୟଥା ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
    {
	list_unref(list);
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
	++ectx->ec_ଗଦା.ga_len;
    tv = ଗଦା_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_lock = 0;
    ଯଦି (list != ଖାଲି)
	++list->lv_refcount;
    ଫେରନ୍ତୁ OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exe_newtuple(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ectx_T *ectx)
{
    tuple_T	*tuple = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    typval_T	*tv;

    ଯଦି (ଗଣନା_2 >= 0)
    {
	tuple = tuple_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(ଗଣନା_2);
	ଯଦି (tuple == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ପାଇଁ (idx = 0; idx < ଗଣନା_2; ++idx)
	    tuple_set_item(tuple, idx, ଗଦା_TV_BOT(idx - ଗଣନା_2));
    }

    ଯଦି (ଗଣନା_2 > 0)
	ectx->ec_ଗଦା.ga_len -= ଗଣନା_2 - 1;
    ଅନ୍ୟଥା ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
    {
	tuple_unref(tuple);
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
	++ectx->ec_ଗଦା.ga_len;
    tv = ଗଦା_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_lock = 0;
    ଯଦି (tuple != ଖାଲି)
	++tuple->tv_refcount;

    ଫେରନ୍ତୁ OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exe_newdict(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2, ectx_T *ectx)
{
    dict_T	*dict = ଖାଲି;
    dictitem_T	*item;
    char_u	*ଚାବି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    typval_T	*tv;

    ଯଦି (ଗଣନା_2 >= 0)
    {
	dict = dict_alloc();
	ଯଦି (unlikely(dict == ଖାଲି))
	    ଫେରନ୍ତୁ FAIL;
	ପାଇଁ (idx = 0; idx < ଗଣନା_2; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = ଗଦା_TV_BOT(2 * (idx - ଗଣନା_2));
	    // check key is unique
	    ଚାବି = tv->vval.v_string == ଖାଲି ? (char_u *)"" : tv->vval.v_string;
	    item = dict_find(dict, ଚାବି, -1);
	    ଯଦି (item != ଖାଲି)
	    {
		semsg(_(e_duplicate_ଚାବି_in_dictionary_str), ଚାବି);
		dict_unref(dict);
		ଫେରନ୍ତୁ MAYBE;
	    }
	    item = dictitem_alloc(ଚାବି);
	    clear_tv(tv);
	    ଯଦି (unlikely(item == ଖାଲି))
	    {
		dict_unref(dict);
		ଫେରନ୍ତୁ FAIL;
	    }
	    tv = ଗଦା_TV_BOT(2 * (idx - ଗଣନା_2) + 1);
	    item->di_tv = *tv;
	    item->di_tv.v_lock = 0;
	    tv->v_type = VAR_UNKNOWN;
	    ଯଦି (dict_add(dict, item) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(item);
		dict_unref(dict);
		ଫେରନ୍ତୁ FAIL;
	    }
	}
    }

    ଯଦି (ଗଣନା_2 > 0)
	ectx->ec_ଗଦା.ga_len -= 2 * ଗଣନା_2 - 1;
    ଅନ୍ୟଥା ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
    {
	dict_unref(dict);
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
	++ectx->ec_ଗଦା.ga_len;
    tv = ଗଦା_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_lock = 0;
    tv->vval.v_dict = dict;
    ଯଦି (dict != ଖାଲି)
	++dict->dv_refcount;
    ଫେରନ୍ତୁ OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    ଶୂନ୍ୟ
update_has_breakpoint(ufunc_T *ufunc)
{
    ଯଦି (ufunc->uf_debug_tick == debug_tick)
	ଫେରନ୍ତୁ;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

ସ୍ଥିର garray_T dict_ଗଦା = GA_EMPTY;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
dict_ଗଦା_save(typval_T *tv)
{
    ଯଦି (dict_ଗଦା.ga_growsize == 0)
	ga_init2(&dict_ଗଦା, ଆକାର(typval_T), 10);
    ଯଦି (ga_grow(&dict_ଗଦା, 1) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ((typval_T *)dict_ଗଦା.ga_data)[dict_ଗଦା.ga_len] = *tv;
    ++dict_ଗଦା.ga_len;
    ଫେରନ୍ତୁ OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    ସ୍ଥିର typval_T *
dict_ଗଦା_get_tv(ଶୂନ୍ୟ)
{
    ଯଦି (dict_ଗଦା.ga_len == 0)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ ((typval_T *)dict_ଗଦା.ga_data) + dict_ଗଦା.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    ସ୍ଥିର dict_T *
dict_ଗଦା_get_dict(ଶୂନ୍ୟ)
{
    typval_T *tv;

    ଯଦି (dict_ଗଦା.ga_len == 0)
	ଫେରନ୍ତୁ ଖାଲି;
    tv = ((typval_T *)dict_ଗଦା.ga_data) + dict_ଗଦା.ga_len - 1;
    ଯଦି (tv->v_type == VAR_DICT)
	ଫେରନ୍ତୁ tv->vval.v_dict;
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Drop an item from the dict stack.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
dict_ଗଦା_drop(ଶୂନ୍ୟ)
{
    ଯଦି (dict_ଗଦା.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	ଫେରନ୍ତୁ;
    }
    --dict_ଗଦା.ga_len;
    clear_tv(((typval_T *)dict_ଗଦା.ga_data) + dict_ଗଦା.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
dict_ଗଦା_clear(ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଯେତେବେଳେ (dict_ଗଦା.ga_len > len)
	dict_ଗଦା_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    ସ୍ଥିର outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    ଯଦି (ptref == ଖାଲି)
	ଫେରନ୍ତୁ &pt->pt_outer;

    // partial using partial (recursively)
    ଯେତେବେଳେ (ptref->pt_outer_partial != ଖାଲି)
	ptref = ptref->pt_outer_partial;
    ଫେରନ୍ତୁ &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_ufunc_arg_types(ufunc_T *ufunc, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, ପୂର୍ଣ୍ଣସଂଖ୍ୟା off, ectx_T *ectx)
{
    ଯଦି (ufunc->uf_arg_types == ଖାଲି && ufunc->uf_va_type == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    typval_T	*argv = ଗଦା_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < argcount; ++i)
    {
	type_T *type = ଖାଲି;

	// assume a v:none argument, using the default value, is always OK
	ଯଦି (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_ସଂଖ୍ୟା == VVAL_NONE)
	    ଜାରି;
	// only pass values to user functions, never types
	ଯଦି (check_typval_is_ମୂଲ୍ୟ(&argv[i]) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (i < ufunc->uf_args.ga_len && ufunc->uf_arg_types != ଖାଲି)
	    type = ufunc->uf_arg_types[i];
	ଅନ୍ୟଥା ଯଦି (ufunc->uf_va_type != ଖାଲି)
	    type = ufunc->uf_va_type->tt_member;
	ଯଦି (type != ଖାଲି && check_typval_arg_type(type,
		    &argv[i], ଖାଲି, i + 1) == FAIL)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_dfunc(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cdf_idx,
	partial_T	*pt_arg,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount_arg,
	ectx_T		*ectx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg_cumul + did_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		arg_to_add;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vararg_ଗଣନା = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		varcount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    estack_T	*entry;
    funclocal_T	*floc = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = OK;
    compiletype_T compile_type;

    ଯଦି (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == ଖାଲି ? (char_u *)"unknown" : dfunc->df_name);
	ଫେରନ୍ତୁ FAIL;
    }

#ifdef FEAT_PROFILE
    ଯଦି (do_profiling == PROF_YES)
    {
	ଯଦି (GA_GROW_OK(&profile_info_ga, 1))
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
    ଯଦି (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, ଖାଲି);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    }
    ଯଦି (res == FAIL || INSTRUCTIONS(dfunc) == ଖାଲି)
    {
	ଯଦି (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (ufunc->uf_va_name != ଖାଲି)
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
	vararg_ଗଣନା = argcount - ufunc->uf_args.ga_len;
	ଯଦି (vararg_ଗଣନା < 0)
	    vararg_ଗଣନା = 0;
	ଅନ୍ୟଥା
	    argcount -= vararg_ଗଣନା;
	ଯଦି (exe_newlist(vararg_ଗଣନା, ectx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	vararg_ଗଣନା = 1;
    }

    arg_to_add = ufunc->uf_args.ga_len - argcount;
    ଯଦି (arg_to_add < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
			-arg_to_add), -arg_to_add);
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (arg_to_add > ufunc->uf_def_args.ga_len)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା missing = arg_to_add - ufunc->uf_def_args.ga_len;

	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
			missing), missing);
	ଫେରନ୍ତୁ FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = ଗଦା_TV_BOT(0) - argcount - vararg_ଗଣନା - 1;

    ଯଦି (IS_OBJECT_METHOD(ufunc) && !IS_CONSTRUCTOR_METHOD(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == ଖାଲି)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	ଫେରନ୍ତୁ FAIL;
    }

    // Check the argument types.
    ଯଦି (check_ufunc_arg_types(ufunc, argcount, vararg_ଗଣନା, ectx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

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
    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା,
				     arg_to_add + ଗଦା_FRAME_SIZE + varcount))
	ଫେରନ୍ତୁ FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = ଗଦା_TV_BOT(0) - argcount - vararg_ଗଣନା - 1;

    // If depth of calling is getting too high, don't execute the function.
    ଯଦି (funcdepth_increment() == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    ଯଦି (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	ଯଦି (floc == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    ଯଦି (vararg_ଗଣନା > 0 && arg_to_add > 0)
	*ଗଦା_TV_BOT(arg_to_add - 1) = *ଗଦା_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    ପାଇଁ (idx = 0; idx < arg_to_add; ++idx)
	ଗଦା_TV_BOT(idx - vararg_ଗଣନା)->v_type = VAR_UNKNOWN;
    ectx->ec_ଗଦା.ga_len += arg_to_add;

    // Store current execution state in stack frame for ISN_RETURN.
    ଗଦା_TV_BOT(ଗଦା_FRAME_FUNC_OFF)->vval.v_ସଂଖ୍ୟା = ectx->ec_dfunc_idx;
    ଗଦା_TV_BOT(ଗଦା_FRAME_IIDX_OFF)->vval.v_ସଂଖ୍ୟା = ectx->ec_iidx;
    ଗଦା_TV_BOT(ଗଦା_FRAME_INSTR_OFF)->vval.v_string = (ଶୂନ୍ୟ *)ectx->ec_instr;
    ଗଦା_TV_BOT(ଗଦା_FRAME_OUTER_OFF)->vval.v_string =
						    (ଶୂନ୍ୟ *)ectx->ec_outer_ref;
    ଗଦା_TV_BOT(ଗଦା_FRAME_FUNCLOCAL_OFF)->vval.v_string = (ଶୂନ୍ୟ *)floc;
    ଗଦା_TV_BOT(ଗଦା_FRAME_IDX_OFF)->vval.v_ସଂଖ୍ୟା = ectx->ec_frame_idx;
    ectx->ec_frame_idx = ectx->ec_ଗଦା.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା initcount = dfunc->df_varcount + (dfunc->df_has_closure ? 1 : 0);
    ପାଇଁ (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = ଗଦା_TV_BOT(ଗଦା_FRAME_SIZE + idx);

	tv->v_type = VAR_ସଂଖ୍ୟା;
	tv->vval.v_ସଂଖ୍ୟା = 0;
    }
    ectx->ec_ଗଦା.ga_len += ଗଦା_FRAME_SIZE + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    ଯଦି (IS_OBJECT_METHOD(ufunc))
    {
	ଯଦି (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_internal_error_str), "type in stack is not an object");
	    ଫେରନ୍ତୁ FAIL;
	}

	*ଗଦା_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != ଖାଲି ? pt_arg : ufunc->uf_partial;
    ଯଦି (pt != ଖାଲି || (ufunc->uf_flags & FC_CLOSURE))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	ଯଦି (ref == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (pt != ଖାଲି)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	ଅନ୍ୟଥା
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    ଯଦି (unlikely(ref->or_outer == ଖାଲି))
	    {
		vim_free(ref);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->out_ଗଦା = &ectx->ec_ଗଦା;
	    ref->or_outer->out_frame_idx = ectx->ec_frame_idx;
	    ଯଦି (ectx->ec_outer_ref != ଖାଲି)
		ref->or_outer->out_up = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    ଅନ୍ୟଥା
	ectx->ec_outer_ref = ଖାଲି;

    ଯଦି (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    entry = estack_ଠେଲ_ufunc(ufunc, 1);
    ଯଦି (entry != ଖାଲି)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	entry->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    ଫେରନ୍ତୁ OK;
}

// Double linked list of funcstack_T in use.
ସ୍ଥିର funcstack_T *first_funcstack = ଖାଲି;

    ସ୍ଥିର ଶୂନ୍ୟ
add_funcstack_to_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    ଯଦି (first_funcstack != ଖାଲି)
	first_funcstack->fs_prev = funcstack;
    funcstack->fs_next = first_funcstack;
    funcstack->fs_prev = ଖାଲି;
    first_funcstack = funcstack;
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_funcstack_from_list(funcstack_T *funcstack)
{
    ଯଦି (funcstack->fs_prev == ଖାଲି)
	first_funcstack = funcstack->fs_next;
    ଅନ୍ୟଥା
	funcstack->fs_prev->fs_next = funcstack->fs_next;
    ଯଦି (funcstack->fs_next != ଖାଲି)
	funcstack->fs_next->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
handle_closure_in_use(ectx_T *ectx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଶୀର୍ଷ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    typval_T	*tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_ଗଣନା;

    ଯଦି (dfunc->df_ufunc == ଖାଲି)
	ଫେରନ୍ତୁ OK;  // function was freed
    ଯଦି (dfunc->df_has_closure == 0)
	ଫେରନ୍ତୁ OK;  // no closures
    tv = ଗଦା_TV(ectx->ec_frame_idx + ଗଦା_FRAME_SIZE + dfunc->df_varcount);
    closure_ଗଣନା = tv->vval.v_ସଂଖ୍ୟା;
    ଯଦି (closure_ଗଣନା == 0)
	ଫେରନ୍ତୁ OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    ଶୀର୍ଷ = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    ପାଇଁ (idx = 0; idx < closure_ଗଣନା; ++idx)
    {
	partial_T   *pt;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    off = gap->ga_len - closure_ଗଣନା + idx;

	ଯଦି (off < 0)
	    ଜାରି;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	ଯଦି (pt->pt_refcount > 1)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା refcount = pt->pt_refcount;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    ପାଇଁ (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = ଗଦା_TV(ectx->ec_frame_idx
						       + ଗଦା_FRAME_SIZE + i);
		ଯଦି (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ଯଦି (refcount > 1)
	    {
		closure_in_use = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *ଗଦା;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	ଯଦି (funcstack == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	funcstack->fs_var_offset = argcount + ଗଦା_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset
							  + dfunc->df_varcount;
	ଗଦା = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = ଗଦା;
	ଯଦି (ଗଦା == ଖାଲି)
	{
	    vim_free(funcstack);
	    ଫେରନ୍ତୁ FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	ପାଇଁ (idx = 0; idx < argcount; ++idx)
	{
	    tv = ଗଦା_TV(ଶୀର୍ଷ + idx);
	    ଯଦି (free_arguments)
	    {
		*(ଗଦା + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    ଅନ୍ୟଥା ଯଦି (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, ଗଦା + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	ପାଇଁ (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = ଗଦା_TV(ectx->ec_frame_idx + ଗଦା_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    ଯଦି (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

		ପାଇଁ (i = 0; i < closure_ଗଣନା; ++i)
		    ଯଦି (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_ଗଣନା + i])
			++funcstack->fs_min_refcount;
	    }

	    *(ଗଦା + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	ପାଇଁ (idx = 0; idx < closure_ଗଣନା; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_ଗଣନା + idx];
	    ଯଦି (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_ଗଦା = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - ଶୀର୍ଷ;
	    }
	}
    }

    ପାଇଁ (idx = 0; idx < closure_ଗଣନା; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_ଗଣନା + idx]);
    gap->ga_len -= closure_ଗଣନା;
    ଯଦି (gap->ga_len == 0)
	ga_clear(gap);

    ଫେରନ୍ତୁ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
funcstack_check_refcount(funcstack_T *funcstack)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    garray_T	*gap = &funcstack->fs_ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		done = 0;
    typval_T	*ଗଦା;

    ଯଦି (funcstack->fs_refcount > funcstack->fs_min_refcount)
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (i = funcstack->fs_var_offset; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_data) + i;

	ଯଦି (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ଖାଲି
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++done;
    }
    ଯଦି (done != funcstack->fs_min_refcount)
	ଫେରନ୍ତୁ FALSE;

    ଗଦା = gap->ga_data;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	clear_tv(ଗଦା + i);
    vim_free(ଗଦା);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    ଫେରନ୍ତୁ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_funcstacks(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    funcstack_T *funcstack;

    ପାଇଁ (funcstack = first_funcstack; funcstack != ଖାଲି;
						funcstack = funcstack->fs_next)
    {
	typval_T    *ଗଦା = funcstack->fs_ga.ga_data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

	ପାଇଁ (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    ଯଦି (set_ref_in_item(ଗଦା + i, copyID, ଖାଲି, ଖାଲି, ଖାଲି))
		ଫେରନ୍ତୁ TRUE;  // abort
    }
    ଫେରନ୍ତୁ FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
ସ୍ଥିର ectx_T *current_ectx = ଖାଲି;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
in_def_function(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ current_ectx != ଖାଲି;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fill_exec_lval_ମୂଳ(lval_ମୂଳ_T *ମୂଳ)
{
    ectx_T *ectx = current_ectx;
    ଯଦି (ectx != ଖାଲି)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	ଯଦି (ufunc->uf_class != ଖାଲି)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    ଯଦି (tv != ଖାଲି)
	    {
		CLEAR_POINTER(ମୂଳ);
		ମୂଳ->lr_tv = tv;
		copy_tv(ଗଦା_TV_VAR(0), ମୂଳ->lr_tv);
		ମୂଳ->lr_cl_exec = ufunc->uf_class;
		++ମୂଳ->lr_cl_exec->class_refcount;
		ଫେରନ୍ତୁ OK;
	    }
	}
    }

    ଫେରନ୍ତୁ FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(ଶୂନ୍ୟ)
{
    ectx_T *r = current_ectx;

    current_ectx = ଖାଲି;
    ଫେରନ୍ତୁ r;
}

    ଶୂନ୍ୟ
restore_current_ectx(ectx_T *ectx)
{
    ଯଦି (current_ectx != ଖାଲି)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    ସ୍ଥିର list_T *
add_defer_item(ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = ଗଦା_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    ଯଦି (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	ଯଦି (rettv_list_alloc(defer_tv) == FAIL)
	    ଫେରନ୍ତୁ ଖାଲି;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(argcount + 1);
    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_lock = 0;
    ଯଦି (list_ଭର୍ତ୍ତି_tv(defer_l, &listval, defer_l->lv_first) == FAIL)
    {
	list_free(l);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
defer_command(ପୂର୍ଣ୍ଣସଂଖ୍ୟା var_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, ectx_T *ectx)
{
    list_T	*l = add_defer_item(var_idx, argcount, ectx);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    typval_T	*func_tv;

    ଯଦି (l == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    func_tv = ଗଦା_TV_BOT(-argcount - 1);
    ଯଦି (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	ଫେରନ୍ତୁ FAIL;
    }
    list_set_item(l, 0, func_tv);

    ପାଇଁ (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, ଗଦା_TV_BOT(-argcount + i));
    ectx->ec_ଗଦା.ga_len -= argcount + 1;
    ଫେରନ୍ତୁ OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
add_defer_function(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	ଫେରନ୍ତୁ FAIL;
    }

    l = add_defer_item(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    ଯଦି (l == ଖାଲି)
    {
	vim_free(name);
	ଫେରନ୍ତୁ FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_lock = 0;
    func_tv.vval.v_string = name;
    list_set_item(l, 0, &func_tv);

    ପାଇଁ (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, argvars + i);
    ଫେରନ୍ତୁ OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = ଗଦା_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    ଯଦି (defer_tv->v_type != VAR_LIST)
	ଫେରନ୍ତୁ;	 // no function added
    FOR_ALL_LIST_ବସ୍ତୁଗୁଡ଼ିକ(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ARGS];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
	listitem_T  *arg_li = l->lv_first;
	typval_T    *functv = &l->lv_first->li_tv;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount = l->lv_len - 1;

	ଯଦି (functv->vval.v_string == ଖାଲି)
	    // already being called, can happen if function does ":qa"
	    ଜାରି;

	ପାଇଁ (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_next;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = ଖାଲି;
	partial_T	*pt = ଖାଲି;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	ଯଦି (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = ଖାଲି;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    ଯଦି (funcexe.fe_object != ଖାଲି)
		++funcexe.fe_object->obj_refcount;
	}
	ଅନ୍ୟଥା
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = ଖାଲି;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(ଶୂନ୍ୟ)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	ଯଦି (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	ଅନ୍ୟଥା
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
func_return(ectx_T *ectx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*entry;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_dfunc_idx = ଗଦା_TV(ectx->ec_frame_idx
					+ ଗଦା_FRAME_FUNC_OFF)->vval.v_ସଂଖ୍ୟା;
    funclocal_T	*floc;
#ifdef FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_data)
							      + prev_dfunc_idx;

    ଯଦି (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	ଯଦି (dfunc->df_ufunc->uf_profiling
				   || (caller != ଖାଲି && caller->uf_profiling))
	{
	    profile_may_end_func(((profinfo_T *)profile_info_ga.ga_data)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#endif

    ଯଦି (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    ଯଦି (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    entry = estack_କାଢ଼();
    ଯଦି (entry != ଖାଲି)
	current_sctx = entry->es_save_sctx;

    ଯଦି (handle_closure_in_use(ectx, TRUE) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ = ectx->ec_frame_idx - argcount;
    ଯଦି (IS_OBJECT_METHOD(dfunc->df_ufunc))
	--ଶୀର୍ଷ;
    ପାଇଁ (idx = ଶୀର୍ଷ; idx < ectx->ec_frame_idx; ++idx)
	clear_tv(ଗଦା_TV(idx));

    // Clear local variables and temp values, but not the return value.
    ପାଇଁ (idx = ectx->ec_frame_idx + ଗଦା_FRAME_SIZE;
					idx < ectx->ec_ଗଦା.ga_len - 1; ++idx)
	clear_tv(ଗଦା_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_ଗଦା.ga_len - 1;
    ଯଦି (ret_idx == ectx->ec_frame_idx + ଗଦା_FRAME_IDX_OFF)
	ret_idx = 0;

    ଯଦି (ectx->ec_outer_ref != ଖାଲି)
    {
	ଯଦି (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = ଗଦା_TV(ectx->ec_frame_idx
					+ ଗଦା_FRAME_IIDX_OFF)->vval.v_ସଂଖ୍ୟା;
    ectx->ec_instr = (ଶୂନ୍ୟ *)ଗଦା_TV(ectx->ec_frame_idx
				       + ଗଦା_FRAME_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (ଶୂନ୍ୟ *)ଗଦା_TV(ectx->ec_frame_idx
				       + ଗଦା_FRAME_OUTER_OFF)->vval.v_string;
    floc = (ଶୂନ୍ୟ *)ଗଦା_TV(ectx->ec_frame_idx
				   + ଗଦା_FRAME_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_frame_idx = ଗଦା_TV(ectx->ec_frame_idx
				       + ଗଦା_FRAME_IDX_OFF)->vval.v_ସଂଖ୍ୟା;

    ଯଦି (floc == ଖାଲି)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    ଅନ୍ୟଥା
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    ଯଦି (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_ଗଦା.ga_len = ଶୀର୍ଷ + 1;
	*ଗଦା_TV_BOT(-1) = *ଗଦା_TV(ret_idx);
    }
    ଅନ୍ୟଥା
	// Reset the stack to the position before the call.
	ectx->ec_ଗଦା.ga_len = ଶୀର୍ଷ;

    funcdepth_decrement();
    --ex_nesting_level;
    ଫେରନ୍ତୁ OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_prepare(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, typval_T *argvars, ectx_T *ectx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    ପାଇଁ (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *ଗଦା_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    ଯଦି (argcount > 0)
	ectx->ec_ଗଦା.ga_len -= argcount - 1;
    ଅନ୍ୟଥା ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
	ଫେରନ୍ତୁ FAIL;
    ଅନ୍ୟଥା
	++ectx->ec_ଗଦା.ga_len;

    // Default return value is zero.
    tv = ଗଦା_TV_BOT(-1);
    tv->v_type = VAR_ସଂଖ୍ୟା;
    tv->vval.v_ସଂଖ୍ୟା = 0;
    tv->v_lock = 0;

    ଫେରନ୍ତୁ OK;
}

/*
 * Call a builtin function by index.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_bfunc(ପୂର୍ଣ୍ଣସଂଖ୍ୟା func_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    ଅକ୍ଷର	*save_func_name = ectx->ec_where.wt_func_name;

    ଯଦି (call_prepare(argcount, argvars, ectx) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    // Check for void value being passed as an argument.
    ପାଇଁ (idx = 0; idx < argcount; ++idx)
	ଯଦି (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
	    ପାଇଁ (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    ଫେରନ୍ତୁ FAIL;
	}

    ectx->ec_where.wt_func_name = internal_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_internal_func_by_idx(func_idx, argvars, ଗଦା_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    ପାଇଁ (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ଯଦି (did_emsg > did_emsg_before)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    funcexe_T   funcexe;
    funcerror_T	error;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    ଯଦି (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, ଖାଲି)
								       == FAIL)
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (ufunc->uf_def_status == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	ଯଦି (error != FCERR_UNKNOWN)
	{
	    ଯଦି (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    ଅନ୍ୟଥା
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    ଫେରନ୍ତୁ FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	ଯଦି (iptr != ଖାଲି)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	ଫେରନ୍ତୁ call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    ଯଦି (call_prepare(argcount, argvars, ectx) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != ଖାଲି ? selfdict : dict_ଗଦା_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      ଗଦା_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    ପାଇଁ (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ଯଦି (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_found_var);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * If command modifiers were applied restore them.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
may_restore_cmdmod(funclocal_T *funclocal)
{
    ଯଦି (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ଖାଲି;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim9_aborting(ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_uncaught_emsg)
{
    ଫେରନ୍ତୁ uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_by_name(
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    ଯଦି (builtin_function(name, -1))
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା func_idx = find_internal_func(name);

	ଯଦି (func_idx < 0)  // Impossible?
	    ଫେରନ୍ତୁ FAIL;
	ଯଦି (check_internal_func(func_idx, argcount) < 0)
	    ଫେରନ୍ତୁ FAIL;
	ଫେରନ୍ତୁ call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*start_bracket = generic_func_find_open_bracket(name);
    ଯଦି (start_bracket != ଖାଲି)
    {
	cc = *start_bracket;
	*start_bracket = NUL;
    }

    ufunc = find_func(name, FALSE);

    ଯଦି (ufunc == ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା prev_uncaught_emsg = uncaught_emsg;

	ଯଦି (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = find_func(name, FALSE);

	ଯଦି (vim9_aborting(prev_uncaught_emsg))
	{
	    ଯଦି (start_bracket != ଖାଲି)
		*start_bracket = cc;
	    ଫେରନ୍ତୁ FAIL;  // bail out if loading the script caused an error
	}
    }

    ଯଦି (start_bracket != ଖାଲି)
	*start_bracket = cc;

    ଯଦି (ufunc != ଖାଲି)
    {
	ଯଦି (IS_GENERIC_FUNC(ufunc))
	{
	    ଯଦି (start_bracket != ଖାଲି)
		ufunc = find_generic_func(ufunc, name, &start_bracket);
	    ଅନ୍ୟଥା
	    {
		emsg_funcname(e_generic_func_missing_type_args_str, name);
		ufunc = ଖାଲି;
	    }
	    ଯଦି (ufunc == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (start_bracket != ଖାଲି)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    ଫେରନ୍ତୁ FAIL;
	}

	ଯଦି (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    ଫେରନ୍ତୁ FAIL;

	ଫେରନ୍ତୁ call_ufunc(ufunc, ଖାଲି, argcount, ectx, iptr, selfdict);
    }

    ଫେରନ୍ତୁ FAIL;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_partial(
	typval_T    *tv,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount_arg,
	ectx_T	    *ectx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argcount = argcount_arg;
    char_u	*name = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_before = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = FAIL;
    dict_T	*selfdict = ଖାଲି;

    ଯଦି (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

	ଯଦି (pt->pt_obj != ଖାଲି)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		ଫେରନ୍ତୁ FAIL;
	    ପାଇଁ (i = 1; i <= argcount; ++i)
		*ଗଦା_TV_BOT(-i + 1) = *ଗଦା_TV_BOT(-i);

	    typval_T *obj_tv = ଗଦା_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_lock = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_ଗଦା.ga_len;
	}

	ଯଦି (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, pt->pt_argc))
		ଫେରନ୍ତୁ FAIL;
	    ପାଇଁ (i = 1; i <= argcount; ++i)
		*ଗଦା_TV_BOT(-i + pt->pt_argc) = *ଗଦା_TV_BOT(-i);
	    ectx->ec_ଗଦା.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    ପାଇଁ (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], ଗଦା_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	ଯଦି (pt->pt_func != ଖାଲି)
	    ଫେରନ୍ତୁ call_ufunc(pt->pt_func, pt, argcount, ectx, ଖାଲି, selfdict);

	name = pt->pt_name;
    }
    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    ଯଦି (name != ଖାଲି)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = ଖାଲି;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ଯଦି (error != FCERR_NONE)
	    res = FAIL;
	ଅନ୍ୟଥା
	    res = call_by_name(fname, argcount, ectx, ଖାଲି, selfdict);
	vim_free(tofree);
    }

    ଯଦି (res == FAIL)
    {
	ଯଦି (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == ଖାଲି ? (char_u *)"[unknown]" : name);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
error_if_locked(ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock, ଅକ୍ଷର *error)
{
    ଯଦି (lock & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_for_ସଂଖ୍ୟା(typval_T *tv)
{
    ଯଦି (tv->v_type != VAR_ସଂଖ୍ୟା)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_ସଂଖ୍ୟା), vartype_name(tv->v_type));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    flags = ASSIGN_DECL;

    ଯଦି (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, ଖାଲି, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_2string(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_2string_any, ପୂର୍ଣ୍ଣସଂଖ୍ୟା tostring_flags)
{
    ଯଦି (tv->v_type == VAR_STRING)
	ଫେରନ୍ତୁ OK;

    char_u *str;

    ଯଦି (is_2string_any)
    {
	ବାଛ (tv->v_type)
	{
	    ସ୍ଥିତି VAR_SPECIAL:
	    ସ୍ଥିତି VAR_BOOL:
	    ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    ସ୍ଥିତି VAR_FLOAT:
	    ସ୍ଥିତି VAR_DICT:
	    ସ୍ଥିତି VAR_BLOB:	ଭାଙ୍ଗ;

	    ସ୍ଥିତି VAR_LIST:
				ଯଦି (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, ଆକାର(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    ଯଦି (str == ଖାଲି)
					ଫେରନ୍ତୁ FAIL;
				    s = str;
				    ଯେତେବେଳେ ((e = vim_strchr(s, '\n')) != ଖାଲି)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					ଯଦି (p != ଖାଲି)
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
				    ଫେରନ୍ତୁ OK;
				}
				ଯଦି (tostring_flags & TOSTRING_INTERPOLATE)
				    ଭାଙ୍ଗ;
				// FALLTHROUGH
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	to_string_error(tv->v_type);
			ଫେରନ୍ତୁ FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    ଫେରନ୍ତୁ OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_LIST:
	    ଯଦି (tv->vval.v_list == ଖାଲି && sv->sv_type != &t_list_empty)
		(ଶୂନ୍ୟ)rettv_list_alloc(tv);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_TUPLE:
	    ଯଦି (tv->vval.v_tuple == ଖାଲି && sv->sv_type != &t_tuple_empty)
		(ଶୂନ୍ୟ)rettv_tuple_alloc(tv);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_DICT:
	    ଯଦି (tv->vval.v_dict == ଖାଲି && sv->sv_type != &t_dict_empty)
		(ଶୂନ୍ୟ)rettv_dict_alloc(tv);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି VAR_BLOB:
	    ଯଦି (tv->vval.v_blob == ଖାଲି && sv->sv_type != &t_blob_null)
		(ଶୂନ୍ୟ)rettv_blob_alloc(tv);
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
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

    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    ଯଦି (index < 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	clen = 0;

	ପାଇଁ (nbyte = 0; nbyte < slen; ++clen)
	{
	    ଯଦି (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    ଅନ୍ୟଥା ଯଦି (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    ଅନ୍ୟଥା
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + index;
	ଯଦି (nchar < 0)
	    // unlike list: index out of range results in empty string
	    ଫେରନ୍ତୁ ଖାଲି;
    }

    ପାଇଁ (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	ଯଦି (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	ଅନ୍ୟଥା ଯଦି (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	ଅନ୍ୟଥା
	    nbyte += mb_ptr2len(str + nbyte);
    }
    ଯଦି (nbyte >= slen)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    ସ୍ଥିର long
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    ଯଦି (nchar >= 0)
    {
	ଯେତେବେଳେ (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    ଅନ୍ୟଥା
    {
	nbyte = str_len;
	ଯେତେବେଳେ (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_head_off(str, str + nbyte);
	    ++nchar;
	}
	ଯଦି (nchar < 0)
	    ଫେରନ୍ତୁ -1;
    }
    ଫେରନ୍ତୁ (long)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_slice(char_u *str, varnumber_T first, varnumber_T last, ପୂର୍ଣ୍ଣସଂଖ୍ୟା exclusive)
{
    long	start_byte, end_byte;
    size_t	slen;

    ଯଦି (str == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    slen = STRLEN(str);
    start_byte = char_idx2byte(str, slen, first);
    ଯଦି (start_byte < 0)
	start_byte = 0; // first index very negative: use zero
    ଯଦି ((last == -1 && !exclusive) || last == VARNUM_MAX)
	end_byte = (long)slen;
    ଅନ୍ୟଥା
    {
	end_byte = char_idx2byte(str, slen, last);
	ଯଦି (!exclusive && end_byte >= 0 && end_byte < (long)slen)
	    // end index is inclusive
	    end_byte += mb_ptr2len(str + end_byte);
    }

    ଯଦି (start_byte >= (long)slen || end_byte <= start_byte)
	ଫେରନ୍ତୁ ଖାଲି;
    ଫେରନ୍ତୁ vim_strnsave(str + start_byte, end_byte - start_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    ସ୍ଥିର svar_T *
get_script_svar(scriptref_T *sref, ପୂର୍ଣ୍ଣସଂଖ୍ୟା dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? ଖାଲି
			      : ((dfunc_T *)def_functions.ga_data) + dfunc_idx;
    svar_T	    *sv;

    ଯଦି (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	ଯଦି (dfunc != ଖାଲି)
	    semsg(_(e_script_variable_invalid_after_reload_in_function_str),
					 printable_func_name(dfunc->df_ufunc));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_data) + sref->sref_idx;
    ଯଦି (sv->sv_name == ଖାଲି)
    {
	ଯଦି (dfunc != ଖାଲି)
	    emsg(_(e_script_variable_was_deleted));
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	ଯଦି (dfunc != ଖାଲି)
	    emsg(_(e_script_variable_type_changed));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	ଯଦି (dfunc != ଖାଲି)
	    semsg(_(e_item_not_exported_in_script_str), sv->sv_name);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଫେରନ୍ତୁ sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    ସ୍ଥିର char_u *
get_split_sourceline(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା c UNUSED,
	ଶୂନ୍ୟ *cookie,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା indent UNUSED,
	getline_opt_T options UNUSED)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*p;
    char_u		*ଧାଡ଼ି;

    p = vim_strchr(sp->nextline, '\n');
    ଯଦି (p == ଖାଲି)
    {
	ଧାଡ଼ି = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    ଅନ୍ୟଥା
    {
	ଧାଡ଼ି = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    ଫେରନ୍ତୁ ଧାଡ଼ି;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_eval_func(
	char_u	    *name,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    called_emsg_before = called_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    res;

    res = call_by_name(name, argcount, ectx, iptr, ଖାଲି);
    ଯଦି (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = find_var(name, ଖାଲି, FALSE);
	ଯଦି (v == ଖାଲି || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଫେରନ୍ତୁ call_partial(&v->di_tv, argcount, ectx);
    }
    ଫେରନ୍ତୁ res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
fill_partial_and_closure(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ଯଦି (ufunc->uf_flags & FC_CLOSURE)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.out_ଗଦା = &ectx->ec_ଗଦା;
	pt->pt_outer.out_frame_idx = ectx->ec_frame_idx;
	ଯଦି (ectx->ec_outer_ref != ଖାଲି)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.out_up = ectx->ec_outer_ref->or_outer;
	    ଯଦି (ectx->ec_outer_ref->or_partial != ଖାଲି)
	    {
		pt->pt_outer.out_up_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.out_up_partial->pt_refcount;
	    }
	}

	ଯଦି (lvi != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	depth;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    ପାଇଁ (depth = 0; depth < lvi->lvi_depth; ++depth)
	    {
		pt->pt_outer.out_loop[depth].ଗଦା = &ectx->ec_ଗଦା;
		pt->pt_outer.out_loop[depth].var_idx = ectx->ec_frame_idx
			 + ଗଦା_FRAME_SIZE + lvi->lvi_loop[depth].var_idx;
		pt->pt_outer.out_loop[depth].var_ଗଣନା =
					    lvi->lvi_loop[depth].var_ଗଣନା;
	    }
	    pt->pt_outer.out_loop_size = lvi->lvi_depth;
	}
	ଅନ୍ୟଥା
	    pt->pt_outer.out_loop_size = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	ଯଦି (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    ଫେରନ୍ତୁ FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_ଗଦା.ga_data) + ectx->ec_frame_idx
		       + ଗଦା_FRAME_SIZE + dfunc->df_varcount)->vval.v_ସଂଖ୍ୟା;

	((partial_T **)ectx->ec_funcrefs.ga_data)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    ଫେରନ୍ତୁ OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exec_command(isn_T *iptr, char_u *cmd_string)
{
    source_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_FIELD(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    ଯଦି (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_load_script(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sid, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *loaded)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);

    ଯଦି (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	ଯଦି (do_source(si->sn_name, FALSE, DOSO_NONE, ଖାଲି) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    ଫେରନ୍ତୁ FAIL;
	}
    }
    ଫେରନ୍ତୁ OK;
}

// used for v_instr of typval of VAR_INSTR
ସଂରଚନା instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା subs_ପଦସମଷ୍ଟି_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		subs_status;
} subs_ପଦସମଷ୍ଟି_T;

// Set when calling do_debug().
ସ୍ଥିର ectx_T	*debug_context = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	debug_var_ଗଣନା;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    varargs_off;

    ଯଦି (ectx == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    ପାଇଁ (idx = debug_var_ଗଣନା - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_names.ga_data)[idx];

	// the variable name may be NULL when not available in this block
	ଯଦି (varname != ଖାଲି && STRCMP(varname, name) == 0)
	    ଫେରନ୍ତୁ ଗଦା_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == ଖାଲି ? 0 : 1;
    ପାଇଁ (idx = 0; idx < ufunc->uf_args.ga_len; ++idx)
	ଯଦି (STRCMP(((char_u **)(ufunc->uf_args.ga_data))[idx], name) == 0)
	    ଫେରନ୍ତୁ ଗଦା_TV(ectx->ec_frame_idx - ufunc->uf_args.ga_len
							  - varargs_off + idx);
    ଯଦି (ufunc->uf_va_name != ଖାଲି && STRCMP(ufunc->uf_va_name, name) == 0)
	ଫେରନ୍ତୁ ଗଦା_TV(ectx->ec_frame_idx - 1);

    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
may_break_in_function(ufunc_T *ufunc)
{
    ଫେରନ୍ତୁ ufunc->uf_has_breakpoint || debug_has_ପଦସମଷ୍ଟି_breakpoint();
}

    ସ୍ଥିର ଶୂନ୍ୟ
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*ଧାଡ଼ି;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		end_lnum = iptr->isn_lnum;
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		lnum;

    ଯଦି (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	ଯଦି (!may_break_in_function(ufunc))
	    ଫେରନ୍ତୁ;

	// check for the next breakpoint if needed
	breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	ଯଦି (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    ଫେରନ୍ତୁ;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_ଗଣନା = iptr->isn_arg.debug.dbg_var_names_len;

    ପାଇଁ (ni = iptr + 1; ni->isn_type != ISN_FINISH; ++ni)
	ଯଦି (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    end_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    ଭାଙ୍ଗ;
	}

    ଯଦି (end_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, ଆକାର(char_u *), 10);
	ପାଇଁ (lnum = iptr->isn_lnum; lnum < end_lnum
				     && lnum <= ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_data)[lnum - 1];

	    ଯଦି (p == ଖାଲି)
		ଜାରି;  // left over from continuation line
	    p = skipwhite(p);
	    ଯଦି (*p == '#')
		ଭାଙ୍ଗ;
	    ଯଦି (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_data))[ga.ga_len++] = p;
	    ଯଦି (STRNCMP(p, "def ", 4) == 0)
		ଭାଙ୍ଗ;
	}
	ଧାଡ଼ି = ga_concat_strings(&ga, "  ");
	vim_free(ga.ga_data);
    }
    ଅନ୍ୟଥା
	ଧାଡ଼ି = ((char_u **)ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_data)[iptr->isn_lnum - 1];

    do_debug(ଧାଡ଼ି == ଖାଲି ? (char_u *)"[empty]" : ଧାଡ଼ି);
    debug_context = ଖାଲି;

    ଯଦି (end_lnum > iptr->isn_lnum)
	vim_free(ଧାଡ଼ି);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    ଯଦି (lhs_list->lv_type == ଖାଲି || lhs_list->lv_type->tt_member == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    ଫେରନ୍ତୁ check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = ଗଦା_TV_BOT(-2);
    long	lidx = 0;
    typval_T	*tv_dest = ଗଦା_TV_BOT(-1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_rhs_type = FALSE;

    ଯଦି (tv_idx->v_type == VAR_ସଂଖ୍ୟା)
	lidx = (long)tv_idx->vval.v_ସଂଖ୍ୟା;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = ଗଦା_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    ଯଦି (dest_type == VAR_OBJECT && tv_dest->vval.v_object == ଖାଲି)
    {
	emsg(_(e_using_null_object));
	status = FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	ଯଦି (dest_type == VAR_DICT)
	    status = do_2string(tv_idx, TRUE, FALSE);
	ଅନ୍ୟଥା ଯଦି (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    ଯଦି (obj == ଖାଲି)
	    {
		emsg(_(e_using_null_object));
		status = FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		ଯଦି (m != ଖାଲି)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    ଯଦି ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == ଖାଲି ||
			     !class_instance_of(ufunc->uf_class, cl)))
		    {
			ଅକ୍ଷର *ସନ୍ଦେଶ = (m->ocm_access == VIM_ACCESS_PRIVATE)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(ସନ୍ଦେଶ, m->ocm_name.string, 0, cl);
			status = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    ଅନ୍ୟଥା ଯଦି (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			status = FAIL;
		    ଅନ୍ୟଥା
			lidx = m_idx;
		}
		ଅନ୍ୟଥା
		{
		    member_not_found_ସନ୍ଦେଶ(cl, VAR_OBJECT, member, 0);
		    status = FAIL;
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    status = FAIL;
	}
	ଅନ୍ୟଥା ଯଦି (dest_type == VAR_LIST && tv_idx->v_type != VAR_ସଂଖ୍ୟା)
	{
	    emsg(_(e_ସଂଖ୍ୟା_expected));
	    status = FAIL;
	}
    }

    ଯଦି (status == OK)
    {
	ଯଦି (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    ଯଦି (list == ଖାଲି)
	    {
		emsg(_(e_list_not_set));
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    ଯଦି (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_index_out_of_range_nr), lidx);
		ଫେରନ୍ତୁ FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    ଯଦି (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		ଫେରନ୍ତୁ FAIL;

	    ଯଦି (lidx < list->lv_len)
	    {
		listitem_T *li = list_find(list, lidx);
		ଯଦି (li == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		ଯଦି (error_if_locked(li->li_tv.v_lock,
					     e_cannot_change_locked_list_item))
		    ଫେରନ୍ତୁ FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (error_if_locked(list->lv_lock, e_cannot_change_locked_list))
		    ଫେରନ୍ତୁ FAIL;
		// append to list, only fails when out of memory
		ଯଦି (list_append_tv(list, tv) == FAIL)
		    ଫେରନ୍ତୁ NOTDONE;
		clear_tv(tv);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (dest_type == VAR_DICT)
	{
	    char_u		*ଚାବି = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    ଯଦି (dict == ଖାଲି)
	    {
		emsg(_(e_dictionary_not_set));
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (ଚାବି == ଖାଲି)
		ଚାବି = (char_u *)"";
	    di = dict_find(dict, ଚାବି, -1);
	    ଯଦି (di != ଖାଲି)
	    {
		ଯଦି (error_if_locked(di->di_tv.v_lock,
						    e_cannot_change_dict_item))
		    ଫେରନ୍ତୁ FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (error_if_locked(dict->dv_lock, e_cannot_change_dict))
		    ଫେରନ୍ତୁ FAIL;
		// add to dict, only fails when out of memory
		ଯଦି (dict_add_tv(dict, (ଅକ୍ଷର *)ଚାବି, tv) == FAIL)
		    ଫେରନ୍ତୁ NOTDONE;
		clear_tv(tv);
	    }
	}
	ଅନ୍ୟଥା ଯଦି (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    error = FALSE;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    len;

	    ଯଦି (blob == ଖାଲି)
	    {
		emsg(_(e_blob_not_set));
		ଫେରନ୍ତୁ FAIL;
	    }
	    len = blob_len(blob);
	    ଯଦି (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    ଯଦି (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), lidx);
		ଫେରନ୍ତୁ FAIL;
	    }
	    ଯଦି (ମୂଲ୍ୟ_check_lock(blob->bv_lock, (char_u *)"blob", FALSE))
		ଫେରନ୍ତୁ FAIL;
	    nr = tv_get_ସଂଖ୍ୟା_chk(tv, &error);
	    ଯଦି (error)
		ଫେରନ୍ତୁ FAIL;
	    ଯଦି (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_ମୂଲ୍ୟ_for_blob_nr), nr);
		ଫେରନ୍ତୁ FAIL;
	    }
	    blob_set_append(blob, lidx, nr);
	}
	ଅନ୍ୟଥା ଯଦି (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    ଯଦି (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		ଯଦି (itf != ଖାଲି)
		    // convert interface member index to class member index
		    lidx = object_index_from_itf_index(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    ଅନ୍ୟଥା
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	ଅନ୍ୟଥା ଯଦି (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    status = FAIL;
	}
	ଅନ୍ୟଥା
	{
	    status = FAIL;
	    semsg(_(e_cannot_index_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ଗଦା.ga_len -= 3;
    ଯଦି (status == FAIL)
    {
	clear_tv(tv);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Store a value in a list or blob range.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = ଗଦା_TV_BOT(-3);
    typval_T	*tv_idx2 = ଗଦା_TV_BOT(-2);
    typval_T	*tv_dest = ଗଦା_TV_BOT(-1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = ଗଦା_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    ଯଦି (tv_dest->v_type == VAR_LIST)
    {
	long	    n1;
	long	    n2;
	listitem_T  *li1;

	n1 = (long)tv_get_ସଂଖ୍ୟା_chk(tv_idx1, ଖାଲି);
	ଯଦି (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_ସଂଖ୍ୟା == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	ଅନ୍ୟଥା
	    n2 = (long)tv_get_ସଂଖ୍ୟା_chk(tv_idx2, ଖାଲି);

	li1 = check_range_index_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	ଯଦି (li1 == ଖାଲି)
	    status = FAIL;
	ଅନ୍ୟଥା
	{
	    status = check_range_index_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    ଯଦି (status != FAIL)
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
    ଅନ୍ୟଥା ଯଦି (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	long	    bloblen;

	n1 = tv_get_ସଂଖ୍ୟା_chk(tv_idx1, ଖାଲି);
	ଯଦି (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_ସଂଖ୍ୟା == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	ଅନ୍ୟଥା
	    n2 = tv_get_ସଂଖ୍ୟା_chk(tv_idx2, ଖାଲି);
	bloblen = blob_len(tv_dest->vval.v_blob);

	ଯଦି (check_blob_index(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    status = FAIL;
	ଅନ୍ୟଥା
	    status = blob_set_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    ଅନ୍ୟଥା
    {
	status = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ଗଦା.ga_len -= 4;
    clear_tv(tv);

    ଫେରନ୍ତୁ status;
}

/*
 * Unlet item in list or dict variable.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = ଗଦା_TV_BOT(-2);
    typval_T	*tv_dest = ଗଦା_TV_BOT(-1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    ଯଦି (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	ଯଦି (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_ସଂଖ୍ୟା)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    status = FAIL;
	}
	ଅନ୍ୟଥା
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*ଚାବି;
	    dictitem_T  *di = ଖାଲି;

	    ଯଦି (d != ଖାଲି && ମୂଲ୍ୟ_check_lock(
				      d->dv_lock, ଖାଲି, FALSE))
		status = FAIL;
	    ଅନ୍ୟଥା
	    {
		ଯଦି (tv_idx->v_type == VAR_STRING)
		{
		    ଚାବି = tv_idx->vval.v_string;
		    ଯଦି (ଚାବି == ଖାଲି)
			ଚାବି = (char_u *)"";
		}
		ଅନ୍ୟଥା
		{
		    ଚାବି = tv_get_string(tv_idx);
		}
		ଯଦି (d != ଖାଲି)
		    di = dict_find(d, ଚାବି, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ଚାବି));
		ଯଦି (di == ଖାଲି)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_ଚାବି_not_present_in_dictionary_str), ଚାବି);
		    status = FAIL;
		}
		ଅନ୍ୟଥା ଯଦି (var_check_fixed(di->di_flags,
						   ଖାଲି, FALSE)
			|| var_check_ro(di->di_flags,
						  ଖାଲି, FALSE))
		    status = FAIL;
		ଅନ୍ୟଥା
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    ଅନ୍ୟଥା ଯଦି (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	ଯଦି (check_for_ସଂଖ୍ୟା(tv_idx) == FAIL)
	{
	    status = FAIL;
	}
	ଅନ୍ୟଥା
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n = (long)tv_idx->vval.v_ସଂଖ୍ୟା;

	    ଯଦି (l != ଖାଲି && ମୂଲ୍ୟ_check_lock(
				      l->lv_lock, ଖାଲି, FALSE))
		status = FAIL;
	    ଅନ୍ୟଥା
	    {
		listitem_T	*li = list_find(l, n);

		ଯଦି (li == ଖାଲି)
		{
		    semsg(_(e_list_index_out_of_range_nr), n);
		    status = FAIL;
		}
		ଅନ୍ୟଥା
		    listitem_remove(l, li);
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	status = FAIL;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_ଗଦା.ga_len -= 2;

    ଫେରନ୍ତୁ status;
}

/*
 * Unlet a range of items in a list variable.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = ଗଦା_TV_BOT(-3);
    typval_T	*tv_idx2 = ଗଦା_TV_BOT(-2);
    typval_T	*tv_dest = ଗଦା_TV_BOT(-1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;

    ଯଦି (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	ଯଦି (check_for_ସଂଖ୍ୟା(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_ସଂଖ୍ୟା(tv_idx2) == FAIL))
	{
	    status = FAIL;
	}
	ଅନ୍ୟଥା
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n1 = (long)tv_idx1->vval.v_ସଂଖ୍ୟା;
	    long	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (long)tv_idx2->vval.v_ସଂଖ୍ୟା;
	    listitem_T	*li;

	    li = list_find_index(l, &n1);
	    ଯଦି (li == ଖାଲି)
	    {
		semsg(_(e_list_index_out_of_range_nr),
						 (long)tv_idx1->vval.v_ସଂଖ୍ୟା);
		status = FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		ଯଦି (n1 < 0)
		    n1 = list_idx_of_item(l, li);
		ଯଦି (n2 < 0)
		{
		    listitem_T *li2 = list_find(l, n2);

		    ଯଦି (li2 == ଖାଲି)
		    {
			semsg(_(e_list_index_out_of_range_nr), n2);
			status = FAIL;
		    }
		    ଅନ୍ୟଥା
			n2 = list_idx_of_item(l, li2);
		}
		ଯଦି (status != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_index_out_of_range_nr), n2);
		    status = FAIL;
		}
		ଯଦି (status != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	status = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_ଗଦା.ga_len -= 3;

    ଫେରନ୍ତୁ status;
}

/*
 * Top of a for loop.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		jump = FALSE;
    typval_T	*ltv = ଗଦା_TV_BOT(-1);
    typval_T	*idxtv =
		   ଗଦା_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
	ଫେରନ୍ତୁ FAIL;
    ଯଦି (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_ସଂଖ୍ୟା;
	ଯଦି (list == ଖାଲି
		       || idxtv->vval.v_ସଂଖ୍ୟା >= list->lv_len)
	{
	    jump = TRUE;
	}
	ଅନ୍ୟଥା ଯଦି (list->lv_first == &range_list_item)
	{
	    // non-materialized range() list
	    tv = ଗଦା_TV_BOT(0);
	    tv->v_type = VAR_ସଂଖ୍ୟା;
	    tv->v_lock = 0;
	    tv->vval.v_ସଂଖ୍ୟା = list_find_nr(
			     list, idxtv->vval.v_ସଂଖ୍ୟା, ଖାଲି);
	    ++ectx->ec_ଗଦା.ga_len;
	}
	ଅନ୍ୟଥା
	{
	    listitem_T *li = list_find(list,
					 idxtv->vval.v_ସଂଖ୍ୟା);
	    ଯଦି (li == ଖାଲି)
		ଫେରନ୍ତୁ FAIL;
	    copy_tv(&li->li_tv, ଗଦା_TV_BOT(0));
	    ++ectx->ec_ଗଦା.ga_len;
	}
    }
    ଅନ୍ୟଥା ଯଦି (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_ସଂଖ୍ୟା;
	ଯଦି (tuple == ଖାଲି || idxtv->vval.v_ସଂଖ୍ୟା >= TUPLE_LEN(tuple))
	    jump = TRUE;
	ଅନ୍ୟଥା
	{
	    copy_tv(TUPLE_ITEM(tuple, idxtv->vval.v_ସଂଖ୍ୟା), ଗଦା_TV_BOT(0));
	    ++ectx->ec_ଗଦା.ga_len;
	}
    }
    ଅନ୍ୟଥା ଯଦି (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_ସଂଖ୍ୟା;
	ଯଦି (str == ଖାଲି || str[idxtv->vval.v_ସଂଖ୍ୟା] == NUL)
	{
	    jump = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	clen = mb_ptr2len(str + idxtv->vval.v_ସଂଖ୍ୟା);

	    // Push the next character from the string.
	    tv = ଗଦା_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_ସଂଖ୍ୟା, clen);
	    ++ectx->ec_ଗଦା.ga_len;
	    idxtv->vval.v_ସଂଖ୍ୟା += clen - 1;
	}
    }
    ଅନ୍ୟଥା ଯଦି (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	ଯଦି (idxtv->vval.v_ସଂଖ୍ୟା == -1 && blob != ଖାଲି)
	{
	    ltv->v_lock = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_ସଂଖ୍ୟା;
	ଯଦି (blob == ଖାଲି || idxtv->vval.v_ସଂଖ୍ୟା >= blob_len(blob))
	{
	    jump = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    // Push the next byte from the blob.
	    tv = ଗଦା_TV_BOT(0);
	    tv->v_type = VAR_ସଂଖ୍ୟା;
	    tv->vval.v_ସଂଖ୍ୟା = blob_get(blob,
					 idxtv->vval.v_ସଂଖ୍ୟା);
	    ++ectx->ec_ଗଦା.ga_len;
	}
    }
    ଅନ୍ୟଥା
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_end;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    ଅନ୍ୟଥା
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = ଗଦା_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_ସଂଖ୍ୟା = ectx->ec_funcrefs.ga_len;
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
ସ୍ଥିର loopvars_T *first_loopvars = ଖାଲି;

    ସ୍ଥିର ଶୂନ୍ୟ
add_loopvars_to_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    ଯଦି (first_loopvars != ଖାଲି)
	first_loopvars->lvs_prev = loopvars;
    loopvars->lvs_next = first_loopvars;
    loopvars->lvs_prev = ଖାଲି;
    first_loopvars = loopvars;
}

    ସ୍ଥିର ଶୂନ୍ୟ
remove_loopvars_from_list(loopvars_T *loopvars)
{
    ଯଦି (loopvars->lvs_prev == ଖାଲି)
	first_loopvars = loopvars->lvs_next;
    ଅନ୍ୟଥା
	loopvars->lvs_prev->lvs_next = loopvars->lvs_next;
    ଯଦି (loopvars->lvs_next != ଖାଲି)
	loopvars->lvs_next->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = ଗଦା_TV_VAR(endloop->end_funcref_idx);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_closure_ଗଣନା = tv_refcount->vval.v_ସଂଖ୍ୟା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth = endloop->end_depth;
    garray_T	*gap = &ectx->ec_funcrefs;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		closure_in_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *ଗଦା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    ପାଇଁ (idx = prev_closure_ଗଣନା; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	ଯଦି (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା refcount = pt->pt_refcount;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    ପାଇଁ (i = 0; i < endloop->end_var_ଗଣନା; ++i)
	    {
		typval_T *stv = ଗଦା_TV_VAR(endloop->end_var_idx + i);

		ଯଦି (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ଯଦି (refcount > 1)
	    {
		closure_in_use = TRUE;
		ଭାଙ୍ଗ;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    ଯଦି (!closure_in_use)
	ଫେରନ୍ତୁ OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    ଯଦି (loopvars == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    loopvars->lvs_ga.ga_len = endloop->end_var_ଗଣନା;
    ଗଦା = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_data = ଗଦା;
    ଯଦି (ଗଦା == ଖାଲି)
    {
	vim_free(loopvars);
	ଫେରନ୍ତୁ FAIL;
    }
    add_loopvars_to_list(loopvars);

    // Move the variable values.
    ପାଇଁ (idx = 0; idx < endloop->end_var_ଗଣନା; ++idx)
    {
	typval_T *tv = ଗଦା_TV_VAR(endloop->end_var_idx + idx);

	*(ଗଦା + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    ପାଇଁ (idx = prev_closure_ଗଣନା; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	ଯଦି (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == ଖାଲି)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[depth] = loopvars;

	    pt->pt_outer.out_loop[depth].ଗଦା = &loopvars->lvs_ga;
	    pt->pt_outer.out_loop[depth].var_idx -=
		  ectx->ec_frame_idx + ଗଦା_FRAME_SIZE + endloop->end_var_idx;
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
loopvars_check_refcount(loopvars_T *loopvars)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    done = 0;
	typval_T	*ଗଦା = gap->ga_data;

    ଯଦି (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	ଫେରନ୍ତୁ FALSE;
    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_data) + i;

	ଯଦି (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != ଖାଲି
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    depth;

	    ପାଇଁ (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		ଯଦି (tv->vval.v_partial->pt_loopvars[depth] == loopvars)
		    ++done;
	}
    }
    ଯଦି (done != loopvars->lvs_min_refcount)
	ଫେରନ୍ତୁ FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    ଗଦା = gap->ga_data;
    ପାଇଁ (i = 0; i < gap->ga_len; ++i)
	clear_tv(ଗଦା + i);
    vim_free(ଗଦା);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    ଫେରନ୍ତୁ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
set_ref_in_loopvars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    loopvars_T *loopvars;

    ପାଇଁ (loopvars = first_loopvars; loopvars != ଖାଲି;
						 loopvars = loopvars->lvs_next)
    {
	typval_T    *ଗଦା = loopvars->lvs_ga.ga_data;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

	ପାଇଁ (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    ଯଦି (set_ref_in_item(ଗଦା + i, copyID, ଖାଲି, ଖାଲି, ଖାଲି))
		ଫେରନ୍ତୁ TRUE;  // abort
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
load_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = ଖାଲି;
    hashtab_T	*ht = ଖାଲି;
    ଅକ୍ଷର	namespace;

    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
	ଫେରନ୍ତୁ NOTDONE;
    ବାଛ (isn_type)
    {
	ସ୍ଥିତି ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  // Cannot reach here
	    ଫେରନ୍ତୁ NOTDONE;
    }
    di = find_var_in_ht(ht, 0, iptr->isn_arg.string, TRUE);

    ଯଦି (di == ଖାଲି)
    {
	ଯଦି (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = find_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    ଯଦି (ufunc != ଖାଲି)
	    {
		typval_T    *tv = ଗଦା_TV_BOT(0);

		++ectx->ec_ଗଦା.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		ଯଦି (tv->vval.v_string == ଖାଲି)
		    ଫେରନ୍ତୁ FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		ଫେରନ୍ତୁ OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	ଯଦି (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != ଖାଲି)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_item_not_found_in_script_str), iptr->isn_arg.string);
	ଅନ୍ୟଥା
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = ଗଦା_TV_BOT(0);
	tv->v_type = VAR_ସଂଖ୍ୟା;
	tv->v_lock = 0;
	tv->vval.v_ସଂଖ୍ୟା = 0;
	++ectx->ec_ଗଦା.ga_len;
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
    {
	copy_tv(&di->di_tv, ଗଦା_TV_BOT(0));
	++ectx->ec_ଗଦା.ga_len;
    }
    ଫେରନ୍ତୁ OK;
}


    ସ୍ଥିର ଶୂନ୍ୟ
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, ଆକାର(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    ଅକ୍ଷର *tofree = ଖାଲି;
    ଅକ୍ଷର *typename = type_name(type, &tofree);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = ଖାଲି;
    typval_T	mtv;

    ଯଦି (is_object)
    {
	obj = tv->vval.v_object;
	ଯଦି (obj == ଖାଲି)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    ଫେରନ୍ତୁ FAIL;
	}
	tv_cl = obj->obj_class;
    }
    ଅନ୍ୟଥା
    {
	tv_cl = tv->vval.v_class;
	ଯଦି (tv_cl == ଖାଲି)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		namelen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(iptr->isn_arg.string);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_did_emsg = did_emsg;

    ଯଦି (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି (did_emsg != save_did_emsg)
	ଫେରନ୍ତୁ FAIL;

    // could be a class or instance method
    ufunc_T	*oc_method;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oc_method_idx;

    oc_method = method_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_method_idx);
    ଯଦି (oc_method == ଖାଲି)
    {
	ଅକ୍ଷର	*ସନ୍ଦେଶ;

	SOURCING_LNUM = iptr->isn_lnum;
	ଯଦି (is_object)
	    ସନ୍ଦେଶ = e_variable_not_found_on_object_str_str;
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ = e_class_variable_str_not_found_in_class_str;
	semsg(_(ସନ୍ଦେଶ), iptr->isn_arg.string, tv_cl->class_name.string);
	ଫେରନ୍ତୁ FAIL;
    }

    // Protected methods are not accessible outside the class
    ଯଦି (*oc_method->uf_name == '_'
			&& !class_instance_of(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_method_str), oc_method->uf_name);
	ଫେରନ୍ତୁ FAIL;
    }

    // Create a partial for the instance or class method
    ଯଦି (obj_method_to_partial_tv(is_object ? obj : ଖାଲି, oc_method, tv)
								== FAIL)
	ଫେରନ୍ତୁ FAIL;

    ଫେରନ୍ତୁ OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    ସ୍ଥିର ଶୂନ୍ୟ
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fixindent)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*ପଦସମଷ୍ଟି = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir = FORWARD;

    ଯଦି (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = ଗଦା_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_ସଂଖ୍ୟା;
	ଯଦି (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_ଗଦା.ga_len;
    }
    ଅନ୍ୟଥା ଯଦି (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    ଅନ୍ୟଥା ଯଦି (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	ଯଦି (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    ଯଦି (regname == '=')
    {
	tv = ଗଦା_TV_BOT(-1);
	ଯଦି (tv->v_type == VAR_STRING)
	    ପଦସମଷ୍ଟି = tv->vval.v_string;
	ଅନ୍ୟଥା
	{
	    ପଦସମଷ୍ଟି = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_ଗଦା.ga_len;
    }
    check_cursor();

    ଯଦି (fixindent)
	do_put(regname, ପଦସମଷ୍ଟି, dir, 1L, PUT_ଧାଡ଼ି|PUT_CURSLINE|PUT_FIXINDENT);
    ଅନ୍ୟଥା
	do_put(regname, ପଦସମଷ୍ଟି, dir, 1L, PUT_ଧାଡ଼ି|PUT_CURSLINE);
    vim_free(ପଦସମଷ୍ଟି);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = iptr->isn_arg.unpack.unp_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    l = tv->vval.v_list;
    ଯଦି (l == ଖାଲି
	    || l->lv_len < (semicolon ? ଗଣନା_2 - 1 : ଗଣନା_2))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_ମୂଲ୍ୟ_does_not_have_enough_ବସ୍ତୁଗୁଡ଼ିକ));
	ଫେରନ୍ତୁ EXEC_FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (!semicolon && l->lv_len > ଗଣନା_2)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_ମୂଲ୍ୟ_has_more_ବସ୍ତୁଗୁଡ଼ିକ_than_targets));
	ଫେରନ୍ତୁ EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, ଗଣନା_2 - 1))
	ଫେରନ୍ତୁ EXEC_DONE;
    ectx->ec_ଗଦା.ga_len += ଗଣନା_2 - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ଯଦି (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(l->lv_len - ଗଣନା_2 + 1);

	ଯଦି (rem_list == ଖାଲି)
	    ଫେରନ୍ତୁ EXEC_DONE;
	tv = ଗଦା_TV_BOT(-ଗଣନା_2);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	li = l->lv_first;
	ପାଇଁ (i = 0; i < ଗଣନା_2 - 1; ++i)
	    li = li->li_next;
	ପାଇଁ (i = 0; li != ଖାଲି; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_set_item(rem_list, i, &tvcopy);
	    li = li->li_next;
	}
	--ଗଣନା_2;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_first;
    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
    {
	tv = ଗଦା_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_next;
    }

    list_unref(l);

    ଫେରନ୍ତୁ EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = iptr->isn_arg.unpack.unp_ଗଣନା;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    tuple = tv->vval.v_tuple;
    ଯଦି (tuple == ଖାଲି
	    || TUPLE_LEN(tuple) < (semicolon ? ଗଣନା_2 - 1 : ଗଣନା_2))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_more_targets_than_tuple_ବସ୍ତୁଗୁଡ଼ିକ));
	ଫେରନ୍ତୁ EXEC_FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (!semicolon && TUPLE_LEN(tuple) > ଗଣନା_2)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_ବସ୍ତୁଗୁଡ଼ିକ));
	ଫେରନ୍ତୁ EXEC_FAIL;
    }

    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, ଗଣନା_2 - 1))
	ଫେରନ୍ତୁ EXEC_DONE;
    ectx->ec_ଗଦା.ga_len += ଗଣନା_2 - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ଯଦି (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(TUPLE_LEN(tuple) - ଗଣନା_2 + 1);

	ଯଦି (rem_list == ଖାଲି)
	    ଫେରନ୍ତୁ EXEC_DONE;
	tv = ଗଦା_TV_BOT(-ଗଣନା_2);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	ପାଇଁ (i = ଗଣନା_2 - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ITEM(tuple, i), &tvcopy);
	    list_set_item(rem_list, i - (ଗଣନା_2 - 1), &tvcopy);
	}
	--ଗଣନା_2;
    }

    // Produce the values in reverse order, first item last.
    ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
    {
	tv = ଗଦା_TV_BOT(-i - 1);
	copy_tv(TUPLE_ITEM(tuple, i), tv);
    }

    tuple_unref(tuple);

    ଫେରନ୍ତୁ EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exec_instructions(ectx_T *ectx)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_trylevel_at_start = ectx->ec_trylevel_at_start;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dict_ଗଦା_len_at_start = dict_ଗଦା.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_start = trylevel;

    ପାଇଁ (;;)
    {
	ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  breakcheck_ଗଣନା = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	ଯଦି (unlikely(++breakcheck_ଗଣନା >= 100))
	{
	    ଧାଡ଼ି_breakcheck();
	    breakcheck_ଗଣନା = 0;
	}
	ଯଦି (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    ଯଦି (throw_exception("Vim:Interrupt", ET_INTERRUPT, ଖାଲି) == FAIL)
		ଯାଅ theend;
	    did_throw = TRUE;
	}

	ଯଦି (unlikely(did_emsg && ସନ୍ଦେଶ_list != ଖାଲି && *ସନ୍ଦେଶ_list != ଖାଲି))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    ଯଦି (throw_exception(*ସନ୍ଦେଶ_list, ET_ERROR, ଖାଲି) == FAIL)
		ଯାଅ theend;
	    did_throw = TRUE;
	    *ସନ୍ଦେଶ_list = ଖାଲି;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    ଯଦି (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		ଯଦି (trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		{
		    ଯଦି (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			finish_exception(caught_ଗଦା);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	ଯଦି (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = ଖାଲି;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		index = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    ଯେତେବେଳେ (index > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_data) + index - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		ଯଦି (!trycmd->tcd_in_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    ଭାଙ୍ଗ;
		// In the catch and finally block of this try we have to go up
		// one level.
		--index;
		trycmd = ଖାଲି;
	    }
	    ଯଦି (trycmd != ଖାଲି && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
	    {
		ଯଦି (trycmd->tcd_in_catch)
		{
		    ଯଦି (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    ଅନ୍ୟଥା
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		ଅନ୍ୟଥା
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_in_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    ଅନ୍ୟଥା
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    ଯାଅ theend;
		tv = ଗଦା_TV_BOT(0);
		tv->v_type = VAR_ସଂଖ୍ୟା;
		tv->vval.v_ସଂଖ୍ୟା = 0;
		++ectx->ec_ଗଦା.ga_len;
		ଯଦି (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    ଯଦି (handle_closure_in_use(ectx, FALSE) == FAIL)
			ଯାଅ theend;
		    ଯାଅ done;
		}

		ଯଦି (func_return(ectx) == FAIL)
		    ଯାଅ theend;
	    }
	    ଜାରି;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	ବାଛ (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    ସ୍ଥିତି ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = ଗଦା_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		ଭାଙ୍ଗ;

	    // execute Ex command line
	    ସ୍ଥିତି ISN_EXEC:
		ଯଦି (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    ଯାଅ on_error;
		ଭାଙ୍ଗ;

	    // execute Ex command line split at NL characters.
	    ସ୍ଥିତି ISN_EXEC_SPLIT:
		{
		    source_cookie_T cookie;
		    char_u	    *ଧାଡ଼ି;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    ଧାଡ଼ି = get_split_sourceline(0, &cookie, 0, 0);
		    ଯଦି (do_cmdline(ଧାଡ଼ି,
				get_split_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(ଧାଡ଼ି);
			ଯାଅ on_error;
		    }
		    vim_free(ଧାଡ଼ି);
		}
		ଭାଙ୍ଗ;

	    // execute Ex command line that is only a range
	    ସ୍ଥିତି ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    ଅକ୍ଷର	*error = ଖାଲି;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = CMD_SIZE;
		    ea.addr_type = ADDR_ଧାଡ଼ିଗୁଡ଼ିକ;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_address(&ea, &error, FALSE);
		    ଯଦି (ea.cmd == ଖାଲି)
			ଯାଅ on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    ଯଦି (error != ଖାଲି)
		    {
			emsg(error);
			ଯାଅ on_error;
		    }
		}
		ଭାଙ୍ଗ;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    ସ୍ଥିତି ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    res;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    save_flags = cmdmod.cmod_flags;

		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    tv = ଗଦା_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, ଖାଲି, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    ଯଦି (res == FAIL)
			ଯାଅ on_error;
		    ++ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    // push typeval VAR_INSTR with instructions to be executed
	    ସ୍ଥିତି ISN_INSTR:
		{
		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    tv = ଗଦା_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    ଯଦି (tv->vval.v_instr == ଖାଲି)
			ଯାଅ on_error;
		    ++ectx->ec_ଗଦା.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_SOURCE:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (may_load_script((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)iptr->isn_arg.ସଂଖ୍ୟା, &notused)
								       == FAIL)
			ଯାଅ on_error;
		}
		ଭାଙ୍ଗ;

	    // execute :substitute with an expression
	    ସ୍ଥିତି ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    source_cookie_T	cookie;
		    ସଂରଚନା subs_ପଦସମଷ୍ଟି_S	*save_instr = substitute_instr;
		    ସଂରଚନା subs_ପଦସମଷ୍ଟି_S	subs_instr;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			res;

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

		    ଯଦି (res == FAIL || did_emsg
					     || subs_instr.subs_status == FAIL)
			ଯାଅ on_error;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_FINISH:
		ଯାଅ done;

	    ସ୍ଥିତି ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		ଯଦି (alloc_redir_lval() == FAIL)
		    ଯାଅ on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    {
			vim_free(res);
			ଯାଅ theend;
		    }
		    tv = ଗଦା_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		force_abort = TRUE;
		ଯଦି (trigger_cexpr_autocmd(iptr->isn_arg.ସଂଖ୍ୟା) == FAIL)
		    ଯାଅ on_error;
		force_abort = FALSE;
#endif
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    exarg_T ea;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    res;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_ଗଦା.ga_len;
		    tv = ଗଦା_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    ଯଦି (res == FAIL)
			ଯାଅ on_error;
		}
#endif
		ଭାଙ୍ଗ;

	    // execute Ex command from pieces on the stack
	    ସ୍ଥିତି ISN_EXECCONCAT:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ଗଣନା_2 = iptr->isn_arg.ସଂଖ୍ୟା;
		    size_t  len = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    pass;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;
		    char_u  *cmd = ଖାଲି;
		    char_u  *str;

		    ପାଇଁ (pass = 1; pass <= 2; ++pass)
		    {
			ପାଇଁ (i = 0; i < ଗଣନା_2; ++i)
			{
			    tv = ଗଦା_TV_BOT(i - ଗଣନା_2);
			    str = tv->vval.v_string;
			    ଯଦି (str != ଖାଲି && *str != NUL)
			    {
				ଯଦି (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    ଯଦି (pass == 2)
				clear_tv(tv);
			}
			ଯଦି (pass == 1)
			{
			    cmd = alloc(len + 1);
			    ଯଦି (unlikely(cmd == ଖାଲି))
				ଯାଅ theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		ଭାଙ୍ଗ;

	    // execute :echo {string} ...
	    ସ୍ଥିତି ISN_ECHO:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = iptr->isn_arg.echo.echo_ଗଣନା;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	atstart = TRUE;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା needclr = TRUE;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	idx;

		    ପାଇଁ (idx = 0; idx < ଗଣନା_2; ++idx)
		    {
			tv = ଗଦା_TV_BOT(idx - ଗଣନା_2);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    ଯଦି (needclr)
			ସନ୍ଦେଶ_clr_eos();
		    ectx->ec_ଗଦା.ga_len -= ଗଣନା_2;
		}
		ଭାଙ୍ଗ;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    ସ୍ଥିତି ISN_EXECUTE:
	    ସ୍ଥିତି ISN_ECHOMSG:
	    ସ୍ଥିତି ISN_ECHOWINDOW:
	    ସ୍ଥିତି ISN_ECHOCONSOLE:
	    ସ୍ଥିତି ISN_ECHOERR:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		failed = FALSE;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

		    ଯଦି (iptr->isn_type == ISN_ECHOWINDOW)
			ଗଣନା_2 = iptr->isn_arg.echowin.ewin_ଗଣନା;
		    ଅନ୍ୟଥା
			ଗଣନା_2 = iptr->isn_arg.ସଂଖ୍ୟା;
		    ga_init2(&ga, 1, 80);
		    ପାଇଁ (idx = 0; idx < ଗଣନା_2; ++idx)
		    {
			tv = ଗଦା_TV_BOT(idx - ଗଣନା_2);
			ଯଦି (iptr->isn_type == ISN_EXECUTE)
			{
			    ଯଦି (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_JOB)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_ମୂଲ୍ୟ_as_string_str),
						    vartype_name(tv->v_type));
				ଭାଙ୍ଗ;
			    }
			    ଅନ୍ୟଥା
				p = tv_get_string_buf(tv, buf);
			}
			ଅନ୍ୟଥା
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(p);
			ଯଦି (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			ଅନ୍ୟଥା
			{
			    ଯଦି (ga.ga_len > 0)
				((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_ଗଦା.ga_len -= ଗଣନା_2;
		    ଯଦି (failed)
		    {
			ga_clear(&ga);
			ଯାଅ on_error;
		    }

		    ଯଦି (ga.ga_data != ଖାଲି)
		    {
			ଯଦି (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_data);
			    ଯଦି (did_emsg)
			    {
				ga_clear(&ga);
				ଯାଅ on_error;
			    }
			}
			ଅନ୍ୟଥା
			{
			    ସନ୍ଦେଶ_sb_eol();
			    ଯଦି (iptr->isn_type == ISN_ECHOMSG)
			    {
				ସନ୍ଦେଶ_attr(ga.ga_data, echo_attr);
				out_flush();
			    }
#ifdef HAS_MESSAGE_WINDOW
			    ଅନ୍ୟଥା ଯଦି (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				start_echowindow(
					      iptr->isn_arg.echowin.ewin_time);
				ସନ୍ଦେଶ_attr(ga.ga_data, echo_attr);
				end_echowindow();
			    }
#endif
			    ଅନ୍ୟଥା ଯଦି (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_data, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ga.ga_data),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    ଅନ୍ୟଥା
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_data);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		ଭାଙ୍ଗ;

	    // load local variable or argument
	    ସ୍ଥିତି ISN_LOAD:
		ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    ଯାଅ theend;
		tv = ଗଦା_TV_VAR(iptr->isn_arg.ସଂଖ୍ୟା);
		ଯଦି (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    ଗଦା_TV_BOT(0)->v_type = VAR_SPECIAL;
		    ଗଦା_TV_BOT(0)->vval.v_ସଂଖ୍ୟା = VVAL_NONE;
		}
		ଅନ୍ୟଥା
		    copy_tv(tv, ଗଦା_TV_BOT(0));
		++ectx->ec_ଗଦା.ga_len;
		ଭାଙ୍ଗ;

	    // load v: variable
	    ସ୍ଥିତି ISN_LOADV:
		ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    ଯାଅ theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.ସଂଖ୍ୟା), ଗଦା_TV_BOT(0));
		++ectx->ec_ଗଦା.ga_len;
		ଭାଙ୍ଗ;

	    // load s: variable in Vim9 script
	    ସ୍ଥିତି ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ଯଦି (sv == ଖାଲି)
			ଯାଅ theend;
		    allocate_if_null(sv);
		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    copy_tv(sv->sv_tv, ଗଦା_TV_BOT(0));
		    ++ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    // load s: variable in old script or autoload import
	    ସ୍ଥିତି ISN_LOADS:
	    ସ୍ଥିତି ISN_LOADEXPORT:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = find_var_in_ht(ht, 0, name, TRUE);

		    ଯଦି (di == ଖାଲି)
		    {
			ufunc_T	*ufunc = ଖାଲି;

			ଯଦି (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (ଶୂନ୍ୟ)find_exported(sid, name, &ufunc, &type,
							 ଖାଲି, ଖାଲି, FALSE);
			}
			ଯଦି (ufunc == ଖାଲି)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    ଯାଅ on_error;
			}
			ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			    ଯାଅ theend;
			tv = ଗଦା_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_ଗଦା.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (iptr->isn_type == ISN_LOADEXPORT)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = get_script_item_idx(sid, name, 0,
								   ଖାଲି, ଖାଲି);
			    svar_T	*sv;

			    ଯଦି (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;
				ଯଦି ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    ଯାଅ on_error;
				}
			    }
			}
			ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			    ଯାଅ theend;
			copy_tv(&di->di_tv, ଗଦା_TV_BOT(0));
			++ectx->ec_ଗଦା.ga_len;
		    }
		}
		ଭାଙ୍ଗ;

	    // load g:/b:/w:/t: variable
	    ସ୍ଥିତି ISN_LOADG:
	    ସ୍ଥିତି ISN_LOADB:
	    ସ୍ଥିତି ISN_LOADW:
	    ସ୍ଥିତି ISN_LOADT:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = load_namespace_var(ectx, iptr->isn_type, iptr);

		    ଯଦି (res == NOTDONE)
			ଯାଅ theend;
		    ଯଦି (res == FAIL)
			ଯାଅ on_error;
		}

		ଭାଙ୍ଗ;

	    // load autoload variable
	    ସ୍ଥିତି ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (eval_variable(name, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(name), 0,
			      ଗଦା_TV_BOT(0), ଖାଲି, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = ଗଦା_TV_BOT(0);
			tv->v_type = VAR_ସଂଖ୍ୟା;
			tv->v_lock = 0;
			tv->vval.v_ସଂଖ୍ୟା = 0;
			++ectx->ec_ଗଦା.ga_len;
			ଯାଅ on_error;
		    }
		    ++ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    // load g:/b:/w:/t: namespace
	    ସ୍ଥିତି ISN_LOADGDICT:
	    ସ୍ଥିତି ISN_LOADBDICT:
	    ସ୍ଥିତି ISN_LOADWDICT:
	    ସ୍ଥିତି ISN_LOADTDICT:
		{
		    dict_T *d = ଖାଲି;

		    ବାଛ (iptr->isn_type)
		    {
			ସ୍ଥିତି ISN_LOADGDICT: d = get_globvar_dict(); ଭାଙ୍ଗ;
			ସ୍ଥିତି ISN_LOADBDICT: d = curbuf->b_vars; ଭାଙ୍ଗ;
			ସ୍ଥିତି ISN_LOADWDICT: d = curwin->w_vars; ଭାଙ୍ଗ;
			ସ୍ଥିତି ISN_LOADTDICT: d = curtab->tp_vars; ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  // Cannot reach here
			    ଯାଅ theend;
		    }
		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    tv = ଗଦା_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_lock = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    // load &option
	    ସ୍ଥିତି ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    ଯଦି (eval_option(&name, &optval, TRUE) == FAIL)
			ଯାଅ theend;
		    *ଗଦା_TV_BOT(0) = optval;
		    ++ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    // load $ENV
	    ସ୍ଥିତି ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    // name is always valid, checked when compiling
		    (ଶୂନ୍ୟ)eval_env_var(&name, &optval, TRUE);
		    *ଗଦା_TV_BOT(0) = optval;
		    ++ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    // load @register
	    ସ୍ଥିତି ISN_LOADREG:
		ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    ଯାଅ theend;
		tv = ଗଦା_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.ସଂଖ୍ୟା, GREG_ପଦସମଷ୍ଟି_SRC);
		++ectx->ec_ଗଦା.ga_len;
		ଭାଙ୍ଗ;

	    // store local variable
	    ସ୍ଥିତି ISN_STORE:
		--ectx->ec_ଗଦା.ga_len;
		tv = ଗଦା_TV_VAR(iptr->isn_arg.ସଂଖ୍ୟା);
		ଯଦି (check_typval_is_ମୂଲ୍ୟ(ଗଦା_TV_BOT(0)) == FAIL
			|| ଗଦା_TV_BOT(0)->v_type == VAR_VOID)
		{
		    ଯଦି (ଗଦା_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_ମୂଲ୍ୟ));
		    clear_tv(ଗଦା_TV_BOT(0));
		    ଯାଅ on_error;
		}
		clear_tv(tv);
		*tv = *ଗଦା_TV_BOT(0);
		ଭାଙ୍ଗ;

	    // store s: variable in old script or autoload import
	    ସ୍ଥିତି ISN_STORES:
	    ସ୍ଥିତି ISN_STOREEXPORT:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = ଖାଲି;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    ଯଦି (iptr->isn_type == ISN_STOREEXPORT)
			di = find_var_autoload_prefix(name, sid, ଖାଲି, ଖାଲି);
		    // Then look for a variable in the script's variables.
		    ଯଦି (di == ଖାଲି)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = find_var_in_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_ଗଦା.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (di == ଖାଲି)
		    {
			ଯଦି (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(ଗଦା_TV_BOT(0));
			    ଯାଅ on_error;
			}
			store_var(name, ଗଦା_TV_BOT(0));
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (iptr->isn_type == ISN_STOREEXPORT)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = get_script_item_idx(sid, name, 0,
								   ଖାଲି, ଖାଲି);

			    // can this ever fail?
			    ଯଦି (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;

				ଯଦି ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    clear_tv(ଗଦା_TV_BOT(0));
				    ଯାଅ on_error;
				}
			    }
			}
			ଯଦି (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(ଗଦା_TV_BOT(0));
			    ଯାଅ on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *ଗଦା_TV_BOT(0);
		    }
		}
		ଭାଙ୍ଗ;

	    // store script-local variable in Vim9 script
	    ସ୍ଥିତି ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ଯଦି (sv == ଖାଲି)
			ଯାଅ theend;
		    --ectx->ec_ଗଦା.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (ମୂଲ୍ୟ_check_lock(sv->sv_tv->v_lock, sv->sv_name, FALSE))
		    {
			clear_tv(ଗଦା_TV_BOT(0));
			ଯାଅ on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *ଗଦା_TV_BOT(0);
		}
		ଭାଙ୍ଗ;

	    // store option
	    ସ୍ଥିତି ISN_STOREOPT:
	    ସ୍ଥିତି ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    long	n = 0;
		    char_u	*s = ଖାଲି;
		    ଅକ୍ଷର	*ସନ୍ଦେଶ;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = ଖାଲି;

		    --ectx->ec_ଗଦା.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ଗଦା_TV_BOT(0);
		    ଯଦି (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			ଯଦି (s == ଖାଲି)
			    s = (char_u *)"";
		    }
		    ଅନ୍ୟଥା ଯଦି (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			ଯଦି (s == ଖାଲି || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    ଯାଅ on_error;
			}
		    }
		    ଅନ୍ୟଥା
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_ସଂଖ୍ୟା;
		    ସନ୍ଦେଶ = set_option_ମୂଲ୍ୟ(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    ଯଦି (ସନ୍ଦେଶ != ଖାଲି)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(ସନ୍ଦେଶ));
			ଯାଅ on_error;
		    }
		}
		ଭାଙ୍ଗ;

	    // store $ENV
	    ସ୍ଥିତି ISN_STOREENV:
		ଯଦି (check_secure() || check_restricted())
		    ଯାଅ theend;
		--ectx->ec_ଗଦା.ga_len;
		tv = ଗଦା_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		ଭାଙ୍ଗ;

	    // store @r
	    ସ୍ଥିତି ISN_STOREREG:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	reg = iptr->isn_arg.ସଂଖ୍ୟା;

		    --ectx->ec_ଗଦା.ga_len;
		    tv = ଗଦା_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		ଭାଙ୍ଗ;

	    // store v: variable
	    ସ୍ଥିତି ISN_STOREV:
		--ectx->ec_ଗଦା.ga_len;
		ଯଦି (set_vim_var_tv(iptr->isn_arg.ସଂଖ୍ୟା, ଗଦା_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    ଯାଅ on_error;
		ଭାଙ୍ଗ;

	    // store g:/b:/w:/t: variable
	    ସ୍ଥିତି ISN_STOREG:
	    ସ୍ଥିତି ISN_STOREB:
	    ସ୍ଥିତି ISN_STOREW:
	    ସ୍ଥିତି ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    ବାଛ (iptr->isn_type)
		    {
			ସ୍ଥିତି ISN_STOREG:
			    ht = get_globvar_ht();
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  // Cannot reach here
			    ଯାଅ theend;
		    }

		    --ectx->ec_ଗଦା.ga_len;
		    di = find_var_in_ht(ht, 0, name, TRUE);
		    ଯଦି (di == ଖାଲି)
			store_var(iptr->isn_arg.string, ଗଦା_TV_BOT(0));
		    ଅନ୍ୟଥା
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ଯଦି (var_check_permission(di, name) == FAIL)
			    ଯାଅ on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *ଗଦା_TV_BOT(0);
		    }
		}
		ଭାଙ୍ଗ;

	    // store an autoload variable
	    ସ୍ଥିତି ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		set_var(iptr->isn_arg.string, ଗଦା_TV_BOT(-1), TRUE);
		clear_tv(ଗଦା_TV_BOT(-1));
		--ectx->ec_ଗଦା.ga_len;
		ଭାଙ୍ଗ;

	    // store number in local variable
	    ସ୍ଥିତି ISN_STORENR:
		tv = ଗଦା_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_ସଂଖ୍ୟା;
		tv->vval.v_ସଂଖ୍ୟା = iptr->isn_arg.storenr.stnr_val;
		ଭାଙ୍ଗ;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    ସ୍ଥିତି ISN_STOREINDEX:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = execute_storeindex(iptr, ectx);

		    ଯଦି (res == FAIL)
			ଯାଅ on_error;
		    ଯଦି (res == NOTDONE)
			ଯାଅ theend;
		}
		ଭାଙ୍ଗ;

	    // store value in list or blob range
	    ସ୍ଥିତି ISN_STORERANGE:
		ଯଦି (execute_storerange(iptr, ectx) == FAIL)
		    ଯାଅ on_error;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_LOAD_CLASSMEMBER:
		{
		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      ଗଦା_TV_BOT(0));
		    ++ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *ଗଦା_TV_BOT(-1);
		    --ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    // Load or store variable or argument from outer scope.
	    ସ୍ଥିତି ISN_LOADOUTER:
	    ସ୍ଥିତି ISN_STOREOUTER:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth = iptr->isn_arg.outer.outer_depth;
		    outer_T	*outer = ectx->ec_outer_ref == ଖାଲି ? ଖାଲି
						: ectx->ec_outer_ref->or_outer;

		    ଯେତେବେଳେ (depth > 1 && outer != ଖାଲି)
		    {
			outer = outer->out_up;
			--depth;
		    }
		    ଯଦି (outer == ଖାଲି)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ଯଦି (ectx->ec_frame_idx == ectx->ec_initial_frame_idx
						 || ectx->ec_outer_ref == ଖାଲି)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_closure_called_from_invalid_context));
			ଅନ୍ୟଥା
			    iemsg("LOADOUTER depth more than scope levels");
			ଯାଅ theend;
		    }
		    ଯଦି (depth < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->out_loop[-depth - 1]
							       .ଗଦା->ga_data)
					  + outer->out_loop[-depth - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    ଅନ୍ୟଥା
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->out_ଗଦା->ga_data)
				      + outer->out_frame_idx + ଗଦା_FRAME_SIZE
				      + iptr->isn_arg.outer.outer_idx;
		    ଯଦି (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			    ଯାଅ theend;
			// careful: ga_grow_inner may re-alloc the stack
			ଯଦି (depth < 0)
			    copy = ((typval_T *)outer->out_loop[-depth - 1]
								   .ଗଦା->ga_data)
					      + outer->out_loop[-depth - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			ଅନ୍ୟଥା
			    copy = ((typval_T *)outer->out_ଗଦା->ga_data)
					  + outer->out_frame_idx + ଗଦା_FRAME_SIZE
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			ଯଦି (copy != tv)
			    tv = copy;
			copy_tv(tv, ଗଦା_TV_BOT(0));
			++ectx->ec_ଗଦା.ga_len;
		    }
		    ଅନ୍ୟଥା
		    {
			--ectx->ec_ଗଦା.ga_len;
			clear_tv(tv);
			*tv = *ଗଦା_TV_BOT(0);
		    }
		}
		ଭାଙ୍ଗ;

	    // unlet item in list or dict variable
	    ସ୍ଥିତି ISN_UNLETINDEX:
		ଯଦି (execute_unletindex(iptr, ectx) == FAIL)
		    ଯାଅ on_error;
		ଭାଙ୍ଗ;

	    // unlet range of items in list variable
	    ସ୍ଥିତି ISN_UNLETRANGE:
		ଯଦି (execute_unletrange(iptr, ectx) == FAIL)
		    ଯାଅ on_error;
		ଭାଙ୍ଗ;

	    // push constant
	    ସ୍ଥିତି ISN_PUSHNR:
	    ସ୍ଥିତି ISN_PUSHBOOL:
	    ସ୍ଥିତି ISN_PUSHSPEC:
	    ସ୍ଥିତି ISN_PUSHF:
	    ସ୍ଥିତି ISN_PUSHS:
	    ସ୍ଥିତି ISN_PUSHBLOB:
	    ସ୍ଥିତି ISN_PUSHFUNC:
	    ସ୍ଥିତି ISN_PUSHCHANNEL:
	    ସ୍ଥିତି ISN_PUSHJOB:
	    ସ୍ଥିତି ISN_PUSHOBJ:
	    ସ୍ଥିତି ISN_PUSHCLASS:
		ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    ଯାଅ theend;
		tv = ଗଦା_TV_BOT(0);
		tv->v_lock = 0;
		++ectx->ec_ଗଦା.ga_len;
		ବାଛ (iptr->isn_type)
		{
		    ସ୍ଥିତି ISN_PUSHNR:
			tv->v_type = VAR_ସଂଖ୍ୟା;
			tv->vval.v_ସଂଖ୍ୟା = iptr->isn_arg.ସଂଖ୍ୟା;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_ସଂଖ୍ୟା = iptr->isn_arg.ସଂଖ୍ୟା;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_ସଂଖ୍ୟା = iptr->isn_arg.ସଂଖ୍ୟା;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			ଯଦି (iptr->isn_arg.string == ଖାଲି)
			    tv->vval.v_string = ଖାଲି;
			ଅନ୍ୟଥା
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = ଖାଲି;
#endif
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_JOB;
			tv->vval.v_job = ଖାଲି;
#endif
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = ଖାଲି;
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			ଭାଙ୍ଗ;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == ଖାଲି
				    ? ଖାଲି : vim_strsave(iptr->isn_arg.string);
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (ଶୂନ୍ୟ)script_autoload(name, FALSE);
		    ଯଦି (find_func(name, TRUE))
		    {
			ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			    ଯାଅ theend;
			tv = ଗଦା_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_ଗଦା.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    ଅନ୍ୟଥା
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = load_namespace_var(ectx, ISN_LOADG, iptr);

			ଯଦି (res == NOTDONE)
			    ଯାଅ theend;
			ଯଦି (res == FAIL)
			    ଯାଅ on_error;
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_UNLET:
		ଯଦି (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    ଯାଅ on_error;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_LOCKUNLOCK:
		{
#ifdef LOG_LOCKVAR
		    ch_log(ଖାଲି, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#endif
		    lval_ମୂଳ_T	*lval_ମୂଳ_save = lval_ମୂଳ;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_ଗଦା.ga_len;
		    lval_ମୂଳ_T ମୂଳ;
		    ମୂଳ.lr_tv      = ଗଦା_TV_BOT(0);
		    ମୂଳ.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    ମୂଳ.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_ମୂଳ = &ମୂଳ;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(ମୂଳ.lr_tv);
		    lval_ମୂଳ = lval_ମୂଳ_save;
		    ଯଦି (res == FAIL)
			ଯାଅ on_error;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_LOCKCONST:
		item_lock(ଗଦା_TV_BOT(-1), 100, TRUE, TRUE);
		ଭାଙ୍ଗ;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    ସ୍ଥିତି ISN_NEWLIST:
		ଯଦି (exe_newlist(iptr->isn_arg.ସଂଖ୍ୟା, ectx) == FAIL)
		    ଯାଅ theend;
		ଭାଙ୍ଗ;

	    // create a tuple from items on the stack
	    ସ୍ଥିତି ISN_NEWTUPLE:
		ଯଦି (exe_newtuple(iptr->isn_arg.ସଂଖ୍ୟା, ectx) == FAIL)
		    ଯାଅ theend;
		ଭାଙ୍ଗ;

	    // create a dict from items on the stack
	    ସ୍ଥିତି ISN_NEWDICT:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.ସଂଖ୍ୟା, ectx);
		    ଯଦି (res == FAIL)
			ଯାଅ theend;
		    ଯଦି (res == MAYBE)
			ଯାଅ on_error;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CONCAT:
		ଯଦି (exe_concat(iptr->isn_arg.ସଂଖ୍ୟା, ectx) == FAIL)
		    ଯାଅ theend;
		ଭାଙ୍ଗ;

	    // create a partial with NULL value
	    ସ୍ଥିତି ISN_NEWPARTIAL:
		ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    ଯାଅ theend;
		++ectx->ec_ଗଦା.ga_len;
		tv = ଗଦା_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_lock = 0;
		tv->vval.v_partial = ଖାଲି;
		ଭାଙ୍ଗ;

	    // call a :def function
	    ସ୍ଥିତି ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ଯଦି (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				ଖାଲି,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    ଯାଅ on_error;
		ଭାଙ୍ଗ;

	    // call a method on an interface
	    ସ୍ଥିତି ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = ଗଦା_TV_BOT(-1 - mfunc->cmf_argcount);
		    ଯଦି (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			ଯାଅ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ଯଦି (obj == ଖାଲି)
		    {
			emsg(_(e_using_null_object));
			ଯାଅ on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    ଯଦି (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    ଅନ୍ୟଥା
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = object_index_from_itf_index(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    ଯଦି (mfunc->cmf_ufunc != ଖାଲି && IS_GENERIC_FUNC(ufunc))
		    {
			ଯଦି (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			ଅନ୍ୟଥା
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    ଯଦି (ufunc == ଖାଲି)
				ଯାଅ on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    ଯଦି (call_ufunc(ufunc, ଖାଲି, mfunc->cmf_argcount, ectx,
							   ଖାଲି, ଖାଲି) == FAIL)
			ଯାଅ on_error;
		}
		ଭାଙ୍ଗ;

	    // call a builtin function
	    ସ୍ଥିତି ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ଯଦି (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    ଯାଅ on_error;
		ଭାଙ୍ଗ;

	    // call a funcref or partial
	    ସ୍ଥିତି ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (pfunc->cpf_ଶୀର୍ଷ)
		    {
			// funcref is above the arguments
			tv = ଗଦା_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    ଅନ୍ୟଥା
		    {
			// Get the funcref from the stack.
			--ectx->ec_ଗଦା.ga_len;
			partial_tv = *ଗଦା_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    ଯଦି (tv == &partial_tv)
			clear_tv(&partial_tv);
		    ଯଦି (r == FAIL)
			ଯାଅ on_error;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_PCALL_END:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_ଗଦା.ga_len;
		clear_tv(ଗଦା_TV_BOT(-1));
		*ଗଦା_TV_BOT(-1) = *ଗଦା_TV_BOT(0);
		ଭାଙ୍ଗ;

	    // call a user defined function or funcref/partial
	    ସ୍ଥିତି ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			ଯାଅ on_error;
		}
		ଭାଙ୍ଗ;

	    // :defer func(arg)
	    ସ୍ଥିତି ISN_DEFER:
		ଯଦି (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    ଯାଅ on_error;
		ଭାଙ୍ଗ;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    ସ୍ଥିତି ISN_RETURN_VOID:
	    ସ୍ଥିତି ISN_RETURN_OBJECT:
		ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    ଯାଅ theend;
		tv = ଗଦା_TV_BOT(0);
		++ectx->ec_ଗଦା.ga_len;
		ଯଦି (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_ସଂଖ୍ୟା = 0;
		    tv->v_lock = 0;
		}
		ଅନ୍ୟଥା
		{
		    *tv = *ଗଦା_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_lock_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    ସ୍ଥିତି ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ଖାଲି;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    ଯଦି (check_typval_is_ମୂଲ୍ୟ(ଗଦା_TV_BOT(-1)) == FAIL)
			ଯାଅ theend;

		    ଯଦି (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    ଯଦି (trycmd != ଖାଲି
				 && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    {
			// jump to ":finally" or ":endtry"
			ଯଦି (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			ଅନ୍ୟଥା
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    ଅନ୍ୟଥା
			ଯାଅ func_return;
		}
		ଭାଙ୍ଗ;

	    // push a partial, a reference to a compiled function
	    ସ୍ଥିତି ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    ଯଦି (pt == ଖାଲି)
			ଯାଅ theend;
		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    {
			vim_free(pt);
			ଯାଅ theend;
		    }
		    ଯଦି (extra != ଖାଲି && extra->fre_class != ଖାଲି)
		    {
			class_T	*cl;
			ଯଦି (extra->fre_object_method)
			{
			    tv = ଗଦା_TV_BOT(-1);
			    ଯଦି (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				ଯାଅ on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    ଯଦି (obj == ଖାଲି)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				ଯାଅ on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ଗଦା.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			ଅନ୍ୟଥା
			    cl = extra->fre_class;

			ଯଦି (extra->fre_object_method)
			{
			    // object method
			    // convert the interface index to the object index
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx =
				object_index_from_itf_index(extra->fre_class,
					TRUE, extra->fre_method_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			ଅନ୍ୟଥା
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_method_idx];
			}
		    }
		    ଅନ୍ୟଥା ଯଦି (extra == ଖାଲି || extra->fre_func_name == ଖାଲି)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    ଅନ୍ୟଥା
		    {
			ufunc = find_func(extra->fre_func_name, FALSE);
		    }
		    ଯଦି (ufunc == ଖାଲି)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			ଯାଅ theend;
		    }
		    ଯଦି (extra != ଖାଲି && extra->fre_outer_depth > 0
					   && (ufunc->uf_flags & FC_CLOSURE))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == ଖାଲି
				    ? ଖାଲି : ectx->ec_outer_ref->or_partial;

			ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା depth = 1; outer_pt != ଖାଲି
				    && depth < extra->fre_outer_depth; ++depth)
			    outer_pt = get_pt_outer(outer_pt)->out_up_partial;
			ଯଦି (outer_pt == ଖାଲି)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    ଯାଅ theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    ଅନ୍ୟଥା ଯଦି (fill_partial_and_closure(pt, ufunc,
			       extra == ଖାଲି ? ଖାଲି : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			ଯାଅ theend;
		    }
		    tv = ଗଦା_TV_BOT(0);
		    ++ectx->ec_ଗଦା.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_lock = 0;
		}
		ଭାଙ୍ଗ;

	    // Create a global function from a lambda.
	    ସ୍ଥିତି ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ଯଦି (copy_lambda_to_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			ଯାଅ theend;
		}
		ଭାଙ୍ଗ;

	    // List functions
	    ସ୍ଥିତି ISN_DEF:
		ଯଦି (iptr->isn_arg.string == ଖାଲି)
		    list_functions(ଖାଲି);
		ଅନ୍ୟଥା
		{
		    exarg_T	ea;
		    garray_T	ଧାଡ଼ିଗୁଡ଼ିକ_to_free;

		    CLEAR_FIELD(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free, ଆକାର(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, ଖାଲି, &ଧାଡ଼ିଗୁଡ଼ିକ_to_free, 0, ଖାଲି, 0, ଖାଲି);
		    ga_clear_strings(&ଧାଡ଼ିଗୁଡ଼ିକ_to_free);
		}
		ଭାଙ୍ଗ;

	    // jump if a condition is met
	    ସ୍ଥିତି ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		jump = TRUE;

		    ଯଦି (when != JUMP_ALWAYS)
		    {
			tv = ଗଦା_TV_BOT(-1);
			ଯଦି (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    ଯଦି (error)
				ଯାଅ on_error;
			}
			ଅନ୍ୟଥା
			    jump = tv2bool(tv);
			ଯଦି (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			ଯଦି (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_ଗଦା.ga_len;
			}
		    }
		    ଯଦି (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		ଭାଙ୍ଗ;

	    // "while": jump to end if a condition is false
	    ସ୍ଥିତି ISN_WHILE:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		jump = TRUE;

		    tv = ଗଦା_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    ଯଦି (error)
			ଯାଅ on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_ଗଦା.ga_len;
		    ଯଦି (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_end;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = ଗଦା_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_ସଂଖ୍ୟା = ectx->ec_funcrefs.ga_len;
		}
		ଭାଙ୍ଗ;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    ସ୍ଥିତି ISN_JUMP_IF_ARG_SET:
	    ସ୍ଥିତି ISN_JUMP_IF_ARG_NOT_SET:
		tv = ଗଦା_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା arg_set = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_ସଂଖ୍ୟା == VVAL_NONE);

		ଯଦି (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_SET && !arg_set)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_data)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_args.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ ଗଦା_FRAME_SIZE;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_POINTER(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    ଯଦି (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		ଯଦି (iptr->isn_type == ISN_JUMP_IF_ARG_SET ? arg_set : !arg_set)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		ଭାଙ୍ଗ;

	    // top of a for loop
	    ସ୍ଥିତି ISN_FOR:
		ଯଦି (execute_for(iptr, ectx) == FAIL)
		    ଯାଅ theend;
		ଭାଙ୍ଗ;

	    // end of a for or while loop
	    ସ୍ଥିତି ISN_ENDLOOP:
		ଯଦି (execute_endloop(iptr, ectx) == FAIL)
		    ଯାଅ theend;
		ଭାଙ୍ଗ;

	    // start of ":try" block
	    ସ୍ଥିତି ISN_TRY:
		{
		    trycmd_T    *trycmd = ଖାଲି;

		    ଯଦି (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			ଯାଅ theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_data)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_POINTER(trycmd);
		    trycmd->tcd_frame_idx = ectx->ec_frame_idx;
		    trycmd->tcd_ଗଦା_len = ectx->ec_ଗଦା.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_PUSHEXC:
		ଯଦି (current_exception == ଖାଲି)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    ଯାଅ theend;
		}
		ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
		    ଯାଅ theend;
		tv = ଗଦା_TV_BOT(0);
		++ectx->ec_ଗଦା.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->ମୂଲ୍ୟ);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CATCH:
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
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
		    trycmd_T    *trycmd;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		iidx = trycont->tct_where;

		    ଯଦି (trystack->ga_len < trycont->tct_levels)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_levels, trystack->ga_len);
			ଯାଅ theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    ପାଇଁ (i = trycont->tct_levels; i > 0; --i)
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
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    ଯଦି (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_ଗଦା);
			trycmd->tcd_caught = FALSE;
		    }
		    ଭାଙ୍ଗ;
		}

	    // end of ":try" block
	    ସ୍ଥିତି ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_data) + trystack->ga_len;
		    ଯଦି (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    ଯଦି (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_ଗଦା);
		    }

		    ଯଦି (trycmd->tcd_return)
			ଯାଅ func_return;

		    ଯେତେବେଳେ (ectx->ec_ଗଦା.ga_len > trycmd->tcd_ଗଦା_len)
		    {
			--ectx->ec_ଗଦା.ga_len;
			clear_tv(ଗଦା_TV_BOT(0));
		    }
		    ଯଦି (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    ଯଦି (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = ଗଦା_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_ସଂଖ୍ୟା;
			tv->vval.v_ସଂଖ୍ୟା = 0;
			ଯାଅ done;
		    }
		    --ectx->ec_ଗଦା.ga_len;
		    tv = ଗଦା_TV_BOT(0);
		    ଯଦି (tv->vval.v_string == ଖାଲି
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_empty_string));
			ଯାଅ theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    ଯଦି (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
			ଯଦି (trycmd->tcd_caught)
			{
			    // discard the exception
			    finish_exception(caught_ଗଦା);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (throw_exception(tv->vval.v_string, ET_USER, ଖାଲି)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			ଯାଅ theend;
		    }
		    did_throw = TRUE;
		}
		ଭାଙ୍ଗ;

	    // compare with special values
	    ସ୍ଥିତି ISN_COMPAREBOOL:
	    ସ୍ଥିତି ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = ଗଦା_TV_BOT(-2);
		    typval_T	*tv2 = ଗଦା_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_ସଂଖ୍ୟା;
		    varnumber_T arg2 = tv2->vval.v_ସଂଖ୍ୟା;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

		    ଯଦି (iptr->isn_arg.op.op_type == ପଦସମଷ୍ଟି_EQUAL)
			res = arg1 == arg2;
		    ଅନ୍ୟଥା
			res = arg1 != arg2;

		    --ectx->ec_ଗଦା.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ସଂଖ୍ୟା = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_COMPARENULL:
		{
		    typval_T	*tv1 = ଗଦା_TV_BOT(-2);
		    typval_T	*tv2 = ଗଦା_TV_BOT(-1);
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

		    res = typval_compare_null(tv1, tv2);
		    ଯଦି (res == MAYBE)
			ଯାଅ on_error;
		    ଯଦି (iptr->isn_arg.op.op_type == ପଦସମଷ୍ଟି_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_ଗଦା.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ସଂଖ୍ୟା = res ? VVAL_TRUE : VVAL_FALSE;
		}
		ଭାଙ୍ଗ;

	    // Operation with two number arguments
	    ସ୍ଥିତି ISN_OPNR:
	    ସ୍ଥିତି ISN_COMPARENR:
		{
		    typval_T	*tv1 = ଗଦା_TV_BOT(-2);
		    typval_T	*tv2 = ଗଦା_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_ସଂଖ୍ୟା;
		    varnumber_T arg2 = tv2->vval.v_ସଂଖ୍ୟା;
		    varnumber_T res = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		div_zero = FALSE;

		    ଯଦି (iptr->isn_arg.op.op_type == ପଦସମଷ୍ଟି_LSHIFT
			    || iptr->isn_arg.op.op_type == ପଦସମଷ୍ଟି_RSHIFT)
		    {
			ଯଦି (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_positive));
			    ଯାଅ on_error;
			}
		    }

		    ବାଛ (iptr->isn_arg.op.op_type)
		    {
			ସ୍ଥିତି ପଦସମଷ୍ଟି_MULT: res = arg1 * arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_DIV:  ଯଦି (arg2 == 0)
					    div_zero = TRUE;
					ଅନ୍ୟଥା
					    res = arg1 / arg2;
					ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_REM:  ଯଦି (arg2 == 0)
					    div_zero = TRUE;
					ଅନ୍ୟଥା
					    res = arg1 % arg2;
					ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_SUB: res = arg1 - arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_ADD: res = arg1 + arg2; ଭାଙ୍ଗ;

			ସ୍ଥିତି ପଦସମଷ୍ଟି_EQUAL: res = arg1 == arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_NEQUAL: res = arg1 != arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_GREATER: res = arg1 > arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_GEQUAL: res = arg1 >= arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_SMALLER: res = arg1 < arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_SEQUAL: res = arg1 <= arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_LSHIFT: ଯଦି (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  ଅନ୍ୟଥା
					      res = (uvarnumber_T)arg1 << arg2;
					  ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_RSHIFT: ଯଦି (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  ଅନ୍ୟଥା
					      res = (uvarnumber_T)arg1 >> arg2;
					  ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଭାଙ୍ଗ;
		    }

		    --ectx->ec_ଗଦା.ga_len;
		    ଯଦି (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_ସଂଖ୍ୟା = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ଅନ୍ୟଥା
			tv1->vval.v_ସଂଖ୍ୟା = res;
		    ଯଦି (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			ଯାଅ on_error;
		    }
		}
		ଭାଙ୍ଗ;

	    // Computation with two float arguments
	    ସ୍ଥିତି ISN_OPFLOAT:
	    ସ୍ଥିତି ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = ଗଦା_TV_BOT(-2);
		    typval_T	*tv2 = ଗଦା_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cmp = FALSE;

		    ବାଛ (iptr->isn_arg.op.op_type)
		    {
			ସ୍ଥିତି ପଦସମଷ୍ଟି_MULT: res = arg1 * arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_DIV: res = arg1 / arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_SUB: res = arg1 - arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_ADD: res = arg1 + arg2; ଭାଙ୍ଗ;

			ସ୍ଥିତି ପଦସମଷ୍ଟି_EQUAL: cmp = arg1 == arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_NEQUAL: cmp = arg1 != arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_GREATER: cmp = arg1 > arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_GEQUAL: cmp = arg1 >= arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_SMALLER: cmp = arg1 < arg2; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_SEQUAL: cmp = arg1 <= arg2; ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: cmp = 0; ଭାଙ୍ଗ;
		    }
		    --ectx->ec_ଗଦା.ga_len;
		    ଯଦି (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_ସଂଖ୍ୟା = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    ଅନ୍ୟଥା
			tv1->vval.v_float = res;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_COMPARELIST:
	    ସ୍ଥିତି ISN_COMPARETUPLE:
	    ସ୍ଥିତି ISN_COMPAREDICT:
	    ସ୍ଥିତି ISN_COMPAREFUNC:
	    ସ୍ଥିତି ISN_COMPARESTRING:
	    ସ୍ଥିତି ISN_COMPAREBLOB:
	    ସ୍ଥିତି ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = ଗଦା_TV_BOT(-2);
		    typval_T	*tv2 = ଗଦା_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic = iptr->isn_arg.op.op_ic;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res = FALSE;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (iptr->isn_type == ISN_COMPARELIST)
		    {
			status = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ଅନ୍ୟଥା ଯଦି (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			status = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ଅନ୍ୟଥା ଯଦି (iptr->isn_type == ISN_COMPAREDICT)
		    {
			status = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ଅନ୍ୟଥା ଯଦି (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			status = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ଅନ୍ୟଥା ଯଦି (iptr->isn_type == ISN_COMPARESTRING)
		    {
			status = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    ଅନ୍ୟଥା ଯଦି (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			status = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    ଅନ୍ୟଥା // ISN_COMPAREOBJECT
		    {
			status = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_ଗଦା.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_ସଂଖ୍ୟା = res ? VVAL_TRUE : VVAL_FALSE;
		    ଯଦି (status == FAIL)
			ଯାଅ theend;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_COMPAREANY:
		{
		    typval_T	*tv1 = ଗଦା_TV_BOT(-2);
		    typval_T	*tv2 = ଗଦା_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic = iptr->isn_arg.op.op_ic;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status;

		    SOURCING_LNUM = iptr->isn_lnum;
		    status = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_ଗଦା.ga_len;
		    ଯଦି (status == FAIL)
			ଯାଅ theend;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_ADDLIST:
	    ସ୍ଥିତି ISN_ADDTUPLE:
	    ସ୍ଥିତି ISN_ADDBLOB:
		{
		    typval_T *tv1 = ଗଦା_TV_BOT(-2);
		    typval_T *tv2 = ଗଦା_TV_BOT(-1);

		    // add two lists or blobs
		    ଯଦି (iptr->isn_type == ISN_ADDLIST)
		    {
			ଯଦି (iptr->isn_arg.op.op_type == ପଦସମଷ୍ଟି_APPEND
						   && tv1->vval.v_list != ଖାଲି)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 ଖାଲି);
			ଅନ୍ୟଥା
			    eval_addlist(tv1, tv2);
		    }
		    ଅନ୍ୟଥା ଯଦି (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    ଅନ୍ୟଥା
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = ଗଦା_TV_BOT(-2);
		    typval_T	*tv2 = ଗଦା_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (l == ଖାଲି)
		    {
			emsg(_(e_cannot_add_to_null_list));
			ଯାଅ on_error;
		    }
		    ଯଦି (ମୂଲ୍ୟ_check_lock(l->lv_lock, ଖାଲି, FALSE))
			ଯାଅ on_error;
		    ଯଦି (list_append_tv(l, tv2) == FAIL)
			ଯାଅ theend;
		    clear_tv(tv2);
		    --ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = ଗଦା_TV_BOT(-2);
		    typval_T	*tv2 = ଗଦା_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    ଯଦି (b == ଖାଲି)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_add_to_null_blob));
			ଯାଅ on_error;
		    }
		    n = tv_get_ସଂଖ୍ୟା_chk(tv2, &error);
		    ଯଦି (error)
			ଯାଅ on_error;
		    ga_append(&b->bv_ga, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n);
		    --ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    // Computation with two arguments of unknown type
	    ସ୍ଥିତି ISN_OPANY:
		{
		    typval_T	*tv1 = ଗଦା_TV_BOT(-2);
		    typval_T	*tv2 = ଗଦା_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		error = FALSE;

		    ଯଦି (iptr->isn_arg.op.op_type == ପଦସମଷ୍ଟି_ADD)
		    {
			ଯଦି (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ଗଦା.ga_len;
			    ଭାଙ୍ଗ;
			}
			ଅନ୍ୟଥା ଯଦି (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ଗଦା.ga_len;
			    ଭାଙ୍ଗ;
			}
			ଅନ୍ୟଥା ଯଦି (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_ଗଦା.ga_len;
			    ଭାଙ୍ଗ;
			}
		    }
		    ଯଦି (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    ଅନ୍ୟଥା
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_ସଂଖ୍ୟା_chk(tv1, &error);
			ଯଦି (error)
			    ଯାଅ on_error;
			ଯଦି (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    ଯଦି (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    ଅନ୍ୟଥା
		    {
			n2 = tv_get_ସଂଖ୍ୟା_chk(tv2, &error);
			ଯଦି (error)
			    ଯାଅ on_error;
			ଯଦି (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    ଯଦି (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			ବାଛ (iptr->isn_arg.op.op_type)
			{
			    ସ୍ଥିତି ପଦସମଷ୍ଟି_MULT: f1 = f1 * f2; ଭାଙ୍ଗ;
			    ସ୍ଥିତି ପଦସମଷ୍ଟି_DIV:  f1 = f1 / f2; ଭାଙ୍ଗ;
			    ସ୍ଥିତି ପଦସମଷ୍ଟି_SUB:  f1 = f1 - f2; ଭାଙ୍ଗ;
			    ସ୍ଥିତି ପଦସମଷ୍ଟି_ADD:  f1 = f1 + f2; ଭାଙ୍ଗ;
			    ପୂର୍ବନିର୍ଦ୍ଧାରିତ: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     ଯାଅ on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_ଗଦା.ga_len;
		    }
		    ଅନ୍ୟଥା
		    {
			ପୂର୍ଣ୍ଣସଂଖ୍ୟା failed = FALSE;

			ବାଛ (iptr->isn_arg.op.op_type)
			{
			    ସ୍ଥିତି ପଦସମଷ୍ଟି_MULT: n1 = n1 * n2; ଭାଙ୍ଗ;
			    ସ୍ଥିତି ପଦସମଷ୍ଟି_DIV:  n1 = ଅଙ୍କ_divide(n1, n2, &failed);
					    ଯଦି (failed)
						ଯାଅ on_error;
					    ଭାଙ୍ଗ;
			    ସ୍ଥିତି ପଦସମଷ୍ଟି_SUB:  n1 = n1 - n2; ଭାଙ୍ଗ;
			    ସ୍ଥିତି ପଦସମଷ୍ଟି_ADD:  n1 = n1 + n2; ଭାଙ୍ଗ;
			    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	    n1 = ଅଙ୍କ_modulus(n1, n2, &failed);
					    ଯଦି (failed)
						ଯାଅ on_error;
					    ଭାଙ୍ଗ;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_ସଂଖ୍ୟା;
			tv1->vval.v_ସଂଖ୍ୟା = n1;
			--ectx->ec_ଗଦା.ga_len;
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_STRINDEX:
	    ସ୍ଥିତି ISN_STRSLICE:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_slice = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    ଯଦି (is_slice)
		    {
			tv = ଗଦା_TV_BOT(-2);
			n1 = tv->vval.v_ସଂଖ୍ୟା;
		    }

		    tv = ଗଦା_TV_BOT(-1);
		    n2 = tv->vval.v_ସଂଖ୍ୟା;

		    ectx->ec_ଗଦା.ga_len -= is_slice ? 2 : 1;
		    tv = ଗଦା_TV_BOT(-1);
		    ଯଦି (is_slice)
			// Slice: Select the characters from the string
			res = string_slice(tv->vval.v_string, n1, n2, FALSE);
		    ଅନ୍ୟଥା
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_LISTINDEX:
	    ସ୍ଥିତି ISN_LISTSLICE:
	    ସ୍ଥିତି ISN_TUPLEINDEX:
	    ସ୍ଥିତି ISN_TUPLESLICE:
	    ସ୍ଥିତି ISN_BLOBINDEX:
	    ସ୍ଥିତି ISN_BLOBSLICE:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_slice = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_slice ? ଗଦା_TV_BOT(-3) : ଗଦା_TV_BOT(-2);

		    tv = ଗଦା_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_ସଂଖ୍ୟା;
		    clear_tv(tv);

		    ଯଦି (is_slice)
		    {
			tv = ଗଦା_TV_BOT(-2);
			n1 = tv->vval.v_ସଂଖ୍ୟା;
			clear_tv(tv);
		    }

		    ectx->ec_ଗଦା.ga_len -= is_slice ? 2 : 1;
		    tv = ଗଦା_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (is_blob)
		    {
			ଯଦି (blob_slice_or_index(val_tv->vval.v_blob, is_slice,
						    n1, n2, FALSE, tv) == FAIL)
			    ଯାଅ on_error;
		    }
		    ଅନ୍ୟଥା ଯଦି (is_tuple)
		    {
			ଯଦି (tuple_slice_or_index(val_tv->vval.v_tuple,
				is_slice, n1, n2, FALSE, tv, TRUE) == FAIL)
			    ଯାଅ on_error;
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (list_slice_or_index(val_tv->vval.v_list, is_slice,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    ଯାଅ on_error;
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_ANYINDEX:
	    ସ୍ଥିତି ISN_ANYSLICE:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		is_slice = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_slice ? ଗଦା_TV_BOT(-3) : ଗଦା_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (check_can_index(tv, TRUE, TRUE) == FAIL)
			ଯାଅ on_error;
		    var1 = is_slice ? ଗଦା_TV_BOT(-2) : ଗଦା_TV_BOT(-1);
		    var2 = is_slice ? ଗଦା_TV_BOT(-1) : ଖାଲି;
		    res = eval_index_inner(tv, is_slice, var1, var2,
							FALSE, ଖାଲି, -1, TRUE);
		    clear_tv(var1);
		    ଯଦି (is_slice)
			clear_tv(var2);
		    ectx->ec_ଗଦା.ga_len -= is_slice ? 2 : 1;
		    ଯଦି (res == FAIL)
			ଯାଅ on_error;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_SLICE:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଗଣନା_2 = iptr->isn_arg.ସଂଖ୍ୟା;

		    // type will have been checked to be a list
		    tv = ଗଦା_TV_BOT(-1);
		    ଯଦି (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			ଯଦି (list != ଖାଲି && list->lv_len >= ଗଣନା_2)
			{
			    list_T	*newlist = list_slice(list,
				    ଗଣନା_2, list->lv_len - 1);

			    ଯଦି (newlist != ଖାଲି)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    ଅନ୍ୟଥା
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			ଯଦି (tuple != ଖାଲି && TUPLE_LEN(tuple) >= ଗଣନା_2)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_slice(tuple, ଗଣନା_2,
							TUPLE_LEN(tuple) - 1);
			    ଯଦି (newtuple != ଖାଲି)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*item_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = ଗଦା_TV_BOT(-1 - gi->gi_with_op);
		    ଯଦି (tv->v_type == VAR_LIST)
		    {
			li = list_find(tv->vval.v_list, gi->gi_index);
			ଯଦି (li == ଖାଲି)
			    ଯାଅ on_error;
			item_tv = &li->li_tv;
		    }
		    ଅନ୍ୟଥା
			item_tv = TUPLE_ITEM(tv->vval.v_tuple, gi->gi_index);

		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    ++ectx->ec_ଗଦା.ga_len;
		    copy_tv(item_tv, ଗଦା_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_index = gi->gi_index + 1;
		    ectx->ec_where.wt_kind = WT_VARIABLE;
		}
		ଭାଙ୍ଗ;

	    // dict member with string key (dict['member'])
	    ସ୍ଥିତି ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*ଚାବି;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = ଗଦା_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = ଗଦା_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    ଚାବି = tv->vval.v_string;
		    ଯଦି (ଚାବି == ଖାଲି)
			ଚାବି = (char_u *)"";

		    ଯଦି ((di = dict_find(dict, ଚାବି, -1)) == ଖାଲି)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_ଚାବି_not_present_in_dictionary_str), ଚାବି);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_ଗଦା.ga_len;
			tv = ଗଦା_TV_BOT(-1);
			(ଶୂନ୍ୟ) dict_ଗଦା_save(tv);
			tv->v_type = VAR_ସଂଖ୍ୟା;
			tv->vval.v_ସଂଖ୍ୟା = 0;
			ଯାଅ on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_ଗଦା.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = ଗଦା_TV_BOT(-1);
		    ଯଦି (dict_ଗଦା_save(tv) == FAIL)
			ଯାଅ on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		ଭାଙ୍ଗ;

	    // dict member with string key (dict.member)
	    // or can be an object
	    ସ୍ଥିତି ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = ଗଦା_TV_BOT(-1);

		    ଯଦି (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			ଯଦି (dict_ଗଦା_save(tv) == FAIL)
			    ଯାଅ on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			ଯଦି (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    ଯାଅ on_error;
		    }
		    ଅନ୍ୟଥା
		    {
			ଯଦି (tv->v_type != VAR_DICT || tv->vval.v_dict == ଖାଲି)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    ଯାଅ on_error;
			}
			dict = tv->vval.v_dict;

			ଯଦି ((di = dict_find(dict, iptr->isn_arg.string, -1))
								   == ଖାଲି)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_ଚାବି_not_present_in_dictionary_str),
						     iptr->isn_arg.string);
			    ଯାଅ on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			ଯଦି (dict_ଗଦା_save(tv) == FAIL)
			    ଯାଅ on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_GET_OBJ_MEMBER:
	    ସ୍ଥିତି ISN_GET_ITF_MEMBER:
		{
		    tv = ଗଦା_TV_BOT(-1);
		    ଯଦି (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			ଯାଅ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ଯଦି (obj == ଖାଲି)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			ଯାଅ on_error;
		    }

		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx;
		    ଯଦି (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    ଅନ୍ୟଥା
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_index_from_itf_index(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    ଯଦି (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			ଯାଅ on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_STORE_THIS:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = iptr->isn_arg.ସଂଖ୍ୟା;
		    object_T *obj = ଗଦା_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *ଗଦା_TV_BOT(-1);
		    --ectx->ec_ଗଦା.ga_len;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CLEARDICT:
		dict_ଗଦା_drop();
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_ଗଦା_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = ଗଦା_TV_BOT(-1);
		    ଯଦି (dict_tv != ଖାଲି
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != ଖାଲି
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == ଖାଲି))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_ଗଦା_drop();
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_NEGATENR:
		tv = ଗଦା_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		ଯଦି (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		ଅନ୍ୟଥା
		    tv->vval.v_ସଂଖ୍ୟା = -tv->vval.v_ସଂଖ୍ୟା;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;
		    where_T	where = WHERE_INIT;

		    tv = ଗଦା_TV_BOT((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ଯଦି (ct->ct_arg_idx > 0)
		    {
			where.wt_index = ct->ct_arg_idx;
			where.wt_kind = ct->ct_is_var ? WT_VARIABLE : WT_ARGUMENT;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    ଯଦି (r == FAIL)
			ଯାଅ on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    ଯଦି (tv->v_type == VAR_ସଂଖ୍ୟା
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_ସଂଖ୍ୟା == 0
						|| tv->vval.v_ସଂଖ୍ୟା == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_ସଂଖ୍ୟା = tv->vval.v_ସଂଖ୍ୟା
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CHECKLEN:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = ଖାଲି;
		    tuple_T *tuple = ଖାଲି;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len = 0;

		    tv = ଗଦା_TV_BOT(-1);

		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len_check_failed = FALSE;
		    ଯଦି (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			ଯଦି (list == ଖାଲି || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			ଯଦି (list != ଖାଲି)
			    len = list->lv_len;
		    }
		    ଅନ୍ୟଥା ଯଦି (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			ଯଦି (tuple == ଖାଲି || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			ଯଦି (tuple != ଖାଲି)
			    len = TUPLE_LEN(tuple);
		    }
		    ଅନ୍ୟଥା
			len_check_failed = TRUE;

		    ଯଦି (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_ବସ୍ତୁଗୁଡ଼ିକ_but_got_nr),
				     min_len, len);
			ଯାଅ on_error;
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_SETTYPE:
		set_tv_type(ଗଦା_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_2BOOL:
	    ସ୍ଥିତି ISN_COND2BOOL:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା n;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା error = FALSE;

		    ଯଦି (iptr->isn_type == ISN_2BOOL)
		    {
			tv = ଗଦା_TV_BOT(iptr->isn_arg.tobool.offset);
			n = tv2bool(tv);
			ଯଦି (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    ଅନ୍ୟଥା
		    {
			tv = ଗଦା_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			ଯଦି (error)
			    ଯାଅ on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_ସଂଖ୍ୟା = n ? VVAL_TRUE : VVAL_FALSE;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_2STRING:
	    ସ୍ଥିତି ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		ଯଦି (do_2string(ଗଦା_TV_BOT(iptr->isn_arg.tostring.offset),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    ଯାଅ on_error;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_RANGE:
		{
		    exarg_T	ea;
		    ଅକ୍ଷର	*errormsg;

		    ea.line2 = 0;
		    ea.addr_ଗଣନା = 0;
		    ea.addr_type = ADDR_ଧାଡ଼ିଗୁଡ଼ିକ;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    ଯଦି (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
			ଯାଅ on_error;

		    ଯଦି (GA_GROW_FAILS(&ectx->ec_ଗଦା, 1))
			ଯାଅ theend;
		    ++ectx->ec_ଗଦା.ga_len;
		    tv = ଗଦା_TV_BOT(-1);
		    tv->v_type = VAR_ସଂଖ୍ୟା;
		    tv->v_lock = 0;
		    tv->vval.v_ସଂଖ୍ୟା = ea.line2;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_PUT:
		tv = ଖାଲି; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_IPUT:
		tv = ଖାଲି;
		isn_put_do(ectx, iptr, tv, TRUE);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_ଗଦା.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = ଖାଲି;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = ଗଦା_TV_BOT(-1);
		ଯଦି (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_argument_must_be_sequence_of_lists_or_tuples));
		    ଯାଅ on_error;
		}

		ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
		ଯଦି (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		ଅନ୍ୟଥା
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		ଯଦି (rc != EXEC_OK)
		{
		    ଯଦି (rc == EXEC_FAIL)
			ଯାଅ on_error;
		    ଯାଅ theend;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_PROF_START:
	    ସ୍ଥିତି ISN_PROF_END:
		{
#ifdef FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    ଯଦି (iptr->isn_type == ISN_PROF_START)
		    {
			func_ଧାଡ଼ି_start(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_ଧାଡ଼ି_exec(&cookie);
		    }
		    ଅନ୍ୟଥା
			func_ଧାଡ଼ି_end(&cookie);
#endif
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_DEBUG:
		handle_debug(iptr, ectx);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		item = iptr->isn_arg.shuffle.shfl_item;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		up = iptr->isn_arg.shuffle.shfl_up;

		    tmp_tv = *ଗଦା_TV_BOT(-item);
		    ପାଇଁ ( ; up > 0 && item > 1; --up)
		    {
			*ଗଦା_TV_BOT(-item) = *ଗଦା_TV_BOT(-item + 1);
			--item;
		    }
		    *ଗଦା_TV_BOT(-item) = tmp_tv;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_DROP:
		--ectx->ec_ଗଦା.ga_len;
		clear_tv(ଗଦା_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_SCRIPTCTX_SET:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		ଭାଙ୍ଗ;
	}
	ଜାରି;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	ଯଦି (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
	    ଯାଅ done;

	ଯଦି (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    ଯାଅ theend;
	ଜାରି;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	ଯଦି (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    ଯଦି (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		ଯେତେବେଳେ (ectx->ec_ଗଦା.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_ଗଦା.ga_len;
		    clear_tv(ଗଦା_TV_BOT(0));
		}
		ଯେତେବେଳେ (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    ଜାରି;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	ଯଦି (trylevel <= ectx->ec_trylevel_at_start)
	    ଯାଅ theend;
    }

done:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_ଗଦା_clear(dict_ଗଦା_len_at_start);
    ectx->ec_trylevel_at_start = save_trylevel_at_start;
    ଫେରନ୍ତୁ ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_iidx = ectx->ec_iidx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    ଯଦି (res == OK)
    {
	*rettv = *ଗଦା_TV_BOT(-1);
	--ectx->ec_ଗଦା.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    ଫେରନ୍ତୁ res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(ଶୂନ୍ୟ)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    ଯଦି (exec_instructions(ectx) == OK)
    {
	typval_T *tv = ଗଦା_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_ଗଦା.ga_len;
	clear_tv(tv);
    }
    ଅନ୍ୟଥା
    {
	substitute_instr->subs_status = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    ଫେରନ୍ତୁ res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
call_def_function(
    ufunc_T	*ufunc,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc = argc_arg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		partial_argc = partial == ଖାଲି
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		total_argc = argc + partial_argc;
    typval_T	*tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		defcount = ufunc->uf_args.ga_len - total_argc;
    sctx_T	save_current_sctx = current_sctx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_emsg_before = did_emsg_cumul + did_emsg;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_ସନ୍ଦେଶ_list = ଖାଲି;
    msglist_T	*private_ସନ୍ଦେଶ_list = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_emsg_silent_def = emsg_silent_def;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_did_emsg_def = did_emsg_def;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_funcdepth;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#undef ଗଦା_TV
#define ଗଦା_TV(idx) (((typval_T *)ectx.ec_ଗଦା.ga_data) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#undef ଗଦା_TV_BOT
#define ଗଦା_TV_BOT(idx) (((typval_T *)ectx.ec_ଗଦା.ga_data) + ectx.ec_ଗଦା.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#undef ଗଦା_TV_VAR
#define ଗଦା_TV_VAR(idx) (((typval_T *)ectx.ec_ଗଦା.ga_data) + ectx.ec_frame_idx + ଗଦା_FRAME_SIZE + idx)

    ଯଦି (ufunc->uf_def_status == UF_NOT_COMPILED
	    || ufunc->uf_def_status == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), ଖାଲି) == FAIL))
    {
	ଯଦି (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	ଫେରନ୍ତୁ FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ଯଦି (dfunc->df_ufunc == ଖାଲି)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (INSTRUCTIONS(dfunc) == ଖାଲି)
	{
	    iemsg("using call_def_function() on not compiled function");
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    ଯଦି (funcdepth_increment() == FAIL)
	ଫେରନ୍ତୁ FAIL;

    CLEAR_FIELD(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_ଗଦା, ଆକାର(typval_T), 500);
    ଯଦି (GA_GROW_FAILS(&ectx.ec_ଗଦା, 20))
    {
	funcdepth_decrement();
	ଫେରନ୍ତୁ FAIL;
    }
    ga_init2(&ectx.ec_trystack, ଆକାର(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, ଆକାର(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = total_argc - ufunc->uf_args.ga_len;
    ଯଦି (idx > 0 && ufunc->uf_va_name == ଖାଲି)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
								    idx), idx);
	ଯାଅ failed_early;
    }
    idx = total_argc - ufunc->uf_args.ga_len + ufunc->uf_def_args.ga_len;
    ଯଦି (idx < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	ଯାଅ failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    ପାଇଁ (idx = 0; idx < total_argc
	    && (ufunc->uf_va_name != ଖାଲି || idx < ufunc->uf_args.ga_len);
									 ++idx)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	ଯଦି (idx >= ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_ସଂଖ୍ୟା == VVAL_NONE)
	{
	    // Use the default value.
	    ଗଦା_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା done = FALSE;
	    ଯଦି (ufunc->uf_arg_types != ଖାଲି && idx < ufunc->uf_args.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		ଯଦି (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_ସଂଖ୍ୟା)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    ଗଦା_TV_BOT(0)->v_type = VAR_FLOAT;
		    ଗଦା_TV_BOT(0)->v_lock = 0;
		    ଗଦା_TV_BOT(0)->vval.v_float = tv->vval.v_ସଂଖ୍ୟା;
		    done = TRUE;
		}
		ଅନ୍ୟଥା ଯଦି (check_typval_arg_type(expected, tv,
						   ଖାଲି, argv_idx + 1) == FAIL)
		    ଯାଅ failed_early;
	    }
	    ଯଦି (!done)
		copy_tv(tv, ଗଦା_TV_BOT(0));
	}
	++ectx.ec_ଗଦା.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    ଯଦି (ufunc->uf_va_name != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା vararg_ଗଣନା = argc - ufunc->uf_args.ga_len;

	ଯଦି (vararg_ଗଣନା < 0)
	    vararg_ଗଣନା = 0;
	ଅନ୍ୟଥା
	    argc -= vararg_ଗଣନା;
	ଯଦି (exe_newlist(vararg_ଗଣନା, &ectx) == FAIL)
	    ଯାଅ failed_early;

	// Check the type of the list items.
	tv = ଗଦା_TV_BOT(-1);
	ଯଦି (ufunc->uf_va_type != ଖାଲି
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != ଖାଲି)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_first;

	    ପାଇଁ (idx = 0; idx < vararg_ଗଣନା; ++idx)
	    {
		ଯଦି (check_typval_arg_type(expected, &li->li_tv,
						 ଖାଲି, argc + idx + 1) == FAIL)
		    ଯାଅ failed_early;
		li = li->li_next;
	    }
	}

	ଯଦି (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *ଗଦା_TV_BOT(defcount - 1) = *ଗଦା_TV_BOT(-1);
	--ectx.ec_ଗଦା.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    ଯଦି (defcount > 0)
	ପାଇଁ (idx = 0; idx < defcount; ++idx)
	{
	    ଗଦା_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_ଗଦା.ga_len;
	}
    ଯଦି (ufunc->uf_va_name != ଖାଲି)
	++ectx.ec_ଗଦା.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_frame_idx = ectx.ec_ଗଦା.ga_len;
    ectx.ec_initial_frame_idx = ectx.ec_frame_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *base_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	ଯଦି (partial != ଖାଲି || base_ufunc->uf_partial != ଖାଲି)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    ଯଦି (ectx.ec_outer_ref == ଖାଲି)
		ଯାଅ failed_early;
	    ଯଦି (partial != ଖାଲି)
	    {
		outer_T *outer = get_pt_outer(partial);

		ଯଦି (outer->out_ଗଦା == ଖାଲି && outer->out_loop_size == 0)
		{
		    // no stack was set
		    ଯଦି (current_ectx != ଖାଲି)
		    {
			ଯଦି (current_ectx->ec_outer_ref != ଖାଲି
			       && current_ectx->ec_outer_ref->or_outer != ଖାଲି)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		ଅନ୍ୟଥା
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    ଅନ୍ୟଥା
	    {
		ectx.ec_outer_ref->or_outer = &base_ufunc->uf_partial->pt_outer;
		++base_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = base_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    ପାଇଁ (idx = 0; idx < ଗଦା_FRAME_SIZE; ++idx)
    {
	ଗଦା_TV(ectx.ec_ଗଦା.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_ଗଦା.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	ପାଇଁ (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    ଗଦା_TV_VAR(idx)->v_type = VAR_ସଂଖ୍ୟା;
	    ଗଦା_TV_VAR(idx)->vval.v_ସଂଖ୍ୟା = 0;
	}
	ectx.ec_ଗଦା.ga_len += dfunc->df_varcount;

	ଯଦି (object != ଖାଲି)
	{
	    // the object is always the variable at index zero
	    tv = ଗଦା_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	ଯଦି (dfunc->df_has_closure)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    ଗଦା_TV_VAR(idx)->v_type = VAR_ସଂଖ୍ୟା;
	    ଗଦା_TV_VAR(idx)->vval.v_ସଂଖ୍ୟା = 0;
	    ++ectx.ec_ଗଦା.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    ଯଦି (funccal != ଖାଲି)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_ଠେଲ_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_ସନ୍ଦେଶ_list = ସନ୍ଦେଶ_list;
    ସନ୍ଦେଶ_list = &private_ସନ୍ଦେଶ_list;

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
    ଯଦି (ret == OK)
    {
	// function finished, get result from the stack.
	ଯଦି (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	ଅନ୍ୟଥା
	{
	    tv = ଗଦା_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    ଯଦି (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_closure_in_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_କାଢ଼();
    current_sctx = save_current_sctx;

    ଯଦି (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    ଯଦି (*ସନ୍ଦେଶ_list != ଖାଲି && saved_ସନ୍ଦେଶ_list != ଖାଲି)
    {
	msglist_T **plist = saved_ସନ୍ଦେଶ_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	ଯେତେବେଳେ (*plist != ଖାଲି)
	    plist = &(*plist)->next;

	*plist = *ସନ୍ଦେଶ_list;
    }
    ସନ୍ଦେଶ_list = saved_ସନ୍ଦେଶ_list;

    ଯଦି (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = ଖାଲି;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    ପାଇଁ (idx = 0; idx < ectx.ec_ଗଦା.ga_len; ++idx)
    {
	tv = ଗଦା_TV(idx);
	ଯଦି (tv->v_type != VAR_ସଂଖ୍ୟା && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_ଗଦା.ga_data);
    vim_free(ectx.ec_trystack.ga_data);
    ଯଦି (ectx.ec_outer_ref != ଖାଲି)
    {
	ଯଦି (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    ଯଦି (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    ଫେରନ୍ତୁ ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    ଶୂନ୍ୟ
unwind_def_callstack(ectx_T *ectx)
{
    ଯେତେବେଳେ (ectx->ec_frame_idx != ectx->ec_initial_frame_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    ଶୂନ୍ୟ
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    ଯଦି (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    ସ୍ଥିର char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		depth;

    ga_init2(&ga, 1, 100);
    ପାଇଁ (depth = 0; depth < lvi->lvi_depth; ++depth)
    {
	ଯଦି (ga_grow(&ga, 50) == FAIL)
	    ଭାଙ୍ଗ;
	ଯଦି (lvi->lvi_loop[depth].var_idx == 0)
	    STRCPY((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, " -");
	ଅନ୍ୟଥା
	    vim_snprintf((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[depth].var_idx,
			    lvi->lvi_loop[depth].var_idx
					 + lvi->lvi_loop[depth].var_ଗଣନା - 1);
	ga.ga_len = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(ga.ga_data);
    }
    ଫେରନ୍ତୁ ga.ga_data;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
list_instructions(ଅକ୍ଷର *pfx, isn_T *instr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା instr_ଗଣନା, ufunc_T *ufunc)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଧାଡ଼ି_idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_current = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		current;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    ପାଇଁ (current = 0; current < instr_ଗଣନା; ++current)
    {
	isn_T	    *iptr = &instr[current];
	ଅକ୍ଷର	    *ଧାଡ଼ି;

	ଯଦି (ufunc != ଖାଲି)
	{
	    ଯେତେବେଳେ (ଧାଡ଼ି_idx < iptr->isn_lnum
					  && ଧାଡ଼ି_idx < ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_len)
	    {
		ଯଦି (current > prev_current)
		{
		    ସନ୍ଦେଶ_puts("\n\n");
		    prev_current = current;
		}
		ଧାଡ଼ି = ((ଅକ୍ଷର **)ufunc->uf_ଧାଡ଼ିଗୁଡ଼ିକ.ga_data)[ଧାଡ଼ି_idx++];
		ଯଦି (ଧାଡ଼ି != ଖାଲି)
		    ସନ୍ଦେଶ(ଧାଡ଼ି);
	    }
	    ଯଦି (iptr->isn_type == ISN_JUMP_IF_ARG_SET)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	first_def_arg = ufunc->uf_args.ga_len
						   - ufunc->uf_def_args.ga_len;

		ଯଦି (def_arg_idx > 0)
		    ସନ୍ଦେଶ_puts("\n\n");
		ସନ୍ଦେଶ_start();
		ସନ୍ଦେଶ_puts("  ");
		ସନ୍ଦେଶ_puts(((ଅକ୍ଷର **)(ufunc->uf_args.ga_data))[
						 first_def_arg + def_arg_idx]);
		ସନ୍ଦେଶ_puts(" = ");
		ସନ୍ଦେଶ_puts(((ଅକ୍ଷର **)(ufunc->uf_def_args.ga_data))[def_arg_idx++]);
		ସନ୍ଦେଶ_clr_eos();
		ସନ୍ଦେଶ_end();
	    }
	}

	ବାଛ (iptr->isn_type)
	{
	    ସ୍ଥିତି ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)iptr->isn_arg.construct.construct_size);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_EXEC_SPLIT:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.ସଂଖ୍ୟା ? " append" : "");
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.ସଂଖ୍ୟା));
#endif
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#endif
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, INT_MAX, ଖାଲି);
		ସନ୍ଦେଶ("     -------------");
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_SOURCE:
		{
		    scriptitem_T    *si = SCRIPT_ITEM(iptr->isn_arg.ସଂଖ୍ୟା);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, INT_MAX, ଖାଲି);
		    ସନ୍ଦେଶ("     -------------");
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.ସଂଖ୍ୟା);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_ଗଣନା);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ECHOWINDOW:
		ଯଦି (iptr->isn_arg.echowin.ewin_time > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_ଗଣନା,
				      iptr->isn_arg.echowin.ewin_time);
		ଅନ୍ୟଥା
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_ଗଣନା);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOAD:
		{
		    ଯଦି (iptr->isn_arg.ସଂଖ୍ୟା < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା
							  + ଗଦା_FRAME_SIZE));
		    ଅନ୍ୟଥା
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ଯଦି (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_depth,
					outer->outer_idx + ଗଦା_FRAME_SIZE);
		    ଅନ୍ୟଥା ଯଦି (outer->outer_depth < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_depth);
		    ଅନ୍ୟଥା
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_depth,
					      outer->outer_idx);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ଯଦି (sv == ଖାଲି)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    ଅନ୍ୟଥା
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADS:
	    ସ୍ଥିତି ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_STORE:
		ଯଦି (iptr->isn_arg.ସଂଖ୍ୟା < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.ସଂଖ୍ୟା + ଗଦା_FRAME_SIZE);
		ଅନ୍ୟଥା
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.ସଂଖ୍ୟା);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ଯଦି (outer->outer_depth < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_depth);
		    ଅନ୍ୟଥା
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_depth, outer->outer_idx);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STORES:
	    ସ୍ଥିତି ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ଯଦି (sv == ଖାଲି)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    ଅନ୍ୟଥା
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STOREOPT:
	    ସ୍ଥିତି ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)iptr->isn_arg.ସଂଖ୍ୟା);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_LOAD_CLASSMEMBER:
	    ସ୍ଥିତି ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_LOAD_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		ଭାଙ୍ଗ;

	    // constants
	    ସ୍ଥିତି ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHBOOL:
	    ସ୍ଥିତି ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHFUNC:
		{
		    ଅକ୍ଷର *name = (ଅକ୍ଷର *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == ଖାଲି ? "[none]" : name);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#endif
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#endif
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == ଖାଲି ? "null"
				 : (ଅକ୍ଷର *)iptr->isn_arg.classarg->class_name.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		ଭାଙ୍ଗ;

	    // function call
	    ସ୍ଥିତି ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    internal_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != ଖାଲି
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_ଶୀର୍ଷ ? " top" : "", cpfunc->cpf_argcount);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PCALL_END:
		smsg("%s%4d PCALL end", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)iptr->isn_arg.defer.defer_argcount);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    ଯଦି (extra == ଖାଲି || extra->fre_func_name == ଖାଲି)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    ଅନ୍ୟଥା
			name = extra->fre_func_name;
		    ଯଦି (extra != ଖାଲି && extra->fre_class != ଖାଲି)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    ଅନ୍ୟଥା ଯଦି (extra == ଖାଲି
				     || extra->fre_loopvar_info.lvi_depth == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    ଅନ୍ୟଥା
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ଯଦି (arg->nfa_loopvar_info.lvi_depth == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    ଅନ୍ୟଥା
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == ଖାଲି ? (char_u *)"" : name);
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_JUMP:
		{
		    ଅକ୍ଷର *when = "?";

		    ବାଛ (iptr->isn_arg.jump.jump_when)
		    {
			ସ୍ଥିତି JUMP_ALWAYS:
			    when = "JUMP";
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  // unused
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    ଭାଙ୍ଗ;
			ସ୍ଥିତି JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    ଭାଙ୍ଗ;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_JUMP_IF_ARG_SET:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ଗଦା_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_JUMP_IF_ARG_NOT_SET:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + ଗଦା_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_end);
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->end_funcref_idx,
			    endloop->end_var_idx,
			    endloop->end_var_idx + endloop->end_var_ଗଣନା - 1,
			    endloop->end_depth);
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_end);
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    ଯଦି (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    ଅନ୍ୟଥା
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_levels,
				      trycont->tct_levels == 1 ? "" : "s",
				      trycont->tct_where);
		}
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		ଭାଙ୍ଗ;

	    // expression operations on number
	    ସ୍ଥିତି ISN_OPNR:
	    ସ୍ଥିତି ISN_OPFLOAT:
	    ସ୍ଥିତି ISN_OPANY:
		{
		    ଅକ୍ଷର *what;
		    ଅକ୍ଷର *ins;

		    ବାଛ (iptr->isn_arg.op.op_type)
		    {
			ସ୍ଥିତି ପଦସମଷ୍ଟି_MULT: what = "*"; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_DIV: what = "/"; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_REM: what = "%"; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_SUB: what = "-"; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_ADD: what = "+"; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_LSHIFT: what = "<<"; ଭାଙ୍ଗ;
			ସ୍ଥିତି ପଦସମଷ୍ଟି_RSHIFT: what = ">>"; ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ:       what = "???"; ଭାଙ୍ଗ;
		    }
		    ବାଛ (iptr->isn_type)
		    {
			ସ୍ଥିତି ISN_OPNR: ins = "OPNR"; ଭାଙ୍ଗ;
			ସ୍ଥିତି ISN_OPFLOAT: ins = "OPFLOAT"; ଭାଙ୍ଗ;
			ସ୍ଥିତି ISN_OPANY: ins = "OPANY"; ଭାଙ୍ଗ;
			ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ins = "???"; ଭାଙ୍ଗ;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_COMPAREBOOL:
	    ସ୍ଥିତି ISN_COMPARESPECIAL:
	    ସ୍ଥିତି ISN_COMPARENULL:
	    ସ୍ଥିତି ISN_COMPARENR:
	    ସ୍ଥିତି ISN_COMPAREFLOAT:
	    ସ୍ଥିତି ISN_COMPARESTRING:
	    ସ୍ଥିତି ISN_COMPAREBLOB:
	    ସ୍ଥିତି ISN_COMPARELIST:
	    ସ୍ଥିତି ISN_COMPARETUPLE:
	    ସ୍ଥିତି ISN_COMPAREDICT:
	    ସ୍ଥିତି ISN_COMPAREFUNC:
	    ସ୍ଥିତି ISN_COMPAREOBJECT:
	    ସ୍ଥିତି ISN_COMPAREANY:
		   {
		       ଅକ୍ଷର *p;
		       ଅକ୍ଷର buf[10];
		       ଅକ୍ଷର *type;

		       ବାଛ (iptr->isn_arg.op.op_type)
		       {
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_EQUAL:	 p = "=="; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_NEQUAL:    p = "!="; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_GREATER:   p = ">"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_GEQUAL:    p = ">="; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_SMALLER:   p = "<"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_SEQUAL:    p = "<="; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_MATCH:	 p = "=~"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_IS:	 p = "is"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_ISNOT:	 p = "isnot"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ପଦସମଷ୍ଟି_NOMATCH:	 p = "!~"; ଭାଙ୍ଗ;
			   ପୂର୍ବନିର୍ଦ୍ଧାରିତ:  p = "???"; ଭାଙ୍ଗ;
		       }
		       STRCPY(buf, p);
		       ଯଦି (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       ବାଛ(iptr->isn_type)
		       {
			   ସ୍ଥିତି ISN_COMPAREBOOL: type = "COMPAREBOOL"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPARENULL: type = "COMPARENULL"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPARENR: type = "COMPARENR"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPARESTRING:
						  type = "COMPARESTRING"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPAREBLOB: type = "COMPAREBLOB"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPARELIST: type = "COMPARELIST"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPARETUPLE: type = "COMPARETUPLE"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPAREDICT: type = "COMPAREDICT"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPAREFUNC: type = "COMPAREFUNC"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; ଭାଙ୍ଗ;
			   ସ୍ଥିତି ISN_COMPAREANY: type = "COMPAREANY"; ଭାଙ୍ଗ;
			   ପୂର୍ବନିର୍ଦ୍ଧାରିତ: type = "???"; ଭାଙ୍ଗ;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); ଭାଙ୍ଗ;

	    // expression operations
	    ସ୍ଥିତି ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.ସଂଖ୍ୟା));
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_SLICE: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.ସଂଖ୍ୟା); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_index,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)iptr->isn_arg.classmember.cm_idx);
				     ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)iptr->isn_arg.ସଂଖ୍ୟା); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      ଅକ୍ଷର	    *tofree = ଖାଲି;
		      ଅକ୍ଷର	    *typename;

		      ଯଦି (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_ସଂଖ୍ୟା_OK))
			  typename = "float|number";
		      ଅନ୍ୟଥା ଯଦି (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      ଅନ୍ୟଥା
			  typename = type_name(ct->ct_type, &tofree);

		      ଯଦି (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ct->ct_off);
		      ଅନ୍ୟଥା
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ct->ct_arg_idx);
		      vim_free(tofree);
		      ଭାଙ୍ଗ;
		  }
	    ସ୍ଥିତି ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_more_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_SETTYPE:
		  {
		      ଅକ୍ଷର *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      ଭାଙ୍ଗ;
		  }
	    ସ୍ଥିତି ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_2BOOL: ଯଦି (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    ଅନ୍ୟଥା
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_PUT:
		ଯଦି (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ଅନ୍ୟଥା ଯଦି (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ଅନ୍ୟଥା
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_IPUT:
		ଯଦି (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ଅନ୍ୟଥା ଯଦି (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		ଅନ୍ୟଥା
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  ଖାଲି, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    ଯଦି (likely(buf != ଖାଲି))
		    {
			(ଶୂନ୍ୟ)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    ଭାଙ୍ଗ;
		}
	    ସ୍ଥିତି ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_PROF_START:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_PROF_END:
		smsg("%s%4d PROFILE END", pfx, current);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_names_len);
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_ଗଣନା,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_item,
					 iptr->isn_arg.shuffle.shfl_up);
			      ଭାଙ୍ଗ;
	    ସ୍ଥିତି ISN_DROP: smsg("%s%4d DROP", pfx, current); ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_SCRIPTCTX_SET:
		{
		    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ITEM(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି ISN_FINISH: // End of list of instructions for ISN_SUBSTITUTE.
			   ଫେରନ୍ତୁ;
	}

	out_flush();	    // output one line at a time
	ui_breakcheck();
	ଯଦି (got_int)
	    ଭାଙ୍ଗ;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    ଶୂନ୍ୟ
set_context_in_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_pattern = arg;

    // first argument already typed: only user function names
    ଯଦି (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
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
get_disassemble_argument(expand_T *xp, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    ଯଦି (idx == 0)
	ଫେରନ୍ତୁ (char_u *)"debug";
    ଯଦି (idx == 1)
	ଫେରନ୍ତୁ (char_u *)"profile";
    ଫେରନ୍ତୁ get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    ଶୂନ୍ୟ
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = ଖାଲି;  // init to shut up gcc warning
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		instr_ଗଣନା = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = find_func_by_name(arg, &compile_type);
    ଯଦି (ufunc == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, ଖାଲି) == FAIL)
	ଫେରନ୍ତୁ;
    ଯଦି (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	ଫେରନ୍ତୁ;
    }
    ସନ୍ଦେଶ((ଅକ୍ଷର *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    ବାଛ (compile_type)
    {
	ସ୍ଥିତି CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_ଗଣନା = dfunc->df_instr_prof_ଗଣନା;
	    ଭାଙ୍ଗ;
#endif
	    // FALLTHROUGH
	ସ୍ଥିତି CT_NONE:
	    instr = dfunc->df_instr;
	    instr_ଗଣନା = dfunc->df_instr_ଗଣନା;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_ଗଣନା = dfunc->df_instr_debug_ଗଣନା;
	    ଭାଙ୍ଗ;
    }

    list_instructions("", instr, instr_ଗଣନା, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tv2bool(typval_T *tv)
{
    ବାଛ (tv->v_type)
    {
	ସ୍ଥିତି VAR_ସଂଖ୍ୟା:
	    ଫେରନ୍ତୁ tv->vval.v_ସଂଖ୍ୟା != 0;
	ସ୍ଥିତି VAR_FLOAT:
	    ଫେରନ୍ତୁ tv->vval.v_float != 0.0;
	ସ୍ଥିତି VAR_PARTIAL:
	    ଫେରନ୍ତୁ tv->vval.v_partial != ଖାଲି;
	ସ୍ଥିତି VAR_FUNC:
	ସ୍ଥିତି VAR_STRING:
	    ଫେରନ୍ତୁ tv->vval.v_string != ଖାଲି && *tv->vval.v_string != NUL;
	ସ୍ଥିତି VAR_LIST:
	    ଫେରନ୍ତୁ tv->vval.v_list != ଖାଲି && tv->vval.v_list->lv_len > 0;
	ସ୍ଥିତି VAR_TUPLE:
	    ଫେରନ୍ତୁ tuple_len(tv->vval.v_tuple) > 0;
	ସ୍ଥିତି VAR_DICT:
	    ଫେରନ୍ତୁ tv->vval.v_dict != ଖାଲି
				    && tv->vval.v_dict->dv_hashtab.ht_used > 0;
	ସ୍ଥିତି VAR_BOOL:
	ସ୍ଥିତି VAR_SPECIAL:
	    ଫେରନ୍ତୁ tv->vval.v_ସଂଖ୍ୟା == VVAL_TRUE ? TRUE : FALSE;
	ସ୍ଥିତି VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    ଫେରନ୍ତୁ tv->vval.v_job != ଖାଲି;
#ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    ଫେରନ୍ତୁ tv->vval.v_channel != ଖାଲି;
#ଅନ୍ୟଥା
	    ଭାଙ୍ଗ;
#endif
	ସ୍ଥିତି VAR_BLOB:
	    ଫେରନ୍ତୁ tv->vval.v_blob != ଖାଲି && tv->vval.v_blob->bv_ga.ga_len > 0;

	ସ୍ଥିତି VAR_OBJECT:
	    ଫେରନ୍ତୁ tv->vval.v_object != ଖାଲି;

	ସ୍ଥିତି VAR_CLASS:
	ସ୍ଥିତି VAR_TYPEALIAS:
	    check_typval_is_ମୂଲ୍ୟ(tv);
	    ଭାଙ୍ଗ;

	ସ୍ଥିତି VAR_UNKNOWN:
	ସ୍ଥିତି VAR_ANY:
	ସ୍ଥିତି VAR_VOID:
	ସ୍ଥିତି VAR_INSTR:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ଶୂନ୍ୟ
emsg_using_string_as(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା as_ସଂଖ୍ୟା)
{
    semsg(_(as_ସଂଖ୍ୟା ? e_using_string_as_ସଂଖ୍ୟା_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == ଖାଲି
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_not_string(typval_T *tv)
{
    ଯଦି (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

#endif // FEAT_EVAL
