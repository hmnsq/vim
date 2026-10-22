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

#إذا defined(FEAT_EVAL)

// Tuple heads for garbage collection.
ساكن tuple_T		*پهريون_tuple = عدم;	// list of all tuples

    ساكن فراغ
tuple_init(tuple_T *tuple)
{
    // Prepend the tuple to the list of tuples for garbage collection.
    إذا (پهريون_tuple != عدم)
	پهريون_tuple->tv_استعمال_ٿيل_prev = tuple;
    tuple->tv_استعمال_ٿيل_prev = عدم;
    tuple->tv_استعمال_ٿيل_ايندڙ = پهريون_tuple;
    پهريون_tuple = tuple;

    ga_init2(&tuple->tv_شيون, حجم(typval_T), 20);
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
    إذا (tuple != عدم)
	tuple_init(tuple);
    أرجع tuple;
}

/*
 * Allocate space for a tuple with "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next tuple_set_item() must be called for each item.
 */
    tuple_T *
tuple_alloc_with_شيون(صحيح ڳڻپ)
{
    tuple_T	*tuple;

    tuple = tuple_alloc();
    إذا (tuple == عدم)
	أرجع عدم;

    إذا (ڳڻپ <= 0)
	أرجع tuple;

    إذا (ga_grow(&tuple->tv_شيون, ڳڻپ) == FAIL)
    {
	tuple_free(tuple);
	أرجع عدم;
    }

    أرجع tuple;
}

/*
 * Set item "idx" for a tuple previously allocated with
 * tuple_alloc_with_items().
 * The contents of "tv" is copied into the tuple item.
 * Each item must be set exactly once.
 */
    فراغ
tuple_سيٽ_شي(tuple_T *tuple, صحيح idx, typval_T *tv)
{
    *TUPLE_شي(tuple, idx) = *tv;
    tuple->tv_شيون.ga_len++;
}

/*
 * Allocate an empty tuple for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    صحيح
rettv_tuple_alloc(typval_T *rettv)
{
    tuple_T	*tuple = tuple_alloc();

    إذا (tuple == عدم)
	أرجع FAIL;

    rettv->v_تالو = 0;
    rettv_tuple_سيٽ(rettv, tuple);
    أرجع OK;
}

/*
 * Set a tuple as the return value.  Increments the reference count.
 */
    فراغ
rettv_tuple_سيٽ(typval_T *rettv, tuple_T *tuple)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = tuple;
    إذا (tuple != عدم)
	++tuple->tv_refcount;
}

/*
 * Set a new tuple with "count" items as the return value.
 * Returns OK on success and FAIL on allocation failure.
 */
    صحيح
rettv_tuple_سيٽ_with_شيون(typval_T *rettv, صحيح ڳڻپ)
{
    tuple_T *new_tuple;

    new_tuple = tuple_alloc_with_شيون(ڳڻپ);
    إذا (new_tuple == عدم)
	أرجع FAIL;

    rettv_tuple_سيٽ(rettv, new_tuple);

    أرجع OK;
}

/*
 * Unreference a tuple: decrement the reference count and free it when it
 * becomes zero.
 */
    فراغ
tuple_unref(tuple_T *tuple)
{
    إذا (tuple != عدم && --tuple->tv_refcount <= 0)
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
	clear_tv(TUPLE_شي(tuple, i));

    ga_clear(&tuple->tv_شيون);
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

    لكل (tt = پهريون_tuple; tt != عدم; tt = tt->tv_استعمال_ٿيل_ايندڙ)
	إذا ((tt->tv_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب))
	{
	    // Free the Tuple and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    tuple_free_contents(tt);
	    did_free = TRUE;
	}
    أرجع did_free;
}

    ساكن فراغ
tuple_free_list(tuple_T  *tuple)
{
    // Remove the tuple from the list of tuples for garbage collection.
    إذا (tuple->tv_استعمال_ٿيل_prev == عدم)
	پهريون_tuple = tuple->tv_استعمال_ٿيل_ايندڙ;
    وإلا
	tuple->tv_استعمال_ٿيل_prev->tv_استعمال_ٿيل_ايندڙ = tuple->tv_استعمال_ٿيل_ايندڙ;
    إذا (tuple->tv_استعمال_ٿيل_ايندڙ != عدم)
	tuple->tv_استعمال_ٿيل_ايندڙ->tv_استعمال_ٿيل_prev = tuple->tv_استعمال_ٿيل_prev;

    free_type(tuple->tv_type);
    vim_free(tuple);
}

    فراغ
