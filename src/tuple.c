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

#जर defined(FEAT_EVAL)

// Tuple heads for garbage collection.
स्थिर tuple_T		*first_tuple = रिक्त;	// list of all tuples

    स्थिर शून्य
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    जर (first_tuple != रिक्त)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = रिक्त;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_वस्तुसंच, आकार(typval_T), 20);
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
    जर (tuple != रिक्त)
	tuple_init(tuple);
    परत tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_वस्तुसंच(पूर्णांक मोजणी)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    जर (tuple == रिक्त)
	परत रिक्त;

    जर (मोजणी <= 0)
	परत tuple;

    जर (ga_grow(&tuple->tv_वस्तुसंच, मोजणी) == FAIL)
    {
	tuple_free(tuple);
	परत रिक्त;
    }

    परत tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    शून्य
tuple_set_item(tuple_T *tuple, पूर्णांक idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_वस्तुसंच.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    पूर्णांक
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    जर (tuple == रिक्त)
	परत FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    परत OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    शून्य
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    जर (tuple != रिक्त)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    पूर्णांक
rettv_tuple_set_with_वस्तुसंच(typval_T *rettv, पूर्णांक मोजणी)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_वस्तुसंच(मोजणी);
    जर (new_tuple == रिक्त)
	परत FAIL;

    rettv_tuple_set(rettv, new_tuple);

    परत OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    शून्य
tuple_unref(tuple_T *tuple)
{
    जर (tuple != रिक्त && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    स्थिर शून्य
tuple_free_contents(tuple_T *tuple)
{
    साठी (पूर्णांक i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_वस्तुसंच);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    पूर्णांक
tuple_free_nonref(पूर्णांक copyID)
{
    tuple_T	*tt;
    पूर्णांक		did_free = FALSE;

    साठी (tt = first_tuple; tt != रिक्त; tt = tt->tv_used_next)
	जर ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    परत did_free;
}

    स्थिर शून्य
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    जर (tuple->tv_used_prev == रिक्त)
	first_tuple = tuple->tv_used_next;
    अन्यथा
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    जर (tuple->tv_used_next != रिक्त)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    शून्य
tuple_free_वस्तुसंच(पूर्णांक copyID)
{
    tuple_T	*tt, *tt_next;

    साठी (tt = first_tuple; tt != रिक्त; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	जर ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
    जर (in_free_unref_वस्तुसंच)
	परत;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    दीर्घ
tuple_len(tuple_T *tuple)
{
    जर (tuple == रिक्त)
	परत 0L;
    परत tuple->tv_वस्तुसंच.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    पूर्णांक
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    पूर्णांक		ic)	// ignore case for strings
{
    जर (t1 == t2)
	परत TRUE;

    पूर्णांक t1_len = tuple_len(t1);
    पूर्णांक t2_len = tuple_len(t2);

    जर (t1_len != t2_len)
	परत FALSE;

    जर (t1_len == 0)
	// empty and NULL tuples are considered equal
	परत TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    साठी (पूर्णांक i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	जर (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    परत FALSE;

    परत TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, दीर्घ n)
{
    जर (tuple == रिक्त)
	परत रिक्त;

    // Negative index is relative to the end.
    जर (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    जर (n < 0 || n >= TUPLE_LEN(tuple))
	परत रिक्त;

    परत TUPLE_ITEM(tuple, n);
}

    पूर्णांक
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    जर (ga_grow_id(&tuple->tv_वस्तुसंच, 1, aid_tuple_append) == FAIL)
	परत FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    परत OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    पूर्णांक
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    जर (t1 == रिक्त)
	tuple = tuple_alloc();
    अन्यथा
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    जर (tuple == रिक्त)
	परत FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    जर (t1 == रिक्त)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    साठी (पूर्णांक i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	जर (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    परत FAIL;
	}
    }

    परत OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, दीर्घ n1, दीर्घ n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_वस्तुसंच(n2 - n1 + 1);
    जर (new_tuple == रिक्त)
	परत रिक्त;

    साठी (पूर्णांक i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	जर (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    परत रिक्त;
	}
    }

    परत new_tuple;
}

    पूर्णांक
tuple_slice_or_index(
    tuple_T	*tuple,
    पूर्णांक		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    पूर्णांक		exclusive,
    typval_T	*rettv,
    पूर्णांक		verbose)
{
    दीर्घ	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    जर (n1 < 0)
	n1 = len + n1;
    जर (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	जर (!range)
	{
	    जर (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (दीर्घ)n1_arg);
	    परत FAIL;
	}
	जर (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	अन्यथा
	    n1 = len;
    }
    जर (range)
    {
	tuple_T	*new_tuple;

	जर (n2 < 0)
	    n2 = len + n2;
	अन्यथा जर (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	जर (exclusive)
	    --n2;
	जर (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	जर (new_tuple == रिक्त)
	    परत FAIL;
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
    परत OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, पूर्णांक deep, पूर्णांक शीर्ष, पूर्णांक copyID)
{
    tuple_T	*copy;
    पूर्णांक		idx;

    जर (orig == रिक्त)
	परत रिक्त;

    copy = tuple_alloc_with_वस्तुसंच(TUPLE_LEN(orig));
    जर (copy == रिक्त)
	परत रिक्त;

    जर (orig->tv_type == रिक्त || शीर्ष || deep)
	copy->tv_type = रिक्त;
    अन्यथा
	copy->tv_type = alloc_type(orig->tv_type);
    जर (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    साठी (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_वस्तुसंच.ga_len++;
	जर (deep)
	{
	    जर (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		थांबा;
	}
	अन्यथा
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    जर (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = रिक्त;
    }

    परत copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    पूर्णांक
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, पूर्णांक do_error)
{
    पूर्णांक		evaluate = evalarg == रिक्त ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = रिक्त;
    typval_T	tv;
    पूर्णांक		vim9script = in_vim9script();
    पूर्णांक		had_comma;

    जर (check_typval_is_मूल्य(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	परत FAIL;
    }

    जर (evaluate)
    {
	tuple = tuple_alloc();
	जर (tuple == रिक्त)
	    परत FAIL;

	जर (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    जर (tuple_append_tv(tuple, rettv) == FAIL)
		जा failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    जर (**arg == ')')
	// empty tuple
	जा done;

    जर (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	जा failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    पर्यंत (**arg != ')' && **arg != NUL)
    {
	जर (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    जा failret;
	जर (check_typval_is_मूल्य(&tv) == FAIL)
	{
	    जर (evaluate)
		clear_tv(&tv);
	    जा failret;
	}

	जर (evaluate)
	{
	    जर (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		जा failret;
	    }
	}

	जर (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	जर (had_comma)
	{
	    जर (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		जा failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	जर (**arg == ')')
	    थांबा;

	जर (!had_comma)
	{
	    जर (do_error)
	    {
		जर (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		अन्यथा
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    जा failret;
	}
    }

    जर (**arg != ')')
    {
	जर (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	जर (evaluate)
	    tuple_free(tuple);
	परत FAIL;
    }

done:
    *arg += 1;
    जर (evaluate)
	rettv_tuple_set(rettv, tuple);

    परत OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    शून्य
tuple_lock(tuple_T *tuple, पूर्णांक deep, पूर्णांक lock, पूर्णांक check_refcount)
{
    जर (tuple == रिक्त || (check_refcount && tuple->tv_refcount > 1))
	परत;

    जर (lock)
	tuple->tv_lock |= VAR_LOCKED;
    अन्यथा
	tuple->tv_lock &= ~VAR_LOCKED;

    जर (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	साठी (पूर्णांक i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

प्रकारव्याख्या संरचना join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    स्थिर पूर्णांक
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    पूर्णांक		echo_style,
    पूर्णांक		restore_copyID,
    पूर्णांक		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    पूर्णांक		i;
    join_T	*p;
    पूर्णांक		sumlen = 0;
    पूर्णांक		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    माप_प्रकार	seplen = 0;

    // Stringify each item in the tuple.
    साठी (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	जर (s.string == रिक्त)
	    परत FAIL;

	s.length = STRLEN(s.string);
	sumlen += (पूर्णांक)s.length;

	(शून्य)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	जर (tofree != रिक्त || s.string != numbuf)
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

	ओळ_breakcheck();
	जर (did_echo_string_emsg)  // recursion error, bail out
	    थांबा;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    जर (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	जर (join_gap->ga_len >= 2)
	    sumlen += (पूर्णांक)seplen * (join_gap->ga_len - 1);
    }
    जर (ga_grow(gap, sumlen + 2) == FAIL)
	परत FAIL;

    साठी (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	जर (first)
	    first = FALSE;
	अन्यथा
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	जर (p->s.string != रिक्त)
	    ga_concat_len(gap, p->s.string, p->s.length);
	ओळ_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    जर (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    परत OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    पूर्णांक
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    पूर्णांक		echo_style,
    पूर्णांक		restore_copyID,
    पूर्णांक		copyID)
{
    garray_T	join_ga;
    पूर्णांक		retval;
    join_T	*p;
    पूर्णांक		i;

    जर (TUPLE_LEN(tuple) < 1)
	परत OK; // nothing to do
    ga_init2(&join_ga, आकार(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    जर (join_ga.ga_data == रिक्त)
	परत retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    साठी (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    परत retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, पूर्णांक copyID, पूर्णांक restore_copyID)
{
    garray_T	ga;

    जर (tv->vval.v_tuple == रिक्त)
	परत रिक्त;
    ga_init2(&ga, आकार(अक्षर), 80);
    ga_append(&ga, '(');
    जर (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	परत रिक्त;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    परत (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    शून्य
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*अभिव्यक्ती)
{
    पूर्णांक		len = tuple_len(tuple);
    पूर्णांक		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_किल्ली, VAR_संख्या);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ती_get_funccal(अभिव्यक्ती, &newtv);

    साठी (पूर्णांक idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_किल्ली, idx);
	जर (filter_map_one(TUPLE_ITEM(tuple, idx), अभिव्यक्ती, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    थांबा;
    }

    जर (fc != रिक्त)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    दीर्घ
tuple_मोजणी(tuple_T *tuple, typval_T *needle, दीर्घ idx, पूर्णांक ic)
{
    दीर्घ	n = 0;

    जर (tuple == रिक्त)
	परत 0;

    पूर्णांक	len = TUPLE_LEN(tuple);
    जर (len == 0)
	परत 0;

    जर (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	परत 0;
    }

    साठी (पूर्णांक i = idx; i < len; i++)
    {
	जर (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    परत n;
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

    जर (rettv_list_alloc(rettv) == FAIL)
	परत;

    जर (tuple == रिक्त)
	परत;  // null tuple behaves like an empty list

    साठी (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	जर (l == रिक्त)
	    थांबा;

	जर (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    थांबा;
	}
	जर (list_append_संख्या(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    थांबा;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    पूर्णांक
index_tuple(tuple_T *tuple, typval_T *tv, पूर्णांक start_idx, पूर्णांक ic)
{
    जर (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	जर (start_idx < 0)
	    start_idx = 0;
    }

    साठी (पूर्णांक idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	जर (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    परत idx;
    }

    परत -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    पूर्णांक
indexof_tuple(tuple_T *tuple, दीर्घ startidx, typval_T *अभिव्यक्ती)
{
    दीर्घ	idx = 0;
    पूर्णांक		len;
    पूर्णांक		found;

    जर (tuple == रिक्त)
	परत -1;

    len = TUPLE_LEN(tuple);

    जर (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	जर (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_किल्ली, VAR_संख्या);

    पूर्णांक		called_emsg_start = called_emsg;

    साठी (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_किल्ली, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_अभिव्यक्ती(अभिव्यक्ती);
	clear_tv(get_vim_var_tv(VV_VAL));

	जर (found)
	    परत idx;

	जर (called_emsg != called_emsg_start)
	    परत -1;
    }

    परत -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, पूर्णांक domax)
{
    typval_T	*tv = रिक्त;
    पूर्णांक		res;

    जर (tuple == रिक्त || TUPLE_LEN(tuple) == 0)
	परत रिक्त;

    साठी (पूर्णांक idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	जर (tv == रिक्त)
	    tv = TUPLE_ITEM(tuple, idx);
	अन्यथा
	{
	    जर (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? अभिव्यक्ती_GREATER : अभिव्यक्ती_SMALLER, FALSE, &res) == FAIL)
		परत रिक्त;
	    जर (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    परत tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    शून्य
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = रिक्त;

    जर (tuple == रिक्त || TUPLE_LEN(tuple) == 0 || n <= 0)
	परत;

    जर (check_repeat_मोजणी(TUPLE_LEN(tuple), n) == FAIL)
	परत;

    जर (rettv_tuple_set_with_वस्तुसंच(rettv, TUPLE_LEN(tuple) * (पूर्णांक)n) == FAIL)
	परत;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    साठी (पूर्णांक मोजणी = 0; मोजणी < (पूर्णांक)n; मोजणी++)
    {
	साठी (पूर्णांक idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_वस्तुसंच.ga_len++;
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

    पूर्णांक	len = tuple_len(tuple);

    जर (len == 0)
	परत;

    जर (rettv_tuple_set_with_वस्तुसंच(rettv, len) == FAIL)
	परत;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    साठी (पूर्णांक i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_वस्तुसंच.ga_len = tuple->tv_वस्तुसंच.ga_len;
}

/*
 * Tuple reduce() function
 */
    शून्य
tuple_reduce(typval_T *argvars, typval_T *अभिव्यक्ती, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    पूर्णांक		called_emsg_start = called_emsg;
    typval_T	initial;
    पूर्णांक		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    पूर्णांक		r;

    जर (argvars[2].v_type == VAR_UNKNOWN)
    {
	जर (tuple == रिक्त || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_मूल्य), "Tuple");
	    परत;
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

    जर (tuple == रिक्त)
	परत;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_अभिव्यक्ती_get_funccal(अभिव्यक्ती, rettv);

    साठी ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_अभिव्यक्ती_typval(अभिव्यक्ती, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	जर (r == FAIL || called_emsg != called_emsg_start)
	    थांबा;
    }

    जर (fc != रिक्त)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    पूर्णांक
check_tuples_addable(type_T *type1, type_T *type2)
{
    पूर्णांक	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    जर ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    जर ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	जर (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	अन्यथा जर ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    जर (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	परत FAIL;
    }

    परत OK;
}

#endif // defined(FEAT_EVAL)
