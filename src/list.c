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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_EVAL)

// List heads for garbage collection.
ਸਥਿਰ list_T		*ਪਹਿਲਾ_list = ਨਿਰਾ;	// list of all lists

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FOR_ALL_WATCHERS(l, lw) \
    ਲਈ ((lw) = (l)->lv_watch; (lw) != ਨਿਰਾ; (lw) = (lw)->lw_ਅਗਲਾ)

ਸਥਿਰ ਖਾਲੀ list_free_ਵਸਤੂ(list_T *l, listitem_T *ਵਸਤੂ);

/*
 * Add a watcher to a list.
 */
    ਖਾਲੀ
list_ਜੋੜੋ_watch(list_T *l, listwatch_T *lw)
{
    lw->lw_ਅਗਲਾ = l->lv_watch;
    l->lv_watch = lw;
}

/*
 * Remove a watcher from a list.
 * No warning when it isn't found...
 */
    ਖਾਲੀ
list_rem_watch(list_T *l, listwatch_T *lwrem)
{
    listwatch_T	*lw, **lwp;

    lwp = &l->lv_watch;
    FOR_ALL_WATCHERS(l, lw)
    {
	ਜੇ (lw == lwrem)
	{
	    *lwp = lw->lw_ਅਗਲਾ;
	    ਰੋਕੋ;
	}
	lwp = &lw->lw_ਅਗਲਾ;
    }
}

/*
 * Just before removing an item from a list: advance watchers to the next
 * item.
 */
    ਸਥਿਰ ਖਾਲੀ
list_fix_watch(list_T *l, listitem_T *ਵਸਤੂ)
{
    listwatch_T	*lw;

    FOR_ALL_WATCHERS(l, lw)
	ਜੇ (lw->lw_ਵਸਤੂ == ਵਸਤੂ)
	    lw->lw_ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ;
}

    ਸਥਿਰ ਖਾਲੀ
list_init(list_T *l)
{
    // Prepend the list to the list of lists for garbage collection.
    ਜੇ (ਪਹਿਲਾ_list != ਨਿਰਾ)
	ਪਹਿਲਾ_list->lv_ਵਰਤਿਆ_prev = l;
    l->lv_ਵਰਤਿਆ_prev = ਨਿਰਾ;
    l->lv_ਵਰਤਿਆ_ਅਗਲਾ = ਪਹਿਲਾ_list;
    ਪਹਿਲਾ_list = l;
}

/*
 * Allocate an empty header for a list.
 * Caller should take care of the reference count.
 */
    list_T *
list_alloc(ਖਾਲੀ)
{
    list_T  *l;

    l = ALLOC_CLEAR_ONE(list_T);
    ਜੇ (l != ਨਿਰਾ)
	list_init(l);
    ਵਾਪਸ l;
}

/*
 * list_alloc() with an ID for alloc_fail().
 */
    list_T *
list_alloc_id(alloc_id_T id ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (alloc_fail_id == id && alloc_does_fail(ਆਕਾਰ(list_T)))
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ (list_alloc());
}

/*
 * Allocate space for a list, plus "count" items.
 * This uses one allocation for efficiency.
 * The reference count is not set.
 * Next list_set_item() must be called for each item.
 */
    list_T *
list_alloc_with_ਚੀਜ਼ਾਂ(ਪੂਰਨ ਗਿਣਤੀ_2)
{
    list_T	*l;

    ਜੇ (ਗਿਣਤੀ_2 > 0
	    && (size_t)ਗਿਣਤੀ_2 > (ਮਾਪ_MAX - ਆਕਾਰ(list_T)) / ਆਕਾਰ(listitem_T))
	ਵਾਪਸ ਨਿਰਾ;
    l = (list_T *)alloc_clear(ਆਕਾਰ(list_T) + ਗਿਣਤੀ_2 * ਆਕਾਰ(listitem_T));
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    list_init(l);

    ਜੇ (ਗਿਣਤੀ_2 <= 0)
	ਵਾਪਸ l;

    listitem_T	*li = (listitem_T *)(l + 1);
    ਪੂਰਨ		i;

    l->lv_len = ਗਿਣਤੀ_2;
    l->lv_with_ਚੀਜ਼ਾਂ = ਗਿਣਤੀ_2;
    l->lv_ਪਹਿਲਾ = li;
    l->lv_u.mat.lv_ਆਖਰੀ = li + ਗਿਣਤੀ_2 - 1;
    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
    {
	ਜੇ (i == 0)
	    li->li_prev = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    li->li_prev = li - 1;
	ਜੇ (i == ਗਿਣਤੀ_2 - 1)
	    li->li_ਅਗਲਾ = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    li->li_ਅਗਲਾ = li + 1;
	++li;
    }

    ਵਾਪਸ l;
}

/*
 * Set item "idx" for a list previously allocated with list_alloc_with_items().
 * The contents of "tv" is moved into the list item.
 * Each item must be set exactly once.
 */
    ਖਾਲੀ
list_ਸੈੱਟ_ਵਸਤੂ(list_T *l, ਪੂਰਨ idx, typval_T *tv)
{
    listitem_T	*li = (listitem_T *)(l + 1) + idx;

    li->li_tv = *tv;
}

/*
 * Allocate an empty list for a return value, with reference count set.
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
rettv_list_alloc(typval_T *rettv)
{
    list_T	*l = list_alloc();

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    rettv->v_ਤਾਲਾ = 0;
    rettv_list_ਸੈੱਟ(rettv, l);
    ਵਾਪਸ OK;
}

/*
 * Same as rettv_list_alloc() but uses an allocation id for testing.
 */
    ਪੂਰਨ
rettv_list_alloc_id(typval_T *rettv, alloc_id_T id ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ਜੇ (alloc_fail_id == id && alloc_does_fail(ਆਕਾਰ(list_T)))
	ਵਾਪਸ FAIL;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ rettv_list_alloc(rettv);
}


/*
 * Set a list as the return value.  Increments the reference count.
 */
    ਖਾਲੀ
rettv_list_ਸੈੱਟ(typval_T *rettv, list_T *l)
{
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ਜੇ (l != ਨਿਰਾ)
	++l->lv_refcount;
}

/*
 * Unreference a list: decrement the reference count and free it when it
 * becomes zero.
 */
    ਖਾਲੀ
list_unref(list_T *l)
{
    ਜੇ (l != ਨਿਰਾ && --l->lv_refcount <= 0)
	list_free(l);
}

/*
 * Free a list, including all non-container items it points to.
 * Ignores the reference count.
 */
    ਸਥਿਰ ਖਾਲੀ
list_free_contents(list_T *l)
{
    listitem_T *ਵਸਤੂ;

    ਜੇ (l->lv_ਪਹਿਲਾ != &range_list_ਵਸਤੂ)
	ਲਈ (ਵਸਤੂ = l->lv_ਪਹਿਲਾ; ਵਸਤੂ != ਨਿਰਾ; ਵਸਤੂ = l->lv_ਪਹਿਲਾ)
	{
	    // Remove the item before deleting it.
	    l->lv_ਪਹਿਲਾ = ਵਸਤੂ->li_ਅਗਲਾ;
	    clear_tv(&ਵਸਤੂ->li_tv);
	    list_free_ਵਸਤੂ(l, ਵਸਤੂ);
	}
}

/*
 * Go through the list of lists and free items without the copyID.
 * But don't free a list that has a watcher (used in a for loop), these
 * are not referenced anywhere.
 */
    ਪੂਰਨ
list_free_nonref(ਪੂਰਨ copyID)
{
    list_T	*ll;
    ਪੂਰਨ		did_free = FALSE;

    ਲਈ (ll = ਪਹਿਲਾ_list; ll != ਨਿਰਾ; ll = ll->lv_ਵਰਤਿਆ_ਅਗਲਾ)
	ਜੇ ((ll->lv_copyID & COPYID_ਨਕਾਬ) != (copyID & COPYID_ਨਕਾਬ)
						      && ll->lv_watch == ਨਿਰਾ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_contents(ll);
	    did_free = TRUE;
	}
    ਵਾਪਸ did_free;
}

    ਸਥਿਰ ਖਾਲੀ
list_free_list(list_T  *l)
{
    // Remove the list from the list of lists for garbage collection.
    ਜੇ (l->lv_ਵਰਤਿਆ_prev == ਨਿਰਾ)
	ਪਹਿਲਾ_list = l->lv_ਵਰਤਿਆ_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	l->lv_ਵਰਤਿਆ_prev->lv_ਵਰਤਿਆ_ਅਗਲਾ = l->lv_ਵਰਤਿਆ_ਅਗਲਾ;
    ਜੇ (l->lv_ਵਰਤਿਆ_ਅਗਲਾ != ਨਿਰਾ)
	l->lv_ਵਰਤਿਆ_ਅਗਲਾ->lv_ਵਰਤਿਆ_prev = l->lv_ਵਰਤਿਆ_prev;

    free_type(l->lv_type);
    vim_free(l);
}

    ਖਾਲੀ
list_free_ਚੀਜ਼ਾਂ(ਪੂਰਨ copyID)
{
    list_T	*ll, *ll_ਅਗਲਾ;

    ਲਈ (ll = ਪਹਿਲਾ_list; ll != ਨਿਰਾ; ll = ll_ਅਗਲਾ)
    {
	ll_ਅਗਲਾ = ll->lv_ਵਰਤਿਆ_ਅਗਲਾ;
	ਜੇ ((ll->lv_copyID & COPYID_ਨਕਾਬ) != (copyID & COPYID_ਨਕਾਬ)
						      && ll->lv_watch == ਨਿਰਾ)
	{
	    // Free the List and ordinary items it contains, but don't recurse
	    // into Lists and Dictionaries, they will be in the list of dicts
	    // or list of lists.
	    list_free_list(ll);
	}
    }
}

    ਖਾਲੀ
list_free(list_T *l)
{
    ਜੇ (ਵਿੱਚ_free_unref_ਚੀਜ਼ਾਂ)
	ਵਾਪਸ;

    list_free_contents(l);
    list_free_list(l);
}

/*
 * Allocate a list item.
 * It is not initialized, don't forget to set v_lock.
 */
    listitem_T *
