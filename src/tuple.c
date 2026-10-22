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

#यदि defined(FEAT_EVAL)

// Tuple heads for garbage collection.
स्थिर tuple_T		*first_tuple = रिक्त;	// list of all tuples

    स्थिर शून्य
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    यदि (first_tuple != रिक्त)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = रिक्त;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_वस्तुएँ, परिमाण(typval_T), 20);
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
    यदि (tuple != रिक्त)
	tuple_init(tuple);
    प्रत्यागम tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_वस्तुएँ(पूर्णाङ्क गिनती)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    यदि (tuple == रिक्त)
	प्रत्यागम रिक्त;

    यदि (गिनती <= 0)
	प्रत्यागम tuple;

    यदि (ga_grow(&tuple->tv_वस्तुएँ, गिनती) == FAIL)
    {
	tuple_free(tuple);
	प्रत्यागम रिक्त;
    }

    प्रत्यागम tuple;
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
    tuple->tv_वस्तुएँ.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    यदि (tuple == रिक्त)
	प्रत्यागम FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    प्रत्यागम OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    शून्य
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    यदि (tuple != रिक्त)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    पूर्णाङ्क
rettv_tuple_set_with_वस्तुएँ(typval_T *rettv, पूर्णाङ्क गिनती)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_वस्तुएँ(गिनती);
    यदि (new_tuple == रिक्त)
	प्रत्यागम FAIL;

    rettv_tuple_set(rettv, new_tuple);

    प्रत्यागम OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    शून्य
