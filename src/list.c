/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * list.c: List support and container (List, Dict, Blob) functions.
 */

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_EVAL)

// List heads for garbage collection.
స్థిర list_T		*మొదటి_list = ఖాళీ;	// list of all lists

#నిర్వచించు FOR_ALL_WATCHERS(l, lw) \
    కొరకు ((lw) = (l)->lv_watch; (lw) != ఖాళీ; (lw) = (lw)->lw_తదుపరి)

స్థిర శూన్య list_free_అంశం(list_T *l, listitem_T *అంశం);

/*
 * Add a watcher to a list.
 */
    శూన్య
list_కూడు_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_తదుపరి = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    శూన్య
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	ఒకవేళ (lw == lwrem)
	{
	    *lwp = lw->lw_తదుపరి;
	    ఆపు;
	}
	lwp = &lw->lw_తదుపరి;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    స్థిర శూన్య
list_fix_watch(list_T *l, listitem_T *అంశం)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	ఒకవేళ (lw->lw_అంశం == అంశం)
	    lw->lw_అంశం = అంశం->li_తదుపరి;
}

    స్థిర శూన్య
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    ఒకవేళ (మొదటి_list != ఖాళీ)
	మొదటి_list->lv_వాడిన_prev = l;
    l->lv_వాడిన_prev = ఖాళీ;
    l->lv_వాడిన_తదుపరి = మొదటి_list;
    మొదటి_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(శూన్య)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    ఒకవేళ (l != ఖాళీ)
	list_init(l);
    తిరిగి l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (alloc_fail_id == id && alloc_does_fail(పరిమాణం(list_T)))
	తిరిగి ఖాళీ;