listitem_alloc(ਖਾਲੀ)
{
    ਵਾਪਸ ALLOC_ONE(listitem_T);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Does not clear the value.  Does not notify watchers.
 */
    ਸਥਿਰ ਖਾਲੀ
list_free_ਵਸਤੂ(list_T *l, listitem_T *ਵਸਤੂ)
{
    ਜੇ (l->lv_with_ਚੀਜ਼ਾਂ == 0 || ਵਸਤੂ < (listitem_T *)l
			   || ਵਸਤੂ >= (listitem_T *)(l + 1) + l->lv_with_ਚੀਜ਼ਾਂ)
	vim_free(ਵਸਤੂ);
}

/*
 * Free a list item, unless it was allocated together with the list itself.
 * Also clears the value.  Does not notify watchers.
 */
    ਖਾਲੀ
listitem_free(list_T *l, listitem_T *ਵਸਤੂ)
{
    clear_tv(&ਵਸਤੂ->li_tv);
    list_free_ਵਸਤੂ(l, ਵਸਤੂ);
}

/*
 * Remove a list item from a List and free it.  Also clears the value.
 */
    ਖਾਲੀ
listitem_remove(list_T *l, listitem_T *ਵਸਤੂ)
{
    vimlist_remove(l, ਵਸਤੂ, ਵਸਤੂ);
    listitem_free(l, ਵਸਤੂ);
}

/*
 * Get the number of items in a list.
 */
    ਲੰਮਾ
list_len(list_T *l)
{
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ 0L;
    ਵਾਪਸ l->lv_len;
}

/*
 * Return TRUE when two lists have exactly the same values.
 */
    ਪੂਰਨ
list_equal(
    list_T	*l1,
    list_T	*l2,
    ਪੂਰਨ		ic)	// ignore case for strings
{
    listitem_T	*item1, *item2;

    ਜੇ (l1 == l2)
	ਵਾਪਸ TRUE;
    ਜੇ (list_len(l1) != list_len(l2))
	ਵਾਪਸ FALSE;
    ਜੇ (list_len(l1) == 0)
	// empty and NULL list are considered equal
	ਵਾਪਸ TRUE;
    ਜੇ (l1 == ਨਿਰਾ || l2 == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    ਲਈ (item1 = l1->lv_ਪਹਿਲਾ, item2 = l2->lv_ਪਹਿਲਾ;
	    item1 != ਨਿਰਾ && item2 != ਨਿਰਾ;
			       item1 = item1->li_ਅਗਲਾ, item2 = item2->li_ਅਗਲਾ)
	ਜੇ (!tv_equal(&item1->li_tv, &item2->li_tv, ic))
	    ਵਾਪਸ FALSE;
    ਵਾਪਸ item1 == ਨਿਰਾ && item2 == ਨਿਰਾ;
}

/*
 * Locate item with index "n" in list "l" and return it.
 * A negative index is counted from the end; -1 is the last item.
 * Returns NULL when "n" is out of range.
 */
    listitem_T *
list_ਲੱਭੋ(list_T *l, ਲੰਮਾ n)
{
    listitem_T	*ਵਸਤੂ;
    ਲੰਮਾ	idx;

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // Negative index is relative to the end.
    ਜੇ (n < 0)
	n = l->lv_len + n;

    // Check for index out of range.
    ਜੇ (n < 0 || n >= l->lv_len)
	ਵਾਪਸ ਨਿਰਾ;

    CHECK_LIST_MATERIALIZE(l);

    // range_list_materialize may reset l->lv_len
    ਜੇ (n >= l->lv_len)
	ਵਾਪਸ ਨਿਰਾ;

    // When there is a cached index may start search from there.
    ਜੇ (l->lv_u.mat.lv_idx_ਵਸਤੂ != ਨਿਰਾ)
    {
	ਜੇ (n < l->lv_u.mat.lv_idx / 2)
	{
	    // closest to the start of the list
	    ਵਸਤੂ = l->lv_ਪਹਿਲਾ;
	    idx = 0;
	}
	ਨਹੀਂਤਾਂ ਜੇ (n > (l->lv_u.mat.lv_idx + l->lv_len) / 2)
	{
	    // closest to the end of the list
	    ਵਸਤੂ = l->lv_u.mat.lv_ਆਖਰੀ;
	    idx = l->lv_len - 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    // closest to the cached index
	    ਵਸਤੂ = l->lv_u.mat.lv_idx_ਵਸਤੂ;
	    idx = l->lv_u.mat.lv_idx;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (n < l->lv_len / 2)
	{
	    // closest to the start of the list
	    ਵਸਤੂ = l->lv_ਪਹਿਲਾ;
	    idx = 0;
	}
	ਨਹੀਂਤਾਂ
	{
	    // closest to the end of the list
	    ਵਸਤੂ = l->lv_u.mat.lv_ਆਖਰੀ;
	    idx = l->lv_len - 1;
	}
    }

    ਜਦੋਂ (n > idx)
    {
	// search forward
	ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ;
	++idx;
    }
    ਜਦੋਂ (n < idx)
    {
	// search backward
	ਵਸਤੂ = ਵਸਤੂ->li_prev;
	--idx;
    }

    // cache the used index
    l->lv_u.mat.lv_idx = idx;
    l->lv_u.mat.lv_idx_ਵਸਤੂ = ਵਸਤੂ;

    ਵਾਪਸ ਵਸਤੂ;
}

/*
 * Get list item "l[idx]" as a number.
 */
    ਲੰਮਾ
list_ਲੱਭੋ_nr(
    list_T	*l,
    ਲੰਮਾ	idx,
    ਪੂਰਨ		*errorp)	// set to TRUE when something wrong
{
    listitem_T	*li;

    ਜੇ (l != ਨਿਰਾ && l->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ)
    {
	ਲੰਮਾ	    n = idx;

	// not materialized range() list: compute the value.
	// Negative index is relative to the end.
	ਜੇ (n < 0)
	    n = l->lv_len + n;

	// Check for index out of range.
	ਜੇ (n < 0 || n >= l->lv_len)
	{
	    ਜੇ (errorp != ਨਿਰਾ)
		*errorp = TRUE;
	    ਵਾਪਸ -1L;
	}

	ਵਾਪਸ l->lv_u.nonmat.lv_ਸ਼ੁਰੂ + n * l->lv_u.nonmat.lv_stride;
    }

    li = list_ਲੱਭੋ(l, idx);
    ਜੇ (li == ਨਿਰਾ)
    {
	ਜੇ (errorp != ਨਿਰਾ)
	    *errorp = TRUE;
	ਵਾਪਸ -1L;
    }
    ਵਾਪਸ (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&li->li_tv, errorp);
}

/*
 * Get list item "l[idx - 1]" as a string.  Returns NULL for failure.
 */
    char_u *
list_ਲੱਭੋ_str(list_T *l, ਲੰਮਾ idx)
{
    listitem_T	*li;

    li = list_ਲੱਭੋ(l, idx - 1);
    ਜੇ (li == ਨਿਰਾ)
    {
	semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), idx);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ tv_get_string(&li->li_tv);
}

/*
 * Like list_find() but when a negative index is used that is not found use
 * zero and set "idx" to zero.  Used for first index of a range.
 */
    listitem_T *
list_ਲੱਭੋ_ਸੂਚਕਾਂਕ(list_T *l, ਲੰਮਾ *idx)
{
    listitem_T *li = list_ਲੱਭੋ(l, *idx);

    ਜੇ (li != ਨਿਰਾ)
	ਵਾਪਸ li;

    ਜੇ (*idx < 0)
    {
	*idx = 0;
	li = list_ਲੱਭੋ(l, *idx);
    }
    ਵਾਪਸ li;
}

/*
 * Locate "item" list "l" and return its index.
 * Returns -1 when "item" is not in the list.
 */
    ਲੰਮਾ
list_idx_ਦਾ_ਵਸਤੂ(list_T *l, listitem_T *ਵਸਤੂ)
{
    ਲੰਮਾ	idx = 0;
    listitem_T	*li;

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ -1;
    CHECK_LIST_MATERIALIZE(l);
    idx = 0;
    ਲਈ (li = l->lv_ਪਹਿਲਾ; li != ਨਿਰਾ && li != ਵਸਤੂ; li = li->li_ਅਗਲਾ)
	++idx;
    ਜੇ (li == ਨਿਰਾ)
	ਵਾਪਸ -1;
    ਵਾਪਸ idx;
}

/*
 * Append item "item" to the end of list "l".
 */
    ਖਾਲੀ
list_append(list_T *l, listitem_T *ਵਸਤੂ)
{
    CHECK_LIST_MATERIALIZE(l);
    ਜੇ (l->lv_u.mat.lv_ਆਖਰੀ == ਨਿਰਾ)
    {
	// empty list
	l->lv_ਪਹਿਲਾ = ਵਸਤੂ;
	ਵਸਤੂ->li_prev = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	l->lv_u.mat.lv_ਆਖਰੀ->li_ਅਗਲਾ = ਵਸਤੂ;
	ਵਸਤੂ->li_prev = l->lv_u.mat.lv_ਆਖਰੀ;
    }
    l->lv_u.mat.lv_ਆਖਰੀ = ਵਸਤੂ;
    ++l->lv_len;
    ਵਸਤੂ->li_ਅਗਲਾ = ਨਿਰਾ;
}

/*
 * Append typval_T "tv" to the end of list "l".  "tv" is copied.
 * Return FAIL when out of memory or the type is wrong.
 */
    ਪੂਰਨ
list_append_tv(list_T *l, typval_T *tv)
{
    listitem_T	*li;

    ਜੇ (l->lv_type != ਨਿਰਾ && l->lv_type->tt_member != ਨਿਰਾ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ਨਿਰਾ, 0) == FAIL)
	ਵਾਪਸ FAIL;
    li = listitem_alloc();
    ਜੇ (li == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    copy_tv(tv, &li->li_tv);
    list_append(l, li);
    ਵਾਪਸ OK;
}

/*
 * As list_append_tv() but move the value instead of copying it.
 * Return FAIL when out of memory.
 */
    ਸਥਿਰ ਪੂਰਨ
list_append_tv_move(list_T *l, typval_T *tv)
{
    listitem_T	*li = listitem_alloc();

    ਜੇ (li == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    li->li_tv = *tv;
    list_append(l, li);
    ਵਾਪਸ OK;
}

/*
 * Add a dictionary to a list.  Used by getqflist().
 * Return FAIL when out of memory.
 */
    ਪੂਰਨ
list_append_dict(list_T *list, dict_T *dict)
{
    listitem_T	*li = listitem_alloc();

    ਜੇ (li == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    li->li_tv.v_type = VAR_DICT;
    li->li_tv.v_ਤਾਲਾ = 0;
    li->li_tv.vval.v_dict = dict;
    list_append(list, li);
    ++dict->dv_refcount;
    ਵਾਪਸ OK;
}

/*
 * Append list2 to list1.
 * Return FAIL when out of memory.
 */
    ਪੂਰਨ
list_append_list(list_T *list1, list_T *list2)
{
    listitem_T	*li = listitem_alloc();

    ਜੇ (li == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    li->li_tv.v_type = VAR_LIST;
    li->li_tv.v_ਤਾਲਾ = 0;
    li->li_tv.vval.v_list = list2;
    list_append(list1, li);
    ++list2->lv_refcount;
    ਵਾਪਸ OK;
}

/*
 * Make a copy of "str" and append it as an item to list "l".
 * When "len" >= 0 use "str[len]".
 * Returns FAIL when out of memory.
 */
    ਪੂਰਨ
list_append_string(list_T *l, char_u *str, ਪੂਰਨ len)
{
    listitem_T *li = listitem_alloc();

    ਜੇ (li == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    list_append(l, li);
    li->li_tv.v_type = VAR_STRING;
    li->li_tv.v_ਤਾਲਾ = 0;
    ਜੇ (str == ਨਿਰਾ)
	li->li_tv.vval.v_string = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ ਜੇ ((li->li_tv.vval.v_string = (len >= 0 ? vim_strnsave(str, len)
						 : vim_strsave(str))) == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Append "n" to list "l".
 * Returns FAIL when out of memory.
 */
    ਪੂਰਨ
list_append_ਸੰਖਿਆ(list_T *l, varnumber_T n)
{
    listitem_T	*li;

    li = listitem_alloc();
    ਜੇ (li == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    li->li_tv.v_type = VAR_ਸੰਖਿਆ;
    li->li_tv.v_ਤਾਲਾ = 0;
    li->li_tv.vval.v_ਸੰਖਿਆ = n;
    list_append(l, li);
    ਵਾਪਸ OK;
}

/*
 * Insert typval_T "tv" in list "l" before "item".
 * If "item" is NULL append at the end.
 * Return FAIL when out of memory or the type is wrong.
 */
    ਪੂਰਨ
list_ਪਾਓ_tv(list_T *l, typval_T *tv, listitem_T *ਵਸਤੂ)
{
    listitem_T	*ni;

    ਜੇ (l->lv_type != ਨਿਰਾ && l->lv_type->tt_member != ਨਿਰਾ
		&& check_typval_arg_type(l->lv_type->tt_member, tv,
							      ਨਿਰਾ, 0) == FAIL)
	ਵਾਪਸ FAIL;
    ni = listitem_alloc();
    ਜੇ (ni == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    copy_tv(tv, &ni->li_tv);
    list_ਪਾਓ(l, ni, ਵਸਤੂ);
    ਵਾਪਸ OK;
}

    ਖਾਲੀ
list_ਪਾਓ(list_T *l, listitem_T *ni, listitem_T *ਵਸਤੂ)
{
    CHECK_LIST_MATERIALIZE(l);
    ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	// Append new item at end of list.
	list_append(l, ni);
    ਨਹੀਂਤਾਂ
    {
	// Insert new item before existing item.
	ni->li_prev = ਵਸਤੂ->li_prev;
	ni->li_ਅਗਲਾ = ਵਸਤੂ;
	ਜੇ (ਵਸਤੂ->li_prev == ਨਿਰਾ)
	{
	    l->lv_ਪਹਿਲਾ = ni;
	    ++l->lv_u.mat.lv_idx;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਵਸਤੂ->li_prev->li_ਅਗਲਾ = ni;
	    l->lv_u.mat.lv_idx_ਵਸਤੂ = ਨਿਰਾ;
	}
	ਵਸਤੂ->li_prev = ni;
	++l->lv_len;
    }
}

/*
 * Append a new empty item into "l" based on the list item type.  Used when
 * adding a new element to a List in Vim9script by using the current list
 * length as the index.
 */
    ਸਥਿਰ ਪੂਰਨ
list_append_new_ਵਸਤੂ(list_T *l)
{
    typval_T	tv;

    ਜੇ (l->lv_type != ਨਿਰਾ && l->lv_type->tt_member != ਨਿਰਾ)
	tv.v_type = l->lv_type->tt_member->tt_type;
    ਨਹੀਂਤਾਂ
	tv.v_type = VAR_ਸੰਖਿਆ;
    tv.v_ਤਾਲਾ = 0;

    ਚੋਣ (tv.v_type)
    {
	ਹਾਲਤ VAR_BOOL: tv.vval.v_ਸੰਖਿਆ = VVAL_FALSE; ਰੋਕੋ;
	ਹਾਲਤ VAR_SPECIAL: tv.vval.v_ਸੰਖਿਆ = 0; ਰੋਕੋ;
	ਹਾਲਤ VAR_ਸੰਖਿਆ: tv.vval.v_ਸੰਖਿਆ = 0; ਰੋਕੋ;
	ਹਾਲਤ VAR_FLOAT: tv.vval.v_float = 0; ਰੋਕੋ;
	ਹਾਲਤ VAR_STRING: tv.vval.v_string = ਨਿਰਾ; ਰੋਕੋ;
	ਹਾਲਤ VAR_BLOB: tv.vval.v_blob = blob_alloc(); ਰੋਕੋ;
	ਹਾਲਤ VAR_FUNC: tv.vval.v_string = ਨਿਰਾ; ਰੋਕੋ;
	ਹਾਲਤ VAR_PARTIAL: tv.vval.v_partial = ਨਿਰਾ; ਰੋਕੋ;
	ਹਾਲਤ VAR_LIST: tv.vval.v_list = list_alloc(); ਰੋਕੋ;
	ਹਾਲਤ VAR_DICT: tv.vval.v_dict = dict_alloc(); ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
	ਹਾਲਤ VAR_ਕੰਮ: tv.vval.v_ਕੰਮ = ਨਿਰਾ; ਰੋਕੋ;
	ਹਾਲਤ VAR_CHANNEL: tv.vval.v_channel = ਨਿਰਾ; ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਹਾਲਤ VAR_CLASS:
	    ਜੇ (l->lv_type != ਨਿਰਾ && l->lv_type->tt_member != ਨਿਰਾ)
		tv.vval.v_class = l->lv_type->tt_member->tt_class;
	    ਨਹੀਂਤਾਂ
		tv.vval.v_class = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_OBJECT:
	    ਜੇ (l->lv_type != ਨਿਰਾ && l->lv_type->tt_member != ਨਿਰਾ)
		tv.vval.v_object =
		    alloc_object(l->lv_type->tt_member->tt_class);
	    ਨਹੀਂਤਾਂ
		tv.vval.v_object = ਨਿਰਾ;
	    ਰੋਕੋ;
	ਹਾਲਤ VAR_TYPEALIAS: tv.vval.v_typealias = ਨਿਰਾ; ਰੋਕੋ;
	ਹਾਲਤ VAR_TUPLE: tv.vval.v_tuple = tuple_alloc(); ਰੋਕੋ;
	ਮੂਲ:
	    tv.v_type = VAR_ਸੰਖਿਆ;
	    tv.vval.v_ਸੰਖਿਆ = 0;
	    ਰੋਕੋ;
    }

    ਜੇ (l->lv_type != ਨਿਰਾ && l->lv_type->tt_member != ਨਿਰਾ)
	ਸੈੱਟ_tv_type(&tv, l->lv_type->tt_member);

    ਵਾਪਸ list_append_tv(l, &tv);
}

/*
 * Get the list item in "l" with index "n1".  "n1" is adjusted if needed.
 * In Vim9, it is at the end of the list, add an item if "can_append" is TRUE.
 * Return NULL if there is no such item.
 */
    listitem_T *
check_range_ਸੂਚਕਾਂਕ_one(list_T *l, ਲੰਮਾ *n1, ਪੂਰਨ can_append, ਪੂਰਨ quiet)
{
    ਲੰਮਾ	orig_n1 = *n1;
    listitem_T	*li = list_ਲੱਭੋ_ਸੂਚਕਾਂਕ(l, n1);

    ਜੇ (li != ਨਿਰਾ)
	ਵਾਪਸ li;

    // Vim9: Allow for adding an item at the end.
    ਜੇ (can_append && ਵਿੱਚ_vim9script()
	    && *n1 == l->lv_len && l->lv_ਤਾਲਾ == 0)
    {
	ਜੇ (list_append_new_ਵਸਤੂ(l) == FAIL)
	    ਵਾਪਸ ਨਿਰਾ;
	li = list_ਲੱਭੋ_ਸੂਚਕਾਂਕ(l, n1);
    }
    ਜੇ (li == ਨਿਰਾ)
    {
	ਜੇ (!quiet)
	    semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), orig_n1);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਵਾਪਸ li;
}

/*
 * Check that "n2" can be used as the second index in a range of list "l".
 * If "n1" or "n2" is negative it is changed to the positive index.
 * "li1" is the item for item "n1".
 * Return OK or FAIL.
 */
    ਪੂਰਨ
check_range_ਸੂਚਕਾਂਕ_two(
	list_T	    *l,
	ਲੰਮਾ	    *n1,
	listitem_T  *li1,
	ਲੰਮਾ	    *n2,
	ਪੂਰਨ	    quiet)
{
    ਜੇ (*n2 < 0)
    {
	listitem_T	*ni = list_ਲੱਭੋ(l, *n2);

	ਜੇ (ni == ਨਿਰਾ)
	{
	    ਜੇ (!quiet)
		semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), *n2);
	    ਵਾਪਸ FAIL;
	}
	*n2 = list_idx_ਦਾ_ਵਸਤੂ(l, ni);
    }

    // Check that n2 isn't before n1.
    ਜੇ (*n1 < 0)
	*n1 = list_idx_ਦਾ_ਵਸਤੂ(l, li1);
    ਜੇ (*n2 < *n1)
    {
	ਜੇ (!quiet)
	    semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), *n2);
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
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
    ਪੂਰਨ
list_ਸੌਂਪੋ_range(
	list_T	    *dest,
	list_T	    *src,
	ਲੰਮਾ	    idx1_arg,
	ਲੰਮਾ	    idx2,
	ਪੂਰਨ	    ਖਾਲੀ_ਥਾਵਾਂ_idx2,
	char_u	    *op,
	char_u	    *varname)
{
    listitem_T	*src_li;
    listitem_T	*dest_li;
    ਲੰਮਾ	idx1 = idx1_arg;
    listitem_T	*ਪਹਿਲਾ_li = list_ਲੱਭੋ_ਸੂਚਕਾਂਕ(dest, &idx1);
    ਲੰਮਾ	idx;
    type_T	*member_type = ਨਿਰਾ;

    // Check whether any of the list items is locked before making any changes.
    idx = idx1;
    dest_li = ਪਹਿਲਾ_li;
    ਲਈ (src_li = src->lv_ਪਹਿਲਾ; src_li != ਨਿਰਾ && dest_li != ਨਿਰਾ; )
    {
	ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(dest_li->li_tv.v_ਤਾਲਾ, varname, FALSE))
	    ਵਾਪਸ FAIL;
	src_li = src_li->li_ਅਗਲਾ;
	ਜੇ (src_li == ਨਿਰਾ || (!ਖਾਲੀ_ਥਾਵਾਂ_idx2 && idx2 == idx))
	    ਰੋਕੋ;
	dest_li = dest_li->li_ਅਗਲਾ;
	++idx;
    }

    ਜੇ (ਵਿੱਚ_vim9script() && dest->lv_type != ਨਿਰਾ
					   && dest->lv_type->tt_member != ਨਿਰਾ)
	member_type = dest->lv_type->tt_member;

    // Assign the List values to the list items.
    idx = idx1;
    dest_li = ਪਹਿਲਾ_li;
    ਲਈ (src_li = src->lv_ਪਹਿਲਾ; src_li != ਨਿਰਾ; )
    {
	ਜੇ (op != ਨਿਰਾ && *op != '=')
	    tv_op(&dest_li->li_tv, &src_li->li_tv, op);
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (member_type != ਨਿਰਾ
		    && check_typval_arg_type(member_type, &src_li->li_tv,
							      ਨਿਰਾ, 0) == FAIL)
		ਵਾਪਸ FAIL;
	    clear_tv(&dest_li->li_tv);
	    copy_tv(&src_li->li_tv, &dest_li->li_tv);
	}
	src_li = src_li->li_ਅਗਲਾ;
	ਜੇ (src_li == ਨਿਰਾ || (!ਖਾਲੀ_ਥਾਵਾਂ_idx2 && idx2 == idx))
	    ਰੋਕੋ;
	ਜੇ (dest_li->li_ਅਗਲਾ == ਨਿਰਾ)
	{
	    // Need to add an empty item.
	    ਜੇ (list_append_ਸੰਖਿਆ(dest, 0) == FAIL)
	    {
		src_li = ਨਿਰਾ;
		ਰੋਕੋ;
	    }
	}
	dest_li = dest_li->li_ਅਗਲਾ;
	++idx;
    }
    ਜੇ (src_li != ਨਿਰਾ)
    {
	emsg(_(e_list_ਮੁੱਲ_has_ਹੋਰ_ਚੀਜ਼ਾਂ_than_targets));
	ਵਾਪਸ FAIL;
    }
    ਜੇ (ਖਾਲੀ_ਥਾਵਾਂ_idx2
	    ? (dest_li != ਨਿਰਾ && dest_li->li_ਅਗਲਾ != ਨਿਰਾ)
	    : idx != idx2)
    {
	emsg(_(e_list_ਮੁੱਲ_does_not_have_enough_ਚੀਜ਼ਾਂ));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Flatten up to "maxitems" in "list", starting at "first" to depth "maxdepth".
 * When "first" is NULL use the first item.
 * It does nothing if "maxdepth" is 0.
 * Returns FAIL when out of memory.
 */
    ਸਥਿਰ ਖਾਲੀ
list_flatten(list_T *list, listitem_T *ਪਹਿਲਾ, ਲੰਮਾ maxitems, ਲੰਮਾ maxdepth)
{
    listitem_T	*ਵਸਤੂ;
    ਪੂਰਨ		ਪੂਰਾ = 0;

    ਜੇ (maxdepth == 0)
	ਵਾਪਸ;
    CHECK_LIST_MATERIALIZE(list);
    ਜੇ (ਪਹਿਲਾ == ਨਿਰਾ)
	ਵਸਤੂ = list->lv_ਪਹਿਲਾ;
    ਨਹੀਂਤਾਂ
	ਵਸਤੂ = ਪਹਿਲਾ;

    ਜਦੋਂ (ਵਸਤੂ != ਨਿਰਾ && ਪੂਰਾ < maxitems)
    {
	listitem_T	*ਅਗਲਾ = ਵਸਤੂ->li_ਅਗਲਾ;

	fast_breakcheck();
	ਜੇ (got_int)
	    ਵਾਪਸ;

	ਜੇ (ਵਸਤੂ->li_tv.v_type == VAR_LIST)
	{
	    list_T	*itemlist = ਵਸਤੂ->li_tv.vval.v_list;

	    vimlist_remove(list, ਵਸਤੂ, ਵਸਤੂ);
	    ਜੇ (list_extend(list, itemlist, ਅਗਲਾ) == FAIL)
	    {
		list_free_ਵਸਤੂ(list, ਵਸਤੂ);
		ਵਾਪਸ;
	    }

	    ਜੇ (maxdepth > 0)
		list_flatten(list, ਵਸਤੂ->li_prev == ਨਿਰਾ
				     ? list->lv_ਪਹਿਲਾ : ਵਸਤੂ->li_prev->li_ਅਗਲਾ,
				itemlist->lv_len, maxdepth - 1);
	    clear_tv(&ਵਸਤੂ->li_tv);
	    list_free_ਵਸਤੂ(list, ਵਸਤੂ);
	}

	++ਪੂਰਾ;
	ਵਸਤੂ = ਅਗਲਾ;
    }
}

/*
 * "flatten()" and "flattennew()" functions
 */
    ਸਥਿਰ ਖਾਲੀ
flatten_common(typval_T *argvars, typval_T *rettv, ਪੂਰਨ make_copy)
{
    list_T  *l;
    ਲੰਮਾ    maxdepth;
    ਪੂਰਨ	    error = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list), "flatten()");
	ਵਾਪਸ;
    }

    ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
	maxdepth = 999999;
    ਨਹੀਂਤਾਂ
    {
	maxdepth = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
	ਜੇ (error)
	    ਵਾਪਸ;
	ਜੇ (maxdepth < 0)
	{
	    emsg(_(e_maxdepth_must_be_non_negative_ਸੰਖਿਆ));
	    ਵਾਪਸ;
	}
    }

    l = argvars[0].vval.v_list;
    rettv->v_type = VAR_LIST;
    rettv->vval.v_list = l;
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (make_copy)
    {
	l = list_copy(l, FALSE, TRUE, get_copyID());
	rettv->vval.v_list = l;
	ਜੇ (l == ਨਿਰਾ)
	    ਵਾਪਸ;
	// The type will change.
	free_type(l->lv_type);
	l->lv_type = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(l->lv_ਤਾਲਾ,
				     (char_u *)N_("flatten() argument"), TRUE))
	    ਵਾਪਸ;
	++l->lv_refcount;
    }

    list_flatten(l, ਨਿਰਾ, l->lv_len, maxdepth);
}

/*
 * "flatten(list[, {maxdepth}])" function
 */
    ਖਾਲੀ
f_flatten(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script())
	emsg(_(e_cannot_use_flatten_ਵਿੱਚ_vim9_script));
    ਨਹੀਂਤਾਂ
	flatten_common(argvars, rettv, FALSE);
}

/*
 * "flattennew(list[, {maxdepth}])" function
 */
    ਖਾਲੀ
f_flattennew(typval_T *argvars, typval_T *rettv)
{
    flatten_common(argvars, rettv, TRUE);
}

/*
 * "items(list)" function
 * Caller must have already checked that argvars[0] is a List.
 */
    ਖਾਲੀ
list2items(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T	*li;
    varnumber_T	idx;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;  // null list behaves like an empty list

    // TODO: would be more efficient to not materialize the argument
    CHECK_LIST_MATERIALIZE(l);
    ਲਈ (idx = 0, li = l->lv_ਪਹਿਲਾ; li != ਨਿਰਾ; li = li->li_ਅਗਲਾ, ++idx)
    {
	list_T	    *l2 = list_alloc();

	ਜੇ (l2 == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ਰੋਕੋ;
	}
	ਜੇ (list_append_ਸੰਖਿਆ(l2, idx) == FAIL
		|| list_append_tv(l2, &li->li_tv) == FAIL)
	    ਰੋਕੋ;
    }
}

/*
 * "items(string)" function
 * Caller must have already checked that argvars[0] is a String.
 */
    ਖਾਲੀ
string2items(typval_T *argvars, typval_T *rettv)
{
    char_u	*p = argvars[0].vval.v_string;
    varnumber_T idx;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;
    ਜੇ (p == ਨਿਰਾ)
	ਵਾਪਸ;  // null string behaves like an empty string

    ਲਈ (idx = 0; *p != NUL; ++idx)
    {
	ਪੂਰਨ	    len = mb_ptr2len(p);
	list_T	    *l2;

	ਜੇ (len == 0)
	    ਰੋਕੋ;
	l2 = list_alloc();
	ਜੇ (l2 == ਨਿਰਾ)
	    ਰੋਕੋ;
	ਜੇ (list_append_list(rettv->vval.v_list, l2) == FAIL)
	{
	    list_free(l2);
	    ਰੋਕੋ;
	}
	ਜੇ (list_append_ਸੰਖਿਆ(l2, idx) == FAIL
		|| list_append_string(l2, p, len) == FAIL)
	    ਰੋਕੋ;
	p += len;
    }
}

/*
 * Extend "l1" with "l2".  "l1" must not be NULL.
 * If "bef" is NULL append at the end, otherwise insert before this item.
 * Returns FAIL when out of memory.
 */
    ਪੂਰਨ
list_extend(list_T *l1, list_T *l2, listitem_T *bef)
{
    listitem_T	*ਵਸਤੂ;
    ਪੂਰਨ		todo;
    listitem_T	*bef_prev;

    // NULL list is equivalent to an empty list: nothing to do.
    ਜੇ (l2 == ਨਿਰਾ || l2->lv_len == 0)
	ਵਾਪਸ OK;

    todo = l2->lv_len;
    CHECK_LIST_MATERIALIZE(l1);
    CHECK_LIST_MATERIALIZE(l2);

    // When exending a list with itself, at some point we run into the item
    // that was before "bef" and need to skip over the already inserted items
    // to "bef".
    bef_prev = bef == ਨਿਰਾ ? ਨਿਰਾ : bef->li_prev;

    // We also quit the loop when we have inserted the original item count of
    // the list, avoid a hang when we extend a list with itself.
    ਲਈ (ਵਸਤੂ = l2->lv_ਪਹਿਲਾ; ਵਸਤੂ != ਨਿਰਾ && --todo >= 0;
				 ਵਸਤੂ = ਵਸਤੂ == bef_prev ? bef : ਵਸਤੂ->li_ਅਗਲਾ)
	ਜੇ (list_ਪਾਓ_tv(l1, &ਵਸਤੂ->li_tv, bef) == FAIL)
	    ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}

/*
 * Concatenate lists "l1" and "l2" into a new list, stored in "tv".
 * Return FAIL when out of memory.
 */
    ਪੂਰਨ
list_concat(list_T *l1, list_T *l2, typval_T *tv)
{
    list_T	*l;
    ਪੂਰਨ		len1 = l1 == ਨਿਰਾ ? 0 : l1->lv_len;
    ਪੂਰਨ		len2 = l2 == ਨਿਰਾ ? 0 : l2->lv_len;
    ਲੰਮਾ	totallen = (ਲੰਮਾ)len1 + (ਲੰਮਾ)len2;
    ਪੂਰਨ		i;
    listitem_T	*ਵਸਤੂ;

    ਜੇ (totallen == 0)
    {
	l = list_alloc();
	ਜੇ (l == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	++l->lv_refcount;
	tv->v_type = VAR_LIST;
	tv->v_ਤਾਲਾ = 0;
	tv->vval.v_list = l;
	ਵਾਪਸ OK;
    }
    ਜੇ (totallen > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ)
	ਵਾਪਸ FAIL;

    // allocate all items at once for efficiency
    l = list_alloc_with_ਚੀਜ਼ਾਂ((ਪੂਰਨ)totallen);
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    i = 0;
    ਜੇ (len1 > 0)
    {
	CHECK_LIST_MATERIALIZE(l1);
	ਲਈ (ਵਸਤੂ = l1->lv_ਪਹਿਲਾ; ਵਸਤੂ != ਨਿਰਾ && !got_int;
		ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ)
	{
	    typval_T new_tv;

	    copy_tv(&ਵਸਤੂ->li_tv, &new_tv);
	    list_ਸੈੱਟ_ਵਸਤੂ(l, i++, &new_tv);
	}
    }
    ਜੇ (len2 > 0)
    {
	CHECK_LIST_MATERIALIZE(l2);
	ਲਈ (ਵਸਤੂ = l2->lv_ਪਹਿਲਾ; ਵਸਤੂ != ਨਿਰਾ && !got_int;
		ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ)
	{
	    typval_T new_tv;

	    copy_tv(&ਵਸਤੂ->li_tv, &new_tv);
	    list_ਸੈੱਟ_ਵਸਤੂ(l, i++, &new_tv);
	}
    }

    ++l->lv_refcount;
    tv->v_type = VAR_LIST;
    tv->v_ਤਾਲਾ = 0;
    tv->vval.v_list = l;
    ਵਾਪਸ OK;
}

    list_T *
list_ਟੁਕੜਾ(list_T *ol, ਲੰਮਾ n1, ਲੰਮਾ n2)
{
    listitem_T	*ਵਸਤੂ;
    list_T	*l;
    ਲੰਮਾ	ਗਿਣਤੀ_2;
    ਲੰਮਾ	idx;

    ਜੇ (n2 < n1)
	ਵਾਪਸ list_alloc();
    ਜੇ (n1 > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - (n2 - n1) - 1)
	ਵਾਪਸ ਨਿਰਾ;
    ਗਿਣਤੀ_2 = n2 - n1 + 1;

    // The length is known, so allocate the list and all items at once.
    l = list_alloc_with_ਚੀਜ਼ਾਂ((ਪੂਰਨ)ਗਿਣਤੀ_2);
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਵਸਤੂ = list_ਲੱਭੋ(ol, n1);
    ਲਈ (idx = 0; idx < ਗਿਣਤੀ_2; ++idx)
    {
	typval_T	new_tv;

	// "item" is NULL when materializing "ol" stopped early.
	ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	{
	    list_free(l);
	    ਵਾਪਸ ਨਿਰਾ;
	}

	copy_tv(&ਵਸਤੂ->li_tv, &new_tv);
	list_ਸੈੱਟ_ਵਸਤੂ(l, (ਪੂਰਨ)idx, &new_tv);
	ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ;
    }
    ਵਾਪਸ l;
}

    ਪੂਰਨ
list_ਟੁਕੜਾ_or_ਸੂਚਕਾਂਕ(
	    list_T	*list,
	    ਪੂਰਨ		range,
	    varnumber_T	n1_arg,
	    varnumber_T	n2_arg,
	    ਪੂਰਨ		exclusive,
	    typval_T	*rettv,
	    ਪੂਰਨ		verbose)
{
    ਲੰਮਾ	len = list_len(list);
    varnumber_T	n1 = n1_arg;
    varnumber_T	n2 = n2_arg;
    typval_T	var1;

    ਜੇ (n1 < 0)
	n1 = len + n1;
    ਜੇ (n1 < 0 || n1 >= len)
    {
	// For a range we allow invalid values and for legacy script return an
	// empty list, for Vim9 script start at the first item.
	// A list index out of range is an error.
	ਜੇ (!range)
	{
	    ਜੇ (verbose)
		semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), (ਲੰਮਾ)n1_arg);
	    ਵਾਪਸ FAIL;
	}
	ਜੇ (ਵਿੱਚ_vim9script())
	    n1 = n1 < 0 ? 0 : len;
	ਨਹੀਂਤਾਂ
	    n1 = len;
    }
    ਜੇ (range)
    {
	list_T	*l;

	ਜੇ (n2 < 0)
	    n2 = len + n2;
	ਨਹੀਂਤਾਂ ਜੇ (n2 >= len)
	    n2 = len - (exclusive ? 0 : 1);
	ਜੇ (exclusive)
	    --n2;
	ਜੇ (n2 < 0 || n2 + 1 < n1)
	    n2 = -1;
	l = list_ਟੁਕੜਾ(list, n1, n2);
	ਜੇ (l == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	clear_tv(rettv);
	rettv_list_ਸੈੱਟ(rettv, l);
    }
    ਨਹੀਂਤਾਂ
    {
	// copy the item to "var1" to avoid that freeing the list makes it
	// invalid.
	ਲੰਮਾ ਸੂਚਕਾਂਕ = n1;
	listitem_T *li = check_range_ਸੂਚਕਾਂਕ_one(list, &ਸੂਚਕਾਂਕ, TRUE, TRUE);
	ਜੇ (li == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
	copy_tv(&li->li_tv, &var1);

	// If "var1" is a List and the List item type is not set, then set it
	// from the declared list item type.
	ਜੇ (ਵਿੱਚ_vim9script() && var1.v_type == VAR_LIST
					&& var1.vval.v_list != ਨਿਰਾ
					&& var1.vval.v_list->lv_type == ਨਿਰਾ)
	{
	    ਜੇ (list->lv_type != ਨਿਰਾ && list->lv_type->tt_member != ਨਿਰਾ)
		ਸੈੱਟ_tv_type(&var1, list->lv_type->tt_member);
	}

	clear_tv(rettv);
	*rettv = var1;
    }
    ਵਾਪਸ OK;
}

/*
 * Make a copy of list "orig".  Shallow if "deep" is FALSE.
 * The refcount of the new list is set to 1.
 * See item_copy() for "top" and "copyID".
 * Returns NULL when out of memory.
 */
    list_T *
list_copy(list_T *orig, ਪੂਰਨ deep, ਪੂਰਨ ਸਿਖਰ, ਪੂਰਨ copyID)
{
    list_T	*copy;
    listitem_T	*ਵਸਤੂ;
    ਪੂਰਨ		idx = 0;

    ਜੇ (orig == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    // The length is known, so allocate the list and all items at once.
    CHECK_LIST_MATERIALIZE(orig);
    copy = list_alloc_with_ਚੀਜ਼ਾਂ(orig->lv_len);
    ਜੇ (copy == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (orig->lv_type == ਨਿਰਾ || ਸਿਖਰ || deep)
	copy->lv_type = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	copy->lv_type = alloc_type(orig->lv_type);
    ਜੇ (copyID != 0)
    {
	// Do this before adding the items, because one of the items may
	// refer back to this list.
	orig->lv_copyID = copyID;
	orig->lv_copylist = copy;
    }
    ਲਈ (ਵਸਤੂ = orig->lv_ਪਹਿਲਾ; ਵਸਤੂ != ਨਿਰਾ && !got_int;
	    ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ)
    {
	typval_T	new_tv;

	ਜੇ (deep)
	{
	    ਜੇ (ਵਸਤੂ_copy(&ਵਸਤੂ->li_tv, &new_tv, deep, FALSE, copyID) == FAIL)
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	    copy_tv(&ਵਸਤੂ->li_tv, &new_tv);
	list_ਸੈੱਟ_ਵਸਤੂ(copy, idx++, &new_tv);
    }
    ++copy->lv_refcount;
    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
    {
	list_unref(copy);
	copy = ਨਿਰਾ;
    }

    ਵਾਪਸ copy;
}

/*
 * Remove items "item" to "item2" from list "l".
 * Does not free the listitem or the value!
 * This used to be called list_remove, but that conflicts with a Sun header
 * file.
 */
    ਖਾਲੀ
vimlist_remove(list_T *l, listitem_T *ਵਸਤੂ, listitem_T *item2)
{
    listitem_T	*ip;

    CHECK_LIST_MATERIALIZE(l);

    // notify watchers
    ਲਈ (ip = ਵਸਤੂ; ip != ਨਿਰਾ; ip = ip->li_ਅਗਲਾ)
    {
	--l->lv_len;
	list_fix_watch(l, ip);
	ਜੇ (ip == item2)
	    ਰੋਕੋ;
    }

    ਜੇ (item2->li_ਅਗਲਾ == ਨਿਰਾ)
	l->lv_u.mat.lv_ਆਖਰੀ = ਵਸਤੂ->li_prev;
    ਨਹੀਂਤਾਂ
	item2->li_ਅਗਲਾ->li_prev = ਵਸਤੂ->li_prev;
    ਜੇ (ਵਸਤੂ->li_prev == ਨਿਰਾ)
	l->lv_ਪਹਿਲਾ = item2->li_ਅਗਲਾ;
    ਨਹੀਂਤਾਂ
	ਵਸਤੂ->li_prev->li_ਅਗਲਾ = item2->li_ਅਗਲਾ;
    l->lv_u.mat.lv_idx_ਵਸਤੂ = ਨਿਰਾ;
}

/*
 * Return an allocated string with the string representation of a list.
 * May return NULL.
 */
    char_u *
list2string(typval_T *tv, ਪੂਰਨ copyID, ਪੂਰਨ restore_copyID)
{
    garray_T	ga;

    ਜੇ (tv->vval.v_list == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 80);
    ga_append(&ga, '[');
    CHECK_LIST_MATERIALIZE(tv->vval.v_list);
    ਜੇ (list_join(&ga, tv->vval.v_list, (char_u *)", ",
				       FALSE, restore_copyID, copyID) == FAIL)
    {
	vim_free(ga.ga_ਅੰਕੜੇ);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ga_append(&ga, ']');
    ga_append(&ga, NUL);
    ਵਾਪਸ (char_u *)ga.ga_ਅੰਕੜੇ;
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ join_S {
    string_T	s;
    char_u	*tofree;
} join_T;

    ਸਥਿਰ ਪੂਰਨ
list_join_inner(
    garray_T	*gap,		// to store the result in
    list_T	*l,
    char_u	*sep,
    ਪੂਰਨ		echo_style,
    ਪੂਰਨ		restore_copyID,
    ਪੂਰਨ		copyID,
    garray_T	*join_gap)	// to keep each list item string
{
    ਪੂਰਨ		i;
    join_T	*p;
    ਲੰਮਾ	sumlen = 0;
    ਪੂਰਨ		ਪਹਿਲਾ = TRUE;
    ਪੂਰਨ		prev_ਤਾਲਾ;
    char_u	*tofree;
    char_u	numbuf[NUMBUFLEN];
    listitem_T	*ਵਸਤੂ;
    string_T	s;
    size_t	seplen;

    // Stringify each item in the list.
    CHECK_LIST_MATERIALIZE(l);
    // Lock the list, the string() method of an object item could remove the
    // item the loop is standing on.
    prev_ਤਾਲਾ = l->lv_ਤਾਲਾ;
    ਜੇ (l->lv_ਤਾਲਾ == 0)
	l->lv_ਤਾਲਾ = VAR_LOCKED;
    ਲਈ (ਵਸਤੂ = l->lv_ਪਹਿਲਾ; ਵਸਤੂ != ਨਿਰਾ && !got_int; ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ)
    {
	s.string = echo_string_core(&ਵਸਤੂ->li_tv, &tofree, numbuf, copyID,
				      echo_style, restore_copyID, !echo_style);
	ਜੇ (s.string == ਨਿਰਾ)
	{
	    l->lv_ਤਾਲਾ = prev_ਤਾਲਾ;
	    ਵਾਪਸ FAIL;
	}

	s.ਲੰਮਾਈ = STRLEN(s.string);
	sumlen += (ਲੰਮਾ)s.ਲੰਮਾਈ;

	(ਖਾਲੀ)ga_grow(join_gap, 1);
	p = ((join_T *)join_gap->ga_ਅੰਕੜੇ) + (join_gap->ga_len++);
	ਜੇ (tofree != ਨਿਰਾ || s.string != numbuf)
	{
	    p->s.string = s.string;
	    p->s.ਲੰਮਾਈ = s.ਲੰਮਾਈ;
	    p->tofree = tofree;
	}
	ਨਹੀਂਤਾਂ
	{
	    p->s.string = vim_strnsave(s.string, s.ਲੰਮਾਈ);
	    p->s.ਲੰਮਾਈ = s.ਲੰਮਾਈ;
	    p->tofree = p->s.string;
	}

	ਸਤਰ_breakcheck();
	ਜੇ (did_echo_string_emsg)  // recursion error, bail out
	    ਰੋਕੋ;
    }
    l->lv_ਤਾਲਾ = prev_ਤਾਲਾ;

    // Allocate result buffer with its total size, avoid re-allocation and
    // multiple copy operations.  Add 2 for a tailing ']' and NUL.
    seplen = STRLEN(sep);
    ਜੇ (join_gap->ga_len >= 2)
	sumlen += (ਲੰਮਾ)seplen * (join_gap->ga_len - 1);
    ਜੇ (sumlen > ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ - 2 || ga_grow(gap, (ਪੂਰਨ)sumlen + 2) == FAIL)
	ਵਾਪਸ FAIL;

    ਲਈ (i = 0; i < join_gap->ga_len && !got_int; ++i)
    {
	ਜੇ (ਪਹਿਲਾ)
	    ਪਹਿਲਾ = FALSE;
	ਨਹੀਂਤਾਂ
	    ga_concat_len(gap, sep, seplen);
	p = ((join_T *)join_gap->ga_ਅੰਕੜੇ) + i;

	ਜੇ (p->s.string != ਨਿਰਾ)
	    ga_concat_len(gap, p->s.string, p->s.ਲੰਮਾਈ);
	ਸਤਰ_breakcheck();
    }

    ਵਾਪਸ OK;
}

/*
 * Join list "l" into a string in "*gap", using separator "sep".
 * When "echo_style" is TRUE use String as echoed, otherwise as inside a List.
 * Return FAIL or OK.
 */
    ਪੂਰਨ
list_join(
    garray_T	*gap,
    list_T	*l,
    char_u	*sep,
    ਪੂਰਨ		echo_style,
    ਪੂਰਨ		restore_copyID,
    ਪੂਰਨ		copyID)
{
    garray_T	join_ga;
    ਪੂਰਨ		retval;
    join_T	*p;
    ਪੂਰਨ		i;

    ਜੇ (l->lv_len < 1)
	ਵਾਪਸ OK; // nothing to do
    ga_init2(&join_ga, ਆਕਾਰ(join_T), l->lv_len);
    retval = list_join_inner(gap, l, sep, echo_style, restore_copyID,
							    copyID, &join_ga);

    ਜੇ (join_ga.ga_ਅੰਕੜੇ == ਨਿਰਾ)
	ਵਾਪਸ retval;

    // Dispose each item in join_ga.
    p = (join_T *)join_ga.ga_ਅੰਕੜੇ;
    ਲਈ (i = 0; i < join_ga.ga_len; ++i)
    {
	vim_free(p->tofree);
	++p;
    }
    ga_clear(&join_ga);

    ਵਾਪਸ retval;
}

/*
 * "join()" function
 */
    ਖਾਲੀ
f_join(typval_T *argvars, typval_T *rettv)
{
    garray_T	ga;
    char_u	*sep;

    rettv->v_type = VAR_STRING;

    ਜੇ (check_for_list_or_tuple_arg(argvars, 0) == FAIL
	    || check_for_opt_string_arg(argvars, 1) == FAIL)
	ਵਾਪਸ;

    ਜੇ ((argvars[0].v_type == VAR_LIST && argvars[0].vval.v_list == ਨਿਰਾ)
	    || (argvars[0].v_type == VAR_TUPLE
		&& argvars[0].vval.v_tuple == ਨਿਰਾ))
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
	sep = (char_u *)" ";
    ਨਹੀਂਤਾਂ
	sep = tv_get_string_chk(&argvars[1]);

    ਜੇ (sep != ਨਿਰਾ)
    {
	ga_init2(&ga, ਆਕਾਰ(ਅੱਖਰ), 80);
	ਜੇ (argvars[0].v_type == VAR_LIST)
	    list_join(&ga, argvars[0].vval.v_list, sep, TRUE, FALSE, 0);
	ਨਹੀਂਤਾਂ
	    tuple_join(&ga, argvars[0].vval.v_tuple, sep, TRUE, FALSE, 0);
	ga_append(&ga, NUL);
	rettv->vval.v_string = (char_u *)ga.ga_ਅੰਕੜੇ;
    }
    ਨਹੀਂਤਾਂ
	rettv->vval.v_string = ਨਿਰਾ;
}

/*
 * Allocate a variable for a List and fill it from "*arg".
 * "*arg" points to the "[".
 * Return OK or FAIL.
 */
    ਪੂਰਨ
eval_list(char_u **arg, typval_T *rettv, evalarg_T *evalarg, ਪੂਰਨ do_error)
{
    ਪੂਰਨ		evaluate = evalarg == ਨਿਰਾ ? FALSE
					 : evalarg->eval_flags & EVAL_EVALUATE;
    list_T	*l = ਨਿਰਾ;
    typval_T	tv;
    listitem_T	*ਵਸਤੂ;
    ਪੂਰਨ		vim9script = ਵਿੱਚ_vim9script();
    ਪੂਰਨ		had_comma;

    ਜੇ (evaluate)
    {
	l = list_alloc();
	ਜੇ (l == ਨਿਰਾ)
	    ਵਾਪਸ FAIL;
    }

    *arg = skipwhite_and_linebreak(*arg + 1, evalarg);
    ਜਦੋਂ (**arg != ']' && **arg != NUL)
    {
	ਜੇ (eval1(arg, &tv, evalarg) == FAIL)	// recursive!
	    ਜਾਓ failret;
	ਜੇ (check_typval_is_ਮੁੱਲ(&tv) == FAIL)
	{
	    ਜੇ (evaluate)
		clear_tv(&tv);
	    ਜਾਓ failret;
	}
	ਜੇ (evaluate)
	{
	    ਵਸਤੂ = listitem_alloc();
	    ਜੇ (ਵਸਤੂ != ਨਿਰਾ)
	    {
		ਵਸਤੂ->li_tv = tv;
		ਵਸਤੂ->li_tv.v_ਤਾਲਾ = 0;
		list_append(l, ਵਸਤੂ);
	    }
	    ਨਹੀਂਤਾਂ
		clear_tv(&tv);
	}
	// Legacy Vim script allowed a space before the comma.
	ਜੇ (!vim9script)
	    *arg = skipwhite(*arg);

	// the comma must come after the value
	had_comma = **arg == ',';
	ਜੇ (had_comma)
	{
	    ਜੇ (vim9script && !IS_WHITE_NL_OR_NUL((*arg)[1]) && (*arg)[1] != ']')
	    {
		semsg(_(e_white_space_required_after_str_str), ",", *arg);
		ਜਾਓ failret;
	    }
	    *arg = skipwhite(*arg + 1);
	}

	// The "]" can be on the next line.  But a double quoted string may
	// follow, not a comment.
	*arg = skipwhite_and_linebreak(*arg, evalarg);
	ਜੇ (**arg == ']')
	    ਰੋਕੋ;

	ਜੇ (!had_comma)
	{
	    ਜੇ (do_error)
	    {
		ਜੇ (**arg == ',')
		    semsg(_(e_no_white_space_allowed_before_str_str),
								    ",", *arg);
		ਨਹੀਂਤਾਂ
		    semsg(_(e_missing_comma_ਵਿੱਚ_list_str), *arg);
	    }
	    ਜਾਓ failret;
	}
    }

    ਜੇ (**arg != ']')
    {
	ਜੇ (do_error)
	    semsg(_(e_missing_ਸਿਰਾ_ਦਾ_list_rsb_str), *arg);
failret:
	ਜੇ (evaluate)
	    list_free(l);
	ਵਾਪਸ FAIL;
    }

    *arg += 1;
    ਜੇ (evaluate)
	rettv_list_ਸੈੱਟ(rettv, l);

    ਵਾਪਸ OK;
}

/*
 * Write "list" of strings to file "fd".
 */
    ਪੂਰਨ
write_list(FILE *fd, list_T *list, ਪੂਰਨ ਦੁਆਧਾਰੀ)
{
    listitem_T	*li;
    ਪੂਰਨ		c;
    ਪੂਰਨ		ret = OK;
    char_u	*s;

    CHECK_LIST_MATERIALIZE(list);
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(list, li)
    {
	ਲਈ (s = tv_get_string(&li->li_tv); *s != NUL; ++s)
	{
	    ਜੇ (*s == '\n')
		c = putc(NUL, fd);
	    ਨਹੀਂਤਾਂ
		c = putc(*s, fd);
	    ਜੇ (c == EOF)
	    {
		ret = FAIL;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (!ਦੁਆਧਾਰੀ || li->li_ਅਗਲਾ != ਨਿਰਾ)
	    ਜੇ (putc('\n', fd) == EOF)
	    {
		ret = FAIL;
		ਰੋਕੋ;
	    }
	ਜੇ (ret == FAIL)
	{
	    emsg(_(e_error_while_writing));
	    ਰੋਕੋ;
	}
    }
    ਵਾਪਸ ret;
}

/*
 * Initialize a static list with 10 items.
 */
    ਖਾਲੀ
init_static_list(staticList10_T *sl)
{
    list_T  *l = &sl->sl_list;
    ਪੂਰਨ	    i;

    CLEAR_ਸੂਚਕ(sl);
    l->lv_ਪਹਿਲਾ = &sl->sl_ਚੀਜ਼ਾਂ[0];
    l->lv_u.mat.lv_ਆਖਰੀ = &sl->sl_ਚੀਜ਼ਾਂ[9];
    l->lv_refcount = DO_NOT_FREE_CNT;
    l->lv_ਤਾਲਾ = VAR_FIXED;
    sl->sl_list.lv_len = 10;

    ਲਈ (i = 0; i < 10; ++i)
    {
	listitem_T *li = &sl->sl_ਚੀਜ਼ਾਂ[i];

	ਜੇ (i == 0)
	    li->li_prev = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    li->li_prev = li - 1;
	ਜੇ (i == 9)
	    li->li_ਅਗਲਾ = ਨਿਰਾ;
	ਨਹੀਂਤਾਂ
	    li->li_ਅਗਲਾ = li + 1;
    }
}

/*
 * "list2str()" function
 */
    ਖਾਲੀ
f_list2str(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    garray_T	ga;
    ਪੂਰਨ		utf8 = FALSE;

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ਨਿਰਾ;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 1) == FAIL))
	ਵਾਪਸ;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    l = argvars[0].vval.v_list;
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;  // empty list results in empty string

    ਜੇ (argvars[1].v_type != VAR_UNKNOWN)
	utf8 = (ਪੂਰਨ)tv_get_bool_chk(&argvars[1], ਨਿਰਾ);

    CHECK_LIST_MATERIALIZE(l);
    ga_init2(&ga, 1, 80);
    ਜੇ (has_mbyte || utf8)
    {
	char_u	buf[MB_MAXBYTES + 1];
	ਪੂਰਨ	(*char2bytes)(ਪੂਰਨ, char_u *);

	ਜੇ (utf8 || enc_utf8)
	    char2bytes = utf_char2bytes;
	ਨਹੀਂਤਾਂ
	    char2bytes = mb_char2bytes;

	FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
	{
	    size_t  buflen;

	    buflen = (size_t)(*char2bytes)(tv_get_ਸੰਖਿਆ(&li->li_tv), buf);
	    buf[buflen] = NUL;
	    ga_concat_len(&ga, buf, buflen);
	}
	ga_append(&ga, NUL);
    }
    ਨਹੀਂਤਾਂ
    {
	ਲੰਮਾ len = (ਲੰਮਾ)list_len(l) + 1;

	ਜੇ (len <= ਵੱਧ_ਤੋਂ_ਵੱਧ_ਅੰਕ && ga_grow(&ga, (ਪੂਰਨ)len) == OK)
	{
	    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
		ga_append(&ga, tv_get_ਸੰਖਿਆ(&li->li_tv));
	    ga_append(&ga, NUL);
	}
    }

    rettv->v_type = VAR_STRING;
    rettv->vval.v_string = ga.ga_ਅੰਕੜੇ;
}

/*
 * "list2tuple()" function
 */
    ਖਾਲੀ
f_list2tuple(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    listitem_T	*li;
    tuple_T	*tuple;

    rettv->v_type = VAR_TUPLE;
    rettv->vval.v_tuple = ਨਿਰਾ;

    ਜੇ (check_for_list_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    l = argvars[0].vval.v_list;
    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;  // empty list results in empty tuple

    CHECK_LIST_MATERIALIZE(l);

    ਜੇ (rettv_tuple_ਸੈੱਟ_with_ਚੀਜ਼ਾਂ(rettv, list_len(l)) == FAIL)
	ਵਾਪਸ;

    tuple = rettv->vval.v_tuple;
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	copy_tv(&li->li_tv, TUPLE_ਵਸਤੂ(tuple, TUPLE_LEN(tuple)));
	tuple->tv_ਚੀਜ਼ਾਂ.ga_len++;
    }
}

/*
 * "tuple2list()" function
 */
    ਖਾਲੀ
f_tuple2list(typval_T *argvars, typval_T *rettv)
{
    list_T	*l;
    tuple_T	*tuple;

    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    ਜੇ (check_for_tuple_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    tuple = argvars[0].vval.v_tuple;
    ਜੇ (tuple == ਨਿਰਾ)
	ਵਾਪਸ;  // empty tuple results in empty list

    l = rettv->vval.v_list;
    ਲਈ (ਪੂਰਨ i = 0; i < tuple_len(tuple); i++)
	list_append_tv(l, TUPLE_ਵਸਤੂ(tuple, i));
}

/*
 * Remove item argvars[1] from List argvars[0]. If argvars[2] is supplied, then
 * remove the range of items from argvars[1] to argvars[2] (inclusive).
 */
    ਸਥਿਰ ਖਾਲੀ
list_remove(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg)
{
    list_T	*l;
    listitem_T	*ਵਸਤੂ, *item2;
    listitem_T	*li;
    ਪੂਰਨ		error = FALSE;
    ਲੰਮਾ	idx;
    ਲੰਮਾ	ਸਿਰਾ;
    ਪੂਰਨ		cnt = 0;
    list_T	*rl;

    ਜੇ ((l = argvars[0].vval.v_list) == ਨਿਰਾ
			     || ਮੁੱਲ_check_ਤਾਲਾ(l->lv_ਤਾਲਾ, arg_errmsg, TRUE))
	ਵਾਪਸ;

    idx = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
    ਜੇ (error)
	ਵਾਪਸ;		// type error: do nothing, errmsg already given

    ਜੇ ((ਵਸਤੂ = list_ਲੱਭੋ(l, idx)) == ਨਿਰਾ)
    {
	semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), idx);
	ਵਾਪਸ;
    }

    ਜੇ (argvars[2].v_type == VAR_UNKNOWN)
    {
	// Remove one item, return its value.
	vimlist_remove(l, ਵਸਤੂ, ਵਸਤੂ);
	*rettv = ਵਸਤੂ->li_tv;
	list_free_ਵਸਤੂ(l, ਵਸਤੂ);
	ਵਾਪਸ;
    }

    // Remove range of items, return list with values.
    ਸਿਰਾ = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
    ਜੇ (error)
	ਵਾਪਸ;		// type error: do nothing

    ਜੇ ((item2 = list_ਲੱਭੋ(l, ਸਿਰਾ)) == ਨਿਰਾ)
    {
	semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), ਸਿਰਾ);
	ਵਾਪਸ;
    }

    ਲਈ (li = ਵਸਤੂ; li != ਨਿਰਾ; li = li->li_ਅਗਲਾ)
    {
	++cnt;
	ਜੇ (li == item2)
	    ਰੋਕੋ;
    }
    ਜੇ (li == ਨਿਰਾ)  // didn't find "item2" after "item"
    {
	emsg(_(e_invalid_range));
	ਵਾਪਸ;
    }

    vimlist_remove(l, ਵਸਤੂ, item2);
    ਜੇ (rettv_list_alloc(rettv) == FAIL)
	ਵਾਪਸ;

    rl = rettv->vval.v_list;

    ਜੇ (l->lv_with_ਚੀਜ਼ਾਂ > 0)
    {
	// need to copy the list items and move the value
	ਜਦੋਂ (ਵਸਤੂ != ਨਿਰਾ)
	{
	    li = listitem_alloc();
	    ਜੇ (li == ਨਿਰਾ)
		ਵਾਪਸ;
	    li->li_tv = ਵਸਤੂ->li_tv;
	    init_tv(&ਵਸਤੂ->li_tv);
	    list_append(rl, li);
	    ਜੇ (ਵਸਤੂ == item2)
		ਰੋਕੋ;
	    ਵਸਤੂ = ਵਸਤੂ->li_ਅਗਲਾ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	rl->lv_ਪਹਿਲਾ = ਵਸਤੂ;
	rl->lv_u.mat.lv_ਆਖਰੀ = item2;
	ਵਸਤੂ->li_prev = ਨਿਰਾ;
	item2->li_ਅਗਲਾ = ਨਿਰਾ;
	rl->lv_len = cnt;
    }
}

ਸਥਿਰ ਪੂਰਨ ਵਸਤੂ_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2);
ਸਥਿਰ ਪੂਰਨ ਵਸਤੂ_compare2(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2);

// struct used in the array that's given to qsort()
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    listitem_T	*ਵਸਤੂ;
    ਪੂਰਨ		idx;
    // Sort key precomputed once per item for the numeric compare modes.
    // Only valid when sortinfo->item_compare_keys_ready is set (the sort()
    // path); uniq() passes a bare listitem_T pointer and must not read this.
    ਸੰਘ {
	varnumber_T	inum;	// for "N"
	ਦੁੱਗਣਾ		fnum;	// for "n" and "f"
    } ਕੁੰਜੀ;
} ਛਾਂਟੋ_ਵਸਤੂ_T;

// struct storing information about current sort
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਪੂਰਨ		ਵਸਤੂ_compare_ic;
    ਪੂਰਨ		ਵਸਤੂ_compare_lc;
    ਪੂਰਨ		ਵਸਤੂ_compare_numeric;
    ਪੂਰਨ		ਵਸਤੂ_compare_numbers;
    ਪੂਰਨ		ਵਸਤੂ_compare_float;
    char_u	*ਵਸਤੂ_compare_func;
    partial_T	*ਵਸਤੂ_compare_partial;
    dict_T	*ਵਸਤੂ_compare_selfdict;
    ਪੂਰਨ		ਵਸਤੂ_compare_func_err;
    ਪੂਰਨ		ਵਸਤੂ_compare_keep_zero;
    ਪੂਰਨ		ਵਸਤੂ_compare_ਕੁੰਜੀਆਂ_ready;  // ptrs[].key is precomputed
} sortinfo_T;
ਸਥਿਰ sortinfo_T	*sortinfo = ਨਿਰਾ;
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਵਸਤੂ_COMPARE_FAIL 999

/*
 * Compare functions for f_sort() and f_uniq() below.
 */
    ਸਥਿਰ ਪੂਰਨ
ਵਸਤੂ_compare(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਛਾਂਟੋ_ਵਸਤੂ_T  *si1, *si2;
    typval_T	*tv1, *tv2;
    char_u	*p1, *p2;
    char_u	*tofree1 = ਨਿਰਾ, *tofree2 = ਨਿਰਾ;
    ਪੂਰਨ		res;
    char_u	numbuf1[NUMBUFLEN];
    char_u	numbuf2[NUMBUFLEN];

    si1 = (ਛਾਂਟੋ_ਵਸਤੂ_T *)s1;
    si2 = (ਛਾਂਟੋ_ਵਸਤੂ_T *)s2;
    tv1 = &si1->ਵਸਤੂ->li_tv;
    tv2 = &si2->ਵਸਤੂ->li_tv;

    ਜੇ (sortinfo->ਵਸਤੂ_compare_numbers)
    {
	varnumber_T	v1 = sortinfo->ਵਸਤੂ_compare_ਕੁੰਜੀਆਂ_ready
					? si1->ਕੁੰਜੀ.inum : tv_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(tv1);
	varnumber_T	v2 = sortinfo->ਵਸਤੂ_compare_ਕੁੰਜੀਆਂ_ready
					? si2->ਕੁੰਜੀ.inum : tv_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(tv2);

	ਵਾਪਸ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ਜੇ (sortinfo->ਵਸਤੂ_compare_float)
    {
	float_T	v1 = sortinfo->ਵਸਤੂ_compare_ਕੁੰਜੀਆਂ_ready
					? si1->ਕੁੰਜੀ.fnum : tv_get_float(tv1);
	float_T	v2 = sortinfo->ਵਸਤੂ_compare_ਕੁੰਜੀਆਂ_ready
					? si2->ਕੁੰਜੀ.fnum : tv_get_float(tv2);

	ਵਾਪਸ v1 == v2 ? 0 : v1 > v2 ? 1 : -1;
    }

    ਜੇ (sortinfo->ਵਸਤੂ_compare_numeric && sortinfo->ਵਸਤੂ_compare_ਕੁੰਜੀਆਂ_ready)
    {
	ਦੁੱਗਣਾ	n1 = si1->ਕੁੰਜੀ.fnum;
	ਦੁੱਗਣਾ	n2 = si2->ਕੁੰਜੀ.fnum;

	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
	ਜੇ (res == 0 && !sortinfo->ਵਸਤੂ_compare_keep_zero)
	    res = si1->idx > si2->idx ? 1 : -1;
	ਵਾਪਸ res;
    }

    // tv2string() puts quotes around a string and allocates memory.  Don't do
    // that for string variables. Use a single quote when comparing with a
    // non-string to do what the docs promise.
    ਜੇ (tv1->v_type == VAR_STRING)
    {
	ਜੇ (tv2->v_type != VAR_STRING || sortinfo->ਵਸਤੂ_compare_numeric)
	    p1 = (char_u *)"'";
	ਨਹੀਂਤਾਂ
	    p1 = tv1->vval.v_string;
    }
    ਨਹੀਂਤਾਂ
	p1 = tv2string(tv1, &tofree1, numbuf1, 0);
    ਜੇ (tv2->v_type == VAR_STRING)
    {
	ਜੇ (tv1->v_type != VAR_STRING || sortinfo->ਵਸਤੂ_compare_numeric)
	    p2 = (char_u *)"'";
	ਨਹੀਂਤਾਂ
	    p2 = tv2->vval.v_string;
    }
    ਨਹੀਂਤਾਂ
	p2 = tv2string(tv2, &tofree2, numbuf2, 0);
    ਜੇ (p1 == ਨਿਰਾ)
	p1 = (char_u *)"";
    ਜੇ (p2 == ਨਿਰਾ)
	p2 = (char_u *)"";
    ਜੇ (!sortinfo->ਵਸਤੂ_compare_numeric)
    {
	ਜੇ (sortinfo->ਵਸਤੂ_compare_lc)
	    res = strcoll((ਅੱਖਰ *)p1, (ਅੱਖਰ *)p2);
	ਨਹੀਂਤਾਂ
	    res = sortinfo->ਵਸਤੂ_compare_ic ? STRICMP(p1, p2): STRCMP(p1, p2);
    }
    ਨਹੀਂਤਾਂ
    {
	ਦੁੱਗਣਾ n1, n2;
	n1 = strtod((ਅੱਖਰ *)p1, (ਅੱਖਰ **)&p1);
	n2 = strtod((ਅੱਖਰ *)p2, (ਅੱਖਰ **)&p2);
	res = n1 == n2 ? 0 : n1 > n2 ? 1 : -1;
    }

    // When the result would be zero, compare the item indexes.  Makes the
    // sort stable.
    ਜੇ (res == 0 && !sortinfo->ਵਸਤੂ_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    vim_free(tofree1);
    vim_free(tofree2);
    ਵਾਪਸ res;
}

    ਸਥਿਰ ਪੂਰਨ
ਵਸਤੂ_compare2(ਅਚਲ ਖਾਲੀ *s1, ਅਚਲ ਖਾਲੀ *s2)
{
    ਛਾਂਟੋ_ਵਸਤੂ_T  *si1, *si2;
    ਪੂਰਨ		res;
    typval_T	rettv;
    typval_T	argv[3];
    char_u	*func_name;
    partial_T	*partial = sortinfo->ਵਸਤੂ_compare_partial;
    funcexe_T	funcexe;
    ਪੂਰਨ		did_emsg_before = did_emsg;

    // shortcut after failure in previous call; compare all items equal
    ਜੇ (sortinfo->ਵਸਤੂ_compare_func_err)
	ਵਾਪਸ 0;

    si1 = (ਛਾਂਟੋ_ਵਸਤੂ_T *)s1;
    si2 = (ਛਾਂਟੋ_ਵਸਤੂ_T *)s2;

    ਜੇ (partial == ਨਿਰਾ)
	func_name = sortinfo->ਵਸਤੂ_compare_func;
    ਨਹੀਂਤਾਂ
	func_name = partial_name(partial);

    // Copy the values.  This is needed to be able to set v_lock to VAR_FIXED
    // in the copy without changing the original list items.
    copy_tv(&si1->ਵਸਤੂ->li_tv, &argv[0]);
    copy_tv(&si2->ਵਸਤੂ->li_tv, &argv[1]);

    rettv.v_type = VAR_UNKNOWN;		// clear_tv() uses this
    CLEAR_ਖੇਤਰ(funcexe);
    funcexe.fe_evaluate = TRUE;
    funcexe.fe_partial = partial;
    funcexe.fe_selfdict = sortinfo->ਵਸਤੂ_compare_selfdict;
    res = call_func(func_name, -1, &rettv, 2, argv, &funcexe);
    clear_tv(&argv[0]);
    clear_tv(&argv[1]);

    ਜੇ (res == FAIL || did_emsg > did_emsg_before)
	res = ਵਸਤੂ_COMPARE_FAIL;
    ਨਹੀਂਤਾਂ
    {
	varnumber_T	n;

	n = tv_get_ਸੰਖਿਆ_chk(&rettv, &sortinfo->ਵਸਤੂ_compare_func_err);
	res = (n > 0) ? 1 : (n < 0) ? -1 : 0;
    }
    ਜੇ (sortinfo->ਵਸਤੂ_compare_func_err)
	res = ਵਸਤੂ_COMPARE_FAIL;  // return value has wrong type
    clear_tv(&rettv);

    // When the result would be zero, compare the pointers themselves.  Makes
    // the sort stable.
    ਜੇ (res == 0 && !sortinfo->ਵਸਤੂ_compare_keep_zero)
	res = si1->idx > si2->idx ? 1 : -1;

    ਵਾਪਸ res;
}

/*
 * Precompute the numeric sort key of each item.  Only for the builtin numeric
 * compare modes; each key is computed exactly as item_compare() would have.
 */
    ਸਥਿਰ ਖਾਲੀ
ਛਾਂਟੋ_compute_ਕੁੰਜੀਆਂ(ਛਾਂਟੋ_ਵਸਤੂ_T *ptrs, ਲੰਮਾ len, sortinfo_T *info)
{
    ਲੰਮਾ	i;

    ਜੇ (info->ਵਸਤੂ_compare_numbers)
    {
	ਲਈ (i = 0; i < len; ++i)
	    ptrs[i].ਕੁੰਜੀ.inum = tv_ਮੰਜ਼ਿਲ_ਸੰਖਿਆ(&ptrs[i].ਵਸਤੂ->li_tv);
    }
    ਨਹੀਂਤਾਂ ਜੇ (info->ਵਸਤੂ_compare_float)
    {
	ਲਈ (i = 0; i < len; ++i)
	    ptrs[i].ਕੁੰਜੀ.fnum = tv_get_float(&ptrs[i].ਵਸਤੂ->li_tv);
    }
    ਨਹੀਂਤਾਂ // info->item_compare_numeric
    {
	ਲਈ (i = 0; i < len; ++i)
	{
	    typval_T	*tv = &ptrs[i].ਵਸਤੂ->li_tv;

	    // A string is compared as a single quote in numeric mode, which
	    // strtod() reads as 0.
	    ਜੇ (tv->v_type == VAR_STRING)
		ptrs[i].ਕੁੰਜੀ.fnum = 0.0;
	    ਨਹੀਂਤਾਂ
	    {
		char_u	numbuf[NUMBUFLEN];
		char_u	*tofree = ਨਿਰਾ;
		char_u	*p = tv2string(tv, &tofree, numbuf, 0);

		ptrs[i].ਕੁੰਜੀ.fnum = p == ਨਿਰਾ ? 0.0 : strtod((ਅੱਖਰ *)p, ਨਿਰਾ);
		vim_free(tofree);
	    }
	}
    }
    info->ਵਸਤੂ_compare_ਕੁੰਜੀਆਂ_ready = TRUE;
}

/*
 * sort() List "l"
 */
    ਸਥਿਰ ਖਾਲੀ
do_ਛਾਂਟੋ(list_T *l, sortinfo_T *info)
{
    ਲੰਮਾ	len;
    ਛਾਂਟੋ_ਵਸਤੂ_T	*ptrs;
    ਲੰਮਾ	i = 0;
    listitem_T	*li;

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(ਛਾਂਟੋ_ਵਸਤੂ_T, len);
    ਜੇ (ptrs == ਨਿਰਾ)
	ਵਾਪਸ;

    // sort(): ptrs will be the list to sort
    FOR_ALL_LIST_ਚੀਜ਼ਾਂ(l, li)
    {
	ptrs[i].ਵਸਤੂ = li;
	ptrs[i].idx = i;
	++i;
    }

    info->ਵਸਤੂ_compare_func_err = FALSE;
    info->ਵਸਤੂ_compare_keep_zero = FALSE;
    info->ਵਸਤੂ_compare_ਕੁੰਜੀਆਂ_ready = FALSE;

    ਜੇ (info->ਵਸਤੂ_compare_func == ਨਿਰਾ && info->ਵਸਤੂ_compare_partial == ਨਿਰਾ
	    && (info->ਵਸਤੂ_compare_numbers || info->ਵਸਤੂ_compare_float
						|| info->ਵਸਤੂ_compare_numeric))
	ਛਾਂਟੋ_compute_ਕੁੰਜੀਆਂ(ptrs, len, info);
    // test the compare function
    ਜੇ ((info->ਵਸਤੂ_compare_func != ਨਿਰਾ
		|| info->ਵਸਤੂ_compare_partial != ਨਿਰਾ)
	    && ਵਸਤੂ_compare2((ਖਾਲੀ *)&ptrs[0], (ਖਾਲੀ *)&ptrs[1])
	    == ਵਸਤੂ_COMPARE_FAIL)
	emsg(_(e_ਛਾਂਟੋ_compare_function_failed));
    ਨਹੀਂਤਾਂ
    {
	// Sort the array with item pointers.
	qsort((ਖਾਲੀ *)ptrs, (size_t)len, ਆਕਾਰ(ਛਾਂਟੋ_ਵਸਤੂ_T),
		info->ਵਸਤੂ_compare_func == ਨਿਰਾ
		&& info->ਵਸਤੂ_compare_partial == ਨਿਰਾ
		? ਵਸਤੂ_compare : ਵਸਤੂ_compare2);

	ਜੇ (!info->ਵਸਤੂ_compare_func_err)
	{
	    // Clear the List and append the items in sorted order.
	    l->lv_ਪਹਿਲਾ = l->lv_u.mat.lv_ਆਖਰੀ
		= l->lv_u.mat.lv_idx_ਵਸਤੂ = ਨਿਰਾ;
	    l->lv_len = 0;
	    ਲਈ (i = 0; i < len; ++i)
		list_append(l, ptrs[i].ਵਸਤੂ);
	}
    }

    vim_free(ptrs);
}

/*
 * uniq() List "l"
 */
    ਸਥਿਰ ਖਾਲੀ
do_uniq(list_T *l, sortinfo_T *info)
{
    ਲੰਮਾ	len;
    ਛਾਂਟੋ_ਵਸਤੂ_T	*ptrs;
    ਲੰਮਾ	i = 0;
    listitem_T	*li;
    ਪੂਰਨ	(*ਵਸਤੂ_compare_func_ptr)(ਅਚਲ ਖਾਲੀ *, ਅਚਲ ਖਾਲੀ *);

    len = list_len(l);

    // Make an array with each entry pointing to an item in the List.
    ptrs = ALLOC_MULT(ਛਾਂਟੋ_ਵਸਤੂ_T, len);
    ਜੇ (ptrs == ਨਿਰਾ)
	ਵਾਪਸ;

    // f_uniq(): ptrs will be a stack of items to remove
    info->ਵਸਤੂ_compare_func_err = FALSE;
    info->ਵਸਤੂ_compare_keep_zero = TRUE;
    ਵਸਤੂ_compare_func_ptr = info->ਵਸਤੂ_compare_func != ਨਿਰਾ
	|| info->ਵਸਤੂ_compare_partial != ਨਿਰਾ
	? ਵਸਤੂ_compare2 : ਵਸਤੂ_compare;

    ਲਈ (li = l->lv_ਪਹਿਲਾ; li != ਨਿਰਾ && li->li_ਅਗਲਾ != ਨਿਰਾ;
	    li = li->li_ਅਗਲਾ)
    {
	ਜੇ (ਵਸਤੂ_compare_func_ptr((ਖਾਲੀ *)&li, (ਖਾਲੀ *)&li->li_ਅਗਲਾ)
		== 0)
	    ptrs[i++].ਵਸਤੂ = li;
	ਜੇ (info->ਵਸਤੂ_compare_func_err)
	{
	    emsg(_(e_uniq_compare_function_failed));
	    ਰੋਕੋ;
	}
    }

    ਜੇ (!info->ਵਸਤੂ_compare_func_err)
    {
	ਜਦੋਂ (--i >= 0)
	{
	    li = ptrs[i].ਵਸਤੂ->li_ਅਗਲਾ;
	    ptrs[i].ਵਸਤੂ->li_ਅਗਲਾ = li->li_ਅਗਲਾ;
	    ਜੇ (li->li_ਅਗਲਾ != ਨਿਰਾ)
		li->li_ਅਗਲਾ->li_prev = ptrs[i].ਵਸਤੂ;
	    ਨਹੀਂਤਾਂ
		l->lv_u.mat.lv_ਆਖਰੀ = ptrs[i].ਵਸਤੂ;
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
    ਸਥਿਰ ਪੂਰਨ
parse_ਛਾਂਟੋ_uniq_ਦਲੀਲਾਂ(typval_T *argvars, sortinfo_T *info)
{
    info->ਵਸਤੂ_compare_ic = FALSE;
    info->ਵਸਤੂ_compare_lc = FALSE;
    info->ਵਸਤੂ_compare_numeric = FALSE;
    info->ਵਸਤੂ_compare_numbers = FALSE;
    info->ਵਸਤੂ_compare_float = FALSE;
    info->ਵਸਤੂ_compare_func = ਨਿਰਾ;
    info->ਵਸਤੂ_compare_partial = ਨਿਰਾ;
    info->ਵਸਤੂ_compare_selfdict = ਨਿਰਾ;
    info->ਵਸਤੂ_compare_ਕੁੰਜੀਆਂ_ready = FALSE;

    ਜੇ (argvars[1].v_type == VAR_UNKNOWN)
	ਵਾਪਸ OK;

    // optional second argument: {func}
    ਜੇ (argvars[1].v_type == VAR_FUNC)
	info->ਵਸਤੂ_compare_func = argvars[1].vval.v_string;
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_PARTIAL)
	info->ਵਸਤੂ_compare_partial = argvars[1].vval.v_partial;
    ਨਹੀਂਤਾਂ
    {
	ਪੂਰਨ	    error = FALSE;
	ਪੂਰਨ	    nr = 0;

	ਜੇ (argvars[1].v_type == VAR_ਸੰਖਿਆ)
	{
	    nr = tv_get_ਸੰਖਿਆ_chk(&argvars[1], &error);
	    ਜੇ (error)
		ਵਾਪਸ FAIL;
	    ਜੇ (nr == 1)
		info->ਵਸਤੂ_compare_ic = TRUE;
	}
	ਜੇ (nr != 1)
	{
	    ਜੇ (argvars[1].v_type != VAR_ਸੰਖਿਆ)
		info->ਵਸਤੂ_compare_func = tv_get_string(&argvars[1]);
	    ਨਹੀਂਤਾਂ ਜੇ (nr != 0)
	    {
		emsg(_(e_invalid_ਦਲੀਲ));
		ਵਾਪਸ FAIL;
	    }
	}
	ਜੇ (info->ਵਸਤੂ_compare_func != ਨਿਰਾ)
	{
	    ਜੇ (*info->ਵਸਤੂ_compare_func == NUL)
	    {
		// empty string means default sort
		info->ਵਸਤੂ_compare_func = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(info->ਵਸਤੂ_compare_func, "n") == 0)
	    {
		info->ਵਸਤੂ_compare_func = ਨਿਰਾ;
		info->ਵਸਤੂ_compare_numeric = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(info->ਵਸਤੂ_compare_func, "N") == 0)
	    {
		info->ਵਸਤੂ_compare_func = ਨਿਰਾ;
		info->ਵਸਤੂ_compare_numbers = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(info->ਵਸਤੂ_compare_func, "f") == 0)
	    {
		info->ਵਸਤੂ_compare_func = ਨਿਰਾ;
		info->ਵਸਤੂ_compare_float = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(info->ਵਸਤੂ_compare_func, "i") == 0)
	    {
		info->ਵਸਤੂ_compare_func = ਨਿਰਾ;
		info->ਵਸਤੂ_compare_ic = TRUE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(info->ਵਸਤੂ_compare_func, "l") == 0)
	    {
		info->ਵਸਤੂ_compare_func = ਨਿਰਾ;
		info->ਵਸਤੂ_compare_lc = TRUE;
	    }
	}
    }

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
    {
	// optional third argument: {dict}
	ਜੇ (check_for_dict_arg(argvars, 2) == FAIL)
	    ਵਾਪਸ FAIL;
	info->ਵਸਤੂ_compare_selfdict = argvars[2].vval.v_dict;
    }

    ਵਾਪਸ OK;
}

/*
 * "sort()" or "uniq()" function
 */
    ਸਥਿਰ ਖਾਲੀ
do_ਛਾਂਟੋ_uniq(typval_T *argvars, typval_T *rettv, ਪੂਰਨ ਛਾਂਟੋ)
{
    list_T	*l;
    sortinfo_T	*old_sortinfo;
    sortinfo_T	info;
    ਲੰਮਾ	len;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_arg(argvars, 0) == FAIL
		|| (argvars[1].v_type != VAR_UNKNOWN
		    && (check_for_string_or_func_arg(argvars, 1) == FAIL
			      || check_for_opt_dict_arg(argvars, 2) == FAIL))))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type != VAR_LIST)
    {
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list), ਛਾਂਟੋ ? "sort()" : "uniq()");
	ਵਾਪਸ;
    }

    // Pointer to current info struct used in compare function. Save and
    // restore the current one for nested calls.
    old_sortinfo = sortinfo;
    sortinfo = &info;

    l = argvars[0].vval.v_list;
    ਜੇ (l != ਨਿਰਾ && ਮੁੱਲ_check_ਤਾਲਾ(l->lv_ਤਾਲਾ,
		(char_u *)(ਛਾਂਟੋ ? N_("sort() argument") : N_("uniq() argument")),
		TRUE))
	ਜਾਓ theend;
    rettv_list_ਸੈੱਟ(rettv, l);
    ਜੇ (l == ਨਿਰਾ)
	ਜਾਓ theend;
    CHECK_LIST_MATERIALIZE(l);

    len = list_len(l);
    ਜੇ (len <= 1)
	ਜਾਓ theend;	// short list sorts pretty quickly

    ਜੇ (parse_ਛਾਂਟੋ_uniq_ਦਲੀਲਾਂ(argvars, &info) == FAIL)
	ਜਾਓ theend;

    ਜੇ (ਛਾਂਟੋ)
	do_ਛਾਂਟੋ(l, &info);
    ਨਹੀਂਤਾਂ
	do_uniq(l, &info);

theend:
    sortinfo = old_sortinfo;
}

/*
 * "sort({list})" function
 */
    ਖਾਲੀ
f_ਛਾਂਟੋ(typval_T *argvars, typval_T *rettv)
{
    do_ਛਾਂਟੋ_uniq(argvars, rettv, TRUE);
}

/*
 * "uniq({list})" function
 */
    ਖਾਲੀ
f_uniq(typval_T *argvars, typval_T *rettv)
{
    do_ਛਾਂਟੋ_uniq(argvars, rettv, FALSE);
}

/*
 * Handle one item for map(), filter(), foreach().
 * Sets v:val to "tv".  Caller must set v:key.
 */
    ਪੂਰਨ
filter_map_one(
	typval_T	*tv,	    // original value
	typval_T	*ਵਿਅੰਜਕ,	    // callback
	filtermap_T	filtermap,
	funccall_T	*fc,	    // from eval_expr_get_funccal()
	typval_T	*newtv,	    // for map() and mapnew(): new value
	ਪੂਰਨ		*remp)	    // for filter(): remove flag
{
    typval_T	argv[3];
    ਪੂਰਨ		retval = FAIL;

    copy_tv(tv, get_vim_var_tv(VV_VAL));

    newtv->v_type = VAR_UNKNOWN;
    ਜੇ (filtermap == FILTERMAP_FOREACH && ਵਿਅੰਜਕ->v_type == VAR_STRING)
    {
	// foreach() is not limited to an expression
	do_cmdline_cmd(ਵਿਅੰਜਕ->vval.v_string);
	ਜੇ (!did_emsg)
	    retval = OK;
	ਜਾਓ theend;
    }

    argv[0] = *get_vim_var_tv(VV_ਕੁੰਜੀ);
    argv[1] = *get_vim_var_tv(VV_VAL);
    ਜੇ (eval_ਵਿਅੰਜਕ_typval(ਵਿਅੰਜਕ, FALSE, argv, 2, fc, newtv) == FAIL)
	ਜਾਓ theend;
    ਜੇ (filtermap == FILTERMAP_FILTER)
    {
	ਪੂਰਨ	    error = FALSE;

	// filter(): when expr is zero remove the item
	ਜੇ (ਵਿੱਚ_vim9script())
	    *remp = !tv_get_bool_chk(newtv, &error);
	ਨਹੀਂਤਾਂ
	    *remp = (tv_get_ਸੰਖਿਆ_chk(newtv, &error) == 0);
	clear_tv(newtv);
	// On type error, nothing has been removed; return FAIL to stop the
	// loop.  The error message was given by tv_get_number_chk().
	ਜੇ (error)
	    ਜਾਓ theend;
    }
    ਨਹੀਂਤਾਂ ਜੇ (filtermap == FILTERMAP_FOREACH)
	clear_tv(newtv);
    retval = OK;
theend:
    clear_tv(get_vim_var_tv(VV_VAL));
    ਵਾਪਸ retval;
}

/*
 * Implementation of map(), filter(), foreach() for a List.  Apply "expr" to
 * every item in List "l" and return the result in "rettv".
 */
    ਸਥਿਰ ਖਾਲੀ
list_filter_map(
	list_T		*l,
	filtermap_T	filtermap,
	type_T		*argtype,
	ਅੱਖਰ		*func_name,
	char_u		*arg_errmsg,
	typval_T	*ਵਿਅੰਜਕ,
	typval_T	*rettv)
{
    ਪੂਰਨ		prev_ਤਾਲਾ;
    list_T	*l_ret = ਨਿਰਾ;
    ਪੂਰਨ		idx = 0;
    ਪੂਰਨ		rem;
    listitem_T	*li, *nli;
    typval_T	newtv;
    funccall_T	*fc;

    ਜੇ (filtermap == FILTERMAP_MAPNEW)
    {
	rettv->v_type = VAR_LIST;
	rettv->vval.v_list = ਨਿਰਾ;
    }
    ਜੇ (l == ਨਿਰਾ || (filtermap == FILTERMAP_FILTER
			    && ਮੁੱਲ_check_ਤਾਲਾ(l->lv_ਤਾਲਾ, arg_errmsg, TRUE)))
	ਵਾਪਸ;

    prev_ਤਾਲਾ = l->lv_ਤਾਲਾ;

    ਜੇ (filtermap == FILTERMAP_MAPNEW)
    {
	ਜੇ (rettv_list_alloc(rettv) == FAIL)
	    ਵਾਪਸ;
	l_ret = rettv->vval.v_list;
    }
    // set_vim_var_nr() doesn't set the type
    ਸੈੱਟ_vim_var_type(VV_ਕੁੰਜੀ, VAR_ਸੰਖਿਆ);

    ਜੇ (l->lv_ਤਾਲਾ == 0)
	l->lv_ਤਾਲਾ = VAR_LOCKED;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ਵਿਅੰਜਕ_get_funccal(ਵਿਅੰਜਕ, &newtv);

    ਜੇ (l->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ)
    {
	varnumber_T	val = l->lv_u.nonmat.lv_ਸ਼ੁਰੂ;
	ਪੂਰਨ		len = l->lv_len;
	ਪੂਰਨ		stride = l->lv_u.nonmat.lv_stride;

	// List from range(): loop over the numbers
	// NOTE: foreach() returns the range_list_item
	ਜੇ (filtermap != FILTERMAP_MAPNEW && filtermap != FILTERMAP_FOREACH)
	{
	    l->lv_ਪਹਿਲਾ = ਨਿਰਾ;
	    l->lv_u.mat.lv_ਆਖਰੀ = ਨਿਰਾ;
	    l->lv_len = 0;
	    l->lv_u.mat.lv_idx_ਵਸਤੂ = ਨਿਰਾ;
	}

	ਲਈ (idx = 0; idx < len; ++idx)
	{
	    typval_T tv;

	    tv.v_type = VAR_ਸੰਖਿਆ;
	    tv.v_ਤਾਲਾ = 0;
	    tv.vval.v_ਸੰਖਿਆ = val;
	    ਸੈੱਟ_vim_var_nr(VV_ਕੁੰਜੀ, idx);
	    ਜੇ (filter_map_one(&tv, ਵਿਅੰਜਕ, filtermap, fc, &newtv, &rem) == FAIL)
		ਰੋਕੋ;
	    ਜੇ (did_emsg)
	    {
		clear_tv(&newtv);
		ਰੋਕੋ;
	    }
	    ਜੇ (filtermap != FILTERMAP_FOREACH)
	    {
		ਜੇ (filtermap != FILTERMAP_FILTER)
		{
		    ਜੇ (filtermap == FILTERMAP_MAP && argtype != ਨਿਰਾ
			&& check_typval_arg_type(
						 argtype->tt_member, &newtv,
						 func_name, 0) == FAIL)
		    {
			clear_tv(&newtv);
			ਰੋਕੋ;
		    }
		    // map(), mapnew(): always append the new value to the
		    // list
		    ਜੇ (list_append_tv_move(filtermap == FILTERMAP_MAP
					    ? l : l_ret, &newtv) == FAIL)
			ਰੋਕੋ;
		}
		ਨਹੀਂਤਾਂ ਜੇ (!rem)
		{
		    // filter(): append the list item value when not rem
		    ਜੇ (list_append_tv_move(l, &tv) == FAIL)
			ਰੋਕੋ;
		}
	    }

	    val += stride;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	// Materialized list: loop over the items
	ਲਈ (li = l->lv_ਪਹਿਲਾ; li != ਨਿਰਾ; li = nli)
	{
	    ਜੇ (filtermap == FILTERMAP_MAP && ਮੁੱਲ_check_ਤਾਲਾ(
			li->li_tv.v_ਤਾਲਾ, arg_errmsg, TRUE))
		ਰੋਕੋ;
	    nli = li->li_ਅਗਲਾ;
	    ਸੈੱਟ_vim_var_nr(VV_ਕੁੰਜੀ, idx);
	    ਜੇ (filter_map_one(&li->li_tv, ਵਿਅੰਜਕ, filtermap, fc,
							 &newtv, &rem) == FAIL)
		ਰੋਕੋ;
	    ਜੇ (did_emsg)
	    {
		clear_tv(&newtv);
		ਰੋਕੋ;
	    }
	    ਜੇ (filtermap == FILTERMAP_MAP)
	    {
		ਜੇ (argtype != ਨਿਰਾ && check_typval_arg_type(
			    argtype->tt_member, &newtv, func_name, 0) == FAIL)
		{
		    clear_tv(&newtv);
		    ਰੋਕੋ;
		}
		// map(): replace the list item value
		clear_tv(&li->li_tv);
		newtv.v_ਤਾਲਾ = 0;
		li->li_tv = newtv;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (filtermap == FILTERMAP_MAPNEW)
	    {
		// mapnew(): append the list item value
		ਜੇ (list_append_tv_move(l_ret, &newtv) == FAIL)
		    ਰੋਕੋ;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (filtermap == FILTERMAP_FILTER && rem)
		    listitem_remove(l, li);
	    ++idx;
	}
    }

    l->lv_ਤਾਲਾ = prev_ਤਾਲਾ;
    ਜੇ (fc != ਨਿਰਾ)
	remove_funccal();
}

/*
 * Implementation of map(), filter() and foreach().
 */
    ਸਥਿਰ ਖਾਲੀ
filter_map(typval_T *argvars, typval_T *rettv, filtermap_T filtermap)
{
    typval_T	*ਵਿਅੰਜਕ;
    ਅੱਖਰ	*func_name = filtermap == FILTERMAP_MAP ? "map()"
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
    ਪੂਰਨ		save_did_emsg;
    type_T	*type = ਨਿਰਾ;

    // map(), filter(), foreach() return the first argument, also on failure.
    ਜੇ (filtermap != FILTERMAP_MAPNEW && argvars[0].v_type != VAR_STRING)
	copy_tv(&argvars[0], rettv);

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_tuple_dict_blob_or_string_arg(argvars, 0)
								== FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_TUPLE && filtermap != FILTERMAP_FOREACH)
    {
	semsg(_(e_cannot_use_tuple_with_function_str), func_name);
	ਵਾਪਸ;
    }

    ਜੇ (filtermap == FILTERMAP_MAP && ਵਿੱਚ_vim9script())
    {
	// Check that map() does not change the declared type of the list or
	// dict.
	ਜੇ (argvars[0].v_type == VAR_DICT && argvars[0].vval.v_dict != ਨਿਰਾ)
	    type = argvars[0].vval.v_dict->dv_type;
	ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_LIST
					     && argvars[0].vval.v_list != ਨਿਰਾ)
	    type = argvars[0].vval.v_list->lv_type;
    }

    ਜੇ (argvars[0].v_type != VAR_BLOB
	    && argvars[0].v_type != VAR_LIST
	    && argvars[0].v_type != VAR_TUPLE
	    && argvars[0].v_type != VAR_DICT
	    && argvars[0].v_type != VAR_STRING)
    {
	ਅੱਖਰ *ਸੁਨੇਹਾ;

	ਜੇ (filtermap == FILTERMAP_FOREACH)
	    ਸੁਨੇਹਾ = e_ਦਲੀਲ_ਦਾ_str_must_be_list_tuple_string_dictionary_or_blob;
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ = e_ਦਲੀਲ_ਦਾ_str_must_be_list_string_dictionary_or_blob;
	semsg(_(ਸੁਨੇਹਾ), func_name);
	ਵਾਪਸ;
    }

    // On type errors, the preceding call has already displayed an error
    // message.  Avoid a misleading error message for an empty string that
    // was not passed as argument.
    ਵਿਅੰਜਕ = &argvars[1];
    ਜੇ (ਵਿਅੰਜਕ->v_type == VAR_UNKNOWN)
	ਵਾਪਸ;

    typval_T	save_val;
    typval_T	save_ਕੁੰਜੀ;

    prepare_vimvar(VV_VAL, &save_val);
    prepare_vimvar(VV_ਕੁੰਜੀ, &save_ਕੁੰਜੀ);

    // We reset "did_emsg" to be able to detect whether an error
    // occurred during evaluation of the expression.
    save_did_emsg = did_emsg;
    did_emsg = FALSE;

    ਜੇ (argvars[0].v_type == VAR_DICT)
	dict_filter_map(argvars[0].vval.v_dict, filtermap, type, func_name,
						      arg_errmsg, ਵਿਅੰਜਕ, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_BLOB)
	blob_filter_map(argvars[0].vval.v_blob, filtermap, ਵਿਅੰਜਕ,
							    arg_errmsg, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_STRING)
	string_filter_map(tv_get_string(&argvars[0]), filtermap, ਵਿਅੰਜਕ, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_TUPLE)
	tuple_foreach(argvars[0].vval.v_tuple, filtermap, ਵਿਅੰਜਕ);
    ਨਹੀਂਤਾਂ // argvars[0].v_type == VAR_LIST
	list_filter_map(argvars[0].vval.v_list, filtermap, type, func_name,
						      arg_errmsg, ਵਿਅੰਜਕ, rettv);

    restore_vimvar(VV_ਕੁੰਜੀ, &save_ਕੁੰਜੀ);
    restore_vimvar(VV_VAL, &save_val);

    did_emsg |= save_did_emsg;
}

/*
 * "filter()" function
 */
    ਖਾਲੀ
f_filter(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FILTER);
}

/*
 * "map()" function
 */
    ਖਾਲੀ
f_map(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAP);
}

