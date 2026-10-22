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

#නම් defined(FEAT_EVAL)

// Tuple heads for garbage collection.
static tuple_T		*first_tuple = NULL;	// list of all tuples

    static void
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    නම් (first_tuple != NULL)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = NULL;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_items, parimaa_na(typval_T), 20);
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
    නම් (tuple != NULL)
	tuple_init(tuple);
    ආපසු tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_items(පූර්ණ_සංඛ්‍යා count)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    නම් (tuple == NULL)
	ආපසු NULL;

    නම් (count <= 0)
	ආපසු tuple;

    නම් (ga_grow(&tuple->tv_items, count) == FAIL)
    {
	tuple_free(tuple);
	ආපසු NULL;
    }

    ආපසු tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    void
tuple_set_item(tuple_T *tuple, පූර්ණ_සංඛ්‍යා idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_items.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    පූර්ණ_සංඛ්‍යා
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    නම් (tuple == NULL)
	ආපසු FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    ආපසු OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    void
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    නම් (tuple != NULL)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    පූර්ණ_සංඛ්‍යා
rettv_tuple_set_with_items(typval_T *rettv, පූර්ණ_සංඛ්‍යා count)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_items(count);
    නම් (new_tuple == NULL)
	ආපසු FAIL;

    rettv_tuple_set(rettv, new_tuple);

    ආපසු OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    void
tuple_unref(tuple_T *tuple)
{
    නම් (tuple != NULL && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    static void
tuple_free_contents(tuple_T *tuple)
{
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_items);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    පූර්ණ_සංඛ්‍යා
tuple_free_nonref(පූර්ණ_සංඛ්‍යා copyID)
{
    tuple_T	*tt;
    පූර්ණ_සංඛ්‍යා		did_free = FALSE;

    සඳහා (tt = first_tuple; tt != NULL; tt = tt->tv_used_next)
	නම් ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ආපසු did_free;
}

    static void
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    නම් (tuple->tv_used_prev == NULL)
	first_tuple = tuple->tv_used_next;
    නැතහොත්
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    නම් (tuple->tv_used_next != NULL)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    void
tuple_free_items(පූර්ණ_සංඛ්‍යා copyID)
{
    tuple_T	*tt, *tt_next;

    සඳහා (tt = first_tuple; tt != NULL; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	නම් ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
    නම් (in_free_unref_items)
	ආපසු;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    නම් (tuple == NULL)
	ආපසු 0L;
    ආපසු tuple->tv_items.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    පූර්ණ_සංඛ්‍යා
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    පූර්ණ_සංඛ්‍යා		ic)	// ignore case for strings
{
    නම් (t1 == t2)
	ආපසු TRUE;

    පූර්ණ_සංඛ්‍යා t1_len = tuple_len(t1);
    පූර්ණ_සංඛ්‍යා t2_len = tuple_len(t2);

    නම් (t1_len != t2_len)
	ආපසු FALSE;

    නම් (t1_len == 0)
	// empty and NULL tuples are considered equal
	ආපසු TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    සඳහා (පූර්ණ_සංඛ්‍යා i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	නම් (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    ආපසු FALSE;

    ආපසු TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    නම් (tuple == NULL)
	ආපසු NULL;

    // Negative index is relative to the end.
    නම් (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    නම් (n < 0 || n >= TUPLE_LEN(tuple))
	ආපසු NULL;

    ආපසු TUPLE_ITEM(tuple, n);
}

    පූර්ණ_සංඛ්‍යා
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    නම් (ga_grow_id(&tuple->tv_items, 1, aid_tuple_append) == FAIL)
	ආපසු FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    ආපසු OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    පූර්ණ_සංඛ්‍යා
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    නම් (t1 == NULL)
	tuple = tuple_alloc();
    නැතහොත්
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    නම් (tuple == NULL)
	ආපසු FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    නම් (t1 == NULL)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	නම් (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ආපසු FAIL;
	}
    }

    ආපසු OK;
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
    නම් (new_tuple == NULL)
	ආපසු NULL;

    සඳහා (පූර්ණ_සංඛ්‍යා i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	නම් (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ආපසු NULL;
	}
    }

    ආපසු new_tuple;
}

    පූර්ණ_සංඛ්‍යා
tuple_slice_or_index(
    tuple_T	*tuple,
    පූර්ණ_සංඛ්‍යා		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    පූර්ණ_සංඛ්‍යා		exclusive,
    typval_T	*rettv,
    පූර්ණ_සංඛ්‍යා		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    නම් (n1 < 0)
	n1 = len + n1;
    නම් (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	නම් (!range)
	{
	    නම් (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    ආපසු FAIL;
	}
	නම් (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	නැතහොත්
	    n1 = len;
    }
    නම් (range)
    {
	tuple_T	*new_tuple;

	නම් (n2 < 0)
	    n2 = len + n2;
	නැතහොත් නම් (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	නම් (exclusive)
	    --n2;
	නම් (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	නම් (new_tuple == NULL)
	    ආපසු FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    නැතහොත්
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ආපසු OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, පූර්ණ_සංඛ්‍යා deep, පූර්ණ_සංඛ්‍යා top, පූර්ණ_සංඛ්‍යා copyID)
{
    tuple_T	*copy;
    පූර්ණ_සංඛ්‍යා		idx;

    නම් (orig == NULL)
	ආපසු NULL;

    copy = tuple_alloc_with_items(TUPLE_LEN(orig));
    නම් (copy == NULL)
	ආපසු NULL;

    නම් (orig->tv_type == NULL || top || deep)
	copy->tv_type = NULL;
    නැතහොත්
	copy->tv_type = alloc_type(orig->tv_type);
    නම් (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    සඳහා (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_items.ga_len++;
	නම් (deep)
	{
	    නම් (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		bha_nga;
	}
	නැතහොත්
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    නම් (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = NULL;
    }

    ආපසු copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    පූර්ණ_සංඛ්‍යා
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, පූර්ණ_සංඛ්‍යා do_error)
{
    පූර්ණ_සංඛ්‍යා		evaluate = evalarg == NULL ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = NULL;
    typval_T	tv;
    පූර්ණ_සංඛ්‍යා		vim9script = in_vim9script();
    පූර්ණ_සංඛ්‍යා		had_comma;

    නම් (check_typval_is_value(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ආපසු FAIL;
    }

    නම් (evaluate)
    {
	tuple = tuple_alloc();
	නම් (tuple == NULL)
	    ආපසු FAIL;

	නම් (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    නම් (tuple_append_tv(tuple, rettv) == FAIL)
		goto failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    නම් (**arg == ')')
	// empty tuple
	goto done;

    නම් (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	goto failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    අතරතුර (**arg != ')' && **arg != NUL)
    {
	නම් (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    goto failret;
	නම් (check_typval_is_value(&tv) == FAIL)
	{
	    නම් (evaluate)
		clear_tv(&tv);
	    goto failret;
	}

	නම් (evaluate)
	{
	    නම් (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		goto failret;
	    }
	}

	නම් (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	නම් (had_comma)
	{
	    නම් (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		goto failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	නම් (**arg == ')')
	    bha_nga;

	නම් (!had_comma)
	{
	    නම් (do_error)
	    {
		නම් (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		නැතහොත්
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    goto failret;
	}
    }

    නම් (**arg != ')')
    {
	නම් (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	නම් (evaluate)
	    tuple_free(tuple);
	ආපසු FAIL;
    }

done:
    *arg += 1;
    නම් (evaluate)
	rettv_tuple_set(rettv, tuple);

    ආපසු OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    void
tuple_lock(tuple_T *tuple, පූර්ණ_සංඛ්‍යා deep, පූර්ණ_සංඛ්‍යා lock, පූර්ණ_සංඛ්‍යා check_refcount)
{
    නම් (tuple == NULL || (check_refcount && tuple->tv_refcount > 1))
	ආපසු;

    නම් (lock)
	tuple->tv_lock |= VAR_LOCKED;
    නැතහොත්
	tuple->tv_lock &= ~VAR_LOCKED;

    නම් (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

typedef sa_nracanaa join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    static පූර්ණ_සංඛ්‍යා
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    පූර්ණ_සංඛ්‍යා		echo_style,
    පූර්ණ_සංඛ්‍යා		restore_copyID,
    පූර්ණ_සංඛ්‍යා		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    පූර්ණ_සංඛ්‍යා		i;
    join_T	*p;
    පූර්ණ_සංඛ්‍යා		sumlen = 0;
    පූර්ණ_සංඛ්‍යා		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    සඳහා (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	නම් (s.string == NULL)
	    ආපසු FAIL;

	s.length = STRLEN(s.string);
	sumlen += (පූර්ණ_සංඛ්‍යා)s.length;

	(void)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	නම් (tofree != NULL || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	නැතහොත්
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	line_breakcheck();
	නම් (did_echo_string_emsg)  // recursion error, bail out
	    bha_nga;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    නම් (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	නම් (join_gap->ga_len >= 2)
	    sumlen += (පූර්ණ_සංඛ්‍යා)seplen * (join_gap->ga_len - 1);
    }
    නම් (ga_grow(gap, sumlen + 2) == FAIL)
	ආපසු FAIL;

    සඳහා (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	නම් (first)
	    first = FALSE;
	නැතහොත්
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	නම් (p->s.string != NULL)
	    ga_concat_len(gap, p->s.string, p->s.length);
	line_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    නම් (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ආපසු OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    පූර්ණ_සංඛ්‍යා
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    පූර්ණ_සංඛ්‍යා		echo_style,
    පූර්ණ_සංඛ්‍යා		restore_copyID,
    පූර්ණ_සංඛ්‍යා		copyID)
{
    garray_T	join_ga;
    පූර්ණ_සංඛ්‍යා		retval;
    join_T	*p;
    පූර්ණ_සංඛ්‍යා		i;

    නම් (TUPLE_LEN(tuple) < 1)
	ආපසු OK; // nothing to do
    ga_init2(&join_ga, parimaa_na(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    නම් (join_ga.ga_data == NULL)
	ආපසු retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    සඳහා (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ආපසු retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, පූර්ණ_සංඛ්‍යා copyID, පූර්ණ_සංඛ්‍යා restore_copyID)
{
    garray_T	ga;

    නම් (tv->vval.v_tuple == NULL)
	ආපසු NULL;
    ga_init2(&ga, parimaa_na(var_na), 80);
    ga_append(&ga, '(');
    නම් (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ආපසු NULL;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ආපසු (char_u *)ga.ga_data;
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
    පූර්ණ_සංඛ්‍යා		len = tuple_len(tuple);
    පූර්ණ_සංඛ්‍යා		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    සඳහා (පූර්ණ_සංඛ්‍යා idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	නම් (filter_map_one(TUPLE_ITEM(tuple, idx), expr, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    bha_nga;
    }

    නම් (fc != NULL)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_count(tuple_T *tuple, typval_T *needle, long idx, පූර්ණ_සංඛ්‍යා ic)
{
    long	n = 0;

    නම් (tuple == NULL)
	ආපසු 0;

    පූර්ණ_සංඛ්‍යා	len = TUPLE_LEN(tuple);
    නම් (len == 0)
	ආපසු 0;

    නම් (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	ආපසු 0;
    }

    සඳහා (පූර්ණ_සංඛ්‍යා i = idx; i < len; i++)
    {
	නම් (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    ආපසු n;
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

    නම් (rettv_list_alloc(rettv) == FAIL)
	ආපසු;

    නම් (tuple == NULL)
	ආපසු;  // null tuple behaves like an empty list

    සඳහා (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	නම් (l == NULL)
	    bha_nga;

	නම් (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    bha_nga;
	}
	නම් (list_append_number(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    bha_nga;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    පූර්ණ_සංඛ්‍යා
index_tuple(tuple_T *tuple, typval_T *tv, පූර්ණ_සංඛ්‍යා start_idx, පූර්ණ_සංඛ්‍යා ic)
{
    නම් (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	නම් (start_idx < 0)
	    start_idx = 0;
    }

    සඳහා (පූර්ණ_සංඛ්‍යා idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	නම් (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    ආපසු idx;
    }

    ආපසු -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    පූර්ණ_සංඛ්‍යා
indexof_tuple(tuple_T *tuple, long startidx, typval_T *expr)
{
    long	idx = 0;
    පූර්ණ_සංඛ්‍යා		len;
    පූර්ණ_සංඛ්‍යා		found;

    නම් (tuple == NULL)
	ආපසු -1;

    len = TUPLE_LEN(tuple);

    නම් (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	නම් (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_KEY, VAR_NUMBER);

    පූර්ණ_සංඛ්‍යා		called_emsg_start = called_emsg;

    සඳහා (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_expr(expr);
	clear_tv(get_vim_var_tv(VV_VAL));

	නම් (found)
	    ආපසු idx;

	නම් (called_emsg != called_emsg_start)
	    ආපසු -1;
    }

    ආපසු -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, පූර්ණ_සංඛ්‍යා domax)
{
    typval_T	*tv = NULL;
    පූර්ණ_සංඛ්‍යා		res;

    නම් (tuple == NULL || TUPLE_LEN(tuple) == 0)
	ආපසු NULL;

    සඳහා (පූර්ණ_සංඛ්‍යා idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	නම් (tv == NULL)
	    tv = TUPLE_ITEM(tuple, idx);
	නැතහොත්
	{
	    නම් (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? EXPR_GREATER : EXPR_SMALLER, FALSE, &res) == FAIL)
		ආපසු NULL;
	    නම් (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    ආපසු tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    void
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = NULL;

    නම් (tuple == NULL || TUPLE_LEN(tuple) == 0 || n <= 0)
	ආපසු;

    නම් (check_repeat_count(TUPLE_LEN(tuple), n) == FAIL)
	ආපසු;

    නම් (rettv_tuple_set_with_items(rettv, TUPLE_LEN(tuple) * (පූර්ණ_සංඛ්‍යා)n) == FAIL)
	ආපසු;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    සඳහා (පූර්ණ_සංඛ්‍යා count = 0; count < (පූර්ණ_සංඛ්‍යා)n; count++)
    {
	සඳහා (පූර්ණ_සංඛ්‍යා idx = 0; idx < TUPLE_LEN(tuple); idx++)
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

    පූර්ණ_සංඛ්‍යා	len = tuple_len(tuple);

    නම් (len == 0)
	ආපසු;

    නම් (rettv_tuple_set_with_items(rettv, len) == FAIL)
	ආපසු;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    සඳහා (පූර්ණ_සංඛ්‍යා i = 0; i < len; i++)
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
    පූර්ණ_සංඛ්‍යා		called_emsg_start = called_emsg;
    typval_T	initial;
    පූර්ණ_සංඛ්‍යා		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    පූර්ණ_සංඛ්‍යා		r;

    නම් (argvars[2].v_type == VAR_UNKNOWN)
    {
	නම් (tuple == NULL || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Tuple");
	    ආපසු;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    නැතහොත්
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    නම් (tuple == NULL)
	ආපසු;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    සඳහා ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	නම් (r == FAIL || called_emsg != called_emsg_start)
	    bha_nga;
    }

    නම් (fc != NULL)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    පූර්ණ_සංඛ්‍යා
check_tuples_addable(type_T *type1, type_T *type2)
{
    පූර්ණ_සංඛ්‍යා	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    නම් ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    නම් ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	නම් (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	නැතහොත් නම් ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    නම් (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	ආපසු FAIL;
    }

    ආපසු OK;
}

#endif // defined(FEAT_EVAL)
