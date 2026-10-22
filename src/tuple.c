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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

// Tuple heads for garbage collection.
స్థిర tuple_T		*మొదటి_tuple = ఖాళీ;	// list of all tuples

    స్థిర శూన్య
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    ఒకవేళ (మొదటి_tuple != ఖాళీ)
	మొదటి_tuple->tv_వాడిన_prev = tuple;
    tuple->tv_వాడిన_prev = ఖాళీ;
    tuple->tv_వాడిన_తదుపరి = మొదటి_tuple;
    మొదటి_tuple = tuple;

    ga_init2(&tuple->tv_వస్తువులు, పరిమాణం(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(శూన్య)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    ఒకవేళ (tuple != ఖాళీ)
	tuple_init(tuple);
    తిరిగి tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_వస్తువులు(పూర్ణాంకం లెక్క)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (లెక్క <= 0)
	తిరిగి tuple;

    ఒకవేళ (ga_grow(&tuple->tv_వస్తువులు, లెక్క) == FAIL)
    {
	tuple_free(tuple);
	తిరిగి ఖాళీ;
    }

    తిరిగి tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    శూన్య
tuple_సమితి_అంశం(tuple_T *tuple, పూర్ణాంకం idx, typval_T *tv)
{
    *TUPLE_అంశం(tuple, idx) = *tv;
    tuple->tv_వస్తువులు.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి FAIL;

    rettv->v_తాళం = 0;
    rettv_tuple_సమితి(rettv, tuple);
    తిరిగి OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    శూన్య
rettv_tuple_సమితి(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    ఒకవేళ (tuple != ఖాళీ)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    పూర్ణాంకం
rettv_tuple_సమితి_with_వస్తువులు(typval_T *rettv, పూర్ణాంకం లెక్క)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_వస్తువులు(లెక్క);
    ఒకవేళ (new_tuple == ఖాళీ)
	తిరిగి FAIL;

    rettv_tuple_సమితి(rettv, new_tuple);

    తిరిగి OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    శూన్య
tuple_unref(tuple_T *tuple)
{
    ఒకవేళ (tuple != ఖాళీ && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    స్థిర శూన్య
tuple_free_contents(tuple_T *tuple)
{
    కొరకు (పూర్ణాంకం i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_అంశం(tuple, i));

    ga_clear(&tuple->tv_వస్తువులు);
}

/*
 * Go through the list of tuples and free items without the copyID.
 * But don't free a tuple that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    పూర్ణాంకం
tuple_free_nonref(పూర్ణాంకం copyID)
{
    tuple_T	*tt;
    పూర్ణాంకం		did_free = FALSE;

    కొరకు (tt = మొదటి_tuple; tt != ఖాళీ; tt = tt->tv_వాడిన_తదుపరి)
	ఒకవేళ ((tt->tv_copyID & COPYID_ముసుగు) != (copyID & COPYID_ముసుగు))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    తిరిగి did_free;
}

    స్థిర శూన్య
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    ఒకవేళ (tuple->tv_వాడిన_prev == ఖాళీ)
	మొదటి_tuple = tuple->tv_వాడిన_తదుపరి;
    లేకపోతే
	tuple->tv_వాడిన_prev->tv_వాడిన_తదుపరి = tuple->tv_వాడిన_తదుపరి;
    ఒకవేళ (tuple->tv_వాడిన_తదుపరి != ఖాళీ)
	tuple->tv_వాడిన_తదుపరి->tv_వాడిన_prev = tuple->tv_వాడిన_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    శూన్య
tuple_free_వస్తువులు(పూర్ణాంకం copyID)
{
    tuple_T	*tt, *tt_తదుపరి;

    కొరకు (tt = మొదటి_tuple; tt != ఖాళీ; tt = tt_తదుపరి)
    {
	tt_తదుపరి = tt->tv_వాడిన_తదుపరి;
	ఒకవేళ ((tt->tv_copyID & COPYID_ముసుగు) != (copyID & COPYID_ముసుగు))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    శూన్య
tuple_free(tuple_T *tuple)
{
    ఒకవేళ (లోపల_free_unref_వస్తువులు)
	తిరిగి;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    దీర్ఘ
tuple_len(tuple_T *tuple)
{
    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి 0L;
    తిరిగి tuple->tv_వస్తువులు.ga_len;
}

/*
 * Return TRUE when two tuples have exactly the same values.
 */
    పూర్ణాంకం
tuple_equal(
    tuple_T	*t1,
    tuple_T	*t2,
    పూర్ణాంకం		ic)	// ignore case for strings
{
    ఒకవేళ (t1 == t2)
	తిరిగి TRUE;

    పూర్ణాంకం t1_len = tuple_len(t1);
    పూర్ణాంకం t2_len = tuple_len(t2);

    ఒకవేళ (t1_len != t2_len)
	తిరిగి FALSE;

    ఒకవేళ (t1_len == 0)
	// empty and NULL tuples are considered equal
	తిరిగి TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    కొరకు (పూర్ణాంకం i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	ఒకవేళ (!tv_equal(TUPLE_అంశం(t1, i), TUPLE_అంశం(t2, j), ic))
	    తిరిగి FALSE;

    తిరిగి TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_కనుగొను(tuple_T *tuple, దీర్ఘ n)
{
    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి ఖాళీ;

    // Negative index is relative to the end.
    ఒకవేళ (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    ఒకవేళ (n < 0 || n >= TUPLE_LEN(tuple))
	తిరిగి ఖాళీ;

    తిరిగి TUPLE_అంశం(tuple, n);
}

    పూర్ణాంకం
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    ఒకవేళ (ga_grow_id(&tuple->tv_వస్తువులు, 1, aid_tuple_append) == FAIL)
	తిరిగి FAIL;

    tuple_సమితి_అంశం(tuple, TUPLE_LEN(tuple), tv);

    తిరిగి OK;
}

/*
 * Concatenate tuples "t1" and "t2" into a new tuple, stored in "tv".
 * Return FAIL when out of memory.
 */
    పూర్ణాంకం
tuple_concat(tuple_T *t1, tuple_T *t2, typval_T *tv)
{
    tuple_T	*tuple;

    // make a copy of the first tuple.
    ఒకవేళ (t1 == ఖాళీ)
	tuple = tuple_alloc();
    లేకపోతే
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_తాళం = 0;
    tv->vval.v_tuple = tuple;
    ఒకవేళ (t1 == ఖాళీ)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    కొరకు (పూర్ణాంకం i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_అంశం(t2, i), &new_tv);

	ఒకవేళ (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    తిరిగి FAIL;
	}
    }

    తిరిగి OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_ముక్క(tuple_T *tuple, దీర్ఘ n1, దీర్ఘ n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_వస్తువులు(n2 - n1 + 1);
    ఒకవేళ (new_tuple == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (పూర్ణాంకం i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_అంశం(tuple, i), &new_tv);

	ఒకవేళ (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    తిరిగి ఖాళీ;
	}
    }

    తిరిగి new_tuple;
}

    పూర్ణాంకం
tuple_ముక్క_or_సూచీ(
    tuple_T	*tuple,
    పూర్ణాంకం		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    పూర్ణాంకం		exclusive,
    typval_T	*rettv,
    పూర్ణాంకం		verbose)
{
    దీర్ఘ	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ఒకవేళ (n1 < 0)
	n1 = len + n1;
    ఒకవేళ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	ఒకవేళ (!range)
	{
	    ఒకవేళ (verbose)
		semsg(_(e_tuple_సూచీ_బహిర్గతం_యొక్క_range_nr), (దీర్ఘ)n1_arg);
	    తిరిగి FAIL;
	}
	ఒకవేళ (లోపల_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	లేకపోతే
	    n1 = len;
    }
    ఒకవేళ (range)
    {
	tuple_T	*new_tuple;

	ఒకవేళ (n2 < 0)
	    n2 = len + n2;
	లేకపోతే ఒకవేళ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ఒకవేళ (exclusive)
	    --n2;
	ఒకవేళ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_ముక్క(tuple, n1, n2);
	ఒకవేళ (new_tuple == ఖాళీ)
	    తిరిగి FAIL;
	clear_tv(rettv);
	rettv_tuple_సమితి(rettv, new_tuple);
    }
    లేకపోతే
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_కనుగొను(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    తిరిగి OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, పూర్ణాంకం deep, పూర్ణాంకం శిఖరం, పూర్ణాంకం copyID)
{
    tuple_T	*copy;
    పూర్ణాంకం		idx;

    ఒకవేళ (orig == ఖాళీ)
	తిరిగి ఖాళీ;

    copy = tuple_alloc_with_వస్తువులు(TUPLE_LEN(orig));
    ఒకవేళ (copy == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (orig->tv_type == ఖాళీ || శిఖరం || deep)
	copy->tv_type = ఖాళీ;
    లేకపోతే
	copy->tv_type = alloc_type(orig->tv_type);
    ఒకవేళ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    కొరకు (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_వస్తువులు.ga_len++;
	ఒకవేళ (deep)
	{
	    ఒకవేళ (అంశం_copy(TUPLE_అంశం(orig, idx), TUPLE_అంశం(copy, idx),
						deep, FALSE, copyID) == FAIL)
		ఆపు;
	}
	లేకపోతే
	    copy_tv(TUPLE_అంశం(orig, idx), TUPLE_అంశం(copy, idx));
    }

    ++copy->tv_refcount;
    ఒకవేళ (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = ఖాళీ;
    }

    తిరిగి copy;
}

/*
 * Allocate a variable for a tuple and fill it from "*arg".
 * "*arg" points to the "," after the first element.
 * "rettv" contains the first element.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
eval_tuple(char_u **arg, typval_T *rettv, evalarg_T *evalarg, పూర్ణాంకం do_error)
{
    పూర్ణాంకం		evaluate = evalarg == ఖాళీ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = ఖాళీ;
    typval_T	tv;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    పూర్ణాంకం		had_comma;

    ఒకవేళ (check_typval_is_విలువ(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	తిరిగి FAIL;
    }

    ఒకవేళ (evaluate)
    {
	tuple = tuple_alloc();
	ఒకవేళ (tuple == ఖాళీ)
	    తిరిగి FAIL;

	ఒకవేళ (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    ఒకవేళ (tuple_append_tv(tuple, rettv) == FAIL)
		వెళ్ళు failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    ఒకవేళ (**arg == ')')
	// empty tuple
	వెళ్ళు పూర్తి;

    ఒకవేళ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	వెళ్ళు failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    అయితే (**arg != ')' && **arg != NUL)
    {
	ఒకవేళ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    వెళ్ళు failret;
	ఒకవేళ (check_typval_is_విలువ(&tv) == FAIL)
	{
	    ఒకవేళ (evaluate)
		clear_tv(&tv);
	    వెళ్ళు failret;
	}

	ఒకవేళ (evaluate)
	{
	    ఒకవేళ (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		వెళ్ళు failret;
	    }
	}

	ఒకవేళ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ఒకవేళ (had_comma)
	{
	    ఒకవేళ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		వెళ్ళు failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ఒకవేళ (**arg == ')')
	    ఆపు;

	ఒకవేళ (!had_comma)
	{
	    ఒకవేళ (do_error)
	    {
		ఒకవేళ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		లేకపోతే
		    semsg(_(e_missing_comma_లోపల_tuple_str), *arg);
	    }
	    వెళ్ళు failret;
	}
    }

    ఒకవేళ (**arg != ')')
    {
	ఒకవేళ (do_error)
	    semsg(_(e_missing_చివర_యొక్క_tuple_rsp_str), *arg);
failret:
	ఒకవేళ (evaluate)
	    tuple_free(tuple);
	తిరిగి FAIL;
    }

పూర్తి:
    *arg += 1;
    ఒకవేళ (evaluate)
	rettv_tuple_సమితి(rettv, tuple);

    తిరిగి OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    శూన్య
tuple_తాళం(tuple_T *tuple, పూర్ణాంకం deep, పూర్ణాంకం తాళం, పూర్ణాంకం check_refcount)
{
    ఒకవేళ (tuple == ఖాళీ || (check_refcount && tuple->tv_refcount > 1))
	తిరిగి;

    ఒకవేళ (తాళం)
	tuple->tv_తాళం |= VAR_LOCKED;
    లేకపోతే
	tuple->tv_తాళం &= ~VAR_LOCKED;

    ఒకవేళ (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	కొరకు (పూర్ణాంకం i = 0; i < TUPLE_LEN(tuple); i++)
	    అంశం_తాళం(TUPLE_అంశం(tuple, i), deep - 1, తాళం, check_refcount);
    }
}

రకనిర్వచనం నిర్మాణం join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    స్థిర పూర్ణాంకం
tuple_join_inner(
    garray_T	*gap,		// to store the result in
    tuple_T	*tuple,
    char_u	*sep,
    పూర్ణాంకం		echo_style,
    పూర్ణాంకం		restore_copyID,
    పూర్ణాంకం		copyID,
    garray_T	*join_gap)	// to keep each tuple item string
{
    పూర్ణాంకం		i;
    join_T	*p;
    పూర్ణాంకం		sumlen = 0;
    పూర్ణాంకం		మొదటి = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    కొరకు (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_అంశం(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ఒకవేళ (s.string == ఖాళీ)
	    తిరిగి FAIL;

	s.నిడివి = STRLEN(s.string);
	sumlen += (పూర్ణాంకం)s.నిడివి;

	(శూన్య)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_దత్తాంశం) + (join_gap->ga_len++);
	ఒకవేళ (tofree != ఖాళీ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.నిడివి = s.నిడివి;
	    p->tofree = tofree;
	}
	లేకపోతే
	{
	    p->s.string = vim_strnsave(s.string, s.నిడివి);
	    p->s.నిడివి = s.నిడివి;
	    p->tofree = p->s.string;
	}

	పంక్తి_breakcheck();
	ఒకవేళ (did_echo_string_emsg)  // recursion error, bail out
	    ఆపు;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    ఒకవేళ (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	ఒకవేళ (join_gap->ga_len >= 2)
	    sumlen += (పూర్ణాంకం)seplen * (join_gap->ga_len - 1);
    }
    ఒకవేళ (ga_grow(gap, sumlen + 2) == FAIL)
	తిరిగి FAIL;

    కొరకు (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ఒకవేళ (మొదటి)
	    మొదటి = FALSE;
	లేకపోతే
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_దత్తాంశం) + i;

	ఒకవేళ (p->s.string != ఖాళీ)
	    ga_concat_len(gap, p->s.string, p->s.నిడివి);
	పంక్తి_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    ఒకవేళ (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    తిరిగి OK;
}

/*
 * Join tuple "tuple" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a Tuple.
 * Return FAIL or OK.
 */
    పూర్ణాంకం
tuple_join(
    garray_T	*gap,
    tuple_T	*tuple,
    char_u	*sep,
    పూర్ణాంకం		echo_style,
    పూర్ణాంకం		restore_copyID,
    పూర్ణాంకం		copyID)
{
    garray_T	join_ga;
    పూర్ణాంకం		retval;
    join_T	*p;
    పూర్ణాంకం		i;

    ఒకవేళ (TUPLE_LEN(tuple) < 1)
	తిరిగి OK; // nothing to do
    ga_init2(&join_ga, పరిమాణం(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ఒకవేళ (join_ga.ga_దత్తాంశం == ఖాళీ)
	తిరిగి retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_దత్తాంశం;
    కొరకు (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    తిరిగి retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, పూర్ణాంకం copyID, పూర్ణాంకం restore_copyID)
{
    garray_T	ga;

    ఒకవేళ (tv->vval.v_tuple == ఖాళీ)
	తిరిగి ఖాళీ;
    ga_init2(&ga, పరిమాణం(అక్షరం), 80);
    ga_append(&ga, '(');
    ఒకవేళ (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_దత్తాంశం);
	తిరిగి ఖాళీ;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    తిరిగి (char_u *)ga.ga_దత్తాంశం;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    శూన్య
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*సమాసం)
{
    పూర్ణాంకం		len = tuple_len(tuple);
    పూర్ణాంకం		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    సమితి_vim_var_type(VV_కీలకం, VAR_సంఖ్య);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_సమాసం_get_funccal(సమాసం, &newtv);

    కొరకు (పూర్ణాంకం idx = 0; idx < len; idx++)
    {
	సమితి_vim_var_nr(VV_కీలకం, idx);
	ఒకవేళ (filter_map_one(TUPLE_అంశం(tuple, idx), సమాసం, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    ఆపు;
    }

    ఒకవేళ (fc != ఖాళీ)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    దీర్ఘ
tuple_లెక్క(tuple_T *tuple, typval_T *needle, దీర్ఘ idx, పూర్ణాంకం ic)
{
    దీర్ఘ	n = 0;

    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి 0;

    పూర్ణాంకం	len = TUPLE_LEN(tuple);
    ఒకవేళ (len == 0)
	తిరిగి 0;

    ఒకవేళ (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_సూచీ_బహిర్గతం_యొక్క_range_nr), idx);
	తిరిగి 0;
    }

    కొరకు (పూర్ణాంకం i = idx; i < len; i++)
    {
	ఒకవేళ (tv_equal(TUPLE_అంశం(tuple, i), needle, ic))
	    ++n;
    }

    తిరిగి n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    శూన్య
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి;  // null tuple behaves like an empty list

    కొరకు (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	ఒకవేళ (l == ఖాళీ)
	    ఆపు;

	ఒకవేళ (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    ఆపు;
	}
	ఒకవేళ (list_append_సంఖ్య(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_అంశం(tuple, idx)) == FAIL)
	    ఆపు;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    పూర్ణాంకం
సూచీ_tuple(tuple_T *tuple, typval_T *tv, పూర్ణాంకం ప్రారంభం_idx, పూర్ణాంకం ic)
{
    ఒకవేళ (ప్రారంభం_idx < 0)
    {
	ప్రారంభం_idx = TUPLE_LEN(tuple) + ప్రారంభం_idx;
	ఒకవేళ (ప్రారంభం_idx < 0)
	    ప్రారంభం_idx = 0;
    }

    కొరకు (పూర్ణాంకం idx = ప్రారంభం_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	ఒకవేళ (tv_equal(TUPLE_అంశం(tuple, idx), tv, ic))
	    తిరిగి idx;
    }

    తిరిగి -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    పూర్ణాంకం
indexof_tuple(tuple_T *tuple, దీర్ఘ startidx, typval_T *సమాసం)
{
    దీర్ఘ	idx = 0;
    పూర్ణాంకం		len;
    పూర్ణాంకం		దొరికింది;

    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి -1;

    len = TUPLE_LEN(tuple);

    ఒకవేళ (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	ఒకవేళ (startidx < 0)
	    startidx = 0;
    }

    సమితి_vim_var_type(VV_కీలకం, VAR_సంఖ్య);

    పూర్ణాంకం		called_emsg_ప్రారంభం = called_emsg;

    కొరకు (idx = startidx; idx < len; idx++)
    {
	సమితి_vim_var_nr(VV_కీలకం, idx);
	copy_tv(TUPLE_అంశం(tuple, idx), get_vim_var_tv(VV_VAL));

	దొరికింది = indexof_eval_సమాసం(సమాసం);
	clear_tv(get_vim_var_tv(VV_VAL));

	ఒకవేళ (దొరికింది)
	    తిరిగి idx;

	ఒకవేళ (called_emsg != called_emsg_ప్రారంభం)
	    తిరిగి -1;
    }

    తిరిగి -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, పూర్ణాంకం domax)
{
    typval_T	*tv = ఖాళీ;
    పూర్ణాంకం		res;

    ఒకవేళ (tuple == ఖాళీ || TUPLE_LEN(tuple) == 0)
	తిరిగి ఖాళీ;

    కొరకు (పూర్ణాంకం idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	ఒకవేళ (tv == ఖాళీ)
	    tv = TUPLE_అంశం(tuple, idx);
	లేకపోతే
	{
	    ఒకవేళ (typval_compare2(TUPLE_అంశం(tuple, idx), tv,
		    domax ? సమాసం_GREATER : సమాసం_SMALLER, FALSE, &res) == FAIL)
		తిరిగి ఖాళీ;
	    ఒకవేళ (res == OK)
		tv = TUPLE_అంశం(tuple, idx);
	}
    }

    తిరిగి tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    శూన్య
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ఖాళీ;

    ఒకవేళ (tuple == ఖాళీ || TUPLE_LEN(tuple) == 0 || n <= 0)
	తిరిగి;

    ఒకవేళ (check_repeat_లెక్క(TUPLE_LEN(tuple), n) == FAIL)
	తిరిగి;

    ఒకవేళ (rettv_tuple_సమితి_with_వస్తువులు(rettv, TUPLE_LEN(tuple) * (పూర్ణాంకం)n) == FAIL)
	తిరిగి;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    కొరకు (పూర్ణాంకం లెక్క = 0; లెక్క < (పూర్ణాంకం)n; లెక్క++)
    {
	కొరకు (పూర్ణాంకం idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_అంశం(tuple, idx),
		    TUPLE_అంశం(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_వస్తువులు.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    శూన్య
tuple_తిరగేయి(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ఖాళీ;

    పూర్ణాంకం	len = tuple_len(tuple);

    ఒకవేళ (len == 0)
	తిరిగి;

    ఒకవేళ (rettv_tuple_సమితి_with_వస్తువులు(rettv, len) == FAIL)
	తిరిగి;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    కొరకు (పూర్ణాంకం i = 0; i < len; i++)
	copy_tv(TUPLE_అంశం(tuple, i), TUPLE_అంశం(new_tuple, len - i - 1));
    new_tuple->tv_వస్తువులు.ga_len = tuple->tv_వస్తువులు.ga_len;
}

/*
 * Tuple reduce() function
 */
    శూన్య
tuple_reduce(typval_T *argvars, typval_T *సమాసం, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    పూర్ణాంకం		called_emsg_ప్రారంభం = called_emsg;
    typval_T	initial;
    పూర్ణాంకం		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    పూర్ణాంకం		r;

    ఒకవేళ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ఒకవేళ (tuple == ఖాళీ || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_యొక్క_an_ఖాళీలు_str_with_no_initial_విలువ), "Tuple");
	    తిరిగి;
	}
	initial = *TUPLE_అంశం(tuple, 0);
	idx = 1;
    }
    లేకపోతే
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_సమాసం_get_funccal(సమాసం, rettv);

    కొరకు ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_అంశం(tuple, idx);

	r = eval_సమాసం_typval(సమాసం, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	ఒకవేళ (r == FAIL || called_emsg != called_emsg_ప్రారంభం)
	    ఆపు;
    }

    ఒకవేళ (fc != ఖాళీ)
	remove_funccal();
}

/*
 * Returns TRUE if two tuples with types "type1" and "type2" are addable.
 * Otherwise returns FALSE.
 */
    పూర్ణాంకం
check_tuples_addable(type_T *type1, type_T *type2)
{
    పూర్ణాంకం	addable = TRUE;

    // If the first operand is a variadic tuple and the second argument is
    // non-variadic, then concatenation is not possible.
    ఒకవేళ ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    ఒకవేళ ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	ఒకవేళ (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	లేకపోతే ఒకవేళ ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_వాదనలు[0], type2->tt_వాదనలు[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    ఒకవేళ (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_లోపల_concatenation));
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

#అయితే_ముగింపు // defined(FEAT_EVAL)
