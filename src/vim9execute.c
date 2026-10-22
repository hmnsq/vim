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
#अन्तर्भाव "vim.h"

#यदि defined(FEAT_EVAL)

// Structure put on ec_trystack when ISN_TRY is encountered.
प्रकारनिर्देश संरचना {
    पूर्णाङ्क	    tcd_frame_idx;	// ec_frame_idx at ISN_TRY
    पूर्णाङ्क	    tcd_थाक_len;	// size of ectx.ec_stack at ISN_TRY
    पूर्णाङ्क	    tcd_in_catch;	// in catch or finally block
    पूर्णाङ्क	    tcd_did_throw;	// set did_throw in :endtry
    पूर्णाङ्क	    tcd_catch_idx;	// instruction of the first :catch or :finally
    पूर्णाङ्क	    tcd_finally_idx;	// instruction of the :finally block or zero
    पूर्णाङ्क	    tcd_endtry_idx;	// instruction of the :endtry
    पूर्णाङ्क	    tcd_caught;		// catch block entered
    पूर्णाङ्क	    tcd_cont;		// :continue encountered, jump here (minus one)
    पूर्णाङ्क	    tcd_return;		// when TRUE return from end of :finally
} trycmd_T;

// Data local to a function.
// On a function call, if not empty, is saved on the stack and restored when
// returning.
प्रकारनिर्देश संरचना {
    पूर्णाङ्क		floc_restore_cmdmod;
    cmdmod_T	floc_save_cmdmod;
    पूर्णाङ्क		floc_restore_cmdmod_stacklen;
} funclocal_T;

// Structure to hold a reference to an outer_T, with information of whether it
// was allocated.
प्रकारनिर्देश संरचना {
    outer_T	*or_outer;
    partial_T	*or_partial;	// decrement "or_partial->pt_refcount" later
    पूर्णाङ्क		or_outer_allocated;  // free "or_outer" later
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
संरचना ectx_S {
    garray_T	ec_थाक;	// stack of typval_T values
    पूर्णाङ्क		ec_frame_idx;	// index in ec_stack: context of ec_dfunc_idx
    पूर्णाङ्क		ec_initial_frame_idx;	// frame index when called

    outer_ref_T	*ec_outer_ref;	// outer scope used for closures, allocated
    funclocal_T ec_funclocal;

    garray_T	ec_trystack;	// stack of trycmd_T values

    isn_T	*ec_instr;	// array with instructions
    पूर्णाङ्क		ec_dfunc_idx;	// current function index
    पूर्णाङ्क		ec_iidx;	// index in ec_instr: instruction to execute

    garray_T	ec_funcrefs;	// partials that might be a closure

    पूर्णाङ्क		ec_did_emsg_before;
    पूर्णाङ्क		ec_trylevel_at_start;
    where_T	ec_where;
};

#ifdef FEAT_PROFILE
// stack of profinfo_T used when profiling.
स्थिर garray_T profile_info_ga = {0, 0, परिमाण(profinfo_T), 20, रिक्त};
#endif

// Get pointer to item in the stack.
#define थाक_TV(idx) (((typval_T *)ectx->ec_थाक.ga_data) + idx)

// Get pointer to item relative to the bottom of the stack, -1 is the last one.
#define थाक_TV_BOT(idx) (((typval_T *)ectx->ec_थाक.ga_data) + ectx->ec_थाक.ga_len + (idx))

// Get pointer to a local variable on the stack.  Negative for arguments.
#define थाक_TV_VAR(idx) (((typval_T *)ectx->ec_थाक.ga_data) + ectx->ec_frame_idx + थाक_FRAME_SIZE + idx)

// Return value for functions used to execute instructions
#define EXEC_FAIL	0
#define EXEC_OK		1
#define EXEC_DONE	2

    शून्य
to_string_error(vartype_T vartype)
{
    semsg(_(e_cannot_convert_str_to_string), vartype_name(vartype));
}

/*
 * Return the number of arguments, including optional arguments and any vararg.
 */
    स्थिर पूर्णाङ्क
ufunc_argcount(ufunc_T *ufunc)
{
    फर्काऊ ufunc->uf_args.ga_len + (ufunc->uf_va_name != रिक्त ? 1 : 0);
}

/*
 * Create a new string from "count" items at the bottom of the stack.
 * A trailing NUL is appended.
 * When "count" is zero an empty string is added to the stack.
 */
    स्थिर पूर्णाङ्क
exe_concat(पूर्णाङ्क गन्ती, ectx_T *ectx)
{
    पूर्णाङ्क		idx;
    माप_प्रकार	len = 0;
    garray_T	ga;
    प्रकारनिर्देश संरचना
    {
	typval_T    *tv;
	माप_प्रकार	    length;
    } string_segment_T;
    गणना
    {
	STRING_SEGMENT_CACHE_SIZE = 10
    };
    string_segment_T	fixed_string_segment_tab[STRING_SEGMENT_CACHE_SIZE];
    string_segment_T	*string_segment_tab = &fixed_string_segment_tab[0];	// an array of cached typevals and lengths
    string_segment_T    *segment;

    यदि (गन्ती > (पूर्णाङ्क)ARRAY_LENGTH(fixed_string_segment_tab))
    {
	// make an array big enough to store the length of each string segment
	string_segment_tab = ALLOC_MULT(string_segment_T, गन्ती);
	यदि (string_segment_tab == रिक्त)
	    फर्काऊ FAIL;
    }

    ga_init2(&ga, परिमाण(वर्ण), 1);
    // Preallocate enough space for the whole string to avoid having to grow
    // and copy.
    लागि (idx = 0; idx < गन्ती; ++idx)
    {
	segment = &string_segment_tab[idx];
	segment->tv = थाक_TV_BOT(idx - गन्ती);
	यदि (segment->tv->vval.v_string != रिक्त)
	{
	    segment->length = STRLEN(segment->tv->vval.v_string);
	    len += segment->length;
	}
	अन्यथा
	    segment->length = 0;    // Ensure clean state for the second pass
    }

    यदि (ga_grow(&ga, (पूर्णाङ्क)len + 1) == FAIL)
    {
	यदि (string_segment_tab != fixed_string_segment_tab)
	    vim_free(string_segment_tab);
	फर्काऊ FAIL;
    }

    लागि (idx = 0; idx < गन्ती; ++idx)
    {
	segment = &string_segment_tab[idx];
	ga_concat_len(&ga, segment->tv->vval.v_string, segment->length);
	clear_tv(segment->tv);
    }

    // add a terminating NUL
    ga_append(&ga, NUL);

    ectx->ec_थाक.ga_len -= गन्ती - 1;
    थाक_TV_BOT(-1)->vval.v_string = ga.ga_data;

    यदि (string_segment_tab != fixed_string_segment_tab)
	vim_free(string_segment_tab);

    फर्काऊ OK;
}

/*
 * Create a new list from "count" items at the bottom of the stack.
 * When "count" is zero an empty list is added to the stack.
 * When "count" is -1 a NULL list is added to the stack.
 */
    स्थिर पूर्णाङ्क
exe_newlist(पूर्णाङ्क गन्ती, ectx_T *ectx)
{
    list_T	*list = रिक्त;
    पूर्णाङ्क		idx;
    typval_T	*tv;

    यदि (गन्ती >= 0)
    {
	list = list_alloc_with_वस्तुहरू(गन्ती);
	यदि (list == रिक्त)
	    फर्काऊ FAIL;
	लागि (idx = 0; idx < गन्ती; ++idx)
	    list_set_item(list, idx, थाक_TV_BOT(idx - गन्ती));
    }

    यदि (गन्ती > 0)
	ectx->ec_थाक.ga_len -= गन्ती - 1;
    अन्यथा यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
    {
	list_unref(list);
	फर्काऊ FAIL;
    }
    अन्यथा
	++ectx->ec_थाक.ga_len;
    tv = थाक_TV_BOT(-1);
    tv->v_type = VAR_LIST;
    tv->vval.v_list = list;
    tv->v_lock = 0;
    यदि (list != रिक्त)
	++list->lv_refcount;
    फर्काऊ OK;
}

/*
 * Create a new tuple from "count" items at the bottom of the stack.
 * When "count" is zero an empty tuple is added to the stack.
 * When "count" is -1 a NULL tuple is added to the stack.
 */
    स्थिर पूर्णाङ्क
exe_newtuple(पूर्णाङ्क गन्ती, ectx_T *ectx)
{
    tuple_T	*tuple = रिक्त;
    पूर्णाङ्क		idx;
    typval_T	*tv;

    यदि (गन्ती >= 0)
    {
	tuple = tuple_alloc_with_वस्तुहरू(गन्ती);
	यदि (tuple == रिक्त)
	    फर्काऊ FAIL;
	लागि (idx = 0; idx < गन्ती; ++idx)
	    tuple_set_item(tuple, idx, थाक_TV_BOT(idx - गन्ती));
    }

    यदि (गन्ती > 0)
	ectx->ec_थाक.ga_len -= गन्ती - 1;
    अन्यथा यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
    {
	tuple_unref(tuple);
	फर्काऊ FAIL;
    }
    अन्यथा
	++ectx->ec_थाक.ga_len;
    tv = थाक_TV_BOT(-1);
    tv->v_type = VAR_TUPLE;
    tv->vval.v_tuple = tuple;
    tv->v_lock = 0;
    यदि (tuple != रिक्त)
	++tuple->tv_refcount;

    फर्काऊ OK;
}

/*
 * Implementation of ISN_NEWDICT.
 * Returns FAIL on total failure, MAYBE on error.
 */
    स्थिर पूर्णाङ्क
exe_newdict(पूर्णाङ्क गन्ती, ectx_T *ectx)
{
    dict_T	*dict = रिक्त;
    dictitem_T	*item;
    char_u	*साँचो;
    पूर्णाङ्क		idx;
    typval_T	*tv;

    यदि (गन्ती >= 0)
    {
	dict = dict_alloc();
	यदि (unlikely(dict == रिक्त))
	    फर्काऊ FAIL;
	लागि (idx = 0; idx < गन्ती; ++idx)
	{
	    // have already checked key type is VAR_STRING
	    tv = थाक_TV_BOT(2 * (idx - गन्ती));
	    // check key is unique
	    साँचो = tv->vval.v_string == रिक्त ? (char_u *)"" : tv->vval.v_string;
	    item = dict_find(dict, साँचो, -1);
	    यदि (item != रिक्त)
	    {
		semsg(_(e_duplicate_साँचो_in_dictionary_str), साँचो);
		dict_unref(dict);
		फर्काऊ MAYBE;
	    }
	    item = dictitem_alloc(साँचो);
	    clear_tv(tv);
	    यदि (unlikely(item == रिक्त))
	    {
		dict_unref(dict);
		फर्काऊ FAIL;
	    }
	    tv = थाक_TV_BOT(2 * (idx - गन्ती) + 1);
	    item->di_tv = *tv;
	    item->di_tv.v_lock = 0;
	    tv->v_type = VAR_UNKNOWN;
	    यदि (dict_add(dict, item) == FAIL)
	    {
		// can this ever happen?
		dictitem_free(item);
		dict_unref(dict);
		फर्काऊ FAIL;
	    }
	}
    }

    यदि (गन्ती > 0)
	ectx->ec_थाक.ga_len -= 2 * गन्ती - 1;
    अन्यथा यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
    {
	dict_unref(dict);
	फर्काऊ FAIL;
    }
    अन्यथा
	++ectx->ec_थाक.ga_len;
    tv = थाक_TV_BOT(-1);
    tv->v_type = VAR_DICT;
    tv->v_lock = 0;
    tv->vval.v_dict = dict;
    यदि (dict != रिक्त)
	++dict->dv_refcount;
    फर्काऊ OK;
}

/*
 * If debug_tick changed check if "ufunc" has a breakpoint and update
 * "uf_has_breakpoint".
 */
    शून्य
update_has_breakpoint(ufunc_T *ufunc)
{
    यदि (ufunc->uf_debug_tick == debug_tick)
	फर्काऊ;

    linenr_T breakpoint;

    ufunc->uf_debug_tick = debug_tick;
    breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name, 0);
    ufunc->uf_has_breakpoint = breakpoint > 0;
}

स्थिर garray_T dict_थाक = GA_EMPTY;

/*
 * Put a value on the dict stack.  This consumes "tv".
 */
    स्थिर पूर्णाङ्क
dict_थाक_save(typval_T *tv)
{
    यदि (dict_थाक.ga_growsize == 0)
	ga_init2(&dict_थाक, परिमाण(typval_T), 10);
    यदि (ga_grow(&dict_थाक, 1) == FAIL)
	फर्काऊ FAIL;
    ((typval_T *)dict_थाक.ga_data)[dict_थाक.ga_len] = *tv;
    ++dict_थाक.ga_len;
    फर्काऊ OK;
}

/*
 * Get the typval at top of the dict stack.
 */
    स्थिर typval_T *
dict_थाक_get_tv(शून्य)
{
    यदि (dict_थाक.ga_len == 0)
	फर्काऊ रिक्त;
    फर्काऊ ((typval_T *)dict_थाक.ga_data) + dict_थाक.ga_len - 1;
}

/*
 * Get the dict at top of the dict stack.
 */
    स्थिर dict_T *
dict_थाक_get_dict(शून्य)
{
    typval_T *tv;

    यदि (dict_थाक.ga_len == 0)
	फर्काऊ रिक्त;
    tv = ((typval_T *)dict_थाक.ga_data) + dict_थाक.ga_len - 1;
    यदि (tv->v_type == VAR_DICT)
	फर्काऊ tv->vval.v_dict;
    फर्काऊ रिक्त;
}

/*
 * Drop an item from the dict stack.
 */
    स्थिर शून्य
dict_थाक_drop(शून्य)
{
    यदि (dict_थाक.ga_len == 0)
    {
	iemsg("Dict stack underflow");
	फर्काऊ;
    }
    --dict_थाक.ga_len;
    clear_tv(((typval_T *)dict_थाक.ga_data) + dict_थाक.ga_len);
}

/*
 * Drop items from the dict stack until the length is equal to "len".
 */
    स्थिर शून्य
dict_थाक_clear(पूर्णाङ्क len)
{
    जबसम्म (dict_थाक.ga_len > len)
	dict_थाक_drop();
}

/*
 * Get a pointer to useful "pt_outer" of "pt".
 */
    स्थिर outer_T *
get_pt_outer(partial_T *pt)
{
    partial_T *ptref = pt->pt_outer_partial;

    यदि (ptref == रिक्त)
	फर्काऊ &pt->pt_outer;

    // partial using partial (recursively)
    जबसम्म (ptref->pt_outer_partial != रिक्त)
	ptref = ptref->pt_outer_partial;
    फर्काऊ &ptref->pt_outer;
}

/*
 * Check "argcount" arguments on the stack against what "ufunc" expects.
 * "off" is the offset of arguments on the stack.
 * Return OK or FAIL.
 */
    स्थिर पूर्णाङ्क
check_ufunc_arg_types(ufunc_T *ufunc, पूर्णाङ्क argcount, पूर्णाङ्क off, ectx_T *ectx)
{
    यदि (ufunc->uf_arg_types == रिक्त && ufunc->uf_va_type == रिक्त)
	फर्काऊ OK;

    typval_T	*argv = थाक_TV_BOT(0) - argcount - off;

    // The function can change at runtime, check that the argument
    // types are correct.
    लागि (पूर्णाङ्क i = 0; i < argcount; ++i)
    {
	type_T *type = रिक्त;

	// assume a v:none argument, using the default value, is always OK
	यदि (argv[i].v_type == VAR_SPECIAL
		&& argv[i].vval.v_सङ्ख्या == VVAL_NONE)
	    जारी;
	// only pass values to user functions, never types
	यदि (check_typval_is_मान(&argv[i]) == FAIL)
	    फर्काऊ FAIL;

	यदि (i < ufunc->uf_args.ga_len && ufunc->uf_arg_types != रिक्त)
	    type = ufunc->uf_arg_types[i];
	अन्यथा यदि (ufunc->uf_va_type != रिक्त)
	    type = ufunc->uf_va_type->tt_member;
	यदि (type != रिक्त && check_typval_arg_type(type,
		    &argv[i], रिक्त, i + 1) == FAIL)
	    फर्काऊ FAIL;
    }
    फर्काऊ OK;
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
    स्थिर पूर्णाङ्क
call_dfunc(
	पूर्णाङ्क		cdf_idx,
	partial_T	*pt_arg,
	पूर्णाङ्क		argcount_arg,
	ectx_T		*ectx)
{
    पूर्णाङ्क		argcount = argcount_arg;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    ufunc_T	*ufunc = dfunc->df_ufunc;
    पूर्णाङ्क		did_emsg_before = did_emsg_cumul + did_emsg;
    पूर्णाङ्क		arg_to_add;
    पूर्णाङ्क		vararg_गन्ती = 0;
    पूर्णाङ्क		varcount;
    पूर्णाङ्क		idx;
    estack_T	*entry;
    funclocal_T	*floc = रिक्त;
    पूर्णाङ्क		res = OK;
    compiletype_T compile_type;

    यदि (dfunc->df_deleted)
    {
	// don't use ufunc->uf_name, it may have been freed
	emsg_funcname(e_function_was_deleted_str,
		dfunc->df_name == रिक्त ? (char_u *)"unknown" : dfunc->df_name);
	फर्काऊ FAIL;
    }

#ifdef FEAT_PROFILE
    यदि (do_profiling == PROF_YES)
    {
	यदि (GA_GROW_OK(&profile_info_ga, 1))
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
    यदि (func_needs_compiling(ufunc, compile_type))
    {
	res = compile_def_function(ufunc, FALSE, compile_type, रिक्त);

	// compile_def_function() may cause def_functions.ga_data to change
	dfunc = ((dfunc_T *)def_functions.ga_data) + cdf_idx;
    }
    यदि (res == FAIL || INSTRUCTIONS(dfunc) == रिक्त)
    {
	यदि (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	फर्काऊ FAIL;
    }

    यदि (ufunc->uf_va_name != रिक्त)
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
	vararg_गन्ती = argcount - ufunc->uf_args.ga_len;
	यदि (vararg_गन्ती < 0)
	    vararg_गन्ती = 0;
	अन्यथा
	    argcount -= vararg_गन्ती;
	यदि (exe_newlist(vararg_गन्ती, ectx) == FAIL)
	    फर्काऊ FAIL;

	vararg_गन्ती = 1;
    }

    arg_to_add = ufunc->uf_args.ga_len - argcount;
    यदि (arg_to_add < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
			-arg_to_add), -arg_to_add);
	फर्काऊ FAIL;
    }
    अन्यथा यदि (arg_to_add > ufunc->uf_def_args.ga_len)
    {
	पूर्णाङ्क missing = arg_to_add - ufunc->uf_def_args.ga_len;

	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
			missing), missing);
	फर्काऊ FAIL;
    }

    // If this is an object method, the object is just before the arguments.
    typval_T	*obj = थाक_TV_BOT(0) - argcount - vararg_गन्ती - 1;

    यदि (IS_OBJECT_METHOD(ufunc) && !IS_CONSTRUCTOR_METHOD(ufunc)
	    && obj->v_type == VAR_OBJECT && obj->vval.v_object == रिक्त)
    {
	// If this is not a constructor method, then a valid object is
	// needed.
	emsg(_(e_using_null_object));
	फर्काऊ FAIL;
    }

    // Check the argument types.
    यदि (check_ufunc_arg_types(ufunc, argcount, vararg_गन्ती, ectx) == FAIL)
	फर्काऊ FAIL;

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
    यदि (GA_GROW_FAILS(&ectx->ec_थाक,
				     arg_to_add + थाक_FRAME_SIZE + varcount))
	फर्काऊ FAIL;

    // The object pointer is in the execution typval stack.  The GA_GROW call
    // above may have reallocated the execution typval stack.  So the object
    // pointer may not be valid anymore.  Get the object pointer again from the
    // execution stack.
    obj = थाक_TV_BOT(0) - argcount - vararg_गन्ती - 1;

    // If depth of calling is getting too high, don't execute the function.
    यदि (funcdepth_increment() == FAIL)
	फर्काऊ FAIL;
    ++ex_nesting_level;

    // Only make a copy of funclocal if it contains something to restore.
    यदि (ectx->ec_funclocal.floc_restore_cmdmod)
    {
	floc = ALLOC_ONE(funclocal_T);
	यदि (floc == रिक्त)
	    फर्काऊ FAIL;
	*floc = ectx->ec_funclocal;
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    }

    // Move the vararg-list to below the missing optional arguments.
    यदि (vararg_गन्ती > 0 && arg_to_add > 0)
	*थाक_TV_BOT(arg_to_add - 1) = *थाक_TV_BOT(-1);

    // Reserve space for omitted optional arguments, filled in soon.
    लागि (idx = 0; idx < arg_to_add; ++idx)
	थाक_TV_BOT(idx - vararg_गन्ती)->v_type = VAR_UNKNOWN;
    ectx->ec_थाक.ga_len += arg_to_add;

    // Store current execution state in stack frame for ISN_RETURN.
    थाक_TV_BOT(थाक_FRAME_FUNC_OFF)->vval.v_सङ्ख्या = ectx->ec_dfunc_idx;
    थाक_TV_BOT(थाक_FRAME_IIDX_OFF)->vval.v_सङ्ख्या = ectx->ec_iidx;
    थाक_TV_BOT(थाक_FRAME_INSTR_OFF)->vval.v_string = (शून्य *)ectx->ec_instr;
    थाक_TV_BOT(थाक_FRAME_OUTER_OFF)->vval.v_string =
						    (शून्य *)ectx->ec_outer_ref;
    थाक_TV_BOT(थाक_FRAME_FUNCLOCAL_OFF)->vval.v_string = (शून्य *)floc;
    थाक_TV_BOT(थाक_FRAME_IDX_OFF)->vval.v_सङ्ख्या = ectx->ec_frame_idx;
    ectx->ec_frame_idx = ectx->ec_थाक.ga_len;

    // Initialize all local variables to number zero.  Also initialize the
    // variable that counts how many closures were created.  This is used in
    // handle_closure_in_use().
    पूर्णाङ्क initcount = dfunc->df_varcount + (dfunc->df_has_closure ? 1 : 0);
    लागि (idx = 0; idx < initcount; ++idx)
    {
	typval_T *tv = थाक_TV_BOT(थाक_FRAME_SIZE + idx);

	tv->v_type = VAR_सङ्ख्या;
	tv->vval.v_सङ्ख्या = 0;
    }
    ectx->ec_थाक.ga_len += थाक_FRAME_SIZE + varcount;

    // For an object method move the object from just before the arguments to
    // the first local variable.
    यदि (IS_OBJECT_METHOD(ufunc))
    {
	यदि (obj->v_type != VAR_OBJECT)
	{
	    semsg(_(e_internal_error_str), "type in stack is not an object");
	    फर्काऊ FAIL;
	}

	*थाक_TV_VAR(0) = *obj;
	obj->v_type = VAR_UNKNOWN;
    }

    partial_T *pt = pt_arg != रिक्त ? pt_arg : ufunc->uf_partial;
    यदि (pt != रिक्त || (ufunc->uf_flags & FC_CLOSURE))
    {
	outer_ref_T *ref = ALLOC_CLEAR_ONE(outer_ref_T);

	यदि (ref == रिक्त)
	    फर्काऊ FAIL;
	यदि (pt != रिक्त)
	{
	    ref->or_outer = get_pt_outer(pt);
	    ++pt->pt_refcount;
	    ref->or_partial = pt;
	}
	अन्यथा
	{
	    ref->or_outer = ALLOC_CLEAR_ONE(outer_T);
	    यदि (unlikely(ref->or_outer == रिक्त))
	    {
		vim_free(ref);
		फर्काऊ FAIL;
	    }
	    ref->or_outer_allocated = TRUE;
	    ref->or_outer->out_थाक = &ectx->ec_थाक;
	    ref->or_outer->out_frame_idx = ectx->ec_frame_idx;
	    यदि (ectx->ec_outer_ref != रिक्त)
		ref->or_outer->out_up = ectx->ec_outer_ref->or_outer;
	}
	ectx->ec_outer_ref = ref;
    }
    अन्यथा
	ectx->ec_outer_ref = रिक्त;

    यदि (ufunc->uf_flags & FC_SANDBOX)
	++sandbox;

    ++ufunc->uf_calls;

    // Set execution state to the start of the called function.
    ectx->ec_dfunc_idx = cdf_idx;
    ectx->ec_instr = INSTRUCTIONS(dfunc);
    entry = estack_थप_ufunc(ufunc, 1);
    यदि (entry != रिक्त)
    {
	// Set the script context to the script where the function was defined.
	// Save the current context so it can be restored on return.
	entry->es_save_sctx = current_sctx;
	current_sctx = ufunc->uf_script_ctx;
    }

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    फर्काऊ OK;
}

// Double linked list of funcstack_T in use.
स्थिर funcstack_T *first_funcstack = रिक्त;

    स्थिर शून्य
add_funcstack_to_list(funcstack_T *funcstack)
{
	// Link in list of funcstacks.
    यदि (first_funcstack != रिक्त)
	first_funcstack->fs_prev = funcstack;
    funcstack->fs_next = first_funcstack;
    funcstack->fs_prev = रिक्त;
    first_funcstack = funcstack;
}

    स्थिर शून्य
remove_funcstack_from_list(funcstack_T *funcstack)
{
    यदि (funcstack->fs_prev == रिक्त)
	first_funcstack = funcstack->fs_next;
    अन्यथा
	funcstack->fs_prev->fs_next = funcstack->fs_next;
    यदि (funcstack->fs_next != रिक्त)
	funcstack->fs_next->fs_prev = funcstack->fs_prev;
}

/*
 * Used when returning from a function: Check if any closure is still
 * referenced.  If so then move the arguments and variables to a separate piece
 * of stack to be used when the closure is called.
 * When "free_arguments" is TRUE the arguments are to be freed.
 * Returns FAIL when out of memory.
 */
    स्थिर पूर्णाङ्क
