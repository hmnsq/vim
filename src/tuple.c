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

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_EVAL)

// Tuple heads for garbage collection.
സ്ഥിതം tuple_T		*first_tuple = ശൂന്യ;	// list of all tuples

    സ്ഥിതം ശൂന്യം
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    എങ്കിൽ (first_tuple != ശൂന്യ)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = ശൂന്യ;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_ഇനങ്ങൾ, വലുപ്പം(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(ശൂന്യം)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    എങ്കിൽ (tuple != ശൂന്യ)
	tuple_init(tuple);
    തിരികെ tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ എണ്ണം)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (എണ്ണം <= 0)
	തിരികെ tuple;

    എങ്കിൽ (ga_grow(&tuple->tv_ഇനങ്ങൾ, എണ്ണം) == FAIL)
    {
	tuple_free(tuple);
	തിരികെ ശൂന്യ;
    }

    തിരികെ tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    ശൂന്യം
tuple_set_item(tuple_T *tuple, പൂർണ്ണസംഖ്യ idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_ഇനങ്ങൾ.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    തിരികെ OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    ശൂന്യം
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    എങ്കിൽ (tuple != ശൂന്യ)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    പൂർണ്ണസംഖ്യ
rettv_tuple_set_with_ഇനങ്ങൾ(typval_T *rettv, പൂർണ്ണസംഖ്യ എണ്ണം)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_ഇനങ്ങൾ(എണ്ണം);
    എങ്കിൽ (new_tuple == ശൂന്യ)
	തിരികെ FAIL;

    rettv_tuple_set(rettv, new_tuple);

    തിരികെ OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    ശൂന്യം
tuple_unref(tuple_T *tuple)
{
    എങ്കിൽ (tuple != ശൂന്യ && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    സ്ഥിതം ശൂന്യം
tuple_free_contents(tuple_T *tuple)
{
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_ഇനങ്ങൾ);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    പൂർണ്ണസംഖ്യ
tuple_free_nonref(പൂർണ്ണസംഖ്യ copyID)
{
    tuple_T	*tt;
    പൂർണ്ണസംഖ്യ		did_free = FALSE;

    വേണ്ടി (tt = first_tuple; tt != ശൂന്യ; tt = tt->tv_used_next)
	എങ്കിൽ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    തിരികെ did_free;
}

    സ്ഥിതം ശൂന്യം
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    എങ്കിൽ (tuple->tv_used_prev == ശൂന്യ)
	first_tuple = tuple->tv_used_next;
    അല്ലെങ്കിൽ
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    എങ്കിൽ (tuple->tv_used_next != ശൂന്യ)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    ശൂന്യം
tuple_free_ഇനങ്ങൾ(പൂർണ്ണസംഖ്യ copyID)
{
    tuple_T	*tt, *tt_next;

    വേണ്ടി (tt = first_tuple; tt != ശൂന്യ; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	എങ്കിൽ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    ശൂന്യം
tuple_free(tuple_T *tuple)
{
    എങ്കിൽ (in_free_unref_ഇനങ്ങൾ)
	തിരികെ;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ 0L;
    തിരികെ tuple->tv_ഇനങ്ങൾ.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    പൂർണ്ണസംഖ്യ
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    പൂർണ്ണസംഖ്യ		ic)	// ignore case for strings
{
    എങ്കിൽ (t1 == t2)
	തിരികെ TRUE;

    പൂർണ്ണസംഖ്യ t1_len = tuple_len(t1);
    പൂർണ്ണസംഖ്യ t2_len = tuple_len(t2);

    എങ്കിൽ (t1_len != t2_len)
	തിരികെ FALSE;

    എങ്കിൽ (t1_len == 0)
	// empty and NULL tuples are considered equal
	തിരികെ TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	എങ്കിൽ (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    തിരികെ FALSE;

    തിരികെ TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ ശൂന്യ;

    // Negative index is relative to the end.
    എങ്കിൽ (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    എങ്കിൽ (n < 0 || n >= TUPLE_LEN(tuple))
	തിരികെ ശൂന്യ;

    തിരികെ TUPLE_ITEM(tuple, n);
}

    പൂർണ്ണസംഖ്യ
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    എങ്കിൽ (ga_grow_id(&tuple->tv_ഇനങ്ങൾ, 1, aid_tuple_append) == FAIL)
	തിരികെ FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    തിരികെ OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    പൂർണ്ണസംഖ്യ
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    എങ്കിൽ (t1 == ശൂന്യ)
	tuple = tuple_alloc();
    അല്ലെങ്കിൽ
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    എങ്കിൽ (t1 == ശൂന്യ)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	എങ്കിൽ (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    തിരികെ FAIL;
	}
    }

    തിരികെ OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_ഇനങ്ങൾ(n2 - n1 + 1);
    എങ്കിൽ (new_tuple == ശൂന്യ)
	തിരികെ ശൂന്യ;

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	എങ്കിൽ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    തിരികെ ശൂന്യ;
	}
    }

    തിരികെ new_tuple;
}

    പൂർണ്ണസംഖ്യ
tuple_slice_or_index(
    tuple_T	*tuple,
    പൂർണ്ണസംഖ്യ		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    പൂർണ്ണസംഖ്യ		exclusive,
    typval_T	*rettv,
    പൂർണ്ണസംഖ്യ		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    എങ്കിൽ (n1 < 0)
	n1 = len + n1;
    എങ്കിൽ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	എങ്കിൽ (!range)
	{
	    എങ്കിൽ (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    തിരികെ FAIL;
	}
	എങ്കിൽ (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	അല്ലെങ്കിൽ
	    n1 = len;
    }
    എങ്കിൽ (range)
    {
	tuple_T	*new_tuple;

	എങ്കിൽ (n2 < 0)
	    n2 = len + n2;
	അല്ലെങ്കിൽ എങ്കിൽ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	എങ്കിൽ (exclusive)
	    --n2;
	എങ്കിൽ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	എങ്കിൽ (new_tuple == ശൂന്യ)
	    തിരികെ FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    അല്ലെങ്കിൽ
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    തിരികെ OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, പൂർണ്ണസംഖ്യ deep, പൂർണ്ണസംഖ്യ മുകൾ, പൂർണ്ണസംഖ്യ copyID)
{
    tuple_T	*copy;
    പൂർണ്ണസംഖ്യ		idx;

    എങ്കിൽ (orig == ശൂന്യ)
	തിരികെ ശൂന്യ;

    copy = tuple_alloc_with_ഇനങ്ങൾ(TUPLE_LEN(orig));
    എങ്കിൽ (copy == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (orig->tv_type == ശൂന്യ || മുകൾ || deep)
	copy->tv_type = ശൂന്യ;
    അല്ലെങ്കിൽ
	copy->tv_type = alloc_type(orig->tv_type);
    എങ്കിൽ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    വേണ്ടി (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_ഇനങ്ങൾ.ga_len++;
	എങ്കിൽ (deep)
	{
	    എങ്കിൽ (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    എങ്കിൽ (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = ശൂന്യ;
    }

    തിരികെ copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, പൂർണ്ണസംഖ്യ do_error)
{
    പൂർണ്ണസംഖ്യ		evaluate = evalarg == ശൂന്യ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = ശൂന്യ;
    typval_T	tv;
    പൂർണ്ണസംഖ്യ		vim9script = in_vim9script();
    പൂർണ്ണസംഖ്യ		had_comma;

    എങ്കിൽ (check_typval_is_മൂല്യം(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	തിരികെ FAIL;
    }

    എങ്കിൽ (evaluate)
    {
	tuple = tuple_alloc();
	എങ്കിൽ (tuple == ശൂന്യ)
	    തിരികെ FAIL;

	എങ്കിൽ (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    എങ്കിൽ (tuple_append_tv(tuple, rettv) == FAIL)
		പോകുക failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    എങ്കിൽ (**arg == ')')
	// empty tuple
	പോകുക done;

    എങ്കിൽ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	പോകുക failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ആകുമ്പോൾ (**arg != ')' && **arg != NUL)
    {
	എങ്കിൽ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    പോകുക failret;
	എങ്കിൽ (check_typval_is_മൂല്യം(&tv) == FAIL)
	{
	    എങ്കിൽ (evaluate)
		clear_tv(&tv);
	    പോകുക failret;
	}

	എങ്കിൽ (evaluate)
	{
	    എങ്കിൽ (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		പോകുക failret;
	    }
	}

	എങ്കിൽ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	എങ്കിൽ (had_comma)
	{
	    എങ്കിൽ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		പോകുക failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	എങ്കിൽ (**arg == ')')
	    നിർത്തുക;

	എങ്കിൽ (!had_comma)
	{
	    എങ്കിൽ (do_error)
	    {
		എങ്കിൽ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		അല്ലെങ്കിൽ
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    പോകുക failret;
	}
    }

    എങ്കിൽ (**arg != ')')
    {
	എങ്കിൽ (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	എങ്കിൽ (evaluate)
	    tuple_free(tuple);
	തിരികെ FAIL;
    }

done:
    *arg += 1;
    എങ്കിൽ (evaluate)
	rettv_tuple_set(rettv, tuple);

    തിരികെ OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    ശൂന്യം
tuple_lock(tuple_T *tuple, പൂർണ്ണസംഖ്യ deep, പൂർണ്ണസംഖ്യ lock, പൂർണ്ണസംഖ്യ check_refcount)
{
    എങ്കിൽ (tuple == ശൂന്യ || (check_refcount && tuple->tv_refcount > 1))
	തിരികെ;

    എങ്കിൽ (lock)
	tuple->tv_lock |= VAR_LOCKED;
    അല്ലെങ്കിൽ
	tuple->tv_lock &= ~VAR_LOCKED;

    എങ്കിൽ (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

തരനിർവചനം ഘടന join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    പൂർണ്ണസംഖ്യ		echo_style,
    പൂർണ്ണസംഖ്യ		restore_copyID,
    പൂർണ്ണസംഖ്യ		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    പൂർണ്ണസംഖ്യ		i;
    join_T	*p;
    പൂർണ്ണസംഖ്യ		sumlen = 0;
    പൂർണ്ണസംഖ്യ		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    വേണ്ടി (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	എങ്കിൽ (s.string == ശൂന്യ)
	    തിരികെ FAIL;

	s.length = STRLEN(s.string);
	sumlen += (പൂർണ്ണസംഖ്യ)s.length;

	(ശൂന്യം)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	എങ്കിൽ (tofree != ശൂന്യ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	അല്ലെങ്കിൽ
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	വരി_breakcheck();
	എങ്കിൽ (did_echo_string_emsg)  // recursion error, bail out
	    നിർത്തുക;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    എങ്കിൽ (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	എങ്കിൽ (join_gap->ga_len >= 2)
	    sumlen += (പൂർണ്ണസംഖ്യ)seplen * (join_gap->ga_len - 1);
    }
    എങ്കിൽ (ga_grow(gap, sumlen + 2) == FAIL)
	തിരികെ FAIL;

    വേണ്ടി (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	എങ്കിൽ (first)
	    first = FALSE;
	അല്ലെങ്കിൽ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	എങ്കിൽ (p->s.string != ശൂന്യ)
	    ga_concat_len(gap, p->s.string, p->s.length);
	വരി_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    എങ്കിൽ (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    തിരികെ OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    പൂർണ്ണസംഖ്യ
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    പൂർണ്ണസംഖ്യ		echo_style,
    പൂർണ്ണസംഖ്യ		restore_copyID,
    പൂർണ്ണസംഖ്യ		copyID)
{
    garray_T	join_ga;
    പൂർണ്ണസംഖ്യ		retval;
    join_T	*p;
    പൂർണ്ണസംഖ്യ		i;

    എങ്കിൽ (TUPLE_LEN(tuple) < 1)
	തിരികെ OK; // nothing to do
    ga_init2(&join_ga, വലുപ്പം(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    എങ്കിൽ (join_ga.ga_data == ശൂന്യ)
	തിരികെ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    വേണ്ടി (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    തിരികെ retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, പൂർണ്ണസംഖ്യ copyID, പൂർണ്ണസംഖ്യ restore_copyID)
{
    garray_T	ga;

    എങ്കിൽ (tv->vval.v_tuple == ശൂന്യ)
	തിരികെ ശൂന്യ;
    ga_init2(&ga, വലുപ്പം(അക്ഷരം), 80);
    ga_append(&ga, '(');
    എങ്കിൽ (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	തിരികെ ശൂന്യ;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    തിരികെ (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    ശൂന്യം
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*വ്യഞ്ജകം)
{
    പൂർണ്ണസംഖ്യ		len = tuple_len(tuple);
    പൂർണ്ണസംഖ്യ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_താക്കോൽ, VAR_സംഖ്യ);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_വ്യഞ്ജകം_get_funccal(വ്യഞ്ജകം, &newtv);

    വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_താക്കോൽ, idx);
	എങ്കിൽ (filter_map_one(TUPLE_ITEM(tuple, idx), വ്യഞ്ജകം, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    നിർത്തുക;
    }

    എങ്കിൽ (fc != ശൂന്യ)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_എണ്ണം(tuple_T *tuple, typval_T *needle, long idx, പൂർണ്ണസംഖ്യ ic)
{
    long	n = 0;

    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ 0;

    പൂർണ്ണസംഖ്യ	len = TUPLE_LEN(tuple);
    എങ്കിൽ (len == 0)
	തിരികെ 0;

    എങ്കിൽ (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	തിരികെ 0;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = idx; i < len; i++)
    {
	എങ്കിൽ (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    തിരികെ n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    ശൂന്യം
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    എങ്കിൽ (rettv_list_alloc(rettv) == FAIL)
	തിരികെ;

    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ;  // null tuple behaves like an empty list

    വേണ്ടി (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	എങ്കിൽ (l == ശൂന്യ)
	    നിർത്തുക;

	എങ്കിൽ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    നിർത്തുക;
	}
	എങ്കിൽ (list_append_സംഖ്യ(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    നിർത്തുക;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    പൂർണ്ണസംഖ്യ
index_tuple(tuple_T *tuple, typval_T *tv, പൂർണ്ണസംഖ്യ start_idx, പൂർണ്ണസംഖ്യ ic)
{
    എങ്കിൽ (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	എങ്കിൽ (start_idx < 0)
	    start_idx = 0;
    }

    വേണ്ടി (പൂർണ്ണസംഖ്യ idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	എങ്കിൽ (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    തിരികെ idx;
    }

    തിരികെ -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    പൂർണ്ണസംഖ്യ
indexof_tuple(tuple_T *tuple, long startidx, typval_T *വ്യഞ്ജകം)
{
    long	idx = 0;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		found;

    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ -1;

    len = TUPLE_LEN(tuple);

    എങ്കിൽ (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	എങ്കിൽ (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_താക്കോൽ, VAR_സംഖ്യ);

    പൂർണ്ണസംഖ്യ		called_emsg_start = called_emsg;

    വേണ്ടി (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_താക്കോൽ, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_വ്യഞ്ജകം(വ്യഞ്ജകം);
	clear_tv(get_vim_var_tv(VV_VAL));

	എങ്കിൽ (found)
	    തിരികെ idx;

	എങ്കിൽ (called_emsg != called_emsg_start)
	    തിരികെ -1;
    }

    തിരികെ -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, പൂർണ്ണസംഖ്യ domax)
{
    typval_T	*tv = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		res;

    എങ്കിൽ (tuple == ശൂന്യ || TUPLE_LEN(tuple) == 0)
	തിരികെ ശൂന്യ;

    വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	എങ്കിൽ (tv == ശൂന്യ)
	    tv = TUPLE_ITEM(tuple, idx);
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? വ്യഞ്ജകം_GREATER : വ്യഞ്ജകം_SMALLER, FALSE, &res) == FAIL)
		തിരികെ ശൂന്യ;
	    എങ്കിൽ (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    തിരികെ tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    ശൂന്യം
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ശൂന്യ;

    എങ്കിൽ (tuple == ശൂന്യ || TUPLE_LEN(tuple) == 0 || n <= 0)
	തിരികെ;

    എങ്കിൽ (check_repeat_എണ്ണം(TUPLE_LEN(tuple), n) == FAIL)
	തിരികെ;

    എങ്കിൽ (rettv_tuple_set_with_ഇനങ്ങൾ(rettv, TUPLE_LEN(tuple) * (പൂർണ്ണസംഖ്യ)n) == FAIL)
	തിരികെ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    വേണ്ടി (പൂർണ്ണസംഖ്യ എണ്ണം = 0; എണ്ണം < (പൂർണ്ണസംഖ്യ)n; എണ്ണം++)
    {
	വേണ്ടി (പൂർണ്ണസംഖ്യ idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_ഇനങ്ങൾ.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    ശൂന്യം
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ശൂന്യ;

    പൂർണ്ണസംഖ്യ	len = tuple_len(tuple);

    എങ്കിൽ (len == 0)
	തിരികെ;

    എങ്കിൽ (rettv_tuple_set_with_ഇനങ്ങൾ(rettv, len) == FAIL)
	തിരികെ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_ഇനങ്ങൾ.ga_len = tuple->tv_ഇനങ്ങൾ.ga_len;
}

/*
 * Tuple reduce() function
 */
    ശൂന്യം
tuple_reduce(typval_T *argvars, typval_T *വ്യഞ്ജകം, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    പൂർണ്ണസംഖ്യ		called_emsg_start = called_emsg;
    typval_T	initial;
    പൂർണ്ണസംഖ്യ		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    പൂർണ്ണസംഖ്യ		r;

    എങ്കിൽ (argvars[2].v_type == VAR_UNKNOWN)
    {
	എങ്കിൽ (tuple == ശൂന്യ || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_മൂല്യം), "Tuple");
	    തിരികെ;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    അല്ലെങ്കിൽ
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    എങ്കിൽ (tuple == ശൂന്യ)
	തിരികെ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_വ്യഞ്ജകം_get_funccal(വ്യഞ്ജകം, rettv);

    വേണ്ടി ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_വ്യഞ്ജകം_typval(വ്യഞ്ജകം, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	എങ്കിൽ (r == FAIL || called_emsg != called_emsg_start)
	    നിർത്തുക;
    }

    എങ്കിൽ (fc != ശൂന്യ)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    പൂർണ്ണസംഖ്യ
check_tuples_addable(type_T *type1, type_T *type2)
{
    പൂർണ്ണസംഖ്യ	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    എങ്കിൽ ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    എങ്കിൽ ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	എങ്കിൽ (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	അല്ലെങ്കിൽ എങ്കിൽ ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    എങ്കിൽ (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

#endif // defined(FEAT_EVAL)