tuple_free_شيون(صحيح copyID)
{
    tuple_T	*tt, *tt_ايندڙ;

    لكل (tt = پهريون_tuple; tt != عدم; tt = tt_ايندڙ)
    {
	tt_ايندڙ = tt->tv_استعمال_ٿيل_ايندڙ;
	إذا ((tt->tv_copyID & COPYID_نقاب) != (copyID & COPYID_نقاب))
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
    إذا (اندريون_free_unref_شيون)
	أرجع;

    tuple_free_contents(tuple);
    tuple_free_list(tuple);
}

/*
 * Get the number of items in a tuple.
 */
    ڊگهو
tuple_len(tuple_T *tuple)
{
    إذا (tuple == عدم)
	أرجع 0L;
    أرجع tuple->tv_شيون.ga_len;
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
    إذا (t1 == t2)
	أرجع TRUE;

    صحيح t1_len = tuple_len(t1);
    صحيح t2_len = tuple_len(t2);

    إذا (t1_len != t2_len)
	أرجع FALSE;

    إذا (t1_len == 0)
	// empty and NULL tuples are considered equal
	أرجع TRUE;

    // If the tuples "t1" or "t2" is NULL, then it is handled by the length
    // checks above.

    لكل (صحيح i = 0, j = 0; i < t1_len && j < t2_len; i++, j++)
	إذا (!tv_equal(TUPLE_شي(t1, i), TUPLE_شي(t2, j), ic))
	    أرجع FALSE;

    أرجع TRUE;
}

/*
 * Locate item with index "n" in tuple "tuple" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    typval_T *
tuple_ڳوليو(tuple_T *tuple, ڊگهو n)
{
    إذا (tuple == عدم)
	أرجع عدم;

    // Negative index is relative to the end.
    إذا (n < 0)
	n = TUPLE_LEN(tuple) + n;

    // Check for index out of range.
    إذا (n < 0 || n >= TUPLE_LEN(tuple))
	أرجع عدم;

    أرجع TUPLE_شي(tuple, n);
}

    صحيح
tuple_append_tv(tuple_T *tuple, typval_T *tv)
{
    إذا (ga_grow_id(&tuple->tv_شيون, 1, aid_tuple_append) == FAIL)
	أرجع FAIL;

    tuple_سيٽ_شي(tuple, TUPLE_LEN(tuple), tv);

    أرجع OK;
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
    إذا (t1 == عدم)
	tuple = tuple_alloc();
    وإلا
	tuple = tuple_copy(t1, FALSE, TRUE, 0);
    إذا (tuple == عدم)
	أرجع FAIL;

    tv->v_type = VAR_TUPLE;
    tv->v_تالو = 0;
    tv->vval.v_tuple = tuple;
    إذا (t1 == عدم)
	++tuple->tv_refcount;

    // append all the items from the second tuple
    لكل (صحيح i = 0; i < tuple_len(t2); i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_شي(t2, i), &new_tv);

	إذا (tuple_append_tv(tuple, &new_tv) == FAIL)
	{
	    tuple_free(tuple);
	    أرجع FAIL;
	}
    }

    أرجع OK;
}

/*
 * Return a slice of tuple starting at index n1 and ending at index n2,
 * inclusive (tuple[n1 : n2])
 */
    tuple_T *
tuple_ٽڪرو(tuple_T *tuple, ڊگهو n1, ڊگهو n2)
{
    tuple_T	*new_tuple;

    new_tuple = tuple_alloc_with_شيون(n2 - n1 + 1);
    إذا (new_tuple == عدم)
	أرجع عدم;

    لكل (صحيح i = n1; i <= n2; i++)
    {
	typval_T    new_tv;

	copy_tv(TUPLE_شي(tuple, i), &new_tv);

	إذا (tuple_append_tv(new_tuple, &new_tv) == FAIL)
	{
	    tuple_free(new_tuple);
	    أرجع عدم;
	}
    }

    أرجع new_tuple;
}

    صحيح
tuple_ٽڪرو_or_اشاريو(
    tuple_T	*tuple,
    صحيح		range,
    varnumber_T	n1_arg,
    varnumber_T	n2_arg,
    صحيح		exclusive,
    typval_T	*rettv,
    صحيح		verbose)
{
    ڊگهو	len = tuple_len(tuple);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    إذا (n1 < 0)
	n1 = len + n1;
    إذا (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty tuple, for Vim9 script start at the first item.
	// A tuple index out of range is an error.
	إذا (!range)
	{
	    إذا (verbose)
		semsg(_(e_tuple_اشاريو_ٻاهريون_جو_range_nr), (ڊگهو)n1_arg);
	    أرجع FAIL;
	}
	إذا (اندريون_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	وإلا
	    n1 = len;
    }
    إذا (range)
    {
	tuple_T	*new_tuple;

	إذا (n2 < 0)
	    n2 = len + n2;
	وإلا إذا (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	إذا (exclusive)
	    --n2;
	إذا (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	new_tuple = tuple_ٽڪرو(tuple, n1, n2);
	إذا (new_tuple == عدم)
	    أرجع FAIL;
	clear_tv(rettv);
	rettv_tuple_سيٽ(rettv, new_tuple);
    }
    وإلا
    {
	// copy the item to "var1" to avoid that freeing the tuple makes it
	// invalid.
	copy_tv(tuple_ڳوليو(tuple, n1), &var1);
	clear_tv(rettv);
	*rettv = var1;
    }
    أرجع OK;
}

/*
 * Make a copy of tuple "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new tuple is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    tuple_T *
tuple_copy(tuple_T *orig, صحيح deep, صحيح چوٽي, صحيح copyID)
{
    tuple_T	*copy;
    صحيح		idx;

    إذا (orig == عدم)
	أرجع عدم;

    copy = tuple_alloc_with_شيون(TUPLE_LEN(orig));
    إذا (copy == عدم)
	أرجع عدم;

    إذا (orig->tv_type == عدم || چوٽي || deep)
	copy->tv_type = عدم;
    وإلا
	copy->tv_type = alloc_type(orig->tv_type);
    إذا (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this tuple.
	orig->tv_copyID = copyID;
	orig->tv_copytuple = copy;
    }

    لكل (idx = 0; idx < TUPLE_LEN(orig) && !got_int; idx++)
    {
	copy->tv_شيون.ga_len++;
	إذا (deep)
	{
	    إذا (شي_copy(TUPLE_شي(orig, idx), TUPLE_شي(copy, idx),
						deep, FALSE, copyID) == FAIL)
		اكسر;
	}
	وإلا
	    copy_tv(TUPLE_شي(orig, idx), TUPLE_شي(copy, idx));
    }

    ++copy->tv_refcount;
    إذا (idx != TUPLE_LEN(orig))
    {
	tuple_unref(copy);
	copy = عدم;
    }

    أرجع copy;
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
    صحيح		vim9script = اندريون_vim9script();
    صحيح		had_comma;

    إذا (check_typval_is_قدر(rettv) == FAIL)
    {
	// the first item is not a valid value type
	clear_tv(rettv);
	أرجع FAIL;
    }

    إذا (evaluate)
    {
	tuple = tuple_alloc();
	إذا (tuple == عدم)
	    أرجع FAIL;

	إذا (rettv->v_type != VAR_UNKNOWN)
	{
	    // Add the first item to the tuple from "rettv"
	    إذا (tuple_append_tv(tuple, rettv) == FAIL)
		اذهب failret;
	    // The first item in "rettv" is added to the tuple.  Set the rettv
	    // type to unknown, so that the caller doesn't free it.
	    rettv->v_type = VAR_UNKNOWN;
	}
    }

    إذا (**arg == ')')
	// empty tuple
	اذهب پورو;

    إذا (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
    {
	semsg(_(e_white_space_required_after_str_str), ",", *arg);
	اذهب failret;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    طالما (**arg != ')' && **arg != NUL)
    {
	إذا (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    اذهب failret;
	إذا (check_typval_is_قدر(&tv) == FAIL)
	{
	    إذا (evaluate)
		clear_tv(&tv);
	    اذهب failret;
	}

	إذا (evaluate)
	{
	    إذا (tuple_append_tv(tuple, &tv) == FAIL)
	    {
		clear_tv(&tv);
		اذهب failret;
	    }
	}

	إذا (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	إذا (had_comma)
	{
	    إذا (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ')')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		اذهب failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The ")" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	إذا (**arg == ')')
	    اكسر;

	إذا (!had_comma)
	{
	    إذا (do_error)
	    {
		إذا (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		وإلا
		    semsg(_(e_missing_comma_اندريون_tuple_str), *arg);
	    }
	    اذهب failret;
	}
    }

    إذا (**arg != ')')
    {
	إذا (do_error)
	    semsg(_(e_missing_پڇاڙي_جو_tuple_rsp_str), *arg);
failret:
	إذا (evaluate)
	    tuple_free(tuple);
	أرجع FAIL;
    }

پورو:
    *arg += 1;
    إذا (evaluate)
	rettv_tuple_سيٽ(rettv, tuple);

    أرجع OK;
}

/*
 * Lock or unlock a tuple.  "deep" is number of levels to go.
 * When "check_refcount" is TRUE do not lock a tuple with a reference
 * count larger than 1.
 */
    فراغ
tuple_تالو(tuple_T *tuple, صحيح deep, صحيح تالو, صحيح check_refcount)
{
    إذا (tuple == عدم || (check_refcount && tuple->tv_refcount > 1))
	أرجع;

    إذا (تالو)
	tuple->tv_تالو |= VAR_LOCKED;
    وإلا
	tuple->tv_تالو &= ~VAR_LOCKED;

    إذا (deep < 0 || deep > 1)
    {
	// recursive: lock/unlock the items the Tuple contains
	لكل (صحيح i = 0; i < TUPLE_LEN(tuple); i++)
	    شي_تالو(TUPLE_شي(tuple, i), deep - 1, تالو, check_refcount);
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
    صحيح		پهريون = TRUE;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    string_T	s;
    typval_T	*tv;
    size_t	seplen = 0;

    // Stringify each item in the tuple.
    لكل (i = 0; i < TUPLE_LEN(tuple) && !got_int; i++)
    {
	tv = TUPLE_شي(tuple, i);
	s.string = echo_string_core(tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	إذا (s.string == عدم)
	    أرجع FAIL;

	s.ڊيگهائي = STRLEN(s.string);
	sumlen += (صحيح)s.ڊيگهائي;

	(فراغ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_معلومات) + (join_gap->ga_len++);
	إذا (tofree != عدم || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.ڊيگهائي = s.ڊيگهائي;
	    p->tofree = tofree;
	}
	وإلا
	{
	    p->s.string = vim_strnsave(s.string, s.ڊيگهائي);
	    p->s.ڊيگهائي = s.ڊيگهائي;
	    p->tofree = p->s.string;
	}

	سٽ_breakcheck();
	إذا (did_echo_string_emsg)  // recursion error, bail out
	    اكسر;
    }

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ')' and NUL.
    إذا (join_gap->ga_len > 0)
    {
	seplen = STRLEN(sep);
	إذا (join_gap->ga_len >= 2)
	    sumlen += (صحيح)seplen * (join_gap->ga_len - 1);
    }
    إذا (ga_grow(gap, sumlen + 2) == FAIL)
	أرجع FAIL;

    لكل (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	إذا (پهريون)
	    پهريون = FALSE;
	وإلا
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_معلومات) + i;

	إذا (p->s.string != عدم)
	    ga_concat_len(gap, p->s.string, p->s.ڊيگهائي);
	سٽ_breakcheck();
    }

    // If there is only one item in the tuple, then add the separator after
    // that.
    إذا (join_gap->ga_len == 1)
	ga_concat_len(gap, sep, seplen);

    أرجع OK;
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

    إذا (TUPLE_LEN(tuple) < 1)
	أرجع OK; // nothing to do
    ga_init2(&join_ga, حجم(join_T), TUPLE_LEN(tuple));
    retval = tuple_join_inner(gap, tuple, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    إذا (join_ga.ga_معلومات == عدم)
	أرجع retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_معلومات;
    لكل (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    أرجع retval;
}

/*
 * Return an allocated string with the string representation of a tuple.
 * May return NULL.
 */
    char_u *
tuple2string(typval_T *tv, صحيح copyID, صحيح restore_copyID)
{
    garray_T	ga;

    إذا (tv->vval.v_tuple == عدم)
	أرجع عدم;
    ga_init2(&ga, حجم(حرف), 80);
    ga_append(&ga, '(');
    إذا (tuple_join(&ga, tv->vval.v_tuple, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_معلومات);
	أرجع عدم;
    }
    ga_append(&ga, ')');
    ga_append(&ga, NUL);
    أرجع (char_u *)ga.ga_معلومات;
}

/*
 * Implementation of foreach() for a Tuple.  Apply "expr" to
 * every item in Tuple "tuple" and return the result in "rettv".
 */
    فراغ
tuple_foreach(
    tuple_T	*tuple,
    filtermap_T	filtermap,
    typval_T	*اظهار)
{
    صحيح		len = tuple_len(tuple);
    صحيح		rem;
    typval_T	newtv;
    funccall_T	*fc;

    // set_vim_var_nr() doesn't set the type
    سيٽ_vim_var_type(VV_ڪنجي, VAR_عدد);

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظهار_get_funccal(اظهار, &newtv);

    لكل (صحيح idx = 0; idx < len; idx++)
    {
	سيٽ_vim_var_nr(VV_ڪنجي, idx);
	إذا (filter_map_one(TUPLE_شي(tuple, idx), اظهار, filtermap, fc,
						     &newtv, &rem) == FAIL)
	    اكسر;
    }

    إذا (fc != عدم)
	remove_funccal();
}

/*
 * Count the number of times item "needle" occurs in Tuple "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ڊگهو
tuple_ڳڻپ(tuple_T *tuple, typval_T *needle, ڊگهو idx, صحيح ic)
{
    ڊگهو	n = 0;

    إذا (tuple == عدم)
	أرجع 0;

    صحيح	len = TUPLE_LEN(tuple);
    إذا (len == 0)
	أرجع 0;

    إذا (idx < 0 || idx >= len)
    {
	semsg(_(e_tuple_اشاريو_ٻاهريون_جو_range_nr), idx);
	أرجع 0;
    }

    لكل (صحيح i = idx; i < len; i++)
    {
	إذا (tv_equal(TUPLE_شي(tuple, i), needle, ic))
	    ++n;
    }

    أرجع n;
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

    إذا (rettv_list_alloc(rettv) == FAIL)
	أرجع;

    إذا (tuple == عدم)
	أرجع;  // null tuple behaves like an empty list

    لكل (idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	list_T	*l = list_alloc();

	إذا (l == عدم)
	    اكسر;

	إذا (list_append_list(rettv->vval.v_list, l) == FAIL)
	{
	    list_free(l);
	    اكسر;
	}
	إذا (list_append_عدد(l, idx) == FAIL
		|| list_append_tv(l, TUPLE_شي(tuple, idx)) == FAIL)
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
اشاريو_tuple(tuple_T *tuple, typval_T *tv, صحيح شروعات_idx, صحيح ic)
{
    إذا (شروعات_idx < 0)
    {
	شروعات_idx = TUPLE_LEN(tuple) + شروعات_idx;
	إذا (شروعات_idx < 0)
	    شروعات_idx = 0;
    }

    لكل (صحيح idx = شروعات_idx; idx < TUPLE_LEN(tuple); idx++)
    {
	إذا (tv_equal(TUPLE_شي(tuple, idx), tv, ic))
	    أرجع idx;
    }

    أرجع -1;		// "tv" not found
}

/*
 * Evaluate 'expr' for each item in the Tuple 'tuple' starting with the item at
 * 'startidx' and return the index of the item where 'expr' is TRUE.  Returns
 * -1 if 'expr' doesn't evaluate to TRUE for any of the items.
 */
    صحيح
indexof_tuple(tuple_T *tuple, ڊگهو startidx, typval_T *اظهار)
{
    ڊگهو	idx = 0;
    صحيح		len;
    صحيح		مليو;

    إذا (tuple == عدم)
	أرجع -1;

    len = TUPLE_LEN(tuple);

    إذا (startidx < 0)
    {
	// negative index: index from the end
	startidx = len + startidx;
	إذا (startidx < 0)
	    startidx = 0;
    }

    سيٽ_vim_var_type(VV_ڪنجي, VAR_عدد);

    صحيح		called_emsg_شروعات = called_emsg;

    لكل (idx = startidx; idx < len; idx++)
    {
	سيٽ_vim_var_nr(VV_ڪنجي, idx);
	copy_tv(TUPLE_شي(tuple, idx), get_vim_var_tv(VV_VAL));

	مليو = indexof_eval_اظهار(اظهار);
	clear_tv(get_vim_var_tv(VV_VAL));

	إذا (مليو)
	    أرجع idx;

	إذا (called_emsg != called_emsg_شروعات)
	    أرجع -1;
    }

    أرجع -1;
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

    إذا (tuple == عدم || TUPLE_LEN(tuple) == 0)
	أرجع عدم;

    لكل (صحيح idx = 0; idx < TUPLE_LEN(tuple); idx++)
    {
	إذا (tv == عدم)
	    tv = TUPLE_شي(tuple, idx);
	وإلا
	{
	    إذا (typval_compare2(TUPLE_شي(tuple, idx), tv,
		    domax ? اظهار_GREATER : اظهار_SMALLER, FALSE, &res) == FAIL)
		أرجع عدم;
	    إذا (res == OK)
		tv = TUPLE_شي(tuple, idx);
	}
    }

    أرجع tv;
}

/*
 * Repeat the tuple "tuple" "n" times and set "rettv" to the new tuple.
 */
    فراغ
tuple_repeat(tuple_T *tuple, varnumber_T n, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = عدم;

    إذا (tuple == عدم || TUPLE_LEN(tuple) == 0 || n <= 0)
	أرجع;

    إذا (check_repeat_ڳڻپ(TUPLE_LEN(tuple), n) == FAIL)
	أرجع;

    إذا (rettv_tuple_سيٽ_with_شيون(rettv, TUPLE_LEN(tuple) * (صحيح)n) == FAIL)
	أرجع;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    لكل (صحيح ڳڻپ = 0; ڳڻپ < (صحيح)n; ڳڻپ++)
    {
	لكل (صحيح idx = 0; idx < TUPLE_LEN(tuple); idx++)
	{
	    copy_tv(TUPLE_شي(tuple, idx),
		    TUPLE_شي(new_tuple, TUPLE_LEN(new_tuple)));
	    new_tuple->tv_شيون.ga_len++;
	}
    }
}

/*
 * Reverse "tuple" and return the new tuple in "rettv"
 */
    فراغ
tuple_ابتو(tuple_T *tuple, typval_T *rettv)
{
    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = عدم;

    صحيح	len = tuple_len(tuple);

    إذا (len == 0)
	أرجع;

    إذا (rettv_tuple_سيٽ_with_شيون(rettv, len) == FAIL)
	أرجع;

    tuple_T	*new_tuple = rettv->vval.v_tuple;
    لكل (صحيح i = 0; i < len; i++)
	copy_tv(TUPLE_شي(tuple, i), TUPLE_شي(new_tuple, len - i - 1));
    new_tuple->tv_شيون.ga_len = tuple->tv_شيون.ga_len;
}

/*
 * Tuple reduce() function
 */
    فراغ
tuple_reduce(typval_T *argvars, typval_T *اظهار, typval_T *rettv)
{
    tuple_T	*tuple = argvars[0].vval.v_tuple;
    صحيح		called_emsg_شروعات = called_emsg;
    typval_T	initial;
    صحيح		idx = 0;
    funccall_T	*fc;
    typval_T	argv[3];
    صحيح		r;

    إذا (argvars[2].v_type == VAR_UNKNOWN)
    {
	إذا (tuple == عدم || TUPLE_LEN(tuple) == 0)
	{
	    semsg(_(e_reduce_جو_an_خالي_جايون_str_with_no_initial_قدر), "Tuple");
	    أرجع;
	}
	initial = *TUPLE_شي(tuple, 0);
	idx = 1;
    }
    وإلا
    {
	initial = argvars[2];
	idx = 0;
    }

    copy_tv(&initial, rettv);

    إذا (tuple == عدم)
	أرجع;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_اظهار_get_funccal(اظهار, rettv);

    لكل ( ; idx < TUPLE_LEN(tuple); idx++)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;
	argv[1] = *TUPLE_شي(tuple, idx);

	r = eval_اظهار_typval(اظهار, TRUE, argv, 2, fc, rettv);

	clear_tv(&argv[0]);

	إذا (r == FAIL || called_emsg != called_emsg_شروعات)
	    اكسر;
    }

    إذا (fc != عدم)
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
    إذا ((type1->tt_flags & TTFLAG_VARARGS)
	    && !(type2->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_argcount > 0))
	addable = FALSE;

    إذا ((type1->tt_flags & TTFLAG_VARARGS)
	    && (type2->tt_flags & TTFLAG_VARARGS))
    {
	// two variadic tuples
	إذا (type1->tt_argcount > 1 || type2->tt_argcount > 1)
	    // one of the variadic tuple has fixed number of items
	    addable = FALSE;
	وإلا إذا ((type1->tt_argcount == 1 && type2->tt_argcount == 1)
		&& !equal_type(type1->tt_دليلون[0], type2->tt_دليلون[0], 0))
	    // the tuples have different item types
	    addable = FALSE;
    }

    إذا (!addable)
    {
	emsg(_(e_cannot_use_variadic_tuple_اندريون_concatenation));
	أرجع FAIL;
    }

    أرجع OK;
}

#جيڪڏهن_پورو // defined(FEAT_EVAL)
