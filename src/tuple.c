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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_EVAL)

// Tuple heads for garbage collection.
ସ୍ଥିର tuple_T		*first_tuple = ଖାଲି;	// list of all tuples

    ସ୍ଥିର ଶୂନ୍ୟ
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    ଯଦି (first_tuple != ଖାଲି)
	first_tuple->tv_used_prev = tuple;
    tuple->tv_used_prev = ଖାଲି;
    tuple->tv_used_next = first_tuple;
    first_tuple = tuple;

    ga_init2(&tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ, ଆକାର(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(ଶୂନ୍ୟ)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    ଯଦି (tuple != ଖାଲି)
	tuple_init(tuple);
    ଫେରନ୍ତୁ tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (ଗଣନା_2 <= 0)
	ଫେରନ୍ତୁ tuple;

    ଯଦି (ga_grow(&tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ, ଗଣନା_2) == FAIL)
    {
	tuple_free(tuple);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଫେରନ୍ତୁ tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    ଶୂନ୍ୟ
tuple_set_item(tuple_T *tuple, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    ଫେରନ୍ତୁ OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    ଶୂନ୍ୟ
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    ଯଦି (tuple != ଖାଲି)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
rettv_tuple_set_with_ବସ୍ତୁଗୁଡ଼ିକ(typval_T *rettv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(ଗଣନା_2);
    ଯଦି (new_tuple == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    rettv_tuple_set(rettv, new_tuple);

    ଫେରନ୍ତୁ OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    ଶୂନ୍ୟ
tuple_unref(tuple_T *tuple)
{
    ଯଦି (tuple != ଖାଲି && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tuple_free_contents(tuple_T *tuple)
{
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tuple_free_nonref(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    tuple_T	*tt;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_free = FALSE;

    ପାଇଁ (tt = first_tuple; tt != ଖାଲି; tt = tt->tv_used_next)
	ଯଦି ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ଫେରନ୍ତୁ did_free;
}

    ସ୍ଥିର ଶୂନ୍ୟ
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    ଯଦି (tuple->tv_used_prev == ଖାଲି)
	first_tuple = tuple->tv_used_next;
    ଅନ୍ୟଥା
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    ଯଦି (tuple->tv_used_next != ଖାଲି)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    ଶୂନ୍ୟ
tuple_free_ବସ୍ତୁଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    tuple_T	*tt, *tt_next;

    ପାଇଁ (tt = first_tuple; tt != ଖାଲି; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	ଯଦି ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    ଶୂନ୍ୟ
tuple_free(tuple_T *tuple)
{
    ଯଦି (in_free_unref_ବସ୍ତୁଗୁଡ଼ିକ)
	ଫେରନ୍ତୁ;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ 0L;
    ଫେରନ୍ତୁ tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ic)	// ignore case for strings
{
    ଯଦି (t1 == t2)
	ଫେରନ୍ତୁ TRUE;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା t1_len = tuple_len(t1);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା t2_len = tuple_len(t2);

    ଯଦି (t1_len != t2_len)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (t1_len == 0)
	// empty and NULL tuples are considered equal
	ଫେରନ୍ତୁ TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	ଯଦି (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    ଫେରନ୍ତୁ FALSE;

    ଫେରନ୍ତୁ TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    // Negative index is relative to the end.
    ଯଦି (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    ଯଦି (n < 0 || n >= TUPLE_LEN(tuple))
	ଫେରନ୍ତୁ ଖାଲି;

    ଫେରନ୍ତୁ TUPLE_ITEM(tuple, n);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    ଯଦି (ga_grow_id(&tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ, 1, aid_tuple_append) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    ଫେରନ୍ତୁ OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    ଯଦି (t1 == ଖାଲି)
	tuple = tuple_alloc();
    ଅନ୍ୟଥା
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    ଯଦି (t1 == ଖାଲି)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	ଯଦି (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ଫେରନ୍ତୁ FAIL;
	}
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_slice(tuple_T *tuple, long n1, long n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(n2 - n1 + 1);
    ଯଦି (new_tuple == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	ଯଦି (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }

    ଫେରନ୍ତୁ new_tuple;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tuple_slice_or_index(
    tuple_T	*tuple,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		exclusive,
    typval_T	*rettv,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ଯଦି (n1 < 0)
	n1 = len + n1;
    ଯଦି (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	ଯଦି (!range)
	{
	    ଯଦି (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    ଫେରନ୍ତୁ FAIL;
	}
	ଯଦି (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ଅନ୍ୟଥା
	    n1 = len;
    }
    ଯଦି (range)
    {
	tuple_T	*new_tuple;

	ଯଦି (n2 < 0)
	    n2 = len + n2;
	ଅନ୍ୟଥା ଯଦି (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ଯଦି (exclusive)
	    --n2;
	ଯଦି (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	ଯଦି (new_tuple == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    ଅନ୍ୟଥା
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ଫେରନ୍ତୁ OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, ପୂର୍ଣ୍ଣସଂଖ୍ୟା deep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଶୀର୍ଷ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID)
{
    tuple_T	*copy;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx;

    ଯଦି (orig == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    copy = tuple_alloc_with_ବସ୍ତୁଗୁଡ଼ିକ(TUPLE_LEN(orig));
    ଯଦି (copy == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (orig->tv_type == ଖାଲି || ଶୀର୍ଷ || deep)
	copy->tv_type = ଖାଲି;
    ଅନ୍ୟଥା
	copy->tv_type = alloc_type(orig->tv_type);
    ଯଦି (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    ପାଇଁ (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_ବସ୍ତୁଗୁଡ଼ିକ.ga_len++;
	ଯଦି (deep)
	{
	    ଯଦି (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    ଯଦି (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = ଖାଲି;
    }

    ଫେରନ୍ତୁ copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_error)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		evaluate = evalarg == ଖାଲି ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = ଖାଲି;
    typval_T	tv;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		vim9script = in_vim9script();
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		had_comma;

    ଯଦି (check_typval_is_ମୂଲ୍ୟ(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (evaluate)
    {
	tuple = tuple_alloc();
	ଯଦି (tuple == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	ଯଦି (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    ଯଦି (tuple_append_tv(tuple, rettv) == FAIL)
		ଯାଅ failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    ଯଦି (**arg == ')')
	// empty tuple
	ଯାଅ done;

    ଯଦି (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	ଯାଅ failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ଯେତେବେଳେ (**arg != ')' && **arg != NUL)
    {
	ଯଦି (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    ଯାଅ failret;
	ଯଦି (check_typval_is_ମୂଲ୍ୟ(&tv) == FAIL)
	{
	    ଯଦି (evaluate)
		clear_tv(&tv);
	    ଯାଅ failret;
	}

	ଯଦି (evaluate)
	{
	    ଯଦି (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		ଯାଅ failret;
	    }
	}

	ଯଦି (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ଯଦି (had_comma)
	{
	    ଯଦି (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ଯାଅ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ଯଦି (**arg == ')')
	    ଭାଙ୍ଗ;

	ଯଦି (!had_comma)
	{
	    ଯଦି (do_error)
	    {
		ଯଦି (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ଅନ୍ୟଥା
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    ଯାଅ failret;
	}
    }

    ଯଦି (**arg != ')')
    {
	ଯଦି (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	ଯଦି (evaluate)
	    tuple_free(tuple);
	ଫେରନ୍ତୁ FAIL;
    }

done:
    *arg += 1;
    ଯଦି (evaluate)
	rettv_tuple_set(rettv, tuple);

    ଫେରନ୍ତୁ OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    ଶୂନ୍ୟ
tuple_lock(tuple_T *tuple, ପୂର୍ଣ୍ଣସଂଖ୍ୟା deep, ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock, ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_refcount)
{
    ଯଦି (tuple == ଖାଲି || (check_refcount && tuple->tv_refcount > 1))
	ଫେରନ୍ତୁ;

    ଯଦି (lock)
	tuple->tv_lock |= VAR_LOCKED;
    ଅନ୍ୟଥା
	tuple->tv_lock &= ~VAR_LOCKED;

    ଯଦି (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		echo_style,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    join_T	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		sumlen = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    ପାଇଁ (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ଯଦି (s.string == ଖାଲି)
	    ଫେରନ୍ତୁ FAIL;

	s.length = STRLEN(s.string);
	sumlen += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)s.length;

	(ଶୂନ୍ୟ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	ଯଦି (tofree != ଖାଲି || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	ଅନ୍ୟଥା
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	ଧାଡ଼ି_breakcheck();
	ଯଦି (did_echo_string_emsg)  // recursion error, bail out
	    ଭାଙ୍ଗ;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    ଯଦି (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	ଯଦି (join_gap->ga_len >= 2)
	    sumlen += (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)seplen * (join_gap->ga_len - 1);
    }
    ଯଦି (ga_grow(gap, sumlen + 2) == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ପାଇଁ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ଯଦି (first)
	    first = FALSE;
	ଅନ୍ୟଥା
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	ଯଦି (p->s.string != ଖାଲି)
	    ga_concat_len(gap, p->s.string, p->s.length);
	ଧାଡ଼ି_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    ଯଦି (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ଫେରନ୍ତୁ OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		echo_style,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		restore_copyID,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		copyID)
{
    garray_T	join_ga;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;
    join_T	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଯଦି (TUPLE_LEN(tuple) < 1)
	ଫେରନ୍ତୁ OK; // nothing to do
    ga_init2(&join_ga, ଆକାର(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ଯଦି (join_ga.ga_data == ଖାଲି)
	ଫେରନ୍ତୁ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    ପାଇଁ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ଫେରନ୍ତୁ retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା copyID, ପୂର୍ଣ୍ଣସଂଖ୍ୟା restore_copyID)
{
    garray_T	ga;

    ଯଦି (tv->vval.v_tuple == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ga_init2(&ga, ଆକାର(ଅକ୍ଷର), 80);
    ga_append(&ga, '(');
    ଯଦି (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ଫେରନ୍ତୁ (char_u *)ga.ga_data;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    ଶୂନ୍ୟ
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*ପଦସମଷ୍ଟି)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len = tuple_len(tuple);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_ଚାବି, VAR_ସଂଖ୍ୟା);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ପଦସମଷ୍ଟି_get_funccal(ପଦସମଷ୍ଟି, &newtv);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_ଚାବି, idx);
	ଯଦି (filter_map_one(TUPLE_ITEM(tuple, idx), ପଦସମଷ୍ଟି, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (fc != ଖାଲି)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_ଗଣନା(tuple_T *tuple, typval_T *needle, long idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ic)
{
    long	n = 0;

    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ 0;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = TUPLE_LEN(tuple);
    ଯଦି (len == 0)
	ଫେରନ୍ତୁ 0;

    ଯଦି (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	ଫେରନ୍ତୁ 0;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = idx; i < len; i++)
    {
	ଯଦି (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    ଫେରନ୍ତୁ n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    ଶୂନ୍ୟ
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    ଯଦି (rettv_list_alloc(rettv) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ;  // null tuple behaves like an empty list

    ପାଇଁ (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	ଯଦି (l == ଖାଲି)
	    ଭାଙ୍ଗ;

	ଯଦି (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ଭାଙ୍ଗ;
	}
	ଯଦି (list_append_ସଂଖ୍ୟା(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ITEM(tuple, idx)) == FAIL)
	    ଭାଙ୍ଗ;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
index_tuple(tuple_T *tuple, typval_T *tv, ପୂର୍ଣ୍ଣସଂଖ୍ୟା start_idx, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ic)
{
    ଯଦି (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	ଯଦି (start_idx < 0)
	    start_idx = 0;
    }

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	ଯଦି (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    ଫେରନ୍ତୁ idx;
    }

    ଫେରନ୍ତୁ -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
indexof_tuple(tuple_T *tuple, long startidx, typval_T *ପଦସମଷ୍ଟି)
{
    long	idx = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		found;

    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    len = TUPLE_LEN(tuple);

    ଯଦି (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	ଯଦି (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_ଚାବି, VAR_ସଂଖ୍ୟା);

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_start = called_emsg;

    ପାଇଁ (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_ଚାବି, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_ପଦସମଷ୍ଟି(ପଦସମଷ୍ଟି);
	clear_tv(get_vim_var_tv(VV_VAL));

	ଯଦି (found)
	    ଫେରନ୍ତୁ idx;

	ଯଦି (called_emsg != called_emsg_start)
	    ଫେରନ୍ତୁ -1;
    }

    ଫେରନ୍ତୁ -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, ପୂର୍ଣ୍ଣସଂଖ୍ୟା domax)
{
    typval_T	*tv = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		res;

    ଯଦି (tuple == ଖାଲି || TUPLE_LEN(tuple) == 0)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	ଯଦି (tv == ଖାଲି)
	    tv = TUPLE_ITEM(tuple, idx);
	ଅନ୍ୟଥା
	{
	    ଯଦି (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? ପଦସମଷ୍ଟି_GREATER : ପଦସମଷ୍ଟି_SMALLER, FALSE, &res) == FAIL)
		ଫେରନ୍ତୁ ଖାଲି;
	    ଯଦି (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    ଫେରନ୍ତୁ tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    ଶୂନ୍ୟ
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ଖାଲି;

    ଯଦି (tuple == ଖାଲି || TUPLE_LEN(tuple) == 0 || n <= 0)
	ଫେରନ୍ତୁ;

    ଯଦି (check_repeat_ଗଣନା(TUPLE_LEN(tuple), n) == FAIL)
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_tuple_set_with_ବସ୍ତୁଗୁଡ଼ିକ(rettv, TUPLE_LEN(tuple) * (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n) == FAIL)
	ଫେରନ୍ତୁ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଣନା_2 = 0; ଗଣନା_2 < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)n; ଗଣନା_2++)
    {
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ITEM(tuple, idx),
		    TUPLE_ITEM(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    ଶୂନ୍ୟ
tuple_reverse(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ଖାଲି;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	len = tuple_len(tuple);

    ଯଦି (len == 0)
	ଫେରନ୍ତୁ;

    ଯଦି (rettv_tuple_set_with_ବସ୍ତୁଗୁଡ଼ିକ(rettv, len) == FAIL)
	ଫେରନ୍ତୁ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; i++)
	copy_tv(TUPLE_ITEM(tuple, i), TUPLE_ITEM(new_tuple, len - i - 1));
    new_tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ.ga_len = tuple->tv_ବସ୍ତୁଗୁଡ଼ିକ.ga_len;
}

/*
 * Tuple reduce() function
 */
    ଶୂନ୍ୟ
tuple_reduce(typval_T *argvars, typval_T *ପଦସମଷ୍ଟି, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		called_emsg_start = called_emsg;
    typval_T	initial;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		r;

    ଯଦି (argvars[2].v_type == VAR_UNKNOWN)
    {
	ଯଦି (tuple == ଖାଲି || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_ମୂଲ୍ୟ), "Tuple");
	    ଫେରନ୍ତୁ;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    ଅନ୍ୟଥା
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    ଯଦି (tuple == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ପଦସମଷ୍ଟି_get_funccal(ପଦସମଷ୍ଟି, rettv);

    ପାଇଁ ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_ପଦସମଷ୍ଟି_typval(ପଦସମଷ୍ଟି, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	ଯଦି (r == FAIL || called_emsg != called_emsg_start)
	    ଭାଙ୍ଗ;
    }

    ଯଦି (fc != ଖାଲି)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
check_tuples_addable(type_T *type1, type_T *type2)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    ଯଦି ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    ଯଦି ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	ଯଦି (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	ଅନ୍ୟଥା ଯଦି ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    ଯଦି (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}

#endif // defined(FEAT_EVAL)
