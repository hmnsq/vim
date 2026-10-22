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

#شامل "vim.h"

#اگر defined(FEAT_EVAL)

// Tuple heads for garbage collection.
ایستا tuple_T		*first_tuple = پوچ;	// list of all tuples

    ایستا تهی
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    اگر (first_tuple != پوچ)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = پوچ;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_اقلام, اندازه(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(تهی)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    اگر (tuple != پوچ)
	tuple_init(tuple);
    بازگشت tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_اقلام(صحیح شمار)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    اگر (tuple == پوچ)
	بازگشت پوچ;

    اگر (شمار <= 0)
	بازگشت tuple;

    اگر (ga_grow(&tuple->tv_اقلام, شمار) == FAIL)
    {
	tuple_free(tuple);
	بازگشت پوچ;
    }

    بازگشت tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    تهی
tuple_set_item(tuple_T *tuple, صحیح idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_اقلام.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحیح
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    اگر (tuple == پوچ)
	بازگشت FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    بازگشت OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    تهی
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    اگر (tuple != پوچ)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    صحیح
rettv_tuple_set_with_اقلام(typval_T *rettv, صحیح شمار)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_اقلام(شمار);
    اگر (new_tuple == پوچ)
	بازگشت FAIL;

    rettv_tuple_set(rettv, new_tuple);

    بازگشت OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    تهی
tuple_unref(tuple_T *tuple)
{
    اگر (tuple != پوچ && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    ایستا تهی
tuple_free_contents(tuple_T *tuple)
{
    برای (صحیح i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_اقلام);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    صحیح
tuple_free_nonref(صحیح copyID)
{
    tuple_T	*tt;
    صحیح		did_free = FALSE;

    برای (tt = first_tuple; tt != پوچ; tt = tt->tv_used_next)
	اگر ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    بازگشت did_free;
}

    ایستا تهی
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    اگر (tuple->tv_used_prev == پوچ)
	first_tuple = tuple->tv_used_next;
    وگرنه
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    اگر (tuple->tv_used_next != پوچ)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    تهی
tuple_free_اقلام(صحیح copyID)
{
    tuple_T	*tt, *tt_next;

    برای (tt = first_tuple; tt != پوچ; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	اگر ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    تهی
tuple_free(tuple_T *tuple)
{
    اگر (in_free_unref_اقلام)
	بازگشت;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    اگر (tuple == پوچ)
	بازگشت 0L;
    بازگشت tuple->tv_اقلام.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    صحیح
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    صحیح		ic)	// ignore case for strings
{
    اگر (t1 == t2)
	بازگشت TRUE;

    صحیح t1_len = tuple_len(t1);
    صحیح t2_len = tuple_len(t2);

    اگر (t1_len != t2_len)
	بازگشت FALSE;

    اگر (t1_len == 0)
	// empty and NULL tuples are considered equal
	بازگشت TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    برای (صحیح i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	اگر (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    بازگشت FALSE;

    بازگشت TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    اگر (tuple == پوچ)
	بازگشت پوچ;

    // Negative index is relative to the end.
    اگر (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    اگر (n < 0 || n >= TUPLE_LEN(tuple))
	بازگشت پوچ;

    بازگشت TUPLE_ITEM(tuple, n);
}

    صحیح
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    اگر (ga_grow_id(&tuple->tv_اقلام, 1, aid_tuple_append) == FAIL)
	بازگشت FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    بازگشت OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    صحیح
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    اگر (t1 == پوچ)
	tuple = tuple_alloc();
    وگرنه
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    اگر (tuple == پوچ)
	بازگشت FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    اگر (t1 == پوچ)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    برای (صحیح i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	اگر (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    بازگشت FAIL;
	}
    }

    بازگشت OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_اقلام(n2 - n1 + 1);
    اگر (new_tuple == پوچ)
	بازگشت پوچ;

    برای (صحیح i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	اگر (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    بازگشت پوچ;
	}
    }

    بازگشت new_tuple;
}

    صحیح
tuple_slice_or_index(
    tuple_T	*tuple,
    صحیح		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    صحیح		exclusive,
    typval_T	*rettv,
    صحیح		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    اگر (n1 < 0)
	n1 = len + n1;
    اگر (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	اگر (!range)
	{
	    اگر (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    بازگشت FAIL;
	}
	اگر (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	وگرنه
	    n1 = len;
    }
    اگر (range)
    {
	tuple_T	*new_tuple;

	اگر (n2 < 0)
	    n2 = len + n2;
	وگرنه اگر (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	اگر (exclusive)
	    --n2;
	اگر (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	اگر (new_tuple == پوچ)
	    بازگشت FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    وگرنه
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    بازگشت OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, صحیح deep, صحیح بالاترین, صحیح copyID)
{
    tuple_T	*copy;
    صحیح		idx;

    اگر (orig == پوچ)
	بازگشت پوچ;

    copy = tuple_alloc_with_اقلام(TUPLE_LEN(orig));
    اگر (copy == پوچ)
	بازگشت پوچ;

    اگر (orig->tv_type == پوچ || بالاترین || deep)
	copy->tv_type = پوچ;
    وگرنه
	copy->tv_type = alloc_type(orig->tv_type);
    اگر (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    برای (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_اقلام.ga_len++;
	اگر (deep)
	{
	    اگر (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		بشکن;
	}
	وگرنه
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    اگر (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = پوچ;
    }

    بازگشت copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    صحیح
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, صحیح do_error)
{
    صحیح		evaluate = evalarg == پوچ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = پوچ;
    typval_T	tv;
    صحیح		vim9script = in_vim9script();
    صحیح		had_comma;

    اگر (check_typval_is_مقدار(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	بازگشت FAIL;
    }

    اگر (evaluate)
    {
	tuple = tuple_alloc();
	اگر (tuple == پوچ)
	    بازگشت FAIL;

	اگر (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    اگر (tuple_append_tv(tuple, rettv) == FAIL)
		برو failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    اگر (**arg == ')')
	// empty tuple
	برو done;

    اگر (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	برو failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    وقتی (**arg != ')' && **arg != NUL)
    {
	اگر (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    برو failret;
	اگر (check_typval_is_مقدار(&tv) == FAIL)
	{
	    اگر (evaluate)
		clear_tv(&tv);
	    برو failret;
	}

	اگر (evaluate)
	{
	    اگر (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		برو failret;
	    }
	}

	اگر (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	اگر (had_comma)
	{
	    اگر (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		برو failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg == ')')
	    بشکن;

	اگر (!had_comma)
	{
	    اگر (do_error)
	    {
		اگر (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		وگرنه
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    برو failret;
	}
    }

    اگر (**arg != ')')
    {
	اگر (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	اگر (evaluate)
	    tuple_free(tuple);
	بازگشت FAIL;
    }

done:
    *arg += 1;
    اگر (evaluate)
	rettv_tuple_set(rettv, tuple);

    بازگشت OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    تهی
tuple_lock(tuple_T *tuple, صحیح deep, صحیح lock, صحیح check_refcount)
{
    اگر (tuple == پوچ || (check_refcount && tuple->tv_refcount > 1))
	بازگشت;

    اگر (lock)
	tuple->tv_lock |= VAR_LOCKED;
    وگرنه
	tuple->tv_lock &= ~VAR_LOCKED;

    اگر (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	برای (صحیح i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

تعریفنوع ساختار join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ایستا صحیح
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    صحیح		echo_style,
    صحیح		restore_copyID,
    صحیح		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    صحیح		i;
    join_T	*p;
    صحیح		sumlen = 0;
    صحیح		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    برای (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	اگر (s.string == پوچ)
	    بازگشت FAIL;

	s.length = STRLEN(s.string);
	sumlen += (صحیح)s.length;

	(تهی)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	اگر (tofree != پوچ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	وگرنه
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	سطر_breakcheck();
	اگر (did_echo_string_emsg)  // recursion error, bail out
	    بشکن;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    اگر (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	اگر (join_gap->ga_len >= 2)
	    sumlen += (صحیح)seplen * (join_gap->ga_len - 1);
    }
    اگر (ga_grow(gap, sumlen + 2) == FAIL)
	بازگشت FAIL;

    برای (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	اگر (first)
	    first = FALSE;
	وگرنه
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	اگر (p->s.string != پوچ)
	    ga_concat_len(gap, p->s.string, p->s.length);
	سطر_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    اگر (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    بازگشت OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    صحیح
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    صحیح		echo_style,
    صحیح		restore_copyID,
    صحیح		copyID)
{
    garray_T	join_ga;
    صحیح		retval;
    join_T	*p;
    صحیح		i;

    اگر (TUPLE_LEN(tuple) < 1)
	بازگشت OK; // nothing to do
    ga_init2(&join_ga, اندازه(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    اگر (join_ga.ga_data == پوچ)
	بازگشت retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    برای (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    بازگشت retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, صحیح copyID, صحیح restore_copyID)
{
    garray_T	ga;

    اگر (tv->vval.v_tuple == پوچ)
	بازگشت پوچ;
    ga_init2(&ga, اندازه(نویسه), 80);
    ga_append(&ga, '(');
    اگر (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	بازگشت پوچ;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    بازگشت (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    تهی
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*عبارت)
{
    صحیح		len = tuple_len(tuple);
    صحیح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_کلید, VAR_عدد);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, &newtv);

    برای (صحیح idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_کلید, idx);
	اگر (filter_map_one(TUPLE_ITEM(tuple, idx), عبارت, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    بشکن;
    }

    اگر (fc != پوچ)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_شمار(tuple_T *tuple, typval_T *needle, long idx, صحیح ic)
{
    long	n = 0;

    اگر (tuple == پوچ)
	بازگشت 0;

    صحیح	len = TUPLE_LEN(tuple);
    اگر (len == 0)
	بازگشت 0;

    اگر (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	بازگشت 0;
    }

    برای (صحیح i = idx; i < len; i++)
    {
	اگر (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    بازگشت n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    تهی
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    اگر (rettv_list_alloc(rettv) == FAIL)
	بازگشت;

    اگر (tuple == پوچ)
	بازگشت;  // null tuple behaves like an empty list

    برای (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	اگر (l == پوچ)
	    بشکن;

	اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    بشکن;
	}
	اگر (list_append_عدد(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    بشکن;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    صحیح
index_tuple(tuple_T *tuple, typval_T *tv, صحیح start_idx, صحیح ic)
{
    اگر (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	اگر (start_idx < 0)
	    start_idx = 0;
    }

    برای (صحیح idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	اگر (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    بازگشت idx;
    }

    بازگشت -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    صحیح
indexof_tuple(tuple_T *tuple, long startidx, typval_T *عبارت)
{
    long	idx = 0;
    صحیح		len;
    صحیح		found;

    اگر (tuple == پوچ)
	بازگشت -1;

    len = TUPLE_LEN(tuple);

    اگر (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	اگر (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_کلید, VAR_عدد);

    صحیح		called_emsg_start = called_emsg;

    برای (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_کلید, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_عبارت(عبارت);
	clear_tv(get_vim_var_tv(VV_VAL));

	اگر (found)
	    بازگشت idx;

	اگر (called_emsg != called_emsg_start)
	    بازگشت -1;
    }

    بازگشت -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, صحیح domax)
{
    typval_T	*tv = پوچ;
    صحیح		res;

    اگر (tuple == پوچ || TUPLE_LEN(tuple) == 0)
	بازگشت پوچ;

    برای (صحیح idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	اگر (tv == پوچ)
	    tv = TUPLE_ITEM(tuple, idx);
	وگرنه
	{
	    اگر (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? عبارت_GREATER : عبارت_SMALLER, FALSE, &res) == FAIL)
		بازگشت پوچ;
	    اگر (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    بازگشت tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    تهی
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = پوچ;

    اگر (tuple == پوچ || TUPLE_LEN(tuple) == 0 || n <= 0)
	بازگشت;

    اگر (check_repeat_شمار(TUPLE_LEN(tuple), n) == FAIL)
	بازگشت;

    اگر (rettv_tuple_set_with_اقلام(rettv, TUPLE_LEN(tuple) * (صحیح)n) == FAIL)
	بازگشت;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    برای (صحیح شمار = 0; شمار < (صحیح)n; شمار++)
    {
	برای (صحیح idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_اقلام.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    تهی
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = پوچ;

    صحیح	len = tuple_len(tuple);

    اگر (len == 0)
	بازگشت;

    اگر (rettv_tuple_set_with_اقلام(rettv, len) == FAIL)
	بازگشت;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    برای (صحیح i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_اقلام.ga_len = tuple->tv_اقلام.ga_len;
}

/*
 * Tuple reduce() function
 */
    تهی
tuple_reduce(typval_T *argvars, typval_T *عبارت, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    صحیح		called_emsg_start = called_emsg;
    typval_T	initial;
    صحیح		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    صحیح		r;

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	اگر (tuple == پوچ || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_مقدار), "Tuple");
	    بازگشت;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    وگرنه
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    اگر (tuple == پوچ)
	بازگشت;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_عبارت_get_funccal(عبارت, rettv);

    برای ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_عبارت_typval(عبارت, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	اگر (r == FAIL || called_emsg != called_emsg_start)
	    بشکن;
    }

    اگر (fc != پوچ)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    صحیح
check_tuples_addable(type_T *type1, type_T *type2)
{
    صحیح	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    اگر ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    اگر ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	اگر (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	وگرنه اگر ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    اگر (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	بازگشت FAIL;
    }

    بازگشت OK;
}

#endif // defined(FEAT_EVAL)
