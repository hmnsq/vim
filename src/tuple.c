/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * tuple.c: Tuple support functions.
 */

#include "vim.h"

#もし defined(FEAT_EVAL)

// Tuple heads for garbage collection.
static tuple_T		*first_tuple = NULL;	// list of all tuples

    static void
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    もし (first_tuple != NULL)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = NULL;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_items, sizeof(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(void)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    もし (tuple != NULL)
	tuple_init(tuple);
    戻る tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_items(整数 count)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    もし (tuple == NULL)
	戻る NULL;

    もし (count <= 0)
	戻る tuple;

    もし (ga_grow(&tuple->tv_items, count) == FAIL)
    {
	tuple_free(tuple);
	戻る NULL;
    }

    戻る tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    void
tuple_set_item(tuple_T *tuple, 整数 idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_items.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    整数
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    もし (tuple == NULL)
	戻る FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    戻る OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    void
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    もし (tuple != NULL)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    整数
rettv_tuple_set_with_items(typval_T *rettv, 整数 count)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_items(count);
    もし (new_tuple == NULL)
	戻る FAIL;

    rettv_tuple_set(rettv, new_tuple);

    戻る OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    void
tuple_unref(tuple_T *tuple)
{
    もし (tuple != NULL && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    static void
tuple_free_contents(tuple_T *tuple)
{
    ために (整数 i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_items);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    整数
tuple_free_nonref(整数 copyID)
{
    tuple_T	*tt;
    整数		did_free = FALSE;

    ために (tt = first_tuple; tt != NULL; tt = tt->tv_used_next)
	もし ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    戻る did_free;
}

    static void
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    もし (tuple->tv_used_prev == NULL)
	first_tuple = tuple->tv_used_next;
    否则
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    もし (tuple->tv_used_next != NULL)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    void
tuple_free_items(整数 copyID)
{
    tuple_T	*tt, *tt_next;

    ために (tt = first_tuple; tt != NULL; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	もし ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    void
tuple_free(tuple_T *tuple)
{
    もし (in_free_unref_items)
	戻る;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    もし (tuple == NULL)
	戻る 0L;
    戻る tuple->tv_items.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    整数
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    整数		ic)	// ignore case for strings
{
    もし (t1 == t2)
	戻る TRUE;

    整数 t1_len = tuple_len(t1);
    整数 t2_len = tuple_len(t2);

    もし (t1_len != t2_len)
	戻る FALSE;

    もし (t1_len == 0)
	// empty and NULL tuples are considered equal
	戻る TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    ために (整数 i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	もし (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    戻る FALSE;

    戻る TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    もし (tuple == NULL)
	戻る NULL;

    // Negative index is relative to the end.
    もし (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    もし (n < 0 || n >= TUPLE_LEN(tuple))
	戻る NULL;

    戻る TUPLE_ITEM(tuple, n);
}

    整数
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    もし (ga_grow_id(&tuple->tv_items, 1, aid_tuple_append) == FAIL)
	戻る FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    戻る OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    整数
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    もし (t1 == NULL)
	tuple = tuple_alloc();
    否则
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    もし (tuple == NULL)
	戻る FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    もし (t1 == NULL)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    ために (整数 i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	もし (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    戻る FAIL;
	}
    }

    戻る OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_items(n2 - n1 + 1);
    もし (new_tuple == NULL)
	戻る NULL;

    ために (整数 i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	もし (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    戻る NULL;
	}
    }

    戻る new_tuple;
}

    整数
tuple_slice_or_index(
    tuple_T	*tuple,
    整数		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    整数		exclusive,
    typval_T	*rettv,
    整数		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    もし (n1 < 0)
	n1 = len + n1;
    もし (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	もし (!range)
	{
	    もし (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    戻る FAIL;
	}
	もし (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	否则
	    n1 = len;
    }
    もし (range)
    {
	tuple_T	*new_tuple;

	もし (n2 < 0)
	    n2 = len + n2;
	否则 もし (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	もし (exclusive)
	    --n2;
	もし (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	もし (new_tuple == NULL)
	    戻る FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    否则
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    戻る OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, 整数 deep, 整数 top, 整数 copyID)
{
    tuple_T	*copy;
    整数		idx;

    もし (orig == NULL)
	戻る NULL;

    copy = tuple_alloc_with_items(TUPLE_LEN(orig));
    もし (copy == NULL)
	戻る NULL;

    もし (orig->tv_type == NULL || top || deep)
	copy->tv_type = NULL;
    否则
	copy->tv_type = alloc_type(orig->tv_type);
    もし (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    ために (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_items.ga_len++;
	もし (deep)
	{
	    もし (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		break;
	}
	否则
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    もし (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = NULL;
    }

    戻る copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    整数
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, 整数 do_error)
{
    整数		evaluate = evalarg == NULL ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = NULL;
    typval_T	tv;
    整数		vim9script = in_vim9script();
    整数		had_comma;

    もし (check_typval_is_value(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	戻る FAIL;
    }

    もし (evaluate)
    {
	tuple = tuple_alloc();
	もし (tuple == NULL)
	    戻る FAIL;

	もし (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    もし (tuple_append_tv(tuple, rettv) == FAIL)
		goto failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    もし (**arg == ')')
	// empty tuple
	goto done;

    もし (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	goto failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ながら (**arg != ')' && **arg != NUL)
    {
	もし (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    goto failret;
	もし (check_typval_is_value(&tv) == FAIL)
	{
	    もし (evaluate)
		clear_tv(&tv);
	    goto failret;
	}

	もし (evaluate)
	{
	    もし (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		goto failret;
	    }
	}

	もし (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	もし (had_comma)
	{
	    もし (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		goto failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	もし (**arg == ')')
	    break;

	もし (!had_comma)
	{
	    もし (do_error)
	    {
		もし (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		否则
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    goto failret;
	}
    }

    もし (**arg != ')')
    {
	もし (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	もし (evaluate)
	    tuple_free(tuple);
	戻る FAIL;
    }

done:
    *arg += 1;
    もし (evaluate)
	rettv_tuple_set(rettv, tuple);

    戻る OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    void
tuple_lock(tuple_T *tuple, 整数 deep, 整数 lock, 整数 check_refcount)
{
    もし (tuple == NULL || (check_refcount && tuple->tv_refcount > 1))
	戻る;

    もし (lock)
	tuple->tv_lock |= VAR_LOCKED;
    否则
	tuple->tv_lock &= ~VAR_LOCKED;

    もし (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	ために (整数 i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

typedef struct join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    static 整数
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    整数		echo_style,
    整数		restore_copyID,
    整数		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    整数		i;
    join_T	*p;
    整数		sumlen = 0;
    整数		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    ために (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	もし (s.string == NULL)
	    戻る FAIL;

	s.length = STRLEN(s.string);
	sumlen += (整数)s.length;

	(void)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	もし (tofree != NULL || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	否则
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	line_breakcheck();
	もし (did_echo_string_emsg)  // recursion error, bail out
	    break;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    もし (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	もし (join_gap->ga_len >= 2)
	    sumlen += (整数)seplen * (join_gap->ga_len - 1);
    }
    もし (ga_grow(gap, sumlen + 2) == FAIL)
	戻る FAIL;

    ために (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	もし (first)
	    first = FALSE;
	否则
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	もし (p->s.string != NULL)
	    ga_concat_len(gap, p->s.string, p->s.length);
	line_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    もし (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    戻る OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    整数
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    整数		echo_style,
    整数		restore_copyID,
    整数		copyID)
{
    garray_T	join_ga;
    整数		retval;
    join_T	*p;
    整数		i;

    もし (TUPLE_LEN(tuple) < 1)
	戻る OK; // nothing to do
    ga_init2(&join_ga, sizeof(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    もし (join_ga.ga_data == NULL)
	戻る retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    ために (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    戻る retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, 整数 copyID, 整数 restore_copyID)
{
    garray_T	ga;

    もし (tv->vval.v_tuple == NULL)
	戻る NULL;
    ga_init2(&ga, sizeof(char), 80);
    ga_append(&ga, '(');
    もし (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	戻る NULL;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    戻る (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    void
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*expr)
{
    整数		len = tuple_len(tuple);
    整数		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    ために (整数 idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	もし (filter_map_one(TUPLE_ITEM(tuple, idx), expr, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    break;
    }

    もし (fc != NULL)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_count(tuple_T *tuple, typval_T *needle, long idx, 整数 ic)
{
    long	n = 0;

    もし (tuple == NULL)
	戻る 0;

    整数	len = TUPLE_LEN(tuple);
    もし (len == 0)
	戻る 0;

    もし (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	戻る 0;
    }

    ために (整数 i = idx; i < len; i++)
    {
	もし (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    戻る n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    void
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    もし (rettv_list_alloc(rettv) == FAIL)
	戻る;

    もし (tuple == NULL)
	戻る;  // null tuple behaves like an empty list

    ために (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	もし (l == NULL)
	    break;

	もし (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    break;
	}
	もし (list_append_number(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    break;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    整数
index_tuple(tuple_T *tuple, typval_T *tv, 整数 start_idx, 整数 ic)
{
    もし (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	もし (start_idx < 0)
	    start_idx = 0;
    }

    ために (整数 idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	もし (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    戻る idx;
    }

    戻る -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    整数
indexof_tuple(tuple_T *tuple, long startidx, typval_T *expr)
{
    long	idx = 0;
    整数		len;
    整数		found;

    もし (tuple == NULL)
	戻る -1;

    len = TUPLE_LEN(tuple);

    もし (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	もし (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_KEY, VAR_NUMBER);

    整数		called_emsg_start = called_emsg;

    ために (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_expr(expr);
	clear_tv(get_vim_var_tv(VV_VAL));

	もし (found)
	    戻る idx;

	もし (called_emsg != called_emsg_start)
	    戻る -1;
    }

    戻る -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, 整数 domax)
{
    typval_T	*tv = NULL;
    整数		res;

    もし (tuple == NULL || TUPLE_LEN(tuple) == 0)
	戻る NULL;

    ために (整数 idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	もし (tv == NULL)
	    tv = TUPLE_ITEM(tuple, idx);
	否则
	{
	    もし (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? EXPR_GREATER : EXPR_SMALLER, FALSE, &res) == FAIL)
		戻る NULL;
	    もし (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    戻る tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    void
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = NULL;

    もし (tuple == NULL || TUPLE_LEN(tuple) == 0 || n <= 0)
	戻る;

    もし (check_repeat_count(TUPLE_LEN(tuple), n) == FAIL)
	戻る;

    もし (rettv_tuple_set_with_items(rettv, TUPLE_LEN(tuple) * (整数)n) == FAIL)
	戻る;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ために (整数 count = 0; count < (整数)n; count++)
    {
	ために (整数 idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_items.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    void
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = NULL;

    整数	len = tuple_len(tuple);

    もし (len == 0)
	戻る;

    もし (rettv_tuple_set_with_items(rettv, len) == FAIL)
	戻る;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ために (整数 i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_items.ga_len = tuple->tv_items.ga_len;
}

/*
 * Tuple reduce() function
 */
    void
tuple_reduce(typval_T *argvars, typval_T *expr, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    整数		called_emsg_start = called_emsg;
    typval_T	initial;
    整数		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    整数		r;

    もし (argvars[2].v_type == VAR_UNKNOWN)
    {
	もし (tuple == NULL || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Tuple");
	    戻る;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    否则
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    もし (tuple == NULL)
	戻る;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    ために ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	もし (r == FAIL || called_emsg != called_emsg_start)
	    break;
    }

    もし (fc != NULL)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    整数
check_tuples_addable(type_T *type1, type_T *type2)
{
    整数	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    もし ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    もし ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	もし (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	否则 もし ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    もし (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	戻る FAIL;
    }

    戻る OK;
}

#endif // defined(FEAT_EVAL)
