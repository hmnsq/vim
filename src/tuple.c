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
ساکن tuple_T		*پہلا_tuple = تہی;	// list of all tuples

    ساکن خالی
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    اگر (پہلا_tuple != تہی)
	پہلا_tuple->tv_مستعمل_پچھلا = tuple;
    tuple->tv_مستعمل_پچھلا = تہی;
    tuple->tv_مستعمل_اگلا = پہلا_tuple;
    پہلا_tuple = tuple;

    ga_init2(&tuple->tv_اشیاء, حجم(typval_T), 20);
}

/*
 * Allocate an empty header for a tuple.
 * Caller should take care of the reference count.
 */
    tuple_T *
tuple_alloc(خالی)
{
    tuple_T  *tuple;

    tuple = ALLOC_CLEAR_ONE(tuple_T);
    اگر (tuple != تہی)
	tuple_init(tuple);
    واپس tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_اشیاء(صحیح گنتی)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    اگر (tuple == تہی)
	واپس تہی;

    اگر (گنتی <= 0)
	واپس tuple;

    اگر (ga_grow(&tuple->tv_اشیاء, گنتی) == FAIL)
    {
	tuple_free(tuple);
	واپس تہی;
    }

    واپس tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    خالی
tuple_سیٹ_شے(tuple_T *tuple, صحیح idx, typval_T *tv)
{
    *TUPLE_شے(tuple, idx) = *tv;
    tuple->tv_اشیاء.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحیح
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    اگر (tuple == تہی)
	واپس FAIL;

    rettv->v_تالا = 0;
    rettv_tuple_سیٹ(rettv, tuple);
    واپس OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    خالی
rettv_tuple_سیٹ(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    اگر (tuple != تہی)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    صحیح
rettv_tuple_سیٹ_with_اشیاء(typval_T *rettv, صحیح گنتی)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_اشیاء(گنتی);
    اگر (new_tuple == تہی)
	واپس FAIL;

    rettv_tuple_سیٹ(rettv, new_tuple);

    واپس OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    خالی
tuple_unref(tuple_T *tuple)
{
    اگر (tuple != تہی && --tuple->tv_refcount <= 0)
	tuple_free(tuple);
}

/*
 * Free a tuple, including all non-container items it points to.
 * Ignores the reference count.
 */
    ساکن خالی
tuple_free_contents(tuple_T *tuple)
{
    برائے (صحیح i = 0; i < TUPLE_LEN(tuple); i++)
	clear_tv(TUPLE_شے(tuple, i));

    ga_clear(&tuple->tv_اشیاء);
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

    برائے (tt = پہلا_tuple; tt != تہی; tt = tt->tv_مستعمل_اگلا)
	اگر ((tt->tv_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    واپس did_free;
}

    ساکن خالی
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    اگر (tuple->tv_مستعمل_پچھلا == تہی)
	پہلا_tuple = tuple->tv_مستعمل_اگلا;
    ورنہ
	tuple->tv_مستعمل_پچھلا->tv_مستعمل_اگلا = tuple->tv_مستعمل_اگلا;
    اگر (tuple->tv_مستعمل_اگلا != تہی)
	tuple->tv_مستعمل_اگلا->tv_مستعمل_پچھلا = tuple->tv_مستعمل_پچھلا;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    خالی
tuple_free_اشیاء(صحیح copyID)
{
    tuple_T	*tt, *tt_اگلا;

    برائے (tt = پہلا_tuple; tt != تہی; tt = tt_اگلا)
    {
	tt_اگلا = tt->tv_مستعمل_اگلا;
	اگر ((tt->tv_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب))
	{
	    // Free the tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_list(tt);
	}
    }
}

    خالی
tuple_free(tuple_T *tuple)
{
    اگر (اندر_free_unref_اشیاء)
	واپس;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    طویل
tuple_len(tuple_T *tuple)
{
    اگر (tuple == تہی)
	واپس 0L;
    واپس tuple->tv_اشیاء.ga_len;
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
	واپس TRUE;

    صحیح t1_len = tuple_len(t1);
    صحیح t2_len = tuple_len(t2);

    اگر (t1_len != t2_len)
	واپس FALSE;

    اگر (t1_len == 0)
	// empty and NULL tuples are considered equal
	واپس TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    برائے (صحیح i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	اگر (!tv_equal(TUPLE_شے(t1, i), TUPLE_شے(t2, j), ic))
	    واپس FALSE;

    واپس TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_ڈھونڈو(tuple_T *tuple, طویل n)
{
    اگر (tuple == تہی)
	واپس تہی;

    // Negative index is relative to the end.
    اگر (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    اگر (n < 0 || n >= TUPLE_LEN(tuple))
	واپس تہی;

    واپس TUPLE_شے(tuple, n);
}

    صحیح
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    اگر (ga_grow_id(&tuple->tv_اشیاء, 1, aid_tuple_append) == FAIL)
	واپس FAIL;

    tuple_سیٹ_شے(tuple, TUPLE_LEN(tuple), tv);

    واپس OK;
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
    اگر (t1 == تہی)
	tuple = tuple_alloc();
    ورنہ
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    اگر (tuple == تہی)
	واپس FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_تالا = 0;
    tv->vval.v_tuple = tuple;
    اگر (t1 == تہی)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    برائے (صحیح i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_شے(t2, i), &new_tv);

	اگر (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    واپس FAIL;
	}
    }

    واپس OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_قاش(tuple_T *tuple, طویل n1, طویل n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_اشیاء(n2 - n1 + 1);
    اگر (new_tuple == تہی)
	واپس تہی;

    برائے (صحیح i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_شے(tuple, i), &new_tv);

	اگر (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    واپس تہی;
	}
    }

    واپس new_tuple;
}

    صحیح
tuple_قاش_or_اشاریہ(
    tuple_T	*tuple,
    صحیح		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    صحیح		exclusive,
    typval_T	*rettv,
    صحیح		verbose)
{
    طویل	len = tuple_len(tuple);
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
		semsg(_(e_tuple_اشاریہ_برآمد_کا_range_nr), (طویل)n1_arg);
	    واپس FAIL;
	}
	اگر (اندر_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ورنہ
	    n1 = len;
    }
    اگر (range)
    {
	tuple_T	*new_tuple;

	اگر (n2 < 0)
	    n2 = len + n2;
	ورنہ اگر (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	اگر (exclusive)
	    --n2;
	اگر (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_قاش(tuple, n1, n2);
	اگر (new_tuple == تہی)
	    واپس FAIL;
	clear_tv(rettv);
	rettv_tuple_سیٹ(rettv, new_tuple);
    }
    ورنہ
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_ڈھونڈو(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    واپس OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, صحیح deep, صحیح چوٹی, صحیح copyID)
{
    tuple_T	*copy;
    صحیح		idx;

    اگر (orig == تہی)
	واپس تہی;

    copy = tuple_alloc_with_اشیاء(TUPLE_LEN(orig));
    اگر (copy == تہی)
	واپس تہی;

    اگر (orig->tv_type == تہی || چوٹی || deep)
	copy->tv_type = تہی;
    ورنہ
	copy->tv_type = alloc_type(orig->tv_type);
    اگر (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    برائے (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_اشیاء.ga_len++;
	اگر (deep)
	{
	    اگر (شے_copy(TUPLE_شے(orig, idx), TUPLE_شے(copy, idx),
						deep, FALSE, copyID) == FAIL)
		توڑو;
	}
	ورنہ
	    copy_tv(TUPLE_شے(orig, idx), TUPLE_شے(copy, idx));
    }

    ++copy->tv_refcount;
    اگر (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = تہی;
    }

    واپس copy;
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
    صحیح		evaluate = evalarg == تہی ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    tuple_T	*tuple = تہی;
    typval_T	tv;
    صحیح		vim9script = اندر_vim9script();
    صحیح		had_comma;

    اگر (check_typval_is_قدر(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	واپس FAIL;
    }

    اگر (evaluate)
    {
	tuple = tuple_alloc();
	اگر (tuple == تہی)
	    واپس FAIL;

	اگر (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    اگر (tuple_append_tv(tuple, rettv) == FAIL)
		جاؤ failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    اگر (**arg == ')')
	// empty tuple
	جاؤ مکمل;

    اگر (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	جاؤ failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    جب (**arg != ')' && **arg != NUL)
    {
	اگر (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    جاؤ failret;
	اگر (check_typval_is_قدر(&tv) == FAIL)
	{
	    اگر (evaluate)
		clear_tv(&tv);
	    جاؤ failret;
	}

	اگر (evaluate)
	{
	    اگر (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		جاؤ failret;
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
		جاؤ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	اگر (**arg == ')')
	    توڑو;

	اگر (!had_comma)
	{
	    اگر (do_error)
	    {
		اگر (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ورنہ
		    semsg(_(e_missing_comma_اندر_tuple_str), *arg);
	    }
	    جاؤ failret;
	}
    }

    اگر (**arg != ')')
    {
	اگر (do_error)
	    semsg(_(e_missing_انتہا_کا_tuple_rsp_str), *arg);
failret:
	اگر (evaluate)
	    tuple_free(tuple);
	واپس FAIL;
    }

مکمل:
    *arg += 1;
    اگر (evaluate)
	rettv_tuple_سیٹ(rettv, tuple);

    واپس OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    خالی
tuple_تالا(tuple_T *tuple, صحیح deep, صحیح تالا, صحیح check_refcount)
{
    اگر (tuple == تہی || (check_refcount && tuple->tv_refcount > 1))
	واپس;

    اگر (تالا)
	tuple->tv_تالا |= VAR_LOCKED;
    ورنہ
	tuple->tv_تالا &= ~VAR_LOCKED;

    اگر (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	برائے (صحیح i = 0; i < TUPLE_LEN(tuple); i++)
	    شے_تالا(TUPLE_شے(tuple, i), deep - 1, تالا, check_refcount);
    }
}

قسمتعریف ساخت join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ساکن صحیح
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
    صحیح		پہلا = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    برائے (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_شے(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	اگر (s.string == تہی)
	    واپس FAIL;

	s.لمبائی = STRLEN(s.string);
	sumlen += (صحیح)s.لمبائی;

	(خالی)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_ڈیٹا) + (join_gap->ga_len++);
	اگر (tofree != تہی || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.لمبائی = s.لمبائی;
	    p->tofree = tofree;
	}
	ورنہ
	{
	    p->s.string = vim_strnsave(s.string, s.لمبائی);
	    p->s.لمبائی = s.لمبائی;
	    p->tofree = p->s.string;
	}

	سطر_breakcheck();
	اگر (did_echo_string_emsg)  // recursion error, bail out
	    توڑو;
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
	واپس FAIL;

    برائے (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	اگر (پہلا)
	    پہلا = FALSE;
	ورنہ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_ڈیٹا) + i;

	اگر (p->s.string != تہی)
	    ga_concat_len(gap, p->s.string, p->s.لمبائی);
	سطر_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    اگر (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    واپس OK;
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
	واپس OK; // nothing to do
    ga_init2(&join_ga, حجم(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    اگر (join_ga.ga_ڈیٹا == تہی)
	واپس retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_ڈیٹا;
    برائے (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    واپس retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, صحیح copyID, صحیح restore_copyID)
{
    garray_T	ga;

    اگر (tv->vval.v_tuple == تہی)
	واپس تہی;
    ga_init2(&ga, حجم(حرف), 80);
    ga_append(&ga, '(');
    اگر (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_ڈیٹا);
	واپس تہی;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    واپس (char_u *)ga.ga_ڈیٹا;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    خالی
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*اظہاریہ)
{
    صحیح		len = tuple_len(tuple);
    صحیح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    سیٹ_vim_var_type(VV_کلید, VAR_نمبر);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظہاریہ_get_funccal(اظہاریہ, &newtv);

    برائے (صحیح idx = 0; idx < len; idx++)
    {
	سیٹ_vim_var_nr(VV_کلید, idx);
	اگر (filter_map_one(TUPLE_شے(tuple, idx), اظہاریہ, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    توڑو;
    }

    اگر (fc != تہی)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    طویل
tuple_گنتی(tuple_T *tuple, typval_T *needle, طویل idx, صحیح ic)
{
    طویل	n = 0;

    اگر (tuple == تہی)
	واپس 0;

    صحیح	len = TUPLE_LEN(tuple);
    اگر (len == 0)
	واپس 0;

    اگر (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_اشاریہ_برآمد_کا_range_nr), idx);
	واپس 0;
    }

    برائے (صحیح i = idx; i < len; i++)
    {
	اگر (tv_equal(TUPLE_شے(tuple, i), needle, ic))
	    ++n;
    }

    واپس n;
}

/*
 * "items(tuple)" function
 * Caller must have already checked that argvars[0] is a tuple.
 */
    خالی
tuple2items(typval_T *argvars, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    varnumber_T	idx;

    اگر (rettv_list_alloc(rettv) == FAIL)
	واپس;

    اگر (tuple == تہی)
	واپس;  // null tuple behaves like an empty list

    برائے (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	اگر (l == تہی)
	    توڑو;

	اگر (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    توڑو;
	}
	اگر (list_append_نمبر(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_شے(tuple, idx)) == FAIL)
	    توڑو;
    }
}

/*
 * Search for item "tv" in tuple "tuple" starting from index "start_idx".
 * If "ic" is set to TRUE, then case is ignored.
 *
 * Returns the index where "tv" is present or -1 if it is not found.
 */
    صحیح
اشاریہ_tuple(tuple_T *tuple, typval_T *tv, صحیح آغاز_idx, صحیح ic)
{
    اگر (آغاز_idx < 0)
    {
	آغاز_idx = TUPLE_LEN(tuple) + آغاز_idx;
	اگر (آغاز_idx < 0)
	    آغاز_idx = 0;
    }

    برائے (صحیح idx = آغاز_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	اگر (tv_equal(TUPLE_شے(tuple, idx), tv, ic))
	    واپس idx;
    }

    واپس -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    صحیح
indexof_tuple(tuple_T *tuple, طویل startidx, typval_T *اظہاریہ)
{
    طویل	idx = 0;
    صحیح		len;
    صحیح		ملا;

    اگر (tuple == تہی)
	واپس -1;

    len = TUPLE_LEN(tuple);

    اگر (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	اگر (startidx < 0)
	    startidx = 0;
    }

    سیٹ_vim_var_type(VV_کلید, VAR_نمبر);

    صحیح		called_emsg_آغاز = called_emsg;

    برائے (idx = startidx; idx < len; idx++)
    {
	سیٹ_vim_var_nr(VV_کلید, idx);
	copy_tv(TUPLE_شے(tuple, idx), get_vim_var_tv(VV_VAL));

	ملا = indexof_eval_اظہاریہ(اظہاریہ);
	clear_tv(get_vim_var_tv(VV_VAL));

	اگر (ملا)
	    واپس idx;

	اگر (called_emsg != called_emsg_آغاز)
	    واپس -1;
    }

    واپس -1;
}

/*
 * Return the max or min of the items in tuple "tuple".
 * If an error occurs NULL is returned instead.
 */
    typval_T *
tuple_max_min(tuple_T *tuple, صحیح domax)
{
    typval_T	*tv = تہی;
    صحیح		res;

    اگر (tuple == تہی || TUPLE_LEN(tuple) == 0)
	واپس تہی;

    برائے (صحیح idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	اگر (tv == تہی)
	    tv = TUPLE_شے(tuple, idx);
	ورنہ
	{
	    اگر (typval_compare2(TUPLE_شے(tuple, idx), tv,
		    domax ? اظہاریہ_GREATER : اظہاریہ_SMALLER, FALSE, &res) == FAIL)
		واپس تہی;
	    اگر (res == OK)
		tv = TUPLE_شے(tuple, idx);
	}
    }

    واپس tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    خالی
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = تہی;

    اگر (tuple == تہی || TUPLE_LEN(tuple) == 0 || n <= 0)
	واپس;

    اگر (check_repeat_گنتی(TUPLE_LEN(tuple), n) == FAIL)
	واپس;

    اگر (rettv_tuple_سیٹ_with_اشیاء(rettv, TUPLE_LEN(tuple) * (صحیح)n) == FAIL)
	واپس;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    برائے (صحیح گنتی = 0; گنتی < (صحیح)n; گنتی++)
    {
	برائے (صحیح idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_شے(tuple, idx),
		    TUPLE_شے(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_اشیاء.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    خالی
tuple_الٹ(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = تہی;

    صحیح	len = tuple_len(tuple);

    اگر (len == 0)
	واپس;

    اگر (rettv_tuple_سیٹ_with_اشیاء(rettv, len) == FAIL)
	واپس;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    برائے (صحیح i = 0; i < len; i++)
	copy_tv(TUPLE_شے(tuple, i), TUPLE_شے(new_tuple, len - i - 1));
    new_tuple->tv_اشیاء.ga_len = tuple->tv_اشیاء.ga_len;
}

/*
 * Tuple reduce() function
 */
    خالی
tuple_reduce(typval_T *argvars, typval_T *اظہاریہ, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    صحیح		called_emsg_آغاز = called_emsg;
    typval_T	initial;
    صحیح		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    صحیح		r;

    اگر (argvars[2].v_type == VAR_UNKNOWN)
    {
	اگر (tuple == تہی || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_کا_an_خالی_خانے_str_with_no_initial_قدر), "Tuple");
	    واپس;
	}
	initial = *TUPLE_شے(tuple, 0);
	idx = 1;
    }
    ورنہ
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    اگر (tuple == تہی)
	واپس;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظہاریہ_get_funccal(اظہاریہ, rettv);

    برائے ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_شے(tuple, idx);

	r = eval_اظہاریہ_typval(اظہاریہ, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	اگر (r == FAIL || called_emsg != called_emsg_آغاز)
	    توڑو;
    }

    اگر (fc != تہی)
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
	ورنہ اگر ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_دلائل[0], type2->tt_دلائل[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    اگر (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_اندر_concatenation));
	واپس FAIL;
    }

    واپس OK;
}

#اگر_ختم // defined(FEAT_EVAL)