/*
 * "mapnew()" function
 */
    ਖਾਲੀ
f_mapnew(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_MAPNEW);
}

/*
 * "foreach()" function
 */
    ਖਾਲੀ
f_foreach(typval_T *argvars, typval_T *rettv)
{
    filter_map(argvars, rettv, FILTERMAP_FOREACH);
}

/*
 * "add(list, item)" function
 */
    ਸਥਿਰ ਖਾਲੀ
list_ਜੋੜੋ(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;

    ਜੇ (l == ਨਿਰਾ)
    {
	ਜੇ (ਵਿੱਚ_vim9script())
	    emsg(_(e_cannot_ਜੋੜੋ_ਮੰਜ਼ਿਲ_null_list));
    }
    ਨਹੀਂਤਾਂ ਜੇ (!ਮੁੱਲ_check_ਤਾਲਾ(l->lv_ਤਾਲਾ,
		(char_u *)N_("add() argument"), TRUE)
	    && list_append_tv(l, &argvars[1]) == OK)
    {
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "add(object, item)" function
 */
    ਖਾਲੀ
f_ਜੋੜੋ(typval_T *argvars, typval_T *rettv)
{
    rettv->vval.v_ਸੰਖਿਆ = 1; // Default: Failed

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_LIST)
	list_ਜੋੜੋ(argvars, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_BLOB)
	blob_ਜੋੜੋ(argvars, rettv);
    ਨਹੀਂਤਾਂ
	emsg(_(e_list_or_blob_required));
}

/*
 * Count the number of times item "needle" occurs in List "l" starting at index
 * "idx". Case is ignored if "ic" is TRUE.
 */
    ਸਥਿਰ ਲੰਮਾ
list_ਗਿਣਤੀ(list_T *l, typval_T *needle, ਲੰਮਾ idx, ਪੂਰਨ ic)
{
    ਲੰਮਾ	n = 0;
    listitem_T	*li;

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ 0;

    CHECK_LIST_MATERIALIZE(l);

    ਜੇ (list_len(l) == 0)
	ਵਾਪਸ 0;

    li = list_ਲੱਭੋ(l, idx);
    ਜੇ (li == ਨਿਰਾ)
    {
	semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), idx);
	ਵਾਪਸ 0;
    }

    ਲਈ ( ; li != ਨਿਰਾ; li = li->li_ਅਗਲਾ)
	ਜੇ (tv_equal(&li->li_tv, needle, ic))
	    ++n;

    ਵਾਪਸ n;
}

