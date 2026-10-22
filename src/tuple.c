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

#གལ་སྲིད defined(FEAT_EVAL)

// Tuple heads for garbage collection.
static tuple_T		*first_tuple = NULL;	// list of all tuples

    static void
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    གལ་སྲིད (first_tuple != NULL)
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
    གལ་སྲིད (tuple != NULL)
	tuple_init(tuple);
    ལོག tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_items(ཧྲིལ count)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    གལ་སྲིད (tuple == NULL)
	ལོག NULL;

    གལ་སྲིད (count <= 0)
	ལོག tuple;

    གལ་སྲིད (ga_grow(&tuple->tv_items, count) == FAIL)
    {
	tuple_free(tuple);
	ལོག NULL;
    }

    ལོག tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    void
tuple_set_item(tuple_T *tuple, ཧྲིལ idx, typval_T *tv)
{
    *TUPLE_ITEM(tuple, idx) = *tv;
    tuple->tv_items.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ཧྲིལ
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    གལ་སྲིད (tuple == NULL)
	ལོག FAIL;

    rettv->v_lock = 0;
    rettv_tuple_set(rettv, tuple);
    ལོག OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    void
rettv_tuple_set(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    གལ་སྲིད (tuple != NULL)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    ཧྲིལ
rettv_tuple_set_with_items(typval_T *rettv, ཧྲིལ count)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_items(count);
    གལ་སྲིད (new_tuple == NULL)
	ལོག FAIL;

    rettv_tuple_set(rettv, new_tuple);

    ལོག OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    void
tuple_unref(tuple_T *tuple)
{
    གལ་སྲིད (tuple != NULL && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    static void
tuple_free_contents(tuple_T *tuple)
{
    དོན་ལུ (ཧྲིལ i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_ITEM(tuple, i));

    ga_clear(&tuple->tv_items);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ཧྲིལ
tuple_free_nonref(ཧྲིལ copyID)
{
    tuple_T	*tt;
    ཧྲིལ		did_free = FALSE;

    དོན་ལུ (tt = first_tuple; tt != NULL; tt = tt->tv_used_next)
	གལ་སྲིད ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    ལོག did_free;
}

    static void
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    གལ་སྲིད (tuple->tv_used_prev == NULL)
	first_tuple = tuple->tv_used_next;
    དེ་མིན
	tuple->tv_used_prev->tv_used_next = tuple->tv_used_next;
    གལ་སྲིད (tuple->tv_used_next != NULL)
	tuple->tv_used_next->tv_used_prev = tuple->tv_used_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    void
tuple_free_items(ཧྲིལ copyID)
{
    tuple_T	*tt, *tt_next;

    དོན་ལུ (tt = first_tuple; tt != NULL; tt = tt_next)
    {
	tt_next = tt->tv_used_next;
	གལ་སྲིད ((tt->tv_copyID & COPYID_MASK) != (copyID & COPYID_MASK))
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
    གལ་སྲིད (in_free_unref_items)
	ལོག;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    long
tuple_len(tuple_T *tuple)
{
    གལ་སྲིད (tuple == NULL)
	ལོག 0L;
    ལོག tuple->tv_items.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    ཧྲིལ
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    ཧྲིལ		ic)	// ignore case for strings
{
    གལ་སྲིད (t1 == t2)
	ལོག TRUE;

    ཧྲིལ t1_len = tuple_len(t1);
    ཧྲིལ t2_len = tuple_len(t2);

    གལ་སྲིད (t1_len != t2_len)
	ལོག FALSE;

    གལ་སྲིད (t1_len == 0)
	// empty and NULL tuples are considered equal
	ལོག TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    དོན་ལུ (ཧྲིལ i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	གལ་སྲིད (!tv_equal(TUPLE_ITEM(t1, i), TUPLE_ITEM(t2, j), ic))
	    ལོག FALSE;

    ལོག TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_find(tuple_T *tuple, long n)
{
    གལ་སྲིད (tuple == NULL)
	ལོག NULL;

    // Negative index is relative to the end.
    གལ་སྲིད (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    གལ་སྲིད (n < 0 || n >= TUPLE_LEN(tuple))
	ལོག NULL;

    ལོག TUPLE_ITEM(tuple, n);
}

    ཧྲིལ
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    གལ་སྲིད (ga_grow_id(&tuple->tv_items, 1, aid_tuple_append) == FAIL)
	ལོག FAIL;

    tuple_set_item(tuple, TUPLE_LEN(tuple), tv);

    ལོག OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    ཧྲིལ
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    གལ་སྲིད (t1 == NULL)
	tuple = tuple_alloc();
    དེ་མིན
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    གལ་སྲིད (tuple == NULL)
	ལོག FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_lock = 0;
    tv->vval.v_tuple = tuple;
    གལ་སྲིད (t1 == NULL)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    དོན་ལུ (ཧྲིལ i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(t2, i), &new_tv);

	གལ་སྲིད (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    ལོག FAIL;
	}
    }

    ལོག OK;
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
    གལ་སྲིད (new_tuple == NULL)
	ལོག NULL;

    དོན་ལུ (ཧྲིལ i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_ITEM(tuple, i), &new_tv);

	གལ་སྲིད (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    ལོག NULL;
	}
    }

    ལོག new_tuple;
}

    ཧྲིལ
tuple_slice_or_index(
    tuple_T	*tuple,
    ཧྲིལ		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    ཧྲིལ		exclusive,
    typval_T	*rettv,
    ཧྲིལ		verbose)
{
    long	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    གལ་སྲིད (n1 < 0)
	n1 = len + n1;
    གལ་སྲིད (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	གལ་སྲིད (!range)
	{
	    གལ་སྲིད (verbose)
		semsg(_(e_tuple_index_out_of_range_nr), (long)n1_arg);
	    ལོག FAIL;
	}
	གལ་སྲིད (in_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	དེ་མིན
	    n1 = len;
    }
    གལ་སྲིད (range)
    {
	tuple_T	*new_tuple;

	གལ་སྲིད (n2 < 0)
	    n2 = len + n2;
	དེ་མིན གལ་སྲིད (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	གལ་སྲིད (exclusive)
	    --n2;
	གལ་སྲིད (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_slice(tuple, n1, n2);
	གལ་སྲིད (new_tuple == NULL)
	    ལོག FAIL;
	clear_tv(rettv);
	rettv_tuple_set(rettv, new_tuple);
    }
    དེ་མིན
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_find(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    ལོག OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, ཧྲིལ deep, ཧྲིལ top, ཧྲིལ copyID)
{
    tuple_T	*copy;
    ཧྲིལ		idx;

    གལ་སྲིད (orig == NULL)
	ལོག NULL;

    copy = tuple_alloc_with_items(TUPLE_LEN(orig));
    གལ་སྲིད (copy == NULL)
	ལོག NULL;

    གལ་སྲིད (orig->tv_type == NULL || top || deep)
	copy->tv_type = NULL;
    དེ་མིན
	copy->tv_type = alloc_type(orig->tv_type);
    གལ་སྲིད (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    དོན་ལུ (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_items.ga_len++;
	གལ་སྲིད (deep)
	{
	    གལ་སྲིད (item_copy(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx),
						deep, FALSE, copyID) == FAIL)
		bha_nga;
	}
	དེ་མིན
	    copy_tv(TUPLE_ITEM(orig, idx), TUPLE_ITEM(copy, idx));
    }

    ++copy->tv_refcount;
    གལ་སྲིད (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = NULL;
    }

    ལོག copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    ཧྲིལ
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ཧྲིལ do_error)
{
    ཧྲིལ		evaluate = evalarg == NULL ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = NULL;
    typval_T	tv;
    ཧྲིལ		vim9script = in_vim9script();
    ཧྲིལ		had_comma;

    གལ་སྲིད (check_typval_is_value(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	ལོག FAIL;
    }

    གལ་སྲིད (evaluate)
    {
	tuple = tuple_alloc();
	གལ་སྲིད (tuple == NULL)
	    ལོག FAIL;

	གལ་སྲིད (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    གལ་སྲིད (tuple_append_tv(tuple, rettv) == FAIL)
		goto failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    གལ་སྲིད (**arg == ')')
	// empty tuple
	goto done;

    གལ་སྲིད (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	goto failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    སྐབས (**arg != ')' && **arg != NUL)
    {
	གལ་སྲིད (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    goto failret;
	གལ་སྲིད (check_typval_is_value(&tv) == FAIL)
	{
	    གལ་སྲིད (evaluate)
		clear_tv(&tv);
	    goto failret;
	}

	གལ་སྲིད (evaluate)
	{
	    གལ་སྲིད (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		goto failret;
	    }
	}

	གལ་སྲིད (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	གལ་སྲིད (had_comma)
	{
	    གལ་སྲིད (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		goto failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	གལ་སྲིད (**arg == ')')
	    bha_nga;

	གལ་སྲིད (!had_comma)
	{
	    གལ་སྲིད (do_error)
	    {
		གལ་སྲིད (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		དེ་མིན
		    semsg(_(e_missing_comma_in_tuple_str), *arg);
	    }
	    goto failret;
	}
    }

    གལ་སྲིད (**arg != ')')
    {
	གལ་སྲིད (do_error)
	    semsg(_(e_missing_end_of_tuple_rsp_str), *arg);
failret:
	གལ་སྲིད (evaluate)
	    tuple_free(tuple);
	ལོག FAIL;
    }

done:
    *arg += 1;
    གལ་སྲིད (evaluate)
	rettv_tuple_set(rettv, tuple);

    ལོག OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    void
tuple_lock(tuple_T *tuple, ཧྲིལ deep, ཧྲིལ lock, ཧྲིལ check_refcount)
{
    གལ་སྲིད (tuple == NULL || (check_refcount && tuple->tv_refcount > 1))
	ལོག;

    གལ་སྲིད (lock)
	tuple->tv_lock |= VAR_LOCKED;
    དེ་མིན
	tuple->tv_lock &= ~VAR_LOCKED;

    གལ་སྲིད (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	དོན་ལུ (ཧྲིལ i = 0; i < TUPLE_LEN(tuple); i++)
	    item_lock(TUPLE_ITEM(tuple, i), deep - 1, lock, check_refcount);
    }
}

typedef sa_nracanaa join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    static ཧྲིལ
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    ཧྲིལ		echo_style,
    ཧྲིལ		restore_copyID,
    ཧྲིལ		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    ཧྲིལ		i;
    join_T	*p;
    ཧྲིལ		sumlen = 0;
    ཧྲིལ		first = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    དོན་ལུ (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_ITEM(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	གལ་སྲིད (s.string == NULL)
	    ལོག FAIL;

	s.length = STRLEN(s.string);
	sumlen += (ཧྲིལ)s.length;

	(void)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_data) + (join_gap->ga_len++);
	གལ་སྲིད (tofree != NULL || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.length = s.length;
	    p->tofree = tofree;
	}
	དེ་མིན
	{
	    p->s.string = vim_strnsave(s.string, s.length);
	    p->s.length = s.length;
	    p->tofree = p->s.string;
	}

	line_breakcheck();
	གལ་སྲིད (did_echo_string_emsg)  // recursion error, bail out
	    bha_nga;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    གལ་སྲིད (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	གལ་སྲིད (join_gap->ga_len >= 2)
	    sumlen += (ཧྲིལ)seplen * (join_gap->ga_len - 1);
    }
    གལ་སྲིད (ga_grow(gap, sumlen + 2) == FAIL)
	ལོག FAIL;

    དོན་ལུ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	གལ་སྲིད (first)
	    first = FALSE;
	དེ་མིན
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_data) + i;

	གལ་སྲིད (p->s.string != NULL)
	    ga_concat_len(gap, p->s.string, p->s.length);
	line_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    གལ་སྲིད (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    ལོག OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    ཧྲིལ
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    ཧྲིལ		echo_style,
    ཧྲིལ		restore_copyID,
    ཧྲིལ		copyID)
{
    garray_T	join_ga;
    ཧྲིལ		retval;
    join_T	*p;
    ཧྲིལ		i;

    གལ་སྲིད (TUPLE_LEN(tuple) < 1)
	ལོག OK; // nothing to do
    ga_init2(&join_ga, parimaa_na(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    གལ་སྲིད (join_ga.ga_data == NULL)
	ལོག retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_data;
    དོན་ལུ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ལོག retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, ཧྲིལ copyID, ཧྲིལ restore_copyID)
{
    garray_T	ga;

    གལ་སྲིད (tv->vval.v_tuple == NULL)
	ལོག NULL;
    ga_init2(&ga, parimaa_na(var_na), 80);
    ga_append(&ga, '(');
    གལ་སྲིད (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_data);
	ལོག NULL;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    ལོག (char_u *)ga.ga_data;
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
    ཧྲིལ		len = tuple_len(tuple);
    ཧྲིལ		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    set_vim_var_type(VV_KEY, VAR_NUMBER);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, &newtv);

    དོན་ལུ (ཧྲིལ idx = 0; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	གལ་སྲིད (filter_map_one(TUPLE_ITEM(tuple, idx), expr, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    bha_nga;
    }

    གལ་སྲིད (fc != NULL)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    long
tuple_count(tuple_T *tuple, typval_T *needle, long idx, ཧྲིལ ic)
{
    long	n = 0;

    གལ་སྲིད (tuple == NULL)
	ལོག 0;

    ཧྲིལ	len = TUPLE_LEN(tuple);
    གལ་སྲིད (len == 0)
	ལོག 0;

    གལ་སྲིད (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_index_out_of_range_nr), idx);
	ལོག 0;
    }

    དོན་ལུ (ཧྲིལ i = idx; i < len; i++)
    {
	གལ་སྲིད (tv_equal(TUPLE_ITEM(tuple, i), needle, ic))
	    ++n;
    }

    ལོག n;
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

    གལ་སྲིད (rettv_list_alloc(rettv) == FAIL)
	ལོག;

    གལ་སྲིད (tuple == NULL)
	ལོག;  // null tuple behaves like an empty list

    དོན་ལུ (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	གལ་སྲིད (l == NULL)
	    bha_nga;

	གལ་སྲིད (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    bha_nga;
	}
	གལ་སྲིད (list_append_number(l, idx) == FAIL
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
    ཧྲིལ
index_tuple(tuple_T *tuple, typval_T *tv, ཧྲིལ start_idx, ཧྲིལ ic)
{
    གལ་སྲིད (start_idx < 0)
    {
	start_idx = TUPLE_LEN(tuple) + start_idx;
	གལ་སྲིད (start_idx < 0)
	    start_idx = 0;
    }

    དོན་ལུ (ཧྲིལ idx = start_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	གལ་སྲིད (tv_equal(TUPLE_ITEM(tuple, idx), tv, ic))
	    ལོག idx;
    }

    ལོག -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    ཧྲིལ
indexof_tuple(tuple_T *tuple, long startidx, typval_T *expr)
{
    long	idx = 0;
    ཧྲིལ		len;
    ཧྲིལ		found;

    གལ་སྲིད (tuple == NULL)
	ལོག -1;

    len = TUPLE_LEN(tuple);

    གལ་སྲིད (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	གལ་སྲིད (startidx < 0)
	    startidx = 0;
    }

    set_vim_var_type(VV_KEY, VAR_NUMBER);

    ཧྲིལ		called_emsg_start = called_emsg;

    དོན་ལུ (idx = startidx; idx < len; idx++)
    {
	set_vim_var_nr(VV_KEY, idx);
	copy_tv(TUPLE_ITEM(tuple, idx), get_vim_var_tv(VV_VAL));

	found = indexof_eval_expr(expr);
	clear_tv(get_vim_var_tv(VV_VAL));

	གལ་སྲིད (found)
	    ལོག idx;

	གལ་སྲིད (called_emsg != called_emsg_start)
	    ལོག -1;
    }

    ལོག -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, ཧྲིལ domax)
{
    typval_T	*tv = NULL;
    ཧྲིལ		res;

    གལ་སྲིད (tuple == NULL || TUPLE_LEN(tuple) == 0)
	ལོག NULL;

    དོན་ལུ (ཧྲིལ idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	གལ་སྲིད (tv == NULL)
	    tv = TUPLE_ITEM(tuple, idx);
	དེ་མིན
	{
	    གལ་སྲིད (typval_compare2(TUPLE_ITEM(tuple, idx), tv,
		    domax ? EXPR_GREATER : EXPR_SMALLER, FALSE, &res) == FAIL)
		ལོག NULL;
	    གལ་སྲིད (res == OK)
		tv = TUPLE_ITEM(tuple, idx);
	}
    }

    ལོག tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    void
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = NULL;

    གལ་སྲིད (tuple == NULL || TUPLE_LEN(tuple) == 0 || n <= 0)
	ལོག;

    གལ་སྲིད (check_repeat_count(TUPLE_LEN(tuple), n) == FAIL)
	ལོག;

    གལ་སྲིད (rettv_tuple_set_with_items(rettv, TUPLE_LEN(tuple) * (ཧྲིལ)n) == FAIL)
	ལོག;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    དོན་ལུ (ཧྲིལ count = 0; count < (ཧྲིལ)n; count++)
    {
	དོན་ལུ (ཧྲིལ idx = 0; idx < TUPLE_LEN(tuple); idx++)
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

    ཧྲིལ	len = tuple_len(tuple);

    གལ་སྲིད (len == 0)
	ལོག;

    གལ་སྲིད (rettv_tuple_set_with_items(rettv, len) == FAIL)
	ལོག;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    དོན་ལུ (ཧྲིལ i = 0; i < len; i++)
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
    ཧྲིལ		called_emsg_start = called_emsg;
    typval_T	initial;
    ཧྲིལ		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    ཧྲིལ		r;

    གལ་སྲིད (argvars[2].v_type == VAR_UNKNOWN)
    {
	གལ་སྲིད (tuple == NULL || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_of_an_empty_str_with_no_initial_value), "Tuple");
	    ལོག;
	}
	initial = *TUPLE_ITEM(tuple, 0);
	idx = 1;
    }
    དེ་མིན
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    གལ་སྲིད (tuple == NULL)
	ལོག;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_expr_get_funccal(expr, rettv);

    དོན་ལུ ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_ITEM(tuple, idx);

	r = eval_expr_typval(expr, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	གལ་སྲིད (r == FAIL || called_emsg != called_emsg_start)
	    bha_nga;
    }

    གལ་སྲིད (fc != NULL)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    ཧྲིལ
check_tuples_addable(type_T *type1, type_T *type2)
{
    ཧྲིལ	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    གལ་སྲིད ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    གལ་སྲིད ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	གལ་སྲིད (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	དེ་མིན གལ་སྲིད ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_args[0], type2->tt_args[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    གལ་སྲིད (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_in_concatenation));
	ལོག FAIL;
    }

    ལོག OK;
}

#endif // defined(FEAT_EVAL)
