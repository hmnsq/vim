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

#ꦪꦼꦤ꧀ defined(FEAT_EVAL)

// Tuple heads for garbage collection.
static tuple_T		*first_tuple = NULL;	// list of all tuples

    static void
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    ꦪꦼꦤ꧀ (first_tuple != NULL)
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
    ꦪꦼꦤ꧀ (tuple != NULL)
	tuple_init(tuple);
    ꦮꦭꦶ tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_items(puur_naa_nka count)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    ꦪꦼꦤ꧀ (tuple == NULL)
	ꦮꦭꦶ NULL;

    ꦪꦼꦤ꧀ (count <= 0)
	ꦮꦭꦶ tuple;

    ꦪꦼꦤ꧀ (ga_grow(&tuple->tv_items, count) == FAIL)
    {
	tuple_free(tuple);
	ꦮꦭꦶ NULL;
    }

    ꦮꦭꦶ tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    void
tuple_set_item(tuple_T *tuple, puur_naa_nka idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_items.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    puur_naa_nka
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    ꦪꦼꦤ꧀ (tuple == NULL)
	ꦮꦭꦶ FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    ꦮꦭꦶ OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    void
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    ꦪꦼꦤ꧀ (tuple != NULL)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    puur_naa_nka
rettv_tuple_set_with_items(typval_T *rettv, puur_naa_nka count)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_items(count);
    ꦪꦼꦤ꧀ (new_tuple == NULL)
	ꦮꦭꦶ FAIL;

    rettv_tuple_set(rettv, new_tuple);

    ꦮꦭꦶ OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    void
tuple_unref(tuple_T *tuple)
{
    ꦪꦼꦤ꧀ (tuple != NULL && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    static void
tuple_free_contents(tuple_T *tuple)
{
    ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_items);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    puur_naa_nka
tuple_free_nonref(puur_naa_nka copyID)
{
    tuple_T	*tt;
    puur_naa_nka		did_free = FALSE;

    ꦏꦁꦒꦺꦴ (tt = first_tuple; tt != NULL; tt = tt->tv_used_next)
	ꦪꦼꦤ꧀ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ꦮꦭꦶ did_free;
}

    static void
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    ꦪꦼꦤ꧀ (tuple->tv_used_prev == NULL)
	first_tuple = tuple->tv_used_next;
    ꦭꦶꦪ
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    ꦪꦼꦤ꧀ (tuple->tv_used_next != NULL)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    void
tuple_free_items(puur_naa_nka copyID)
{
    tuple_T	*tt, *tt_next;

    ꦏꦁꦒꦺꦴ (tt = first_tuple; tt != NULL; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	ꦪꦼꦤ꧀ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
    ꦪꦼꦤ꧀ (in_free_unref_items)
	ꦮꦭꦶ;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    ꦪꦼꦤ꧀ (tuple == NULL)
	ꦮꦭꦶ 0L;
    ꦮꦭꦶ tuple->tv_items.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    puur_naa_nka
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    puur_naa_nka		ic)	// ignore case for strings
{
    ꦪꦼꦤ꧀ (t1 == t2)
	ꦮꦭꦶ TRUE;

    puur_naa_nka t1_len = tuple_len(t1);
    puur_naa_nka t2_len = tuple_len(t2);

    ꦪꦼꦤ꧀ (t1_len != t2_len)
	ꦮꦭꦶ FALSE;

    ꦪꦼꦤ꧀ (t1_len == 0)
	// empty and NULL tuples are considered equal
	ꦮꦭꦶ TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	ꦪꦼꦤ꧀ (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    ꦮꦭꦶ FALSE;

    ꦮꦭꦶ TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    ꦪꦼꦤ꧀ (tuple == NULL)
	ꦮꦭꦶ NULL;

    // Negative index is relative to the end.
    ꦪꦼꦤ꧀ (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    ꦪꦼꦤ꧀ (n < 0 || n >= TUPLE_LEN(tuple))
	ꦮꦭꦶ NULL;

    ꦮꦭꦶ TUPLE_ITEM(tuple, n);
}

    puur_naa_nka
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    ꦪꦼꦤ꧀ (ga_grow_id(&tuple->tv_items, 1, aid_tuple_append) == FAIL)
	ꦮꦭꦶ FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    ꦮꦭꦶ OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    puur_naa_nka
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    ꦪꦼꦤ꧀ (t1 == NULL)
	tuple = tuple_alloc();
    ꦭꦶꦪ
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    ꦪꦼꦤ꧀ (tuple == NULL)
	ꦮꦭꦶ FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    ꦪꦼꦤ꧀ (t1 == NULL)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	ꦪꦼꦤ꧀ (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ꦮꦭꦶ FAIL;
	}
    }

    ꦮꦭꦶ OK;
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
    ꦪꦼꦤ꧀ (new_tuple == NULL)
	ꦮꦭꦶ NULL;

    ꦏꦁꦒꦺꦴ (puur_naa_nka i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	ꦪꦼꦤ꧀ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ꦮꦭꦶ NULL;
	}
    }

    ꦮꦭꦶ new_tuple;
}

    puur_naa_nka
tuple_slice_or_index(
    tuple_T	*tuple,
    puur_naa_nka		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    puur_naa_nka		exclusive,
    typval_T	*rettv,
    puur_naa_nka		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ꦪꦼꦤ꧀ (n1 < 0)
	n1 = len + n1;
    ꦪꦼꦤ꧀ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	ꦪꦼꦤ꧀ (!range)
	{
	    ꦪꦼꦤ꧀ (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    ꦮꦭꦶ FAIL;
	}
	ꦪꦼꦤ꧀ (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ꦭꦶꦪ
	    n1 = len;
    }
    ꦪꦼꦤ꧀ (range)
    {
	tuple_T	*new_tuple;

	ꦪꦼꦤ꧀ (n2 < 0)
	    n2 = len + n2;
	ꦭꦶꦪ ꦪꦼꦤ꧀ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ꦪꦼꦤ꧀ (exclusive)
	    --n2;
	ꦪꦼꦤ꧀ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	ꦪꦼꦤ꧀ (new_tuple == NULL)
	    ꦮꦭꦶ FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    ꦭꦶꦪ
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ꦮꦭꦶ OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, puur_naa_nka deep, puur_naa_nka top, puur_naa_nka copyID)
{
    tuple_T	*copy;
    puur_naa_nka		idx;

    ꦪꦼꦤ꧀ (orig == NULL)
	ꦮꦭꦶ NULL;

    copy = tuple_alloc_with_items(TUPLE_LEN(orig));
    ꦪꦼꦤ꧀ (copy == NULL)
	ꦮꦭꦶ NULL;

    ꦪꦼꦤ꧀ (orig->tv_type == NULL || top || deep)
	copy->tv_type = NULL;
    ꦭꦶꦪ
	copy->tv_type = alloc_type(orig->tv_type);
    ꦪꦼꦤ꧀ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    ꦏꦁꦒꦺꦴ (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_items.ga_len++;
	ꦪꦼꦤ꧀ (deep)
	{
	    ꦪꦼꦤ꧀ (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		bha_nga;
	}
	ꦭꦶꦪ
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    ꦪꦼꦤ꧀ (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = NULL;
    }

    ꦮꦭꦶ copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    puur_naa_nka
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, puur_naa_nka do_error)
{
    puur_naa_nka		evaluate = evalarg == NULL ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = NULL;
    typval_T	tv;
    puur_naa_nka		vim9script = in_vim9script();
    puur_naa_nka		had_comma;

    ꦪꦼꦤ꧀ (check_typval_is_value(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ꦮꦭꦶ FAIL;
    }

    ꦪꦼꦤ꧀ (evaluate)
    {
	tuple = tuple_alloc();
	ꦪꦼꦤ꧀ (tuple == NULL)
	    ꦮꦭꦶ FAIL;

	ꦪꦼꦤ꧀ (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    ꦪꦼꦤ꧀ (tuple_append_tv(tuple, rettv) == FAIL)
		goto failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    ꦪꦼꦤ꧀ (**arg == ')')
	// empty tuple
	goto done;

    ꦪꦼꦤ꧀ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	goto failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ꦱꦼꦢꦼꦭ (**arg != ')' && **arg != NUL)
    {
	ꦪꦼꦤ꧀ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    goto failret;
	ꦪꦼꦤ꧀ (check_typval_is_value(&tv) == FAIL)
	{
	    ꦪꦼꦤ꧀ (evaluate)
		clear_tv(&tv);
	    goto failret;
	}

	ꦪꦼꦤ꧀ (evaluate)
	{
	    ꦪꦼꦤ꧀ (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		goto failret;
	    }
	}

	ꦪꦼꦤ꧀ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ꦪꦼꦤ꧀ (had_comma)
	{
	    ꦪꦼꦤ꧀ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		goto failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ꦪꦼꦤ꧀ (**arg == ')')
	    bha_nga;

	ꦪꦼꦤ꧀ (!had_comma)
	{
	    ꦪꦼꦤ꧀ (do_error)
	    {
		ꦪꦼꦤ꧀ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ꦭꦶꦪ
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    goto failret;
	}
    }

    ꦪꦼꦤ꧀ (**arg != ')')
    {
	ꦪꦼꦤ꧀ (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	ꦪꦼꦤ꧀ (evaluate)
	    tuple_free(tuple);
	ꦮꦭꦶ FAIL;
    }

done:
    *arg += 1;
    ꦪꦼꦤ꧀ (evaluate)
	rettv_tuple_set(rettv, tuple);

    ꦮꦭꦶ OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    void
tuple_lock(tuple_T *tuple, puur_naa_nka deep, puur_naa_nka lock, puur_naa_nka check_refcount)
{
    ꦪꦼꦤ꧀ (tuple == NULL || (check_refcount && tuple->tv_refcount > 1))
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (lock)
	tuple->tv_lock |= VAR_LOCKED;
    ꦭꦶꦪ
	tuple->tv_lock &= ~VAR_LOCKED;

    ꦪꦼꦤ꧀ (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

typedef sa_nracanaa join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    static puur_naa_nka
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    puur_naa_nka		echo_style,
    puur_naa_nka		restore_copyID,
    puur_naa_nka		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    puur_naa_nka		i;
    join_T	*p;
    puur_naa_nka		sumlen = 0;
    puur_naa_nka		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    ꦏꦁꦒꦺꦴ (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ꦪꦼꦤ꧀ (s.string == NULL)
	    ꦮꦭꦶ FAIL;

	s.length = STRLEN(s.string);
	sumlen += (puur_naa_nka)s.length;

	(void)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	ꦪꦼꦤ꧀ (tofree != NULL || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	ꦭꦶꦪ
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	line_breakcheck();
	ꦪꦼꦤ꧀ (did_echo_string_emsg)  // recursion error, bail out
	    bha_nga;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    ꦪꦼꦤ꧀ (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	ꦪꦼꦤ꧀ (join_gap->ga_len >= 2)
	    sumlen += (puur_naa_nka)seplen * (join_gap->ga_len - 1);
    }
    ꦪꦼꦤ꧀ (ga_grow(gap, sumlen + 2) == FAIL)
	ꦮꦭꦶ FAIL;

    ꦏꦁꦒꦺꦴ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ꦪꦼꦤ꧀ (first)
	    first = FALSE;
	ꦭꦶꦪ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	ꦪꦼꦤ꧀ (p->s.string != NULL)
	    ga_concat_len(gap, p->s.string, p->s.length);
	line_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    ꦪꦼꦤ꧀ (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ꦮꦭꦶ OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    puur_naa_nka
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    puur_naa_nka		echo_style,
    puur_naa_nka		restore_copyID,
    puur_naa_nka		copyID)
{
    garray_T	join_ga;
    puur_naa_nka		retval;
    join_T	*p;
    puur_naa_nka		i;

    ꦪꦼꦤ꧀ (TUPLE_LEN(tuple) < 1)
	ꦮꦭꦶ OK; // nothing to do
    ga_init2(&join_ga, parimaa_na(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ꦪꦼꦤ꧀ (join_ga.ga_data == NULL)
	ꦮꦭꦶ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    ꦏꦁꦒꦺꦴ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ꦮꦭꦶ retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, puur_naa_nka copyID, puur_naa_nka restore_copyID)
{
    garray_T	ga;

    ꦪꦼꦤ꧀ (tv->vval.v_tuple == NULL)
	ꦮꦭꦶ NULL;
    ga_init2(&ga, parimaa_na(var_na), 80);
    ga_append(&ga, '(');
    ꦪꦼꦤ꧀ (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ꦮꦭꦶ NULL;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ꦮꦭꦶ (char_u *)ga.ga_data;
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
    puur_naa_nka		len = tuple_len(tuple);
    puur_naa_nka		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    ꦏꦁꦒꦺꦴ (puur_naa_nka idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	ꦪꦼꦤ꧀ (filter_map_one(TUPLE_ITEM(tuple, idx), expr, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    bha_nga;
    }

    ꦪꦼꦤ꧀ (fc != NULL)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_count(tuple_T *tuple, typval_T *needle, long idx, puur_naa_nka ic)
{
    long	n = 0;

    ꦪꦼꦤ꧀ (tuple == NULL)
	ꦮꦭꦶ 0;

    puur_naa_nka	len = TUPLE_LEN(tuple);
    ꦪꦼꦤ꧀ (len == 0)
	ꦮꦭꦶ 0;

    ꦪꦼꦤ꧀ (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	ꦮꦭꦶ 0;
    }

    ꦏꦁꦒꦺꦴ (puur_naa_nka i = idx; i < len; i++)
    {
	ꦪꦼꦤ꧀ (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    ꦮꦭꦶ n;
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

    ꦪꦼꦤ꧀ (rettv_list_alloc(rettv) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (tuple == NULL)
	ꦮꦭꦶ;  // null tuple behaves like an empty list

    ꦏꦁꦒꦺꦴ (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	ꦪꦼꦤ꧀ (l == NULL)
	    bha_nga;

	ꦪꦼꦤ꧀ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    bha_nga;
	}
	ꦪꦼꦤ꧀ (list_append_number(l, idx) == FAIL
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
    puur_naa_nka
index_tuple(tuple_T *tuple, typval_T *tv, puur_naa_nka start_idx, puur_naa_nka ic)
{
    ꦪꦼꦤ꧀ (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	ꦪꦼꦤ꧀ (start_idx < 0)
	    start_idx = 0;
    }

    ꦏꦁꦒꦺꦴ (puur_naa_nka idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	ꦪꦼꦤ꧀ (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    ꦮꦭꦶ idx;
    }

    ꦮꦭꦶ -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    puur_naa_nka
indexof_tuple(tuple_T *tuple, long startidx, typval_T *expr)
{
    long	idx = 0;
    puur_naa_nka		len;
    puur_naa_nka		found;

    ꦪꦼꦤ꧀ (tuple == NULL)
	ꦮꦭꦶ -1;

    len = TUPLE_LEN(tuple);

    ꦪꦼꦤ꧀ (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	ꦪꦼꦤ꧀ (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_KEY, VAR_NUMBER);

    puur_naa_nka		called_emsg_start = called_emsg;

    ꦏꦁꦒꦺꦴ (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_expr(expr);
	clear_tv(get_vim_var_tv(VV_VAL));

	ꦪꦼꦤ꧀ (found)
	    ꦮꦭꦶ idx;

	ꦪꦼꦤ꧀ (called_emsg != called_emsg_start)
	    ꦮꦭꦶ -1;
    }

    ꦮꦭꦶ -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, puur_naa_nka domax)
{
    typval_T	*tv = NULL;
    puur_naa_nka		res;

    ꦪꦼꦤ꧀ (tuple == NULL || TUPLE_LEN(tuple) == 0)
	ꦮꦭꦶ NULL;

    ꦏꦁꦒꦺꦴ (puur_naa_nka idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	ꦪꦼꦤ꧀ (tv == NULL)
	    tv = TUPLE_ITEM(tuple, idx);
	ꦭꦶꦪ
	{
	    ꦪꦼꦤ꧀ (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? EXPR_GREATER : EXPR_SMALLER, FALSE, &res) == FAIL)
		ꦮꦭꦶ NULL;
	    ꦪꦼꦤ꧀ (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    ꦮꦭꦶ tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    void
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = NULL;

    ꦪꦼꦤ꧀ (tuple == NULL || TUPLE_LEN(tuple) == 0 || n <= 0)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (check_repeat_count(TUPLE_LEN(tuple), n) == FAIL)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (rettv_tuple_set_with_items(rettv, TUPLE_LEN(tuple) * (puur_naa_nka)n) == FAIL)
	ꦮꦭꦶ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ꦏꦁꦒꦺꦴ (puur_naa_nka count = 0; count < (puur_naa_nka)n; count++)
    {
	ꦏꦁꦒꦺꦴ (puur_naa_nka idx = 0; idx < TUPLE_LEN(tuple); idx++)
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

    puur_naa_nka	len = tuple_len(tuple);

    ꦪꦼꦤ꧀ (len == 0)
	ꦮꦭꦶ;

    ꦪꦼꦤ꧀ (rettv_tuple_set_with_items(rettv, len) == FAIL)
	ꦮꦭꦶ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ꦏꦁꦒꦺꦴ (puur_naa_nka i = 0; i < len; i++)
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
    puur_naa_nka		called_emsg_start = called_emsg;
    typval_T	initial;
    puur_naa_nka		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    puur_naa_nka		r;

    ꦪꦼꦤ꧀ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ꦪꦼꦤ꧀ (tuple == NULL || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Tuple");
	    ꦮꦭꦶ;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    ꦭꦶꦪ
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    ꦪꦼꦤ꧀ (tuple == NULL)
	ꦮꦭꦶ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    ꦏꦁꦒꦺꦴ ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	ꦪꦼꦤ꧀ (r == FAIL || called_emsg != called_emsg_start)
	    bha_nga;
    }

    ꦪꦼꦤ꧀ (fc != NULL)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    puur_naa_nka
check_tuples_addable(type_T *type1, type_T *type2)
{
    puur_naa_nka	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    ꦪꦼꦤ꧀ ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    ꦪꦼꦤ꧀ ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	ꦪꦼꦤ꧀ (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	ꦭꦶꦪ ꦪꦼꦤ꧀ ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    ꦪꦼꦤ꧀ (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	ꦮꦭꦶ FAIL;
    }

    ꦮꦭꦶ OK;
}

#endif // defined(FEAT_EVAL)