/*
 * "count()" function
 */
    ਖਾਲੀ
f_ਗਿਣਤੀ(typval_T *argvars, typval_T *rettv)
{
    ਲੰਮਾ	n = 0;
    ਪੂਰਨ		ic = FALSE;
    ਪੂਰਨ		error = FALSE;

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_string_list_tuple_or_dict_arg(argvars, 0) == FAIL
		|| check_for_opt_bool_arg(argvars, 2) == FAIL
		|| (argvars[2].v_type != VAR_UNKNOWN
		    && check_for_opt_ਸੰਖਿਆ_arg(argvars, 3) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	ic = (ਪੂਰਨ)tv_get_bool_chk(&argvars[2], &error);

    ਜੇ (!error && argvars[0].v_type == VAR_STRING)
	n = string_ਗਿਣਤੀ(argvars[0].vval.v_string,
					tv_get_string_chk(&argvars[1]), ic);
    ਨਹੀਂਤਾਂ ਜੇ (!error && argvars[0].v_type == VAR_LIST)
    {
	ਲੰਮਾ idx = 0;

	ਜੇ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[3], &error);
	ਜੇ (!error)
	    n = list_ਗਿਣਤੀ(argvars[0].vval.v_list, &argvars[1], idx, ic);
    }
    ਨਹੀਂਤਾਂ ਜੇ (!error && argvars[0].v_type == VAR_TUPLE)
    {
	ਲੰਮਾ idx = 0;

	ਜੇ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    idx = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[3], &error);
	ਜੇ (!error)
	    n = tuple_ਗਿਣਤੀ(argvars[0].vval.v_tuple, &argvars[1], idx, ic);
    }
    ਨਹੀਂਤਾਂ ਜੇ (!error && argvars[0].v_type == VAR_DICT)
    {
	ਜੇ (argvars[2].v_type != VAR_UNKNOWN
		&& argvars[3].v_type != VAR_UNKNOWN)
	    emsg(_(e_invalid_ਦਲੀਲ));
	ਨਹੀਂਤਾਂ
	    n = dict_ਗਿਣਤੀ(argvars[0].vval.v_dict, &argvars[1], ic);
    }
    ਨਹੀਂਤਾਂ ਜੇ (!error)
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list_string_or_dictionary),
								    "count()");
    rettv->vval.v_ਸੰਖਿਆ = n;
}