#అయితే_ముగింపు
    తిరిగి (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_వస్తువులు(పూర్ణాంకం లెక్క)
{
    list_T	*l;

    ఒకవేళ (లెక్క > 0
	    && (size_t)లెక్క > (కొలత_MAX - పరిమాణం(list_T)) / పరిమాణం(listitem_T))
	తిరిగి ఖాళీ;
    l = (list_T *)alloc_clear(పరిమాణం(list_T) + లెక్క * పరిమాణం(listitem_T));
    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;

    list_init(l);

    ఒకవేళ (లెక్క <= 0)
	తిరిగి l;

    listitem_T	*li = (listitem_T *)(l + 1);
    పూర్ణాంకం		i;

    l->lv_len = లెక్క;
    l->lv_with_వస్తువులు = లెక్క;
    l->lv_మొదటి = li;
    l->lv_u.mat.lv_చివరి = li + లెక్క - 1;
    కొరకు (i = 0; i < లెక్క; ++i)
    {
	ఒకవేళ (i == 0)
	    li->li_prev = ఖాళీ;
	లేకపోతే
	    li->li_prev = li - 1;
	ఒకవేళ (i == లెక్క - 1)
	    li->li_తదుపరి = ఖాళీ;
	లేకపోతే
	    li->li_తదుపరి = li + 1;
	++li;
    }

    తిరిగి l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    శూన్య
list_సమితి_అంశం(list_T *l, పూర్ణాంకం idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    ఒకవేళ (l == ఖాళీ)
	తిరిగి FAIL;

    rettv->v_తాళం = 0;
    rettv_list_సమితి(rettv, l);
    తిరిగి OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    పూర్ణాంకం
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id వాడని)
{
#నిర్వచితమైతే FEAT_EVAL
    ఒకవేళ (alloc_fail_id == id && alloc_does_fail(పరిమాణం(list_T)))
	తిరిగి FAIL;
#అయితే_ముగింపు
    తిరిగి rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    శూన్య
rettv_list_సమితి(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ఒకవేళ (l != ఖాళీ)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    శూన్య
list_unref(list_T *l)
{
    ఒకవేళ (l != ఖాళీ && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    స్థిర శూన్య
list_free_contents(list_T *l)
{
    listitem_T *అంశం;

    ఒకవేళ (l->lv_మొదటి != &range_list_అంశం)
	కొరకు (అంశం = l->lv_మొదటి; అంశం != ఖాళీ; అంశం = l->lv_మొదటి)
	{
	    // Remove the item before deleting it.
	    l->lv_మొదటి = అంశం->li_తదుపరి;
	    clear_tv(&అంశం->li_tv);
	    list_free_అంశం(l, అంశం);
	}
}

/*
 * Go through the list of lists and free items without the copyID.
 * But don't free a list that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    పూర్ణాంకం
list_free_nonref(పూర్ణాంకం copyID)
{
    list_T	*ll;
    పూర్ణాంకం		did_free = FALSE;

    కొరకు (ll = మొదటి_list; ll != ఖాళీ; ll = ll->lv_వాడిన_తదుపరి)
	ఒకవేళ ((ll->lv_copyID & COPYID_ముసుగు) != (copyID & COPYID_ముసుగు)
						      && ll->lv_watch == ఖాళీ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    తిరిగి did_free;
}

    స్థిర శూన్య
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    ఒకవేళ (l->lv_వాడిన_prev == ఖాళీ)
	మొదటి_list = l->lv_వాడిన_తదుపరి;
    లేకపోతే
	l->lv_వాడిన_prev->lv_వాడిన_తదుపరి = l->lv_వాడిన_తదుపరి;
    ఒకవేళ (l->lv_వాడిన_తదుపరి != ఖాళీ)
	l->lv_వాడిన_తదుపరి->lv_వాడిన_prev = l->lv_వాడిన_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    శూన్య
list_free_వస్తువులు(పూర్ణాంకం copyID)
{
    list_T	*ll, *ll_తదుపరి;

    కొరకు (ll = మొదటి_list; ll != ఖాళీ; ll = ll_తదుపరి)
    {
	ll_తదుపరి = ll->lv_వాడిన_తదుపరి;
	ఒకవేళ ((ll->lv_copyID & COPYID_ముసుగు) != (copyID & COPYID_ముసుగు)
						      && ll->lv_watch == ఖాళీ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    శూన్య
list_free(list_T *l)
{
    ఒకవేళ (లోపల_free_unref_వస్తువులు)
	తిరిగి;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(శూన్య)
{
    తిరిగి ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    స్థిర శూన్య
list_free_అంశం(list_T *l, listitem_T *అంశం)
{
    ఒకవేళ (l->lv_with_వస్తువులు == 0 || అంశం < (listitem_T *)l
			   || అంశం >= (listitem_T *)(l + 1) + l->lv_with_వస్తువులు)
	vim_free(అంశం);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    శూన్య
listitem_free(list_T *l, listitem_T *అంశం)
{
    clear_tv(&అంశం->li_tv);
    list_free_అంశం(l, అంశం);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    శూన్య
listitem_remove(list_T *l, listitem_T *అంశం)
{
    vimlist_remove(l, అంశం, అంశం);
    listitem_free(l, అంశం);
}

/*
 * Get the number of items in a list.
 */
    దీర్ఘ
list_len(list_T *l)
{
    ఒకవేళ (l == ఖాళీ)
	తిరిగి 0L;
    తిరిగి l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    పూర్ణాంకం
list_equal(
    list_T	*l1,
    list_T	*l2,
    పూర్ణాంకం		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    ఒకవేళ (l1 == l2)
	తిరిగి TRUE;
    ఒకవేళ (list_len(l1) != list_len(l2))
	తిరిగి FALSE;
    ఒకవేళ (list_len(l1) == 0)
	// empty and NULL list are considered equal
	తిరిగి TRUE;
    ఒకవేళ (l1 == ఖాళీ || l2 == ఖాళీ)
	తిరిగి FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    కొరకు (item1 = l1->lv_మొదటి, item2 = l2->lv_మొదటి;
	    item1 != ఖాళీ && item2 != ఖాళీ;
			       item1 = item1->li_తదుపరి, item2 = item2->li_తదుపరి)
	ఒకవేళ (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    తిరిగి FALSE;
    తిరిగి item1 == ఖాళీ && item2 == ఖాళీ;
}

/*
 * Locate item with index "n" in list "l" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    listitem_T *
list_కనుగొను(list_T *l, దీర్ఘ n)
{
    listitem_T	*అంశం;
    దీర్ఘ	idx;

    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;

    // Negative index is relative to the end.
    ఒకవేళ (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    ఒకవేళ (n < 0 || n >= l->lv_len)
	తిరిగి ఖాళీ;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    ఒకవేళ (n >= l->lv_len)
	తిరిగి ఖాళీ;

    // When there is a cached index may start search from there.
    ఒకవేళ (l->lv_u.mat.lv_idx_అంశం != ఖాళీ)
    {
	ఒకవేళ (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    అంశం = l->lv_మొదటి;
	    idx = 0;
	}
	లేకపోతే ఒకవేళ (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    అంశం = l->lv_u.mat.lv_చివరి;
	    idx = l->lv_len - 1;
	}
	లేకపోతే
	{
	    // closest to the cached index
	    అంశం = l->lv_u.mat.lv_idx_అంశం;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    లేకపోతే
    {
	ఒకవేళ (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    అంశం = l->lv_మొదటి;
	    idx = 0;
	}
	లేకపోతే
	{
	    // closest to the end of the list
	    అంశం = l->lv_u.mat.lv_చివరి;
	    idx = l->lv_len - 1;
	}
    }

    అయితే (n > idx)
    {
	// search forward
	అంశం = అంశం->li_తదుపరి;
	++idx;
    }
    అయితే (n < idx)
    {
	// search backward
	అంశం = అంశం->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_అంశం = అంశం;

    తిరిగి అంశం;
}

/*
 * Get list item "l[idx]" as a number.
 */
    దీర్ఘ
list_కనుగొను_nr(
    list_T	*l,
    దీర్ఘ	idx,
    పూర్ణాంకం		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    ఒకవేళ (l != ఖాళీ && l->lv_మొదటి == &range_list_అంశం)
    {
	దీర్ఘ	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	ఒకవేళ (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	ఒకవేళ (n < 0 || n >= l->lv_len)
	{
	    ఒకవేళ (errorp != ఖాళీ)
		*errorp = TRUE;
	    తిరిగి -1L;
	}

	తిరిగి l->lv_u.nonmat.lv_ప్రారంభం + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_కనుగొను(l, idx);
    ఒకవేళ (li == ఖాళీ)
    {
	ఒకవేళ (errorp != ఖాళీ)
	    *errorp = TRUE;
	తిరిగి -1L;
    }
    తిరిగి (దీర్ఘ)tv_get_సంఖ్య_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_కనుగొను_str(list_T *l, దీర్ఘ idx)
{
    listitem_T	*li;

    li = list_కనుగొను(l, idx - 1);
    ఒకవేళ (li == ఖాళీ)
    {
	semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), idx);
	తిరిగి ఖాళీ;
    }
    తిరిగి tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_కనుగొను_సూచీ(list_T *l, దీర్ఘ *idx)
{
    listitem_T *li = list_కనుగొను(l, *idx);

    ఒకవేళ (li != ఖాళీ)
	తిరిగి li;

    ఒకవేళ (*idx < 0)
    {
	*idx = 0;
	li = list_కనుగొను(l, *idx);
    }
    తిరిగి li;
}

/*
 * Locate "item" list "l" and return its index.
 * Returns -1 when "item" is not in the list.
 */
    దీర్ఘ
list_idx_యొక్క_అంశం(list_T *l, listitem_T *అంశం)
{
    దీర్ఘ	idx = 0;
    listitem_T	*li;

    ఒకవేళ (l == ఖాళీ)
	తిరిగి -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    కొరకు (li = l->lv_మొదటి; li != ఖాళీ && li != అంశం; li = li->li_తదుపరి)
	++idx;
    ఒకవేళ (li == ఖాళీ)
	తిరిగి -1;
    తిరిగి idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    శూన్య
list_append(list_T *l, listitem_T *అంశం)
{
    CHECK_LIST_MATERIALIZE(l);
    ఒకవేళ (l->lv_u.mat.lv_చివరి == ఖాళీ)
    {
	// empty list
	l->lv_మొదటి = అంశం;
	అంశం->li_prev = ఖాళీ;
    }
    లేకపోతే
    {
	l->lv_u.mat.lv_చివరి->li_తదుపరి = అంశం;
	అంశం->li_prev = l->lv_u.mat.lv_చివరి;
    }
    l->lv_u.mat.lv_చివరి = అంశం;
    ++l->lv_len;
    అంశం->li_తదుపరి = ఖాళీ;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    పూర్ణాంకం
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    ఒకవేళ (l->lv_type != ఖాళీ && l->lv_type->tt_member != ఖాళీ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ఖాళీ, 0) == FAIL)
	తిరిగి FAIL;
    li = listitem_alloc();
    ఒకవేళ (li == ఖాళీ)
	తిరిగి FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    తిరిగి OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    స్థిర పూర్ణాంకం
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    ఒకవేళ (li == ఖాళీ)
	తిరిగి FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    తిరిగి OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    పూర్ణాంకం
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    ఒకవేళ (li == ఖాళీ)
	తిరిగి FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_తాళం = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    తిరిగి OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    పూర్ణాంకం
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    ఒకవేళ (li == ఖాళీ)
	తిరిగి FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_తాళం = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    తిరిగి OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    పూర్ణాంకం
list_append_string(list_T *l, char_u *str, పూర్ణాంకం len)
{
    listitem_T *li = listitem_alloc();

    ఒకవేళ (li == ఖాళీ)
	తిరిగి FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_తాళం = 0;
    ఒకవేళ (str == ఖాళీ)
	li->li_tv.vval.v_string = ఖాళీ;
    లేకపోతే ఒకవేళ ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == ఖాళీ)
	తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    పూర్ణాంకం
list_append_సంఖ్య(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    ఒకవేళ (li == ఖాళీ)
	తిరిగి FAIL;
    li->li_tv.v_type = VAR_సంఖ్య;
    li->li_tv.v_తాళం = 0;
    li->li_tv.vval.v_సంఖ్య = n;
    list_append(l, li);
    తిరిగి OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    పూర్ణాంకం
list_చొప్పించు_tv(list_T *l, typval_T *tv, listitem_T *అంశం)
{
    listitem_T	*ni;

    ఒకవేళ (l->lv_type != ఖాళీ && l->lv_type->tt_member != ఖాళీ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ఖాళీ, 0) == FAIL)
	తిరిగి FAIL;
    ni = listitem_alloc();
    ఒకవేళ (ni == ఖాళీ)
	తిరిగి FAIL;
    copy_tv(tv, &ni->li_tv);
    list_చొప్పించు(l, ni, అంశం);
    తిరిగి OK;
}

    శూన్య
list_చొప్పించు(list_T *l, listitem_T *ni, listitem_T *అంశం)
{
    CHECK_LIST_MATERIALIZE(l);
    ఒకవేళ (అంశం == ఖాళీ)
	// Append new item at end of list.
	list_append(l, ni);
    లేకపోతే
    {
	// Insert new item before existing item.
	ni->li_prev = అంశం->li_prev;
	ni->li_తదుపరి = అంశం;
	ఒకవేళ (అంశం->li_prev == ఖాళీ)
	{
	    l->lv_మొదటి = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	లేకపోతే
	{
	    అంశం->li_prev->li_తదుపరి = ni;
	    l->lv_u.mat.lv_idx_అంశం = ఖాళీ;
	}
	అంశం->li_prev = ni;
	++l->lv_len;
    }
}

/*
 * Append a new empty item into "l" based on the list item type.  Used when
 * adding a new element to a List in Vim9script by using the current list
 * length as the index.
 */
    స్థిర పూర్ణాంకం
list_append_new_అంశం(list_T *l)
{
    typval_T	tv;

    ఒకవేళ (l->lv_type != ఖాళీ && l->lv_type->tt_member != ఖాళీ)
	tv.v_type = l->lv_type->tt_member->tt_type;
    లేకపోతే
	tv.v_type = VAR_సంఖ్య;
    tv.v_తాళం = 0;

    ఎంపిక (tv.v_type)
    {
	స్థితి VAR_BOOL: tv.vval.v_సంఖ్య = VVAL_FALSE; ఆపు;
	స్థితి VAR_SPECIAL: tv.vval.v_సంఖ్య = 0; ఆపు;
	స్థితి VAR_సంఖ్య: tv.vval.v_సంఖ్య = 0; ఆపు;
	స్థితి VAR_FLOAT: tv.vval.v_float = 0; ఆపు;
	స్థితి VAR_STRING: tv.vval.v_string = ఖాళీ; ఆపు;
	స్థితి VAR_BLOB: tv.vval.v_blob = blob_alloc(); ఆపు;
	స్థితి VAR_FUNC: tv.vval.v_string = ఖాళీ; ఆపు;
	స్థితి VAR_PARTIAL: tv.vval.v_partial = ఖాళీ; ఆపు;
	స్థితి VAR_LIST: tv.vval.v_list = list_alloc(); ఆపు;
	స్థితి VAR_DICT: tv.vval.v_dict = dict_alloc(); ఆపు;
#నిర్వచితమైతే FEAT_పని_CHANNEL
	స్థితి VAR_పని: tv.vval.v_పని = ఖాళీ; ఆపు;
	స్థితి VAR_CHANNEL: tv.vval.v_channel = ఖాళీ; ఆపు;
#అయితే_ముగింపు
	స్థితి VAR_CLASS:
	    ఒకవేళ (l->lv_type != ఖాళీ && l->lv_type->tt_member != ఖాళీ)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    లేకపోతే
		tv.vval.v_class = ఖాళీ;
	    ఆపు;
	స్థితి VAR_OBJECT:
	    ఒకవేళ (l->lv_type != ఖాళీ && l->lv_type->tt_member != ఖాళీ)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    లేకపోతే
		tv.vval.v_object = ఖాళీ;
	    ఆపు;
	స్థితి VAR_TYPEALIAS: tv.vval.v_typealias = ఖాళీ; ఆపు;
	స్థితి VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); ఆపు;
	అప్రమేయ:
	    tv.v_type = VAR_సంఖ్య;
	    tv.vval.v_సంఖ్య = 0;
	    ఆపు;
    }

    ఒకవేళ (l->lv_type != ఖాళీ && l->lv_type->tt_member != ఖాళీ)
	సమితి_tv_type(&tv, l->lv_type->tt_member);

    తిరిగి list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_సూచీ_one(list_T *l, దీర్ఘ *n1, పూర్ణాంకం can_append, పూర్ణాంకం quiet)
{
    దీర్ఘ	orig_n1 = *n1;
    listitem_T	*li = list_కనుగొను_సూచీ(l, n1);

    ఒకవేళ (li != ఖాళీ)
	తిరిగి li;

    // Vim9: Allow for adding an item at the end.
    ఒకవేళ (can_append && లోపల_vim9script()
	    && *n1 == l->lv_len && l->lv_తాళం == 0)
    {
	ఒకవేళ (list_append_new_అంశం(l) == FAIL)
	    తిరిగి ఖాళీ;
	li = list_కనుగొను_సూచీ(l, n1);
    }
    ఒకవేళ (li == ఖాళీ)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), orig_n1);
	తిరిగి ఖాళీ;
    }
    తిరిగి li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    పూర్ణాంకం
check_range_సూచీ_two(
	list_T	    *l,
	దీర్ఘ	    *n1,
	listitem_T  *li1,
	దీర్ఘ	    *n2,
	పూర్ణాంకం	    quiet)
{
    ఒకవేళ (*n2 < 0)
    {
	listitem_T	*ni = list_కనుగొను(l, *n2);

	ఒకవేళ (ni == ఖాళీ)
	{
	    ఒకవేళ (!quiet)
		semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), *n2);
	    తిరిగి FAIL;
	}
	*n2 = list_idx_యొక్క_అంశం(l, ni);
    }

    // Check that n2 isn't before n1.
    ఒకవేళ (*n1 < 0)
	*n1 = list_idx_యొక్క_అంశం(l, li1);
    ఒకవేళ (*n2 < *n1)
    {
	ఒకవేళ (!quiet)
	    semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), *n2);
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Assign values from list "src" into a range of "dest".
 * "idx1_arg" is the index of the first item in "dest" to be replaced.
 * "idx2" is the index of last item to be replaced, but when "empty_idx2" is
 * TRUE then replace all items after "idx1".
 * "op" is the operator, normally "=" but can be "+=" and the like.
 * "varname" is used for error messages.
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
list_కేటాయించు_range(
	list_T	    *dest,
	list_T	    *src,
	దీర్ఘ	    idx1_arg,
	దీర్ఘ	    idx2,
	పూర్ణాంకం	    ఖాళీలు_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    దీర్ఘ	idx1 = idx1_arg;
    listitem_T	*మొదటి_li = list_కనుగొను_సూచీ(dest, &idx1);
    దీర్ఘ	idx;
    type_T	*member_type = ఖాళీ;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = మొదటి_li;
    కొరకు (src_li = src->lv_మొదటి; src_li != ఖాళీ && dest_li != ఖాళీ; )
    {
	ఒకవేళ (విలువ_check_తాళం(dest_li->li_tv.v_తాళం, varname, FALSE))
	    తిరిగి FAIL;
	src_li = src_li->li_తదుపరి;
	ఒకవేళ (src_li == ఖాళీ || (!ఖాళీలు_idx2 && idx2 == idx))
	    ఆపు;
	dest_li = dest_li->li_తదుపరి;
	++idx;
    }

    ఒకవేళ (లోపల_vim9script() && dest->lv_type != ఖాళీ
					   && dest->lv_type->tt_member != ఖాళీ)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = మొదటి_li;
    కొరకు (src_li = src->lv_మొదటి; src_li != ఖాళీ; )
    {
	ఒకవేళ (op != ఖాళీ && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	లేకపోతే
	{
	    ఒకవేళ (member_type != ఖాళీ
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      ఖాళీ, 0) == FAIL)
		తిరిగి FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_తదుపరి;
	ఒకవేళ (src_li == ఖాళీ || (!ఖాళీలు_idx2 && idx2 == idx))
	    ఆపు;
	ఒకవేళ (dest_li->li_తదుపరి == ఖాళీ)
	{
	    // Need to add an empty item.
	    ఒకవేళ (list_append_సంఖ్య(dest, 0) == FAIL)
	    {
		src_li = ఖాళీ;
		ఆపు;
	    }
	}
	dest_li = dest_li->li_తదుపరి;
	++idx;
    }
    ఒకవేళ (src_li != ఖాళీ)
    {
	emsg(_(e_list_విలువ_has_మరిన్ని_వస్తువులు_than_targets));
	తిరిగి FAIL;
    }
    ఒకవేళ (ఖాళీలు_idx2
	    ? (dest_li != ఖాళీ && dest_li->li_తదుపరి != ఖాళీ)
	    : idx != idx2)
    {
	emsg(_(e_list_విలువ_does_not_have_enough_వస్తువులు));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    స్థిర శూన్య
list_flatten(list_T *list, listitem_T *మొదటి, దీర్ఘ maxitems, దీర్ఘ maxdepth)
{
    listitem_T	*అంశం;
    పూర్ణాంకం		పూర్తి = 0;

    ఒకవేళ (maxdepth == 0)
	తిరిగి;
    CHECK_LIST_MATERIALIZE(list);
    ఒకవేళ (మొదటి == ఖాళీ)
	అంశం = list->lv_మొదటి;
    లేకపోతే
	అంశం = మొదటి;

    అయితే (అంశం != ఖాళీ && పూర్తి < maxitems)
    {
	listitem_T	*తదుపరి = అంశం->li_తదుపరి;

	fast_breakcheck();
	ఒకవేళ (got_int)
	    తిరిగి;

	ఒకవేళ (అంశం->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = అంశం->li_tv.vval.v_list;

	    vimlist_remove(list, అంశం, అంశం);
	    ఒకవేళ (list_extend(list, itemlist, తదుపరి) == FAIL)
	    {
		list_free_అంశం(list, అంశం);
		తిరిగి;
	    }

	    ఒకవేళ (maxdepth > 0)
		list_flatten(list, అంశం->li_prev == ఖాళీ
				     ? list->lv_మొదటి : అంశం->li_prev->li_తదుపరి,
				itemlist->lv_len, maxdepth - 1);
	    clear_tv(&అంశం->li_tv);
	    list_free_అంశం(list, అంశం);
	}

	++పూర్తి;
	అంశం = తదుపరి;
    }
}

/*
 * "flatten()" and "flattennew()" functions
 */
    స్థిర శూన్య
flatten_common(typval_T *argvars, typval_T *rettv, పూర్ణాంకం make_copy)
{
    list_T  *l;
    దీర్ఘ    maxdepth;
    పూర్ణాంకం	    error = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_వాదం_యొక్క_str_must_be_list), "flatten()");
	తిరిగి;
    }

    ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    లేకపోతే
    {
	maxdepth = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[1], &error);
	ఒకవేళ (error)
	    తిరిగి;
	ఒకవేళ (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_సంఖ్య));
	    తిరిగి;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;

    ఒకవేళ (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	ఒకవేళ (l == ఖాళీ)
	    తిరిగి;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = ఖాళీ;
    }
    లేకపోతే
    {
	ఒకవేళ (విలువ_check_తాళం(l->lv_తాళం,
				     (char_u *)N_("flatten() argument"), TRUE))
	    తిరిగి;
	++l->lv_refcount;
    }

    list_flatten(l, ఖాళీ, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    శూన్య
f_flatten(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script())
	emsg(_(e_cannot_use_flatten_లోపల_vim9_script));
    లేకపోతే
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    శూన్య
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    శూన్య
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    కొరకు (idx = 0, li = l->lv_మొదటి; li != ఖాళీ; li = li->li_తదుపరి, ++idx)
    {
	list_T	    *l2 = list_alloc();

	ఒకవేళ (l2 == ఖాళీ)
	    ఆపు;
	ఒకవేళ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ఆపు;
	}
	ఒకవేళ (list_append_సంఖ్య(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    ఆపు;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    శూన్య
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;
    ఒకవేళ (p == ఖాళీ)
	తిరిగి;  // null string behaves like an empty string

    కొరకు (idx = 0; *p != NUL; ++idx)
    {
	పూర్ణాంకం	    len = mb_ptr2len(p);
	list_T	    *l2;

	ఒకవేళ (len == 0)
	    ఆపు;
	l2 = list_alloc();
	ఒకవేళ (l2 == ఖాళీ)
	    ఆపు;
	ఒకవేళ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ఆపు;
	}
	ఒకవేళ (list_append_సంఖ్య(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    ఆపు;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    పూర్ణాంకం
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*అంశం;
    పూర్ణాంకం		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    ఒకవేళ (l2 == ఖాళీ || l2->lv_len == 0)
	తిరిగి OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == ఖాళీ ? ఖాళీ : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    కొరకు (అంశం = l2->lv_మొదటి; అంశం != ఖాళీ && --todo >= 0;
				 అంశం = అంశం == bef_prev ? bef : అంశం->li_తదుపరి)
	ఒకవేళ (list_చొప్పించు_tv(l1, &అంశం->li_tv, bef) == FAIL)
	    తిరిగి FAIL;
    తిరిగి OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    పూర్ణాంకం
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    పూర్ణాంకం		len1 = l1 == ఖాళీ ? 0 : l1->lv_len;
    పూర్ణాంకం		len2 = l2 == ఖాళీ ? 0 : l2->lv_len;
    దీర్ఘ	totallen = (దీర్ఘ)len1 + (దీర్ఘ)len2;
    పూర్ణాంకం		i;
    listitem_T	*అంశం;

    ఒకవేళ (totallen == 0)
    {
	l = list_alloc();
	ఒకవేళ (l == ఖాళీ)
	    తిరిగి FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_తాళం = 0;
	tv->vval.v_list = l;
	తిరిగి OK;
    }
    ఒకవేళ (totallen > గరిష్ఠ_పూర్ణాంకం)
	తిరిగి FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_వస్తువులు((పూర్ణాంకం)totallen);
    ఒకవేళ (l == ఖాళీ)
	తిరిగి FAIL;

    i = 0;
    ఒకవేళ (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	కొరకు (అంశం = l1->lv_మొదటి; అంశం != ఖాళీ && !got_int;
		అంశం = అంశం->li_తదుపరి)
	{
	    typval_T new_tv;

	    copy_tv(&అంశం->li_tv, &new_tv);
	    list_సమితి_అంశం(l, i++, &new_tv);
	}
    }
    ఒకవేళ (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	కొరకు (అంశం = l2->lv_మొదటి; అంశం != ఖాళీ && !got_int;
		అంశం = అంశం->li_తదుపరి)
	{
	    typval_T new_tv;

	    copy_tv(&అంశం->li_tv, &new_tv);
	    list_సమితి_అంశం(l, i++, &new_tv);
	}
    }

    ++l->lv_refcount;
    tv->v_type = VAR_LIST;
    tv->v_తాళం = 0;
    tv->vval.v_list = l;
    తిరిగి OK;
}

    list_T *
list_ముక్క(list_T *ol, దీర్ఘ n1, దీర్ఘ n2)
{
    listitem_T	*అంశం;
    list_T	*l;
    దీర్ఘ	లెక్క;
    దీర్ఘ	idx;

    ఒకవేళ (n2 < n1)
	తిరిగి list_alloc();
    ఒకవేళ (n1 > గరిష్ఠ_పూర్ణాంకం - (n2 - n1) - 1)
	తిరిగి ఖాళీ;
    లెక్క = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_వస్తువులు((పూర్ణాంకం)లెక్క);
    ఒకవేళ (l == ఖాళీ)
	తిరిగి ఖాళీ;

    అంశం = list_కనుగొను(ol, n1);
    కొరకు (idx = 0; idx < లెక్క; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	ఒకవేళ (అంశం == ఖాళీ)
	{
	    list_free(l);
	    తిరిగి ఖాళీ;
	}

	copy_tv(&అంశం->li_tv, &new_tv);
	list_సమితి_అంశం(l, (పూర్ణాంకం)idx, &new_tv);
	అంశం = అంశం->li_తదుపరి;
    }
    తిరిగి l;
}

    పూర్ణాంకం
list_ముక్క_or_సూచీ(
	    list_T	*list,
	    పూర్ణాంకం		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    పూర్ణాంకం		exclusive,
	    typval_T	*rettv,
	    పూర్ణాంకం		verbose)
{
    దీర్ఘ	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ఒకవేళ (n1 < 0)
	n1 = len + n1;
    ఒకవేళ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	ఒకవేళ (!range)
	{
	    ఒకవేళ (verbose)
		semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), (దీర్ఘ)n1_arg);
	    తిరిగి FAIL;
	}
	ఒకవేళ (లోపల_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	లేకపోతే
	    n1 = len;
    }
    ఒకవేళ (range)
    {
	list_T	*l;

	ఒకవేళ (n2 < 0)
	    n2 = len + n2;
	లేకపోతే ఒకవేళ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ఒకవేళ (exclusive)
	    --n2;
	ఒకవేళ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_ముక్క(list, n1, n2);
	ఒకవేళ (l == ఖాళీ)
	    తిరిగి FAIL;
	clear_tv(rettv);
	rettv_list_సమితి(rettv, l);
    }
    లేకపోతే
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	దీర్ఘ సూచీ = n1;
	listitem_T *li = check_range_సూచీ_one(list, &సూచీ, TRUE, TRUE);
	ఒకవేళ (li == ఖాళీ)
	    తిరిగి FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	ఒకవేళ (లోపల_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != ఖాళీ
					&& var1.vval.v_list->lv_type == ఖాళీ)
	{
	    ఒకవేళ (list->lv_type != ఖాళీ && list->lv_type->tt_member != ఖాళీ)
		సమితి_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    తిరిగి OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, పూర్ణాంకం deep, పూర్ణాంకం శిఖరం, పూర్ణాంకం copyID)
{
    list_T	*copy;
    listitem_T	*అంశం;
    పూర్ణాంకం		idx = 0;

    ఒకవేళ (orig == ఖాళీ)
	తిరిగి ఖాళీ;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_వస్తువులు(orig->lv_len);
    ఒకవేళ (copy == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (orig->lv_type == ఖాళీ || శిఖరం || deep)
	copy->lv_type = ఖాళీ;
    లేకపోతే
	copy->lv_type = alloc_type(orig->lv_type);
    ఒకవేళ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    కొరకు (అంశం = orig->lv_మొదటి; అంశం != ఖాళీ && !got_int;
	    అంశం = అంశం->li_తదుపరి)
    {
	typval_T	new_tv;

	ఒకవేళ (deep)
	{
	    ఒకవేళ (అంశం_copy(&అంశం->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		ఆపు;
	}
	లేకపోతే
	    copy_tv(&అంశం->li_tv, &new_tv);
	list_సమితి_అంశం(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    ఒకవేళ (అంశం != ఖాళీ)
    {
	list_unref(copy);
	copy = ఖాళీ;
    }

    తిరిగి copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    శూన్య
vimlist_remove(list_T *l, listitem_T *అంశం, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    కొరకు (ip = అంశం; ip != ఖాళీ; ip = ip->li_తదుపరి)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	ఒకవేళ (ip == item2)
	    ఆపు;
    }

    ఒకవేళ (item2->li_తదుపరి == ఖాళీ)
	l->lv_u.mat.lv_చివరి = అంశం->li_prev;
    లేకపోతే
	item2->li_తదుపరి->li_prev = అంశం->li_prev;
    ఒకవేళ (అంశం->li_prev == ఖాళీ)
	l->lv_మొదటి = item2->li_తదుపరి;
    లేకపోతే
	అంశం->li_prev->li_తదుపరి = item2->li_తదుపరి;
    l->lv_u.mat.lv_idx_అంశం = ఖాళీ;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, పూర్ణాంకం copyID, పూర్ణాంకం restore_copyID)
{
    garray_T	ga;

    ఒకవేళ (tv->vval.v_list == ఖాళీ)
	తిరిగి ఖాళీ;
    ga_init2(&ga, పరిమాణం(అక్షరం), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    ఒకవేళ (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_దత్తాంశం);
	తిరిగి ఖాళీ;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    తిరిగి (char_u *)ga.ga_దత్తాంశం;
}

రకనిర్వచనం నిర్మాణం join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    స్థిర పూర్ణాంకం
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    పూర్ణాంకం		echo_style,
    పూర్ణాంకం		restore_copyID,
    పూర్ణాంకం		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    పూర్ణాంకం		i;
    join_T	*p;
    దీర్ఘ	sumlen = 0;
    పూర్ణాంకం		మొదటి = TRUE;
    పూర్ణాంకం		prev_తాళం;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    listitem_T	*అంశం;
    string_T	s;
    size_t	seplen;

    // Stringify each item in the list.
    CHECK_LIST_MATERIALIZE(l);
    // Lock the list, the string() method of an object item could remove the
    // item the loop is standing on.
    prev_తాళం = l->lv_తాళం;
    ఒకవేళ (l->lv_తాళం == 0)
	l->lv_తాళం = VAR_LOCKED;
    కొరకు (అంశం = l->lv_మొదటి; అంశం != ఖాళీ && !got_int; అంశం = అంశం->li_తదుపరి)
    {
	s.string = echo_string_core(&అంశం->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ఒకవేళ (s.string == ఖాళీ)
	{
	    l->lv_తాళం = prev_తాళం;
	    తిరిగి FAIL;
	}

	s.నిడివి = STRLEN(s.string);
	sumlen += (దీర్ఘ)s.నిడివి;

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
    l->lv_తాళం = prev_తాళం;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    ఒకవేళ (join_gap->ga_len >= 2)
	sumlen += (దీర్ఘ)seplen * (join_gap->ga_len - 1);
    ఒకవేళ (sumlen > గరిష్ఠ_పూర్ణాంకం - 2 || ga_grow(gap, (పూర్ణాంకం)sumlen + 2) == FAIL)
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

    తిరిగి OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    పూర్ణాంకం
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    పూర్ణాంకం		echo_style,
    పూర్ణాంకం		restore_copyID,
    పూర్ణాంకం		copyID)
{
    garray_T	join_ga;
    పూర్ణాంకం		retval;
    join_T	*p;
    పూర్ణాంకం		i;

    ఒకవేళ (l->lv_len < 1)
	తిరిగి OK; // nothing to do
    ga_init2(&join_ga, పరిమాణం(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
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
 * "join()" function
 */
    శూన్య
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    ఒకవేళ (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	తిరిగి;

    ఒకవేళ ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == ఖాళీ)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == ఖాళీ))
	తిరిగి;

    ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    లేకపోతే
	sep = tv_get_string_chk(&argvars[1]);

    ఒకవేళ (sep != ఖాళీ)
    {
	ga_init2(&ga, పరిమాణం(అక్షరం), 80);
	ఒకవేళ (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	లేకపోతే
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_దత్తాంశం;
    }
    లేకపోతే
	rettv->vval.v_string = ఖాళీ;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    పూర్ణాంకం
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, పూర్ణాంకం do_error)
{
    పూర్ణాంకం		evaluate = evalarg == ఖాళీ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = ఖాళీ;
    typval_T	tv;
    listitem_T	*అంశం;
    పూర్ణాంకం		vim9script = లోపల_vim9script();
    పూర్ణాంకం		had_comma;

    ఒకవేళ (evaluate)
    {
	l = list_alloc();
	ఒకవేళ (l == ఖాళీ)
	    తిరిగి FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    అయితే (**arg != ']' && **arg != NUL)
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
	    అంశం = listitem_alloc();
	    ఒకవేళ (అంశం != ఖాళీ)
	    {
		అంశం->li_tv = tv;
		అంశం->li_tv.v_తాళం = 0;
		list_append(l, అంశం);
	    }
	    లేకపోతే
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	ఒకవేళ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ఒకవేళ (had_comma)
	{
	    ఒకవేళ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		వెళ్ళు failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ఒకవేళ (**arg == ']')
	    ఆపు;

	ఒకవేళ (!had_comma)
	{
	    ఒకవేళ (do_error)
	    {
		ఒకవేళ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		లేకపోతే
		    semsg(_(e_missing_comma_లోపల_list_str), *arg);
	    }
	    వెళ్ళు failret;
	}
    }

    ఒకవేళ (**arg != ']')
    {
	ఒకవేళ (do_error)
	    semsg(_(e_missing_చివర_యొక్క_list_rsb_str), *arg);
failret:
	ఒకవేళ (evaluate)
	    list_free(l);
	తిరిగి FAIL;
    }

    *arg += 1;
    ఒకవేళ (evaluate)
	rettv_list_సమితి(rettv, l);

    తిరిగి OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    పూర్ణాంకం
write_list(FILE *fd, list_T *list, పూర్ణాంకం ద్విమాన)
{
    listitem_T	*li;
    పూర్ణాంకం		c;
    పూర్ణాంకం		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_వస్తువులు(list, li)
    {
	కొరకు (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    ఒకవేళ (*s == '\n')
		c = putc(NUL, fd);
	    లేకపోతే
		c = putc(*s, fd);
	    ఒకవేళ (c == EOF)
	    {
		ret = FAIL;
		ఆపు;
	    }
	}
	ఒకవేళ (!ద్విమాన || li->li_తదుపరి != ఖాళీ)
	    ఒకవేళ (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		ఆపు;
	    }
	ఒకవేళ (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    ఆపు;
	}
    }
    తిరిగి ret;
}

/*
 * Initialize a static list with 10 items.
 */
    శూన్య
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    పూర్ణాంకం	    i;

    CLEAR_సూచిక(sl);
    l->lv_మొదటి = &sl->sl_వస్తువులు[0];
    l->lv_u.mat.lv_చివరి = &sl->sl_వస్తువులు[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_తాళం = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    కొరకు (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_వస్తువులు[i];

	ఒకవేళ (i == 0)
	    li->li_prev = ఖాళీ;
	లేకపోతే
	    li->li_prev = li - 1;
	ఒకవేళ (i == 9)
	    li->li_తదుపరి = ఖాళీ;
	లేకపోతే
	    li->li_తదుపరి = li + 1;
    }
}

/*
 * "list2str()" function
 */
    శూన్య
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    పూర్ణాంకం		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ఖాళీ;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	తిరిగి;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    l = argvars[0].vval.v_list;
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;  // empty list results in empty string

    ఒకవేళ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (పూర్ణాంకం)tv_get_bool_chk(&argvars[1], ఖాళీ);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    ఒకవేళ (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	పూర్ణాంకం	(*char2bytes)(పూర్ణాంకం, char_u *);

	ఒకవేళ (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	లేకపోతే
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_వస్తువులు(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_సంఖ్య(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    లేకపోతే
    {
	దీర్ఘ len = (దీర్ఘ)list_len(l) + 1;

	ఒకవేళ (len <= గరిష్ఠ_పూర్ణాంకం && ga_grow(&ga, (పూర్ణాంకం)len) == OK)
	{
	    FOR_ALL_LIST_వస్తువులు(l, li)
		ga_append(&ga, tv_get_సంఖ్య(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_దత్తాంశం;
}

/*
 * "list2tuple()" function
 */
    శూన్య
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ఖాళీ;

    ఒకవేళ (check_for_list_arg(argvars, 0) == FAIL)
	తిరిగి;

    l = argvars[0].vval.v_list;
    ఒకవేళ (l == ఖాళీ)
	తిరిగి;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    ఒకవేళ (rettv_tuple_సమితి_with_వస్తువులు(rettv, list_len(l)) == FAIL)
	తిరిగి;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_అంశం(tuple, TUPLE_LEN(tuple)));
	tuple->tv_వస్తువులు.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    శూన్య
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    ఒకవేళ (check_for_tuple_arg(argvars, 0) == FAIL)
	తిరిగి;

    tuple = argvars[0].vval.v_tuple;
    ఒకవేళ (tuple == ఖాళీ)
	తిరిగి;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    కొరకు (పూర్ణాంకం i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_అంశం(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    స్థిర శూన్య
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*అంశం, *item2;
    listitem_T	*li;
    పూర్ణాంకం		error = FALSE;
    దీర్ఘ	idx;
    దీర్ఘ	చివర;
    పూర్ణాంకం		cnt = 0;
    list_T	*rl;

    ఒకవేళ ((l = argvars[0].vval.v_list) == ఖాళీ
			     || విలువ_check_తాళం(l->lv_తాళం, arg_errmsg, TRUE))
	తిరిగి;

    idx = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[1], &error);
    ఒకవేళ (error)
	తిరిగి;		// type error: do nothing, errmsg already given

    ఒకవేళ ((అంశం = list_కనుగొను(l, idx)) == ఖాళీ)
    {
	semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), idx);
	తిరిగి;
    }

    ఒకవేళ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, అంశం, అంశం);
	*rettv = అంశం->li_tv;
	list_free_అంశం(l, అంశం);
	తిరిగి;
    }

    // Remove range of items, return list with values.
    చివర = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], &error);
    ఒకవేళ (error)
	తిరిగి;		// type error: do nothing

    ఒకవేళ ((item2 = list_కనుగొను(l, చివర)) == ఖాళీ)
    {
	semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), చివర);
	తిరిగి;
    }

    కొరకు (li = అంశం; li != ఖాళీ; li = li->li_తదుపరి)
    {
	++cnt;
	ఒకవేళ (li == item2)
	    ఆపు;
    }
    ఒకవేళ (li == ఖాళీ)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	తిరిగి;
    }

    vimlist_remove(l, అంశం, item2);
    ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	తిరిగి;

    rl = rettv->vval.v_list;

    ఒకవేళ (l->lv_with_వస్తువులు > 0)
    {
	// need to copy the list items and move the value
	అయితే (అంశం != ఖాళీ)
	{
	    li = listitem_alloc();
	    ఒకవేళ (li == ఖాళీ)
		తిరిగి;
	    li->li_tv = అంశం->li_tv;
	    init_tv(&అంశం->li_tv);
	    list_append(rl, li);
	    ఒకవేళ (అంశం == item2)
		ఆపు;
	    అంశం = అంశం->li_తదుపరి;
	}
    }
    లేకపోతే
    {
	rl->lv_మొదటి = అంశం;
	rl->lv_u.mat.lv_చివరి = item2;
	అంశం->li_prev = ఖాళీ;
	item2->li_తదుపరి = ఖాళీ;
	rl->lv_len = cnt;
    }
}

స్థిర పూర్ణాంకం అంశం_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2);
స్థిర పూర్ణాంకం అంశం_compare2(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2);

// struct used in the array that's given to qsort()
రకనిర్వచనం నిర్మాణం
{
    listitem_T	*అంశం;
    పూర్ణాంకం		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    సంఘం {
	varnumber_T	inum;	// for "N"
	ద్విగుణ		fnum;	// for "n" and "f"
    } కీలకం;
} క్రమీకరించు_అంశం_T;

// struct storing information about current sort
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		అంశం_compare_ic;
    పూర్ణాంకం		అంశం_compare_lc;
    పూర్ణాంకం		అంశం_compare_numeric;
    పూర్ణాంకం		అంశం_compare_numbers;
    పూర్ణాంకం		అంశం_compare_float;
    char_u	*అంశం_compare_func;
    partial_T	*అంశం_compare_partial;
    dict_T	*అంశం_compare_selfdict;
    పూర్ణాంకం		అంశం_compare_func_err;
    పూర్ణాంకం		అంశం_compare_keep_zero;
    పూర్ణాంకం		అంశం_compare_కీలకాలు_ready;  // ptrs[].key is precomputed
} sortinfo_T;
స్థిర sortinfo_T	*sortinfo = ఖాళీ;
#నిర్వచించు అంశం_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    స్థిర పూర్ణాంకం
అంశం_compare(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    క్రమీకరించు_అంశం_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = ఖాళీ, *tofree2 = ఖాళీ;
    పూర్ణాంకం		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (క్రమీకరించు_అంశం_T *)s1;
    si2 = (క్రమీకరించు_అంశం_T *)s2;
    tv1 = &si1->అంశం->li_tv;
    tv2 = &si2->అంశం->li_tv;

    ఒకవేళ (sortinfo->అంశం_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->అంశం_compare_కీలకాలు_ready
					? si1->కీలకం.inum : tv_గమ్యం_సంఖ్య(tv1);
	varnumber_T	v2 = sortinfo->అంశం_compare_కీలకాలు_ready
					? si2->కీలకం.inum : tv_గమ్యం_సంఖ్య(tv2);

	తిరిగి v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ఒకవేళ (sortinfo->అంశం_compare_float)
    {
	float_T	v1 = sortinfo->అంశం_compare_కీలకాలు_ready
					? si1->కీలకం.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->అంశం_compare_కీలకాలు_ready
					? si2->కీలకం.fnum : tv_get_float(tv2);

	తిరిగి v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ఒకవేళ (sortinfo->అంశం_compare_numeric && sortinfo->అంశం_compare_కీలకాలు_ready)
    {
	ద్విగుణ	n1 = si1->కీలకం.fnum;
	ద్విగుణ	n2 = si2->కీలకం.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	ఒకవేళ (res == 0 && !sortinfo->అంశం_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	తిరిగి res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    ఒకవేళ (tv1->v_type == VAR_STRING)
    {
	ఒకవేళ (tv2->v_type != VAR_STRING || sortinfo->అంశం_compare_numeric)
	    p1 = (char_u *)"'";
	లేకపోతే
	    p1 = tv1->vval.v_string;
    }
    లేకపోతే
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    ఒకవేళ (tv2->v_type == VAR_STRING)
    {
	ఒకవేళ (tv1->v_type != VAR_STRING || sortinfo->అంశం_compare_numeric)
	    p2 = (char_u *)"'";
	లేకపోతే
	    p2 = tv2->vval.v_string;
    }
    లేకపోతే
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    ఒకవేళ (p1 == ఖాళీ)
	p1 = (char_u *)"";
    ఒకవేళ (p2 == ఖాళీ)
	p2 = (char_u *)"";
    ఒకవేళ (!sortinfo->అంశం_compare_numeric)
    {
	ఒకవేళ (sortinfo->అంశం_compare_lc)
	    res = strcoll((అక్షరం *)p1, (అక్షరం *)p2);
	లేకపోతే
	    res = sortinfo->అంశం_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    లేకపోతే
    {
	ద్విగుణ n1, n2;
	n1 = strtod((అక్షరం *)p1, (అక్షరం **)&p1);
	n2 = strtod((అక్షరం *)p2, (అక్షరం **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    ఒకవేళ (res == 0 && !sortinfo->అంశం_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    తిరిగి res;
}

    స్థిర పూర్ణాంకం
అంశం_compare2(స్థిరరాశి శూన్య *s1, స్థిరరాశి శూన్య *s2)
{
    క్రమీకరించు_అంశం_T  *si1, *si2;
    పూర్ణాంకం		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->అంశం_compare_partial;
    funcexe_T	funcexe;
    పూర్ణాంకం		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    ఒకవేళ (sortinfo->అంశం_compare_func_err)
	తిరిగి 0;

    si1 = (క్రమీకరించు_అంశం_T *)s1;
    si2 = (క్రమీకరించు_అంశం_T *)s2;

    ఒకవేళ (partial == ఖాళీ)
	func_name = sortinfo->అంశం_compare_func;
    లేకపోతే
	func_name = partial_name(partial);

    // Copy the values.  This is needed to be able to set v_lock to VAR_FIXED
    // in the copy without changing the original list items.
    copy_tv(&si1->అంశం->li_tv, &argv[0]);
    copy_tv(&si2->అంశం->li_tv, &argv[1]);

    rettv.v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_క్షేత్రం(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = partial;
    funcexe.fe_selfdict = sortinfo->అంశం_compare_selfdict;
    res = call_func(func_name, -1, &rettv, 2, argv, &funcexe);
    clear_tv(&argv[0]);
    clear_tv(&argv[1]);

    ఒకవేళ (res == FAIL || did_emsg > did_emsg_before)
	res = అంశం_COMPARE_FAIL;
    లేకపోతే
    {
	varnumber_T	n;

	n = tv_get_సంఖ్య_chk(&rettv, &sortinfo->అంశం_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    ఒకవేళ (sortinfo->అంశం_compare_func_err)
	res = అంశం_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    ఒకవేళ (res == 0 && !sortinfo->అంశం_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    తిరిగి res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    స్థిర శూన్య
క్రమీకరించు_compute_కీలకాలు(క్రమీకరించు_అంశం_T *ptrs, దీర్ఘ len, sortinfo_T *info)
{
    దీర్ఘ	i;

    ఒకవేళ (info->అంశం_compare_numbers)
    {
	కొరకు (i = 0; i < len; ++i)
	    ptrs[i].కీలకం.inum = tv_గమ్యం_సంఖ్య(&ptrs[i].అంశం->li_tv);
    }
    లేకపోతే ఒకవేళ (info->అంశం_compare_float)
    {
	కొరకు (i = 0; i < len; ++i)
	    ptrs[i].కీలకం.fnum = tv_get_float(&ptrs[i].అంశం->li_tv);
    }
    లేకపోతే // info->item_compare_numeric
    {
	కొరకు (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].అంశం->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    ఒకవేళ (tv->v_type == VAR_STRING)
		ptrs[i].కీలకం.fnum = 0.0;
	    లేకపోతే
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = ఖాళీ;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].కీలకం.fnum = p == ఖాళీ ? 0.0 : strtod((అక్షరం *)p, ఖాళీ);
		vim_free(tofree);
	    }
	}
    }
    info->అంశం_compare_కీలకాలు_ready = TRUE;
}

/*
 * sort() List "l"
 */
    స్థిర శూన్య
do_క్రమీకరించు(list_T *l, sortinfo_T *info)
{
    దీర్ఘ	len;
    క్రమీకరించు_అంశం_T	*ptrs;
    దీర్ఘ	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(క్రమీకరించు_అంశం_T, len);
    ఒకవేళ (ptrs == ఖాళీ)
	తిరిగి;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_వస్తువులు(l, li)
    {
	ptrs[i].అంశం = li;
	ptrs[i].idx = i;
	++i;
    }

    info->అంశం_compare_func_err = FALSE;
    info->అంశం_compare_keep_zero = FALSE;
    info->అంశం_compare_కీలకాలు_ready = FALSE;

    ఒకవేళ (info->అంశం_compare_func == ఖాళీ && info->అంశం_compare_partial == ఖాళీ
	    && (info->అంశం_compare_numbers || info->అంశం_compare_float
						|| info->అంశం_compare_numeric))
	క్రమీకరించు_compute_కీలకాలు(ptrs, len, info);
    // test the compare function
    ఒకవేళ ((info->అంశం_compare_func != ఖాళీ
		|| info->అంశం_compare_partial != ఖాళీ)
	    && అంశం_compare2((శూన్య *)&ptrs[0], (శూన్య *)&ptrs[1])
	    == అంశం_COMPARE_FAIL)
	emsg(_(e_క్రమీకరించు_compare_function_failed));
    లేకపోతే
    {
	// Sort the array with item pointers.
	qsort((శూన్య *)ptrs, (size_t)len, పరిమాణం(క్రమీకరించు_అంశం_T),
		info->అంశం_compare_func == ఖాళీ
		&& info->అంశం_compare_partial == ఖాళీ
		? అంశం_compare : అంశం_compare2);

	ఒకవేళ (!info->అంశం_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_మొదటి = l->lv_u.mat.lv_చివరి
		= l->lv_u.mat.lv_idx_అంశం = ఖాళీ;
	    l->lv_len = 0;
	    కొరకు (i = 0; i < len; ++i)
		list_append(l, ptrs[i].అంశం);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    స్థిర శూన్య
do_uniq(list_T *l, sortinfo_T *info)
{
    దీర్ఘ	len;
    క్రమీకరించు_అంశం_T	*ptrs;
    దీర్ఘ	i = 0;
    listitem_T	*li;
    పూర్ణాంకం	(*అంశం_compare_func_ptr)(స్థిరరాశి శూన్య *, స్థిరరాశి శూన్య *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(క్రమీకరించు_అంశం_T, len);
    ఒకవేళ (ptrs == ఖాళీ)
	తిరిగి;

    // f_uniq(): ptrs will be a stack of items to remove
    info->అంశం_compare_func_err = FALSE;
    info->అంశం_compare_keep_zero = TRUE;
    అంశం_compare_func_ptr = info->అంశం_compare_func != ఖాళీ
	|| info->అంశం_compare_partial != ఖాళీ
	? అంశం_compare2 : అంశం_compare;

    కొరకు (li = l->lv_మొదటి; li != ఖాళీ && li->li_తదుపరి != ఖాళీ;
	    li = li->li_తదుపరి)
    {
	ఒకవేళ (అంశం_compare_func_ptr((శూన్య *)&li, (శూన్య *)&li->li_తదుపరి)
		== 0)
	    ptrs[i++].అంశం = li;
	ఒకవేళ (info->అంశం_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    ఆపు;
	}
    }

    ఒకవేళ (!info->అంశం_compare_func_err)
    {
	అయితే (--i >= 0)
	{
	    li = ptrs[i].అంశం->li_తదుపరి;
	    ptrs[i].అంశం->li_తదుపరి = li->li_తదుపరి;
	    ఒకవేళ (li->li_తదుపరి != ఖాళీ)
		li->li_తదుపరి->li_prev = ptrs[i].అంశం;
	    లేకపోతే
		l->lv_u.mat.lv_చివరి = ptrs[i].అంశం;
	    list_fix_watch(l, li);
	    listitem_free(l, li);
	    l->lv_len--;
	}
    }

    vim_free(ptrs);
}

/*
 * Parse the optional arguments supplied to the sort() or uniq() function and
 * return the values in "info".
 */
    స్థిర పూర్ణాంకం
parse_క్రమీకరించు_uniq_వాదనలు(typval_T *argvars, sortinfo_T *info)
{
    info->అంశం_compare_ic = FALSE;
    info->అంశం_compare_lc = FALSE;
    info->అంశం_compare_numeric = FALSE;
    info->అంశం_compare_numbers = FALSE;
    info->అంశం_compare_float = FALSE;
    info->అంశం_compare_func = ఖాళీ;
    info->అంశం_compare_partial = ఖాళీ;
    info->అంశం_compare_selfdict = ఖాళీ;
    info->అంశం_compare_కీలకాలు_ready = FALSE;

    ఒకవేళ (argvars[1].v_type == VAR_UNKNOWN)
	తిరిగి OK;

    // optional second argument: {func}
    ఒకవేళ (argvars[1].v_type == VAR_FUNC)
	info->అంశం_compare_func = argvars[1].vval.v_string;
    లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_PARTIAL)
	info->అంశం_compare_partial = argvars[1].vval.v_partial;
    లేకపోతే
    {
	పూర్ణాంకం	    error = FALSE;
	పూర్ణాంకం	    nr = 0;

	ఒకవేళ (argvars[1].v_type == VAR_సంఖ్య)
	{
	    nr = tv_get_సంఖ్య_chk(&argvars[1], &error);
	    ఒకవేళ (error)
		తిరిగి FAIL;
	    ఒకవేళ (nr == 1)
		info->అంశం_compare_ic = TRUE;
	}
	ఒకవేళ (nr != 1)
	{
	    ఒకవేళ (argvars[1].v_type != VAR_సంఖ్య)
		info->అంశం_compare_func = tv_get_string(&argvars[1]);
	    లేకపోతే ఒకవేళ (nr != 0)
	    {
		emsg(_(e_invalid_వాదం));
		తిరిగి FAIL;
	    }
	}
	ఒకవేళ (info->అంశం_compare_func != ఖాళీ)
	{
	    ఒకవేళ (*info->అంశం_compare_func == NUL)
	    {
		// empty string means default sort
		info->అంశం_compare_func = ఖాళీ;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(info->అంశం_compare_func, "n") == 0)
	    {
		info->అంశం_compare_func = ఖాళీ;
		info->అంశం_compare_numeric = TRUE;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(info->అంశం_compare_func, "N") == 0)
	    {
		info->అంశం_compare_func = ఖాళీ;
		info->అంశం_compare_numbers = TRUE;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(info->అంశం_compare_func, "f") == 0)
	    {
		info->అంశం_compare_func = ఖాళీ;
		info->అంశం_compare_float = TRUE;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(info->అంశం_compare_func, "i") == 0)
	    {
		info->అంశం_compare_func = ఖాళీ;
		info->అంశం_compare_ic = TRUE;
	    }
	    లేకపోతే ఒకవేళ (STRCMP(info->అంశం_compare_func, "l") == 0)
	    {
		info->అంశం_compare_func = ఖాళీ;
		info->అంశం_compare_lc = TRUE;
	    }
	}
    }

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	ఒకవేళ (check_for_dict_arg(argvars, 2) == FAIL)
	    తిరిగి FAIL;
	info->అంశం_compare_selfdict = argvars[2].vval.v_dict;
    }

    తిరిగి OK;
}

/*
 * "sort()" or "uniq()" function
 */
    స్థిర శూన్య
do_క్రమీకరించు_uniq(typval_T *argvars, typval_T *rettv, పూర్ణాంకం క్రమీకరించు)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    దీర్ఘ	len;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_వాదం_యొక్క_str_must_be_list), క్రమీకరించు ? "sort()" : "uniq()");
	తిరిగి;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    ఒకవేళ (l != ఖాళీ && విలువ_check_తాళం(l->lv_తాళం,
		(char_u *)(క్రమీకరించు ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	వెళ్ళు theend;
    rettv_list_సమితి(rettv, l);
    ఒకవేళ (l == ఖాళీ)
	వెళ్ళు theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    ఒకవేళ (len <= 1)
	వెళ్ళు theend;	// short list sorts pretty quickly

    ఒకవేళ (parse_క్రమీకరించు_uniq_వాదనలు(argvars, &info) == FAIL)
	వెళ్ళు theend;

    ఒకవేళ (క్రమీకరించు)
	do_క్రమీకరించు(l, &info);
    లేకపోతే
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    శూన్య
f_క్రమీకరించు(typval_T *argvars, typval_T *rettv)
{
    do_క్రమీకరించు_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    శూన్య
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_క్రమీకరించు_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    పూర్ణాంకం
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*సమాసం,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	పూర్ణాంకం		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    పూర్ణాంకం		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    ఒకవేళ (filtermap == FILTERMAP_FOREACH && సమాసం->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(సమాసం->vval.v_string);
	ఒకవేళ (!did_emsg)
	    retval = OK;
	వెళ్ళు theend;
    }

    argv[0] = *get_vim_var_tv(VV_కీలకం);
    argv[1] = *get_vim_var_tv(VV_VAL);
    ఒకవేళ (eval_సమాసం_typval(సమాసం, FALSE, argv, 2, fc, newtv) == FAIL)
	వెళ్ళు theend;
    ఒకవేళ (filtermap == FILTERMAP_FILTER)
    {
	పూర్ణాంకం	    error = FALSE;

	// filter(): when expr is zero remove the item
	ఒకవేళ (లోపల_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	లేకపోతే
	    *remp = (tv_get_సంఖ్య_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	ఒకవేళ (error)
	    వెళ్ళు theend;
    }
    లేకపోతే ఒకవేళ (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    తిరిగి retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    స్థిర శూన్య
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	అక్షరం		*func_name,
	char_u		*arg_errmsg,
	typval_T	*సమాసం,
	typval_T	*rettv)
{
    పూర్ణాంకం		prev_తాళం;
    list_T	*l_ret = ఖాళీ;
    పూర్ణాంకం		idx = 0;
    పూర్ణాంకం		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    ఒకవేళ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = ఖాళీ;
    }
    ఒకవేళ (l == ఖాళీ || (filtermap == FILTERMAP_FILTER
			    && విలువ_check_తాళం(l->lv_తాళం, arg_errmsg, TRUE)))
	తిరిగి;

    prev_తాళం = l->lv_తాళం;

    ఒకవేళ (filtermap == FILTERMAP_MAPNEW)
    {
	ఒకవేళ (rettv_list_alloc(rettv) == FAIL)
	    తిరిగి;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    సమితి_vim_var_type(VV_కీలకం, VAR_సంఖ్య);

    ఒకవేళ (l->lv_తాళం == 0)
	l->lv_తాళం = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_సమాసం_get_funccal(సమాసం, &newtv);

    ఒకవేళ (l->lv_మొదటి == &range_list_అంశం)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_ప్రారంభం;
	పూర్ణాంకం		len = l->lv_len;
	పూర్ణాంకం		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	ఒకవేళ (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_మొదటి = ఖాళీ;
	    l->lv_u.mat.lv_చివరి = ఖాళీ;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_అంశం = ఖాళీ;
	}

	కొరకు (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_సంఖ్య;
	    tv.v_తాళం = 0;
	    tv.vval.v_సంఖ్య = val;
	    సమితి_vim_var_nr(VV_కీలకం, idx);
	    ఒకవేళ (filter_map_one(&tv, సమాసం, filtermap, fc, &newtv, &rem) == FAIL)
		ఆపు;
	    ఒకవేళ (did_emsg)
	    {
		clear_tv(&newtv);
		ఆపు;
	    }
	    ఒకవేళ (filtermap != FILTERMAP_FOREACH)
	    {
		ఒకవేళ (filtermap != FILTERMAP_FILTER)
		{
		    ఒకవేళ (filtermap == FILTERMAP_MAP && argtype != ఖాళీ
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			ఆపు;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    ఒకవేళ (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			ఆపు;
		}
		లేకపోతే ఒకవేళ (!rem)
		{
		    // filter(): append the list item value when not rem
		    ఒకవేళ (list_append_tv_move(l, &tv) == FAIL)
			ఆపు;
		}
	    }

	    val += stride;
	}
    }
    లేకపోతే
    {
	// Materialized list: loop over the items
	కొరకు (li = l->lv_మొదటి; li != ఖాళీ; li = nli)
	{
	    ఒకవేళ (filtermap == FILTERMAP_MAP && విలువ_check_తాళం(
			li->li_tv.v_తాళం, arg_errmsg, TRUE))
		ఆపు;
	    nli = li->li_తదుపరి;
	    సమితి_vim_var_nr(VV_కీలకం, idx);
	    ఒకవేళ (filter_map_one(&li->li_tv, సమాసం, filtermap, fc,
							 &newtv, &rem) == FAIL)
		ఆపు;
	    ఒకవేళ (did_emsg)
	    {
		clear_tv(&newtv);
		ఆపు;
	    }
	    ఒకవేళ (filtermap == FILTERMAP_MAP)
	    {
		ఒకవేళ (argtype != ఖాళీ && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ఆపు;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_తాళం = 0;
		li->li_tv = newtv;
	    }
	    లేకపోతే ఒకవేళ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		ఒకవేళ (list_append_tv_move(l_ret, &newtv) == FAIL)
		    ఆపు;
	    }
	    లేకపోతే ఒకవేళ (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_తాళం = prev_తాళం;
    ఒకవేళ (fc != ఖాళీ)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    స్థిర శూన్య
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*సమాసం;
    అక్షరం	*func_name = filtermap == FILTERMAP_MAP ? "map()"
				  : filtermap == FILTERMAP_MAPNEW ? "mapnew()"
				  : filtermap == FILTERMAP_FILTER ? "filter()"
				  : "foreach()";
    char_u	*arg_errmsg = (char_u *)(filtermap == FILTERMAP_MAP
							 ? N_("map() argument")
				       : filtermap == FILTERMAP_MAPNEW
						      ? N_("mapnew() argument")
				       : filtermap == FILTERMAP_FILTER
						      ? N_("filter() argument")
						   : N_("foreach() argument"));
    పూర్ణాంకం		save_did_emsg;
    type_T	*type = ఖాళీ;

    // map(), filter(), foreach() return the first argument, also on failure.
    ఒకవేళ (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	తిరిగి;
    }

    ఒకవేళ (filtermap == FILTERMAP_MAP && లోపల_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	ఒకవేళ (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != ఖాళీ)
	    type = argvars[0].vval.v_dict->dv_type;
	లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != ఖాళీ)
	    type = argvars[0].vval.v_list->lv_type;
    }

    ఒకవేళ (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	అక్షరం *సందేశం;

	ఒకవేళ (filtermap == FILTERMAP_FOREACH)
	    సందేశం = e_వాదం_యొక్క_str_must_be_list_tuple_string_dictionary_or_blob;
	లేకపోతే
	    సందేశం = e_వాదం_యొక్క_str_must_be_list_string_dictionary_or_blob;
	semsg(_(సందేశం), func_name);
	తిరిగి;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    సమాసం = &argvars[1];
    ఒకవేళ (సమాసం->v_type == VAR_UNKNOWN)
	తిరిగి;

    typval_T	save_val;
    typval_T	save_కీలకం;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_కీలకం, &save_కీలకం);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ఒకవేళ (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, సమాసం, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, సమాసం,
							    arg_errmsg, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, సమాసం, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, సమాసం);
    లేకపోతే // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, సమాసం, rettv);

    restore_vimvar(VV_కీలకం, &save_కీలకం);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    శూన్య
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    శూన్య
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    శూన్య
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    శూన్య
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    స్థిర శూన్య
list_కూడు(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    ఒకవేళ (l == ఖాళీ)
    {
	ఒకవేళ (లోపల_vim9script())
	    emsg(_(e_cannot_కూడు_గమ్యం_null_list));
    }
    లేకపోతే ఒకవేళ (!విలువ_check_తాళం(l->lv_తాళం,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    శూన్య
f_కూడు(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_సంఖ్య = 1; // Default: Failed

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_సంఖ్య_arg(argvars, 1) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_LIST)
	list_కూడు(argvars, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_BLOB)
	blob_కూడు(argvars, rettv);
    లేకపోతే
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    స్థిర దీర్ఘ
list_లెక్క(list_T *l, typval_T *needle, దీర్ఘ idx, పూర్ణాంకం ic)
{
    దీర్ఘ	n = 0;
    listitem_T	*li;

    ఒకవేళ (l == ఖాళీ)
	తిరిగి 0;

    CHECK_LIST_MATERIALIZE(l);

    ఒకవేళ (list_len(l) == 0)
	తిరిగి 0;

    li = list_కనుగొను(l, idx);
    ఒకవేళ (li == ఖాళీ)
    {
	semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), idx);
	తిరిగి 0;
    }

    కొరకు ( ; li != ఖాళీ; li = li->li_తదుపరి)
	ఒకవేళ (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    తిరిగి n;
}

/*
 * "count()" function
 */
    శూన్య
f_లెక్క(typval_T *argvars, typval_T *rettv)
{
    దీర్ఘ	n = 0;
    పూర్ణాంకం		ic = FALSE;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_సంఖ్య_arg(argvars, 3) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	ic = (పూర్ణాంకం)tv_get_bool_chk(&argvars[2], &error);

    ఒకవేళ (!error && argvars[0].v_type == VAR_STRING)
	n = string_లెక్క(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    లేకపోతే ఒకవేళ (!error && argvars[0].v_type == VAR_LIST)
    {
	దీర్ఘ idx = 0;

	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[3], &error);
	ఒకవేళ (!error)
	    n = list_లెక్క(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    లేకపోతే ఒకవేళ (!error && argvars[0].v_type == VAR_TUPLE)
    {
	దీర్ఘ idx = 0;

	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[3], &error);
	ఒకవేళ (!error)
	    n = tuple_లెక్క(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    లేకపోతే ఒకవేళ (!error && argvars[0].v_type == VAR_DICT)
    {
	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_వాదం));
	లేకపోతే
	    n = dict_లెక్క(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    లేకపోతే ఒకవేళ (!error)
	semsg(_(e_వాదం_యొక్క_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_సంఖ్య = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    స్థిర శూన్య
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	అక్షరం		*func_name,
	char_u		*arg_errmsg,
	పూర్ణాంకం		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*అంశం;
    దీర్ఘ	before;
    పూర్ణాంకం		error = FALSE;

    l1 = argvars[0].vval.v_list;
    ఒకవేళ (l1 == ఖాళీ)
    {
	emsg(_(e_cannot_extend_null_list));
	తిరిగి;
    }
    ఒకవేళ (is_new || !విలువ_check_తాళం(l1->lv_తాళం, arg_errmsg, TRUE))
    {
	ఒకవేళ (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    ఒకవేళ (l1 == ఖాళీ)
		తిరిగి;
	}

	l2 = argvars[1].vval.v_list;
	ఒకవేళ (l2 == ఖాళీ)
	    వెళ్ళు theend;

	ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], &error);
	    ఒకవేళ (error)
		వెళ్ళు cleanup;		// type error; errmsg already given
	    ఒకవేళ (before == l1->lv_len)
		అంశం = ఖాళీ;
	    లేకపోతే
	    {
		అంశం = list_కనుగొను(l1, before);
		ఒకవేళ (అంశం == ఖాళీ)
		{
		    semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), before);
		    వెళ్ళు cleanup;
		}
	    }
	}
	లేకపోతే
	    అంశం = ఖాళీ;
	ఒకవేళ (type != ఖాళీ && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    వెళ్ళు cleanup;
	list_extend(l1, l2, అంశం);

theend:
	ఒకవేళ (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_తాళం = FALSE;
	}
	లేకపోతే
	    copy_tv(&argvars[0], rettv);
	తిరిగి;

cleanup:
	ఒకవేళ (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    స్థిర శూన్య
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, పూర్ణాంకం is_new)
{
    type_T	*type = ఖాళీ;
    అక్షరం	*func_name = is_new ? "extendnew()" : "extend()";

    ఒకవేళ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	ఒకవేళ (!is_new && లోపల_vim9script() && argvars[0].vval.v_list != ఖాళీ)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	ఒకవేళ (!is_new && లోపల_vim9script() && argvars[0].vval.v_dict != ఖాళీ)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    లేకపోతే
	semsg(_(e_వాదం_యొక్క_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    శూన్య
f_extend(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extend() argument");

    extend(argvars, rettv, errmsg, FALSE);
}

/*
 * "extendnew(list, list [, idx])" function
 * "extendnew(dict, dict [, action])" function
 * "extendnew(blob, blob [, idx])" function
 */
    శూన్య
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    స్థిర శూన్య
list_చొప్పించు_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    దీర్ఘ	before = 0;
    listitem_T	*అంశం;
    పూర్ణాంకం		error = FALSE;

    ఒకవేళ (l == ఖాళీ)
    {
	ఒకవేళ (లోపల_vim9script())
	    emsg(_(e_cannot_కూడు_గమ్యం_null_list));
	తిరిగి;
    }

    ఒకవేళ (విలువ_check_తాళం(l->lv_తాళం, (char_u *)N_("insert() argument"), TRUE))
	తిరిగి;

    ఒకవేళ (argvars[2].v_type != VAR_UNKNOWN)
	before = (దీర్ఘ)tv_get_సంఖ్య_chk(&argvars[2], &error);
    ఒకవేళ (error)
	తిరిగి;		// type error; errmsg already given

    ఒకవేళ (before == l->lv_len)
	అంశం = ఖాళీ;
    లేకపోతే
    {
	అంశం = list_కనుగొను(l, before);
	ఒకవేళ (అంశం == ఖాళీ)
	{
	    semsg(_(e_list_సూచీ_బహిర్గతం_యొక్క_range_nr), before);
	    l = ఖాళీ;
	}
    }
    ఒకవేళ (l != ఖాళీ)
    {
	(శూన్య)list_చొప్పించు_tv(l, &argvars[1], అంశం);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    శూన్య
f_చొప్పించు(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_సంఖ్య_arg(argvars, 1) == FAIL)
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_BLOB)
	blob_చొప్పించు_func(argvars, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type != VAR_LIST)
	semsg(_(e_వాదం_యొక్క_str_must_be_list_or_blob), "insert()");
    లేకపోతే
	list_చొప్పించు_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    శూన్య
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    ఒకవేళ (లోపల_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_సంఖ్య_arg(argvars, 1) == FAIL
			|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_సంఖ్య_arg(argvars, 1) == FAIL)))
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    లేకపోతే
	semsg(_(e_వాదం_యొక్క_str_must_be_list_dictionary_or_blob), "remove()");
}

    స్థిర శూన్య
list_తిరగేయి(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_సమితి(rettv, l);
    ఒకవేళ (l != ఖాళీ
	    && !విలువ_check_తాళం(l->lv_తాళం,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	ఒకవేళ (l->lv_మొదటి == &range_list_అంశం)
	{
	    varnumber_T new_ప్రారంభం = l->lv_u.nonmat.lv_ప్రారంభం
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_చివర = new_ప్రారంభం
		- (l->lv_u.nonmat.lv_చివర - l->lv_u.nonmat.lv_ప్రారంభం);
	    l->lv_u.nonmat.lv_ప్రారంభం = new_ప్రారంభం;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    తిరిగి;
	}
	li = l->lv_u.mat.lv_చివరి;
	l->lv_మొదటి = l->lv_u.mat.lv_చివరి = ఖాళీ;
	l->lv_len = 0;
	అయితే (li != ఖాళీ)
	{
	    ni = li->li_prev;
	    list_append(l, li);
	    li = ni;
	}
	l->lv_u.mat.lv_idx = l->lv_len - l->lv_u.mat.lv_idx - 1;
    }
}

/*
 * "reverse({list})" function
 */
    శూన్య
f_తిరగేయి(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[0].v_type == VAR_BLOB)
	blob_తిరగేయి(argvars[0].vval.v_blob, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	ఒకవేళ (argvars[0].vval.v_string != ఖాళీ)
	    rettv->vval.v_string = తిరగేయి_పాఠ్యం(argvars[0].vval.v_string);
	లేకపోతే
	    rettv->vval.v_string = ఖాళీ;
    }
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_LIST)
	list_తిరగేయి(argvars[0].vval.v_list, rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_TUPLE)
	tuple_తిరగేయి(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    స్థిర శూన్య
list_reduce(
	typval_T	*argvars,
	typval_T	*సమాసం,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = ఖాళీ;
    పూర్ణాంకం		range_list;
    పూర్ణాంకం		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    పూర్ణాంకం		r;
    పూర్ణాంకం		called_emsg_ప్రారంభం = called_emsg;
    పూర్ణాంకం		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != ఖాళీ && l->lv_మొదటి == &range_list_అంశం;
    ఒకవేళ (range_list)
	range_val = l->lv_u.nonmat.lv_ప్రారంభం;

    ఒకవేళ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ఒకవేళ (l == ఖాళీ || l->lv_len == 0)
	{
	    semsg(_(e_reduce_యొక్క_an_ఖాళీలు_str_with_no_initial_విలువ), "List");
	    తిరిగి;
	}
	ఒకవేళ (range_list)
	{
	    initial.v_type = VAR_సంఖ్య;
	    initial.vval.v_సంఖ్య = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	లేకపోతే
	{
	    initial = l->lv_మొదటి->li_tv;
	    li = l->lv_మొదటి->li_తదుపరి;
	}
    }
    లేకపోతే
    {
	initial = argvars[2];
	ఒకవేళ (l != ఖాళీ && !range_list)
	    li = l->lv_మొదటి;
    }
    copy_tv(&initial, rettv);

    ఒకవేళ (l == ఖాళీ)
	తిరిగి;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_సమాసం_get_funccal(సమాసం, rettv);

    prev_locked = l->lv_తాళం;
    l->lv_తాళం = VAR_FIXED;  // disallow the list changing here

    అయితే (range_list ? range_idx < l->lv_len : li != ఖాళీ)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	ఒకవేళ (range_list)
	{
	    argv[1].v_type = VAR_సంఖ్య;
	    argv[1].vval.v_సంఖ్య = range_val;
	}
	లేకపోతే
	    argv[1] = li->li_tv;

	r = eval_సమాసం_typval(సమాసం, TRUE, argv, 2, fc, rettv);

	ఒకవేళ (argv[0].v_type != VAR_సంఖ్య && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	ఒకవేళ (r == FAIL || called_emsg != called_emsg_ప్రారంభం)
	    ఆపు;

	// advance to the next item
	ఒకవేళ (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	లేకపోతే
	    li = li->li_తదుపరి;
    }

    ఒకవేళ (fc != ఖాళీ)
	remove_funccal();

    l->lv_తాళం = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    శూన్య
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    ఒకవేళ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	తిరిగి;

    ఒకవేళ (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    లేకపోతే ఒకవేళ (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    లేకపోతే
	func_name = tv_get_string_strict(&argvars[1]);
    ఒకవేళ (func_name == ఖాళీ || *func_name == NUL)
    {
	emsg(_(e_missing_function_వాదం));
	తిరిగి;
    }

    ఒకవేళ (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    లేకపోతే ఒకవేళ (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    లేకపోతే
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    శూన్య
f_ముక్క(typval_T *argvars, typval_T *rettv)
{
    ఒకవేళ (లోపల_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_సంఖ్య_arg(argvars, 1) == FAIL
		|| check_for_opt_సంఖ్య_arg(argvars, 2) == FAIL))
	తిరిగి;

    ఒకవేళ (check_can_సూచీ(&argvars[0], TRUE, FALSE) != OK)
	తిరిగి;

    copy_tv(argvars, rettv);
    eval_సూచీ_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? ఖాళీ : argvars + 2,
	    TRUE, ఖాళీ, 0, FALSE);
}

#అయితే_ముగింపు // defined(FEAT_EVAL)
