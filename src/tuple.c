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

#ប្រសិនបើ defined(FEAT_EVAL)

// Tuple heads for garbage collection.
static tuple_T		*first_tuple = NULL;	// list of all tuples

    static void
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    ប្រសិនបើ (first_tuple != NULL)
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
    ប្រសិនបើ (tuple != NULL)
	tuple_init(tuple);
    ត្រឡប់ tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_items(ចំនួនគត់ count)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់ NULL;

    ប្រសិនបើ (count <= 0)
	ត្រឡប់ tuple;

    ប្រសិនបើ (ga_grow(&tuple->tv_items, count) == FAIL)
    {
	tuple_free(tuple);
	ត្រឡប់ NULL;
    }

    ត្រឡប់ tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    void
tuple_set_item(tuple_T *tuple, ចំនួនគត់ idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_items.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ចំនួនគត់
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់ FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    ត្រឡប់ OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    void
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    ប្រសិនបើ (tuple != NULL)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    ចំនួនគត់
rettv_tuple_set_with_items(typval_T *rettv, ចំនួនគត់ count)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_items(count);
    ប្រសិនបើ (new_tuple == NULL)
	ត្រឡប់ FAIL;

    rettv_tuple_set(rettv, new_tuple);

    ត្រឡប់ OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    void
tuple_unref(tuple_T *tuple)
{
    ប្រសិនបើ (tuple != NULL && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    static void
tuple_free_contents(tuple_T *tuple)
{
    សម្រាប់ (ចំនួនគត់ i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_items);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ចំនួនគត់
tuple_free_nonref(ចំនួនគត់ copyID)
{
    tuple_T	*tt;
    ចំនួនគត់		did_free = FALSE;

    សម្រាប់ (tt = first_tuple; tt != NULL; tt = tt->tv_used_next)
	ប្រសិនបើ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ត្រឡប់ did_free;
}

    static void
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    ប្រសិនបើ (tuple->tv_used_prev == NULL)
	first_tuple = tuple->tv_used_next;
    បើមិនដូច្នេះ
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    ប្រសិនបើ (tuple->tv_used_next != NULL)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    void
tuple_free_items(ចំនួនគត់ copyID)
{
    tuple_T	*tt, *tt_next;

    សម្រាប់ (tt = first_tuple; tt != NULL; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	ប្រសិនបើ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
    ប្រសិនបើ (in_free_unref_items)
	ត្រឡប់;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់ 0L;
    ត្រឡប់ tuple->tv_items.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    ចំនួនគត់
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    ចំនួនគត់		ic)	// ignore case for strings
{
    ប្រសិនបើ (t1 == t2)
	ត្រឡប់ TRUE;

    ចំនួនគត់ t1_len = tuple_len(t1);
    ចំនួនគត់ t2_len = tuple_len(t2);

    ប្រសិនបើ (t1_len != t2_len)
	ត្រឡប់ FALSE;

    ប្រសិនបើ (t1_len == 0)
	// empty and NULL tuples are considered equal
	ត្រឡប់ TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    សម្រាប់ (ចំនួនគត់ i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	ប្រសិនបើ (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    ត្រឡប់ FALSE;

    ត្រឡប់ TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់ NULL;

    // Negative index is relative to the end.
    ប្រសិនបើ (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    ប្រសិនបើ (n < 0 || n >= TUPLE_LEN(tuple))
	ត្រឡប់ NULL;

    ត្រឡប់ TUPLE_ITEM(tuple, n);
}

    ចំនួនគត់
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    ប្រសិនបើ (ga_grow_id(&tuple->tv_items, 1, aid_tuple_append) == FAIL)
	ត្រឡប់ FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    ត្រឡប់ OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    ចំនួនគត់
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    ប្រសិនបើ (t1 == NULL)
	tuple = tuple_alloc();
    បើមិនដូច្នេះ
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់ FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    ប្រសិនបើ (t1 == NULL)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    សម្រាប់ (ចំនួនគត់ i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	ប្រសិនបើ (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ត្រឡប់ FAIL;
	}
    }

    ត្រឡប់ OK;
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
    ប្រសិនបើ (new_tuple == NULL)
	ត្រឡប់ NULL;

    សម្រាប់ (ចំនួនគត់ i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	ប្រសិនបើ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ត្រឡប់ NULL;
	}
    }

    ត្រឡប់ new_tuple;
}

    ចំនួនគត់
tuple_slice_or_index(
    tuple_T	*tuple,
    ចំនួនគត់		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    ចំនួនគត់		exclusive,
    typval_T	*rettv,
    ចំនួនគត់		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ប្រសិនបើ (n1 < 0)
	n1 = len + n1;
    ប្រសិនបើ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	ប្រសិនបើ (!range)
	{
	    ប្រសិនបើ (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    ត្រឡប់ FAIL;
	}
	ប្រសិនបើ (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	បើមិនដូច្នេះ
	    n1 = len;
    }
    ប្រសិនបើ (range)
    {
	tuple_T	*new_tuple;

	ប្រសិនបើ (n2 < 0)
	    n2 = len + n2;
	បើមិនដូច្នេះ ប្រសិនបើ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ប្រសិនបើ (exclusive)
	    --n2;
	ប្រសិនបើ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	ប្រសិនបើ (new_tuple == NULL)
	    ត្រឡប់ FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    បើមិនដូច្នេះ
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ត្រឡប់ OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, ចំនួនគត់ deep, ចំនួនគត់ top, ចំនួនគត់ copyID)
{
    tuple_T	*copy;
    ចំនួនគត់		idx;

    ប្រសិនបើ (orig == NULL)
	ត្រឡប់ NULL;

    copy = tuple_alloc_with_items(TUPLE_LEN(orig));
    ប្រសិនបើ (copy == NULL)
	ត្រឡប់ NULL;

    ប្រសិនបើ (orig->tv_type == NULL || top || deep)
	copy->tv_type = NULL;
    បើមិនដូច្នេះ
	copy->tv_type = alloc_type(orig->tv_type);
    ប្រសិនបើ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    សម្រាប់ (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_items.ga_len++;
	ប្រសិនបើ (deep)
	{
	    ប្រសិនបើ (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		bha_nga;
	}
	បើមិនដូច្នេះ
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    ប្រសិនបើ (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = NULL;
    }

    ត្រឡប់ copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    ចំនួនគត់
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ចំនួនគត់ do_error)
{
    ចំនួនគត់		evaluate = evalarg == NULL ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = NULL;
    typval_T	tv;
    ចំនួនគត់		vim9script = in_vim9script();
    ចំនួនគត់		had_comma;

    ប្រសិនបើ (check_typval_is_value(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ត្រឡប់ FAIL;
    }

    ប្រសិនបើ (evaluate)
    {
	tuple = tuple_alloc();
	ប្រសិនបើ (tuple == NULL)
	    ត្រឡប់ FAIL;

	ប្រសិនបើ (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    ប្រសិនបើ (tuple_append_tv(tuple, rettv) == FAIL)
		goto failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    ប្រសិនបើ (**arg == ')')
	// empty tuple
	goto done;

    ប្រសិនបើ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	goto failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ខណៈពេល (**arg != ')' && **arg != NUL)
    {
	ប្រសិនបើ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    goto failret;
	ប្រសិនបើ (check_typval_is_value(&tv) == FAIL)
	{
	    ប្រសិនបើ (evaluate)
		clear_tv(&tv);
	    goto failret;
	}

	ប្រសិនបើ (evaluate)
	{
	    ប្រសិនបើ (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		goto failret;
	    }
	}

	ប្រសិនបើ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ប្រសិនបើ (had_comma)
	{
	    ប្រសិនបើ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		goto failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ប្រសិនបើ (**arg == ')')
	    bha_nga;

	ប្រសិនបើ (!had_comma)
	{
	    ប្រសិនបើ (do_error)
	    {
		ប្រសិនបើ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		បើមិនដូច្នេះ
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    goto failret;
	}
    }

    ប្រសិនបើ (**arg != ')')
    {
	ប្រសិនបើ (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	ប្រសិនបើ (evaluate)
	    tuple_free(tuple);
	ត្រឡប់ FAIL;
    }

done:
    *arg += 1;
    ប្រសិនបើ (evaluate)
	rettv_tuple_set(rettv, tuple);

    ត្រឡប់ OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    void
tuple_lock(tuple_T *tuple, ចំនួនគត់ deep, ចំនួនគត់ lock, ចំនួនគត់ check_refcount)
{
    ប្រសិនបើ (tuple == NULL || (check_refcount && tuple->tv_refcount > 1))
	ត្រឡប់;

    ប្រសិនបើ (lock)
	tuple->tv_lock |= VAR_LOCKED;
    បើមិនដូច្នេះ
	tuple->tv_lock &= ~VAR_LOCKED;

    ប្រសិនបើ (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	សម្រាប់ (ចំនួនគត់ i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

typedef sa_nracanaa join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    static ចំនួនគត់
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    ចំនួនគត់		echo_style,
    ចំនួនគត់		restore_copyID,
    ចំនួនគត់		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    ចំនួនគត់		i;
    join_T	*p;
    ចំនួនគត់		sumlen = 0;
    ចំនួនគត់		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    សម្រាប់ (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ប្រសិនបើ (s.string == NULL)
	    ត្រឡប់ FAIL;

	s.length = STRLEN(s.string);
	sumlen += (ចំនួនគត់)s.length;

	(void)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	ប្រសិនបើ (tofree != NULL || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	បើមិនដូច្នេះ
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	line_breakcheck();
	ប្រសិនបើ (did_echo_string_emsg)  // recursion error, bail out
	    bha_nga;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    ប្រសិនបើ (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	ប្រសិនបើ (join_gap->ga_len >= 2)
	    sumlen += (ចំនួនគត់)seplen * (join_gap->ga_len - 1);
    }
    ប្រសិនបើ (ga_grow(gap, sumlen + 2) == FAIL)
	ត្រឡប់ FAIL;

    សម្រាប់ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ប្រសិនបើ (first)
	    first = FALSE;
	បើមិនដូច្នេះ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	ប្រសិនបើ (p->s.string != NULL)
	    ga_concat_len(gap, p->s.string, p->s.length);
	line_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    ប្រសិនបើ (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ត្រឡប់ OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    ចំនួនគត់
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    ចំនួនគត់		echo_style,
    ចំនួនគត់		restore_copyID,
    ចំនួនគត់		copyID)
{
    garray_T	join_ga;
    ចំនួនគត់		retval;
    join_T	*p;
    ចំនួនគត់		i;

    ប្រសិនបើ (TUPLE_LEN(tuple) < 1)
	ត្រឡប់ OK; // nothing to do
    ga_init2(&join_ga, parimaa_na(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ប្រសិនបើ (join_ga.ga_data == NULL)
	ត្រឡប់ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    សម្រាប់ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ត្រឡប់ retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, ចំនួនគត់ copyID, ចំនួនគត់ restore_copyID)
{
    garray_T	ga;

    ប្រសិនបើ (tv->vval.v_tuple == NULL)
	ត្រឡប់ NULL;
    ga_init2(&ga, parimaa_na(var_na), 80);
    ga_append(&ga, '(');
    ប្រសិនបើ (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ត្រឡប់ NULL;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ត្រឡប់ (char_u *)ga.ga_data;
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
    ចំនួនគត់		len = tuple_len(tuple);
    ចំនួនគត់		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    សម្រាប់ (ចំនួនគត់ idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	ប្រសិនបើ (filter_map_one(TUPLE_ITEM(tuple, idx), expr, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    bha_nga;
    }

    ប្រសិនបើ (fc != NULL)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_count(tuple_T *tuple, typval_T *needle, long idx, ចំនួនគត់ ic)
{
    long	n = 0;

    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់ 0;

    ចំនួនគត់	len = TUPLE_LEN(tuple);
    ប្រសិនបើ (len == 0)
	ត្រឡប់ 0;

    ប្រសិនបើ (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	ត្រឡប់ 0;
    }

    សម្រាប់ (ចំនួនគត់ i = idx; i < len; i++)
    {
	ប្រសិនបើ (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    ត្រឡប់ n;
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

    ប្រសិនបើ (rettv_list_alloc(rettv) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់;  // null tuple behaves like an empty list

    សម្រាប់ (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	ប្រសិនបើ (l == NULL)
	    bha_nga;

	ប្រសិនបើ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    bha_nga;
	}
	ប្រសិនបើ (list_append_number(l, idx) == FAIL
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
    ចំនួនគត់
index_tuple(tuple_T *tuple, typval_T *tv, ចំនួនគត់ start_idx, ចំនួនគត់ ic)
{
    ប្រសិនបើ (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	ប្រសិនបើ (start_idx < 0)
	    start_idx = 0;
    }

    សម្រាប់ (ចំនួនគត់ idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	ប្រសិនបើ (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    ត្រឡប់ idx;
    }

    ត្រឡប់ -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ចំនួនគត់
indexof_tuple(tuple_T *tuple, long startidx, typval_T *expr)
{
    long	idx = 0;
    ចំនួនគត់		len;
    ចំនួនគត់		found;

    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់ -1;

    len = TUPLE_LEN(tuple);

    ប្រសិនបើ (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	ប្រសិនបើ (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_KEY, VAR_NUMBER);

    ចំនួនគត់		called_emsg_start = called_emsg;

    សម្រាប់ (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_expr(expr);
	clear_tv(get_vim_var_tv(VV_VAL));

	ប្រសិនបើ (found)
	    ត្រឡប់ idx;

	ប្រសិនបើ (called_emsg != called_emsg_start)
	    ត្រឡប់ -1;
    }

    ត្រឡប់ -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, ចំនួនគត់ domax)
{
    typval_T	*tv = NULL;
    ចំនួនគត់		res;

    ប្រសិនបើ (tuple == NULL || TUPLE_LEN(tuple) == 0)
	ត្រឡប់ NULL;

    សម្រាប់ (ចំនួនគត់ idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	ប្រសិនបើ (tv == NULL)
	    tv = TUPLE_ITEM(tuple, idx);
	បើមិនដូច្នេះ
	{
	    ប្រសិនបើ (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? EXPR_GREATER : EXPR_SMALLER, FALSE, &res) == FAIL)
		ត្រឡប់ NULL;
	    ប្រសិនបើ (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    ត្រឡប់ tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    void
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = NULL;

    ប្រសិនបើ (tuple == NULL || TUPLE_LEN(tuple) == 0 || n <= 0)
	ត្រឡប់;

    ប្រសិនបើ (check_repeat_count(TUPLE_LEN(tuple), n) == FAIL)
	ត្រឡប់;

    ប្រសិនបើ (rettv_tuple_set_with_items(rettv, TUPLE_LEN(tuple) * (ចំនួនគត់)n) == FAIL)
	ត្រឡប់;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    សម្រាប់ (ចំនួនគត់ count = 0; count < (ចំនួនគត់)n; count++)
    {
	សម្រាប់ (ចំនួនគត់ idx = 0; idx < TUPLE_LEN(tuple); idx++)
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

    ចំនួនគត់	len = tuple_len(tuple);

    ប្រសិនបើ (len == 0)
	ត្រឡប់;

    ប្រសិនបើ (rettv_tuple_set_with_items(rettv, len) == FAIL)
	ត្រឡប់;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    សម្រាប់ (ចំនួនគត់ i = 0; i < len; i++)
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
    ចំនួនគត់		called_emsg_start = called_emsg;
    typval_T	initial;
    ចំនួនគត់		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    ចំនួនគត់		r;

    ប្រសិនបើ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ប្រសិនបើ (tuple == NULL || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Tuple");
	    ត្រឡប់;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    បើមិនដូច្នេះ
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    ប្រសិនបើ (tuple == NULL)
	ត្រឡប់;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    សម្រាប់ ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	ប្រសិនបើ (r == FAIL || called_emsg != called_emsg_start)
	    bha_nga;
    }

    ប្រសិនបើ (fc != NULL)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    ចំនួនគត់
check_tuples_addable(type_T *type1, type_T *type2)
{
    ចំនួនគត់	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    ប្រសិនបើ ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    ប្រសិនបើ ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	ប្រសិនបើ (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	បើមិនដូច្នេះ ប្រសិនបើ ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    ប្រសិនបើ (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	ត្រឡប់ FAIL;
    }

    ត្រឡប់ OK;
}

#endif // defined(FEAT_EVAL)