/*
 * extend() a List. Append List argvars[1] to List argvars[0] before index
 * argvars[3] and return the resulting list in "rettv".  "is_new" is TRUE for
 * extendnew().
 */
    ਸਥਿਰ ਖਾਲੀ
list_extend_func(
	typval_T	*argvars,
	type_T		*type,
	ਅੱਖਰ		*func_name,
	char_u		*arg_errmsg,
	ਪੂਰਨ		is_new,
	typval_T	*rettv)
{
    list_T	*l1, *l2;
    listitem_T	*ਵਸਤੂ;
    ਲੰਮਾ	before;
    ਪੂਰਨ		error = FALSE;

    l1 = argvars[0].vval.v_list;
    ਜੇ (l1 == ਨਿਰਾ)
    {
	emsg(_(e_cannot_extend_null_list));
	ਵਾਪਸ;
    }
    ਜੇ (is_new || !ਮੁੱਲ_check_ਤਾਲਾ(l1->lv_ਤਾਲਾ, arg_errmsg, TRUE))
    {
	ਜੇ (is_new)
	{
	    l1 = list_copy(l1, FALSE, TRUE, get_copyID());
	    ਜੇ (l1 == ਨਿਰਾ)
		ਵਾਪਸ;
	}

	l2 = argvars[1].vval.v_list;
	ਜੇ (l2 == ਨਿਰਾ)
	    ਜਾਓ theend;

	ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	{
	    before = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
	    ਜੇ (error)
		ਜਾਓ cleanup;		// type error; errmsg already given
	    ਜੇ (before == l1->lv_len)
		ਵਸਤੂ = ਨਿਰਾ;
	    ਨਹੀਂਤਾਂ
	    {
		ਵਸਤੂ = list_ਲੱਭੋ(l1, before);
		ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
		{
		    semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), before);
		    ਜਾਓ cleanup;
		}
	    }
	}
	ਨਹੀਂਤਾਂ
	    ਵਸਤੂ = ਨਿਰਾ;
	ਜੇ (type != ਨਿਰਾ && check_typval_arg_type(
		    type, &argvars[1], func_name, 2) == FAIL)
	    ਜਾਓ cleanup;
	list_extend(l1, l2, ਵਸਤੂ);

