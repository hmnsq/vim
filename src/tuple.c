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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಆದರೆ defined(FEAT_EVAL)

// Tuple heads for garbage collection.
ಸ್ಥಿರ tuple_T		*ಮೊದಲ_tuple = ಖಾಲಿ;	// list of all tuples

    ಸ್ಥಿರ ಶೂನ್ಯ
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    ಆದರೆ (ಮೊದಲ_tuple != ಖಾಲಿ)
	ಮೊದಲ_tuple->tv_ಬಳಸಿದ_prev = tuple;
    tuple->tv_ಬಳಸಿದ_prev = ಖಾಲಿ;
    tuple->tv_ಬಳಸಿದ_ಮುಂದಿನ = ಮೊದಲ_tuple;
    ಮೊದಲ_tuple = tuple;

    ga_init2(&tuple->tv_ವಸ್ತುಗಳು, ಗಾತ್ರ(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(ಶೂನ್ಯ)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    ಆದರೆ (tuple != ಖಾಲಿ)
	tuple_init(tuple);
    ಹಿಂತಿರುಗಿ tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (ಎಣಿಕೆ_2 <= 0)
	ಹಿಂತಿರುಗಿ tuple;

    ಆದರೆ (ga_grow(&tuple->tv_ವಸ್ತುಗಳು, ಎಣಿಕೆ_2) == FAIL)
    {
	tuple_free(tuple);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    ಶೂನ್ಯ
tuple_ಗಣ_ಅಂಶ(tuple_T *tuple, ಪೂರ್ಣಾಂಕ idx, typval_T *tv)
{
    *TUPLE_ಅಂಶ(tuple, idx) = *tv;
    tuple->tv_ವಸ್ತುಗಳು.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    rettv->v_ಬೀಗ = 0;
    rettv_tuple_ಗಣ(rettv, tuple);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    ಶೂನ್ಯ
rettv_tuple_ಗಣ(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    ಆದರೆ (tuple != ಖಾಲಿ)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    ಪೂರ್ಣಾಂಕ
rettv_tuple_ಗಣ_with_ವಸ್ತುಗಳು(typval_T *rettv, ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_ವಸ್ತುಗಳು(ಎಣಿಕೆ_2);
    ಆದರೆ (new_tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    rettv_tuple_ಗಣ(rettv, new_tuple);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    ಶೂನ್ಯ
tuple_unref(tuple_T *tuple)
{
    ಆದರೆ (tuple != ಖಾಲಿ && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tuple_free_contents(tuple_T *tuple)
{
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ಅಂಶ(tuple, i));

    ga_clear(&tuple->tv_ವಸ್ತುಗಳು);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ಪೂರ್ಣಾಂಕ
tuple_free_nonref(ಪೂರ್ಣಾಂಕ copyID)
{
    tuple_T	*tt;
    ಪೂರ್ಣಾಂಕ		did_free = FALSE;

    ಗಾಗಿ (tt = ಮೊದಲ_tuple; tt != ಖಾಲಿ; tt = tt->tv_ಬಳಸಿದ_ಮುಂದಿನ)
	ಆದರೆ ((tt->tv_copyID & COPYID_ಮುಖವಾಡ) != (copyID & COPYID_ಮುಖವಾಡ))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ಹಿಂತಿರುಗಿ did_free;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    ಆದರೆ (tuple->tv_ಬಳಸಿದ_prev == ಖಾಲಿ)
	ಮೊದಲ_tuple = tuple->tv_ಬಳಸಿದ_ಮುಂದಿನ;
    ಇಲ್ಲದಿದ್ದರೆ
	tuple->tv_ಬಳಸಿದ_prev->tv_ಬಳಸಿದ_ಮುಂದಿನ = tuple->tv_ಬಳಸಿದ_ಮುಂದಿನ;
    ಆದರೆ (tuple->tv_ಬಳಸಿದ_ಮುಂದಿನ != ಖಾಲಿ)
	tuple->tv_ಬಳಸಿದ_ಮುಂದಿನ->tv_ಬಳಸಿದ_prev = tuple->tv_ಬಳಸಿದ_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    ಶೂನ್ಯ
tuple_free_ವಸ್ತುಗಳು(ಪೂರ್ಣಾಂಕ copyID)
{
    tuple_T	*tt, *tt_ಮುಂದಿನ;

    ಗಾಗಿ (tt = ಮೊದಲ_tuple; tt != ಖಾಲಿ; tt = tt_ಮುಂದಿನ)
    {
	tt_ಮುಂದಿನ = tt->tv_ಬಳಸಿದ_ಮುಂದಿನ;
	ಆದರೆ ((tt->tv_copyID & COPYID_ಮುಖವಾಡ) != (copyID & COPYID_ಮುಖವಾಡ))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    ಶೂನ್ಯ
tuple_free(tuple_T *tuple)
{
    ಆದರೆ (ಒಳ_free_unref_ವಸ್ತುಗಳು)
	ಹಿಂತಿರುಗಿ;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    ದೀರ್ಘ
tuple_len(tuple_T *tuple)
{
    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0L;
    ಹಿಂತಿರುಗಿ tuple->tv_ವಸ್ತುಗಳು.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    ಪೂರ್ಣಾಂಕ
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    ಪೂರ್ಣಾಂಕ		ic)	// ignore case for strings
{
    ಆದರೆ (t1 == t2)
	ಹಿಂತಿರುಗಿ TRUE;

    ಪೂರ್ಣಾಂಕ t1_len = tuple_len(t1);
    ಪೂರ್ಣಾಂಕ t2_len = tuple_len(t2);

    ಆದರೆ (t1_len != t2_len)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (t1_len == 0)
	// empty and NULL tuples are considered equal
	ಹಿಂತಿರುಗಿ TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	ಆದರೆ (!tv_equal(TUPLE_ಅಂಶ(t1, i), TUPLE_ಅಂಶ(t2, j), ic))
	    ಹಿಂತಿರುಗಿ FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_ಕಂಡುಹಿಡಿ(tuple_T *tuple, ದೀರ್ಘ n)
{
    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    // Negative index is relative to the end.
    ಆದರೆ (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    ಆದರೆ (n < 0 || n >= TUPLE_LEN(tuple))
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಹಿಂತಿರುಗಿ TUPLE_ಅಂಶ(tuple, n);
}

    ಪೂರ್ಣಾಂಕ
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    ಆದರೆ (ga_grow_id(&tuple->tv_ವಸ್ತುಗಳು, 1, aid_tuple_append) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    tuple_ಗಣ_ಅಂಶ(tuple, TUPLE_LEN(tuple), tv);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    ಪೂರ್ಣಾಂಕ
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    ಆದರೆ (t1 == ಖಾಲಿ)
	tuple = tuple_alloc();
    ಇಲ್ಲದಿದ್ದರೆ
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_ಬೀಗ = 0;
    tv->vval.v_tuple = tuple;
    ಆದರೆ (t1 == ಖಾಲಿ)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ಅಂಶ(t2, i), &new_tv);

	ಆದರೆ (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_ತುಂಡು(tuple_T *tuple, ದೀರ್ಘ n1, ದೀರ್ಘ n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_ವಸ್ತುಗಳು(n2 - n1 + 1);
    ಆದರೆ (new_tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ಅಂಶ(tuple, i), &new_tv);

	ಆದರೆ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }

    ಹಿಂತಿರುಗಿ new_tuple;
}

    ಪೂರ್ಣಾಂಕ
tuple_ತುಂಡು_or_ಸೂಚ್ಯಂಕ(
    tuple_T	*tuple,
    ಪೂರ್ಣಾಂಕ		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    ಪೂರ್ಣಾಂಕ		exclusive,
    typval_T	*rettv,
    ಪೂರ್ಣಾಂಕ		verbose)
{
    ದೀರ್ಘ	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ಆದರೆ (n1 < 0)
	n1 = len + n1;
    ಆದರೆ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	ಆದರೆ (!range)
	{
	    ಆದರೆ (verbose)
		semsg(_(e_tuple_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), (ದೀರ್ಘ)n1_arg);
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಆದರೆ (ಒಳ_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ಇಲ್ಲದಿದ್ದರೆ
	    n1 = len;
    }
    ಆದರೆ (range)
    {
	tuple_T	*new_tuple;

	ಆದರೆ (n2 < 0)
	    n2 = len + n2;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ಆದರೆ (exclusive)
	    --n2;
	ಆದರೆ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_ತುಂಡು(tuple, n1, n2);
	ಆದರೆ (new_tuple == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;
	clear_tv(rettv);
	rettv_tuple_ಗಣ(rettv, new_tuple);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_ಕಂಡುಹಿಡಿ(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, ಪೂರ್ಣಾಂಕ deep, ಪೂರ್ಣಾಂಕ ತುದಿ, ಪೂರ್ಣಾಂಕ copyID)
{
    tuple_T	*copy;
    ಪೂರ್ಣಾಂಕ		idx;

    ಆದರೆ (orig == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    copy = tuple_alloc_with_ವಸ್ತುಗಳು(TUPLE_LEN(orig));
    ಆದರೆ (copy == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (orig->tv_type == ಖಾಲಿ || ತುದಿ || deep)
	copy->tv_type = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	copy->tv_type = alloc_type(orig->tv_type);
    ಆದರೆ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    ಗಾಗಿ (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_ವಸ್ತುಗಳು.ga_len++;
	ಆದರೆ (deep)
	{
	    ಆದರೆ (ಅಂಶ_copy(TUPLE_ಅಂಶ(orig, idx), TUPLE_ಅಂಶ(copy, idx),
						deep, FALSE, copyID) == FAIL)
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    copy_tv(TUPLE_ಅಂಶ(orig, idx), TUPLE_ಅಂಶ(copy, idx));
    }

    ++copy->tv_refcount;
    ಆದರೆ (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ಪೂರ್ಣಾಂಕ do_error)
{
    ಪೂರ್ಣಾಂಕ		evaluate = evalarg == ಖಾಲಿ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = ಖಾಲಿ;
    typval_T	tv;
    ಪೂರ್ಣಾಂಕ		vim9script = ಒಳ_vim9script();
    ಪೂರ್ಣಾಂಕ		had_comma;

    ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (evaluate)
    {
	tuple = tuple_alloc();
	ಆದರೆ (tuple == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	ಆದರೆ (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    ಆದರೆ (tuple_append_tv(tuple, rettv) == FAIL)
		ಹೋಗು failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    ಆದರೆ (**arg == ')')
	// empty tuple
	ಹೋಗು ಮುಗಿದ;

    ಆದರೆ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	ಹೋಗು failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ಆಗ (**arg != ')' && **arg != NUL)
    {
	ಆದರೆ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    ಹೋಗು failret;
	ಆದರೆ (check_typval_is_ಮೌಲ್ಯ(&tv) == FAIL)
	{
	    ಆದರೆ (evaluate)
		clear_tv(&tv);
	    ಹೋಗು failret;
	}

	ಆದರೆ (evaluate)
	{
	    ಆದರೆ (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		ಹೋಗು failret;
	    }
	}

	ಆದರೆ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ಆದರೆ (had_comma)
	{
	    ಆದರೆ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ಹೋಗು failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ಆದರೆ (**arg == ')')
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (!had_comma)
	{
	    ಆದರೆ (do_error)
	    {
		ಆದರೆ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ಇಲ್ಲದಿದ್ದರೆ
		    semsg(_(e_missing_comma_ಒಳ_tuple_str), *arg);
	    }
	    ಹೋಗು failret;
	}
    }

    ಆದರೆ (**arg != ')')
    {
	ಆದರೆ (do_error)
	    semsg(_(e_missing_ಕೊನೆ_ದ_tuple_rsp_str), *arg);
failret:
	ಆದರೆ (evaluate)
	    tuple_free(tuple);
	ಹಿಂತಿರುಗಿ FAIL;
    }

ಮುಗಿದ:
    *arg += 1;
    ಆದರೆ (evaluate)
	rettv_tuple_ಗಣ(rettv, tuple);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    ಶೂನ್ಯ
tuple_ಬೀಗ(tuple_T *tuple, ಪೂರ್ಣಾಂಕ deep, ಪೂರ್ಣಾಂಕ ಬೀಗ, ಪೂರ್ಣಾಂಕ check_refcount)
{
    ಆದರೆ (tuple == ಖಾಲಿ || (check_refcount && tuple->tv_refcount > 1))
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (ಬೀಗ)
	tuple->tv_ಬೀಗ |= VAR_LOCKED;
    ಇಲ್ಲದಿದ್ದರೆ
	tuple->tv_ಬೀಗ &= ~VAR_LOCKED;

    ಆದರೆ (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < TUPLE_LEN(tuple); i++)
	    ಅಂಶ_ಬೀಗ(TUPLE_ಅಂಶ(tuple, i), deep - 1, ಬೀಗ, check_refcount);
    }
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    ಪೂರ್ಣಾಂಕ		echo_style,
    ಪೂರ್ಣಾಂಕ		restore_copyID,
    ಪೂರ್ಣಾಂಕ		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    ಪೂರ್ಣಾಂಕ		i;
    join_T	*p;
    ಪೂರ್ಣಾಂಕ		sumlen = 0;
    ಪೂರ್ಣಾಂಕ		ಮೊದಲ = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    ಗಾಗಿ (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ಅಂಶ(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ಆದರೆ (s.string == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ FAIL;

	s.ಉದ್ದಳತೆ = STRLEN(s.string);
	sumlen += (ಪೂರ್ಣಾಂಕ)s.ಉದ್ದಳತೆ;

	(ಶೂನ್ಯ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_ದತ್ತಾಂಶ) + (join_gap->ga_len++);
	ಆದರೆ (tofree != ಖಾಲಿ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.ಉದ್ದಳತೆ = s.ಉದ್ದಳತೆ;
	    p->tofree = tofree;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    p->s.string = vim_strnsave(s.string, s.ಉದ್ದಳತೆ);
	    p->s.ಉದ್ದಳತೆ = s.ಉದ್ದಳತೆ;
	    p->tofree = p->s.string;
	}

	ಸಾಲು_breakcheck();
	ಆದರೆ (did_echo_string_emsg)  // recursion error, bail out
	    ನಿಲ್ಲಿಸು;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    ಆದರೆ (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	ಆದರೆ (join_gap->ga_len >= 2)
	    sumlen += (ಪೂರ್ಣಾಂಕ)seplen * (join_gap->ga_len - 1);
    }
    ಆದರೆ (ga_grow(gap, sumlen + 2) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಗಾಗಿ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ಆದರೆ (ಮೊದಲ)
	    ಮೊದಲ = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_ದತ್ತಾಂಶ) + i;

	ಆದರೆ (p->s.string != ಖಾಲಿ)
	    ga_concat_len(gap, p->s.string, p->s.ಉದ್ದಳತೆ);
	ಸಾಲು_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    ಆದರೆ (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    ಪೂರ್ಣಾಂಕ
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    ಪೂರ್ಣಾಂಕ		echo_style,
    ಪೂರ್ಣಾಂಕ		restore_copyID,
    ಪೂರ್ಣಾಂಕ		copyID)
{
    garray_T	join_ga;
    ಪೂರ್ಣಾಂಕ		retval;
    join_T	*p;
    ಪೂರ್ಣಾಂಕ		i;

    ಆದರೆ (TUPLE_LEN(tuple) < 1)
	ಹಿಂತಿರುಗಿ OK; // nothing to do
    ga_init2(&join_ga, ಗಾತ್ರ(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ಆದರೆ (join_ga.ga_ದತ್ತಾಂಶ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_ದತ್ತಾಂಶ;
    ಗಾಗಿ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, ಪೂರ್ಣಾಂಕ copyID, ಪೂರ್ಣಾಂಕ restore_copyID)
{
    garray_T	ga;

    ಆದರೆ (tv->vval.v_tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ga_init2(&ga, ಗಾತ್ರ(ಅಕ್ಷರ), 80);
    ga_append(&ga, '(');
    ಆದರೆ (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ಹಿಂತಿರುಗಿ (char_u *)ga.ga_ದತ್ತಾಂಶ;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    ಶೂನ್ಯ
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*ಅಭಿವ್ಯಕ್ತಿ)
{
    ಪೂರ್ಣಾಂಕ		len = tuple_len(tuple);
    ಪೂರ್ಣಾಂಕ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    ಗಣ_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, &newtv);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < len; idx++)
    {
	ಗಣ_vim_var_nr(VV_ಕೀಲಿ, idx);
	ಆದರೆ (filter_map_one(TUPLE_ಅಂಶ(tuple, idx), ಅಭಿವ್ಯಕ್ತಿ, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (fc != ಖಾಲಿ)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ದೀರ್ಘ
tuple_ಎಣಿಕೆ(tuple_T *tuple, typval_T *needle, ದೀರ್ಘ idx, ಪೂರ್ಣಾಂಕ ic)
{
    ದೀರ್ಘ	n = 0;

    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

    ಪೂರ್ಣಾಂಕ	len = TUPLE_LEN(tuple);
    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ 0;

    ಆದರೆ (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_ಸೂಚ್ಯಂಕ_ಹೊರಹೋಗು_ದ_range_nr), idx);
	ಹಿಂತಿರುಗಿ 0;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = idx; i < len; i++)
    {
	ಆದರೆ (tv_equal(TUPLE_ಅಂಶ(tuple, i), needle, ic))
	    ++n;
    }

    ಹಿಂತಿರುಗಿ n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    ಶೂನ್ಯ
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    ಆದರೆ (rettv_list_alloc(rettv) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;  // null tuple behaves like an empty list

    ಗಾಗಿ (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	ಆದರೆ (l == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (list_append_ಸಂಖ್ಯೆ(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_ಅಂಶ(tuple, idx)) == FAIL)
	    ನಿಲ್ಲಿಸು;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    ಪೂರ್ಣಾಂಕ
ಸೂಚ್ಯಂಕ_tuple(tuple_T *tuple, typval_T *tv, ಪೂರ್ಣಾಂಕ ಆರಂಭ_idx, ಪೂರ್ಣಾಂಕ ic)
{
    ಆದರೆ (ಆರಂಭ_idx < 0)
    {
	ಆರಂಭ_idx = TUPLE_LEN(tuple) + ಆರಂಭ_idx;
	ಆದರೆ (ಆರಂಭ_idx < 0)
	    ಆರಂಭ_idx = 0;
    }

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = ಆರಂಭ_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	ಆದರೆ (tv_equal(TUPLE_ಅಂಶ(tuple, idx), tv, ic))
	    ಹಿಂತಿರುಗಿ idx;
    }

    ಹಿಂತಿರುಗಿ -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ಪೂರ್ಣಾಂಕ
indexof_tuple(tuple_T *tuple, ದೀರ್ಘ startidx, typval_T *ಅಭಿವ್ಯಕ್ತಿ)
{
    ದೀರ್ಘ	idx = 0;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		ಸಿಕ್ಕಿತು;

    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    len = TUPLE_LEN(tuple);

    ಆದರೆ (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	ಆದರೆ (startidx < 0)
	    startidx = 0;
    }

    ಗಣ_vim_var_type(VV_ಕೀಲಿ, VAR_ಸಂಖ್ಯೆ);

    ಪೂರ್ಣಾಂಕ		called_emsg_ಆರಂಭ = called_emsg;

    ಗಾಗಿ (idx = startidx; idx < len; idx++)
    {
	ಗಣ_vim_var_nr(VV_ಕೀಲಿ, idx);
	copy_tv(TUPLE_ಅಂಶ(tuple, idx), get_vim_var_tv(VV_VAL));

	ಸಿಕ್ಕಿತು = indexof_eval_ಅಭಿವ್ಯಕ್ತಿ(ಅಭಿವ್ಯಕ್ತಿ);
	clear_tv(get_vim_var_tv(VV_VAL));

	ಆದರೆ (ಸಿಕ್ಕಿತು)
	    ಹಿಂತಿರುಗಿ idx;

	ಆದರೆ (called_emsg != called_emsg_ಆರಂಭ)
	    ಹಿಂತಿರುಗಿ -1;
    }

    ಹಿಂತಿರುಗಿ -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, ಪೂರ್ಣಾಂಕ domax)
{
    typval_T	*tv = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		res;

    ಆದರೆ (tuple == ಖಾಲಿ || TUPLE_LEN(tuple) == 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	ಆದರೆ (tv == ಖಾಲಿ)
	    tv = TUPLE_ಅಂಶ(tuple, idx);
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (typval_compare2(TUPLE_ಅಂಶ(tuple, idx), tv,
		    domax ? ಅಭಿವ್ಯಕ್ತಿ_GREATER : ಅಭಿವ್ಯಕ್ತಿ_SMALLER, FALSE, &res) == FAIL)
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    ಆದರೆ (res == OK)
		tv = TUPLE_ಅಂಶ(tuple, idx);
	}
    }

    ಹಿಂತಿರುಗಿ tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    ಶೂನ್ಯ
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ಖಾಲಿ;

    ಆದರೆ (tuple == ಖಾಲಿ || TUPLE_LEN(tuple) == 0 || n <= 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (check_repeat_ಎಣಿಕೆ(TUPLE_LEN(tuple), n) == FAIL)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_tuple_ಗಣ_with_ವಸ್ತುಗಳು(rettv, TUPLE_LEN(tuple) * (ಪೂರ್ಣಾಂಕ)n) == FAIL)
	ಹಿಂತಿರುಗಿ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಎಣಿಕೆ_2 = 0; ಎಣಿಕೆ_2 < (ಪೂರ್ಣಾಂಕ)n; ಎಣಿಕೆ_2++)
    {
	ಗಾಗಿ (ಪೂರ್ಣಾಂಕ idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_ಅಂಶ(tuple, idx),
		    TUPLE_ಅಂಶ(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_ವಸ್ತುಗಳು.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    ಶೂನ್ಯ
tuple_ಹಿಮ್ಮುಖ(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ಖಾಲಿ;

    ಪೂರ್ಣಾಂಕ	len = tuple_len(tuple);

    ಆದರೆ (len == 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (rettv_tuple_ಗಣ_with_ವಸ್ತುಗಳು(rettv, len) == FAIL)
	ಹಿಂತಿರುಗಿ;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < len; i++)
	copy_tv(TUPLE_ಅಂಶ(tuple, i), TUPLE_ಅಂಶ(new_tuple, len - i - 1));
    new_tuple->tv_ವಸ್ತುಗಳು.ga_len = tuple->tv_ವಸ್ತುಗಳು.ga_len;
}

/*
 * Tuple reduce() function
 */
    ಶೂನ್ಯ
tuple_reduce(typval_T *argvars, typval_T *ಅಭಿವ್ಯಕ್ತಿ, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    ಪೂರ್ಣಾಂಕ		called_emsg_ಆರಂಭ = called_emsg;
    typval_T	initial;
    ಪೂರ್ಣಾಂಕ		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    ಪೂರ್ಣಾಂಕ		r;

    ಆದರೆ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ಆದರೆ (tuple == ಖಾಲಿ || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_ದ_an_ಖಾಲಿಗಳು_str_with_no_initial_ಮೌಲ್ಯ), "Tuple");
	    ಹಿಂತಿರುಗಿ;
	}
	initial = *TUPLE_ಅಂಶ(tuple, 0);
	idx = 1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    ಆದರೆ (tuple == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ಅಭಿವ್ಯಕ್ತಿ_get_funccal(ಅಭಿವ್ಯಕ್ತಿ, rettv);

    ಗಾಗಿ ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ಅಂಶ(tuple, idx);

	r = eval_ಅಭಿವ್ಯಕ್ತಿ_typval(ಅಭಿವ್ಯಕ್ತಿ, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	ಆದರೆ (r == FAIL || called_emsg != called_emsg_ಆರಂಭ)
	    ನಿಲ್ಲಿಸು;
    }

    ಆದರೆ (fc != ಖಾಲಿ)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    ಪೂರ್ಣಾಂಕ
check_tuples_addable(type_T *type1, type_T *type2)
{
    ಪೂರ್ಣಾಂಕ	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    ಆದರೆ ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    ಆದರೆ ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	ಆದರೆ (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_ವಾದಗಳು[0], type2->tt_ವಾದಗಳು[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    ಆದರೆ (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_ಒಳ_concatenation));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_EVAL)
