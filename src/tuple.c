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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_EVAL)

// Tuple heads for garbage collection.
நிலையான tuple_T		*first_tuple = வெற்று;	// list of all tuples

    நிலையான வெறுமை
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    எனில் (first_tuple != வெற்று)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = வெற்று;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_பொருட்கள், அளவு(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(வெறுமை)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    எனில் (tuple != வெற்று)
	tuple_init(tuple);
    திரும்பு tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_பொருட்கள்(முழு மொத்தம்)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    எனில் (tuple == வெற்று)
	திரும்பு வெற்று;

    எனில் (மொத்தம் <= 0)
	திரும்பு tuple;

    எனில் (ga_grow(&tuple->tv_பொருட்கள், மொத்தம்) == FAIL)
    {
	tuple_free(tuple);
	திரும்பு வெற்று;
    }

    திரும்பு tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    வெறுமை
tuple_set_item(tuple_T *tuple, முழு idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_பொருட்கள்.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    முழு
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    எனில் (tuple == வெற்று)
	திரும்பு FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    திரும்பு OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    வெறுமை
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    எனில் (tuple != வெற்று)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    முழு
rettv_tuple_set_with_பொருட்கள்(typval_T *rettv, முழு மொத்தம்)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_பொருட்கள்(மொத்தம்);
    எனில் (new_tuple == வெற்று)
	திரும்பு FAIL;

    rettv_tuple_set(rettv, new_tuple);

    திரும்பு OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    வெறுமை
tuple_unref(tuple_T *tuple)
{
    எனில் (tuple != வெற்று && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    நிலையான வெறுமை
tuple_free_contents(tuple_T *tuple)
{
    க்கு (முழு i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_பொருட்கள்);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    முழு
tuple_free_nonref(முழு copyID)
{
    tuple_T	*tt;
    முழு		did_free = FALSE;

    க்கு (tt = first_tuple; tt != வெற்று; tt = tt->tv_used_next)
	எனில் ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    திரும்பு did_free;
}

    நிலையான வெறுமை
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    எனில் (tuple->tv_used_prev == வெற்று)
	first_tuple = tuple->tv_used_next;
    இல்லையெனில்
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    எனில் (tuple->tv_used_next != வெற்று)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    வெறுமை
tuple_free_பொருட்கள்(முழு copyID)
{
    tuple_T	*tt, *tt_next;

    க்கு (tt = first_tuple; tt != வெற்று; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	எனில் ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    வெறுமை
tuple_free(tuple_T *tuple)
{
    எனில் (in_free_unref_பொருட்கள்)
	திரும்பு;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    எனில் (tuple == வெற்று)
	திரும்பு 0L;
    திரும்பு tuple->tv_பொருட்கள்.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    முழு
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    முழு		ic)	// ignore case for strings
{
    எனில் (t1 == t2)
	திரும்பு TRUE;

    முழு t1_len = tuple_len(t1);
    முழு t2_len = tuple_len(t2);

    எனில் (t1_len != t2_len)
	திரும்பு FALSE;

    எனில் (t1_len == 0)
	// empty and NULL tuples are considered equal
	திரும்பு TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    க்கு (முழு i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	எனில் (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    திரும்பு FALSE;

    திரும்பு TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    எனில் (tuple == வெற்று)
	திரும்பு வெற்று;

    // Negative index is relative to the end.
    எனில் (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    எனில் (n < 0 || n >= TUPLE_LEN(tuple))
	திரும்பு வெற்று;

    திரும்பு TUPLE_ITEM(tuple, n);
}

    முழு
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    எனில் (ga_grow_id(&tuple->tv_பொருட்கள், 1, aid_tuple_append) == FAIL)
	திரும்பு FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    திரும்பு OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    முழு
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    எனில் (t1 == வெற்று)
	tuple = tuple_alloc();
    இல்லையெனில்
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    எனில் (tuple == வெற்று)
	திரும்பு FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    எனில் (t1 == வெற்று)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    க்கு (முழு i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	எனில் (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    திரும்பு FAIL;
	}
    }

    திரும்பு OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_பொருட்கள்(n2 - n1 + 1);
    எனில் (new_tuple == வெற்று)
	திரும்பு வெற்று;

    க்கு (முழு i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	எனில் (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    திரும்பு வெற்று;
	}
    }

    திரும்பு new_tuple;
}

    முழு
tuple_slice_or_index(
    tuple_T	*tuple,
    முழு		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    முழு		exclusive,
    typval_T	*rettv,
    முழு		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    எனில் (n1 < 0)
	n1 = len + n1;
    எனில் (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	எனில் (!range)
	{
	    எனில் (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    திரும்பு FAIL;
	}
	எனில் (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	இல்லையெனில்
	    n1 = len;
    }
    எனில் (range)
    {
	tuple_T	*new_tuple;

	எனில் (n2 < 0)
	    n2 = len + n2;
	இல்லையெனில் எனில் (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	எனில் (exclusive)
	    --n2;
	எனில் (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	எனில் (new_tuple == வெற்று)
	    திரும்பு FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    இல்லையெனில்
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    திரும்பு OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, முழு deep, முழு உச்சி, முழு copyID)
{
    tuple_T	*copy;
    முழு		idx;

    எனில் (orig == வெற்று)
	திரும்பு வெற்று;

    copy = tuple_alloc_with_பொருட்கள்(TUPLE_LEN(orig));
    எனில் (copy == வெற்று)
	திரும்பு வெற்று;

    எனில் (orig->tv_type == வெற்று || உச்சி || deep)
	copy->tv_type = வெற்று;
    இல்லையெனில்
	copy->tv_type = alloc_type(orig->tv_type);
    எனில் (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    க்கு (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_பொருட்கள்.ga_len++;
	எனில் (deep)
	{
	    எனில் (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		நிறுத்து;
	}
	இல்லையெனில்
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    எனில் (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = வெற்று;
    }

    திரும்பு copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    முழு
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, முழு do_error)
{
    முழு		evaluate = evalarg == வெற்று ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = வெற்று;
    typval_T	tv;
    முழு		vim9script = in_vim9script();
    முழு		had_comma;

    எனில் (check_typval_is_மதிப்பு(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	திரும்பு FAIL;
    }

    எனில் (evaluate)
    {
	tuple = tuple_alloc();
	எனில் (tuple == வெற்று)
	    திரும்பு FAIL;

	எனில் (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    எனில் (tuple_append_tv(tuple, rettv) == FAIL)
		செல் failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    எனில் (**arg == ')')
	// empty tuple
	செல் done;

    எனில் (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	செல் failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    போது (**arg != ')' && **arg != NUL)
    {
	எனில் (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    செல் failret;
	எனில் (check_typval_is_மதிப்பு(&tv) == FAIL)
	{
	    எனில் (evaluate)
		clear_tv(&tv);
	    செல் failret;
	}

	எனில் (evaluate)
	{
	    எனில் (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		செல் failret;
	    }
	}

	எனில் (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	எனில் (had_comma)
	{
	    எனில் (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		செல் failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	எனில் (**arg == ')')
	    நிறுத்து;

	எனில் (!had_comma)
	{
	    எனில் (do_error)
	    {
		எனில் (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		இல்லையெனில்
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    செல் failret;
	}
    }

    எனில் (**arg != ')')
    {
	எனில் (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	எனில் (evaluate)
	    tuple_free(tuple);
	திரும்பு FAIL;
    }

done:
    *arg += 1;
    எனில் (evaluate)
	rettv_tuple_set(rettv, tuple);

    திரும்பு OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    வெறுமை
tuple_lock(tuple_T *tuple, முழு deep, முழு lock, முழு check_refcount)
{
    எனில் (tuple == வெற்று || (check_refcount && tuple->tv_refcount > 1))
	திரும்பு;

    எனில் (lock)
	tuple->tv_lock |= VAR_LOCKED;
    இல்லையெனில்
	tuple->tv_lock &= ~VAR_LOCKED;

    எனில் (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	க்கு (முழு i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

வகைவரை கட்டமைப்பு join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    நிலையான முழு
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    முழு		echo_style,
    முழு		restore_copyID,
    முழு		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    முழு		i;
    join_T	*p;
    முழு		sumlen = 0;
    முழு		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    க்கு (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	எனில் (s.string == வெற்று)
	    திரும்பு FAIL;

	s.length = STRLEN(s.string);
	sumlen += (முழு)s.length;

	(வெறுமை)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	எனில் (tofree != வெற்று || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	இல்லையெனில்
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	வரி_breakcheck();
	எனில் (did_echo_string_emsg)  // recursion error, bail out
	    நிறுத்து;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    எனில் (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	எனில் (join_gap->ga_len >= 2)
	    sumlen += (முழு)seplen * (join_gap->ga_len - 1);
    }
    எனில் (ga_grow(gap, sumlen + 2) == FAIL)
	திரும்பு FAIL;

    க்கு (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	எனில் (first)
	    first = FALSE;
	இல்லையெனில்
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	எனில் (p->s.string != வெற்று)
	    ga_concat_len(gap, p->s.string, p->s.length);
	வரி_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    எனில் (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    திரும்பு OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    முழு
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    முழு		echo_style,
    முழு		restore_copyID,
    முழு		copyID)
{
    garray_T	join_ga;
    முழு		retval;
    join_T	*p;
    முழு		i;

    எனில் (TUPLE_LEN(tuple) < 1)
	திரும்பு OK; // nothing to do
    ga_init2(&join_ga, அளவு(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    எனில் (join_ga.ga_data == வெற்று)
	திரும்பு retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    க்கு (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    திரும்பு retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, முழு copyID, முழு restore_copyID)
{
    garray_T	ga;

    எனில் (tv->vval.v_tuple == வெற்று)
	திரும்பு வெற்று;
    ga_init2(&ga, அளவு(எழுத்து), 80);
    ga_append(&ga, '(');
    எனில் (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	திரும்பு வெற்று;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    திரும்பு (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    வெறுமை
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*கோவை)
{
    முழு		len = tuple_len(tuple);
    முழு		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_சாவி, VAR_எண்);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_கோவை_get_funccal(கோவை, &newtv);

    க்கு (முழு idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_சாவி, idx);
	எனில் (filter_map_one(TUPLE_ITEM(tuple, idx), கோவை, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    நிறுத்து;
    }

    எனில் (fc != வெற்று)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_மொத்தம்(tuple_T *tuple, typval_T *needle, long idx, முழு ic)
{
    long	n = 0;

    எனில் (tuple == வெற்று)
	திரும்பு 0;

    முழு	len = TUPLE_LEN(tuple);
    எனில் (len == 0)
	திரும்பு 0;

    எனில் (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	திரும்பு 0;
    }

    க்கு (முழு i = idx; i < len; i++)
    {
	எனில் (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    திரும்பு n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    வெறுமை
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    எனில் (rettv_list_alloc(rettv) == FAIL)
	திரும்பு;

    எனில் (tuple == வெற்று)
	திரும்பு;  // null tuple behaves like an empty list

    க்கு (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	எனில் (l == வெற்று)
	    நிறுத்து;

	எனில் (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    நிறுத்து;
	}
	எனில் (list_append_எண்(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    நிறுத்து;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    முழு
index_tuple(tuple_T *tuple, typval_T *tv, முழு start_idx, முழு ic)
{
    எனில் (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	எனில் (start_idx < 0)
	    start_idx = 0;
    }

    க்கு (முழு idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	எனில் (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    திரும்பு idx;
    }

    திரும்பு -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    முழு
indexof_tuple(tuple_T *tuple, long startidx, typval_T *கோவை)
{
    long	idx = 0;
    முழு		len;
    முழு		found;

    எனில் (tuple == வெற்று)
	திரும்பு -1;

    len = TUPLE_LEN(tuple);

    எனில் (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	எனில் (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_சாவி, VAR_எண்);

    முழு		called_emsg_start = called_emsg;

    க்கு (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_சாவி, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_கோவை(கோவை);
	clear_tv(get_vim_var_tv(VV_VAL));

	எனில் (found)
	    திரும்பு idx;

	எனில் (called_emsg != called_emsg_start)
	    திரும்பு -1;
    }

    திரும்பு -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, முழு domax)
{
    typval_T	*tv = வெற்று;
    முழு		res;

    எனில் (tuple == வெற்று || TUPLE_LEN(tuple) == 0)
	திரும்பு வெற்று;

    க்கு (முழு idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	எனில் (tv == வெற்று)
	    tv = TUPLE_ITEM(tuple, idx);
	இல்லையெனில்
	{
	    எனில் (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? கோவை_GREATER : கோவை_SMALLER, FALSE, &res) == FAIL)
		திரும்பு வெற்று;
	    எனில் (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    திரும்பு tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    வெறுமை
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = வெற்று;

    எனில் (tuple == வெற்று || TUPLE_LEN(tuple) == 0 || n <= 0)
	திரும்பு;

    எனில் (check_repeat_மொத்தம்(TUPLE_LEN(tuple), n) == FAIL)
	திரும்பு;

    எனில் (rettv_tuple_set_with_பொருட்கள்(rettv, TUPLE_LEN(tuple) * (முழு)n) == FAIL)
	திரும்பு;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    க்கு (முழு மொத்தம் = 0; மொத்தம் < (முழு)n; மொத்தம்++)
    {
	க்கு (முழு idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_பொருட்கள்.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    வெறுமை
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = வெற்று;

    முழு	len = tuple_len(tuple);

    எனில் (len == 0)
	திரும்பு;

    எனில் (rettv_tuple_set_with_பொருட்கள்(rettv, len) == FAIL)
	திரும்பு;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    க்கு (முழு i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_பொருட்கள்.ga_len = tuple->tv_பொருட்கள்.ga_len;
}

/*
 * Tuple reduce() function
 */
    வெறுமை
tuple_reduce(typval_T *argvars, typval_T *கோவை, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    முழு		called_emsg_start = called_emsg;
    typval_T	initial;
    முழு		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    முழு		r;

    எனில் (argvars[2].v_type == VAR_UNKNOWN)
    {
	எனில் (tuple == வெற்று || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_மதிப்பு), "Tuple");
	    திரும்பு;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    இல்லையெனில்
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    எனில் (tuple == வெற்று)
	திரும்பு;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_கோவை_get_funccal(கோவை, rettv);

    க்கு ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_கோவை_typval(கோவை, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	எனில் (r == FAIL || called_emsg != called_emsg_start)
	    நிறுத்து;
    }

    எனில் (fc != வெற்று)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    முழு
check_tuples_addable(type_T *type1, type_T *type2)
{
    முழு	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    எனில் ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    எனில் ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	எனில் (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	இல்லையெனில் எனில் ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    எனில் (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	திரும்பு FAIL;
    }

    திரும்பு OK;
}

#endif // defined(FEAT_EVAL)