theend:
	ਜੇ (is_new)
	{
	    rettv->v_type = VAR_LIST;
	    rettv->vval.v_list = l1;
	    rettv->v_ਤਾਲਾ = FALSE;
	}
	ਨਹੀਂਤਾਂ
	    copy_tv(&argvars[0], rettv);
	ਵਾਪਸ;

cleanup:
	ਜੇ (is_new)
	    list_unref(l1);
    }
}

/*
 * "extend()" or "extendnew()" function.  "is_new" is TRUE for extendnew().
 */
    ਸਥਿਰ ਖਾਲੀ
extend(typval_T *argvars, typval_T *rettv, char_u *arg_errmsg, ਪੂਰਨ is_new)
{
    type_T	*type = ਨਿਰਾ;
    ਅੱਖਰ	*func_name = is_new ? "extendnew()" : "extend()";

    ਜੇ (argvars[0].v_type == VAR_LIST && argvars[1].v_type == VAR_LIST)
    {
	// Check that extend() does not change the type of the list if it was
	// declared.
	ਜੇ (!is_new && ਵਿੱਚ_vim9script() && argvars[0].vval.v_list != ਨਿਰਾ)
	    type = argvars[0].vval.v_list->lv_type;
	list_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_DICT && argvars[1].v_type == VAR_DICT)
    {
	// Check that extend() does not change the type of the dict if it was
	// declared.
	ਜੇ (!is_new && ਵਿੱਚ_vim9script() && argvars[0].vval.v_dict != ਨਿਰਾ)
	    type = argvars[0].vval.v_dict->dv_type;
	dict_extend_func(argvars, type, func_name, arg_errmsg, is_new, rettv);
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_BLOB && argvars[1].v_type == VAR_BLOB)
	blob_extend_func(argvars, arg_errmsg, is_new, rettv);
    ਨਹੀਂਤਾਂ
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list_dictionary_or_blob), func_name);
}

