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

#अन्तर्भाव "vim.h"

#सचे defined(FEAT_EVAL)

// Tuple heads for garbage collection.
स्थिर tuple_T		*first_tuple = रिक्त;	// list of all tuples

    स्थिर शून्य
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    सचे (first_tuple != रिक्त)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = रिक्त;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_वस्तूनि, परिमाण(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(शून्य)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    सचे (tuple != रिक्त)
	tuple_init(tuple);
    पटिनिवत्त tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_वस्तूनि(पूर्णाङ्क गणनम्)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    सचे (tuple == रिक्त)
	पटिनिवत्त रिक्त;

    सचे (गणनम् <= 0)
	पटिनिवत्त tuple;

    सचे (ga_grow(&tuple->tv_वस्तूनि, गणनम्) == FAIL)
    {
	tuple_free(tuple);
	पटिनिवत्त रिक्त;
    }

    पटिनिवत्त tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    शून्य
tuple_set_item(tuple_T *tuple, पूर्णाङ्क idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_वस्तूनि.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    सचे (tuple == रिक्त)
	पटिनिवत्त FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    पटिनिवत्त OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    शून्य
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    सचे (tuple != रिक्त)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    पूर्णाङ्क
rettv_tuple_set_with_वस्तूनि(typval_T *rettv, पूर्णाङ्क गणनम्)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_वस्तूनि(गणनम्);
    सचे (new_tuple == रिक्त)
	पटिनिवत्त FAIL;

    rettv_tuple_set(rettv, new_tuple);

    पटिनिवत्त OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    शून्य
tuple_unref(tuple_T *tuple)
{
    सचे (tuple != रिक्त && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    स्थिर शून्य
tuple_free_contents(tuple_T *tuple)
{
    अत्थाय (पूर्णाङ्क i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_वस्तूनि);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    पूर्णाङ्क
tuple_free_nonref(पूर्णाङ्क copyID)
{
    tuple_T	*tt;
    पूर्णाङ्क		did_free = FALSE;

    अत्थाय (tt = first_tuple; tt != रिक्त; tt = tt->tv_used_next)
	सचे ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    पटिनिवत्त did_free;
}

    स्थिर शून्य
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    सचे (tuple->tv_used_prev == रिक्त)
	first_tuple = tuple->tv_used_next;
    अञ्ञथा
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    सचे (tuple->tv_used_next != रिक्त)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    शून्य
tuple_free_वस्तूनि(पूर्णाङ्क copyID)
{
    tuple_T	*tt, *tt_next;

    अत्थाय (tt = first_tuple; tt != रिक्त; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	सचे ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    शून्य
tuple_free(tuple_T *tuple)
{
    सचे (in_free_unref_वस्तूनि)
	पटिनिवत्त;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    सचे (tuple == रिक्त)
	पटिनिवत्त 0L;
    पटिनिवत्त tuple->tv_वस्तूनि.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    पूर्णाङ्क
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    पूर्णाङ्क		ic)	// ignore case for strings
{
    सचे (t1 == t2)
	पटिनिवत्त TRUE;

    पूर्णाङ्क t1_len = tuple_len(t1);
    पूर्णाङ्क t2_len = tuple_len(t2);

    सचे (t1_len != t2_len)
	पटिनिवत्त FALSE;

    सचे (t1_len == 0)
	// empty and NULL tuples are considered equal
	पटिनिवत्त TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    अत्थाय (पूर्णाङ्क i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	सचे (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    पटिनिवत्त FALSE;

    पटिनिवत्त TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    सचे (tuple == रिक्त)
	पटिनिवत्त रिक्त;

    // Negative index is relative to the end.
    सचे (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    सचे (n < 0 || n >= TUPLE_LEN(tuple))
	पटिनिवत्त रिक्त;

    पटिनिवत्त TUPLE_ITEM(tuple, n);
}

    पूर्णाङ्क
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    सचे (ga_grow_id(&tuple->tv_वस्तूनि, 1, aid_tuple_append) == FAIL)
	पटिनिवत्त FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    पटिनिवत्त OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    पूर्णाङ्क
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    सचे (t1 == रिक्त)
	tuple = tuple_alloc();
    अञ्ञथा
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    सचे (tuple == रिक्त)
	पटिनिवत्त FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    सचे (t1 == रिक्त)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    अत्थाय (पूर्णाङ्क i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	सचे (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    पटिनिवत्त FAIL;
	}
    }

    पटिनिवत्त OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_वस्तूनि(n2 - n1 + 1);
    सचे (new_tuple == रिक्त)
	पटिनिवत्त रिक्त;

    अत्थाय (पूर्णाङ्क i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	सचे (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    पटिनिवत्त रिक्त;
	}
    }

    पटिनिवत्त new_tuple;
}

    पूर्णाङ्क
tuple_slice_or_index(
    tuple_T	*tuple,
    पूर्णाङ्क		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    पूर्णाङ्क		exclusive,
    typval_T	*rettv,
    पूर्णाङ्क		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    सचे (n1 < 0)
	n1 = len + n1;
    सचे (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	सचे (!range)
	{
	    सचे (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    पटिनिवत्त FAIL;
	}
	सचे (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	अञ्ञथा
	    n1 = len;
    }
    सचे (range)
    {
	tuple_T	*new_tuple;

	सचे (n2 < 0)
	    n2 = len + n2;
	अञ्ञथा सचे (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	सचे (exclusive)
	    --n2;
	सचे (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	सचे (new_tuple == रिक्त)
	    पटिनिवत्त FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    अञ्ञथा
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    पटिनिवत्त OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, पूर्णाङ्क deep, पूर्णाङ्क शिखरम्, पूर्णाङ्क copyID)
{
    tuple_T	*copy;
    पूर्णाङ्क		idx;

    सचे (orig == रिक्त)
	पटिनिवत्त रिक्त;

    copy = tuple_alloc_with_वस्तूनि(TUPLE_LEN(orig));
    सचे (copy == रिक्त)
	पटिनिवत्त रिक्त;

    सचे (orig->tv_type == रिक्त || शिखरम् || deep)
	copy->tv_type = रिक्त;
    अञ्ञथा
	copy->tv_type = alloc_type(orig->tv_type);
    सचे (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    अत्थाय (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_वस्तूनि.ga_len++;
	सचे (deep)
	{
	    सचे (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		भङ्ग;
	}
	अञ्ञथा
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    सचे (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = रिक्त;
    }

    पटिनिवत्त copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, पूर्णाङ्क do_error)
{
    पूर्णाङ्क		evaluate = evalarg == रिक्त ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = रिक्त;
    typval_T	tv;
    पूर्णाङ्क		vim9script = in_vim9script();
    पूर्णाङ्क		had_comma;

    सचे (check_typval_is_मानम्(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	पटिनिवत्त FAIL;
    }

    सचे (evaluate)
    {
	tuple = tuple_alloc();
	सचे (tuple == रिक्त)
	    पटिनिवत्त FAIL;

	सचे (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    सचे (tuple_append_tv(tuple, rettv) == FAIL)
		गच्छ failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    सचे (**arg == ')')
	// empty tuple
	गच्छ done;

    सचे (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	गच्छ failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    याव (**arg != ')' && **arg != NUL)
    {
	सचे (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    गच्छ failret;
	सचे (check_typval_is_मानम्(&tv) == FAIL)
	{
	    सचे (evaluate)
		clear_tv(&tv);
	    गच्छ failret;
	}

	सचे (evaluate)
	{
	    सचे (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		गच्छ failret;
	    }
	}

	सचे (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	सचे (had_comma)
	{
	    सचे (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		गच्छ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	सचे (**arg == ')')
	    भङ्ग;

	सचे (!had_comma)
	{
	    सचे (do_error)
	    {
		सचे (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		अञ्ञथा
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    गच्छ failret;
	}
    }

    सचे (**arg != ')')
    {
	सचे (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	सचे (evaluate)
	    tuple_free(tuple);
	पटिनिवत्त FAIL;
    }

done:
    *arg += 1;
    सचे (evaluate)
	rettv_tuple_set(rettv, tuple);

    पटिनिवत्त OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    शून्य
tuple_lock(tuple_T *tuple, पूर्णाङ्क deep, पूर्णाङ्क lock, पूर्णाङ्क check_refcount)
{
    सचे (tuple == रिक्त || (check_refcount && tuple->tv_refcount > 1))
	पटिनिवत्त;

    सचे (lock)
	tuple->tv_lock |= VAR_LOCKED;
    अञ्ञथा
	tuple->tv_lock &= ~VAR_LOCKED;

    सचे (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	अत्थाय (पूर्णाङ्क i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

प्रकारनिर्देश संरचना join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    स्थिर पूर्णाङ्क
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    पूर्णाङ्क		echo_style,
    पूर्णाङ्क		restore_copyID,
    पूर्णाङ्क		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    पूर्णाङ्क		i;
    join_T	*p;
    पूर्णाङ्क		sumlen = 0;
    पूर्णाङ्क		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    अत्थाय (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	सचे (s.string == रिक्त)
	    पटिनिवत्त FAIL;

	s.length = STRLEN(s.string);
	sumlen += (पूर्णाङ्क)s.length;

	(शून्य)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	सचे (tofree != रिक्त || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	अञ्ञथा
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	पङ्क्तिः_breakcheck();
	सचे (did_echo_string_emsg)  // recursion error, bail out
	    भङ्ग;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    सचे (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	सचे (join_gap->ga_len >= 2)
	    sumlen += (पूर्णाङ्क)seplen * (join_gap->ga_len - 1);
    }
    सचे (ga_grow(gap, sumlen + 2) == FAIL)
	पटिनिवत्त FAIL;

    अत्थाय (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	सचे (first)
	    first = FALSE;
	अञ्ञथा
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	सचे (p->s.string != रिक्त)
	    ga_concat_len(gap, p->s.string, p->s.length);
	पङ्क्तिः_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    सचे (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    पटिनिवत्त OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    पूर्णाङ्क
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    पूर्णाङ्क		echo_style,
    पूर्णाङ्क		restore_copyID,
    पूर्णाङ्क		copyID)
{
    garray_T	join_ga;
    पूर्णाङ्क		retval;
    join_T	*p;
    पूर्णाङ्क		i;

    सचे (TUPLE_LEN(tuple) < 1)
	पटिनिवत्त OK; // nothing to do
    ga_init2(&join_ga, परिमाण(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    सचे (join_ga.ga_data == रिक्त)
	पटिनिवत्त retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    अत्थाय (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    पटिनिवत्त retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, पूर्णाङ्क copyID, पूर्णाङ्क restore_copyID)
{
    garray_T	ga;

    सचे (tv->vval.v_tuple == रिक्त)
	पटिनिवत्त रिक्त;
    ga_init2(&ga, परिमाण(वर्ण), 80);
    ga_append(&ga, '(');
    सचे (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	पटिनिवत्त रिक्त;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    पटिनिवत्त (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    शून्य
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*व्यञ्जकम्)
{
    पूर्णाङ्क		len = tuple_len(tuple);
    पूर्णाङ्क		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_कुञ्चिका, VAR_सङ्ख्या);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_व्यञ्जकम्_get_funccal(व्यञ्जकम्, &newtv);

    अत्थाय (पूर्णाङ्क idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_कुञ्चिका, idx);
	सचे (filter_map_one(TUPLE_ITEM(tuple, idx), व्यञ्जकम्, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    भङ्ग;
    }

    सचे (fc != रिक्त)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_गणनम्(tuple_T *tuple, typval_T *needle, long idx, पूर्णाङ्क ic)
{
    long	n = 0;

    सचे (tuple == रिक्त)
	पटिनिवत्त 0;

    पूर्णाङ्क	len = TUPLE_LEN(tuple);
    सचे (len == 0)
	पटिनिवत्त 0;

    सचे (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	पटिनिवत्त 0;
    }

    अत्थाय (पूर्णाङ्क i = idx; i < len; i++)
    {
	सचे (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    पटिनिवत्त n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    शून्य
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    सचे (rettv_list_alloc(rettv) == FAIL)
	पटिनिवत्त;

    सचे (tuple == रिक्त)
	पटिनिवत्त;  // null tuple behaves like an empty list

    अत्थाय (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	सचे (l == रिक्त)
	    भङ्ग;

	सचे (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    भङ्ग;
	}
	सचे (list_append_सङ्ख्या(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    भङ्ग;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    पूर्णाङ्क
index_tuple(tuple_T *tuple, typval_T *tv, पूर्णाङ्क start_idx, पूर्णाङ्क ic)
{
    सचे (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	सचे (start_idx < 0)
	    start_idx = 0;
    }

    अत्थाय (पूर्णाङ्क idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	सचे (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    पटिनिवत्त idx;
    }

    पटिनिवत्त -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    पूर्णाङ्क
indexof_tuple(tuple_T *tuple, long startidx, typval_T *व्यञ्जकम्)
{
    long	idx = 0;
    पूर्णाङ्क		len;
    पूर्णाङ्क		found;

    सचे (tuple == रिक्त)
	पटिनिवत्त -1;

    len = TUPLE_LEN(tuple);

    सचे (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	सचे (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_कुञ्चिका, VAR_सङ्ख्या);

    पूर्णाङ्क		called_emsg_start = called_emsg;

    अत्थाय (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_कुञ्चिका, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_व्यञ्जकम्(व्यञ्जकम्);
	clear_tv(get_vim_var_tv(VV_VAL));

	सचे (found)
	    पटिनिवत्त idx;

	सचे (called_emsg != called_emsg_start)
	    पटिनिवत्त -1;
    }

    पटिनिवत्त -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, पूर्णाङ्क domax)
{
    typval_T	*tv = रिक्त;
    पूर्णाङ्क		res;

    सचे (tuple == रिक्त || TUPLE_LEN(tuple) == 0)
	पटिनिवत्त रिक्त;

    अत्थाय (पूर्णाङ्क idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	सचे (tv == रिक्त)
	    tv = TUPLE_ITEM(tuple, idx);
	अञ्ञथा
	{
	    सचे (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? व्यञ्जकम्_GREATER : व्यञ्जकम्_SMALLER, FALSE, &res) == FAIL)
		पटिनिवत्त रिक्त;
	    सचे (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    पटिनिवत्त tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    शून्य
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = रिक्त;

    सचे (tuple == रिक्त || TUPLE_LEN(tuple) == 0 || n <= 0)
	पटिनिवत्त;

    सचे (check_repeat_गणनम्(TUPLE_LEN(tuple), n) == FAIL)
	पटिनिवत्त;

    सचे (rettv_tuple_set_with_वस्तूनि(rettv, TUPLE_LEN(tuple) * (पूर्णाङ्क)n) == FAIL)
	पटिनिवत्त;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    अत्थाय (पूर्णाङ्क गणनम् = 0; गणनम् < (पूर्णाङ्क)n; गणनम्++)
    {
	अत्थाय (पूर्णाङ्क idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_वस्तूनि.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    शून्य
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = रिक्त;

    पूर्णाङ्क	len = tuple_len(tuple);

    सचे (len == 0)
	पटिनिवत्त;

    सचे (rettv_tuple_set_with_वस्तूनि(rettv, len) == FAIL)
	पटिनिवत्त;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    अत्थाय (पूर्णाङ्क i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_वस्तूनि.ga_len = tuple->tv_वस्तूनि.ga_len;
}

/*
 * Tuple reduce() function
 */
    शून्य
tuple_reduce(typval_T *argvars, typval_T *व्यञ्जकम्, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    पूर्णाङ्क		called_emsg_start = called_emsg;
    typval_T	initial;
    पूर्णाङ्क		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    पूर्णाङ्क		r;

    सचे (argvars[2].v_type == VAR_UNKNOWN)
    {
	सचे (tuple == रिक्त || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_मानम्), "Tuple");
	    पटिनिवत्त;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    अञ्ञथा
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    सचे (tuple == रिक्त)
	पटिनिवत्त;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_व्यञ्जकम्_get_funccal(व्यञ्जकम्, rettv);

    अत्थाय ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_व्यञ्जकम्_typval(व्यञ्जकम्, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	सचे (r == FAIL || called_emsg != called_emsg_start)
	    भङ्ग;
    }

    सचे (fc != रिक्त)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    पूर्णाङ्क
check_tuples_addable(type_T *type1, type_T *type2)
{
    पूर्णाङ्क	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    सचे ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    सचे ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	सचे (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	अञ्ञथा सचे ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    सचे (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	पटिनिवत्त FAIL;
    }

    पटिनिवत्त OK;
}

#endif // defined(FEAT_EVAL)
