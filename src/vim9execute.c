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
#include "vim.h"

#ако defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
typedef struct {
    цяло	    tcd_frame_idx;	// ec_frame_idx at ISN_TRY
    цяло	    tcd_stack_len;	// size of ectx.ec_stack at ISN_TRY
    цяло	    tcd_in_catch;	// in catch or finally block
    цяло	    tcd_did_throw;	// set did_throw in :endtry
    цяло	    tcd_catch_idx;	// instruction of the first :catch or :finally
    цяло	    tcd_finally_idx;	// instruction of the :finally block or zero
    цяло	    tcd_endtry_idx;	// instruction of the :endtry
    цяло	    tcd_caught;		// catch block entered
    цяло	    tcd_cont;		// :continue encountered, jump here (minus one)
    цяло	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
typedef struct {
    цяло		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    цяло		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
typedef struct {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    цяло		or_outer_allocated;  // free "or_outer" later
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
struct ectx_S {
    garray_T	ec_stack;	// stack of typval_T values
    цяло		ec_frame_idx;	// index in ec_stack: context of ec_dfunc_idx
    цяло		ec_initial_frame_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    цяло		ec_dfunc_idx;	// current function index
    цяло		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    цяло		ec_did_emsg_before;
    цяло		ec_trylevel_at_start;
    where_T	ec_where;
};

#ifdef FEAT_PROFILE
// stack of profinfo_T used when profiling.
static garray_T profile_info_ga = {0, 0, sizeof(profinfo_T), 20, NULL};
#endif

// Get pointer to item in the stack.
#define STACK_TV(idx) (((typval_T *)ectx->ec_stack.ga_data) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#define STACK_TV_BOT(idx) (((typval_T *)ectx->ec_stack.ga_data) + ectx->ec_stack.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#define STACK_TV_VAR(idx) (((typval_T *)ectx->ec_stack.ga_data) + ectx->ec_frame_idx + STACK_FRAME_SIZE + idx)

// Return value for functions used to execute instructions
#define EXEC_FAIL	0
#define EXEC_OK		1
#define EXEC_DONE	2

    void
to_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_to_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    static цяло
ufunc_argcount(ufunc_T *ufunc)
{
    върни ufunc->uf_args.ga_len + (ufunc->uf_va_name != NULL ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    static цяло
exe_concat(цяло count, ectx_T *ectx)
{
    цяло		idx;
    size_t	len = 0;
    garray_T	ga;
    typedef struct
    {
	typval_T    *tv;
	size_t	    length;
    } string_segment_T;
    enum
    {
	STRING_SEGMENT_CACHE_SIZE = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_SIZE];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    ако (count > (цяло)ARRAY_LENGTH(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, count);
	ако (string_segment_tab == NULL)
	    върни FAIL;
    }

    ga_init2(&ga, sizeof(char), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    за (idx = 0; idx < count; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = STACK_TV_BOT(idx - count);
	ако (segment->tv->vval.v_string != NULL)
	{
	    segment->length = STRLEN(segment->tv->vval.v_string);
	    len += segment->length;
	}
	иначе
	    segment->length = 0;    // Ensure clean state for the second pass
    }

    ако (ga_grow(&ga, (цяло)len + 1) == FAIL)
    {
	ако (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	върни FAIL;
    }

    за (idx = 0; idx < count; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->length);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_stack.ga_len -= count - 1;
    STACK_TV_BOT(-1)->vval.v_string = ga.ga_data;

    ако (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    върни OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    static цяло
exe_newlist(цяло count, ectx_T *ectx)
{
    list_T	*list = NULL;
    цяло		idx;
    typval_T	*tv;

    ако (count >= 0)
    {
	list = list_alloc_with_items(count);
	ако (list == NULL)
	    върни FAIL;
	за (idx = 0; idx < count; ++idx)
	    list_set_item(list, idx, STACK_TV_BOT(idx - count));
    }

    ако (count > 0)
	ectx->ec_stack.ga_len -= count - 1;
    иначе ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
    {
	list_unref(list);
	върни FAIL;
    }
    иначе
	++ectx->ec_stack.ga_len;
    tv = STACK_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_lock = 0;
    ако (list != NULL)
	++list->lv_refcount;
    върни OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    static цяло
exe_newtuple(цяло count, ectx_T *ectx)
{
    tuple_T	*tuple = NULL;
    цяло		idx;
    typval_T	*tv;

    ако (count >= 0)
    {
	tuple = tuple_alloc_with_items(count);
	ако (tuple == NULL)
	    върни FAIL;
	за (idx = 0; idx < count; ++idx)
	    tuple_set_item(tuple, idx, STACK_TV_BOT(idx - count));
    }

    ако (count > 0)
	ectx->ec_stack.ga_len -= count - 1;
    иначе ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
    {
	tuple_unref(tuple);
	върни FAIL;
    }
    иначе
	++ectx->ec_stack.ga_len;
    tv = STACK_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_lock = 0;
    ако (tuple != NULL)
	++tuple->tv_refcount;

    върни OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    static цяло
exe_newdict(цяло count, ectx_T *ectx)
{
    dict_T	*dict = NULL;
    dictitem_T	*item;
    char_u	*key;
    цяло		idx;
    typval_T	*tv;

    ако (count >= 0)
    {
	dict = dict_alloc();
	ако (unlikely(dict == NULL))
	    върни FAIL;
	за (idx = 0; idx < count; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = STACK_TV_BOT(2 * (idx - count));
	    // check key is unique
	    key = tv->vval.v_string == NULL ? (char_u *)"" : tv->vval.v_string;
	    item = dict_find(dict, key, -1);
	    ако (item != NULL)
	    {
		semsg(_(e_duplicate_key_in_dictionary_str), key);
		dict_unref(dict);
		върни MAYBE;
	    }
	    item = dictitem_alloc(key);
	    clear_tv(tv);
	    ако (unlikely(item == NULL))
	    {
		dict_unref(dict);
		върни FAIL;
	    }
	    tv = STACK_TV_BOT(2 * (idx - count) + 1);
	    item->di_tv = *tv;
	    item->di_tv.v_lock = 0;
	    tv->v_type = VAR_UNKNOWN;
	    ако (dict_add(dict, item) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(item);
		dict_unref(dict);
		върни FAIL;
	    }
	}
    }

    ако (count > 0)
	ectx->ec_stack.ga_len -= 2 * count - 1;
    иначе ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
    {
	dict_unref(dict);
	върни FAIL;
    }
    иначе
	++ectx->ec_stack.ga_len;
    tv = STACK_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_lock = 0;
    tv->vval.v_dict = dict;
    ако (dict != NULL)
	++dict->dv_refcount;
    върни OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    void
update_has_breakpoint(ufunc_T *ufunc)
{
    ако (ufunc->uf_debug_tick == debug_tick)
	върни;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

static garray_T dict_stack = GA_EMPTY;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    static цяло
dict_stack_save(typval_T *tv)
{
    ако (dict_stack.ga_growsize == 0)
	ga_init2(&dict_stack, sizeof(typval_T), 10);
    ако (ga_grow(&dict_stack, 1) == FAIL)
	върни FAIL;
    ((typval_T *)dict_stack.ga_data)[dict_stack.ga_len] = *tv;
    ++dict_stack.ga_len;
    върни OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    static typval_T *
dict_stack_get_tv(void)
{
    ако (dict_stack.ga_len == 0)
	върни NULL;
    върни ((typval_T *)dict_stack.ga_data) + dict_stack.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    static dict_T *
dict_stack_get_dict(void)
{
    typval_T *tv;

    ако (dict_stack.ga_len == 0)
	върни NULL;
    tv = ((typval_T *)dict_stack.ga_data) + dict_stack.ga_len - 1;
    ако (tv->v_type == VAR_DICT)
	върни tv->vval.v_dict;
    върни NULL;
}

/*
 * Drop an item from the dict stack.
 */
    static void
dict_stack_drop(void)
{
    ако (dict_stack.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	върни;
    }
    --dict_stack.ga_len;
    clear_tv(((typval_T *)dict_stack.ga_data) + dict_stack.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    static void
dict_stack_clear(цяло len)
{
    докато (dict_stack.ga_len > len)
	dict_stack_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    static outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    ако (ptref == NULL)
	върни &pt->pt_outer;

    // partial using partial (recursively)
    докато (ptref->pt_outer_partial != NULL)
	ptref = ptref->pt_outer_partial;
    върни &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    static цяло
check_ufunc_arg_types(ufunc_T *ufunc, цяло argcount, цяло off, ectx_T *ectx)
{
    ако (ufunc->uf_arg_types == NULL && ufunc->uf_va_type == NULL)
	върни OK;

    typval_T	*argv = STACK_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    за (цяло i = 0; i < argcount; ++i)
    {
	type_T *type = NULL;

	// assume a v:none argument, using the default value, is always OK
	ако (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_number == VVAL_NONE)
	    continue;
	// only pass values to user functions, never types
	ако (check_typval_is_value(&argv[i]) == FAIL)
	    върни FAIL;

	ако (i < ufunc->uf_args.ga_len && ufunc->uf_arg_types != NULL)
	    type = ufunc->uf_arg_types[i];
	иначе ако (ufunc->uf_va_type != NULL)
	    type = ufunc->uf_va_type->tt_member;
	ако (type != NULL && check_typval_arg_type(type,
		    &argv[i], NULL, i + 1) == FAIL)
	    върни FAIL;
    }
    върни OK;
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
    static цяло
call_dfunc(
	цяло		cdf_idx,
	partial_T	*pt_arg,
	цяло		argcount_arg,
	ectx_T		*ectx)
{
    цяло		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    цяло		did_emsg_before = did_emsg_cumul + did_emsg;
    цяло		arg_to_add;
    цяло		vararg_count = 0;
    цяло		varcount;
    цяло		idx;
    estack_T	*entry;
    funclocal_T	*floc = NULL;
    цяло		res = OK;
    compiletype_T compile_type;

    ако (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == NULL ? (char_u *)"unknown" : dfunc->df_name);
	върни FAIL;
    }

#ifdef FEAT_PROFILE
    ако (do_profiling == PROF_YES)
    {
	ако (GA_GROW_OK(&profile_info_ga, 1))
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
    ако (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, NULL);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    }
    ако (res == FAIL || INSTRUCTIONS(dfunc) == NULL)
    {
	ако (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	върни FAIL;
    }

    ако (ufunc->uf_va_name != NULL)
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
	vararg_count = argcount - ufunc->uf_args.ga_len;
	ако (vararg_count < 0)
	    vararg_count = 0;
	иначе
	    argcount -= vararg_count;
	ако (exe_newlist(vararg_count, ectx) == FAIL)
	    върни FAIL;

	vararg_count = 1;
    }

    arg_to_add = ufunc->uf_args.ga_len - argcount;
    ако (arg_to_add < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
			-arg_to_add), -arg_to_add);
	върни FAIL;
    }
    иначе ако (arg_to_add > ufunc->uf_def_args.ga_len)
    {
	цяло missing = arg_to_add - ufunc->uf_def_args.ga_len;

	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
			missing), missing);
	върни FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = STACK_TV_BOT(0) - argcount - vararg_count - 1;

    ако (IS_OBJECT_METHOD(ufunc) && !IS_CONSTRUCTOR_METHOD(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == NULL)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	върни FAIL;
    }

    // Check the argument types.
    ако (check_ufunc_arg_types(ufunc, argcount, vararg_count, ectx) == FAIL)
	върни FAIL;

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
    ако (GA_GROW_FAILS(&ectx->ec_stack,
				     arg_to_add + STACK_FRAME_SIZE + varcount))
	върни FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = STACK_TV_BOT(0) - argcount - vararg_count - 1;

    // If depth of calling is getting too high, don't execute the function.
    ако (funcdepth_increment() == FAIL)
	върни FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    ако (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	ако (floc == NULL)
	    върни FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    ако (vararg_count > 0 && arg_to_add > 0)
	*STACK_TV_BOT(arg_to_add - 1) = *STACK_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    за (idx = 0; idx < arg_to_add; ++idx)
	STACK_TV_BOT(idx - vararg_count)->v_type = VAR_UNKNOWN;
    ectx->ec_stack.ga_len += arg_to_add;

    // Store current execution state in stack frame for ISN_RETURN.
    STACK_TV_BOT(STACK_FRAME_FUNC_OFF)->vval.v_number = ectx->ec_dfunc_idx;
    STACK_TV_BOT(STACK_FRAME_IIDX_OFF)->vval.v_number = ectx->ec_iidx;
    STACK_TV_BOT(STACK_FRAME_INSTR_OFF)->vval.v_string = (void *)ectx->ec_instr;
    STACK_TV_BOT(STACK_FRAME_OUTER_OFF)->vval.v_string =
						    (void *)ectx->ec_outer_ref;
    STACK_TV_BOT(STACK_FRAME_FUNCLOCAL_OFF)->vval.v_string = (void *)floc;
    STACK_TV_BOT(STACK_FRAME_IDX_OFF)->vval.v_number = ectx->ec_frame_idx;
    ectx->ec_frame_idx = ectx->ec_stack.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    цяло initcount = dfunc->df_varcount + (dfunc->df_has_closure ? 1 : 0);
    за (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = STACK_TV_BOT(STACK_FRAME_SIZE + idx);

	tv->v_type = VAR_NUMBER;
	tv->vval.v_number = 0;
    }
    ectx->ec_stack.ga_len += STACK_FRAME_SIZE + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    ако (IS_OBJECT_METHOD(ufunc))
    {
	ако (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_internal_error_str), "type in stack is not an object");
	    върни FAIL;
	}

	*STACK_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != NULL ? pt_arg : ufunc->uf_partial;
    ако (pt != NULL || (ufunc->uf_flags & FC_CLOSURE))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	ако (ref == NULL)
	    върни FAIL;
	ако (pt != NULL)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	иначе
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    ако (unlikely(ref->or_outer == NULL))
	    {
		vim_free(ref);
		върни FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->out_stack = &ectx->ec_stack;
	    ref->or_outer->out_frame_idx = ectx->ec_frame_idx;
	    ако (ectx->ec_outer_ref != NULL)
		ref->or_outer->out_up = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    иначе
	ectx->ec_outer_ref = NULL;

    ако (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    entry = estack_push_ufunc(ufunc, 1);
    ако (entry != NULL)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	entry->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    върни OK;
}

// Double linked list of funcstack_T in use.
static funcstack_T *first_funcstack = NULL;

    static void
add_funcstack_to_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    ако (first_funcstack != NULL)
	first_funcstack->fs_prev = funcstack;
    funcstack->fs_next = first_funcstack;
    funcstack->fs_prev = NULL;
    first_funcstack = funcstack;
}

    static void
remove_funcstack_from_list(funcstack_T *funcstack)
{
    ако (funcstack->fs_prev == NULL)
	first_funcstack = funcstack->fs_next;
    иначе
	funcstack->fs_prev->fs_next = funcstack->fs_next;
    ако (funcstack->fs_next != NULL)
	funcstack->fs_next->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    static цяло
handle_closure_in_use(ectx_T *ectx, цяло free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    цяло		argcount;
    цяло		top;
    цяло		idx;
    typval_T	*tv;
    цяло		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_count;

    ако (dfunc->df_ufunc == NULL)
	върни OK;  // function was freed
    ако (dfunc->df_has_closure == 0)
	върни OK;  // no closures
    tv = STACK_TV(ectx->ec_frame_idx + STACK_FRAME_SIZE + dfunc->df_varcount);
    closure_count = tv->vval.v_number;
    ако (closure_count == 0)
	върни OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    top = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    за (idx = 0; idx < closure_count; ++idx)
    {
	partial_T   *pt;
	цяло	    off = gap->ga_len - closure_count + idx;

	ако (off < 0)
	    continue;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	ако (pt->pt_refcount > 1)
	{
	    цяло refcount = pt->pt_refcount;
	    цяло i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    за (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = STACK_TV(ectx->ec_frame_idx
						       + STACK_FRAME_SIZE + i);
		ако (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ако (refcount > 1)
	    {
		closure_in_use = TRUE;
		break;
	    }
	}
    }

    ако (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *stack;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	ако (funcstack == NULL)
	    върни FAIL;

	funcstack->fs_var_offset = argcount + STACK_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset
							  + dfunc->df_varcount;
	stack = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = stack;
	ако (stack == NULL)
	{
	    vim_free(funcstack);
	    върни FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	за (idx = 0; idx < argcount; ++idx)
	{
	    tv = STACK_TV(top + idx);
	    ако (free_arguments)
	    {
		*(stack + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    иначе ако (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, stack + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	за (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = STACK_TV(ectx->ec_frame_idx + STACK_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    ако (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL)
	    {
		цяло	    i;

		за (i = 0; i < closure_count; ++i)
		    ако (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_count + i])
			++funcstack->fs_min_refcount;
	    }

	    *(stack + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	за (idx = 0; idx < closure_count; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_count + idx];
	    ако (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_stack = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - top;
	    }
	}
    }

    за (idx = 0; idx < closure_count; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_count + idx]);
    gap->ga_len -= closure_count;
    ако (gap->ga_len == 0)
	ga_clear(gap);

    върни OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    цяло
funcstack_check_refcount(funcstack_T *funcstack)
{
    цяло		i;
    garray_T	*gap = &funcstack->fs_ga;
    цяло		done = 0;
    typval_T	*stack;

    ако (funcstack->fs_refcount > funcstack->fs_min_refcount)
	върни FALSE;
    за (i = funcstack->fs_var_offset; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_data) + i;

	ако (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++done;
    }
    ако (done != funcstack->fs_min_refcount)
	върни FALSE;

    stack = gap->ga_data;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    за (i = 0; i < gap->ga_len; ++i)
	clear_tv(stack + i);
    vim_free(stack);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    върни TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    цяло
set_ref_in_funcstacks(цяло copyID)
{
    funcstack_T *funcstack;

    за (funcstack = first_funcstack; funcstack != NULL;
						funcstack = funcstack->fs_next)
    {
	typval_T    *stack = funcstack->fs_ga.ga_data;
	цяло	    i;

	за (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    ако (set_ref_in_item(stack + i, copyID, NULL, NULL, NULL))
		върни TRUE;  // abort
    }
    върни FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
static ectx_T *current_ectx = NULL;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    цяло
in_def_function(void)
{
    върни current_ectx != NULL;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    цяло
fill_exec_lval_root(lval_root_T *root)
{
    ectx_T *ectx = current_ectx;
    ако (ectx != NULL)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	ако (ufunc->uf_class != NULL)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    ако (tv != NULL)
	    {
		CLEAR_POINTER(root);
		root->lr_tv = tv;
		copy_tv(STACK_TV_VAR(0), root->lr_tv);
		root->lr_cl_exec = ufunc->uf_class;
		++root->lr_cl_exec->class_refcount;
		върни OK;
	    }
	}
    }

    върни FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(void)
{
    ectx_T *r = current_ectx;

    current_ectx = NULL;
    върни r;
}

    void
restore_current_ectx(ectx_T *ectx)
{
    ако (current_ectx != NULL)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    static list_T *
add_defer_item(цяло var_idx, цяло argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = STACK_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    ако (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	ако (rettv_list_alloc(defer_tv) == FAIL)
	    върни NULL;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_items(argcount + 1);
    ако (l == NULL)
	върни NULL;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_lock = 0;
    ако (list_insert_tv(defer_l, &listval, defer_l->lv_first) == FAIL)
    {
	list_free(l);
	върни NULL;
    }

    върни l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    static цяло
defer_command(цяло var_idx, цяло argcount, ectx_T *ectx)
{
    list_T	*l = add_defer_item(var_idx, argcount, ectx);
    цяло		i;
    typval_T	*func_tv;

    ако (l == NULL)
	върни FAIL;

    func_tv = STACK_TV_BOT(-argcount - 1);
    ако (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	върни FAIL;
    }
    list_set_item(l, 0, func_tv);

    за (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, STACK_TV_BOT(-argcount + i));
    ectx->ec_stack.ga_len -= argcount + 1;
    върни OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    цяло
add_defer_function(char_u *name, цяло argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    цяло		i;

    ако (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	върни FAIL;
    }

    l = add_defer_item(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    ако (l == NULL)
    {
	vim_free(name);
	върни FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_lock = 0;
    func_tv.vval.v_string = name;
    list_set_item(l, 0, &func_tv);

    за (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, argvars + i);
    върни OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    static void
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = STACK_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    ако (defer_tv->v_type != VAR_LIST)
	върни;	 // no function added
    FOR_ALL_LIST_ITEMS(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ARGS];
	цяло	    i;
	listitem_T  *arg_li = l->lv_first;
	typval_T    *functv = &l->lv_first->li_tv;
	цяло	    argcount = l->lv_len - 1;

	ако (functv->vval.v_string == NULL)
	    // already being called, can happen if function does ":qa"
	    continue;

	за (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_next;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = NULL;
	partial_T	*pt = NULL;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	ако (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = NULL;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    ако (funcexe.fe_object != NULL)
		++funcexe.fe_object->obj_refcount;
	}
	иначе
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = NULL;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(void)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	ако (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	иначе
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    static цяло
func_return(ectx_T *ectx)
{
    цяло		idx;
    цяло		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    цяло		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*entry;
    цяло		prev_dfunc_idx = STACK_TV(ectx->ec_frame_idx
					+ STACK_FRAME_FUNC_OFF)->vval.v_number;
    funclocal_T	*floc;
#ifdef FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_data)
							      + prev_dfunc_idx;

    ако (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	ако (dfunc->df_ufunc->uf_profiling
				   || (caller != NULL && caller->uf_profiling))
	{
	    profile_may_end_func(((profinfo_T *)profile_info_ga.ga_data)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#endif

    ако (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    ако (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    entry = estack_pop();
    ако (entry != NULL)
	current_sctx = entry->es_save_sctx;

    ако (handle_closure_in_use(ectx, TRUE) == FAIL)
	върни FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    цяло top = ectx->ec_frame_idx - argcount;
    ако (IS_OBJECT_METHOD(dfunc->df_ufunc))
	--top;
    за (idx = top; idx < ectx->ec_frame_idx; ++idx)
	clear_tv(STACK_TV(idx));

    // Clear local variables and temp values, but not the return value.
    за (idx = ectx->ec_frame_idx + STACK_FRAME_SIZE;
					idx < ectx->ec_stack.ga_len - 1; ++idx)
	clear_tv(STACK_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_stack.ga_len - 1;
    ако (ret_idx == ectx->ec_frame_idx + STACK_FRAME_IDX_OFF)
	ret_idx = 0;

    ако (ectx->ec_outer_ref != NULL)
    {
	ако (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = STACK_TV(ectx->ec_frame_idx
					+ STACK_FRAME_IIDX_OFF)->vval.v_number;
    ectx->ec_instr = (void *)STACK_TV(ectx->ec_frame_idx
				       + STACK_FRAME_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (void *)STACK_TV(ectx->ec_frame_idx
				       + STACK_FRAME_OUTER_OFF)->vval.v_string;
    floc = (void *)STACK_TV(ectx->ec_frame_idx
				   + STACK_FRAME_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_frame_idx = STACK_TV(ectx->ec_frame_idx
				       + STACK_FRAME_IDX_OFF)->vval.v_number;

    ако (floc == NULL)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    иначе
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    ако (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_stack.ga_len = top + 1;
	*STACK_TV_BOT(-1) = *STACK_TV(ret_idx);
    }
    иначе
	// Reset the stack to the position before the call.
	ectx->ec_stack.ga_len = top;

    funcdepth_decrement();
    --ex_nesting_level;
    върни OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    static цяло
call_prepare(цяло argcount, typval_T *argvars, ectx_T *ectx)
{
    цяло		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    за (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *STACK_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    ако (argcount > 0)
	ectx->ec_stack.ga_len -= argcount - 1;
    иначе ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
	върни FAIL;
    иначе
	++ectx->ec_stack.ga_len;

    // Default return value is zero.
    tv = STACK_TV_BOT(-1);
    tv->v_type = VAR_NUMBER;
    tv->vval.v_number = 0;
    tv->v_lock = 0;

    върни OK;
}

/*
 * Call a builtin function by index.
 */
    static цяло
call_bfunc(цяло func_idx, цяло argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    цяло		idx;
    цяло		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    char	*save_func_name = ectx->ec_where.wt_func_name;

    ако (call_prepare(argcount, argvars, ectx) == FAIL)
	върни FAIL;

    // Check for void value being passed as an argument.
    за (idx = 0; idx < argcount; ++idx)
	ако (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_value));
	    за (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    върни FAIL;
	}

    ectx->ec_where.wt_func_name = internal_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_internal_func_by_idx(func_idx, argvars, STACK_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    за (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ако (did_emsg > did_emsg_before)
	върни FAIL;
    върни OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    static цяло
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	цяло	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    funcexe_T   funcexe;
    funcerror_T	error;
    цяло		idx;
    цяло		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    ако (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, NULL)
								       == FAIL)
	върни FAIL;
    ако (ufunc->uf_def_status == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	ако (error != FCERR_UNKNOWN)
	{
	    ако (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    иначе
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    върни FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	ако (iptr != NULL)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	върни call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    ако (call_prepare(argcount, argvars, ectx) == FAIL)
	върни FAIL;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != NULL ? selfdict : dict_stack_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      STACK_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    за (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    ако (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_found_var);
	върни FAIL;
    }
    ако (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	върни FAIL;
    върни OK;
}

/*
 * If command modifiers were applied restore them.
 */
    static void
may_restore_cmdmod(funclocal_T *funclocal)
{
    ако (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = NULL;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    static цяло
vim9_aborting(цяло prev_uncaught_emsg)
{
    върни uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    static цяло
call_by_name(
	char_u	    *name,
	цяло	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    ако (builtin_function(name, -1))
    {
	цяло func_idx = find_internal_func(name);

	ако (func_idx < 0)  // Impossible?
	    върни FAIL;
	ако (check_internal_func(func_idx, argcount) < 0)
	    върни FAIL;
	върни call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*start_bracket = generic_func_find_open_bracket(name);
    ако (start_bracket != NULL)
    {
	cc = *start_bracket;
	*start_bracket = NUL;
    }

    ufunc = find_func(name, FALSE);

    ако (ufunc == NULL)
    {
	цяло prev_uncaught_emsg = uncaught_emsg;

	ако (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = find_func(name, FALSE);

	ако (vim9_aborting(prev_uncaught_emsg))
	{
	    ако (start_bracket != NULL)
		*start_bracket = cc;
	    върни FAIL;  // bail out if loading the script caused an error
	}
    }

    ако (start_bracket != NULL)
	*start_bracket = cc;

    ако (ufunc != NULL)
    {
	ако (IS_GENERIC_FUNC(ufunc))
	{
	    ако (start_bracket != NULL)
		ufunc = find_generic_func(ufunc, name, &start_bracket);
	    иначе
	    {
		emsg_funcname(e_generic_func_missing_type_args_str, name);
		ufunc = NULL;
	    }
	    ако (ufunc == NULL)
		върни FAIL;
	}
	иначе ако (start_bracket != NULL)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    върни FAIL;
	}

	ако (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    върни FAIL;

	върни call_ufunc(ufunc, NULL, argcount, ectx, iptr, selfdict);
    }

    върни FAIL;
}

    static цяло
call_partial(
	typval_T    *tv,
	цяло	    argcount_arg,
	ectx_T	    *ectx)
{
    цяло		argcount = argcount_arg;
    char_u	*name = NULL;
    цяло		called_emsg_before = called_emsg;
    цяло		res = FAIL;
    dict_T	*selfdict = NULL;

    ако (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	цяло	    i;

	ако (pt->pt_obj != NULL)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		върни FAIL;
	    за (i = 1; i <= argcount; ++i)
		*STACK_TV_BOT(-i + 1) = *STACK_TV_BOT(-i);

	    typval_T *obj_tv = STACK_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_lock = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_stack.ga_len;
	}

	ако (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    ако (GA_GROW_FAILS(&ectx->ec_stack, pt->pt_argc))
		върни FAIL;
	    за (i = 1; i <= argcount; ++i)
		*STACK_TV_BOT(-i + pt->pt_argc) = *STACK_TV_BOT(-i);
	    ectx->ec_stack.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    за (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], STACK_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	ако (pt->pt_func != NULL)
	    върни call_ufunc(pt->pt_func, pt, argcount, ectx, NULL, selfdict);

	name = pt->pt_name;
    }
    иначе ако (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    ако (name != NULL)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = NULL;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	ако (error != FCERR_NONE)
	    res = FAIL;
	иначе
	    res = call_by_name(fname, argcount, ectx, NULL, selfdict);
	vim_free(tofree);
    }

    ако (res == FAIL)
    {
	ако (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == NULL ? (char_u *)"[unknown]" : name);
	върни FAIL;
    }
    върни OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    static цяло
error_if_locked(цяло lock, char *error)
{
    ако (lock & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	върни TRUE;
    }
    върни FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    static цяло
check_for_number(typval_T *tv)
{
    ако (tv->v_type != VAR_NUMBER)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_NUMBER), vartype_name(tv->v_type));
	върни FAIL;
    }
    върни OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    static void
store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    цяло		    flags = ASSIGN_DECL;

    ако (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, NULL, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    static цяло
do_2string(typval_T *tv, цяло is_2string_any, цяло tostring_flags)
{
    ако (tv->v_type == VAR_STRING)
	върни OK;

    char_u *str;

    ако (is_2string_any)
    {
	switch (tv->v_type)
	{
	    case VAR_SPECIAL:
	    case VAR_BOOL:
	    case VAR_NUMBER:
	    case VAR_FLOAT:
	    case VAR_DICT:
	    case VAR_BLOB:	break;

	    case VAR_LIST:
				ако (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, sizeof(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    ако (str == NULL)
					върни FAIL;
				    s = str;
				    докато ((e = vim_strchr(s, '\n')) != NULL)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					ако (p != NULL)
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
				    върни OK;
				}
				ако (tostring_flags & TOSTRING_INTERPOLATE)
				    break;
				// FALLTHROUGH
	    default:	to_string_error(tv->v_type);
			върни FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    върни OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    static void
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    switch (tv->v_type)
    {
	case VAR_LIST:
	    ако (tv->vval.v_list == NULL && sv->sv_type != &t_list_empty)
		(void)rettv_list_alloc(tv);
	    break;
	case VAR_TUPLE:
	    ако (tv->vval.v_tuple == NULL && sv->sv_type != &t_tuple_empty)
		(void)rettv_tuple_alloc(tv);
	    break;
	case VAR_DICT:
	    ако (tv->vval.v_dict == NULL && sv->sv_type != &t_dict_empty)
		(void)rettv_dict_alloc(tv);
	    break;
	case VAR_BLOB:
	    ако (tv->vval.v_blob == NULL && sv->sv_type != &t_blob_null)
		(void)rettv_blob_alloc(tv);
	    break;
	default:
	    break;
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

    ако (str == NULL)
	върни NULL;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    ако (index < 0)
    {
	цяло	clen = 0;

	за (nbyte = 0; nbyte < slen; ++clen)
	{
	    ако (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    иначе ако (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    иначе
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + index;
	ако (nchar < 0)
	    // unlike list: index out of range results in empty string
	    върни NULL;
    }

    за (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	ако (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	иначе ако (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	иначе
	    nbyte += mb_ptr2len(str + nbyte);
    }
    ако (nbyte >= slen)
	върни NULL;
    върни vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    static long
char_idx2byte(char_u *str, size_t str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    size_t	nbyte = 0;

    ако (nchar >= 0)
    {
	докато (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    иначе
    {
	nbyte = str_len;
	докато (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_head_off(str, str + nbyte);
	    ++nchar;
	}
	ако (nchar < 0)
	    върни -1;
    }
    върни (long)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_slice(char_u *str, varnumber_T first, varnumber_T last, цяло exclusive)
{
    long	start_byte, end_byte;
    size_t	slen;

    ако (str == NULL)
	върни NULL;
    slen = STRLEN(str);
    start_byte = char_idx2byte(str, slen, first);
    ако (start_byte < 0)
	start_byte = 0; // first index very negative: use zero
    ако ((last == -1 && !exclusive) || last == VARNUM_MAX)
	end_byte = (long)slen;
    иначе
    {
	end_byte = char_idx2byte(str, slen, last);
	ако (!exclusive && end_byte >= 0 && end_byte < (long)slen)
	    // end index is inclusive
	    end_byte += mb_ptr2len(str + end_byte);
    }

    ако (start_byte >= (long)slen || end_byte <= start_byte)
	върни NULL;
    върни vim_strnsave(str + start_byte, end_byte - start_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    static svar_T *
get_script_svar(scriptref_T *sref, цяло dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? NULL
			      : ((dfunc_T *)def_functions.ga_data) + dfunc_idx;
    svar_T	    *sv;

    ако (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	ако (dfunc != NULL)
	    semsg(_(e_script_variable_invalid_after_reload_in_function_str),
					 printable_func_name(dfunc->df_ufunc));
	върни NULL;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_data) + sref->sref_idx;
    ако (sv->sv_name == NULL)
    {
	ако (dfunc != NULL)
	    emsg(_(e_script_variable_was_deleted));
	върни NULL;
    }
    ако (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	ако (dfunc != NULL)
	    emsg(_(e_script_variable_type_changed));
	върни NULL;
    }

    ако ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	ако (dfunc != NULL)
	    semsg(_(e_item_not_exported_in_script_str), sv->sv_name);
	върни NULL;
    }
    върни sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    static char_u *
get_split_sourceline(
	цяло c UNUSED,
	void *cookie,
	цяло indent UNUSED,
	getline_opt_T options UNUSED)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*p;
    char_u		*line;

    p = vim_strchr(sp->nextline, '\n');
    ако (p == NULL)
    {
	line = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    иначе
    {
	line = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    върни line;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    static цяло
call_eval_func(
	char_u	    *name,
	цяло	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    цяло	    called_emsg_before = called_emsg;
    цяло	    res;

    res = call_by_name(name, argcount, ectx, iptr, NULL);
    ако (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = find_var(name, NULL, FALSE);
	ако (v == NULL || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    върни FAIL;
	}
	върни call_partial(&v->di_tv, argcount, ectx);
    }
    върни res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    цяло
fill_partial_and_closure(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    ако (ufunc->uf_flags & FC_CLOSURE)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.out_stack = &ectx->ec_stack;
	pt->pt_outer.out_frame_idx = ectx->ec_frame_idx;
	ако (ectx->ec_outer_ref != NULL)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.out_up = ectx->ec_outer_ref->or_outer;
	    ако (ectx->ec_outer_ref->or_partial != NULL)
	    {
		pt->pt_outer.out_up_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.out_up_partial->pt_refcount;
	    }
	}

	ако (lvi != NULL)
	{
	    цяло	depth;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    за (depth = 0; depth < lvi->lvi_depth; ++depth)
	    {
		pt->pt_outer.out_loop[depth].stack = &ectx->ec_stack;
		pt->pt_outer.out_loop[depth].var_idx = ectx->ec_frame_idx
			 + STACK_FRAME_SIZE + lvi->lvi_loop[depth].var_idx;
		pt->pt_outer.out_loop[depth].var_count =
					    lvi->lvi_loop[depth].var_count;
	    }
	    pt->pt_outer.out_loop_size = lvi->lvi_depth;
	}
	иначе
	    pt->pt_outer.out_loop_size = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	ако (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    върни FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_stack.ga_data) + ectx->ec_frame_idx
		       + STACK_FRAME_SIZE + dfunc->df_varcount)->vval.v_number;

	((partial_T **)ectx->ec_funcrefs.ga_data)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    върни OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    static цяло
exec_command(isn_T *iptr, char_u *cmd_string)
{
    source_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_FIELD(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    ако (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	върни FAIL;
    върни OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    цяло
may_load_script(цяло sid, цяло *loaded)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);

    ако (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	ако (do_source(si->sn_name, FALSE, DOSO_NONE, NULL) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    върни FAIL;
	}
    }
    върни OK;
}

// used for v_instr of typval of VAR_INSTR
struct instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
typedef struct subs_expr_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    цяло		subs_status;
} subs_expr_T;

// Set when calling do_debug().
static ectx_T	*debug_context = NULL;
static цяло	debug_var_count;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    цяло		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    цяло		    varargs_off;

    ако (ectx == NULL)
	върни NULL;
    dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    за (idx = debug_var_count - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_names.ga_data)[idx];

	// the variable name may be NULL when not available in this block
	ако (varname != NULL && STRCMP(varname, name) == 0)
	    върни STACK_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == NULL ? 0 : 1;
    за (idx = 0; idx < ufunc->uf_args.ga_len; ++idx)
	ако (STRCMP(((char_u **)(ufunc->uf_args.ga_data))[idx], name) == 0)
	    върни STACK_TV(ectx->ec_frame_idx - ufunc->uf_args.ga_len
							  - varargs_off + idx);
    ако (ufunc->uf_va_name != NULL && STRCMP(ufunc->uf_va_name, name) == 0)
	върни STACK_TV(ectx->ec_frame_idx - 1);

    върни NULL;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    цяло
may_break_in_function(ufunc_T *ufunc)
{
    върни ufunc->uf_has_breakpoint || debug_has_expr_breakpoint();
}

    static void
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*line;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    цяло		end_lnum = iptr->isn_lnum;
    garray_T	ga;
    цяло		lnum;

    ако (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	ако (!may_break_in_function(ufunc))
	    върни;

	// check for the next breakpoint if needed
	breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	ако (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    върни;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_count = iptr->isn_arg.debug.dbg_var_names_len;

    за (ni = iptr + 1; ni->isn_type != ISN_FINISH; ++ni)
	ако (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    end_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    break;
	}

    ако (end_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, sizeof(char_u *), 10);
	за (lnum = iptr->isn_lnum; lnum < end_lnum
				     && lnum <= ufunc->uf_lines.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_lines.ga_data)[lnum - 1];

	    ако (p == NULL)
		continue;  // left over from continuation line
	    p = skipwhite(p);
	    ако (*p == '#')
		break;
	    ако (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_data))[ga.ga_len++] = p;
	    ако (STRNCMP(p, "def ", 4) == 0)
		break;
	}
	line = ga_concat_strings(&ga, "  ");
	vim_free(ga.ga_data);
    }
    иначе
	line = ((char_u **)ufunc->uf_lines.ga_data)[iptr->isn_lnum - 1];

    do_debug(line == NULL ? (char_u *)"[empty]" : line);
    debug_context = NULL;

    ако (end_lnum > iptr->isn_lnum)
	vim_free(line);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    static цяло
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    ако (lhs_list->lv_type == NULL || lhs_list->lv_type->tt_member == NULL)
	върни OK;

    върни check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    static цяло
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = STACK_TV_BOT(-2);
    long	lidx = 0;
    typval_T	*tv_dest = STACK_TV_BOT(-1);
    цяло		status = OK;
    цяло		check_rhs_type = FALSE;

    ако (tv_idx->v_type == VAR_NUMBER)
	lidx = (long)tv_idx->vval.v_number;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = STACK_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    ако (dest_type == VAR_OBJECT && tv_dest->vval.v_object == NULL)
    {
	emsg(_(e_using_null_object));
	status = FAIL;
    }
    иначе ако (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	ако (dest_type == VAR_DICT)
	    status = do_2string(tv_idx, TRUE, FALSE);
	иначе ако (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    ако (obj == NULL)
	    {
		emsg(_(e_using_null_object));
		status = FAIL;
	    }
	    иначе
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		цяло		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		ако (m != NULL)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    ако ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == NULL ||
			     !class_instance_of(ufunc->uf_class, cl)))
		    {
			char *msg = (m->ocm_access == VIM_ACCESS_PRIVATE)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(msg, m->ocm_name.string, 0, cl);
			status = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    иначе ако (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			status = FAIL;
		    иначе
			lidx = m_idx;
		}
		иначе
		{
		    member_not_found_msg(cl, VAR_OBJECT, member, 0);
		    status = FAIL;
		}
	    }
	}
	иначе ако (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    status = FAIL;
	}
	иначе ако (dest_type == VAR_LIST && tv_idx->v_type != VAR_NUMBER)
	{
	    emsg(_(e_number_expected));
	    status = FAIL;
	}
    }

    ако (status == OK)
    {
	ако (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    ако (list == NULL)
	    {
		emsg(_(e_list_not_set));
		върни FAIL;
	    }
	    ако (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    ако (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_index_out_of_range_nr), lidx);
		върни FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    ако (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		върни FAIL;

	    ако (lidx < list->lv_len)
	    {
		listitem_T *li = list_find(list, lidx);
		ако (li == NULL)
		    върни FAIL;
		ако (error_if_locked(li->li_tv.v_lock,
					     e_cannot_change_locked_list_item))
		    върни FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    иначе
	    {
		ако (error_if_locked(list->lv_lock, e_cannot_change_locked_list))
		    върни FAIL;
		// append to list, only fails when out of memory
		ако (list_append_tv(list, tv) == FAIL)
		    върни NOTDONE;
		clear_tv(tv);
	    }
	}
	иначе ако (dest_type == VAR_DICT)
	{
	    char_u		*key = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    ако (dict == NULL)
	    {
		emsg(_(e_dictionary_not_set));
		върни FAIL;
	    }
	    ако (key == NULL)
		key = (char_u *)"";
	    di = dict_find(dict, key, -1);
	    ако (di != NULL)
	    {
		ако (error_if_locked(di->di_tv.v_lock,
						    e_cannot_change_dict_item))
		    върни FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    иначе
	    {
		ако (error_if_locked(dict->dv_lock, e_cannot_change_dict))
		    върни FAIL;
		// add to dict, only fails when out of memory
		ако (dict_add_tv(dict, (char *)key, tv) == FAIL)
		    върни NOTDONE;
		clear_tv(tv);
	    }
	}
	иначе ако (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    цяло		    error = FALSE;
	    цяло		    len;

	    ако (blob == NULL)
	    {
		emsg(_(e_blob_not_set));
		върни FAIL;
	    }
	    len = blob_len(blob);
	    ако (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    ако (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), lidx);
		върни FAIL;
	    }
	    ако (value_check_lock(blob->bv_lock, (char_u *)"blob", FALSE))
		върни FAIL;
	    nr = tv_get_number_chk(tv, &error);
	    ако (error)
		върни FAIL;
	    ако (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_value_for_blob_nr), nr);
		върни FAIL;
	    }
	    blob_set_append(blob, lidx, nr);
	}
	иначе ако (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    ако (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		ако (itf != NULL)
		    // convert interface member index to class member index
		    lidx = object_index_from_itf_index(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    иначе
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	иначе ако (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    status = FAIL;
	}
	иначе
	{
	    status = FAIL;
	    semsg(_(e_cannot_index_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_stack.ga_len -= 3;
    ако (status == FAIL)
    {
	clear_tv(tv);
	върни FAIL;
    }
    върни OK;
}

/*
 * Store a value in a list or blob range.
 */
    static цяло
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = STACK_TV_BOT(-3);
    typval_T	*tv_idx2 = STACK_TV_BOT(-2);
    typval_T	*tv_dest = STACK_TV_BOT(-1);
    цяло		status = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = STACK_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    ако (tv_dest->v_type == VAR_LIST)
    {
	long	    n1;
	long	    n2;
	listitem_T  *li1;

	n1 = (long)tv_get_number_chk(tv_idx1, NULL);
	ако (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_number == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	иначе
	    n2 = (long)tv_get_number_chk(tv_idx2, NULL);

	li1 = check_range_index_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	ако (li1 == NULL)
	    status = FAIL;
	иначе
	{
	    status = check_range_index_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    ако (status != FAIL)
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
    иначе ако (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	long	    bloblen;

	n1 = tv_get_number_chk(tv_idx1, NULL);
	ако (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_number == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	иначе
	    n2 = tv_get_number_chk(tv_idx2, NULL);
	bloblen = blob_len(tv_dest->vval.v_blob);

	ако (check_blob_index(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    status = FAIL;
	иначе
	    status = blob_set_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    иначе
    {
	status = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_stack.ga_len -= 4;
    clear_tv(tv);

    върни status;
}

/*
 * Unlet item in list or dict variable.
 */
    static цяло
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = STACK_TV_BOT(-2);
    typval_T	*tv_dest = STACK_TV_BOT(-1);
    цяло		status = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    ако (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	ако (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_NUMBER)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    status = FAIL;
	}
	иначе
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*key;
	    dictitem_T  *di = NULL;

	    ако (d != NULL && value_check_lock(
				      d->dv_lock, NULL, FALSE))
		status = FAIL;
	    иначе
	    {
		ако (tv_idx->v_type == VAR_STRING)
		{
		    key = tv_idx->vval.v_string;
		    ако (key == NULL)
			key = (char_u *)"";
		}
		иначе
		{
		    key = tv_get_string(tv_idx);
		}
		ако (d != NULL)
		    di = dict_find(d, key, (цяло)STRLEN(key));
		ако (di == NULL)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_key_not_present_in_dictionary_str), key);
		    status = FAIL;
		}
		иначе ако (var_check_fixed(di->di_flags,
						   NULL, FALSE)
			|| var_check_ro(di->di_flags,
						  NULL, FALSE))
		    status = FAIL;
		иначе
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    иначе ако (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	ако (check_for_number(tv_idx) == FAIL)
	{
	    status = FAIL;
	}
	иначе
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n = (long)tv_idx->vval.v_number;

	    ако (l != NULL && value_check_lock(
				      l->lv_lock, NULL, FALSE))
		status = FAIL;
	    иначе
	    {
		listitem_T	*li = list_find(l, n);

		ако (li == NULL)
		{
		    semsg(_(e_list_index_out_of_range_nr), n);
		    status = FAIL;
		}
		иначе
		    listitem_remove(l, li);
	    }
	}
    }
    иначе
    {
	status = FAIL;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_stack.ga_len -= 2;

    върни status;
}

/*
 * Unlet a range of items in a list variable.
 */
    static цяло
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = STACK_TV_BOT(-3);
    typval_T	*tv_idx2 = STACK_TV_BOT(-2);
    typval_T	*tv_dest = STACK_TV_BOT(-1);
    цяло		status = OK;

    ако (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	ако (check_for_number(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_number(tv_idx2) == FAIL))
	{
	    status = FAIL;
	}
	иначе
	{
	    list_T	*l = tv_dest->vval.v_list;
	    long	n1 = (long)tv_idx1->vval.v_number;
	    long	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (long)tv_idx2->vval.v_number;
	    listitem_T	*li;

	    li = list_find_index(l, &n1);
	    ако (li == NULL)
	    {
		semsg(_(e_list_index_out_of_range_nr),
						 (long)tv_idx1->vval.v_number);
		status = FAIL;
	    }
	    иначе
	    {
		ако (n1 < 0)
		    n1 = list_idx_of_item(l, li);
		ако (n2 < 0)
		{
		    listitem_T *li2 = list_find(l, n2);

		    ако (li2 == NULL)
		    {
			semsg(_(e_list_index_out_of_range_nr), n2);
			status = FAIL;
		    }
		    иначе
			n2 = list_idx_of_item(l, li2);
		}
		ако (status != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_index_out_of_range_nr), n2);
		    status = FAIL;
		}
		ако (status != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    иначе
    {
	status = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_stack.ga_len -= 3;

    върни status;
}

/*
 * Top of a for loop.
 */
    static цяло
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    цяло		jump = FALSE;
    typval_T	*ltv = STACK_TV_BOT(-1);
    typval_T	*idxtv =
		   STACK_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
	върни FAIL;
    ако (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_number;
	ако (list == NULL
		       || idxtv->vval.v_number >= list->lv_len)
	{
	    jump = TRUE;
	}
	иначе ако (list->lv_first == &range_list_item)
	{
	    // non-materialized range() list
	    tv = STACK_TV_BOT(0);
	    tv->v_type = VAR_NUMBER;
	    tv->v_lock = 0;
	    tv->vval.v_number = list_find_nr(
			     list, idxtv->vval.v_number, NULL);
	    ++ectx->ec_stack.ga_len;
	}
	иначе
	{
	    listitem_T *li = list_find(list,
					 idxtv->vval.v_number);
	    ако (li == NULL)
		върни FAIL;
	    copy_tv(&li->li_tv, STACK_TV_BOT(0));
	    ++ectx->ec_stack.ga_len;
	}
    }
    иначе ако (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_number;
	ако (tuple == NULL || idxtv->vval.v_number >= TUPLE_LEN(tuple))
	    jump = TRUE;
	иначе
	{
	    copy_tv(TUPLE_ITEM(tuple, idxtv->vval.v_number), STACK_TV_BOT(0));
	    ++ectx->ec_stack.ga_len;
	}
    }
    иначе ако (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_number;
	ако (str == NULL || str[idxtv->vval.v_number] == NUL)
	{
	    jump = TRUE;
	}
	иначе
	{
	    цяло	clen = mb_ptr2len(str + idxtv->vval.v_number);

	    // Push the next character from the string.
	    tv = STACK_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_number, clen);
	    ++ectx->ec_stack.ga_len;
	    idxtv->vval.v_number += clen - 1;
	}
    }
    иначе ако (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	ако (idxtv->vval.v_number == -1 && blob != NULL)
	{
	    ltv->v_lock = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_number;
	ако (blob == NULL || idxtv->vval.v_number >= blob_len(blob))
	{
	    jump = TRUE;
	}
	иначе
	{
	    // Push the next byte from the blob.
	    tv = STACK_TV_BOT(0);
	    tv->v_type = VAR_NUMBER;
	    tv->vval.v_number = blob_get(blob,
					 idxtv->vval.v_number);
	    ++ectx->ec_stack.ga_len;
	}
    }
    иначе
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	върни FAIL;
    }

    ако (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_end;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    иначе
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = STACK_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_number = ectx->ec_funcrefs.ga_len;
    }

    върни OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
static loopvars_T *first_loopvars = NULL;

    static void
add_loopvars_to_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    ако (first_loopvars != NULL)
	first_loopvars->lvs_prev = loopvars;
    loopvars->lvs_next = first_loopvars;
    loopvars->lvs_prev = NULL;
    first_loopvars = loopvars;
}

    static void
remove_loopvars_from_list(loopvars_T *loopvars)
{
    ако (loopvars->lvs_prev == NULL)
	first_loopvars = loopvars->lvs_next;
    иначе
	loopvars->lvs_prev->lvs_next = loopvars->lvs_next;
    ако (loopvars->lvs_next != NULL)
	loopvars->lvs_next->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    static цяло
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = STACK_TV_VAR(endloop->end_funcref_idx);
    цяло		prev_closure_count = tv_refcount->vval.v_number;
    цяло		depth = endloop->end_depth;
    garray_T	*gap = &ectx->ec_funcrefs;
    цяло		closure_in_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *stack;
    цяло		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    за (idx = prev_closure_count; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	ако (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == NULL)
	{
	    цяло refcount = pt->pt_refcount;
	    цяло i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    за (i = 0; i < endloop->end_var_count; ++i)
	    {
		typval_T *stv = STACK_TV_VAR(endloop->end_var_idx + i);

		ако (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    ако (refcount > 1)
	    {
		closure_in_use = TRUE;
		break;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    ако (!closure_in_use)
	върни OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    ако (loopvars == NULL)
	върни FAIL;

    loopvars->lvs_ga.ga_len = endloop->end_var_count;
    stack = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_data = stack;
    ако (stack == NULL)
    {
	vim_free(loopvars);
	върни FAIL;
    }
    add_loopvars_to_list(loopvars);

    // Move the variable values.
    за (idx = 0; idx < endloop->end_var_count; ++idx)
    {
	typval_T *tv = STACK_TV_VAR(endloop->end_var_idx + idx);

	*(stack + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    за (idx = prev_closure_count; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	ако (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == NULL)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[depth] = loopvars;

	    pt->pt_outer.out_loop[depth].stack = &loopvars->lvs_ga;
	    pt->pt_outer.out_loop[depth].var_idx -=
		  ectx->ec_frame_idx + STACK_FRAME_SIZE + endloop->end_var_idx;
	}
    }

    върни OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    цяло
loopvars_check_refcount(loopvars_T *loopvars)
{
    цяло		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    цяло		    done = 0;
	typval_T	*stack = gap->ga_data;

    ако (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	върни FALSE;
    за (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_data) + i;

	ако (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != NULL
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    цяло	    depth;

	    за (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		ако (tv->vval.v_partial->pt_loopvars[depth] == loopvars)
		    ++done;
	}
    }
    ако (done != loopvars->lvs_min_refcount)
	върни FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    stack = gap->ga_data;
    за (i = 0; i < gap->ga_len; ++i)
	clear_tv(stack + i);
    vim_free(stack);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    върни TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    цяло
set_ref_in_loopvars(цяло copyID)
{
    loopvars_T *loopvars;

    за (loopvars = first_loopvars; loopvars != NULL;
						 loopvars = loopvars->lvs_next)
    {
	typval_T    *stack = loopvars->lvs_ga.ga_data;
	цяло	    i;

	за (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    ако (set_ref_in_item(stack + i, copyID, NULL, NULL, NULL))
		върни TRUE;  // abort
    }
    върни FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    static цяло
load_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = NULL;
    hashtab_T	*ht = NULL;
    char	namespace;

    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
	върни NOTDONE;
    switch (isn_type)
    {
	case ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    break;
	case ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    break;
	case ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    break;
	case ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    break;
	default:  // Cannot reach here
	    върни NOTDONE;
    }
    di = find_var_in_ht(ht, 0, iptr->isn_arg.string, TRUE);

    ако (di == NULL)
    {
	ако (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = find_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    ако (ufunc != NULL)
	    {
		typval_T    *tv = STACK_TV_BOT(0);

		++ectx->ec_stack.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		ако (tv->vval.v_string == NULL)
		    върни FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		върни OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	ако (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != NULL)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_item_not_found_in_script_str), iptr->isn_arg.string);
	иначе
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = STACK_TV_BOT(0);
	tv->v_type = VAR_NUMBER;
	tv->v_lock = 0;
	tv->vval.v_number = 0;
	++ectx->ec_stack.ga_len;
	върни FAIL;
    }
    иначе
    {
	copy_tv(&di->di_tv, STACK_TV_BOT(0));
	++ectx->ec_stack.ga_len;
    }
    върни OK;
}


    static void
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, sizeof(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    char *tofree = NULL;
    char *typename = type_name(type, &tofree);
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
    static цяло
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    цяло		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = NULL;
    typval_T	mtv;

    ако (is_object)
    {
	obj = tv->vval.v_object;
	ако (obj == NULL)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    върни FAIL;
	}
	tv_cl = obj->obj_class;
    }
    иначе
    {
	tv_cl = tv->vval.v_class;
	ако (tv_cl == NULL)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    върни FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    цяло		namelen = (цяло)STRLEN(iptr->isn_arg.string);
    цяло		save_did_emsg = did_emsg;

    ако (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	върни OK;
    }

    ако (did_emsg != save_did_emsg)
	върни FAIL;

    // could be a class or instance method
    ufunc_T	*oc_method;
    цяло		oc_method_idx;

    oc_method = method_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_method_idx);
    ако (oc_method == NULL)
    {
	char	*msg;

	SOURCING_LNUM = iptr->isn_lnum;
	ако (is_object)
	    msg = e_variable_not_found_on_object_str_str;
	иначе
	    msg = e_class_variable_str_not_found_in_class_str;
	semsg(_(msg), iptr->isn_arg.string, tv_cl->class_name.string);
	върни FAIL;
    }

    // Protected methods are not accessible outside the class
    ако (*oc_method->uf_name == '_'
			&& !class_instance_of(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_method_str), oc_method->uf_name);
	върни FAIL;
    }

    // Create a partial for the instance or class method
    ако (obj_method_to_partial_tv(is_object ? obj : NULL, oc_method, tv)
								== FAIL)
	върни FAIL;

    върни OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    static void
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, цяло fixindent)
{
    цяло		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*expr = NULL;
    цяло		dir = FORWARD;

    ако (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = STACK_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_number;
	ако (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_stack.ga_len;
    }
    иначе ако (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    иначе ако (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	ако (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    ако (regname == '=')
    {
	tv = STACK_TV_BOT(-1);
	ако (tv->v_type == VAR_STRING)
	    expr = tv->vval.v_string;
	иначе
	{
	    expr = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_stack.ga_len;
    }
    check_cursor();

    ако (fixindent)
	do_put(regname, expr, dir, 1L, PUT_LINE|PUT_CURSLINE|PUT_FIXINDENT);
    иначе
	do_put(regname, expr, dir, 1L, PUT_LINE|PUT_CURSLINE);
    vim_free(expr);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    static цяло
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    цяло		count = iptr->isn_arg.unpack.unp_count;
    цяло		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    цяло		i;

    l = tv->vval.v_list;
    ако (l == NULL
	    || l->lv_len < (semicolon ? count - 1 : count))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_value_does_not_have_enough_items));
	върни EXEC_FAIL;
    }
    иначе ако (!semicolon && l->lv_len > count)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_value_has_more_items_than_targets));
	върни EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    ако (GA_GROW_FAILS(&ectx->ec_stack, count - 1))
	върни EXEC_DONE;
    ectx->ec_stack.ga_len += count - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ако (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_items(l->lv_len - count + 1);

	ако (rem_list == NULL)
	    върни EXEC_DONE;
	tv = STACK_TV_BOT(-count);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	li = l->lv_first;
	за (i = 0; i < count - 1; ++i)
	    li = li->li_next;
	за (i = 0; li != NULL; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_set_item(rem_list, i, &tvcopy);
	    li = li->li_next;
	}
	--count;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_first;
    за (i = 0; i < count; ++i)
    {
	tv = STACK_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_next;
    }

    list_unref(l);

    върни EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    static цяло
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    цяло		count = iptr->isn_arg.unpack.unp_count;
    цяло		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    цяло		i;

    tuple = tv->vval.v_tuple;
    ако (tuple == NULL
	    || TUPLE_LEN(tuple) < (semicolon ? count - 1 : count))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_more_targets_than_tuple_items));
	върни EXEC_FAIL;
    }
    иначе ако (!semicolon && TUPLE_LEN(tuple) > count)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_items));
	върни EXEC_FAIL;
    }

    ако (GA_GROW_FAILS(&ectx->ec_stack, count - 1))
	върни EXEC_DONE;
    ectx->ec_stack.ga_len += count - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    ако (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_items(TUPLE_LEN(tuple) - count + 1);

	ако (rem_list == NULL)
	    върни EXEC_DONE;
	tv = STACK_TV_BOT(-count);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	за (i = count - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ITEM(tuple, i), &tvcopy);
	    list_set_item(rem_list, i - (count - 1), &tvcopy);
	}
	--count;
    }

    // Produce the values in reverse order, first item last.
    за (i = 0; i < count; ++i)
    {
	tv = STACK_TV_BOT(-i - 1);
	copy_tv(TUPLE_ITEM(tuple, i), tv);
    }

    tuple_unref(tuple);

    върни EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    static цяло
exec_instructions(ectx_T *ectx)
{
    цяло		ret = FAIL;
    цяло		save_trylevel_at_start = ectx->ec_trylevel_at_start;
    цяло		dict_stack_len_at_start = dict_stack.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_start = trylevel;

    за (;;)
    {
	static цяло  breakcheck_count = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	ако (unlikely(++breakcheck_count >= 100))
	{
	    line_breakcheck();
	    breakcheck_count = 0;
	}
	ако (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    ако (throw_exception("Vim:Interrupt", ET_INTERRUPT, NULL) == FAIL)
		goto theend;
	    did_throw = TRUE;
	}

	ако (unlikely(did_emsg && msg_list != NULL && *msg_list != NULL))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    ако (throw_exception(*msg_list, ET_ERROR, NULL) == FAIL)
		goto theend;
	    did_throw = TRUE;
	    *msg_list = NULL;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    ако (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		ако (trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		{
		    ако (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			finish_exception(caught_stack);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	ако (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = NULL;
	    цяло		index = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    докато (index > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_data) + index - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		ако (!trycmd->tcd_in_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    break;
		// In the catch and finally block of this try we have to go up
		// one level.
		--index;
		trycmd = NULL;
	    }
	    ако (trycmd != NULL && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
	    {
		ако (trycmd->tcd_in_catch)
		{
		    ако (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    иначе
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		иначе
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_in_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    иначе
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    goto theend;
		tv = STACK_TV_BOT(0);
		tv->v_type = VAR_NUMBER;
		tv->vval.v_number = 0;
		++ectx->ec_stack.ga_len;
		ако (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    ако (handle_closure_in_use(ectx, FALSE) == FAIL)
			goto theend;
		    goto done;
		}

		ако (func_return(ectx) == FAIL)
		    goto theend;
	    }
	    continue;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	switch (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    case ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = STACK_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		break;

	    // execute Ex command line
	    case ISN_EXEC:
		ако (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    goto on_error;
		break;

	    // execute Ex command line split at NL characters.
	    case ISN_EXEC_SPLIT:
		{
		    source_cookie_T cookie;
		    char_u	    *line;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    line = get_split_sourceline(0, &cookie, 0, 0);
		    ако (do_cmdline(line,
				get_split_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(line);
			goto on_error;
		    }
		    vim_free(line);
		}
		break;

	    // execute Ex command line that is only a range
	    case ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    char	*error = NULL;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = CMD_SIZE;
		    ea.addr_type = ADDR_LINES;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_address(&ea, &error, FALSE);
		    ако (ea.cmd == NULL)
			goto on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    ако (error != NULL)
		    {
			emsg(error);
			goto on_error;
		    }
		}
		break;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    case ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    цяло	    res;
		    цяло	    save_flags = cmdmod.cmod_flags;

		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    tv = STACK_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, NULL, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    ако (res == FAIL)
			goto on_error;
		    ++ectx->ec_stack.ga_len;
		}
		break;

	    // push typeval VAR_INSTR with instructions to be executed
	    case ISN_INSTR:
		{
		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    tv = STACK_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    ако (tv->vval.v_instr == NULL)
			goto on_error;
		    ++ectx->ec_stack.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		break;

	    case ISN_SOURCE:
		{
		    цяло notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (may_load_script((цяло)iptr->isn_arg.number, &notused)
								       == FAIL)
			goto on_error;
		}
		break;

	    // execute :substitute with an expression
	    case ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    source_cookie_T	cookie;
		    struct subs_expr_S	*save_instr = substitute_instr;
		    struct subs_expr_S	subs_instr;
		    цяло			res;

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

		    ако (res == FAIL || did_emsg
					     || subs_instr.subs_status == FAIL)
			goto on_error;
		}
		break;

	    case ISN_FINISH:
		goto done;

	    case ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		ако (alloc_redir_lval() == FAIL)
		    goto on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		break;

	    case ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    {
			vim_free(res);
			goto theend;
		    }
		    tv = STACK_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_stack.ga_len;
		}
		break;

	    case ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		force_abort = TRUE;
		ако (trigger_cexpr_autocmd(iptr->isn_arg.number) == FAIL)
		    goto on_error;
		force_abort = FALSE;
#endif
		break;

	    case ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    exarg_T ea;
		    цяло	    res;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_stack.ga_len;
		    tv = STACK_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    ако (res == FAIL)
			goto on_error;
		}
#endif
		break;

	    // execute Ex command from pieces on the stack
	    case ISN_EXECCONCAT:
		{
		    цяло	    count = iptr->isn_arg.number;
		    size_t  len = 0;
		    цяло	    pass;
		    цяло	    i;
		    char_u  *cmd = NULL;
		    char_u  *str;

		    за (pass = 1; pass <= 2; ++pass)
		    {
			за (i = 0; i < count; ++i)
			{
			    tv = STACK_TV_BOT(i - count);
			    str = tv->vval.v_string;
			    ако (str != NULL && *str != NUL)
			    {
				ако (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    ако (pass == 2)
				clear_tv(tv);
			}
			ако (pass == 1)
			{
			    cmd = alloc(len + 1);
			    ако (unlikely(cmd == NULL))
				goto theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		break;

	    // execute :echo {string} ...
	    case ISN_ECHO:
		{
		    цяло count = iptr->isn_arg.echo.echo_count;
		    цяло	atstart = TRUE;
		    цяло needclr = TRUE;
		    цяло	idx;

		    за (idx = 0; idx < count; ++idx)
		    {
			tv = STACK_TV_BOT(idx - count);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    ако (needclr)
			msg_clr_eos();
		    ectx->ec_stack.ga_len -= count;
		}
		break;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    case ISN_EXECUTE:
	    case ISN_ECHOMSG:
	    case ISN_ECHOWINDOW:
	    case ISN_ECHOCONSOLE:
	    case ISN_ECHOERR:
		{
		    цяло		count;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    цяло		len;
		    цяло		failed = FALSE;
		    цяло		idx;

		    ако (iptr->isn_type == ISN_ECHOWINDOW)
			count = iptr->isn_arg.echowin.ewin_count;
		    иначе
			count = iptr->isn_arg.number;
		    ga_init2(&ga, 1, 80);
		    за (idx = 0; idx < count; ++idx)
		    {
			tv = STACK_TV_BOT(idx - count);
			ако (iptr->isn_type == ISN_EXECUTE)
			{
			    ако (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_JOB)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_value_as_string_str),
						    vartype_name(tv->v_type));
				break;
			    }
			    иначе
				p = tv_get_string_buf(tv, buf);
			}
			иначе
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (цяло)STRLEN(p);
			ако (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			иначе
			{
			    ако (ga.ga_len > 0)
				((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_stack.ga_len -= count;
		    ако (failed)
		    {
			ga_clear(&ga);
			goto on_error;
		    }

		    ако (ga.ga_data != NULL)
		    {
			ако (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_data);
			    ако (did_emsg)
			    {
				ga_clear(&ga);
				goto on_error;
			    }
			}
			иначе
			{
			    msg_sb_eol();
			    ако (iptr->isn_type == ISN_ECHOMSG)
			    {
				msg_attr(ga.ga_data, echo_attr);
				out_flush();
			    }
#ifdef HAS_MESSAGE_WINDOW
			    иначе ако (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				start_echowindow(
					      iptr->isn_arg.echowin.ewin_time);
				msg_attr(ga.ga_data, echo_attr);
				end_echowindow();
			    }
#endif
			    иначе ако (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_data, (цяло)STRLEN(ga.ga_data),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    иначе
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_data);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		break;

	    // load local variable or argument
	    case ISN_LOAD:
		ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    goto theend;
		tv = STACK_TV_VAR(iptr->isn_arg.number);
		ако (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    STACK_TV_BOT(0)->v_type = VAR_SPECIAL;
		    STACK_TV_BOT(0)->vval.v_number = VVAL_NONE;
		}
		иначе
		    copy_tv(tv, STACK_TV_BOT(0));
		++ectx->ec_stack.ga_len;
		break;

	    // load v: variable
	    case ISN_LOADV:
		ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    goto theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.number), STACK_TV_BOT(0));
		++ectx->ec_stack.ga_len;
		break;

	    // load s: variable in Vim9 script
	    case ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ако (sv == NULL)
			goto theend;
		    allocate_if_null(sv);
		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    copy_tv(sv->sv_tv, STACK_TV_BOT(0));
		    ++ectx->ec_stack.ga_len;
		}
		break;

	    // load s: variable in old script or autoload import
	    case ISN_LOADS:
	    case ISN_LOADEXPORT:
		{
		    цяло		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = find_var_in_ht(ht, 0, name, TRUE);

		    ако (di == NULL)
		    {
			ufunc_T	*ufunc = NULL;

			ако (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (void)find_exported(sid, name, &ufunc, &type,
							 NULL, NULL, FALSE);
			}
			ако (ufunc == NULL)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    goto on_error;
			}
			ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			    goto theend;
			tv = STACK_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_stack.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    иначе
		    {
			ако (iptr->isn_type == ISN_LOADEXPORT)
			{
			    цяло idx = get_script_item_idx(sid, name, 0,
								   NULL, NULL);
			    svar_T	*sv;

			    ако (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;
				ако ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    goto on_error;
				}
			    }
			}
			ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			    goto theend;
			copy_tv(&di->di_tv, STACK_TV_BOT(0));
			++ectx->ec_stack.ga_len;
		    }
		}
		break;

	    // load g:/b:/w:/t: variable
	    case ISN_LOADG:
	    case ISN_LOADB:
	    case ISN_LOADW:
	    case ISN_LOADT:
		{
		    цяло res = load_namespace_var(ectx, iptr->isn_type, iptr);

		    ако (res == NOTDONE)
			goto theend;
		    ако (res == FAIL)
			goto on_error;
		}

		break;

	    // load autoload variable
	    case ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (eval_variable(name, (цяло)STRLEN(name), 0,
			      STACK_TV_BOT(0), NULL, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = STACK_TV_BOT(0);
			tv->v_type = VAR_NUMBER;
			tv->v_lock = 0;
			tv->vval.v_number = 0;
			++ectx->ec_stack.ga_len;
			goto on_error;
		    }
		    ++ectx->ec_stack.ga_len;
		}
		break;

	    // load g:/b:/w:/t: namespace
	    case ISN_LOADGDICT:
	    case ISN_LOADBDICT:
	    case ISN_LOADWDICT:
	    case ISN_LOADTDICT:
		{
		    dict_T *d = NULL;

		    switch (iptr->isn_type)
		    {
			case ISN_LOADGDICT: d = get_globvar_dict(); break;
			case ISN_LOADBDICT: d = curbuf->b_vars; break;
			case ISN_LOADWDICT: d = curwin->w_vars; break;
			case ISN_LOADTDICT: d = curtab->tp_vars; break;
			default:  // Cannot reach here
			    goto theend;
		    }
		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    tv = STACK_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_lock = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_stack.ga_len;
		}
		break;

	    // load &option
	    case ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    ако (eval_option(&name, &optval, TRUE) == FAIL)
			goto theend;
		    *STACK_TV_BOT(0) = optval;
		    ++ectx->ec_stack.ga_len;
		}
		break;

	    // load $ENV
	    case ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    // name is always valid, checked when compiling
		    (void)eval_env_var(&name, &optval, TRUE);
		    *STACK_TV_BOT(0) = optval;
		    ++ectx->ec_stack.ga_len;
		}
		break;

	    // load @register
	    case ISN_LOADREG:
		ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    goto theend;
		tv = STACK_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.number, GREG_EXPR_SRC);
		++ectx->ec_stack.ga_len;
		break;

	    // store local variable
	    case ISN_STORE:
		--ectx->ec_stack.ga_len;
		tv = STACK_TV_VAR(iptr->isn_arg.number);
		ако (check_typval_is_value(STACK_TV_BOT(0)) == FAIL
			|| STACK_TV_BOT(0)->v_type == VAR_VOID)
		{
		    ако (STACK_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_value));
		    clear_tv(STACK_TV_BOT(0));
		    goto on_error;
		}
		clear_tv(tv);
		*tv = *STACK_TV_BOT(0);
		break;

	    // store s: variable in old script or autoload import
	    case ISN_STORES:
	    case ISN_STOREEXPORT:
		{
		    цяло		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = NULL;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    ако (iptr->isn_type == ISN_STOREEXPORT)
			di = find_var_autoload_prefix(name, sid, NULL, NULL);
		    // Then look for a variable in the script's variables.
		    ако (di == NULL)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = find_var_in_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_stack.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (di == NULL)
		    {
			ако (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(STACK_TV_BOT(0));
			    goto on_error;
			}
			store_var(name, STACK_TV_BOT(0));
		    }
		    иначе
		    {
			ако (iptr->isn_type == ISN_STOREEXPORT)
			{
			    цяло idx = get_script_item_idx(sid, name, 0,
								   NULL, NULL);

			    // can this ever fail?
			    ако (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;

				ако ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    clear_tv(STACK_TV_BOT(0));
				    goto on_error;
				}
			    }
			}
			ако (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(STACK_TV_BOT(0));
			    goto on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *STACK_TV_BOT(0);
		    }
		}
		break;

	    // store script-local variable in Vim9 script
	    case ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    ако (sv == NULL)
			goto theend;
		    --ectx->ec_stack.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (value_check_lock(sv->sv_tv->v_lock, sv->sv_name, FALSE))
		    {
			clear_tv(STACK_TV_BOT(0));
			goto on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *STACK_TV_BOT(0);
		}
		break;

	    // store option
	    case ISN_STOREOPT:
	    case ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    цяло		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    long	n = 0;
		    char_u	*s = NULL;
		    char	*msg;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = NULL;

		    --ectx->ec_stack.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = STACK_TV_BOT(0);
		    ако (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			ако (s == NULL)
			    s = (char_u *)"";
		    }
		    иначе ако (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			ако (s == NULL || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    goto on_error;
			}
		    }
		    иначе
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_number;
		    msg = set_option_value(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    ако (msg != NULL)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(msg));
			goto on_error;
		    }
		}
		break;

	    // store $ENV
	    case ISN_STOREENV:
		ако (check_secure() || check_restricted())
		    goto theend;
		--ectx->ec_stack.ga_len;
		tv = STACK_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		break;

	    // store @r
	    case ISN_STOREREG:
		{
		    цяло	reg = iptr->isn_arg.number;

		    --ectx->ec_stack.ga_len;
		    tv = STACK_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		break;

	    // store v: variable
	    case ISN_STOREV:
		--ectx->ec_stack.ga_len;
		ако (set_vim_var_tv(iptr->isn_arg.number, STACK_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    goto on_error;
		break;

	    // store g:/b:/w:/t: variable
	    case ISN_STOREG:
	    case ISN_STOREB:
	    case ISN_STOREW:
	    case ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    switch (iptr->isn_type)
		    {
			case ISN_STOREG:
			    ht = get_globvar_ht();
			    break;
			case ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    break;
			case ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    break;
			case ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    break;
			default:  // Cannot reach here
			    goto theend;
		    }

		    --ectx->ec_stack.ga_len;
		    di = find_var_in_ht(ht, 0, name, TRUE);
		    ако (di == NULL)
			store_var(iptr->isn_arg.string, STACK_TV_BOT(0));
		    иначе
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ако (var_check_permission(di, name) == FAIL)
			    goto on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *STACK_TV_BOT(0);
		    }
		}
		break;

	    // store an autoload variable
	    case ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		set_var(iptr->isn_arg.string, STACK_TV_BOT(-1), TRUE);
		clear_tv(STACK_TV_BOT(-1));
		--ectx->ec_stack.ga_len;
		break;

	    // store number in local variable
	    case ISN_STORENR:
		tv = STACK_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_NUMBER;
		tv->vval.v_number = iptr->isn_arg.storenr.stnr_val;
		break;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    case ISN_STOREINDEX:
		{
		    цяло res = execute_storeindex(iptr, ectx);

		    ако (res == FAIL)
			goto on_error;
		    ако (res == NOTDONE)
			goto theend;
		}
		break;

	    // store value in list or blob range
	    case ISN_STORERANGE:
		ако (execute_storerange(iptr, ectx) == FAIL)
		    goto on_error;
		break;

	    case ISN_LOAD_CLASSMEMBER:
		{
		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      STACK_TV_BOT(0));
		    ++ectx->ec_stack.ga_len;
		}
		break;

	    case ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *STACK_TV_BOT(-1);
		    --ectx->ec_stack.ga_len;
		}
		break;

	    // Load or store variable or argument from outer scope.
	    case ISN_LOADOUTER:
	    case ISN_STOREOUTER:
		{
		    цяло		depth = iptr->isn_arg.outer.outer_depth;
		    outer_T	*outer = ectx->ec_outer_ref == NULL ? NULL
						: ectx->ec_outer_ref->or_outer;

		    докато (depth > 1 && outer != NULL)
		    {
			outer = outer->out_up;
			--depth;
		    }
		    ако (outer == NULL)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			ако (ectx->ec_frame_idx == ectx->ec_initial_frame_idx
						 || ectx->ec_outer_ref == NULL)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_closure_called_from_invalid_context));
			иначе
			    iemsg("LOADOUTER depth more than scope levels");
			goto theend;
		    }
		    ако (depth < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->out_loop[-depth - 1]
							       .stack->ga_data)
					  + outer->out_loop[-depth - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    иначе
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->out_stack->ga_data)
				      + outer->out_frame_idx + STACK_FRAME_SIZE
				      + iptr->isn_arg.outer.outer_idx;
		    ако (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			    goto theend;
			// careful: ga_grow_inner may re-alloc the stack
			ако (depth < 0)
			    copy = ((typval_T *)outer->out_loop[-depth - 1]
								   .stack->ga_data)
					      + outer->out_loop[-depth - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			иначе
			    copy = ((typval_T *)outer->out_stack->ga_data)
					  + outer->out_frame_idx + STACK_FRAME_SIZE
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			ако (copy != tv)
			    tv = copy;
			copy_tv(tv, STACK_TV_BOT(0));
			++ectx->ec_stack.ga_len;
		    }
		    иначе
		    {
			--ectx->ec_stack.ga_len;
			clear_tv(tv);
			*tv = *STACK_TV_BOT(0);
		    }
		}
		break;

	    // unlet item in list or dict variable
	    case ISN_UNLETINDEX:
		ако (execute_unletindex(iptr, ectx) == FAIL)
		    goto on_error;
		break;

	    // unlet range of items in list variable
	    case ISN_UNLETRANGE:
		ако (execute_unletrange(iptr, ectx) == FAIL)
		    goto on_error;
		break;

	    // push constant
	    case ISN_PUSHNR:
	    case ISN_PUSHBOOL:
	    case ISN_PUSHSPEC:
	    case ISN_PUSHF:
	    case ISN_PUSHS:
	    case ISN_PUSHBLOB:
	    case ISN_PUSHFUNC:
	    case ISN_PUSHCHANNEL:
	    case ISN_PUSHJOB:
	    case ISN_PUSHOBJ:
	    case ISN_PUSHCLASS:
		ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    goto theend;
		tv = STACK_TV_BOT(0);
		tv->v_lock = 0;
		++ectx->ec_stack.ga_len;
		switch (iptr->isn_type)
		{
		    case ISN_PUSHNR:
			tv->v_type = VAR_NUMBER;
			tv->vval.v_number = iptr->isn_arg.number;
			break;
		    case ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_number = iptr->isn_arg.number;
			break;
		    case ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_number = iptr->isn_arg.number;
			break;
		    case ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			break;
		    case ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			break;
		    case ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			ако (iptr->isn_arg.string == NULL)
			    tv->vval.v_string = NULL;
			иначе
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			break;
		    case ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = NULL;
#endif
			break;
		    case ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_JOB;
			tv->vval.v_job = NULL;
#endif
			break;
		    case ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = NULL;
			break;
		    case ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			break;
		    default:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == NULL
				    ? NULL : vim_strsave(iptr->isn_arg.string);
		}
		break;

	    case ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (void)script_autoload(name, FALSE);
		    ако (find_func(name, TRUE))
		    {
			ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			    goto theend;
			tv = STACK_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_stack.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    иначе
		    {
			цяло res = load_namespace_var(ectx, ISN_LOADG, iptr);

			ако (res == NOTDONE)
			    goto theend;
			ако (res == FAIL)
			    goto on_error;
		    }
		}
		break;

	    case ISN_UNLET:
		ако (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    goto on_error;
		break;
	    case ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		break;

	    case ISN_LOCKUNLOCK:
		{
#ifdef LOG_LOCKVAR
		    ch_log(NULL, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#endif
		    lval_root_T	*lval_root_save = lval_root;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_stack.ga_len;
		    lval_root_T root;
		    root.lr_tv      = STACK_TV_BOT(0);
		    root.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    root.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_root = &root;
		    цяло res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(root.lr_tv);
		    lval_root = lval_root_save;
		    ако (res == FAIL)
			goto on_error;
		}
		break;

	    case ISN_LOCKCONST:
		item_lock(STACK_TV_BOT(-1), 100, TRUE, TRUE);
		break;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    case ISN_NEWLIST:
		ако (exe_newlist(iptr->isn_arg.number, ectx) == FAIL)
		    goto theend;
		break;

	    // create a tuple from items on the stack
	    case ISN_NEWTUPLE:
		ако (exe_newtuple(iptr->isn_arg.number, ectx) == FAIL)
		    goto theend;
		break;

	    // create a dict from items on the stack
	    case ISN_NEWDICT:
		{
		    цяло res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.number, ectx);
		    ако (res == FAIL)
			goto theend;
		    ако (res == MAYBE)
			goto on_error;
		}
		break;

	    case ISN_CONCAT:
		ако (exe_concat(iptr->isn_arg.number, ectx) == FAIL)
		    goto theend;
		break;

	    // create a partial with NULL value
	    case ISN_NEWPARTIAL:
		ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    goto theend;
		++ectx->ec_stack.ga_len;
		tv = STACK_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_lock = 0;
		tv->vval.v_partial = NULL;
		break;

	    // call a :def function
	    case ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ако (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				NULL,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    goto on_error;
		break;

	    // call a method on an interface
	    case ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = STACK_TV_BOT(-1 - mfunc->cmf_argcount);
		    ако (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			goto on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ако (obj == NULL)
		    {
			emsg(_(e_using_null_object));
			goto on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    ако (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    иначе
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			цяло idx = object_index_from_itf_index(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    ако (mfunc->cmf_ufunc != NULL && IS_GENERIC_FUNC(ufunc))
		    {
			ако (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			иначе
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    ако (ufunc == NULL)
				goto on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    ако (call_ufunc(ufunc, NULL, mfunc->cmf_argcount, ectx,
							   NULL, NULL) == FAIL)
			goto on_error;
		}
		break;

	    // call a builtin function
	    case ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		ако (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    goto on_error;
		break;

	    // call a funcref or partial
	    case ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    цяло		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (pfunc->cpf_top)
		    {
			// funcref is above the arguments
			tv = STACK_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    иначе
		    {
			// Get the funcref from the stack.
			--ectx->ec_stack.ga_len;
			partial_tv = *STACK_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    ако (tv == &partial_tv)
			clear_tv(&partial_tv);
		    ако (r == FAIL)
			goto on_error;
		}
		break;

	    case ISN_PCALL_END:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_stack.ga_len;
		clear_tv(STACK_TV_BOT(-1));
		*STACK_TV_BOT(-1) = *STACK_TV_BOT(0);
		break;

	    // call a user defined function or funcref/partial
	    case ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			goto on_error;
		}
		break;

	    // :defer func(arg)
	    case ISN_DEFER:
		ако (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    goto on_error;
		break;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    case ISN_RETURN_VOID:
	    case ISN_RETURN_OBJECT:
		ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    goto theend;
		tv = STACK_TV_BOT(0);
		++ectx->ec_stack.ga_len;
		ако (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_number = 0;
		    tv->v_lock = 0;
		}
		иначе
		{
		    *tv = *STACK_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_lock_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    case ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = NULL;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    ако (check_typval_is_value(STACK_TV_BOT(-1)) == FAIL)
			goto theend;

		    ако (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    ако (trycmd != NULL
				 && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    {
			// jump to ":finally" or ":endtry"
			ако (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			иначе
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    иначе
			goto func_return;
		}
		break;

	    // push a partial, a reference to a compiled function
	    case ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    ако (pt == NULL)
			goto theend;
		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    {
			vim_free(pt);
			goto theend;
		    }
		    ако (extra != NULL && extra->fre_class != NULL)
		    {
			class_T	*cl;
			ако (extra->fre_object_method)
			{
			    tv = STACK_TV_BOT(-1);
			    ако (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				goto on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    ако (obj == NULL)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				goto on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_stack.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			иначе
			    cl = extra->fre_class;

			ако (extra->fre_object_method)
			{
			    // object method
			    // convert the interface index to the object index
			    цяло idx =
				object_index_from_itf_index(extra->fre_class,
					TRUE, extra->fre_method_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			иначе
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_method_idx];
			}
		    }
		    иначе ако (extra == NULL || extra->fre_func_name == NULL)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    иначе
		    {
			ufunc = find_func(extra->fre_func_name, FALSE);
		    }
		    ако (ufunc == NULL)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			goto theend;
		    }
		    ако (extra != NULL && extra->fre_outer_depth > 0
					   && (ufunc->uf_flags & FC_CLOSURE))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == NULL
				    ? NULL : ectx->ec_outer_ref->or_partial;

			за (цяло depth = 1; outer_pt != NULL
				    && depth < extra->fre_outer_depth; ++depth)
			    outer_pt = get_pt_outer(outer_pt)->out_up_partial;
			ако (outer_pt == NULL)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    goto theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    иначе ако (fill_partial_and_closure(pt, ufunc,
			       extra == NULL ? NULL : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			goto theend;
		    }
		    tv = STACK_TV_BOT(0);
		    ++ectx->ec_stack.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_lock = 0;
		}
		break;

	    // Create a global function from a lambda.
	    case ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ако (copy_lambda_to_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			goto theend;
		}
		break;

	    // List functions
	    case ISN_DEF:
		ако (iptr->isn_arg.string == NULL)
		    list_functions(NULL);
		иначе
		{
		    exarg_T	ea;
		    garray_T	lines_to_free;

		    CLEAR_FIELD(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&lines_to_free, sizeof(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, NULL, &lines_to_free, 0, NULL, 0, NULL);
		    ga_clear_strings(&lines_to_free);
		}
		break;

	    // jump if a condition is met
	    case ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    цяло		error = FALSE;
		    цяло		jump = TRUE;

		    ако (when != JUMP_ALWAYS)
		    {
			tv = STACK_TV_BOT(-1);
			ако (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    ако (error)
				goto on_error;
			}
			иначе
			    jump = tv2bool(tv);
			ако (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			ако (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_stack.ga_len;
			}
		    }
		    ако (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		break;

	    // "while": jump to end if a condition is false
	    case ISN_WHILE:
		{
		    цяло		error = FALSE;
		    цяло		jump = TRUE;

		    tv = STACK_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    ако (error)
			goto on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_stack.ga_len;
		    ако (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_end;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = STACK_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_number = ectx->ec_funcrefs.ga_len;
		}
		break;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    case ISN_JUMP_IF_ARG_SET:
	    case ISN_JUMP_IF_ARG_NOT_SET:
		tv = STACK_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		цяло arg_set = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_number == VVAL_NONE);

		ако (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_SET && !arg_set)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_data)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    size_t argidx = ufunc->uf_def_args.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ STACK_FRAME_SIZE;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_POINTER(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    ако (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		ако (iptr->isn_type == ISN_JUMP_IF_ARG_SET ? arg_set : !arg_set)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		break;

	    // top of a for loop
	    case ISN_FOR:
		ако (execute_for(iptr, ectx) == FAIL)
		    goto theend;
		break;

	    // end of a for or while loop
	    case ISN_ENDLOOP:
		ако (execute_endloop(iptr, ectx) == FAIL)
		    goto theend;
		break;

	    // start of ":try" block
	    case ISN_TRY:
		{
		    trycmd_T    *trycmd = NULL;

		    ако (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			goto theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_data)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_POINTER(trycmd);
		    trycmd->tcd_frame_idx = ectx->ec_frame_idx;
		    trycmd->tcd_stack_len = ectx->ec_stack.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		break;

	    case ISN_PUSHEXC:
		ако (current_exception == NULL)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    goto theend;
		}
		ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
		    goto theend;
		tv = STACK_TV_BOT(0);
		++ectx->ec_stack.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->value);
		break;

	    case ISN_CATCH:
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
		break;

	    case ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    цяло		i;
		    trycmd_T    *trycmd;
		    цяло		iidx = trycont->tct_where;

		    ако (trystack->ga_len < trycont->tct_levels)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_levels, trystack->ga_len);
			goto theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    за (i = trycont->tct_levels; i > 0; --i)
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
		break;

	    case ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    ако (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_stack);
			trycmd->tcd_caught = FALSE;
		    }
		    break;
		}

	    // end of ":try" block
	    case ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_data) + trystack->ga_len;
		    ако (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    ако (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_stack);
		    }

		    ако (trycmd->tcd_return)
			goto func_return;

		    докато (ectx->ec_stack.ga_len > trycmd->tcd_stack_len)
		    {
			--ectx->ec_stack.ga_len;
			clear_tv(STACK_TV_BOT(0));
		    }
		    ако (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		break;

	    case ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    ако (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = STACK_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_NUMBER;
			tv->vval.v_number = 0;
			goto done;
		    }
		    --ectx->ec_stack.ga_len;
		    tv = STACK_TV_BOT(0);
		    ако (tv->vval.v_string == NULL
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_empty_string));
			goto theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    ако (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
			ако (trycmd->tcd_caught)
			{
			    // discard the exception
			    finish_exception(caught_stack);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (throw_exception(tv->vval.v_string, ET_USER, NULL)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			goto theend;
		    }
		    did_throw = TRUE;
		}
		break;

	    // compare with special values
	    case ISN_COMPAREBOOL:
	    case ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = STACK_TV_BOT(-2);
		    typval_T	*tv2 = STACK_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_number;
		    varnumber_T arg2 = tv2->vval.v_number;
		    цяло		res;

		    ако (iptr->isn_arg.op.op_type == EXPR_EQUAL)
			res = arg1 == arg2;
		    иначе
			res = arg1 != arg2;

		    --ectx->ec_stack.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_number = res ? VVAL_TRUE : VVAL_FALSE;
		}
		break;

	    case ISN_COMPARENULL:
		{
		    typval_T	*tv1 = STACK_TV_BOT(-2);
		    typval_T	*tv2 = STACK_TV_BOT(-1);
		    цяло		res;

		    res = typval_compare_null(tv1, tv2);
		    ако (res == MAYBE)
			goto on_error;
		    ако (iptr->isn_arg.op.op_type == EXPR_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_stack.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_number = res ? VVAL_TRUE : VVAL_FALSE;
		}
		break;

	    // Operation with two number arguments
	    case ISN_OPNR:
	    case ISN_COMPARENR:
		{
		    typval_T	*tv1 = STACK_TV_BOT(-2);
		    typval_T	*tv2 = STACK_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_number;
		    varnumber_T arg2 = tv2->vval.v_number;
		    varnumber_T res = 0;
		    цяло		div_zero = FALSE;

		    ако (iptr->isn_arg.op.op_type == EXPR_LSHIFT
			    || iptr->isn_arg.op.op_type == EXPR_RSHIFT)
		    {
			ако (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_positive));
			    goto on_error;
			}
		    }

		    switch (iptr->isn_arg.op.op_type)
		    {
			case EXPR_MULT: res = arg1 * arg2; break;
			case EXPR_DIV:  ако (arg2 == 0)
					    div_zero = TRUE;
					иначе
					    res = arg1 / arg2;
					break;
			case EXPR_REM:  ако (arg2 == 0)
					    div_zero = TRUE;
					иначе
					    res = arg1 % arg2;
					break;
			case EXPR_SUB: res = arg1 - arg2; break;
			case EXPR_ADD: res = arg1 + arg2; break;

			case EXPR_EQUAL: res = arg1 == arg2; break;
			case EXPR_NEQUAL: res = arg1 != arg2; break;
			case EXPR_GREATER: res = arg1 > arg2; break;
			case EXPR_GEQUAL: res = arg1 >= arg2; break;
			case EXPR_SMALLER: res = arg1 < arg2; break;
			case EXPR_SEQUAL: res = arg1 <= arg2; break;
			case EXPR_LSHIFT: ако (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  иначе
					      res = (uvarnumber_T)arg1 << arg2;
					  break;
			case EXPR_RSHIFT: ако (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  иначе
					      res = (uvarnumber_T)arg1 >> arg2;
					  break;
			default: break;
		    }

		    --ectx->ec_stack.ga_len;
		    ако (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_number = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    иначе
			tv1->vval.v_number = res;
		    ако (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			goto on_error;
		    }
		}
		break;

	    // Computation with two float arguments
	    case ISN_OPFLOAT:
	    case ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = STACK_TV_BOT(-2);
		    typval_T	*tv2 = STACK_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    цяло		cmp = FALSE;

		    switch (iptr->isn_arg.op.op_type)
		    {
			case EXPR_MULT: res = arg1 * arg2; break;
			case EXPR_DIV: res = arg1 / arg2; break;
			case EXPR_SUB: res = arg1 - arg2; break;
			case EXPR_ADD: res = arg1 + arg2; break;

			case EXPR_EQUAL: cmp = arg1 == arg2; break;
			case EXPR_NEQUAL: cmp = arg1 != arg2; break;
			case EXPR_GREATER: cmp = arg1 > arg2; break;
			case EXPR_GEQUAL: cmp = arg1 >= arg2; break;
			case EXPR_SMALLER: cmp = arg1 < arg2; break;
			case EXPR_SEQUAL: cmp = arg1 <= arg2; break;
			default: cmp = 0; break;
		    }
		    --ectx->ec_stack.ga_len;
		    ако (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_number = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    иначе
			tv1->vval.v_float = res;
		}
		break;

	    case ISN_COMPARELIST:
	    case ISN_COMPARETUPLE:
	    case ISN_COMPAREDICT:
	    case ISN_COMPAREFUNC:
	    case ISN_COMPARESTRING:
	    case ISN_COMPAREBLOB:
	    case ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = STACK_TV_BOT(-2);
		    typval_T	*tv2 = STACK_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    цяло		ic = iptr->isn_arg.op.op_ic;
		    цяло		res = FALSE;
		    цяло		status = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (iptr->isn_type == ISN_COMPARELIST)
		    {
			status = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    иначе ако (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			status = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    иначе ако (iptr->isn_type == ISN_COMPAREDICT)
		    {
			status = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    иначе ако (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			status = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    иначе ако (iptr->isn_type == ISN_COMPARESTRING)
		    {
			status = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    иначе ако (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			status = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    иначе // ISN_COMPAREOBJECT
		    {
			status = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_stack.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_number = res ? VVAL_TRUE : VVAL_FALSE;
		    ако (status == FAIL)
			goto theend;
		}
		break;

	    case ISN_COMPAREANY:
		{
		    typval_T	*tv1 = STACK_TV_BOT(-2);
		    typval_T	*tv2 = STACK_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    цяло		ic = iptr->isn_arg.op.op_ic;
		    цяло		status;

		    SOURCING_LNUM = iptr->isn_lnum;
		    status = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_stack.ga_len;
		    ако (status == FAIL)
			goto theend;
		}
		break;

	    case ISN_ADDLIST:
	    case ISN_ADDTUPLE:
	    case ISN_ADDBLOB:
		{
		    typval_T *tv1 = STACK_TV_BOT(-2);
		    typval_T *tv2 = STACK_TV_BOT(-1);

		    // add two lists or blobs
		    ако (iptr->isn_type == ISN_ADDLIST)
		    {
			ако (iptr->isn_arg.op.op_type == EXPR_APPEND
						   && tv1->vval.v_list != NULL)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 NULL);
			иначе
			    eval_addlist(tv1, tv2);
		    }
		    иначе ако (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    иначе
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_stack.ga_len;
		}
		break;

	    case ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = STACK_TV_BOT(-2);
		    typval_T	*tv2 = STACK_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (l == NULL)
		    {
			emsg(_(e_cannot_add_to_null_list));
			goto on_error;
		    }
		    ако (value_check_lock(l->lv_lock, NULL, FALSE))
			goto on_error;
		    ако (list_append_tv(l, tv2) == FAIL)
			goto theend;
		    clear_tv(tv2);
		    --ectx->ec_stack.ga_len;
		}
		break;

	    case ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = STACK_TV_BOT(-2);
		    typval_T	*tv2 = STACK_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    цяло		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    ако (b == NULL)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_add_to_null_blob));
			goto on_error;
		    }
		    n = tv_get_number_chk(tv2, &error);
		    ако (error)
			goto on_error;
		    ga_append(&b->bv_ga, (цяло)n);
		    --ectx->ec_stack.ga_len;
		}
		break;

	    // Computation with two arguments of unknown type
	    case ISN_OPANY:
		{
		    typval_T	*tv1 = STACK_TV_BOT(-2);
		    typval_T	*tv2 = STACK_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    цяло		error = FALSE;

		    ако (iptr->isn_arg.op.op_type == EXPR_ADD)
		    {
			ако (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_stack.ga_len;
			    break;
			}
			иначе ако (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_stack.ga_len;
			    break;
			}
			иначе ако (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_stack.ga_len;
			    break;
			}
		    }
		    ако (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    иначе
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_number_chk(tv1, &error);
			ако (error)
			    goto on_error;
			ако (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    ако (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    иначе
		    {
			n2 = tv_get_number_chk(tv2, &error);
			ако (error)
			    goto on_error;
			ако (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    ако (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			switch (iptr->isn_arg.op.op_type)
			{
			    case EXPR_MULT: f1 = f1 * f2; break;
			    case EXPR_DIV:  f1 = f1 / f2; break;
			    case EXPR_SUB:  f1 = f1 - f2; break;
			    case EXPR_ADD:  f1 = f1 + f2; break;
			    default: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     goto on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_stack.ga_len;
		    }
		    иначе
		    {
			цяло failed = FALSE;

			switch (iptr->isn_arg.op.op_type)
			{
			    case EXPR_MULT: n1 = n1 * n2; break;
			    case EXPR_DIV:  n1 = num_divide(n1, n2, &failed);
					    ако (failed)
						goto on_error;
					    break;
			    case EXPR_SUB:  n1 = n1 - n2; break;
			    case EXPR_ADD:  n1 = n1 + n2; break;
			    default:	    n1 = num_modulus(n1, n2, &failed);
					    ако (failed)
						goto on_error;
					    break;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_NUMBER;
			tv1->vval.v_number = n1;
			--ectx->ec_stack.ga_len;
		    }
		}
		break;

	    case ISN_STRINDEX:
	    case ISN_STRSLICE:
		{
		    цяло		is_slice = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    ако (is_slice)
		    {
			tv = STACK_TV_BOT(-2);
			n1 = tv->vval.v_number;
		    }

		    tv = STACK_TV_BOT(-1);
		    n2 = tv->vval.v_number;

		    ectx->ec_stack.ga_len -= is_slice ? 2 : 1;
		    tv = STACK_TV_BOT(-1);
		    ако (is_slice)
			// Slice: Select the characters from the string
			res = string_slice(tv->vval.v_string, n1, n2, FALSE);
		    иначе
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		break;

	    case ISN_LISTINDEX:
	    case ISN_LISTSLICE:
	    case ISN_TUPLEINDEX:
	    case ISN_TUPLESLICE:
	    case ISN_BLOBINDEX:
	    case ISN_BLOBSLICE:
		{
		    цяло		is_slice = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    цяло		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    цяло		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_slice ? STACK_TV_BOT(-3) : STACK_TV_BOT(-2);

		    tv = STACK_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_number;
		    clear_tv(tv);

		    ако (is_slice)
		    {
			tv = STACK_TV_BOT(-2);
			n1 = tv->vval.v_number;
			clear_tv(tv);
		    }

		    ectx->ec_stack.ga_len -= is_slice ? 2 : 1;
		    tv = STACK_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (is_blob)
		    {
			ако (blob_slice_or_index(val_tv->vval.v_blob, is_slice,
						    n1, n2, FALSE, tv) == FAIL)
			    goto on_error;
		    }
		    иначе ако (is_tuple)
		    {
			ако (tuple_slice_or_index(val_tv->vval.v_tuple,
				is_slice, n1, n2, FALSE, tv, TRUE) == FAIL)
			    goto on_error;
		    }
		    иначе
		    {
			ако (list_slice_or_index(val_tv->vval.v_list, is_slice,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    goto on_error;
		    }
		}
		break;

	    case ISN_ANYINDEX:
	    case ISN_ANYSLICE:
		{
		    цяло		is_slice = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    цяло		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_slice ? STACK_TV_BOT(-3) : STACK_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (check_can_index(tv, TRUE, TRUE) == FAIL)
			goto on_error;
		    var1 = is_slice ? STACK_TV_BOT(-2) : STACK_TV_BOT(-1);
		    var2 = is_slice ? STACK_TV_BOT(-1) : NULL;
		    res = eval_index_inner(tv, is_slice, var1, var2,
							FALSE, NULL, -1, TRUE);
		    clear_tv(var1);
		    ако (is_slice)
			clear_tv(var2);
		    ectx->ec_stack.ga_len -= is_slice ? 2 : 1;
		    ако (res == FAIL)
			goto on_error;
		}
		break;

	    case ISN_SLICE:
		{
		    цяло		count = iptr->isn_arg.number;

		    // type will have been checked to be a list
		    tv = STACK_TV_BOT(-1);
		    ако (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			ако (list != NULL && list->lv_len >= count)
			{
			    list_T	*newlist = list_slice(list,
				    count, list->lv_len - 1);

			    ако (newlist != NULL)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    иначе
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			ако (tuple != NULL && TUPLE_LEN(tuple) >= count)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_slice(tuple, count,
							TUPLE_LEN(tuple) - 1);
			    ако (newtuple != NULL)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		break;

	    case ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*item_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = STACK_TV_BOT(-1 - gi->gi_with_op);
		    ако (tv->v_type == VAR_LIST)
		    {
			li = list_find(tv->vval.v_list, gi->gi_index);
			ако (li == NULL)
			    goto on_error;
			item_tv = &li->li_tv;
		    }
		    иначе
			item_tv = TUPLE_ITEM(tv->vval.v_tuple, gi->gi_index);

		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    ++ectx->ec_stack.ga_len;
		    copy_tv(item_tv, STACK_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_index = gi->gi_index + 1;
		    ectx->ec_where.wt_kind = WT_VARIABLE;
		}
		break;

	    // dict member with string key (dict['member'])
	    case ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*key;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = STACK_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = STACK_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    key = tv->vval.v_string;
		    ако (key == NULL)
			key = (char_u *)"";

		    ако ((di = dict_find(dict, key, -1)) == NULL)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_key_not_present_in_dictionary_str), key);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_stack.ga_len;
			tv = STACK_TV_BOT(-1);
			(void) dict_stack_save(tv);
			tv->v_type = VAR_NUMBER;
			tv->vval.v_number = 0;
			goto on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_stack.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = STACK_TV_BOT(-1);
		    ако (dict_stack_save(tv) == FAIL)
			goto on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		break;

	    // dict member with string key (dict.member)
	    // or can be an object
	    case ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = STACK_TV_BOT(-1);

		    ако (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			ако (dict_stack_save(tv) == FAIL)
			    goto on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			ако (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    goto on_error;
		    }
		    иначе
		    {
			ако (tv->v_type != VAR_DICT || tv->vval.v_dict == NULL)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    goto on_error;
			}
			dict = tv->vval.v_dict;

			ако ((di = dict_find(dict, iptr->isn_arg.string, -1))
								   == NULL)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_key_not_present_in_dictionary_str),
						     iptr->isn_arg.string);
			    goto on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			ако (dict_stack_save(tv) == FAIL)
			    goto on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		break;

	    case ISN_GET_OBJ_MEMBER:
	    case ISN_GET_ITF_MEMBER:
		{
		    tv = STACK_TV_BOT(-1);
		    ако (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			goto on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    ако (obj == NULL)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			goto on_error;
		    }

		    цяло idx;
		    ако (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    иначе
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_index_from_itf_index(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    ако (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			goto on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		break;

	    case ISN_STORE_THIS:
		{
		    цяло idx = iptr->isn_arg.number;
		    object_T *obj = STACK_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *STACK_TV_BOT(-1);
		    --ectx->ec_stack.ga_len;
		}
		break;

	    case ISN_CLEARDICT:
		dict_stack_drop();
		break;

	    case ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_stack_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = STACK_TV_BOT(-1);
		    ако (dict_tv != NULL
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != NULL
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == NULL))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_stack_drop();
		}
		break;

	    case ISN_NEGATENR:
		tv = STACK_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		ако (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		иначе
		    tv->vval.v_number = -tv->vval.v_number;
		break;

	    case ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    цяло		r;
		    where_T	where = WHERE_INIT;

		    tv = STACK_TV_BOT((цяло)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    ако (ct->ct_arg_idx > 0)
		    {
			where.wt_index = ct->ct_arg_idx;
			where.wt_kind = ct->ct_is_var ? WT_VARIABLE : WT_ARGUMENT;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    ако (r == FAIL)
			goto on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    ако (tv->v_type == VAR_NUMBER
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_number == 0
						|| tv->vval.v_number == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_number = tv->vval.v_number
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		break;

	    case ISN_CHECKLEN:
		{
		    цяло	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = NULL;
		    tuple_T *tuple = NULL;
		    цяло	    len = 0;

		    tv = STACK_TV_BOT(-1);

		    цяло		len_check_failed = FALSE;
		    ако (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			ако (list == NULL || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			ако (list != NULL)
			    len = list->lv_len;
		    }
		    иначе ако (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			ако (tuple == NULL || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			ако (tuple != NULL)
			    len = TUPLE_LEN(tuple);
		    }
		    иначе
			len_check_failed = TRUE;

		    ако (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_items_but_got_nr),
				     min_len, len);
			goto on_error;
		    }
		}
		break;

	    case ISN_SETTYPE:
		set_tv_type(STACK_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		break;

	    case ISN_2BOOL:
	    case ISN_COND2BOOL:
		{
		    цяло n;
		    цяло error = FALSE;

		    ако (iptr->isn_type == ISN_2BOOL)
		    {
			tv = STACK_TV_BOT(iptr->isn_arg.tobool.offset);
			n = tv2bool(tv);
			ако (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    иначе
		    {
			tv = STACK_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			ако (error)
			    goto on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_number = n ? VVAL_TRUE : VVAL_FALSE;
		}
		break;

	    case ISN_2STRING:
	    case ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		ако (do_2string(STACK_TV_BOT(iptr->isn_arg.tostring.offset),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    goto on_error;
		break;

	    case ISN_RANGE:
		{
		    exarg_T	ea;
		    char	*errormsg;

		    ea.line2 = 0;
		    ea.addr_count = 0;
		    ea.addr_type = ADDR_LINES;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    ако (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
			goto on_error;

		    ако (GA_GROW_FAILS(&ectx->ec_stack, 1))
			goto theend;
		    ++ectx->ec_stack.ga_len;
		    tv = STACK_TV_BOT(-1);
		    tv->v_type = VAR_NUMBER;
		    tv->v_lock = 0;
		    tv->vval.v_number = ea.line2;
		}
		break;

	    case ISN_PUT:
		tv = NULL; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		break;
	    case ISN_IPUT:
		tv = NULL;
		isn_put_do(ectx, iptr, tv, TRUE);
		break;

	    case ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_stack.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		break;

	    case ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = NULL;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		break;

	    case ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = STACK_TV_BOT(-1);
		ако (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_argument_must_be_sequence_of_lists_or_tuples));
		    goto on_error;
		}

		цяло rc;
		ако (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		иначе
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		ако (rc != EXEC_OK)
		{
		    ако (rc == EXEC_FAIL)
			goto on_error;
		    goto theend;
		}
		break;

	    case ISN_PROF_START:
	    case ISN_PROF_END:
		{
#ifdef FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    ако (iptr->isn_type == ISN_PROF_START)
		    {
			func_line_start(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_line_exec(&cookie);
		    }
		    иначе
			func_line_end(&cookie);
#endif
		}
		break;

	    case ISN_DEBUG:
		handle_debug(iptr, ectx);
		break;

	    case ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    цяло		item = iptr->isn_arg.shuffle.shfl_item;
		    цяло		up = iptr->isn_arg.shuffle.shfl_up;

		    tmp_tv = *STACK_TV_BOT(-item);
		    за ( ; up > 0 && item > 1; --up)
		    {
			*STACK_TV_BOT(-item) = *STACK_TV_BOT(-item + 1);
			--item;
		    }
		    *STACK_TV_BOT(-item) = tmp_tv;
		}
		break;

	    case ISN_DROP:
		--ectx->ec_stack.ga_len;
		clear_tv(STACK_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		break;

	    case ISN_SCRIPTCTX_SET:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		break;
	}
	continue;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	ако (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
	    goto done;

	ако (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    goto theend;
	continue;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	ако (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    ако (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		докато (ectx->ec_stack.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_stack.ga_len;
		    clear_tv(STACK_TV_BOT(0));
		}
		докато (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    continue;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	ако (trylevel <= ectx->ec_trylevel_at_start)
	    goto theend;
    }

done:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_stack_clear(dict_stack_len_at_start);
    ectx->ec_trylevel_at_start = save_trylevel_at_start;
    върни ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    цяло
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    цяло		save_iidx = ectx->ec_iidx;
    цяло		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    ако (res == OK)
    {
	*rettv = *STACK_TV_BOT(-1);
	--ectx->ec_stack.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    върни res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(void)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    цяло		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    ако (exec_instructions(ectx) == OK)
    {
	typval_T *tv = STACK_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_stack.ga_len;
	clear_tv(tv);
    }
    иначе
    {
	substitute_instr->subs_status = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    върни res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    цяло
call_def_function(
    ufunc_T	*ufunc,
    цяло		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    цяло		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    цяло		argc = argc_arg;
    цяло		partial_argc = partial == NULL
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    цяло		total_argc = argc + partial_argc;
    typval_T	*tv;
    цяло		idx;
    цяло		ret = FAIL;
    цяло		defcount = ufunc->uf_args.ga_len - total_argc;
    sctx_T	save_current_sctx = current_sctx;
    цяло		did_emsg_before = did_emsg_cumul + did_emsg;
    цяло		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_msg_list = NULL;
    msglist_T	*private_msg_list = NULL;
    цяло		save_emsg_silent_def = emsg_silent_def;
    цяло		save_did_emsg_def = did_emsg_def;
    цяло		orig_funcdepth;
    цяло		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#undef STACK_TV
#define STACK_TV(idx) (((typval_T *)ectx.ec_stack.ga_data) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#undef STACK_TV_BOT
#define STACK_TV_BOT(idx) (((typval_T *)ectx.ec_stack.ga_data) + ectx.ec_stack.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#undef STACK_TV_VAR
#define STACK_TV_VAR(idx) (((typval_T *)ectx.ec_stack.ga_data) + ectx.ec_frame_idx + STACK_FRAME_SIZE + idx)

    ако (ufunc->uf_def_status == UF_NOT_COMPILED
	    || ufunc->uf_def_status == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), NULL) == FAIL))
    {
	ако (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	върни FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ако (dfunc->df_ufunc == NULL)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    върни FAIL;
	}
	ако (INSTRUCTIONS(dfunc) == NULL)
	{
	    iemsg("using call_def_function() on not compiled function");
	    върни FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    ако (funcdepth_increment() == FAIL)
	върни FAIL;

    CLEAR_FIELD(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_stack, sizeof(typval_T), 500);
    ако (GA_GROW_FAILS(&ectx.ec_stack, 20))
    {
	funcdepth_decrement();
	върни FAIL;
    }
    ga_init2(&ectx.ec_trystack, sizeof(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, sizeof(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = total_argc - ufunc->uf_args.ga_len;
    ако (idx > 0 && ufunc->uf_va_name == NULL)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
								    idx), idx);
	goto failed_early;
    }
    idx = total_argc - ufunc->uf_args.ga_len + ufunc->uf_def_args.ga_len;
    ако (idx < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	goto failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    за (idx = 0; idx < total_argc
	    && (ufunc->uf_va_name != NULL || idx < ufunc->uf_args.ga_len);
									 ++idx)
    {
	цяло argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	ако (idx >= ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_number == VVAL_NONE)
	{
	    // Use the default value.
	    STACK_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	иначе
	{
	    цяло done = FALSE;
	    ако (ufunc->uf_arg_types != NULL && idx < ufunc->uf_args.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		ако (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_NUMBER)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    STACK_TV_BOT(0)->v_type = VAR_FLOAT;
		    STACK_TV_BOT(0)->v_lock = 0;
		    STACK_TV_BOT(0)->vval.v_float = tv->vval.v_number;
		    done = TRUE;
		}
		иначе ако (check_typval_arg_type(expected, tv,
						   NULL, argv_idx + 1) == FAIL)
		    goto failed_early;
	    }
	    ако (!done)
		copy_tv(tv, STACK_TV_BOT(0));
	}
	++ectx.ec_stack.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    ако (ufunc->uf_va_name != NULL)
    {
	цяло vararg_count = argc - ufunc->uf_args.ga_len;

	ако (vararg_count < 0)
	    vararg_count = 0;
	иначе
	    argc -= vararg_count;
	ако (exe_newlist(vararg_count, &ectx) == FAIL)
	    goto failed_early;

	// Check the type of the list items.
	tv = STACK_TV_BOT(-1);
	ако (ufunc->uf_va_type != NULL
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != NULL)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_first;

	    за (idx = 0; idx < vararg_count; ++idx)
	    {
		ако (check_typval_arg_type(expected, &li->li_tv,
						 NULL, argc + idx + 1) == FAIL)
		    goto failed_early;
		li = li->li_next;
	    }
	}

	ако (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *STACK_TV_BOT(defcount - 1) = *STACK_TV_BOT(-1);
	--ectx.ec_stack.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    ако (defcount > 0)
	за (idx = 0; idx < defcount; ++idx)
	{
	    STACK_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_stack.ga_len;
	}
    ако (ufunc->uf_va_name != NULL)
	++ectx.ec_stack.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_frame_idx = ectx.ec_stack.ga_len;
    ectx.ec_initial_frame_idx = ectx.ec_frame_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *base_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	ако (partial != NULL || base_ufunc->uf_partial != NULL)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    ако (ectx.ec_outer_ref == NULL)
		goto failed_early;
	    ако (partial != NULL)
	    {
		outer_T *outer = get_pt_outer(partial);

		ако (outer->out_stack == NULL && outer->out_loop_size == 0)
		{
		    // no stack was set
		    ако (current_ectx != NULL)
		    {
			ако (current_ectx->ec_outer_ref != NULL
			       && current_ectx->ec_outer_ref->or_outer != NULL)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		иначе
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    иначе
	    {
		ectx.ec_outer_ref->or_outer = &base_ufunc->uf_partial->pt_outer;
		++base_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = base_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    за (idx = 0; idx < STACK_FRAME_SIZE; ++idx)
    {
	STACK_TV(ectx.ec_stack.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_stack.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	за (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    STACK_TV_VAR(idx)->v_type = VAR_NUMBER;
	    STACK_TV_VAR(idx)->vval.v_number = 0;
	}
	ectx.ec_stack.ga_len += dfunc->df_varcount;

	ако (object != NULL)
	{
	    // the object is always the variable at index zero
	    tv = STACK_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	ако (dfunc->df_has_closure)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    STACK_TV_VAR(idx)->v_type = VAR_NUMBER;
	    STACK_TV_VAR(idx)->vval.v_number = 0;
	    ++ectx.ec_stack.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    ако (funccal != NULL)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_push_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_msg_list = msg_list;
    msg_list = &private_msg_list;

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
    ако (ret == OK)
    {
	// function finished, get result from the stack.
	ако (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	иначе
	{
	    tv = STACK_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    ако (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_closure_in_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_pop();
    current_sctx = save_current_sctx;

    ако (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    ако (*msg_list != NULL && saved_msg_list != NULL)
    {
	msglist_T **plist = saved_msg_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	докато (*plist != NULL)
	    plist = &(*plist)->next;

	*plist = *msg_list;
    }
    msg_list = saved_msg_list;

    ако (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = NULL;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    за (idx = 0; idx < ectx.ec_stack.ga_len; ++idx)
    {
	tv = STACK_TV(idx);
	ако (tv->v_type != VAR_NUMBER && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_stack.ga_data);
    vim_free(ectx.ec_trystack.ga_data);
    ако (ectx.ec_outer_ref != NULL)
    {
	ако (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    ако (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    върни ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    void
unwind_def_callstack(ectx_T *ectx)
{
    докато (ectx->ec_frame_idx != ectx->ec_initial_frame_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    void
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    ако (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    static char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    цяло		depth;

    ga_init2(&ga, 1, 100);
    за (depth = 0; depth < lvi->lvi_depth; ++depth)
    {
	ако (ga_grow(&ga, 50) == FAIL)
	    break;
	ако (lvi->lvi_loop[depth].var_idx == 0)
	    STRCPY((char *)ga.ga_data + ga.ga_len, " -");
	иначе
	    vim_snprintf((char *)ga.ga_data + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[depth].var_idx,
			    lvi->lvi_loop[depth].var_idx
					 + lvi->lvi_loop[depth].var_count - 1);
	ga.ga_len = (цяло)STRLEN(ga.ga_data);
    }
    върни ga.ga_data;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    static void
list_instructions(char *pfx, isn_T *instr, цяло instr_count, ufunc_T *ufunc)
{
    цяло		line_idx = 0;
    цяло		prev_current = 0;
    цяло		current;
    цяло		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    за (current = 0; current < instr_count; ++current)
    {
	isn_T	    *iptr = &instr[current];
	char	    *line;

	ако (ufunc != NULL)
	{
	    докато (line_idx < iptr->isn_lnum
					  && line_idx < ufunc->uf_lines.ga_len)
	    {
		ако (current > prev_current)
		{
		    msg_puts("\n\n");
		    prev_current = current;
		}
		line = ((char **)ufunc->uf_lines.ga_data)[line_idx++];
		ако (line != NULL)
		    msg(line);
	    }
	    ако (iptr->isn_type == ISN_JUMP_IF_ARG_SET)
	    {
		цяло	first_def_arg = ufunc->uf_args.ga_len
						   - ufunc->uf_def_args.ga_len;

		ако (def_arg_idx > 0)
		    msg_puts("\n\n");
		msg_start();
		msg_puts("  ");
		msg_puts(((char **)(ufunc->uf_args.ga_data))[
						 first_def_arg + def_arg_idx]);
		msg_puts(" = ");
		msg_puts(((char **)(ufunc->uf_def_args.ga_data))[def_arg_idx++]);
		msg_clr_eos();
		msg_end();
	    }
	}

	switch (iptr->isn_type)
	{
	    case ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (цяло)iptr->isn_arg.construct.construct_size);
		break;
	    case ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_EXEC_SPLIT:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		break;
	    case ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		break;
	    case ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.number ? " append" : "");
		break;
	    case ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.number));
#endif
		break;
	    case ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#endif
		break;
	    case ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, INT_MAX, NULL);
		msg("     -------------");
		break;
	    case ISN_SOURCE:
		{
		    scriptitem_T    *si = SCRIPT_ITEM(iptr->isn_arg.number);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		break;
	    case ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, INT_MAX, NULL);
		    msg("     -------------");
		}
		break;
	    case ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.number);
		break;
	    case ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_count);
		}
		break;
	    case ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.number));
		break;
	    case ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.number));
		break;
	    case ISN_ECHOWINDOW:
		ако (iptr->isn_arg.echowin.ewin_time > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_count,
				      iptr->isn_arg.echowin.ewin_time);
		иначе
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_count);
		break;
	    case ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.number));
		break;
	    case ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.number));
		break;
	    case ISN_LOAD:
		{
		    ако (iptr->isn_arg.number < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.number
							  + STACK_FRAME_SIZE));
		    иначе
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.number));
		}
		break;
	    case ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ако (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_depth,
					outer->outer_idx + STACK_FRAME_SIZE);
		    иначе ако (outer->outer_depth < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_depth);
		    иначе
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_depth,
					      outer->outer_idx);
		}
		break;
	    case ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.number));
		break;
	    case ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ако (sv == NULL)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    иначе
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		break;
	    case ISN_LOADS:
	    case ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		break;
	    case ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		break;
	    case ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		break;
	    case ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		break;
	    case ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		break;
	    case ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (цяло)(iptr->isn_arg.number));
		break;

	    case ISN_STORE:
		ако (iptr->isn_arg.number < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.number + STACK_FRAME_SIZE);
		иначе
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.number);
		break;
	    case ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    ако (outer->outer_depth < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_depth);
		    иначе
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_depth, outer->outer_idx);
		}
		break;
	    case ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.number));
		break;
	    case ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_STORES:
	    case ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		break;
	    case ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    ако (sv == NULL)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    иначе
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		break;
	    case ISN_STOREOPT:
	    case ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		break;
	    case ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (цяло)iptr->isn_arg.number);
		break;
	    case ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		break;

	    case ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		break;

	    case ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		break;

	    case ISN_LOAD_CLASSMEMBER:
	    case ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    цяло	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_LOAD_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		break;

	    // constants
	    case ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.number));
		break;
	    case ISN_PUSHBOOL:
	    case ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.number));
		break;
	    case ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		break;
	    case ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		break;
	    case ISN_PUSHFUNC:
		{
		    char *name = (char *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == NULL ? "[none]" : name);
		}
		break;
	    case ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#endif
		break;
	    case ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#endif
		break;
	    case ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		break;
	    case ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == NULL ? "null"
				 : (char *)iptr->isn_arg.classarg->class_name.string);
		break;
	    case ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		break;
	    case ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		break;
	    case ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		break;
	    case ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		break;
	    case ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		break;
	    case ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		break;
	    case ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		break;
	    case ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.number));
		break;
	    case ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.number));
		break;
	    case ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.number));
		break;
	    case ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		break;

	    // function call
	    case ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    internal_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		break;
	    case ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		break;
	    case ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != NULL
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		break;
	    case ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		break;
	    case ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_top ? " top" : "", cpfunc->cpf_argcount);
		}
		break;
	    case ISN_PCALL_END:
		smsg("%s%4d PCALL end", pfx, current);
		break;
	    case ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (цяло)iptr->isn_arg.defer.defer_argcount);
		break;
	    case ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		break;
	    case ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		break;
	    case ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		break;
	    case ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    ако (extra == NULL || extra->fre_func_name == NULL)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    иначе
			name = extra->fre_func_name;
		    ако (extra != NULL && extra->fre_class != NULL)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    иначе ако (extra == NULL
				     || extra->fre_loopvar_info.lvi_depth == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    иначе
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		break;

	    case ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    ако (arg->nfa_loopvar_info.lvi_depth == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    иначе
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		break;

	    case ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == NULL ? (char_u *)"" : name);
		}
		break;

	    case ISN_JUMP:
		{
		    char *when = "?";

		    switch (iptr->isn_arg.jump.jump_when)
		    {
			case JUMP_ALWAYS:
			    when = "JUMP";
			    break;
			case JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    break;
			case JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    break;
			case JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    break;
			case JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  // unused
			    break;
			case JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    break;
			case JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    break;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		break;

	    case ISN_JUMP_IF_ARG_SET:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + STACK_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		break;

	    case ISN_JUMP_IF_ARG_NOT_SET:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + STACK_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		break;

	    case ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_end);
		}
		break;

	    case ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->end_funcref_idx,
			    endloop->end_var_idx,
			    endloop->end_var_idx + endloop->end_var_count - 1,
			    endloop->end_depth);
		}
		break;

	    case ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_end);
		}
		break;

	    case ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    ако (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    иначе
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		break;
	    case ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		break;
	    case ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_levels,
				      trycont->tct_levels == 1 ? "" : "s",
				      trycont->tct_where);
		}
		break;
	    case ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		break;
	    case ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		break;
	    case ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		break;

	    // expression operations on number
	    case ISN_OPNR:
	    case ISN_OPFLOAT:
	    case ISN_OPANY:
		{
		    char *what;
		    char *ins;

		    switch (iptr->isn_arg.op.op_type)
		    {
			case EXPR_MULT: what = "*"; break;
			case EXPR_DIV: what = "/"; break;
			case EXPR_REM: what = "%"; break;
			case EXPR_SUB: what = "-"; break;
			case EXPR_ADD: what = "+"; break;
			case EXPR_LSHIFT: what = "<<"; break;
			case EXPR_RSHIFT: what = ">>"; break;
			default:       what = "???"; break;
		    }
		    switch (iptr->isn_type)
		    {
			case ISN_OPNR: ins = "OPNR"; break;
			case ISN_OPFLOAT: ins = "OPFLOAT"; break;
			case ISN_OPANY: ins = "OPANY"; break;
			default: ins = "???"; break;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		break;

	    case ISN_COMPAREBOOL:
	    case ISN_COMPARESPECIAL:
	    case ISN_COMPARENULL:
	    case ISN_COMPARENR:
	    case ISN_COMPAREFLOAT:
	    case ISN_COMPARESTRING:
	    case ISN_COMPAREBLOB:
	    case ISN_COMPARELIST:
	    case ISN_COMPARETUPLE:
	    case ISN_COMPAREDICT:
	    case ISN_COMPAREFUNC:
	    case ISN_COMPAREOBJECT:
	    case ISN_COMPAREANY:
		   {
		       char *p;
		       char buf[10];
		       char *type;

		       switch (iptr->isn_arg.op.op_type)
		       {
			   case EXPR_EQUAL:	 p = "=="; break;
			   case EXPR_NEQUAL:    p = "!="; break;
			   case EXPR_GREATER:   p = ">"; break;
			   case EXPR_GEQUAL:    p = ">="; break;
			   case EXPR_SMALLER:   p = "<"; break;
			   case EXPR_SEQUAL:    p = "<="; break;
			   case EXPR_MATCH:	 p = "=~"; break;
			   case EXPR_IS:	 p = "is"; break;
			   case EXPR_ISNOT:	 p = "isnot"; break;
			   case EXPR_NOMATCH:	 p = "!~"; break;
			   default:  p = "???"; break;
		       }
		       STRCPY(buf, p);
		       ако (iptr->isn_arg.op.op_ic == TRUE)
			   strcat(buf, "?");
		       switch(iptr->isn_type)
		       {
			   case ISN_COMPAREBOOL: type = "COMPAREBOOL"; break;
			   case ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; break;
			   case ISN_COMPARENULL: type = "COMPARENULL"; break;
			   case ISN_COMPARENR: type = "COMPARENR"; break;
			   case ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; break;
			   case ISN_COMPARESTRING:
						  type = "COMPARESTRING"; break;
			   case ISN_COMPAREBLOB: type = "COMPAREBLOB"; break;
			   case ISN_COMPARELIST: type = "COMPARELIST"; break;
			   case ISN_COMPARETUPLE: type = "COMPARETUPLE"; break;
			   case ISN_COMPAREDICT: type = "COMPAREDICT"; break;
			   case ISN_COMPAREFUNC: type = "COMPAREFUNC"; break;
			   case ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; break;
			   case ISN_COMPAREANY: type = "COMPAREANY"; break;
			   default: type = "???"; break;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   break;

	    case ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); break;
	    case ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); break;
	    case ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); break;

	    // expression operations
	    case ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.number));
		break;
	    case ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); break;
	    case ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); break;
	    case ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); break;
	    case ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); break;
	    case ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); break;
	    case ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); break;
	    case ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); break;
	    case ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); break;
	    case ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); break;
	    case ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); break;
	    case ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); break;
	    case ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); break;
	    case ISN_SLICE: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.number); break;
	    case ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_index,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); break;
	    case ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); break;
	    case ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); break;
	    case ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (цяло)iptr->isn_arg.classmember.cm_idx);
				     break;
	    case ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (цяло)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     break;
	    case ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (цяло)iptr->isn_arg.number); break;
	    case ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); break;
	    case ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); break;

	    case ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); break;

	    case ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      char	    *tofree = NULL;
		      char	    *typename;

		      ако (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_NUMBER_OK))
			  typename = "float|number";
		      иначе ако (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      иначе
			  typename = type_name(ct->ct_type, &tofree);

		      ако (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (цяло)ct->ct_off);
		      иначе
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (цяло)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (цяло)ct->ct_arg_idx);
		      vim_free(tofree);
		      break;
		  }
	    case ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_more_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       break;
	    case ISN_SETTYPE:
		  {
		      char *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      break;
		  }
	    case ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); break;
	    case ISN_2BOOL: ако (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    иначе
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    break;
	    case ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      break;
	    case ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      break;
	    case ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    break;
	    case ISN_PUT:
		ако (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		иначе ако (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		иначе
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		break;
	    case ISN_IPUT:
		ако (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		иначе ако (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		иначе
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (long)iptr->isn_arg.put.put_lnum);
		break;
	    case ISN_CMDMOD:
		{
		    char_u  *buf;
		    size_t  len = produce_cmdmods(
				  NULL, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    ако (likely(buf != NULL))
		    {
			(void)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    break;
		}
	    case ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); break;

	    case ISN_PROF_START:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 break;

	    case ISN_PROF_END:
		smsg("%s%4d PROFILE END", pfx, current);
		break;

	    case ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_names_len);
		break;

	    case ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_count,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      break;
	    case ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_item,
					 iptr->isn_arg.shuffle.shfl_up);
			      break;
	    case ISN_DROP: smsg("%s%4d DROP", pfx, current); break;

	    case ISN_SCRIPTCTX_SET:
		{
		    цяло	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ITEM(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		break;

	    case ISN_FINISH: // End of list of instructions for ISN_SUBSTITUTE.
			   върни;
	}

	out_flush();	    // output one line at a time
	ui_breakcheck();
	ако (got_int)
	    break;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    void
set_context_in_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_pattern = arg;

    // first argument already typed: only user function names
    ако (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
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
get_disassemble_argument(expand_T *xp, цяло idx)
{
    ако (idx == 0)
	върни (char_u *)"debug";
    ако (idx == 1)
	върни (char_u *)"profile";
    върни get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    void
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = NULL;  // init to shut up gcc warning
    цяло		instr_count = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = find_func_by_name(arg, &compile_type);
    ако (ufunc == NULL)
	върни;
    ако (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, NULL) == FAIL)
	върни;
    ако (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	върни;
    }
    msg((char *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    switch (compile_type)
    {
	case CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_count = dfunc->df_instr_prof_count;
	    break;
#endif
	    // FALLTHROUGH
	case CT_NONE:
	    instr = dfunc->df_instr;
	    instr_count = dfunc->df_instr_count;
	    break;
	case CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_count = dfunc->df_instr_debug_count;
	    break;
    }

    list_instructions("", instr, instr_count, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    цяло
tv2bool(typval_T *tv)
{
    switch (tv->v_type)
    {
	case VAR_NUMBER:
	    върни tv->vval.v_number != 0;
	case VAR_FLOAT:
	    върни tv->vval.v_float != 0.0;
	case VAR_PARTIAL:
	    върни tv->vval.v_partial != NULL;
	case VAR_FUNC:
	case VAR_STRING:
	    върни tv->vval.v_string != NULL && *tv->vval.v_string != NUL;
	case VAR_LIST:
	    върни tv->vval.v_list != NULL && tv->vval.v_list->lv_len > 0;
	case VAR_TUPLE:
	    върни tuple_len(tv->vval.v_tuple) > 0;
	case VAR_DICT:
	    върни tv->vval.v_dict != NULL
				    && tv->vval.v_dict->dv_hashtab.ht_used > 0;
	case VAR_BOOL:
	case VAR_SPECIAL:
	    върни tv->vval.v_number == VVAL_TRUE ? TRUE : FALSE;
	case VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    върни tv->vval.v_job != NULL;
#иначе
	    break;
#endif
	case VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    върни tv->vval.v_channel != NULL;
#иначе
	    break;
#endif
	case VAR_BLOB:
	    върни tv->vval.v_blob != NULL && tv->vval.v_blob->bv_ga.ga_len > 0;

	case VAR_OBJECT:
	    върни tv->vval.v_object != NULL;

	case VAR_CLASS:
	case VAR_TYPEALIAS:
	    check_typval_is_value(tv);
	    break;

	case VAR_UNKNOWN:
	case VAR_ANY:
	case VAR_VOID:
	case VAR_INSTR:
	    break;
    }
    върни FALSE;
}

    void
emsg_using_string_as(typval_T *tv, цяло as_number)
{
    semsg(_(as_number ? e_using_string_as_number_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == NULL
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    цяло
check_not_string(typval_T *tv)
{
    ако (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	върни FAIL;
    }
    върни OK;
}

#endif // FEAT_EVAL