/*
 * "extend(list, list [, idx])" function
 * "extend(dict, dict [, action])" function
 * "extend(blob, blob [, idx])" function
 */
    ਖਾਲੀ
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
    ਖਾਲੀ
f_extendnew(typval_T *argvars, typval_T *rettv)
{
    char_u      *errmsg = (char_u *)N_("extendnew() argument");

    extend(argvars, rettv, errmsg, TRUE);
}

    ਸਥਿਰ ਖਾਲੀ
list_ਪਾਓ_func(typval_T *argvars, typval_T *rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    ਲੰਮਾ	before = 0;
    listitem_T	*ਵਸਤੂ;
    ਪੂਰਨ		error = FALSE;

    ਜੇ (l == ਨਿਰਾ)
    {
	ਜੇ (ਵਿੱਚ_vim9script())
	    emsg(_(e_cannot_ਜੋੜੋ_ਮੰਜ਼ਿਲ_null_list));
	ਵਾਪਸ;
    }

    ਜੇ (ਮੁੱਲ_check_ਤਾਲਾ(l->lv_ਤਾਲਾ, (char_u *)N_("insert() argument"), TRUE))
	ਵਾਪਸ;

    ਜੇ (argvars[2].v_type != VAR_UNKNOWN)
	before = (ਲੰਮਾ)tv_get_ਸੰਖਿਆ_chk(&argvars[2], &error);
    ਜੇ (error)
	ਵਾਪਸ;		// type error; errmsg already given

    ਜੇ (before == l->lv_len)
	ਵਸਤੂ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
    {
	ਵਸਤੂ = list_ਲੱਭੋ(l, before);
	ਜੇ (ਵਸਤੂ == ਨਿਰਾ)
	{
	    semsg(_(e_list_ਸੂਚਕਾਂਕ_ਬਾਹਰਲਾ_ਦਾ_range_nr), before);
	    l = ਨਿਰਾ;
	}
    }
    ਜੇ (l != ਨਿਰਾ)
    {
	(ਖਾਲੀ)list_ਪਾਓ_tv(l, &argvars[1], ਵਸਤੂ);
	copy_tv(&argvars[0], rettv);
    }
}

