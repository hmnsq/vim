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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

// Tuple heads for garbage collection.
ਸਥਿਰ tuple_T		*ਪਹਿਲਾ_tuple = ਨਿਰਾ;	// list of all tuples

    ਸਥਿਰ ਖਾਲੀ
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    ਜੇ (ਪਹਿਲਾ_tuple != ਨਿਰਾ)
	ਪਹਿਲਾ_tuple->tv_ਵਰਤਿਆ_prev = tuple;
    tuple->tv_ਵਰਤਿਆ_prev = ਨਿਰਾ;
    tuple->tv_ਵਰਤਿਆ_ਅਗਲਾ = ਪਹਿਲਾ_tuple;
    ਪਹਿਲਾ_tuple = tuple;

    ga_init2(&tuple->tv_ਚੀਜ਼ਾਂ, ਆਕਾਰ(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(ਖਾਲੀ)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    ਜੇ (tuple != ਨਿਰਾ)
	tuple_init(tuple);
    ਵਾਪਸ tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_ਚੀਜ਼ਾਂ(ਪੂਰਨ ਗਿਣਤੀ_2)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (ਗਿਣਤੀ_2 <= 0)
	ਵਾਪਸ tuple;

    ਜੇ (ga_grow(&tuple->tv_ਚੀਜ਼ਾਂ, ਗਿਣਤੀ_2) == FAIL)
    {
	tuple_free(tuple);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    ਖਾਲੀ
tuple_ਸੈੱਟ_ਵਸਤੂ(tuple_T *tuple, ਪੂਰਨ idx, typval_T *tv)
{
    *TUPLE_ਵਸਤੂ(tuple, idx) = *tv;
    tuple->tv_ਚੀਜ਼ਾਂ.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    rettv->v_ਤਾਲਾ = 0;
    rettv_tuple_ਸੈੱਟ(rettv, tuple);
    ਵਾਪਸ OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    ਖਾਲੀ
rettv_tuple_ਸੈੱਟ(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    ਜੇ (tuple != ਨਿਰਾ)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    ਪੂਰਨ
rettv_tuple_ਸੈੱਟ_with_ਚੀਜ਼ਾਂ(typval_T *rettv, ਪੂਰਨ ਗਿਣਤੀ_2)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_ਚੀਜ਼ਾਂ(ਗਿਣਤੀ_2);
    ਜੇ (new_tuple == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    rettv_tuple_ਸੈੱਟ(rettv, new_tuple);

    ਵਾਪਸ OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    ਖਾਲੀ
tuple_unref(tuple_T *tuple)
{
    ਜੇ (tuple != ਨਿਰਾ && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    ਸਥਿਰ ਖਾਲੀ
tuple_free_contents(tuple_T *tuple)
{
    ਲਈ (ਪੂਰਨ i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ਵਸਤੂ(tuple, i));

    ga_clear(&tuple->tv_ਚੀਜ਼ਾਂ);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ਪੂਰਨ
tuple_free_nonref(ਪੂਰਨ copyID)
{
    tuple_T	*tt;
    ਪੂਰਨ		did_free = FALSE;

    ਲਈ (tt = ਪਹਿਲਾ_tuple; tt != ਨਿਰਾ; tt = tt->tv_ਵਰਤਿਆ_ਅਗਲਾ)
	ਜੇ ((tt->tv_copyID & COPYID_ਨਕਾਬ) != (copyID & COPYID_ਨਕਾਬ))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ਵਾਪਸ did_free;
}

    ਸਥਿਰ ਖਾਲੀ
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    ਜੇ (tuple->tv_ਵਰਤਿਆ_prev == ਨਿਰਾ)
	ਪਹਿਲਾ_tuple = tuple->tv_ਵਰਤਿਆ_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	tuple->tv_ਵਰਤਿਆ_prev->tv_ਵਰਤਿਆ_ਅਗਲਾ = tuple->tv_ਵਰਤਿਆ_ਅਗਲਾ;
    ਜੇ (tuple->tv_ਵਰਤਿਆ_ਅਗਲਾ != ਨਿਰਾ)
	tuple->tv_ਵਰਤਿਆ_ਅਗਲਾ->tv_ਵਰਤਿਆ_prev = tuple->tv_ਵਰਤਿਆ_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    ਖਾਲੀ
tuple_free_ਚੀਜ਼ਾਂ(ਪੂਰਨ copyID)
{
    tuple_T	*tt, *tt_ਅਗਲਾ;

    ਲਈ (tt = ਪਹਿਲਾ_tuple; tt != ਨਿਰਾ; tt = tt_ਅਗਲਾ)
    {
	tt_ਅਗਲਾ = tt->tv_ਵਰਤਿਆ_ਅਗਲਾ;
	ਜੇ ((tt->tv_copyID & COPYID_ਨਕਾਬ) != (copyID & COPYID_ਨਕਾਬ))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    ਖਾਲੀ
tuple_free(tuple_T *tuple)
{
    ਜੇ (ਵਿੱਚ_free_unref_ਚੀਜ਼ਾਂ)
	ਵਾਪਸ;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    ਲੰਮਾ
tuple_len(tuple_T *tuple)
{
    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ 0L;
    ਵਾਪਸ tuple->tv_ਚੀਜ਼ਾਂ.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    ਪੂਰਨ
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    ਪੂਰਨ		ic)	// ignore case for strings
{
    ਜੇ (t1 == t2)
	ਵਾਪਸ TRUE;

    ਪੂਰਨ t1_len = tuple_len(t1);
    ਪੂਰਨ t2_len = tuple_len(t2);

    ਜੇ (t1_len != t2_len)
	ਵਾਪਸ FALSE;

    ਜੇ (t1_len == 0)
	// empty and NULL tuples are considered equal
	ਵਾਪਸ TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    ਲਈ (ਪੂਰਨ i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	ਜੇ (!tv_equal(TUPLE_ਵਸਤੂ(t1, i), TUPLE_ਵਸਤੂ(t2, j), ic))
	    ਵਾਪਸ FALSE;

    ਵਾਪਸ TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_ਲੱਭੋ(tuple_T *tuple, ਲੰਮਾ n)
{
    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Negative index is relative to the end.
    ਜੇ (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    ਜੇ (n < 0 || n >= TUPLE_LEN(tuple))
	ਵਾਪਸ ਨਿਰਾ;

    ਵਾਪਸ TUPLE_ਵਸਤੂ(tuple, n);
}

    ਪੂਰਨ
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    ਜੇ (ga_grow_id(&tuple->tv_ਚੀਜ਼ਾਂ, 1, aid_tuple_append) == FAIL)
	ਵਾਪਸ FAIL;

    tuple_ਸੈੱਟ_ਵਸਤੂ(tuple, TUPLE_LEN(tuple), tv);

    ਵਾਪਸ OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    ਪੂਰਨ
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    ਜੇ (t1 == ਨਿਰਾ)
	tuple = tuple_alloc();
    ਨਹੀਂਤਾਂ
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_ਤਾਲਾ = 0;
    tv->vval.v_tuple = tuple;
    ਜੇ (t1 == ਨਿਰਾ)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    ਲਈ (ਪੂਰਨ i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ਵਸਤੂ(t2, i), &new_tv);

	ਜੇ (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ਵਾਪਸ FAIL;
	}
    }

    ਵਾਪਸ OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_ਟੁਕੜਾ(tuple_T *tuple, ਲੰਮਾ n1, ਲੰਮਾ n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_ਚੀਜ਼ਾਂ(n2 - n1 + 1);
    ਜੇ (new_tuple == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (ਪੂਰਨ i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ਵਸਤੂ(tuple, i), &new_tv);

	ਜੇ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }

    ਵਾਪਸ new_tuple;
}

    ਪੂਰਨ
tuple_ਟੁਕੜਾ_or_ਸੂਚਕਾਂਕ(
    tuple_T	*tuple,
    ਪੂਰਨ		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    ਪੂਰਨ		exclusive,
    typval_T	*rettv,
    ਪੂਰਨ		verbose)
{
    ਲੰਮਾ	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ਜੇ (n1 < 0)
	n1 = len + n1;
    ਜੇ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	ਜੇ (!range)
	{
	    ਜੇ (verbose)
		semsg(_(e_tuple_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), (ਲੰਮਾ)n1_arg);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (ਵਿੱਚ_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ਨਹੀਂਤਾਂ
	    n1 = len;
    }
    ਜੇ (range)
    {
	tuple_T	*new_tuple;

	ਜੇ (n2 < 0)
	    n2 = len + n2;
	ਨਹੀਂਤਾਂ ਜੇ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ਜੇ (exclusive)
	    --n2;
	ਜੇ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_ਟੁਕੜਾ(tuple, n1, n2);
	ਜੇ (new_tuple == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	clear_tv(rettv);
	rettv_tuple_ਸੈੱਟ(rettv, new_tuple);
    }
    ਨਹੀਂਤਾਂ
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_ਲੱਭੋ(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ਵਾਪਸ OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, ਪੂਰਨ deep, ਪੂਰਨ ਸਿਖਰ, ਪੂਰਨ copyID)
{
    tuple_T	*copy;
    ਪੂਰਨ		idx;

    ਜੇ (orig == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    copy = tuple_alloc_with_ਚੀਜ਼ਾਂ(TUPLE_LEN(orig));
    ਜੇ (copy == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (orig->tv_type == ਨਿਰਾ || ਸਿਖਰ || deep)
	copy->tv_type = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	copy->tv_type = alloc_type(orig->tv_type);
    ਜੇ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    ਲਈ (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_ਚੀਜ਼ਾਂ.ga_len++;
	ਜੇ (deep)
	{
	    ਜੇ (ਵਸਤੂ_copy(TUPLE_ਵਸਤੂ(orig, idx), TUPLE_ਵਸਤੂ(copy, idx),
						deep, FALSE, copyID) == FAIL)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	    copy_tv(TUPLE_ਵਸਤੂ(orig, idx), TUPLE_ਵਸਤੂ(copy, idx));
    }

    ++copy->tv_refcount;
    ਜੇ (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = ਨਿਰਾ;
    }

    ਵਾਪਸ copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ਪੂਰਨ do_error)
{
    ਪੂਰਨ		evaluate = evalarg == ਨਿਰਾ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = ਨਿਰਾ;
    typval_T	tv;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ		had_comma;

    ਜੇ (check_typval_is_ਮੁੱਲ(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ਵਾਪਸ FAIL;
    }

    ਜੇ (evaluate)
    {
	tuple = tuple_alloc();
	ਜੇ (tuple == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	ਜੇ (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    ਜੇ (tuple_append_tv(tuple, rettv) == FAIL)
		ਜਾਓ failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    ਜੇ (**arg == ')')
	// empty tuple
	ਜਾਓ ਪੂਰਾ;

    ਜੇ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	ਜਾਓ failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ਜਦੋਂ (**arg != ')' && **arg != NUL)
    {
	ਜੇ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    ਜਾਓ failret;
	ਜੇ (check_typval_is_ਮੁੱਲ(&tv) == FAIL)
	{
	    ਜੇ (evaluate)
		clear_tv(&tv);
	    ਜਾਓ failret;
	}

	ਜੇ (evaluate)
	{
	    ਜੇ (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		ਜਾਓ failret;
	    }
	}

	ਜੇ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ਜੇ (had_comma)
	{
	    ਜੇ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ਜਾਓ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ਜੇ (**arg == ')')
	    ਰੋਕੋ;

	ਜੇ (!had_comma)
	{
	    ਜੇ (do_error)
	    {
		ਜੇ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ਨਹੀਂਤਾਂ
		    semsg(_(e_missing_comma_ਵਿੱਚ_tuple_str), *arg);
	    }
	    ਜਾਓ failret;
	}
    }

    ਜੇ (**arg != ')')
    {
	ਜੇ (do_error)
	    semsg(_(e_missing_ਸਿਰਾ_ਦਾ_tuple_rsp_str), *arg);
failret:
	ਜੇ (evaluate)
	    tuple_free(tuple);
	ਵਾਪਸ FAIL;
    }

ਪੂਰਾ:
    *arg += 1;
    ਜੇ (evaluate)
	rettv_tuple_ਸੈੱਟ(rettv, tuple);

    ਵਾਪਸ OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    ਖਾਲੀ
tuple_ਤਾਲਾ(tuple_T *tuple, ਪੂਰਨ deep, ਪੂਰਨ ਤਾਲਾ, ਪੂਰਨ check_refcount)
{
    ਜੇ (tuple == ਨਿਰਾ || (check_refcount && tuple->tv_refcount > 1))
	ਵਾਪਸ;

    ਜੇ (ਤਾਲਾ)
	tuple->tv_ਤਾਲਾ |= VAR_LOCKED;
    ਨਹੀਂਤਾਂ
	tuple->tv_ਤਾਲਾ &= ~VAR_LOCKED;

    ਜੇ (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	ਲਈ (ਪੂਰਨ i = 0; i < TUPLE_LEN(tuple); i++)
	    ਵਸਤੂ_ਤਾਲਾ(TUPLE_ਵਸਤੂ(tuple, i), deep - 1, ਤਾਲਾ, check_refcount);
    }
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ਸਥਿਰ ਪੂਰਨ
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    ਪੂਰਨ		echo_style,
    ਪੂਰਨ		restore_copyID,
    ਪੂਰਨ		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    ਪੂਰਨ		i;
    join_T	*p;
    ਪੂਰਨ		sumlen = 0;
    ਪੂਰਨ		ਪਹਿਲਾ = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    ਲਈ (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ਵਸਤੂ(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ਜੇ (s.string == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;

	s.ਲੰਮਾਈ = STRLEN(s.string);
	sumlen += (ਪੂਰਨ)s.ਲੰਮਾਈ;

	(ਖਾਲੀ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_ਅੰਕੜੇ) + (join_gap->ga_len++);
	ਜੇ (tofree != ਨਿਰਾ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.ਲੰਮਾਈ = s.ਲੰਮਾਈ;
	    p->tofree = tofree;
	}
	ਨਹੀਂਤਾਂ
	{
	    p->s.string = vim_strnsave(s.string, s.ਲੰਮਾਈ);
	    p->s.ਲੰਮਾਈ = s.ਲੰਮਾਈ;
	    p->tofree = p->s.string;
	}

	ਸਤਰ_breakcheck();
	ਜੇ (did_echo_string_emsg)  // recursion error, bail out
	    ਰੋਕੋ;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    ਜੇ (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	ਜੇ (join_gap->ga_len >= 2)
	    sumlen += (ਪੂਰਨ)seplen * (join_gap->ga_len - 1);
    }
    ਜੇ (ga_grow(gap, sumlen + 2) == FAIL)
	ਵਾਪਸ FAIL;

    ਲਈ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ਜੇ (ਪਹਿਲਾ)
	    ਪਹਿਲਾ = FALSE;
	ਨਹੀਂਤਾਂ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_ਅੰਕੜੇ) + i;

	ਜੇ (p->s.string != ਨਿਰਾ)
	    ga_concat_len(gap, p->s.string, p->s.ਲੰਮਾਈ);
	ਸਤਰ_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    ਜੇ (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ਵਾਪਸ OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    ਪੂਰਨ
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    ਪੂਰਨ		echo_style,
    ਪੂਰਨ		restore_copyID,
    ਪੂਰਨ		copyID)
{
    garray_T	join_ga;
    ਪੂਰਨ		retval;
    join_T	*p;
    ਪੂਰਨ		i;

    ਜੇ (TUPLE_LEN(tuple) < 1)
	ਵਾਪਸ OK; // nothing to do
    ga_init2(&join_ga, ਆਕਾਰ(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ਜੇ (join_ga.ga_ਅੰਕੜੇ == ਨਿਰਾ)
	ਵਾਪਸ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ਵਾਪਸ retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, ਪੂਰਨ copyID, ਪੂਰਨ restore_copyID)
{
    garray_T	ga;

    ਜੇ (tv->vval.v_tuple == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 80);
    ga_append(&ga, '(');
    ਜੇ (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_ਅੰਕੜੇ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ਵਾਪਸ (char_u *)ga.ga_ਅੰਕੜੇ;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    ਖਾਲੀ
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*ਵਿਅੰਜਕ)
{
    ਪੂਰਨ		len = tuple_len(tuple);
    ਪੂਰਨ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    ਸੈੱਟ_vim_var_type(VV_ਕੁੰਜੀ, VAR_ਸੰਖਿਆ);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ਵਿਅੰਜਕ_get_funccal(ਵਿਅੰਜਕ, &newtv);

    ਲਈ (ਪੂਰਨ idx = 0; idx < len; idx++)
    {
	ਸੈੱਟ_vim_var_nr(VV_ਕੁੰਜੀ, idx);
	ਜੇ (filter_map_one(TUPLE_ਵਸਤੂ(tuple, idx), ਵਿਅੰਜਕ, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    ਰੋਕੋ;
    }

    ਜੇ (fc != ਨਿਰਾ)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ਲੰਮਾ
tuple_ਗਿਣਤੀ(tuple_T *tuple, typval_T *needle, ਲੰਮਾ idx, ਪੂਰਨ ic)
{
    ਲੰਮਾ	n = 0;

    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ 0;

    ਪੂਰਨ	len = TUPLE_LEN(tuple);
    ਜੇ (len == 0)
	ਵਾਪਸ 0;

    ਜੇ (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), idx);
	ਵਾਪਸ 0;
    }

    ਲਈ (ਪੂਰਨ i = idx; i < len; i++)
    {
	ਜੇ (tv_equal(TUPLE_ਵਸਤੂ(tuple, i), needle, ic))
	    ++n;
    }

    ਵਾਪਸ n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    ਖਾਲੀ
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ;  // null tuple behaves like an empty list

    ਲਈ (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	ਜੇ (l == ਨਿਰਾ)
	    ਰੋਕੋ;

	ਜੇ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ਰੋਕੋ;
	}
	ਜੇ (list_append_ਸੰਖਿਆ(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ਵਸਤੂ(tuple, idx)) == FAIL)
	    ਰੋਕੋ;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    ਪੂਰਨ
ਸੂਚਕਾਂਕ_tuple(tuple_T *tuple, typval_T *tv, ਪੂਰਨ ਸ਼ੁਰੂ_idx, ਪੂਰਨ ic)
{
    ਜੇ (ਸ਼ੁਰੂ_idx < 0)
    {
	ਸ਼ੁਰੂ_idx = TUPLE_LEN(tuple) + ਸ਼ੁਰੂ_idx;
	ਜੇ (ਸ਼ੁਰੂ_idx < 0)
	    ਸ਼ੁਰੂ_idx = 0;
    }

    ਲਈ (ਪੂਰਨ idx = ਸ਼ੁਰੂ_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	ਜੇ (tv_equal(TUPLE_ਵਸਤੂ(tuple, idx), tv, ic))
	    ਵਾਪਸ idx;
    }

    ਵਾਪਸ -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ਪੂਰਨ
indexof_tuple(tuple_T *tuple, ਲੰਮਾ startidx, typval_T *ਵਿਅੰਜਕ)
{
    ਲੰਮਾ	idx = 0;
    ਪੂਰਨ		len;
    ਪੂਰਨ		ਮਿਲਿਆ;

    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ -1;

    len = TUPLE_LEN(tuple);

    ਜੇ (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	ਜੇ (startidx < 0)
	    startidx = 0;
    }

    ਸੈੱਟ_vim_var_type(VV_ਕੁੰਜੀ, VAR_ਸੰਖਿਆ);

    ਪੂਰਨ		called_emsg_ਸ਼ੁਰੂ = called_emsg;

    ਲਈ (idx = startidx; idx < len; idx++)
    {
	ਸੈੱਟ_vim_var_nr(VV_ਕੁੰਜੀ, idx);
	copy_tv(TUPLE_ਵਸਤੂ(tuple, idx), get_vim_var_tv(VV_VAL));

	ਮਿਲਿਆ = indexof_eval_ਵਿਅੰਜਕ(ਵਿਅੰਜਕ);
	clear_tv(get_vim_var_tv(VV_VAL));

	ਜੇ (ਮਿਲਿਆ)
	    ਵਾਪਸ idx;

	ਜੇ (called_emsg != called_emsg_ਸ਼ੁਰੂ)
	    ਵਾਪਸ -1;
    }

    ਵਾਪਸ -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, ਪੂਰਨ domax)
{
    typval_T	*tv = ਨਿਰਾ;
    ਪੂਰਨ		res;

    ਜੇ (tuple == ਨਿਰਾ || TUPLE_LEN(tuple) == 0)
	ਵਾਪਸ ਨਿਰਾ;

    ਲਈ (ਪੂਰਨ idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	ਜੇ (tv == ਨਿਰਾ)
	    tv = TUPLE_ਵਸਤੂ(tuple, idx);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (typval_compare2(TUPLE_ਵਸਤੂ(tuple, idx), tv,
		    domax ? ਵਿਅੰਜਕ_GREATER : ਵਿਅੰਜਕ_SMALLER, FALSE, &res) == FAIL)
		ਵਾਪਸ ਨਿਰਾ;
	    ਜੇ (res == OK)
		tv = TUPLE_ਵਸਤੂ(tuple, idx);
	}
    }

    ਵਾਪਸ tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    ਖਾਲੀ
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ਨਿਰਾ;

    ਜੇ (tuple == ਨਿਰਾ || TUPLE_LEN(tuple) == 0 || n <= 0)
	ਵਾਪਸ;

    ਜੇ (check_repeat_ਗਿਣਤੀ(TUPLE_LEN(tuple), n) == FAIL)
	ਵਾਪਸ;

    ਜੇ (rettv_tuple_ਸੈੱਟ_with_ਚੀਜ਼ਾਂ(rettv, TUPLE_LEN(tuple) * (ਪੂਰਨ)n) == FAIL)
	ਵਾਪਸ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ਲਈ (ਪੂਰਨ ਗਿਣਤੀ_2 = 0; ਗਿਣਤੀ_2 < (ਪੂਰਨ)n; ਗਿਣਤੀ_2++)
    {
	ਲਈ (ਪੂਰਨ idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ਵਸਤੂ(tuple, idx),
		    TUPLE_ਵਸਤੂ(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_ਚੀਜ਼ਾਂ.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    ਖਾਲੀ
tuple_ਉਲਟ(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ਨਿਰਾ;

    ਪੂਰਨ	len = tuple_len(tuple);

    ਜੇ (len == 0)
	ਵਾਪਸ;

    ਜੇ (rettv_tuple_ਸੈੱਟ_with_ਚੀਜ਼ਾਂ(rettv, len) == FAIL)
	ਵਾਪਸ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ਲਈ (ਪੂਰਨ i = 0; i < len; i++)
	copy_tv(TUPLE_ਵਸਤੂ(tuple, i), TUPLE_ਵਸਤੂ(new_tuple, len - i - 1));
    new_tuple->tv_ਚੀਜ਼ਾਂ.ga_len = tuple->tv_ਚੀਜ਼ਾਂ.ga_len;
}

/*
 * Tuple reduce() function
 */
    ਖਾਲੀ
tuple_reduce(typval_T *argvars, typval_T *ਵਿਅੰਜਕ, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    ਪੂਰਨ		called_emsg_ਸ਼ੁਰੂ = called_emsg;
    typval_T	initial;
    ਪੂਰਨ		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    ਪੂਰਨ		r;

    ਜੇ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ਜੇ (tuple == ਨਿਰਾ || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_ਦਾ_an_ਖਾਲੀ_ਥਾਵਾਂ_str_with_no_initial_ਮੁੱਲ), "Tuple");
	    ਵਾਪਸ;
	}
	initial = *TUPLE_ਵਸਤੂ(tuple, 0);
	idx = 1;
    }
    ਨਹੀਂਤਾਂ
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ਵਿਅੰਜਕ_get_funccal(ਵਿਅੰਜਕ, rettv);

    ਲਈ ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ਵਸਤੂ(tuple, idx);

	r = eval_ਵਿਅੰਜਕ_typval(ਵਿਅੰਜਕ, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	ਜੇ (r == FAIL || called_emsg != called_emsg_ਸ਼ੁਰੂ)
	    ਰੋਕੋ;
    }

    ਜੇ (fc != ਨਿਰਾ)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    ਪੂਰਨ
check_tuples_addable(type_T *type1, type_T *type2)
{
    ਪੂਰਨ	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    ਜੇ ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    ਜੇ ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	ਜੇ (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	ਨਹੀਂਤਾਂ ਜੇ ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_ਦਲੀਲਾਂ[0], type2->tt_ਦਲੀਲਾਂ[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    ਜੇ (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_ਵਿੱਚ_concatenation));
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

#ਜੇ_ਅੰਤ // defined(FEAT_EVAL)
