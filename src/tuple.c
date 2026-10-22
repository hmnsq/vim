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

#якщо defined(FEAT_EVAL)

// Tuple heads for garbage collection.
static tuple_T		*first_tuple = NULL;	// list of all tuples

    static void
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    якщо (first_tuple != NULL)
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
    якщо (tuple != NULL)
	tuple_init(tuple);
    повернути tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_items(ціле count)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    якщо (tuple == NULL)
	повернути NULL;

    якщо (count <= 0)
	повернути tuple;

    якщо (ga_grow(&tuple->tv_items, count) == FAIL)
    {
	tuple_free(tuple);
	повернути NULL;
    }

    повернути tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    void
tuple_set_item(tuple_T *tuple, ціле idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_items.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ціле
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    якщо (tuple == NULL)
	повернути FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    повернути OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    void
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    якщо (tuple != NULL)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    ціле
rettv_tuple_set_with_items(typval_T *rettv, ціле count)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_items(count);
    якщо (new_tuple == NULL)
	повернути FAIL;

    rettv_tuple_set(rettv, new_tuple);

    повернути OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    void
tuple_unref(tuple_T *tuple)
{
    якщо (tuple != NULL && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    static void
tuple_free_contents(tuple_T *tuple)
{
    для (ціле i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_items);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ціле
tuple_free_nonref(ціле copyID)
{
    tuple_T	*tt;
    ціле		did_free = FALSE;

    для (tt = first_tuple; tt != NULL; tt = tt->tv_used_next)
	якщо ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    повернути did_free;
}

    static void
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    якщо (tuple->tv_used_prev == NULL)
	first_tuple = tuple->tv_used_next;
    інакше
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    якщо (tuple->tv_used_next != NULL)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    void
tuple_free_items(ціле copyID)
{
    tuple_T	*tt, *tt_next;

    для (tt = first_tuple; tt != NULL; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	якщо ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
    якщо (in_free_unref_items)
	повернути;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    якщо (tuple == NULL)
	повернути 0L;
    повернути tuple->tv_items.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    ціле
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    ціле		ic)	// ignore case for strings
{
    якщо (t1 == t2)
	повернути TRUE;

    ціле t1_len = tuple_len(t1);
    ціле t2_len = tuple_len(t2);

    якщо (t1_len != t2_len)
	повернути FALSE;

    якщо (t1_len == 0)
	// empty and NULL tuples are considered equal
	повернути TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    для (ціле i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	якщо (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    повернути FALSE;

    повернути TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    якщо (tuple == NULL)
	повернути NULL;

    // Negative index is relative to the end.
    якщо (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    якщо (n < 0 || n >= TUPLE_LEN(tuple))
	повернути NULL;

    повернути TUPLE_ITEM(tuple, n);
}

    ціле
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    якщо (ga_grow_id(&tuple->tv_items, 1, aid_tuple_append) == FAIL)
	повернути FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    повернути OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    ціле
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    якщо (t1 == NULL)
	tuple = tuple_alloc();
    інакше
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    якщо (tuple == NULL)
	повернути FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    якщо (t1 == NULL)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    для (ціле i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	якщо (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    повернути FAIL;
	}
    }

    повернути OK;
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
    якщо (new_tuple == NULL)
	повернути NULL;

    для (ціле i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	якщо (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    повернути NULL;
	}
    }

    повернути new_tuple;
}

    ціле
tuple_slice_or_index(
    tuple_T	*tuple,
    ціле		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    ціле		exclusive,
    typval_T	*rettv,
    ціле		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    якщо (n1 < 0)
	n1 = len + n1;
    якщо (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	якщо (!range)
	{
	    якщо (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    повернути FAIL;
	}
	якщо (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	інакше
	    n1 = len;
    }
    якщо (range)
    {
	tuple_T	*new_tuple;

	якщо (n2 < 0)
	    n2 = len + n2;
	інакше якщо (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	якщо (exclusive)
	    --n2;
	якщо (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	якщо (new_tuple == NULL)
	    повернути FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    інакше
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    повернути OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, ціле deep, ціле top, ціле copyID)
{
    tuple_T	*copy;
    ціле		idx;

    якщо (orig == NULL)
	повернути NULL;

    copy = tuple_alloc_with_items(TUPLE_LEN(orig));
    якщо (copy == NULL)
	повернути NULL;

    якщо (orig->tv_type == NULL || top || deep)
	copy->tv_type = NULL;
    інакше
	copy->tv_type = alloc_type(orig->tv_type);
    якщо (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    для (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_items.ga_len++;
	якщо (deep)
	{
	    якщо (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		break;
	}
	інакше
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    якщо (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = NULL;
    }

    повернути copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    ціле
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ціле do_error)
{
    ціле		evaluate = evalarg == NULL ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = NULL;
    typval_T	tv;
    ціле		vim9script = in_vim9script();
    ціле		had_comma;

    якщо (check_typval_is_value(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	повернути FAIL;
    }

    якщо (evaluate)
    {
	tuple = tuple_alloc();
	якщо (tuple == NULL)
	    повернути FAIL;

	якщо (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    якщо (tuple_append_tv(tuple, rettv) == FAIL)
		goto failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    якщо (**arg == ')')
	// empty tuple
	goto done;

    якщо (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	goto failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    поки (**arg != ')' && **arg != NUL)
    {
	якщо (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    goto failret;
	якщо (check_typval_is_value(&tv) == FAIL)
	{
	    якщо (evaluate)
		clear_tv(&tv);
	    goto failret;
	}

	якщо (evaluate)
	{
	    якщо (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		goto failret;
	    }
	}

	якщо (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	якщо (had_comma)
	{
	    якщо (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		goto failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	якщо (**arg == ')')
	    break;

	якщо (!had_comma)
	{
	    якщо (do_error)
	    {
		якщо (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		інакше
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    goto failret;
	}
    }

    якщо (**arg != ')')
    {
	якщо (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	якщо (evaluate)
	    tuple_free(tuple);
	повернути FAIL;
    }

done:
    *arg += 1;
    якщо (evaluate)
	rettv_tuple_set(rettv, tuple);

    повернути OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    void
tuple_lock(tuple_T *tuple, ціле deep, ціле lock, ціле check_refcount)
{
    якщо (tuple == NULL || (check_refcount && tuple->tv_refcount > 1))
	повернути;

    якщо (lock)
	tuple->tv_lock |= VAR_LOCKED;
    інакше
	tuple->tv_lock &= ~VAR_LOCKED;

    якщо (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	для (ціле i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

typedef struct join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    static ціле
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    ціле		echo_style,
    ціле		restore_copyID,
    ціле		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    ціле		i;
    join_T	*p;
    ціле		sumlen = 0;
    ціле		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    для (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	якщо (s.string == NULL)
	    повернути FAIL;

	s.length = STRLEN(s.string);
	sumlen += (ціле)s.length;

	(void)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	якщо (tofree != NULL || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	інакше
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	line_breakcheck();
	якщо (did_echo_string_emsg)  // recursion error, bail out
	    break;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    якщо (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	якщо (join_gap->ga_len >= 2)
	    sumlen += (ціле)seplen * (join_gap->ga_len - 1);
    }
    якщо (ga_grow(gap, sumlen + 2) == FAIL)
	повернути FAIL;

    для (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	якщо (first)
	    first = FALSE;
	інакше
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	якщо (p->s.string != NULL)
	    ga_concat_len(gap, p->s.string, p->s.length);
	line_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    якщо (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    повернути OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    ціле
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    ціле		echo_style,
    ціле		restore_copyID,
    ціле		copyID)
{
    garray_T	join_ga;
    ціле		retval;
    join_T	*p;
    ціле		i;

    якщо (TUPLE_LEN(tuple) < 1)
	повернути OK; // nothing to do
    ga_init2(&join_ga, sizeof(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    якщо (join_ga.ga_data == NULL)
	повернути retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    для (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    повернути retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, ціле copyID, ціле restore_copyID)
{
    garray_T	ga;

    якщо (tv->vval.v_tuple == NULL)
	повернути NULL;
    ga_init2(&ga, sizeof(char), 80);
    ga_append(&ga, '(');
    якщо (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	повернути NULL;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    повернути (char_u *)ga.ga_data;
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
    ціле		len = tuple_len(tuple);
    ціле		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    для (ціле idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	якщо (filter_map_one(TUPLE_ITEM(tuple, idx), expr, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    break;
    }

    якщо (fc != NULL)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_count(tuple_T *tuple, typval_T *needle, long idx, ціле ic)
{
    long	n = 0;

    якщо (tuple == NULL)
	повернути 0;

    ціле	len = TUPLE_LEN(tuple);
    якщо (len == 0)
	повернути 0;

    якщо (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	повернути 0;
    }

    для (ціле i = idx; i < len; i++)
    {
	якщо (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    повернути n;
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

    якщо (rettv_list_alloc(rettv) == FAIL)
	повернути;

    якщо (tuple == NULL)
	повернути;  // null tuple behaves like an empty list

    для (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	якщо (l == NULL)
	    break;

	якщо (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    break;
	}
	якщо (list_append_number(l, idx) == FAIL
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
    ціле
index_tuple(tuple_T *tuple, typval_T *tv, ціле start_idx, ціле ic)
{
    якщо (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	якщо (start_idx < 0)
	    start_idx = 0;
    }

    для (ціле idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	якщо (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    повернути idx;
    }

    повернути -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ціле
indexof_tuple(tuple_T *tuple, long startidx, typval_T *expr)
{
    long	idx = 0;
    ціле		len;
    ціле		found;

    якщо (tuple == NULL)
	повернути -1;

    len = TUPLE_LEN(tuple);

    якщо (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	якщо (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_KEY, VAR_NUMBER);

    ціле		called_emsg_start = called_emsg;

    для (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_expr(expr);
	clear_tv(get_vim_var_tv(VV_VAL));

	якщо (found)
	    повернути idx;

	якщо (called_emsg != called_emsg_start)
	    повернути -1;
    }

    повернути -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, ціле domax)
{
    typval_T	*tv = NULL;
    ціле		res;

    якщо (tuple == NULL || TUPLE_LEN(tuple) == 0)
	повернути NULL;

    для (ціле idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	якщо (tv == NULL)
	    tv = TUPLE_ITEM(tuple, idx);
	інакше
	{
	    якщо (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? EXPR_GREATER : EXPR_SMALLER, FALSE, &res) == FAIL)
		повернути NULL;
	    якщо (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    повернути tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    void
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = NULL;

    якщо (tuple == NULL || TUPLE_LEN(tuple) == 0 || n <= 0)
	повернути;

    якщо (check_repeat_count(TUPLE_LEN(tuple), n) == FAIL)
	повернути;

    якщо (rettv_tuple_set_with_items(rettv, TUPLE_LEN(tuple) * (ціле)n) == FAIL)
	повернути;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    для (ціле count = 0; count < (ціле)n; count++)
    {
	для (ціле idx = 0; idx < TUPLE_LEN(tuple); idx++)
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

    ціле	len = tuple_len(tuple);

    якщо (len == 0)
	повернути;

    якщо (rettv_tuple_set_with_items(rettv, len) == FAIL)
	повернути;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    для (ціле i = 0; i < len; i++)
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
    ціле		called_emsg_start = called_emsg;
    typval_T	initial;
    ціле		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    ціле		r;

    якщо (argvars[2].v_type == VAR_UNKNOWN)
    {
	якщо (tuple == NULL || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Tuple");
	    повернути;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    інакше
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    якщо (tuple == NULL)
	повернути;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    для ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	якщо (r == FAIL || called_emsg != called_emsg_start)
	    break;
    }

    якщо (fc != NULL)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    ціле
check_tuples_addable(type_T *type1, type_T *type2)
{
    ціле	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    якщо ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    якщо ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	якщо (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	інакше якщо ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    якщо (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	повернути FAIL;
    }

    повернути OK;
}

#endif // defined(FEAT_EVAL)
