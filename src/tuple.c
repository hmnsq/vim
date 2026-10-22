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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_EVAL)

// Tuple heads for garbage collection.
স্থির tuple_T		*first_tuple = রিক্ত;	// list of all tuples

    স্থির শূন্য
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    যদি (first_tuple != রিক্ত)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = রিক্ত;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_বস্তুসমূহ, পরিমাণ(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(শূন্য)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    যদি (tuple != রিক্ত)
	tuple_init(tuple);
    ঘূৰাই tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_বস্তুসমূহ(পূর্ণাঙ্ক গুনতি)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    যদি (tuple == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (গুনতি <= 0)
	ঘূৰাই tuple;

    যদি (ga_grow(&tuple->tv_বস্তুসমূহ, গুনতি) == FAIL)
    {
	tuple_free(tuple);
	ঘূৰাই রিক্ত;
    }

    ঘূৰাই tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    শূন্য
tuple_set_item(tuple_T *tuple, পূর্ণাঙ্ক idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_বস্তুসমূহ.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    যদি (tuple == রিক্ত)
	ঘূৰাই FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    ঘূৰাই OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    শূন্য
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    যদি (tuple != রিক্ত)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    পূর্ণাঙ্ক
rettv_tuple_set_with_বস্তুসমূহ(typval_T *rettv, পূর্ণাঙ্ক গুনতি)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_বস্তুসমূহ(গুনতি);
    যদি (new_tuple == রিক্ত)
	ঘূৰাই FAIL;

    rettv_tuple_set(rettv, new_tuple);

    ঘূৰাই OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    শূন্য
tuple_unref(tuple_T *tuple)
{
    যদি (tuple != রিক্ত && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    স্থির শূন্য
tuple_free_contents(tuple_T *tuple)
{
    বাবে (পূর্ণাঙ্ক i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_বস্তুসমূহ);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    পূর্ণাঙ্ক
tuple_free_nonref(পূর্ণাঙ্ক copyID)
{
    tuple_T	*tt;
    পূর্ণাঙ্ক		did_free = FALSE;

    বাবে (tt = first_tuple; tt != রিক্ত; tt = tt->tv_used_next)
	যদি ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ঘূৰাই did_free;
}

    স্থির শূন্য
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    যদি (tuple->tv_used_prev == রিক্ত)
	first_tuple = tuple->tv_used_next;
    অন্যথা
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    যদি (tuple->tv_used_next != রিক্ত)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    শূন্য
tuple_free_বস্তুসমূহ(পূর্ণাঙ্ক copyID)
{
    tuple_T	*tt, *tt_next;

    বাবে (tt = first_tuple; tt != রিক্ত; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	যদি ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    শূন্য
tuple_free(tuple_T *tuple)
{
    যদি (in_free_unref_বস্তুসমূহ)
	ঘূৰাই;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    যদি (tuple == রিক্ত)
	ঘূৰাই 0L;
    ঘূৰাই tuple->tv_বস্তুসমূহ.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    পূর্ণাঙ্ক
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    পূর্ণাঙ্ক		ic)	// ignore case for strings
{
    যদি (t1 == t2)
	ঘূৰাই TRUE;

    পূর্ণাঙ্ক t1_len = tuple_len(t1);
    পূর্ণাঙ্ক t2_len = tuple_len(t2);

    যদি (t1_len != t2_len)
	ঘূৰাই FALSE;

    যদি (t1_len == 0)
	// empty and NULL tuples are considered equal
	ঘূৰাই TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    বাবে (পূর্ণাঙ্ক i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	যদি (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    ঘূৰাই FALSE;

    ঘূৰাই TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    যদি (tuple == রিক্ত)
	ঘূৰাই রিক্ত;

    // Negative index is relative to the end.
    যদি (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    যদি (n < 0 || n >= TUPLE_LEN(tuple))
	ঘূৰাই রিক্ত;

    ঘূৰাই TUPLE_ITEM(tuple, n);
}

    পূর্ণাঙ্ক
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    যদি (ga_grow_id(&tuple->tv_বস্তুসমূহ, 1, aid_tuple_append) == FAIL)
	ঘূৰাই FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    ঘূৰাই OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    পূর্ণাঙ্ক
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    যদি (t1 == রিক্ত)
	tuple = tuple_alloc();
    অন্যথা
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    যদি (tuple == রিক্ত)
	ঘূৰাই FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    যদি (t1 == রিক্ত)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    বাবে (পূর্ণাঙ্ক i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	যদি (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ঘূৰাই FAIL;
	}
    }

    ঘূৰাই OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_বস্তুসমূহ(n2 - n1 + 1);
    যদি (new_tuple == রিক্ত)
	ঘূৰাই রিক্ত;

    বাবে (পূর্ণাঙ্ক i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	যদি (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ঘূৰাই রিক্ত;
	}
    }

    ঘূৰাই new_tuple;
}

    পূর্ণাঙ্ক
tuple_slice_or_index(
    tuple_T	*tuple,
    পূর্ণাঙ্ক		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    পূর্ণাঙ্ক		exclusive,
    typval_T	*rettv,
    পূর্ণাঙ্ক		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    যদি (n1 < 0)
	n1 = len + n1;
    যদি (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	যদি (!range)
	{
	    যদি (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    ঘূৰাই FAIL;
	}
	যদি (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	অন্যথা
	    n1 = len;
    }
    যদি (range)
    {
	tuple_T	*new_tuple;

	যদি (n2 < 0)
	    n2 = len + n2;
	অন্যথা যদি (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	যদি (exclusive)
	    --n2;
	যদি (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	যদি (new_tuple == রিক্ত)
	    ঘূৰাই FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    অন্যথা
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ঘূৰাই OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, পূর্ণাঙ্ক deep, পূর্ণাঙ্ক শীৰ্ষ, পূর্ণাঙ্ক copyID)
{
    tuple_T	*copy;
    পূর্ণাঙ্ক		idx;

    যদি (orig == রিক্ত)
	ঘূৰাই রিক্ত;

    copy = tuple_alloc_with_বস্তুসমূহ(TUPLE_LEN(orig));
    যদি (copy == রিক্ত)
	ঘূৰাই রিক্ত;

    যদি (orig->tv_type == রিক্ত || শীৰ্ষ || deep)
	copy->tv_type = রিক্ত;
    অন্যথা
	copy->tv_type = alloc_type(orig->tv_type);
    যদি (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    বাবে (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_বস্তুসমূহ.ga_len++;
	যদি (deep)
	{
	    যদি (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		ভঙ্গ;
	}
	অন্যথা
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    যদি (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = রিক্ত;
    }

    ঘূৰাই copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, পূর্ণাঙ্ক do_error)
{
    পূর্ণাঙ্ক		evaluate = evalarg == রিক্ত ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = রিক্ত;
    typval_T	tv;
    পূর্ণাঙ্ক		vim9script = in_vim9script();
    পূর্ণাঙ্ক		had_comma;

    যদি (check_typval_is_মান(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ঘূৰাই FAIL;
    }

    যদি (evaluate)
    {
	tuple = tuple_alloc();
	যদি (tuple == রিক্ত)
	    ঘূৰাই FAIL;

	যদি (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    যদি (tuple_append_tv(tuple, rettv) == FAIL)
		গচ্ছ failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    যদি (**arg == ')')
	// empty tuple
	গচ্ছ done;

    যদি (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	গচ্ছ failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    যেতিয়ালৈ (**arg != ')' && **arg != NUL)
    {
	যদি (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    গচ্ছ failret;
	যদি (check_typval_is_মান(&tv) == FAIL)
	{
	    যদি (evaluate)
		clear_tv(&tv);
	    গচ্ছ failret;
	}

	যদি (evaluate)
	{
	    যদি (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		গচ্ছ failret;
	    }
	}

	যদি (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	যদি (had_comma)
	{
	    যদি (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		গচ্ছ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	যদি (**arg == ')')
	    ভঙ্গ;

	যদি (!had_comma)
	{
	    যদি (do_error)
	    {
		যদি (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		অন্যথা
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    গচ্ছ failret;
	}
    }

    যদি (**arg != ')')
    {
	যদি (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	যদি (evaluate)
	    tuple_free(tuple);
	ঘূৰাই FAIL;
    }

done:
    *arg += 1;
    যদি (evaluate)
	rettv_tuple_set(rettv, tuple);

    ঘূৰাই OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    শূন্য
tuple_lock(tuple_T *tuple, পূর্ণাঙ্ক deep, পূর্ণাঙ্ক lock, পূর্ণাঙ্ক check_refcount)
{
    যদি (tuple == রিক্ত || (check_refcount && tuple->tv_refcount > 1))
	ঘূৰাই;

    যদি (lock)
	tuple->tv_lock |= VAR_LOCKED;
    অন্যথা
	tuple->tv_lock &= ~VAR_LOCKED;

    যদি (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	বাবে (পূর্ণাঙ্ক i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

প্রকারনির্দেশ সংরচনা join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    স্থির পূর্ণাঙ্ক
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    পূর্ণাঙ্ক		echo_style,
    পূর্ণাঙ্ক		restore_copyID,
    পূর্ণাঙ্ক		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    পূর্ণাঙ্ক		i;
    join_T	*p;
    পূর্ণাঙ্ক		sumlen = 0;
    পূর্ণাঙ্ক		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    বাবে (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	যদি (s.string == রিক্ত)
	    ঘূৰাই FAIL;

	s.length = STRLEN(s.string);
	sumlen += (পূর্ণাঙ্ক)s.length;

	(শূন্য)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	যদি (tofree != রিক্ত || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	অন্যথা
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	সাৰি_breakcheck();
	যদি (did_echo_string_emsg)  // recursion error, bail out
	    ভঙ্গ;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    যদি (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	যদি (join_gap->ga_len >= 2)
	    sumlen += (পূর্ণাঙ্ক)seplen * (join_gap->ga_len - 1);
    }
    যদি (ga_grow(gap, sumlen + 2) == FAIL)
	ঘূৰাই FAIL;

    বাবে (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	যদি (first)
	    first = FALSE;
	অন্যথা
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	যদি (p->s.string != রিক্ত)
	    ga_concat_len(gap, p->s.string, p->s.length);
	সাৰি_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    যদি (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ঘূৰাই OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    পূর্ণাঙ্ক
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    পূর্ণাঙ্ক		echo_style,
    পূর্ণাঙ্ক		restore_copyID,
    পূর্ণাঙ্ক		copyID)
{
    garray_T	join_ga;
    পূর্ণাঙ্ক		retval;
    join_T	*p;
    পূর্ণাঙ্ক		i;

    যদি (TUPLE_LEN(tuple) < 1)
	ঘূৰাই OK; // nothing to do
    ga_init2(&join_ga, পরিমাণ(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    যদি (join_ga.ga_data == রিক্ত)
	ঘূৰাই retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    বাবে (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ঘূৰাই retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, পূর্ণাঙ্ক copyID, পূর্ণাঙ্ক restore_copyID)
{
    garray_T	ga;

    যদি (tv->vval.v_tuple == রিক্ত)
	ঘূৰাই রিক্ত;
    ga_init2(&ga, পরিমাণ(ৱর্ণ), 80);
    ga_append(&ga, '(');
    যদি (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ঘূৰাই রিক্ত;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ঘূৰাই (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    শূন্য
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*ৰাশিমালা)
{
    পূর্ণাঙ্ক		len = tuple_len(tuple);
    পূর্ণাঙ্ক		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_চাবি, VAR_সংখ্যা);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ৰাশিমালা_get_funccal(ৰাশিমালা, &newtv);

    বাবে (পূর্ণাঙ্ক idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_চাবি, idx);
	যদি (filter_map_one(TUPLE_ITEM(tuple, idx), ৰাশিমালা, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    ভঙ্গ;
    }

    যদি (fc != রিক্ত)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_গুনতি(tuple_T *tuple, typval_T *needle, long idx, পূর্ণাঙ্ক ic)
{
    long	n = 0;

    যদি (tuple == রিক্ত)
	ঘূৰাই 0;

    পূর্ণাঙ্ক	len = TUPLE_LEN(tuple);
    যদি (len == 0)
	ঘূৰাই 0;

    যদি (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	ঘূৰাই 0;
    }

    বাবে (পূর্ণাঙ্ক i = idx; i < len; i++)
    {
	যদি (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    ঘূৰাই n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    শূন্য
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    যদি (rettv_list_alloc(rettv) == FAIL)
	ঘূৰাই;

    যদি (tuple == রিক্ত)
	ঘূৰাই;  // null tuple behaves like an empty list

    বাবে (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	যদি (l == রিক্ত)
	    ভঙ্গ;

	যদি (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ভঙ্গ;
	}
	যদি (list_append_সংখ্যা(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    ভঙ্গ;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    পূর্ণাঙ্ক
index_tuple(tuple_T *tuple, typval_T *tv, পূর্ণাঙ্ক start_idx, পূর্ণাঙ্ক ic)
{
    যদি (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	যদি (start_idx < 0)
	    start_idx = 0;
    }

    বাবে (পূর্ণাঙ্ক idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	যদি (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    ঘূৰাই idx;
    }

    ঘূৰাই -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    পূর্ণাঙ্ক
indexof_tuple(tuple_T *tuple, long startidx, typval_T *ৰাশিমালা)
{
    long	idx = 0;
    পূর্ণাঙ্ক		len;
    পূর্ণাঙ্ক		found;

    যদি (tuple == রিক্ত)
	ঘূৰাই -1;

    len = TUPLE_LEN(tuple);

    যদি (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	যদি (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_চাবি, VAR_সংখ্যা);

    পূর্ণাঙ্ক		called_emsg_start = called_emsg;

    বাবে (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_চাবি, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_ৰাশিমালা(ৰাশিমালা);
	clear_tv(get_vim_var_tv(VV_VAL));

	যদি (found)
	    ঘূৰাই idx;

	যদি (called_emsg != called_emsg_start)
	    ঘূৰাই -1;
    }

    ঘূৰাই -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, পূর্ণাঙ্ক domax)
{
    typval_T	*tv = রিক্ত;
    পূর্ণাঙ্ক		res;

    যদি (tuple == রিক্ত || TUPLE_LEN(tuple) == 0)
	ঘূৰাই রিক্ত;

    বাবে (পূর্ণাঙ্ক idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	যদি (tv == রিক্ত)
	    tv = TUPLE_ITEM(tuple, idx);
	অন্যথা
	{
	    যদি (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? ৰাশিমালা_GREATER : ৰাশিমালা_SMALLER, FALSE, &res) == FAIL)
		ঘূৰাই রিক্ত;
	    যদি (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    ঘূৰাই tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    শূন্য
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = রিক্ত;

    যদি (tuple == রিক্ত || TUPLE_LEN(tuple) == 0 || n <= 0)
	ঘূৰাই;

    যদি (check_repeat_গুনতি(TUPLE_LEN(tuple), n) == FAIL)
	ঘূৰাই;

    যদি (rettv_tuple_set_with_বস্তুসমূহ(rettv, TUPLE_LEN(tuple) * (পূর্ণাঙ্ক)n) == FAIL)
	ঘূৰাই;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    বাবে (পূর্ণাঙ্ক গুনতি = 0; গুনতি < (পূর্ণাঙ্ক)n; গুনতি++)
    {
	বাবে (পূর্ণাঙ্ক idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_বস্তুসমূহ.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    শূন্য
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = রিক্ত;

    পূর্ণাঙ্ক	len = tuple_len(tuple);

    যদি (len == 0)
	ঘূৰাই;

    যদি (rettv_tuple_set_with_বস্তুসমূহ(rettv, len) == FAIL)
	ঘূৰাই;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    বাবে (পূর্ণাঙ্ক i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_বস্তুসমূহ.ga_len = tuple->tv_বস্তুসমূহ.ga_len;
}

/*
 * Tuple reduce() function
 */
    শূন্য
tuple_reduce(typval_T *argvars, typval_T *ৰাশিমালা, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    পূর্ণাঙ্ক		called_emsg_start = called_emsg;
    typval_T	initial;
    পূর্ণাঙ্ক		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    পূর্ণাঙ্ক		r;

    যদি (argvars[2].v_type == VAR_UNKNOWN)
    {
	যদি (tuple == রিক্ত || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_মান), "Tuple");
	    ঘূৰাই;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    অন্যথা
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    যদি (tuple == রিক্ত)
	ঘূৰাই;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ৰাশিমালা_get_funccal(ৰাশিমালা, rettv);

    বাবে ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_ৰাশিমালা_typval(ৰাশিমালা, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	যদি (r == FAIL || called_emsg != called_emsg_start)
	    ভঙ্গ;
    }

    যদি (fc != রিক্ত)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    পূর্ণাঙ্ক
check_tuples_addable(type_T *type1, type_T *type2)
{
    পূর্ণাঙ্ক	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    যদি ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    যদি ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	যদি (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	অন্যথা যদি ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    যদি (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
}

#endif // defined(FEAT_EVAL)
