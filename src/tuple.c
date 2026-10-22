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

#ضمن "vim.h"

#ܐܢ defined(FEAT_EVAL)

// Tuple heads for garbage collection.
ساكن tuple_T		*first_tuple = عدم;	// list of all tuples

    ساكن فراغ
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    ܐܢ (first_tuple != عدم)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = عدم;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_items, حجم(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(فراغ)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    ܐܢ (tuple != عدم)
	tuple_init(tuple);
    ܦܢܐ tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_items(صحيح count)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    ܐܢ (tuple == عدم)
	ܦܢܐ عدم;

    ܐܢ (count <= 0)
	ܦܢܐ tuple;

    ܐܢ (ga_grow(&tuple->tv_items, count) == FAIL)
    {
	tuple_free(tuple);
	ܦܢܐ عدم;
    }

    ܦܢܐ tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    فراغ
tuple_set_item(tuple_T *tuple, صحيح idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_items.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحيح
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    ܐܢ (tuple == عدم)
	ܦܢܐ FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    ܦܢܐ OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    فراغ
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    ܐܢ (tuple != عدم)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    صحيح
rettv_tuple_set_with_items(typval_T *rettv, صحيح count)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_items(count);
    ܐܢ (new_tuple == عدم)
	ܦܢܐ FAIL;

    rettv_tuple_set(rettv, new_tuple);

    ܦܢܐ OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    فراغ
tuple_unref(tuple_T *tuple)
{
    ܐܢ (tuple != عدم && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    ساكن فراغ
tuple_free_contents(tuple_T *tuple)
{
    لكل (صحيح i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_items);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    صحيح
tuple_free_nonref(صحيح copyID)
{
    tuple_T	*tt;
    صحيح		did_free = FALSE;

    لكل (tt = first_tuple; tt != عدم; tt = tt->tv_used_next)
	ܐܢ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ܦܢܐ did_free;
}

    ساكن فراغ
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    ܐܢ (tuple->tv_used_prev == عدم)
	first_tuple = tuple->tv_used_next;
    ܐܚܪܢܐ
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    ܐܢ (tuple->tv_used_next != عدم)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    فراغ
tuple_free_items(صحيح copyID)
{
    tuple_T	*tt, *tt_next;

    لكل (tt = first_tuple; tt != عدم; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	ܐܢ ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    فراغ
tuple_free(tuple_T *tuple)
{
    ܐܢ (in_free_unref_items)
	ܦܢܐ;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    ܐܢ (tuple == عدم)
	ܦܢܐ 0L;
    ܦܢܐ tuple->tv_items.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    صحيح
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    صحيح		ic)	// ignore case for strings
{
    ܐܢ (t1 == t2)
	ܦܢܐ TRUE;

    صحيح t1_len = tuple_len(t1);
    صحيح t2_len = tuple_len(t2);

    ܐܢ (t1_len != t2_len)
	ܦܢܐ FALSE;

    ܐܢ (t1_len == 0)
	// empty and NULL tuples are considered equal
	ܦܢܐ TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    لكل (صحيح i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	ܐܢ (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    ܦܢܐ FALSE;

    ܦܢܐ TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    ܐܢ (tuple == عدم)
	ܦܢܐ عدم;

    // Negative index is relative to the end.
    ܐܢ (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    ܐܢ (n < 0 || n >= TUPLE_LEN(tuple))
	ܦܢܐ عدم;

    ܦܢܐ TUPLE_ITEM(tuple, n);
}

    صحيح
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    ܐܢ (ga_grow_id(&tuple->tv_items, 1, aid_tuple_append) == FAIL)
	ܦܢܐ FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    ܦܢܐ OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    صحيح
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    ܐܢ (t1 == عدم)
	tuple = tuple_alloc();
    ܐܚܪܢܐ
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    ܐܢ (tuple == عدم)
	ܦܢܐ FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    ܐܢ (t1 == عدم)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    لكل (صحيح i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	ܐܢ (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ܦܢܐ FAIL;
	}
    }

    ܦܢܐ OK;
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
    ܐܢ (new_tuple == عدم)
	ܦܢܐ عدم;

    لكل (صحيح i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	ܐܢ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ܦܢܐ عدم;
	}
    }

    ܦܢܐ new_tuple;
}

    صحيح
tuple_slice_or_index(
    tuple_T	*tuple,
    صحيح		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    صحيح		exclusive,
    typval_T	*rettv,
    صحيح		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ܐܢ (n1 < 0)
	n1 = len + n1;
    ܐܢ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	ܐܢ (!range)
	{
	    ܐܢ (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    ܦܢܐ FAIL;
	}
	ܐܢ (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ܐܚܪܢܐ
	    n1 = len;
    }
    ܐܢ (range)
    {
	tuple_T	*new_tuple;

	ܐܢ (n2 < 0)
	    n2 = len + n2;
	ܐܚܪܢܐ ܐܢ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ܐܢ (exclusive)
	    --n2;
	ܐܢ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	ܐܢ (new_tuple == عدم)
	    ܦܢܐ FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    ܐܚܪܢܐ
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ܦܢܐ OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, صحيح deep, صحيح top, صحيح copyID)
{
    tuple_T	*copy;
    صحيح		idx;

    ܐܢ (orig == عدم)
	ܦܢܐ عدم;

    copy = tuple_alloc_with_items(TUPLE_LEN(orig));
    ܐܢ (copy == عدم)
	ܦܢܐ عدم;

    ܐܢ (orig->tv_type == عدم || top || deep)
	copy->tv_type = عدم;
    ܐܚܪܢܐ
	copy->tv_type = alloc_type(orig->tv_type);
    ܐܢ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    لكل (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_items.ga_len++;
	ܐܢ (deep)
	{
	    ܐܢ (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		اكسر;
	}
	ܐܚܪܢܐ
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    ܐܢ (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = عدم;
    }

    ܦܢܐ copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    صحيح
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحيح do_error)
{
    صحيح		evaluate = evalarg == عدم ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = عدم;
    typval_T	tv;
    صحيح		vim9script = in_vim9script();
    صحيح		had_comma;

    ܐܢ (check_typval_is_value(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ܦܢܐ FAIL;
    }

    ܐܢ (evaluate)
    {
	tuple = tuple_alloc();
	ܐܢ (tuple == عدم)
	    ܦܢܐ FAIL;

	ܐܢ (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    ܐܢ (tuple_append_tv(tuple, rettv) == FAIL)
		اذهب failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    ܐܢ (**arg == ')')
	// empty tuple
	اذهب done;

    ܐܢ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	اذهب failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ܟܡܐ (**arg != ')' && **arg != NUL)
    {
	ܐܢ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    اذهب failret;
	ܐܢ (check_typval_is_value(&tv) == FAIL)
	{
	    ܐܢ (evaluate)
		clear_tv(&tv);
	    اذهب failret;
	}

	ܐܢ (evaluate)
	{
	    ܐܢ (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		اذهب failret;
	    }
	}

	ܐܢ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ܐܢ (had_comma)
	{
	    ܐܢ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		اذهب failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ܐܢ (**arg == ')')
	    اكسر;

	ܐܢ (!had_comma)
	{
	    ܐܢ (do_error)
	    {
		ܐܢ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ܐܚܪܢܐ
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    اذهب failret;
	}
    }

    ܐܢ (**arg != ')')
    {
	ܐܢ (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	ܐܢ (evaluate)
	    tuple_free(tuple);
	ܦܢܐ FAIL;
    }

done:
    *arg += 1;
    ܐܢ (evaluate)
	rettv_tuple_set(rettv, tuple);

    ܦܢܐ OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    فراغ
tuple_lock(tuple_T *tuple, صحيح deep, صحيح lock, صحيح check_refcount)
{
    ܐܢ (tuple == عدم || (check_refcount && tuple->tv_refcount > 1))
	ܦܢܐ;

    ܐܢ (lock)
	tuple->tv_lock |= VAR_LOCKED;
    ܐܚܪܢܐ
	tuple->tv_lock &= ~VAR_LOCKED;

    ܐܢ (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	لكل (صحيح i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

تعريفنوع بنية join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ساكن صحيح
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    صحيح		echo_style,
    صحيح		restore_copyID,
    صحيح		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    صحيح		i;
    join_T	*p;
    صحيح		sumlen = 0;
    صحيح		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    لكل (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ܐܢ (s.string == عدم)
	    ܦܢܐ FAIL;

	s.length = STRLEN(s.string);
	sumlen += (صحيح)s.length;

	(فراغ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	ܐܢ (tofree != عدم || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	ܐܚܪܢܐ
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	line_breakcheck();
	ܐܢ (did_echo_string_emsg)  // recursion error, bail out
	    اكسر;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    ܐܢ (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	ܐܢ (join_gap->ga_len >= 2)
	    sumlen += (صحيح)seplen * (join_gap->ga_len - 1);
    }
    ܐܢ (ga_grow(gap, sumlen + 2) == FAIL)
	ܦܢܐ FAIL;

    لكل (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ܐܢ (first)
	    first = FALSE;
	ܐܚܪܢܐ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	ܐܢ (p->s.string != عدم)
	    ga_concat_len(gap, p->s.string, p->s.length);
	line_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    ܐܢ (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ܦܢܐ OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    صحيح
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    صحيح		echo_style,
    صحيح		restore_copyID,
    صحيح		copyID)
{
    garray_T	join_ga;
    صحيح		retval;
    join_T	*p;
    صحيح		i;

    ܐܢ (TUPLE_LEN(tuple) < 1)
	ܦܢܐ OK; // nothing to do
    ga_init2(&join_ga, حجم(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ܐܢ (join_ga.ga_data == عدم)
	ܦܢܐ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    لكل (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ܦܢܐ retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, صحيح copyID, صحيح restore_copyID)
{
    garray_T	ga;

    ܐܢ (tv->vval.v_tuple == عدم)
	ܦܢܐ عدم;
    ga_init2(&ga, حجم(حرف), 80);
    ga_append(&ga, '(');
    ܐܢ (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ܦܢܐ عدم;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ܦܢܐ (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    فراغ
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*expr)
{
    صحيح		len = tuple_len(tuple);
    صحيح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    لكل (صحيح idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	ܐܢ (filter_map_one(TUPLE_ITEM(tuple, idx), expr, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    اكسر;
    }

    ܐܢ (fc != عدم)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_count(tuple_T *tuple, typval_T *needle, long idx, صحيح ic)
{
    long	n = 0;

    ܐܢ (tuple == عدم)
	ܦܢܐ 0;

    صحيح	len = TUPLE_LEN(tuple);
    ܐܢ (len == 0)
	ܦܢܐ 0;

    ܐܢ (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	ܦܢܐ 0;
    }

    لكل (صحيح i = idx; i < len; i++)
    {
	ܐܢ (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    ܦܢܐ n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    فراغ
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    ܐܢ (rettv_list_alloc(rettv) == FAIL)
	ܦܢܐ;

    ܐܢ (tuple == عدم)
	ܦܢܐ;  // null tuple behaves like an empty list

    لكل (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	ܐܢ (l == عدم)
	    اكسر;

	ܐܢ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اكسر;
	}
	ܐܢ (list_append_number(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    اكسر;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    صحيح
index_tuple(tuple_T *tuple, typval_T *tv, صحيح start_idx, صحيح ic)
{
    ܐܢ (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	ܐܢ (start_idx < 0)
	    start_idx = 0;
    }

    لكل (صحيح idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	ܐܢ (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    ܦܢܐ idx;
    }

    ܦܢܐ -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    صحيح
indexof_tuple(tuple_T *tuple, long startidx, typval_T *expr)
{
    long	idx = 0;
    صحيح		len;
    صحيح		found;

    ܐܢ (tuple == عدم)
	ܦܢܐ -1;

    len = TUPLE_LEN(tuple);

    ܐܢ (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	ܐܢ (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_KEY, VAR_NUMBER);

    صحيح		called_emsg_start = called_emsg;

    لكل (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_expr(expr);
	clear_tv(get_vim_var_tv(VV_VAL));

	ܐܢ (found)
	    ܦܢܐ idx;

	ܐܢ (called_emsg != called_emsg_start)
	    ܦܢܐ -1;
    }

    ܦܢܐ -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, صحيح domax)
{
    typval_T	*tv = عدم;
    صحيح		res;

    ܐܢ (tuple == عدم || TUPLE_LEN(tuple) == 0)
	ܦܢܐ عدم;

    لكل (صحيح idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	ܐܢ (tv == عدم)
	    tv = TUPLE_ITEM(tuple, idx);
	ܐܚܪܢܐ
	{
	    ܐܢ (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? EXPR_GREATER : EXPR_SMALLER, FALSE, &res) == FAIL)
		ܦܢܐ عدم;
	    ܐܢ (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    ܦܢܐ tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    فراغ
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = عدم;

    ܐܢ (tuple == عدم || TUPLE_LEN(tuple) == 0 || n <= 0)
	ܦܢܐ;

    ܐܢ (check_repeat_count(TUPLE_LEN(tuple), n) == FAIL)
	ܦܢܐ;

    ܐܢ (rettv_tuple_set_with_items(rettv, TUPLE_LEN(tuple) * (صحيح)n) == FAIL)
	ܦܢܐ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    لكل (صحيح count = 0; count < (صحيح)n; count++)
    {
	لكل (صحيح idx = 0; idx < TUPLE_LEN(tuple); idx++)
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
    فراغ
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = عدم;

    صحيح	len = tuple_len(tuple);

    ܐܢ (len == 0)
	ܦܢܐ;

    ܐܢ (rettv_tuple_set_with_items(rettv, len) == FAIL)
	ܦܢܐ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    لكل (صحيح i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_items.ga_len = tuple->tv_items.ga_len;
}

/*
 * Tuple reduce() function
 */
    فراغ
tuple_reduce(typval_T *argvars, typval_T *expr, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    صحيح		called_emsg_start = called_emsg;
    typval_T	initial;
    صحيح		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    صحيح		r;

    ܐܢ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ܐܢ (tuple == عدم || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Tuple");
	    ܦܢܐ;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    ܐܚܪܢܐ
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    ܐܢ (tuple == عدم)
	ܦܢܐ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    لكل ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	ܐܢ (r == FAIL || called_emsg != called_emsg_start)
	    اكسر;
    }

    ܐܢ (fc != عدم)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    صحيح
check_tuples_addable(type_T *type1, type_T *type2)
{
    صحيح	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    ܐܢ ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    ܐܢ ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	ܐܢ (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	ܐܚܪܢܐ ܐܢ ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    ܐܢ (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	ܦܢܐ FAIL;
    }

    ܦܢܐ OK;
}

#endif // defined(FEAT_EVAL)