handle_closure_in_use(ectx_T *ectx, पूर्णाङ्क free_arguments)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    पूर्णाङ्क		argcount;
    पूर्णाङ्क		शीर्ष;
    पूर्णाङ्क		idx;
    typval_T	*tv;
    पूर्णाङ्क		closure_in_use = FALSE;
    garray_T	*gap = &ectx->ec_funcrefs;
    varnumber_T	closure_गन्ती;

    यदि (dfunc->df_ufunc == रिक्त)
	फर्काऊ OK;  // function was freed
    यदि (dfunc->df_has_closure == 0)
	फर्काऊ OK;  // no closures
    tv = थाक_TV(ectx->ec_frame_idx + थाक_FRAME_SIZE + dfunc->df_varcount);
    closure_गन्ती = tv->vval.v_सङ्ख्या;
    यदि (closure_गन्ती == 0)
	फर्काऊ OK;  // no funcrefs created

    // Compute "top": the first entry in the stack used by the function.
    // This is the first argument (after that comes the stack frame and then
    // the local variables).
    argcount = ufunc_argcount(dfunc->df_ufunc);
    शीर्ष = ectx->ec_frame_idx - argcount;

    // Check if any created closure is still in use.
    लागि (idx = 0; idx < closure_गन्ती; ++idx)
    {
	partial_T   *pt;
	पूर्णाङ्क	    off = gap->ga_len - closure_गन्ती + idx;

	यदि (off < 0)
	    जारी;  // count is off or already done
	pt = ((partial_T **)gap->ga_data)[off];
	यदि (pt->pt_refcount > 1)
	{
	    पूर्णाङ्क refcount = pt->pt_refcount;
	    पूर्णाङ्क i;

	    // A Reference in a local variable doesn't count, it gets
	    // unreferenced on return.
	    लागि (i = 0; i < dfunc->df_varcount; ++i)
	    {
		typval_T *stv = थाक_TV(ectx->ec_frame_idx
						       + थाक_FRAME_SIZE + i);
		यदि (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    यदि (refcount > 1)
	    {
		closure_in_use = TRUE;
		रोक;
	    }
	}
    }

    यदि (closure_in_use)
    {
	funcstack_T *funcstack = ALLOC_CLEAR_ONE(funcstack_T);
	typval_T    *थाक;

	// A closure is using the arguments and/or local variables.
	// Move them to the called function.
	यदि (funcstack == रिक्त)
	    फर्काऊ FAIL;

	funcstack->fs_var_offset = argcount + थाक_FRAME_SIZE;
	funcstack->fs_ga.ga_len = funcstack->fs_var_offset
							  + dfunc->df_varcount;
	थाक = ALLOC_CLEAR_MULT(typval_T, funcstack->fs_ga.ga_len);
	funcstack->fs_ga.ga_data = थाक;
	यदि (थाक == रिक्त)
	{
	    vim_free(funcstack);
	    फर्काऊ FAIL;
	}
	add_funcstack_to_list(funcstack);

	// Move or copy the arguments.
	लागि (idx = 0; idx < argcount; ++idx)
	{
	    tv = थाक_TV(शीर्ष + idx);
	    यदि (free_arguments)
	    {
		*(थाक + idx) = *tv;
		tv->v_type = VAR_UNKNOWN;
	    }
	    अन्यथा यदि (tv->v_type != VAR_UNKNOWN)
		// Skip an argument that was not set, the stack was cleared.
		copy_tv(tv, थाक + idx);
	}
	// Skip the stack frame.
	// Move the local variables.
	लागि (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    tv = थाक_TV(ectx->ec_frame_idx + थाक_FRAME_SIZE + idx);

	    // A partial created for a local function, that is also used as a
	    // local variable, has a reference count for the variable, thus
	    // will never go down to zero.  When all these refcounts are one
	    // then the funcstack is unused.  We need to count how many we have
	    // so we know when to check.
	    यदि (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != रिक्त)
	    {
		पूर्णाङ्क	    i;

		लागि (i = 0; i < closure_गन्ती; ++i)
		    यदि (tv->vval.v_partial == ((partial_T **)gap->ga_data)[
					      gap->ga_len - closure_गन्ती + i])
			++funcstack->fs_min_refcount;
	    }

	    *(थाक + funcstack->fs_var_offset + idx) = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}

	लागि (idx = 0; idx < closure_गन्ती; ++idx)
	{
	    partial_T *pt = ((partial_T **)gap->ga_data)[gap->ga_len
							- closure_गन्ती + idx];
	    यदि (pt->pt_refcount > 1)
	    {
		++funcstack->fs_refcount;
		pt->pt_funcstack = funcstack;
		pt->pt_outer.out_थाक = &funcstack->fs_ga;
		pt->pt_outer.out_frame_idx = ectx->ec_frame_idx - शीर्ष;
	    }
	}
    }

    लागि (idx = 0; idx < closure_गन्ती; ++idx)
	partial_unref(((partial_T **)gap->ga_data)[gap->ga_len
						       - closure_गन्ती + idx]);
    gap->ga_len -= closure_गन्ती;
    यदि (gap->ga_len == 0)
	ga_clear(gap);

    फर्काऊ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * funcstack may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the funcstack have a reference count of one.
 * Returns TRUE if the funcstack is freed, the partial referencing it will then
 * also have been freed.
 */
    पूर्णाङ्क
funcstack_check_refcount(funcstack_T *funcstack)
{
    पूर्णाङ्क		i;
    garray_T	*gap = &funcstack->fs_ga;
    पूर्णाङ्क		done = 0;
    typval_T	*थाक;

    यदि (funcstack->fs_refcount > funcstack->fs_min_refcount)
	फर्काऊ FALSE;
    लागि (i = funcstack->fs_var_offset; i < gap->ga_len; ++i)
    {
	typval_T *tv = ((typval_T *)gap->ga_data) + i;

	यदि (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != रिक्त
		&& tv->vval.v_partial->pt_funcstack == funcstack
		&& tv->vval.v_partial->pt_refcount == 1)
	    ++done;
    }
    यदि (done != funcstack->fs_min_refcount)
	फर्काऊ FALSE;

    थाक = gap->ga_data;

    // All partials referencing the funcstack have a reference count of
    // one, thus the funcstack is no longer of use.
    लागि (i = 0; i < gap->ga_len; ++i)
	clear_tv(थाक + i);
    vim_free(थाक);
    remove_funcstack_from_list(funcstack);
    vim_free(funcstack);

    फर्काऊ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all funcstacks.
 */
    पूर्णाङ्क
set_ref_in_funcstacks(पूर्णाङ्क copyID)
{
    funcstack_T *funcstack;

    लागि (funcstack = first_funcstack; funcstack != रिक्त;
						funcstack = funcstack->fs_next)
    {
	typval_T    *थाक = funcstack->fs_ga.ga_data;
	पूर्णाङ्क	    i;

	लागि (i = 0; i < funcstack->fs_ga.ga_len; ++i)
	    यदि (set_ref_in_item(थाक + i, copyID, रिक्त, रिक्त, रिक्त))
		फर्काऊ TRUE;  // abort
    }
    फर्काऊ FALSE;
}

// Ugly static to avoid passing the execution context around through many
// layers.
स्थिर ectx_T *current_ectx = रिक्त;

/*
 * Return TRUE if currently executing a :def function.
 * Can be used by builtin functions only.
 */
    पूर्णाङ्क
in_def_function(शून्य)
{
    फर्काऊ current_ectx != रिक्त;
}

/*
 * If executing a class/object method, then fill in the lval_T.
 * Set lr_tv to the executing item, and lr_exec_class to the executing class;
 * use free_tv and class_unref when finished with the lval_root.
 * For use by builtin functions.
 *
 * Return FAIL and do nothing if not executing in a class; otherwise OK.
 */
    पूर्णाङ्क
fill_exec_lval_जरा(lval_जरा_T *जरा)
{
    ectx_T *ectx = current_ectx;
    यदि (ectx != रिक्त)
    {
	dfunc_T	    *dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
	ufunc_T    *ufunc = dfunc->df_ufunc;
	यदि (ufunc->uf_class != रिक्त)	// executing a method?
	{
	    typval_T *tv = alloc_tv();
	    यदि (tv != रिक्त)
	    {
		CLEAR_POINTER(जरा);
		जरा->lr_tv = tv;
		copy_tv(थाक_TV_VAR(0), जरा->lr_tv);
		जरा->lr_cl_exec = ufunc->uf_class;
		++जरा->lr_cl_exec->class_refcount;
		फर्काऊ OK;
	    }
	}
    }

    फर्काऊ FAIL;
}

/*
 * Clear "current_ectx" and return the previous value.  To be used when calling
 * a user function.
 */
    ectx_T *
clear_current_ectx(शून्य)
{
    ectx_T *r = current_ectx;

    current_ectx = रिक्त;
    फर्काऊ r;
}

    शून्य
restore_current_ectx(ectx_T *ectx)
{
    यदि (current_ectx != रिक्त)
	iemsg("Restoring current_ectx while it is not NULL");
    current_ectx = ectx;
}

/*
 * Add an entry for a deferred function call to the currently executing
 * function.
 * Return the list or NULL when failed.
 */
    स्थिर list_T *
add_defer_item(पूर्णाङ्क var_idx, पूर्णाङ्क argcount, ectx_T *ectx)
{
    typval_T	*defer_tv = थाक_TV_VAR(var_idx);
    list_T	*defer_l;
    list_T	*l;
    typval_T	listval;

    यदि (defer_tv->v_type != VAR_LIST)
    {
	// first time, allocate the list
	यदि (rettv_list_alloc(defer_tv) == FAIL)
	    फर्काऊ रिक्त;
    }
    defer_l = defer_tv->vval.v_list;

    l = list_alloc_with_वस्तुहरू(argcount + 1);
    यदि (l == रिक्त)
	फर्काऊ रिक्त;
    listval.v_type = VAR_LIST;
    listval.vval.v_list = l;
    listval.v_lock = 0;
    यदि (list_घुसाउ_tv(defer_l, &listval, defer_l->lv_first) == FAIL)
    {
	list_free(l);
	फर्काऊ रिक्त;
    }

    फर्काऊ l;
}

/*
 * Handle ISN_DEFER.  Stack has a function reference and "argcount" arguments.
 * The local variable that lists deferred functions is "var_idx".
 * Returns OK or FAIL.
 */
    स्थिर पूर्णाङ्क
defer_command(पूर्णाङ्क var_idx, पूर्णाङ्क argcount, ectx_T *ectx)
{
    list_T	*l = add_defer_item(var_idx, argcount, ectx);
    पूर्णाङ्क		i;
    typval_T	*func_tv;

    यदि (l == रिक्त)
	फर्काऊ FAIL;

    func_tv = थाक_TV_BOT(-argcount - 1);
    यदि (func_tv->v_type != VAR_PARTIAL && func_tv->v_type != VAR_FUNC)
    {
	semsg(_(e_expected_str_but_got_str),
		"function or partial",
		vartype_name(func_tv->v_type));
	फर्काऊ FAIL;
    }
    list_set_item(l, 0, func_tv);

    लागि (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, थाक_TV_BOT(-argcount + i));
    ectx->ec_थाक.ga_len -= argcount + 1;
    फर्काऊ OK;
}

/*
 * Add a deferred call for "name" with arguments "argvars[argcount]".
 * Consumes "name", also on failure.
 * Only to be called when in_def_function() returns TRUE.
 */
    पूर्णाङ्क
add_defer_function(char_u *name, पूर्णाङ्क argcount, typval_T *argvars)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
						  + current_ectx->ec_dfunc_idx;
    list_T	*l;
    typval_T	func_tv;
    पूर्णाङ्क		i;

    यदि (dfunc->df_defer_var_idx == 0)
    {
	iemsg("df_defer_var_idx is zero");
	vim_free(name);
	फर्काऊ FAIL;
    }

    l = add_defer_item(dfunc->df_defer_var_idx - 1, argcount, current_ectx);
    यदि (l == रिक्त)
    {
	vim_free(name);
	फर्काऊ FAIL;
    }

    func_tv.v_type = VAR_FUNC;
    func_tv.v_lock = 0;
    func_tv.vval.v_string = name;
    list_set_item(l, 0, &func_tv);

    लागि (i = 0; i < argcount; ++i)
	list_set_item(l, i + 1, argvars + i);
    फर्काऊ OK;
}

/*
 * Invoked when returning from a function: Invoke any deferred calls.
 */
    स्थिर शून्य
invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    typval_T	*defer_tv = थाक_TV_VAR(dfunc->df_defer_var_idx - 1);
    listitem_T	*li;

    यदि (defer_tv->v_type != VAR_LIST)
	फर्काऊ;	 // no function added
    FOR_ALL_LIST_वस्तुहरू(defer_tv->vval.v_list, li)
    {
	list_T	    *l = li->li_tv.vval.v_list;
	typval_T    rettv;
	typval_T    argvars[MAX_FUNC_ARGS];
	पूर्णाङ्क	    i;
	listitem_T  *arg_li = l->lv_first;
	typval_T    *functv = &l->lv_first->li_tv;
	पूर्णाङ्क	    argcount = l->lv_len - 1;

	यदि (functv->vval.v_string == रिक्त)
	    // already being called, can happen if function does ":qa"
	    जारी;

	लागि (i = 0; i < argcount; ++i)
	{
	    arg_li = arg_li->li_next;
	    argvars[i] = arg_li->li_tv;
	}

	funcexe_T	funcexe;
	char_u		*name = रिक्त;
	partial_T	*pt = रिक्त;

	CLEAR_FIELD(funcexe);
	funcexe.fe_evaluate = TRUE;
	rettv.v_type = VAR_UNKNOWN;

	यदि (functv->v_type == VAR_PARTIAL)
	{
	    pt = functv->vval.v_partial;
	    functv->vval.v_partial = रिक्त;

	    name = pt->pt_func->uf_name;
	    funcexe.fe_partial = pt;
	    funcexe.fe_object = pt->pt_obj;
	    यदि (funcexe.fe_object != रिक्त)
		++funcexe.fe_object->obj_refcount;
	}
	अन्यथा
	{
	    name = functv->vval.v_string;
	    functv->vval.v_string = रिक्त;
	}

	// If the deferred function is called after an exception, then only the
	// first statement in the function will be executed (because of the
	// exception).  So save and restore the try/catch/throw exception
	// state.
	exception_state_T estate;
	exception_state_save(&estate);
	exception_state_clear();

	(शून्य)call_func(name, -1, &rettv, argcount, argvars, &funcexe);

	exception_state_restore(&estate);

	clear_tv(&rettv);
	यदि (functv->v_type == VAR_PARTIAL)
	    partial_unref(pt);
	अन्यथा
	    vim_free(name);
    }
}

/*
 * Return from the current function.
 */
    स्थिर पूर्णाङ्क
func_return(ectx_T *ectx)
{
    पूर्णाङ्क		idx;
    पूर्णाङ्क		ret_idx;
    dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;
    पूर्णाङ्क		argcount = ufunc_argcount(dfunc->df_ufunc);
    estack_T	*entry;
    पूर्णाङ्क		prev_dfunc_idx = थाक_TV(ectx->ec_frame_idx
					+ थाक_FRAME_FUNC_OFF)->vval.v_सङ्ख्या;
    funclocal_T	*floc;
#ifdef FEAT_PROFILE
    dfunc_T	*prev_dfunc = ((dfunc_T *)def_functions.ga_data)
							      + prev_dfunc_idx;

    यदि (do_profiling == PROF_YES)
    {
	ufunc_T *caller = prev_dfunc->df_ufunc;

	यदि (dfunc->df_ufunc->uf_profiling
				   || (caller != रिक्त && caller->uf_profiling))
	{
	    profile_may_end_func(((profinfo_T *)profile_info_ga.ga_data)
			+ profile_info_ga.ga_len - 1, dfunc->df_ufunc, caller);
	    --profile_info_ga.ga_len;
	}
    }
#endif

    यदि (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);

    यदि (dfunc->df_ufunc->uf_flags & FC_SANDBOX)
	--sandbox;

    // No check for uf_refcount being zero, cannot think of a way that would
    // happen.
    --dfunc->df_ufunc->uf_calls;

    // execution context goes one level up
    entry = estack_झिक();
    यदि (entry != रिक्त)
	current_sctx = entry->es_save_sctx;

    यदि (handle_closure_in_use(ectx, TRUE) == FAIL)
	फर्काऊ FAIL;

    // Clear the arguments.  If this was an object method also clear the
    // object, it is just before the arguments.
    पूर्णाङ्क शीर्ष = ectx->ec_frame_idx - argcount;
    यदि (IS_OBJECT_METHOD(dfunc->df_ufunc))
	--शीर्ष;
    लागि (idx = शीर्ष; idx < ectx->ec_frame_idx; ++idx)
	clear_tv(थाक_TV(idx));

    // Clear local variables and temp values, but not the return value.
    लागि (idx = ectx->ec_frame_idx + थाक_FRAME_SIZE;
					idx < ectx->ec_थाक.ga_len - 1; ++idx)
	clear_tv(थाक_TV(idx));

    // The return value should be on top of the stack.  However, when aborting
    // it may not be there and ec_frame_idx is the top of the stack.
    ret_idx = ectx->ec_थाक.ga_len - 1;
    यदि (ret_idx == ectx->ec_frame_idx + थाक_FRAME_IDX_OFF)
	ret_idx = 0;

    यदि (ectx->ec_outer_ref != रिक्त)
    {
	यदि (ectx->ec_outer_ref->or_outer_allocated)
	    vim_free(ectx->ec_outer_ref->or_outer);
	partial_unref(ectx->ec_outer_ref->or_partial);
	vim_free(ectx->ec_outer_ref);
    }

    // Restore the previous frame.
    ectx->ec_dfunc_idx = prev_dfunc_idx;
    ectx->ec_iidx = थाक_TV(ectx->ec_frame_idx
					+ थाक_FRAME_IIDX_OFF)->vval.v_सङ्ख्या;
    ectx->ec_instr = (शून्य *)थाक_TV(ectx->ec_frame_idx
				       + थाक_FRAME_INSTR_OFF)->vval.v_string;
    ectx->ec_outer_ref = (शून्य *)थाक_TV(ectx->ec_frame_idx
				       + थाक_FRAME_OUTER_OFF)->vval.v_string;
    floc = (शून्य *)थाक_TV(ectx->ec_frame_idx
				   + थाक_FRAME_FUNCLOCAL_OFF)->vval.v_string;
    // restoring ec_frame_idx must be last
    ectx->ec_frame_idx = थाक_TV(ectx->ec_frame_idx
				       + थाक_FRAME_IDX_OFF)->vval.v_सङ्ख्या;

    यदि (floc == रिक्त)
	ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
    अन्यथा
    {
	ectx->ec_funclocal = *floc;
	vim_free(floc);
    }

    यदि (ret_idx > 0)
    {
	// Reset the stack to the position before the call, with a spot for the
	// return value, moved there from above the frame.
	ectx->ec_थाक.ga_len = शीर्ष + 1;
	*थाक_TV_BOT(-1) = *थाक_TV(ret_idx);
    }
    अन्यथा
	// Reset the stack to the position before the call.
	ectx->ec_थाक.ga_len = शीर्ष;

    funcdepth_decrement();
    --ex_nesting_level;
    फर्काऊ OK;
}

/*
 * Prepare arguments and rettv for calling a builtin or user function.
 */
    स्थिर पूर्णाङ्क
call_prepare(पूर्णाङ्क argcount, typval_T *argvars, ectx_T *ectx)
{
    पूर्णाङ्क		idx;
    typval_T	*tv;

    // Move arguments from bottom of the stack to argvars[] and add terminator.
    लागि (idx = 0; idx < argcount; ++idx)
	argvars[idx] = *थाक_TV_BOT(idx - argcount);
    argvars[argcount].v_type = VAR_UNKNOWN;

    // Result replaces the arguments on the stack.
    यदि (argcount > 0)
	ectx->ec_थाक.ga_len -= argcount - 1;
    अन्यथा यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
	फर्काऊ FAIL;
    अन्यथा
	++ectx->ec_थाक.ga_len;

    // Default return value is zero.
    tv = थाक_TV_BOT(-1);
    tv->v_type = VAR_सङ्ख्या;
    tv->vval.v_सङ्ख्या = 0;
    tv->v_lock = 0;

    फर्काऊ OK;
}

/*
 * Call a builtin function by index.
 */
    स्थिर पूर्णाङ्क
call_bfunc(पूर्णाङ्क func_idx, पूर्णाङ्क argcount, ectx_T *ectx)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    पूर्णाङ्क		idx;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    ectx_T	*prev_ectx = current_ectx;
    वर्ण	*save_func_name = ectx->ec_where.wt_func_name;

    यदि (call_prepare(argcount, argvars, ectx) == FAIL)
	फर्काऊ FAIL;

    // Check for void value being passed as an argument.
    लागि (idx = 0; idx < argcount; ++idx)
	यदि (argvars[idx].v_type == VAR_VOID)
	{
	    emsg(_(e_cannot_use_void_मान));
	    लागि (idx = 0; idx < argcount; ++idx)
		clear_tv(&argvars[idx]);
	    फर्काऊ FAIL;
	}

    ectx->ec_where.wt_func_name = internal_func_name(func_idx);

    // Call the builtin function.  Set "current_ectx" so that when it
    // recursively invokes call_def_function() a closure context can be set.
    current_ectx = ectx;
    call_internal_func_by_idx(func_idx, argvars, थाक_TV_BOT(-1));
    current_ectx = prev_ectx;
    ectx->ec_where.wt_func_name = save_func_name;

    // Clear the arguments.
    लागि (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    यदि (did_emsg > did_emsg_before)
	फर्काऊ FAIL;
    फर्काऊ OK;
}

/*
 * Execute a user defined function.
 * If the function is compiled this will add a stack frame and set the
 * instruction pointer at the start of the function.
 * Otherwise the function is called here.
 * If "pt" is not null use "pt->pt_outer" for ec_outer_ref->or_outer.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    स्थिर पूर्णाङ्क
call_ufunc(
	ufunc_T	    *ufunc,
	partial_T   *pt,
	पूर्णाङ्क	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    typval_T	argvars[MAX_FUNC_ARGS];
    funcexe_T   funcexe;
    funcerror_T	error;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		did_emsg_before = did_emsg;
    compiletype_T compile_type = get_compile_type(ufunc);

    यदि (func_needs_compiling(ufunc, compile_type)
		&& compile_def_function(ufunc, FALSE, compile_type, रिक्त)
								       == FAIL)
	फर्काऊ FAIL;
    यदि (ufunc->uf_def_status == UF_COMPILED)
    {
	error = check_user_func_argcount(ufunc, argcount);
	यदि (error != FCERR_UNKNOWN)
	{
	    यदि (error == FCERR_TOOMANY)
		semsg(_(e_too_many_arguments_for_function_str),
						   printable_func_name(ufunc));
	    अन्यथा
		semsg(_(e_not_enough_arguments_for_function_str),
						   printable_func_name(ufunc));
	    फर्काऊ FAIL;
	}

	// The function has been compiled, can call it quickly.  For a function
	// that was defined later: we can call it directly next time.
	यदि (iptr != रिक्त)
	{
	    delete_instr(iptr);
	    iptr->isn_type = ISN_DCALL;
	    iptr->isn_arg.dfunc.cdf_idx = ufunc->uf_dfunc_idx;
	    iptr->isn_arg.dfunc.cdf_argcount = argcount;
	}
	फर्काऊ call_dfunc(ufunc->uf_dfunc_idx, pt, argcount, ectx);
    }

    यदि (call_prepare(argcount, argvars, ectx) == FAIL)
	फर्काऊ FAIL;
    CLEAR_FIELD(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_selfdict = selfdict != रिक्त ? selfdict : dict_थाक_get_dict();

    // Call the user function.  Result goes in last position on the stack.
    error = call_user_func_check(ufunc, argcount, argvars,
			      थाक_TV_BOT(-1), &funcexe, funcexe.fe_selfdict);

    // Clear the arguments.
    लागि (idx = 0; idx < argcount; ++idx)
	clear_tv(&argvars[idx]);

    यदि (error != FCERR_NONE)
    {
	user_func_error(error, printable_func_name(ufunc),
							 funcexe.fe_found_var);
	फर्काऊ FAIL;
    }
    यदि (did_emsg > did_emsg_before)
	// Error other than from calling the function itself.
	फर्काऊ FAIL;
    फर्काऊ OK;
}

/*
 * If command modifiers were applied restore them.
 */
    स्थिर शून्य
may_restore_cmdmod(funclocal_T *funclocal)
{
    यदि (funclocal->floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = रिक्त;
	undo_cmdmod(&cmdmod);
	cmdmod = funclocal->floc_save_cmdmod;
	funclocal->floc_restore_cmdmod = FALSE;
    }
}

/*
 * Return TRUE if an error was given (not caught in try/catch) or CTRL-C was
 * pressed.
 */
    स्थिर पूर्णाङ्क
vim9_aborting(पूर्णाङ्क prev_uncaught_emsg)
{
    फर्काऊ uncaught_emsg > prev_uncaught_emsg || got_int || did_throw;
}

/*
 * Execute a function by "name".
 * This can be a builtin function or a user function.
 * "iptr" can be used to replace the instruction with a more efficient one.
 * Returns FAIL if not found without an error message.
 */
    स्थिर पूर्णाङ्क
call_by_name(
	char_u	    *name,
	पूर्णाङ्क	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr,
	dict_T	    *selfdict)
{
    ufunc_T *ufunc;

    यदि (builtin_function(name, -1))
    {
	पूर्णाङ्क func_idx = find_internal_func(name);

	यदि (func_idx < 0)  // Impossible?
	    फर्काऊ FAIL;
	यदि (check_internal_func(func_idx, argcount) < 0)
	    फर्काऊ FAIL;
	फर्काऊ call_bfunc(func_idx, argcount, ectx);
    }

    char_u	cc = NUL;
    char_u	*start_bracket = generic_func_find_open_bracket(name);
    यदि (start_bracket != रिक्त)
    {
	cc = *start_bracket;
	*start_bracket = NUL;
    }

    ufunc = find_func(name, FALSE);

    यदि (ufunc == रिक्त)
    {
	पूर्णाङ्क prev_uncaught_emsg = uncaught_emsg;

	यदि (script_autoload(name, TRUE))
	    // loaded a package, search for the function again
	    ufunc = find_func(name, FALSE);

	यदि (vim9_aborting(prev_uncaught_emsg))
	{
	    यदि (start_bracket != रिक्त)
		*start_bracket = cc;
	    फर्काऊ FAIL;  // bail out if loading the script caused an error
	}
    }

    यदि (start_bracket != रिक्त)
	*start_bracket = cc;

    यदि (ufunc != रिक्त)
    {
	यदि (IS_GENERIC_FUNC(ufunc))
	{
	    यदि (start_bracket != रिक्त)
		ufunc = find_generic_func(ufunc, name, &start_bracket);
	    अन्यथा
	    {
		emsg_funcname(e_generic_func_missing_type_args_str, name);
		ufunc = रिक्त;
	    }
	    यदि (ufunc == रिक्त)
		फर्काऊ FAIL;
	}
	अन्यथा यदि (start_bracket != रिक्त)
	{
	    emsg_funcname(e_not_a_generic_function_str, name);
	    फर्काऊ FAIL;
	}

	यदि (check_ufunc_arg_types(ufunc, argcount, 0, ectx) == FAIL)
	    फर्काऊ FAIL;

	फर्काऊ call_ufunc(ufunc, रिक्त, argcount, ectx, iptr, selfdict);
    }

    फर्काऊ FAIL;
}

    स्थिर पूर्णाङ्क
call_partial(
	typval_T    *tv,
	पूर्णाङ्क	    argcount_arg,
	ectx_T	    *ectx)
{
    पूर्णाङ्क		argcount = argcount_arg;
    char_u	*name = रिक्त;
    पूर्णाङ्क		called_emsg_before = called_emsg;
    पूर्णाङ्क		res = FAIL;
    dict_T	*selfdict = रिक्त;

    यदि (tv->v_type == VAR_PARTIAL)
    {
	partial_T   *pt = tv->vval.v_partial;
	पूर्णाङ्क	    i;

	यदि (pt->pt_obj != रिक्त)
	{
	    // partial with an object method.  Push the object before the
	    // function arguments.
	    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		फर्काऊ FAIL;
	    लागि (i = 1; i <= argcount; ++i)
		*थाक_TV_BOT(-i + 1) = *थाक_TV_BOT(-i);

	    typval_T *obj_tv = थाक_TV_BOT(-argcount);
	    obj_tv->v_type = VAR_OBJECT;
	    obj_tv->v_lock = 0;
	    obj_tv->vval.v_object = pt->pt_obj;
	    ++pt->pt_obj->obj_refcount;
	    ++ectx->ec_थाक.ga_len;
	}

	यदि (pt->pt_argc > 0)
	{
	    // Make space for arguments from the partial, shift the "argcount"
	    // arguments up.
	    यदि (GA_GROW_FAILS(&ectx->ec_थाक, pt->pt_argc))
		फर्काऊ FAIL;
	    लागि (i = 1; i <= argcount; ++i)
		*थाक_TV_BOT(-i + pt->pt_argc) = *थाक_TV_BOT(-i);
	    ectx->ec_थाक.ga_len += pt->pt_argc;
	    argcount += pt->pt_argc;

	    // copy the arguments from the partial onto the stack
	    लागि (i = 0; i < pt->pt_argc; ++i)
		copy_tv(&pt->pt_argv[i], थाक_TV_BOT(-argcount + i));
	}
	selfdict = pt->pt_dict;

	यदि (pt->pt_func != रिक्त)
	    फर्काऊ call_ufunc(pt->pt_func, pt, argcount, ectx, रिक्त, selfdict);

	name = pt->pt_name;
    }
    अन्यथा यदि (tv->v_type == VAR_FUNC)
	name = tv->vval.v_string;
    यदि (name != रिक्त)
    {
	char_u	    fname_buf[FLEN_FIXED + 1];
	char_u	    *tofree = रिक्त;
	funcerror_T error = FCERR_NONE;
	char_u	    *fname;

	// May need to translate <SNR>123_ to K_SNR.
	fname = fname_trans_sid(name, fname_buf, &tofree, &error);
	यदि (error != FCERR_NONE)
	    res = FAIL;
	अन्यथा
	    res = call_by_name(fname, argcount, ectx, रिक्त, selfdict);
	vim_free(tofree);
    }

    यदि (res == FAIL)
    {
	यदि (called_emsg == called_emsg_before)
	    emsg_funcname(e_unknown_function_str,
				  name == रिक्त ? (char_u *)"[unknown]" : name);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Check if "lock" is VAR_LOCKED or VAR_FIXED.  If so give an error and return
 * TRUE.
 */
    स्थिर पूर्णाङ्क
error_if_locked(पूर्णाङ्क lock, वर्ण *error)
{
    यदि (lock & (VAR_LOCKED | VAR_FIXED))
    {
	emsg(_(error));
	फर्काऊ TRUE;
    }
    फर्काऊ FALSE;
}

/*
 * Give an error if "tv" is not a number and return FAIL.
 */
    स्थिर पूर्णाङ्क
check_for_सङ्ख्या(typval_T *tv)
{
    यदि (tv->v_type != VAR_सङ्ख्या)
    {
	semsg(_(e_expected_str_but_got_str),
		vartype_name(VAR_सङ्ख्या), vartype_name(tv->v_type));
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Store "tv" in variable "name".
 * This is for s: and g: variables.
 */
    स्थिर शून्य
store_var(char_u *name, typval_T *tv)
{
    funccal_entry_T entry;
    पूर्णाङ्क		    flags = ASSIGN_DECL;

    यदि (tv->v_lock)
	flags |= ASSIGN_CONST;
    save_funccal(&entry);
    set_var_const(name, 0, रिक्त, tv, FALSE, flags, 0);
    restore_funccal();
}

/*
 * Convert "tv" to a string.
 * Return FAIL if not allowed.
 */
    स्थिर पूर्णाङ्क
do_2string(typval_T *tv, पूर्णाङ्क is_2string_any, पूर्णाङ्क tostring_flags)
{
    यदि (tv->v_type == VAR_STRING)
	फर्काऊ OK;

    char_u *str;

    यदि (is_2string_any)
    {
	विकल्प (tv->v_type)
	{
	    पक्ष VAR_SPECIAL:
	    पक्ष VAR_BOOL:
	    पक्ष VAR_सङ्ख्या:
	    पक्ष VAR_FLOAT:
	    पक्ष VAR_DICT:
	    पक्ष VAR_BLOB:	रोक;

	    पक्ष VAR_LIST:
				यदि (tostring_flags & TOSTRING_TOLERANT)
				{
				    char_u	*s, *e, *p;
				    garray_T	ga;

				    ga_init2(&ga, परिमाण(char_u *), 1);

				    // Convert to NL separated items, then
				    // escape the items and replace the NL with
				    // a space.
				    str = typval2string(tv, TRUE);
				    यदि (str == रिक्त)
					फर्काऊ FAIL;
				    s = str;
				    जबसम्म ((e = vim_strchr(s, '\n')) != रिक्त)
				    {
					*e = NUL;
					p = vim_strsave_fnameescape(s,
						VSE_NONE);
					यदि (p != रिक्त)
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
				    फर्काऊ OK;
				}
				यदि (tostring_flags & TOSTRING_INTERPOLATE)
				    रोक;
				// FALLTHROUGH
	    स्वभाव:	to_string_error(tv->v_type);
			फर्काऊ FAIL;
	}
    }
    str = typval_tostring(tv, TRUE);
    clear_tv(tv);
    tv->v_type = VAR_STRING;
    tv->vval.v_string = str;
    फर्काऊ OK;
}

/*
 * When the value of "sv" is a null list of dict, allocate it.
 */
    स्थिर शून्य
allocate_if_null(svar_T *sv)
{
    typval_T *tv = sv->sv_tv;

    विकल्प (tv->v_type)
    {
	पक्ष VAR_LIST:
	    यदि (tv->vval.v_list == रिक्त && sv->sv_type != &t_list_empty)
		(शून्य)rettv_list_alloc(tv);
	    रोक;
	पक्ष VAR_TUPLE:
	    यदि (tv->vval.v_tuple == रिक्त && sv->sv_type != &t_tuple_empty)
		(शून्य)rettv_tuple_alloc(tv);
	    रोक;
	पक्ष VAR_DICT:
	    यदि (tv->vval.v_dict == रिक्त && sv->sv_type != &t_dict_empty)
		(शून्य)rettv_dict_alloc(tv);
	    रोक;
	पक्ष VAR_BLOB:
	    यदि (tv->vval.v_blob == रिक्त && sv->sv_type != &t_blob_null)
		(शून्य)rettv_blob_alloc(tv);
	    रोक;
	स्वभाव:
	    रोक;
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
    माप_प्रकार	    nbyte = 0;
    varnumber_T	    nchar = index;
    माप_प्रकार	    slen;

    यदि (str == रिक्त)
	फर्काऊ रिक्त;
    slen = STRLEN(str);

    // Do the same as for a list: a negative index counts from the end.
    // Optimization to check the first byte to be below 0x80 (and no composing
    // character follows) makes this a lot faster.
    यदि (index < 0)
    {
	पूर्णाङ्क	clen = 0;

	लागि (nbyte = 0; nbyte < slen; ++clen)
	{
	    यदि (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
		++nbyte;
	    अन्यथा यदि (enc_utf8)
		nbyte += utfc_ptr2len(str + nbyte);
	    अन्यथा
		nbyte += mb_ptr2len(str + nbyte);
	}
	nchar = clen + index;
	यदि (nchar < 0)
	    // unlike list: index out of range results in empty string
	    फर्काऊ रिक्त;
    }

    लागि (nbyte = 0; nchar > 0 && nbyte < slen; --nchar)
    {
	यदि (str[nbyte] < 0x80 && str[nbyte + 1] < 0x80)
	    ++nbyte;
	अन्यथा यदि (enc_utf8)
	    nbyte += utfc_ptr2len(str + nbyte);
	अन्यथा
	    nbyte += mb_ptr2len(str + nbyte);
    }
    यदि (nbyte >= slen)
	फर्काऊ रिक्त;
    फर्काऊ vim_strnsave(str + nbyte, mb_ptr2len(str + nbyte));
}

/*
 * Get the byte index for character index "idx" in string "str" with length
 * "str_len".  Composing characters are included.
 * If going over the end return "str_len".
 * If "idx" is negative count from the end, -1 is the last character.
 * When going over the start return -1.
 */
    स्थिर दीर्घ
char_idx2byte(char_u *str, माप_प्रकार str_len, varnumber_T idx)
{
    varnumber_T nchar = idx;
    माप_प्रकार	nbyte = 0;

    यदि (nchar >= 0)
    {
	जबसम्म (nchar > 0 && nbyte < str_len)
	{
	    nbyte += mb_ptr2len(str + nbyte);
	    --nchar;
	}
    }
    अन्यथा
    {
	nbyte = str_len;
	जबसम्म (nchar < 0 && nbyte > 0)
	{
	    --nbyte;
	    nbyte -= mb_head_off(str, str + nbyte);
	    ++nchar;
	}
	यदि (nchar < 0)
	    फर्काऊ -1;
    }
    फर्काऊ (दीर्घ)nbyte;
}

/*
 * Return the slice "str[first : last]" using character indexes.  Composing
 * characters are included.
 * "exclusive" is TRUE for slice().
 * Return NULL when the result is empty.
 */
    char_u *
string_slice(char_u *str, varnumber_T first, varnumber_T last, पूर्णाङ्क exclusive)
{
    दीर्घ	start_byte, end_byte;
    माप_प्रकार	slen;

    यदि (str == रिक्त)
	फर्काऊ रिक्त;
    slen = STRLEN(str);
    start_byte = char_idx2byte(str, slen, first);
    यदि (start_byte < 0)
	start_byte = 0; // first index very negative: use zero
    यदि ((last == -1 && !exclusive) || last == VARNUM_MAX)
	end_byte = (दीर्घ)slen;
    अन्यथा
    {
	end_byte = char_idx2byte(str, slen, last);
	यदि (!exclusive && end_byte >= 0 && end_byte < (दीर्घ)slen)
	    // end index is inclusive
	    end_byte += mb_ptr2len(str + end_byte);
    }

    यदि (start_byte >= (दीर्घ)slen || end_byte <= start_byte)
	फर्काऊ रिक्त;
    फर्काऊ vim_strnsave(str + start_byte, end_byte - start_byte);
}

/*
 * Get a script variable for ISN_STORESCRIPT and ISN_LOADSCRIPT.
 * When "dfunc_idx" is negative don't give an error.
 * Returns NULL for an error.
 */
    स्थिर svar_T *
get_script_svar(scriptref_T *sref, पूर्णाङ्क dfunc_idx)
{
    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
    dfunc_T	    *dfunc = dfunc_idx < 0 ? रिक्त
			      : ((dfunc_T *)def_functions.ga_data) + dfunc_idx;
    svar_T	    *sv;

    यदि (sref->sref_seq != si->sn_script_seq)
    {
	// The script was reloaded after the function was compiled, the
	// script_idx may not be valid.
	यदि (dfunc != रिक्त)
	    semsg(_(e_script_variable_invalid_after_reload_in_function_str),
					 printable_func_name(dfunc->df_ufunc));
	फर्काऊ रिक्त;
    }
    sv = ((svar_T *)si->sn_var_vals.ga_data) + sref->sref_idx;
    यदि (sv->sv_name == रिक्त)
    {
	यदि (dfunc != रिक्त)
	    emsg(_(e_script_variable_was_deleted));
	फर्काऊ रिक्त;
    }
    यदि (!equal_type(sv->sv_type, sref->sref_type, 0))
    {
	यदि (dfunc != रिक्त)
	    emsg(_(e_script_variable_type_changed));
	फर्काऊ रिक्त;
    }

    यदि ((sv->sv_flags & SVFLAG_EXPORTED) == 0
				      && sref->sref_sid != current_sctx.sc_sid)
    {
	यदि (dfunc != रिक्त)
	    semsg(_(e_item_not_exported_in_script_str), sv->sv_name);
	फर्काऊ रिक्त;
    }
    फर्काऊ sv;
}

/*
 * Function passed to do_cmdline() for splitting a script joined by NL
 * characters.
 */
    स्थिर char_u *
get_split_sourceline(
	पूर्णाङ्क c UNUSED,
	शून्य *cookie,
	पूर्णाङ्क indent UNUSED,
	getline_opt_T options UNUSED)
{
    source_cookie_T	*sp = (source_cookie_T *)cookie;
    char_u		*p;
    char_u		*पङ्क्ति;

    p = vim_strchr(sp->nextline, '\n');
    यदि (p == रिक्त)
    {
	पङ्क्ति = vim_strsave(sp->nextline);
	sp->nextline += STRLEN(sp->nextline);
    }
    अन्यथा
    {
	पङ्क्ति = vim_strnsave(sp->nextline, p - sp->nextline);
	sp->nextline = p + 1;
    }
    फर्काऊ पङ्क्ति;
}

/*
 * Execute a function by "name".
 * This can be a builtin function, user function or a funcref.
 * "iptr" can be used to replace the instruction with a more efficient one.
 */
    स्थिर पूर्णाङ्क
call_eval_func(
	char_u	    *name,
	पूर्णाङ्क	    argcount,
	ectx_T	    *ectx,
	isn_T	    *iptr)
{
    पूर्णाङ्क	    called_emsg_before = called_emsg;
    पूर्णाङ्क	    res;

    res = call_by_name(name, argcount, ectx, iptr, रिक्त);
    यदि (res == FAIL && called_emsg == called_emsg_before)
    {
	dictitem_T	*v;

	v = find_var(name, रिक्त, FALSE);
	यदि (v == रिक्त || (v->di_tv.v_type != VAR_PARTIAL
					       && v->di_tv.v_type != VAR_FUNC))
	{
	    emsg_funcname(e_unknown_function_str, name);
	    फर्काऊ FAIL;
	}
	फर्काऊ call_partial(&v->di_tv, argcount, ectx);
    }
    फर्काऊ res;
}

/*
 * When a function reference is used, fill a partial with the information
 * needed, especially when it is used as a closure.
 */
    पूर्णाङ्क
fill_partial_and_closure(
	partial_T	*pt,
	ufunc_T		*ufunc,
	loopvarinfo_T	*lvi,
	ectx_T		*ectx)
{
    pt->pt_func = ufunc;
    pt->pt_refcount = 1;

    यदि (ufunc->uf_flags & FC_CLOSURE)
    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							  + ectx->ec_dfunc_idx;

	// The closure may need to find arguments and local variables of the
	// current function in the stack.
	pt->pt_outer.out_थाक = &ectx->ec_थाक;
	pt->pt_outer.out_frame_idx = ectx->ec_frame_idx;
	यदि (ectx->ec_outer_ref != रिक्त)
	{
	    // The current context already has a context, link to that one.
	    pt->pt_outer.out_up = ectx->ec_outer_ref->or_outer;
	    यदि (ectx->ec_outer_ref->or_partial != रिक्त)
	    {
		pt->pt_outer.out_up_partial = ectx->ec_outer_ref->or_partial;
		++pt->pt_outer.out_up_partial->pt_refcount;
	    }
	}

	यदि (lvi != रिक्त)
	{
	    पूर्णाङ्क	depth;

	    // The closure may need to find variables defined inside a loop,
	    // for every nested loop.  A new reference is made every time,
	    // ISN_ENDLOOP will check if they are actually used.
	    लागि (depth = 0; depth < lvi->lvi_depth; ++depth)
	    {
		pt->pt_outer.out_loop[depth].थाक = &ectx->ec_थाक;
		pt->pt_outer.out_loop[depth].var_idx = ectx->ec_frame_idx
			 + थाक_FRAME_SIZE + lvi->lvi_loop[depth].var_idx;
		pt->pt_outer.out_loop[depth].var_गन्ती =
					    lvi->lvi_loop[depth].var_गन्ती;
	    }
	    pt->pt_outer.out_loop_size = lvi->lvi_depth;
	}
	अन्यथा
	    pt->pt_outer.out_loop_size = 0;

	// If the function currently executing returns and the closure is still
	// being referenced, we need to make a copy of the context (arguments
	// and local variables) so that the closure can use it later.
	// Store a reference to the partial so we can handle that.
	यदि (GA_GROW_FAILS(&ectx->ec_funcrefs, 1))
	    // caller needs to free pt
	    फर्काऊ FAIL;
	// Extra variable keeps the count of closures created in the current
	// function call.
	++(((typval_T *)ectx->ec_थाक.ga_data) + ectx->ec_frame_idx
		       + थाक_FRAME_SIZE + dfunc->df_varcount)->vval.v_सङ्ख्या;

	((partial_T **)ectx->ec_funcrefs.ga_data)[ectx->ec_funcrefs.ga_len]
									  = pt;
	++pt->pt_refcount;
	++ectx->ec_funcrefs.ga_len;
    }
    ++ufunc->uf_refcount;
    फर्काऊ OK;
}

/*
 * Execute iptr->isn_arg.string as an Ex command.
 */
    स्थिर पूर्णाङ्क
exec_command(isn_T *iptr, char_u *cmd_string)
{
    source_cookie_T cookie;

    SOURCING_LNUM = iptr->isn_lnum;
    // Pass getsourceline to get an error for a missing ":end" command.
    CLEAR_FIELD(cookie);
    cookie.sourcing_lnum = iptr->isn_lnum - 1;
    यदि (do_cmdline(cmd_string, getsourceline, &cookie,
			     DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED) == FAIL
		|| did_emsg)
	फर्काऊ FAIL;
    फर्काऊ OK;
}

/*
 * If script "sid" is not loaded yet then load it now.
 * Caller must make sure "sid" is a valid script ID.
 * "loaded" is set to TRUE if the script had to be loaded.
 * Returns FAIL if loading fails, OK if already loaded or loaded now.
 */
    पूर्णाङ्क
may_load_script(पूर्णाङ्क sid, पूर्णाङ्क *loaded)
{
    scriptitem_T *si = SCRIPT_ITEM(sid);

    यदि (si->sn_state == SN_STATE_NOT_LOADED)
    {
	*loaded = TRUE;
	यदि (do_source(si->sn_name, FALSE, DOSO_NONE, रिक्त) == FAIL)
	{
	    semsg(_(e_cant_open_file_str), si->sn_name);
	    फर्काऊ FAIL;
	}
    }
    फर्काऊ OK;
}

// used for v_instr of typval of VAR_INSTR
संरचना instr_S {
    ectx_T	*instr_ectx;
    isn_T	*instr_instr;
};

// used for substitute_instr
प्रकारनिर्देश संरचना subs_अभिव्यक्ति_S {
    ectx_T	*subs_ectx;
    isn_T	*subs_instr;
    पूर्णाङ्क		subs_status;
} subs_अभिव्यक्ति_T;

// Set when calling do_debug().
स्थिर ectx_T	*debug_context = रिक्त;
स्थिर पूर्णाङ्क	debug_var_गन्ती;

/*
 * When debugging lookup "name" and return the typeval.
 * When not found return NULL.
 */
    typval_T *
lookup_debug_var(char_u *name)
{
    पूर्णाङ्क		    idx;
    dfunc_T	    *dfunc;
    ufunc_T	    *ufunc;
    ectx_T	    *ectx = debug_context;
    पूर्णाङ्क		    varargs_off;

    यदि (ectx == रिक्त)
	फर्काऊ रिक्त;
    dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    // Go through the local variable names, from last to first.
    लागि (idx = debug_var_गन्ती - 1; idx >= 0; --idx)
    {
	char_u *varname = ((char_u **)dfunc->df_var_names.ga_data)[idx];

	// the variable name may be NULL when not available in this block
	यदि (varname != रिक्त && STRCMP(varname, name) == 0)
	    फर्काऊ थाक_TV_VAR(idx);
    }

    // Go through argument names.
    ufunc = dfunc->df_ufunc;
    varargs_off = ufunc->uf_va_name == रिक्त ? 0 : 1;
    लागि (idx = 0; idx < ufunc->uf_args.ga_len; ++idx)
	यदि (STRCMP(((char_u **)(ufunc->uf_args.ga_data))[idx], name) == 0)
	    फर्काऊ थाक_TV(ectx->ec_frame_idx - ufunc->uf_args.ga_len
							  - varargs_off + idx);
    यदि (ufunc->uf_va_name != रिक्त && STRCMP(ufunc->uf_va_name, name) == 0)
	फर्काऊ थाक_TV(ectx->ec_frame_idx - 1);

    फर्काऊ रिक्त;
}

/*
 * Return TRUE if there might be a breakpoint in "ufunc", which is when a
 * breakpoint was set in that function or when there is any expression.
 */
    पूर्णाङ्क
may_break_in_function(ufunc_T *ufunc)
{
    फर्काऊ ufunc->uf_has_breakpoint || debug_has_अभिव्यक्ति_breakpoint();
}

    स्थिर शून्य
handle_debug(isn_T *iptr, ectx_T *ectx)
{
    char_u	*पङ्क्ति;
    ufunc_T	*ufunc = (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;
    isn_T	*ni;
    पूर्णाङ्क		end_lnum = iptr->isn_lnum;
    garray_T	ga;
    पूर्णाङ्क		lnum;

    यदि (ex_nesting_level > debug_break_level)
    {
	linenr_T breakpoint;

	यदि (!may_break_in_function(ufunc))
	    फर्काऊ;

	// check for the next breakpoint if needed
	breakpoint = dbg_find_breakpoint(FALSE, ufunc->uf_name,
					   iptr->isn_arg.debug.dbg_break_lnum);
	यदि (breakpoint <= 0 || breakpoint > iptr->isn_lnum)
	    फर्काऊ;
    }

    SOURCING_LNUM = iptr->isn_lnum;
    debug_context = ectx;
    debug_var_गन्ती = iptr->isn_arg.debug.dbg_var_names_len;

    लागि (ni = iptr + 1; ni->isn_type != ISN_FINISH; ++ni)
	यदि (ni->isn_type == ISN_DEBUG
		  || ni->isn_type == ISN_RETURN
		  || ni->isn_type == ISN_RETURN_OBJECT
		  || ni->isn_type == ISN_RETURN_VOID)
	{
	    end_lnum = ni->isn_lnum + (ni->isn_type == ISN_DEBUG ? 0 : 1);
	    रोक;
	}

    यदि (end_lnum > iptr->isn_lnum)
    {
	ga_init2(&ga, परिमाण(char_u *), 10);
	लागि (lnum = iptr->isn_lnum; lnum < end_lnum
				     && lnum <= ufunc->uf_पङ्क्तिहरू.ga_len; ++lnum)
	{
	    char_u *p = ((char_u **)ufunc->uf_पङ्क्तिहरू.ga_data)[lnum - 1];

	    यदि (p == रिक्त)
		जारी;  // left over from continuation line
	    p = skipwhite(p);
	    यदि (*p == '#')
		रोक;
	    यदि (GA_GROW_OK(&ga, 1))
		((char_u **)(ga.ga_data))[ga.ga_len++] = p;
	    यदि (STRNCMP(p, "def ", 4) == 0)
		रोक;
	}
	पङ्क्ति = ga_concat_strings(&ga, "  ");
	vim_free(ga.ga_data);
    }
    अन्यथा
	पङ्क्ति = ((char_u **)ufunc->uf_पङ्क्तिहरू.ga_data)[iptr->isn_lnum - 1];

    do_debug(पङ्क्ति == रिक्त ? (char_u *)"[empty]" : पङ्क्ति);
    debug_context = रिक्त;

    यदि (end_lnum > iptr->isn_lnum)
	vim_free(पङ्क्ति);
}

/*
 * Do a runtime check of the RHS value against the LHS List member type.
 * This is used by the STOREINDEX instruction to perform a type check
 * at runtime if compile time type check cannot be performed (VAR_ANY).
 * Returns FAIL if there is a type mismatch.
 */
    स्थिर पूर्णाङ्क
storeindex_check_list_member_type(
    list_T	*lhs_list,
    typval_T	*rhs_tv,
    ectx_T	*ectx)
{
    यदि (lhs_list->lv_type == रिक्त || lhs_list->lv_type->tt_member == रिक्त)
	फर्काऊ OK;

    फर्काऊ check_typval_type(lhs_list->lv_type->tt_member, rhs_tv,
			     ectx->ec_where);
}

/*
 * Store a value in a list, dict, blob or object variable.
 * Returns OK, FAIL or NOTDONE (uncatchable error).
 */
    स्थिर पूर्णाङ्क
execute_storeindex(isn_T *iptr, ectx_T *ectx)
{
    vartype_T	dest_type = iptr->isn_arg.storeindex.si_vartype;
    typval_T	*tv;
    typval_T	*tv_idx = थाक_TV_BOT(-2);
    दीर्घ	lidx = 0;
    typval_T	*tv_dest = थाक_TV_BOT(-1);
    पूर्णाङ्क		status = OK;
    पूर्णाङ्क		check_rhs_type = FALSE;

    यदि (tv_idx->v_type == VAR_सङ्ख्या)
	lidx = (दीर्घ)tv_idx->vval.v_सङ्ख्या;

    // Stack contains:
    // -3 value to be stored
    // -2 index
    // -1 dict, list, blob, object or class
    tv = थाक_TV_BOT(-3);
    SOURCING_LNUM = iptr->isn_lnum;

    // Make sure an object has been initialized
    यदि (dest_type == VAR_OBJECT && tv_dest->vval.v_object == रिक्त)
    {
	emsg(_(e_using_null_object));
	status = FAIL;
    }
    अन्यथा यदि (dest_type == VAR_ANY)
    {
	check_rhs_type = TRUE;
	dest_type = tv_dest->v_type;
	यदि (dest_type == VAR_DICT)
	    status = do_2string(tv_idx, TRUE, FALSE);
	अन्यथा यदि (dest_type == VAR_OBJECT && tv_idx->v_type == VAR_STRING)
	{
	    // Need to get the member index now that the class is known.
	    object_T *obj = tv_dest->vval.v_object;
	    यदि (obj == रिक्त)
	    {
		emsg(_(e_using_null_object));
		status = FAIL;
	    }
	    अन्यथा
	    {
		class_T *cl = obj->obj_class;
		char_u  *member = tv_idx->vval.v_string;

		पूर्णाङ्क		m_idx;
		ocmember_T *m = object_member_lookup(cl, member, 0, &m_idx);
		यदि (m != रिक्त)
		{
		    // Get the current function
		    ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
		    where_T where = WHERE_INIT;

		    // Check whether the member variable is writeable
		    यदि ((m->ocm_access != VIM_ACCESS_ALL) &&
			    (ufunc->uf_class == रिक्त ||
			     !class_instance_of(ufunc->uf_class, cl)))
		    {
			वर्ण *सन्देश = (m->ocm_access == VIM_ACCESS_PRIVATE)
			    ? e_cannot_access_protected_variable_str
			    : e_variable_is_not_writable_str;
			emsg_var_cl_define(सन्देश, m->ocm_name.string, 0, cl);
			status = FAIL;
		    }
		    // Fail if the variable is a const or final or the type
		    // is not compatible
		    अन्यथा यदि (oc_var_check_ro(cl, m) ||
			     check_typval_type(m->ocm_type, tv, where)
								== FAIL)
			status = FAIL;
		    अन्यथा
			lidx = m_idx;
		}
		अन्यथा
		{
		    member_not_found_सन्देश(cl, VAR_OBJECT, member, 0);
		    status = FAIL;
		}
	    }
	}
	अन्यथा यदि (dest_type == VAR_OBJECT)
	{
	    emsg(_(e_string_required));
	    status = FAIL;
	}
	अन्यथा यदि (dest_type == VAR_LIST && tv_idx->v_type != VAR_सङ्ख्या)
	{
	    emsg(_(e_सङ्ख्या_expected));
	    status = FAIL;
	}
    }

    यदि (status == OK)
    {
	यदि (dest_type == VAR_LIST)
	{
	    list_T	    *list = tv_dest->vval.v_list;

	    यदि (list == रिक्त)
	    {
		emsg(_(e_list_not_set));
		फर्काऊ FAIL;
	    }
	    यदि (lidx < 0 && list->lv_len + lidx >= 0)
		// negative index is relative to the end
		lidx = list->lv_len + lidx;
	    यदि (lidx < 0 || lidx > list->lv_len)
	    {
		semsg(_(e_list_index_out_of_range_nr), lidx);
		फर्काऊ FAIL;
	    }

	    // Do a runtime type check for VAR_ANY
	    यदि (check_rhs_type &&
		    storeindex_check_list_member_type(list, tv, ectx) == FAIL)
		फर्काऊ FAIL;

	    यदि (lidx < list->lv_len)
	    {
		listitem_T *li = list_find(list, lidx);
		यदि (li == रिक्त)
		    फर्काऊ FAIL;
		यदि (error_if_locked(li->li_tv.v_lock,
					     e_cannot_change_locked_list_item))
		    फर्काऊ FAIL;
		// overwrite existing list item
		clear_tv(&li->li_tv);
		li->li_tv = *tv;
	    }
	    अन्यथा
	    {
		यदि (error_if_locked(list->lv_lock, e_cannot_change_locked_list))
		    फर्काऊ FAIL;
		// append to list, only fails when out of memory
		यदि (list_append_tv(list, tv) == FAIL)
		    फर्काऊ NOTDONE;
		clear_tv(tv);
	    }
	}
	अन्यथा यदि (dest_type == VAR_DICT)
	{
	    char_u		*साँचो = tv_idx->vval.v_string;
	    dict_T		*dict = tv_dest->vval.v_dict;
	    dictitem_T	*di;

	    SOURCING_LNUM = iptr->isn_lnum;
	    यदि (dict == रिक्त)
	    {
		emsg(_(e_dictionary_not_set));
		फर्काऊ FAIL;
	    }
	    यदि (साँचो == रिक्त)
		साँचो = (char_u *)"";
	    di = dict_find(dict, साँचो, -1);
	    यदि (di != रिक्त)
	    {
		यदि (error_if_locked(di->di_tv.v_lock,
						    e_cannot_change_dict_item))
		    फर्काऊ FAIL;
		// overwrite existing value
		clear_tv(&di->di_tv);
		di->di_tv = *tv;
	    }
	    अन्यथा
	    {
		यदि (error_if_locked(dict->dv_lock, e_cannot_change_dict))
		    फर्काऊ FAIL;
		// add to dict, only fails when out of memory
		यदि (dict_add_tv(dict, (वर्ण *)साँचो, tv) == FAIL)
		    फर्काऊ NOTDONE;
		clear_tv(tv);
	    }
	}
	अन्यथा यदि (dest_type == VAR_BLOB)
	{
	    blob_T	    *blob = tv_dest->vval.v_blob;
	    varnumber_T	    nr;
	    पूर्णाङ्क		    error = FALSE;
	    पूर्णाङ्क		    len;

	    यदि (blob == रिक्त)
	    {
		emsg(_(e_blob_not_set));
		फर्काऊ FAIL;
	    }
	    len = blob_len(blob);
	    यदि (lidx < 0 && len + lidx >= 0)
		// negative index is relative to the end
		lidx = len + lidx;

	    // Can add one byte at the end.
	    यदि (lidx < 0 || lidx > len)
	    {
		semsg(_(e_blob_index_out_of_range_nr), lidx);
		फर्काऊ FAIL;
	    }
	    यदि (मान_check_lock(blob->bv_lock, (char_u *)"blob", FALSE))
		फर्काऊ FAIL;
	    nr = tv_get_सङ्ख्या_chk(tv, &error);
	    यदि (error)
		फर्काऊ FAIL;
	    यदि (nr < 0 || nr > 255)
	    {
		semsg(_(e_invalid_मान_for_blob_nr), nr);
		फर्काऊ FAIL;
	    }
	    blob_set_append(blob, lidx, nr);
	}
	अन्यथा यदि (dest_type == VAR_CLASS || dest_type == VAR_OBJECT)
	{
	    typval_T	    *otv;

	    यदि (dest_type == VAR_OBJECT)
	    {
		object_T	*obj = tv_dest->vval.v_object;

		otv = (typval_T *)(obj + 1);
		class_T	    *itf = iptr->isn_arg.storeindex.si_class;
		यदि (itf != रिक्त)
		    // convert interface member index to class member index
		    lidx = object_index_from_itf_index(itf, FALSE, lidx,
						       obj->obj_class);
	    }
	    अन्यथा
	    {
		// VAR_CLASS
		class_T	*class = tv_dest->vval.v_class;
		otv = class->class_members_tv;
	    }

	    clear_tv(&otv[lidx]);
	    otv[lidx] = *tv;
	}
	अन्यथा यदि (dest_type == VAR_TUPLE)
	{
	    emsg(_(e_tuple_is_immutable));
	    status = FAIL;
	}
	अन्यथा
	{
	    status = FAIL;
	    semsg(_(e_cannot_index_str), vartype_name(dest_type));
	}
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_थाक.ga_len -= 3;
    यदि (status == FAIL)
    {
	clear_tv(tv);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

/*
 * Store a value in a list or blob range.
 */
    स्थिर पूर्णाङ्क
execute_storerange(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    typval_T	*tv_idx1 = थाक_TV_BOT(-3);
    typval_T	*tv_idx2 = थाक_TV_BOT(-2);
    typval_T	*tv_dest = थाक_TV_BOT(-1);
    पूर्णाङ्क		status = OK;

    // Stack contains:
    // -4 value to be stored
    // -3 first index or "none"
    // -2 second index or "none"
    // -1 destination list or blob
    tv = थाक_TV_BOT(-4);
    SOURCING_LNUM = iptr->isn_lnum;
    यदि (tv_dest->v_type == VAR_LIST)
    {
	दीर्घ	    n1;
	दीर्घ	    n2;
	listitem_T  *li1;

	n1 = (दीर्घ)tv_get_सङ्ख्या_chk(tv_idx1, रिक्त);
	यदि (tv_idx2->v_type == VAR_SPECIAL
		    && tv_idx2->vval.v_सङ्ख्या == VVAL_NONE)
	    n2 = list_len(tv_dest->vval.v_list) - 1;
	अन्यथा
	    n2 = (दीर्घ)tv_get_सङ्ख्या_chk(tv_idx2, रिक्त);

	li1 = check_range_index_one(tv_dest->vval.v_list, &n1, TRUE, FALSE);
	यदि (li1 == रिक्त)
	    status = FAIL;
	अन्यथा
	{
	    status = check_range_index_two(tv_dest->vval.v_list,
							 &n1, li1, &n2, FALSE);
	    यदि (status != FAIL)
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
    अन्यथा यदि (tv_dest->v_type == VAR_BLOB)
    {
	varnumber_T n1;
	varnumber_T n2;
	दीर्घ	    bloblen;

	n1 = tv_get_सङ्ख्या_chk(tv_idx1, रिक्त);
	यदि (tv_idx2->v_type == VAR_SPECIAL
					&& tv_idx2->vval.v_सङ्ख्या == VVAL_NONE)
	    n2 = blob_len(tv_dest->vval.v_blob) - 1;
	अन्यथा
	    n2 = tv_get_सङ्ख्या_chk(tv_idx2, रिक्त);
	bloblen = blob_len(tv_dest->vval.v_blob);

	यदि (check_blob_index(bloblen, n1, FALSE) == FAIL
		|| check_blob_range(bloblen, n1, n2, FALSE) == FAIL)
	    status = FAIL;
	अन्यथा
	    status = blob_set_range(tv_dest->vval.v_blob, n1, n2, tv);
    }
    अन्यथा
    {
	status = FAIL;
	emsg(_(e_list_or_blob_required));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_थाक.ga_len -= 4;
    clear_tv(tv);

    फर्काऊ status;
}

/*
 * Unlet item in list or dict variable.
 */
    स्थिर पूर्णाङ्क
execute_unletindex(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv_idx = थाक_TV_BOT(-2);
    typval_T	*tv_dest = थाक_TV_BOT(-1);
    पूर्णाङ्क		status = OK;

    // Stack contains:
    // -2 index
    // -1 dict or list
    SOURCING_LNUM = iptr->isn_lnum;
    यदि (tv_dest->v_type == VAR_DICT)
    {
	// unlet a dict item, index must be a string
	यदि (tv_idx->v_type != VAR_STRING && tv_idx->v_type != VAR_सङ्ख्या)
	{
	    semsg(_(e_expected_str_but_got_str),
			vartype_name(VAR_STRING),
			vartype_name(tv_idx->v_type));
	    status = FAIL;
	}
	अन्यथा
	{
	    dict_T	*d = tv_dest->vval.v_dict;
	    char_u	*साँचो;
	    dictitem_T  *di = रिक्त;

	    यदि (d != रिक्त && मान_check_lock(
				      d->dv_lock, रिक्त, FALSE))
		status = FAIL;
	    अन्यथा
	    {
		यदि (tv_idx->v_type == VAR_STRING)
		{
		    साँचो = tv_idx->vval.v_string;
		    यदि (साँचो == रिक्त)
			साँचो = (char_u *)"";
		}
		अन्यथा
		{
		    साँचो = tv_get_string(tv_idx);
		}
		यदि (d != रिक्त)
		    di = dict_find(d, साँचो, (पूर्णाङ्क)STRLEN(साँचो));
		यदि (di == रिक्त)
		{
		    // NULL dict is equivalent to empty dict
		    semsg(_(e_साँचो_not_present_in_dictionary_str), साँचो);
		    status = FAIL;
		}
		अन्यथा यदि (var_check_fixed(di->di_flags,
						   रिक्त, FALSE)
			|| var_check_ro(di->di_flags,
						  रिक्त, FALSE))
		    status = FAIL;
		अन्यथा
		    dictitem_remove(d, di, "unlet");
	    }
	}
    }
    अन्यथा यदि (tv_dest->v_type == VAR_LIST)
    {
	// unlet a List item, index must be a number
	यदि (check_for_सङ्ख्या(tv_idx) == FAIL)
	{
	    status = FAIL;
	}
	अन्यथा
	{
	    list_T	*l = tv_dest->vval.v_list;
	    दीर्घ	n = (दीर्घ)tv_idx->vval.v_सङ्ख्या;

	    यदि (l != रिक्त && मान_check_lock(
				      l->lv_lock, रिक्त, FALSE))
		status = FAIL;
	    अन्यथा
	    {
		listitem_T	*li = list_find(l, n);

		यदि (li == रिक्त)
		{
		    semsg(_(e_list_index_out_of_range_nr), n);
		    status = FAIL;
		}
		अन्यथा
		    listitem_remove(l, li);
	    }
	}
    }
    अन्यथा
    {
	status = FAIL;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx);
    clear_tv(tv_dest);
    ectx->ec_थाक.ga_len -= 2;

    फर्काऊ status;
}

/*
 * Unlet a range of items in a list variable.
 */
    स्थिर पूर्णाङ्क
execute_unletrange(isn_T *iptr, ectx_T *ectx)
{
    // Stack contains:
    // -3 index1
    // -2 index2
    // -1 dict or list
    typval_T	*tv_idx1 = थाक_TV_BOT(-3);
    typval_T	*tv_idx2 = थाक_TV_BOT(-2);
    typval_T	*tv_dest = थाक_TV_BOT(-1);
    पूर्णाङ्क		status = OK;

    यदि (tv_dest->v_type == VAR_LIST)
    {
	// indexes must be a number
	SOURCING_LNUM = iptr->isn_lnum;
	यदि (check_for_सङ्ख्या(tv_idx1) == FAIL
		|| (tv_idx2->v_type != VAR_SPECIAL
			 && check_for_सङ्ख्या(tv_idx2) == FAIL))
	{
	    status = FAIL;
	}
	अन्यथा
	{
	    list_T	*l = tv_dest->vval.v_list;
	    दीर्घ	n1 = (दीर्घ)tv_idx1->vval.v_सङ्ख्या;
	    दीर्घ	n2 = tv_idx2->v_type == VAR_SPECIAL
			    ? 0 : (दीर्घ)tv_idx2->vval.v_सङ्ख्या;
	    listitem_T	*li;

	    li = list_find_index(l, &n1);
	    यदि (li == रिक्त)
	    {
		semsg(_(e_list_index_out_of_range_nr),
						 (दीर्घ)tv_idx1->vval.v_सङ्ख्या);
		status = FAIL;
	    }
	    अन्यथा
	    {
		यदि (n1 < 0)
		    n1 = list_idx_of_item(l, li);
		यदि (n2 < 0)
		{
		    listitem_T *li2 = list_find(l, n2);

		    यदि (li2 == रिक्त)
		    {
			semsg(_(e_list_index_out_of_range_nr), n2);
			status = FAIL;
		    }
		    अन्यथा
			n2 = list_idx_of_item(l, li2);
		}
		यदि (status != FAIL
			&& tv_idx2->v_type != VAR_SPECIAL
			&& n2 < n1)
		{
		    semsg(_(e_list_index_out_of_range_nr), n2);
		    status = FAIL;
		}
		यदि (status != FAIL)
		    list_unlet_range(l, li, n1,
					   tv_idx2->v_type != VAR_SPECIAL, n2);
	    }
	}
    }
    अन्यथा
    {
	status = FAIL;
	SOURCING_LNUM = iptr->isn_lnum;
	semsg(_(e_cannot_index_str),
				vartype_name(tv_dest->v_type));
    }

    clear_tv(tv_idx1);
    clear_tv(tv_idx2);
    clear_tv(tv_dest);
    ectx->ec_थाक.ga_len -= 3;

    फर्काऊ status;
}

/*
 * Top of a for loop.
 */
    स्थिर पूर्णाङ्क
execute_for(isn_T *iptr, ectx_T *ectx)
{
    typval_T	*tv;
    पूर्णाङ्क		jump = FALSE;
    typval_T	*ltv = थाक_TV_BOT(-1);
    typval_T	*idxtv =
		   थाक_TV_VAR(iptr->isn_arg.forloop.for_loop_idx);

    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
	फर्काऊ FAIL;
    यदि (ltv->v_type == VAR_LIST)
    {
	list_T *list = ltv->vval.v_list;

	// push the next item from the list
	++idxtv->vval.v_सङ्ख्या;
	यदि (list == रिक्त
		       || idxtv->vval.v_सङ्ख्या >= list->lv_len)
	{
	    jump = TRUE;
	}
	अन्यथा यदि (list->lv_first == &range_list_item)
	{
	    // non-materialized range() list
	    tv = थाक_TV_BOT(0);
	    tv->v_type = VAR_सङ्ख्या;
	    tv->v_lock = 0;
	    tv->vval.v_सङ्ख्या = list_find_nr(
			     list, idxtv->vval.v_सङ्ख्या, रिक्त);
	    ++ectx->ec_थाक.ga_len;
	}
	अन्यथा
	{
	    listitem_T *li = list_find(list,
					 idxtv->vval.v_सङ्ख्या);
	    यदि (li == रिक्त)
		फर्काऊ FAIL;
	    copy_tv(&li->li_tv, थाक_TV_BOT(0));
	    ++ectx->ec_थाक.ga_len;
	}
    }
    अन्यथा यदि (ltv->v_type == VAR_TUPLE)
    {
	tuple_T *tuple = ltv->vval.v_tuple;

	// push the next item from the tuple
	++idxtv->vval.v_सङ्ख्या;
	यदि (tuple == रिक्त || idxtv->vval.v_सङ्ख्या >= TUPLE_LEN(tuple))
	    jump = TRUE;
	अन्यथा
	{
	    copy_tv(TUPLE_ITEM(tuple, idxtv->vval.v_सङ्ख्या), थाक_TV_BOT(0));
	    ++ectx->ec_थाक.ga_len;
	}
    }
    अन्यथा यदि (ltv->v_type == VAR_STRING)
    {
	char_u	*str = ltv->vval.v_string;

	// The index is for the last byte of the previous
	// character.
	++idxtv->vval.v_सङ्ख्या;
	यदि (str == रिक्त || str[idxtv->vval.v_सङ्ख्या] == NUL)
	{
	    jump = TRUE;
	}
	अन्यथा
	{
	    पूर्णाङ्क	clen = mb_ptr2len(str + idxtv->vval.v_सङ्ख्या);

	    // Push the next character from the string.
	    tv = थाक_TV_BOT(0);
	    tv->v_type = VAR_STRING;
	    tv->vval.v_string = vim_strnsave(
			     str + idxtv->vval.v_सङ्ख्या, clen);
	    ++ectx->ec_थाक.ga_len;
	    idxtv->vval.v_सङ्ख्या += clen - 1;
	}
    }
    अन्यथा यदि (ltv->v_type == VAR_BLOB)
    {
	blob_T	*blob = ltv->vval.v_blob;

	// When we get here the first time make a copy of the
	// blob, so that the iteration still works when it is
	// changed.
	यदि (idxtv->vval.v_सङ्ख्या == -1 && blob != रिक्त)
	{
	    ltv->v_lock = 0;
	    ltv->vval.v_blob = blob_copy(blob);
	    blob_unref(blob);
	    blob = ltv->vval.v_blob;
	}

	// The index is for the previous byte.
	++idxtv->vval.v_सङ्ख्या;
	यदि (blob == रिक्त || idxtv->vval.v_सङ्ख्या >= blob_len(blob))
	{
	    jump = TRUE;
	}
	अन्यथा
	{
	    // Push the next byte from the blob.
	    tv = थाक_TV_BOT(0);
	    tv->v_type = VAR_सङ्ख्या;
	    tv->vval.v_सङ्ख्या = blob_get(blob,
					 idxtv->vval.v_सङ्ख्या);
	    ++ectx->ec_थाक.ga_len;
	}
    }
    अन्यथा
    {
	semsg(_(e_for_loop_on_str_not_supported),
				    vartype_name(ltv->v_type));
	फर्काऊ FAIL;
    }

    यदि (jump)
    {
	// past the end of the list/string/blob, jump to "endfor"
	ectx->ec_iidx = iptr->isn_arg.forloop.for_end;
	may_restore_cmdmod(&ectx->ec_funclocal);
    }
    अन्यथा
    {
	// Store the current number of funcrefs, this may be used in
	// ISN_LOOPEND.  The variable index is always one more than the loop
	// variable index.
	tv = थाक_TV_VAR(iptr->isn_arg.forloop.for_loop_idx + 1);
	tv->vval.v_सङ्ख्या = ectx->ec_funcrefs.ga_len;
    }

    फर्काऊ OK;
}

/*
 * Code for handling variables declared inside a loop and used in a closure.
 * This is very similar to what is done with funcstack_T.  The difference is
 * that the funcstack_T has the scope of a function, while a loopvars_T has the
 * scope of the block inside a loop and each loop may have its own.
 */

// Double linked list of loopvars_T in use.
स्थिर loopvars_T *first_loopvars = रिक्त;

    स्थिर शून्य
add_loopvars_to_list(loopvars_T *loopvars)
{
	// Link in list of loopvarss.
    यदि (first_loopvars != रिक्त)
	first_loopvars->lvs_prev = loopvars;
    loopvars->lvs_next = first_loopvars;
    loopvars->lvs_prev = रिक्त;
    first_loopvars = loopvars;
}

    स्थिर शून्य
remove_loopvars_from_list(loopvars_T *loopvars)
{
    यदि (loopvars->lvs_prev == रिक्त)
	first_loopvars = loopvars->lvs_next;
    अन्यथा
	loopvars->lvs_prev->lvs_next = loopvars->lvs_next;
    यदि (loopvars->lvs_next != रिक्त)
	loopvars->lvs_next->lvs_prev = loopvars->lvs_prev;
}

/*
 * End of a for or while loop: Handle any variables used by a closure.
 */
    स्थिर पूर्णाङ्क
execute_endloop(isn_T *iptr, ectx_T *ectx)
{
    endloop_T	*endloop = &iptr->isn_arg.endloop;
    typval_T	*tv_refcount = थाक_TV_VAR(endloop->end_funcref_idx);
    पूर्णाङ्क		prev_closure_गन्ती = tv_refcount->vval.v_सङ्ख्या;
    पूर्णाङ्क		depth = endloop->end_depth;
    garray_T	*gap = &ectx->ec_funcrefs;
    पूर्णाङ्क		closure_in_use = FALSE;
    loopvars_T  *loopvars;
    typval_T    *थाक;
    पूर्णाङ्क		idx;

    // Check if any created closure is still being referenced and loopvars have
    // not been saved yet for the current depth.
    लागि (idx = prev_closure_गन्ती; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	यदि (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == रिक्त)
	{
	    पूर्णाङ्क refcount = pt->pt_refcount;
	    पूर्णाङ्क i;

	    // A Reference in a variable inside the loop doesn't count, it gets
	    // unreferenced at the end of the loop.
	    लागि (i = 0; i < endloop->end_var_गन्ती; ++i)
	    {
		typval_T *stv = थाक_TV_VAR(endloop->end_var_idx + i);

		यदि (stv->v_type == VAR_PARTIAL && pt == stv->vval.v_partial)
		    --refcount;
	    }
	    यदि (refcount > 1)
	    {
		closure_in_use = TRUE;
		रोक;
	    }
	}
    }

    // If no function reference were created since the start of the loop block
    // or it is no longer referenced there is nothing to do.
    यदि (!closure_in_use)
	फर्काऊ OK;

    // A closure is using variables declared inside the loop.
    // Move them to the called function.
    loopvars = ALLOC_CLEAR_ONE(loopvars_T);
    यदि (loopvars == रिक्त)
	फर्काऊ FAIL;

    loopvars->lvs_ga.ga_len = endloop->end_var_गन्ती;
    थाक = ALLOC_CLEAR_MULT(typval_T, loopvars->lvs_ga.ga_len);
    loopvars->lvs_ga.ga_data = थाक;
    यदि (थाक == रिक्त)
    {
	vim_free(loopvars);
	फर्काऊ FAIL;
    }
    add_loopvars_to_list(loopvars);

    // Move the variable values.
    लागि (idx = 0; idx < endloop->end_var_गन्ती; ++idx)
    {
	typval_T *tv = थाक_TV_VAR(endloop->end_var_idx + idx);

	*(थाक + idx) = *tv;
	tv->v_type = VAR_UNKNOWN;
    }

    लागि (idx = prev_closure_गन्ती; idx < gap->ga_len; ++idx)
    {
	partial_T   *pt = ((partial_T **)gap->ga_data)[idx];

	यदि (pt->pt_refcount > 1 && pt->pt_loopvars[depth] == रिक्त)
	{
	    ++loopvars->lvs_refcount;
	    pt->pt_loopvars[depth] = loopvars;

	    pt->pt_outer.out_loop[depth].थाक = &loopvars->lvs_ga;
	    pt->pt_outer.out_loop[depth].var_idx -=
		  ectx->ec_frame_idx + थाक_FRAME_SIZE + endloop->end_var_idx;
	}
    }

    फर्काऊ OK;
}

/*
 * Called when a partial is freed or its reference count goes down to one.  The
 * loopvars may be the only reference to the partials in the local variables.
 * Go over all of them, the funcref and can be freed if all partials
 * referencing the loopvars have a reference count of one.
 * Return TRUE if it was freed.
 */
    पूर्णाङ्क
loopvars_check_refcount(loopvars_T *loopvars)
{
    पूर्णाङ्क		    i;
    garray_T	    *gap = &loopvars->lvs_ga;
    पूर्णाङ्क		    done = 0;
	typval_T	*थाक = gap->ga_data;

    यदि (loopvars->lvs_refcount > loopvars->lvs_min_refcount)
	फर्काऊ FALSE;
    लागि (i = 0; i < gap->ga_len; ++i)
    {
	typval_T    *tv = ((typval_T *)gap->ga_data) + i;

	यदि (tv->v_type == VAR_PARTIAL && tv->vval.v_partial != रिक्त
		&& tv->vval.v_partial->pt_refcount == 1)
	{
	    पूर्णाङ्क	    depth;

	    लागि (depth = 0; depth < MAX_LOOP_DEPTH; ++depth)
		यदि (tv->vval.v_partial->pt_loopvars[depth] == loopvars)
		    ++done;
	}
    }
    यदि (done != loopvars->lvs_min_refcount)
	फर्काऊ FALSE;

    // All partials referencing the loopvars have a reference count of
    // one, thus the loopvars is no longer of use.
    थाक = gap->ga_data;
    लागि (i = 0; i < gap->ga_len; ++i)
	clear_tv(थाक + i);
    vim_free(थाक);
    remove_loopvars_from_list(loopvars);
    vim_free(loopvars);
    फर्काऊ TRUE;
}

/*
 * For garbage collecting: set references in all variables referenced by
 * all loopvars.
 */
    पूर्णाङ्क
set_ref_in_loopvars(पूर्णाङ्क copyID)
{
    loopvars_T *loopvars;

    लागि (loopvars = first_loopvars; loopvars != रिक्त;
						 loopvars = loopvars->lvs_next)
    {
	typval_T    *थाक = loopvars->lvs_ga.ga_data;
	पूर्णाङ्क	    i;

	लागि (i = 0; i < loopvars->lvs_ga.ga_len; ++i)
	    यदि (set_ref_in_item(थाक + i, copyID, रिक्त, रिक्त, रिक्त))
		फर्काऊ TRUE;  // abort
    }
    फर्काऊ FALSE;
}

/*
 * Load instruction for w:/b:/g:/t: variable.
 * "isn_type" is used instead of "iptr->isn_type".
 */
    स्थिर पूर्णाङ्क
load_namespace_var(ectx_T *ectx, isntype_T isn_type, isn_T *iptr)
{
    dictitem_T	*di = रिक्त;
    hashtab_T	*ht = रिक्त;
    वर्ण	namespace;

    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
	फर्काऊ NOTDONE;
    विकल्प (isn_type)
    {
	पक्ष ISN_LOADG:
	    ht = get_globvar_ht();
	    namespace = 'g';
	    रोक;
	पक्ष ISN_LOADB:
	    ht = &curbuf->b_vars->dv_hashtab;
	    namespace = 'b';
	    रोक;
	पक्ष ISN_LOADW:
	    ht = &curwin->w_vars->dv_hashtab;
	    namespace = 'w';
	    रोक;
	पक्ष ISN_LOADT:
	    ht = &curtab->tp_vars->dv_hashtab;
	    namespace = 't';
	    रोक;
	स्वभाव:  // Cannot reach here
	    फर्काऊ NOTDONE;
    }
    di = find_var_in_ht(ht, 0, iptr->isn_arg.string, TRUE);

    यदि (di == रिक्त)
    {
	यदि (isn_type == ISN_LOADG)
	{
	    ufunc_T *ufunc = find_func(iptr->isn_arg.string, TRUE);

	    // g:Something could be a function
	    यदि (ufunc != रिक्त)
	    {
		typval_T    *tv = थाक_TV_BOT(0);

		++ectx->ec_थाक.ga_len;
		tv->v_type = VAR_FUNC;
		tv->vval.v_string = alloc(STRLEN(iptr->isn_arg.string) + 3);
		यदि (tv->vval.v_string == रिक्त)
		    फर्काऊ FAIL;
		STRCPY(tv->vval.v_string, "g:");
		STRCPY(tv->vval.v_string + 2, iptr->isn_arg.string);
		फर्काऊ OK;
	    }
	}
	SOURCING_LNUM = iptr->isn_lnum;
	यदि (vim_strchr(iptr->isn_arg.string, AUTOLOAD_CHAR) != रिक्त)
	    // no check if the item exists in the script but
	    // isn't exported, it is too complicated
	    semsg(_(e_item_not_found_in_script_str), iptr->isn_arg.string);
	अन्यथा
	    semsg(_(e_undefined_variable_char_str),
					      namespace, iptr->isn_arg.string);
	// Push a value anyway, the stack must have the expected size when the
	// error is ignored because of ":silent!" in a calling function.
	typval_T *tv = थाक_TV_BOT(0);
	tv->v_type = VAR_सङ्ख्या;
	tv->v_lock = 0;
	tv->vval.v_सङ्ख्या = 0;
	++ectx->ec_थाक.ga_len;
	फर्काऊ FAIL;
    }
    अन्यथा
    {
	copy_tv(&di->di_tv, थाक_TV_BOT(0));
	++ectx->ec_थाक.ga_len;
    }
    फर्काऊ OK;
}


    स्थिर शून्य
object_required_error(typval_T *tv)
{
    garray_T type_list;
    ga_init2(&type_list, परिमाण(type_T *), 10);
    type_T *type = typval2type(tv, get_copyID(), &type_list, TVTT_DO_MEMBER);
    वर्ण *tofree = रिक्त;
    वर्ण *typename = type_name(type, &tofree);
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
    स्थिर पूर्णाङ्क
var_any_get_oc_member(class_T *current_class, isn_T *iptr, typval_T *tv)
{
    पूर्णाङ्क		is_object = tv->v_type == VAR_OBJECT;
    class_T	*tv_cl;
    object_T	*obj = रिक्त;
    typval_T	mtv;

    यदि (is_object)
    {
	obj = tv->vval.v_object;
	यदि (obj == रिक्त)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_object));
	    फर्काऊ FAIL;
	}
	tv_cl = obj->obj_class;
    }
    अन्यथा
    {
	tv_cl = tv->vval.v_class;
	यदि (tv_cl == रिक्त)
	{
	    SOURCING_LNUM = iptr->isn_lnum;
	    emsg(_(e_using_null_class));
	    फर्काऊ FAIL;
	}
    }

    // get_member_tv() needs the class/object information in the typval
    // argument.  So set the object information.
    copy_tv(tv, &mtv);

    // 'name' can either be an instance or class variable or method
    पूर्णाङ्क		namelen = (पूर्णाङ्क)STRLEN(iptr->isn_arg.string);
    पूर्णाङ्क		save_did_emsg = did_emsg;

    यदि (get_member_tv(tv_cl, is_object, iptr->isn_arg.string, namelen,
						current_class, &mtv) == OK)
    {
	// instance or class variable
	copy_tv(&mtv, tv);
	clear_tv(&mtv);
	फर्काऊ OK;
    }

    यदि (did_emsg != save_did_emsg)
	फर्काऊ FAIL;

    // could be a class or instance method
    ufunc_T	*oc_method;
    पूर्णाङ्क		oc_method_idx;

    oc_method = method_lookup(tv_cl, tv->v_type, iptr->isn_arg.string,
						namelen, &oc_method_idx);
    यदि (oc_method == रिक्त)
    {
	वर्ण	*सन्देश;

	SOURCING_LNUM = iptr->isn_lnum;
	यदि (is_object)
	    सन्देश = e_variable_not_found_on_object_str_str;
	अन्यथा
	    सन्देश = e_class_variable_str_not_found_in_class_str;
	semsg(_(सन्देश), iptr->isn_arg.string, tv_cl->class_name.string);
	फर्काऊ FAIL;
    }

    // Protected methods are not accessible outside the class
    यदि (*oc_method->uf_name == '_'
			&& !class_instance_of(current_class, tv_cl))
    {
	semsg(_(e_cannot_access_protected_method_str), oc_method->uf_name);
	फर्काऊ FAIL;
    }

    // Create a partial for the instance or class method
    यदि (obj_method_to_partial_tv(is_object ? obj : रिक्त, oc_method, tv)
								== FAIL)
	फर्काऊ FAIL;

    फर्काऊ OK;
}

/*
 * do ISN_PUT or ISN_IPUT instruction depending on fixindent parameter
 */
    स्थिर शून्य
isn_put_do(ectx_T *ectx, isn_T *iptr, typval_T *tv, पूर्णाङ्क fixindent)
{
    पूर्णाङ्क		regname = iptr->isn_arg.put.put_regname;
    linenr_T	lnum = iptr->isn_arg.put.put_lnum;
    char_u	*अभिव्यक्ति = रिक्त;
    पूर्णाङ्क		dir = FORWARD;

    यदि (lnum < -2)
    {
	// line number was put on the stack by ISN_RANGE
	tv = थाक_TV_BOT(-1);
	curwin->w_cursor.lnum = tv->vval.v_सङ्ख्या;
	यदि (lnum == LNUM_VARIABLE_RANGE_ABOVE)
	    dir = BACKWARD;
	--ectx->ec_थाक.ga_len;
    }
    अन्यथा यदि (lnum == -2)
	// :put! above cursor
	dir = BACKWARD;
    अन्यथा यदि (lnum >= 0)
    {
	curwin->w_cursor.lnum = lnum;
	यदि (lnum == 0)
	    // check_cursor() below will move to line 1
	    dir = BACKWARD;
    }

    यदि (regname == '=')
    {
	tv = थाक_TV_BOT(-1);
	यदि (tv->v_type == VAR_STRING)
	    अभिव्यक्ति = tv->vval.v_string;
	अन्यथा
	{
	    अभिव्यक्ति = typval2string(tv, TRUE); // allocates value
	    clear_tv(tv);
	}
	--ectx->ec_थाक.ga_len;
    }
    check_cursor();

    यदि (fixindent)
	do_put(regname, अभिव्यक्ति, dir, 1L, PUT_पङ्क्ति|PUT_CURSLINE|PUT_FIXINDENT);
    अन्यथा
	do_put(regname, अभिव्यक्ति, dir, 1L, PUT_पङ्क्ति|PUT_CURSLINE);
    vim_free(अभिव्यक्ति);
}

/*
 * Execute the ISN_UNPACK instruction for a List
 */
    स्थिर पूर्णाङ्क
exec_unpack_list(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    पूर्णाङ्क		गन्ती = iptr->isn_arg.unpack.unp_गन्ती;
    पूर्णाङ्क		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    list_T	*l;
    listitem_T	*li;
    पूर्णाङ्क		i;

    l = tv->vval.v_list;
    यदि (l == रिक्त
	    || l->lv_len < (semicolon ? गन्ती - 1 : गन्ती))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_मान_does_not_have_enough_वस्तुहरू));
	फर्काऊ EXEC_FAIL;
    }
    अन्यथा यदि (!semicolon && l->lv_len > गन्ती)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_list_मान_has_more_वस्तुहरू_than_targets));
	फर्काऊ EXEC_FAIL;
    }

    CHECK_LIST_MATERIALIZE(l);
    यदि (GA_GROW_FAILS(&ectx->ec_थाक, गन्ती - 1))
	फर्काऊ EXEC_DONE;
    ectx->ec_थाक.ga_len += गन्ती - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    यदि (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_वस्तुहरू(l->lv_len - गन्ती + 1);

	यदि (rem_list == रिक्त)
	    फर्काऊ EXEC_DONE;
	tv = थाक_TV_BOT(-गन्ती);
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	li = l->lv_first;
	लागि (i = 0; i < गन्ती - 1; ++i)
	    li = li->li_next;
	लागि (i = 0; li != रिक्त; ++i)
	{
	    typval_T tvcopy;

	    copy_tv(&li->li_tv, &tvcopy);
	    list_set_item(rem_list, i, &tvcopy);
	    li = li->li_next;
	}
	--गन्ती;
    }

    // Produce the values in reverse order, first item last.
    li = l->lv_first;
    लागि (i = 0; i < गन्ती; ++i)
    {
	tv = थाक_TV_BOT(-i - 1);
	copy_tv(&li->li_tv, tv);
	li = li->li_next;
    }

    list_unref(l);

    फर्काऊ EXEC_OK;
}

/*
 * Execute the ISN_UNPACK instruction for a Tuple
 */
    स्थिर पूर्णाङ्क
exec_unpack_tuple(ectx_T *ectx, isn_T *iptr, typval_T *tv)
{
    पूर्णाङ्क		गन्ती = iptr->isn_arg.unpack.unp_गन्ती;
    पूर्णाङ्क		semicolon = iptr->isn_arg.unpack.unp_semicolon;
    tuple_T	*tuple;
    पूर्णाङ्क		i;

    tuple = tv->vval.v_tuple;
    यदि (tuple == रिक्त
	    || TUPLE_LEN(tuple) < (semicolon ? गन्ती - 1 : गन्ती))
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_more_targets_than_tuple_वस्तुहरू));
	फर्काऊ EXEC_FAIL;
    }
    अन्यथा यदि (!semicolon && TUPLE_LEN(tuple) > गन्ती)
    {
	SOURCING_LNUM = iptr->isn_lnum;
	emsg(_(e_less_targets_than_tuple_वस्तुहरू));
	फर्काऊ EXEC_FAIL;
    }

    यदि (GA_GROW_FAILS(&ectx->ec_थाक, गन्ती - 1))
	फर्काऊ EXEC_DONE;
    ectx->ec_थाक.ga_len += गन्ती - 1;

    // Variable after semicolon gets a list with the remaining
    // items.
    यदि (semicolon)
    {
	list_T	*rem_list =
	    list_alloc_with_वस्तुहरू(TUPLE_LEN(tuple) - गन्ती + 1);

	यदि (rem_list == रिक्त)
	    फर्काऊ EXEC_DONE;
	tv = थाक_TV_BOT(-गन्ती);
	tv->v_type = VAR_LIST;
	tv->vval.v_list = rem_list;
	++rem_list->lv_refcount;
	tv->v_lock = 0;
	लागि (i = गन्ती - 1; i < TUPLE_LEN(tuple); ++i)
	{
	    typval_T tvcopy;

	    copy_tv(TUPLE_ITEM(tuple, i), &tvcopy);
	    list_set_item(rem_list, i - (गन्ती - 1), &tvcopy);
	}
	--गन्ती;
    }

    // Produce the values in reverse order, first item last.
    लागि (i = 0; i < गन्ती; ++i)
    {
	tv = थाक_TV_BOT(-i - 1);
	copy_tv(TUPLE_ITEM(tuple, i), tv);
    }

    tuple_unref(tuple);

    फर्काऊ EXEC_OK;
}

/*
 * Execute instructions in execution context "ectx".
 * Return OK or FAIL;
 */
    स्थिर पूर्णाङ्क
exec_instructions(ectx_T *ectx)
{
    पूर्णाङ्क		ret = FAIL;
    पूर्णाङ्क		save_trylevel_at_start = ectx->ec_trylevel_at_start;
    पूर्णाङ्क		dict_थाक_len_at_start = dict_थाक.ga_len;

    // Start execution at the first instruction.
    ectx->ec_iidx = 0;

    // Only catch exceptions in this instruction list.
    ectx->ec_trylevel_at_start = trylevel;

    लागि (;;)
    {
	स्थिर पूर्णाङ्क  breakcheck_गन्ती = 0;  // using "static" makes it faster
	isn_T	    *iptr;
	typval_T    *tv;

	यदि (unlikely(++breakcheck_गन्ती >= 100))
	{
	    पङ्क्ति_breakcheck();
	    breakcheck_गन्ती = 0;
	}
	यदि (unlikely(got_int))
	{
	    // Turn CTRL-C into an exception.
	    got_int = FALSE;
	    यदि (throw_exception("Vim:Interrupt", ET_INTERRUPT, रिक्त) == FAIL)
		गच्छ theend;
	    did_throw = TRUE;
	}

	यदि (unlikely(did_emsg && सन्देश_list != रिक्त && *सन्देश_list != रिक्त))
	{
	    // Turn an error message into an exception.
	    did_emsg = FALSE;
	    यदि (throw_exception(*सन्देश_list, ET_ERROR, रिक्त) == FAIL)
		गच्छ theend;
	    did_throw = TRUE;
	    *सन्देश_list = रिक्त;

	    // This exception was not caught (yet).
	    garray_T	*trystack = &ectx->ec_trystack;
	    यदि (trystack->ga_len > 0)
	    {
		trycmd_T *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		यदि (trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		{
		    यदि (trycmd->tcd_caught)
		    {
			// Inside a "catch" we need to first discard the caught
			// exception.
			finish_exception(caught_थाक);
			trycmd->tcd_caught = FALSE;
		    }
		}
	    }
	}

	यदि (unlikely(did_throw))
	{
	    garray_T	*trystack = &ectx->ec_trystack;
	    trycmd_T    *trycmd = रिक्त;
	    पूर्णाङ्क		index = trystack->ga_len;

	    // An exception jumps to the first catch, finally, or returns from
	    // the current function.
	    जबसम्म (index > 0)
	    {
		trycmd = ((trycmd_T *)trystack->ga_data) + index - 1;
		// 1. after :try and before :catch - jump to first :catch
		// 2. in :catch block - jump to :finally
		// 3. in :catch block and no finally - jump to :endtry
		यदि (!trycmd->tcd_in_catch || trycmd->tcd_finally_idx != 0
				|| trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    रोक;
		// In the catch and finally block of this try we have to go up
		// one level.
		--index;
		trycmd = रिक्त;
	    }
	    यदि (trycmd != रिक्त && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
	    {
		यदि (trycmd->tcd_in_catch)
		{
		    यदि (trycmd->tcd_finally_idx > 0)
		    {
			// exception inside ":catch", jump to ":finally" once
			ectx->ec_iidx = trycmd->tcd_finally_idx;
			trycmd->tcd_finally_idx = 0;
		    }
		    अन्यथा
		    {
			// exception inside ":catch" or ":finally", jump to
			// ":endtry"
			ectx->ec_iidx = trycmd->tcd_endtry_idx;
		    }
		}
		अन्यथा
		{
		    // jump to first ":catch"
		    ectx->ec_iidx = trycmd->tcd_catch_idx;
		    trycmd->tcd_in_catch = TRUE;
		}
		did_throw = FALSE;  // don't come back here until :endtry
		trycmd->tcd_did_throw = TRUE;
	    }
	    अन्यथा
	    {
		// Not inside try or need to return from current function.
		// Push a dummy return value.
		यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    गच्छ theend;
		tv = थाक_TV_BOT(0);
		tv->v_type = VAR_सङ्ख्या;
		tv->vval.v_सङ्ख्या = 0;
		++ectx->ec_थाक.ga_len;
		यदि (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
		{
		    // At the toplevel we are done.
		    need_rethrow = TRUE;
		    यदि (handle_closure_in_use(ectx, FALSE) == FAIL)
			गच्छ theend;
		    गच्छ done;
		}

		यदि (func_return(ectx) == FAIL)
		    गच्छ theend;
	    }
	    जारी;
	}

	/*
	 * Big switch on the instruction.  Most compilers will be turning this
	 * into an efficient lookup table, since the "case" values are an enum
	 * with sequential numbers.  It may look ugly, but it should be the
	 * most efficient way.
	 */
	iptr = &ectx->ec_instr[ectx->ec_iidx++];
	विकल्प (iptr->isn_type)
	{
	    // Constructor, first instruction in a new() method.
	    पक्ष ISN_CONSTRUCT:
		// "this" is always the local variable at index zero
		tv = थाक_TV_VAR(0);
		tv->v_type = VAR_OBJECT;
		tv->vval.v_object =
		    alloc_object(iptr->isn_arg.construct.construct_class);
		रोक;

	    // execute Ex command line
	    पक्ष ISN_EXEC:
		यदि (exec_command(iptr, iptr->isn_arg.string) == FAIL)
		    गच्छ on_error;
		रोक;

	    // execute Ex command line split at NL characters.
	    पक्ष ISN_EXEC_SPLIT:
		{
		    source_cookie_T cookie;
		    char_u	    *पङ्क्ति;

		    SOURCING_LNUM = iptr->isn_lnum;
		    CLEAR_FIELD(cookie);
		    cookie.sourcing_lnum = iptr->isn_lnum - 1;
		    cookie.nextline = iptr->isn_arg.string;
		    पङ्क्ति = get_split_sourceline(0, &cookie, 0, 0);
		    यदि (do_cmdline(पङ्क्ति,
				get_split_sourceline, &cookie,
				   DOCMD_VERBOSE|DOCMD_NOWAIT|DOCMD_KEYTYPED)
									== FAIL
				|| did_emsg)
		    {
			vim_free(पङ्क्ति);
			गच्छ on_error;
		    }
		    vim_free(पङ्क्ति);
		}
		रोक;

	    // execute Ex command line that is only a range
	    पक्ष ISN_EXECRANGE:
		{
		    exarg_T	ea;
		    वर्ण	*error = रिक्त;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = CMD_SIZE;
		    ea.addr_type = ADDR_पङ्क्तिहरू;
		    ea.cmd = iptr->isn_arg.string;
		    SOURCING_LNUM = iptr->isn_lnum;
		    parse_cmd_address(&ea, &error, FALSE);
		    यदि (ea.cmd == रिक्त)
			गच्छ on_error;
		    // error is always NULL when using ADDR_LINES
		    error = ex_range_without_command(&ea);
		    यदि (error != रिक्त)
		    {
			emsg(error);
			गच्छ on_error;
		    }
		}
		रोक;

	    // Evaluate an expression with legacy syntax, push it onto the
	    // stack.
	    पक्ष ISN_LEGACY_EVAL:
		{
		    char_u  *arg = iptr->isn_arg.string;
		    पूर्णाङ्क	    res;
		    पूर्णाङ्क	    save_flags = cmdmod.cmod_flags;

		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    tv = थाक_TV_BOT(0);
		    init_tv(tv);
		    cmdmod.cmod_flags |= CMOD_LEGACY;
		    res = eval0(arg, tv, रिक्त, &EVALARG_EVALUATE);
		    cmdmod.cmod_flags = save_flags;
		    यदि (res == FAIL)
			गच्छ on_error;
		    ++ectx->ec_थाक.ga_len;
		}
		रोक;

	    // push typeval VAR_INSTR with instructions to be executed
	    पक्ष ISN_INSTR:
		{
		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    tv = थाक_TV_BOT(0);
		    tv->vval.v_instr = ALLOC_ONE(instr_T);
		    यदि (tv->vval.v_instr == रिक्त)
			गच्छ on_error;
		    ++ectx->ec_थाक.ga_len;

		    tv->v_type = VAR_INSTR;
		    tv->vval.v_instr->instr_ectx = ectx;
		    tv->vval.v_instr->instr_instr = iptr->isn_arg.instr;
		}
		रोक;

	    पक्ष ISN_SOURCE:
		{
		    पूर्णाङ्क notused;

		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (may_load_script((पूर्णाङ्क)iptr->isn_arg.सङ्ख्या, &notused)
								       == FAIL)
			गच्छ on_error;
		}
		रोक;

	    // execute :substitute with an expression
	    पक्ष ISN_SUBSTITUTE:
		{
		    subs_T		*subs = &iptr->isn_arg.subs;
		    source_cookie_T	cookie;
		    संरचना subs_अभिव्यक्ति_S	*save_instr = substitute_instr;
		    संरचना subs_अभिव्यक्ति_S	subs_instr;
		    पूर्णाङ्क			res;

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

		    यदि (res == FAIL || did_emsg
					     || subs_instr.subs_status == FAIL)
			गच्छ on_error;
		}
		रोक;

	    पक्ष ISN_FINISH:
		गच्छ done;

	    पक्ष ISN_REDIRSTART:
		// create a dummy entry for var_redir_str()
		यदि (alloc_redir_lval() == FAIL)
		    गच्छ on_error;

		// The output is stored in growarray "redir_ga" until
		// redirection ends.
		init_redir_ga();
		redir_vname = 1;
		रोक;

	    पक्ष ISN_REDIREND:
		{
		    char_u *res = get_clear_redir_ga();

		    // End redirection, put redirected text on the stack.
		    clear_redir_lval();
		    redir_vname = 0;

		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    {
			vim_free(res);
			गच्छ theend;
		    }
		    tv = थाक_TV_BOT(0);
		    tv->v_type = VAR_STRING;
		    tv->vval.v_string = res;
		    ++ectx->ec_थाक.ga_len;
		}
		रोक;

	    पक्ष ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		force_abort = TRUE;
		यदि (trigger_cexpr_autocmd(iptr->isn_arg.सङ्ख्या) == FAIL)
		    गच्छ on_error;
		force_abort = FALSE;
#endif
		रोक;

	    पक्ष ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    exarg_T ea;
		    पूर्णाङ्क	    res;

		    CLEAR_FIELD(ea);
		    ea.cmdidx = iptr->isn_arg.cexpr.cexpr_ref->cer_cmdidx;
		    ea.forceit = iptr->isn_arg.cexpr.cexpr_ref->cer_forceit;
		    ea.cmdlinep = &iptr->isn_arg.cexpr.cexpr_ref->cer_cmdline;
		    --ectx->ec_थाक.ga_len;
		    tv = थाक_TV_BOT(0);
		    SOURCING_LNUM = iptr->isn_lnum;
		    res = cexpr_core(&ea, tv);
		    clear_tv(tv);
		    यदि (res == FAIL)
			गच्छ on_error;
		}
#endif
		रोक;

	    // execute Ex command from pieces on the stack
	    पक्ष ISN_EXECCONCAT:
		{
		    पूर्णाङ्क	    गन्ती = iptr->isn_arg.सङ्ख्या;
		    माप_प्रकार  len = 0;
		    पूर्णाङ्क	    pass;
		    पूर्णाङ्क	    i;
		    char_u  *cmd = रिक्त;
		    char_u  *str;

		    लागि (pass = 1; pass <= 2; ++pass)
		    {
			लागि (i = 0; i < गन्ती; ++i)
			{
			    tv = थाक_TV_BOT(i - गन्ती);
			    str = tv->vval.v_string;
			    यदि (str != रिक्त && *str != NUL)
			    {
				यदि (pass == 2)
				    STRCPY(cmd + len, str);
				len += STRLEN(str);
			    }
			    यदि (pass == 2)
				clear_tv(tv);
			}
			यदि (pass == 1)
			{
			    cmd = alloc(len + 1);
			    यदि (unlikely(cmd == रिक्त))
				गच्छ theend;
			    len = 0;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    do_cmdline_cmd(cmd);
		    vim_free(cmd);
		}
		रोक;

	    // execute :echo {string} ...
	    पक्ष ISN_ECHO:
		{
		    पूर्णाङ्क गन्ती = iptr->isn_arg.echo.echo_गन्ती;
		    पूर्णाङ्क	atstart = TRUE;
		    पूर्णाङ्क needclr = TRUE;
		    पूर्णाङ्क	idx;

		    लागि (idx = 0; idx < गन्ती; ++idx)
		    {
			tv = थाक_TV_BOT(idx - गन्ती);
			echo_one(tv, iptr->isn_arg.echo.echo_with_white,
							   &atstart, &needclr);
			clear_tv(tv);
		    }
		    यदि (needclr)
			सन्देश_clr_eos();
		    ectx->ec_थाक.ga_len -= गन्ती;
		}
		रोक;

	    // :execute {string} ...
	    // :echomsg {string} ...
	    // :echowindow {string} ...
	    // :echoconsole {string} ...
	    // :echoerr {string} ...
	    पक्ष ISN_EXECUTE:
	    पक्ष ISN_ECHOMSG:
	    पक्ष ISN_ECHOWINDOW:
	    पक्ष ISN_ECHOCONSOLE:
	    पक्ष ISN_ECHOERR:
		{
		    पूर्णाङ्क		गन्ती;
		    garray_T	ga;
		    char_u	buf[NUMBUFLEN];
		    char_u	*p;
		    पूर्णाङ्क		len;
		    पूर्णाङ्क		failed = FALSE;
		    पूर्णाङ्क		idx;

		    यदि (iptr->isn_type == ISN_ECHOWINDOW)
			गन्ती = iptr->isn_arg.echowin.ewin_गन्ती;
		    अन्यथा
			गन्ती = iptr->isn_arg.सङ्ख्या;
		    ga_init2(&ga, 1, 80);
		    लागि (idx = 0; idx < गन्ती; ++idx)
		    {
			tv = थाक_TV_BOT(idx - गन्ती);
			यदि (iptr->isn_type == ISN_EXECUTE)
			{
			    यदि (tv->v_type == VAR_CHANNEL
						      || tv->v_type == VAR_JOB)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				semsg(_(e_using_invalid_मान_as_string_str),
						    vartype_name(tv->v_type));
				रोक;
			    }
			    अन्यथा
				p = tv_get_string_buf(tv, buf);
			}
			अन्यथा
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    p = tv_stringify(tv, buf);
			}

			len = (पूर्णाङ्क)STRLEN(p);
			यदि (GA_GROW_FAILS(&ga, len + 2))
			    failed = TRUE;
			अन्यथा
			{
			    यदि (ga.ga_len > 0)
				((char_u *)(ga.ga_data))[ga.ga_len++] = ' ';
			    STRCPY((char_u *)(ga.ga_data) + ga.ga_len, p);
			    ga.ga_len += len;
			}
			clear_tv(tv);
		    }
		    ectx->ec_थाक.ga_len -= गन्ती;
		    यदि (failed)
		    {
			ga_clear(&ga);
			गच्छ on_error;
		    }

		    यदि (ga.ga_data != रिक्त)
		    {
			यदि (iptr->isn_type == ISN_EXECUTE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    do_cmdline_cmd((char_u *)ga.ga_data);
			    यदि (did_emsg)
			    {
				ga_clear(&ga);
				गच्छ on_error;
			    }
			}
			अन्यथा
			{
			    सन्देश_sb_eol();
			    यदि (iptr->isn_type == ISN_ECHOMSG)
			    {
				सन्देश_attr(ga.ga_data, echo_attr);
				out_flush();
			    }
#ifdef HAS_MESSAGE_WINDOW
			    अन्यथा यदि (iptr->isn_type == ISN_ECHOWINDOW)
			    {
				start_echowindow(
					      iptr->isn_arg.echowin.ewin_time);
				सन्देश_attr(ga.ga_data, echo_attr);
				end_echowindow();
			    }
#endif
			    अन्यथा यदि (iptr->isn_type == ISN_ECHOCONSOLE)
			    {
				ui_write(ga.ga_data, (पूर्णाङ्क)STRLEN(ga.ga_data),
									 TRUE);
				ui_write((char_u *)"\r\n", 2, TRUE);
			    }
			    अन्यथा
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(ga.ga_data);
			    }
			}
		    }
		    ga_clear(&ga);
		}
		रोक;

	    // load local variable or argument
	    पक्ष ISN_LOAD:
		यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    गच्छ theend;
		tv = थाक_TV_VAR(iptr->isn_arg.सङ्ख्या);
		यदि (tv->v_type == VAR_UNKNOWN)
		{
		    // missing argument or default value v:none
		    थाक_TV_BOT(0)->v_type = VAR_SPECIAL;
		    थाक_TV_BOT(0)->vval.v_सङ्ख्या = VVAL_NONE;
		}
		अन्यथा
		    copy_tv(tv, थाक_TV_BOT(0));
		++ectx->ec_थाक.ga_len;
		रोक;

	    // load v: variable
	    पक्ष ISN_LOADV:
		यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    गच्छ theend;
		copy_tv(get_vim_var_tv(iptr->isn_arg.सङ्ख्या), थाक_TV_BOT(0));
		++ectx->ec_थाक.ga_len;
		रोक;

	    // load s: variable in Vim9 script
	    पक्ष ISN_LOADSCRIPT:
		{
		    scriptref_T	*sref = iptr->isn_arg.script.scriptref;
		    svar_T	 *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    यदि (sv == रिक्त)
			गच्छ theend;
		    allocate_if_null(sv);
		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    copy_tv(sv->sv_tv, थाक_TV_BOT(0));
		    ++ectx->ec_थाक.ga_len;
		}
		रोक;

	    // load s: variable in old script or autoload import
	    पक्ष ISN_LOADS:
	    पक्ष ISN_LOADEXPORT:
		{
		    पूर्णाङ्क		sid = iptr->isn_arg.loadstore.ls_sid;
		    hashtab_T	*ht = &SCRIPT_VARS(sid);
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = find_var_in_ht(ht, 0, name, TRUE);

		    यदि (di == रिक्त)
		    {
			ufunc_T	*ufunc = रिक्त;

			यदि (iptr->isn_type == ISN_LOADEXPORT)
			{
			    type_T	*type;

			    // Not a variable, it can be an exported function
			    // used as a value.
			    (शून्य)find_exported(sid, name, &ufunc, &type,
							 रिक्त, रिक्त, FALSE);
			}
			यदि (ufunc == रिक्त)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_undefined_variable_str), name);
			    गच्छ on_error;
			}
			यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			    गच्छ theend;
			tv = थाक_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_थाक.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(ufunc->uf_name);
		    }
		    अन्यथा
		    {
			यदि (iptr->isn_type == ISN_LOADEXPORT)
			{
			    पूर्णाङ्क idx = get_script_item_idx(sid, name, 0,
								   रिक्त, रिक्त);
			    svar_T	*sv;

			    यदि (idx >= 0)
			    {
				sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;
				यदि ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    SOURCING_LNUM = iptr->isn_lnum;
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    गच्छ on_error;
				}
			    }
			}
			यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			    गच्छ theend;
			copy_tv(&di->di_tv, थाक_TV_BOT(0));
			++ectx->ec_थाक.ga_len;
		    }
		}
		रोक;

	    // load g:/b:/w:/t: variable
	    पक्ष ISN_LOADG:
	    पक्ष ISN_LOADB:
	    पक्ष ISN_LOADW:
	    पक्ष ISN_LOADT:
		{
		    पूर्णाङ्क res = load_namespace_var(ectx, iptr->isn_type, iptr);

		    यदि (res == NOTDONE)
			गच्छ theend;
		    यदि (res == FAIL)
			गच्छ on_error;
		}

		रोक;

	    // load autoload variable
	    पक्ष ISN_LOADAUTO:
		{
		    char_u *name = iptr->isn_arg.string;

		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (eval_variable(name, (पूर्णाङ्क)STRLEN(name), 0,
			      थाक_TV_BOT(0), रिक्त, EVAL_VAR_VERBOSE) == FAIL)
		    {
			tv = थाक_TV_BOT(0);
			tv->v_type = VAR_सङ्ख्या;
			tv->v_lock = 0;
			tv->vval.v_सङ्ख्या = 0;
			++ectx->ec_थाक.ga_len;
			गच्छ on_error;
		    }
		    ++ectx->ec_थाक.ga_len;
		}
		रोक;

	    // load g:/b:/w:/t: namespace
	    पक्ष ISN_LOADGDICT:
	    पक्ष ISN_LOADBDICT:
	    पक्ष ISN_LOADWDICT:
	    पक्ष ISN_LOADTDICT:
		{
		    dict_T *d = रिक्त;

		    विकल्प (iptr->isn_type)
		    {
			पक्ष ISN_LOADGDICT: d = get_globvar_dict(); रोक;
			पक्ष ISN_LOADBDICT: d = curbuf->b_vars; रोक;
			पक्ष ISN_LOADWDICT: d = curwin->w_vars; रोक;
			पक्ष ISN_LOADTDICT: d = curtab->tp_vars; रोक;
			स्वभाव:  // Cannot reach here
			    गच्छ theend;
		    }
		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    tv = थाक_TV_BOT(0);
		    tv->v_type = VAR_DICT;
		    tv->v_lock = 0;
		    tv->vval.v_dict = d;
		    ++d->dv_refcount;
		    ++ectx->ec_थाक.ga_len;
		}
		रोक;

	    // load &option
	    पक्ष ISN_LOADOPT:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    // This is not expected to fail, name is checked during
		    // compilation: don't set SOURCING_LNUM.
		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    यदि (eval_option(&name, &optval, TRUE) == FAIL)
			गच्छ theend;
		    *थाक_TV_BOT(0) = optval;
		    ++ectx->ec_थाक.ga_len;
		}
		रोक;

	    // load $ENV
	    पक्ष ISN_LOADENV:
		{
		    typval_T	optval;
		    char_u	*name = iptr->isn_arg.string;

		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    // name is always valid, checked when compiling
		    (शून्य)eval_env_var(&name, &optval, TRUE);
		    *थाक_TV_BOT(0) = optval;
		    ++ectx->ec_थाक.ga_len;
		}
		रोक;

	    // load @register
	    पक्ष ISN_LOADREG:
		यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    गच्छ theend;
		tv = थाक_TV_BOT(0);
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		// This may result in NULL, which should be equivalent to an
		// empty string.
		tv->vval.v_string = get_reg_contents(
					  iptr->isn_arg.सङ्ख्या, GREG_अभिव्यक्ति_SRC);
		++ectx->ec_थाक.ga_len;
		रोक;

	    // store local variable
	    पक्ष ISN_STORE:
		--ectx->ec_थाक.ga_len;
		tv = थाक_TV_VAR(iptr->isn_arg.सङ्ख्या);
		यदि (check_typval_is_मान(थाक_TV_BOT(0)) == FAIL
			|| थाक_TV_BOT(0)->v_type == VAR_VOID)
		{
		    यदि (थाक_TV_BOT(0)->v_type == VAR_VOID)
			emsg(_(e_cannot_use_void_मान));
		    clear_tv(थाक_TV_BOT(0));
		    गच्छ on_error;
		}
		clear_tv(tv);
		*tv = *थाक_TV_BOT(0);
		रोक;

	    // store s: variable in old script or autoload import
	    पक्ष ISN_STORES:
	    पक्ष ISN_STOREEXPORT:
		{
		    पूर्णाङ्क		sid = iptr->isn_arg.loadstore.ls_sid;
		    char_u	*name = iptr->isn_arg.loadstore.ls_name;
		    dictitem_T	*di = रिक्त;
		    // First check for a variable from an exported autoload
		    // with an autoload_prefix; it would be in globals.
		    यदि (iptr->isn_type == ISN_STOREEXPORT)
			di = find_var_autoload_prefix(name, sid, रिक्त, रिक्त);
		    // Then look for a variable in the script's variables.
		    यदि (di == रिक्त)
		    {
			hashtab_T	*ht = &SCRIPT_VARS(sid);
			di = find_var_in_ht(ht, 0, STRNCMP("s:", name, 2) == 0
						     ? name + 2 : name, TRUE);
		    }

		    --ectx->ec_थाक.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (di == रिक्त)
		    {
			यदि (iptr->isn_type == ISN_STOREEXPORT)
			{
			    semsg(_(e_undefined_variable_str), name);
			    clear_tv(थाक_TV_BOT(0));
			    गच्छ on_error;
			}
			store_var(name, थाक_TV_BOT(0));
		    }
		    अन्यथा
		    {
			यदि (iptr->isn_type == ISN_STOREEXPORT)
			{
			    पूर्णाङ्क idx = get_script_item_idx(sid, name, 0,
								   रिक्त, रिक्त);

			    // can this ever fail?
			    यदि (idx >= 0)
			    {
				svar_T	*sv = ((svar_T *)SCRIPT_ITEM(sid)
						  ->sn_var_vals.ga_data) + idx;

				यदि ((sv->sv_flags & SVFLAG_EXPORTED) == 0)
				{
				    semsg(_(e_item_not_exported_in_script_str),
									 name);
				    clear_tv(थाक_TV_BOT(0));
				    गच्छ on_error;
				}
			    }
			}
			यदि (var_check_permission(di, name) == FAIL)
			{
			    clear_tv(थाक_TV_BOT(0));
			    गच्छ on_error;
			}
			clear_tv(&di->di_tv);
			di->di_tv = *थाक_TV_BOT(0);
		    }
		}
		रोक;

	    // store script-local variable in Vim9 script
	    पक्ष ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    svar_T	    *sv;

		    sv = get_script_svar(sref, ectx->ec_dfunc_idx);
		    यदि (sv == रिक्त)
			गच्छ theend;
		    --ectx->ec_थाक.ga_len;

		    // "const" and "final" are checked at compile time, locking
		    // the value needs to be checked here.
		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (मान_check_lock(sv->sv_tv->v_lock, sv->sv_name, FALSE))
		    {
			clear_tv(थाक_TV_BOT(0));
			गच्छ on_error;
		    }

		    clear_tv(sv->sv_tv);
		    *sv->sv_tv = *थाक_TV_BOT(0);
		}
		रोक;

	    // store option
	    पक्ष ISN_STOREOPT:
	    पक्ष ISN_STOREFUNCOPT:
		{
		    char_u	*opt_name = iptr->isn_arg.storeopt.so_name;
		    पूर्णाङ्क		opt_flags = iptr->isn_arg.storeopt.so_flags;
		    दीर्घ	n = 0;
		    char_u	*s = रिक्त;
		    वर्ण	*सन्देश;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree = रिक्त;

		    --ectx->ec_थाक.ga_len;
		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = थाक_TV_BOT(0);
		    यदि (tv->v_type == VAR_STRING)
		    {
			s = tv->vval.v_string;
			यदि (s == रिक्त)
			    s = (char_u *)"";
		    }
		    अन्यथा यदि (iptr->isn_type == ISN_STOREFUNCOPT)
		    {
			// If the option can be set to a function reference or
			// a lambda and the passed value is a function
			// reference, then convert it to the name (string) of
			// the function reference.
			s = tv2string(tv, &tofree, numbuf, 0);
			यदि (s == रिक्त || *s == NUL)
			{
			    // cannot happen?
			    clear_tv(tv);
			    vim_free(tofree);
			    गच्छ on_error;
			}
		    }
		    अन्यथा
			// must be VAR_NUMBER, CHECKTYPE makes sure
			n = tv->vval.v_सङ्ख्या;
		    सन्देश = set_option_मान(opt_name, n, s, opt_flags);
		    clear_tv(tv);
		    vim_free(tofree);
		    यदि (सन्देश != रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(सन्देश));
			गच्छ on_error;
		    }
		}
		रोक;

	    // store $ENV
	    पक्ष ISN_STOREENV:
		यदि (check_secure() || check_restricted())
		    गच्छ theend;
		--ectx->ec_थाक.ga_len;
		tv = थाक_TV_BOT(0);
		vim_setenv_ext(iptr->isn_arg.string, tv_get_string(tv));
		clear_tv(tv);
		रोक;

	    // store @r
	    पक्ष ISN_STOREREG:
		{
		    पूर्णाङ्क	reg = iptr->isn_arg.सङ्ख्या;

		    --ectx->ec_थाक.ga_len;
		    tv = थाक_TV_BOT(0);
		    write_reg_contents(reg, tv_get_string(tv), -1, FALSE);
		    clear_tv(tv);
		}
		रोक;

	    // store v: variable
	    पक्ष ISN_STOREV:
		--ectx->ec_थाक.ga_len;
		यदि (set_vim_var_tv(iptr->isn_arg.सङ्ख्या, थाक_TV_BOT(0))
								       == FAIL)
		    // should not happen, type is checked when compiling
		    गच्छ on_error;
		रोक;

	    // store g:/b:/w:/t: variable
	    पक्ष ISN_STOREG:
	    पक्ष ISN_STOREB:
	    पक्ष ISN_STOREW:
	    पक्ष ISN_STORET:
		{
		    dictitem_T	*di;
		    hashtab_T	*ht;
		    char_u	*name = iptr->isn_arg.string + 2;

		    विकल्प (iptr->isn_type)
		    {
			पक्ष ISN_STOREG:
			    ht = get_globvar_ht();
			    रोक;
			पक्ष ISN_STOREB:
			    ht = &curbuf->b_vars->dv_hashtab;
			    रोक;
			पक्ष ISN_STOREW:
			    ht = &curwin->w_vars->dv_hashtab;
			    रोक;
			पक्ष ISN_STORET:
			    ht = &curtab->tp_vars->dv_hashtab;
			    रोक;
			स्वभाव:  // Cannot reach here
			    गच्छ theend;
		    }

		    --ectx->ec_थाक.ga_len;
		    di = find_var_in_ht(ht, 0, name, TRUE);
		    यदि (di == रिक्त)
			store_var(iptr->isn_arg.string, थाक_TV_BOT(0));
		    अन्यथा
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			यदि (var_check_permission(di, name) == FAIL)
			    गच्छ on_error;
			clear_tv(&di->di_tv);
			di->di_tv = *थाक_TV_BOT(0);
		    }
		}
		रोक;

	    // store an autoload variable
	    पक्ष ISN_STOREAUTO:
		SOURCING_LNUM = iptr->isn_lnum;
		set_var(iptr->isn_arg.string, थाक_TV_BOT(-1), TRUE);
		clear_tv(थाक_TV_BOT(-1));
		--ectx->ec_थाक.ga_len;
		रोक;

	    // store number in local variable
	    पक्ष ISN_STORENR:
		tv = थाक_TV_VAR(iptr->isn_arg.storenr.stnr_idx);
		clear_tv(tv);
		tv->v_type = VAR_सङ्ख्या;
		tv->vval.v_सङ्ख्या = iptr->isn_arg.storenr.stnr_val;
		रोक;

	    // Store a value in a list, tuple, dict, blob or object variable.
	    पक्ष ISN_STOREINDEX:
		{
		    पूर्णाङ्क res = execute_storeindex(iptr, ectx);

		    यदि (res == FAIL)
			गच्छ on_error;
		    यदि (res == NOTDONE)
			गच्छ theend;
		}
		रोक;

	    // store value in list or blob range
	    पक्ष ISN_STORERANGE:
		यदि (execute_storerange(iptr, ectx) == FAIL)
		    गच्छ on_error;
		रोक;

	    पक्ष ISN_LOAD_CLASSMEMBER:
		{
		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    copy_tv(cm->cm_class->class_members_tv + cm->cm_idx,
							      थाक_TV_BOT(0));
		    ++ectx->ec_थाक.ga_len;
		}
		रोक;

	    पक्ष ISN_STORE_CLASSMEMBER:
		{
		    classmember_T *cm = &iptr->isn_arg.classmember;
		    tv = &cm->cm_class->class_members_tv[cm->cm_idx];
		    clear_tv(tv);
		    *tv = *थाक_TV_BOT(-1);
		    --ectx->ec_थाक.ga_len;
		}
		रोक;

	    // Load or store variable or argument from outer scope.
	    पक्ष ISN_LOADOUTER:
	    पक्ष ISN_STOREOUTER:
		{
		    पूर्णाङ्क		depth = iptr->isn_arg.outer.outer_depth;
		    outer_T	*outer = ectx->ec_outer_ref == रिक्त ? रिक्त
						: ectx->ec_outer_ref->or_outer;

		    जबसम्म (depth > 1 && outer != रिक्त)
		    {
			outer = outer->out_up;
			--depth;
		    }
		    यदि (outer == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			यदि (ectx->ec_frame_idx == ectx->ec_initial_frame_idx
						 || ectx->ec_outer_ref == रिक्त)
			    // Possibly :def function called from legacy
			    // context.
			    emsg(_(e_closure_called_from_invalid_context));
			अन्यथा
			    iemsg("LOADOUTER depth more than scope levels");
			गच्छ theend;
		    }
		    यदि (depth < 0)
			// Variable declared in loop.  May be copied if the
			// loop block has already ended.
			tv = ((typval_T *)outer->out_loop[-depth - 1]
							       .थाक->ga_data)
					  + outer->out_loop[-depth - 1].var_idx
					  + iptr->isn_arg.outer.outer_idx;
		    अन्यथा
			// Variable declared in a function.  May be copied if
			// the function has already returned.
			tv = ((typval_T *)outer->out_थाक->ga_data)
				      + outer->out_frame_idx + थाक_FRAME_SIZE
				      + iptr->isn_arg.outer.outer_idx;
		    यदि (iptr->isn_type == ISN_LOADOUTER)
		    {
			typval_T *copy;
			यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			    गच्छ theend;
			// careful: ga_grow_inner may re-alloc the stack
			यदि (depth < 0)
			    copy = ((typval_T *)outer->out_loop[-depth - 1]
								   .थाक->ga_data)
					      + outer->out_loop[-depth - 1].var_idx
					      + iptr->isn_arg.outer.outer_idx;
			अन्यथा
			    copy = ((typval_T *)outer->out_थाक->ga_data)
					  + outer->out_frame_idx + थाक_FRAME_SIZE
					  + iptr->isn_arg.outer.outer_idx;
			// memory was freed, get tv again
			यदि (copy != tv)
			    tv = copy;
			copy_tv(tv, थाक_TV_BOT(0));
			++ectx->ec_थाक.ga_len;
		    }
		    अन्यथा
		    {
			--ectx->ec_थाक.ga_len;
			clear_tv(tv);
			*tv = *थाक_TV_BOT(0);
		    }
		}
		रोक;

	    // unlet item in list or dict variable
	    पक्ष ISN_UNLETINDEX:
		यदि (execute_unletindex(iptr, ectx) == FAIL)
		    गच्छ on_error;
		रोक;

	    // unlet range of items in list variable
	    पक्ष ISN_UNLETRANGE:
		यदि (execute_unletrange(iptr, ectx) == FAIL)
		    गच्छ on_error;
		रोक;

	    // push constant
	    पक्ष ISN_PUSHNR:
	    पक्ष ISN_PUSHBOOL:
	    पक्ष ISN_PUSHSPEC:
	    पक्ष ISN_PUSHF:
	    पक्ष ISN_PUSHS:
	    पक्ष ISN_PUSHBLOB:
	    पक्ष ISN_PUSHFUNC:
	    पक्ष ISN_PUSHCHANNEL:
	    पक्ष ISN_PUSHJOB:
	    पक्ष ISN_PUSHOBJ:
	    पक्ष ISN_PUSHCLASS:
		यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    गच्छ theend;
		tv = थाक_TV_BOT(0);
		tv->v_lock = 0;
		++ectx->ec_थाक.ga_len;
		विकल्प (iptr->isn_type)
		{
		    पक्ष ISN_PUSHNR:
			tv->v_type = VAR_सङ्ख्या;
			tv->vval.v_सङ्ख्या = iptr->isn_arg.सङ्ख्या;
			रोक;
		    पक्ष ISN_PUSHBOOL:
			tv->v_type = VAR_BOOL;
			tv->vval.v_सङ्ख्या = iptr->isn_arg.सङ्ख्या;
			रोक;
		    पक्ष ISN_PUSHSPEC:
			tv->v_type = VAR_SPECIAL;
			tv->vval.v_सङ्ख्या = iptr->isn_arg.सङ्ख्या;
			रोक;
		    पक्ष ISN_PUSHF:
			tv->v_type = VAR_FLOAT;
			tv->vval.v_float = iptr->isn_arg.fnumber;
			रोक;
		    पक्ष ISN_PUSHBLOB:
			tv->v_type = VAR_BLOB;
			tv->vval.v_blob = blob_copy(iptr->isn_arg.blob);
			रोक;
		    पक्ष ISN_PUSHFUNC:
			tv->v_type = VAR_FUNC;
			यदि (iptr->isn_arg.string == रिक्त)
			    tv->vval.v_string = रिक्त;
			अन्यथा
			    tv->vval.v_string =
					     vim_strsave(iptr->isn_arg.string);
			रोक;
		    पक्ष ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_CHANNEL;
			tv->vval.v_channel = रिक्त;
#endif
			रोक;
		    पक्ष ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
			tv->v_type = VAR_JOB;
			tv->vval.v_job = रिक्त;
#endif
			रोक;
		    पक्ष ISN_PUSHOBJ:
			tv->v_type = VAR_OBJECT;
			tv->vval.v_object = रिक्त;
			रोक;
		    पक्ष ISN_PUSHCLASS:
			tv->v_type = VAR_CLASS;
			tv->vval.v_class = iptr->isn_arg.classarg;
			रोक;
		    स्वभाव:
			tv->v_type = VAR_STRING;
			tv->vval.v_string = iptr->isn_arg.string == रिक्त
				    ? रिक्त : vim_strsave(iptr->isn_arg.string);
		}
		रोक;

	    पक्ष ISN_AUTOLOAD:
		{
		    char_u  *name = iptr->isn_arg.string;

		    (शून्य)script_autoload(name, FALSE);
		    यदि (find_func(name, TRUE))
		    {
			यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			    गच्छ theend;
			tv = थाक_TV_BOT(0);
			tv->v_lock = 0;
			++ectx->ec_थाक.ga_len;
			tv->v_type = VAR_FUNC;
			tv->vval.v_string = vim_strsave(name);
		    }
		    अन्यथा
		    {
			पूर्णाङ्क res = load_namespace_var(ectx, ISN_LOADG, iptr);

			यदि (res == NOTDONE)
			    गच्छ theend;
			यदि (res == FAIL)
			    गच्छ on_error;
		    }
		}
		रोक;

	    पक्ष ISN_UNLET:
		यदि (do_unlet(iptr->isn_arg.unlet.ul_name,
				       iptr->isn_arg.unlet.ul_forceit) == FAIL)
		    गच्छ on_error;
		रोक;
	    पक्ष ISN_UNLETENV:
		vim_unsetenv_ext(iptr->isn_arg.unlet.ul_name);
		रोक;

	    पक्ष ISN_LOCKUNLOCK:
		{
#ifdef LOG_LOCKVAR
		    ch_log(रिक्त, "LKVAR: execute INS_LOCKUNLOCK isn_arg %s",
							iptr->isn_arg.string);
#endif
		    lval_जरा_T	*lval_जरा_save = lval_जरा;

		    // Stack has the local variable, argument the whole :lock
		    // or :unlock command, like ISN_EXEC.
		    --ectx->ec_थाक.ga_len;
		    lval_जरा_T जरा;
		    जरा.lr_tv      = थाक_TV_BOT(0);
		    जरा.lr_cl_exec = iptr->isn_arg.lockunlock.lu_cl_exec;
		    जरा.lr_is_arg  = iptr->isn_arg.lockunlock.lu_is_arg;
		    lval_जरा = &जरा;
		    पूर्णाङ्क res = exec_command(iptr,
					iptr->isn_arg.lockunlock.lu_string);
		    clear_tv(जरा.lr_tv);
		    lval_जरा = lval_जरा_save;
		    यदि (res == FAIL)
			गच्छ on_error;
		}
		रोक;

	    पक्ष ISN_LOCKCONST:
		item_lock(थाक_TV_BOT(-1), 100, TRUE, TRUE);
		रोक;

	    // create a list from items on the stack; uses a single allocation
	    // for the list header and the items
	    पक्ष ISN_NEWLIST:
		यदि (exe_newlist(iptr->isn_arg.सङ्ख्या, ectx) == FAIL)
		    गच्छ theend;
		रोक;

	    // create a tuple from items on the stack
	    पक्ष ISN_NEWTUPLE:
		यदि (exe_newtuple(iptr->isn_arg.सङ्ख्या, ectx) == FAIL)
		    गच्छ theend;
		रोक;

	    // create a dict from items on the stack
	    पक्ष ISN_NEWDICT:
		{
		    पूर्णाङ्क res;

		    SOURCING_LNUM = iptr->isn_lnum;
		    res = exe_newdict(iptr->isn_arg.सङ्ख्या, ectx);
		    यदि (res == FAIL)
			गच्छ theend;
		    यदि (res == MAYBE)
			गच्छ on_error;
		}
		रोक;

	    पक्ष ISN_CONCAT:
		यदि (exe_concat(iptr->isn_arg.सङ्ख्या, ectx) == FAIL)
		    गच्छ theend;
		रोक;

	    // create a partial with NULL value
	    पक्ष ISN_NEWPARTIAL:
		यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    गच्छ theend;
		++ectx->ec_थाक.ga_len;
		tv = थाक_TV_BOT(-1);
		tv->v_type = VAR_PARTIAL;
		tv->v_lock = 0;
		tv->vval.v_partial = रिक्त;
		रोक;

	    // call a :def function
	    पक्ष ISN_DCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		यदि (call_dfunc(iptr->isn_arg.dfunc.cdf_idx,
				रिक्त,
				iptr->isn_arg.dfunc.cdf_argcount,
				ectx) == FAIL)
		    गच्छ on_error;
		रोक;

	    // call a method on an interface
	    पक्ष ISN_METHODCALL:
		{
		    cmfunc_T *mfunc = iptr->isn_arg.mfunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    tv = थाक_TV_BOT(-1 - mfunc->cmf_argcount);
		    यदि (tv->v_type != VAR_OBJECT)
		    {
			object_required_error(tv);
			गच्छ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    यदि (obj == रिक्त)
		    {
			emsg(_(e_using_null_object));
			गच्छ on_error;
		    }

		    ufunc_T *ufunc;
		    class_T *cl;
		    यदि (mfunc->cmf_is_super)
		    {
			// Doing "super.Func", use the specific ufunc.
			cl = mfunc->cmf_itf;
			ufunc = cl->class_obj_methods[mfunc->cmf_idx];
		    }
		    अन्यथा
		    {
			cl = obj->obj_class;

			// convert the interface index to the object index
			पूर्णाङ्क idx = object_index_from_itf_index(mfunc->cmf_itf,
						     TRUE, mfunc->cmf_idx, cl);
			ufunc = cl->class_obj_methods[idx];
		    }

		    // A generic method: use the method with the type arguments
		    // used when compiling.  The last one is cached, since
		    // looking it up is slow.
		    यदि (mfunc->cmf_ufunc != रिक्त && IS_GENERIC_FUNC(ufunc))
		    {
			यदि (mfunc->cmf_cache_cl == cl
				&& mfunc->cmf_cache_generic == ufunc)
			    ufunc = mfunc->cmf_cache_ufunc;
			अन्यथा
			{
			    ufunc_T *generic_ufunc = ufunc;

			    ufunc = generic_func_get_same_types(generic_ufunc,
							    mfunc->cmf_ufunc);
			    यदि (ufunc == रिक्त)
				गच्छ on_error;

			    ++cl->class_refcount;
			    class_unref(mfunc->cmf_cache_cl);
			    mfunc->cmf_cache_cl = cl;
			    mfunc->cmf_cache_generic = generic_ufunc;
			    mfunc->cmf_cache_ufunc = ufunc;
			}
		    }

		    यदि (call_ufunc(ufunc, रिक्त, mfunc->cmf_argcount, ectx,
							   रिक्त, रिक्त) == FAIL)
			गच्छ on_error;
		}
		रोक;

	    // call a builtin function
	    पक्ष ISN_BCALL:
		SOURCING_LNUM = iptr->isn_lnum;
		यदि (call_bfunc(iptr->isn_arg.bfunc.cbf_idx,
			      iptr->isn_arg.bfunc.cbf_argcount,
			      ectx) == FAIL)
		    गच्छ on_error;
		रोक;

	    // call a funcref or partial
	    पक्ष ISN_PCALL:
		{
		    cpfunc_T	*pfunc = &iptr->isn_arg.pfunc;
		    पूर्णाङ्क		r;
		    typval_T	partial_tv;

		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (pfunc->cpf_शीर्ष)
		    {
			// funcref is above the arguments
			tv = थाक_TV_BOT(-pfunc->cpf_argcount - 1);
		    }
		    अन्यथा
		    {
			// Get the funcref from the stack.
			--ectx->ec_थाक.ga_len;
			partial_tv = *थाक_TV_BOT(0);
			tv = &partial_tv;
		    }
		    r = call_partial(tv, pfunc->cpf_argcount, ectx);
		    यदि (tv == &partial_tv)
			clear_tv(&partial_tv);
		    यदि (r == FAIL)
			गच्छ on_error;
		}
		रोक;

	    पक्ष ISN_PCALL_END:
		// PCALL finished, arguments have been consumed and replaced by
		// the return value.  Now clear the funcref from the stack,
		// and move the return value in its place.
		--ectx->ec_थाक.ga_len;
		clear_tv(थाक_TV_BOT(-1));
		*थाक_TV_BOT(-1) = *थाक_TV_BOT(0);
		रोक;

	    // call a user defined function or funcref/partial
	    पक्ष ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (call_eval_func(cufunc->cuf_name, cufunc->cuf_argcount,
							   ectx, iptr) == FAIL)
			गच्छ on_error;
		}
		रोक;

	    // :defer func(arg)
	    पक्ष ISN_DEFER:
		यदि (defer_command(iptr->isn_arg.defer.defer_var_idx,
			     iptr->isn_arg.defer.defer_argcount, ectx) == FAIL)
		    गच्छ on_error;
		रोक;

	    // Return from a :def function call without a value.
	    // Return from a constructor.
	    पक्ष ISN_RETURN_VOID:
	    पक्ष ISN_RETURN_OBJECT:
		यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    गच्छ theend;
		tv = थाक_TV_BOT(0);
		++ectx->ec_थाक.ga_len;
		यदि (iptr->isn_type == ISN_RETURN_VOID)
		{
		    tv->v_type = VAR_VOID;
		    tv->vval.v_सङ्ख्या = 0;
		    tv->v_lock = 0;
		}
		अन्यथा
		{
		    *tv = *थाक_TV_VAR(0);
		    object_T *obj = tv->vval.v_object;
		    ++obj->obj_refcount;

		    // Lock all the constant object variables
		    obj_lock_const_vars(obj);
		}
		// FALLTHROUGH

	    // return from a :def function call with what is on the stack
	    पक्ष ISN_RETURN:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = रिक्त;

		    ///////////////////////////////////////////////////
		    // TODO: If FAIL, line number in output not correct
		    ///////////////////////////////////////////////////
		    यदि (check_typval_is_मान(थाक_TV_BOT(-1)) == FAIL)
			गच्छ theend;

		    यदि (trystack->ga_len > 0)
			trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
		    यदि (trycmd != रिक्त
				 && trycmd->tcd_frame_idx == ectx->ec_frame_idx)
		    {
			// jump to ":finally" or ":endtry"
			यदि (trycmd->tcd_finally_idx != 0)
			    ectx->ec_iidx = trycmd->tcd_finally_idx;
			अन्यथा
			    ectx->ec_iidx = trycmd->tcd_endtry_idx;
			trycmd->tcd_return = TRUE;
		    }
		    अन्यथा
			गच्छ func_return;
		}
		रोक;

	    // push a partial, a reference to a compiled function
	    पक्ष ISN_FUNCREF:
		{
		    partial_T	    *pt = ALLOC_CLEAR_ONE(partial_T);
		    ufunc_T	    *ufunc;
		    funcref_T	    *funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T *extra = funcref->fr_extra;

		    यदि (pt == रिक्त)
			गच्छ theend;
		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    {
			vim_free(pt);
			गच्छ theend;
		    }
		    यदि (extra != रिक्त && extra->fre_class != रिक्त)
		    {
			class_T	*cl;
			यदि (extra->fre_object_method)
			{
			    tv = थाक_TV_BOT(-1);
			    यदि (tv->v_type != VAR_OBJECT)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				object_required_error(tv);
				vim_free(pt);
				गच्छ on_error;
			    }

			    object_T *obj = tv->vval.v_object;
			    यदि (obj == रिक्त)
			    {
				SOURCING_LNUM = iptr->isn_lnum;
				emsg(_(e_using_null_object));
				vim_free(pt);
				गच्छ on_error;
			    }

			    cl = obj->obj_class;
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_थाक.ga_len;

			    pt->pt_obj = obj;
			    ++obj->obj_refcount;
			}
			अन्यथा
			    cl = extra->fre_class;

			यदि (extra->fre_object_method)
			{
			    // object method
			    // convert the interface index to the object index
			    पूर्णाङ्क idx =
				object_index_from_itf_index(extra->fre_class,
					TRUE, extra->fre_method_idx, cl);
			    ufunc = cl->class_obj_methods[idx];
			}
			अन्यथा
			{
			    // class method
			    ufunc =
				cl->class_class_functions[extra->fre_method_idx];
			}
		    }
		    अन्यथा यदि (extra == रिक्त || extra->fre_func_name == रिक्त)
		    {
			dfunc_T	*pt_dfunc = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;

			ufunc = pt_dfunc->df_ufunc;
		    }
		    अन्यथा
		    {
			ufunc = find_func(extra->fre_func_name, FALSE);
		    }
		    यदि (ufunc == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			iemsg("ufunc unexpectedly NULL for FUNCREF");
			vim_free(pt);
			गच्छ theend;
		    }
		    यदि (extra != रिक्त && extra->fre_outer_depth > 0
					   && (ufunc->uf_flags & FC_CLOSURE))
		    {
			// Use the context of the function where the nested
			// generic function is defined.
			partial_T *outer_pt = ectx->ec_outer_ref == रिक्त
				    ? रिक्त : ectx->ec_outer_ref->or_partial;

			लागि (पूर्णाङ्क depth = 1; outer_pt != रिक्त
				    && depth < extra->fre_outer_depth; ++depth)
			    outer_pt = get_pt_outer(outer_pt)->out_up_partial;
			यदि (outer_pt == रिक्त)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    iemsg("outer partial unexpectedly NULL for FUNCREF");
			    vim_free(pt);
			    गच्छ theend;
			}
			pt->pt_func = ufunc;
			pt->pt_refcount = 1;
			pt->pt_outer_partial = outer_pt;
			++outer_pt->pt_refcount;
			++ufunc->uf_refcount;
		    }
		    अन्यथा यदि (fill_partial_and_closure(pt, ufunc,
			       extra == रिक्त ? रिक्त : &extra->fre_loopvar_info,
								 ectx) == FAIL)
		    {
			vim_free(pt);
			गच्छ theend;
		    }
		    tv = थाक_TV_BOT(0);
		    ++ectx->ec_थाक.ga_len;
		    tv->vval.v_partial = pt;
		    tv->v_type = VAR_PARTIAL;
		    tv->v_lock = 0;
		}
		रोक;

	    // Create a global function from a lambda.
	    पक्ष ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    यदि (copy_lambda_to_global_func(arg->nfa_lambda,
				       arg->nfa_global, &arg->nfa_loopvar_info,
				       ectx) == FAIL)
			गच्छ theend;
		}
		रोक;

	    // List functions
	    पक्ष ISN_DEF:
		यदि (iptr->isn_arg.string == रिक्त)
		    list_functions(रिक्त);
		अन्यथा
		{
		    exarg_T	ea;
		    garray_T	पङ्क्तिहरू_to_free;

		    CLEAR_FIELD(ea);
		    ea.cmd = ea.arg = iptr->isn_arg.string;
		    ga_init2(&पङ्क्तिहरू_to_free, परिमाण(char_u *), 50);
		    SOURCING_LNUM = iptr->isn_lnum;
		    define_function(&ea, रिक्त, &पङ्क्तिहरू_to_free, 0, रिक्त, 0, रिक्त);
		    ga_clear_strings(&पङ्क्तिहरू_to_free);
		}
		रोक;

	    // jump if a condition is met
	    पक्ष ISN_JUMP:
		{
		    jumpwhen_T	when = iptr->isn_arg.jump.jump_when;
		    पूर्णाङ्क		error = FALSE;
		    पूर्णाङ्क		jump = TRUE;

		    यदि (when != JUMP_ALWAYS)
		    {
			tv = थाक_TV_BOT(-1);
			यदि (when == JUMP_IF_COND_FALSE
				|| when == JUMP_IF_FALSE
				|| when == JUMP_IF_COND_TRUE)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    jump = tv_get_bool_chk(tv, &error);
			    यदि (error)
				गच्छ on_error;
			}
			अन्यथा
			    jump = tv2bool(tv);
			यदि (when == JUMP_IF_FALSE || when == JUMP_IF_COND_FALSE)
			    jump = !jump;
			यदि (when == JUMP_IF_FALSE || !jump)
			{
			    // drop the value from the stack
			    clear_tv(tv);
			    --ectx->ec_थाक.ga_len;
			}
		    }
		    यदि (jump)
			ectx->ec_iidx = iptr->isn_arg.jump.jump_where;
		}
		रोक;

	    // "while": jump to end if a condition is false
	    पक्ष ISN_WHILE:
		{
		    पूर्णाङ्क		error = FALSE;
		    पूर्णाङ्क		jump = TRUE;

		    tv = थाक_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    jump = !tv_get_bool_chk(tv, &error);
		    यदि (error)
			गच्छ on_error;
		    // drop the value from the stack
		    clear_tv(tv);
		    --ectx->ec_थाक.ga_len;
		    यदि (jump)
			ectx->ec_iidx = iptr->isn_arg.whileloop.while_end;

		    // Store the current funcref count, may be used by
		    // ISN_ENDLOOP later
		    tv = थाक_TV_VAR(
				    iptr->isn_arg.whileloop.while_funcref_idx);
		    tv->vval.v_सङ्ख्या = ectx->ec_funcrefs.ga_len;
		}
		रोक;

	    // Jump if an argument with a default value was already set and not
	    // v:none.
	    पक्ष ISN_JUMP_IF_ARG_SET:
	    पक्ष ISN_JUMP_IF_ARG_NOT_SET:
		tv = थाक_TV_VAR(iptr->isn_arg.jumparg.jump_arg_off);
		पूर्णाङ्क arg_set = tv->v_type != VAR_UNKNOWN
				&& !(tv->v_type == VAR_SPECIAL
					    && tv->vval.v_सङ्ख्या == VVAL_NONE);

		यदि (iptr->isn_type == ISN_JUMP_IF_ARG_NOT_SET && !arg_set)
		{
		    dfunc_T *df = ((dfunc_T *)def_functions.ga_data)
							+ ectx->ec_dfunc_idx;
		    ufunc_T *ufunc = df->df_ufunc;
		    // jump_arg_off is negative for arguments
		    माप_प्रकार argidx = ufunc->uf_def_args.ga_len
					+ iptr->isn_arg.jumparg.jump_arg_off
					+ थाक_FRAME_SIZE;
		    type_T *type = ufunc->uf_arg_types[argidx];
		    CLEAR_POINTER(tv);
		    // "any" is not a type a value can have, leave the
		    // argument marked as not set.
		    यदि (type->tt_type != VAR_ANY)
			tv->v_type = type->tt_type;
		}

		यदि (iptr->isn_type == ISN_JUMP_IF_ARG_SET ? arg_set : !arg_set)
		    ectx->ec_iidx = iptr->isn_arg.jumparg.jump_where;
		रोक;

	    // top of a for loop
	    पक्ष ISN_FOR:
		यदि (execute_for(iptr, ectx) == FAIL)
		    गच्छ theend;
		रोक;

	    // end of a for or while loop
	    पक्ष ISN_ENDLOOP:
		यदि (execute_endloop(iptr, ectx) == FAIL)
		    गच्छ theend;
		रोक;

	    // start of ":try" block
	    पक्ष ISN_TRY:
		{
		    trycmd_T    *trycmd = रिक्त;

		    यदि (GA_GROW_FAILS(&ectx->ec_trystack, 1))
			गच्छ theend;
		    trycmd = ((trycmd_T *)ectx->ec_trystack.ga_data)
						     + ectx->ec_trystack.ga_len;
		    ++ectx->ec_trystack.ga_len;
		    ++trylevel;
		    CLEAR_POINTER(trycmd);
		    trycmd->tcd_frame_idx = ectx->ec_frame_idx;
		    trycmd->tcd_थाक_len = ectx->ec_थाक.ga_len;
		    trycmd->tcd_catch_idx =
				       iptr->isn_arg.tryref.try_ref->try_catch;
		    trycmd->tcd_finally_idx =
				     iptr->isn_arg.tryref.try_ref->try_finally;
		    trycmd->tcd_endtry_idx =
				      iptr->isn_arg.tryref.try_ref->try_endtry;
		}
		रोक;

	    पक्ष ISN_PUSHEXC:
		यदि (current_exception == रिक्त)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    iemsg("Evaluating catch while current_exception is NULL");
		    गच्छ theend;
		}
		यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
		    गच्छ theend;
		tv = थाक_TV_BOT(0);
		++ectx->ec_थाक.ga_len;
		tv->v_type = VAR_STRING;
		tv->v_lock = 0;
		tv->vval.v_string = vim_strsave(
					   (char_u *)current_exception->मान);
		रोक;

	    पक्ष ISN_CATCH:
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
		रोक;

	    पक्ष ISN_TRYCONT:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycont_T	*trycont = &iptr->isn_arg.trycont;
		    पूर्णाङ्क		i;
		    trycmd_T    *trycmd;
		    पूर्णाङ्क		iidx = trycont->tct_where;

		    यदि (trystack->ga_len < trycont->tct_levels)
		    {
			siemsg("TRYCONT: expected %d levels, found %d",
					trycont->tct_levels, trystack->ga_len);
			गच्छ theend;
		    }
		    // Make :endtry jump to any outer try block and the last
		    // :endtry inside the loop to the loop start.
		    लागि (i = trycont->tct_levels; i > 0; --i)
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
		रोक;

	    पक्ष ISN_FINALLY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;

		    // Reset the index to avoid a return statement jumps here
		    // again.
		    trycmd->tcd_finally_idx = 0;
		    यदि (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_थाक);
			trycmd->tcd_caught = FALSE;
		    }
		    रोक;
		}

	    // end of ":try" block
	    पक्ष ISN_ENDTRY:
		{
		    garray_T	*trystack = &ectx->ec_trystack;
		    trycmd_T    *trycmd;

		    --trystack->ga_len;
		    --trylevel;
		    trycmd = ((trycmd_T *)trystack->ga_data) + trystack->ga_len;
		    यदि (trycmd->tcd_did_throw)
			did_throw = TRUE;
		    यदि (trycmd->tcd_caught)
		    {
			// discard the exception
			finish_exception(caught_थाक);
		    }

		    यदि (trycmd->tcd_return)
			गच्छ func_return;

		    जबसम्म (ectx->ec_थाक.ga_len > trycmd->tcd_थाक_len)
		    {
			--ectx->ec_थाक.ga_len;
			clear_tv(थाक_TV_BOT(0));
		    }
		    यदि (trycmd->tcd_cont != 0)
			// handling :continue: jump to outer try block or
			// start of the loop
			ectx->ec_iidx = trycmd->tcd_cont - 1;
		}
		रोक;

	    पक्ष ISN_THROW:
		{
		    garray_T	*trystack = &ectx->ec_trystack;

		    यदि (trystack->ga_len == 0 && trylevel == 0 && emsg_silent)
		    {
			// Throwing an exception while using "silent!" causes
			// the function to abort but not display an error.
			tv = थाक_TV_BOT(-1);
			clear_tv(tv);
			tv->v_type = VAR_सङ्ख्या;
			tv->vval.v_सङ्ख्या = 0;
			गच्छ done;
		    }
		    --ectx->ec_थाक.ga_len;
		    tv = थाक_TV_BOT(0);
		    यदि (tv->vval.v_string == रिक्त
				       || *skipwhite(tv->vval.v_string) == NUL)
		    {
			vim_free(tv->vval.v_string);
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_throw_with_empty_string));
			गच्छ theend;
		    }

		    // Inside a "catch" we need to first discard the caught
		    // exception.
		    यदि (trystack->ga_len > 0)
		    {
			trycmd_T    *trycmd = ((trycmd_T *)trystack->ga_data)
							+ trystack->ga_len - 1;
			यदि (trycmd->tcd_caught)
			{
			    // discard the exception
			    finish_exception(caught_थाक);
			    trycmd->tcd_caught = FALSE;
			}
		    }

		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (throw_exception(tv->vval.v_string, ET_USER, रिक्त)
								       == FAIL)
		    {
			vim_free(tv->vval.v_string);
			गच्छ theend;
		    }
		    did_throw = TRUE;
		}
		रोक;

	    // compare with special values
	    पक्ष ISN_COMPAREBOOL:
	    पक्ष ISN_COMPARESPECIAL:
		{
		    typval_T	*tv1 = थाक_TV_BOT(-2);
		    typval_T	*tv2 = थाक_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_सङ्ख्या;
		    varnumber_T arg2 = tv2->vval.v_सङ्ख्या;
		    पूर्णाङ्क		res;

		    यदि (iptr->isn_arg.op.op_type == अभिव्यक्ति_EQUAL)
			res = arg1 == arg2;
		    अन्यथा
			res = arg1 != arg2;

		    --ectx->ec_थाक.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_सङ्ख्या = res ? VVAL_TRUE : VVAL_FALSE;
		}
		रोक;

	    पक्ष ISN_COMPARENULL:
		{
		    typval_T	*tv1 = थाक_TV_BOT(-2);
		    typval_T	*tv2 = थाक_TV_BOT(-1);
		    पूर्णाङ्क		res;

		    res = typval_compare_null(tv1, tv2);
		    यदि (res == MAYBE)
			गच्छ on_error;
		    यदि (iptr->isn_arg.op.op_type == अभिव्यक्ति_NEQUAL)
			res = !res;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    --ectx->ec_थाक.ga_len;
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_सङ्ख्या = res ? VVAL_TRUE : VVAL_FALSE;
		}
		रोक;

	    // Operation with two number arguments
	    पक्ष ISN_OPNR:
	    पक्ष ISN_COMPARENR:
		{
		    typval_T	*tv1 = थाक_TV_BOT(-2);
		    typval_T	*tv2 = थाक_TV_BOT(-1);
		    varnumber_T arg1 = tv1->vval.v_सङ्ख्या;
		    varnumber_T arg2 = tv2->vval.v_सङ्ख्या;
		    varnumber_T res = 0;
		    पूर्णाङ्क		div_zero = FALSE;

		    यदि (iptr->isn_arg.op.op_type == अभिव्यक्ति_LSHIFT
			    || iptr->isn_arg.op.op_type == अभिव्यक्ति_RSHIFT)
		    {
			यदि (arg2 < 0)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_bitshift_ops_must_be_positive));
			    गच्छ on_error;
			}
		    }

		    विकल्प (iptr->isn_arg.op.op_type)
		    {
			पक्ष अभिव्यक्ति_MULT: res = arg1 * arg2; रोक;
			पक्ष अभिव्यक्ति_DIV:  यदि (arg2 == 0)
					    div_zero = TRUE;
					अन्यथा
					    res = arg1 / arg2;
					रोक;
			पक्ष अभिव्यक्ति_REM:  यदि (arg2 == 0)
					    div_zero = TRUE;
					अन्यथा
					    res = arg1 % arg2;
					रोक;
			पक्ष अभिव्यक्ति_SUB: res = arg1 - arg2; रोक;
			पक्ष अभिव्यक्ति_ADD: res = arg1 + arg2; रोक;

			पक्ष अभिव्यक्ति_EQUAL: res = arg1 == arg2; रोक;
			पक्ष अभिव्यक्ति_NEQUAL: res = arg1 != arg2; रोक;
			पक्ष अभिव्यक्ति_GREATER: res = arg1 > arg2; रोक;
			पक्ष अभिव्यक्ति_GEQUAL: res = arg1 >= arg2; रोक;
			पक्ष अभिव्यक्ति_SMALLER: res = arg1 < arg2; रोक;
			पक्ष अभिव्यक्ति_SEQUAL: res = arg1 <= arg2; रोक;
			पक्ष अभिव्यक्ति_LSHIFT: यदि (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  अन्यथा
					      res = (uvarnumber_T)arg1 << arg2;
					  रोक;
			पक्ष अभिव्यक्ति_RSHIFT: यदि (arg2 > MAX_LSHIFT_BITS)
					      res = 0;
					  अन्यथा
					      res = (uvarnumber_T)arg1 >> arg2;
					  रोक;
			स्वभाव: रोक;
		    }

		    --ectx->ec_थाक.ga_len;
		    यदि (iptr->isn_type == ISN_COMPARENR)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_सङ्ख्या = res ? VVAL_TRUE : VVAL_FALSE;
		    }
		    अन्यथा
			tv1->vval.v_सङ्ख्या = res;
		    यदि (div_zero)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_divide_by_zero));
			गच्छ on_error;
		    }
		}
		रोक;

	    // Computation with two float arguments
	    पक्ष ISN_OPFLOAT:
	    पक्ष ISN_COMPAREFLOAT:
		{
		    typval_T	*tv1 = थाक_TV_BOT(-2);
		    typval_T	*tv2 = थाक_TV_BOT(-1);
		    float_T	arg1 = tv1->vval.v_float;
		    float_T	arg2 = tv2->vval.v_float;
		    float_T	res = 0;
		    पूर्णाङ्क		cmp = FALSE;

		    विकल्प (iptr->isn_arg.op.op_type)
		    {
			पक्ष अभिव्यक्ति_MULT: res = arg1 * arg2; रोक;
			पक्ष अभिव्यक्ति_DIV: res = arg1 / arg2; रोक;
			पक्ष अभिव्यक्ति_SUB: res = arg1 - arg2; रोक;
			पक्ष अभिव्यक्ति_ADD: res = arg1 + arg2; रोक;

			पक्ष अभिव्यक्ति_EQUAL: cmp = arg1 == arg2; रोक;
			पक्ष अभिव्यक्ति_NEQUAL: cmp = arg1 != arg2; रोक;
			पक्ष अभिव्यक्ति_GREATER: cmp = arg1 > arg2; रोक;
			पक्ष अभिव्यक्ति_GEQUAL: cmp = arg1 >= arg2; रोक;
			पक्ष अभिव्यक्ति_SMALLER: cmp = arg1 < arg2; रोक;
			पक्ष अभिव्यक्ति_SEQUAL: cmp = arg1 <= arg2; रोक;
			स्वभाव: cmp = 0; रोक;
		    }
		    --ectx->ec_थाक.ga_len;
		    यदि (iptr->isn_type == ISN_COMPAREFLOAT)
		    {
			tv1->v_type = VAR_BOOL;
			tv1->vval.v_सङ्ख्या = cmp ? VVAL_TRUE : VVAL_FALSE;
		    }
		    अन्यथा
			tv1->vval.v_float = res;
		}
		रोक;

	    पक्ष ISN_COMPARELIST:
	    पक्ष ISN_COMPARETUPLE:
	    पक्ष ISN_COMPAREDICT:
	    पक्ष ISN_COMPAREFUNC:
	    पक्ष ISN_COMPARESTRING:
	    पक्ष ISN_COMPAREBLOB:
	    पक्ष ISN_COMPAREOBJECT:
		{
		    typval_T	*tv1 = थाक_TV_BOT(-2);
		    typval_T	*tv2 = थाक_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    पूर्णाङ्क		ic = iptr->isn_arg.op.op_ic;
		    पूर्णाङ्क		res = FALSE;
		    पूर्णाङ्क		status = OK;

		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (iptr->isn_type == ISN_COMPARELIST)
		    {
			status = typval_compare_list(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अन्यथा यदि (iptr->isn_type == ISN_COMPARETUPLE)
		    {
			status = typval_compare_tuple(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अन्यथा यदि (iptr->isn_type == ISN_COMPAREDICT)
		    {
			status = typval_compare_dict(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अन्यथा यदि (iptr->isn_type == ISN_COMPAREFUNC)
		    {
			status = typval_compare_func(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अन्यथा यदि (iptr->isn_type == ISN_COMPARESTRING)
		    {
			status = typval_compare_string(tv1, tv2,
							   exprtype, ic, &res);
		    }
		    अन्यथा यदि (iptr->isn_type == ISN_COMPAREBLOB)
		    {
			status = typval_compare_blob(tv1, tv2, exprtype, &res);
		    }
		    अन्यथा // ISN_COMPAREOBJECT
		    {
			status = typval_compare_object(tv1, tv2,
							exprtype, FALSE, &res);
		    }
		    --ectx->ec_थाक.ga_len;
		    clear_tv(tv1);
		    clear_tv(tv2);
		    tv1->v_type = VAR_BOOL;
		    tv1->vval.v_सङ्ख्या = res ? VVAL_TRUE : VVAL_FALSE;
		    यदि (status == FAIL)
			गच्छ theend;
		}
		रोक;

	    पक्ष ISN_COMPAREANY:
		{
		    typval_T	*tv1 = थाक_TV_BOT(-2);
		    typval_T	*tv2 = थाक_TV_BOT(-1);
		    exprtype_T	exprtype = iptr->isn_arg.op.op_type;
		    पूर्णाङ्क		ic = iptr->isn_arg.op.op_ic;
		    पूर्णाङ्क		status;

		    SOURCING_LNUM = iptr->isn_lnum;
		    status = typval_compare(tv1, tv2, exprtype, ic);
		    clear_tv(tv2);
		    --ectx->ec_थाक.ga_len;
		    यदि (status == FAIL)
			गच्छ theend;
		}
		रोक;

	    पक्ष ISN_ADDLIST:
	    पक्ष ISN_ADDTUPLE:
	    पक्ष ISN_ADDBLOB:
		{
		    typval_T *tv1 = थाक_TV_BOT(-2);
		    typval_T *tv2 = थाक_TV_BOT(-1);

		    // add two lists or blobs
		    यदि (iptr->isn_type == ISN_ADDLIST)
		    {
			यदि (iptr->isn_arg.op.op_type == अभिव्यक्ति_APPEND
						   && tv1->vval.v_list != रिक्त)
			    list_extend(tv1->vval.v_list, tv2->vval.v_list,
									 रिक्त);
			अन्यथा
			    eval_addlist(tv1, tv2);
		    }
		    अन्यथा यदि (iptr->isn_type == ISN_ADDTUPLE)
			eval_addtuple(tv1, tv2);
		    अन्यथा
			eval_addblob(tv1, tv2);
		    clear_tv(tv2);
		    --ectx->ec_थाक.ga_len;
		}
		रोक;

	    पक्ष ISN_LISTAPPEND:
		{
		    typval_T	*tv1 = थाक_TV_BOT(-2);
		    typval_T	*tv2 = थाक_TV_BOT(-1);
		    list_T	*l = tv1->vval.v_list;

		    // add an item to a list
		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (l == रिक्त)
		    {
			emsg(_(e_cannot_add_to_null_list));
			गच्छ on_error;
		    }
		    यदि (मान_check_lock(l->lv_lock, रिक्त, FALSE))
			गच्छ on_error;
		    यदि (list_append_tv(l, tv2) == FAIL)
			गच्छ theend;
		    clear_tv(tv2);
		    --ectx->ec_थाक.ga_len;
		}
		रोक;

	    पक्ष ISN_BLOBAPPEND:
		{
		    typval_T	*tv1 = थाक_TV_BOT(-2);
		    typval_T	*tv2 = थाक_TV_BOT(-1);
		    blob_T	*b = tv1->vval.v_blob;
		    पूर्णाङ्क		error = FALSE;
		    varnumber_T n;

		    // add a number to a blob
		    यदि (b == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_cannot_add_to_null_blob));
			गच्छ on_error;
		    }
		    n = tv_get_सङ्ख्या_chk(tv2, &error);
		    यदि (error)
			गच्छ on_error;
		    ga_append(&b->bv_ga, (पूर्णाङ्क)n);
		    --ectx->ec_थाक.ga_len;
		}
		रोक;

	    // Computation with two arguments of unknown type
	    पक्ष ISN_OPANY:
		{
		    typval_T	*tv1 = थाक_TV_BOT(-2);
		    typval_T	*tv2 = थाक_TV_BOT(-1);
		    varnumber_T	n1, n2;
		    float_T	f1 = 0, f2 = 0;
		    पूर्णाङ्क		error = FALSE;

		    यदि (iptr->isn_arg.op.op_type == अभिव्यक्ति_ADD)
		    {
			यदि (tv1->v_type == VAR_LIST && tv2->v_type == VAR_LIST)
			{
			    eval_addlist(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_थाक.ga_len;
			    रोक;
			}
			अन्यथा यदि (tv1->v_type == VAR_TUPLE
						&& tv2->v_type == VAR_TUPLE)
			{
			    eval_addtuple(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_थाक.ga_len;
			    रोक;
			}
			अन्यथा यदि (tv1->v_type == VAR_BLOB
						    && tv2->v_type == VAR_BLOB)
			{
			    eval_addblob(tv1, tv2);
			    clear_tv(tv2);
			    --ectx->ec_थाक.ga_len;
			    रोक;
			}
		    }
		    यदि (tv1->v_type == VAR_FLOAT)
		    {
			f1 = tv1->vval.v_float;
			n1 = 0;
		    }
		    अन्यथा
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			n1 = tv_get_सङ्ख्या_chk(tv1, &error);
			यदि (error)
			    गच्छ on_error;
			यदि (tv2->v_type == VAR_FLOAT)
			    f1 = n1;
		    }
		    यदि (tv2->v_type == VAR_FLOAT)
		    {
			f2 = tv2->vval.v_float;
			n2 = 0;
		    }
		    अन्यथा
		    {
			n2 = tv_get_सङ्ख्या_chk(tv2, &error);
			यदि (error)
			    गच्छ on_error;
			यदि (tv1->v_type == VAR_FLOAT)
			    f2 = n2;
		    }
		    // if there is a float on either side the result is a float
		    यदि (tv1->v_type == VAR_FLOAT || tv2->v_type == VAR_FLOAT)
		    {
			विकल्प (iptr->isn_arg.op.op_type)
			{
			    पक्ष अभिव्यक्ति_MULT: f1 = f1 * f2; रोक;
			    पक्ष अभिव्यक्ति_DIV:  f1 = f1 / f2; रोक;
			    पक्ष अभिव्यक्ति_SUB:  f1 = f1 - f2; रोक;
			    पक्ष अभिव्यक्ति_ADD:  f1 = f1 + f2; रोक;
			    स्वभाव: SOURCING_LNUM = iptr->isn_lnum;
				     emsg(_(e_cannot_use_percent_with_float));
				     गच्छ on_error;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_FLOAT;
			tv1->vval.v_float = f1;
			--ectx->ec_थाक.ga_len;
		    }
		    अन्यथा
		    {
			पूर्णाङ्क failed = FALSE;

			विकल्प (iptr->isn_arg.op.op_type)
			{
			    पक्ष अभिव्यक्ति_MULT: n1 = n1 * n2; रोक;
			    पक्ष अभिव्यक्ति_DIV:  n1 = अङ्क_divide(n1, n2, &failed);
					    यदि (failed)
						गच्छ on_error;
					    रोक;
			    पक्ष अभिव्यक्ति_SUB:  n1 = n1 - n2; रोक;
			    पक्ष अभिव्यक्ति_ADD:  n1 = n1 + n2; रोक;
			    स्वभाव:	    n1 = अङ्क_modulus(n1, n2, &failed);
					    यदि (failed)
						गच्छ on_error;
					    रोक;
			}
			clear_tv(tv1);
			clear_tv(tv2);
			tv1->v_type = VAR_सङ्ख्या;
			tv1->vval.v_सङ्ख्या = n1;
			--ectx->ec_थाक.ga_len;
		    }
		}
		रोक;

	    पक्ष ISN_STRINDEX:
	    पक्ष ISN_STRSLICE:
		{
		    पूर्णाङ्क		is_slice = iptr->isn_type == ISN_STRSLICE;
		    varnumber_T	n1 = 0, n2;
		    char_u	*res;

		    // string index: string is at stack-2, index at stack-1
		    // string slice: string is at stack-3, first index at
		    // stack-2, second index at stack-1
		    यदि (is_slice)
		    {
			tv = थाक_TV_BOT(-2);
			n1 = tv->vval.v_सङ्ख्या;
		    }

		    tv = थाक_TV_BOT(-1);
		    n2 = tv->vval.v_सङ्ख्या;

		    ectx->ec_थाक.ga_len -= is_slice ? 2 : 1;
		    tv = थाक_TV_BOT(-1);
		    यदि (is_slice)
			// Slice: Select the characters from the string
			res = string_slice(tv->vval.v_string, n1, n2, FALSE);
		    अन्यथा
			// Index: The resulting variable is a string of a
			// single character (including composing characters).
			// If the index is too big or negative the result is
			// empty.
			res = char_from_string(tv->vval.v_string, n2);
		    vim_free(tv->vval.v_string);
		    tv->vval.v_string = res;
		}
		रोक;

	    पक्ष ISN_LISTINDEX:
	    पक्ष ISN_LISTSLICE:
	    पक्ष ISN_TUPLEINDEX:
	    पक्ष ISN_TUPLESLICE:
	    पक्ष ISN_BLOBINDEX:
	    पक्ष ISN_BLOBSLICE:
		{
		    पूर्णाङ्क		is_slice = iptr->isn_type == ISN_LISTSLICE
				    || iptr->isn_type == ISN_TUPLESLICE
				    || iptr->isn_type == ISN_BLOBSLICE;
		    पूर्णाङ्क		is_blob = iptr->isn_type == ISN_BLOBINDEX
					    || iptr->isn_type == ISN_BLOBSLICE;
		    पूर्णाङ्क		is_tuple = iptr->isn_type == ISN_TUPLEINDEX
				    || iptr->isn_type == ISN_TUPLESLICE;
		    varnumber_T	n1, n2;
		    typval_T	*val_tv;

		    // list index: list is at stack-2, index at stack-1
		    // list slice: list is at stack-3, indexes at stack-2 and
		    // stack-1
		    // Same for tuple and blob.
		    val_tv = is_slice ? थाक_TV_BOT(-3) : थाक_TV_BOT(-2);

		    tv = थाक_TV_BOT(-1);
		    n1 = n2 = tv->vval.v_सङ्ख्या;
		    clear_tv(tv);

		    यदि (is_slice)
		    {
			tv = थाक_TV_BOT(-2);
			n1 = tv->vval.v_सङ्ख्या;
			clear_tv(tv);
		    }

		    ectx->ec_थाक.ga_len -= is_slice ? 2 : 1;
		    tv = थाक_TV_BOT(-1);
		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (is_blob)
		    {
			यदि (blob_slice_or_index(val_tv->vval.v_blob, is_slice,
						    n1, n2, FALSE, tv) == FAIL)
			    गच्छ on_error;
		    }
		    अन्यथा यदि (is_tuple)
		    {
			यदि (tuple_slice_or_index(val_tv->vval.v_tuple,
				is_slice, n1, n2, FALSE, tv, TRUE) == FAIL)
			    गच्छ on_error;
		    }
		    अन्यथा
		    {
			यदि (list_slice_or_index(val_tv->vval.v_list, is_slice,
					      n1, n2, FALSE, tv, TRUE) == FAIL)
			    गच्छ on_error;
		    }
		}
		रोक;

	    पक्ष ISN_ANYINDEX:
	    पक्ष ISN_ANYSLICE:
		{
		    पूर्णाङ्क		is_slice = iptr->isn_type == ISN_ANYSLICE;
		    typval_T	*var1, *var2;
		    पूर्णाङ्क		res;

		    // index: composite is at stack-2, index at stack-1
		    // slice: composite is at stack-3, indexes at stack-2 and
		    // stack-1
		    tv = is_slice ? थाक_TV_BOT(-3) : थाक_TV_BOT(-2);
		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (check_can_index(tv, TRUE, TRUE) == FAIL)
			गच्छ on_error;
		    var1 = is_slice ? थाक_TV_BOT(-2) : थाक_TV_BOT(-1);
		    var2 = is_slice ? थाक_TV_BOT(-1) : रिक्त;
		    res = eval_index_inner(tv, is_slice, var1, var2,
							FALSE, रिक्त, -1, TRUE);
		    clear_tv(var1);
		    यदि (is_slice)
			clear_tv(var2);
		    ectx->ec_थाक.ga_len -= is_slice ? 2 : 1;
		    यदि (res == FAIL)
			गच्छ on_error;
		}
		रोक;

	    पक्ष ISN_SLICE:
		{
		    पूर्णाङ्क		गन्ती = iptr->isn_arg.सङ्ख्या;

		    // type will have been checked to be a list
		    tv = थाक_TV_BOT(-1);
		    यदि (tv->v_type == VAR_LIST)
		    {
			list_T *list = tv->vval.v_list;

			// no error for short list, expect it to be checked
			// earlier
			यदि (list != रिक्त && list->lv_len >= गन्ती)
			{
			    list_T	*newlist = list_slice(list,
				    गन्ती, list->lv_len - 1);

			    यदि (newlist != रिक्त)
			    {
				list_unref(list);
				tv->vval.v_list = newlist;
				++newlist->lv_refcount;
			    }
			}
		    }
		    अन्यथा
		    {
			tuple_T *tuple = tv->vval.v_tuple;

			// no error for short tuple, expect it to be checked
			// earlier
			यदि (tuple != रिक्त && TUPLE_LEN(tuple) >= गन्ती)
			{
			    tuple_T *newtuple;

			    newtuple = tuple_slice(tuple, गन्ती,
							TUPLE_LEN(tuple) - 1);
			    यदि (newtuple != रिक्त)
			    {
				tuple_unref(tuple);
				tv->v_type = VAR_TUPLE;
				tv->vval.v_tuple = newtuple;
				++newtuple->tv_refcount;
			    }
			}
		    }
		}
		रोक;

	    पक्ष ISN_GETITEM:
		{
		    listitem_T	*li;
		    typval_T	*item_tv;
		    getitem_T	*gi = &iptr->isn_arg.getitem;

		    // Get list item: list is at stack-1, push item.
		    // List type and length is checked for when compiling.
		    tv = थाक_TV_BOT(-1 - gi->gi_with_op);
		    यदि (tv->v_type == VAR_LIST)
		    {
			li = list_find(tv->vval.v_list, gi->gi_index);
			यदि (li == रिक्त)
			    गच्छ on_error;
			item_tv = &li->li_tv;
		    }
		    अन्यथा
			item_tv = TUPLE_ITEM(tv->vval.v_tuple, gi->gi_index);

		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    ++ectx->ec_थाक.ga_len;
		    copy_tv(item_tv, थाक_TV_BOT(-1));

		    // Useful when used in unpack assignment.  Reset at
		    // ISN_DROP.
		    ectx->ec_where.wt_index = gi->gi_index + 1;
		    ectx->ec_where.wt_kind = WT_VARIABLE;
		}
		रोक;

	    // dict member with string key (dict['member'])
	    पक्ष ISN_MEMBER:
		{
		    dict_T	*dict;
		    char_u	*साँचो;
		    dictitem_T	*di;

		    // dict member: dict is at stack-2, key at stack-1
		    tv = थाक_TV_BOT(-2);
		    // no need to check for VAR_DICT, CHECKTYPE will check.
		    dict = tv->vval.v_dict;

		    tv = थाक_TV_BOT(-1);
		    // no need to check for VAR_STRING, 2STRING will check.
		    साँचो = tv->vval.v_string;
		    यदि (साँचो == रिक्त)
			साँचो = (char_u *)"";

		    यदि ((di = dict_find(dict, साँचो, -1)) == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_साँचो_not_present_in_dictionary_str), साँचो);

			// If :silent! is used we will continue, make sure the
			// stack contents makes sense and the dict stack is
			// updated.
			clear_tv(tv);
			--ectx->ec_थाक.ga_len;
			tv = थाक_TV_BOT(-1);
			(शून्य) dict_थाक_save(tv);
			tv->v_type = VAR_सङ्ख्या;
			tv->vval.v_सङ्ख्या = 0;
			गच्छ on_fatal_error;
		    }
		    clear_tv(tv);
		    --ectx->ec_थाक.ga_len;
		    // Put the dict used on the dict stack, it might be used by
		    // a dict function later.
		    tv = थाक_TV_BOT(-1);
		    यदि (dict_थाक_save(tv) == FAIL)
			गच्छ on_fatal_error;
		    copy_tv(&di->di_tv, tv);
		}
		रोक;

	    // dict member with string key (dict.member)
	    // or can be an object
	    पक्ष ISN_STRINGMEMBER:
		{
		    dict_T	*dict;
		    dictitem_T	*di;

		    tv = थाक_TV_BOT(-1);

		    यदि (tv->v_type == VAR_OBJECT
			    || tv->v_type == VAR_CLASS)
		    {
			यदि (dict_थाक_save(tv) == FAIL)
			    गच्छ on_fatal_error;

			ufunc_T *ufunc = (((dfunc_T *)def_functions.ga_data)
					+ ectx->ec_dfunc_idx)->df_ufunc;
			// Class or an object (not a Dict)
			यदि (var_any_get_oc_member(ufunc->uf_class, iptr, tv) == FAIL)
			    गच्छ on_error;
		    }
		    अन्यथा
		    {
			यदि (tv->v_type != VAR_DICT || tv->vval.v_dict == रिक्त)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    emsg(_(e_dictionary_required));
			    गच्छ on_error;
			}
			dict = tv->vval.v_dict;

			यदि ((di = dict_find(dict, iptr->isn_arg.string, -1))
								   == रिक्त)
			{
			    SOURCING_LNUM = iptr->isn_lnum;
			    semsg(_(e_साँचो_not_present_in_dictionary_str),
						     iptr->isn_arg.string);
			    गच्छ on_error;
			}
			// Put the dict used on the dict stack, it might be
			// used by a dict function later.
			यदि (dict_थाक_save(tv) == FAIL)
			    गच्छ on_fatal_error;

			copy_tv(&di->di_tv, tv);
		    }
		}
		रोक;

	    पक्ष ISN_GET_OBJ_MEMBER:
	    पक्ष ISN_GET_ITF_MEMBER:
		{
		    tv = थाक_TV_BOT(-1);
		    यदि (tv->v_type != VAR_OBJECT)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			object_required_error(tv);
			गच्छ on_error;
		    }

		    object_T *obj = tv->vval.v_object;
		    यदि (obj == रिक्त)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			emsg(_(e_using_null_object));
			गच्छ on_error;
		    }

		    पूर्णाङ्क idx;
		    यदि (iptr->isn_type == ISN_GET_OBJ_MEMBER)
			idx = iptr->isn_arg.classmember.cm_idx;
		    अन्यथा
		    {
			idx = iptr->isn_arg.classmember.cm_idx;
			// convert the interface index to the object index
			idx = object_index_from_itf_index(
					iptr->isn_arg.classmember.cm_class,
					FALSE, idx, obj->obj_class);
		    }

		    // The members are located right after the object struct.
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    यदि (mtv->v_type == VAR_UNKNOWN)
		    {
			// Referencing an object variable (without a type)
			// which is not yet initialized.  So the type is not
			// yet known.
			ocmember_T *m = &obj->obj_class->class_obj_members[idx];
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_uninitialized_object_var_reference),
				m->ocm_name.string);
			गच्छ on_error;
		    }
		    copy_tv(mtv, tv);

		    // Unreference the object after getting the member, it may
		    // be freed.
		    object_unref(obj);
		}
		रोक;

	    पक्ष ISN_STORE_THIS:
		{
		    पूर्णाङ्क idx = iptr->isn_arg.सङ्ख्या;
		    object_T *obj = थाक_TV_VAR(0)->vval.v_object;
		    // the members are located right after the object struct
		    typval_T *mtv = ((typval_T *)(obj + 1)) + idx;
		    clear_tv(mtv);
		    *mtv = *थाक_TV_BOT(-1);
		    --ectx->ec_थाक.ga_len;
		}
		रोक;

	    पक्ष ISN_CLEARDICT:
		dict_थाक_drop();
		रोक;

	    पक्ष ISN_USEDICT:
		{
		    typval_T *dict_tv = dict_थाक_get_tv();

		    // Turn "dict.Func" into a partial for "Func" bound to
		    // "dict".  Don't do this when "Func" is already a partial
		    // that was bound explicitly (pt_auto is FALSE).
		    tv = थाक_TV_BOT(-1);
		    यदि (dict_tv != रिक्त
			    && dict_tv->v_type == VAR_DICT
			    && dict_tv->vval.v_dict != रिक्त
			    && (tv->v_type == VAR_FUNC
				|| (tv->v_type == VAR_PARTIAL
				    && (tv->vval.v_partial->pt_auto
				     || tv->vval.v_partial->pt_dict == रिक्त))))
		    dict_tv->vval.v_dict =
					make_partial(dict_tv->vval.v_dict, tv);
		    dict_थाक_drop();
		}
		रोक;

	    पक्ष ISN_NEGATENR:
		tv = थाक_TV_BOT(-1);
		// CHECKTYPE should have checked the variable type
		यदि (tv->v_type == VAR_FLOAT)
		    tv->vval.v_float = -tv->vval.v_float;
		अन्यथा
		    tv->vval.v_सङ्ख्या = -tv->vval.v_सङ्ख्या;
		रोक;

	    पक्ष ISN_CHECKTYPE:
		{
		    checktype_T *ct = &iptr->isn_arg.type;
		    पूर्णाङ्क		r;
		    where_T	where = WHERE_INIT;

		    tv = थाक_TV_BOT((पूर्णाङ्क)ct->ct_off);
		    SOURCING_LNUM = iptr->isn_lnum;
		    यदि (ct->ct_arg_idx > 0)
		    {
			where.wt_index = ct->ct_arg_idx;
			where.wt_kind = ct->ct_is_var ? WT_VARIABLE : WT_ARGUMENT;
		    }
		    where.wt_func_name = ectx->ec_where.wt_func_name;
		    r = check_typval_type(ct->ct_type, tv, where);
		    यदि (r == FAIL)
			गच्छ on_error;

		    // number 0 is FALSE, number 1 is TRUE
		    यदि (tv->v_type == VAR_सङ्ख्या
			    && ct->ct_type->tt_type == VAR_BOOL
			    && (tv->vval.v_सङ्ख्या == 0
						|| tv->vval.v_सङ्ख्या == 1))
		    {
			tv->v_type = VAR_BOOL;
			tv->vval.v_सङ्ख्या = tv->vval.v_सङ्ख्या
						      ? VVAL_TRUE : VVAL_FALSE;
		    }
		}
		रोक;

	    पक्ष ISN_CHECKLEN:
		{
		    पूर्णाङ्क	    min_len = iptr->isn_arg.checklen.cl_min_len;
		    list_T  *list = रिक्त;
		    tuple_T *tuple = रिक्त;
		    पूर्णाङ्क	    len = 0;

		    tv = थाक_TV_BOT(-1);

		    पूर्णाङ्क		len_check_failed = FALSE;
		    यदि (tv->v_type == VAR_LIST)
		    {
			list = tv->vval.v_list;
			यदि (list == रिक्त || list->lv_len < min_len
			    || (list->lv_len > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			यदि (list != रिक्त)
			    len = list->lv_len;
		    }
		    अन्यथा यदि (tv->v_type == VAR_TUPLE)
		    {
			tuple = tv->vval.v_tuple;
			यदि (tuple == रिक्त || TUPLE_LEN(tuple) < min_len
			    || (TUPLE_LEN(tuple) > min_len
					&& !iptr->isn_arg.checklen.cl_more_OK))
			    len_check_failed = TRUE;
			यदि (tuple != रिक्त)
			    len = TUPLE_LEN(tuple);
		    }
		    अन्यथा
			len_check_failed = TRUE;

		    यदि (len_check_failed)
		    {
			SOURCING_LNUM = iptr->isn_lnum;
			semsg(_(e_expected_nr_वस्तुहरू_but_got_nr),
				     min_len, len);
			गच्छ on_error;
		    }
		}
		रोक;

	    पक्ष ISN_SETTYPE:
		set_tv_type(थाक_TV_BOT(-1), iptr->isn_arg.type.ct_type);
		रोक;

	    पक्ष ISN_2BOOL:
	    पक्ष ISN_COND2BOOL:
		{
		    पूर्णाङ्क n;
		    पूर्णाङ्क error = FALSE;

		    यदि (iptr->isn_type == ISN_2BOOL)
		    {
			tv = थाक_TV_BOT(iptr->isn_arg.tobool.offset);
			n = tv2bool(tv);
			यदि (iptr->isn_arg.tobool.invert)
			    n = !n;
		    }
		    अन्यथा
		    {
			tv = थाक_TV_BOT(-1);
			SOURCING_LNUM = iptr->isn_lnum;
			n = tv_get_bool_chk(tv, &error);
			यदि (error)
			    गच्छ on_error;
		    }
		    clear_tv(tv);
		    tv->v_type = VAR_BOOL;
		    tv->vval.v_सङ्ख्या = n ? VVAL_TRUE : VVAL_FALSE;
		}
		रोक;

	    पक्ष ISN_2STRING:
	    पक्ष ISN_2STRING_ANY:
		SOURCING_LNUM = iptr->isn_lnum;
		यदि (do_2string(थाक_TV_BOT(iptr->isn_arg.tostring.offset),
				iptr->isn_type == ISN_2STRING_ANY,
				      iptr->isn_arg.tostring.flags) == FAIL)
			    गच्छ on_error;
		रोक;

	    पक्ष ISN_RANGE:
		{
		    exarg_T	ea;
		    वर्ण	*errormsg;

		    ea.line2 = 0;
		    ea.addr_गन्ती = 0;
		    ea.addr_type = ADDR_पङ्क्तिहरू;
		    ea.cmd = iptr->isn_arg.string;
		    ea.skip = FALSE;
		    यदि (parse_cmd_address(&ea, &errormsg, FALSE) == FAIL)
			गच्छ on_error;

		    यदि (GA_GROW_FAILS(&ectx->ec_थाक, 1))
			गच्छ theend;
		    ++ectx->ec_थाक.ga_len;
		    tv = थाक_TV_BOT(-1);
		    tv->v_type = VAR_सङ्ख्या;
		    tv->v_lock = 0;
		    tv->vval.v_सङ्ख्या = ea.line2;
		}
		रोक;

	    पक्ष ISN_PUT:
		tv = रिक्त; // initialize it so we don't get a warning
		isn_put_do(ectx, iptr, tv, FALSE);
		रोक;
	    पक्ष ISN_IPUT:
		tv = रिक्त;
		isn_put_do(ectx, iptr, tv, TRUE);
		रोक;

	    पक्ष ISN_CMDMOD:
		ectx->ec_funclocal.floc_save_cmdmod = cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = TRUE;
		ectx->ec_funclocal.floc_restore_cmdmod_stacklen =
							 ectx->ec_थाक.ga_len;
		cmdmod = *iptr->isn_arg.cmdmod.cf_cmdmod;
		apply_cmdmod(&cmdmod);
		रोक;

	    पक्ष ISN_CMDMOD_REV:
		// filter regprog is owned by the instruction, don't free it
		cmdmod.cmod_filter_regmatch.regprog = रिक्त;
		undo_cmdmod(&cmdmod);
		cmdmod = ectx->ec_funclocal.floc_save_cmdmod;
		ectx->ec_funclocal.floc_restore_cmdmod = FALSE;
		रोक;

	    पक्ष ISN_UNPACK:
		// Check there is a valid list to unpack.
		tv = थाक_TV_BOT(-1);
		यदि (tv->v_type != VAR_LIST && tv->v_type != VAR_TUPLE)
		{
		    SOURCING_LNUM = iptr->isn_lnum;
		    emsg(_(e_for_argument_must_be_sequence_of_lists_or_tuples));
		    गच्छ on_error;
		}

		पूर्णाङ्क rc;
		यदि (tv->v_type == VAR_LIST)
		    rc = exec_unpack_list(ectx, iptr, tv);
		अन्यथा
		    rc = exec_unpack_tuple(ectx, iptr, tv);
		यदि (rc != EXEC_OK)
		{
		    यदि (rc == EXEC_FAIL)
			गच्छ on_error;
		    गच्छ theend;
		}
		रोक;

	    पक्ष ISN_PROF_START:
	    पक्ष ISN_PROF_END:
		{
#ifdef FEAT_PROFILE
		    funccall_T	cookie;
		    ufunc_T	*cur_ufunc =
				    (((dfunc_T *)def_functions.ga_data)
					       + ectx->ec_dfunc_idx)->df_ufunc;

		    cookie.fc_func = cur_ufunc;
		    यदि (iptr->isn_type == ISN_PROF_START)
		    {
			func_पङ्क्ति_start(&cookie, iptr->isn_lnum);
			// if we get here the instruction is executed
			func_पङ्क्ति_exec(&cookie);
		    }
		    अन्यथा
			func_पङ्क्ति_end(&cookie);
#endif
		}
		रोक;

	    पक्ष ISN_DEBUG:
		handle_debug(iptr, ectx);
		रोक;

	    पक्ष ISN_SHUFFLE:
		{
		    typval_T	tmp_tv;
		    पूर्णाङ्क		item = iptr->isn_arg.shuffle.shfl_item;
		    पूर्णाङ्क		up = iptr->isn_arg.shuffle.shfl_up;

		    tmp_tv = *थाक_TV_BOT(-item);
		    लागि ( ; up > 0 && item > 1; --up)
		    {
			*थाक_TV_BOT(-item) = *थाक_TV_BOT(-item + 1);
			--item;
		    }
		    *थाक_TV_BOT(-item) = tmp_tv;
		}
		रोक;

	    पक्ष ISN_DROP:
		--ectx->ec_थाक.ga_len;
		clear_tv(थाक_TV_BOT(0));
		ectx->ec_where = (where_T)WHERE_INIT;
		रोक;

	    पक्ष ISN_SCRIPTCTX_SET:
		// change the script context.  Used to evaluate an object
		// member variable initialization expression in the context of
		// the script where the class is defined.
		current_sctx = iptr->isn_arg.setsctx;
		रोक;
	}
	जारी;

func_return:
	// Restore previous function. If the frame pointer is where we started
	// then there is none and we are done.
	यदि (ectx->ec_frame_idx == ectx->ec_initial_frame_idx)
	    गच्छ done;

	यदि (func_return(ectx) == FAIL)
	    // only fails when out of memory
	    गच्छ theend;
	जारी;

on_error:
	// Jump here for an error that does not require aborting execution.
	// If "emsg_silent" is set then ignore the error, unless it was set
	// when calling the function.
	यदि (did_emsg_cumul + did_emsg == ectx->ec_did_emsg_before
					   && emsg_silent && did_emsg_def == 0)
	{
	    // If a sequence of instructions causes an error while ":silent!"
	    // was used, restore the stack length and jump ahead to restoring
	    // the cmdmod.
	    यदि (ectx->ec_funclocal.floc_restore_cmdmod)
	    {
		जबसम्म (ectx->ec_थाक.ga_len
			     > ectx->ec_funclocal.floc_restore_cmdmod_stacklen)
		{
		    --ectx->ec_थाक.ga_len;
		    clear_tv(थाक_TV_BOT(0));
		}
		जबसम्म (ectx->ec_instr[ectx->ec_iidx].isn_type != ISN_CMDMOD_REV)
		    ++ectx->ec_iidx;
	    }
	    जारी;
	}
on_fatal_error:
	// Jump here for an error that messes up the stack.
	// If we are not inside a try-catch started here, abort execution.
	यदि (trylevel <= ectx->ec_trylevel_at_start)
	    गच्छ theend;
    }

done:
    ret = OK;
theend:
    may_invoke_defer_funcs(ectx);

    dict_थाक_clear(dict_थाक_len_at_start);
    ectx->ec_trylevel_at_start = save_trylevel_at_start;
    फर्काऊ ret;
}