/*
 * "insert()" function
 */
    ਖਾਲੀ
f_ਪਾਓ(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_or_blob_arg(argvars, 0) == FAIL
		|| (argvars[0].v_type == VAR_BLOB
		    && check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_BLOB)
	blob_ਪਾਓ_func(argvars, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type != VAR_LIST)
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list_or_blob), "insert()");
    ਨਹੀਂਤਾਂ
	list_ਪਾਓ_func(argvars, rettv);
}

/*
 * "remove()" function
 */
    ਖਾਲੀ
f_remove(typval_T *argvars, typval_T *rettv)
{
    char_u	*arg_errmsg = (char_u *)N_("remove() argument");

    ਜੇ (ਵਿੱਚ_vim9script()
	    && (check_for_list_or_dict_or_blob_arg(argvars, 0) == FAIL
		|| ((argvars[0].v_type == VAR_LIST
			|| argvars[0].v_type == VAR_BLOB)
		    && (check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
			|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL))
		|| (argvars[0].v_type == VAR_DICT
		    && check_for_string_or_ਸੰਖਿਆ_arg(argvars, 1) == FAIL)))
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_DICT)
	dict_remove(argvars, rettv, arg_errmsg);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_BLOB)
	blob_remove(argvars, rettv, arg_errmsg);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_LIST)
	list_remove(argvars, rettv, arg_errmsg);
    ਨਹੀਂਤਾਂ
	semsg(_(e_ਦਲੀਲ_ਦਾ_str_must_be_list_dictionary_or_blob), "remove()");
}

    ਸਥਿਰ ਖਾਲੀ
list_ਉਲਟ(list_T *l, typval_T *rettv)
{
    listitem_T	*li, *ni;

    rettv_list_ਸੈੱਟ(rettv, l);
    ਜੇ (l != ਨਿਰਾ
	    && !ਮੁੱਲ_check_ਤਾਲਾ(l->lv_ਤਾਲਾ,
		(char_u *)N_("reverse() argument"), TRUE))
    {
	ਜੇ (l->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ)
	{
	    varnumber_T new_ਸ਼ੁਰੂ = l->lv_u.nonmat.lv_ਸ਼ੁਰੂ
		+ ((varnumber_T)l->lv_len - 1) * l->lv_u.nonmat.lv_stride;
	    l->lv_u.nonmat.lv_ਸਿਰਾ = new_ਸ਼ੁਰੂ
		- (l->lv_u.nonmat.lv_ਸਿਰਾ - l->lv_u.nonmat.lv_ਸ਼ੁਰੂ);
	    l->lv_u.nonmat.lv_ਸ਼ੁਰੂ = new_ਸ਼ੁਰੂ;
	    l->lv_u.nonmat.lv_stride = -l->lv_u.nonmat.lv_stride;
	    ਵਾਪਸ;
	}
	li = l->lv_u.mat.lv_ਆਖਰੀ;
	l->lv_ਪਹਿਲਾ = l->lv_u.mat.lv_ਆਖਰੀ = ਨਿਰਾ;
	l->lv_len = 0;
	ਜਦੋਂ (li != ਨਿਰਾ)
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
    ਖਾਲੀ
f_ਉਲਟ(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[0].v_type == VAR_BLOB)
	blob_ਉਲਟ(argvars[0].vval.v_blob, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_STRING)
    {
	rettv->v_type = VAR_STRING;
	ਜੇ (argvars[0].vval.v_string != ਨਿਰਾ)
	    rettv->vval.v_string = ਉਲਟ_ਪਾਠ(argvars[0].vval.v_string);
	ਨਹੀਂਤਾਂ
	    rettv->vval.v_string = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_LIST)
	list_ਉਲਟ(argvars[0].vval.v_list, rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_TUPLE)
	tuple_ਉਲਟ(argvars[0].vval.v_tuple, rettv);
}

/*
 * Implementation of reduce() for list "argvars[0]", using the function "expr"
 * starting with the optional initial value argvars[2] and return the result in
 * "rettv".
 */
    ਸਥਿਰ ਖਾਲੀ
list_reduce(
	typval_T	*argvars,
	typval_T	*ਵਿਅੰਜਕ,
	typval_T	*rettv)
{
    list_T	*l = argvars[0].vval.v_list;
    listitem_T  *li = ਨਿਰਾ;
    ਪੂਰਨ		range_list;
    ਪੂਰਨ		range_idx = 0;
    varnumber_T	range_val = 0;
    typval_T	initial;
    typval_T	argv[3];
    ਪੂਰਨ		r;
    ਪੂਰਨ		called_emsg_ਸ਼ੁਰੂ = called_emsg;
    ਪੂਰਨ		prev_locked;
    funccall_T	*fc;

    // Using reduce on a range() uses "range_idx" and "range_val".
    range_list = l != ਨਿਰਾ && l->lv_ਪਹਿਲਾ == &range_list_ਵਸਤੂ;
    ਜੇ (range_list)
	range_val = l->lv_u.nonmat.lv_ਸ਼ੁਰੂ;

    ਜੇ (argvars[2].v_type == VAR_UNKNOWN)
    {
	ਜੇ (l == ਨਿਰਾ || l->lv_len == 0)
	{
	    semsg(_(e_reduce_ਦਾ_an_ਖਾਲੀ_ਥਾਵਾਂ_str_with_no_initial_ਮੁੱਲ), "List");
	    ਵਾਪਸ;
	}
	ਜੇ (range_list)
	{
	    initial.v_type = VAR_ਸੰਖਿਆ;
	    initial.vval.v_ਸੰਖਿਆ = range_val;
	    range_val += l->lv_u.nonmat.lv_stride;
	    range_idx = 1;
	}
	ਨਹੀਂਤਾਂ
	{
	    initial = l->lv_ਪਹਿਲਾ->li_tv;
	    li = l->lv_ਪਹਿਲਾ->li_ਅਗਲਾ;
	}
    }
    ਨਹੀਂਤਾਂ
    {
	initial = argvars[2];
	ਜੇ (l != ਨਿਰਾ && !range_list)
	    li = l->lv_ਪਹਿਲਾ;
    }
    copy_tv(&initial, rettv);

    ਜੇ (l == ਨਿਰਾ)
	ਵਾਪਸ;

    // Create one funccall_T for all eval_expr_typval() calls.
    fc = eval_ਵਿਅੰਜਕ_get_funccal(ਵਿਅੰਜਕ, rettv);

    prev_locked = l->lv_ਤਾਲਾ;
    l->lv_ਤਾਲਾ = VAR_FIXED;  // disallow the list changing here

    ਜਦੋਂ (range_list ? range_idx < l->lv_len : li != ਨਿਰਾ)
    {
	argv[0] = *rettv;
	rettv->v_type = VAR_UNKNOWN;

	ਜੇ (range_list)
	{
	    argv[1].v_type = VAR_ਸੰਖਿਆ;
	    argv[1].vval.v_ਸੰਖਿਆ = range_val;
	}
	ਨਹੀਂਤਾਂ
	    argv[1] = li->li_tv;

	r = eval_ਵਿਅੰਜਕ_typval(ਵਿਅੰਜਕ, TRUE, argv, 2, fc, rettv);

	ਜੇ (argv[0].v_type != VAR_ਸੰਖਿਆ && argv[0].v_type != VAR_UNKNOWN)
	    clear_tv(&argv[0]);
	ਜੇ (r == FAIL || called_emsg != called_emsg_ਸ਼ੁਰੂ)
	    ਰੋਕੋ;

	// advance to the next item
	ਜੇ (range_list)
	{
	    range_val += l->lv_u.nonmat.lv_stride;
	    ++range_idx;
	}
	ਨਹੀਂਤਾਂ
	    li = li->li_ਅਗਲਾ;
    }

    ਜੇ (fc != ਨਿਰਾ)
	remove_funccal();

    l->lv_ਤਾਲਾ = prev_locked;
}

/*
 * "reduce(list, { accumulator, element -> value } [, initial])" function
 * "reduce(tuple, { accumulator, element -> value } [, initial])" function
 * "reduce(blob, { accumulator, element -> value } [, initial])"
 * "reduce(string, { accumulator, element -> value } [, initial])"
 */
    ਖਾਲੀ
f_reduce(typval_T *argvars, typval_T *rettv)
{
    char_u	*func_name;

    ਜੇ (check_for_string_or_list_or_tuple_or_blob_arg(argvars, 0) == FAIL)
	ਵਾਪਸ;

    ਜੇ (argvars[1].v_type == VAR_FUNC)
	func_name = argvars[1].vval.v_string;
    ਨਹੀਂਤਾਂ ਜੇ (argvars[1].v_type == VAR_PARTIAL)
	func_name = partial_name(argvars[1].vval.v_partial);
    ਨਹੀਂਤਾਂ
	func_name = tv_get_string_strict(&argvars[1]);
    ਜੇ (func_name == ਨਿਰਾ || *func_name == NUL)
    {
	emsg(_(e_missing_function_ਦਲੀਲ));
	ਵਾਪਸ;
    }

    ਜੇ (argvars[0].v_type == VAR_LIST)
	list_reduce(argvars, &argvars[1], rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_TUPLE)
	tuple_reduce(argvars, &argvars[1], rettv);
    ਨਹੀਂਤਾਂ ਜੇ (argvars[0].v_type == VAR_STRING)
	string_reduce(argvars, &argvars[1], rettv);
    ਨਹੀਂਤਾਂ
	blob_reduce(argvars, &argvars[1], rettv);
}

/*
 * slice() function
 */
    ਖਾਲੀ
f_ਟੁਕੜਾ(typval_T *argvars, typval_T *rettv)
{
    ਜੇ (ਵਿੱਚ_vim9script()
	    && ((argvars[0].v_type != VAR_STRING
		    && argvars[0].v_type != VAR_LIST
		    && argvars[0].v_type != VAR_TUPLE
		    && argvars[0].v_type != VAR_BLOB
		    && check_for_list_arg(argvars, 0) == FAIL)
		|| check_for_ਸੰਖਿਆ_arg(argvars, 1) == FAIL
		|| check_for_opt_ਸੰਖਿਆ_arg(argvars, 2) == FAIL))
	ਵਾਪਸ;

    ਜੇ (check_can_ਸੂਚਕਾਂਕ(&argvars[0], TRUE, FALSE) != OK)
	ਵਾਪਸ;

    copy_tv(argvars, rettv);
    eval_ਸੂਚਕਾਂਕ_inner(rettv, TRUE, argvars + 1,
	    argvars[2].v_type == VAR_UNKNOWN ? ਨਿਰਾ : argvars + 2,
	    TRUE, ਨਿਰਾ, 0, FALSE);
}

#ਜੇ_ਅੰਤ // defined(FEAT_EVAL)
