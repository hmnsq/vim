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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_EVAL)

// Tuple heads for garbage collection.
ಸ್ಥಿರ tuple_T		*first_tuple = ರಿಕ್ತ;	// list of all tuples

    ಸ್ಥಿರ ಶೂನ್ಯ
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    ಯದಿ (first_tuple != ರಿಕ್ತ)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = ರಿಕ್ತ;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_ವಸ್ತುಗಳು, ಪರಿಮಾಣ(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(ಶೂನ್ಯ)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    ಯದಿ (tuple != ರಿಕ್ತ)
	tuple_init(tuple);
    ಪ್ರತ್ಯಾಗಮ tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (ಎಣಿಕೆ <= 0)
	ಪ್ರತ್ಯಾಗಮ tuple;

    ಯದಿ (ga_grow(&tuple->tv_ವಸ್ತುಗಳು, ಎಣಿಕೆ) == FAIL)
    {
	tuple_free(tuple);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    ಶೂನ್ಯ
tuple_set_item(tuple_T *tuple, ಪೂರ್ಣಾಙ್ಕ idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_ವಸ್ತುಗಳು.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    ಶೂನ್ಯ
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    ಯದಿ (tuple != ರಿಕ್ತ)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
rettv_tuple_set_with_ವಸ್ತುಗಳು(typval_T *rettv, ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_ವಸ್ತುಗಳು(ಎಣಿಕೆ);
    ಯದಿ (new_tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    rettv_tuple_set(rettv, new_tuple);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    ಶೂನ್ಯ
tuple_unref(tuple_T *tuple)
{
    ಯದಿ (tuple != ರಿಕ್ತ && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tuple_free_contents(tuple_T *tuple)
{
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_ವಸ್ತುಗಳು);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ಪೂರ್ಣಾಙ್ಕ
tuple_free_nonref(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    tuple_T	*tt;
    ಪೂರ್ಣಾಙ್ಕ		did_free = FALSE;

    ಪ್ರತಿ (tt = first_tuple; tt != ರಿಕ್ತ; tt = tt->tv_used_next)
	ಯದಿ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ಪ್ರತ್ಯಾಗಮ did_free;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    ಯದಿ (tuple->tv_used_prev == ರಿಕ್ತ)
	first_tuple = tuple->tv_used_next;
    ಅನ್ಯಥಾ
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    ಯದಿ (tuple->tv_used_next != ರಿಕ್ತ)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    ಶೂನ್ಯ
tuple_free_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಙ್ಕ copyID)
{
    tuple_T	*tt, *tt_next;

    ಪ್ರತಿ (tt = first_tuple; tt != ರಿಕ್ತ; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	ಯದಿ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    ಶೂನ್ಯ
tuple_free(tuple_T *tuple)
{
    ಯದಿ (in_free_unref_ವಸ್ತುಗಳು)
	ಪ್ರತ್ಯಾಗಮ;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0L;
    ಪ್ರತ್ಯಾಗಮ tuple->tv_ವಸ್ತುಗಳು.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    ಪೂರ್ಣಾಙ್ಕ
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    ಪೂರ್ಣಾಙ್ಕ		ic)	// ignore case for strings
{
    ಯದಿ (t1 == t2)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಪೂರ್ಣಾಙ್ಕ t1_len = tuple_len(t1);
    ಪೂರ್ಣಾಙ್ಕ t2_len = tuple_len(t2);

    ಯದಿ (t1_len != t2_len)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (t1_len == 0)
	// empty and NULL tuples are considered equal
	ಪ್ರತ್ಯಾಗಮ TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	ಯದಿ (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    ಪ್ರತ್ಯಾಗಮ FALSE;

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    // Negative index is relative to the end.
    ಯದಿ (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    ಯದಿ (n < 0 || n >= TUPLE_LEN(tuple))
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತ್ಯಾಗಮ TUPLE_ITEM(tuple, n);
}

    ಪೂರ್ಣಾಙ್ಕ
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    ಯದಿ (ga_grow_id(&tuple->tv_ವಸ್ತುಗಳು, 1, aid_tuple_append) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಙ್ಕ
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    ಯದಿ (t1 == ರಿಕ್ತ)
	tuple = tuple_alloc();
    ಅನ್ಯಥಾ
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    ಯದಿ (t1 == ರಿಕ್ತ)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	ಯದಿ (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_ವಸ್ತುಗಳು(n2 - n1 + 1);
    ಯದಿ (new_tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	ಯದಿ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }

    ಪ್ರತ್ಯಾಗಮ new_tuple;
}

    ಪೂರ್ಣಾಙ್ಕ
tuple_slice_or_index(
    tuple_T	*tuple,
    ಪೂರ್ಣಾಙ್ಕ		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    ಪೂರ್ಣಾಙ್ಕ		exclusive,
    typval_T	*rettv,
    ಪೂರ್ಣಾಙ್ಕ		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ಯದಿ (n1 < 0)
	n1 = len + n1;
    ಯದಿ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	ಯದಿ (!range)
	{
	    ಯದಿ (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಯದಿ (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ಅನ್ಯಥಾ
	    n1 = len;
    }
    ಯದಿ (range)
    {
	tuple_T	*new_tuple;

	ಯದಿ (n2 < 0)
	    n2 = len + n2;
	ಅನ್ಯಥಾ ಯದಿ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ಯದಿ (exclusive)
	    --n2;
	ಯದಿ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	ಯದಿ (new_tuple == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    ಅನ್ಯಥಾ
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, ಪೂರ್ಣಾಙ್ಕ deep, ಪೂರ್ಣಾಙ್ಕ ತುದಿ, ಪೂರ್ಣಾಙ್ಕ copyID)
{
    tuple_T	*copy;
    ಪೂರ್ಣಾಙ್ಕ		idx;

    ಯದಿ (orig == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    copy = tuple_alloc_with_ವಸ್ತುಗಳು(TUPLE_LEN(orig));
    ಯದಿ (copy == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (orig->tv_type == ರಿಕ್ತ || ತುದಿ || deep)
	copy->tv_type = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	copy->tv_type = alloc_type(orig->tv_type);
    ಯದಿ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    ಪ್ರತಿ (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_ವಸ್ತುಗಳು.ga_len++;
	ಯದಿ (deep)
	{
	    ಯದಿ (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    ಯದಿ (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = ರಿಕ್ತ;
    }

    ಪ್ರತ್ಯಾಗಮ copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಙ್ಕ do_error)
{
    ಪೂರ್ಣಾಙ್ಕ		evaluate = evalarg == ರಿಕ್ತ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = ರಿಕ್ತ;
    typval_T	tv;
    ಪೂರ್ಣಾಙ್ಕ		vim9script = in_vim9script();
    ಪೂರ್ಣಾಙ್ಕ		had_comma;

    ಯದಿ (check_typval_is_ಮೌಲ್ಯ(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (evaluate)
    {
	tuple = tuple_alloc();
	ಯದಿ (tuple == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	ಯದಿ (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    ಯದಿ (tuple_append_tv(tuple, rettv) == FAIL)
		ಗಚ್ಛ failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    ಯದಿ (**arg == ')')
	// empty tuple
	ಗಚ್ಛ done;

    ಯದಿ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	ಗಚ್ಛ failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ಯಾವತ್ (**arg != ')' && **arg != NUL)
    {
	ಯದಿ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    ಗಚ್ಛ failret;
	ಯದಿ (check_typval_is_ಮೌಲ್ಯ(&tv) == FAIL)
	{
	    ಯದಿ (evaluate)
		clear_tv(&tv);
	    ಗಚ್ಛ failret;
	}

	ಯದಿ (evaluate)
	{
	    ಯದಿ (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		ಗಚ್ಛ failret;
	    }
	}

	ಯದಿ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ಯದಿ (had_comma)
	{
	    ಯದಿ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ಗಚ್ಛ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಯದಿ (**arg == ')')
	    ಭಙ್ಗ;

	ಯದಿ (!had_comma)
	{
	    ಯದಿ (do_error)
	    {
		ಯದಿ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ಅನ್ಯಥಾ
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    ಗಚ್ಛ failret;
	}
    }

    ಯದಿ (**arg != ')')
    {
	ಯದಿ (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	ಯದಿ (evaluate)
	    tuple_free(tuple);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

done:
    *arg += 1;
    ಯದಿ (evaluate)
	rettv_tuple_set(rettv, tuple);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    ಶೂನ್ಯ
tuple_lock(tuple_T *tuple, ಪೂರ್ಣಾಙ್ಕ deep, ಪೂರ್ಣಾಙ್ಕ lock, ಪೂರ್ಣಾಙ್ಕ check_refcount)
{
    ಯದಿ (tuple == ರಿಕ್ತ || (check_refcount && tuple->tv_refcount > 1))
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (lock)
	tuple->tv_lock |= VAR_LOCKED;
    ಅನ್ಯಥಾ
	tuple->tv_lock &= ~VAR_LOCKED;

    ಯದಿ (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    ಪೂರ್ಣಾಙ್ಕ		echo_style,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID,
    ಪೂರ್ಣಾಙ್ಕ		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    join_T	*p;
    ಪೂರ್ಣಾಙ್ಕ		sumlen = 0;
    ಪೂರ್ಣಾಙ್ಕ		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    ಪ್ರತಿ (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ಯದಿ (s.string == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ FAIL;

	s.length = STRLEN(s.string);
	sumlen += (ಪೂರ್ಣಾಙ್ಕ)s.length;

	(ಶೂನ್ಯ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	ಯದಿ (tofree != ರಿಕ್ತ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	ಅನ್ಯಥಾ
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	ಸಾಲು_breakcheck();
	ಯದಿ (did_echo_string_emsg)  // recursion error, bail out
	    ಭಙ್ಗ;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    ಯದಿ (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	ಯದಿ (join_gap->ga_len >= 2)
	    sumlen += (ಪೂರ್ಣಾಙ್ಕ)seplen * (join_gap->ga_len - 1);
    }
    ಯದಿ (ga_grow(gap, sumlen + 2) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪ್ರತಿ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ಯದಿ (first)
	    first = FALSE;
	ಅನ್ಯಥಾ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	ಯದಿ (p->s.string != ರಿಕ್ತ)
	    ga_concat_len(gap, p->s.string, p->s.length);
	ಸಾಲು_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    ಯದಿ (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಙ್ಕ
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    ಪೂರ್ಣಾಙ್ಕ		echo_style,
    ಪೂರ್ಣಾಙ್ಕ		restore_copyID,
    ಪೂರ್ಣಾಙ್ಕ		copyID)
{
    garray_T	join_ga;
    ಪೂರ್ಣಾಙ್ಕ		retval;
    join_T	*p;
    ಪೂರ್ಣಾಙ್ಕ		i;

    ಯದಿ (TUPLE_LEN(tuple) < 1)
	ಪ್ರತ್ಯಾಗಮ OK; // nothing to do
    ga_init2(&join_ga, ಪರಿಮಾಣ(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ಯದಿ (join_ga.ga_data == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    ಪ್ರತಿ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, ಪೂರ್ಣಾಙ್ಕ copyID, ಪೂರ್ಣಾಙ್ಕ restore_copyID)
{
    garray_T	ga;

    ಯದಿ (tv->vval.v_tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ga_init2(&ga, ಪರಿಮಾಣ(ವರ್ಣ), 80);
    ga_append(&ga, '(');
    ಯದಿ (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ಪ್ರತ್ಯಾಗಮ (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    ಶೂನ್ಯ
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ)
{
    ಪೂರ್ಣಾಙ್ಕ		len = tuple_len(tuple);
    ಪೂರ್ಣಾಙ್ಕ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_ಕೀಲಿ, idx);
	ಯದಿ (filter_map_one(TUPLE_ITEM(tuple, idx), ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    ಭಙ್ಗ;
    }

    ಯದಿ (fc != ರಿಕ್ತ)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_ಎಣಿಕೆ(tuple_T *tuple, typval_T *needle, long idx, ಪೂರ್ಣಾಙ್ಕ ic)
{
    long	n = 0;

    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಪೂರ್ಣಾಙ್ಕ	len = TUPLE_LEN(tuple);
    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ 0;

    ಯದಿ (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	ಪ್ರತ್ಯಾಗಮ 0;
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = idx; i < len; i++)
    {
	ಯದಿ (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    ಪ್ರತ್ಯಾಗಮ n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    ಶೂನ್ಯ
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    ಯದಿ (rettv_list_alloc(rettv) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;  // null tuple behaves like an empty list

    ಪ್ರತಿ (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	ಯದಿ (l == ರಿಕ್ತ)
	    ಭಙ್ಗ;

	ಯದಿ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ಭಙ್ಗ;
	}
	ಯದಿ (list_append_ಸಂಖ್ಯೆ(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    ಭಙ್ಗ;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    ಪೂರ್ಣಾಙ್ಕ
index_tuple(tuple_T *tuple, typval_T *tv, ಪೂರ್ಣಾಙ್ಕ start_idx, ಪೂರ್ಣಾಙ್ಕ ic)
{
    ಯದಿ (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	ಯದಿ (start_idx < 0)
	    start_idx = 0;
    }

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	ಯದಿ (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    ಪ್ರತ್ಯಾಗಮ idx;
    }

    ಪ್ರತ್ಯಾಗಮ -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ಪೂರ್ಣಾಙ್ಕ
indexof_tuple(tuple_T *tuple, long startidx, typval_T *ಅಭಿವ್ಯಕ್ತಿ)
{
    long	idx = 0;
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		found;

    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    len = TUPLE_LEN(tuple);

    ಯದಿ (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	ಯದಿ (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    ಪೂರ್ಣಾಙ್ಕ		called_emsg_start = called_emsg;

    ಪ್ರತಿ (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_ಕೀಲಿ, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ);
	clear_tv(get_vim_var_tv(VV_VAL));

	ಯದಿ (found)
	    ಪ್ರತ್ಯಾಗಮ idx;

	ಯದಿ (called_emsg != called_emsg_start)
	    ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಪ್ರತ್ಯಾಗಮ -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, ಪೂರ್ಣಾಙ್ಕ domax)
{
    typval_T	*tv = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		res;

    ಯದಿ (tuple == ರಿಕ್ತ || TUPLE_LEN(tuple) == 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	ಯದಿ (tv == ರಿಕ್ತ)
	    tv = TUPLE_ITEM(tuple, idx);
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? ಅಭಿವ್ಯಕ್ತಿ_GREATER : ಅಭಿವ್ಯಕ್ತಿ_SMALLER, FALSE, &res) == FAIL)
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    ಯದಿ (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    ಪ್ರತ್ಯಾಗಮ tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    ಶೂನ್ಯ
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ರಿಕ್ತ;

    ಯದಿ (tuple == ರಿಕ್ತ || TUPLE_LEN(tuple) == 0 || n <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (check_repeat_ಎಣಿಕೆ(TUPLE_LEN(tuple), n) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_tuple_set_with_ವಸ್ತುಗಳು(rettv, TUPLE_LEN(tuple) * (ಪೂರ್ಣಾಙ್ಕ)n) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ ಎಣಿಕೆ = 0; ಎಣಿಕೆ < (ಪೂರ್ಣಾಙ್ಕ)n; ಎಣಿಕೆ++)
    {
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_ವಸ್ತುಗಳು.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    ಶೂನ್ಯ
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ರಿಕ್ತ;

    ಪೂರ್ಣಾಙ್ಕ	len = tuple_len(tuple);

    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (rettv_tuple_set_with_ವಸ್ತುಗಳು(rettv, len) == FAIL)
	ಪ್ರತ್ಯಾಗಮ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_ವಸ್ತುಗಳು.ga_len = tuple->tv_ವಸ್ತುಗಳು.ga_len;
}

/*
 * Tuple reduce() function
 */
    ಶೂನ್ಯ
tuple_reduce(typval_T *argvars, typval_T *ಅಭಿವ್ಯಕ್ತಿ, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    ಪೂರ್ಣಾಙ್ಕ		called_emsg_start = called_emsg;
    typval_T	initial;
    ಪೂರ್ಣಾಙ್ಕ		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    ಪೂರ್ಣಾಙ್ಕ		r;

    ಯದಿ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ಯದಿ (tuple == ರಿಕ್ತ || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_ಮೌಲ್ಯ), "Tuple");
	    ಪ್ರತ್ಯಾಗಮ;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    ಅನ್ಯಥಾ
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    ಯದಿ (tuple == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, rettv);

    ಪ್ರತಿ ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	ಯದಿ (r == FAIL || called_emsg != called_emsg_start)
	    ಭಙ್ಗ;
    }

    ಯದಿ (fc != ರಿಕ್ತ)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    ಪೂರ್ಣಾಙ್ಕ
check_tuples_addable(type_T *type1, type_T *type2)
{
    ಪೂರ್ಣಾಙ್ಕ	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    ಯದಿ ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    ಯದಿ ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	ಯದಿ (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	ಅನ್ಯಥಾ ಯದಿ ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    ಯದಿ (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

#endif // defined(FEAT_EVAL)