/*
 * Execute the instructions from a VAR_INSTR typval and put the result in
 * "rettv".
 * Return OK or FAIL.
 */
    पूर्णाङ्क
exe_typval_instr(typval_T *tv, typval_T *rettv)
{
    ectx_T	*ectx = tv->vval.v_instr->instr_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    पूर्णाङ्क		save_iidx = ectx->ec_iidx;
    पूर्णाङ्क		res;

    // Initialize rettv so that it is safe for caller to invoke clear_tv(rettv)
    // even when the compilation fails.
    rettv->v_type = VAR_UNKNOWN;

    ectx->ec_instr = tv->vval.v_instr->instr_instr;
    res = exec_instructions(ectx);
    यदि (res == OK)
    {
	*rettv = *थाक_TV_BOT(-1);
	--ectx->ec_थाक.ga_len;
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    फर्काऊ res;
}

/*
 * Execute the instructions from an ISN_SUBSTITUTE command, which are in
 * "substitute_instr".
 */
    char_u *
exe_substitute_instr(शून्य)
{
    ectx_T	*ectx = substitute_instr->subs_ectx;
    isn_T	*save_instr = ectx->ec_instr;
    पूर्णाङ्क		save_iidx = ectx->ec_iidx;
    char_u	*res;

    ectx->ec_instr = substitute_instr->subs_instr;
    यदि (exec_instructions(ectx) == OK)
    {
	typval_T *tv = थाक_TV_BOT(-1);

	res = typval2string(tv, TRUE);
	--ectx->ec_थाक.ga_len;
	clear_tv(tv);
    }
    अन्यथा
    {
	substitute_instr->subs_status = FAIL;
	res = vim_strsave((char_u *)"");
    }

    ectx->ec_instr = save_instr;
    ectx->ec_iidx = save_iidx;

    फर्काऊ res;
}

/*
 * Call a "def" function from old Vim script.
 * Return OK or FAIL.
 */
    पूर्णाङ्क
call_def_function(
    ufunc_T	*ufunc,
    पूर्णाङ्क		argc_arg,	// nr of arguments
    typval_T	*argv,		// arguments
    पूर्णाङ्क		flags,		// DEF_ flags
    partial_T	*partial,	// optional partial for context
    object_T	*object,	// object, e.g. for this.Func()
    funccall_T	*funccal,
    typval_T	*rettv)		// return value
{
    ectx_T	ectx;		// execution context
    पूर्णाङ्क		argc = argc_arg;
    पूर्णाङ्क		partial_argc = partial == रिक्त
				  || (flags & DEF_USE_PT_ARGV) == 0
							? 0 : partial->pt_argc;
    पूर्णाङ्क		total_argc = argc + partial_argc;
    typval_T	*tv;
    पूर्णाङ्क		idx;
    पूर्णाङ्क		ret = FAIL;
    पूर्णाङ्क		defcount = ufunc->uf_args.ga_len - total_argc;
    sctx_T	save_current_sctx = current_sctx;
    पूर्णाङ्क		did_emsg_before = did_emsg_cumul + did_emsg;
    पूर्णाङ्क		save_suppress_errthrow = suppress_errthrow;
    msglist_T	**saved_सन्देश_list = रिक्त;
    msglist_T	*private_सन्देश_list = रिक्त;
    पूर्णाङ्क		save_emsg_silent_def = emsg_silent_def;
    पूर्णाङ्क		save_did_emsg_def = did_emsg_def;
    पूर्णाङ्क		orig_funcdepth;
    पूर्णाङ्क		orig_nesting_level = ex_nesting_level;

// Get pointer to item in the stack.
#undef थाक_TV
#define थाक_TV(idx) (((typval_T *)ectx.ec_थाक.ga_data) + idx)

// Get pointer to item at the bottom of the stack, -1 is the bottom.
#undef थाक_TV_BOT
#define थाक_TV_BOT(idx) (((typval_T *)ectx.ec_थाक.ga_data) + ectx.ec_थाक.ga_len + idx)

// Get pointer to a local variable on the stack.  Negative for arguments.
#undef थाक_TV_VAR
#define थाक_TV_VAR(idx) (((typval_T *)ectx.ec_थाक.ga_data) + ectx.ec_frame_idx + थाक_FRAME_SIZE + idx)

    यदि (ufunc->uf_def_status == UF_NOT_COMPILED
	    || ufunc->uf_def_status == UF_COMPILE_ERROR
	    || (func_needs_compiling(ufunc, get_compile_type(ufunc))
		&& compile_def_function(ufunc, FALSE,
				       get_compile_type(ufunc), रिक्त) == FAIL))
    {
	यदि (did_emsg_cumul + did_emsg == did_emsg_before)
	    semsg(_(e_function_is_not_compiled_str),
						   printable_func_name(ufunc));
	फर्काऊ FAIL;
    }

    {
	// Check the function was really compiled.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	यदि (dfunc->df_ufunc == रिक्त)
	{
	    semsg(_(e_function_was_deleted_str), printable_func_name(ufunc));
	    फर्काऊ FAIL;
	}
	यदि (INSTRUCTIONS(dfunc) == रिक्त)
	{
	    iemsg("using call_def_function() on not compiled function");
	    फर्काऊ FAIL;
	}
    }

    // If depth of calling is getting too high, don't execute the function.
    orig_funcdepth = funcdepth_get();
    यदि (funcdepth_increment() == FAIL)
	फर्काऊ FAIL;

    CLEAR_FIELD(ectx);
    ectx.ec_dfunc_idx = ufunc->uf_dfunc_idx;
    ga_init2(&ectx.ec_थाक, परिमाण(typval_T), 500);
    यदि (GA_GROW_FAILS(&ectx.ec_थाक, 20))
    {
	funcdepth_decrement();
	फर्काऊ FAIL;
    }
    ga_init2(&ectx.ec_trystack, परिमाण(trycmd_T), 10);
    ga_init2(&ectx.ec_funcrefs, परिमाण(partial_T *), 10);
    ectx.ec_did_emsg_before = did_emsg_before;
    ++ex_nesting_level;

    idx = total_argc - ufunc->uf_args.ga_len;
    यदि (idx > 0 && ufunc->uf_va_name == रिक्त)
    {
	semsg(NGETTEXT(e_one_argument_too_many, e_nr_arguments_too_many,
								    idx), idx);
	गच्छ failed_early;
    }
    idx = total_argc - ufunc->uf_args.ga_len + ufunc->uf_def_args.ga_len;
    यदि (idx < 0)
    {
	semsg(NGETTEXT(e_one_argument_too_few, e_nr_arguments_too_few,
								  -idx), -idx);
	गच्छ failed_early;
    }

    // Put values from the partial and arguments on the stack, but no more than
    // what the function expects.  A lambda can be called with more arguments
    // than it uses.
    लागि (idx = 0; idx < total_argc
	    && (ufunc->uf_va_name != रिक्त || idx < ufunc->uf_args.ga_len);
									 ++idx)
    {
	पूर्णाङ्क argv_idx = idx - partial_argc;

	tv = idx < partial_argc ? partial->pt_argv + idx : argv + argv_idx;
	यदि (idx >= ufunc->uf_args.ga_len - ufunc->uf_def_args.ga_len
		&& tv->v_type == VAR_SPECIAL
		&& tv->vval.v_सङ्ख्या == VVAL_NONE)
	{
	    // Use the default value.
	    थाक_TV_BOT(0)->v_type = VAR_UNKNOWN;
	}
	अन्यथा
	{
	    पूर्णाङ्क done = FALSE;
	    यदि (ufunc->uf_arg_types != रिक्त && idx < ufunc->uf_args.ga_len)
	    {
		type_T *expected = ufunc->uf_arg_types[idx];
		यदि (expected->tt_type == VAR_FLOAT && tv->v_type == VAR_सङ्ख्या)
		{
		    // When a float is expected and a number was given, convert
		    // the value.
		    थाक_TV_BOT(0)->v_type = VAR_FLOAT;
		    थाक_TV_BOT(0)->v_lock = 0;
		    थाक_TV_BOT(0)->vval.v_float = tv->vval.v_सङ्ख्या;
		    done = TRUE;
		}
		अन्यथा यदि (check_typval_arg_type(expected, tv,
						   रिक्त, argv_idx + 1) == FAIL)
		    गच्छ failed_early;
	    }
	    यदि (!done)
		copy_tv(tv, थाक_TV_BOT(0));
	}
	++ectx.ec_थाक.ga_len;
    }

    // Turn varargs into a list.  Empty list if no args.
    यदि (ufunc->uf_va_name != रिक्त)
    {
	पूर्णाङ्क vararg_गन्ती = argc - ufunc->uf_args.ga_len;

	यदि (vararg_गन्ती < 0)
	    vararg_गन्ती = 0;
	अन्यथा
	    argc -= vararg_गन्ती;
	यदि (exe_newlist(vararg_गन्ती, &ectx) == FAIL)
	    गच्छ failed_early;

	// Check the type of the list items.
	tv = थाक_TV_BOT(-1);
	यदि (ufunc->uf_va_type != रिक्त
		&& ufunc->uf_va_type != &t_list_any
		&& ufunc->uf_va_type->tt_member != &t_any
		&& tv->vval.v_list != रिक्त)
	{
	    type_T	*expected = ufunc->uf_va_type->tt_member;
	    listitem_T	*li = tv->vval.v_list->lv_first;

	    लागि (idx = 0; idx < vararg_गन्ती; ++idx)
	    {
		यदि (check_typval_arg_type(expected, &li->li_tv,
						 रिक्त, argc + idx + 1) == FAIL)
		    गच्छ failed_early;
		li = li->li_next;
	    }
	}

	यदि (defcount > 0)
	    // Move varargs list to below missing default arguments.
	    *थाक_TV_BOT(defcount - 1) = *थाक_TV_BOT(-1);
	--ectx.ec_थाक.ga_len;
    }

    // Make space for omitted arguments, will store default value below.
    // Any varargs list goes after them.
    यदि (defcount > 0)
	लागि (idx = 0; idx < defcount; ++idx)
	{
	    थाक_TV_BOT(0)->v_type = VAR_UNKNOWN;
	    ++ectx.ec_थाक.ga_len;
	}
    यदि (ufunc->uf_va_name != रिक्त)
	++ectx.ec_थाक.ga_len;

    // Frame pointer points to just after arguments.
    ectx.ec_frame_idx = ectx.ec_थाक.ga_len;
    ectx.ec_initial_frame_idx = ectx.ec_frame_idx;

    {
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;
	ufunc_T *base_ufunc = dfunc->df_ufunc;

	// "uf_partial" is on the ufunc that "df_ufunc" points to, as is done
	// by copy_lambda_to_global_func().
	यदि (partial != रिक्त || base_ufunc->uf_partial != रिक्त)
	{
	    ectx.ec_outer_ref = ALLOC_CLEAR_ONE(outer_ref_T);
	    यदि (ectx.ec_outer_ref == रिक्त)
		गच्छ failed_early;
	    यदि (partial != रिक्त)
	    {
		outer_T *outer = get_pt_outer(partial);

		यदि (outer->out_थाक == रिक्त && outer->out_loop_size == 0)
		{
		    // no stack was set
		    यदि (current_ectx != रिक्त)
		    {
			यदि (current_ectx->ec_outer_ref != रिक्त
			       && current_ectx->ec_outer_ref->or_outer != रिक्त)
			    ectx.ec_outer_ref->or_outer =
					  current_ectx->ec_outer_ref->or_outer;
		    }
		    // else: should there be an error here?
		}
		अन्यथा
		{
		    ectx.ec_outer_ref->or_outer = outer;
		    ++partial->pt_refcount;
		    ectx.ec_outer_ref->or_partial = partial;
		}
	    }
	    अन्यथा
	    {
		ectx.ec_outer_ref->or_outer = &base_ufunc->uf_partial->pt_outer;
		++base_ufunc->uf_partial->pt_refcount;
		ectx.ec_outer_ref->or_partial = base_ufunc->uf_partial;
	    }
	}
    }

    // dummy frame entries
    लागि (idx = 0; idx < थाक_FRAME_SIZE; ++idx)
    {
	थाक_TV(ectx.ec_थाक.ga_len)->v_type = VAR_UNKNOWN;
	++ectx.ec_थाक.ga_len;
    }

    {
	// Reserve space for local variables and any closure reference count.
	dfunc_T	*dfunc = ((dfunc_T *)def_functions.ga_data)
							 + ufunc->uf_dfunc_idx;

	// Initialize variables to zero.  That avoids having to generate
	// initializing instructions for "var nr: number", "var x: any", etc.
	लागि (idx = 0; idx < dfunc->df_varcount; ++idx)
	{
	    थाक_TV_VAR(idx)->v_type = VAR_सङ्ख्या;
	    थाक_TV_VAR(idx)->vval.v_सङ्ख्या = 0;
	}
	ectx.ec_थाक.ga_len += dfunc->df_varcount;

	यदि (object != रिक्त)
	{
	    // the object is always the variable at index zero
	    tv = थाक_TV_VAR(0);
	    tv->v_type = VAR_OBJECT;
	    tv->vval.v_object = object;
	}

	यदि (dfunc->df_has_closure)
	{
	    // Initialize the variable that counts how many closures were
	    // created.  This is used in handle_closure_in_use().
	    थाक_TV_VAR(idx)->v_type = VAR_सङ्ख्या;
	    थाक_TV_VAR(idx)->vval.v_सङ्ख्या = 0;
	    ++ectx.ec_थाक.ga_len;
	}

	ectx.ec_instr = INSTRUCTIONS(dfunc);
    }

    // Store the execution context in funccal, used by invoke_all_defer().
    यदि (funccal != रिक्त)
	funccal->fc_ectx = &ectx;

    // Following errors are in the function, not the caller.
    // Commands behave like vim9script.
    estack_थप_ufunc(ufunc, 1);
    current_sctx = ufunc->uf_script_ctx;
    current_sctx.sc_version = SCRIPT_VERSION_VIM9;

    // Use a specific location for storing error messages to be converted to an
    // exception.
    saved_सन्देश_list = सन्देश_list;
    सन्देश_list = &private_सन्देश_list;

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
    यदि (ret == OK)
    {
	// function finished, get result from the stack.
	यदि (ufunc->uf_ret_type == &t_void)
	{
	    rettv->v_type = VAR_VOID;
	}
	अन्यथा
	{
	    tv = थाक_TV_BOT(-1);
	    *rettv = *tv;
	    tv->v_type = VAR_UNKNOWN;
	}
    }

    // When failed need to unwind the call stack.
    unwind_def_callstack(&ectx);

    // Deal with any remaining closures, they may be in use somewhere.
    यदि (ectx.ec_funcrefs.ga_len > 0)
    {
	handle_closure_in_use(&ectx, FALSE);
	ga_clear(&ectx.ec_funcrefs);
    }

    estack_झिक();
    current_sctx = save_current_sctx;

    यदि (--ufunc->uf_calls <= 0 && ufunc->uf_refcount <= 0)
	// Function was unreferenced while being used, free it now.
	func_clear_free(ufunc, FALSE);

    यदि (*सन्देश_list != रिक्त && saved_सन्देश_list != रिक्त)
    {
	msglist_T **plist = saved_सन्देश_list;

	// Append entries from the current msg_list (uncaught exceptions) to
	// the saved msg_list.
	जबसम्म (*plist != रिक्त)
	    plist = &(*plist)->next;

	*plist = *सन्देश_list;
    }
    सन्देश_list = saved_सन्देश_list;

    यदि (ectx.ec_funclocal.floc_restore_cmdmod)
    {
	cmdmod.cmod_filter_regmatch.regprog = रिक्त;
	undo_cmdmod(&cmdmod);
	cmdmod = ectx.ec_funclocal.floc_save_cmdmod;
    }
    emsg_silent_def = save_emsg_silent_def;
    did_emsg_def += save_did_emsg_def;

failed_early:
    // Free all arguments and local variables.
    लागि (idx = 0; idx < ectx.ec_थाक.ga_len; ++idx)
    {
	tv = थाक_TV(idx);
	यदि (tv->v_type != VAR_सङ्ख्या && tv->v_type != VAR_UNKNOWN)
	    clear_tv(tv);
    }
    ex_nesting_level = orig_nesting_level;

    vim_free(ectx.ec_थाक.ga_data);
    vim_free(ectx.ec_trystack.ga_data);
    यदि (ectx.ec_outer_ref != रिक्त)
    {
	यदि (ectx.ec_outer_ref->or_outer_allocated)
	    vim_free(ectx.ec_outer_ref->or_outer);
	partial_unref(ectx.ec_outer_ref->or_partial);
	vim_free(ectx.ec_outer_ref);
    }

    // Not sure if this is necessary.
    suppress_errthrow = save_suppress_errthrow;

    यदि (ret != OK && did_emsg_cumul + did_emsg == did_emsg_before
							      && !need_rethrow)
	semsg(_(e_unknown_error_while_executing_str),
						   printable_func_name(ufunc));
    funcdepth_restore(orig_funcdepth);
    फर्काऊ ret;
}

/*
 * Called when a def function has finished (possibly failed).
 * Invoke all the function returns to clean up and invoke deferred functions,
 * except the toplevel one.
 */
    शून्य
unwind_def_callstack(ectx_T *ectx)
{
    जबसम्म (ectx->ec_frame_idx != ectx->ec_initial_frame_idx)
	func_return(ectx);
}

/*
 * Invoke any deferred functions for the top function in "ectx".
 */
    शून्य
may_invoke_defer_funcs(ectx_T *ectx)
{
    dfunc_T *dfunc = ((dfunc_T *)def_functions.ga_data) + ectx->ec_dfunc_idx;

    यदि (dfunc->df_defer_var_idx > 0)
	invoke_defer_funcs(ectx);
}

/*
 * Return loopvarinfo in a printable form in allocated memory.
 */
    स्थिर char_u *
printable_loopvarinfo(loopvarinfo_T *lvi)
{
    garray_T	ga;
    पूर्णाङ्क		depth;

    ga_init2(&ga, 1, 100);
    लागि (depth = 0; depth < lvi->lvi_depth; ++depth)
    {
	यदि (ga_grow(&ga, 50) == FAIL)
	    रोक;
	यदि (lvi->lvi_loop[depth].var_idx == 0)
	    STRCPY((वर्ण *)ga.ga_data + ga.ga_len, " -");
	अन्यथा
	    vim_snprintf((वर्ण *)ga.ga_data + ga.ga_len, 50, " $%d-$%d",
			    lvi->lvi_loop[depth].var_idx,
			    lvi->lvi_loop[depth].var_idx
					 + lvi->lvi_loop[depth].var_गन्ती - 1);
	ga.ga_len = (पूर्णाङ्क)STRLEN(ga.ga_data);
    }
    फर्काऊ ga.ga_data;
}

/*
 * List instructions "instr" up to "instr_count" or until ISN_FINISH.
 * "ufunc" has the source lines, NULL for the instructions of ISN_SUBSTITUTE.
 * "pfx" is prefixed to every line.
 */
    स्थिर शून्य
list_instructions(वर्ण *pfx, isn_T *instr, पूर्णाङ्क instr_गन्ती, ufunc_T *ufunc)
{
    पूर्णाङ्क		पङ्क्ति_idx = 0;
    पूर्णाङ्क		prev_current = 0;
    पूर्णाङ्क		current;
    पूर्णाङ्क		def_arg_idx = 0;
    sctx_T	script_ctx = current_sctx;

    लागि (current = 0; current < instr_गन्ती; ++current)
    {
	isn_T	    *iptr = &instr[current];
	वर्ण	    *पङ्क्ति;

	यदि (ufunc != रिक्त)
	{
	    जबसम्म (पङ्क्ति_idx < iptr->isn_lnum
					  && पङ्क्ति_idx < ufunc->uf_पङ्क्तिहरू.ga_len)
	    {
		यदि (current > prev_current)
		{
		    सन्देश_puts("\n\n");
		    prev_current = current;
		}
		पङ्क्ति = ((वर्ण **)ufunc->uf_पङ्क्तिहरू.ga_data)[पङ्क्ति_idx++];
		यदि (पङ्क्ति != रिक्त)
		    सन्देश(पङ्क्ति);
	    }
	    यदि (iptr->isn_type == ISN_JUMP_IF_ARG_SET)
	    {
		पूर्णाङ्क	first_def_arg = ufunc->uf_args.ga_len
						   - ufunc->uf_def_args.ga_len;

		यदि (def_arg_idx > 0)
		    सन्देश_puts("\n\n");
		सन्देश_start();
		सन्देश_puts("  ");
		सन्देश_puts(((वर्ण **)(ufunc->uf_args.ga_data))[
						 first_def_arg + def_arg_idx]);
		सन्देश_puts(" = ");
		सन्देश_puts(((वर्ण **)(ufunc->uf_def_args.ga_data))[def_arg_idx++]);
		सन्देश_clr_eos();
		सन्देश_end();
	    }
	}

	विकल्प (iptr->isn_type)
	{
	    पक्ष ISN_CONSTRUCT:
		smsg("%s%4d NEW %s size %d", pfx, current,
			iptr->isn_arg.construct.construct_class->class_name.string,
				  (पूर्णाङ्क)iptr->isn_arg.construct.construct_size);
		रोक;
	    पक्ष ISN_EXEC:
		smsg("%s%4d EXEC %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_EXEC_SPLIT:
		smsg("%s%4d EXEC_SPLIT %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_EXECRANGE:
		smsg("%s%4d EXECRANGE %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_LEGACY_EVAL:
		smsg("%s%4d EVAL legacy %s", pfx, current,
							 iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_REDIRSTART:
		smsg("%s%4d REDIR", pfx, current);
		रोक;
	    पक्ष ISN_REDIREND:
		smsg("%s%4d REDIR END%s", pfx, current,
					iptr->isn_arg.सङ्ख्या ? " append" : "");
		रोक;
	    पक्ष ISN_CEXPR_AUCMD:
#ifdef FEAT_QUICKFIX
		smsg("%s%4d CEXPR pre %s", pfx, current,
				       cexpr_get_auname(iptr->isn_arg.सङ्ख्या));
#endif
		रोक;
	    पक्ष ISN_CEXPR_CORE:
#ifdef FEAT_QUICKFIX
		{
		    cexprref_T	    *cer = iptr->isn_arg.cexpr.cexpr_ref;

		    smsg("%s%4d CEXPR core %s%s \"%s\"", pfx, current,
				       cexpr_get_auname(cer->cer_cmdidx),
				       cer->cer_forceit ? "!" : "",
				       cer->cer_cmdline);
		}
#endif
		रोक;
	    पक्ष ISN_INSTR:
		smsg("%s%4d INSTR", pfx, current);
		list_instructions("    ", iptr->isn_arg.instr, पूर्णांक_उच्च, रिक्त);
		सन्देश("     -------------");
		रोक;
	    पक्ष ISN_SOURCE:
		{
		    scriptitem_T    *si = SCRIPT_ITEM(iptr->isn_arg.सङ्ख्या);

		    smsg("%s%4d SOURCE %s", pfx, current, si->sn_name);
		}
		रोक;
	    पक्ष ISN_SUBSTITUTE:
		{
		    subs_T *subs = &iptr->isn_arg.subs;

		    smsg("%s%4d SUBSTITUTE %s", pfx, current, subs->subs_cmd);
		    list_instructions("    ", subs->subs_instr, पूर्णांक_उच्च, रिक्त);
		    सन्देश("     -------------");
		}
		रोक;
	    पक्ष ISN_EXECCONCAT:
		smsg("%s%4d EXECCONCAT %lld", pfx, current,
					      (varnumber_T)iptr->isn_arg.सङ्ख्या);
		रोक;
	    पक्ष ISN_ECHO:
		{
		    echo_T *echo = &iptr->isn_arg.echo;

		    smsg("%s%4d %s %d", pfx, current,
			    echo->echo_with_white ? "ECHO" : "ECHON",
			    echo->echo_गन्ती);
		}
		रोक;
	    पक्ष ISN_EXECUTE:
		smsg("%s%4d EXECUTE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_ECHOMSG:
		smsg("%s%4d ECHOMSG %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_ECHOWINDOW:
		यदि (iptr->isn_arg.echowin.ewin_time > 0)
		    smsg("%s%4d ECHOWINDOW %d (%ld sec)", pfx, current,
				      iptr->isn_arg.echowin.ewin_गन्ती,
				      iptr->isn_arg.echowin.ewin_time);
		अन्यथा
		    smsg("%s%4d ECHOWINDOW %d", pfx, current,
					     iptr->isn_arg.echowin.ewin_गन्ती);
		रोक;
	    पक्ष ISN_ECHOCONSOLE:
		smsg("%s%4d ECHOCONSOLE %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_ECHOERR:
		smsg("%s%4d ECHOERR %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_LOAD:
		{
		    यदि (iptr->isn_arg.सङ्ख्या < 0)
			smsg("%s%4d LOAD arg[%lld]", pfx, current,
				(varnumber_T)(iptr->isn_arg.सङ्ख्या
							  + थाक_FRAME_SIZE));
		    अन्यथा
			smsg("%s%4d LOAD $%lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		}
		रोक;
	    पक्ष ISN_LOADOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    यदि (outer->outer_idx < 0)
			smsg("%s%4d LOADOUTER level %d arg[%d]", pfx, current,
					outer->outer_depth,
					outer->outer_idx + थाक_FRAME_SIZE);
		    अन्यथा यदि (outer->outer_depth < 0)
			smsg("%s%4d LOADOUTER $%d in loop level %d",
					       pfx, current,
					       outer->outer_idx,
					       -outer->outer_depth);
		    अन्यथा
			smsg("%s%4d LOADOUTER level %d $%d", pfx, current,
					      outer->outer_depth,
					      outer->outer_idx);
		}
		रोक;
	    पक्ष ISN_LOADV:
		smsg("%s%4d LOADV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_LOADSCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    यदि (sv == रिक्त)
			smsg("%s%4d LOADSCRIPT [deleted] from %s",
						    pfx, current, si->sn_name);
		    अन्यथा
			smsg("%s%4d LOADSCRIPT %s-%d from %s", pfx, current,
					    sv->sv_name,
					    sref->sref_idx,
					    si->sn_name);
		}
		रोक;
	    पक्ष ISN_LOADS:
	    पक्ष ISN_LOADEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s s:%s from %s", pfx, current,
			    iptr->isn_type == ISN_LOADS ? "LOADS"
								: "LOADEXPORT",
			    iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		रोक;
	    पक्ष ISN_LOADAUTO:
		smsg("%s%4d LOADAUTO %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_LOADG:
		smsg("%s%4d LOADG g:%s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_LOADB:
		smsg("%s%4d LOADB b:%s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_LOADW:
		smsg("%s%4d LOADW w:%s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_LOADT:
		smsg("%s%4d LOADT t:%s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_LOADGDICT:
		smsg("%s%4d LOAD g:", pfx, current);
		रोक;
	    पक्ष ISN_LOADBDICT:
		smsg("%s%4d LOAD b:", pfx, current);
		रोक;
	    पक्ष ISN_LOADWDICT:
		smsg("%s%4d LOAD w:", pfx, current);
		रोक;
	    पक्ष ISN_LOADTDICT:
		smsg("%s%4d LOAD t:", pfx, current);
		रोक;
	    पक्ष ISN_LOADOPT:
		smsg("%s%4d LOADOPT %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_LOADENV:
		smsg("%s%4d LOADENV %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_LOADREG:
		smsg("%s%4d LOADREG @%c", pfx, current,
						  (पूर्णाङ्क)(iptr->isn_arg.सङ्ख्या));
		रोक;

	    पक्ष ISN_STORE:
		यदि (iptr->isn_arg.सङ्ख्या < 0)
		    smsg("%s%4d STORE arg[%lld]", pfx, current,
				      iptr->isn_arg.सङ्ख्या + थाक_FRAME_SIZE);
		अन्यथा
		    smsg("%s%4d STORE $%lld", pfx, current,
							 iptr->isn_arg.सङ्ख्या);
		रोक;
	    पक्ष ISN_STOREOUTER:
		{
		    isn_outer_T *outer = &iptr->isn_arg.outer;

		    यदि (outer->outer_depth < 0)
			smsg("%s%4d STOREOUTER $%d in loop level %d",
				pfx, current, outer->outer_idx,
				-outer->outer_depth);
		    अन्यथा
			smsg("%s%4d STOREOUTER level %d $%d", pfx, current,
				outer->outer_depth, outer->outer_idx);
		}
		रोक;
	    पक्ष ISN_STOREV:
		smsg("%s%4d STOREV v:%s", pfx, current,
				       get_vim_var_name(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_STOREAUTO:
		smsg("%s%4d STOREAUTO %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_STOREG:
		smsg("%s%4d STOREG %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_STOREB:
		smsg("%s%4d STOREB %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_STOREW:
		smsg("%s%4d STOREW %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_STORET:
		smsg("%s%4d STORET %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_STORES:
	    पक्ष ISN_STOREEXPORT:
		{
		    scriptitem_T *si = SCRIPT_ITEM(
					       iptr->isn_arg.loadstore.ls_sid);

		    smsg("%s%4d %s %s in %s", pfx, current,
				iptr->isn_type == ISN_STORES
						    ? "STORES" : "STOREEXPORT",
				 iptr->isn_arg.loadstore.ls_name, si->sn_name);
		}
		रोक;
	    पक्ष ISN_STORESCRIPT:
		{
		    scriptref_T	    *sref = iptr->isn_arg.script.scriptref;
		    scriptitem_T    *si = SCRIPT_ITEM(sref->sref_sid);
		    svar_T	    *sv;
		    sctx_T	    save_sctx = current_sctx;

		    current_sctx = script_ctx;
		    sv = get_script_svar(sref, -1);
		    current_sctx = save_sctx;
		    यदि (sv == रिक्त)
			smsg("%s%4d STORESCRIPT [deleted] in %s",
						    pfx, current, si->sn_name);
		    अन्यथा
			smsg("%s%4d STORESCRIPT %s-%d in %s", pfx, current,
					     sv->sv_name,
					     sref->sref_idx,
					     si->sn_name);
		}
		रोक;
	    पक्ष ISN_STOREOPT:
	    पक्ष ISN_STOREFUNCOPT:
		smsg("%s%4d %s &%s", pfx, current,
		  iptr->isn_type == ISN_STOREOPT ? "STOREOPT" : "STOREFUNCOPT",
					       iptr->isn_arg.storeopt.so_name);
		रोक;
	    पक्ष ISN_STOREENV:
		smsg("%s%4d STOREENV $%s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_STOREREG:
		smsg("%s%4d STOREREG @%c", pfx, current,
						    (पूर्णाङ्क)iptr->isn_arg.सङ्ख्या);
		रोक;
	    पक्ष ISN_STORENR:
		smsg("%s%4d STORE %lld in $%d", pfx, current,
				iptr->isn_arg.storenr.stnr_val,
				iptr->isn_arg.storenr.stnr_idx);
		रोक;

	    पक्ष ISN_STOREINDEX:
		smsg("%s%4d STOREINDEX %s", pfx, current,
			    vartype_name(iptr->isn_arg.storeindex.si_vartype));
		रोक;

	    पक्ष ISN_STORERANGE:
		smsg("%s%4d STORERANGE", pfx, current);
		रोक;

	    पक्ष ISN_LOAD_CLASSMEMBER:
	    पक्ष ISN_STORE_CLASSMEMBER:
		{
		    class_T *cl = iptr->isn_arg.classmember.cm_class;
		    पूर्णाङ्क	    idx = iptr->isn_arg.classmember.cm_idx;
		    ocmember_T *ocm = &cl->class_class_members[idx];
		    smsg("%s%4d %s CLASSMEMBER %s.%s", pfx, current,
			    iptr->isn_type == ISN_LOAD_CLASSMEMBER
							    ? "LOAD" : "STORE",
			    cl->class_name.string, ocm->ocm_name.string);
		}
		रोक;

	    // constants
	    पक्ष ISN_PUSHNR:
		smsg("%s%4d PUSHNR %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_PUSHBOOL:
	    पक्ष ISN_PUSHSPEC:
		smsg("%s%4d PUSH %s", pfx, current,
				   get_var_special_name(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_PUSHF:
		smsg("%s%4d PUSHF %g", pfx, current, iptr->isn_arg.fnumber);
		रोक;
	    पक्ष ISN_PUSHS:
		smsg("%s%4d PUSHS \"%s\"", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_PUSHBLOB:
		{
		    char_u	*r;
		    char_u	numbuf[NUMBUFLEN];
		    char_u	*tofree;

		    r = blob2string(iptr->isn_arg.blob, &tofree, numbuf);
		    smsg("%s%4d PUSHBLOB %s", pfx, current, r);
		    vim_free(tofree);
		}
		रोक;
	    पक्ष ISN_PUSHFUNC:
		{
		    वर्ण *name = (वर्ण *)iptr->isn_arg.string;

		    smsg("%s%4d PUSHFUNC \"%s\"", pfx, current,
					       name == रिक्त ? "[none]" : name);
		}
		रोक;
	    पक्ष ISN_PUSHCHANNEL:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHCHANNEL 0", pfx, current);
#endif
		रोक;
	    पक्ष ISN_PUSHJOB:
#ifdef FEAT_JOB_CHANNEL
		smsg("%s%4d PUSHJOB \"no process\"", pfx, current);
#endif
		रोक;
	    पक्ष ISN_PUSHOBJ:
		smsg("%s%4d PUSHOBJ null", pfx, current);
		रोक;
	    पक्ष ISN_PUSHCLASS:
		smsg("%s%4d PUSHCLASS %s", pfx, current,
			iptr->isn_arg.classarg == रिक्त ? "null"
				 : (वर्ण *)iptr->isn_arg.classarg->class_name.string);
		रोक;
	    पक्ष ISN_PUSHEXC:
		smsg("%s%4d PUSH v:exception", pfx, current);
		रोक;
	    पक्ष ISN_AUTOLOAD:
		smsg("%s%4d AUTOLOAD %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_UNLET:
		smsg("%s%4d UNLET%s %s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		रोक;
	    पक्ष ISN_UNLETENV:
		smsg("%s%4d UNLETENV%s $%s", pfx, current,
			iptr->isn_arg.unlet.ul_forceit ? "!" : "",
			iptr->isn_arg.unlet.ul_name);
		रोक;
	    पक्ष ISN_UNLETINDEX:
		smsg("%s%4d UNLETINDEX", pfx, current);
		रोक;
	    पक्ष ISN_UNLETRANGE:
		smsg("%s%4d UNLETRANGE", pfx, current);
		रोक;
	    पक्ष ISN_LOCKUNLOCK:
		smsg("%s%4d LOCKUNLOCK %s", pfx, current, iptr->isn_arg.string);
		रोक;
	    पक्ष ISN_LOCKCONST:
		smsg("%s%4d LOCKCONST", pfx, current);
		रोक;
	    पक्ष ISN_NEWLIST:
		smsg("%s%4d NEWLIST size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_NEWTUPLE:
		smsg("%s%4d NEWTUPLE size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_NEWDICT:
		smsg("%s%4d NEWDICT size %lld", pfx, current,
					  (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_NEWPARTIAL:
		smsg("%s%4d NEWPARTIAL", pfx, current);
		रोक;

	    // function call
	    पक्ष ISN_BCALL:
		{
		    cbfunc_T	*cbfunc = &iptr->isn_arg.bfunc;

		    smsg("%s%4d BCALL %s(argc %d)", pfx, current,
			    internal_func_name(cbfunc->cbf_idx),
			    cbfunc->cbf_argcount);
		}
		रोक;
	    पक्ष ISN_DCALL:
		{
		    cdfunc_T	*cdfunc = &iptr->isn_arg.dfunc;
		    dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
							     + cdfunc->cdf_idx;

		    smsg("%s%4d DCALL %s(argc %d)", pfx, current,
					    printable_func_name(df->df_ufunc),
							 cdfunc->cdf_argcount);
		}
		रोक;
	    पक्ष ISN_METHODCALL:
		{
		    cmfunc_T	*mfunc = iptr->isn_arg.mfunc;

		    smsg("%s%4d METHODCALL %s.%s(argc %d)", pfx, current,
			    mfunc->cmf_itf->class_name.string,
			    mfunc->cmf_ufunc != रिक्त
				? mfunc->cmf_ufunc->uf_name
				: mfunc->cmf_itf->class_obj_methods[
						      mfunc->cmf_idx]->uf_name,
			    mfunc->cmf_argcount);
		}
		रोक;
	    पक्ष ISN_UCALL:
		{
		    cufunc_T	*cufunc = &iptr->isn_arg.ufunc;

		    smsg("%s%4d UCALL %s(argc %d)", pfx, current,
				       cufunc->cuf_name, cufunc->cuf_argcount);
		}
		रोक;
	    पक्ष ISN_PCALL:
		{
		    cpfunc_T	*cpfunc = &iptr->isn_arg.pfunc;

		    smsg("%s%4d PCALL%s (argc %d)", pfx, current,
			   cpfunc->cpf_शीर्ष ? " top" : "", cpfunc->cpf_argcount);
		}
		रोक;
	    पक्ष ISN_PCALL_END:
		smsg("%s%4d PCALL end", pfx, current);
		रोक;
	    पक्ष ISN_DEFER:
		smsg("%s%4d DEFER %d args", pfx, current,
				      (पूर्णाङ्क)iptr->isn_arg.defer.defer_argcount);
		रोक;
	    पक्ष ISN_RETURN:
		smsg("%s%4d RETURN", pfx, current);
		रोक;
	    पक्ष ISN_RETURN_VOID:
		smsg("%s%4d RETURN void", pfx, current);
		रोक;
	    पक्ष ISN_RETURN_OBJECT:
		smsg("%s%4d RETURN object", pfx, current);
		रोक;
	    पक्ष ISN_FUNCREF:
		{
		    funcref_T		*funcref = &iptr->isn_arg.funcref;
		    funcref_extra_T	*extra = funcref->fr_extra;
		    char_u		*name;

		    यदि (extra == रिक्त || extra->fre_func_name == रिक्त)
		    {
			dfunc_T	*df = ((dfunc_T *)def_functions.ga_data)
						       + funcref->fr_dfunc_idx;
			name = df->df_ufunc->uf_name;
		    }
		    अन्यथा
			name = extra->fre_func_name;
		    यदि (extra != रिक्त && extra->fre_class != रिक्त)
		    {
			smsg("%s%4d FUNCREF %s.%s", pfx, current,
					   extra->fre_class->class_name.string, name);
		    }
		    अन्यथा यदि (extra == रिक्त
				     || extra->fre_loopvar_info.lvi_depth == 0)
		    {
			smsg("%s%4d FUNCREF %s", pfx, current, name);
		    }
		    अन्यथा
		    {
			char_u	*info = printable_loopvarinfo(
						     &extra->fre_loopvar_info);

			smsg("%s%4d FUNCREF %s vars %s", pfx, current,
								   name, info);
			vim_free(info);
		    }
		}
		रोक;

	    पक्ष ISN_NEWFUNC:
		{
		    newfuncarg_T    *arg = iptr->isn_arg.newfunc.nf_arg;

		    यदि (arg->nfa_loopvar_info.lvi_depth == 0)
			smsg("%s%4d NEWFUNC %s %s", pfx, current,
					     arg->nfa_lambda, arg->nfa_global);
		    अन्यथा
		    {
			char_u	*info = printable_loopvarinfo(
						       &arg->nfa_loopvar_info);

			smsg("%s%4d NEWFUNC %s %s vars %s", pfx, current,
				       arg->nfa_lambda, arg->nfa_global, info);
			vim_free(info);
		    }
		}
		रोक;

	    पक्ष ISN_DEF:
		{
		    char_u *name = iptr->isn_arg.string;

		    smsg("%s%4d DEF %s", pfx, current,
					   name == रिक्त ? (char_u *)"" : name);
		}
		रोक;

	    पक्ष ISN_JUMP:
		{
		    वर्ण *when = "?";

		    विकल्प (iptr->isn_arg.jump.jump_when)
		    {
			पक्ष JUMP_ALWAYS:
			    when = "JUMP";
			    रोक;
			पक्ष JUMP_NEVER:
			    iemsg("JUMP_NEVER should not be used");
			    रोक;
			पक्ष JUMP_AND_KEEP_IF_TRUE:
			    when = "JUMP_AND_KEEP_IF_TRUE";
			    रोक;
			पक्ष JUMP_IF_FALSE:
			    when = "JUMP_IF_FALSE";
			    रोक;
			पक्ष JUMP_WHILE_FALSE:
			    when = "JUMP_WHILE_FALSE";  // unused
			    रोक;
			पक्ष JUMP_IF_COND_FALSE:
			    when = "JUMP_IF_COND_FALSE";
			    रोक;
			पक्ष JUMP_IF_COND_TRUE:
			    when = "JUMP_IF_COND_TRUE";
			    रोक;
		    }
		    smsg("%s%4d %s -> %d", pfx, current, when,
						iptr->isn_arg.jump.jump_where);
		}
		रोक;

	    पक्ष ISN_JUMP_IF_ARG_SET:
		smsg("%s%4d JUMP_IF_ARG_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + थाक_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		रोक;

	    पक्ष ISN_JUMP_IF_ARG_NOT_SET:
		smsg("%s%4d JUMP_IF_ARG_NOT_SET arg[%d] -> %d", pfx, current,
			 iptr->isn_arg.jumparg.jump_arg_off + थाक_FRAME_SIZE,
						iptr->isn_arg.jump.jump_where);
		रोक;

	    पक्ष ISN_FOR:
		{
		    forloop_T *forloop = &iptr->isn_arg.forloop;

		    smsg("%s%4d FOR $%d -> %d", pfx, current,
				      forloop->for_loop_idx, forloop->for_end);
		}
		रोक;

	    पक्ष ISN_ENDLOOP:
		{
		    endloop_T *endloop = &iptr->isn_arg.endloop;

		    smsg("%s%4d ENDLOOP ref $%d save $%d-$%d depth %d",
								  pfx, current,
			    endloop->end_funcref_idx,
			    endloop->end_var_idx,
			    endloop->end_var_idx + endloop->end_var_गन्ती - 1,
			    endloop->end_depth);
		}
		रोक;

	    पक्ष ISN_WHILE:
		{
		    whileloop_T *whileloop = &iptr->isn_arg.whileloop;

		    smsg("%s%4d WHILE $%d -> %d", pfx, current,
					       whileloop->while_funcref_idx,
					       whileloop->while_end);
		}
		रोक;

	    पक्ष ISN_TRY:
		{
		    try_T *try = &iptr->isn_arg.tryref;

		    यदि (try->try_ref->try_finally == 0)
			smsg("%s%4d TRY catch -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_endtry);
		    अन्यथा
			smsg("%s%4d TRY catch -> %d, finally -> %d, endtry -> %d",
				pfx, current,
				try->try_ref->try_catch,
				try->try_ref->try_finally,
				try->try_ref->try_endtry);
		}
		रोक;
	    पक्ष ISN_CATCH:
		smsg("%s%4d CATCH", pfx, current);
		रोक;
	    पक्ष ISN_TRYCONT:
		{
		    trycont_T *trycont = &iptr->isn_arg.trycont;

		    smsg("%s%4d TRY-CONTINUE %d level%s -> %d", pfx, current,
				      trycont->tct_levels,
				      trycont->tct_levels == 1 ? "" : "s",
				      trycont->tct_where);
		}
		रोक;
	    पक्ष ISN_FINALLY:
		smsg("%s%4d FINALLY", pfx, current);
		रोक;
	    पक्ष ISN_ENDTRY:
		smsg("%s%4d ENDTRY", pfx, current);
		रोक;
	    पक्ष ISN_THROW:
		smsg("%s%4d THROW", pfx, current);
		रोक;

	    // expression operations on number
	    पक्ष ISN_OPNR:
	    पक्ष ISN_OPFLOAT:
	    पक्ष ISN_OPANY:
		{
		    वर्ण *what;
		    वर्ण *ins;

		    विकल्प (iptr->isn_arg.op.op_type)
		    {
			पक्ष अभिव्यक्ति_MULT: what = "*"; रोक;
			पक्ष अभिव्यक्ति_DIV: what = "/"; रोक;
			पक्ष अभिव्यक्ति_REM: what = "%"; रोक;
			पक्ष अभिव्यक्ति_SUB: what = "-"; रोक;
			पक्ष अभिव्यक्ति_ADD: what = "+"; रोक;
			पक्ष अभिव्यक्ति_LSHIFT: what = "<<"; रोक;
			पक्ष अभिव्यक्ति_RSHIFT: what = ">>"; रोक;
			स्वभाव:       what = "???"; रोक;
		    }
		    विकल्प (iptr->isn_type)
		    {
			पक्ष ISN_OPNR: ins = "OPNR"; रोक;
			पक्ष ISN_OPFLOAT: ins = "OPFLOAT"; रोक;
			पक्ष ISN_OPANY: ins = "OPANY"; रोक;
			स्वभाव: ins = "???"; रोक;
		    }
		    smsg("%s%4d %s %s", pfx, current, ins, what);
		}
		रोक;

	    पक्ष ISN_COMPAREBOOL:
	    पक्ष ISN_COMPARESPECIAL:
	    पक्ष ISN_COMPARENULL:
	    पक्ष ISN_COMPARENR:
	    पक्ष ISN_COMPAREFLOAT:
	    पक्ष ISN_COMPARESTRING:
	    पक्ष ISN_COMPAREBLOB:
	    पक्ष ISN_COMPARELIST:
	    पक्ष ISN_COMPARETUPLE:
	    पक्ष ISN_COMPAREDICT:
	    पक्ष ISN_COMPAREFUNC:
	    पक्ष ISN_COMPAREOBJECT:
	    पक्ष ISN_COMPAREANY:
		   {
		       वर्ण *p;
		       वर्ण buf[10];
		       वर्ण *type;

		       विकल्प (iptr->isn_arg.op.op_type)
		       {
			   पक्ष अभिव्यक्ति_EQUAL:	 p = "=="; रोक;
			   पक्ष अभिव्यक्ति_NEQUAL:    p = "!="; रोक;
			   पक्ष अभिव्यक्ति_GREATER:   p = ">"; रोक;
			   पक्ष अभिव्यक्ति_GEQUAL:    p = ">="; रोक;
			   पक्ष अभिव्यक्ति_SMALLER:   p = "<"; रोक;
			   पक्ष अभिव्यक्ति_SEQUAL:    p = "<="; रोक;
			   पक्ष अभिव्यक्ति_MATCH:	 p = "=~"; रोक;
			   पक्ष अभिव्यक्ति_IS:	 p = "is"; रोक;
			   पक्ष अभिव्यक्ति_ISNOT:	 p = "isnot"; रोक;
			   पक्ष अभिव्यक्ति_NOMATCH:	 p = "!~"; रोक;
			   स्वभाव:  p = "???"; रोक;
		       }
		       STRCPY(buf, p);
		       यदि (iptr->isn_arg.op.op_ic == TRUE)
			   म_जोड़ो(buf, "?");
		       विकल्प(iptr->isn_type)
		       {
			   पक्ष ISN_COMPAREBOOL: type = "COMPAREBOOL"; रोक;
			   पक्ष ISN_COMPARESPECIAL:
						 type = "COMPARESPECIAL"; रोक;
			   पक्ष ISN_COMPARENULL: type = "COMPARENULL"; रोक;
			   पक्ष ISN_COMPARENR: type = "COMPARENR"; रोक;
			   पक्ष ISN_COMPAREFLOAT: type = "COMPAREFLOAT"; रोक;
			   पक्ष ISN_COMPARESTRING:
						  type = "COMPARESTRING"; रोक;
			   पक्ष ISN_COMPAREBLOB: type = "COMPAREBLOB"; रोक;
			   पक्ष ISN_COMPARELIST: type = "COMPARELIST"; रोक;
			   पक्ष ISN_COMPARETUPLE: type = "COMPARETUPLE"; रोक;
			   पक्ष ISN_COMPAREDICT: type = "COMPAREDICT"; रोक;
			   पक्ष ISN_COMPAREFUNC: type = "COMPAREFUNC"; रोक;
			   पक्ष ISN_COMPAREOBJECT:
						 type = "COMPAREOBJECT"; रोक;
			   पक्ष ISN_COMPAREANY: type = "COMPAREANY"; रोक;
			   स्वभाव: type = "???"; रोक;
		       }

		       smsg("%s%4d %s %s", pfx, current, type, buf);
		   }
		   रोक;

	    पक्ष ISN_ADDLIST: smsg("%s%4d ADDLIST", pfx, current); रोक;
	    पक्ष ISN_ADDTUPLE: smsg("%s%4d ADDTUPLE", pfx, current); रोक;
	    पक्ष ISN_ADDBLOB: smsg("%s%4d ADDBLOB", pfx, current); रोक;

	    // expression operations
	    पक्ष ISN_CONCAT:
		smsg("%s%4d CONCAT size %lld", pfx, current,
					    (varnumber_T)(iptr->isn_arg.सङ्ख्या));
		रोक;
	    पक्ष ISN_STRINDEX: smsg("%s%4d STRINDEX", pfx, current); रोक;
	    पक्ष ISN_STRSLICE: smsg("%s%4d STRSLICE", pfx, current); रोक;
	    पक्ष ISN_BLOBINDEX: smsg("%s%4d BLOBINDEX", pfx, current); रोक;
	    पक्ष ISN_BLOBSLICE: smsg("%s%4d BLOBSLICE", pfx, current); रोक;
	    पक्ष ISN_LISTAPPEND: smsg("%s%4d LISTAPPEND", pfx, current); रोक;
	    पक्ष ISN_BLOBAPPEND: smsg("%s%4d BLOBAPPEND", pfx, current); रोक;
	    पक्ष ISN_LISTINDEX: smsg("%s%4d LISTINDEX", pfx, current); रोक;
	    पक्ष ISN_LISTSLICE: smsg("%s%4d LISTSLICE", pfx, current); रोक;
	    पक्ष ISN_TUPLEINDEX: smsg("%s%4d TUPLEINDEX", pfx, current); रोक;
	    पक्ष ISN_TUPLESLICE: smsg("%s%4d TUPLESLICE", pfx, current); रोक;
	    पक्ष ISN_ANYINDEX: smsg("%s%4d ANYINDEX", pfx, current); रोक;
	    पक्ष ISN_ANYSLICE: smsg("%s%4d ANYSLICE", pfx, current); रोक;
	    पक्ष ISN_SLICE: smsg("%s%4d SLICE %lld",
				    pfx, current, iptr->isn_arg.सङ्ख्या); रोक;
	    पक्ष ISN_GETITEM: smsg("%s%4d ITEM %lld%s", pfx, current,
					 iptr->isn_arg.getitem.gi_index,
					 iptr->isn_arg.getitem.gi_with_op ?
						       " with op" : ""); रोक;
	    पक्ष ISN_MEMBER: smsg("%s%4d MEMBER", pfx, current); रोक;
	    पक्ष ISN_STRINGMEMBER: smsg("%s%4d MEMBER %s", pfx, current,
						  iptr->isn_arg.string); रोक;
	    पक्ष ISN_GET_OBJ_MEMBER: smsg("%s%4d OBJ_MEMBER %d", pfx, current,
			     (पूर्णाङ्क)iptr->isn_arg.classmember.cm_idx);
				     रोक;
	    पक्ष ISN_GET_ITF_MEMBER: smsg("%s%4d ITF_MEMBER %d on %s",
			     pfx, current,
			     (पूर्णाङ्क)iptr->isn_arg.classmember.cm_idx,
			     iptr->isn_arg.classmember.cm_class->class_name.string);
				     रोक;
	    पक्ष ISN_STORE_THIS: smsg("%s%4d STORE_THIS %d", pfx, current,
					     (पूर्णाङ्क)iptr->isn_arg.सङ्ख्या); रोक;
	    पक्ष ISN_CLEARDICT: smsg("%s%4d CLEARDICT", pfx, current); रोक;
	    पक्ष ISN_USEDICT: smsg("%s%4d USEDICT", pfx, current); रोक;

	    पक्ष ISN_NEGATENR: smsg("%s%4d NEGATENR", pfx, current); रोक;

	    पक्ष ISN_CHECKTYPE:
		  {
		      checktype_T   *ct = &iptr->isn_arg.type;
		      वर्ण	    *tofree = रिक्त;
		      वर्ण	    *typename;

		      यदि (ct->ct_type->tt_type == VAR_FLOAT
			      && (ct->ct_type->tt_flags & TTFLAG_सङ्ख्या_OK))
			  typename = "float|number";
		      अन्यथा यदि (ct->ct_type->tt_type == VAR_LIST
			      && (ct->ct_type->tt_flags & TTFLAG_TUPLE_OK))
			  typename = "list<any>|tuple<any>";
		      अन्यथा
			  typename = type_name(ct->ct_type, &tofree);

		      यदि (ct->ct_arg_idx == 0)
			  smsg("%s%4d CHECKTYPE %s stack[%d]", pfx, current,
					  typename,
					  (पूर्णाङ्क)ct->ct_off);
		      अन्यथा
			  smsg("%s%4d CHECKTYPE %s stack[%d] %s %d",
					  pfx, current,
					  typename,
					  (पूर्णाङ्क)ct->ct_off,
					  ct->ct_is_var ? "var": "arg",
					  (पूर्णाङ्क)ct->ct_arg_idx);
		      vim_free(tofree);
		      रोक;
		  }
	    पक्ष ISN_CHECKLEN: smsg("%s%4d CHECKLEN %s%d", pfx, current,
				iptr->isn_arg.checklen.cl_more_OK ? ">= " : "",
				iptr->isn_arg.checklen.cl_min_len);
			       रोक;
	    पक्ष ISN_SETTYPE:
		  {
		      वर्ण *tofree;

		      smsg("%s%4d SETTYPE %s", pfx, current,
			      type_name(iptr->isn_arg.type.ct_type, &tofree));
		      vim_free(tofree);
		      रोक;
		  }
	    पक्ष ISN_COND2BOOL: smsg("%s%4d COND2BOOL", pfx, current); रोक;
	    पक्ष ISN_2BOOL: यदि (iptr->isn_arg.tobool.invert)
				smsg("%s%4d INVERT %d (!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    अन्यथा
				smsg("%s%4d 2BOOL %d (!!val)", pfx, current,
					 iptr->isn_arg.tobool.offset);
			    रोक;
	    पक्ष ISN_2STRING: smsg("%s%4d 2STRING stack[%lld]", pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      रोक;
	    पक्ष ISN_2STRING_ANY: smsg("%s%4d 2STRING_ANY stack[%lld]",
								  pfx, current,
				 (varnumber_T)(iptr->isn_arg.tostring.offset));
			      रोक;
	    पक्ष ISN_RANGE: smsg("%s%4d RANGE %s", pfx, current,
							 iptr->isn_arg.string);
			    रोक;
	    पक्ष ISN_PUT:
		यदि (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d PUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		अन्यथा यदि (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d PUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		अन्यथा
		    smsg("%s%4d PUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (दीर्घ)iptr->isn_arg.put.put_lnum);
		रोक;
	    पक्ष ISN_IPUT:
		यदि (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE_ABOVE)
		    smsg("%s%4d IPUT %c above range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		अन्यथा यदि (iptr->isn_arg.put.put_lnum == LNUM_VARIABLE_RANGE)
		    smsg("%s%4d IPUT %c range",
				  pfx, current, iptr->isn_arg.put.put_regname);
		अन्यथा
		    smsg("%s%4d IPUT %c %ld", pfx, current,
						 iptr->isn_arg.put.put_regname,
					     (दीर्घ)iptr->isn_arg.put.put_lnum);
		रोक;
	    पक्ष ISN_CMDMOD:
		{
		    char_u  *buf;
		    माप_प्रकार  len = produce_cmdmods(
				  रिक्त, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);

		    buf = alloc(len + 1);
		    यदि (likely(buf != रिक्त))
		    {
			(शून्य)produce_cmdmods(
				   buf, iptr->isn_arg.cmdmod.cf_cmdmod, FALSE);
			smsg("%s%4d CMDMOD %s", pfx, current, buf);
			vim_free(buf);
		    }
		    रोक;
		}
	    पक्ष ISN_CMDMOD_REV: smsg("%s%4d CMDMOD_REV", pfx, current); रोक;

	    पक्ष ISN_PROF_START:
		 smsg("%s%4d PROFILE START line %d", pfx, current,
							       iptr->isn_lnum);
		 रोक;

	    पक्ष ISN_PROF_END:
		smsg("%s%4d PROFILE END", pfx, current);
		रोक;

	    पक्ष ISN_DEBUG:
		smsg("%s%4d DEBUG line %d-%d varcount %lld", pfx, current,
			iptr->isn_arg.debug.dbg_break_lnum + 1,
			iptr->isn_lnum,
			iptr->isn_arg.debug.dbg_var_names_len);
		रोक;

	    पक्ष ISN_UNPACK: smsg("%s%4d UNPACK %d%s", pfx, current,
			iptr->isn_arg.unpack.unp_गन्ती,
			iptr->isn_arg.unpack.unp_semicolon ? " semicolon" : "");
			      रोक;
	    पक्ष ISN_SHUFFLE: smsg("%s%4d SHUFFLE %d up %d", pfx, current,
					 iptr->isn_arg.shuffle.shfl_item,
					 iptr->isn_arg.shuffle.shfl_up);
			      रोक;
	    पक्ष ISN_DROP: smsg("%s%4d DROP", pfx, current); रोक;

	    पक्ष ISN_SCRIPTCTX_SET:
		{
		    पूर्णाङ्क	sid = iptr->isn_arg.setsctx.sc_sid;
		    scriptitem_T *si = SCRIPT_ITEM(sid);
		    smsg("%s%4d SCRIPTCTX_SET %s", pfx, current, si->sn_name);
		    script_ctx = iptr->isn_arg.setsctx;
		}
		रोक;

	    पक्ष ISN_FINISH: // End of list of instructions for ISN_SUBSTITUTE.
			   फर्काऊ;
	}

	out_flush();	    // output one line at a time
	ui_breakcheck();
	यदि (got_int)
	    रोक;
    }
}

/*
 * Handle command line completion for the :disassemble command.
 */
    शून्य
set_context_in_disassemble_cmd(expand_T *xp, char_u *arg)
{
    char_u	*p;

    // Default: expand user functions, "debug" and "profile"
    xp->xp_context = EXPAND_DISASSEMBLE;
    xp->xp_pattern = arg;

    // first argument already typed: only user function names
    यदि (*arg != NUL && *(p = skiptowhite(arg)) != NUL)
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
get_disassemble_argument(expand_T *xp, पूर्णाङ्क idx)
{
    यदि (idx == 0)
	फर्काऊ (char_u *)"debug";
    यदि (idx == 1)
	फर्काऊ (char_u *)"profile";
    फर्काऊ get_user_func_name(xp, idx - 2);
}

/*
 * ":disassemble".
 * We don't really need this at runtime, but we do have tests that require it,
 * so always include this.
 */
    शून्य
ex_disassemble(exarg_T *eap)
{
    char_u	*arg = eap->arg;
    ufunc_T	*ufunc;
    dfunc_T	*dfunc;
    isn_T	*instr = रिक्त;  // init to shut up gcc warning
    पूर्णाङ्क		instr_गन्ती = 0;  // init to shut up gcc warning
    compiletype_T compile_type = CT_NONE;

    ufunc = find_func_by_name(arg, &compile_type);
    यदि (ufunc == रिक्त)
	फर्काऊ;
    यदि (func_needs_compiling(ufunc, compile_type)
	    && compile_def_function(ufunc, FALSE, compile_type, रिक्त) == FAIL)
	फर्काऊ;
    यदि (ufunc->uf_def_status != UF_COMPILED)
    {
	semsg(_(e_function_is_not_compiled_str), eap->arg);
	फर्काऊ;
    }
    सन्देश((वर्ण *)printable_func_name(ufunc));

    dfunc = ((dfunc_T *)def_functions.ga_data) + ufunc->uf_dfunc_idx;
    विकल्प (compile_type)
    {
	पक्ष CT_PROFILE:
#ifdef FEAT_PROFILE
	    instr = dfunc->df_instr_prof;
	    instr_गन्ती = dfunc->df_instr_prof_गन्ती;
	    रोक;
#endif
	    // FALLTHROUGH
	पक्ष CT_NONE:
	    instr = dfunc->df_instr;
	    instr_गन्ती = dfunc->df_instr_गन्ती;
	    रोक;
	पक्ष CT_DEBUG:
	    instr = dfunc->df_instr_debug;
	    instr_गन्ती = dfunc->df_instr_debug_गन्ती;
	    रोक;
    }

    list_instructions("", instr, instr_गन्ती, ufunc);
}

/*
 * Return TRUE when "tv" is not falsy: non-zero, non-empty string, non-empty
 * list, etc.  Mostly like what JavaScript does, except that empty list and
 * empty dictionary are FALSE.
 */
    पूर्णाङ्क
tv2bool(typval_T *tv)
{
    विकल्प (tv->v_type)
    {
	पक्ष VAR_सङ्ख्या:
	    फर्काऊ tv->vval.v_सङ्ख्या != 0;
	पक्ष VAR_FLOAT:
	    फर्काऊ tv->vval.v_float != 0.0;
	पक्ष VAR_PARTIAL:
	    फर्काऊ tv->vval.v_partial != रिक्त;
	पक्ष VAR_FUNC:
	पक्ष VAR_STRING:
	    फर्काऊ tv->vval.v_string != रिक्त && *tv->vval.v_string != NUL;
	पक्ष VAR_LIST:
	    फर्काऊ tv->vval.v_list != रिक्त && tv->vval.v_list->lv_len > 0;
	पक्ष VAR_TUPLE:
	    फर्काऊ tuple_len(tv->vval.v_tuple) > 0;
	पक्ष VAR_DICT:
	    फर्काऊ tv->vval.v_dict != रिक्त
				    && tv->vval.v_dict->dv_hashtab.ht_used > 0;
	पक्ष VAR_BOOL:
	पक्ष VAR_SPECIAL:
	    फर्काऊ tv->vval.v_सङ्ख्या == VVAL_TRUE ? TRUE : FALSE;
	पक्ष VAR_JOB:
#ifdef FEAT_JOB_CHANNEL
	    फर्काऊ tv->vval.v_job != रिक्त;
#अन्यथा
	    रोक;
#endif
	पक्ष VAR_CHANNEL:
#ifdef FEAT_JOB_CHANNEL
	    फर्काऊ tv->vval.v_channel != रिक्त;
#अन्यथा
	    रोक;
#endif
	पक्ष VAR_BLOB:
	    फर्काऊ tv->vval.v_blob != रिक्त && tv->vval.v_blob->bv_ga.ga_len > 0;

	पक्ष VAR_OBJECT:
	    फर्काऊ tv->vval.v_object != रिक्त;

	पक्ष VAR_CLASS:
	पक्ष VAR_TYPEALIAS:
	    check_typval_is_मान(tv);
	    रोक;

	पक्ष VAR_UNKNOWN:
	पक्ष VAR_ANY:
	पक्ष VAR_VOID:
	पक्ष VAR_INSTR:
	    रोक;
    }
    फर्काऊ FALSE;
}

    शून्य
emsg_using_string_as(typval_T *tv, पूर्णाङ्क as_सङ्ख्या)
{
    semsg(_(as_सङ्ख्या ? e_using_string_as_सङ्ख्या_str
						 : e_using_string_as_bool_str),
		       tv->vval.v_string == रिक्त
					   ? (char_u *)"" : tv->vval.v_string);
}

/*
 * If "tv" is a string give an error and return FAIL.
 */
    पूर्णाङ्क
check_not_string(typval_T *tv)
{
    यदि (tv->v_type == VAR_STRING)
    {
	emsg_using_string_as(tv, TRUE);
	clear_tv(tv);
	फर्काऊ FAIL;
    }
    फर्काऊ OK;
}

#endif // FEAT_EVAL