tuple_unref(tuple_T *tuple)
{
    यदि (tuple != रिक्त && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    स्थिर शून्य
tuple_free_contents(tuple_T *tuple)
{
    प्रति (पूर्णाङ्क i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_वस्तुएँ);
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

    प्रति (tt = first_tuple; tt != रिक्त; tt = tt->tv_used_next)
	यदि ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    प्रत्यागम did_free;
}

    स्थिर शून्य
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    यदि (tuple->tv_used_prev == रिक्त)
	first_tuple = tuple->tv_used_next;
    अन्यथा
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    यदि (tuple->tv_used_next != रिक्त)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    शून्य
tuple_free_वस्तुएँ(पूर्णाङ्क copyID)
{
    tuple_T	*tt, *tt_next;

    प्रति (tt = first_tuple; tt != रिक्त; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	यदि ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
    यदि (in_free_unref_वस्तुएँ)
	प्रत्यागम;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    यदि (tuple == रिक्त)
	प्रत्यागम 0L;
    प्रत्यागम tuple->tv_वस्तुएँ.ga_len;
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
    यदि (t1 == t2)
	प्रत्यागम TRUE;

    पूर्णाङ्क t1_len = tuple_len(t1);
    पूर्णाङ्क t2_len = tuple_len(t2);

    यदि (t1_len != t2_len)
	प्रत्यागम FALSE;

    यदि (t1_len == 0)
	// empty and NULL tuples are considered equal
	प्रत्यागम TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    प्रति (पूर्णाङ्क i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	यदि (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    प्रत्यागम FALSE;

    प्रत्यागम TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    यदि (tuple == रिक्त)
	प्रत्यागम रिक्त;

    // Negative index is relative to the end.
    यदि (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    यदि (n < 0 || n >= TUPLE_LEN(tuple))
	प्रत्यागम रिक्त;

    प्रत्यागम TUPLE_ITEM(tuple, n);
}

    पूर्णाङ्क
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    यदि (ga_grow_id(&tuple->tv_वस्तुएँ, 1, aid_tuple_append) == FAIL)
	प्रत्यागम FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    प्रत्यागम OK;
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
    यदि (t1 == रिक्त)
	tuple = tuple_alloc();
    अन्यथा
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    यदि (tuple == रिक्त)
	प्रत्यागम FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    यदि (t1 == रिक्त)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    प्रति (पूर्णाङ्क i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	यदि (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    प्रत्यागम FAIL;
	}
    }

    प्रत्यागम OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_वस्तुएँ(n2 - n1 + 1);
    यदि (new_tuple == रिक्त)
	प्रत्यागम रिक्त;

    प्रति (पूर्णाङ्क i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	यदि (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    प्रत्यागम रिक्त;
	}
    }

    प्रत्यागम new_tuple;
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

    यदि (n1 < 0)
	n1 = len + n1;
    यदि (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	यदि (!range)
	{
	    यदि (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    प्रत्यागम FAIL;
	}
	यदि (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	अन्यथा
	    n1 = len;
    }
    यदि (range)
    {
	tuple_T	*new_tuple;

	यदि (n2 < 0)
	    n2 = len + n2;
	अन्यथा यदि (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	यदि (exclusive)
	    --n2;
	यदि (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	यदि (new_tuple == रिक्त)
	    प्रत्यागम FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    अन्यथा
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    प्रत्यागम OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, पूर्णाङ्क deep, पूर्णाङ्क शीर्ष, पूर्णाङ्क copyID)
{
    tuple_T	*copy;
    पूर्णाङ्क		idx;

    यदि (orig == रिक्त)
	प्रत्यागम रिक्त;

    copy = tuple_alloc_with_वस्तुएँ(TUPLE_LEN(orig));
    यदि (copy == रिक्त)
	प्रत्यागम रिक्त;

    यदि (orig->tv_type == रिक्त || शीर्ष || deep)
	copy->tv_type = रिक्त;
    अन्यथा
	copy->tv_type = alloc_type(orig->tv_type);
    यदि (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    प्रति (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_वस्तुएँ.ga_len++;
	यदि (deep)
	{
	    यदि (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		भङ्ग;
	}
	अन्यथा
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    यदि (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = रिक्त;
    }

    प्रत्यागम copy;
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

    यदि (check_typval_is_मूल्य(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	प्रत्यागम FAIL;
    }

    यदि (evaluate)
    {
	tuple = tuple_alloc();
	यदि (tuple == रिक्त)
	    प्रत्यागम FAIL;

	यदि (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    यदि (tuple_append_tv(tuple, rettv) == FAIL)
		गच्छ failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    यदि (**arg == ')')
	// empty tuple
	गच्छ done;

    यदि (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	गच्छ failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    यावत् (**arg != ')' && **arg != NUL)
    {
	यदि (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    गच्छ failret;
	यदि (check_typval_is_मूल्य(&tv) == FAIL)
	{
	    यदि (evaluate)
		clear_tv(&tv);
	    गच्छ failret;
	}

	यदि (evaluate)
	{
	    यदि (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		गच्छ failret;
	    }
	}

	यदि (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	यदि (had_comma)
	{
	    यदि (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		गच्छ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	यदि (**arg == ')')
	    भङ्ग;

	यदि (!had_comma)
	{
	    यदि (do_error)
	    {
		यदि (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		अन्यथा
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    गच्छ failret;
	}
    }

    यदि (**arg != ')')
    {
	यदि (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	यदि (evaluate)
	    tuple_free(tuple);
	प्रत्यागम FAIL;
    }

done:
    *arg += 1;
    यदि (evaluate)
	rettv_tuple_set(rettv, tuple);

    प्रत्यागम OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    शून्य
tuple_lock(tuple_T *tuple, पूर्णाङ्क deep, पूर्णाङ्क lock, पूर्णाङ्क check_refcount)
{
    यदि (tuple == रिक्त || (check_refcount && tuple->tv_refcount > 1))
	प्रत्यागम;

    यदि (lock)
	tuple->tv_lock |= VAR_LOCKED;
    अन्यथा
	tuple->tv_lock &= ~VAR_LOCKED;

    यदि (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	प्रति (पूर्णाङ्क i = 0; i < TUPLE_LEN(tuple); i++)
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
    प्रति (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	यदि (s.string == रिक्त)
	    प्रत्यागम FAIL;

	s.length = STRLEN(s.string);
	sumlen += (पूर्णाङ्क)s.length;

	(शून्य)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	यदि (tofree != रिक्त || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	अन्यथा
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	पंक्ति_breakcheck();
	यदि (did_echo_string_emsg)  // recursion error, bail out
	    भङ्ग;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    यदि (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	यदि (join_gap->ga_len >= 2)
	    sumlen += (पूर्णाङ्क)seplen * (join_gap->ga_len - 1);
    }
    यदि (ga_grow(gap, sumlen + 2) == FAIL)
	प्रत्यागम FAIL;

    प्रति (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	यदि (first)
	    first = FALSE;
	अन्यथा
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	यदि (p->s.string != रिक्त)
	    ga_concat_len(gap, p->s.string, p->s.length);
	पंक्ति_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    यदि (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    प्रत्यागम OK;
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

    यदि (TUPLE_LEN(tuple) < 1)
	प्रत्यागम OK; // nothing to do
    ga_init2(&join_ga, परिमाण(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    यदि (join_ga.ga_data == रिक्त)
	प्रत्यागम retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    प्रति (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    प्रत्यागम retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, पूर्णाङ्क copyID, पूर्णाङ्क restore_copyID)
{
    garray_T	ga;

    यदि (tv->vval.v_tuple == रिक्त)
	प्रत्यागम रिक्त;
    ga_init2(&ga, परिमाण(वर्ण), 80);
    ga_append(&ga, '(');
    यदि (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	प्रत्यागम रिक्त;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    प्रत्यागम (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    शून्य
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*व्यंजक)
{
    पूर्णाङ्क		len = tuple_len(tuple);
    पूर्णाङ्क		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_कुंजी, VAR_संख्या);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_व्यंजक_get_funccal(व्यंजक, &newtv);

    प्रति (पूर्णाङ्क idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_कुंजी, idx);
	यदि (filter_map_one(TUPLE_ITEM(tuple, idx), व्यंजक, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    भङ्ग;
    }

    यदि (fc != रिक्त)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_गिनती(tuple_T *tuple, typval_T *needle, long idx, पूर्णाङ्क ic)
{
    long	n = 0;

    यदि (tuple == रिक्त)
	प्रत्यागम 0;

    पूर्णाङ्क	len = TUPLE_LEN(tuple);
    यदि (len == 0)
	प्रत्यागम 0;

    यदि (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	प्रत्यागम 0;
    }

    प्रति (पूर्णाङ्क i = idx; i < len; i++)
    {
	यदि (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    प्रत्यागम n;
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

    यदि (rettv_list_alloc(rettv) == FAIL)
	प्रत्यागम;

    यदि (tuple == रिक्त)
	प्रत्यागम;  // null tuple behaves like an empty list

    प्रति (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	यदि (l == रिक्त)
	    भङ्ग;

	यदि (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    भङ्ग;
	}
	यदि (list_append_संख्या(l, idx) == FAIL
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
    यदि (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	यदि (start_idx < 0)
	    start_idx = 0;
    }

    प्रति (पूर्णाङ्क idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	यदि (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    प्रत्यागम idx;
    }

    प्रत्यागम -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    पूर्णाङ्क
indexof_tuple(tuple_T *tuple, long startidx, typval_T *व्यंजक)
{
    long	idx = 0;
    पूर्णाङ्क		len;
    पूर्णाङ्क		found;

    यदि (tuple == रिक्त)
	प्रत्यागम -1;

    len = TUPLE_LEN(tuple);

    यदि (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	यदि (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_कुंजी, VAR_संख्या);

    पूर्णाङ्क		called_emsg_start = called_emsg;

    प्रति (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_कुंजी, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_व्यंजक(व्यंजक);
	clear_tv(get_vim_var_tv(VV_VAL));

	यदि (found)
	    प्रत्यागम idx;

	यदि (called_emsg != called_emsg_start)
	    प्रत्यागम -1;
    }

    प्रत्यागम -1;
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

    यदि (tuple == रिक्त || TUPLE_LEN(tuple) == 0)
	प्रत्यागम रिक्त;

    प्रति (पूर्णाङ्क idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	यदि (tv == रिक्त)
	    tv = TUPLE_ITEM(tuple, idx);
	अन्यथा
	{
	    यदि (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? व्यंजक_GREATER : व्यंजक_SMALLER, FALSE, &res) == FAIL)
		प्रत्यागम रिक्त;
	    यदि (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    प्रत्यागम tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    शून्य
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = रिक्त;

    यदि (tuple == रिक्त || TUPLE_LEN(tuple) == 0 || n <= 0)
	प्रत्यागम;

    यदि (check_repeat_गिनती(TUPLE_LEN(tuple), n) == FAIL)
	प्रत्यागम;

    यदि (rettv_tuple_set_with_वस्तुएँ(rettv, TUPLE_LEN(tuple) * (पूर्णाङ्क)n) == FAIL)
	प्रत्यागम;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    प्रति (पूर्णाङ्क गिनती = 0; गिनती < (पूर्णाङ्क)n; गिनती++)
    {
	प्रति (पूर्णाङ्क idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_वस्तुएँ.ga_len++;
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

    यदि (len == 0)
	प्रत्यागम;

    यदि (rettv_tuple_set_with_वस्तुएँ(rettv, len) == FAIL)
	प्रत्यागम;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    प्रति (पूर्णाङ्क i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_वस्तुएँ.ga_len = tuple->tv_वस्तुएँ.ga_len;
}

/*
 * Tuple reduce() function
 */
    शून्य
tuple_reduce(typval_T *argvars, typval_T *व्यंजक, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    पूर्णाङ्क		called_emsg_start = called_emsg;
    typval_T	initial;
    पूर्णाङ्क		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    पूर्णाङ्क		r;

    यदि (argvars[2].v_type == VAR_UNKNOWN)
    {
	यदि (tuple == रिक्त || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_मूल्य), "Tuple");
	    प्रत्यागम;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    अन्यथा
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    यदि (tuple == रिक्त)
	प्रत्यागम;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_व्यंजक_get_funccal(व्यंजक, rettv);

    प्रति ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_व्यंजक_typval(व्यंजक, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	यदि (r == FAIL || called_emsg != called_emsg_start)
	    भङ्ग;
    }

    यदि (fc != रिक्त)
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
    यदि ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    यदि ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	यदि (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	अन्यथा यदि ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    यदि (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}

#endif // defined(FEAT_EVAL)
