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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_EVAL)

// Tuple heads for garbage collection.
સ્થિર tuple_T		*first_tuple = ખાલી;	// list of all tuples

    સ્થિર શૂન્ય
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    જો (first_tuple != ખાલી)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = ખાલી;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_વસ્તુઓ, કદ(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(શૂન્ય)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    જો (tuple != ખાલી)
	tuple_init(tuple);
    પાછા tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_વસ્તુઓ(પૂર્ણાંક ગણતરી)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    જો (tuple == ખાલી)
	પાછા ખાલી;

    જો (ગણતરી <= 0)
	પાછા tuple;

    જો (ga_grow(&tuple->tv_વસ્તુઓ, ગણતરી) == FAIL)
    {
	tuple_free(tuple);
	પાછા ખાલી;
    }

    પાછા tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    શૂન્ય
tuple_set_item(tuple_T *tuple, પૂર્ણાંક idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_વસ્તુઓ.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    જો (tuple == ખાલી)
	પાછા FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    પાછા OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    શૂન્ય
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    જો (tuple != ખાલી)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    પૂર્ણાંક
rettv_tuple_set_with_વસ્તુઓ(typval_T *rettv, પૂર્ણાંક ગણતરી)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_વસ્તુઓ(ગણતરી);
    જો (new_tuple == ખાલી)
	પાછા FAIL;

    rettv_tuple_set(rettv, new_tuple);

    પાછા OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    શૂન્ય
tuple_unref(tuple_T *tuple)
{
    જો (tuple != ખાલી && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    સ્થિર શૂન્ય
tuple_free_contents(tuple_T *tuple)
{
    માટે (પૂર્ણાંક i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_વસ્તુઓ);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    પૂર્ણાંક
tuple_free_nonref(પૂર્ણાંક copyID)
{
    tuple_T	*tt;
    પૂર્ણાંક		did_free = FALSE;

    માટે (tt = first_tuple; tt != ખાલી; tt = tt->tv_used_next)
	જો ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    પાછા did_free;
}

    સ્થિર શૂન્ય
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    જો (tuple->tv_used_prev == ખાલી)
	first_tuple = tuple->tv_used_next;
    નહીંતર
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    જો (tuple->tv_used_next != ખાલી)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    શૂન્ય
tuple_free_વસ્તુઓ(પૂર્ણાંક copyID)
{
    tuple_T	*tt, *tt_next;

    માટે (tt = first_tuple; tt != ખાલી; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	જો ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    શૂન્ય
tuple_free(tuple_T *tuple)
{
    જો (in_free_unref_વસ્તુઓ)
	પાછા;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    જો (tuple == ખાલી)
	પાછા 0L;
    પાછા tuple->tv_વસ્તુઓ.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    પૂર્ણાંક
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    પૂર્ણાંક		ic)	// ignore case for strings
{
    જો (t1 == t2)
	પાછા TRUE;

    પૂર્ણાંક t1_len = tuple_len(t1);
    પૂર્ણાંક t2_len = tuple_len(t2);

    જો (t1_len != t2_len)
	પાછા FALSE;

    જો (t1_len == 0)
	// empty and NULL tuples are considered equal
	પાછા TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    માટે (પૂર્ણાંક i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	જો (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    પાછા FALSE;

    પાછા TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    જો (tuple == ખાલી)
	પાછા ખાલી;

    // Negative index is relative to the end.
    જો (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    જો (n < 0 || n >= TUPLE_LEN(tuple))
	પાછા ખાલી;

    પાછા TUPLE_ITEM(tuple, n);
}

    પૂર્ણાંક
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    જો (ga_grow_id(&tuple->tv_વસ્તુઓ, 1, aid_tuple_append) == FAIL)
	પાછા FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    પાછા OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    પૂર્ણાંક
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    જો (t1 == ખાલી)
	tuple = tuple_alloc();
    નહીંતર
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    જો (tuple == ખાલી)
	પાછા FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    જો (t1 == ખાલી)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    માટે (પૂર્ણાંક i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	જો (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    પાછા FAIL;
	}
    }

    પાછા OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_વસ્તુઓ(n2 - n1 + 1);
    જો (new_tuple == ખાલી)
	પાછા ખાલી;

    માટે (પૂર્ણાંક i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	જો (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    પાછા ખાલી;
	}
    }

    પાછા new_tuple;
}

    પૂર્ણાંક
tuple_slice_or_index(
    tuple_T	*tuple,
    પૂર્ણાંક		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    પૂર્ણાંક		exclusive,
    typval_T	*rettv,
    પૂર્ણાંક		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    જો (n1 < 0)
	n1 = len + n1;
    જો (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	જો (!range)
	{
	    જો (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    પાછા FAIL;
	}
	જો (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	નહીંતર
	    n1 = len;
    }
    જો (range)
    {
	tuple_T	*new_tuple;

	જો (n2 < 0)
	    n2 = len + n2;
	નહીંતર જો (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	જો (exclusive)
	    --n2;
	જો (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	જો (new_tuple == ખાલી)
	    પાછા FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    નહીંતર
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    પાછા OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, પૂર્ણાંક deep, પૂર્ણાંક ટોચ, પૂર્ણાંક copyID)
{
    tuple_T	*copy;
    પૂર્ણાંક		idx;

    જો (orig == ખાલી)
	પાછા ખાલી;

    copy = tuple_alloc_with_વસ્તુઓ(TUPLE_LEN(orig));
    જો (copy == ખાલી)
	પાછા ખાલી;

    જો (orig->tv_type == ખાલી || ટોચ || deep)
	copy->tv_type = ખાલી;
    નહીંતર
	copy->tv_type = alloc_type(orig->tv_type);
    જો (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    માટે (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_વસ્તુઓ.ga_len++;
	જો (deep)
	{
	    જો (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		તોડો;
	}
	નહીંતર
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    જો (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = ખાલી;
    }

    પાછા copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, પૂર્ણાંક do_error)
{
    પૂર્ણાંક		evaluate = evalarg == ખાલી ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = ખાલી;
    typval_T	tv;
    પૂર્ણાંક		vim9script = in_vim9script();
    પૂર્ણાંક		had_comma;

    જો (check_typval_is_કિંમત(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	પાછા FAIL;
    }

    જો (evaluate)
    {
	tuple = tuple_alloc();
	જો (tuple == ખાલી)
	    પાછા FAIL;

	જો (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    જો (tuple_append_tv(tuple, rettv) == FAIL)
		જાઓ failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    જો (**arg == ')')
	// empty tuple
	જાઓ done;

    જો (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	જાઓ failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    જ્યારે (**arg != ')' && **arg != NUL)
    {
	જો (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    જાઓ failret;
	જો (check_typval_is_કિંમત(&tv) == FAIL)
	{
	    જો (evaluate)
		clear_tv(&tv);
	    જાઓ failret;
	}

	જો (evaluate)
	{
	    જો (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		જાઓ failret;
	    }
	}

	જો (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	જો (had_comma)
	{
	    જો (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		જાઓ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	જો (**arg == ')')
	    તોડો;

	જો (!had_comma)
	{
	    જો (do_error)
	    {
		જો (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		નહીંતર
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    જાઓ failret;
	}
    }

    જો (**arg != ')')
    {
	જો (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	જો (evaluate)
	    tuple_free(tuple);
	પાછા FAIL;
    }

done:
    *arg += 1;
    જો (evaluate)
	rettv_tuple_set(rettv, tuple);

    પાછા OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    શૂન્ય
tuple_lock(tuple_T *tuple, પૂર્ણાંક deep, પૂર્ણાંક lock, પૂર્ણાંક check_refcount)
{
    જો (tuple == ખાલી || (check_refcount && tuple->tv_refcount > 1))
	પાછા;

    જો (lock)
	tuple->tv_lock |= VAR_LOCKED;
    નહીંતર
	tuple->tv_lock &= ~VAR_LOCKED;

    જો (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	માટે (પૂર્ણાંક i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

પ્રકારવ્યાખ્યા સંરચના join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    સ્થિર પૂર્ણાંક
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    પૂર્ણાંક		echo_style,
    પૂર્ણાંક		restore_copyID,
    પૂર્ણાંક		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    પૂર્ણાંક		i;
    join_T	*p;
    પૂર્ણાંક		sumlen = 0;
    પૂર્ણાંક		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    માટે (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	જો (s.string == ખાલી)
	    પાછા FAIL;

	s.length = STRLEN(s.string);
	sumlen += (પૂર્ણાંક)s.length;

	(શૂન્ય)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	જો (tofree != ખાલી || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	નહીંતર
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	લીટી_breakcheck();
	જો (did_echo_string_emsg)  // recursion error, bail out
	    તોડો;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    જો (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	જો (join_gap->ga_len >= 2)
	    sumlen += (પૂર્ણાંક)seplen * (join_gap->ga_len - 1);
    }
    જો (ga_grow(gap, sumlen + 2) == FAIL)
	પાછા FAIL;

    માટે (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	જો (first)
	    first = FALSE;
	નહીંતર
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	જો (p->s.string != ખાલી)
	    ga_concat_len(gap, p->s.string, p->s.length);
	લીટી_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    જો (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    પાછા OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    પૂર્ણાંક
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    પૂર્ણાંક		echo_style,
    પૂર્ણાંક		restore_copyID,
    પૂર્ણાંક		copyID)
{
    garray_T	join_ga;
    પૂર્ણાંક		retval;
    join_T	*p;
    પૂર્ણાંક		i;

    જો (TUPLE_LEN(tuple) < 1)
	પાછા OK; // nothing to do
    ga_init2(&join_ga, કદ(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    જો (join_ga.ga_data == ખાલી)
	પાછા retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    માટે (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    પાછા retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, પૂર્ણાંક copyID, પૂર્ણાંક restore_copyID)
{
    garray_T	ga;

    જો (tv->vval.v_tuple == ખાલી)
	પાછા ખાલી;
    ga_init2(&ga, કદ(અક્ષર), 80);
    ga_append(&ga, '(');
    જો (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	પાછા ખાલી;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    પાછા (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    શૂન્ય
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*પદાવલિ)
{
    પૂર્ણાંક		len = tuple_len(tuple);
    પૂર્ણાંક		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ચાવી, VAR_સંખ્યા);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_પદાવલિ_get_funccal(પદાવલિ, &newtv);

    માટે (પૂર્ણાંક idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_ચાવી, idx);
	જો (filter_map_one(TUPLE_ITEM(tuple, idx), પદાવલિ, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    તોડો;
    }

    જો (fc != ખાલી)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_ગણતરી(tuple_T *tuple, typval_T *needle, long idx, પૂર્ણાંક ic)
{
    long	n = 0;

    જો (tuple == ખાલી)
	પાછા 0;

    પૂર્ણાંક	len = TUPLE_LEN(tuple);
    જો (len == 0)
	પાછા 0;

    જો (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	પાછા 0;
    }

    માટે (પૂર્ણાંક i = idx; i < len; i++)
    {
	જો (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    પાછા n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    શૂન્ય
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    જો (rettv_list_alloc(rettv) == FAIL)
	પાછા;

    જો (tuple == ખાલી)
	પાછા;  // null tuple behaves like an empty list

    માટે (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	જો (l == ખાલી)
	    તોડો;

	જો (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    તોડો;
	}
	જો (list_append_સંખ્યા(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    તોડો;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    પૂર્ણાંક
index_tuple(tuple_T *tuple, typval_T *tv, પૂર્ણાંક start_idx, પૂર્ણાંક ic)
{
    જો (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	જો (start_idx < 0)
	    start_idx = 0;
    }

    માટે (પૂર્ણાંક idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	જો (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    પાછા idx;
    }

    પાછા -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    પૂર્ણાંક
indexof_tuple(tuple_T *tuple, long startidx, typval_T *પદાવલિ)
{
    long	idx = 0;
    પૂર્ણાંક		len;
    પૂર્ણાંક		found;

    જો (tuple == ખાલી)
	પાછા -1;

    len = TUPLE_LEN(tuple);

    જો (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	જો (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_ચાવી, VAR_સંખ્યા);

    પૂર્ણાંક		called_emsg_start = called_emsg;

    માટે (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_ચાવી, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_પદાવલિ(પદાવલિ);
	clear_tv(get_vim_var_tv(VV_VAL));

	જો (found)
	    પાછા idx;

	જો (called_emsg != called_emsg_start)
	    પાછા -1;
    }

    પાછા -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, પૂર્ણાંક domax)
{
    typval_T	*tv = ખાલી;
    પૂર્ણાંક		res;

    જો (tuple == ખાલી || TUPLE_LEN(tuple) == 0)
	પાછા ખાલી;

    માટે (પૂર્ણાંક idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	જો (tv == ખાલી)
	    tv = TUPLE_ITEM(tuple, idx);
	નહીંતર
	{
	    જો (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? પદાવલિ_GREATER : પદાવલિ_SMALLER, FALSE, &res) == FAIL)
		પાછા ખાલી;
	    જો (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    પાછા tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    શૂન્ય
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ખાલી;

    જો (tuple == ખાલી || TUPLE_LEN(tuple) == 0 || n <= 0)
	પાછા;

    જો (check_repeat_ગણતરી(TUPLE_LEN(tuple), n) == FAIL)
	પાછા;

    જો (rettv_tuple_set_with_વસ્તુઓ(rettv, TUPLE_LEN(tuple) * (પૂર્ણાંક)n) == FAIL)
	પાછા;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    માટે (પૂર્ણાંક ગણતરી = 0; ગણતરી < (પૂર્ણાંક)n; ગણતરી++)
    {
	માટે (પૂર્ણાંક idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_વસ્તુઓ.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    શૂન્ય
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ખાલી;

    પૂર્ણાંક	len = tuple_len(tuple);

    જો (len == 0)
	પાછા;

    જો (rettv_tuple_set_with_વસ્તુઓ(rettv, len) == FAIL)
	પાછા;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    માટે (પૂર્ણાંક i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_વસ્તુઓ.ga_len = tuple->tv_વસ્તુઓ.ga_len;
}

/*
 * Tuple reduce() function
 */
    શૂન્ય
tuple_reduce(typval_T *argvars, typval_T *પદાવલિ, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    પૂર્ણાંક		called_emsg_start = called_emsg;
    typval_T	initial;
    પૂર્ણાંક		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    પૂર્ણાંક		r;

    જો (argvars[2].v_type == VAR_UNKNOWN)
    {
	જો (tuple == ખાલી || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_કિંમત), "Tuple");
	    પાછા;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    નહીંતર
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    જો (tuple == ખાલી)
	પાછા;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_પદાવલિ_get_funccal(પદાવલિ, rettv);

    માટે ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_પદાવલિ_typval(પદાવલિ, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	જો (r == FAIL || called_emsg != called_emsg_start)
	    તોડો;
    }

    જો (fc != ખાલી)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    પૂર્ણાંક
check_tuples_addable(type_T *type1, type_T *type2)
{
    પૂર્ણાંક	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    જો ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    જો ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	જો (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	નહીંતર જો ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    જો (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	પાછા FAIL;
    }

    પાછા OK;
}

#endif // defined(FEAT_EVAL)
